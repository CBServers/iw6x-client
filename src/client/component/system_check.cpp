#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "system_check.hpp"

#include "game/game.hpp"

#include <utils/io.hpp>
#include <utils/cryptography.hpp>

namespace system_check
{
	namespace
	{
		std::string read_zone(const std::string& name)
		{
			std::string data{};
			if (utils::io::read_file(name, &data))
			{
				return data;
			}

			if (utils::io::read_file("zone/" + name, &data))
			{
				return data;
			}

			return {};
		}

		std::string hash_zone(const std::string& name)
		{
			const auto data = read_zone(name);
			return utils::cryptography::sha256::compute(data, true);
		}

		bool verify_hashes(const std::unordered_map<std::string, std::string>& zone_hashes)
		{
			for (const auto& zone_hash : zone_hashes)
			{
				const auto hash = hash_zone(zone_hash.first);
				if (hash != zone_hash.second)
				{
					return false;
				}
			}

			return true;
		}

		bool is_system_valid()
		{
			static std::unordered_map<std::string, std::string> mp_zone_hashes =
			{
				{"patch_common_mp.ff", "F1F08BFD03D0496199FAFD49CAA3E5786B70084A4C3C8841ACC4A7B7616D226C"},
			};

			static std::unordered_map<std::string, std::string> sp_zone_hashes =
			{
				{"patch_common.ff", "883DB33A1E386420EC6EF19F25C0D8081D01C1945BA2BCAD9FBD5460A201D6AA"},
				{"patch_common_alien_mp.ff", "78B00BFF961F69F9A45446D40638A5A7F5C3462F1AF05A833066772C62FB5DB2"},
			};

			return verify_hashes(mp_zone_hashes) && (game::environment::is_dedi() || verify_hashes(sp_zone_hashes));
		}

		void verify_binary_version()
		{
			const auto value = *reinterpret_cast<DWORD*>(0x140001337);
			if (value != 0xDB0A33E7 && value != 0xA6D147E7)
			{
				throw std::runtime_error("Unsupported Call of Duty: Ghosts version");
			}
		}
	}

	bool is_valid()
	{
		static auto valid = is_system_valid();
		return valid;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			verify_binary_version();

			if (!is_valid())
			{
				MessageBoxA(nullptr, "Your game files are outdated or unsupported.\n"
				            "Please get the latest officially supported Call of Duty: Ghosts files, or you will get random crashes and issues.",
				            "Invalid game files!", MB_ICONINFORMATION);
			}
		}
	};
}

REGISTER_COMPONENT(system_check::component)
