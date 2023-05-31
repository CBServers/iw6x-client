#pragma once
#include "bit_buffer.hpp"
#include "byte_buffer.hpp"

namespace demonware
{
	class reply
	{
	public:
		virtual ~reply() = default;
		virtual std::string get_data() = 0;
	};

	class raw_reply : public reply
	{
	public:
		raw_reply() = default;

		explicit raw_reply(std::string data) : buffer_(std::move(data))
		{
		}

		virtual std::string get_data() override
		{
			return this->buffer_;
		}

	protected:
		std::string buffer_;
	};

	class typed_reply : public raw_reply
	{
	public:
		typed_reply(uint8_t _type) : type_(_type)
		{
		}

	protected:
		uint8_t get_type() const { return this->type_; }

	private:
		uint8_t type_;
	};

	class encrypted_reply final : public typed_reply
	{
	public:
		encrypted_reply(const uint8_t type, bit_buffer* bbuffer) : typed_reply(type)
		{
			this->buffer_.append(bbuffer->get_buffer());
		}

		encrypted_reply(const uint8_t type, byte_buffer* bbuffer) : typed_reply(type)
		{
			this->buffer_.append(bbuffer->get_buffer());
		}

		virtual std::string get_data() override;
	};

	class unencrypted_reply final : public typed_reply
	{
	public:
		unencrypted_reply(uint8_t _type, bit_buffer* bbuffer) : typed_reply(_type)
		{
			this->buffer_.append(bbuffer->get_buffer());
		}

		unencrypted_reply(uint8_t _type, byte_buffer* bbuffer) : typed_reply(_type)
		{
			this->buffer_.append(bbuffer->get_buffer());
		}

		virtual std::string get_data() override;
	};

	class remote_reply;
	class service_reply;

	class i_server
	{
	public:
		virtual ~i_server() = default;
		virtual int send(const char* buf, int len) = 0;
		virtual int recv(char* buf, int len) = 0;

		virtual void send_reply(reply* reply) = 0;

		virtual std::shared_ptr<remote_reply> create_message(uint8_t type)
		{
			auto reply = std::make_shared<remote_reply>(this, type);
			return reply;
		}

		virtual std::shared_ptr<service_reply> create_reply(uint8_t type,
		                                                    uint32_t error = 0 /*Game::bdLobbyErrorCode::BD_NO_ERROR*/)
		{
			auto reply = std::make_shared<service_reply>(this, type, error);
			return reply;
		}
	};

	class remote_reply final
	{
	public:
		remote_reply(i_server* server, uint8_t _type) : type_(_type), server_(server)
		{
		}

		template <typename BufferType>
		void send(BufferType* buffer, const bool encrypted)
		{
			std::unique_ptr<typed_reply> reply;

			if (encrypted) reply = std::make_unique<encrypted_reply>(this->type_, buffer);
			else reply = std::make_unique<unencrypted_reply>(this->type_, buffer);
			this->server_->send_reply(reply.get());
		}

		uint8_t get_type() const { return this->type_; }

	private:
		uint8_t type_;
		i_server* server_;
	};

	class i_serializable
	{
	public:
		virtual ~i_serializable() = default;

		virtual void serialize(byte_buffer* /*buffer*/)
		{
		}

		virtual void deserialize(byte_buffer* /*buffer*/)
		{
		}
	};

	class service_reply final
	{
	public:
		service_reply(i_server* _server, uint8_t _type, uint32_t _error) : type_(_type), error_(_error),
		                                                                   reply_(_server, 1)
		{
		}

		uint64_t send()
		{
			static uint64_t id = 0x8000000000000001;
			const auto transaction_id = ++id;

			byte_buffer buffer;
			buffer.write_uint64(transaction_id);
			buffer.write_uint32(this->error_);
			buffer.write_byte(this->type_);

			if (!this->error_)
			{
				buffer.write_uint32(uint32_t(this->objects_.size()));
				if (!this->objects_.empty())
				{
					buffer.write_uint32(uint32_t(this->objects_.size()));

					for (auto& object : this->objects_)
					{
						object->serialize(&buffer);
					}

					this->objects_.clear();
				}
			}
			else
			{
				buffer.write_uint64(transaction_id);
			}

			this->reply_.send(&buffer, true);
			return transaction_id;
		}

		void add(const std::shared_ptr<i_serializable>& object)
		{
			this->objects_.push_back(object);
		}

		void add(i_serializable* object)
		{
			this->add(std::shared_ptr<i_serializable>(object));
		}

	private:
		uint8_t type_;
		uint32_t error_;
		remote_reply reply_;

		std::vector<std::shared_ptr<i_serializable>> objects_;
	};
}
