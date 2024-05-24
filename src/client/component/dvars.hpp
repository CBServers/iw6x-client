#pragma once

namespace dvars
{
	namespace override
	{
		void register_bool(const std::string& name, bool value, unsigned int flags);
		void register_int(const std::string& name, int value, int min, int max, unsigned int flags);
		void register_float(const std::string& name, float value, float min, float max, unsigned int flags);
	}

	std::string get_string(const std::string& dvar);
}
