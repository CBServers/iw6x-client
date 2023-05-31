#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdGroup final : public i_generic_service<28>
	{
	public:
		bdGroup();

	private:
		void set_groups(i_server* server, byte_buffer* buffer) const;
		void get_groups(i_server* server, byte_buffer* buffer);

		uint32_t groups[512]{};
	};
}
