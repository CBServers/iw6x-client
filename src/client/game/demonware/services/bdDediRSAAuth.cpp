#include <std_include.hpp>
#include "bdDediRSAAuth.hpp"

#include "game/game.hpp"
#include "steam/steam.hpp"

#include <utils/cryptography.hpp>

namespace demonware
{
	void bdDediRSAAuth::call_service(i_server* server, const std::string& data)
	{
		bit_buffer buffer(data);

		bool more_data;
		buffer.set_use_data_types(false);
		buffer.read_bool(&more_data);
		buffer.set_use_data_types(true);

		uint32_t seed, title_id, ticket_size;
		buffer.read_uint32(&seed);
		buffer.read_uint32(&title_id);

		unsigned char rsa_key[140];
		buffer.read_bytes(sizeof(rsa_key), rsa_key);

		uint8_t ticket[1024];
		buffer.read_bytes(std::min(ticket_size, static_cast<uint32_t>(sizeof(ticket))), ticket);

		game::bdAuthTicket auth_ticket{};
		std::memset(&auth_ticket, 0xA, sizeof auth_ticket);

		auth_ticket.m_magicNumber = 0x0EFBDADDE;
		auth_ticket.m_type = 0;
		auth_ticket.m_titleID = title_id;
		auth_ticket.m_userID = steam::SteamUser()->GetSteamID().bits;

		auto key = utils::cryptography::tiger::compute(SERVER_CD_KEY);

		strcpy_s(auth_ticket.m_username, "IW6x Server");
		std::memcpy(auth_ticket.m_sessionKey, key.data(), 24);
		auth_ticket.m_timeIssued = static_cast<uint32_t>(time(nullptr));

		uint8_t lsg_ticket[128];
		ZeroMemory(&lsg_ticket, sizeof lsg_ticket);
		std::memcpy(lsg_ticket, key.data(), 24);

		const auto iv = utils::cryptography::tiger::compute(std::string(reinterpret_cast<char*>(&seed), 4));

		const std::string enc_key(reinterpret_cast<char*>(&ticket[32]), 24);
		auto enc_ticket = utils::cryptography::des3::encrypt(
			std::string(reinterpret_cast<char*>(&auth_ticket), sizeof(auth_ticket)), iv, enc_key);

		register_hash(&sha1_desc);
		register_prng(&yarrow_desc);

		auto encrypted_key = utils::cryptography::rsa::encrypt(std::string(SERVER_CD_KEY, 24),
		                                                       std::string("DW-RSAENC", 10),
		                                                       std::string(PCHAR(rsa_key), sizeof(rsa_key)));

		bit_buffer response;
		response.set_use_data_types(false);
		response.write_bool(false);
		response.write_uint32(700);
		response.write_uint32(seed);
		response.write_bytes(enc_ticket.size(), enc_ticket.data());
		response.write_bytes(sizeof(lsg_ticket), lsg_ticket);
		response.write_bytes(encrypted_key.size(), encrypted_key.data());

		auto reply = server->create_message(29);
		reply->send(&response, false);
	}
}
