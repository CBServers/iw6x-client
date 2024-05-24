#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "console.hpp"
#include "command.hpp"
#include "game_console.hpp"
#include "rcon.hpp"
#include "scheduler.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/thread.hpp>

namespace console
{
	namespace
	{
		using message_queue = std::queue<std::string>;
		utils::concurrency::container<message_queue> message_queue_;

		std::atomic_bool started_{false};
		std::atomic_bool terminate_runner_{false};

		void print_message(const char* message)
		{
#ifdef _DEBUG
			OutputDebugStringA(message);
#endif

			if (game::is_headless())
			{
				std::fputs(message, stdout);
			}
			else
			{
				game::Conbuf_AppendText(message);
			}
		}

		std::string format(va_list* ap, const char* message)
		{
			static thread_local char buffer[0x1000];

			const auto count = vsnprintf_s(buffer, _TRUNCATE, message, *ap);

			if (count < 0) return {};
			return {buffer, static_cast<size_t>(count)};
		}

		void dispatch_message(const int type, const std::string& message)
		{
			if (rcon::message_redirect(message))
			{
				return;
			}

			game_console::print(type, message);

			if (game::is_headless())
			{
				std::fputs(message.data(), stdout);
				return;
			}

			message_queue_.access([&message](message_queue& queue)
			{
				queue.emplace(message);
			});
		}

		message_queue empty_message_queue()
		{
			message_queue current_queue{};

			message_queue_.access([&](message_queue& queue)
			{
				current_queue = std::move(queue);
				queue = {};
			});

			return current_queue;
		}

		void print_stub(const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);

			char buffer[4096]{};
			const auto res = vsnprintf_s(buffer, _TRUNCATE, fmt, ap);
			(void)res;
			print_message(buffer);

			va_end(ap);
		}

		void append_text(const char* text)
		{
			dispatch_message(con_type_info, text);
		}
	}

	class component final : public component_interface
	{
	public:
		component()
		{
			if (game::is_headless())
			{
				if (!AttachConsole(ATTACH_PARENT_PROCESS))
				{
					AllocConsole();
					AttachConsole(GetCurrentProcessId());
				}

				ShowWindow(GetConsoleWindow(), SW_SHOW);

				FILE* fp;
				freopen_s(&fp, "CONIN$", "r", stdin);
				freopen_s(&fp, "CONOUT$", "w", stdout);
				freopen_s(&fp, "CONOUT$", "w", stderr);
			}
		}

		void post_unpack() override
		{
			// Redirect input (]command)
			utils::hook::jump(SELECT_VALUE(0x14043DFA0, 0x140502A80), append_text);

			utils::hook::jump(printf, print_stub);

			if (game::is_headless())
			{
				return;
			}

			terminate_runner_ = false;

			this->message_runner_ = utils::thread::create_named_thread("Console IO", []
			{
				while (!started_)
				{
					std::this_thread::sleep_for(10ms);
				}

				while (!terminate_runner_)
				{
					std::string message_buffer{};
					auto current_queue = empty_message_queue();

					while (!current_queue.empty())
					{
						const auto& msg = current_queue.front();
						message_buffer.append(msg);
						current_queue.pop();
					}

					if (!message_buffer.empty())
					{
						print_message(message_buffer.data());
					}

					std::this_thread::sleep_for(5ms);
				}
			});

			this->console_runner_ = utils::thread::create_named_thread("Console Window", [this]
			{
				game::Sys_ShowConsole();

				MSG msg{};
				while (!terminate_runner_)
				{
					if (PeekMessageW(&msg, nullptr, NULL, NULL, PM_REMOVE))
					{
						if (msg.message == WM_QUIT)
						{
							command::execute("quit", false);
							break;
						}

						TranslateMessage(&msg);
						DispatchMessageW(&msg);
					}
					else
					{
						std::this_thread::sleep_for(5ms);
					}
				}
			});

			// Give the console a chance to open or we will lose some early messages
			// like the ones printed from the filesystem component
			scheduler::once([]() -> void
			{
				started_ = true;
			}, scheduler::pipeline::main);
		}

		void pre_destroy() override
		{
			terminate_runner_ = true;

			if (this->message_runner_.joinable())
			{
				this->message_runner_.join();
			}

			if (this->console_runner_.joinable())
			{
				this->console_runner_.join();
			}
		}

	private:
		std::thread console_runner_{};
		std::thread message_runner_{};
	};

	HWND get_window()
	{
		return *reinterpret_cast<HWND*>((SELECT_VALUE(0x145A7B490, 0x147AD1DB0)));
	}

	void set_title(std::string title)
	{
		if (game::is_headless())
		{
			SetConsoleTitleA(title.data());
		}
		else
		{
			SetWindowTextA(get_window(), title.data());
		}
	}

	void set_size(const int width, const int height)
	{
		RECT rect;
		GetWindowRect(get_window(), &rect);

		SetWindowPos(get_window(), nullptr, rect.left, rect.top, width, height, 0);

		auto* const logo_window = *reinterpret_cast<HWND*>(SELECT_VALUE(0x145A7B4A0, 0x147AD1DC0));
		SetWindowPos(logo_window, nullptr, 5, 5, width - 25, 60, 0);
	}

	void print(const int type, const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		const auto result = format(&ap, fmt);
		va_end(ap);

		dispatch_message(type, result);
	}
}

REGISTER_COMPONENT(console::component)
