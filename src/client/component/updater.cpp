#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "updater.hpp"
#include "filesystem.hpp"

#include <utils/flags.hpp>
#include <updater/_updater.hpp>



namespace updater
{
	void update()
	{
		if (utils::flags::has_flag("noupdate"))
		{
			return;
		}

		try
		{
			const auto path = std::filesystem::path(filesystem::get_binary_directory());
			run(path);
		}
		catch (update_cancelled&)
		{
			TerminateProcess(GetCurrentProcess(), 0);
		}
		catch (...)
		{
		}
	}

	class component final : public component_interface
	{
	public:

		void pre_destroy() override
		{
			join();
		}

		void post_unpack() override
		{
			join();
		}

	private:
		std::thread update_thread_{};

		void join()
		{
			if (this->update_thread_.joinable())
			{
				this->update_thread_.join();
			}
		}
	};
}

REGISTER_COMPONENT(updater::component)