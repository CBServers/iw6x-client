#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include "command.hpp"

#include "localized_strings.hpp"
#include "console.hpp"
#include "game_module.hpp"

#include "game/ui_scripting/execution.hpp"

#include "ui_scripting.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>

#include <lua.h>

namespace ui_scripting
{
	namespace
	{
		std::unordered_map<game::hks::cclosure*, std::function<arguments(const function_arguments& args)>> converted_functions;

		utils::hook::detour hks_start_hook;
		utils::hook::detour hks_shutdown_hook;
		utils::hook::detour hks_package_require_hook;

		struct globals_t
		{
			std::string in_require_script;
			std::unordered_map<std::string, std::string> loaded_scripts;
			bool load_raw_script{};
			std::string raw_script_name{};
		};

		globals_t globals;

		bool is_loaded_script(const std::string& name)
		{
			return globals.loaded_scripts.contains(name);
		}

		std::string get_root_script(const std::string& name)
		{
			const auto itr = globals.loaded_scripts.find(name);
			return (itr == globals.loaded_scripts.end()) ? std::string() : itr->second;
		}

		table get_globals()
		{
			const auto state = *game::hks::lua_state;
			return state->globals.v.table;
		}

		void print_error(const std::string& error)
		{
			console::error("************** LUI script execution error **************\n");
			console::error("%s\n", error.data());
			console::error("********************************************************\n");
		}

		void print_loading_script(const std::string& name)
		{
			console::info("Loading LUI script '%s'\n", name.data());
		}

		std::string get_current_script()
		{
			const auto state = *game::hks::lua_state;
			game::hks::lua_Debug info{};
			game::hks::hksi_lua_getstack(state, 1, &info);
			game::hks::hksi_lua_getinfo(state, "nSl", &info);
			return info.short_src;
		}

		int load_buffer(const std::string& name, const std::string& data)
		{
			const auto state = *game::hks::lua_state;
			const auto sharing_mode = state->m_global->m_bytecodeSharingMode;
			state->m_global->m_bytecodeSharingMode = game::hks::HKS_BYTECODE_SHARING_ON;

			const auto _0 = gsl::finally([&]
			{
				state->m_global->m_bytecodeSharingMode = sharing_mode;
			});

			game::hks::HksCompilerSettings compiler_settings{};
			return game::hks::hksi_hksL_loadbuffer(state, &compiler_settings, data.data(), data.size(), name.data());
		}

		void load_script(const std::string& name, const std::string& data)
		{
			globals.loaded_scripts[name] = name;

			const auto lua = get_globals();
			const auto load_results = lua["loadstring"](data, name);

			if (load_results[0].is<function>())
			{
				const auto results = lua["pcall"](load_results);
				if (!results[0].as<bool>())
				{
					print_error(results[1].as<std::string>());
				}
			}
			else if (load_results[1].is<std::string>())
			{
				print_error(load_results[1].as<std::string>());
			}
		}

		void load_scripts(const std::string& script_dir)
		{
			if (!utils::io::directory_exists(script_dir))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir);

			for (const auto& script : scripts)
			{
				std::string data;
				if (std::filesystem::is_directory(script) && utils::io::read_file(script + "/__init__.lua", &data))
				{
					print_loading_script(script);
					load_script(script + "/__init__.lua", data);
				}
			}
		}

		void setup_functions()
		{
			const auto lua = get_globals();

			using game = table;
			auto game_type = game();
			lua["game"] = game_type;

			game_type["issingleplayer"] = [](const game&)
			{
				return ::game::environment::is_sp();
			};

			game_type["ismultiplayer"] = [](const game&)
			{
				return ::game::environment::is_mp();
			};

			game_type["addlocalizedstring"] = [](const game&, const std::string& string,
				const std::string& value)
			{
				localized_strings::override(string, value);
			};
		}

		void enable_globals()
		{
			const auto lua = get_globals();
			const std::string code =
				"local g = getmetatable(_G)\n"
				"if not g then\n"
					"g = {}\n"
					"setmetatable(_G, g)\n"
				"end\n"
				"g.__newindex = nil\n";

			(void)lua["loadstring"](code)[0]();
		}

		void start()
		{
			globals = {};

			const auto lua = get_globals();
			enable_globals();

			setup_functions();

			lua["print"] = function(reinterpret_cast<game::hks::lua_function>(0x14017B120)); // hks::base_print
			lua["table"]["unpack"] = lua["unpack"];
			lua["luiglobals"] = lua;

			load_scripts(game_module::get_host_module().get_folder() + "/data/ui_scripts/");
			load_scripts("iw6x/ui_scripts/");
			load_scripts("data/ui_scripts/");
		}

		void try_start()
		{
			try
			{
				start();
			}
			catch (const std::exception& ex)
			{
				console::error("Failed to load LUI scripts: %s\n", ex.what());
			}
		}

		void* hks_start_stub(char a1)
		{
			const auto _0 = gsl::finally(&try_start);
			return hks_start_hook.invoke<void*>(a1);
		}

		void hks_shutdown_stub()
		{
			converted_functions.clear();
			globals = {};
			return hks_shutdown_hook.invoke<void>();
		}

		void* hks_package_require_stub(game::hks::lua_State* state)
		{
			const auto script = get_current_script();
			const auto root = get_root_script(script);
			globals.in_require_script = root;
			return hks_package_require_hook.invoke<void*>(state);
		}

		game::XAssetHeader db_find_x_asset_header_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			game::XAssetHeader header {.luaFile = nullptr};

			if (!is_loaded_script(globals.in_require_script))
			{
				return game::DB_FindXAssetHeader(type, name, allow_create_default);
			}

			const auto folder = globals.in_require_script.substr(0, globals.in_require_script.find_last_of("/\\"));
			const std::string name_ = name;
			const std::string target_script = folder + "/" + name_ + ".lua";

			if (utils::io::file_exists(target_script))
			{
				globals.load_raw_script = true;
				globals.raw_script_name = target_script;
				header.luaFile = reinterpret_cast<game::LuaFile*>(1);
			}
			else if (name_.starts_with("ui/LUI/"))
			{
				return game::DB_FindXAssetHeader(type, name, allow_create_default);
			}

			return header;
		}

		int hks_load_stub(game::hks::lua_State* state, void* compiler_options, void* reader, void* reader_data, const char* chunk_name)
		{
			if (globals.load_raw_script)
			{
				globals.load_raw_script = false;
				globals.loaded_scripts[globals.raw_script_name] = globals.in_require_script;
				return load_buffer(globals.raw_script_name, utils::io::read_file(globals.raw_script_name));
			}

			return utils::hook::invoke<int>(0x140198B00, state, compiler_options, reader, reader_data, chunk_name);
		}

		lua_CFunction lua_atpanic_stub(lua_State* l, [[maybe_unused]] lua_CFunction panicf)
		{
			return utils::hook::invoke<lua_CFunction>(SELECT_VALUE(0x1401851F0, 0x1401A4730), l, SELECT_VALUE(0x1401789A0, 0x140197BC0));
		}

		int luaopen_stub([[maybe_unused]] lua_State* l)
		{
			return 0;
		}

		int hks_base_stub([[maybe_unused]] lua_State* l)
		{
			return 0;
		}
	}

	int main_handler(game::hks::lua_State* state)
	{
		const auto value = state->m_apistack.base[-1];
		if (value.t != game::hks::TCFUNCTION)
		{
			return 0;
		}

		const auto closure = value.v.cClosure;
		if (!converted_functions.contains(closure))
		{
			return 0;
		}

		const auto& function = converted_functions[closure];

		try
		{
			const auto args = get_return_values();
			const auto results = function(args);

			for (const auto& result : results)
			{
				push_value(result);
			}

			return static_cast<int>(results.size());
		}
		catch (const std::exception& ex)
		{
			game::hks::hksi_luaL_error(state, ex.what());
		}

		return 0;
	}

	template <typename F>
	game::hks::cclosure* convert_function(F f)
	{
		const auto state = *game::hks::lua_state;
		const auto closure = game::hks::cclosure_Create(state, main_handler, 0, 0, 0);
		converted_functions[closure] = wrap_function(f);
		return closure;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Disable debug breakpoints in the assembly of hksDefaultPanic
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x140178C5E, 0x140197E7E), 0x90);

			// Restore hksDefaultPanic for complete error messages
			utils::hook::call(SELECT_VALUE(0x1401B0028, 0x1401CE6C8), lua_atpanic_stub);

			// Do not allow the HKS vm to open LUA's libraries
			utils::hook::jump(SELECT_VALUE(0x14015BD10, 0x14017E040), luaopen_stub); // io
			utils::hook::jump(SELECT_VALUE(0x14015C3D0, 0x14017E700), luaopen_stub); // os
			utils::hook::jump(SELECT_VALUE(0x14015D400, 0x14017F730), luaopen_stub); // serialize
			utils::hook::jump(SELECT_VALUE(0x14015D3D0, 0x14017F700), luaopen_stub); // havokscript
			utils::hook::jump(SELECT_VALUE(0x14015C930, 0x14017EC60), luaopen_stub); // debug

			utils::hook::nop(SELECT_VALUE(0x14015B639, 0x14017D969), 5); // coroutine

			// Disable unsafe functions
			utils::hook::jump(SELECT_VALUE(0x140158C20, 0x14017AF50), hks_base_stub); // base_loadfile
			utils::hook::jump(SELECT_VALUE(0x140158B10, 0x14017AE40), hks_base_stub); // base_dofile
			utils::hook::jump(SELECT_VALUE(0x14015D7E0, 0x14017FB10), hks_base_stub); // base_load

			utils::hook::jump(SELECT_VALUE(0x1401569D0, 0x140178CF0), hks_base_stub); // package_loadlib
			utils::hook::jump(SELECT_VALUE(0x140163CD0, 0x140177E00), hks_base_stub); // package_c_loader
			utils::hook::jump(SELECT_VALUE(0x140163DE0, 0x140177F10), hks_base_stub); // package_all_in_one_loader

			utils::hook::jump(SELECT_VALUE(0x140157DA0, 0x14017A0D0), hks_base_stub); // string_dump
			utils::hook::jump(SELECT_VALUE(0x1401606A0, 0x1401747D0), hks_base_stub); // os_execute

			if (!game::environment::is_mp())
			{
				return;
			}

			utils::hook::call(0x1401C7F57, db_find_x_asset_header_stub);
			utils::hook::call(0x1401C7F24, hks_load_stub);

			hks_package_require_hook.create(0x140178020, hks_package_require_stub);
			hks_start_hook.create(0x1401D8E90, hks_start_stub);
			hks_shutdown_hook.create(0x1401D24A0, hks_shutdown_stub);

			command::add("lui_restart", []
			{
				utils::hook::invoke<void>(0x1401D24A0);
				utils::hook::invoke<void>(0x1401D9FB0);
			});
		}
	};
}

REGISTER_COMPONENT(ui_scripting::component)
