#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "images.hpp"
#include "console.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/image.hpp>
#include <utils/io.hpp>
#include <utils/concurrency.hpp>

namespace images
{
	namespace
	{
		utils::hook::detour load_texture_hook;
		utils::hook::detour load_material_hook;
		utils::concurrency::container<std::unordered_map<std::string, std::string>> overriden_textures;

		static_assert(sizeof(game::GfxImage) == 104);
		static_assert(offsetof(game::GfxImage, name) == (sizeof(game::GfxImage) - sizeof(void*)));
		static_assert(offsetof(game::GfxImage, pixelData) == 56);
		static_assert(offsetof(game::GfxImage, width) == 44);
		static_assert(offsetof(game::GfxImage, height) == 46);
		static_assert(offsetof(game::GfxImage, depth) == 48);
		static_assert(offsetof(game::GfxImage, numElements) == 50);

		std::optional<std::string> load_image(game::GfxImage* image)
		{
			std::string data{};
			overriden_textures.access([&](const std::unordered_map<std::string, std::string>& textures)
				{
					if (const auto i = textures.find(image->name); i != textures.end())
					{
						data = i->second;
					}
				});

			if (data.empty() && !utils::io::read_file(utils::string::va("iw6x/images/%s.png", image->name), &data))
			{
				return {};
			}

			return { std::move(data) };
		}

		std::optional<utils::image> load_raw_image_from_file(game::GfxImage* image)
		{
			const auto image_file = load_image(image);
			if (!image_file)
			{
				return {};
			}

			return utils::image(*image_file);
		}

		bool load_custom_texture(game::GfxImage* image)
		{
			auto raw_image = load_raw_image_from_file(image);
			if (!raw_image)
			{
				return false;
			}

			image->imageFormat = 0x1000003;

			D3D11_SUBRESOURCE_DATA data{};
			data.SysMemPitch = raw_image->get_width() * 4;
			data.SysMemSlicePitch = data.SysMemPitch * raw_image->get_height();
			data.pSysMem = raw_image->get_buffer();

			game::Image_Setup(image, raw_image->get_width(), raw_image->get_height(), image->depth, image->numElements,
				image->imageFormat, DXGI_FORMAT_R8G8B8A8_UNORM, image->name, &data);

			return true;
		}

		void load_texture_stub(game::GfxImageLoadDef** load_def, game::GfxImage* image)
		{
#if defined(DEV_BUILD) && defined(DEBUG)
			printf("Loading: %s\n", image->name);
#endif

			try
			{
				if (load_custom_texture(image))
				{
					return;
				}
			}
			catch (std::exception& e)
			{
				console::error("Failed to load image %s: %s\n", image->name, e.what());
			}

			load_texture_hook.invoke(load_def, image);
		}

		int load_material_stub(game::GfxImage* image, void* a2, void* a3)
		{
#if defined(DEV_BUILD) && defined(DEBUG)
			printf("Loading: %s\n", image->name);
#endif

			try
			{
				if (load_custom_texture(image))
				{
					return 0;
				}
			}
			catch (std::exception& e)
			{
				console::error("Failed to load image %s: %s\n", image->name, e.what());
			}

			return load_material_hook.invoke<int>(image, a2, a3);
		}
	}

	void override_texture(std::string name, std::string data)
	{
		overriden_textures.access([&](std::unordered_map<std::string, std::string>& textures)
		{
			textures[std::move(name)] = std::move(data);
		});
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_dedi()) return;
		
			load_texture_hook.create(SELECT_VALUE(0x140514FD0, 0x1405E1A30), load_texture_stub);
			load_material_hook.create(SELECT_VALUE(0x140516000, 0x1405E2A60), load_material_stub);
		}
	};
}

REGISTER_COMPONENT(images::component)