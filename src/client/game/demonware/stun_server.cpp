#include <std_include.hpp>
#include <utility>

#include "component/demonware.hpp"
#include "game/demonware/stun_server.hpp"
#include "byte_buffer.hpp"

#include <utils/cryptography.hpp>

namespace demonware
{
	stun_server::stun_server(std::string _name) : name_(std::move(_name))
	{
		this->address_ = utils::cryptography::jenkins_one_at_a_time::compute(this->name_);
	}

	unsigned long stun_server::get_address() const
	{
		return this->address_;
	}

	void stun_server::ip_discovery(SOCKET s, const sockaddr* to, const int tolen) const
	{
		const uint32_t ip = 0x0100007f;

		byte_buffer buffer;
		buffer.set_use_data_types(false);
		buffer.write_byte(31); // type
		buffer.write_byte(2); // version
		buffer.write_byte(0); // version
		buffer.write_uint32(ip); // external ip
		buffer.write_uint16(3074); // port

		send_datagram_packet(s, buffer.get_buffer(), to, tolen);
	}

	void stun_server::nat_discovery(SOCKET s, const sockaddr* to, const int tolen) const
	{
		const uint32_t ip = 0x0100007f;

		byte_buffer buffer;
		buffer.set_use_data_types(false);
		buffer.write_byte(21); // type
		buffer.write_byte(2); // version
		buffer.write_byte(0); // version
		buffer.write_uint32(ip); // external ip
		buffer.write_uint16(3074); // port
		buffer.write_uint32(this->get_address()); // server ip
		buffer.write_uint16(3074); // server port

		send_datagram_packet(s, buffer.get_buffer(), to, tolen);
	}

	int stun_server::send(const SOCKET s, const char* buf, const int len, const sockaddr* to, const int tolen) const
	{
		uint8_t type, version, padding;

		byte_buffer buffer(std::string(buf, len));
		buffer.set_use_data_types(false);
		buffer.read_byte(&type);
		buffer.read_byte(&version);
		buffer.read_byte(&padding);

		switch (type)
		{
		case 30:
			this->ip_discovery(s, to, tolen);
			break;
		case 20:
			this->nat_discovery(s, to, tolen);
			break;
		default:
			break;
		}

		return len;
	}
}
