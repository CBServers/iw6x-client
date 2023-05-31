#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/scripting/execution.hpp"
#include "game/scripting/function.hpp"
#include "game/scripting/functions.hpp"
#include "game/scripting/lua/error.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

#include "component/console.hpp"
#include "component/scripting.hpp"
#include "component/notifies.hpp"
#include "component/command.hpp"

#include "script_extension.hpp"
#include "script_error.hpp"
#include "script_loading.hpp"

namespace gsc
{
	std::uint16_t function_id_start = 0x25D;
	std::uint16_t method_id_start = 0x8429;

	void* func_table[0x1000];

	const game::dvar_t* developer_script = nullptr;

	namespace
	{
		#define RVA(ptr) static_cast<std::uint32_t>(reinterpret_cast<std::size_t>(ptr) - 0x140000000)

		struct gsc_error : public std::runtime_error
		{
			using std::runtime_error::runtime_error;
		};

		std::unordered_map<std::uint16_t, game::BuiltinFunction> functions;

		bool force_error_print = false;
		std::optional<std::string> gsc_error_msg;

		unsigned int scr_get_function_stub(const char** p_name, int* type)
		{
			const auto result = utils::hook::invoke<unsigned int>(0x1403CD9F0, p_name, type);

			for (const auto& [name, func] : functions)
			{
				game::Scr_RegisterFunction(func, 0, name);
			}

			return result;
		}

		scripting::script_value get_argument(int index)
		{
			if (static_cast<std::uint32_t>(index) >= game::scr_VmPub->outparamcount)
			{
				throw gsc_error(std::format("Parameter {} does not exist", index + 1));
			}

			return {game::scr_VmPub->top[-index]};
		}

		void execute_custom_function(game::BuiltinFunction function)
		{
			auto error = false;

			try
			{
				function();
			}
			catch (const std::exception& ex)
			{
				error = true;
				force_error_print = true;
				gsc_error_msg = ex.what();
			}

			if (error)
			{
				game::Scr_ErrorInternal();
			}
		}

		void vm_call_builtin_function(const std::uint32_t index)
		{
			const auto func = reinterpret_cast<game::BuiltinFunction>(scripting::get_function_by_index(index));

			const auto custom =  functions.contains(static_cast<std::uint16_t>(index));
			if (!custom)
			{
				func();
			}
			else
			{
				execute_custom_function(func);
			}
		}

		void builtin_call_error(const std::string& error)
		{
			const auto pos = game::scr_function_stack->pos;
			const auto function_id = *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::size_t>(pos - 2));

			if (function_id > 0x1000)
			{
				console::warn("in call to builtin method \"%s\"%s", gsc_ctx->meth_name(function_id).data(), error.data());
			}
			else
			{
				console::warn("in call to builtin function \"%s\"%s", gsc_ctx->func_name(function_id).data(), error.data());
			}
		}

		std::optional<std::string> get_opcode_name(const std::uint8_t opcode)
		{
			try
			{
				const auto index = gsc_ctx->opcode_enum(opcode);
				return {gsc_ctx->opcode_name(index)};
			}
			catch (...)
			{
				return {};
			}
		}

		void print_callstack()
		{
			for (auto frame = game::scr_VmPub->function_frame; frame != game::scr_VmPub->function_frame_start; --frame)
			{
				const auto pos = frame == game::scr_VmPub->function_frame ? game::scr_function_stack->pos : frame->fs.pos;
				const auto function = find_function(frame->fs.pos);

				if (function.has_value())
				{
					console::warn("\tat function \"%s\" in file \"%s.gsc\"\n", function.value().first.data(), function.value().second.data());
				}
				else
				{
					console::warn("\tat unknown location %p\n", pos);
				}
			}
		}

		void vm_error_stub(int mark_pos)
		{
			if (!developer_script->current.enabled && !force_error_print)
			{
				utils::hook::invoke<void>(0x1404E4D00, mark_pos);
				return;
			}

			console::warn("******* script runtime error ********\n");
			const auto opcode_id = *reinterpret_cast<std::uint8_t*>(0x144D57840);

			const std::string error = gsc_error_msg.has_value() ? std::format(": {}", gsc_error_msg.value()) : std::string();

			if ((opcode_id >= 0x1A && opcode_id <= 0x20) || (opcode_id >= 0xA8 && opcode_id <= 0xAE))
			{
				builtin_call_error(error);
			}
			else
			{
				const auto opcode = get_opcode_name(opcode_id);
				if (opcode.has_value())
				{
					console::warn("while processing instruction %s%s\n", opcode.value().data(), error.data());
				}
				else
				{
					console::warn("while processing instruction 0x%X%s\n", opcode_id, error.data());
				}
			}

			force_error_print = false;
			gsc_error_msg = {};

			print_callstack();
			console::warn("************************************\n");
			utils::hook::invoke<void>(0x1404E4D00, mark_pos);
		}

		void inc_in_param()
		{
			game::Scr_ClearOutParams();

			if (game::scr_VmPub->top == game::scr_VmPub->maxstack)
			{
				game::Sys_Error("Internal script stack overflow");
			}

			++game::scr_VmPub->top;
			++game::scr_VmPub->inparamcount;
		}

		void add_code_pos(const char* pos)
		{
			inc_in_param();
			game::scr_VmPub->top->type = game::VAR_FUNCTION;
			game::scr_VmPub->top->u.codePosValue = pos;
		}

		void scr_print()
		{
			for (auto i = 0u; i < game::Scr_GetNumParam(); ++i)
			{
				console::info("%s", game::Scr_GetString(i));
			}
		}

		void scr_print_ln()
		{
			for (auto i = 0u; i < game::Scr_GetNumParam(); ++i)
			{
				console::info("%s", game::Scr_GetString(i));
			}

			console::info("\n");
		}

		void assert_cmd()
		{
			if (!game::Scr_GetInt(0))
			{
				scr_error("Assert fail");
			}
		}

		void assert_ex_cmd()
		{
			if (!game::Scr_GetInt(0))
			{
				scr_error(utils::string::va("Assert fail: %s", game::Scr_GetString(1)));
			}
		}

		void assert_msg_cmd()
		{
			scr_error(utils::string::va("Assert fail: %s", game::Scr_GetString(0)));
		}
	}

	void add_function(const std::string& name, game::BuiltinFunction function)
	{
		++function_id_start;
		functions[function_id_start] = function;
		gsc_ctx->func_add(name, function_id_start);
	}

	void scr_error(const char* error)
	{
		force_error_print = true;
		gsc_error_msg = error;

		game::Scr_ErrorInternal();
	}

	class extension final : public component_interface
	{
	public:
		void post_unpack() override
		{
			utils::hook::set<game::BuiltinFunction>(SELECT_VALUE(0x14086F468, 0x1409E6CE8), scr_print);
			utils::hook::set<game::BuiltinFunction>(SELECT_VALUE(0x14086F480, 0x1409E6D00), scr_print_ln);

			utils::hook::set<std::uint32_t>(SELECT_VALUE(0x1403D353C, 0x14042E33C), 0x1000); // Scr_RegisterFunction

			utils::hook::set<std::uint32_t>(SELECT_VALUE(0x1403D3542 + 4, 0x14042E342 + 4), RVA(&func_table)); // Scr_RegisterFunction
			utils::hook::set<std::uint32_t>(SELECT_VALUE(0x1403E0BDD + 3, 0x14043BBBE + 3), RVA(&func_table)); // VM_Execute_0
			utils::hook::inject(SELECT_VALUE(0x1403D38E4 + 3, 0x14042E734 + 3), &func_table); // Scr_BeginLoadScripts

			if (game::environment::is_sp())
			{
				return;
			}

			developer_script = game::Dvar_RegisterBool("developer_script", false, game::DVAR_FLAG_NONE, "Enable developer script comments");

			utils::hook::nop(0x14043BBBE + 5, 2);
			utils::hook::call(0x14043BBBE, vm_call_builtin_function);

			utils::hook::call(0x14043CEB1, vm_error_stub);

			utils::hook::call(0x14042E76F, scr_get_function_stub);

			utils::hook::set<game::BuiltinFunction>(0x1409E6E38, assert_ex_cmd);
			utils::hook::set<game::BuiltinFunction>(0x1409E6E50, assert_msg_cmd);
			utils::hook::set<game::BuiltinFunction>(0x1409E6E20, assert_cmd);

			add_function("getfunction", []
			{
				const auto* filename = game::Scr_GetString(0);
				const auto* function = game::Scr_GetString(1);

				if (scripting::script_function_table[filename].contains(function))
				{
					const auto func = scripting::function{scripting::script_function_table[filename][function]};
					add_code_pos(func.get_pos());
					return;
				}

				throw gsc_error("Function not found");
			});

			add_function("replacefunc", []
			{
				const auto what = get_argument(0).get_raw();
				const auto with = get_argument(1).get_raw();

				if (what.type != game::VAR_FUNCTION || with.type != game::VAR_FUNCTION)
				{
					throw gsc_error("Parameter must be a function");
				}

				notifies::set_gsc_hook(what.u.codePosValue, with.u.codePosValue);
			});

			add_function("executecommand", []
			{
				const auto cmd = get_argument(0).as<std::string>();
				command::execute(cmd);
			});
		}
	};
}

REGISTER_COMPONENT(gsc::extension)
