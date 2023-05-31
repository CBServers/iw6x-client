#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdDediRSAAuth final : public i_generic_service<26>
	{
	public:
		void call_service(i_server* server, const std::string& data) override;
	};
}
