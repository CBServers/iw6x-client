#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"
#include "game/ui_scripting/execution.hpp"

#include "server_list.hpp"
#include "console.hpp"
#include "command.hpp"
#include "dvars.hpp"
#include "localized_strings.hpp"
#include "network.hpp"
#include "scheduler.hpp"
#include "party.hpp"

#include <utils/cryptography.hpp>
#include <utils/string.hpp>
#include <utils/hook.hpp>

namespace server_list
{
	namespace
	{
		struct server_info
		{
			// gotta add more to this
			int clients;
			int max_clients;
			int bots;
			int ping;
			bool is_private;
			std::string host_name;
			std::string map_name;
			std::string game_type;
			char in_game;
			game::netadr_s address;
		};

		struct
		{
			game::netadr_s address{};
			volatile bool requesting = false;
			std::unordered_map<game::netadr_s, int> queued_servers{};
		} master_state;

		volatile bool update_server_list = false;

		std::mutex mutex;
		std::vector<server_info> servers;

		size_t server_list_index = 0;

		game::dvar_t* master_server_ip;
		game::dvar_t* master_server_port;

		void lui_open_menu_stub(int /*controllerIndex*/, const char* /*menu*/, int /*a3*/, int /*a4*/,
		                        unsigned int /*a5*/)
		{
			game::Cmd_ExecuteSingleCommand(0, 0, "lui_open menu_systemlink_join\n");
		}

		void refresh_server_list()
		{
			{
				std::lock_guard<std::mutex> _(mutex);
				servers.clear();
				master_state.queued_servers.clear();
				server_list_index = 0;
			}

			party::reset_connect_state();

			if (get_master_server(master_state.address))
			{
				master_state.requesting = true;
				network::send(master_state.address, "getservers", utils::string::va("IW6 %i full empty", PROTOCOL));
			}
		}

		void join_server(int, int, const int index)
		{
			std::lock_guard<std::mutex> _(mutex);

			const auto i = static_cast<size_t>(index) + server_list_index;
			if (i < servers.size())
			{
				static size_t last_index = 0xFFFFFFFF;
				if (last_index != i)
				{
					last_index = i;
				}
				else
				{
					console::info("Connecting to (%d - %zu): %s\n", index, i, servers[i].host_name.data());
					party::connect(servers[i].address);
				}
			}
		}

		void trigger_refresh()
		{
			update_server_list = true;
		}

		bool server_list_refresher()
		{
			if (update_server_list)
			{
				update_server_list = false;
			}

			return false;
		}

		int ui_feeder_count()
		{
			std::lock_guard<std::mutex> _(mutex);
			return static_cast<int>(servers.size() - server_list_index);
		}

		const char* ui_feeder_item_text(int /*localClientNum*/, void* /*a2*/, void* /*a3*/, const int index,
		                                const int column)
		{
			std::lock_guard<std::mutex> _(mutex);

			const auto i = server_list_index + index;

			if (i >= servers.size())
			{
				return "";
			}

			if (column == 0)
			{
				return utils::string::va("%s", servers[i].host_name.data());
			}

			if (column == 1)
			{
				return utils::string::va("%s", servers[i].map_name.data());
			}

			if (column == 2)
			{
				const auto client_count = servers[i].clients - servers[i].bots;
				return utils::string::va("%d/%d [%d]", client_count, servers[i].max_clients, servers[i].bots);
			}

			if (column == 3)
			{
				return utils::string::va("%s", servers[i].game_type.data());
			}

			if (column == 4)
			{
				return utils::string::va("%d", servers[i].ping);
			}

			if (column == 5)
			{
				return utils::string::va("%d", servers[i].is_private);
			}

			return "";
		}

		void sort_serverlist()
		{
			std::ranges::stable_sort(servers, [](const server_info& a, const server_info& b)
			{
				const auto a_players = a.clients - a.bots;
				const auto b_players = b.clients - b.bots;
				if (a_players == b_players)
				{
					return a.ping < b.ping;
				}

				return a_players > b_players;
			});
		}

		void insert_server(server_info&& server)
		{
			std::lock_guard<std::mutex> _(mutex);
			servers.emplace_back(std::move(server));
			sort_serverlist();
			trigger_refresh();
		}

		void do_frame_work()
		{
			auto& queue = master_state.queued_servers;
			if (queue.empty())
			{
				return;
			}

			std::lock_guard<std::mutex> _(mutex);

			size_t queried_servers = 0;
			const size_t query_limit = 3;

			for (auto i = queue.begin(); i != queue.end();)
			{
				if (i->second)
				{
					const auto now = game::Sys_Milliseconds();
					if (now - i->second > 10'000)
					{
						i = queue.erase(i);
						continue;
					}
				}
				else if (queried_servers++ < query_limit)
				{
					i->second = game::Sys_Milliseconds();
					network::send(i->first, "getInfo", utils::cryptography::random::get_challenge());
				}

				++i;
			}
		}

		bool is_server_list_open()
		{
			return game::Menu_IsMenuOpenAndVisible(0, "menu_systemlink_join");
		}

		bool scroll_down()
		{
			if (!is_server_list_open())
			{
				return false;
			}

			if (server_list_index + 16 < servers.size())
			{
				++server_list_index;
				trigger_refresh();
			}
			
			return true;
		}

		bool scroll_up()
		{
			if (!is_server_list_open())
			{
				return false;
			}

			if (server_list_index > 0)
			{
				--server_list_index;
				trigger_refresh();
			}

			return true;
		}

		int get_client_count()
		{
			std::lock_guard<std::mutex> _(mutex);
			auto count = 0;

			for (const auto& server : servers)
			{
				count += server.clients;
			}

			return count;
		}

		int get_bot_count()
		{
			std::lock_guard<std::mutex> _(mutex);
			auto count = 0;

			for (const auto& server : servers)
			{
				count += server.bots;
			}

			return count;
		}

		int get_max_clients_count()
		{
			std::lock_guard<std::mutex> _(mutex);
			auto count = 0;

			for (const auto& server : servers)
			{
				count += server.max_clients;
			}

			return count;
		}

		int get_total_active_players_count_stub(game::hks::lua_State* s, void* a2)
		{
			const auto clients = get_client_count();
			const auto bots = get_bot_count();
			const auto max = get_max_clients_count();

			const auto str = utils::string::va("%d/%d [%d]", clients, max, bots);
			ui_scripting::push_value(str);

			return 1;
		}
	}

	bool get_master_server(game::netadr_s& address)
	{
		return game::NET_StringToAdr(utils::string::va("%s:%s",
			master_server_ip->current.string, master_server_port->current.string), &address);
	}

	void handle_info_response(const game::netadr_s& address, const utils::info_string& info)
	{
		int start_time{};
		const auto now = game::Sys_Milliseconds();

		{
			std::lock_guard<std::mutex> _(mutex);
			const auto entry = master_state.queued_servers.find(address);

			if (entry == master_state.queued_servers.end() || !entry->second)
			{
				return;
			}

			start_time = entry->second;
			master_state.queued_servers.erase(entry);
		}

		if (dvars::get_string("ui_customModeName") == "mp"s)
		{
			if (info.get("gametype") == "aliens"s)
			{
				return;
			}
		}
		else if (dvars::get_string("ui_customModeName") == "aliens"s)
		{
			if (info.get("gametype") != "aliens"s)
			{
				return;
			}
		}

		if (dvars::get_string("ui_mapvote_entrya_gametype") != "any"s && dvars::get_string("ui_mapvote_entrya_gametype") != info.get("gametype"))
		{
			return;
		}

		if (dvars::get_string("ui_mapvote_entrya_mapname") != "any"s && dvars::get_string("ui_mapvote_entrya_mapname") != info.get("mapname"))
		{
			return;
		}

		server_info server{};
		server.address = address;
		server.host_name = info.get("hostname");
		server.map_name = game::UI_LocalizeMapname(info.get("mapname").data());
		server.game_type = game::UI_LocalizeGametype(info.get("gametype").data());
		server.clients = atoi(info.get("clients").data());
		server.max_clients = atoi(info.get("sv_maxclients").data());
		server.bots = atoi(info.get("bots").data());
		server.ping = now - start_time;
		server.is_private = info.get("isPrivate") == "1"s;
		server.in_game = 1;

		if (server.host_name.size() > 50)
		{
			server.host_name.resize(50);
		}

		insert_server(std::move(server));
	}

	bool sl_key_event(const int key, const int down)
	{
		if (down)
		{
			if (key == game::keyNum_t::K_MWHEELUP)
			{
				return !scroll_up();
			}

			if (key == game::keyNum_t::K_MWHEELDOWN)
			{
				return !scroll_down();
			}
		}

		return true;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_sp())
			{
				scheduler::once([]()
				{
					// add dvars to change destination master server ip/port
					master_server_ip = game::Dvar_RegisterString("masterServerIP", "server.alterware.dev", game::DVAR_FLAG_NONE,
						"IP of the destination master server to connect to");
					master_server_port = game::Dvar_RegisterString("masterServerPort", "20810", game::DVAR_FLAG_NONE,
						"Port of the destination master server to connect to");
				}, scheduler::pipeline::main);
			}

			if (!game::environment::is_mp()) return;

			localized_strings::override("PLATFORM_SYSTEM_LINK_TITLE", "SERVER LIST");
			localized_strings::override("LUA_MENU_STORE_CAPS", "SERVER LIST");
			localized_strings::override("LUA_MENU_STORE_DESC", "Browse available servers.");
			localized_strings::override("MENU_NUMPLAYERS", "Players");

			// hook LUI_OpenMenu to show server list instead of store popup
			utils::hook::call(0x1404FE840, &lui_open_menu_stub);

			// refresh server list when needed
			utils::hook::call(0x1402F7480, &server_list_refresher);

			// replace UI_RunMenuScript call in LUI_CoD_LuaCall_RefreshServerList to our refresh_servers
			utils::hook::call(0x1401E7171, &refresh_server_list);
			utils::hook::call(0x1401E7616, &join_server);
			utils::hook::nop(0x1401E7635, 5);

			// do feeder stuff
			utils::hook::call(0x1401E7225, &ui_feeder_count);
			utils::hook::call(0x1401E7405, &ui_feeder_item_text);

			utils::hook::jump(0x1401EBE30, &get_total_active_players_count_stub, true);
			scheduler::once(refresh_server_list, scheduler::pipeline::main);

			command::add("lui_open", [](const command::params& params)
			{
				if (params.size() <= 1)
				{
					console::info("usage: lui_open <name>\n");
					return;
				}

				game::LUI_OpenMenu(0, params[1], 1, 0, 0);
			});

			scheduler::loop(do_frame_work, scheduler::pipeline::main);

			network::on("getServersResponse", [](const game::netadr_s& target, const std::string& data)
			{
				{
					std::lock_guard<std::mutex> _(mutex);
					if (!master_state.requesting || master_state.address != target)
					{
						return;
					}

					master_state.requesting = false;

					std::optional<size_t> start{};
					for (std::size_t i = 0; i + 6 < data.size(); ++i)
					{
						if (data[i + 6] == '\\')
						{
							start.emplace(i);
							break;
						}
					}

					if (!start.has_value())
					{
						return;
					}

					for (auto i = start.value(); i + 6 < data.size(); i += 7)
					{
						if (data[i + 6] != '\\')
						{
							break;
						}

						game::netadr_s address{};
						address.type = game::NA_IP;
						address.localNetID = game::NS_CLIENT1;
						std::memcpy(&address.ip[0], data.data() + i + 0, 4);
						std::memcpy(&address.port, data.data() + i + 4, 2);

						master_state.queued_servers[address] = 0;
					}
				}
			});
		}
	};
}

REGISTER_COMPONENT(server_list::component)
