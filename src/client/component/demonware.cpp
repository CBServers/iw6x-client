#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "demonware.hpp"
#include "game_module.hpp"

#include <utils/hook.hpp>
#include <utils/nt.hpp>
#include <utils/cryptography.hpp>
#include <utils/thread.hpp>

#include "game/game.hpp"

#include "game/demonware/stun_server.hpp"
#include "game/demonware/service_server.hpp"

#include "game/demonware/services/bdLSGHello.hpp"       // 7
#include "game/demonware/services/bdStorage.hpp"        // 10
#include "game/demonware/services/bdDediAuth.hpp"       // 12
#include "game/demonware/services/bdTitleUtilities.hpp" // 12
#include "game/demonware/services/bdBandwidthTest.hpp"  // 18
#include "game/demonware/services/bdMatchMaking.hpp"    // 21
#include "game/demonware/services/bdDediRSAAuth.hpp"    // 26
#include "game/demonware/services/bdDML.hpp"            // 27
#include "game/demonware/services/bdGroup.hpp"          // 28
#include "game/demonware/services/bdSteamAuth.hpp"      // 28
#include "game/demonware/services/bdAnticheat.hpp"      // 38
#include "game/demonware/services/bdRelayService.hpp"   // 86

#define TCP_BLOCKING true
#define UDP_BLOCKING false

namespace demonware
{
	namespace
	{
		volatile bool terminate;
		std::thread message_thread;
		std::recursive_mutex server_mutex;
		std::map<SOCKET, bool> blocking_sockets;
		std::map<SOCKET, std::shared_ptr<service_server>> socket_links;
		std::map<unsigned long, std::shared_ptr<service_server>> servers;
		std::map<unsigned long, std::shared_ptr<stun_server>> stun_servers;
		std::map<SOCKET, std::queue<std::pair<std::string, std::string>>> datagram_packets;

		uint8_t encryption_key_[24];
		uint8_t decryption_key_[24];

		std::shared_ptr<service_server> find_server_by_address(const unsigned long address)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			const auto server = servers.find(address);
			if (server != servers.end())
			{
				return server->second;
			}

			return std::shared_ptr<service_server>();
		}

		std::shared_ptr<service_server> find_server_by_name(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);
			return find_server_by_address(utils::cryptography::jenkins_one_at_a_time::compute(name));
		}

		std::shared_ptr<stun_server> find_stun_server_by_address(const unsigned long address)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			const auto server = stun_servers.find(address);
			if (server != stun_servers.end())
			{
				return server->second;
			}

			return std::shared_ptr<stun_server>();
		}

		std::shared_ptr<stun_server> find_stun_server_by_name(const std::string& name)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);
			return find_stun_server_by_address(utils::cryptography::jenkins_one_at_a_time::compute(name));
		}

		std::shared_ptr<service_server> find_server_by_socket(const SOCKET s)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			const auto server = socket_links.find(s);
			if (server != socket_links.end())
			{
				return server->second;
			}

			return std::shared_ptr<service_server>();
		}

		bool link_socket(const SOCKET s, const unsigned long address)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			const auto server = find_server_by_address(address);
			if (!server) return false;

			socket_links[s] = server;
			return true;
		}

		void unlink_socket(const SOCKET sock)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			const auto server = socket_links.find(sock);
			if (server != socket_links.end())
			{
				socket_links.erase(server);
			}

			const auto dgram_packets = datagram_packets.find(sock);
			if (dgram_packets != datagram_packets.end())
			{
				datagram_packets.erase(dgram_packets);
			}
		}

		bool is_blocking_socket(const SOCKET s, const bool def)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			if (blocking_sockets.find(s) != blocking_sockets.end())
			{
				return blocking_sockets[s];
			}

			return def;
		}

		int recv_datagam_packet(const SOCKET s, char* buf, const int len, sockaddr* from, int* fromlen)
		{
			std::unique_lock<std::recursive_mutex> lock(server_mutex);

			auto queue = datagram_packets.find(s);
			if (queue != datagram_packets.end())
			{
				const auto blocking = is_blocking_socket(s, UDP_BLOCKING);

				lock.unlock();
				while (blocking && queue->second.empty())
				{
					std::this_thread::sleep_for(1ms);
				}
				lock.lock();

				if (!queue->second.empty())
				{
					auto [address, data] = queue->second.front();
					queue->second.pop();

					*fromlen = INT(address.size());
					std::memcpy(from, address.data(), address.size());

					const auto size = std::min(size_t(len), data.size());
					std::memcpy(buf, data.data(), size);

					return static_cast<int>(size);
				}

				WSASetLastError(WSAEWOULDBLOCK);
				return -1;
			}

			return 0;
		}

		void remove_blocking_socket(const SOCKET s)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);

			const auto entry = blocking_sockets.find(s);
			if (entry != blocking_sockets.end())
			{
				blocking_sockets.erase(entry);
			}
		}

		void set_blocking_socket(const SOCKET s, const bool blocking)
		{
			std::lock_guard<std::recursive_mutex> _(server_mutex);
			blocking_sockets[s] = blocking;
		}

		void server_thread()
		{
			terminate = false;
			while (!terminate)
			{
				std::unique_lock<std::recursive_mutex> lock(server_mutex);

				for (auto& server : servers)
				{
					server.second->run_frame();
				}

				lock.unlock();

				std::this_thread::sleep_for(50ms);
			}
		}

		void bd_logger_stub(int /*type*/, const char* const /*channelName*/, const char*, const char* const /*file*/,
		                    const char* const function, const unsigned int /*line*/, const char* const msg, ...)
		{		
			static const auto* bd_logger_enabled = game::Dvar_RegisterBool("bd_logger_enabled", false, game::DVAR_FLAG_NONE, "Enable bdLogger");
			if (!bd_logger_enabled->current.enabled)
			{
				return;
			}

			char buffer[2048]{};

			va_list ap;
			va_start(ap, msg);

			vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			printf("%s: %s\n", function, buffer);

			va_end(ap);
		}

		namespace io
		{
			int WINAPI send_to(const SOCKET s, const char* buf, const int len, const int flags, const sockaddr* to,
			                      const int tolen)
			{
				if (tolen == sizeof(sockaddr_in))
				{
					const auto* in_addr = reinterpret_cast<const sockaddr_in*>(to);
					const auto server = find_stun_server_by_address(in_addr->sin_addr.s_addr);
					if (server) return server->send(s, buf, len, to, tolen);
				}

				return sendto(s, buf, len, flags, to, tolen);
			}

			int WINAPI recv_from(const SOCKET s, char* buf, const int len, const int flags, sockaddr* from,
			                        int* fromlen)
			{
				auto res = recv_datagam_packet(s, buf, len, from, fromlen);
				if (res != 0) return res;

				res = recvfrom(s, buf, len, flags, from, fromlen);

				return res;
			}

			int WINAPI send(const SOCKET s, const char* buf, const int len, const int flags)
			{
				auto server = find_server_by_socket(s);
				if (server) return server->send(buf, len);

				return ::send(s, buf, len, flags);
			}

			int WINAPI recv(const SOCKET s, char* buf, const int len, const int flags)
			{
				auto server = find_server_by_socket(s);
				if (server)
				{
					const auto blocking = is_blocking_socket(s, TCP_BLOCKING);

					int result;
					do
					{
						result = server->recv(buf, len);
						if (blocking && result < 0) std::this_thread::sleep_for(1ms);
					}
					while (blocking && result < 0);

					if (!blocking && result < 0)
					{
						WSASetLastError(WSAEWOULDBLOCK);
					}

					return result;
				}

				return ::recv(s, buf, len, flags);
			}

			int WINAPI connect(const SOCKET s, const sockaddr* addr, const int len)
			{
				if (len == sizeof(sockaddr_in))
				{
					const auto* in_addr = reinterpret_cast<const sockaddr_in*>(addr);
					if (link_socket(s, in_addr->sin_addr.s_addr)) return 0;
				}

				return ::connect(s, addr, len);
			}

			int WINAPI close_socket(const SOCKET s)
			{
				remove_blocking_socket(s);
				unlink_socket(s);
				return closesocket(s);
			}

			int WINAPI ioctl_socket(const SOCKET s, const long cmd, u_long* argp)
			{
				if (static_cast<unsigned long>(cmd) == (FIONBIO))
				{
					set_blocking_socket(s, *argp == 0);
				}

				return ioctlsocket(s, cmd, argp);
			}

			hostent* WINAPI get_host_by_name(char* name)
			{
				unsigned long addr = 0;
				const auto server = find_server_by_name(name);
				if (server) addr = server->get_address();

				const auto stun_server = find_stun_server_by_name(name);
				if (stun_server) addr = stun_server->get_address();

				if (server || stun_server)
				{
					static thread_local in_addr address;
					address.s_addr = addr;

					static thread_local in_addr* addr_list[2];
					addr_list[0] = &address;
					addr_list[1] = nullptr;

					static thread_local hostent host;
					host.h_name = name;
					host.h_aliases = nullptr;
					host.h_addrtype = AF_INET;
					host.h_length = sizeof(in_addr);
					host.h_addr_list = reinterpret_cast<char**>(addr_list);

					return &host;
				}

#pragma warning(push)
#pragma warning(disable: 4996)
				return gethostbyname(name);
#pragma warning(pop)
			}

			bool register_hook(const std::string& process, void* stub)
			{
				const auto game_module = game_module::get_game_module();

				auto result = false;
				result = result || utils::hook::iat(game_module, "wsock32.dll", process, stub);
				result = result || utils::hook::iat(game_module, "WS2_32.dll", process, stub);
				return result;
			}
		}

		template <typename... Args>
		std::shared_ptr<service_server> register_server(Args ... args)
		{
			std::lock_guard _(server_mutex);
			auto server = std::make_shared<service_server>(args...);
			servers[server->get_address()] = server;
			return server;
		}

		std::shared_ptr<stun_server> register_stun_server(const std::string& name)
		{
			std::lock_guard _(server_mutex);
			auto server = std::make_shared<stun_server>(name);
			stun_servers[server->get_address()] = server;
			return server;
		}

		void startup_dw()
		{
			register_stun_server("ghosts-stun.us.demonware.net");
			register_stun_server("ghosts-stun.eu.demonware.net");
			register_stun_server("ghosts-stun.jp.demonware.net");
			register_stun_server("ghosts-stun.au.demonware.net");

			auto lsg_server = register_server("ghosts-pc-lobby.prod.demonware.net");
			auto auth_server = register_server("ghosts-pc-auth.prod.demonware.net");

			auth_server->register_service<bdDediAuth>();
			auth_server->register_service<bdSteamAuth>();
			auth_server->register_service<bdDediRSAAuth>();

			lsg_server->register_service<bdLSGHello>();
			lsg_server->register_service<bdStorage>();
			lsg_server->register_service<bdTitleUtilities>();
			lsg_server->register_service<bdDML>();
			lsg_server->register_service<bdMatchMaking>();
			lsg_server->register_service<bdBandwidthTest>();
			lsg_server->register_service<bdGroup>();
			lsg_server->register_service<bdAnticheat>();
			lsg_server->register_service<bdRelayService>();
		}
	}

	void send_datagram_packet(const SOCKET s, const std::string& data, const sockaddr* to, const int tolen)
	{
		std::lock_guard<std::recursive_mutex> _(server_mutex);
		datagram_packets[s].push({std::string(LPSTR(to), size_t(tolen)), data});
	}

	uint8_t* get_key(const bool encrypt)
	{
		return encrypt ? encryption_key_ : decryption_key_;
	}

	void set_key(const bool encrypt, uint8_t* key)
	{
		static_assert(sizeof encryption_key_ == sizeof decryption_key_);
		std::memcpy(encrypt ? encryption_key_ : decryption_key_, key, sizeof encryption_key_);
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			startup_dw();

			message_thread = utils::thread::create_named_thread("Demonware", server_thread);

			io::register_hook("send", io::send);
			io::register_hook("recv", io::recv);
			io::register_hook("sendto", io::send_to);
			io::register_hook("recvfrom", io::recv_from);
			io::register_hook("connect", io::connect);
			io::register_hook("closesocket", io::close_socket);
			io::register_hook("ioctlsocket", io::ioctl_socket);
			io::register_hook("gethostbyname", io::get_host_by_name);
		}

		void post_unpack() override
		{
			utils::hook::jump(SELECT_VALUE(0x140602230, 0x1406F54D0), bd_logger_stub);
		}

		void pre_destroy() override
		{
			std::lock_guard _(server_mutex);

			terminate = true;
			if (message_thread.joinable())
			{
				message_thread.join();
			}

			servers.clear();
			stun_servers.clear();
			socket_links.clear();
			blocking_sockets.clear();
			datagram_packets.clear();
		}
	};
}

REGISTER_COMPONENT(demonware::component)
