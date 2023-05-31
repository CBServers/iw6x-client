#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "steam/steam.hpp"

#include "auth.hpp"
#include "command.hpp"
#include "console.hpp"
#include "network.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/smbios.hpp>
#include <utils/info_string.hpp>
#include <utils/cryptography.hpp>

namespace auth
{
	namespace
	{
		std::string get_hdd_serial()
		{
			DWORD serial{};
			if (!GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr, nullptr, 0))
			{
				return {};
			}

			return utils::string::va("%08X", serial);
		}

		std::string get_hw_profile_guid()
		{
			HW_PROFILE_INFO info;
			if (!GetCurrentHwProfileA(&info))
			{
				return {};
			}

			return std::string{ info.szHwProfileGuid, sizeof(info.szHwProfileGuid) };
		}

		std::string get_protected_data()
		{
			std::string input = "X-Labs-IW6x-Auth";

			DATA_BLOB data_in{}, data_out{};
			data_in.pbData = reinterpret_cast<uint8_t*>(input.data());
			data_in.cbData = static_cast<DWORD>(input.size());
			if (CryptProtectData(&data_in, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_LOCAL_MACHINE, &data_out) != TRUE)
			{
				return {};
			}

			const auto size = std::min(data_out.cbData, 52ul);
			std::string result{ reinterpret_cast<char*>(data_out.pbData), size };
			LocalFree(data_out.pbData);

			return result;
		}

		std::string get_key_entropy()
		{
			std::string entropy{};
			entropy.append(utils::smbios::get_uuid());
			entropy.append(get_hw_profile_guid());
			entropy.append(get_protected_data());
			entropy.append(get_hdd_serial());

			if (entropy.empty())
			{
				entropy.resize(32);
				utils::cryptography::random::get_data(entropy.data(), entropy.size());
			}

			return entropy;
		}

		utils::cryptography::ecc::key& get_key()
		{
			static auto key = utils::cryptography::ecc::generate_key(512, get_key_entropy());
			return key;
		}

		int send_connect_data_stub(game::netsrc_t sock, game::netadr_s* adr, const char* format, const int len)
		{
			std::string connect_string(format, len);
			game::SV_Cmd_TokenizeString(connect_string.data());
			const auto _ = gsl::finally([]()
			{
				game::SV_Cmd_EndTokenizedString();
			});

			const command::params_sv params;
			if (params.size() < 3)
			{
				return false;
			}

			const utils::info_string info_string{std::string{params[2]}};
			const auto challenge = info_string.get("challenge");

			connect_string.clear();
			connect_string.append(params[0]);
			connect_string.append(" ");
			connect_string.append(params[1]);
			connect_string.append(" ");
			connect_string.append("\"" + info_string.build() + "\"");

			proto::network::connect_info info;
			info.set_publickey(get_key().get_public_key());
			info.set_signature(sign_message(get_key(), challenge));
			info.set_infostring(connect_string);

			network::send(*adr, "connect", info.SerializeAsString());
			return true;
		}

		void direct_connect(game::netadr_s* from, game::msg_t* msg)
		{
			const auto offset = sizeof("connect") + 4;

			proto::network::connect_info info;
			if (msg->cursize < offset || !info.ParseFromArray(msg->data + offset, msg->cursize - offset))
			{
				network::send(*from, "error", "Invalid connect data!", '\n');
				return;
			}

			game::SV_Cmd_EndTokenizedString();
			game::SV_Cmd_TokenizeString(info.infostring().data());

			const command::params_sv params;
			if (params.size() < 3)
			{
				network::send(*from, "error", "Invalid connect string!", '\n');
				return;
			}

			const utils::info_string info_string{std::string{params[2]}};
			const auto steam_id = info_string.get("xuid");
			const auto challenge = info_string.get("challenge");

			if (steam_id.empty() || challenge.empty())
			{
				network::send(*from, "error", "Invalid connect data!", '\n');
				return;
			}

			utils::cryptography::ecc::key key;
			key.set(info.publickey());

			const auto xuid = strtoull(steam_id.data(), nullptr, 16);
			if (xuid != key.get_hash())
			{
				network::send(*from, "error", "XUID doesn't match the certificate!", '\n');
				return;
			}

			if (!key.is_valid() || !verify_message(key, challenge, info.signature()))
			{
				network::send(*from, "error", "Challenge signature was invalid!", '\n');
				return;
			}

			game::SV_DirectConnect(from);
		}

		void* get_direct_connect_stub()
		{
			return utils::hook::assemble([](utils::hook::assembler& a)
			{
				a.lea(rcx, qword_ptr(rsp, 0x20));
				a.movaps(xmmword_ptr(rsp, 0x20), xmm0);

				a.pushad64();
				a.mov(rdx, rsi);
				a.call_aligned(direct_connect);
				a.popad64();

				a.jmp(0x140479757);
			});
		}
	}

	uint64_t get_guid()
	{
		if (game::environment::is_dedi())
		{
			return 0x110000100000000 | (::utils::cryptography::random::get_integer() & ~0x80000000);
		}

		return get_key().get_hash();
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Patch steam id bit check
			if (game::environment::is_sp())
			{
				utils::hook::jump(0x1404BF3F0, 0x1404BF446);
				utils::hook::jump(0x1404C02AF, 0x1404C02F0);
				utils::hook::jump(0x1404C07F4, 0x1404C0842);
			}
			else
			{
				utils::hook::jump(0x140585410, 0x140585466);
				utils::hook::jump(0x140142252, 0x140142293);
				utils::hook::jump(0x140142334, 0x140142389);
				utils::hook::jump(0x1405864EF, 0x140586530);
				utils::hook::jump(0x140586A80, 0x140586AC6);

				utils::hook::jump(0x140479636, get_direct_connect_stub(), true);
				utils::hook::call(0x1402C4F8E, send_connect_data_stub);
			}

			command::add("guid", []
			{
				console::info("Your guid: %llX\n", steam::SteamUser()->GetSteamID().bits);
			});
		}
	};
}

REGISTER_COMPONENT(auth::component)
