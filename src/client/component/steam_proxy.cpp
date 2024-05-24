#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "scheduler.hpp"

#include <utils/nt.hpp>
#include <utils/string.hpp>
#include <utils/binary_resource.hpp>

#include "game/game.hpp"

#include "steam/interface.hpp"
#include "steam/steam.hpp"

namespace steam_proxy
{
	namespace
	{
		enum class ownership_state
		{
			success,
			unowned,
			nosteam,
			error,
		};

		ownership_state state_;

		utils::binary_resource runner_file(RUNNER, "runner.exe");
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			if (game::environment::is_dedi())
			{
				return;
			}

			this->load_client();
			this->clean_up_on_error();

#ifndef DEV_BUILD
			try
			{
				const std::string game_type = game::environment::is_sp() ? "singleplayer" : "multiplayer";
				state_ = this->start_mod("\xF0\x9F\x91\xBB" " IW6x: " + (game_type), game::environment::is_sp() ? 209160 : 209170);
			}
			catch (const std::exception& ex)
			{
				state_ = ownership_state::error;
				printf("Steam: %s\n", ex.what());
			}
#endif
			(void)state_;
		}

		void pre_destroy() override
		{
			if (this->steam_client_module_)
			{
				if (this->steam_pipe_)
				{
					if (this->global_user_)
					{
						this->steam_client_module_.invoke<void>("Steam_ReleaseUser", this->steam_pipe_, this->global_user_);
					}

					this->steam_client_module_.invoke<bool>("Steam_BReleaseSteamPipe", this->steam_pipe_);
				}
			}
		}

#ifdef LOAD_STEAM_OVERLAY
		[[nodiscard]] const utils::nt::library& get_overlay_module() const
		{
			return steam_overlay_module_;
		}
#endif

	private:
		utils::nt::library steam_client_module_{};
#ifdef LOAD_STEAM_OVERLAY
		utils::nt::library steam_overlay_module_{};
#endif

		steam::interface client_engine_{};
		steam::interface client_user_{};
		steam::interface client_utils_{};

		void* steam_pipe_ = nullptr;
		void* global_user_ = nullptr;

		[[nodiscard]] void* load_client_engine() const
		{
			if (!this->steam_client_module_) return nullptr;

			for (auto i = 1; i < 1000; ++i)
			{
				const auto* name = utils::string::va("CLIENTENGINE_INTERFACE_VERSION%03i", i);
				auto* const client_engine = this->steam_client_module_.invoke<void*>("CreateInterface", name, nullptr);
				if (client_engine) return client_engine;
			}

			return nullptr;
		}

		void load_client()
		{
			const std::filesystem::path steam_path = steam::SteamAPI_GetSteamInstallPath();
			if (steam_path.empty()) return;

			utils::nt::library::load(steam_path / "tier0_s64.dll");
			utils::nt::library::load(steam_path / "vstdlib_s64.dll");
#ifdef LOAD_STEAM_OVERLAY
			this->steam_overlay_module_ = utils::nt::library::load(steam_path / "gameoverlayrenderer64.dll");
#endif
			this->steam_client_module_ = utils::nt::library::load(steam_path / "steamclient64.dll");
			if (!this->steam_client_module_) return;

			this->client_engine_ = load_client_engine();
			if (!this->client_engine_) return;

			this->steam_pipe_ = this->steam_client_module_.invoke<void*>("Steam_CreateSteamPipe");
			this->global_user_ = this->steam_client_module_.invoke<void*>("Steam_ConnectToGlobalUser", this->steam_pipe_);
			this->client_user_ = this->client_engine_.invoke<void*>(8, this->steam_pipe_, this->global_user_);
			// GetIClientUser
			this->client_utils_ = this->client_engine_.invoke<void*>(14, this->steam_pipe_); // GetIClientUtils
		}

		ownership_state start_mod(const std::string& title, const size_t app_id)
		{
			__try
			{
				return this->start_mod_unsafe(title, app_id);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				this->do_cleanup();
				return ownership_state::error;
			}
		}

		ownership_state start_mod_unsafe(const std::string& title, size_t app_id)
		{
			if (!this->client_utils_ || !this->client_user_)
			{
				return ownership_state::nosteam;
			}

			if (!this->client_user_.invoke<bool>("BIsSubscribedApp", app_id))
			{
				app_id = 480; // Spacewar
			}

			this->client_utils_.invoke<void>("SetAppIDForCurrentPipe", app_id, false);

			char our_directory[MAX_PATH]{};
			GetCurrentDirectoryA(sizeof(our_directory), our_directory);

			const auto path = runner_file.get_extracted_file();
			const auto* cmdline = utils::string::va("\"%s\" -proc %d", path.data(), GetCurrentProcessId());

			steam::game_id game_id;
			game_id.raw.type = 1; // k_EGameIDTypeGameMod
			game_id.raw.app_id = app_id & 0xFFFFFF;

			const auto* mod_id = "IW6x";
			game_id.raw.mod_id = *reinterpret_cast<const unsigned int*>(mod_id) | 0x80000000;

			this->client_user_.invoke<bool>("SpawnProcess", path.data(), cmdline, our_directory,
			                                &game_id.bits, title.data(), 0, 0, 0);

			return ownership_state::success;
		}

		void do_cleanup()
		{
			this->client_engine_ = nullptr;
			this->client_user_ = nullptr;
			this->client_utils_ = nullptr;

			this->steam_pipe_ = nullptr;
			this->global_user_ = nullptr;

			this->steam_client_module_ = utils::nt::library{ nullptr };
		}

		void clean_up_on_error()
		{
			scheduler::schedule([this]()
			{
				if (this->steam_client_module_
					&& this->steam_pipe_
					&& this->global_user_
					&& this->steam_client_module_.invoke<bool>("Steam_BConnected", this->global_user_, this->steam_pipe_)
					&& this->steam_client_module_.invoke<bool>("Steam_BLoggedOn", this->global_user_, this->steam_pipe_)
				)
				{
					return scheduler::cond_continue;
				}

				this->do_cleanup();

				return scheduler::cond_end;
			});
		}
	};
}

REGISTER_COMPONENT(steam_proxy::component)
