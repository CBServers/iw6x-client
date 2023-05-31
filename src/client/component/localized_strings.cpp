#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "localized_strings.hpp"
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "game/game.hpp"

namespace localized_strings
{
	namespace
	{
		utils::hook::detour seh_string_ed_get_string_hook;

		std::unordered_map<std::string, std::string>& get_localized_overrides()
		{
			static std::unordered_map<std::string, std::string> overrides;
			return overrides;
		}

		std::mutex& get_synchronization_mutex()
		{
			static std::mutex mutex;
			return mutex;
		}

		const char* seh_string_ed_get_string(const char* reference)
		{
			std::lock_guard<std::mutex> _(get_synchronization_mutex());

			auto& overrides = get_localized_overrides();
			const auto entry = overrides.find(reference);
			if (entry != overrides.end())
			{
				return utils::string::va("%s", entry->second.data());
			}

			return seh_string_ed_get_string_hook.invoke<const char*>(reference);
		}

		unsigned int g_localized_string_index_stub(const char* name, /*ConfigString*/ unsigned int start,
		                                           unsigned int max, int create,
		                                           const char* errormsg)
		{
			create = 1;
			return game::G_FindConfigstringIndex(name, start, max, create, errormsg);
		}
	}

	void override(const std::string& key, const std::string& value)
	{
		std::lock_guard<std::mutex> _(get_synchronization_mutex());
		get_localized_overrides()[key] = value;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Change some localized strings
			seh_string_ed_get_string_hook.create(SELECT_VALUE(0x1403F42D0, 0x1404A5F60), &seh_string_ed_get_string);
		}
	};
}

REGISTER_COMPONENT(localized_strings::component)
