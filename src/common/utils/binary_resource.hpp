#pragma once

#include <string>

namespace utils
{
	class binary_resource
	{
	public:
		binary_resource(int id, std::string file);

		std::string get_extracted_file(bool fatal_if_overwrite_fails = false);

	private:
		std::string resource_;
		std::string filename_;
		std::string path_;
	};
}
