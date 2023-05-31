#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include "command.hpp"
#include "console.hpp"
#include "game_console.hpp"
#include "fastfiles.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>

namespace command
{
	namespace
	{
		constexpr auto CMD_MAX_NESTING = 8;

		utils::hook::detour client_command_hook;

		std::unordered_map<std::string, std::function<void(params&)>> handlers;
		std::unordered_map<std::string, std::function<void(int, params_sv&)>> handlers_sv;

		void main_handler()
		{
			params params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (const auto itr = handlers.find(command); itr != handlers.end())
			{
				itr->second(params);
			}
		}

		void client_command(const int client_num)
		{
			if (game::mp::g_entities[client_num].client == nullptr)
			{
				// Client is not fully connected
				return;
			}

			params_sv params = {};

			const auto command = utils::string::to_lower(params[0]);
			if (const auto itr = handlers_sv.find(command); itr != handlers_sv.end())
			{
				itr->second(client_num, params);
			}

			client_command_hook.invoke<void>(client_num);
		}

		// Shamelessly stolen from Quake3
		// https://github.com/id-Software/Quake-III-Arena/blob/dbe4ddb10315479fc00086f08e25d968b4b43c49/code/qcommon/common.c#L364
		void parse_command_line()
		{
			static auto parsed = false;
			if (parsed)
			{
				return;
			}

			static std::string comand_line_buffer = GetCommandLineA();
			auto* command_line = comand_line_buffer.data();

			auto& com_num_console_lines = *reinterpret_cast<int*>(0x1445CFF98);
			auto* com_console_lines = reinterpret_cast<char**>(0x1445CFFA0);

			auto inq = false;
			com_console_lines[0] = command_line;
			com_num_console_lines = 0;

			while (*command_line)
			{
				if (*command_line == '"')
				{
					inq = !inq;
				}
				// look for a + separating character
				// if commandLine came from a file, we might have real line seperators
				if ((*command_line == '+' && !inq) || *command_line == '\n' || *command_line == '\r')
				{
					if (com_num_console_lines == 0x20) // MAX_CONSOLE_LINES
					{
						break;
					}
					com_console_lines[com_num_console_lines] = command_line + 1;
					com_num_console_lines++;
					*command_line = '\0';
				}
				command_line++;
			}
			parsed = true;
		}

		void parse_commandline_stub()
		{
			parse_command_line();
			reinterpret_cast<void(*)()>(0x140413080)();
		}
	}

	void read_startup_variable(const std::string& dvar)
	{
		// parse the commandline if it's not parsed
		parse_command_line();

		auto com_num_console_lines = *reinterpret_cast<int*>(0x1445CFF98);
		auto* com_console_lines = reinterpret_cast<char**>(0x1445CFFA0);

		for (int i = 0; i < com_num_console_lines; i++)
		{
			game::Com_TokenizeString(com_console_lines[i]);

			// only +set dvar value
			if (game::Cmd_Argc() >= 3 && game::Cmd_Argv(0) == "set"s && game::Cmd_Argv(1) == dvar)
			{
				game::Dvar_SetCommand(game::Cmd_Argv(1), game::Cmd_Argv(2));
			}

			game::Com_EndTokenizeString();
		}
	}

	params::params()
		: nesting_(game::cmd_args->nesting)
	{
		assert(this->nesting_ < CMD_MAX_NESTING);
	}

	int params::size() const
	{
		return game::cmd_args->argc[this->nesting_];
	}

	const char* params::get(const int index) const
	{
		if (index >= this->size())
		{
			return "";
		}

		return game::cmd_args->argv[this->nesting_][index];
	}

	std::string params::join(const int index) const
	{
		std::string result = {};

		for (auto i = index; i < this->size(); i++)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}
		return result;
	}

	params_sv::params_sv()
		: nesting_(game::sv_cmd_args->nesting)
	{
		assert(this->nesting_ < CMD_MAX_NESTING);
	}

	int params_sv::size() const
	{
		return game::sv_cmd_args->argc[this->nesting_];
	}

	const char* params_sv::get(const int index) const
	{
		if (index >= this->size())
		{
			return "";
		}

		return game::sv_cmd_args->argv[this->nesting_][index];
	}

	std::string params_sv::join(const int index) const
	{
		std::string result = {};

		for (auto i = index; i < this->size(); i++)
		{
			if (i > index) result.append(" ");
			result.append(this->get(i));
		}
		return result;
	}

	void add_raw(const char* name, void (*callback)())
	{
		game::Cmd_AddCommandInternal(name, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
	}

	void add(const char* name, const std::function<void(const params&)>& callback)
	{
		const auto command = utils::string::to_lower(name);

		if (!handlers.contains(command))
		{
			add_raw(name, main_handler);
		}

		handlers[command] = callback;
	}

	void add(const char* name, const std::function<void()>& callback)
	{
		add(name, [callback](const params&)
		{
			callback();
		});
	}

	void add_sv(const char* name, const std::function<void(int, const params_sv&)>& callback)
	{
		// doing this so the sv command would show up in the console
		add_raw(name, nullptr);

		const auto command = utils::string::to_lower(name);

		if (!handlers_sv.contains(command))
		{
			handlers_sv[command] = callback;
		}
	}

	void execute(std::string command, const bool sync)
	{
		command += "\n";

		if (sync)
		{
			game::Cmd_ExecuteSingleCommand(0, 0, command.data());
		}
		else
		{
			game::Cbuf_AddText(0, command.data());
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				add_sp_commands();
			}
			else
			{
				utils::hook::call(0x14041213C, &parse_commandline_stub);

				add_mp_commands();
			}
			add_commands_generic();
		}

	private:
		static void add_commands_generic()
		{
			add("quit", game::Com_Quit);
			add("quit_hard", utils::nt::raise_hard_exception);
			add("crash", []
			{
				*reinterpret_cast<int*>(1) = 0x12345678;
			});

			add("dvarDump", [](const params& argument)
			{
				std::string filename;
				if (argument.size() == 2)
				{
					filename = "iw6x/";
					filename.append(argument[1]);
					if (!filename.ends_with(".txt"))
					{
						filename.append(".txt");
					}
				}

				console::info("================================ DVAR DUMP ========================================\n");
				for (auto i = 0; i < *game::dvarCount; i++)
				{
					auto* dvar = game::sortedDvars[i];
					if (dvar)
					{
						if (!filename.empty())
						{
							const auto line = std::format("{} \"{}\"\r\n", dvar->name, game::Dvar_ValueToString(dvar, dvar->current));
							utils::io::write_file(filename, line, i != 0);
						}

						console::info("%s \"%s\"\n", dvar->name, game::Dvar_ValueToString(dvar, dvar->current));
					}
				}

				console::info("\n%i dvars\n", *game::dvarCount);
				console::info("================================ END DVAR DUMP ====================================\n");
			});

			add("commandDump", [](const params& argument)
			{
				std::string filename;
				if (argument.size() == 2)
				{
					filename = "iw6x/";
					filename.append(argument[1]);
					if (!filename.ends_with(".txt"))
					{
						filename.append(".txt");
					}
				}

				console::info("================================ COMMAND DUMP =====================================\n");
				game::cmd_function_s* cmd = *game::cmd_functions;
				auto i = 0;
				while (cmd)
				{
					if (cmd->name)
					{
						if (!filename.empty())
						{
							const auto line = std::format("{}\r\n", cmd->name);
							utils::io::write_file(filename, line, i != 0);
						}

						console::info("%s\n", cmd->name);
						i++;
					}

					cmd = cmd->next;
				}
				console::info("\n%i commands\n", i);
				console::info("================================ END COMMAND DUMP =================================\n");
			});

			add("consoleList", [](const params& params)
			{
				const std::string input = params.get(1);

				std::vector<std::string> matches;
				game_console::find_matches(input, matches, false);

				for (auto& match : matches)
				{
					auto* dvar = game::Dvar_FindVar(match.c_str());
					if (!dvar)
					{
						console::info("[CMD]\t %s\n", match.c_str());
					}
					else
					{
						console::info("[DVAR]\t%s \"%s\"\n", match.c_str(), game::Dvar_ValueToString(dvar, dvar->current));
					}
				}

				console::info("Total %i matches\n", matches.size());
			});

			add("listassetpool", [](const params& params)
			{
				if (params.size() < 2)
				{
					console::info("listassetpool <poolnumber> [filter]: list all the assets in the specified pool\n");

					for (auto i = 0; i < game::XAssetType::ASSET_TYPE_COUNT; i++)
					{
						console::info("%d %s\n", i, game::g_assetNames[i]);
					}
				}
				else
				{
					const auto type = static_cast<game::XAssetType>(atoi(params.get(1)));

					if (type < 0 || type >= game::XAssetType::ASSET_TYPE_COUNT)
					{
						console::error("Invalid pool passed must be between [%d, %d]\n", 0, game::XAssetType::ASSET_TYPE_COUNT - 1);
						return;
					}

					console::info("Listing assets in pool %s\n", game::g_assetNames[type]);

					auto total_assets = 0;
					const std::string filter = params.get(2);
					fastfiles::enum_assets(type, [type, &total_assets, filter](const game::XAssetHeader header)
					{
						const game::XAsset asset{ type, header };
						const auto* const asset_name = game::DB_GetXAssetName(&asset);
						//const auto* const entry = game::DB_FindXAssetEntry(type, asset_name);
						//const char* zone_name;

						total_assets++;

						if (!filter.empty() && !game_console::match_compare(filter, asset_name, false))
						{
							return;
						}
						// TODO: in some cases returning garbage data
						//if (game::environment::is_sp())
						//{
						//	zone_name = game::sp::g_zones_0[entry->zoneIndex].name;
						//}
						//else
						//{
						//	zone_name = game::mp::g_zones_0[entry->zoneIndex].name;
						//}

						console::info("%s\n", asset_name);
					}, true);

					console::info("Total %s assets: %d/%d", game::g_assetNames[type], total_assets, game::g_poolSize[type]);
				}
			});

			add("vstr", [](const params& params)
			{
				if (params.size() < 2)
				{
					console::info("vstr <variablename> : execute a variable command\n");
					return;
				}

				const auto* dvarName = params.get(1);
				const auto* dvar = game::Dvar_FindVar(dvarName);

				if (dvar == nullptr)
				{
					console::info("%s doesn't exist\n", dvarName);
					return;
				}

				if (dvar->type != game::dvar_type::string
					&& dvar->type != game::dvar_type::enumeration)
				{
					console::info("%s is not a string-based dvar\n", dvar->name);
					return;
				}

				execute(dvar->current.string);
			});
		}
		
		void add_sp_commands()
		{
			add("god", []
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				game::sp::g_entities[0].flags ^= game::FL_GODMODE;
				game::CG_GameMessage(0, utils::string::va("godmode %s",
				                                          game::sp::g_entities[0].flags & game::FL_GODMODE
					                                          ? "^2on"
					                                          : "^1off"));
			});

			add("notarget", []
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				game::sp::g_entities[0].flags ^= game::FL_NOTARGET;
				game::CG_GameMessage(0, utils::string::va("notarget %s",
				                                          game::sp::g_entities[0].flags & game::FL_NOTARGET
					                                          ? "^2on"
					                                          : "^1off"));
			});
			
			add("noclip", []
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				game::sp::g_entities[0].client->flags ^= 1;
				game::CG_GameMessage(0, utils::string::va("noclip %s",
				                                          game::sp::g_entities[0].client->flags & 1
					                                          ? "^2on"
					                                          : "^1off"));
			});

			add("ufo", []
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				game::sp::g_entities[0].client->flags ^= 2;
				game::CG_GameMessage(0, utils::string::va("ufo %s", 
				                                          game::sp::g_entities[0].client->flags & 2 
					                                          ? "^2on" 
					                                          : "^1off"));
			});

			add("give", [](const params& params)
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				if (params.size() < 2)
				{
					game::CG_GameMessage(0, "You did not specify a weapon name");
					return;
				}

				auto ps = game::SV_GetPlayerstateForClientNum(0);
				auto wp = game::G_GetWeaponForName(params.get(1));
				if (game::G_GivePlayerWeapon(ps, wp, 0, 0, 0))
				{
					game::G_InitializeAmmo(ps, wp, 0);
					game::G_SelectWeapon(0, wp);
				}
			});

			add("take", [](const params& params)
			{
				if (!game::SV_Loaded())
				{
					return;
				}

				if (params.size() < 2)
				{
					game::CG_GameMessage(0, "You did not specify a weapon name");
					return;
				}

				auto ps = game::SV_GetPlayerstateForClientNum(0);
				auto wp = game::G_GetWeaponForName(params.get(1));
				game::G_TakePlayerWeapon(ps, wp);
			});
		}

		static void add_mp_commands()
		{
			client_command_hook.create(0x1403929B0, &client_command);

			add_sv("god", [&](const int client_num, const params_sv&)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				game::mp::g_entities[client_num].flags ^= game::FL_GODMODE;
				game::SV_GameSendServerCommand(client_num, 1,
				                               utils::string::va("f \"godmode %s\"",
				                                                 game::mp::g_entities[client_num].flags & game::FL_GODMODE
					                                                 ? "^2on"
					                                                 : "^1off"));
			});

			add_sv("notarget", [](const int client_num, const params_sv&)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				game::mp::g_entities[client_num].flags ^= game::FL_NOTARGET;
				game::SV_GameSendServerCommand(client_num, 1,
				                               utils::string::va("f \"notarget %s\"",
				     	                                         game::mp::g_entities[client_num].flags & game::FL_NOTARGET 
					                    	                         ? "^2on" 
				                    		                         : "^1off"));
			});

			add_sv("noclip", [](const int client_num, const params_sv&)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				game::mp::g_entities[client_num].client->flags ^= 1;
				game::SV_GameSendServerCommand(client_num, 1,
				                               utils::string::va("f \"noclip %s\"",
				                                                 game::mp::g_entities[client_num].client->flags & 1
					                                                 ? "^2on"
					                                                 : "^1off"));
			});

			add_sv("ufo", [](const int client_num, const params_sv&)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				game::mp::g_entities[client_num].client->flags ^= 2;
				game::SV_GameSendServerCommand(client_num, 1,
				                               utils::string::va("f \"ufo %s\"",
				                                                 game::mp::g_entities[client_num].client->flags & 2
					                                                 ? "^2on"
					                                                 : "^1off"));
			});

			add_sv("setviewpos", [](const int client_num, const params_sv& params)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				if (params.size() < 4)
				{
					game::SV_GameSendServerCommand(client_num, 1,
					                               "f \"You did not specify the correct number of coordinates\"");
					return;
				}

				game::mp::g_entities[client_num].client->ps.origin[0] = std::strtof(params.get(1), nullptr);
				game::mp::g_entities[client_num].client->ps.origin[1] = std::strtof(params.get(2), nullptr);
				game::mp::g_entities[client_num].client->ps.origin[2] = std::strtof(params.get(3), nullptr);
			});

			add_sv("setviewang", [](const int client_num, const params_sv& params)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				if (params.size() < 4)
				{
					game::SV_GameSendServerCommand(client_num, 1,
					                               "f \"You did not specify the correct number of coordinates\"");
					return;
				}

				game::mp::g_entities[client_num].client->ps.delta_angles[0] = std::strtof(params.get(1), nullptr);
				game::mp::g_entities[client_num].client->ps.delta_angles[1] = std::strtof(params.get(2), nullptr);
				game::mp::g_entities[client_num].client->ps.delta_angles[2] = std::strtof(params.get(3), nullptr);
			});

			add_sv("give", [](const int client_num, const params_sv& params)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				if (params.size() < 2)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"You did not specify a weapon name\"");
					return;
				}

				auto ps = game::SV_GetPlayerstateForClientNum(client_num);
				auto wp = game::G_GetWeaponForName(params.get(1));
				if (game::G_GivePlayerWeapon(ps, wp, 0, 0, 0))
				{
					game::G_InitializeAmmo(ps, wp, 0);
					game::G_SelectWeapon(client_num, wp);
				}
			});

			add_sv("take", [](const int client_num, const params_sv& params)
			{
				if (!dvars::sv_cheats->current.enabled)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"Cheats are not enabled on this server\"");
					return;
				}

				if (params.size() < 2)
				{
					game::SV_GameSendServerCommand(client_num, 1, "f \"You did not specify a weapon name\"");
					return;
				}

				auto ps = game::SV_GetPlayerstateForClientNum(client_num);
				auto wp = game::G_GetWeaponForName(params.get(1));
				game::G_TakePlayerWeapon(ps, wp);
			});
		}
	};
}

REGISTER_COMPONENT(command::component)
