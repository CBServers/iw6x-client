#pragma once

namespace notifies
{
	extern bool hook_enabled;

	void set_lua_hook(const char* pos, const sol::protected_function&);
	void set_gsc_hook(const char* source, const char* target);
	void clear_hook(const char* pos);
	std::size_t get_hook_count();

	void add_player_damage_callback(const sol::protected_function& callback);
	void add_player_killed_callback(const sol::protected_function& callback);
	void clear_callbacks();

	void enable_vm_execute_hook();
	void disable_vm_execute_hook();
}
