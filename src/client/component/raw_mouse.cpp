#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

#include <hidusage.h>

namespace raw_mouse
{
	namespace
	{
		int mouse_raw_x = 0;
		int mouse_raw_y = 0;
		const game::dvar_t* cl_rawInput = nullptr;

		LRESULT wnd_proc_hook(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			if (msg == WM_INPUT)
			{
				std::uint32_t size = sizeof(RAWINPUT);
				static BYTE lpb[sizeof(RAWINPUT)];

				GetRawInputData(
					reinterpret_cast<HRAWINPUT>(lparam), 
					RID_INPUT, lpb, &size, 
					sizeof(RAWINPUTHEADER)
				);

				auto* raw = reinterpret_cast<RAWINPUT*>(lpb);
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					// Is there's really absolute mouse on earth?
					if (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
					{
						mouse_raw_x = raw->data.mouse.lLastX;
						mouse_raw_y = raw->data.mouse.lLastY;
					}
					else
					{
						mouse_raw_x += raw->data.mouse.lLastX;
						mouse_raw_y += raw->data.mouse.lLastY;
					}
				}

				return TRUE;
			}

			return game::MainWndProc(hwnd, msg, wparam, lparam);
		}

		void in_clamp_mouse_move(tagPOINT &current_position)
		{
			tagRECT rc;
			GetWindowRect(game::g_wv->hWnd, &rc);

			auto is_clamped = false;
			if (current_position.x >= rc.left)
			{
				if (current_position.x >= rc.right)
				{
					current_position.x = rc.right - 1;
					is_clamped = true;
				}
			}
			else
			{
				current_position.x = rc.left;
				is_clamped = true;
			}
			if (current_position.y >= rc.top)
			{
				if (current_position.y >= rc.bottom)
				{
					current_position.y = rc.bottom - 1;
					is_clamped = true;
				}
			}
			else
			{
				current_position.y = rc.top;
				is_clamped = true;
			}

			if (is_clamped)
			{
				SetCursorPos(current_position.x, current_position.y);
			}
		}

		void in_raw_mouse_move()
		{
			static const auto* r_displayMode = game::Dvar_FindVar("r_displayMode");

			if (GetForegroundWindow() == game::g_wv->hWnd)
			{
				static tagPOINT current_position;
				GetCursorPos(&current_position);

				// is fullscreen
				if (r_displayMode->current.integer == 0)
				{
					in_clamp_mouse_move(current_position);
				}

				static auto old_x = 0, old_y = 0;

				const auto need_reset = game::s_wmv->oldPos.x == -100000;

				const auto delta_x = need_reset ? 0 : mouse_raw_x - old_x;
				const auto delta_y = need_reset ? 0 : mouse_raw_y - old_y;

				old_x = mouse_raw_x;
				old_y = mouse_raw_y;

				game::s_wmv->oldPos = current_position;
				ScreenToClient(game::g_wv->hWnd, &current_position);

				current_position.x = current_position.x * game::vidConfigOut->displayWidth / game::vidConfigOut->monitorWidth;
				current_position.y = current_position.y * game::vidConfigOut->displayHeight / game::vidConfigOut->monitorHeight;

				game::g_wv->recenterMouse = game::CL_MouseEvent(current_position.x, current_position.y, delta_x, delta_y);

				if (game::g_wv->recenterMouse && (delta_x || delta_y || need_reset))
				{
					game::IN_RecenterMouse();
				}
			}
		}

		void in_mouse_move()
		{
			if (cl_rawInput->current.enabled)
			{
				in_raw_mouse_move();
			}
			else
			{
				game::IN_MouseMove();
			}
		}

		void in_raw_mouse_init(HWND hwnd)
		{
			RAWINPUTDEVICE rid[1];
			rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
			rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
			rid[0].dwFlags = RIDEV_INPUTSINK;
			rid[0].hwndTarget = hwnd;

			RegisterRawInputDevices(rid, ARRAYSIZE(rid), sizeof(rid[0]));
		}

		HWND CALLBACK create_window_ex_a_stub(DWORD dw_ex_style, LPCSTR lp_class_name, LPCSTR lp_window_name,
			DWORD dw_style, int x, int y, int n_width, int n_height, HWND h_wnd_parent, HMENU h_menu, HINSTANCE h_instance, LPVOID lp_param)
		{
			const auto hwnd = CreateWindowExA(dw_ex_style, lp_class_name, lp_window_name,
				dw_style, x, y, n_width, n_height, h_wnd_parent, h_menu, h_instance, lp_param);

			in_raw_mouse_init(hwnd);

			return hwnd;
		}

		ATOM CALLBACK register_class_ex_a_stub(WNDCLASSEXA* wnd_class)
		{
			wnd_class->lpfnWndProc = wnd_proc_hook;
			return RegisterClassExA(wnd_class);
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

			utils::hook::call(SELECT_VALUE(0x14043845F, 0x1404FC3FB), &in_mouse_move);

			utils::hook::call(SELECT_VALUE(0x140519F71, 0x1405E69A1), &create_window_ex_a_stub);
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x140519F76, 0x1405E69A6), 0x90);

			utils::hook::call(SELECT_VALUE(0x14043BF04, 0x140500824), &register_class_ex_a_stub);
			utils::hook::set<std::uint8_t>(SELECT_VALUE(0x14043BF09, 0x140500829), 0x90);

			cl_rawInput = game::Dvar_RegisterBool("cl_rawInput", true,
				game::DVAR_FLAG_SAVED, "Use Raw Input for mouse input. This fixes mouse acceleration issue");
		}
	};
}

REGISTER_COMPONENT(raw_mouse::component)
