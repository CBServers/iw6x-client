#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdSteamAuth final : public i_generic_service<28>
	{
	public:
		void call_service(i_server* server, const std::string& data) override;
	};
}
