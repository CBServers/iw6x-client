#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "party.hpp"
#include "network.hpp"
#include "server_list.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/cryptography.hpp>

namespace bots
{
	namespace
	{
		bool can_add()
		{
			return party::get_client_count() < *game::mp::svs_clientCount;
		}

		void bot_team_join(const int entity_num)
		{
			// schedule the select team call
			scheduler::once([entity_num]()
			{
				game::SV_ExecuteClientCommand(&game::mp::svs_clients[entity_num],
				                              utils::string::va("lui 68 2 %i", *game::mp::sv_serverId_value),
				                              false);

				// scheduler the select class call
				scheduler::once([entity_num]()
				{
					game::SV_ExecuteClientCommand(&game::mp::svs_clients[entity_num],
					                              utils::string::va("lui 5 %i %i", (rand() % 5) + 10,
					                                                *game::mp::sv_serverId_value), false);
				}, scheduler::pipeline::server, 1s);
			}, scheduler::pipeline::server, 1s);
		}

		void bot_team(const int entity_num)
		{
			if (game::SV_BotIsBot(game::mp::g_entities[entity_num].s.clientNum))
			{
				if (game::mp::g_entities[entity_num].client->sess.cs.team == game::mp::team_t::TEAM_SPECTATOR)
				{
					bot_team_join(entity_num);
				}

				scheduler::once([entity_num]()
				{
					bot_team(entity_num);
				}, scheduler::pipeline::server, 3s);
			}
		}

		void spawn_bot(const int entity_num)
		{
			scheduler::once([entity_num]()
			{
				game::SV_SpawnTestClient(&game::mp::g_entities[entity_num]);
				bot_team(entity_num);
			}, scheduler::pipeline::server, 1s);
		}

		void add_bot()
		{
			if (!can_add())
			{
				return;
			}

			auto* bot_name = game::SV_BotGetRandomName();
			auto* bot_ent = game::SV_AddBot(bot_name, 26, 62, 0);
			if (bot_ent)
			{
				spawn_bot(bot_ent->s.number);
			}
		}

		utils::hook::detour get_bot_name_hook;
		volatile bool bot_names_received = false;
		std::vector<std::string> bot_names{};

		const char* get_random_bot_name()
		{
			if (!bot_names_received || bot_names.empty())
			{
				return get_bot_name_hook.invoke<const char*>();
			}

			const auto index = utils::cryptography::random::get_integer() % bot_names.size();
			const auto& name = bot_names.at(index);

			return utils::string::va("%.*s", static_cast<int>(name.size()), name.data());
		}

		void update_bot_names()
		{
			bot_names_received = false;

			game::netadr_s master{};
			if (server_list::get_master_server(master))
			{
				console::info("Getting bots...\n");
				network::send(master, "getbots");
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			get_bot_name_hook.create(game::SV_BotGetRandomName, get_random_bot_name);

			command::add("spawnBot", [](const command::params& params)
			{
				if (!game::SV_Loaded()) return;

				auto num_bots = 1;
				if (params.size() == 2)
				{
					num_bots = atoi(params.get(1));
				}

				num_bots = std::min(num_bots, *game::mp::svs_clientCount);

				for (auto i = 0; i < num_bots; i++)
				{
					scheduler::once(add_bot, scheduler::pipeline::server, 100ms * i);
				}
			});

			scheduler::on_game_initialized([]()
			{
				update_bot_names();
				scheduler::loop(update_bot_names, scheduler::main, 1h);
			}, scheduler::main);

			network::on("getbotsResponse", [](const game::netadr_s& target, const std::string& data)
			{
				game::netadr_s master{};
				if (server_list::get_master_server(master) && !bot_names_received && target == master)
				{
					bot_names = utils::string::split(data, '\n');
					bot_names_received = true;
				}
			});
		}
	};
}

REGISTER_COMPONENT(bots::component)
