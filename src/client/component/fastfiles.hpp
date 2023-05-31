#pragma once

namespace fastfiles
{
	void enum_assets(game::XAssetType type, const std::function<void(game::XAssetHeader)>& callback, bool include_override);
}
