#pragma once

namespace gsc
{
	extern void* func_table[0x1000];

	void add_function(const std::string& name, game::BuiltinFunction function);

	void scr_error(const char* error);
}
