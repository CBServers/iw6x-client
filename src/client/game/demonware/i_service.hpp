#pragma once
#include "i_server.hpp"

namespace demonware
{
	class i_service
	{
	public:
		virtual ~i_service() = default;
		i_service() = default;

		// Copying or moving a service object won't work
		// as the callbacks are bound to the initial object pointer
		// Therefore, you should never declare copy/move
		// constructors when inheriting from IService!
		i_service(i_service&&) = delete;
		i_service(const i_service&) = delete;
		i_service& operator=(const i_service&) = delete;

		typedef std::function<void(i_server*, byte_buffer*)> callback;

		virtual uint16_t getType() = 0;

		virtual void call_service(i_server* server, const std::string& data)
		{
			std::lock_guard _(this->mutex_);

			byte_buffer buffer(data);
			buffer.read_byte(&this->sub_type_);
#ifdef DEBUG
			printf("DW: Handling subservice of type %d\n", this->sub_type_);
#endif

			const auto callback = this->callbacks_.find(this->sub_type_);
			if (callback != this->callbacks_.end())
			{
				callback->second(server, &buffer);
			}
			else
			{
#ifdef DEBUG
				printf("DW: Missing subservice %d for type %d\n", this->sub_type_, this->getType());
#endif
			}
		}

	protected:
		std::map<uint8_t, callback> callbacks_{};

		template <typename Class, typename T, typename... Args>
		void register_service(const uint8_t type, T (Class::*callback)(Args ...) const)
		{
			this->callbacks_[type] = [this, callback](Args ... args) -> T
			{
				return (reinterpret_cast<Class*>(this)->*callback)(args...);
			};
		}

		template <typename Class, typename T, typename... Args>
		void register_service(const uint8_t type, T (Class::*callback)(Args ...))
		{
			this->callbacks_[type] = [this, callback](Args ... args) -> T
			{
				return (reinterpret_cast<Class*>(this)->*callback)(args...);
			};
		}

		uint8_t get_sub_type() const { return this->sub_type_; }

	private:
		std::mutex mutex_;

		uint8_t sub_type_{};
	};

	template <uint16_t Type>
	class i_generic_service : public i_service
	{
	public:
		uint16_t getType() override { return Type; }
	};
}
