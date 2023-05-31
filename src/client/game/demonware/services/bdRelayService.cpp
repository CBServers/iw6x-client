#include <std_include.hpp>
#include "bdRelayService.hpp"
#include "../data_types.hpp"
#include "steam/steam.hpp"

namespace demonware
{
	class DebugObjectUCD : public i_serializable
	{
	public:
		uint64_t user_id;
		std::string platform;
		std::vector<uint64_t> user_ids;
		std::vector<std::string> user_files;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_uint64(this->user_id);
			buffer->write_string(this->platform);

			buffer->write_array_header(10, INT(this->user_ids.size()), sizeof(uint64_t));
			buffer->set_use_data_types(false);
			for (size_t i = 0; i < this->user_ids.size(); i++)
			{
				buffer->write_uint64(this->user_ids[i]);
			}
			buffer->set_use_data_types(true);

			auto stringSize = this->user_files.size();
			for (size_t i = 0; i < this->user_files.size(); i++)
			{
				stringSize += this->user_files[i].length();
			}

			buffer->write_array_header(16, INT(this->user_files.size()), INT(stringSize / this->user_files.size()));

			buffer->set_use_data_types(false);
			for (size_t i = 0; i < this->user_files.size(); i++)
			{
				buffer->write_string(this->user_files[i]);
			}
			buffer->set_use_data_types(true);
		}

		void deserialize(byte_buffer* /*buffer*/) override
		{
		}
	};

	class DebugObjectUNO : public i_serializable
	{
	public:
		std::vector<uint64_t> user_ids;
		std::vector<std::string> user_files;

		void serialize(byte_buffer* buffer) override
		{
			buffer->write_array_header(10, INT(this->user_ids.size()), sizeof(uint64_t));

			buffer->set_use_data_types(false);
			for (size_t i = 0; i < this->user_ids.size(); i++)
			{
				buffer->write_uint64(this->user_ids[i]);
			}
			buffer->set_use_data_types(true);

			auto stringSize = this->user_files.size();
			for (size_t i = 0; i < this->user_files.size(); i++)
			{
				stringSize += this->user_files[i].length();
			}

			buffer->write_array_header(16, INT(this->user_files.size()), INT(stringSize / this->user_files.size()));

			buffer->set_use_data_types(false);
			for (size_t i = 0; i < this->user_files.size(); i++)
			{
				buffer->write_string(this->user_files[i]);
			}
			buffer->set_use_data_types(true);
		}

		void deserialize(byte_buffer* /*buffer*/) override
		{
		}
	};

	bdRelayService::bdRelayService()
	{
		this->register_service(3, &bdRelayService::get_credentials);
		this->register_service(4, &bdRelayService::get_credentials_from_ticket);
	}

	void bdRelayService::get_credentials(i_server* server, byte_buffer* buffer) const
	{
		uint32_t unk1;
		uint64_t user_id;
		std::string platform;

		// User info.
		buffer->read_uint32(&unk1);
		buffer->read_uint64(&user_id);
		buffer->read_string(&platform);

		auto* result = new DebugObjectUCD;
		result->user_id = steam::SteamUser()->GetSteamID().bits;
		result->user_ids.push_back(steam::SteamUser()->GetSteamID().bits);
		result->user_ids.push_back(0x00659CD6);
		result->user_files.push_back("pc");
		result->user_files.push_back("ucd");

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(result);
		reply->send();
	}

	void bdRelayService::get_credentials_from_ticket(i_server* server, byte_buffer* buffer) const
	{
		std::string ticket;
		buffer->read_string(&ticket);

		auto* result = new DebugObjectUNO;

		// Create fake info.
		result->user_ids.push_back(0x1A586A45744DA396);
		result->user_ids.push_back(steam::SteamUser()->GetSteamID().bits);
		result->user_ids.push_back(16326195462233142067);
		result->user_files.push_back("youtube");
		result->user_files.push_back("steam");
		result->user_files.push_back("uno");

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(result);
		reply->send();
	}
}
