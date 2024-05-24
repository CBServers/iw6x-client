#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "dvars.hpp"

#include <utils/hook.hpp>

namespace dvars
{
	struct dvar_base
	{
		unsigned int flags{};
	};

	struct dvar_bool : dvar_base
	{
		bool value{};
	};

	struct dvar_int : dvar_base
	{
		int value{};
		int min{};
		int max{};
	};

	struct dvar_float : dvar_base
	{
		float value{};
		float min{};
		float max{};
	};

	utils::hook::detour dvar_register_bool_hook;
	utils::hook::detour dvar_register_int_hook;
	utils::hook::detour dvar_register_float_hook;

	template <typename T>
	T* find_dvar(std::unordered_map<std::string, T>& map, const std::string& name)
	{
		auto i = map.find(name);
		if (i != map.end())
		{
			return &i->second;
		}

		return nullptr;
	}

	namespace override
	{
		static std::unordered_map<std::string, dvar_bool> register_bool_overrides;
		static std::unordered_map<std::string, dvar_int> register_int_overrides;
		static std::unordered_map<std::string, dvar_float> register_float_overrides;

		void register_bool(const std::string& name, const bool value, const unsigned int flags)
		{
			dvar_bool values;
			values.value = value;
			values.flags = flags;
			register_bool_overrides[name] = values;
		}

		void register_int(const std::string& name, const int value, const int min, const int max, const unsigned int flags)
		{
			dvar_int values;
			values.value = value;
			values.min = min;
			values.max = max;
			values.flags = flags;
			register_int_overrides[name] = std::move(values);
		}

		void register_float(const std::string& name, const float value, const float min, const float max, const unsigned int flags)
		{
			dvar_float values;
			values.value = value;
			values.min = min;
			values.max = max;
			values.flags = flags;
			register_float_overrides[name] = std::move(values);
		}
	}

	std::string get_string(const std::string& dvar)
	{
		const auto* dvar_value = game::Dvar_FindVar(dvar.data());
		if (dvar_value)
		{
			return {dvar_value->current.string};
		}

		return {};
	}

	const game::dvar_t* dvar_register_bool_stub(const char* name, bool value, unsigned int flags, const char* description)
	{
		const auto* var = find_dvar(override::register_bool_overrides, name);
		if (var)
		{
			value = var->value;
			flags = var->flags;
		}

		return dvar_register_bool_hook.invoke<const game::dvar_t*>(name, value, flags, description);
	}

	game::dvar_t* dvar_register_int_stub(const char* name, int value, int min, int max, unsigned int flags, const char* description)
	{
		auto* var = find_dvar(override::register_int_overrides, name);
		if (var)
		{
			value = var->value;
			min = var->min;
			max = var->max;
			flags = var->flags;
		}

		return dvar_register_int_hook.invoke<game::dvar_t*>(name, value, min, max, flags, description);
	}

	game::dvar_t* dvar_register_float_stub(const char* name, float value, float min, float max, unsigned int flags, const char* description)
	{
		auto* var = find_dvar(override::register_float_overrides, name);
		if (var)
		{
			value = var->value;
			min = var->min;
			max = var->max;
			flags = var->flags;
		}

		return dvar_register_float_hook.invoke<game::dvar_t*>(name, value, min, max, flags, description);
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			dvar_register_bool_hook.create(game::Dvar_RegisterBool, &dvar_register_bool_stub);
			dvar_register_int_hook.create(game::Dvar_RegisterInt, &dvar_register_int_stub);
			dvar_register_float_hook.create(game::Dvar_RegisterFloat, &dvar_register_float_stub);
		}

		void pre_destroy() override
		{
			dvar_register_bool_hook.clear();
			dvar_register_int_hook.clear();
			dvar_register_float_hook.clear();
		}
	};
}

REGISTER_COMPONENT(dvars::component)
