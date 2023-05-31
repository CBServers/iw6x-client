#include <std_include.hpp>
#include "bdTitleUtilities.hpp"
#include "../data_types.hpp"

namespace demonware
{
	bdTitleUtilities::bdTitleUtilities()
	{
		this->register_service(6, &bdTitleUtilities::get_server_time);
	}

	void bdTitleUtilities::get_server_time(i_server* server, byte_buffer* /*buffer*/) const
	{
		const auto time_result = new bdTimeStamp;
		time_result->unix_time = uint32_t(time(nullptr));

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(time_result);
		reply->send();
	}
}
