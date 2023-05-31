#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/scripting/entity.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/lua/value_conversion.hpp"
#include "game/scripting/lua/error.hpp"

#include "command.hpp"
#include "notifies.hpp"
#include "scripting.hpp"
#include "scheduler.hpp"
#include "game_log.hpp"

#include <utils/hook.hpp>

namespace notifies
{
	bool hook_enabled = true;

	namespace
	{
		struct gsc_hook
		{
			bool is_lua_hook{};
			const char* target_pos{};
			sol::protected_function lua_function;
		};

		std::unordered_map<const char*, gsc_hook> vm_execute_hooks;
		utils::hook::detour scr_player_killed_hook;
		utils::hook::detour scr_player_damage_hook;

		std::vector<sol::protected_function> player_killed_callbacks;
		std::vector<sol::protected_function> player_damage_callbacks;

		char empty_function[2] = {0x32, 0x34}; // CHECK_CLEAR_PARAMS, END
		const char* target_function = nullptr;

		sol::lua_value convert_entity(lua_State* state, const game::mp::gentity_s* ent)
		{
			if (!ent)
			{
				return {};
			}

			const scripting::entity player{game::Scr_GetEntityId(ent->s.number, 0)};
			return scripting::lua::convert(state, player);
		}

		std::string get_weapon_name(unsigned int weapon, bool is_alternate)
		{
			char output[1024]{};
			game::BG_GetWeaponNameComplete(weapon, is_alternate, output, sizeof(output));

			return output;
		}

		sol::lua_value convert_vector(lua_State* state, const float* vec)
		{
			if (!vec)
			{
				return {};
			}

			const auto _vec = scripting::vector(vec);
			return scripting::lua::convert(state, _vec);
		}

		std::string convert_mod(const int means_of_death)
		{
			const auto value = reinterpret_cast<unsigned int**>(0x1409E6360)[means_of_death];
			return game::SL_ConvertToString(*value);
		}

		void scr_player_killed_stub(game::mp::gentity_s* self, const game::mp::gentity_s* inflictor, game::mp::gentity_s* attacker, int damage,
			const int means_of_death, const unsigned int weapon, const bool is_alternate, const float* v_dir, const unsigned int hit_loc, int ps_time_offset, int death_anim_duration)
		{
			{
				const std::string _hit_loc = reinterpret_cast<const char**>(0x1409E62B0)[hit_loc];
				const auto _mod = convert_mod(means_of_death);

				const auto _weapon = get_weapon_name(weapon, is_alternate);

				for (const auto& callback : player_killed_callbacks)
				{
					const auto state = callback.lua_state();

					const auto _self = convert_entity(state, self);
					const auto _inflictor = convert_entity(state, inflictor);
					const auto _attacker = convert_entity(state, attacker);

					const auto _v_dir = convert_vector(state, v_dir);

					const auto result = callback(_self, _inflictor, _attacker, damage, _mod, _weapon, _v_dir, _hit_loc, ps_time_offset, death_anim_duration);

					scripting::lua::handle_error(result);

					if (result.valid() && result.get_type() == sol::type::number)
					{
						damage = result.get<int>();
					}
				}

				if (damage == 0)
				{
					return;
				}
			}

			scr_player_killed_hook.invoke<void>(self, inflictor, attacker, damage, means_of_death, weapon, is_alternate, v_dir, hit_loc, ps_time_offset, death_anim_duration);
		}

		void scr_player_damage_stub(game::mp::gentity_s* self, const game::mp::gentity_s* inflictor, game::mp::gentity_s* attacker, int damage, int dflags,
			const int means_of_death, const unsigned int weapon, const bool is_alternate, const float* v_point, const float* v_dir, const unsigned int hit_loc, const int time_offset)
		{
			{
				const std::string _hit_loc = reinterpret_cast<const char**>(0x1409E62B0)[hit_loc];
				const auto _mod = convert_mod(means_of_death);

				const auto _weapon = get_weapon_name(weapon, is_alternate);

				for (const auto& callback : player_damage_callbacks)
				{
					const auto state = callback.lua_state();

					const auto _self = convert_entity(state, self);
					const auto _inflictor = convert_entity(state, inflictor);
					const auto _attacker = convert_entity(state, attacker);

					const auto _v_point = convert_vector(state, v_point);
					const auto _v_dir = convert_vector(state, v_dir);

					const auto result = callback(_self, _inflictor, _attacker, damage, dflags, _mod, _weapon, _v_point, _v_dir, _hit_loc);

					scripting::lua::handle_error(result);

					if (result.valid() && result.get_type() == sol::type::number)
					{
						damage = result.get<int>();
					}
				}

				if (damage == 0)
				{
					return;
				}
			}

			scr_player_damage_hook.invoke<void>(self, inflictor, attacker, damage, dflags, means_of_death, weapon, is_alternate, v_point, v_dir, hit_loc, time_offset);
		}

		void client_command_stub(const int client_num)
		{
			if (game::mp::g_entities[client_num].client == nullptr)
			{
				return;
			}

			command::params_sv params;

			if (params[0] == "say"s || params[0] == "say_team"s)
			{
				std::string message(game::ConcatArgs(1));

				auto msg_index = 0;
				if (message[msg_index] == '\x1F')
				{
					msg_index = 1;
				}

				auto hidden = false;
				if (message[msg_index] == '/')
				{
					hidden = true;

					if (msg_index == 1)
					{
						// Overwrite / with \x1F only if present
						message[msg_index] = message[msg_index - 1];
					}
					// Skip over the first character
					message.erase(message.begin());
				}

				scheduler::once([params, message, msg_index, client_num]
				{
					const scripting::entity level{*game::levelEntityId};
					const auto player = scripting::call("getentbynum", {client_num}).as<scripting::entity>();
					// Remove \x1F before sending the notify only if present
					const auto notify_msg = msg_index ? message.substr(1) : message;

					notify(level, params[0], {player, notify_msg});
					notify(player, params[0], {notify_msg});

					game_log::g_log_printf("%s;%s;%i;%s;%s\n",
						params[0],
						player.call("getguid").as<const char*>(),
						client_num,
						player.get("name").as<const char*>(),
						message.data()
					);

				}, scheduler::pipeline::server);

				if (hidden)
				{
					return;
				}
			}

			// ClientCommand
			utils::hook::invoke<void>(0x1403929B0, client_num);
		}

		unsigned int local_id_to_entity(unsigned int local_id)
		{
			const auto variable = game::scr_VarGlob->objectVariableValue[local_id];
			return variable.u.f.next;
		}

		bool execute_vm_hook(const char* pos)
		{
			if (!vm_execute_hooks.contains(pos))
			{
				hook_enabled = true;
				return false;
			}

			if (!hook_enabled && pos > reinterpret_cast<char*>(vm_execute_hooks.size()))
			{
				hook_enabled = true;
				return false;
			}

			const auto hook = vm_execute_hooks[pos];
			if (hook.is_lua_hook)
			{
				const auto& function = hook.lua_function;
				const auto state = function.lua_state();

				const scripting::entity self = local_id_to_entity(game::scr_VmPub->function_frame->fs.localId);
				std::vector<sol::lua_value> args;

				const auto top = game::scr_function_stack->top;
				for (auto* value = top; value->type != game::VAR_PRECODEPOS; --value)
				{
					args.push_back(scripting::lua::convert(state, *value));
				}

				const auto result = function(self, sol::as_args(args));
				scripting::lua::handle_error(result);
				target_function = empty_function;
			}
			else
			{
				target_function = hook.target_pos;
			}

			return true;
		}

		void vm_execute_stub(utils::hook::assembler& a)
		{
			const auto replace = a.newLabel();
			const auto end = a.newLabel();

			a.pushad64();

			a.mov(rcx, r14);
			a.call_aligned(execute_vm_hook);

			a.cmp(al, 0);
			a.jne(replace);

			a.popad64();
			a.jmp(end);

			a.bind(end);

			a.movzx(r15d, byte_ptr(r14));
			a.inc(r14);
			a.lea(eax, dword_ptr(r15, -0x17));
			a.mov(dword_ptr(rbp, 0x60), r15d);

			a.jmp(0x14043A593);

			a.bind(replace);

			a.popad64();
			a.mov(rax, qword_ptr(reinterpret_cast<std::int64_t>(&target_function)));
			a.mov(r14, rax);
			a.jmp(end);
		}
	}

	void add_player_damage_callback(const sol::protected_function& callback)
	{
		player_damage_callbacks.push_back(callback);
	}

	void add_player_killed_callback(const sol::protected_function& callback)
	{
		player_killed_callbacks.push_back(callback);
	}

	void clear_callbacks()
	{
		player_damage_callbacks.clear();
		player_killed_callbacks.clear();
		vm_execute_hooks.clear();
	}

	void enable_vm_execute_hook()
	{
		hook_enabled = true;
	}

	void disable_vm_execute_hook()
	{
		hook_enabled = false;
	}

	void set_lua_hook(const char* pos, const sol::protected_function& callback)
	{
		gsc_hook hook;
		hook.is_lua_hook = true;
		hook.lua_function = callback;
		vm_execute_hooks[pos] = hook;
	}

	void set_gsc_hook(const char* source, const char* target)
	{
		gsc_hook hook;
		hook.is_lua_hook = false;
		hook.target_pos = target;
		vm_execute_hooks[source] = hook;
	}

	void clear_hook(const char* pos)
	{
		vm_execute_hooks.erase(pos);
	}

	std::size_t get_hook_count()
	{
		return vm_execute_hooks.size();
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

			utils::hook::call(0x1404724DD, client_command_stub);

			scr_player_damage_hook.create(0x1403CE0C0, scr_player_damage_stub);
			scr_player_killed_hook.create(0x1403CE260, scr_player_killed_stub);

			utils::hook::jump(0x14043A584, utils::hook::assemble(vm_execute_stub), true);

			scripting::on_shutdown([](bool free_scripts)
			{
				if (free_scripts)
				{
					vm_execute_hooks.clear();
				}

				const scripting::entity level{*game::levelEntityId};
				scripting::notify(level, "shutdownGame_called", {free_scripts});
			});
		}
	};
}

REGISTER_COMPONENT(notifies::component)
