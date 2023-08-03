#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace mechanics
{
	namespace
	{
		utils::hook::detour pm_weap_beginweapraise_hook;
		utils::hook::detour pm_weaponprocesshand_hook;
		game::dvar_t* pm_improvedMechanics;

		void PM_Weapon_BeginWeaponRaise(game::mp::playerState_s** ps_, unsigned int orr, unsigned int p2, unsigned int p4, unsigned int p5, game::PlayerHandIndex handIdx)
		{
			if (pm_improvedMechanics->current.enabled)
			{
				if (orr != 0x12 && orr != 0x13)
				{
					if (orr == 0x1B && p4 == 0x3)
					{
						orr = 0x13;
					}
					else
					{
						orr = 0x01;
					}
				}
			}
			
			pm_weap_beginweapraise_hook.invoke<void>(ps_, orr , p2, p4, p5, handIdx);
		}

		void PM_WeaponProcessHand(game::mp::playerState_s** ps_, long long p2, unsigned int p3, game::PlayerHandIndex handIdx) 
		{
			pm_weaponprocesshand_hook.invoke<void>(ps_, p2, p3, handIdx);

			if (pm_improvedMechanics->current.enabled)
			{
				auto* ps = *ps_;
				// YY cancel -> shoot
				if (ps->weapState[handIdx].weaponState == 0x1 || ps->weapState[handIdx].weaponState == 0x2 || ps->weapState[handIdx].weaponState == 0x5)
				{
					ps->weapState[handIdx].weaponState = 0x0;
					ps->weapState[handIdx].weaponTime = 0x0;
				}
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

			pm_weap_beginweapraise_hook.create(0x140231500, &PM_Weapon_BeginWeaponRaise);
			pm_weaponprocesshand_hook.create(0x140230be0, &PM_WeaponProcessHand);
			pm_improvedMechanics = game::Dvar_RegisterBool("pm_improvedMechanics", true,
				game::DVAR_FLAG_SAVED, "Enable improved YY mechanics");
		}
	};
}

REGISTER_COMPONENT(mechanics::component)