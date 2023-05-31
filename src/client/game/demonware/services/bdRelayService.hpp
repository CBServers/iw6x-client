#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdRelayService final : public i_generic_service<86>
	{
	public:
		bdRelayService();

	private:
		void get_credentials(i_server* server, byte_buffer* buffer) const;
		void get_credentials_from_ticket(i_server* server, byte_buffer* buffer) const;
	};
}
