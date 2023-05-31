#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdDML final : public i_generic_service<27>
	{
	public:
		bdDML();

	private:
		void get_user_raw_data(i_server* server, byte_buffer* buffer) const;
	};
}
