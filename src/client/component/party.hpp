#pragma once

namespace party
{
	void switch_gamemode_if_necessary(const std::string& gametype);
	void perform_game_initialization();

	void reset_connect_state();

	void connect(const game::netadr_s& target);
	void map_restart();

	[[nodiscard]] int server_client_count();

	[[nodiscard]] int get_client_count();
	[[nodiscard]] int get_bot_count();

	[[nodiscard]] game::netadr_s& get_target();
}
