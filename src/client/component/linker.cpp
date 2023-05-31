#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace linker
{
	namespace
	{
		utils::hook::detour db_mark_asset_hook;

		int db_mark_asset_stub(const game::XAssetType type, const game::XAssetHeader header, const int inuse)
		{
			return db_mark_asset_hook.invoke<int>(type, header, inuse);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_linker())
			{
				return;
			}

			db_mark_asset_hook.create(0x140321C00, db_mark_asset_stub);
		}
	};
}

REGISTER_COMPONENT(linker::component)
