#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace ultrawide
{
	namespace
	{
		game::dvar_t* r_aspectRatio;

		game::dvar_t* dvar_register_aspect_ratio(const char* name, const char**, int default_index, unsigned int flags,
		                                         const char* description)
		{
			static const char* values[] =
			{
				"auto",
				"standard",
				"wide 16:10",
				"wide 16:9",
				"custom",
				nullptr
			};

			// register custom aspect ratio dvar
			dvars::r_aspectRatioCustom = game::Dvar_RegisterFloat("r_aspectRatioCustom", 16.0f / 9.0f, 4.0f / 3.0f,
			                                                      63.0f / 9.0f, flags,
			                                                      "Screen aspect ratio. Divide width by height to get the aspect ratio value. For example: 21 / 9 = 2,33");

			// register r_aspectRatio dvar
			r_aspectRatio = game::Dvar_RegisterEnum(name, values, default_index, flags, description);
			return r_aspectRatio;
		}


		struct wnd_struct
		{
			char pad[0x1C];
			float aspect_ratio;
		};

		float hud_aspect_ratio = 1.77778f;
		float hud_aspect_ratio_inv = -1.77778f;

		void ultrawide_patch(wnd_struct* wnd)
		{
			if (r_aspectRatio != nullptr
				&& r_aspectRatio->current.integer == 4 && dvars::r_aspectRatioCustom != nullptr)
			{
				hud_aspect_ratio = dvars::r_aspectRatioCustom->current.value;
				hud_aspect_ratio_inv = -hud_aspect_ratio;

				if (wnd)
				{
					wnd->aspect_ratio = hud_aspect_ratio;
				}
			}
		}

		// general aspect ratio
		auto ultrawide_spawn_window_stub_sp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.call_aligned(0x140519430); // og - setup hwnd struct

			a.lea(rcx, ptr(rsp, 0x0A0));
			a.call_aligned(ultrawide_patch);

			a.mov(edi, ptr(rsp, 0x0B0)); // mov edi, [rsp+0B0h]

			a.jmp(0x140519ED3);
		});

		auto ultrawide_spawn_window_stub_mp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.call_aligned(0x1405E5E70); // og - setup hwnd struct

			a.lea(rcx, ptr(rsp, 0x0A0));
			a.call_aligned(ultrawide_patch);

			a.mov(edi, ptr(rsp, 0x0B0)); // mov edi, [rsp+0B0h]

			a.jmp(0x1405E6903);
		});


		// menu aspect ratio + left offset
		auto ultrawide_menu_stub_01_sp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(eax);
			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio)));
			a.movd(xmm5, eax);

			a.divss(xmm0, xmm5); // divss   xmm0, cs:FLOAT_1_77778 // -> custom aspect ratio
			a.subss(xmm1, xmm0); // subss   xmm1, xmm0
			a.movaps(xmm0, xmm3); // movaps  xmm0, xmm3

			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio_inv)));
			a.movd(xmm5, eax);
			a.mulss(xmm0, xmm5); // mulss   xmm0, cs:dword_14083D47C // -1.77778 // -> left menu offset

			a.pop(eax);

			a.jmp(0x1401CD772);
		});

		auto ultrawide_menu_stub_01_mp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(eax);
			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio)));
			a.movd(xmm5, eax);

			a.divss(xmm0, xmm5); // divss   xmm0, cs:FLOAT_1_77778 // -> custom aspect ratio
			a.subss(xmm1, xmm0); // subss   xmm1, xmm0
			a.movaps(xmm0, xmm3); // movaps  xmm0, xmm3

			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio_inv)));
			a.movd(xmm5, eax);
			a.mulss(xmm0, xmm5); // mulss   xmm0, cs:dword_14083D47C // -1.77778 // -> left menu offset

			a.pop(eax);

			a.jmp(0x1402008D2);
		});


		// menu aspect ratio right offset
		auto ultrawide_menu_stub_02_sp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(eax);
			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio)));
			a.movd(xmm5, eax);

			a.mulss(xmm0, xmm5); // mulss   xmm0, cs:FLOAT_1_77778 // -> custom aspect ratio
			a.mulss(xmm0, xmm1); // mulss   xmm0, xmm1

			a.pop(eax);

			a.jmp(0x1401CD79F);
		});

		auto ultrawide_menu_stub_02_mp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(eax);
			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio)));
			a.movd(xmm5, eax);

			a.mulss(xmm0, xmm5); // mulss   xmm0, cs:FLOAT_1_77778 // -> custom aspect ratio
			a.mulss(xmm0, xmm1); // mulss   xmm0, xmm1

			a.pop(eax);

			a.jmp(0x1402008FF);
		});


		// fix loadscreen and in-game hud aspect ratio
		auto ultrawide_hud_stub_sp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(eax);
			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio)));
			a.movd(xmm5, eax);

			a.divss(xmm6, xmm5); // divss xmm6, cs:FLOAT_1_77778
			a.mulss(xmm6, xmm3); // mulss xmm6, xmm3

			a.pop(eax);

			a.jmp(0x14024D32F);
		});

		auto ultrawide_hud_stub_mp = utils::hook::assemble([](utils::hook::assembler& a)
		{
			a.push(eax);
			a.mov(eax, dword_ptr(reinterpret_cast<int64_t>(&hud_aspect_ratio)));
			a.movd(xmm5, eax);

			a.divss(xmm6, xmm5); // divss xmm6, cs:FLOAT_1_77778
			a.mulss(xmm6, xmm3); // mulss xmm6, xmm3

			a.pop(eax);

			a.jmp(0x1402F704F);
		});
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// ultrawide patches
			utils::hook::call(SELECT_VALUE(0x140510D0E, 0x1405DCF58), dvar_register_aspect_ratio);
			// register r_aspectRatioCustom
			utils::hook::jump(
				SELECT_VALUE(0x140519EC7, 0x1405E68F7),
				SELECT_VALUE(ultrawide_spawn_window_stub_sp, ultrawide_spawn_window_stub_mp), true);
			// apply general aspect ratio
			utils::hook::jump(
				SELECT_VALUE(0x1401CD75B, 0x1402008BB),
				SELECT_VALUE(ultrawide_menu_stub_01_sp, ultrawide_menu_stub_01_mp), true);
			// fix menu aspect ratio and left offset
			utils::hook::jump(
				SELECT_VALUE(0x1401CD793, 0x1402008F3),
				SELECT_VALUE(ultrawide_menu_stub_02_sp, ultrawide_menu_stub_02_mp), true);
			// fix menu aspect ratio right offset
			utils::hook::jump(
				SELECT_VALUE(0x14024D323, 0x1402F7043), SELECT_VALUE(ultrawide_hud_stub_sp, ultrawide_hud_stub_mp),
				true); // fix aspect ratio for loadscreen and general in-game hud

			// safeArea_adjusted dvars :: disable resets (in-game hud padding)
			utils::hook::nop(SELECT_VALUE(0x14024AEF2, 0x1402D4FB2), 5); // safeArea_adjusted_horizontal
			utils::hook::nop(SELECT_VALUE(0x14024D655, 0x1402F7375), 5); // safeArea_adjusted_horizontal

			utils::hook::nop(SELECT_VALUE(0x14024AF19, 0x1402D4FD9), 5);
			utils::hook::set<BYTE>(SELECT_VALUE(0x14024AF19, 0x1402D4FD9), 0xC3);
			// safeArea_adjusted_vertical :: return instead of jumping to Dvar_SetFloat
			utils::hook::nop(SELECT_VALUE(0x14024D664, 0x1402F7384), 5); // safeArea_adjusted_vertical

			// safeArea_ dvars :: remove cheat protection + add saved flag
			utils::hook::set<BYTE>(SELECT_VALUE(0x14024D19F + 4, 0x1402F6EBF + 4), 0x1);
			// register safeArea_horizontal (mov dword ptr [rsp+20h], 4)
			utils::hook::set<BYTE>(SELECT_VALUE(0x14024D1D7 + 4, 0x1402F6EF7 + 4), 0x1);
			// register safeArea_vertical   (mov dword ptr [rsp+20h], 4)

			utils::hook::set<BYTE>(SELECT_VALUE(0x14024D207 + 5, 0x1402F6F27 + 5), 0x0);
			utils::hook::set<BYTE>(SELECT_VALUE(0x14024D207 + 4, 0x1402F6F27 + 4), 0x1);
			// register safeArea_adjusted_horizontal (mov dword ptr [rsp+20h], 2000h)

			utils::hook::set<BYTE>(SELECT_VALUE(0x14024D237 + 5, 0x1402F6F57 + 5), 0x0);
			utils::hook::set<BYTE>(SELECT_VALUE(0x14024D237 + 4, 0x1402F6F57 + 4), 0x1);
			// register safeArea_adjusted_vertical   (mov dword ptr [rsp+20h], 2000h)
		}
	};
}

REGISTER_COMPONENT(ultrawide::component)
