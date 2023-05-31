#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdAnticheat final : public i_generic_service<38>
	{
	public:
		bdAnticheat();

	private:
		void report_console_details(i_server* server, byte_buffer* buffer) const;
	};
}
