init()
{
	if (maps\mp\_utility::is_aliens())
	{
		return;
	}

	if (isdefined(level.ishorde) && level.ishorde)
	{
		return;
	}

	// define onteamselection callback function used in balanceteams()
	level.onteamselection = ::set_team;
}

set_team(team)
{
	if (team != self.pers["team"])
	{
		self.switching_teams = true;
		self.joining_team = team;
		self.leaving_team = self.pers["team"];
	}

	if (self.sessionstate == "playing")
	{
		self suicide();
	}

	maps\mp\gametypes\_menus::addtoteam(team);
	maps\mp\gametypes\_menus::endrespawnnotify();
}
