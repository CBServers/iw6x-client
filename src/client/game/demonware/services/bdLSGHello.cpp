#include <std_include.hpp>
#include "bdLSGHello.hpp"
#include "component/demonware.hpp"

namespace demonware
{
	void bdLSGHello::call_service(i_server* server, const std::string& data)
	{
		bit_buffer buffer(data);

		bool more_data;
		buffer.set_use_data_types(false);
		buffer.read_bool(&more_data);
		buffer.set_use_data_types(true);

		uint32_t seed, title_id;
		buffer.read_uint32(&title_id);
		buffer.read_uint32(&seed);

		uint8_t ticket[128];
		buffer.read_bytes(sizeof(ticket), ticket);

		set_key(true, ticket);
		set_key(false, ticket);
	}
}
