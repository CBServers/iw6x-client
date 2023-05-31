#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include "dvars.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>

namespace ranked
{
	namespace
	{
		utils::hook::detour bg_bot_system_enabled_hook;
		utils::hook::detour bg_ai_system_enabled_hook;
		utils::hook::detour bg_bot_fast_file_enabled_hook;
		utils::hook::detour bg_bots_using_team_difficulty_hook;

		int bg_bot_system_enabled_stub()
		{
			const auto* game_type = game::Dvar_FindVar("g_gametype")->current.string;
			if (!std::strcmp(game_type, "aliens") || !std::strcmp(game_type, "horde"))
			{
				return bg_bot_system_enabled_hook.invoke<int>();
			}

			return 1;
		}

		int bg_ai_system_enabled_stub()
		{
			const auto* game_type = game::Dvar_FindVar("g_gametype")->current.string;
			if (!std::strcmp(game_type, "aliens") || !std::strcmp(game_type, "horde"))
			{
				return bg_ai_system_enabled_hook.invoke<int>();
			}

			return 1;
		}

		int bg_bot_fast_file_enabled_stub()
		{
			const auto* game_type = game::Dvar_FindVar("g_gametype")->current.string;
			if (!std::strcmp(game_type, "aliens") || !std::strcmp(game_type, "horde"))
			{
				return bg_bot_fast_file_enabled_hook.invoke<int>();
			}

			return 1;
		}

		int bg_bots_using_team_difficulty_stub()
		{
			const auto* game_type = game::Dvar_FindVar("g_gametype")->current.string;
			if (!std::strcmp(game_type, "aliens") || !std::strcmp(game_type, "horde"))
			{
				return bg_bots_using_team_difficulty_hook.invoke<int>();
			}

			return 1;
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

			if (game::environment::is_mp())
			{
				// This must be registered as 'true' to avoid crash when starting a private match 
				dvars::override::register_bool("xblive_privatematch", true, game::DVAR_FLAG_REPLICATED);
			}

			if (game::environment::is_dedi() && !utils::flags::has_flag("unranked"))
			{
				dvars::override::register_bool("xblive_privatematch", false, game::DVAR_FLAG_WRITE);

				// Some dvar used in gsc
				game::Dvar_RegisterBool("force_ranking", true, game::DVAR_FLAG_WRITE, "Force ranking");

				// Fix sessionteam always returning none (SV_HasAssignedTeam_Internal)
				utils::hook::set(0x140479CF0, 0xC300B0);
			}

			// Always run bots, even if xblive_privatematch is 0
			bg_bot_system_enabled_hook.create(0x140217020, &bg_bot_system_enabled_stub);
			bg_ai_system_enabled_hook.create(0x140216DC0, &bg_ai_system_enabled_stub);
			bg_bot_fast_file_enabled_hook.create(0x140216F70, &bg_bot_fast_file_enabled_stub);
			bg_bots_using_team_difficulty_hook.create(0x1402170E0, &bg_bots_using_team_difficulty_stub);
		}

		void pre_destroy() override
		{
			bg_bot_system_enabled_hook.clear();
			bg_ai_system_enabled_hook.clear();
			bg_bot_fast_file_enabled_hook.clear();
			bg_bots_using_team_difficulty_hook.clear();
		}
	};
}

REGISTER_COMPONENT(ranked::component)
