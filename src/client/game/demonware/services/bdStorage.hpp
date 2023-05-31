#pragma once
#include "../i_service.hpp"

namespace demonware
{
	class bdStorage final : public i_generic_service<10>
	{
	public:
		bdStorage();

	private:
		using callback = std::function<std::string()>;
		using resource_variant = std::variant<std::string, callback>;
		std::vector<std::pair<std::regex, resource_variant>> publisher_resources_;

		void set_legacy_user_file(i_server* server, byte_buffer* buffer) const;
		void update_legacy_user_file(i_server* server, byte_buffer* buffer) const;
		void get_legacy_user_file(i_server* server, byte_buffer* buffer) const;
		void list_legacy_user_files(i_server* server, byte_buffer* buffer) const;
		void list_publisher_files(i_server* server, byte_buffer* buffer);
		void get_publisher_file(i_server* server, byte_buffer* buffer);
		void delete_user_file(i_server* server, byte_buffer* buffer) const;
		void set_user_file(i_server* server, byte_buffer* buffer) const;
		void get_user_file(i_server* server, byte_buffer* buffer) const;

		void map_publisher_resource(const std::string& expression, const std::string& path, int id);
		void map_publisher_resource_variant(const std::string& expression, resource_variant resource);
		bool load_publisher_resource(const std::string& name, std::string& buffer) const;

		static std::string get_user_file_path(const std::string& name);
		static std::string generate_heat_map();
	};
}
