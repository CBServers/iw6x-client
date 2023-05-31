#include <std_include.hpp>
#include "console.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "scheduler.hpp"
#include <utils\string.hpp>

namespace dedicated_info
{
	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			scheduler::once([]()
			{
				console::set_title("IW6x Dedicated Server");
				console::set_size(800, 600);
			});

			scheduler::loop([]()
			{
				auto* sv_running = game::Dvar_FindVar("sv_running");
				if (!sv_running || !sv_running->current.enabled)
				{
					console::set_title("IW6x Dedicated Server");
					return;
				}

				auto* const sv_hostname = game::Dvar_FindVar("sv_hostname");
				auto* const sv_maxclients = game::Dvar_FindVar("sv_maxclients");
				auto* const mapname = game::Dvar_FindVar("mapname");

				auto client_count = 0;
				auto bot_count = 0;

				for (auto i = 0; i < sv_maxclients->current.integer; i++)
				{
					auto* client = &game::mp::svs_clients[i];
					auto* self = &game::mp::g_entities[i];

					if (client->header.state > game::CS_FREE && self && self->client)
					{
						client_count++;
						if (game::SV_BotIsBot(i))
						{
							++bot_count;
						}
					}
				}

				std::string cleaned_hostname = sv_hostname->current.string;

				utils::string::strip(sv_hostname->current.string, cleaned_hostname.data(),
					cleaned_hostname.size() + 1);

				console::set_title(utils::string::va("%s on %s [%d/%d] (%d)", cleaned_hostname.data(),
				                                     mapname->current.string, client_count,
				                                     sv_maxclients->current.integer, bot_count));
			}, scheduler::pipeline::main, 1s);
		}
	};
}

REGISTER_COMPONENT(dedicated_info::component)
