#include <std_include.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "command.hpp"
#include "console.hpp"

namespace stats
{
	namespace
	{
		const int controller_index = 0;

		//Checks if it's a valid reserved lookup string otherwise it crashes the client.
		bool is_reserved_lookup_string(const char* lookup_string)
		{
			const std::vector<std::string> reserved_lookup_strings
			{
				//EXTINCTION MODE RESERVED LOOKUP STRINGS
				"extinction_purchase_flags",
				"extinction_tokens",
				"extinction_crafted_flags",
				"upgrades_enabled_flags",
				"relics_enabled_flags",
				"bonus_pool_size",
				"bonus_pool_deadline",
				//MULTIPLAYER
				"prestigeLevel",
				//COMMON
				"mp_announcer_type"
			};

			return std::find(reserved_lookup_strings.begin(), reserved_lookup_strings.end(), lookup_string) 
				!= reserved_lookup_strings.end();
		}

		game::StatsGroup get_statsgroup_for_reserved_lookup_string(const char* lookup_string)
		{
			enum game::StatsGroup statsgroup{ game::STATSGROUP_COOP };

			if (!strcmp(lookup_string, "prestigeLevel"))
			{
				statsgroup = game::STATSGROUP_RANKED;
			}
			else if (!strcmp(lookup_string, "mp_announcer_type"))
			{
				statsgroup = game::STATSGROUP_COMMON;
			}

			return statsgroup;
		}

		game::StatsGroup get_statsgroup_for_lookup_string(const char* lookup_string)
		{
			const auto asset{ game::StructuredDataDef_GetAsset("mp/playerdata.def", 0x93FCu) };

			bool found_param{ false };
			enum game::StatsGroup statsgroup{ game::STATSGROUP_RANKED };

			for (int i = 0; i < asset->structs[23].propertyCount; ++i)
			{
				if (!strcmp(game::SL_ConvertToString(asset->structs[23].properties[i].name), lookup_string))
				{
					statsgroup = game::STATSGROUP_COMMON;
					found_param = true;
					break;
				}
			}

			//no need to search it again if we already found it at the common strings
			if (!found_param)
			{
				for (int i = 0; i < asset->structs[22].propertyCount; ++i)
				{
					if (!strcmp(game::SL_ConvertToString(asset->structs[22].properties[i].name), lookup_string))
					{
						statsgroup = game::STATSGROUP_COOP;
						break;
					}
				}
			}

			return statsgroup;
		}

		std::vector<unsigned int> parse_params_to_lookup_strings(const command::params& params, bool setData)
		{
			std::vector<unsigned int> lookup_strings_vector{};
			int amount_lookup_strings{ params.size() };

			//last param is a value if we're setting data.
			if (setData)
			{
				--amount_lookup_strings;
			}

			for (int i = 1; i < amount_lookup_strings; ++i)
			{
				if (utils::string::is_numeric(params.get(i)))
				{
					lookup_strings_vector.push_back(game::SL_GetStringForInt(atoi(params.get(i))));
				}
				else
				{
					lookup_strings_vector.push_back(game::SL_FindString(params.get(i)));
				}
			}

			return lookup_strings_vector;
		}

		void set_player_data_for_lookup_string(const char* lookup_string, 
			const std::vector<std::tuple<const char*, const int>>& data, game::StatsGroup statsgroup)
		{
			unsigned int nav_strings[2]{};

			nav_strings[0] = game::SL_FindString(lookup_string);
			for (int i = 0; i < data.size(); ++i)
			{
				nav_strings[1] = game::SL_FindString(std::get<0>(data[i]));
				game::LiveStorage_PlayerDataSetIntByNameArray(
					controller_index, nav_strings, 2, std::get<1>(data[i]), statsgroup);
			}
		}

		void register_squadmembers_purchase()
		{
			unsigned int nav_strings[2]{};
			const game::StringTable* unlock_table{ nullptr };
			game::StringTable_GetAsset("mp/unlocktable.csv", &unlock_table);

			if (unlock_table)
			{
				//squad purchases challenge is defined at line 112 and ends at 152.
				//it's defined 4 times each hence the jumps of 4
				for (int i = 112; i < 152; i += 4)
				{
					// Register squad purchases for calling cards
					auto squadmember_purchase = game::StringTable_GetColumnValueForRow(unlock_table, i, 3);

					nav_strings[0] = game::SL_FindString("challengeState");
					nav_strings[1] = game::SL_FindString(squadmember_purchase);
					game::LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, 2, game::STATSGROUP_RANKED);

					nav_strings[0] = game::SL_FindString("challengeProgress");
					game::LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, 1, game::STATSGROUP_RANKED);
				}
			}
		}

		void unlock_all_squadmember(const int squadmember_index)
		{
			unsigned int nav_strings[5]{};
			const int amount_regular_loadouts{ 6 };
			const game::StringTable* squad_unlocktable{ nullptr };
			game::StringTable_GetAsset("mp/squadunlocktable.csv", &squad_unlocktable);

			//select specific squadmember
			nav_strings[0] = game::SL_FindString("squadMembers");
			nav_strings[1] = game::SL_GetStringForInt(squadmember_index);

			//unlocks squadmember
			nav_strings[2] = game::SL_FindString("inUse");
			game::LiveStorage_PlayerDataSetIntByNameArray(
				controller_index, nav_strings, 3, 1, game::STATSGROUP_RANKED);

			//choose a default loadout otherwise unlocking loadouts won't work
			nav_strings[2] = game::SL_FindString("defaultSet");
			game::LiveStorage_PlayerDataSetIntByNameArray(
				controller_index, nav_strings, 3, 1, game::STATSGROUP_RANKED);

			//set max xp squad member
			nav_strings[2] = game::SL_FindString("squadMemXP");
			game::LiveStorage_PlayerDataSetIntByNameArray(
				controller_index, nav_strings, 3, 1230080, game::STATSGROUP_RANKED);

			//unlocks loadouts
			nav_strings[2] = game::SL_FindString("loadouts");
			nav_strings[4] = game::SL_FindString("inUse");

			for (int j = 0; j < amount_regular_loadouts; ++j)
			{
				nav_strings[3] = game::SL_GetStringForInt(j);
				game::LiveStorage_PlayerDataSetIntByNameArray(
					controller_index, nav_strings, 5, 2, game::STATSGROUP_RANKED);
			}

			//unlocks all weapons, reticles and killstreaks 
			nav_strings[2] = game::SL_FindString("challengeState");
			if (squad_unlocktable)
			{
				for (int j = 0; j < squad_unlocktable->rowCount; ++j)
				{
					auto unlock_item = game::StringTable_GetColumnValueForRow(squad_unlocktable, j, 3);

					if (unlock_item[0] == '\0')
						continue;

					nav_strings[3] = game::SL_FindString(unlock_item);
					game::LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 4, 2, game::STATSGROUP_RANKED);
				}
			}
		}

		void unlock_all_extinction()
		{
			const auto persistent_data_buffer{ game::LiveStorage_GetPersistentDataBuffer(controller_index) };
			const std::vector<std::tuple<const char*, const int>> data{
				{"prestige", 25},
				{"experience", 1845000},
				//not sure if necessary 
				{"rank", 31}, 
				{"revives", 100},
				{"kills", 10000},
				{"escaped", 10},
				//unlocks background with the challenges "Beat any Extinction map with x Relic(s) Active"
				{"headShots", 5}
			};

			set_player_data_for_lookup_string("alienPlayerStats", data, game::STATSGROUP_COOP);

			game::LiveStorage_PlayerDataSetReservedInt(
				persistent_data_buffer, "extinction_purchase_flags", -1, 0, game::STATSGROUP_COOP);
			game::LiveStorage_PlayerDataSetReservedInt(
				persistent_data_buffer, "extinction_tokens", 5000, 0, game::STATSGROUP_COOP);
		}

		void unlock_all_multiplayer()
		{	
			const int amount_squad_members{ 10 };
			const auto persistent_data_buffer{ game::LiveStorage_GetPersistentDataBuffer(controller_index) };	

			for (int i = 0; i < amount_squad_members; ++i)
			{
				unlock_all_squadmember(i);
			}

			register_squadmembers_purchase();

			//not the correct way to set each squadmembers prestige
			//but i couldn't figure it out how it's done.
			for (int i = 0; i < amount_squad_members; ++i)
			{
				utils::hook::set<int>(0x1445A2B20 + (i * 4), i);
			}

			//set the players prestige to 10th prestige
			game::LiveStorage_PlayerDataSetReservedInt(
				persistent_data_buffer, "prestigeLevel", 10, 0, game::STATSGROUP_RANKED);

			//gives unlock points but not really necassary since everything is already unlocked
			game::LiveStorage_PlayerDataSetIntByName(controller_index, game::SL_FindString("unlockPoints"), 5000, game::StatsGroup::STATSGROUP_RANKED);
		}

		void unlock_all_challenges()
		{
			unsigned int nav_strings[2]{};
			const game::StringTable* challenges_table{ nullptr };
			game::StringTable_GetAsset("mp/allchallengestable.csv", &challenges_table);

			if (challenges_table)
			{
				for (int i = 0; i < challenges_table->rowCount; ++i)
				{
					// Find challenge
					auto challenge{ game::StringTable_GetColumnValueForRow(challenges_table, i, 0) };

					int max_state{ 0 };
					int max_progress{ 0 };

					// Find correct tier and progress
					for (int j = 0; j < 8; ++j)
					{
						int progress{ atoi(game::StringTable_GetColumnValueForRow(challenges_table, i, 9 + j * 2)) };
						if (!progress) break;

						max_state = j + 2;
						max_progress = progress;
					}

					nav_strings[0] = game::SL_FindString("challengeState");
					nav_strings[1] = game::SL_FindString(challenge);
					game::LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, max_state, game::STATSGROUP_RANKED);

					nav_strings[0] = game::SL_FindString("challengeProgress");
					game::LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, max_progress, game::STATSGROUP_RANKED);
				}
			}
		}

		void unlock_past_title_backgrounds()
		{
			const std::vector<std::tuple<const char*, const int>> data{
				{"playedmw3", 1},
				{"playedblackops2", 1},
				{"mw3prestige", 20},
				{"blackops2prestige", 11}
			};

			set_player_data_for_lookup_string("pastTitleData", data, game::STATSGROUP_COMMON);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (!game::environment::is_mp())
			{
				return;
			}

			command::add("setReservedPlayerDataInt", [](const command::params& params)
			{
				if (params.size() < 3 || !is_reserved_lookup_string(params.get(1)))
				{
					console::info("usage: setReservedPlayerDataInt <lookup_string>, <value>\n");
					return;
				}

				const auto persistent_data_buffer{ game::LiveStorage_GetPersistentDataBuffer(controller_index) };
				const char* first_param{ params.get(1) };
				const auto value{ atoi(params.get(2)) };
				const auto statsgroup{ get_statsgroup_for_reserved_lookup_string(first_param) };

				game::LiveStorage_PlayerDataSetReservedInt(
					persistent_data_buffer, first_param, value, 0, statsgroup);
			});

			command::add("getReservedPlayerDataInt", [](const command::params& params)
			{
				if (params.size() < 2 || !is_reserved_lookup_string(params.get(1)))
				{
					console::info("usage: getReservedPlayerDataInt <lookup_string>\n");
					return;
				}

				const auto persistent_data_buffer{ game::LiveStorage_GetPersistentDataBuffer(controller_index) };
				const char* first_param{ params.get(1) };
				const auto statsgroup{ get_statsgroup_for_reserved_lookup_string(first_param) };

				const auto result{ game::LiveStorage_PlayerDataGetReservedInt(
					persistent_data_buffer, first_param, statsgroup) };
				console::info("%d\n", result);
			});

			command::add("setPlayerDataInt", [](const command::params& params)
			{
				if (params.size() < 3)
				{
					console::info("usage: setPlayerDataInt <lookup_string>, ... , <lookup_string>, <value>\n");
					return;
				}

				const std::vector lookup_strings_vector{parse_params_to_lookup_strings(params, true)};
				const auto value = atoi(params.get(params.size() - 1));
				const auto statsgroup{get_statsgroup_for_lookup_string(params.get(1))};

				game::LiveStorage_PlayerDataSetIntByNameArray(controller_index, &lookup_strings_vector[0], params.size() - 2, value, statsgroup);

				//This is necessary for the stats to stick after closing the game
				game::LiveStorage_StatsWriteNeeded(controller_index);
			});

			command::add("getPlayerDataInt", [](const command::params& params)
			{
				if (params.size() < 2)
				{
					console::info("usage: getPlayerDataInt <lookup_string>, ... , <lookup_string>\n");
					return;
				}

				const std::vector lookup_strings_vector{parse_params_to_lookup_strings(params, false)};
				const auto statsgroup{ get_statsgroup_for_lookup_string(params.get(1)) };

				const auto result{game::LiveStorage_PlayerDataGetIntByNameArray(controller_index, &lookup_strings_vector[0], params.size() - 1, statsgroup)};

				console::info("%d\n", result);
			});

			command::add("unlockstats", []()
			{
				unlock_all_extinction();
				unlock_all_multiplayer();
				unlock_all_challenges();
				unlock_past_title_backgrounds();

				//This is necessary for the stats to stick after closing the game
				game::LiveStorage_StatsWriteNeeded(controller_index);
			});
		}
	};
}

REGISTER_COMPONENT(stats::component)
