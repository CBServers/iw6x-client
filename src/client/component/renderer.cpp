#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

namespace renderer
{
	namespace
	{
		utils::hook::detour r_init_draw_method_hook;
		utils::hook::detour r_update_front_end_dvar_options_hook;

		void r_init_draw_method_stub()
		{
			game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;
			game::gfxDrawMethod->baseTechType = dvars::r_fullbright->current.enabled ? game::TECHNIQUE_UNLIT : game::TECHNIQUE_LIT;
			game::gfxDrawMethod->emissiveTechType = dvars::r_fullbright->current.enabled ? game::TECHNIQUE_UNLIT : game::TECHNIQUE_EMISSIVE;
			game::gfxDrawMethod->forceTechType = dvars::r_fullbright->current.enabled ? game::TECHNIQUE_UNLIT : 0x19E;
		}

		bool r_update_front_end_dvar_options_stub()
		{
			if (dvars::r_fullbright->modified)
			{
				dvars::r_fullbright->modified = false;
				game::R_SyncRenderThread();
				
				game::gfxDrawMethod->drawScene = game::GFX_DRAW_SCENE_STANDARD;
				game::gfxDrawMethod->baseTechType = dvars::r_fullbright->current.enabled ? game::TECHNIQUE_UNLIT : game::TECHNIQUE_LIT;
				game::gfxDrawMethod->emissiveTechType = dvars::r_fullbright->current.enabled ? game::TECHNIQUE_UNLIT : game::TECHNIQUE_EMISSIVE;
				game::gfxDrawMethod->forceTechType = dvars::r_fullbright->current.enabled ? game::TECHNIQUE_UNLIT : 0x19E;
			}

			return r_update_front_end_dvar_options_hook.invoke<bool>();
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_linker())
			{
				return;
			}

			dvars::r_fullbright = game::Dvar_RegisterBool("r_fullbright", false, game::DvarFlags::DVAR_FLAG_SAVED,
			                                              "Toggles rendering without lighting");

			r_init_draw_method_hook.create(SELECT_VALUE(0x1404FF600, 0x1405CB470), &r_init_draw_method_stub);
			r_update_front_end_dvar_options_hook.create(
				SELECT_VALUE(0x140535FF0, 0x140603240), &r_update_front_end_dvar_options_stub);
		}
	};
}

REGISTER_COMPONENT(renderer::component)
