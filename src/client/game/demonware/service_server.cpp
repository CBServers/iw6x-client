#include <std_include.hpp>
#include "component/demonware.hpp"
#include "game/demonware/service_server.hpp"

#include <utils/cryptography.hpp>

namespace demonware
{
	std::string unencrypted_reply::get_data()
	{
		byte_buffer result;
		result.set_use_data_types(false);

		result.write_int32(static_cast<int>(this->buffer_.size()) + 2);
		result.write_bool(false);
		result.write_byte(this->get_type());
		result.write(this->buffer_);

		return result.get_buffer();
	}

	std::string encrypted_reply::get_data()
	{
		byte_buffer result;
		result.set_use_data_types(false);

		byte_buffer enc_buffer;
		enc_buffer.set_use_data_types(false);
		enc_buffer.write_int32(0xDEADBEEF);
		enc_buffer.write_byte(this->get_type());
		enc_buffer.write(this->buffer_);

		auto data = enc_buffer.get_buffer();

		auto size = enc_buffer.size();
		size = ~7 & (size + 7); // 8 byte align
		data.resize(size);

		result.write_int32(static_cast<int>(size) + 5);
		result.write_byte(true);

		auto seed = 0x13371337;
		result.write_int32(seed);

		const auto iv = utils::cryptography::tiger::compute(std::string(reinterpret_cast<char*>(&seed), 4));

		const std::string key(reinterpret_cast<char*>(get_key(true)), 24);
		result.write(utils::cryptography::des3::encrypt(data, iv, key));

		return result.get_buffer();
	}

	service_server::service_server(std::string _name) : name_(std::move(_name))
	{
		this->address_ = utils::cryptography::jenkins_one_at_a_time::compute(this->name_);
	}

	unsigned long service_server::get_address() const
	{
		return this->address_;
	}

	int service_server::send(const char* buf, const int len)
	{
		if (len <= 3) return -1;
		std::lock_guard<std::recursive_mutex> _(this->mutex_);

		this->incoming_queue_.push(std::string(buf, len));

		return len;
	}

	int service_server::recv(char* buf, int len)
	{
		if (len > 0 && !this->outgoing_queue_.empty())
		{
			std::lock_guard<std::recursive_mutex> _(this->mutex_);

			len = std::min(len, static_cast<int>(this->outgoing_queue_.size()));
			for (auto i = 0; i < len; ++i)
			{
				buf[i] = this->outgoing_queue_.front();
				this->outgoing_queue_.pop();
			}

			return len;
		}

		return SOCKET_ERROR;
	}

	void service_server::send_reply(reply* data)
	{
		if (!data) return;

		std::lock_guard<std::recursive_mutex> _(this->mutex_);

		this->reply_sent_ = true;
		const auto buffer = data->get_data();
		for (const auto& byte : buffer)
		{
			this->outgoing_queue_.push(byte);
		}
	}

	void service_server::call_handler(const uint8_t type, const std::string& data)
	{
		if (this->services_.find(type) != this->services_.end())
		{
			this->services_[type]->call_service(this, data);
		}
		else
		{
#ifdef DEBUG
			printf("DW: Missing handler of type %d\n", type);
#endif
		}
	}

	void service_server::run_frame()
	{
		if (!this->incoming_queue_.empty())
		{
			std::lock_guard<std::recursive_mutex> _(this->mutex_);
			const auto packet = this->incoming_queue_.front();
			this->incoming_queue_.pop();

			this->parse_packet(packet);
		}
	}

	void service_server::parse_packet(const std::string& packet)
	{
		byte_buffer buffer(packet);
		buffer.set_use_data_types(false);

		try
		{
			while (buffer.has_more_data())
			{
				int size;
				buffer.read_int32(&size);

				if (size <= 0)
				{
					const std::string zero("\x00\x00\x00\x00", 4);

					raw_reply reply(zero);
					this->send_reply(&reply);
					return;
				}
				else if (size == 200) // Connection id
				{
					byte_buffer bbufer;
					bbufer.write_uint64(0x00000000000000FD);

					auto reply = this->create_message(4);
					reply->send(&bbufer, false);
					return;
				}

				if (buffer.size() < size_t(size)) return;

				byte_buffer p_buffer;
				p_buffer.set_use_data_types(false);
				p_buffer.get_buffer().resize(size);
				buffer.read(size, p_buffer.get_buffer().data());

				bool enc;
				p_buffer.read_bool(&enc);

				if (enc)
				{
					int iv;
					p_buffer.read_int32(&iv);

					auto iv_hash = utils::cryptography::tiger::compute(std::string(reinterpret_cast<char*>(&iv), 4));

					const std::string key(reinterpret_cast<char*>(get_key(false)), 24);
					p_buffer = byte_buffer{utils::cryptography::des3::decrypt(p_buffer.get_remaining(), iv_hash, key)};
					p_buffer.set_use_data_types(false);

					int checksum;
					p_buffer.read_int32(&checksum);
				}

				uint8_t type;
				p_buffer.read_byte(&type);
#ifdef DEBUG
				printf("DW: Handling message of type %d (encrypted: %d)\n", type, enc);
#endif

				this->reply_sent_ = false;
				this->call_handler(type, p_buffer.get_remaining());

				if (!this->reply_sent_ && type != 7)
				{
					this->create_reply(type)->send();
				}
			}
		}
		catch (...)
		{
		}
	}
}
