#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "localized_strings.hpp"
#include "scheduler.hpp"
#include "command.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#include "version.hpp"

namespace branding
{
	namespace
	{
		utils::hook::detour ui_get_formatted_build_number_hook;

		void dvar_set_string_stub(game::dvar_t* dvar, const char* string)
		{
			game::Dvar_SetString(dvar, utils::string::va("IW6x %s (game %s)", VERSION, string));
		}

		const char* ui_get_formatted_build_number_stub()
		{
			const auto* const build_num = ui_get_formatted_build_number_hook.invoke<const char*>();

			return utils::string::va("%s (%s)", VERSION, build_num);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi() || game::environment::is_linker())
			{
				return;
			}

			if (game::environment::is_mp())
			{
				command::add("\x6A\x61\x73\x6D\x69\x6E", [&]()
				{
					if (!game::SV_Loaded())
					{
						return;
					}

					const auto* text = "\x4A\x20\x26\x20\x4D\x20\x3C\x33";
					const auto index = game::G_FindConfigstringIndex(text, 0x21D, 0x258, 1, "");

					auto* game_hudelem = game::HudElem_Alloc(0x7FF, 0);
					auto* hudelem = &game_hudelem->elem;

					hudelem->fontScale = 2.0f;
					hudelem->x = 100;
					hudelem->color.rgba = 0xFF000000;
					hudelem->glowColor.rgba = 0xFF0000FF;
					hudelem->text = static_cast<int>(index);
					hudelem->type = game::he_type_t::HE_TYPE_TEXT;
					hudelem->flags |= 1;
				});

				localized_strings::override("LUA_MENU_MULTIPLAYER_CAPS", "IW6x: MULTIPLAYER\n");
			}

			localized_strings::override("LUA_MENU_LEGAL_COPYRIGHT", "IW6x: " VERSION " by X Labs.\n");

			utils::hook::call(SELECT_VALUE(0x1403BDABA, 0x140414424), dvar_set_string_stub);
			ui_get_formatted_build_number_hook.create(
				SELECT_VALUE(0x140415FD0, 0x1404D7C00), ui_get_formatted_build_number_stub);

			scheduler::loop([]()
			{
				const auto x = 3;
				const auto y = 0;
				const auto scale = 0.5f;
				float color[4] = {0.666f, 0.666f, 0.666f, 0.666f};
				const auto* text = "IW6x: " VERSION;

				auto* font = game::R_RegisterFont("fonts/normalfont");
				if (!font) return;

				game::R_AddCmdDrawText(text, 0x7FFFFFFF, font, static_cast<float>(x),
				                       y + static_cast<float>(font->pixelHeight) * scale,
				                       scale, scale, 0.0f, color, 0);
			}, scheduler::pipeline::renderer);
		}
	};
} // namespace branding

REGISTER_COMPONENT(branding::component)
