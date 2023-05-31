#include <std_include.hpp>
#include "bdDML.hpp"
#include "../data_types.hpp"

namespace demonware
{
	bdDML::bdDML()
	{
		this->register_service(2, &bdDML::get_user_raw_data);
	}

	void bdDML::get_user_raw_data(i_server* server, byte_buffer* /*buffer*/) const
	{
		auto result = new bdDMLRawData;
		result->country_code = "US";
		result->country_code = "'Murica";
		result->region = "New York";
		result->city = "New York";
		result->latitude = 0;
		result->longitude = 0;

		result->asn = 0x2119;
		result->timezone = "+01:00";

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(result);
		reply->send();
	}
}
