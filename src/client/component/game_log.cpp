#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include "scheduler.hpp"
#include "scripting.hpp"
#include "console.hpp"
#include "game_log.hpp"

#include "gsc/script_extension.hpp"

#include <utils/hook.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

namespace game_log
{
	namespace
	{
		void gscr_log_print()
		{
			char buf[1024]{};
			std::size_t out_chars = 0;

			for (auto i = 0u; i < game::Scr_GetNumParam(); ++i)
			{
				const auto* value = game::Scr_GetString(i);
				const auto len = std::strlen(value);

				out_chars += len;
				if (out_chars >= sizeof(buf))
				{
					break;
				}

				strncat_s(buf, value, _TRUNCATE);
			}

			g_log_printf("%s", buf);
		}
	}

	void g_log_printf(const char* fmt, ...)
	{
		const auto* log = dvars::g_log->current.string;
		if (*log == '\0')
		{
			return;
		}

		char buffer[0x400]{};

		va_list ap;
		va_start(ap, fmt);

		vsprintf_s(buffer, fmt, ap);

		va_end(ap);

		const auto time = *game::level_time / 1000;
		utils::io::write_file(log, utils::string::va("%3i:%i%i %s",
			time / 60,
			time % 60 / 10,
			time % 60 % 10,
			buffer
		), true);
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			utils::hook::set<game::BuiltinFunction>(0x1409E8A20, gscr_log_print);

			scheduler::once([]
			{
				dvars::g_log = game::Dvar_RegisterString("g_log", "logs/games_mp.log", game::DVAR_FLAG_NONE, "Log file name");
			}, scheduler::pipeline::main);

			scripting::on_init([]
			{
				console::info("------- Game Initialization -------\n");
				console::info("gamename: IW6\n");
				console::info("gamedate: " __DATE__ "\n");

				const auto* log = dvars::g_log->current.string;
				if (*log == '\0')
				{
					console::info("Not logging to disk.\n");
					return;
				}

				console::info("Logging to disk: '%s'.\n", log);
				g_log_printf("------------------------------------------------------------\n");
				g_log_printf("InitGame\n");
			});

			scripting::on_shutdown([](int free_scripts)
			{
				console::info("==== ShutdownGame (%d) ====\n", free_scripts);

				g_log_printf("ShutdownGame:\n");
				g_log_printf("------------------------------------------------------------\n");
			});
		}
	};
}

REGISTER_COMPONENT(game_log::component)
