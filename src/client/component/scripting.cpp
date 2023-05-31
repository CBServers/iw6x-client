#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "game/scripting/entity.hpp"
#include "game/scripting/functions.hpp"
#include "game/scripting/event.hpp"
#include "game/scripting/lua/engine.hpp"
#include "game/scripting/execution.hpp"

#include "scheduler.hpp"
#include "scripting.hpp"

#include "gsc/script_loading.hpp"

#include <utils/hook.hpp>

namespace scripting
{
	std::unordered_map<std::string, std::unordered_map<std::string, const char*>> script_function_table;
	std::unordered_map<std::string, std::vector<std::pair<std::string, const char*>>> script_function_table_sort;
	std::unordered_map<const char*, std::pair<std::string, std::string>> script_function_table_rev;

	std::string current_file;

	namespace
	{
		utils::hook::detour vm_notify_hook;
		utils::hook::detour scr_load_level_hook;
		utils::hook::detour g_shutdown_game_hook;

		utils::hook::detour scr_set_thread_position_hook;
		utils::hook::detour process_script_hook;
		utils::hook::detour sl_get_canonical_string_hook;

		utils::hook::detour g_find_config_string_index;

		std::string current_script_file;
		std::uint32_t current_file_id = 0;

		std::unordered_map<unsigned int, std::string> canonical_string_table;

		std::vector<std::function<void(int)>> shutdown_callbacks;
		std::vector<std::function<void()>> init_callbacks;

		void vm_notify_stub(const unsigned int notify_list_owner_id, const unsigned int string_value, game::VariableValue* top)
		{
			const auto* string = game::SL_ConvertToString(string_value);
			if (string)
			{
				event e;
				e.name = string;
				e.entity = notify_list_owner_id;

				for (auto* value = top; value->type != game::VAR_PRECODEPOS; --value)
				{
					e.arguments.emplace_back(*value);
				}

				if (e.name == "connected")
				{
					clear_entity_fields(e.entity);
				}

				lua::engine::notify(e);
			}

			vm_notify_hook.invoke<void>(notify_list_owner_id, string_value, top);
		}

		void scr_load_level_stub()
		{
			scr_load_level_hook.invoke<void>();

			for (const auto& callback : init_callbacks)
			{
				callback();
			}

			lua::engine::start();
		}

		void g_shutdown_game_stub(const int free_scripts)
		{
			lua::engine::stop();

			if (free_scripts)
			{
				script_function_table_sort.clear();
				script_function_table.clear();
				script_function_table_rev.clear();
				canonical_string_table.clear();
			}

			for (const auto& callback : shutdown_callbacks)
			{
				callback(free_scripts);
			}

			return g_shutdown_game_hook.invoke<void>(free_scripts);
		}

		void process_script_stub(const char* filename)
		{
			current_script_file = filename;

			const auto file_id = std::atoi(filename);
			if (file_id)
			{
				current_file_id = file_id;
			}
			else
			{
				current_file_id = 0;
				current_file = filename;
			}

			process_script_hook.invoke<void>(filename);
		}

		unsigned int sl_get_canonical_string_stub(const char* str)
		{
			const auto result = sl_get_canonical_string_hook.invoke<unsigned int>(str);
			canonical_string_table[result] = str;
			return result;
		}

		void add_function_sort(unsigned int id, const char* pos)
		{
			std::string filename = current_file;
			if (current_file_id)
			{
				filename = get_token(current_file_id);
			}

			if (!script_function_table_sort.contains(filename))
			{
				auto* script = gsc::find_script(game::ASSET_TYPE_SCRIPTFILE, current_script_file.data(), false);
				if (script)
				{
					const auto* end = &script->bytecode[script->bytecodeLen];
					script_function_table_sort[filename].emplace_back("__end__", reinterpret_cast<const char*>(end));
				}
			}

			const auto name = get_token(id);
			auto& itr = script_function_table_sort[filename];
			itr.insert(itr.end() - 1, {name, pos});
		}

		void add_function(const std::string& file, unsigned int id, const char* pos)
		{
			const auto name = get_token(id);
			script_function_table[file][name] = pos;
			script_function_table_rev[pos] = { file, name };
		}

		void scr_set_thread_position_stub(unsigned int thread_name, const char* code_pos)
		{
			add_function_sort(thread_name, code_pos);

			if (current_file_id)
			{
				const auto name = get_token(current_file_id);
				add_function(name, thread_name, code_pos);
			}
			else
			{
				add_function(current_file, thread_name, code_pos);
			}

			scr_set_thread_position_hook.invoke<void>(thread_name, code_pos);
		}
		
		int has_config_string_index(const unsigned int csIndex)
		{
			const auto* s_constantConfigStringTypes = reinterpret_cast<uint8_t*>(0x141721F80);
			return csIndex < 0xDC4 && s_constantConfigStringTypes[csIndex] < 0x18u;
		}

		int is_pre_main_stub()
		{
			return game::SV_Loaded();
		}

		unsigned int g_find_config_string_index_stub(const char* name, const int start, const unsigned int max, const int create, const char* errormsg)
		{
			const auto* sv_running = game::Dvar_FindVar("sv_running");
			return g_find_config_string_index.invoke<unsigned int>(name, start, max, sv_running->current.enabled, errormsg);
		}
	}

	std::string get_token(unsigned int id)
	{
		if (const auto itr = canonical_string_table.find(id); itr != canonical_string_table.end())
		{
			return itr->second;
		}

		return find_token(id);
	}

	void on_shutdown(const std::function<void(int)>& callback)
	{
		shutdown_callbacks.push_back(callback);
	}

	void on_init(const std::function<void()>& callback)
	{
		init_callbacks.push_back(callback);
	}

	std::optional<std::string> get_canonical_string(const unsigned int id)
	{
		if (const auto itr = canonical_string_table.find(id); itr != canonical_string_table.end())
		{
			return {itr->second};
		}

		return {};
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			vm_notify_hook.create(SELECT_VALUE(0x1403E29C0, 0x14043D9B0), &vm_notify_stub);
			// SP address is wrong, but should be ok
			scr_load_level_hook.create(SELECT_VALUE(0x14013D5D0, 0x1403C4E60), &scr_load_level_stub);
			g_shutdown_game_hook.create(SELECT_VALUE(0x140318C10, 0x1403A0DF0), &g_shutdown_game_stub);

			scr_set_thread_position_hook.create(SELECT_VALUE(0x1403D3560, 0x14042E360), &scr_set_thread_position_stub);
			process_script_hook.create(SELECT_VALUE(0x1403DC870, 0x1404378C0), &process_script_stub);
			sl_get_canonical_string_hook.create(game::SL_GetCanonicalString, &sl_get_canonical_string_stub);

			scheduler::loop([]
			{
				lua::engine::run_frame();
			}, scheduler::pipeline::server);

			if (!game::environment::is_sp())
			{
				// Make some room for pre_main hook
				utils::hook::jump(0x1402084A0, has_config_string_index);

				// Allow precaching anytime
				utils::hook::jump(0x1402084A5, is_pre_main_stub);
				utils::hook::set<uint16_t>(0x1402084D0, 0xD3EB); // jump to 0x1402084A5
				g_find_config_string_index.create(0x140161F90, &g_find_config_string_index_stub);
			}
		}
	};
}

REGISTER_COMPONENT(scripting::component)
