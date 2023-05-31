#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdMatchMaking final : public i_generic_service<21>
	{
	public:
		bdMatchMaking();

	private:
		void create_session(i_server* server, byte_buffer* buffer) const;
		void update_session(i_server* server, byte_buffer* buffer) const;
		void delete_session(i_server* server, byte_buffer* buffer) const;
		void get_performance(i_server* server, byte_buffer* buffer) const;
		void find_sessions_two_pass(i_server* server, byte_buffer* buffer) const;
	};
}
