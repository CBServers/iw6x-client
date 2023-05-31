// IW6 GSC SOURCE
// Dumped by https://github.com/xensik/gsc-tool

isswitchingteams()
{
    if ( isdefined( self.switching_teams ) )
        return 1;

    return 0;
}

isteamswitchbalanced()
{
    var_0 = maps\mp\gametypes\_teams::countplayers();
    var_0[self.leaving_team]--;
    var_0[self.joining_team]++;
    return var_0[self.joining_team] - var_0[self.leaving_team] < 2;
}

isfriendlyfire( var_0, var_1 )
{
    if ( !level.teambased )
        return 0;

    if ( !isdefined( var_1 ) )
        return 0;

    if ( !isplayer( var_1 ) && !isdefined( var_1.team ) )
        return 0;

    if ( var_0.team != var_1.team )
        return 0;

    if ( var_0 == var_1 )
        return 0;

    return 1;
}

killedself( var_0 )
{
    if ( !isplayer( var_0 ) )
        return 0;

    if ( var_0 != self )
        return 0;

    return 1;
}

handleteamchangedeath()
{
    if ( !level.teambased )
        return;

    if ( self.joining_team == "spectator" || !isteamswitchbalanced() )
    {
        self thread [[ level.onxpevent ]]( "suicide" );
        maps\mp\_utility::incpersstat( "suicides", 1 );
        self.suicides = maps\mp\_utility::getpersstat( "suicides" );
    }

    if ( isdefined( level.onteamchangedeath ) )
        [[ level.onteamchangedeath ]]( self );
}

handleworlddeath( var_0, var_1, var_2, var_3 )
{
    if ( !isdefined( var_0 ) )
        return;

    if ( !isdefined( var_0.team ) )
    {
        handlesuicidedeath( var_2, var_3 );
        return;
    }

    if ( level.teambased && var_0.team != self.team || !level.teambased )
    {
        if ( isdefined( level.onnormaldeath ) && ( isplayer( var_0 ) || isagent( var_0 ) ) && var_0.team != "spectator" )
            [[ level.onnormaldeath ]]( self, var_0, var_1 );
    }
}

handlesuicidedeath( var_0, var_1 )
{
    self thread [[ level.onxpevent ]]( "suicide" );
    maps\mp\_utility::incpersstat( "suicides", 1 );
    self.suicides = maps\mp\_utility::getpersstat( "suicides" );

    if ( !maps\mp\_utility::matchmakinggame() )
        maps\mp\_utility::incplayerstat( "suicides", 1 );

    var_2 = maps\mp\gametypes\_tweakables::gettweakablevalue( "game", "suicidepointloss" );
    maps\mp\gametypes\_gamescore::_setplayerscore( self, maps\mp\gametypes\_gamescore::_getplayerscore( self ) - var_2 );

    if ( var_0 == "MOD_SUICIDE" && var_1 == "none" && isdefined( self.throwinggrenade ) )
        self.lastgrenadesuicidetime = gettime();

    if ( isdefined( level.onsuicidedeath ) )
        [[ level.onsuicidedeath ]]( self );

    if ( isdefined( self.friendlydamage ) )
        self iprintlnbold( &"MP_FRIENDLY_FIRE_WILL_NOT" );
}

handlefriendlyfiredeath( var_0 )
{
    var_0 thread [[ level.onxpevent ]]( "teamkill" );
    var_0.pers["teamkills"] += 1.0;
    var_0.teamkillsthisround++;

    if ( maps\mp\gametypes\_tweakables::gettweakablevalue( "team", "teamkillpointloss" ) )
    {
        var_1 = maps\mp\gametypes\_rank::getscoreinfovalue( "kill" );
        maps\mp\gametypes\_gamescore::_setplayerscore( var_0, maps\mp\gametypes\_gamescore::_getplayerscore( var_0 ) - var_1 );
    }

    if ( level.maxallowedteamkills < 0 )
        return;

    if ( level.ingraceperiod )
    {
        var_2 = 1;
        var_0.pers["teamkills"] += level.maxallowedteamkills;
    }
    else if ( var_0.pers["teamkills"] > 1 && maps\mp\_utility::gettimepassed() < level.graceperiod * 1000 + 8000 + var_0.pers["teamkills"] * 1000 )
    {
        var_2 = 1;
        var_0.pers["teamkills"] += level.maxallowedteamkills;
    }
    else
        var_2 = var_0 maps\mp\gametypes\_playerlogic::teamkilldelay();

    if ( var_2 > 0 )
    {
        var_0.pers["teamKillPunish"] = 1;
        var_0 maps\mp\_utility::_suicide();
    }
}

handlenormaldeath( var_0, var_1, var_2, var_3, var_4 )
{
    var_1 thread maps\mp\_events::killedplayer( var_0, self, var_3, var_4 );

    if ( var_4 == "MOD_HEAD_SHOT" )
    {
        var_1 maps\mp\_utility::incpersstat( "headshots", 1 );
        var_1.headshots = var_1 maps\mp\_utility::getpersstat( "headshots" );
        var_1 maps\mp\_utility::incplayerstat( "headshots", 1 );

        if ( isdefined( var_1.laststand ) )
            var_5 = maps\mp\gametypes\_rank::getscoreinfovalue( "kill" ) * 2;
        else
            var_5 = undefined;

        var_1 playlocalsound( "bullet_impact_headshot_plr" );
        self playsound( "bullet_impact_headshot" );
    }
    else if ( isdefined( var_1.laststand ) )
        var_5 = maps\mp\gametypes\_rank::getscoreinfovalue( "kill" ) * 2;
    else
        var_5 = undefined;

    var_6 = var_1;

    if ( isdefined( var_1.commanding_bot ) )
        var_6 = var_1.commanding_bot;

    var_7 = 0;

    if ( issquadsmode() )
        var_7 = 1;

    var_6 maps\mp\_utility::incpersstat( "kills", 1, var_7 );
    var_6.kills = var_6 maps\mp\_utility::getpersstat( "kills" );
    var_6 maps\mp\_utility::updatepersratio( "kdRatio", "kills", "deaths" );
    var_6 maps\mp\gametypes\_persistence::statsetchild( "round", "kills", var_6.kills );
    var_6 maps\mp\_utility::incplayerstat( "kills", 1 );

    if ( isflankkill( self, var_1 ) )
    {
        var_6 maps\mp\_utility::incplayerstat( "flankkills", 1 );
        maps\mp\_utility::incplayerstat( "flankdeaths", 1 );
    }

    var_8 = var_1.pers["cur_kill_streak"];

    if ( isalive( var_1 ) || var_1.streaktype == "support" )
    {
        if ( var_4 == "MOD_MELEE" && !var_1 maps\mp\_utility::isjuggernaut() || var_1 maps\mp\_utility::killshouldaddtokillstreak( var_3 ) )
            var_1 registerkill( var_3, 1 );

        var_1 maps\mp\_utility::setplayerstatifgreater( "killstreak", var_1.pers["cur_kill_streak"] );

        if ( var_1.pers["cur_kill_streak"] > var_1 maps\mp\_utility::getpersstat( "longestStreak" ) )
            var_1 maps\mp\_utility::setpersstat( "longestStreak", var_1.pers["cur_kill_streak"] );
    }

    var_1.pers["cur_death_streak"] = 0;
    var_1 thread maps\mp\gametypes\_rank::giverankxp( "kill", var_5, var_3, var_4, undefined, self );

    if ( var_1.pers["cur_kill_streak"] > var_1 maps\mp\gametypes\_persistence::statgetchild( "round", "killStreak" ) )
        var_1 maps\mp\gametypes\_persistence::statsetchild( "round", "killStreak", var_1.pers["cur_kill_streak"] );

    if ( var_1 maps\mp\_utility::rankingenabled() )
    {
        if ( var_1.pers["cur_kill_streak"] > var_1.kill_streak )
        {
            if ( !issquadsmode() )
                var_1 maps\mp\gametypes\_persistence::statset( "killStreak", var_1.pers["cur_kill_streak"] );

            var_1.kill_streak = var_1.pers["cur_kill_streak"];
        }
    }

    maps\mp\gametypes\_gamescore::giveplayerscore( "kill", var_1, self );
    var_9 = maps\mp\gametypes\_tweakables::gettweakablevalue( "game", "deathpointloss" );
    maps\mp\gametypes\_gamescore::_setplayerscore( self, maps\mp\gametypes\_gamescore::_getplayerscore( self ) - var_9 );

    if ( isdefined( level.ac130player ) && level.ac130player == var_1 )
        level notify( "ai_killed", self );

    if ( isdefined( var_1.odin ) )
        level notify( "odin_killed_player", self );

    level notify( "player_got_killstreak_" + var_1.pers["cur_kill_streak"], var_1 );
    var_1 notify( "got_killstreak", var_1.pers["cur_kill_streak"] );
    var_1 notify( "killed_enemy", self, var_3, var_4 );

    if ( isdefined( self.motionsensormarkedby ) )
    {
        if ( self.motionsensormarkedby != var_1 )
            self.motionsensormarkedby thread maps\mp\gametypes\_weapons::motionsensor_processtaggedassist( self );

        self.motionsensormarkedby = undefined;
    }

    if ( isdefined( level.onnormaldeath ) && var_1.pers["team"] != "spectator" )
        [[ level.onnormaldeath ]]( self, var_1, var_0 );

    if ( !level.teambased )
    {
        self.attackers = [];
        return;
    }

    level thread maps\mp\gametypes\_battlechatter_mp::saylocalsounddelayed( var_1, "kill", 0.75 );

    if ( isdefined( self.lastattackedshieldplayer ) && isdefined( self.lastattackedshieldtime ) && self.lastattackedshieldplayer != var_1 )
    {
        if ( gettime() - self.lastattackedshieldtime < 2500 )
        {
            self.lastattackedshieldplayer thread maps\mp\gametypes\_gamescore::processshieldassist( self );

            if ( self.lastattackedshieldplayer maps\mp\_utility::_hasperk( "specialty_assists" ) )
            {
                self.lastattackedshieldplayer.pers["assistsToKill"]++;

                if ( !( self.lastattackedshieldplayer.pers["assistsToKill"] % 2 ) )
                {
                    self.lastattackedshieldplayer maps\mp\gametypes\_missions::processchallenge( "ch_hardlineassists" );
                    self.lastattackedshieldplayer maps\mp\killstreaks\_killstreaks::giveadrenaline( "kill" );
                    self.lastattackedshieldplayer.pers["cur_kill_streak"]++;
                }
            }
            else
                self.lastattackedshieldplayer.pers["assistsToKill"] = 0;
        }
        else if ( isalive( self.lastattackedshieldplayer ) && gettime() - self.lastattackedshieldtime < 5000 )
        {
            var_10 = vectornormalize( anglestoforward( self.angles ) );
            var_11 = vectornormalize( self.lastattackedshieldplayer.origin - self.origin );

            if ( vectordot( var_11, var_10 ) > 0.925 )
            {
                self.lastattackedshieldplayer thread maps\mp\gametypes\_gamescore::processshieldassist( self );

                if ( self.lastattackedshieldplayer maps\mp\_utility::_hasperk( "specialty_assists" ) )
                {
                    self.lastattackedshieldplayer.pers["assistsToKill"]++;

                    if ( !( self.lastattackedshieldplayer.pers["assistsToKill"] % 2 ) )
                    {
                        self.lastattackedshieldplayer maps\mp\gametypes\_missions::processchallenge( "ch_hardlineassists" );
                        self.lastattackedshieldplayer maps\mp\killstreaks\_killstreaks::giveadrenaline( "kill" );
                        self.lastattackedshieldplayer.pers["cur_kill_streak"]++;
                    }
                }
                else
                    self.lastattackedshieldplayer.pers["assistsToKill"] = 0;
            }
        }
    }

    if ( isdefined( self.attackers ) )
    {
        foreach ( var_13 in self.attackers )
        {
            if ( !isdefined( maps\mp\_utility::_validateattacker( var_13 ) ) )
                continue;

            if ( var_13 == var_1 )
                continue;

            if ( self == var_13 )
                continue;

            if ( isdefined( level.assists_disabled ) )
                continue;

            var_13 thread maps\mp\gametypes\_gamescore::processassist( self );

            if ( var_13 maps\mp\_utility::_hasperk( "specialty_assists" ) )
            {
                var_13.pers["assistsToKill"]++;

                if ( !( var_13.pers["assistsToKill"] % 2 ) )
                {
                    var_13 maps\mp\gametypes\_missions::processchallenge( "ch_hardlineassists" );
                    var_13 registerkill( var_3, 0 );
                }

                continue;
            }

            var_13.pers["assistsToKill"] = 0;
        }

        self.attackers = [];
    }
}

isplayerweapon( var_0 )
{
    if ( weaponclass( var_0 ) == "non-player" )
        return 0;

    if ( weaponclass( var_0 ) == "turret" )
        return 0;

    if ( weaponinventorytype( var_0 ) == "primary" || weaponinventorytype( var_0 ) == "altmode" )
        return 1;

    return 0;
}

waitskipkillcambuttonduringdeathtimer()
{
    self endon( "disconnect" );
    self endon( "killcam_death_done_waiting" );
    self notifyonplayercommand( "death_respawn", "+usereload" );
    self notifyonplayercommand( "death_respawn", "+activate" );
    self waittill( "death_respawn" );
    self notify( "killcam_death_button_cancel" );
}

waitskipkillcamduringdeathtimer( var_0 )
{
    self endon( "disconnect" );
    self endon( "killcam_death_button_cancel" );
    wait(var_0);
    self notify( "killcam_death_done_waiting" );
}

skipkillcamduringdeathtimer( var_0 )
{
    self endon( "disconnect" );

    if ( level.showingfinalkillcam )
        return 0;

    if ( !isai( self ) )
    {
        thread waitskipkillcambuttonduringdeathtimer();
        thread waitskipkillcamduringdeathtimer( var_0 );
        var_1 = common_scripts\utility::waittill_any_return( "killcam_death_done_waiting", "killcam_death_button_cancel" );

        if ( var_1 == "killcam_death_done_waiting" )
            return 0;
        else
            return 1;
    }

    return 0;
}

callback_playerkilled( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8 )
{
    playerkilled_internal( var_0, var_1, self, var_2, var_3, var_4, var_5, var_6, var_7, var_8, 0 );
}

queueshieldforremoval( var_0 )
{
    var_1 = 5;

    if ( !isdefined( level.shieldtrasharray ) )
        level.shieldtrasharray = [];

    if ( level.shieldtrasharray.size >= var_1 )
    {
        var_2 = level.shieldtrasharray.size - 1;
        level.shieldtrasharray[0] delete();

        for ( var_3 = 0; var_3 < var_2; var_3++ )
            level.shieldtrasharray[var_3] = level.shieldtrasharray[var_3 + 1];

        level.shieldtrasharray[var_2] = undefined;
    }

    level.shieldtrasharray[level.shieldtrasharray.size] = var_0;
}

launchshield( var_0, var_1 )
{
    if ( isdefined( self.hasriotshieldequipped ) && self.hasriotshieldequipped )
    {
        if ( isdefined( self.riotshieldmodel ) )
            maps\mp\_utility::riotshield_detach( 1 );
        else if ( isdefined( self.riotshieldmodelstowed ) )
            maps\mp\_utility::riotshield_detach( 0 );
    }
}

playerkilled_internal( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 )
{
    var_2 endon( "spawned" );
    var_2 notify( "killed_player" );
    var_2 maps\mp\gametypes\_playerlogic::resetuidvarsondeath();
    var_2.abilitychosen = 0;
    var_2.perkoutlined = 0;

    if ( maps\mp\_utility::gamehasneutralcrateowner( level.gametype ) )
    {
        if ( var_2 != var_1 && var_4 == "MOD_CRUSH" )
        {
            var_0 = var_2;
            var_1 = var_2;
            var_4 = "MOD_SUICIDE";
            var_5 = "none";
            var_7 = "none";
            var_2.attackers = [];
        }
    }

    var_1 = maps\mp\_utility::_validateattacker( var_1 );

    if ( isdefined( var_1 ) )
        var_1.assistedsuicide = undefined;

    if ( !isdefined( var_2.idflags ) )
    {
        if ( var_4 == "MOD_SUICIDE" )
            var_2.idflags = 0;
        else if ( var_4 == "MOD_GRENADE" )
        {
            if ( ( issubstr( var_5, "frag_grenade" ) || issubstr( var_5, "thermobaric_grenade" ) || issubstr( var_5, "mortar_shell" ) ) && var_3 == 100000 )
                var_2.idflags = 0;
            else if ( var_5 == "nuke_mp" )
                var_2.idflags = 0;
            else if ( level.friendlyfire >= 2 )
                var_2.idflags = 0;
            else
            {

            }
        }
    }

    if ( isdefined( var_2.hasriotshieldequipped ) && var_2.hasriotshieldequipped )
        var_2 launchshield( var_3, var_4 );

    var_2 maps\mp\_utility::riotshield_clear();
    maps\mp\gametypes\_weapons::recordtogglescopestates();

    if ( !var_10 )
    {
        if ( isdefined( var_2.endgame ) )
            maps\mp\_utility::restorebasevisionset( 2 );
        else
        {
            maps\mp\_utility::restorebasevisionset( 0 );
            var_2 thermalvisionoff();
        }
    }
    else
    {
        var_2.fauxdead = 1;
        self notify( "death" );
    }

    if ( game["state"] == "postgame" )
        return;

    maps\mp\perks\_perks::updateactiveperks( var_0, var_1, var_2, var_3, var_4 );
    var_11 = 0;

    if ( !isplayer( var_0 ) && isdefined( var_0.primaryweapon ) )
        var_12 = var_0.primaryweapon;
    else if ( isdefined( var_1 ) && isplayer( var_1 ) && var_1 getcurrentprimaryweapon() != "none" )
        var_12 = var_1 getcurrentprimaryweapon();
    else if ( issubstr( var_5, "alt_" ) )
        var_12 = getsubstr( var_5, 4, var_5.size );
    else
        var_12 = undefined;

    if ( isdefined( var_2.uselaststandparams ) || isdefined( var_2.laststandparams ) && var_4 == "MOD_SUICIDE" )
    {
        var_2 ensurelaststandparamsvalidity();
        var_2.uselaststandparams = undefined;
        var_0 = var_2.laststandparams.einflictor;
        var_1 = var_2.laststandparams.attacker;
        var_3 = var_2.laststandparams.idamage;
        var_4 = var_2.laststandparams.smeansofdeath;
        var_5 = var_2.laststandparams.sweapon;
        var_12 = var_2.laststandparams.sprimaryweapon;
        var_6 = var_2.laststandparams.vdir;
        var_7 = var_2.laststandparams.shitloc;
        var_11 = ( gettime() - var_2.laststandparams.laststandstarttime ) / 1000;
        var_2.laststandparams = undefined;
        var_1 = maps\mp\_utility::_validateattacker( var_1 );
    }

    if ( ( !isdefined( var_1 ) || var_1.classname == "trigger_hurt" || var_1.classname == "worldspawn" || var_1 == var_2 ) && isdefined( self.attackers ) )
    {
        var_13 = undefined;

        foreach ( var_15 in self.attackers )
        {
            if ( !isdefined( maps\mp\_utility::_validateattacker( var_15 ) ) )
                continue;

            if ( !isdefined( var_2.attackerdata[var_15.guid].damage ) )
                continue;

            if ( var_15 == var_2 || level.teambased && var_15.team == var_2.team )
                continue;

            if ( var_2.attackerdata[var_15.guid].lasttimedamaged + 2500 < gettime() && ( var_1 != var_2 && ( isdefined( var_2.laststand ) && var_2.laststand ) ) )
                continue;

            if ( var_2.attackerdata[var_15.guid].damage > 1 && !isdefined( var_13 ) )
            {
                var_13 = var_15;
                continue;
            }

            if ( isdefined( var_13 ) && var_2.attackerdata[var_15.guid].damage > var_2.attackerdata[var_13.guid].damage )
                var_13 = var_15;
        }

        if ( isdefined( var_13 ) )
        {
            var_1 = var_13;
            var_1.assistedsuicide = 1;
            var_5 = var_2.attackerdata[var_13.guid].weapon;
            var_6 = var_2.attackerdata[var_13.guid].vdir;
            var_7 = var_2.attackerdata[var_13.guid].shitloc;
            var_8 = var_2.attackerdata[var_13.guid].psoffsettime;
            var_4 = var_2.attackerdata[var_13.guid].smeansofdeath;
            var_3 = var_2.attackerdata[var_13.guid].damage;
            var_12 = var_2.attackerdata[var_13.guid].sprimaryweapon;
            var_0 = var_1;
        }
    }
    else if ( isdefined( var_1 ) )
        var_1.assistedsuicide = undefined;

    if ( maps\mp\_utility::isheadshot( var_5, var_7, var_4, var_1 ) )
        var_4 = "MOD_HEAD_SHOT";
    else if ( !isdefined( var_2.nuked ) )
    {
        if ( isdefined( level.custom_death_sound ) )
            [[ level.custom_death_sound ]]( var_2, var_4, var_0 );
        else if ( var_4 != "MOD_MELEE" )
            var_2 maps\mp\_utility::playdeathsound();
    }

    if ( isdefined( level.custom_death_effect ) )
        [[ level.custom_death_effect ]]( var_2, var_4, var_0 );

    var_17 = isfriendlyfire( var_2, var_1 );

    if ( isdefined( var_1 ) )
    {
        if ( var_1.code_classname == "script_vehicle" && isdefined( var_1.owner ) )
            var_1 = var_1.owner;

        if ( var_1.code_classname == "misc_turret" && isdefined( var_1.owner ) )
        {
            if ( isdefined( var_1.vehicle ) )
                var_1.vehicle notify( "killedPlayer", var_2 );

            var_1 = var_1.owner;
        }

        if ( isagent( var_1 ) )
        {
            var_5 = "agent_mp";
            var_4 = "MOD_RIFLE_BULLET";

            if ( isdefined( var_1.agent_type ) )
            {
                if ( var_1.agent_type == "dog" )
                    var_5 = "guard_dog_mp";
                else if ( var_1.agent_type == "squadmate" )
                    var_5 = "agent_support_mp";
                else if ( var_1.agent_type == "pirate" )
                    var_5 = "pirate_agent_mp";
                else if ( var_1.agent_type == "wolf" )
                    var_5 = "killstreak_wolfpack_mp";
                else if ( var_1.agent_type == "beastmen" )
                    var_5 = "beast_agent_mp";
            }

            if ( isdefined( var_1.owner ) )
                var_1 = var_1.owner;
        }

        if ( var_1.code_classname == "script_model" && isdefined( var_1.owner ) )
        {
            var_1 = var_1.owner;

            if ( !isfriendlyfire( var_2, var_1 ) && var_1 != var_2 )
                var_1 notify( "crushed_enemy" );
        }
    }

    if ( var_4 != "MOD_SUICIDE" && ( maps\mp\_utility::isaigameparticipant( var_2 ) || maps\mp\_utility::isaigameparticipant( var_1 ) ) && isdefined( level.bot_funcs ) && isdefined( level.bot_funcs["get_attacker_ent"] ) )
    {
        var_18 = [[ level.bot_funcs["get_attacker_ent"] ]]( var_1, var_0 );

        if ( isdefined( var_18 ) )
        {
            if ( maps\mp\_utility::isaigameparticipant( var_2 ) )
                var_2 botmemoryevent( "death", var_5, var_18.origin, var_2.origin, var_18 );

            if ( maps\mp\_utility::isaigameparticipant( var_1 ) )
            {
                var_19 = 1;

                if ( var_18.classname == "script_vehicle" && isdefined( var_18.helitype ) || var_18.classname == "rocket" || var_18.classname == "misc_turret" )
                    var_19 = 0;

                if ( var_19 )
                    var_1 botmemoryevent( "kill", var_5, var_18.origin, var_2.origin, var_2 );
            }
        }
    }

    var_2 maps\mp\gametypes\_weapons::dropscavengerfordeath( var_1 );
    var_2 [[ level.weapondropfunction ]]( var_1, var_4 );

    if ( !var_10 )
        var_2 maps\mp\_utility::updatesessionstate( "dead", "hud_status_dead" );

    var_20 = isdefined( var_2.fauxdead ) && var_2.fauxdead && isdefined( var_2.switching_teams ) && var_2.switching_teams;

    if ( !var_20 )
        var_2 maps\mp\gametypes\_playerlogic::removefromalivecount();

    if ( !isdefined( var_2.switching_teams ) )
    {
        var_21 = var_2;

        if ( isdefined( var_2.commanding_bot ) )
            var_21 = var_2.commanding_bot;

        if ( isdefined( level.ishorde ) )
            var_21.deaths++;
        else
        {
            var_22 = 0;

            if ( issquadsmode() )
                var_22 = 1;

            var_21 maps\mp\_utility::incpersstat( "deaths", 1, var_22 );
            var_21.deaths = var_21 maps\mp\_utility::getpersstat( "deaths" );
            var_21 maps\mp\_utility::updatepersratio( "kdRatio", "kills", "deaths" );
            var_21 maps\mp\gametypes\_persistence::statsetchild( "round", "deaths", var_21.deaths );
            var_21 maps\mp\_utility::incplayerstat( "deaths", 1 );
        }
    }

    if ( isdefined( var_1 ) && isplayer( var_1 ) )
        var_1 checkkillsteal( var_2 );

    obituary( var_2, var_1, var_5, var_4 );
    var_23 = 0;
    var_24 = var_2 maps\mp\_matchdata::logplayerlife();
    var_2 logprintplayerdeath( var_24, var_1, var_3, var_4, var_5, var_12, var_7 );
    var_2 maps\mp\_matchdata::logplayerdeath( var_24, var_1, var_3, var_4, var_5, var_12, var_7 );

    if ( isplayer( var_1 ) )
    {
        if ( var_4 == "MOD_MELEE" )
        {
            if ( maps\mp\gametypes\_weapons::isriotshield( var_5 ) )
            {
                var_1 maps\mp\_utility::incplayerstat( "shieldkills", 1 );

                if ( !maps\mp\_utility::matchmakinggame() )
                    var_2 maps\mp\_utility::incplayerstat( "shielddeaths", 1 );
            }
            else
                var_1 maps\mp\_utility::incplayerstat( "knifekills", 1 );

            addattacker( var_2, var_1, var_0, var_5, var_3, ( 0, 0, 0 ), var_6, var_7, var_8, var_4 );
        }
    }

    if ( var_2 isswitchingteams() )
        handleteamchangedeath();
    else if ( !isplayer( var_1 ) || isplayer( var_1 ) && var_4 == "MOD_FALLING" )
    {
        handleworlddeath( var_1, var_24, var_4, var_7 );

        if ( isagent( var_1 ) )
            var_23 = 1;
    }
    else if ( var_1 == var_2 )
        handlesuicidedeath( var_4, var_7 );
    else if ( var_17 )
    {
        if ( !( isdefined( var_2.nuked ) || var_5 == "bomb_site_mp" ) )
            handlefriendlyfiredeath( var_1 );
    }
    else
    {
        if ( var_4 == "MOD_GRENADE" && var_0 == var_1 )
            addattacker( var_2, var_1, var_0, var_5, var_3, ( 0, 0, 0 ), var_6, var_7, var_8, var_4 );

        var_23 = 1;

        if ( isai( var_2 ) && isdefined( level.bot_funcs ) && isdefined( level.bot_funcs["should_do_killcam"] ) )
            var_23 = var_2 [[ level.bot_funcs["should_do_killcam"] ]]();

        if ( isdefined( level.disable_killcam ) && level.disable_killcam )
            var_23 = 0;

        handlenormaldeath( var_24, var_1, var_0, var_5, var_4 );
        var_2 thread maps\mp\gametypes\_missions::playerkilled( var_0, var_1, var_3, var_4, var_5, var_12, var_7, var_1.modifiers );
        var_2.pers["cur_death_streak"]++;

        if ( isplayer( var_1 ) && var_2 maps\mp\_utility::isjuggernaut() )
        {
            if ( isdefined( var_2.isjuggernautmaniac ) && var_2.isjuggernautmaniac )
            {
                var_1 thread maps\mp\_utility::teamplayercardsplash( "callout_killed_maniac", var_1 );

                if ( var_4 == "MOD_MELEE" )
                    var_1 maps\mp\gametypes\_missions::processchallenge( "ch_thisisaknife" );
            }
            else if ( isdefined( var_2.isjuggernautlevelcustom ) && var_2.isjuggernautlevelcustom )
                var_1 thread maps\mp\_utility::teamplayercardsplash( level.mapcustomjuggkilledsplash, var_1 );
            else
                var_1 thread maps\mp\_utility::teamplayercardsplash( "callout_killed_juggernaut", var_1 );
        }
    }

    var_25 = 0;
    var_26 = undefined;

    if ( isdefined( self.previousprimary ) )
    {
        var_25 = 1;
        var_26 = self.previousprimary;
        self.previousprimary = undefined;
    }

    if ( isplayer( var_1 ) && var_1 != self && ( !level.teambased || level.teambased && self.team != var_1.team ) )
    {
        if ( var_25 && isdefined( var_26 ) )
            var_27 = var_26;
        else
            var_27 = self.lastdroppableweapon;

        var_27 = maps\mp\_utility::weaponmap( var_27 );
        thread maps\mp\gametypes\_gamelogic::trackleaderboarddeathstats( var_27, var_4 );
        var_1 thread maps\mp\gametypes\_gamelogic::trackattackerleaderboarddeathstats( var_5, var_4 );
    }

    if ( isdefined( var_1 ) && isdefined( var_2 ) )
        bbprint( "kills", "attackername %s attackerteam %s attackerx %f attackery %f attackerz %f attackerweapon %s victimx %f victimy %f victimz %f victimname %s victimteam %s damage %i damagetype %s damagelocation %s attackerisbot %i victimisbot %i timesincespawn %f", var_1.name, var_1.team, var_1.origin[0], var_1.origin[1], var_1.origin[2], var_5, var_2.origin[0], var_2.origin[1], var_2.origin[2], var_2.name, var_2.team, var_3, var_4, var_7, isai( var_1 ), isai( var_2 ), ( gettime() - var_2.lastspawntime ) / 1000 );

    var_2.wasswitchingteamsforonplayerkilled = undefined;

    if ( isdefined( var_2.switching_teams ) )
        var_2.wasswitchingteamsforonplayerkilled = 1;

    var_2 resetplayervariables();
    var_2.lastattacker = var_1;
    var_2.lastdeathpos = var_2.origin;
    var_2.deathtime = gettime();
    var_2.wantsafespawn = 0;
    var_2.revived = 0;
    var_2.sameshotdamage = 0;

    if ( maps\mp\killstreaks\_killstreaks::streaktyperesetsondeath( var_2.streaktype ) )
        var_2 maps\mp\killstreaks\_killstreaks::resetadrenaline();

    var_28 = undefined;

    if ( maps\mp\_utility::isrocketcorpse() )
    {
        var_23 = 1;
        var_10 = 0;
        var_28 = self.killcament;
        self waittill( "final_rocket_corpse_death" );
    }
    else
    {
        if ( var_10 )
        {
            var_23 = 0;
            var_9 = var_2 playerforcedeathanim( var_0, var_4, var_5, var_7, var_6 );
        }

        var_2.body = var_2 cloneplayer( var_9 );
        var_2.body.targetname = "player_corpse";

        if ( var_10 )
            var_2 playerhide();

        if ( var_2 isonladder() || var_2 ismantling() || !var_2 isonground() || isdefined( var_2.nuked ) || isdefined( var_2.customdeath ) )
        {
            var_29 = 0;

            if ( var_4 == "MOD_MELEE" )
            {
                if ( isdefined( var_2.isplanting ) && var_2.isplanting || isdefined( var_2.nuked ) )
                    var_29 = 1;
            }

            if ( !var_29 )
            {
                var_2.body startragdoll();
                var_2 notify( "start_instant_ragdoll", var_4, var_0 );
            }
        }

        if ( !isdefined( var_2.switching_teams ) )
        {
            if ( isdefined( var_1 ) && isplayer( var_1 ) && !var_1 maps\mp\_utility::_hasperk( "specialty_silentkill" ) )
                thread maps\mp\gametypes\_deathicons::adddeathicon( var_2.body, var_2, var_2.team, 5.0 );
        }

        thread delaystartragdoll( var_2.body, var_7, var_6, var_5, var_0, var_4 );
    }

    var_2 thread [[ level.onplayerkilled ]]( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_24 );

    if ( isai( var_2 ) && isdefined( level.bot_funcs ) && isdefined( level.bot_funcs["on_killed"] ) )
        var_2 thread [[ level.bot_funcs["on_killed"] ]]( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_24 );

    if ( maps\mp\_utility::isgameparticipant( var_1 ) )
        var_30 = var_1 getentitynumber();
    else
        var_30 = -1;

    if ( !isdefined( var_28 ) )
        var_28 = var_2 getkillcamentity( var_1, var_0, var_5 );

    var_31 = -1;
    var_32 = 0;

    if ( isdefined( var_28 ) )
    {
        var_31 = var_28 getentitynumber();
        var_32 = var_28.birthtime;

        if ( !isdefined( var_32 ) )
            var_32 = 0;
    }

    if ( ( !isdefined( level.disable_killcam ) || !level.disable_killcam ) && var_4 != "MOD_SUICIDE" && !( !isdefined( var_1 ) || var_1.classname == "trigger_hurt" || var_1.classname == "worldspawn" || var_1 == var_2 ) )
        recordfinalkillcam( 5.0, var_2, var_1, var_30, var_0, var_31, var_32, var_5, var_11, var_8, var_4 );

    var_2 setcommonplayerdata( "killCamHowKilled", 0 );

    switch ( var_4 )
    {
        case "MOD_HEAD_SHOT":
            var_2 setcommonplayerdata( "killCamHowKilled", 1 );
            break;
        default:
            break;
    }

    var_33 = undefined;

    if ( var_23 )
    {
        var_2 maps\mp\gametypes\_killcam::prekillcamnotify( var_0, var_1 );

        if ( isdefined( var_0 ) && isagent( var_0 ) )
        {
            var_33 = spawnstruct();
            var_33.agent_type = var_0.agent_type;
            var_33.lastspawntime = var_0.lastspawntime;
        }
    }

    if ( !var_10 )
    {
        self.respawntimerstarttime = gettime() + 1000;
        var_34 = maps\mp\gametypes\_playerlogic::timeuntilspawn( 1 );

        if ( var_34 < 1 )
            var_34 = 1;

        var_2 thread maps\mp\gametypes\_playerlogic::predictabouttospawnplayerovertime( var_34 );
        wait 1.0;

        if ( var_23 )
            var_23 = !skipkillcamduringdeathtimer( 0.5 );

        var_2 notify( "death_delay_finished" );
    }

    var_35 = ( gettime() - var_2.deathtime ) / 1000;
    self.respawntimerstarttime = gettime();
    var_23 = var_23 && !var_2 maps\mp\gametypes\_battlebuddy::canbuddyspawn();

    if ( !( isdefined( var_2.cancelkillcam ) && var_2.cancelkillcam ) && var_23 && level.killcam && game["state"] == "playing" && !var_2 maps\mp\_utility::isusingremote() && !level.showingfinalkillcam )
    {
        var_36 = !( maps\mp\_utility::getgametypenumlives() && !var_2.pers["lives"] );
        var_34 = maps\mp\gametypes\_playerlogic::timeuntilspawn( 1 );
        var_37 = var_36 && var_34 <= 0;

        if ( !var_36 )
        {
            var_34 = -1;
            level notify( "player_eliminated", var_2 );
        }

        var_2 maps\mp\gametypes\_killcam::killcam( var_0, var_33, var_30, var_31, var_32, var_5, var_35 + var_11, var_8, var_34, maps\mp\gametypes\_gamelogic::timeuntilroundend(), var_1, var_2, var_4 );
    }

    if ( game["state"] != "playing" )
    {
        if ( !level.showingfinalkillcam )
        {
            var_2 maps\mp\_utility::updatesessionstate( "dead" );
            var_2 maps\mp\_utility::clearkillcamstate();
        }

        return;
    }

    var_38 = maps\mp\_utility::getgametypenumlives();
    var_39 = self.pers["lives"];

    if ( self == var_2 && isdefined( var_2.battlebuddy ) && maps\mp\_utility::isreallyalive( var_2.battlebuddy ) && ( !maps\mp\_utility::getgametypenumlives() || self.pers["lives"] ) && !var_2 maps\mp\_utility::isusingremote() )
        maps\mp\gametypes\_battlebuddy::waitforplayerrespawnchoice();

    if ( maps\mp\_utility::isvalidclass( var_2.class ) )
        var_2 thread maps\mp\gametypes\_playerlogic::spawnclient();
}

checkforcebleedout()
{
    if ( level.diehardmode != 1 )
        return 0;

    if ( !maps\mp\_utility::getgametypenumlives() )
        return 0;

    if ( level.livescount[self.team] > 0 )
        return 0;

    foreach ( var_1 in level.players )
    {
        if ( !isalive( var_1 ) )
            continue;

        if ( var_1.team != self.team )
            continue;

        if ( var_1 == self )
            continue;

        if ( !var_1.inlaststand )
            return 0;
    }

    foreach ( var_1 in level.players )
    {
        if ( !isalive( var_1 ) )
            continue;

        if ( var_1.team != self.team )
            continue;

        if ( var_1.inlaststand && var_1 != self )
            var_1 laststandbleedout( 0 );
    }

    return 1;
}

checkkillsteal( var_0 )
{
    if ( maps\mp\_utility::matchmakinggame() )
        return;

    var_1 = 0;
    var_2 = undefined;

    if ( isdefined( var_0.attackerdata ) && var_0.attackerdata.size > 1 )
    {
        foreach ( var_4 in var_0.attackerdata )
        {
            if ( var_4.damage > var_1 )
            {
                var_1 = var_4.damage;
                var_2 = var_4.attackerent;
            }
        }

        if ( isdefined( var_2 ) && var_2 != self )
            maps\mp\_utility::incplayerstat( "killsteals", 1 );
    }
}

initfinalkillcam()
{
    level.finalkillcam_delay = [];
    level.finalkillcam_victim = [];
    level.finalkillcam_attacker = [];
    level.finalkillcam_attackernum = [];
    level.finalkillcam_inflictor = [];
    level.finalkillcam_inflictor_agent_type = [];
    level.finalkillcam_inflictor_lastspawntime = [];
    level.finalkillcam_killcamentityindex = [];
    level.finalkillcam_killcamentitystarttime = [];
    level.finalkillcam_sweapon = [];
    level.finalkillcam_deathtimeoffset = [];
    level.finalkillcam_psoffsettime = [];
    level.finalkillcam_timerecorded = [];
    level.finalkillcam_timegameended = [];
    level.finalkillcam_smeansofdeath = [];

    if ( level.multiteambased )
    {
        foreach ( var_1 in level.teamnamelist )
        {
            level.finalkillcam_delay[var_1] = undefined;
            level.finalkillcam_victim[var_1] = undefined;
            level.finalkillcam_attacker[var_1] = undefined;
            level.finalkillcam_attackernum[var_1] = undefined;
            level.finalkillcam_inflictor[var_1] = undefined;
            level.finalkillcam_inflictor_agent_type[var_1] = undefined;
            level.finalkillcam_inflictor_lastspawntime[var_1] = undefined;
            level.finalkillcam_killcamentityindex[var_1] = undefined;
            level.finalkillcam_killcamentitystarttime[var_1] = undefined;
            level.finalkillcam_sweapon[var_1] = undefined;
            level.finalkillcam_deathtimeoffset[var_1] = undefined;
            level.finalkillcam_psoffsettime[var_1] = undefined;
            level.finalkillcam_timerecorded[var_1] = undefined;
            level.finalkillcam_timegameended[var_1] = undefined;
            level.finalkillcam_smeansofdeath[var_1] = undefined;
        }
    }
    else
    {
        level.finalkillcam_delay["axis"] = undefined;
        level.finalkillcam_victim["axis"] = undefined;
        level.finalkillcam_attacker["axis"] = undefined;
        level.finalkillcam_attackernum["axis"] = undefined;
        level.finalkillcam_inflictor["axis"] = undefined;
        level.finalkillcam_inflictor_agent_type["axis"] = undefined;
        level.finalkillcam_inflictor_lastspawntime["axis"] = undefined;
        level.finalkillcam_killcamentityindex["axis"] = undefined;
        level.finalkillcam_killcamentitystarttime["axis"] = undefined;
        level.finalkillcam_sweapon["axis"] = undefined;
        level.finalkillcam_deathtimeoffset["axis"] = undefined;
        level.finalkillcam_psoffsettime["axis"] = undefined;
        level.finalkillcam_timerecorded["axis"] = undefined;
        level.finalkillcam_timegameended["axis"] = undefined;
        level.finalkillcam_smeansofdeath["axis"] = undefined;
        level.finalkillcam_delay["allies"] = undefined;
        level.finalkillcam_victim["allies"] = undefined;
        level.finalkillcam_attacker["allies"] = undefined;
        level.finalkillcam_attackernum["allies"] = undefined;
        level.finalkillcam_inflictor["allies"] = undefined;
        level.finalkillcam_inflictor_agent_type["allies"] = undefined;
        level.finalkillcam_inflictor_lastspawntime["allies"] = undefined;
        level.finalkillcam_killcamentityindex["allies"] = undefined;
        level.finalkillcam_killcamentitystarttime["allies"] = undefined;
        level.finalkillcam_sweapon["allies"] = undefined;
        level.finalkillcam_deathtimeoffset["allies"] = undefined;
        level.finalkillcam_psoffsettime["allies"] = undefined;
        level.finalkillcam_timerecorded["allies"] = undefined;
        level.finalkillcam_timegameended["allies"] = undefined;
        level.finalkillcam_smeansofdeath["allies"] = undefined;
    }

    level.finalkillcam_delay["none"] = undefined;
    level.finalkillcam_victim["none"] = undefined;
    level.finalkillcam_attacker["none"] = undefined;
    level.finalkillcam_attackernum["none"] = undefined;
    level.finalkillcam_inflictor["none"] = undefined;
    level.finalkillcam_inflictor_agent_type["none"] = undefined;
    level.finalkillcam_inflictor_lastspawntime["none"] = undefined;
    level.finalkillcam_killcamentityindex["none"] = undefined;
    level.finalkillcam_killcamentitystarttime["none"] = undefined;
    level.finalkillcam_sweapon["none"] = undefined;
    level.finalkillcam_deathtimeoffset["none"] = undefined;
    level.finalkillcam_psoffsettime["none"] = undefined;
    level.finalkillcam_timerecorded["none"] = undefined;
    level.finalkillcam_timegameended["none"] = undefined;
    level.finalkillcam_smeansofdeath["none"] = undefined;
    level.finalkillcam_winner = undefined;
}

recordfinalkillcam( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 )
{
    if ( level.teambased && isdefined( var_2.team ) )
    {
        level.finalkillcam_delay[var_2.team] = var_0;
        level.finalkillcam_victim[var_2.team] = var_1;
        level.finalkillcam_attacker[var_2.team] = var_2;
        level.finalkillcam_attackernum[var_2.team] = var_3;
        level.finalkillcam_inflictor[var_2.team] = var_4;
        level.finalkillcam_killcamentityindex[var_2.team] = var_5;
        level.finalkillcam_killcamentitystarttime[var_2.team] = var_6;
        level.finalkillcam_sweapon[var_2.team] = var_7;
        level.finalkillcam_deathtimeoffset[var_2.team] = var_8;
        level.finalkillcam_psoffsettime[var_2.team] = var_9;
        level.finalkillcam_timerecorded[var_2.team] = maps\mp\_utility::getsecondspassed();
        level.finalkillcam_timegameended[var_2.team] = maps\mp\_utility::getsecondspassed();
        level.finalkillcam_smeansofdeath[var_2.team] = var_10;

        if ( isdefined( var_4 ) && isagent( var_4 ) )
        {
            level.finalkillcam_inflictor_agent_type[var_2.team] = var_4.agent_type;
            level.finalkillcam_inflictor_lastspawntime[var_2.team] = var_4.lastspawntime;
        }
        else
        {
            level.finalkillcam_inflictor_agent_type[var_2.team] = undefined;
            level.finalkillcam_inflictor_lastspawntime[var_2.team] = undefined;
        }
    }

    level.finalkillcam_delay["none"] = var_0;
    level.finalkillcam_victim["none"] = var_1;
    level.finalkillcam_attacker["none"] = var_2;
    level.finalkillcam_attackernum["none"] = var_3;
    level.finalkillcam_inflictor["none"] = var_4;
    level.finalkillcam_killcamentityindex["none"] = var_5;
    level.finalkillcam_killcamentitystarttime["none"] = var_6;
    level.finalkillcam_sweapon["none"] = var_7;
    level.finalkillcam_deathtimeoffset["none"] = var_8;
    level.finalkillcam_psoffsettime["none"] = var_9;
    level.finalkillcam_timerecorded["none"] = maps\mp\_utility::getsecondspassed();
    level.finalkillcam_timegameended["none"] = maps\mp\_utility::getsecondspassed();
    level.finalkillcam_timegameended["none"] = maps\mp\_utility::getsecondspassed();
    level.finalkillcam_smeansofdeath["none"] = var_10;

    if ( isdefined( var_4 ) && isagent( var_4 ) )
    {
        level.finalkillcam_inflictor_agent_type["none"] = var_4.agent_type;
        level.finalkillcam_inflictor_lastspawntime["none"] = var_4.lastspawntime;
    }
    else
    {
        level.finalkillcam_inflictor_agent_type["none"] = undefined;
        level.finalkillcam_inflictor_lastspawntime["none"] = undefined;
    }
}

erasefinalkillcam()
{
    if ( level.multiteambased )
    {
        for ( var_0 = 0; var_0 < level.teamnamelist.size; var_0++ )
        {
            level.finalkillcam_delay[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_victim[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_attacker[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_attackernum[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_inflictor[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_inflictor_agent_type[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_inflictor_lastspawntime[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_killcamentityindex[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_killcamentitystarttime[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_sweapon[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_deathtimeoffset[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_psoffsettime[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_timerecorded[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_timegameended[level.teamnamelist[var_0]] = undefined;
            level.finalkillcam_smeansofdeath[level.teamnamelist[var_0]] = undefined;
        }
    }
    else
    {
        level.finalkillcam_delay["axis"] = undefined;
        level.finalkillcam_victim["axis"] = undefined;
        level.finalkillcam_attacker["axis"] = undefined;
        level.finalkillcam_attackernum["axis"] = undefined;
        level.finalkillcam_inflictor["axis"] = undefined;
        level.finalkillcam_inflictor_agent_type["axis"] = undefined;
        level.finalkillcam_inflictor_lastspawntime["axis"] = undefined;
        level.finalkillcam_killcamentityindex["axis"] = undefined;
        level.finalkillcam_killcamentitystarttime["axis"] = undefined;
        level.finalkillcam_sweapon["axis"] = undefined;
        level.finalkillcam_deathtimeoffset["axis"] = undefined;
        level.finalkillcam_psoffsettime["axis"] = undefined;
        level.finalkillcam_timerecorded["axis"] = undefined;
        level.finalkillcam_timegameended["axis"] = undefined;
        level.finalkillcam_smeansofdeath["axis"] = undefined;
        level.finalkillcam_delay["allies"] = undefined;
        level.finalkillcam_victim["allies"] = undefined;
        level.finalkillcam_attacker["allies"] = undefined;
        level.finalkillcam_attackernum["allies"] = undefined;
        level.finalkillcam_inflictor["allies"] = undefined;
        level.finalkillcam_inflictor_agent_type["allies"] = undefined;
        level.finalkillcam_inflictor_lastspawntime["allies"] = undefined;
        level.finalkillcam_killcamentityindex["allies"] = undefined;
        level.finalkillcam_killcamentitystarttime["allies"] = undefined;
        level.finalkillcam_sweapon["allies"] = undefined;
        level.finalkillcam_deathtimeoffset["allies"] = undefined;
        level.finalkillcam_psoffsettime["allies"] = undefined;
        level.finalkillcam_timerecorded["allies"] = undefined;
        level.finalkillcam_timegameended["allies"] = undefined;
        level.finalkillcam_smeansofdeath["allies"] = undefined;
    }

    level.finalkillcam_delay["none"] = undefined;
    level.finalkillcam_victim["none"] = undefined;
    level.finalkillcam_attacker["none"] = undefined;
    level.finalkillcam_attackernum["none"] = undefined;
    level.finalkillcam_inflictor["none"] = undefined;
    level.finalkillcam_inflictor_agent_type["none"] = undefined;
    level.finalkillcam_inflictor_lastspawntime["none"] = undefined;
    level.finalkillcam_killcamentityindex["none"] = undefined;
    level.finalkillcam_killcamentitystarttime["none"] = undefined;
    level.finalkillcam_sweapon["none"] = undefined;
    level.finalkillcam_deathtimeoffset["none"] = undefined;
    level.finalkillcam_psoffsettime["none"] = undefined;
    level.finalkillcam_timerecorded["none"] = undefined;
    level.finalkillcam_timegameended["none"] = undefined;
    level.finalkillcam_smeansofdeath["none"] = undefined;
    level.finalkillcam_winner = undefined;
}

dofinalkillcam()
{
    level waittill( "round_end_finished" );
    level.showingfinalkillcam = 1;
    var_0 = "none";

    if ( isdefined( level.finalkillcam_winner ) )
        var_0 = level.finalkillcam_winner;

    var_1 = level.finalkillcam_delay[var_0];
    var_2 = level.finalkillcam_victim[var_0];
    var_3 = level.finalkillcam_attacker[var_0];
    var_4 = level.finalkillcam_attackernum[var_0];
    var_5 = level.finalkillcam_inflictor[var_0];
    var_6 = level.finalkillcam_inflictor_agent_type[var_0];
    var_7 = level.finalkillcam_inflictor_lastspawntime[var_0];
    var_8 = level.finalkillcam_killcamentityindex[var_0];
    var_9 = level.finalkillcam_killcamentitystarttime[var_0];
    var_10 = level.finalkillcam_sweapon[var_0];
    var_11 = level.finalkillcam_deathtimeoffset[var_0];
    var_12 = level.finalkillcam_psoffsettime[var_0];
    var_13 = level.finalkillcam_timerecorded[var_0];
    var_14 = level.finalkillcam_timegameended[var_0];
    var_15 = level.finalkillcam_smeansofdeath[var_0];

    if ( !isdefined( var_2 ) || !isdefined( var_3 ) )
    {
        level.showingfinalkillcam = 0;
        level notify( "final_killcam_done" );
        return;
    }

    var_16 = 15;
    var_17 = var_14 - var_13;

    if ( var_17 > var_16 )
    {
        level.showingfinalkillcam = 0;
        level notify( "final_killcam_done" );
        return;
    }

    if ( isdefined( var_3 ) )
    {
        var_3.finalkill = 1;
        var_18 = "none";

        if ( level.teambased )
            var_18 = var_3.team;

        if ( isdefined( level.finalkillcam_attacker[var_18] ) && level.finalkillcam_attacker[var_18] == var_3 )
            maps\mp\gametypes\_missions::processfinalkillchallenges( var_3, var_2 );
    }

    var_19 = spawnstruct();
    var_19.agent_type = var_6;
    var_19.lastspawntime = var_7;
    var_20 = ( gettime() - var_2.deathtime ) / 1000;

    foreach ( var_22 in level.players )
    {
        var_22 maps\mp\_utility::restorebasevisionset( 0 );
        var_22.killcamentitylookat = var_2 getentitynumber();
        var_22 thread maps\mp\gametypes\_killcam::killcam( var_5, var_19, var_4, var_8, var_9, var_10, var_20 + var_11, var_12, 0, 12, var_3, var_2, var_15 );
    }

    wait 0.1;

    while ( anyplayersinkillcam() )
        wait 0.05;

    level notify( "final_killcam_done" );
    level.showingfinalkillcam = 0;
}

anyplayersinkillcam()
{
    foreach ( var_1 in level.players )
    {
        if ( isdefined( var_1.killcam ) )
            return 1;
    }

    return 0;
}

resetplayervariables()
{
    self.killedplayerscurrent = [];
    self.ch_extremecrueltycomplete = 0;
    self.switching_teams = undefined;
    self.joining_team = undefined;
    self.leaving_team = undefined;
    self.pers["cur_kill_streak"] = 0;
    self.pers["cur_kill_streak_for_nuke"] = 0;
    maps\mp\gametypes\_gameobjects::detachusemodels();
}

getkillcamentity( var_0, var_1, var_2 )
{
    if ( !isdefined( var_0 ) || !isdefined( var_1 ) || var_0 == var_1 && !isagent( var_0 ) )
        return undefined;

    switch ( var_2 )
    {
        case "trophy_mp":
        case "bomb_site_mp":
        case "proximity_explosive_mp":
        case "heli_pilot_turret_mp":
        case "sentry_minigun_mp":
        case "hashima_missiles_mp":
            return var_1.killcament;
        case "remote_tank_projectile_mp":
        case "hind_bomb_mp":
        case "hind_missile_mp":
        case "aamissile_projectile_mp":
            if ( isdefined( var_1.vehicle_fired_from ) && isdefined( var_1.vehicle_fired_from.killcament ) )
                return var_1.vehicle_fired_from.killcament;
            else if ( isdefined( var_1.vehicle_fired_from ) )
                return var_1.vehicle_fired_from;

            break;
        case "sam_projectile_mp":
            if ( isdefined( var_1.samturret ) && isdefined( var_1.samturret.killcament ) )
                return var_1.samturret.killcament;

            break;
        case "ims_projectile_mp":
            if ( isdefined( var_0 ) && isdefined( var_0.imskillcament ) )
                return var_0.imskillcament;

            break;
        case "ball_drone_gun_mp":
        case "ball_drone_projectile_mp":
            if ( isplayer( var_0 ) && isdefined( var_0.balldrone ) && isdefined( var_0.balldrone.turret ) && isdefined( var_0.balldrone.turret.killcament ) )
                return var_0.balldrone.turret.killcament;

            break;
        case "none":
        case "artillery_mp":
            if ( isdefined( var_1.targetname ) && var_1.targetname == "care_package" || isdefined( var_1.killcament ) && ( var_1.classname == "script_brushmodel" || var_1.classname == "trigger_multiple" || var_1.classname == "script_model" ) )
                return var_1.killcament;

            break;
        case "ac130_105mm_mp":
        case "ac130_40mm_mp":
        case "remotemissile_projectile_mp":
        case "ac130_25mm_mp":
        case "osprey_player_minigun_mp":
        case "ugv_turret_mp":
        case "remote_turret_mp":
            return undefined;
    }

    if ( maps\mp\_utility::isdestructibleweapon( var_2 ) || maps\mp\_utility::isbombsiteweapon( var_2 ) )
    {
        if ( isdefined( var_1.killcament ) && !var_0 attackerinremotekillstreak() )
            return var_1.killcament;
        else
            return undefined;
    }

    return var_1;
}

attackerinremotekillstreak()
{
    if ( !isdefined( self ) )
        return 0;

    if ( isdefined( level.ac130player ) && self == level.ac130player )
        return 1;

    if ( isdefined( level.chopper ) && isdefined( level.chopper.gunner ) && self == level.chopper.gunner )
        return 1;

    if ( isdefined( level.remote_mortar ) && isdefined( level.remote_mortar.owner ) && self == level.remote_mortar.owner )
        return 1;

    if ( isdefined( self.using_remote_turret ) && self.using_remote_turret )
        return 1;

    if ( isdefined( self.using_remote_tank ) && self.using_remote_tank )
        return 1;
    else if ( isdefined( self.using_remote_a10 ) )
        return 1;

    return 0;
}

hitlocdebug( var_0, var_1, var_2, var_3, var_4 )
{
    var_5 = [];
    var_5[0] = 2;
    var_5[1] = 3;
    var_5[2] = 5;
    var_5[3] = 7;

    if ( !getdvarint( "scr_hitloc_debug" ) )
        return;

    if ( !isdefined( var_0.hitlocinited ) )
    {
        for ( var_6 = 0; var_6 < 6; var_6++ )
            var_0 setclientdvar( "ui_hitloc_" + var_6, "" );

        var_0.hitlocinited = 1;
    }

    if ( level.splitscreen || !isplayer( var_0 ) )
        return;

    var_7 = 6;

    if ( !isdefined( var_0.damageinfo ) )
    {
        var_0.damageinfo = [];

        for ( var_6 = 0; var_6 < var_7; var_6++ )
        {
            var_0.damageinfo[var_6] = spawnstruct();
            var_0.damageinfo[var_6].damage = 0;
            var_0.damageinfo[var_6].hitloc = "";
            var_0.damageinfo[var_6].bp = 0;
            var_0.damageinfo[var_6].jugg = 0;
            var_0.damageinfo[var_6].colorindex = 0;
        }

        var_0.damageinfocolorindex = 0;
        var_0.damageinfovictim = undefined;
    }

    for ( var_6 = var_7 - 1; var_6 > 0; var_6-- )
    {
        var_0.damageinfo[var_6].damage = var_0.damageinfo[var_6 - 1].damage;
        var_0.damageinfo[var_6].hitloc = var_0.damageinfo[var_6 - 1].hitloc;
        var_0.damageinfo[var_6].bp = var_0.damageinfo[var_6 - 1].bp;
        var_0.damageinfo[var_6].jugg = var_0.damageinfo[var_6 - 1].jugg;
        var_0.damageinfo[var_6].colorindex = var_0.damageinfo[var_6 - 1].colorindex;
    }

    var_0.damageinfo[0].damage = var_2;
    var_0.damageinfo[0].hitloc = var_3;
    var_0.damageinfo[0].bp = var_4 & level.idflags_penetration;
    var_0.damageinfo[0].jugg = var_1 maps\mp\_utility::isjuggernaut();

    if ( isdefined( var_0.damageinfovictim ) && var_0.damageinfovictim != var_1 )
    {
        var_0.damageinfocolorindex++;

        if ( var_0.damageinfocolorindex == var_5.size )
            var_0.damageinfocolorindex = 0;
    }

    var_0.damageinfovictim = var_1;
    var_0.damageinfo[0].colorindex = var_0.damageinfocolorindex;

    for ( var_6 = 0; var_6 < var_7; var_6++ )
    {
        var_8 = "^" + var_5[var_0.damageinfo[var_6].colorindex];

        if ( var_0.damageinfo[var_6].hitloc != "" )
        {
            var_9 = var_8 + var_0.damageinfo[var_6].hitloc;

            if ( var_0.damageinfo[var_6].bp )
                var_9 += " (BP)";

            if ( var_0.damageinfo[var_6].jugg )
                var_9 += " (Jugg)";

            var_0 setclientdvar( "ui_hitloc_" + var_6, var_9 );
        }

        var_0 setclientdvar( "ui_hitloc_damage_" + var_6, var_8 + var_0.damageinfo[var_6].damage );
    }
}

giverecentshieldxp()
{
    self endon( "death" );
    self endon( "disconnect" );
    self notify( "giveRecentShieldXP" );
    self endon( "giveRecentShieldXP" );
    self.recentshieldxp++;
    wait 20.0;
    self.recentshieldxp = 0;
}

updateinflictorstat( var_0, var_1, var_2 )
{
    if ( !isdefined( var_0 ) || !isdefined( var_0.alreadyhit ) || !var_0.alreadyhit || !maps\mp\_utility::issinglehitweapon( var_2 ) )
        maps\mp\gametypes\_gamelogic::setinflictorstat( var_0, var_1, var_2 );

    if ( isdefined( var_0 ) )
        var_0.alreadyhit = 1;
}

callback_playerdamage_internal( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 )
{
    var_1 = maps\mp\_utility::_validateattacker( var_1 );
    var_11 = getdvar( "g_gametype" );

    if ( isdefined( var_5 ) && var_5 == "MOD_CRUSH" && isdefined( var_0 ) && isdefined( var_0.classname ) && var_0.classname == "script_vehicle" )
        return "crushed";

    if ( !maps\mp\_utility::isreallyalive( var_2 ) )
        return "!isReallyAlive( victim )";

    if ( var_6 == "hind_bomb_mp" || var_6 == "hind_missile_mp" )
    {
        if ( isdefined( var_1 ) && var_2 == var_1 )
            return 0;
    }

    if ( isdefined( var_1 ) && var_1.classname == "script_origin" && isdefined( var_1.type ) && var_1.type == "soft_landing" )
        return "soft_landing";

    if ( var_6 == "killstreak_emp_mp" )
        return "sWeapon == killstreak_emp_mp";

    if ( var_6 == "bouncingbetty_mp" && !maps\mp\gametypes\_weapons::minedamageheightpassed( var_0, var_2 ) )
        return "mineDamageHeightPassed";

    if ( var_6 == "bouncingbetty_mp" && ( var_2 getstance() == "crouch" || var_2 getstance() == "prone" ) )
        var_3 = int( var_3 / 2 );

    if ( var_6 == "emp_grenade_mp" && var_5 != "MOD_IMPACT" )
        var_2 notify( "emp_damage", var_1 );

    if ( isdefined( level.hostmigrationtimer ) )
        return "level.hostMigrationTimer";

    if ( var_5 == "MOD_FALLING" )
        var_2 thread emitfalldamage( var_3 );

    if ( var_5 == "MOD_EXPLOSIVE_BULLET" && var_3 != 1 )
    {
        var_3 *= getdvarfloat( "scr_explBulletMod" );
        var_3 = int( var_3 );
    }

    if ( isdefined( var_1 ) && var_1.classname == "worldspawn" )
        var_1 = undefined;

    if ( isdefined( var_1 ) && isdefined( var_1.gunner ) )
        var_1 = var_1.gunner;

    if ( isdefined( var_0 ) && isdefined( var_0.damagedby ) )
        var_1 = var_0.damagedby;

    var_12 = isdefined( var_1 ) && !isdefined( var_1.gunner ) && ( var_1.classname == "script_vehicle" || var_1.classname == "misc_turret" || var_1.classname == "script_model" );
    var_13 = maps\mp\_utility::attackerishittingteam( var_2, var_1 );
    var_14 = isdefined( var_1 ) && isdefined( var_0 ) && isdefined( var_2 ) && isplayer( var_1 ) && var_1 == var_0 && var_1 == var_2 && !isdefined( var_0.poison );

    if ( var_14 )
        return "attackerIsInflictorVictim";

    var_15 = 0.0;

    if ( var_4 & level.idflags_stun )
    {
        var_15 = 0.0;
        var_3 = 0.0;
    }
    else if ( var_9 == "shield" )
    {
        if ( var_13 && level.friendlyfire == 0 )
            return "attackerIsHittingTeammate";

        if ( var_5 == "MOD_PISTOL_BULLET" || var_5 == "MOD_RIFLE_BULLET" || var_5 == "MOD_EXPLOSIVE_BULLET" && !var_13 )
        {
            if ( isplayer( var_1 ) )
            {
                if ( isdefined( var_2.owner ) )
                    var_2 = var_2.owner;

                var_1.lastattackedshieldplayer = var_2;
                var_1.lastattackedshieldtime = gettime();
            }

            var_2 notify( "shield_blocked" );

            if ( maps\mp\_utility::isenvironmentweapon( var_6 ) )
                var_16 = 25;
            else
                var_16 = maps\mp\perks\_perks::cac_modified_damage( var_2, var_1, var_3, var_5, var_6, var_7, var_8, var_9 );

            var_2.shielddamage += var_16;

            if ( !maps\mp\_utility::isenvironmentweapon( var_6 ) || common_scripts\utility::cointoss() )
                var_2.shieldbullethits++;

            if ( var_2.shieldbullethits >= level.riotshieldxpbullets )
            {
                if ( self.recentshieldxp > 4 )
                    var_17 = int( 50 / self.recentshieldxp );
                else
                    var_17 = 50;

                var_2 thread maps\mp\gametypes\_rank::giverankxp( "shield_damage", var_17 );
                var_2 thread giverecentshieldxp();
                var_2 thread maps\mp\gametypes\_missions::genericchallenge( "shield_damage", var_2.shielddamage );
                var_2 thread maps\mp\gametypes\_missions::genericchallenge( "shield_bullet_hits", var_2.shieldbullethits );
                var_2.shielddamage = 0;
                var_2.shieldbullethits = 0;
            }
        }

        if ( var_4 & level.idflags_shield_explosive_impact )
        {
            if ( !var_13 )
                var_2 thread maps\mp\gametypes\_missions::genericchallenge( "shield_explosive_hits", 1 );

            var_9 = "none";

            if ( !( var_4 & level.idflags_shield_explosive_impact_huge ) )
                var_3 *= 0.0;
        }
        else if ( var_4 & level.idflags_shield_explosive_splash )
        {
            if ( isdefined( var_0 ) && isdefined( var_0.stuckenemyentity ) && var_0.stuckenemyentity == var_2 )
                var_3 = 151;

            var_2 thread maps\mp\gametypes\_missions::genericchallenge( "shield_explosive_hits", 1 );
            var_9 = "none";
        }
        else
            return "hit shield";
    }
    else if ( var_5 == "MOD_MELEE" && maps\mp\gametypes\_weapons::isriotshield( var_6 ) )
    {
        if ( !( var_13 && level.friendlyfire == 0 ) )
        {
            var_15 = 0.0;
            var_2 stunplayer( 0.0 );
        }
    }

    if ( isdefined( var_0 ) && isdefined( var_0.stuckenemyentity ) && var_0.stuckenemyentity == var_2 )
        var_3 = 151;

    if ( !var_13 )
    {
        if ( maps\mp\_utility::_hasperk( "specialty_moredamage" ) )
            maps\mp\_utility::_unsetperk( "specialty_moredamage" );

        if ( maps\mp\_utility::isbulletdamage( var_5 ) && var_1 maps\mp\_utility::_hasperk( "specialty_deadeye" ) )
            var_1 maps\mp\perks\_perkfunctions::setdeadeyeinternal();

        var_3 = maps\mp\perks\_perks::cac_modified_damage( var_2, var_1, var_3, var_5, var_6, var_7, var_8, var_9, var_0 );

        if ( isplayer( var_1 ) && ( var_6 == "smoke_grenade_mp" || var_6 == "throwingknife_mp" ) )
            var_1 thread maps\mp\gametypes\_gamelogic::threadedsetweaponstatbyname( var_6, 1, "hits" );
    }

    if ( isdefined( level.modifyplayerdamage ) )
        var_3 = [[ level.modifyplayerdamage ]]( var_2, var_1, var_3, var_5, var_6, var_7, var_8, var_9 );

    if ( !var_3 )
        return "!iDamage";

    var_2.idflags = var_4;
    var_2.idflagstime = gettime();

    if ( game["state"] == "postgame" )
        return "game[ state ] == postgame";

    if ( var_2.sessionteam == "spectator" )
        return "victim.sessionteam == spectator";

    if ( isdefined( var_2.candocombat ) && !var_2.candocombat )
        return "!victim.canDoCombat";

    if ( isdefined( var_1 ) && isplayer( var_1 ) && isdefined( var_1.candocombat ) && !var_1.candocombat )
        return "!eAttacker.canDoCombat";

    if ( var_12 && var_13 )
    {
        if ( var_5 == "MOD_CRUSH" )
        {
            var_2 maps\mp\_utility::_suicide();
            return "suicide crush";
        }

        if ( !level.friendlyfire )
            return "!level.friendlyfire";
    }

    if ( isai( self ) )
        self [[ level.bot_funcs["on_damaged"] ]]( var_1, var_3, var_5, var_6, var_0, var_9 );

    if ( !isdefined( var_8 ) )
        var_4 |= level.idflags_no_knockback;

    var_18 = 0;

    if ( var_2.health == var_2.maxhealth && ( !isdefined( var_2.laststand ) || !var_2.laststand ) || !isdefined( var_2.attackers ) && !isdefined( var_2.laststand ) )
        var_2 resetattackerlist_internal();

    if ( maps\mp\_utility::isheadshot( var_6, var_9, var_5, var_1 ) )
        var_5 = "MOD_HEAD_SHOT";

    if ( maps\mp\gametypes\_tweakables::gettweakablevalue( "game", "onlyheadshots" ) )
    {
        if ( var_5 == "MOD_PISTOL_BULLET" || var_5 == "MOD_RIFLE_BULLET" || var_5 == "MOD_EXPLOSIVE_BULLET" )
            return "getTweakableValue( game, onlyheadshots )";
        else if ( var_5 == "MOD_HEAD_SHOT" )
        {
            if ( var_2 maps\mp\_utility::isjuggernaut() )
                var_3 = 75;
            else
                var_3 = 150;
        }
    }

    if ( var_6 == "destructible_toy" && isdefined( var_0 ) )
        var_6 = "destructible_car";

    if ( gettime() < var_2.spawntime + level.killstreakspawnshield )
    {
        var_19 = int( max( var_2.health / 4, 1 ) );

        if ( var_3 >= var_19 && maps\mp\_utility::iskillstreakweapon( var_6 ) && var_5 != "MOD_MELEE" )
            var_3 = var_19;
    }

    if ( !( var_4 & level.idflags_no_protection ) )
    {
        if ( !level.teambased && var_12 && isdefined( var_1.owner ) && var_1.owner == var_2 )
        {
            if ( var_5 == "MOD_CRUSH" )
                var_2 maps\mp\_utility::_suicide();

            return "ffa suicide";
        }

        if ( ( issubstr( var_5, "MOD_GRENADE" ) || issubstr( var_5, "MOD_EXPLOSIVE" ) || issubstr( var_5, "MOD_PROJECTILE" ) ) && isdefined( var_0 ) && isdefined( var_1 ) )
        {
            if ( var_2 != var_1 && var_0.classname == "grenade" && var_2.lastspawntime + 3500 > gettime() && isdefined( var_2.lastspawnpoint ) && distance( var_0.origin, var_2.lastspawnpoint.origin ) < 500 )
                return "spawnkill grenade protection";

            var_2.explosiveinfo = [];
            var_2.explosiveinfo["damageTime"] = gettime();
            var_2.explosiveinfo["damageId"] = var_0 getentitynumber();
            var_2.explosiveinfo["returnToSender"] = 0;
            var_2.explosiveinfo["counterKill"] = 0;
            var_2.explosiveinfo["chainKill"] = 0;
            var_2.explosiveinfo["cookedKill"] = 0;
            var_2.explosiveinfo["throwbackKill"] = 0;
            var_2.explosiveinfo["suicideGrenadeKill"] = 0;
            var_2.explosiveinfo["weapon"] = var_6;
            var_20 = issubstr( var_6, "frag_" );

            if ( var_1 != var_2 )
            {
                if ( ( issubstr( var_6, "c4_" ) || issubstr( var_6, "proximity_explosive_" ) || issubstr( var_6, "claymore_" ) ) && isdefined( var_0.owner ) )
                {
                    var_2.explosiveinfo["returnToSender"] = var_0.owner == var_2;
                    var_2.explosiveinfo["counterKill"] = isdefined( var_0.wasdamaged );
                    var_2.explosiveinfo["chainKill"] = isdefined( var_0.waschained );
                    var_2.explosiveinfo["bulletPenetrationKill"] = isdefined( var_0.wasdamagedfrombulletpenetration );
                    var_2.explosiveinfo["cookedKill"] = 0;
                }

                if ( isdefined( var_1.lastgrenadesuicidetime ) && var_1.lastgrenadesuicidetime >= gettime() - 50 && var_20 )
                    var_2.explosiveinfo["suicideGrenadeKill"] = 1;
            }

            if ( var_20 )
            {
                var_2.explosiveinfo["cookedKill"] = isdefined( var_0.iscooked );
                var_2.explosiveinfo["throwbackKill"] = isdefined( var_0.threwback );
            }

            var_2.explosiveinfo["stickKill"] = isdefined( var_0.isstuck ) && var_0.isstuck == "enemy";
            var_2.explosiveinfo["stickFriendlyKill"] = isdefined( var_0.isstuck ) && var_0.isstuck == "friendly";

            if ( isplayer( var_1 ) && var_1 != self && var_11 != "aliens" )
                updateinflictorstat( var_0, var_1, var_6 );
        }

        if ( issubstr( var_5, "MOD_IMPACT" ) && var_6 == "iw6_rgm_mp" )
        {
            if ( isplayer( var_1 ) && var_1 != self && var_11 != "aliens" )
                updateinflictorstat( var_0, var_1, var_6 );
        }

        if ( isplayer( var_1 ) && isdefined( var_1.pers["participation"] ) )
            var_1.pers["participation"]++;
        else if ( isplayer( var_1 ) )
            var_1.pers["participation"] = 1;

        var_21 = var_2.health / var_2.maxhealth;

        if ( var_13 )
        {
            if ( !maps\mp\_utility::matchmakinggame() && isplayer( var_1 ) )
                var_1 maps\mp\_utility::incplayerstat( "mostff", 1 );

            if ( level.friendlyfire == 0 || !isplayer( var_1 ) && level.friendlyfire != 1 || var_6 == "bomb_site_mp" )
            {
                if ( var_6 == "artillery_mp" || var_6 == "stealth_bomb_mp" )
                    var_2 damageshellshockandrumble( var_0, var_6, var_5, var_3, var_4, var_1 );

                return "friendly fire";
            }
            else if ( level.friendlyfire == 1 )
            {
                if ( var_3 < 1 )
                    var_3 = 1;

                if ( var_2 maps\mp\_utility::isjuggernaut() )
                    var_3 = maps\mp\perks\_perks::cac_modified_damage( var_2, var_1, var_3, var_5, var_6, var_7, var_8, var_9 );

                var_2.lastdamagewasfromenemy = 0;
                var_2 finishplayerdamagewrapper( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_15 );
            }
            else if ( level.friendlyfire == 2 && maps\mp\_utility::isreallyalive( var_1 ) )
            {
                var_3 = int( var_3 * 0.5 );

                if ( var_3 < 1 )
                    var_3 = 1;

                var_1.lastdamagewasfromenemy = 0;
                var_1.friendlydamage = 1;
                var_1 finishplayerdamagewrapper( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_15 );
                var_1.friendlydamage = undefined;
            }
            else if ( level.friendlyfire == 3 && maps\mp\_utility::isreallyalive( var_1 ) )
            {
                var_3 = int( var_3 * 0.5 );

                if ( var_3 < 1 )
                    var_3 = 1;

                var_2.lastdamagewasfromenemy = 0;
                var_1.lastdamagewasfromenemy = 0;
                var_2 finishplayerdamagewrapper( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_15 );

                if ( maps\mp\_utility::isreallyalive( var_1 ) )
                {
                    var_1.friendlydamage = 1;
                    var_1 finishplayerdamagewrapper( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_15 );
                    var_1.friendlydamage = undefined;
                }
            }

            var_18 = 1;
        }
        else
        {
            if ( var_3 < 1 )
                var_3 = 1;

            if ( isdefined( var_1 ) && isplayer( var_1 ) )
                addattacker( var_2, var_1, var_0, var_6, var_3, var_7, var_8, var_9, var_10, var_5 );

            if ( isdefined( var_1 ) && !isplayer( var_1 ) && isdefined( var_1.owner ) && ( !isdefined( var_1.scrambled ) || !var_1.scrambled ) )
                addattacker( var_2, var_1.owner, var_0, var_6, var_3, var_7, var_8, var_9, var_10, var_5 );
            else if ( isdefined( var_1 ) && !isplayer( var_1 ) && isdefined( var_1.secondowner ) && isdefined( var_1.scrambled ) && var_1.scrambled )
                addattacker( var_2, var_1.secondowner, var_0, var_6, var_3, var_7, var_8, var_9, var_10, var_5 );

            if ( var_5 == "MOD_EXPLOSIVE" || var_5 == "MOD_GRENADE_SPLASH" && var_3 < var_2.health )
                var_2 notify( "survived_explosion", var_1 );

            if ( isdefined( var_1 ) )
                level.lastlegitimateattacker = var_1;

            if ( isdefined( var_1 ) && isplayer( var_1 ) && isdefined( var_6 ) )
                var_1 thread maps\mp\gametypes\_weapons::checkhit( var_6, var_2 );

            if ( isdefined( var_1 ) && isplayer( var_1 ) && isdefined( var_6 ) && var_1 != var_2 )
            {
                var_1 thread maps\mp\_events::damagedplayer( self, var_3, var_6 );
                var_2.attackerposition = var_1.origin;
            }
            else
                var_2.attackerposition = undefined;

            if ( issubstr( var_5, "MOD_GRENADE" ) && isdefined( var_0.iscooked ) )
                var_2.wascooked = gettime();
            else
                var_2.wascooked = undefined;

            var_2.lastdamagewasfromenemy = isdefined( var_1 ) && var_1 != var_2;

            if ( var_2.lastdamagewasfromenemy )
            {
                var_22 = gettime();
                var_1.damagedplayers[var_2.guid] = var_22;
                var_2.lastdamagedtime = var_22;
            }

            var_2 finishplayerdamagewrapper( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_15 );

            if ( isdefined( level.ac130player ) && isdefined( var_1 ) && level.ac130player == var_1 )
                level notify( "ai_pain", var_2 );

            var_2 thread maps\mp\gametypes\_missions::playerdamaged( var_0, var_1, var_3, var_5, var_6, var_9 );
        }

        if ( var_12 && isdefined( var_1.gunner ) )
            var_23 = var_1.gunner;
        else
            var_23 = var_1;

        if ( isdefined( var_23 ) && var_23 != var_2 && var_3 > 0 && ( !isdefined( var_9 ) || var_9 != "shield" ) )
        {
            var_24 = !maps\mp\_utility::isreallyalive( var_2 ) || isagent( var_2 ) && var_3 >= var_2.health;

            if ( var_4 & level.idflags_stun )
                var_25 = "stun";
            else if ( isexplosivedamagemod( var_5 ) && ( isdefined( var_2.thermodebuffed ) && var_2.thermodebuffed ) )
                var_25 = common_scripts\utility::ter_op( var_24, "thermodebuff_kill", "thermobaric_debuff" );
            else if ( isexplosivedamagemod( var_5 ) && var_2 maps\mp\_utility::_hasperk( "_specialty_blastshield" ) && !maps\mp\_utility::weaponignoresblastshield( var_6 ) )
                var_25 = common_scripts\utility::ter_op( var_24, "hitkillblast", "hitblastshield" );
            else if ( var_2 maps\mp\_utility::_hasperk( "specialty_combathigh" ) )
                var_25 = "hitendgame";
            else if ( isdefined( var_2.lightarmorhp ) && var_5 != "MOD_HEAD_SHOT" && !maps\mp\_utility::isfmjdamage( var_6, var_5, var_1 ) )
                var_25 = "hitlightarmor";
            else if ( maps\mp\perks\_perkfunctions::hasheavyarmor( var_2 ) )
                var_25 = "hitlightarmor";
            else if ( var_2 maps\mp\_utility::isjuggernaut() )
                var_25 = common_scripts\utility::ter_op( var_24, "hitkilljugg", "hitjuggernaut" );
            else if ( var_2 maps\mp\_utility::_hasperk( "specialty_moreHealth" ) )
                var_25 = "hitmorehealth";
            else if ( var_23 maps\mp\_utility::_hasperk( "specialty_moredamage" ) )
            {
                var_25 = common_scripts\utility::ter_op( var_24, "hitdeadeyekill", "hitcritical" );
                var_23 maps\mp\_utility::_unsetperk( "specialty_moredamage" );
            }
            else if ( !shouldweaponfeedback( var_6 ) )
                var_25 = "none";
            else
                var_25 = common_scripts\utility::ter_op( var_24, "hitkill", "standard" );

            var_23 thread maps\mp\gametypes\_damagefeedback::updatedamagefeedback( var_25 );
        }

        maps\mp\gametypes\_gamelogic::sethasdonecombat( var_2, 1 );
    }

    if ( isdefined( var_1 ) && var_1 != var_2 && !var_18 )
        level.usestartspawns = 0;

    if ( var_3 > 10 && isdefined( var_0 ) && !var_2 maps\mp\_utility::isusingremote() && isplayer( var_2 ) )
    {
        var_2 thread maps\mp\gametypes\_shellshock::bloodeffect( var_0.origin );

        if ( isplayer( var_0 ) && var_5 == "MOD_MELEE" )
            var_0 thread maps\mp\gametypes\_shellshock::bloodmeleeeffect();
    }

    if ( var_2.sessionstate != "dead" )
    {
        var_23 = var_2 getentitynumber();
        var_24 = var_2.name;
        var_25 = var_2.pers["team"];
        var_26 = var_2.guid;
        var_27 = "";

        if ( isplayer( var_1 ) )
        {
            var_28 = var_1 getentitynumber();
            var_29 = var_1.guid;
            var_30 = var_1.name;
            var_27 = var_1.pers["team"];
        }
        else
        {
            var_28 = -1;
            var_29 = "";
            var_30 = "";
            var_27 = "world";
        }

        logprint( "D;" + var_26 + ";" + var_23 + ";" + var_25 + ";" + var_24 + ";" + var_29 + ";" + var_28 + ";" + var_27 + ";" + var_30 + ";" + var_6 + ";" + var_3 + ";" + var_5 + ";" + var_9 + "\n" );
    }

    hitlocdebug( var_1, var_2, var_3, var_9, var_4 );

    if ( isdefined( var_1 ) && var_1 != var_2 )
    {
        if ( isplayer( var_1 ) )
            var_1 maps\mp\_utility::incplayerstat( "damagedone", var_3 );

        var_2 maps\mp\_utility::incplayerstat( "damagetaken", var_3 );
    }

    if ( isagent( self ) )
        self [[ maps\mp\agents\_agent_utility::agentfunc( "on_damaged_finished" ) ]]( var_0, var_1, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 );

    return "finished";
}

shouldweaponfeedback( var_0 )
{
    switch ( var_0 )
    {
        case "artillery_mp":
        case "stealth_bomb_mp":
            return 0;
    }

    return 1;
}

checkvictimstutter( var_0, var_1, var_2, var_3, var_4 )
{
    if ( var_4 == "MOD_PISTOL_BULLET" || var_4 == "MOD_RIFLE_BULLET" || var_4 == "MOD_HEAD_SHOT" )
    {
        if ( distance( var_0.origin, var_1.origin ) > 256 )
            return;

        var_5 = var_0 getvelocity();

        if ( lengthsquared( var_5 ) < 10 )
            return;

        var_6 = maps\mp\_utility::findisfacing( var_0, var_1, 25 );

        if ( var_6 )
            var_0 thread stutterstep();
    }
}

stutterstep( var_0 )
{
    self endon( "disconnect" );
    self endon( "death" );
    level endon( "game_ended" );
    self.instutter = 1;
    self.movespeedscaler = 0.05;
    maps\mp\gametypes\_weapons::updatemovespeedscale();
    wait 0.5;
    self.movespeedscaler = 1;

    if ( maps\mp\_utility::_hasperk( "specialty_lightweight" ) )
        self.movespeedscaler = maps\mp\_utility::lightweightscalar();

    maps\mp\gametypes\_weapons::updatemovespeedscale();
    self.instutter = 0;
}

addattacker( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9 )
{
    if ( !isdefined( var_0.attackerdata ) )
        var_0.attackerdata = [];

    if ( !isdefined( var_0.attackerdata[var_1.guid] ) )
    {
        var_0.attackers[var_1.guid] = var_1;
        var_0.attackerdata[var_1.guid] = spawnstruct();
        var_0.attackerdata[var_1.guid].damage = 0;
        var_0.attackerdata[var_1.guid].attackerent = var_1;
        var_0.attackerdata[var_1.guid].firsttimedamaged = gettime();
    }

    if ( maps\mp\gametypes\_weapons::isprimaryweapon( var_3 ) && !maps\mp\gametypes\_weapons::issidearm( var_3 ) )
        var_0.attackerdata[var_1.guid].isprimary = 1;

    var_0.attackerdata[var_1.guid].damage += var_4;
    var_0.attackerdata[var_1.guid].weapon = var_3;
    var_0.attackerdata[var_1.guid].vpoint = var_5;
    var_0.attackerdata[var_1.guid].vdir = var_6;
    var_0.attackerdata[var_1.guid].shitloc = var_7;
    var_0.attackerdata[var_1.guid].psoffsettime = var_8;
    var_0.attackerdata[var_1.guid].smeansofdeath = var_9;
    var_0.attackerdata[var_1.guid].attackerent = var_1;
    var_0.attackerdata[var_1.guid].lasttimedamaged = gettime();

    if ( isdefined( var_2 ) && !isplayer( var_2 ) && isdefined( var_2.primaryweapon ) )
        var_0.attackerdata[var_1.guid].sprimaryweapon = var_2.primaryweapon;
    else if ( isdefined( var_1 ) && isplayer( var_1 ) && var_1 getcurrentprimaryweapon() != "none" )
        var_0.attackerdata[var_1.guid].sprimaryweapon = var_1 getcurrentprimaryweapon();
    else
        var_0.attackerdata[var_1.guid].sprimaryweapon = undefined;
}

resetattackerlist( var_0 )
{
    self endon( "disconnect" );
    self endon( "death" );
    level endon( "game_ended" );
    wait 1.75;
    resetattackerlist_internal();
}

resetattackerlist_internal()
{
    self.attackers = [];
    self.attackerdata = [];
}

callback_playerdamage( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9 )
{
    var_10 = callback_playerdamage_internal( var_0, var_1, self, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9 );
}

finishplayerdamagewrapper( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 )
{
    if ( maps\mp\_utility::isusingremote() && var_2 >= self.health && !( var_3 & level.idflags_stun ) && allowfauxdeath() || maps\mp\_utility::isrocketcorpse() )
    {
        if ( !isdefined( var_7 ) )
            var_7 = ( 0, 0, 0 );

        if ( !isdefined( var_1 ) && !isdefined( var_0 ) )
        {
            var_1 = self;
            var_0 = var_1;
        }

        playerkilled_internal( var_0, var_1, self, var_2, var_4, var_5, var_7, var_8, var_9, 0, 1 );
    }
    else
    {
        if ( !callback_killingblow( var_0, var_1, var_2 - var_2 * var_10, var_3, var_4, var_5, var_6, var_7, var_8, var_9 ) )
            return;

        if ( !isalive( self ) )
            return;

        if ( isplayer( self ) )
            self finishplayerdamage( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10 );
    }

    if ( var_4 == "MOD_EXPLOSIVE_BULLET" && !maps\mp\_utility::is_aliens() )
        self shellshock( "damage_mp", getdvarfloat( "scr_csmode" ) );

    damageshellshockandrumble( var_0, var_5, var_4, var_2, var_3, var_1 );
}

allowfauxdeath()
{
    if ( !isdefined( level.allowfauxdeath ) )
        level.allowfauxdeath = 1;

    return level.allowfauxdeath;
}

callback_playerlaststand( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8 )
{
    var_9 = spawnstruct();
    var_9.einflictor = var_0;
    var_9.attacker = var_1;
    var_9.idamage = var_2;
    var_9.attackerposition = var_1.origin;

    if ( var_1 == self )
        var_9.smeansofdeath = "MOD_SUICIDE";
    else
        var_9.smeansofdeath = var_3;

    var_9.sweapon = var_4;

    if ( isdefined( var_1 ) && isplayer( var_1 ) && var_1 getcurrentprimaryweapon() != "none" )
        var_9.sprimaryweapon = var_1 getcurrentprimaryweapon();
    else
        var_9.sprimaryweapon = undefined;

    var_9.vdir = var_5;
    var_9.shitloc = var_6;
    var_9.laststandstarttime = gettime();
    var_10 = maydolaststand( var_4, var_3, var_6 );

    if ( isdefined( self.endgame ) )
        var_10 = 0;

    if ( level.teambased && isdefined( var_1.team ) && var_1.team == self.team )
        var_10 = 0;

    if ( level.diehardmode )
    {
        if ( level.teamcount[self.team] <= 1 )
            var_10 = 0;
        else if ( maps\mp\_utility::isteaminlaststand() )
        {
            var_10 = 0;
            maps\mp\_utility::killteaminlaststand( self.team );
        }
    }

    if ( !var_10 )
    {
        self.laststandparams = var_9;
        self.uselaststandparams = 1;
        maps\mp\_utility::_suicide();
    }
    else
    {
        self.inlaststand = 1;
        var_11 = spawnstruct();

        if ( maps\mp\_utility::_hasperk( "specialty_finalstand" ) )
        {
            var_11.titletext = game["strings"]["final_stand"];
            var_11.iconname = "specialty_finalstand";
        }
        else if ( maps\mp\_utility::_hasperk( "specialty_c4death" ) )
        {
            var_11.titletext = game["strings"]["c4_death"];
            var_11.iconname = "specialty_c4death";
        }
        else
        {
            var_11.titletext = game["strings"]["last_stand"];
            var_11.iconname = "specialty_finalstand";
        }

        var_11.glowcolor = ( 1, 0, 0 );
        var_11.sound = "mp_last_stand";
        var_11.duration = 2.0;
        self.health = 1;
        thread maps\mp\gametypes\_hud_message::notifymessage( var_11 );
        var_12 = "frag_grenade_mp";

        if ( isdefined( level.ac130player ) && isdefined( var_1 ) && level.ac130player == var_1 )
            level notify( "ai_crawling", self );

        if ( maps\mp\_utility::_hasperk( "specialty_finalstand" ) )
        {
            self.laststandparams = var_9;
            self.infinalstand = 1;
            var_13 = self getweaponslistexclusives();

            foreach ( var_15 in var_13 )
                self takeweapon( var_15 );

            common_scripts\utility::_disableusability();
            thread enablelaststandweapons();
            thread laststandtimer( 20, 1 );
        }
        else if ( maps\mp\_utility::_hasperk( "specialty_c4death" ) )
        {
            self.previousprimary = self.lastdroppableweapon;
            self.laststandparams = var_9;
            self takeallweapons();
            self giveweapon( "c4death_mp", 0, 0 );
            self switchtoweapon( "c4death_mp" );
            common_scripts\utility::_disableusability();
            self.inc4death = 1;
            thread laststandtimer( 20, 0 );
            thread detonateonuse();
            thread detonateondeath();
        }
        else
        {
            if ( level.diehardmode )
            {
                var_1 maps\mp\gametypes\_rank::giverankxp( "kill", 100, var_4, var_3 );
                self.laststandparams = var_9;
                common_scripts\utility::_disableweapon();
                thread laststandtimer( 20, 0 );
                common_scripts\utility::_disableusability();
                return;
            }

            self.laststandparams = var_9;
            var_17 = undefined;
            var_18 = self getweaponslistprimaries();

            foreach ( var_15 in var_18 )
            {
                if ( maps\mp\gametypes\_weapons::issidearm( var_15 ) )
                    var_17 = var_15;
            }

            if ( !isdefined( var_17 ) )
            {
                var_17 = "iw6_p226_mp";
                maps\mp\_utility::_giveweapon( var_17 );
            }

            self givemaxammo( var_17 );
            self disableweaponswitch();
            common_scripts\utility::_disableusability();

            if ( !maps\mp\_utility::_hasperk( "specialty_laststandoffhand" ) )
                self disableoffhandweapons();

            self switchtoweapon( var_17 );
            thread laststandtimer( 10, 0 );
        }
    }
}

dieaftertime( var_0 )
{
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "joined_team" );
    level endon( "game_ended" );
    wait(var_0);
    self.uselaststandparams = 1;
    maps\mp\_utility::_suicide();
}

detonateonuse()
{
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "joined_team" );
    level endon( "game_ended" );
    self waittill( "detonate" );
    self.uselaststandparams = 1;
    c4deathdetonate();
}

detonateondeath()
{
    self endon( "detonate" );
    self endon( "disconnect" );
    self endon( "joined_team" );
    level endon( "game_ended" );
    self waittill( "death" );
    c4deathdetonate();
}

c4deathdetonate()
{
    self playsound( "detpack_explo_default" );
    radiusdamage( self.origin, 312, 100, 100, self );

    if ( isalive( self ) )
        maps\mp\_utility::_suicide();
}

enablelaststandweapons()
{
    self endon( "death" );
    self endon( "disconnect" );
    level endon( "game_ended" );
    maps\mp\_utility::freezecontrolswrapper( 1 );
    wait 0.3;
    maps\mp\_utility::freezecontrolswrapper( 0 );
}

laststandtimer( var_0, var_1 )
{
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "revive" );
    level endon( "game_ended" );
    level notify( "player_last_stand" );
    thread laststandwaittilldeath();
    self.laststand = 1;

    if ( !var_1 && ( !isdefined( self.inc4death ) || !self.inc4death ) )
    {
        thread laststandallowsuicide();
        maps\mp\_utility::setlowermessage( "last_stand", &"PLATFORM_COWARDS_WAY_OUT", undefined, undefined, undefined, undefined, undefined, undefined, 1 );
        thread laststandkeepoverlay();
    }

    if ( level.diehardmode == 1 && level.diehardmode != 2 )
    {
        var_2 = spawn( "script_model", self.origin );
        var_2 setmodel( "tag_origin" );
        var_2 setcursorhint( "HINT_NOICON" );
        var_2 sethintstring( &"PLATFORM_REVIVE" );
        var_2 revivesetup( self );
        var_2 endon( "death" );
        var_3 = newteamhudelem( self.team );
        var_3 setshader( "waypoint_revive", 8, 8 );
        var_3 setwaypoint( 1, 1 );
        var_3 settargetent( self );
        var_3 thread destroyonreviveentdeath( var_2 );
        var_3.color = ( 0.33, 0.75, 0.24 );
        maps\mp\_utility::playdeathsound();

        if ( var_1 )
        {
            wait(var_0);

            if ( self.infinalstand )
                thread laststandbleedout( var_1, var_2 );
        }

        return;
    }
    else if ( level.diehardmode == 2 )
    {
        thread laststandkeepoverlay();
        var_2 = spawn( "script_model", self.origin );
        var_2 setmodel( "tag_origin" );
        var_2 setcursorhint( "HINT_NOICON" );
        var_2 sethintstring( &"PLATFORM_REVIVE" );
        var_2 revivesetup( self );
        var_2 endon( "death" );
        var_3 = newteamhudelem( self.team );
        var_3 setshader( "waypoint_revive", 8, 8 );
        var_3 setwaypoint( 1, 1 );
        var_3 settargetent( self );
        var_3 thread destroyonreviveentdeath( var_2 );
        var_3.color = ( 0.33, 0.75, 0.24 );
        maps\mp\_utility::playdeathsound();

        if ( var_1 )
        {
            wait(var_0);

            if ( self.infinalstand )
                thread laststandbleedout( var_1, var_2 );
        }

        wait(var_0 / 3);
        var_3.color = ( 1, 0.64, 0 );

        while ( var_2.inuse )
            wait 0.05;

        maps\mp\_utility::playdeathsound();
        wait(var_0 / 3);
        var_3.color = ( 1, 0, 0 );

        while ( var_2.inuse )
            wait 0.05;

        maps\mp\_utility::playdeathsound();
        wait(var_0 / 3);

        while ( var_2.inuse )
            wait 0.05;

        wait 0.05;
        thread laststandbleedout( var_1 );
        return;
    }

    thread laststandkeepoverlay();
    wait(var_0);
    thread laststandbleedout( var_1 );
}

maxhealthoverlay( var_0, var_1 )
{
    self endon( "stop_maxHealthOverlay" );
    self endon( "revive" );
    self endon( "death" );

    for (;;)
    {
        self.health -= 1;
        self.maxhealth = var_0;
        wait 0.05;
        self.maxhealth = 50;
        self.health += 1;
        wait 0.5;
    }
}

laststandbleedout( var_0, var_1 )
{
    if ( var_0 )
    {
        self.laststand = undefined;
        self.infinalstand = 0;
        self notify( "revive" );
        maps\mp\_utility::clearlowermessage( "last_stand" );
        maps\mp\gametypes\_playerlogic::laststandrespawnplayer();

        if ( isdefined( var_1 ) )
            var_1 delete();
    }
    else
    {
        self.uselaststandparams = 1;
        self.beingrevived = 0;
        maps\mp\_utility::_suicide();
    }
}

laststandallowsuicide()
{
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "game_ended" );
    self endon( "revive" );

    for (;;)
    {
        if ( self usebuttonpressed() )
        {
            var_0 = gettime();

            while ( self usebuttonpressed() )
            {
                wait 0.05;

                if ( gettime() - var_0 > 700 )
                    break;
            }

            if ( gettime() - var_0 > 700 )
                break;
        }

        wait 0.05;
    }

    thread laststandbleedout( 0 );
}

laststandkeepoverlay()
{
    level endon( "game_ended" );
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "revive" );

    while ( !level.gameended )
    {
        self.health = 2;
        wait 0.05;
        self.health = 1;
        wait 0.5;
    }

    self.health = self.maxhealth;
}

laststandwaittilldeath()
{
    self endon( "disconnect" );
    self endon( "revive" );
    level endon( "game_ended" );
    self waittill( "death" );
    maps\mp\_utility::clearlowermessage( "last_stand" );
    self.laststand = undefined;
}

maydolaststand( var_0, var_1, var_2 )
{
    if ( var_1 == "MOD_TRIGGER_HURT" )
        return 0;

    if ( var_1 != "MOD_PISTOL_BULLET" && var_1 != "MOD_RIFLE_BULLET" && var_1 != "MOD_FALLING" && var_1 != "MOD_EXPLOSIVE_BULLET" )
        return 0;

    if ( var_1 == "MOD_IMPACT" && maps\mp\gametypes\_weapons::isthrowingknife( var_0 ) )
        return 0;

    if ( var_1 == "MOD_IMPACT" && ( var_0 == "m79_mp" || issubstr( var_0, "gl_" ) ) )
        return 0;

    if ( maps\mp\_utility::isheadshot( var_0, var_2, var_1 ) )
        return 0;

    if ( maps\mp\_utility::isusingremote() )
        return 0;

    return 1;
}

ensurelaststandparamsvalidity()
{
    if ( !isdefined( self.laststandparams.attacker ) )
        self.laststandparams.attacker = self;
}

gethitlocheight( var_0 )
{
    switch ( var_0 )
    {
        case "head":
        case "helmet":
        case "neck":
            return 60;
        case "gun":
        case "torso_upper":
        case "right_arm_upper":
        case "left_arm_upper":
        case "right_arm_lower":
        case "left_arm_lower":
        case "right_hand":
        case "left_hand":
            return 48;
        case "torso_lower":
            return 40;
        case "right_leg_upper":
        case "left_leg_upper":
            return 32;
        case "right_leg_lower":
        case "left_leg_lower":
            return 10;
        case "right_foot":
        case "left_foot":
            return 5;
    }

    return 48;
}

delaystartragdoll( var_0, var_1, var_2, var_3, var_4, var_5 )
{
    if ( isdefined( var_0 ) )
    {
        var_6 = var_0 getcorpseanim();

        if ( animhasnotetrack( var_6, "ignore_ragdoll" ) )
            return;
    }

    if ( isdefined( level.noragdollents ) && level.noragdollents.size )
    {
        foreach ( var_8 in level.noragdollents )
        {
            if ( distancesquared( var_0.origin, var_8.origin ) < 65536 )
                return;
        }
    }

    wait 0.2;

    if ( !isdefined( var_0 ) )
        return;

    if ( var_0 isragdoll() )
        return;

    var_6 = var_0 getcorpseanim();
    var_10 = 0.35;

    if ( animhasnotetrack( var_6, "start_ragdoll" ) )
    {
        var_11 = getnotetracktimes( var_6, "start_ragdoll" );

        if ( isdefined( var_11 ) )
            var_10 = var_11[0];
    }

    var_12 = var_10 * getanimlength( var_6 );
    wait(var_12);

    if ( isdefined( var_0 ) )
        var_0 startragdoll();
}

getmostkilledby()
{
    var_0 = "";
    var_1 = 0;
    var_2 = getarraykeys( self.killedby );

    for ( var_3 = 0; var_3 < var_2.size; var_3++ )
    {
        var_4 = var_2[var_3];

        if ( self.killedby[var_4] <= var_1 )
            continue;

        var_1 = self.killedby[var_4];
        var_5 = var_4;
    }

    return var_0;
}

getmostkilled()
{
    var_0 = "";
    var_1 = 0;
    var_2 = getarraykeys( self.killedplayers );

    for ( var_3 = 0; var_3 < var_2.size; var_3++ )
    {
        var_4 = var_2[var_3];

        if ( self.killedplayers[var_4] <= var_1 )
            continue;

        var_1 = self.killedplayers[var_4];
        var_0 = var_4;
    }

    return var_0;
}

damageshellshockandrumble( var_0, var_1, var_2, var_3, var_4, var_5 )
{
    thread maps\mp\gametypes\_weapons::onweapondamage( var_0, var_1, var_2, var_3, var_5 );

    if ( !isai( self ) )
        self playrumbleonentity( "damage_heavy" );
}

revivesetup( var_0 )
{
    var_1 = var_0.team;
    self linkto( var_0, "tag_origin" );
    self.owner = var_0;
    self.inuse = 0;
    self makeusable();
    updateusablebyteam( var_1 );
    thread trackteamchanges( var_1 );
    thread revivetriggerthink( var_1 );
    thread deleteonreviveordeathordisconnect();
}

deleteonreviveordeathordisconnect()
{
    self endon( "death" );
    self.owner common_scripts\utility::waittill_any( "death", "disconnect" );
    self delete();
}

updateusablebyteam( var_0 )
{
    foreach ( var_2 in level.players )
    {
        if ( var_0 == var_2.team && var_2 != self.owner )
        {
            self enableplayeruse( var_2 );
            continue;
        }

        self disableplayeruse( var_2 );
    }
}

trackteamchanges( var_0 )
{
    self endon( "death" );

    for (;;)
    {
        level waittill( "joined_team" );
        updateusablebyteam( var_0 );
    }
}

tracklaststandchanges( var_0 )
{
    self endon( "death" );

    for (;;)
    {
        level waittill( "player_last_stand" );
        updateusablebyteam( var_0 );
    }
}

revivetriggerthink( var_0 )
{
    self endon( "death" );
    level endon( "game_ended" );

    for (;;)
    {
        self waittill( "trigger", var_1 );
        self.owner.beingrevived = 1;

        if ( isdefined( var_1.beingrevived ) && var_1.beingrevived )
        {
            self.owner.beingrevived = 0;
            continue;
        }

        self makeunusable();
        self.owner maps\mp\_utility::freezecontrolswrapper( 1 );
        var_2 = useholdthink( var_1 );
        self.owner.beingrevived = 0;

        if ( !isalive( self.owner ) )
        {
            self delete();
            return;
        }

        self.owner maps\mp\_utility::freezecontrolswrapper( 0 );

        if ( var_2 )
        {
            var_1 thread maps\mp\gametypes\_hud_message::splashnotifydelayed( "reviver", maps\mp\gametypes\_rank::getscoreinfovalue( "reviver" ) );
            var_1 thread maps\mp\gametypes\_rank::giverankxp( "reviver" );
            self.owner.laststand = undefined;
            self.owner maps\mp\_utility::clearlowermessage( "last_stand" );
            self.owner.movespeedscaler = 1;

            if ( self.owner maps\mp\_utility::_hasperk( "specialty_lightweight" ) )
                self.owner.movespeedscaler = maps\mp\_utility::lightweightscalar();

            self.owner common_scripts\utility::_enableweapon();
            self.owner.maxhealth = 100;
            self.owner maps\mp\gametypes\_weapons::updatemovespeedscale();
            self.owner maps\mp\gametypes\_playerlogic::laststandrespawnplayer();
            self.owner maps\mp\_utility::giveperk( "specialty_pistoldeath", 0 );
            self.owner.beingrevived = 0;
            self delete();
            return;
        }

        self makeusable();
        updateusablebyteam( var_0 );
    }
}

useholdthink( var_0, var_1 )
{
    var_2 = 3000;
    var_3 = spawn( "script_origin", self.origin );
    var_3 hide();
    var_0 playerlinkto( var_3 );
    var_0 playerlinkedoffsetenable();
    var_0 common_scripts\utility::_disableweapon();
    self.curprogress = 0;
    self.inuse = 1;
    self.userate = 0;

    if ( isdefined( var_1 ) )
        self.usetime = var_1;
    else
        self.usetime = var_2;

    var_4 = useholdthinkloop( var_0 );
    self.inuse = 0;
    var_3 delete();

    if ( isdefined( var_0 ) && maps\mp\_utility::isreallyalive( var_0 ) )
    {
        var_0 unlink();
        var_0 common_scripts\utility::_enableweapon();
    }

    if ( isdefined( var_4 ) && var_4 )
    {
        self.owner thread maps\mp\gametypes\_hud_message::playercardsplashnotify( "revived", var_0 );
        self.owner.inlaststand = 0;
        return 1;
    }

    return 0;
}

useholdthinkloop( var_0 )
{
    level endon( "game_ended" );
    self.owner endon( "death" );
    self.owner endon( "disconnect" );

    while ( maps\mp\_utility::isreallyalive( var_0 ) && var_0 usebuttonpressed() && self.curprogress < self.usetime && ( !isdefined( var_0.laststand ) || !var_0.laststand ) )
    {
        self.curprogress += 50 * self.userate;
        self.userate = 1;
        var_0 maps\mp\gametypes\_gameobjects::updateuiprogress( self, 1 );
        self.owner maps\mp\gametypes\_gameobjects::updateuiprogress( self, 1 );

        if ( self.curprogress >= self.usetime )
        {
            self.inuse = 0;
            var_0 maps\mp\gametypes\_gameobjects::updateuiprogress( self, 0 );
            self.owner maps\mp\gametypes\_gameobjects::updateuiprogress( self, 0 );
            return maps\mp\_utility::isreallyalive( var_0 );
        }

        wait 0.05;
    }

    var_0 maps\mp\gametypes\_gameobjects::updateuiprogress( self, 0 );
    self.owner maps\mp\gametypes\_gameobjects::updateuiprogress( self, 0 );
    return 0;
}

callback_killingblow( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9 )
{
    if ( isdefined( self.lastdamagewasfromenemy ) && self.lastdamagewasfromenemy && var_2 >= self.health && isdefined( self.combathigh ) && self.combathigh == "specialty_endgame" )
    {
        maps\mp\_utility::giveperk( "specialty_endgame", 0 );
        return 0;
    }

    return 1;
}

emitfalldamage( var_0 )
{
    physicsexplosionsphere( self.origin, 64, 64, 1 );
    var_1 = [];

    for ( var_2 = 0; var_2 < 360; var_2 += 30 )
    {
        var_3 = cos( var_2 ) * 16;
        var_4 = sin( var_2 ) * 16;
        var_5 = bullettrace( self.origin + ( var_3, var_4, 4 ), self.origin + ( var_3, var_4, -6 ), 1, self );

        if ( isdefined( var_5["entity"] ) && isdefined( var_5["entity"].targetname ) && ( var_5["entity"].targetname == "destructible_vehicle" || var_5["entity"].targetname == "destructible_toy" ) )
            var_1[var_1.size] = var_5["entity"];
    }

    if ( var_1.size )
    {
        var_6 = spawn( "script_origin", self.origin );
        var_6 hide();
        var_6.type = "soft_landing";
        var_6.destructibles = var_1;
        radiusdamage( self.origin, 64, 100, 100, var_6 );
        wait 0.1;
        var_6 delete();
    }
}

isflankkill( var_0, var_1 )
{
    var_2 = anglestoforward( var_0.angles );
    var_2 = ( var_2[0], var_2[1], 0 );
    var_2 = vectornormalize( var_2 );
    var_3 = var_0.origin - var_1.origin;
    var_3 = ( var_3[0], var_3[1], 0 );
    var_3 = vectornormalize( var_3 );
    var_4 = vectordot( var_2, var_3 );

    if ( var_4 > 0 )
        return 1;
    else
        return 0;
}

logprintplayerdeath( var_0, var_1, var_2, var_3, var_4, var_5, var_6 )
{
    var_7 = self getentitynumber();
    var_8 = self.name;
    var_9 = self.team;
    var_10 = self.guid;

    if ( isplayer( var_1 ) )
    {
        var_11 = var_1.guid;
        var_12 = var_1.name;
        var_13 = var_1.team;
        var_14 = var_1 getentitynumber();
        var_15 = var_1 getxuid() + "(" + var_12 + ")";
    }
    else
    {
        var_11 = "";
        var_12 = "";
        var_13 = "world";
        var_14 = -1;
        var_15 = "none";
    }

    logprint( "k;" + var_10 + ";" + var_7 + ";" + var_9 + ";" + var_8 + ";" + var_11 + ";" + var_14 + ";" + var_13 + ";" + var_12 + ";" + var_4 + ";" + var_2 + ";" + var_3 + ";" + var_6 + "\n" );
}

destroyonreviveentdeath( var_0 )
{
    var_0 waittill( "death" );
    self destroy();
}

gamemodemodifyplayerdamage( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7 )
{
    if ( isdefined( var_1 ) && isplayer( var_1 ) && isalive( var_1 ) )
    {
        if ( level.matchrules_damagemultiplier )
            var_2 *= level.matchrules_damagemultiplier;

        if ( level.matchrules_vampirism )
            var_1.health = int( min( float( var_1.maxhealth ), float( var_1.health + 20 ) ) );
    }

    return var_2;
}

registerkill( var_0, var_1 )
{
    thread maps\mp\killstreaks\_killstreaks::giveadrenaline( "kill" );
    self.pers["cur_kill_streak"]++;

    if ( var_1 )
        self notify( "kill_streak_increased" );

    var_2 = !maps\mp\_utility::iskillstreakweapon( var_0 );

    if ( var_2 )
        self.pers["cur_kill_streak_for_nuke"]++;

    var_3 = 25;

    if ( maps\mp\_utility::_hasperk( "specialty_hardline" ) )
        var_3--;

    if ( var_2 && self.pers["cur_kill_streak_for_nuke"] == var_3 && !maps\mp\_utility::isanymlgmatch() )
    {
        if ( !isdefined( level.supportnuke ) || level.supportnuke )
            giveultimatekillstreak( var_3 );
    }
}

giveultimatekillstreak( var_0 )
{
    thread maps\mp\killstreaks\_killstreaks::givekillstreak( "nuke", 0, 1, self );
    thread maps\mp\gametypes\_hud_message::killstreaksplashnotify( "nuke", var_0 );
}

monitordamage( var_0, var_1, var_2, var_3, var_4, var_5 )
{
    self endon( "death" );
    level endon( "game_ended" );

    if ( !isdefined( var_5 ) )
        var_5 = 0;

    self setcandamage( 1 );
    self.health = 999999;
    self.maxhealth = var_0;
    self.damagetaken = 0;

    if ( !isdefined( var_4 ) )
        var_4 = 0;

    for ( var_6 = 1; var_6; var_6 = monitordamageoneshot( var_7, var_8, var_9, var_10, var_11, var_12, var_13, var_14, var_15, var_16, var_1, var_2, var_3, var_4 ) )
    {
        self waittill( "damage", var_7, var_8, var_9, var_10, var_11, var_12, var_13, var_14, var_15, var_16 );

        if ( var_5 )
            self playrumbleonentity( "damage_light" );

        if ( isdefined( self.helitype ) && self.helitype == "littlebird" )
        {
            if ( !isdefined( self.attackers ) )
                self.attackers = [];

            var_17 = "";

            if ( isdefined( var_8 ) && isplayer( var_8 ) )
                var_17 = var_8 maps\mp\_utility::getuniqueid();

            if ( isdefined( self.attackers[var_17] ) )
                self.attackers[var_17] += var_7;
            else
                self.attackers[var_17] = var_7;
        }
    }
}

monitordamageoneshot( var_0, var_1, var_2, var_3, var_4, var_5, var_6, var_7, var_8, var_9, var_10, var_11, var_12, var_13 )
{
    if ( !isdefined( self ) )
        return 0;

    if ( isdefined( var_1 ) && !maps\mp\_utility::isgameparticipant( var_1 ) && !isdefined( var_1.allowmonitoreddamage ) )
        return 1;

    if ( maps\mp\_utility::is_aliens() || isdefined( var_1 ) && !maps\mp\gametypes\_weapons::friendlyfirecheck( self.owner, var_1 ) )
        return 1;

    var_14 = var_0;

    if ( isdefined( var_9 ) )
    {
        switch ( var_9 )
        {
            case "flash_grenade_mp":
            case "smoke_grenade_mp":
            case "smoke_grenadejugg_mp":
            case "concussion_grenade_mp":
                return 1;
        }

        if ( !isdefined( var_12 ) )
            var_12 = ::modifydamage;

        var_14 = [[ var_12 ]]( var_1, var_9, var_4, var_0 );
    }

    if ( var_14 < 0 )
        return 1;

    self.wasdamaged = 1;
    self.damagetaken += var_14;

    if ( isdefined( var_8 ) && var_8 & level.idflags_penetration )
        self.wasdamagedfrombulletpenetration = 1;

    if ( var_13 )
        maps\mp\killstreaks\_killstreaks::killstreakhit( var_1, var_9, self );

    if ( isdefined( var_1 ) )
    {
        if ( isplayer( var_1 ) )
        {
            if ( self.damagetaken >= self.maxhealth )
                var_10 = "hitkill";

            var_1 maps\mp\gametypes\_damagefeedback::updatedamagefeedback( var_10 );
        }
        else if ( isdefined( var_1.owner ) && isplayer( var_1.owner ) )
            var_1.owner maps\mp\gametypes\_damagefeedback::updatedamagefeedback( var_10 );
    }

    if ( self.damagetaken >= self.maxhealth )
    {
        self thread [[ var_11 ]]( var_1, var_9, var_4, var_0 );
        return 0;
    }

    return 1;
}

modifydamage( var_0, var_1, var_2, var_3 )
{
    var_4 = var_3;
    var_4 = handleempdamage( var_1, var_2, var_4 );
    var_4 = handlemissiledamage( var_1, var_2, var_4 );
    var_4 = handlegrenadedamage( var_1, var_2, var_4 );
    var_4 = handleapdamage( var_1, var_2, var_4, var_0 );
    return var_4;
}

handlemissiledamage( var_0, var_1, var_2 )
{
    var_3 = var_2;

    switch ( var_0 )
    {
        case "bomb_site_mp":
        case "iw6_maawschild_mp":
        case "iw6_maawshoming_mp":
        case "iw6_maaws_mp":
        case "iw6_panzerfaust3_mp":
        case "drone_hive_projectile_mp":
        case "odin_projectile_large_rod_mp":
        case "odin_projectile_small_rod_mp":
        case "aamissile_projectile_mp":
        case "ac130_105mm_mp":
        case "ac130_40mm_mp":
        case "maverick_projectile_mp":
            self.largeprojectiledamage = 1;
            var_3 = self.maxhealth + 1;
            break;
        case "remote_tank_projectile_mp":
        case "switch_blade_child_mp":
        case "hind_bomb_mp":
        case "hind_missile_mp":
            self.largeprojectiledamage = 0;
            var_3 = self.maxhealth + 1;
            break;
        case "heli_pilot_turret_mp":
        case "a10_30mm_turret_mp":
            self.largeprojectiledamage = 0;
            var_3 *= 2;
            break;
        case "sam_projectile_mp":
            self.largeprojectiledamage = 1;
            var_3 = var_2;
            break;
    }

    return var_3;
}

handlegrenadedamage( var_0, var_1, var_2 )
{
    if ( isexplosivedamagemod( var_1 ) )
    {
        switch ( var_0 )
        {
            case "mortar_shell_mp":
            case "c4_mp":
            case "proximity_explosive_mp":
            case "iw6_rgm_mp":
                var_2 *= 3;
                break;
            case "frag_grenade_mp":
            case "semtex_mp":
            case "semtexproj_mp":
            case "iw6_mk32_mp":
                var_2 *= 4;
                break;
            default:
                if ( maps\mp\_utility::isstrstart( var_0, "alt_" ) )
                    var_2 *= 3;

                break;
        }
    }

    return var_2;
}

handlemeleedamage( var_0, var_1, var_2 )
{
    if ( var_1 == "MOD_MELEE" )
        return self.maxhealth + 1;

    return var_2;
}

handleempdamage( var_0, var_1, var_2 )
{
    if ( var_0 == "emp_grenade_mp" && var_1 == "MOD_GRENADE_SPLASH" )
    {
        self notify( "emp_damage", var_0.owner, 8.0 );
        return 0;
    }

    return var_2;
}

handleapdamage( var_0, var_1, var_2, var_3 )
{
    if ( var_1 == "MOD_RIFLE_BULLET" || var_1 == "MOD_PISTOL_BULLET" )
    {
        if ( var_3 maps\mp\_utility::_hasperk( "specialty_armorpiercing" ) || maps\mp\_utility::isfmjdamage( var_0, var_1, var_3 ) )
            return var_2 * level.armorpiercingmod;
    }

    return var_2;
}

onkillstreakkilled( var_0, var_1, var_2, var_3, var_4, var_5, var_6 )
{
    var_7 = 0;
    var_8 = undefined;

    if ( isdefined( var_0 ) && isdefined( self.owner ) )
    {
        if ( isdefined( var_0.owner ) && isplayer( var_0.owner ) )
            var_0 = var_0.owner;

        if ( self.owner maps\mp\_utility::isenemy( var_0 ) )
            var_8 = var_0;
    }

    if ( isdefined( var_8 ) )
    {
        var_8 notify( "destroyed_killstreak", var_1 );
        var_9 = 100;

        if ( isdefined( var_4 ) )
        {
            var_9 = maps\mp\gametypes\_rank::getscoreinfovalue( var_4 );
            var_8 thread maps\mp\gametypes\_rank::xpeventpopup( var_4 );
        }

        var_8 thread maps\mp\gametypes\_rank::giverankxp( "kill", var_9, var_1, var_2 );

        if ( isdefined( var_6 ) )
            thread maps\mp\_utility::teamplayercardsplash( var_6, var_8 );

        thread maps\mp\gametypes\_missions::killstreakkilled( self.owner, self, undefined, var_8, var_3, var_2, var_1 );
        var_7 = 1;
    }

    if ( isdefined( self.owner ) && isdefined( var_5 ) )
        self.owner thread maps\mp\_utility::leaderdialogonplayer( var_5, undefined, undefined, self.origin );

    self notify( "death" );
    return var_7;
}
