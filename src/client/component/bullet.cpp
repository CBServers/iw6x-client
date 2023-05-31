#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace bullet
{
	namespace
	{
		utils::hook::detour bg_get_surface_penetration_depth_hook;

		float bg_get_surface_penetration_depth_stub(game::Weapon weapon, bool is_alternate, int surfaceType)
		{
			if (dvars::bg_surfacePenetration->current.value > 0.0f)
			{
				return dvars::bg_surfacePenetration->current.value;
			}

			return bg_get_surface_penetration_depth_hook.invoke<float>(weapon, is_alternate, surfaceType);
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

			dvars::bg_surfacePenetration = game::Dvar_RegisterFloat("bg_surfacePenetration", 0.0f,
				0.0f, std::numeric_limits<float>::max(), game::DVAR_FLAG_SAVED,
				"Set to a value greater than 0 to override the surface penetration depth");
			bg_get_surface_penetration_depth_hook.create(0x140238FD0, &bg_get_surface_penetration_depth_stub);
		}
	};
}

REGISTER_COMPONENT(bullet::component)
