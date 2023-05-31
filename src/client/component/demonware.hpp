#pragma once

namespace demonware
{
	void send_datagram_packet(SOCKET s, const std::string& data, const sockaddr* to, int tolen);

	uint8_t* get_key(const bool encrypt);
	void set_key(bool encrypt, uint8_t* key);
}
