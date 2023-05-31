#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "network.hpp"
#include "scheduler.hpp"

namespace rcon
{
	namespace
	{
		bool is_redirecting_ = false;
		game::netadr_s redirect_target_ = {};
		std::recursive_mutex redirect_lock;

		void setup_redirect(const game::netadr_s& target)
		{
			std::lock_guard<std::recursive_mutex> $(redirect_lock);

			is_redirecting_ = true;
			redirect_target_ = target;
		}

		void clear_redirect()
		{
			std::lock_guard<std::recursive_mutex> $(redirect_lock);

			is_redirecting_ = false;
			redirect_target_ = {};
		}

		std::string build_status_buffer()
		{
			const auto sv_maxclients = game::Dvar_FindVar("sv_maxclients");
			const auto mapname = game::Dvar_FindVar("mapname");

			auto buffer = ""s;
			buffer.append(utils::string::va("map: %s\n", mapname->current.string));
			buffer.append(
				"num score bot ping guid                             name             address               qport\n");
			buffer.append(
				"--- ----- --- ---- -------------------------------- ---------------- --------------------- -----\n");

			for (int i = 0; i < sv_maxclients->current.integer; i++)
			{
				const auto client = &game::mp::svs_clients[i];
				auto self = &game::mp::g_entities[i];

				char clean_name[32] = {0};
				strncpy_s(clean_name, self->client->sess.cs.name, sizeof(clean_name));
				game::I_CleanStr(clean_name);

				if (client->header.state > game::CS_FREE && self && self->client)
				{
					buffer.append(utils::string::va("%3i %5i %3s %s %32s %16s %21s %5i\n",
					                                i,
					                                self->client->sess.scores.score,
					                                game::SV_BotIsBot(i) ? "Yes" : "No",
					                                (client->header.state == game::CS_RECONNECTING)
						                                ? "CNCT"
						                                : (client->header.state == game::CS_ZOMBIE)
						                                ? "ZMBI"
						                                : utils::string::va("%4i", client->ping),
					                                game::SV_GetGuid(i),
					                                clean_name,
					                                network::net_adr_to_string(client->header.netchan.remoteAddress),
					                                client->header.netchan.remoteAddress.port)
					);
				}
			}

			return buffer;
		}

		void send_rcon_command(const std::string& password, const std::string& data)
		{
			// If you are the server, don't bother with rcon and just execute the command
			if (game::Dvar_FindVar("sv_running")->current.enabled)
			{
				game::Cbuf_AddText(0, data.data());
				return;
			}

			if (password.empty())
			{
				console::info("You must login first to use RCON\n");
				return;
			}

			if (*reinterpret_cast<std::int32_t*>(0x1419E1AE0) >= 5) //clientUIActive.connectionState >= CA_CONNECTED
			{
				const auto target = *reinterpret_cast<game::netadr_s*>(0x141CB535C);
				const auto buffer = password + " " + data;
				network::send(target, "rcon", buffer);
			}
			else
			{
				console::warn("You need to be connected to a server!\n");
			}
		}
	}

	bool message_redirect(const std::string& message)
	{
		std::lock_guard<std::recursive_mutex> $(redirect_lock);

		if (is_redirecting_)
		{
			network::send(redirect_target_, "print\n", message);
			return true;
		}
		return false;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp()) return;

			scheduler::once([]()
			{
				game::Dvar_RegisterString("rcon_password", "", game::DvarFlags::DVAR_FLAG_NONE,
				                          "The password for remote console");
			}, scheduler::pipeline::main);

			command::add("status", [&]()
			{
				const auto sv_running = game::Dvar_FindVar("sv_running");
				if (!sv_running || !sv_running->current.enabled)
				{
					console::error("Server is not running\n");
					return;
				}

				auto status_buffer = build_status_buffer();
				console::info(status_buffer.data());
			});

			if (!game::environment::is_dedi())
			{
				command::add("rcon", [&](const command::params& params)
				{
					static std::string rcon_password{};

					if (params.size() < 2) return;

					const auto operation = params.get(1);
					if (operation == "login"s)
					{
						if (params.size() < 3)
							return;

						rcon_password = params.get(2);
					}
					else if (operation == "logout"s)
					{
						rcon_password.clear();
					}
					else
					{
						send_rcon_command(rcon_password, params.join(1));
					}
				});
			}
			else
			{
				network::on("rcon", [](const game::netadr_s& addr, const std::string& data)
				{
					const auto pos = data.find_first_of(" ");
					if (pos == std::string::npos)
					{
						console::info("Invalid RCon request from %s\n", network::net_adr_to_string(addr));
						return;
					}

					const auto password = data.substr(0, pos);
					const auto command = data.substr(pos + 1);
					const auto rcon_password = game::Dvar_FindVar("rcon_password");
					if (command.empty() || !rcon_password || !rcon_password->current.string || !strlen(
						rcon_password->current.string))
					{
						return;
					}

					setup_redirect(addr);

					if (password != rcon_password->current.string)
					{
						console::error("Invalid rcon password\n");
					}
					else
					{
						command::execute(command, true);
					}

					clear_redirect();
				});
			}
		}
	};
}

REGISTER_COMPONENT(rcon::component)
