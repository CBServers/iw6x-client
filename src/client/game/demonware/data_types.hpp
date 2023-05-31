#pragma once
#include "i_server.hpp"
#include "game/structs.hpp"

namespace demonware
{
	class bdFileData final : public i_serializable
	{
	public:
		std::string file_data;

		explicit bdFileData(std::string buffer) : file_data(std::move(buffer))
		{
		}

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_blob(this->file_data);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_blob(&this->file_data);
		}
	};

	class bdFileInfo final : public i_serializable
	{
	public:
		uint64_t file_id;
		uint32_t create_time;
		uint32_t modified_time;
		bool priv;
		uint64_t owner_id;
		std::string filename;
		uint32_t file_size;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_uint32(this->file_size);
			buffer->write_uint64(this->file_id);
			buffer->write_uint32(this->create_time);
			buffer->write_uint32(this->modified_time);
			buffer->write_bool(this->priv);
			buffer->write_uint64(this->owner_id);
			buffer->write_string(this->filename);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_uint32(&this->file_size);
			buffer->read_uint64(&this->file_id);
			buffer->read_uint32(&this->create_time);
			buffer->read_uint32(&this->modified_time);
			buffer->read_bool(&this->priv);
			buffer->read_uint64(&this->owner_id);
			buffer->read_string(&this->filename);
		}
	};

	class bdGroupCount final : public i_serializable
	{
	public:
		uint32_t group_id;
		uint32_t group_count;

		bdGroupCount()
		{
			this->group_id = 0;
			this->group_count = 0;
		}

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_uint32(this->group_id);
			buffer->write_uint32(this->group_count);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_uint32(&this->group_id);
			buffer->read_uint32(&this->group_count);
		}
	};

	class bdTimeStamp final : public i_serializable
	{
	public:
		uint32_t unix_time;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_uint32(this->unix_time);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_uint32(&this->unix_time);
		}
	};

	class bdDMLInfo : public i_serializable
	{
	public:
		std::string country_code; // Char [3]
		std::string country; // Char [65]
		std::string region; // Char [65]
		std::string city; // Char [129]
		float latitude;
		float longitude;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_string(this->country_code);
			buffer->write_string(this->country);
			buffer->write_string(this->region);
			buffer->write_string(this->city);
			buffer->write_float(this->latitude);
			buffer->write_float(this->longitude);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_string(&this->country_code);
			buffer->read_string(&this->country);
			buffer->read_string(&this->region);
			buffer->read_string(&this->city);
			buffer->read_float(&this->latitude);
			buffer->read_float(&this->longitude);
		}
	};

	class bdDMLRawData final : public bdDMLInfo
	{
	public:
		uint32_t asn; // Autonomous System Number.
		std::string timezone;

		void serialize(byte_buffer* buffer) override
		{
			bdDMLInfo::serialize(buffer);

			buffer->write_uint32(this->asn);
			buffer->write_string(this->timezone);
		}

		void deserialize(byte_buffer* buffer) override
		{
			bdDMLInfo::deserialize(buffer);

			buffer->read_uint32(&this->asn);
			buffer->read_string(&this->timezone);
		}
	};

	class bdSessionID final : public i_serializable
	{
	public:
		uint64_t session_id;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_blob(LPSTR(&this->session_id), sizeof this->session_id);
		}

		void deserialize(byte_buffer* buffer) override
		{
			int size{};
			char* data{};
			buffer->read_blob(&data, &size);

			if (data && uint32_t(size) >= sizeof(this->session_id))
			{
				this->session_id = *reinterpret_cast<uint64_t*>(data);
			}
		}
	};

	class bdMatchmakingInfo : public i_serializable
	{
	public:
		bdSessionID session_id{};
		std::string host_addr{};
		uint32_t game_type{};
		uint32_t max_players{};
		uint32_t num_players{};

		bool symmetric = false;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_blob(this->host_addr);
			this->session_id.serialize(buffer);
			buffer->write_uint32(this->game_type);
			buffer->write_uint32(this->max_players);
			buffer->write_uint32(this->num_players);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_blob(&this->host_addr);

			if (this->symmetric) this->session_id.deserialize(buffer);

			buffer->read_uint32(&this->game_type);
			buffer->read_uint32(&this->max_players);

			if (this->symmetric) buffer->read_uint32(&this->num_players);
		}
	};

	class MatchMakingInfo final : public bdMatchmakingInfo
	{
	public:
		int32_t playlist_number{};
		int32_t playlist_version{};
		int32_t netcode_version{};
		int32_t map_packs{};
		int32_t slots_needed_on_team{};
		int32_t skill{};
		uint32_t country_code{};
		uint32_t asn{};
		float latitude{};
		float longitude{};
		int32_t max_reserved_slots{};
		int32_t used_reserved_slots{};
		std::string game_security_key{}; // 16 bytes.
		std::string platform_session_id{}; // 16 bytes.
		uint32_t data_centres{};
		uint32_t coop_state{};

		void serialize(byte_buffer* buffer) override
		{
			bdMatchmakingInfo::serialize(buffer);

			buffer->write_int32(this->playlist_number);
			buffer->write_int32(this->playlist_version);
			buffer->write_int32(this->netcode_version);
			buffer->write_int32(this->map_packs);
			buffer->write_int32(this->slots_needed_on_team);
			buffer->write_int32(this->skill);
			buffer->write_uint32(this->country_code);
			buffer->write_uint32(this->asn);
			buffer->write_float(this->latitude);
			buffer->write_float(this->longitude);
			buffer->write_int32(this->max_reserved_slots);
			buffer->write_int32(this->used_reserved_slots);
			buffer->write_blob(this->game_security_key);
			buffer->write_blob(this->platform_session_id);
			buffer->write_uint32(this->data_centres);
			buffer->write_uint32(this->coop_state);
		}

		void deserialize(byte_buffer* buffer) override
		{
			bdMatchmakingInfo::deserialize(buffer);

			buffer->read_int32(&this->playlist_number);
			buffer->read_int32(&this->playlist_version);
			buffer->read_int32(&this->netcode_version);
			buffer->read_int32(&this->map_packs);
			buffer->read_int32(&this->slots_needed_on_team);
			buffer->read_int32(&this->skill);
			buffer->read_uint32(&this->country_code);
			buffer->read_uint32(&this->asn);
			buffer->read_float(&this->latitude);
			buffer->read_float(&this->longitude);
			buffer->read_int32(&this->max_reserved_slots);
			buffer->read_int32(&this->used_reserved_slots);
			buffer->read_blob(&this->game_security_key);
			buffer->read_blob(&this->platform_session_id);
			buffer->read_uint32(&this->data_centres);
			buffer->read_uint32(&this->coop_state);
		}
	};

	class bdPerformanceValue final : public i_serializable
	{
	public:
		uint64_t user_id;
		int64_t performance;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_uint64(this->user_id);
			buffer->write_int64(this->performance);
		}

		void deserialize(byte_buffer* buffer) override
		{
			buffer->read_uint64(&this->user_id);
			buffer->read_int64(&this->performance);
		}
	};

	struct bdSockAddr final
	{
		bdSockAddr() : in_un(), m_family(AF_INET)
		{
		}

		union
		{
			struct
			{
				char m_b1;
				char m_b2;
				char m_b3;
				char m_b4;
			} m_caddr;

			unsigned int m_iaddr;

			struct
			{
				unsigned __int16 m_w1;
				unsigned __int16 m_w2;
				unsigned __int16 m_w3;
				unsigned __int16 m_w4;
				unsigned __int16 m_w5;
				unsigned __int16 m_w6;
				unsigned __int16 m_w7;
				unsigned __int16 m_w8;
			} m_caddr6;

			char m_iaddr6[16];
			char m_sockaddr_storage[128];
		} in_un;

		unsigned __int16 m_family;
	};

	struct bdInetAddr final : i_serializable
	{
		bdSockAddr m_addr;

		bool is_valid() const
		{
			return (this->m_addr.m_family == AF_INET /*|| this->m_addr.m_family == AF_INET6*/);
		}

		void serialize(byte_buffer* buffer) override
		{
			const auto data_types = buffer->is_using_data_types();
			buffer->set_use_data_types(false);

			if (this->m_addr.m_family == AF_INET)
			{
				buffer->write(4, &this->m_addr.in_un.m_caddr);
			}

			buffer->set_use_data_types(data_types);
		}

		void deserialize(byte_buffer* buffer) override
		{
			const auto data_types = buffer->is_using_data_types();
			buffer->set_use_data_types(false);

			if (this->m_addr.m_family == AF_INET)
			{
				buffer->read(4, &this->m_addr.in_un.m_caddr);
			}

			buffer->set_use_data_types(data_types);
		}
	};

	struct bdAddr final : i_serializable
	{
		bdInetAddr m_address;
		unsigned __int16 m_port{};

		void serialize(byte_buffer* buffer) override
		{
			const bool data_types = buffer->is_using_data_types();
			buffer->set_use_data_types(false);

			this->m_address.serialize(buffer);
			buffer->write_uint16(this->m_port);

			buffer->set_use_data_types(data_types);
		}

		void deserialize(byte_buffer* buffer) override
		{
			const auto data_types = buffer->is_using_data_types();
			buffer->set_use_data_types(false);

			this->m_address.deserialize(buffer);
			buffer->read_uint16(&this->m_port);

			buffer->set_use_data_types(data_types);
		}
	};

	struct bdCommonAddr : i_serializable
	{
		bdAddr m_local_addrs[5];
		bdAddr m_public_addr;
		game::bdNATType m_nat_type;
		unsigned int m_hash;
		bool m_is_loopback;

		void serialize(byte_buffer* buffer) override
		{
			const auto data_types = buffer->is_using_data_types();
			buffer->set_use_data_types(false);

			auto valid = true;
			for (uint32_t i = 0; i < 5 && i < ARRAYSIZE(this->m_local_addrs) && valid; ++i)
			{
				this->m_local_addrs[i].serialize(buffer);
				valid = this->m_local_addrs[i].m_address.is_valid();
			}

			if (valid)
			{
				this->m_public_addr.serialize(buffer);
				buffer->write_byte(this->m_nat_type);
			}

			buffer->set_use_data_types(data_types);
		}

		void deserialize(byte_buffer* buffer) override
		{
			const auto data_types = buffer->is_using_data_types();
			buffer->set_use_data_types(false);

			auto valid = true;
			for (uint32_t i = 0; i < ARRAYSIZE(this->m_local_addrs) && valid; ++i)
			{
				bdAddr addr;
				addr.deserialize(buffer);
				this->m_local_addrs[i] = addr;
				valid = this->m_local_addrs[i].m_address.is_valid();
			}

			if (valid)
			{
				this->m_public_addr.deserialize(buffer);
				buffer->read_byte(reinterpret_cast<uint8_t*>(&this->m_nat_type));
			}

			buffer->set_use_data_types(data_types);
		}
	};
}
