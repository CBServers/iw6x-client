#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include "fastfiles.hpp"
#include "command.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/memory.hpp>
#include <utils/io.hpp>

namespace fastfiles
{
	namespace
	{
		utils::hook::detour db_try_load_x_file_internal_hook;
		utils::hook::detour db_find_x_asset_header_hook;

		void db_try_load_x_file_internal(const char* zoneName, const int zone_flags, const int is_base_map)
		{
			console::info("Loading fastfile %s\n", zoneName);
			return db_try_load_x_file_internal_hook.invoke<void>(zoneName, zone_flags, is_base_map);
		}

		void dump_gsc_script(const std::string& name, game::XAssetHeader header)
		{
			if (!dvars::g_dump_scripts->current.enabled)
			{
				return;
			}

			std::string buffer;
			buffer.append(header.scriptfile->name, std::strlen(header.scriptfile->name) + 1);
			buffer.append(reinterpret_cast<char*>(&header.scriptfile->compressedLen), 4);
			buffer.append(reinterpret_cast<char*>(&header.scriptfile->len), 4);
			buffer.append(reinterpret_cast<char*>(&header.scriptfile->bytecodeLen), 4);
			buffer.append(header.scriptfile->buffer, header.scriptfile->compressedLen);
			buffer.append(reinterpret_cast<char*>(header.scriptfile->bytecode), header.scriptfile->bytecodeLen);

			const auto out_name = std::format("gsc_dump/{}.gscbin", name);
			utils::io::write_file(out_name, buffer);

			console::info("Dumped %s\n", out_name.data());
		}


		game::XAssetHeader db_find_x_asset_header_stub(game::XAssetType type, const char* name, int allow_create_default)
		{
			const auto start = game::Sys_Milliseconds();
			const auto result = db_find_x_asset_header_hook.invoke<game::XAssetHeader>(type, name, allow_create_default);
			const auto diff = game::Sys_Milliseconds() - start;

			if (type == game::ASSET_TYPE_SCRIPTFILE)
			{
				dump_gsc_script(name, result);
			}

			if (diff > 100)
			{
				console::print(
					result.data == nullptr
						? console::con_type_error
						: console::con_type_warning,
					"Waited %i msec for asset '%s' of type '%s'.\n",
					diff,
					name,
					game::g_assetNames[type]
				);
			}

			return result;
		}

		void reallocate_asset_pool(const game::XAssetType type, const unsigned int new_size)
		{
			const size_t element_size = game::DB_GetXAssetTypeSize(type);

			auto* new_pool = utils::memory::get_allocator()->allocate(new_size * element_size);
			std::memmove(new_pool, game::DB_XAssetPool[type], game::g_poolSize[type] * element_size);

			game::DB_XAssetPool[type] = new_pool;
			game::g_poolSize[type] = new_size;
		}
	}

	void enum_assets(const game::XAssetType type, const std::function<void(game::XAssetHeader)>& callback, const bool include_override)
	{
		game::DB_EnumXAssets_Internal(type, static_cast<void(*)(game::XAssetHeader, void*)>([](game::XAssetHeader header, void* data)
		{
			const auto& cb = *static_cast<const std::function<void(game::XAssetHeader)>*>(data);
			cb(header);
		}), &callback, include_override);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			db_try_load_x_file_internal_hook.create(
				SELECT_VALUE(0x140275850, 0x1403237F0), &db_try_load_x_file_internal);

			db_find_x_asset_header_hook.create(game::DB_FindXAssetHeader, db_find_x_asset_header_stub);
			dvars::g_dump_scripts = game::Dvar_RegisterBool("g_dumpScripts", false, game::DVAR_FLAG_NONE, "Dump GSC scripts to binary format");

			command::add("loadzone", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("usage: loadzone <zone>\n");
					return;
				}

				game::XZoneInfo info;
				info.name = params.get(1);
				info.allocFlags = 1;
				info.freeFlags = 0;

				game::DB_LoadXAssets(&info, 1, game::DBSyncMode::DB_LOAD_SYNC);
			});

			command::add("materiallist", [](const command::params& params)
			{
				game::DB_EnumXAssets_FastFile(game::ASSET_TYPE_MATERIAL, [](const game::XAssetHeader header, void*)
				{
					if(header.material && header.material->name)
					{
						console::info("%s\n", header.material->name);
					}
				}, nullptr, false);
			});

			if (!game::environment::is_sp())
			{
				reallocate_asset_pool(game::ASSET_TYPE_WEAPON, 320);
			}
		}
	};
}

REGISTER_COMPONENT(fastfiles::component)
