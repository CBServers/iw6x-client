#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game_module.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>

namespace musical_talent
{
	namespace
	{
		bool is_main_menu_music(game::StreamedSound* sound)
		{
			return sound->filename.fileIndex == 1 && sound->filename.info.packed.offset == 0xBBAD000;
		}

		void open_sound_handle_stub(HANDLE* handle, game::StreamedSound* sound)
		{
			if (is_main_menu_music(sound))
			{
				const auto folder = game_module::get_host_module().get_folder();
				const auto file = folder + "/data/sound/patch-3-music.flac";
				*handle = CreateFileA(file.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
				                      FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED, nullptr);
			}
			else
			{
				return reinterpret_cast<void(*)(HANDLE*, game::StreamedSound*)>(0x1404F8B90)(handle, sound);
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			utils::hook::call(0x1404903BD, open_sound_handle_stub);
		}
	};
}

REGISTER_COMPONENT(musical_talent::component)
