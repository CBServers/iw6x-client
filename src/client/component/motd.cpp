#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "motd.hpp"
#include "images.hpp"

#include <utils/hook.hpp>
#include <utils/http.hpp>
#include <utils/io.hpp>

#include <resource.hpp>

namespace motd
{
	namespace
	{
		std::string motd_resource = utils::nt::load_resource(DW_MOTD);
		std::future<std::optional<std::string>> motd_future;
	}

	std::string get_text()
	{
		try
		{
			return motd_future.get().value_or(motd_resource);
		}
		catch (std::exception&)
		{
		}

		return motd_resource;
	}

	class component final : public component_interface
	{
	public:
		void post_load() override
		{
			motd_future = utils::http::get_data_async("https://xlabs.dev/iw6/motd.txt");
			std::thread([]()
			{
				auto data = utils::http::get_data("https://xlabs.dev/iw6/motd.png");
				if (data)
				{
					images::override_texture("iotd_image", data.value());
				}
			}).detach();
		}
	};
}

REGISTER_COMPONENT(motd::component)