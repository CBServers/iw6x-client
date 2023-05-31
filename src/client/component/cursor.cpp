#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "scheduler.hpp"
#include "game/game.hpp"

#include <utils/hook.hpp>

namespace cursor
{
	namespace
	{
		bool show_cursor_next_frame = false;

		int WINAPI show_cursor_stub(const BOOL show)
		{
			static auto counter = 0;
			counter += show ? 1 : -1;
			return counter;
		}

		void show_cursor(const bool show)
		{
			static auto last_state = false;
			if (last_state == show)
			{
				return;
			}

			if (show)
			{
				while (ShowCursor(TRUE) < 0);
			}
			else
			{
				while (ShowCursor(FALSE) >= 0);
			}
		}

		void draw_cursor_stub()
		{
			show_cursor_next_frame = true;
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// This is the legacy cursor
			// TODO: Find the new cursor drawing...
			utils::hook::call(SELECT_VALUE(0x14040117E, 0x1404BB119), draw_cursor_stub);

			scheduler::loop([]()
			{
				show_cursor(show_cursor_next_frame);
				show_cursor_next_frame = false;
			}, scheduler::pipeline::renderer);

			show_cursor(true);
		}

		void* load_import(const std::string& library, const std::string& function) override
		{
			if (function == "ShowCursor")
			{
				return &show_cursor_stub;
			}

			return nullptr;
		}
	};
}

//REGISTER_COMPONENT(cursor::component)
