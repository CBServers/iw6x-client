#pragma once

namespace notifies
{
	extern bool hook_enabled;

	void set_gsc_hook(const char* source, const char* target);
	void clear_hook(const char* pos);
	std::size_t get_hook_count();

	void clear_callbacks();

	void enable_vm_execute_hook();
	void disable_vm_execute_hook();
}
