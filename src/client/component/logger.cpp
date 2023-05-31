#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "console.hpp"

#include <utils/hook.hpp>

namespace logger
{
	namespace
	{
		utils::hook::detour com_error_hook;

		const game::dvar_t* logger_dev = nullptr;

		void print_com_error(int, const char* msg, ...)
		{
			char buffer[2048]{};
			va_list ap;

			va_start(ap, msg);
			vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			va_end(ap);

			console::error("%s", buffer);
		}

		void com_error_stub(const int error, const char* msg, ...)
		{
			char buffer[2048]{};
			va_list ap;

			va_start(ap, msg);
			vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			va_end(ap);

			console::error("Error: %s\n", buffer);

			com_error_hook.invoke<void>(error, "%s", buffer);
		}

		void print_warning(const char* msg, ...)
		{
			char buffer[2048]{};
			va_list ap;

			va_start(ap, msg);
			vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			va_end(ap);

			console::warn("%s", buffer);
		}

		void print(const char* msg, ...)
		{
			char buffer[2048]{};
			va_list ap;

			va_start(ap, msg);
			vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			va_end(ap);

			console::info("%s", buffer);
		}

		void print_dev(const char* msg, ...)
		{
			if (!logger_dev->current.enabled)
			{
				return;
			}

			char buffer[2048]{};
			va_list ap;

			va_start(ap, msg);
			vsnprintf_s(buffer, _TRUNCATE, msg, ap);
			va_end(ap);

			console::info("%s", buffer);
		}

		// nullsub_56 -> nullsub_16 (iw6x)
		void nullsub_16()
		{
			utils::hook::call(0x1401CCBD2, print_warning);
			utils::hook::call(0x1401CCBDE, print_warning);
			utils::hook::call(0x1401CCBE6, print_warning);
			utils::hook::call(0x1401CCBF2, print_warning);

			utils::hook::call(0x1401CCE48, print_warning);
			utils::hook::call(0x1401CCE54, print_warning);
			utils::hook::call(0x1401CCE5C, print_warning);
			utils::hook::call(0x1401CCE68, print_warning);

			utils::hook::call(0x1401CD0AD, print_warning);
			utils::hook::call(0x1401CD0B9, print_warning);
			utils::hook::call(0x1401CD0C1, print_warning);
			utils::hook::call(0x1401CD0CD, print_warning);

			utils::hook::call(0x1401CF46D, print_warning);
			utils::hook::call(0x1401CF479, print_warning);
			utils::hook::call(0x1401CF481, print_warning);
			utils::hook::call(0x1401CF48D, print_warning);

			utils::hook::call(0x1401D02B8, print_warning);
			utils::hook::call(0x1401D02D8, print_warning);
			utils::hook::call(0x1401D02E4, print_warning);

			utils::hook::call(0x1401D0388, print_warning);
			utils::hook::call(0x1401D03A8, print_warning);
			utils::hook::call(0x1401D03B4, print_warning);
			utils::hook::call(0x1401D03CD, print_warning);

			utils::hook::call(0x1401D1884, print_warning);
			utils::hook::call(0x1401D1A36, print_warning);
		}

		// sub_1400E7420 -> sub_1401DAA40
		void sub_1401DAA40()
		{
			utils::hook::call(0x1401C9CAE, print_warning);
			utils::hook::call(0x1401CC7F5, print_warning);
			utils::hook::call(0x1401CC97E, print_warning);
			utils::hook::call(0x1401CE43C, print_dev); // lua start up
			utils::hook::call(0x1401CF7E4, print_dev);
			utils::hook::call(0x1401D15BA, print_dev); // lua memory used
			utils::hook::call(0x1401D24BC, print_dev); // lui shutting down
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_mp())
			{
				nullsub_16();
				sub_1401DAA40();
			}

			if (!game::environment::is_sp())
			{
				utils::hook::call(0x140501AE3, print_com_error);
			}

			com_error_hook.create(game::Com_Error, com_error_stub);

			// Make havok script's print function actually print
			utils::hook::jump(SELECT_VALUE(0x1406283A4, 0x140732184), print);

			logger_dev = game::Dvar_RegisterBool("logger_dev", false, game::DVAR_FLAG_SAVED, "Print dev stuff");
		}
	};
}

REGISTER_COMPONENT(logger::component)
