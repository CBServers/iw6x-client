#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace gameplay
{
	namespace
	{
		template <typename T, typename R>
		constexpr auto VectorScale(T v, R s, T out) { out[0] = v[0] * s; out[1] = v[1] * s; out[2] = v[2] * s; }

		constexpr auto JUMP_LAND_SLOWDOWN_TIME = 1800;

		utils::hook::detour pm_weapon_use_ammo_hook;

		int stuck_in_client_stub(void* self)
		{
			if (dvars::g_playerEjection->current.enabled)
			{
				return utils::hook::invoke<int>(0x140386950, self); // StuckInClient
			}

			return 0;
		}

		void cm_transformed_capsule_trace_stub(game::trace_t* results, const float* start, const float* end, 
			game::Bounds* bounds, game::Bounds* capsule, int contents, const float* origin, const float* angles)
		{
			if (dvars::g_playerCollision->current.enabled)
			{
				utils::hook::invoke<void>(0x1403F3050,
				results, start, end, bounds, capsule, contents, origin, angles); // CM_TransformedCapsuleTrace
			}
		}

		const auto g_gravity_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::g_gravity)));
			a.mov(eax, dword_ptr(rax, 0x10));
			a.mov(dword_ptr(rbx, 0x5C), eax);
			a.mov(eax, ptr(rbx, 0x33E8));
			a.mov(ptr(rbx, 0x25C), eax);

			a.pop(rax);

			a.jmp(0x1403828D5);
		});

		const auto g_speed_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::g_speed)));
			a.mov(eax, dword_ptr(rax, 0x10));
			a.mov(dword_ptr(rdi, 0x60), eax);

			a.pop(rax);

			a.mov(eax, ptr(rdi, 0xEA4));
			a.add(eax, ptr(rdi, 0xEA0));

			a.jmp(0x140383796);
		});

		const auto pm_bouncing_stub_sp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto no_bounce = a.newLabel();
			const auto loc_14046ED26 = a.newLabel();

			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::pm_bouncing)));
			a.mov(al, byte_ptr(rax, 0x10));
			a.cmp(ptr(rbp, -0x40), al);

			a.pop(rax);
			a.jz(no_bounce);
			a.jmp(0x14046EC7E);

			a.bind(no_bounce);
			a.cmp(ptr(rbp, -0x80), r13d);
			a.jnz(loc_14046ED26);
			a.jmp(0x14046EC6C);

			a.bind(loc_14046ED26);
			a.jmp(0x14046ED26);
		});

		const auto pm_bouncing_stub_mp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			const auto no_bounce = a.newLabel();
			const auto loc_140228FB8 = a.newLabel();

			a.push(rax);

			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::pm_bouncing)));
			a.mov(al, byte_ptr(rax, 0x10));
			a.cmp(byte_ptr(rbp, -0x38), al);

			a.pop(rax);
			a.jz(no_bounce);
			a.jmp(0x140229019);

			a.bind(no_bounce);
			a.cmp(dword_ptr(rbp, -0x70), 0);
			a.jnz(loc_140228FB8);
			a.jmp(0x14022900B);

			a.bind(loc_140228FB8);
			a.jmp(0x140228FB8);
		});

		void pm_crashland_stub(void* ps, void* pml)
		{
			if (dvars::jump_enableFallDamage->current.enabled)
			{
				reinterpret_cast<void(*)(void*, void*)>(0x140220000)(ps, pml);
			}
		}

		void jump_apply_slowdown_stub(game::mp::playerState_s* ps)
		{
			auto scale = 1.0f;

			if (ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME)
			{
				if (ps->pm_time == 0)
				{
					const auto height = ps->jumpOriginZ + 18.0f;
					if (height <= ps->origin[2])
					{
						scale = 0.5f;
						ps->pm_time = 1200;
					}
					else
					{
						scale = 0.65f;
						ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
					}
				}
			}
			else
			{
				game::Jump_ClearState(ps);
				scale = 0.65f;
			}

			if (!dvars::jump_slowdownEnable->current.enabled)
			{
				VectorScale(ps->velocity, scale, ps->velocity);
			}
		}

		float get_jump_height_stub(void* pmove)
		{
			auto jump_height = reinterpret_cast<float (*)(void*)>(0x140213140)(pmove);

			if (jump_height == 39.f)
			{
				jump_height = dvars::jump_height->current.value;
			}

			return jump_height;
		}

		const auto jump_push_off_ladder_stub = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.mov(rax, qword_ptr(reinterpret_cast<int64_t>(&dvars::jump_ladderPushVel)));
			a.movaps(xmm8, dword_ptr(rax, 0x10));

			a.mulss(xmm6, xmm8);
			a.mulss(xmm7, xmm8);

			a.jmp(0x140213494);
		});

		void pm_player_trace_stub(game::pmove_t* move, game::trace_t* trace, const float* f3,
			const float* f4, const game::Bounds* bounds, int a6, int a7)
		{
			game::PM_playerTrace(move, trace, f3, f4, bounds, a6, a7);

			if (dvars::g_enableElevators->current.enabled)
			{
				trace->startsolid = false;
			}
		}

		void pm_trace_stub(const game::pmove_t* move, game::trace_t* trace, const float* f3,
			const float* f4, const game::Bounds* bounds, int a6, int a7)
		{
			game::PM_trace(move, trace, f3, f4, bounds, a6, a7);

			if (dvars::g_enableElevators->current.enabled)
			{
				trace->allsolid = false;
			}
		}

		void pm_weapon_use_ammo_stub(game::playerState_s* ps, game::Weapon weapon,
			bool is_alternate, int amount, game::PlayerHandIndex hand)
		{
			if (!dvars::player_sustainAmmo->current.enabled)
			{
				pm_weapon_use_ammo_hook.invoke<void>(ps, weapon, is_alternate, amount, hand);
			}
		}

		game::mp::gentity_s* weapon_rocket_launcher_fire_stub(game::mp::gentity_s* ent, game::Weapon weapon, float spread, game::weaponParms* wp,
			const float* gun_vel, game::mp::missileFireParms* fire_parms, bool magic_bullet)
		{
			auto* result = utils::hook::invoke<game::mp::gentity_s*>(0x1403DB8A0, ent, weapon, spread, wp, gun_vel, fire_parms, magic_bullet);

			if (ent->client != nullptr && wp->weapDef->inventoryType != game::WEAPINVENTORY_EXCLUSIVE)
			{
				const auto scale = dvars::g_rocketPushbackScale->current.value;
				ent->client->ps.velocity[0] += (0.0f - wp->forward[0]) * scale;
				ent->client->ps.velocity[1] += (0.0f - wp->forward[1]) * scale;
				ent->client->ps.velocity[2] += (0.0f - wp->forward[2]) * scale;
			}

			return result;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Implement bouncing dvar
			if (game::environment::is_sp())
			{
				utils::hook::nop(0x14046EC5C, 16);
			}

			utils::hook::jump(
				SELECT_VALUE(0x14046EC5C, 0x140228FFF), SELECT_VALUE(pm_bouncing_stub_sp, pm_bouncing_stub_mp), true);
			dvars::pm_bouncing = game::Dvar_RegisterBool("pm_bouncing", false,
			                                             game::DVAR_FLAG_REPLICATED, "Enable bouncing");

			dvars::player_sustainAmmo = game::Dvar_RegisterBool("player_sustainAmmo", false,
				game::DVAR_FLAG_REPLICATED, "Firing weapon will not decrease clip ammo.");
			pm_weapon_use_ammo_hook.create(SELECT_VALUE(0x140479640, 0x140238A90), &pm_weapon_use_ammo_stub);

			if (game::environment::is_sp()) return;

			// Implement player ejection dvar
			dvars::g_playerEjection = game::Dvar_RegisterBool("g_playerEjection", true, game::DVAR_FLAG_REPLICATED, "Flag whether player ejection is on or off");
			utils::hook::call(0x140382C13, stuck_in_client_stub);

			// Implement player collision dvar
			dvars::g_playerCollision = game::Dvar_RegisterBool("g_playerCollision", true, game::DVAR_FLAG_REPLICATED, "Flag whether player collision is on or off");
			utils::hook::call(0x14048A49A, cm_transformed_capsule_trace_stub); // SV_ClipMoveToEntity
			utils::hook::call(0x1402B5B88, cm_transformed_capsule_trace_stub); // CG_ClipMoveToEntity

			// Implement gravity dvar
			utils::hook::nop(0x1403828C8, 13);
			utils::hook::jump(0x1403828C8, g_gravity_stub, true);
			dvars::g_gravity = game::Dvar_RegisterInt("g_gravity", 800, 0, 1000, game::DVAR_FLAG_NONE,
			                                          "Game gravity in inches per second squared");

			// Implement speed dvar
			utils::hook::nop(0x140383789, 13);
			utils::hook::jump(0x140383789, g_speed_stub, true);
			dvars::g_speed = game::Dvar_RegisterInt("g_speed", 190, 0, 999, game::DVAR_FLAG_NONE, "Maximum player speed");

			utils::hook::call(0x140225857, jump_apply_slowdown_stub);
			dvars::jump_slowdownEnable = game::Dvar_RegisterBool("jump_slowdownEnable", true,
			                                                     game::DVAR_FLAG_REPLICATED,
			                                                     "Slow player movement after jumping");

			utils::hook::call(0x1402219A5, pm_crashland_stub);
			dvars::jump_enableFallDamage = game::Dvar_RegisterBool("jump_enableFallDamage", true,
			                                                       game::DVAR_FLAG_REPLICATED,
			                                                       "Enable fall damage");

			utils::hook::call(0x140213007, get_jump_height_stub);
			dvars::jump_height = game::Dvar_RegisterFloat("jump_height", 39.f, 0.f, 1024.f,
			                                              game::DVAR_FLAG_REPLICATED, "Jump height");

			utils::hook::jump(0x140213484, jump_push_off_ladder_stub, true);
			dvars::jump_ladderPushVel = game::Dvar_RegisterFloat("jump_ladderPushVel", 128.f, 0.f, 1024.f,
			                                                     game::DVAR_FLAG_REPLICATED,
			                                                     "Ladder push velocity");

			utils::hook::call(0x140221F92, pm_player_trace_stub);
			utils::hook::call(0x140221FFA, pm_player_trace_stub);
			utils::hook::call(0x14021F0E3, pm_trace_stub);
			dvars::g_enableElevators = game::Dvar_RegisterBool("g_enableElevators", false,
				game::DVAR_FLAG_REPLICATED, "Enable Elevators");

			utils::hook::call(0x1403D933E, weapon_rocket_launcher_fire_stub);
			dvars::g_rocketPushbackScale = game::Dvar_RegisterFloat("g_rocketPushbackScale", 1.0f, 1.0f, std::numeric_limits<float>::max(),
				game::DVAR_FLAG_REPLICATED, "The scale applied to the pushback force of a rocket");
		}
	};
}

REGISTER_COMPONENT(gameplay::component)
