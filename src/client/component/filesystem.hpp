#pragma once

namespace filesystem
{
	class file
	{
	public:
		file(std::string name);

		[[nodiscard]] bool exists() const;
		[[nodiscard]] const std::string& get_buffer() const;
		[[nodiscard]] const std::string& get_name() const;

	private:
		bool valid_ = false;
		std::string name_;
		std::string buffer_;
	};

	std::string read_file(const std::string& path);
	bool read_file(const std::string& path, std::string* data, std::string* real_path = nullptr);
	bool find_file(const std::string& path, std::string* real_path);
	bool exists(const std::string& path);

	void register_path(const std::filesystem::path& path);
	void unregister_path(const std::filesystem::path& path);

	std::vector<std::string> get_search_paths();
	std::vector<std::string> get_search_paths_rev();
}
