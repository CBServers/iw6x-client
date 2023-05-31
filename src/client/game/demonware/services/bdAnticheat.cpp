#include <std_include.hpp>
#include "bdAnticheat.hpp"
#include "../data_types.hpp"

namespace demonware
{
	bdAnticheat::bdAnticheat()
	{
		this->register_service(4, &bdAnticheat::report_console_details);
	}

	void bdAnticheat::report_console_details(i_server* server, [[maybe_unused]] byte_buffer* buffer) const
	{
		// TODO: Read data as soon as needed
		auto reply = server->create_reply(this->get_sub_type());
		reply->send();
	}
}
