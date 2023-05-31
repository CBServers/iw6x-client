#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "filesystem.hpp"
#include "game_module.hpp"
#include "console.hpp"

#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>

namespace filesystem
{
	namespace
	{
		bool initialized = false;

		bool custom_path_registered = false;

		std::deque<std::filesystem::path>& get_search_paths_internal()
		{
			static std::deque<std::filesystem::path> search_paths{};
			return search_paths;
		}

		std::string get_binary_directory()
		{
			const auto dir = game_module::get_host_module().get_folder();
			return utils::string::replace(dir, "/", "\\");
		}

		void register_custom_path_stub(const char* path, const char* dir)
		{
			if (!custom_path_registered)
			{
				custom_path_registered = true;

				const auto launcher_dir = get_binary_directory();
				game::FS_AddLocalizedGameDirectory(launcher_dir.data(), "data");
			}

			game::FS_AddLocalizedGameDirectory(path, dir);
		}

		void fs_startup_stub(const char* gamename)
		{
			console::info("[FS] Startup\n");

			custom_path_registered = false;

			game::FS_Startup(gamename);
		}

		std::vector<std::filesystem::path> get_paths(const std::filesystem::path& path)
		{
			std::vector<std::filesystem::path> paths{};

			paths.push_back(path);

			return paths;
		}

		bool can_insert_path(const std::filesystem::path& path)
		{
			for (const auto& path_ : get_search_paths_internal())
			{
				if (path_ == path)
				{
					return false;
				}
			}

			return true;
		}

		void startup()
		{
			register_path("iw6x");
			register_path(get_binary_directory() + "\\data");

			// game's search paths
			register_path("devraw");
			register_path("devraw_shared");
			register_path("raw_shared");
			register_path("raw");
			register_path("main");
		}

		void check_for_startup()
		{
			if (!initialized)
			{
				initialized = true;
				startup();
			}
		}
	}

	file::file(std::string name)
		: name_(std::move(name))
	{
		char* buffer{};
		const auto size = game::FS_ReadFile(this->name_.data(), &buffer);

		if (size >= 0 && buffer)
		{
			this->valid_ = true;
			this->buffer_.append(buffer, size);
			game::FS_FreeFile(buffer);
		}
	}

	bool file::exists() const
	{
		return this->valid_;
	}

	const std::string& file::get_buffer() const
	{
		return this->buffer_;
	}

	const std::string& file::get_name() const
	{
		return this->name_;
	}

	std::string read_file(const std::string& path)
	{
		check_for_startup();

		for (const auto& search_path : get_search_paths_internal())
		{
			const auto path_ = search_path / path;
			if (utils::io::file_exists(path_.generic_string()))
			{
				return utils::io::read_file(path_.generic_string());
			}
		}

		return {};
	}

	bool read_file(const std::string& path, std::string* data, std::string* real_path)
	{
		check_for_startup();

		for (const auto& search_path : get_search_paths_internal())
		{
			const auto path_ = search_path / path;
			if (utils::io::read_file(path_.generic_string(), data))
			{
				if (real_path != nullptr)
				{
					*real_path = path_.generic_string();
				}

				return true;
			}
		}

		return false;
	}

	bool find_file(const std::string& path, std::string* real_path)
	{
		check_for_startup();

		for (const auto& search_path : get_search_paths_internal())
		{
			const auto path_ = search_path / path;
			if (utils::io::file_exists(path_.generic_string()))
			{
				*real_path = path_.generic_string();
				return true;
			}
		}

		return false;
	}

	bool exists(const std::string& path)
	{
		check_for_startup();

		for (const auto& search_path : get_search_paths_internal())
		{
			const auto path_ = search_path / path;
			if (utils::io::file_exists(path_.generic_string()))
			{
				return true;
			}
		}

		return false;
	}

	void register_path(const std::filesystem::path& path)
	{
		const auto paths = get_paths(path);
		for (const auto& path_ : paths)
		{
			if (can_insert_path(path_))
			{
				console::info("[FS] Registering path '%s'\n", path_.generic_string().data());
				get_search_paths_internal().push_front(path_);
			}
		}
	}

	void unregister_path(const std::filesystem::path& path)
	{
		const auto paths = get_paths(path);
		for (const auto& path_ : paths)
		{
			auto& search_paths = get_search_paths_internal();
			for (auto i = search_paths.begin(); i != search_paths.end();)
			{
				if (*i == path_)
				{
					console::info("[FS] Unregistering path '%s'\n", path_.generic_string().data());
					i = search_paths.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
	}

	std::vector<std::string> get_search_paths()
	{
		std::vector<std::string> paths{};

		for (const auto& path : get_search_paths_internal())
		{
			paths.push_back(path.generic_string());
		}

		return paths;
	}

	std::vector<std::string> get_search_paths_rev()
	{
		std::vector<std::string> paths{};
		const auto& search_paths = get_search_paths_internal();

		for (auto i = search_paths.rbegin(); i != search_paths.rend(); ++i)
		{
			paths.push_back(i->generic_string());
		}

		return paths;
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Set fs_basegame
			utils::hook::inject(SELECT_VALUE(0x14041C053, 0x1404DDA13), "iw6x");

			if (game::environment::is_sp())
			{
				utils::hook::call(0x14041B744, fs_startup_stub);

				utils::hook::call(0x14041CD00, register_custom_path_stub);
				utils::hook::call(0x14041CD20, register_custom_path_stub);
				utils::hook::call(0x14041CD5F, register_custom_path_stub);
			}
			else
			{
				utils::hook::call(0x1404DD704, fs_startup_stub);
				utils::hook::call(0x1404DDB43, fs_startup_stub);

				utils::hook::call(0x1404DE550, register_custom_path_stub);
				utils::hook::call(0x1404DE570, register_custom_path_stub);
				utils::hook::call(0x1404DE5AF, register_custom_path_stub);
			}
		}
	};
}

REGISTER_COMPONENT(filesystem::component)
