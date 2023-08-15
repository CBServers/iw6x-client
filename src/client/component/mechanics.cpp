#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace mechanics
{
	namespace
	{
		utils::hook::detour PM_BeginWeaponChange_hook;
		utils::hook::detour PM_Weapon_CheckForChangeWeapon_hook;
		game::dvar_t* pm_improvedMechanics;

		//mw2 mechanics thanks to @plugwalker47
		void PM_BeginWeaponChange_stub(game::pmove_t* pm, const game::Weapon newweapon, bool isNewAlternate, bool quick, unsigned int* holdrand)
		{
			int anim = pm->ps->weapState[0x0].weapAnim;
			int anim2 = pm->ps->weapState[0x1].weapAnim;
			bool keepanim;
			if ((pm->cmd.buttons & 3) != 0)
				keepanim = true;
			else
				keepanim = false;

			PM_BeginWeaponChange_hook.invoke<void>(pm, newweapon, isNewAlternate, quick, holdrand);

			if (pm_improvedMechanics->current.enabled && keepanim)
			{
				pm->ps->weapState[0x0].weapAnim = anim;
				pm->ps->weapState[0x1].weapAnim = anim2;
			}
		}

		void PM_Weapon_CheckForChangeWeapon_stub(game::pmove_t* pm, unsigned int* holdrand)
		{
			if (pm_improvedMechanics->current.enabled && pm->ps->weapon == pm->cmd.weapon && (unsigned int)(pm->ps->weapState[0x0].weaponState - 3) <= 2 && game::PM_Weapon_InValidChangeWeaponState(pm) && pm->ps->weapFlags != 128 && pm->ps->pm_flags != 8 && pm->ps->pm_flags != 40)
			{
				if (pm->ps->weapState[0x0].weapAnim == 30 || pm->ps->weapState[0x0].weapAnim == 2078)
					pm->ps->weapState[0x0].weapAnim = 31;
				else
					pm->ps->weapState[0x0].weapAnim = 1;
				if (pm->ps->weapState[0x1].weapAnim == 30 || pm->ps->weapState[0x1].weapAnim == 2078)
					pm->ps->weapState[0x1].weapAnim = 31;
				else
					pm->ps->weapState[0x1].weapAnim = 1;
				pm->ps->weapState[0x0].weaponState = 0;
				pm->ps->weapState[0x1].weaponState = 0;
				pm->ps->weapState[0x0].weaponDelay = 0;
				pm->ps->weapState[0x1].weaponDelay = 0;
				pm->ps->weapState[0x0].weaponTime = 0;
				pm->ps->weapState[0x1].weaponTime = 0;
				pm->ps->weapState[0x0].weaponRestrictKickTime = 0;
				pm->ps->weapState[0x1].weaponRestrictKickTime = 0;
				game::BG_ClearDropWeaponAnim(pm->ps);
				return;
			}

			PM_Weapon_CheckForChangeWeapon_hook.invoke<void>(pm, holdrand);
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

			PM_BeginWeaponChange_hook.create(0x14022E9D0, &PM_BeginWeaponChange_stub);
			PM_Weapon_CheckForChangeWeapon_hook.create(0x140231830, &PM_Weapon_CheckForChangeWeapon_stub);
			pm_improvedMechanics = game::Dvar_RegisterBool("pm_improvedMechanics", true,
				game::DVAR_FLAG_NONE, "Enable MW2 mechanics");
		}
	};
}

REGISTER_COMPONENT(mechanics::component)