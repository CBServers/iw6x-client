#pragma once
#include "i_service.hpp"

namespace demonware
{
	class service_server final : public i_server
	{
	public:
		explicit service_server(std::string name);

		template <typename T>
		void register_service()
		{
			static_assert(std::is_base_of<i_service, T>::value, "Service must inherit from IService");

			auto service = std::make_unique<T>();
			const uint16_t type = service->getType();

			this->services_[type] = std::move(service);
		}

		unsigned long get_address() const;

		int send(const char* buf, int len) override;
		int recv(char* buf, int len) override;
		void send_reply(reply* data) override;

		void call_handler(uint8_t type, const std::string& data);
		void run_frame();

	private:
		std::string name_;

		std::recursive_mutex mutex_;
		std::queue<char> outgoing_queue_;
		std::queue<std::string> incoming_queue_;
		std::map<uint16_t, std::unique_ptr<i_service>> services_;
		unsigned long address_ = 0;
		bool reply_sent_ = false;

		void parse_packet(const std::string& packet);
	};
}
