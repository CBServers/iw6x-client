// IW6 GSC SOURCE
// Dumped by https://github.com/xensik/gsc-tool

init()
{
    if ( !isdefined( game["gamestarted"] ) )
    {
        game["menu_team"] = "team_marinesopfor";

        if ( level.multiteambased )
            game["menu_team"] = "team_mt_options";

        if ( maps\mp\_utility::bot_is_fireteam_mode() )
        {
            level.fireteam_menu = "class_commander_" + level.gametype;
            game["menu_class"] = level.fireteam_menu;
            game["menu_class_allies"] = level.fireteam_menu;
            game["menu_class_axis"] = level.fireteam_menu;
        }
        else
        {
            game["menu_class"] = "class";
            game["menu_class_allies"] = "class_marines";
            game["menu_class_axis"] = "class_opfor";
        }

        game["menu_changeclass_allies"] = "changeclass_marines";
        game["menu_changeclass_axis"] = "changeclass_opfor";

        if ( level.multiteambased )
        {
            for ( var_0 = 0; var_0 < level.teamnamelist.size; var_0++ )
            {
                var_1 = "menu_class_" + level.teamnamelist[var_0];
                var_2 = "menu_changeclass_" + level.teamnamelist[var_0];
                game[var_1] = game["menu_class_allies"];
                game[var_2] = "changeclass_marines";
            }
        }

        game["menu_changeclass"] = "changeclass";

        if ( level.console )
        {
            game["menu_controls"] = "ingame_controls";

            if ( level.splitscreen )
            {
                if ( level.multiteambased )
                {
                    for ( var_0 = 0; var_0 < level.teamnamelist.size; var_0++ )
                    {
                        var_1 = "menu_class_" + level.teamnamelist[var_0];
                        var_2 = "menu_changeclass_" + level.teamnamelist[var_0];
                        game[var_1] += "_splitscreen";
                        game[var_2] += "_splitscreen";
                    }
                }

                game["menu_team"] += "_splitscreen";
                game["menu_class_allies"] += "_splitscreen";
                game["menu_class_axis"] += "_splitscreen";
                game["menu_changeclass_allies"] += "_splitscreen";
                game["menu_changeclass_axis"] += "_splitscreen";
                game["menu_controls"] += "_splitscreen";
                game["menu_changeclass_defaults_splitscreen"] = "changeclass_splitscreen_defaults";
                game["menu_changeclass_custom_splitscreen"] = "changeclass_splitscreen_custom";
                precachemenu( game["menu_changeclass_defaults_splitscreen"] );
                precachemenu( game["menu_changeclass_custom_splitscreen"] );
            }

            precachemenu( game["menu_controls"] );
        }

        precachemenu( game["menu_team"] );
        precachemenu( game["menu_class_allies"] );
        precachemenu( game["menu_class_axis"] );
        precachemenu( game["menu_changeclass"] );
        precachemenu( game["menu_changeclass_allies"] );
        precachemenu( game["menu_changeclass_axis"] );
        precachemenu( game["menu_class"] );
        precachestring( &"MP_HOST_ENDED_GAME" );
        precachestring( &"MP_HOST_ENDGAME_RESPONSE" );
    }

    level thread onplayerconnect();
}

onplayerconnect()
{
    for (;;)
    {
        level waittill( "connected", var_0 );
        var_0 thread watchforclasschange();
        var_0 thread watchforteamchange();
        var_0 thread watchforleavegame();
        var_0 thread connectedmenus();
    }
}

connectedmenus()
{

}

getclasschoice( var_0 )
{
    if ( var_0 > 10 )
    {
        if ( var_0 > 10 && var_0 < 17 )
        {
            var_0 -= 10;
            var_0 = "axis_recipe" + var_0;
        }
        else if ( var_0 > 16 && var_0 < 23 )
        {
            var_0 -= 16;
            var_0 = "allies_recipe" + var_0;
        }
    }
    else
        var_0 = "custom" + var_0;

    return var_0;
}

watchforclasschange()
{
    self endon( "disconnect" );
    level endon( "game_ended" );

    for (;;)
    {
        self waittill( "luinotifyserver", var_0, var_1 );

        if ( var_0 != "class_select" )
            continue;

        if ( getdvarint( "systemlink" ) && getdvarint( "xblive_competitionmatch" ) && self ismlgspectator() )
        {
            self setclientomnvar( "ui_options_menu", 0 );
            continue;
        }

        var_2 = isai( self ) || issubstr( self.name, "tcBot" );

        if ( !var_2 )
        {
            if ( !isai( self ) && "" + var_1 != "callback" )
                self setclientomnvar( "ui_loadout_selected", var_1 );
        }

        if ( isdefined( self.waitingtoselectclass ) && self.waitingtoselectclass )
            continue;

        if ( !maps\mp\_utility::allowclasschoice() || maps\mp\_utility::showfakeloadout() )
            continue;

        if ( "" + var_1 != "callback" )
        {
            if ( isdefined( self.pers["isBot"] ) && self.pers["isBot"] )
            {
                self.pers["class"] = var_1;
                self.class = var_1;
            }
            else
            {
                var_3 = var_1 + 1;
                var_3 = getclasschoice( var_3 );

                if ( !isdefined( self.pers["class"] ) || var_3 == self.pers["class"] )
                    continue;

                self.pers["class"] = var_3;
                self.class = var_3;

                if ( level.ingraceperiod && !self.hasdonecombat )
                {
                    maps\mp\gametypes\_class::setclass( self.pers["class"] );
                    self.tag_stowed_back = undefined;
                    self._id_7E62 = undefined;
                    maps\mp\gametypes\_class::giveloadout( self.pers["team"], self.pers["class"] );
                }
                else if ( isalive( self ) )
                    self iprintlnbold( game["strings"]["change_class"] );
            }

            continue;
        }

        menuclass( "callback" );
    }
}

watchforleavegame()
{
    self endon( "disconnect" );
    level endon( "game_ended" );

    for (;;)
    {
        self waittill( "luinotifyserver", var_0, var_1 );

        if ( var_0 != "end_game" )
            continue;

        if ( maps\mp\_utility::is_aliens() )
        {
            [[ level.forceendgame_alien ]]();
            continue;
        }

        level thread maps\mp\gametypes\_gamelogic::forceend( var_1 );
    }
}

watchforteamchange()
{
    self endon( "disconnect" );
    level endon( "game_ended" );

    for (;;)
    {
        self waittill( "luinotifyserver", var_0, var_1 );

        if ( var_0 != "team_select" )
            continue;

        if ( maps\mp\_utility::matchmakinggame() && !getdvarint( "force_ranking" ) )
            continue;

        if ( var_1 != 3 )
            thread showloadoutmenu();

        if ( var_1 == 3 )
        {
            self setclientomnvar( "ui_spectator_selected", 1 );
            self setclientomnvar( "ui_loadout_selected", -1 );
            self.spectating_actively = 1;

            if ( getdvarint( "systemlink" ) && getdvarint( "xblive_competitionmatch" ) )
            {
                self setmlgspectator( 1 );
                self.pers["mlgSpectator"] = 1;
                thread maps\mp\gametypes\_spectating::setmlgcamvisibility( 1 );
                thread maps\mp\gametypes\_spectating::setspectatepermissions();
                self setclientomnvar( "ui_options_menu", 2 );
            }
        }
        else
        {
            self setclientomnvar( "ui_spectator_selected", -1 );
            self.spectating_actively = 0;

            if ( getdvarint( "systemlink" ) && getdvarint( "xblive_competitionmatch" ) )
            {
                self setmlgspectator( 0 );
                self.pers["mlgSpectator"] = 0;
                thread maps\mp\gametypes\_spectating::setmlgcamvisibility( 0 );
            }
        }

        self setclientomnvar( "ui_team_selected", var_1 );

        if ( var_1 == 0 )
            var_1 = "axis";
        else if ( var_1 == 1 )
            var_1 = "allies";
        else if ( var_1 == 2 )
            var_1 = "random";
        else
            var_1 = "spectator";

        if ( isdefined( self.pers["team"] ) && var_1 == self.pers["team"] )
        {
            self notify( "selected_same_team" );
            continue;
        }

        self setclientomnvar( "ui_loadout_selected", -1 );

        if ( var_1 == "axis" )
        {
            thread setteam( "axis" );
            continue;
        }

        if ( var_1 == "allies" )
        {
            thread setteam( "allies" );
            continue;
        }

        if ( var_1 == "random" )
        {
            thread autoassign();
            continue;
        }

        if ( var_1 == "spectator" )
            thread setspectator();
    }
}

showloadoutmenu()
{
    self endon( "disconnect" );
    level endon( "game_ended" );
    common_scripts\utility::waittill_any( "joined_team", "selected_same_team" );
    self setclientomnvar( "ui_options_menu", 2 );
}

autoassign()
{
    if ( maps\mp\_utility::is_aliens() || level.gametype == "infect" )
        thread setteam( "allies" );
    else if ( ( getdvarint( "squad_match" ) == 1 || getdvarint( "squad_vs_squad" ) == 1 || getdvarint( "squad_use_hosts_squad" ) == 1 ) && isdefined( self.bot_team ) )
        thread setteam( self.bot_team );
    else if ( !isdefined( self.team ) )
    {
        if ( self ismlgspectator() )
            thread setspectator();
        else if ( level.teamcount["axis"] < level.teamcount["allies"] )
            thread setteam( "axis" );
        else if ( level.teamcount["allies"] < level.teamcount["axis"] )
            thread setteam( "allies" );
        else if ( getteamscore( "allies" ) > getteamscore( "axis" ) )
            thread setteam( "axis" );
        else
            thread setteam( "allies" );
    }
    else
    {
        if ( self ismlgspectator() )
        {
            thread setspectator();
            return;
        }

        if ( level.teamcount["axis"] < level.teamcount["allies"] && self.team != "axis" )
        {
            thread setteam( "axis" );
            return;
        }

        if ( level.teamcount["allies"] < level.teamcount["axis"] && self.team != "allies" )
        {
            thread setteam( "allies" );
            return;
        }

        if ( level.teamcount["allies"] == level.teamcount["axis"] )
        {
            if ( getteamscore( "allies" ) > getteamscore( "axis" ) && self.team != "axis" )
                thread setteam( "axis" );
            else if ( self.team != "allies" )
                thread setteam( "allies" );
        }
    }
}

setteam( var_0 )
{
    self endon( "disconnect" );

    if ( !isai( self ) && level.teambased && !maps\mp\gametypes\_teams::getjointeampermissions( var_0 ) )
        return;

    if ( level.ingraceperiod && !self.hasdonecombat )
        self.hasspawned = 0;

    if ( self.sessionstate == "playing" )
    {
        self.switching_teams = 1;
        self.joining_team = var_0;
        self.leaving_team = self.pers["team"];
    }

    addtoteam( var_0 );

    if ( self.sessionstate == "playing" )
        self suicide();

    waitforclassselect();
    endrespawnnotify();

    if ( self.sessionstate == "spectator" )
    {
        if ( game["state"] == "postgame" )
            return;

        if ( game["state"] == "playing" && !maps\mp\_utility::isinkillcam() )
        {
            if ( isdefined( self.waitingtospawnamortize ) && self.waitingtospawnamortize )
                return;

            thread maps\mp\gametypes\_playerlogic::spawnclient();
        }

        thread maps\mp\gametypes\_spectating::setspectatepermissions();
    }

    self notify( "okToSpawn" );
}

setspectator()
{
    if ( isdefined( self.pers["team"] ) && self.pers["team"] == "spectator" )
        return;

    if ( isalive( self ) )
    {
        self.switching_teams = 1;
        self.joining_team = "spectator";
        self.leaving_team = self.pers["team"];
        self suicide();
    }

    self notify( "becameSpectator" );
    addtoteam( "spectator" );
    self.pers["class"] = undefined;
    self.class = undefined;
    thread maps\mp\gametypes\_playerlogic::spawnspectator();
}

waitforclassselect()
{
    self endon( "disconnect" );
    level endon( "game_ended" );
    self.waitingtoselectclass = 1;

    for (;;)
    {
        if ( level.gametype == "infect" )
        {
            bypassclasschoice();
            break;
        }

        if ( maps\mp\_utility::allowclasschoice() || maps\mp\_utility::showfakeloadout() && !isai( self ) )
            self waittill( "luinotifyserver", var_0, var_1 );
        else
        {
            bypassclasschoice();
            break;
        }

        if ( var_0 != "class_select" )
            continue;

        if ( self.team == "spectator" )
            continue;

        if ( "" + var_1 != "callback" )
        {
            if ( isdefined( self.pers["isBot"] ) && self.pers["isBot"] )
            {
                self.pers["class"] = var_1;
                self.class = var_1;
            }
            else
            {
                var_1 += 1;
                self.pers["class"] = getclasschoice( var_1 );
                self.class = getclasschoice( var_1 );
            }

            self.waitingtoselectclass = 0;
        }
        else
        {
            self.waitingtoselectclass = 0;
            menuclass( "callback" );
        }

        break;
    }
}

beginclasschoice( var_0 )
{
    var_1 = self.pers["team"];

    if ( maps\mp\_utility::allowclasschoice() || maps\mp\_utility::showfakeloadout() && !isai( self ) )
    {
        self setclientomnvar( "ui_options_menu", 2 );

        if ( !self ismlgspectator() )
            waitforclassselect();

        endrespawnnotify();

        if ( self.sessionstate == "spectator" )
        {
            if ( game["state"] == "postgame" )
                return;

            if ( game["state"] == "playing" && !maps\mp\_utility::isinkillcam() )
            {
                if ( isdefined( self.waitingtospawnamortize ) && self.waitingtospawnamortize )
                    return;

                thread maps\mp\gametypes\_playerlogic::spawnclient();
            }

            thread maps\mp\gametypes\_spectating::setspectatepermissions();
        }

        self.connecttime = gettime();
        self notify( "okToSpawn" );
    }
    else
        thread bypassclasschoice();

    if ( !isalive( self ) )
        thread maps\mp\gametypes\_playerlogic::predictabouttospawnplayerovertime( 0.1 );
}

bypassclasschoice()
{
    self.selectedclass = 1;
    self.waitingtoselectclass = 0;

    if ( isdefined( level.bypassclasschoicefunc ) )
    {
        var_0 = self [[ level.bypassclasschoicefunc ]]();
        self.class = var_0;
    }
    else
        self.class = "class0";
}

beginteamchoice()
{
    self setclientomnvar( "ui_options_menu", 1 );
}

showmainmenuforteam()
{
    var_0 = self.pers["team"];
    self openpopupmenu( game["menu_class_" + var_0] );
}

menuspectator()
{
    if ( isdefined( self.pers["team"] ) && self.pers["team"] == "spectator" )
        return;

    if ( isalive( self ) )
    {
        self.switching_teams = 1;
        self.joining_team = "spectator";
        self.leaving_team = self.pers["team"];
        self suicide();
    }

    addtoteam( "spectator" );
    self.pers["class"] = undefined;
    self.class = undefined;
    thread maps\mp\gametypes\_playerlogic::spawnspectator();
}

menuclass( var_0 )
{
    if ( var_0 == "demolitions_mp,0" && self getrankedplayerdata( "featureNew", "demolitions" ) )
        self setrankedplayerdata( "featureNew", "demolitions", 0 );

    if ( var_0 == "sniper_mp,0" && self getrankedplayerdata( "featureNew", "sniper" ) )
        self setrankedplayerdata( "featureNew", "sniper", 0 );

    var_1 = self.pers["team"];
    var_2 = maps\mp\gametypes\_class::getclasschoice( var_0 );
    var_3 = maps\mp\gametypes\_class::getweaponchoice( var_0 );

    if ( var_2 == "restricted" )
    {
        beginclasschoice();
        return;
    }

    if ( isdefined( self.pers["class"] ) && self.pers["class"] == var_2 && ( isdefined( self.pers["primary"] ) && self.pers["primary"] == var_3 ) )
        return;

    if ( self.sessionstate == "playing" )
    {
        if ( isdefined( self.pers["lastClass"] ) && isdefined( self.pers["class"] ) )
        {
            self.pers["lastClass"] = self.pers["class"];
            self.lastclass = self.pers["lastClass"];
        }

        self.pers["class"] = var_2;
        self.class = var_2;
        self.pers["primary"] = var_3;

        if ( game["state"] == "postgame" )
            return;

        if ( level.ingraceperiod && !self.hasdonecombat )
        {
            maps\mp\gametypes\_class::setclass( self.pers["class"] );
            self.tag_stowed_back = undefined;
            self._id_7E62 = undefined;
            maps\mp\gametypes\_class::giveloadout( self.pers["team"], self.pers["class"] );
        }
        else
            self iprintlnbold( game["strings"]["change_class"] );
    }
    else
    {
        if ( isdefined( self.pers["lastClass"] ) && isdefined( self.pers["class"] ) )
        {
            self.pers["lastClass"] = self.pers["class"];
            self.lastclass = self.pers["lastClass"];
        }

        self.pers["class"] = var_2;
        self.class = var_2;
        self.pers["primary"] = var_3;

        if ( game["state"] == "postgame" )
            return;

        if ( game["state"] == "playing" && !maps\mp\_utility::isinkillcam() )
            thread maps\mp\gametypes\_playerlogic::spawnclient();
    }

    thread maps\mp\gametypes\_spectating::setspectatepermissions();
}

update_wargame_after_migration()
{
    foreach ( var_1 in level.players )
    {
        if ( !isai( var_1 ) && var_1 ishost() )
            level.wargame_client = var_1;
    }
}

addtoteam( var_0, var_1, var_2 )
{
    if ( isdefined( self.team ) )
    {
        maps\mp\gametypes\_playerlogic::removefromteamcount();

        if ( isdefined( var_2 ) && var_2 )
            maps\mp\gametypes\_playerlogic::decrementalivecount( self.team );
    }

    self.pers["team"] = var_0;
    self.team = var_0;

    if ( getdvar( "squad_vs_squad" ) == "1" )
    {
        if ( !isai( self ) )
        {
            if ( var_0 == "allies" )
            {
                if ( !isdefined( level.squad_vs_squad_allies_client ) )
                    level.squad_vs_squad_allies_client = self;
            }
            else if ( var_0 == "axis" )
            {
                if ( !isdefined( level.squad_vs_squad_axis_client ) )
                    level.squad_vs_squad_axis_client = self;
            }
        }
    }

    if ( getdvar( "squad_match" ) == "1" )
    {
        if ( !isai( self ) && self ishost() )
        {
            if ( !isdefined( level.squad_match_client ) )
                level.squad_match_client = self;
        }
    }

    if ( getdvar( "squad_use_hosts_squad" ) == "1" )
    {
        if ( !isai( self ) && self ishost() )
        {
            if ( !isdefined( level.wargame_client ) )
                level.wargame_client = self;
        }
    }

   // session team is readonly in ranked matches if "teambased" is set on the playlist
    if ( level.teambased )
        self.sessionteam = var_0;
    else if ( var_0 == "spectator" )
        self.sessionteam = "spectator";
    else
        self.sessionteam = "none";

    if ( game["state"] != "postgame" )
    {
        maps\mp\gametypes\_playerlogic::addtoteamcount();

        if ( isdefined( var_2 ) && var_2 )
            maps\mp\gametypes\_playerlogic::incrementalivecount( self.team );
    }

    maps\mp\_utility::updateobjectivetext();

    if ( isdefined( var_1 ) && var_1 )
        waittillframeend;

    maps\mp\_utility::updatemainmenu();

    if ( var_0 == "spectator" )
    {
        self notify( "joined_spectators" );
        level notify( "joined_team", self );
    }
    else
    {
        self notify( "joined_team" );
        level notify( "joined_team", self );
    }
}

endrespawnnotify()
{
    self.waitingtospawn = 0;
    self notify( "end_respawn" );
}
