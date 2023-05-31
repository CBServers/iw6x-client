#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/compression.hpp>

#include "component/filesystem.hpp"
#include "component/console.hpp"
#include "component/scripting.hpp"
#include "component/fastfiles.hpp"

#include "script_loading.hpp"

namespace gsc
{
	std::unique_ptr<xsk::gsc::iw6_pc::context> gsc_ctx;

	namespace
	{
		std::unordered_map<std::string, unsigned int> main_handles;
		std::unordered_map<std::string, unsigned int> init_handles;

		std::unordered_map<std::string, game::ScriptFile*> loaded_scripts;
		utils::memory::allocator script_allocator;

		const game::dvar_t* developer_script;

		void clear()
		{
			main_handles.clear();
			init_handles.clear();
			loaded_scripts.clear();
			script_allocator.clear();
		}

		bool read_raw_script_file(const std::string& name, std::string* data)
		{
			if (filesystem::read_file(name, data))
			{
				return true;
			}

			// This will prevent 'fake' GSC raw files from being compiled.
			// They are parsed by the game's own parser later as they are special files.
			if (name.starts_with("maps/createfx") || name.starts_with("maps/createart") ||
				(name.starts_with("maps/mp") && name.ends_with("_fx.gsc")))
			{
				return false;
			}

			const auto* name_str = name.data();
			if (game::DB_XAssetExists(game::ASSET_TYPE_RAWFILE, name_str) &&
				!game::DB_IsXAssetDefault(game::ASSET_TYPE_RAWFILE, name_str))
			{
				const auto asset = game::DB_FindXAssetHeader(game::ASSET_TYPE_RAWFILE, name.data(), false);
				const auto len = game::DB_GetRawFileLen(asset.rawfile);
				data->resize(len);
				game::DB_GetRawBuffer(asset.rawfile, data->data(), len);
				if (len > 0)
				{
					data->pop_back();
				}

				return true;
			}

			return false;
		}

		game::ScriptFile* load_custom_script(const char* file_name, const std::string& real_name)
		{
			if (const auto itr = loaded_scripts.find(real_name); itr != loaded_scripts.end())
			{
				return itr->second;
			}

			try
			{
				auto& compiler = gsc_ctx->compiler();
				auto& assembler = gsc_ctx->assembler();

				std::string source_buffer{};
				if (!read_raw_script_file(real_name + ".gsc", &source_buffer))
				{
					return nullptr;
				}

				std::vector<std::uint8_t> data;
				data.assign(source_buffer.begin(), source_buffer.end());

				const auto assembly_ptr = compiler.compile(real_name, data);
				// Pair of two buffers. First is the byte code and second is the stack
				const auto output_script = assembler.assemble(*assembly_ptr);

				const auto script_file_ptr = static_cast<game::ScriptFile*>(script_allocator.allocate(sizeof(game::ScriptFile)));
				script_file_ptr->name = file_name;

				script_file_ptr->len = static_cast<int>(output_script.second.size);
				script_file_ptr->bytecodeLen = static_cast<int>(output_script.first.size);

				const auto stack_size = static_cast<std::uint32_t>(output_script.second.size + 1);
				const auto byte_code_size = static_cast<std::uint32_t>(output_script.first.size + 1);

				script_file_ptr->buffer = static_cast<char*>(script_allocator.allocate(stack_size));
				std::memcpy(const_cast<char*>(script_file_ptr->buffer), output_script.second.data, output_script.second.size);

				script_file_ptr->bytecode = static_cast<std::uint8_t*>(game::PMem_AllocFromSource_NoDebug(byte_code_size, 4, 1, game::PMEM_SOURCE_SCRIPT));
				std::memcpy(script_file_ptr->bytecode, output_script.first.data, output_script.first.size);

				script_file_ptr->compressedLen = 0;

				loaded_scripts[real_name] = script_file_ptr;

				return script_file_ptr;
			}
			catch (const std::exception& ex)
			{
				console::error("*********** script compile error *************\n");
				console::error("failed to compile '%s':\n%s", real_name.data(), ex.what());
				console::error("**********************************************\n");
				return nullptr;
			}
		}

		std::string get_raw_script_file_name(const std::string& name)
		{
			if (name.ends_with(".gsh"))
			{
				return name;
			}

			return name + ".gsc";
		}

		std::string get_script_file_name(const std::string& name)
		{
			const auto id = gsc_ctx->token_id(name);
			if (!id)
			{
				return name;
			}

			return std::to_string(id);
		}

		std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>> read_compiled_script_file(const std::string& name, const std::string& real_name)
		{
			const auto* script_file = game::DB_FindXAssetHeader(game::ASSET_TYPE_SCRIPTFILE, name.data(), false).scriptfile;
			if (!script_file)
			{
				throw std::runtime_error(std::format("Could not load scriptfile '{}'", real_name));
			}

			console::info("Decompiling scriptfile '%s'\n", real_name.data());

			const auto len = script_file->compressedLen;
			const std::string stack{script_file->buffer, static_cast<std::uint32_t>(len)};

			const auto decompressed_stack = utils::compression::zlib::decompress(stack);

			std::vector<std::uint8_t> stack_data;
			stack_data.assign(decompressed_stack.begin(), decompressed_stack.end());

			return {{script_file->bytecode, static_cast<std::uint32_t>(script_file->bytecodeLen)}, stack_data};
		}

		void load_script(const std::string& name)
		{
			if (!game::Scr_LoadScript(name.data()))
			{
				return;
			}

			const auto main_handle = game::Scr_GetFunctionHandle(name.data(), gsc_ctx->token_id("main"));
			const auto init_handle = game::Scr_GetFunctionHandle(name.data(), gsc_ctx->token_id("init"));

			if (main_handle)
			{
				console::info("Loaded '%s::main'\n", name.data());
				main_handles[name] = main_handle;
			}

			if (init_handle)
			{
				console::info("Loaded '%s::init'\n", name.data());
				init_handles[name] = init_handle;
			}
		}

		void load_scripts(const std::filesystem::path& root_dir)
		{
			const std::filesystem::path script_dir = root_dir / "scripts";
			if (!utils::io::directory_exists(script_dir.generic_string()))
			{
				return;
			}

			const auto scripts = utils::io::list_files(script_dir.generic_string());
			for (const auto& script : scripts)
			{
				if (!script.ends_with(".gsc"))
				{
					continue;
				}

				std::filesystem::path path(script);
				const auto relative = path.lexically_relative(root_dir).generic_string();
				const auto base_name = relative.substr(0, relative.size() - 4);

				load_script(base_name);
			}
		}

		int db_is_x_asset_default(game::XAssetType type, const char* name)
		{
			if (loaded_scripts.contains(name))
			{
				return 0;
			}

			return game::DB_IsXAssetDefault(type, name);
		}

		void gscr_load_game_type_script_stub()
		{
			utils::hook::invoke<void>(0x1403CCB10);

			clear();

			fastfiles::enum_assets(game::ASSET_TYPE_RAWFILE, [](const game::XAssetHeader header)
			{
				const std::string name = header.rawfile->name;

				if (name.ends_with(".gsc") && name.starts_with("scripts/"))
				{
					// Remove .gsc from the file name as it will be re-added by the game later
					const auto base_name = name.substr(0, name.size() - 4);
					load_script(base_name);
				}
			}, true);

			for (const auto& path : filesystem::get_search_paths())
			{
				load_scripts(path);
			}
		}

		void db_get_raw_buffer_stub(const game::RawFile* rawfile, char* buf, const int size)
		{
			if (rawfile->len > 0 && rawfile->compressedLen == 0)
			{
				std::memset(buf, 0, size);
				std::memcpy(buf, rawfile->buffer, std::min(rawfile->len, size));
				return;
			}

			game::DB_GetRawBuffer(rawfile, buf, size);
		}

		void g_load_structs_stub()
		{
			for (auto& function_handle : main_handles)
			{
				console::info("Executing '%s::main'\n", function_handle.first.data());
				const auto thread = game::Scr_ExecThread(static_cast<int>(function_handle.second), 0);
				game::RemoveRefToObject(thread);
			}

			utils::hook::invoke<void>(0x1403D2CA0);
		}

		void scr_load_level_stub()
		{
			utils::hook::invoke<void>(0x1403CDC60);

			for (auto& function_handle : init_handles)
			{
				console::info("Executing '%s::init'\n", function_handle.first.data());
				const auto thread = game::Scr_ExecThread(static_cast<int>(function_handle.second), 0);
				game::RemoveRefToObject(thread);
			}
		}

		void scr_begin_load_scripts_stub()
		{
			const auto comp_mode = developer_script->current.enabled ?
				xsk::gsc::build::dev :
				xsk::gsc::build::prod;

			gsc_ctx->init(comp_mode, [](const std::string& include_name) -> std::pair<xsk::gsc::buffer, std::vector<std::uint8_t>>
			{
				const auto real_name = get_raw_script_file_name(include_name);

				std::string file_buffer;
				if (!read_raw_script_file(real_name, &file_buffer) || file_buffer.empty())
				{
					const auto name = get_script_file_name(include_name);
					if (game::DB_XAssetExists(game::ASSET_TYPE_SCRIPTFILE, name.data()))
					{
						return read_compiled_script_file(name, real_name);
					}

					throw std::runtime_error(std::format("Could not load gsc file '{}'", real_name));
				}

				std::vector<std::uint8_t> script_data;
				script_data.assign(file_buffer.begin(), file_buffer.end());

				return {{}, script_data};
			});

			utils::hook::invoke<void>(SELECT_VALUE(0x1403D3860, 0x14042E6B0));
		}

		void scr_end_load_scripts_stub()
		{
			gsc_ctx->cleanup();

			utils::hook::invoke<void>(SELECT_VALUE(0x140603C00, 0x14042E7E0));
		}
	}

	game::ScriptFile* find_script(game::XAssetType type, const char* name, int allow_create_default)
	{
		std::string real_name = name;
		const auto id = static_cast<std::uint16_t>(std::strtol(name, nullptr, 10));
		if (id)
		{
			real_name = gsc_ctx->token_name(id);
		}

		auto* script = load_custom_script(name, real_name);
		if (script)
		{
			return script;
		}

		return game::DB_FindXAssetHeader(type, name, allow_create_default).scriptfile;
	}

	class loading final : public component_interface
	{
	public:
		loading()
		{
			gsc_ctx = std::make_unique<xsk::gsc::iw6_pc::context>();
		}

		void post_unpack() override
		{
			// Load our scripts with an uncompressed stack
			utils::hook::call(SELECT_VALUE(0x1403DC8F0, 0x140437940), db_get_raw_buffer_stub);

			utils::hook::call(SELECT_VALUE(0x14032D1E0, 0x1403CCED9), scr_begin_load_scripts_stub); // GScr_LoadScripts
			utils::hook::call(SELECT_VALUE(0x14032D345, 0x1403CD08D), scr_end_load_scripts_stub); // GScr_LoadScripts

			developer_script = game::Dvar_RegisterBool("developer_script", false, game::DVAR_FLAG_NONE, "Enable developer script comments");

			if (game::environment::is_sp())
			{
				return;
			}

			// ProcessScript
			utils::hook::call(0x1404378D7, find_script);
			utils::hook::call(0x1404378E7, db_is_x_asset_default);

			// GScr_LoadScripts
			utils::hook::call(0x1403CD009, gscr_load_game_type_script_stub);

			// Exec script handles
			utils::hook::call(0x14039F64E, g_load_structs_stub);
			utils::hook::call(0x14039F653, scr_load_level_stub);

			scripting::on_shutdown([](int free_scripts)
			{
				if (free_scripts)
				{
					clear();
				}
			});
		}
	};
}

REGISTER_COMPONENT(gsc::loading)
