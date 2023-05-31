#pragma once

namespace dvars
{
	namespace override
	{
		void register_bool(const std::string& name, bool value, unsigned int flags);
	}

	std::string get_string(const std::string& dvar);
}
