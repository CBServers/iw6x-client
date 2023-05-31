#include <std_include.hpp>
#include "bdGroup.hpp"
#include "../data_types.hpp"

namespace demonware
{
	bdGroup::bdGroup()
	{
		this->register_service(1, &bdGroup::set_groups);
		this->register_service(4, &bdGroup::get_groups);
	}

	void bdGroup::set_groups(i_server* server, byte_buffer* /*buffer*/) const
	{
		//uint32_t groupCount;
		// TODO: Implement array reading

		auto reply = server->create_reply(this->get_sub_type());
		reply->send();
	}

	void bdGroup::get_groups(i_server* server, byte_buffer* buffer)
	{
		uint32_t group_count;
		buffer->read_array_header(8, &group_count);

		auto reply = server->create_reply(this->get_sub_type());

		buffer->set_use_data_types(false);

		for (uint32_t i = 0; i < group_count; ++i)
		{
			auto* count = new bdGroupCount;
			buffer->read_uint32(&count->group_id);

			if (count->group_id < ARRAYSIZE(this->groups))
			{
				this->groups[count->group_id] = 999;
				count->group_count = this->groups[count->group_id];
			}

			reply->add(count);
		}

		buffer->set_use_data_types(true);

		reply->send();
	}
}
