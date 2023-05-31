#pragma once

namespace demonware
{
	class stun_server final
	{
	public:
		explicit stun_server(std::string name);

		unsigned long get_address() const;

		int send(SOCKET s, const char* buf, int len, const sockaddr* to, int tolen) const;

	private:
		std::string name_;
		unsigned long address_;

		void ip_discovery(SOCKET s, const sockaddr* to, int tolen) const;
		void nat_discovery(SOCKET s, const sockaddr* to, int tolen) const;
	};
}
