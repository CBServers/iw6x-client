#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdTitleUtilities final : public i_generic_service<12>
	{
	public:
		bdTitleUtilities();

	private:
		void get_server_time(i_server* server, byte_buffer* buffer) const;
	};
}
