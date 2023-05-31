#include <std_include.hpp>
#include "bdStorage.hpp"

#include "../data_types.hpp"

#include <utils/compression.hpp>
#include <utils/cryptography.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/string.hpp>

#include "component/motd.hpp"
#include "component/filesystem.hpp"

namespace demonware
{
	bdStorage::bdStorage()
	{
		this->register_service(1, &bdStorage::set_legacy_user_file);
		this->register_service(3, &bdStorage::get_legacy_user_file);
		this->register_service(5, &bdStorage::list_legacy_user_files);
		this->register_service(6, &bdStorage::list_publisher_files);
		this->register_service(7, &bdStorage::get_publisher_file);
		this->register_service(8, &bdStorage::update_legacy_user_file);
		this->register_service(10, &bdStorage::set_user_file);
		this->register_service(11, &bdStorage::delete_user_file);
		this->register_service(12, &bdStorage::get_user_file);

		this->map_publisher_resource_variant("motd-.*\\.txt", motd::get_text);
		this->map_publisher_resource("newsfeed-.*\\.txt", "dw/newsfeed.txt", DW_NEWSFEED);
		this->map_publisher_resource("mm\\.cfg", "dw/mm.cfg", DW_MM_CONFIG);
		this->map_publisher_resource("playlists(_.+)?\\.aggr", "dw/playlists_tu14.aggr", DW_PLAYLISTS);
		this->map_publisher_resource("social_[Tt][Uu][0-9]+\\.cfg", "dw/social_tu14.cfg", DW_SOCIAL_CONFIG);
		this->map_publisher_resource("entitlement_config_[Tt][Uu][0-9]+\\.info", "dw/entitlement_config_tu14.info", DW_ENTITLEMENT_CONFIG);

		publisher_resources_.emplace_back(std::regex{"heatmap\\.raw"}, generate_heat_map());
	}

	void bdStorage::map_publisher_resource(const std::string& expression, const std::string& path, const int id)
	{
		auto data = filesystem::exists(path)
			? filesystem::read_file(path)
			: utils::nt::load_resource(id);

		this->map_publisher_resource_variant(expression, std::move(data));
	}

	void bdStorage::map_publisher_resource_variant(const std::string& expression, resource_variant resource)
	{
		if (resource.valueless_by_exception())
		{
			throw std::runtime_error("Publisher resource variant is empty!");
		}

		this->publisher_resources_.emplace_back(std::regex{expression}, std::move(resource));
	}

	bool bdStorage::load_publisher_resource(const std::string& name, std::string& buffer) const
	{
		for (const auto& resource : this->publisher_resources_)
		{
			if (std::regex_match(name, resource.first))
			{
				if (std::holds_alternative<std::string>(resource.second))
				{
					buffer = std::get<std::string>(resource.second);
				}
				else
				{
					buffer = std::get<callback>(resource.second)();
				}
				return true;
			}
		}

#ifdef DEBUG
		printf("[DW]: [bdStorage]: missing publisher file: %s\n", name.data());
#endif

		return false;
	}

	std::string bdStorage::get_user_file_path(const std::string& name)
	{
		return "players2/user/" + name;
	}

	std::string bdStorage::generate_heat_map()
	{
		uint8_t map[256][256];

		for (auto y = 0; y < 256; ++y)
		{
			for (auto x = 0; x < 256; ++x)
			{
				auto data = std::rand() % std::numeric_limits<std::uint8_t>::max();
				if (data % 15 == 0)
				{
					data = 0xF | ((data & 0x7) << 4) | 0x10;
				}
				else
				{
					data = 0;
				}

				map[y][x] = static_cast<std::uint8_t>(data);
			}
		}

		return utils::compression::zlib::compress(std::string(LPSTR(map), sizeof(map)));
	}

	void bdStorage::set_legacy_user_file(i_server* server, byte_buffer* buffer) const
	{
		bool priv;
		std::string filename, data;

		buffer->read_string(&filename);
		buffer->read_bool(&priv);
		buffer->read_blob(&data);

		const auto id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
		std::string id_string = utils::string::va("%llX", id);
#ifdef DEBUG
		printf("DW: Storing user file '%s' as %s\n", filename.data(), id_string.data());
#endif

		const auto path = get_user_file_path(id_string);
		utils::io::write_file(path, data);

		auto* info = new bdFileInfo;

		info->file_id = id;
		info->filename = filename;
		info->create_time = uint32_t(time(nullptr));
		info->modified_time = info->create_time;
		info->file_size = uint32_t(data.size());
		info->owner_id = 0;
		info->priv = priv;

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(info);
		reply->send();
	}

	void bdStorage::update_legacy_user_file(i_server* server, byte_buffer* buffer) const
	{
		uint64_t id;
		std::string data;

		buffer->read_uint64(&id);
		buffer->read_blob(&data);

		std::string id_string = utils::string::va("%llX", id);
#ifdef DEBUG
		printf("DW: Updating user file %s\n", id_string.data());
#endif

		const auto path = get_user_file_path(id_string);
		utils::io::write_file(path, data);

		auto* info = new bdFileInfo;

		info->file_id = id;
		info->filename = "<>";
		info->create_time = uint32_t(time(nullptr));
		info->modified_time = info->create_time;
		info->file_size = uint32_t(data.size());
		info->owner_id = 0;
		info->priv = false;

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(info);
		reply->send();
	}

	void bdStorage::get_legacy_user_file(i_server* server, byte_buffer* buffer) const
	{
		std::string filename, data;
		buffer->read_string(&filename);

		const auto id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
		std::string id_string = utils::string::va("%llX", id);
#ifdef DEBUG
		printf("DW: Loading user file: %s (%s)\n", filename.data(), id_string.data());
#endif

		const auto path = get_user_file_path(id_string);
		if (utils::io::read_file(path, &data))
		{
			auto reply = server->create_reply(this->get_sub_type());
			reply->add(new bdFileData(data));
			reply->send();
		}
		else
		{
			server->create_reply(this->get_sub_type(), game::BD_NO_FILE)->send();
		}
	}

	void bdStorage::list_legacy_user_files(i_server* server, byte_buffer* buffer) const
	{
		uint64_t unk;
		uint32_t date;
		uint16_t num_results, offset;
		std::string filename, data;

		buffer->read_uint64(&unk);
		buffer->read_uint32(&date);
		buffer->read_uint16(&num_results);
		buffer->read_uint16(&offset);
		buffer->read_string(&filename);

		auto reply = server->create_reply(this->get_sub_type());

		const auto path = get_user_file_path(filename);
		if (utils::io::read_file(path, &data))
		{
			auto* info = new bdFileInfo;

			info->file_id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
			info->filename = filename;
			info->create_time = 0;
			info->modified_time = info->create_time;
			info->file_size = uint32_t(data.size());
			info->owner_id = 0;
			info->priv = false;

			reply->add(info);
		}

		reply->send();
	}

	void bdStorage::list_publisher_files(i_server* server, byte_buffer* buffer)
	{
		uint32_t date;
		uint16_t num_results, offset;
		std::string filename, data;

		buffer->read_uint32(&date);
		buffer->read_uint16(&num_results);
		buffer->read_uint16(&offset);
		buffer->read_string(&filename);

		auto reply = server->create_reply(this->get_sub_type());

		if (this->load_publisher_resource(filename, data))
		{
			auto* info = new bdFileInfo;

			info->file_id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
			info->filename = filename;
			info->create_time = 0;
			info->modified_time = info->create_time;
			info->file_size = uint32_t(data.size());
			info->owner_id = 0;
			info->priv = false;

			reply->add(info);
		}

		reply->send();
	}

	void bdStorage::get_publisher_file(i_server* server, byte_buffer* buffer)
	{
		std::string filename;
		buffer->read_string(&filename);

#ifdef DEBUG
		printf("[DW]: [bdStorage]: loading publisher file: %s\n", filename.data());
#endif

		std::string data;
		if (this->load_publisher_resource(filename, data))
		{
#ifdef DEBUG
			printf("[DW]: [bdStorage]: sending publisher file: %s, size: %lld\n", filename.data(), data.size());
#endif
			auto reply = server->create_reply(this->get_sub_type());
			reply->add(new bdFileData(data));
			reply->send();
		}
		else
		{
			server->create_reply(this->get_sub_type(), game::BD_NO_FILE)->send();
		}
	}

	void bdStorage::delete_user_file(i_server* server, byte_buffer* buffer) const
	{
		uint64_t owner;
		std::string game, filename;

		buffer->read_string(&game);
		buffer->read_string(&filename);
		buffer->read_uint64(&owner);

		// Really remove the file?

		auto reply = server->create_reply(this->get_sub_type());
		reply->send();
	}

	void bdStorage::set_user_file(i_server* server, byte_buffer* buffer) const
	{
		bool priv;
		uint64_t owner;
		std::string game, filename, data;

		buffer->read_string(&game);
		buffer->read_string(&filename);
		buffer->read_bool(&priv);
		buffer->read_blob(&data);
		buffer->read_uint64(&owner);

		const auto path = get_user_file_path(filename);
		utils::io::write_file(path, data);

		auto* info = new bdFileInfo;

		info->file_id = *reinterpret_cast<const uint64_t*>(utils::cryptography::sha1::compute(filename).data());
		info->filename = filename;
		info->create_time = uint32_t(time(nullptr));
		info->modified_time = info->create_time;
		info->file_size = uint32_t(data.size());
		info->owner_id = owner;
		info->priv = priv;

		auto reply = server->create_reply(this->get_sub_type());
		reply->add(info);
		reply->send();
	}

	void bdStorage::get_user_file(i_server* server, byte_buffer* buffer) const
	{
		uint64_t owner{};
		std::string game, filename, platform, data;

		buffer->read_string(&game);
		buffer->read_string(&filename);
		buffer->read_uint64(&owner);
		buffer->read_string(&platform);

		const auto path = get_user_file_path(filename);
		if (utils::io::read_file(path, &data))
		{
			auto reply = server->create_reply(this->get_sub_type());
			reply->add(new bdFileData(data));
			reply->send();
		}
		else
		{
			server->create_reply(this->get_sub_type(), game::BD_NO_FILE)->send();
		}
	}
}
