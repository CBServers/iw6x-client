#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "map_rotation.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace map_rotation
{
	namespace
	{
		rotation_data dedicated_rotation;

		const game::dvar_t* sv_map_rotation;
		const game::dvar_t* sv_map_rotation_current;
		const game::dvar_t* sv_random_map_rotation;

		void set_gametype(const std::string& gametype)
		{
			assert(!gametype.empty());

			auto* g_gametype = game::Dvar_FindVar("g_gametype");
			game::Dvar_SetString(g_gametype, gametype.data());
		}

		void launch_map(const std::string& mapname)
		{
			assert(!mapname.empty());

			command::execute(utils::string::va("map %s", mapname.data()), false);
		}

		void launch_default_map()
		{
			auto* mapname = game::Dvar_FindVar("mapname");
			if (mapname && *mapname->current.string)
			{
				launch_map(mapname->current.string);
			}
			else
			{
				launch_map("mp_prisonbreak");
			}
		}

		void apply_rotation(rotation_data& rotation)
		{
			assert(!rotation.empty());

			std::size_t i = 0;
			while (i < rotation.get_entries_size())
			{
				const auto& entry = rotation.get_next_entry();
				if (entry.first == "map"s)
				{
					console::info("Loading new map: '%s'\n", entry.second.data());
					launch_map(entry.second);

					// Map was found so we exit the loop
					break;
				}

				if (entry.first == "gametype"s)
				{
					console::info("Applying new gametype: '%s'\n", entry.second.data());
					set_gametype(entry.second);
				}

				++i;
			}

			if (i == rotation.get_entries_size())
			{
				console::error("Map rotation does not contain any map. Restarting\n");
				launch_default_map();
			}
		}

		void load_rotation(const std::string& data)
		{
			static auto loaded = false;
			if (loaded)
			{
				return;
			}

			loaded = true;
			try
			{
				dedicated_rotation.parse(data);
			}
			catch (const std::exception& ex)
			{
				console::error("%s: %s contains invalid data!\n", ex.what(), sv_map_rotation->name);
			}
#ifdef _DEBUG
			console::info("dedicated_rotation size after parsing is '%llu'", dedicated_rotation.get_entries_size());
#endif
		}

		void load_map_rotation()
		{
			const std::string map_rotation = sv_map_rotation->current.string;
			if (!map_rotation.empty())
			{
#ifdef _DEBUG
				console::info("%s is not empty. Parsing...\n", sv_map_rotation->name);
#endif
				load_rotation(map_rotation);
			}
		}

		void apply_map_rotation_current(const std::string& data)
		{
			assert(!data.empty());

			rotation_data rotation_current;

			try
			{
				rotation_current.parse(data);
			}
			catch (const std::exception& ex)
			{
				console::error("%s: %s contains invalid data!\n", ex.what(), sv_map_rotation_current->name);
			}

			game::Dvar_SetString(sv_map_rotation_current, "");

			if (rotation_current.empty())
			{
				console::warn("%s is empty or contains invalid data\n", sv_map_rotation_current->name);
				launch_default_map();
				return;
			}

			apply_rotation(rotation_current);
		}

		void randomize_map_rotation()
		{
			if (sv_random_map_rotation->current.enabled)
			{
				console::info("Randomizing the map rotation\n");
				dedicated_rotation.randomize();
			}
		}

		void perform_map_rotation()
		{
			if (game::Live_SyncOnlineDataFlags(0) != 0)
			{
				scheduler::on_game_initialized(perform_map_rotation, scheduler::pipeline::main, 1s);
				return;
			}

			console::info("Rotating map...\n");

			// This takes priority because of backwards compatibility
			const std::string map_rotation_current = sv_map_rotation_current->current.string;
			if (!map_rotation_current.empty())
			{
#ifdef _DEBUG
				console::info("Applying %s\n", sv_map_rotation_current->name);
#endif
				apply_map_rotation_current(map_rotation_current);
				return;
			}

			load_map_rotation();
			if (dedicated_rotation.empty())
			{
				console::warn("%s is empty or contains invalid data. Restarting map\n", sv_map_rotation->name);
				launch_default_map();
				return;
			}

			randomize_map_rotation();

			apply_rotation(dedicated_rotation);
		}

		void trigger_map_rotation()
		{
			scheduler::schedule([]
			{
				if (game::CL_IsCgameInitialized())
				{
					return scheduler::cond_continue;
				}

				command::execute("map_rotate", false);
				return scheduler::cond_end;
			}, scheduler::pipeline::main, 1s);
		}
	}

	rotation_data::rotation_data()
		: index_(0)
	{
	}

	void rotation_data::randomize()
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		std::ranges::shuffle(this->rotation_entries_, gen);
	}

	void rotation_data::add_entry(const std::string& key, const std::string& value)
	{
		this->rotation_entries_.emplace_back(std::make_pair(key, value));
	}

	bool rotation_data::contains(const std::string& key, const std::string& value) const
	{
		return std::ranges::any_of(this->rotation_entries_, [&](const auto& entry)
		{
			return entry.first == key && entry.second == value;
		});
	}

	bool rotation_data::empty() const noexcept
	{
		return this->rotation_entries_.empty();
	}

	std::size_t rotation_data::get_entries_size() const noexcept
	{
		return this->rotation_entries_.size();
	}

	rotation_data::rotation_entry& rotation_data::get_next_entry()
	{
		const auto index = this->index_;
		++this->index_ %= this->rotation_entries_.size();
		return this->rotation_entries_.at(index);
	}

	void rotation_data::parse(const std::string& data)
	{
		const auto tokens = utils::string::split(data, ' ');
		for (std::size_t i = 0; !tokens.empty() && i < (tokens.size() - 1); i += 2)
		{
			const auto& key = tokens[i];
			const auto& value = tokens[i + 1];

			if (key == "map"s || key == "gametype"s)
			{
				this->add_entry(key, value);
			}
			else
			{
				throw map_rotation_parse_error();
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_dedi())
			{
				return;
			}

			scheduler::once([]
			{
				sv_map_rotation = game::Dvar_RegisterString("sv_mapRotation", "", game::DvarFlags::DVAR_FLAG_NONE, "");
				sv_map_rotation_current = game::Dvar_RegisterString("sv_mapRotationCurrent", "", game::DvarFlags::DVAR_FLAG_NONE, "");
			}, scheduler::pipeline::main);

			sv_random_map_rotation = game::Dvar_RegisterBool("sv_randomMapRotation", false, game::DVAR_FLAG_NONE, "Randomize map rotation");

			command::add("map_rotate", &perform_map_rotation);

			// Hook SV_MatchEnd in GScr_ExitLevel 
			utils::hook::jump(0x1403CBC30, &trigger_map_rotation);
			//utils::hook::call(0x1403CBC6C, &trigger_map_rotation);
		}
	};
}

REGISTER_COMPONENT(map_rotation::component)
