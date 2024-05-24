// IW6 GSC SOURCE
// Dumped by https://github.com/xensik/gsc-tool

timeuntilwavespawn( var_0 )
{
    if ( !self.hasspawned )
        return 0;

    var_1 = gettime() + var_0 * 1000;
    var_2 = level.lastwave[self.pers["team"]];
    var_3 = level.wavedelay[self.pers["team"]] * 1000;
    var_4 = ( var_1 - var_2 ) / var_3;
    var_5 = ceil( var_4 );
    var_6 = var_2 + var_5 * var_3;

    if ( isdefined( self.respawntimerstarttime ) )
    {
        var_7 = ( gettime() - self.respawntimerstarttime ) / 1000.0;

        if ( self.respawntimerstarttime < var_2 )
            return 0;
    }

    if ( isdefined( self.wavespawnindex ) )
        var_6 += 50 * self.wavespawnindex;

    return ( var_6 - gettime() ) / 1000;
}

teamkilldelay()
{
    var_0 = self.pers["teamkills"];

    if ( level.maxallowedteamkills < 0 || var_0 <= level.maxallowedteamkills )
        return 0;

    var_1 = var_0 - level.maxallowedteamkills;
    return maps\mp\gametypes\_tweakables::gettweakablevalue( "team", "teamkillspawndelay" ) * var_1;
}

timeuntilspawn( var_0 )
{
    if ( level.ingraceperiod && !self.hasspawned || level.gameended )
        return 0;

    var_1 = 0;

    if ( self.hasspawned )
    {
        var_2 = self [[ level.onrespawndelay ]]();

        if ( isdefined( var_2 ) )
            var_1 = var_2;
        else
            var_1 = getdvarint( "scr_" + level.gametype + "_playerrespawndelay" );

        if ( var_0 && isdefined( self.pers["teamKillPunish"] ) && self.pers["teamKillPunish"] )
            var_1 += teamkilldelay();

        if ( isdefined( self.respawntimerstarttime ) )
        {
            var_3 = ( gettime() - self.respawntimerstarttime ) / 1000.0;
            var_1 -= var_3;

            if ( var_1 < 0 )
                var_1 = 0;
        }

        if ( isdefined( self.setspawnpoint ) )
            var_1 += level.tispawndelay;
    }

    var_4 = getdvarint( "scr_" + level.gametype + "_waverespawndelay" ) > 0;

    if ( var_4 )
        return timeuntilwavespawn( var_1 );

    return var_1;
}

mayspawn()
{
    if ( maps\mp\_utility::getgametypenumlives() || isdefined( level.disablespawning ) )
    {
        if ( isdefined( level.disablespawning ) && level.disablespawning )
            return 0;

        if ( isdefined( self.pers["teamKillPunish"] ) && self.pers["teamKillPunish"] )
            return 0;

        if ( self.pers["lives"] <= 0 && maps\mp\_utility::gamehasstarted() )
            return 0;
        else if ( maps\mp\_utility::gamehasstarted() )
        {
            if ( !level.ingraceperiod && !self.hasspawned && ( isdefined( level.allowlatecomers ) && !level.allowlatecomers ) )
            {
                if ( isdefined( self.siegelatecomer ) && !self.siegelatecomer )
                    return 1;

                return 0;
            }
        }
    }

    return 1;
}

spawnclient()
{
    self endon( "becameSpectator" );

    if ( isdefined( self.waitingtoselectclass ) && self.waitingtoselectclass )
        self waittill( "okToSpawn" );

    if ( isdefined( self.addtoteam ) )
    {
        maps\mp\gametypes\_menus::addtoteam( self.addtoteam );
        self.addtoteam = undefined;
    }

    if ( !mayspawn() )
    {
        wait 0.05;
        var_0 = self.origin;
        var_1 = self.angles;
        self notify( "attempted_spawn" );
        var_2 = self.pers["teamKillPunish"];

        if ( isdefined( var_2 ) && var_2 )
        {
            self.pers["teamkills"] = max( self.pers["teamkills"] - 1, 0 );
            maps\mp\_utility::setlowermessage( "friendly_fire", &"MP_FRIENDLY_FIRE_WILL_NOT" );

            if ( !self.hasspawned && self.pers["teamkills"] <= level.maxallowedteamkills )
                self.pers["teamKillPunish"] = 0;
        }
        else if ( maps\mp\_utility::isroundbased() && !maps\mp\_utility::islastround() )
        {
            if ( isdefined( self.tagavailable ) && self.tagavailable )
                maps\mp\_utility::setlowermessage( "spawn_info", game["strings"]["spawn_tag_wait"] );
            else if ( level.gametype == "siege" )
                maps\mp\_utility::setlowermessage( "spawn_info", game["strings"]["spawn_flag_wait"] );
            else
                maps\mp\_utility::setlowermessage( "spawn_info", game["strings"]["spawn_next_round"] );

            thread removespawnmessageshortly( 3.0 );
        }

        if ( self.sessionstate != "spectator" )
            var_0 += ( 0, 0, 60 );

        thread spawnspectator( var_0, var_1 );
        return;
    }

    if ( self.waitingtospawn )
        return;

    self.waitingtospawn = 1;
    waitandspawnclient();

    if ( isdefined( self ) )
        self.waitingtospawn = 0;
}

waitandspawnclient()
{
    self endon( "disconnect" );
    self endon( "end_respawn" );
    level endon( "game_ended" );
    self notify( "attempted_spawn" );
    var_0 = 0;
    var_1 = self.pers["teamKillPunish"];

    if ( isdefined( var_1 ) && var_1 )
    {
        var_2 = teamkilldelay();

        if ( var_2 > 0 )
        {
            maps\mp\_utility::setlowermessage( "friendly_fire", &"MP_FRIENDLY_FIRE_WILL_NOT", var_2, 1, 1 );
            thread respawn_asspectator( self.origin + ( 0, 0, 60 ), self.angles );
            var_0 = 1;
            wait(var_2);
            maps\mp\_utility::clearlowermessage( "friendly_fire" );
            self.respawntimerstarttime = gettime();
        }

        self.pers["teamKillPunish"] = 0;
    }
    else if ( !maps\mp\_utility::is_aliens() && teamkilldelay() )
        self.pers["teamkills"] = max( self.pers["teamkills"] - 1, 0 );

    if ( maps\mp\_utility::isusingremote() )
    {
        self.spawningafterremotedeath = 1;
        self.deathposition = self.origin;
        self waittill( "stopped_using_remote" );
    }

    if ( !isdefined( self.wavespawnindex ) && isdefined( level.waveplayerspawnindex[self.team] ) )
    {
        self.wavespawnindex = level.waveplayerspawnindex[self.team];
        level.waveplayerspawnindex[self.team]++;
    }

    var_3 = timeuntilspawn( 0 );
    thread predictabouttospawnplayerovertime( var_3 );

    if ( var_3 > 0 )
    {
        maps\mp\_utility::setlowermessage( "spawn_info", game["strings"]["waiting_to_spawn"], var_3, 1, 1 );

        if ( !var_0 )
            thread respawn_asspectator( self.origin + ( 0, 0, 60 ), self.angles );

        var_0 = 1;
        maps\mp\_utility::waitfortimeornotify( var_3, "force_spawn" );
        self notify( "stop_wait_safe_spawn_button" );
    }

    if ( needsbuttontorespawn() )
    {
        maps\mp\_utility::setlowermessage( "spawn_info", game["strings"]["press_to_spawn"], undefined, undefined, undefined, undefined, undefined, undefined, 1 );

        if ( !var_0 )
            thread respawn_asspectator( self.origin + ( 0, 0, 60 ), self.angles );

        var_0 = 1;
        waitrespawnbutton();
    }

    self.waitingtospawn = 0;
    maps\mp\_utility::clearlowermessage( "spawn_info" );
    self.wavespawnindex = undefined;
    thread spawnplayer();
}

needsbuttontorespawn()
{
    if ( maps\mp\gametypes\_tweakables::gettweakablevalue( "player", "forcerespawn" ) != 0 )
        return 0;

    if ( !self.hasspawned )
        return 0;

    var_0 = getdvarint( "scr_" + level.gametype + "_waverespawndelay" ) > 0;

    if ( var_0 )
        return 0;

    if ( self.wantsafespawn )
        return 0;

    return 1;
}

waitrespawnbutton()
{
    self endon( "disconnect" );
    self endon( "end_respawn" );

    for (;;)
    {
        if ( self usebuttonpressed() )
            break;

        wait 0.05;
    }
}

removespawnmessageshortly( var_0 )
{
    self endon( "disconnect" );
    level endon( "game_ended" );
    waittillframeend;
    self endon( "end_respawn" );
    wait(var_0);
    maps\mp\_utility::clearlowermessage( "spawn_info" );
}

laststandrespawnplayer()
{
    self laststandrevive();

    if ( maps\mp\_utility::_hasperk( "specialty_finalstand" ) && !level.diehardmode )
        maps\mp\_utility::_unsetperk( "specialty_finalstand" );

    if ( level.diehardmode )
        self.headicon = "";

    self setstance( "crouch" );
    self.revived = 1;
    self notify( "revive" );

    if ( isdefined( self.standardmaxhealth ) )
        self.maxhealth = self.standardmaxhealth;

    self.health = self.maxhealth;
    common_scripts\utility::_enableusability();

    if ( game["state"] == "postgame" )
        maps\mp\gametypes\_gamelogic::freezeplayerforroundend();
}

getdeathspawnpoint()
{
    var_0 = spawn( "script_origin", self.origin );
    var_0 hide();
    var_0.angles = self.angles;
    return var_0;
}

showspawnnotifies()
{
    if ( isdefined( game["defcon"] ) )
        thread maps\mp\gametypes\_hud_message::defconsplashnotify( game["defcon"], 0 );

    if ( !maps\mp\_utility::is_aliens() && maps\mp\_utility::isrested() )
        thread maps\mp\gametypes\_hud_message::splashnotify( "rested" );
}

predictabouttospawnplayerovertime( var_0 )
{
    if ( !0 )
        return;

    self endon( "disconnect" );
    self endon( "spawned" );
    self endon( "used_predicted_spawnpoint" );
    self notify( "predicting_about_to_spawn_player" );
    self endon( "predicting_about_to_spawn_player" );

    if ( var_0 <= 0 )
        return;

    if ( var_0 > 1.0 )
        wait(var_0 - 1.0);

    predictabouttospawnplayer();
    self predictstreampos( self.predictedspawnpoint.origin + ( 0, 0, 60 ), self.predictedspawnpoint.angles );
    self.predictedspawnpointtime = gettime();

    for ( var_1 = 0; var_1 < 30; var_1++ )
    {
        wait 0.4;
        var_2 = self.predictedspawnpoint;
        predictabouttospawnplayer();

        if ( self.predictedspawnpoint != var_2 )
        {
            self predictstreampos( self.predictedspawnpoint.origin + ( 0, 0, 60 ), self.predictedspawnpoint.angles );
            self.predictedspawnpointtime = gettime();
        }
    }
}

predictabouttospawnplayer()
{
    if ( timeuntilspawn( 1 ) > 1.0 )
    {
        self.predictedspawnpoint = getspectatepoint();
        return;
    }

    if ( isdefined( self.setspawnpoint ) )
    {
        self.predictedspawnpoint = self.setspawnpoint;
        return;
    }

    var_0 = self [[ level.getspawnpoint ]]();
    self.predictedspawnpoint = var_0;
}

checkpredictedspawnpointcorrectness( var_0 )
{
    self notify( "used_predicted_spawnpoint" );
    self.predictedspawnpoint = undefined;
}

percentage( var_0, var_1 )
{
    return var_0 + " (" + int( var_0 / var_1 * 100 ) + "%)";
}

printpredictedspawnpointcorrectness()
{

}

getspawnorigin( var_0 )
{
    if ( !positionwouldtelefrag( var_0.origin ) )
        return var_0.origin;

    if ( !isdefined( var_0.alternates ) )
        return var_0.origin;

    foreach ( var_2 in var_0.alternates )
    {
        if ( !positionwouldtelefrag( var_2 ) )
            return var_2;
    }

    return var_0.origin;
}

tivalidationcheck()
{
    if ( !isdefined( self.setspawnpoint ) )
        return 0;

    var_0 = getentarray( "care_package", "targetname" );

    foreach ( var_2 in var_0 )
    {
        if ( distance( var_2.origin, self.setspawnpoint.playerspawnpos ) > 64 )
            continue;

        if ( isdefined( var_2.owner ) )
            maps\mp\gametypes\_hud_message::playercardsplashnotify( "destroyed_insertion", var_2.owner );

        maps\mp\perks\_perkfunctions::deleteti( self.setspawnpoint );
        return 0;
    }

    if ( !bullettracepassed( self.setspawnpoint.origin + ( 0, 0, 60 ), self.setspawnpoint.origin, 0, self.setspawnpoint ) )
        return 0;

    var_4 = self.setspawnpoint.origin + ( 0, 0, 1 );
    var_5 = playerphysicstrace( var_4, self.setspawnpoint.origin + ( 0, 0, -16 ) );

    if ( var_4[2] == var_5[2] )
        return 0;

    return 1;
}

spawningclientthisframereset()
{
    self notify( "spawningClientThisFrameReset" );
    self endon( "spawningClientThisFrameReset" );
    wait 0.05;
    level.numplayerswaitingtospawn--;
}

spawnplayer( var_0 )
{
    self endon( "disconnect" );
    self endon( "joined_spectators" );
    self notify( "spawned" );
    self notify( "end_respawn" );
    self notify( "started_spawnPlayer" );

    if ( !isdefined( var_0 ) )
        var_0 = 0;

    var_1 = undefined;
    self.ti_spawn = 0;
    self setclientomnvar( "ui_options_menu", 0 );
    self setclientomnvar( "ui_hud_shake", 0 );
    self.lastkillsplash = undefined;

    if ( !level.ingraceperiod && !self.hasdonecombat )
    {
        level.numplayerswaitingtospawn++;

        if ( level.numplayerswaitingtospawn > 1 )
        {
            self.waitingtospawnamortize = 1;
            wait(0.05 * ( level.numplayerswaitingtospawn - 1 ));
        }

        thread spawningclientthisframereset();
        self.waitingtospawnamortize = 0;
    }

    // It's possible when changing teams that we don't have our own customization loaded, wait until that's done
    // Setting self.waitingtospawnamortize to false so we can use the default character model and prevent a 5 second respawn delay. (Github issue #109)
    if ( !self hasloadedcustomizationplayerview( self ) )
        self.waitingtospawnamortize = 0;

    if ( isdefined( self.forcespawnorigin ) )
    {
        var_2 = self.forcespawnorigin;
        self.forcespawnorigin = undefined;

        if ( isdefined( self.forcespawnangles ) )
        {
            var_3 = self.forcespawnangles;
            self.forcespawnangles = undefined;
        }
        else
            var_3 = ( 0, randomfloatrange( 0, 360 ), 0 );
    }
    else if ( isdefined( self.setspawnpoint ) && ( isdefined( self.setspawnpoint.notti ) || tivalidationcheck() ) )
    {
        var_1 = self.setspawnpoint;

        if ( !isdefined( self.setspawnpoint.notti ) )
        {
            self.ti_spawn = 1;
            self playlocalsound( "tactical_spawn" );

            if ( level.multiteambased )
            {
                foreach ( var_5 in level.teamnamelist )
                {
                    if ( var_5 != self.team )
                        self playsoundtoteam( "tactical_spawn", var_5 );
                }
            }
            else if ( level.teambased )
                self playsoundtoteam( "tactical_spawn", level.otherteam[self.team] );
            else
                self playsound( "tactical_spawn" );
        }

        foreach ( var_8 in level.ugvs )
        {
            if ( distancesquared( var_8.origin, var_1.playerspawnpos ) < 1024 )
                var_8 notify( "damage", 5000, var_8.owner, ( 0, 0, 0 ), ( 0, 0, 0 ), "MOD_EXPLOSIVE", "", "", "", undefined, "killstreak_emp_mp" );
        }

        var_2 = self.setspawnpoint.playerspawnpos;
        var_3 = self.setspawnpoint.angles;

        if ( isdefined( self.setspawnpoint.enemytrigger ) )
            self.setspawnpoint.enemytrigger delete();

        self.setspawnpoint delete();
        var_1 = undefined;
    }
    else if ( isdefined( self.isspawningonbattlebuddy ) && isdefined( self.battlebuddy ) )
    {
        var_2 = undefined;
        var_3 = undefined;
        var_10 = maps\mp\gametypes\_battlebuddy::checkbuddyspawn();

        if ( var_10.status == 0 )
        {
            var_2 = var_10.origin;
            var_3 = var_10.angles;
        }
        else
        {
            var_1 = self [[ level.getspawnpoint ]]();
            var_2 = var_1.origin;
            var_3 = var_1.angles;
        }

        maps\mp\gametypes\_battlebuddy::cleanupbuddyspawn();
        self setclientomnvar( "cam_scene_name", "battle_spawn" );
        self setclientomnvar( "cam_scene_lead", self.battlebuddy getentitynumber() );
        self setclientomnvar( "cam_scene_support", self getentitynumber() );
    }
    else if ( isdefined( self.helispawning ) && ( !isdefined( self.firstspawn ) || isdefined( self.firstspawn ) && self.firstspawn ) && level.prematchperiod > 0 && self.team == "allies" )
    {
        while ( !isdefined( level.allieschopper ) )
            wait 0.1;

        var_2 = level.allieschopper.origin;
        var_3 = level.allieschopper.angles;
        self.firstspawn = 0;
    }
    else if ( isdefined( self.helispawning ) && ( !isdefined( self.firstspawn ) || isdefined( self.firstspawn ) && self.firstspawn ) && level.prematchperiod > 0 && self.team == "axis" )
    {
        while ( !isdefined( level.axischopper ) )
            wait 0.1;

        var_2 = level.axischopper.origin;
        var_3 = level.axischopper.angles;
        self.firstspawn = 0;
    }
    else
    {
        var_1 = self [[ level.getspawnpoint ]]();
        var_2 = var_1.origin;
        var_3 = var_1.angles;
    }

    setspawnvariables();
    var_11 = self.hasspawned;
    self.fauxdead = undefined;

    if ( !var_0 )
    {
        self.killsthislife = [];
        self.killsthislifeperweapon = [];
        maps\mp\_utility::updatesessionstate( "playing" );
        maps\mp\_utility::clearkillcamstate();
        self.cancelkillcam = undefined;
        self.maxhealth = maps\mp\gametypes\_tweakables::gettweakablevalue( "player", "maxhealth" );
        self.health = self.maxhealth;
        self.friendlydamage = undefined;
        self.hasspawned = 1;
        self.spawntime = gettime();
        self.wasti = !isdefined( var_1 );
        self.afk = 0;
        self.damagedplayers = [];
        self.killstreakscaler = 1;
        self.xpscaler = 1;
        self.objectivescaler = 1;
        self.clampedhealth = undefined;
        self.shielddamage = 0;
        self.shieldbullethits = 0;
        self.recentshieldxp = 0;
    }

    self.movespeedscaler = 1;
    self.inlaststand = 0;
    self.laststand = undefined;
    self.infinalstand = undefined;
    self.inc4death = undefined;
    self.disabledweapon = 0;
    self.disabledweaponswitch = 0;
    self.disabledoffhandweapons = 0;
    common_scripts\utility::resetusability();

    if ( !var_0 )
    {
        self.avoidkillstreakonspawntimer = 5.0;

        if ( !maps\mp\_utility::is_aliens() )
        {
            var_12 = self.pers["lives"];

            if ( var_12 == maps\mp\_utility::getgametypenumlives() )
                addtolivescount();

            if ( var_12 )
                self.pers["lives"]--;
        }

        addtoalivecount();

        if ( !var_11 || maps\mp\_utility::gamehasstarted() || maps\mp\_utility::gamehasstarted() && level.ingraceperiod && self.hasdonecombat )
            removefromlivescount();

        if ( !self.wasaliveatmatchstart )
        {
            var_13 = 20;

            if ( maps\mp\_utility::gettimelimit() > 0 && var_13 < maps\mp\_utility::gettimelimit() * 60 / 4 )
                var_13 = maps\mp\_utility::gettimelimit() * 60 / 4;

            if ( level.ingraceperiod || maps\mp\_utility::gettimepassed() < var_13 * 1000 )
                self.wasaliveatmatchstart = 1;
        }
    }

    self setdepthoffield( 0, 0, 512, 512, 4, 0 );

    if ( level.console )
        self setclientdvar( "cg_fov", "65" );

    resetuidvarsonspawn();

    if ( isdefined( var_1 ) )
    {
        maps\mp\gametypes\_spawnlogic::finalizespawnpointchoice( var_1 );
        var_2 = getspawnorigin( var_1 );
        var_3 = var_1.angles;
    }
    else if ( !isdefined( self.faux_spawn_infected ) )
        self.lastspawntime = gettime();

    self.spawnpos = var_2;
    self spawn( var_2, var_3 );

    if ( var_0 && isdefined( self.faux_spawn_stance ) )
    {
        self setstance( self.faux_spawn_stance );
        self.faux_spawn_stance = undefined;
    }

    if ( isai( self ) )
        maps\mp\_utility::freezecontrolswrapper( 1 );

    [[ level.onspawnplayer ]]();

    if ( isdefined( var_1 ) )
        checkpredictedspawnpointcorrectness( var_1.origin );

    if ( !var_0 )
    {
        maps\mp\gametypes\_missions::playerspawned();

        if ( isai( self ) && isdefined( level.bot_funcs ) && isdefined( level.bot_funcs["player_spawned"] ) )
            self [[ level.bot_funcs["player_spawned"] ]]();

        if ( !isai( self ) )
            thread watchforslide();
    }

    maps\mp\gametypes\_class::setclass( self.class );

    if ( isdefined( level.custom_giveloadout ) )
        self [[ level.custom_giveloadout ]]( var_0 );
    else
        maps\mp\gametypes\_class::giveloadout( self.team, self.class );

    if ( isdefined( game["roundsPlayed"] ) && game["roundsPlayed"] > 0 )
    {
        if ( !isdefined( self.classrefreshed ) || !self.classrefreshed )
        {
            if ( isdefined( self.class_num ) )
            {
                self setclientomnvar( "ui_loadout_selected", self.class_num );
                self.classrefreshed = 1;
            }
        }
    }

    if ( getdvarint( "camera_thirdPerson" ) )
        maps\mp\_utility::setthirdpersondof( 1 );

    if ( !maps\mp\_utility::gameflag( "prematch_done" ) )
        maps\mp\_utility::freezecontrolswrapper( 1 );
    else
        maps\mp\_utility::freezecontrolswrapper( 0 );

    if ( !maps\mp\_utility::gameflag( "prematch_done" ) || !var_11 && game["state"] == "playing" )
    {
        if ( !maps\mp\_utility::is_aliens() )
        {
            if ( game["status"] == "overtime" )
                thread maps\mp\gametypes\_hud_message::oldnotifymessage( game["strings"]["overtime"], game["strings"]["overtime_hint"], undefined, ( 1, 0, 0 ), "mp_last_stand" );
        }

        thread showspawnnotifies();
    }

    if ( maps\mp\_utility::getintproperty( "scr_showperksonspawn", 1 ) == 1 && game["state"] != "postgame" )
    {
        if ( !maps\mp\_utility::is_aliens() )
            self setclientomnvar( "ui_spawn_abilities_show", 1 );
    }

    waittillframeend;
    self.spawningafterremotedeath = undefined;
    self notify( "spawned_player" );
    level notify( "player_spawned", self );

    if ( game["state"] == "postgame" )
        maps\mp\gametypes\_gamelogic::freezeplayerforroundend();
}

spawnspectator( var_0, var_1 )
{
    self notify( "spawned" );
    self notify( "end_respawn" );
    self notify( "joined_spectators" );
    in_spawnspectator( var_0, var_1 );
}

respawn_asspectator( var_0, var_1 )
{
    in_spawnspectator( var_0, var_1 );
}

in_spawnspectator( var_0, var_1 )
{
    setspawnvariables();
    var_2 = self.pers["team"];

    if ( isdefined( var_2 ) && var_2 == "spectator" && !level.gameended )
        maps\mp\_utility::clearlowermessage( "spawn_info" );

    maps\mp\_utility::updatesessionstate( "spectator" );
    maps\mp\_utility::clearkillcamstate();
    self.friendlydamage = undefined;
    resetuidvarsonspectate();

    if ( isdefined( var_2 ) && var_2 == "spectator" )
        self.statusicon = "";
    else
        self.statusicon = "hud_status_dead";

    maps\mp\gametypes\_spectating::setspectatepermissions();
    onspawnspectator( var_0, var_1 );

    if ( level.teambased && !level.splitscreen && !self issplitscreenplayer() )
        self setdepthoffield( 0, 128, 512, 4000, 6, 1.8 );
}

getplayerfromclientnum( var_0 )
{
    if ( var_0 < 0 )
        return undefined;

    for ( var_1 = 0; var_1 < level.players.size; var_1++ )
    {
        if ( level.players[var_1] getentitynumber() == var_0 )
            return level.players[var_1];
    }

    return undefined;
}

onspawnspectator( var_0, var_1 )
{
    if ( isdefined( var_0 ) && isdefined( var_1 ) )
    {
        self setspectatedefaults( var_0, var_1 );
        self spawn( var_0, var_1 );
        checkpredictedspawnpointcorrectness( var_0 );
        return;
    }

    var_2 = getspectatepoint();

    if ( !maps\mp\_utility::is_aliens() )
    {
        var_3 = getentarray( "mp_mlg_camera", "classname" );

        if ( isdefined( var_3 ) && var_3.size )
        {
            for ( var_4 = 0; var_4 < var_3.size && var_4 < 4; var_4++ )
            {
                self setmlgcameradefaults( var_4, var_3[var_4].origin, var_3[var_4].angles );
                level.cameramapobjs[var_4].origin = var_3[var_4].origin;
                level.cameramapobjs[var_4].angles = var_3[var_4].angles;
            }
        }
        else if ( isdefined( level.camera3pos ) )
        {
            var_5 = tolower( getdvar( "mapname" ) );

            if ( var_5 == "mp_strikezone" && isdefined( level.teleport_zone_current ) && level.teleport_zone_current != "start" )
            {
                self setmlgcameradefaults( 0, level.camera5pos, level.camera5ang );
                level.cameramapobjs[0].origin = level.camera5pos;
                level.cameramapobjs[0].angles = level.camera5ang;
                self setmlgcameradefaults( 1, level.camera6pos, level.camera6ang );
                level.cameramapobjs[1].origin = level.camera6pos;
                level.cameramapobjs[1].angles = level.camera6ang;
                self setmlgcameradefaults( 2, level.camera7pos, level.camera7ang );
                level.cameramapobjs[2].origin = level.camera7pos;
                level.cameramapobjs[2].angles = level.camera7ang;
                self setmlgcameradefaults( 3, level.camera8pos, level.camera8ang );
                level.cameramapobjs[3].origin = level.camera8pos;
                level.cameramapobjs[3].angles = level.camera8ang;
            }
            else
            {
                self setmlgcameradefaults( 0, level.camera1pos, level.camera1ang );
                level.cameramapobjs[0].origin = level.camera1pos;
                level.cameramapobjs[0].angles = level.camera1ang;
                self setmlgcameradefaults( 1, level.camera2pos, level.camera2ang );
                level.cameramapobjs[1].origin = level.camera2pos;
                level.cameramapobjs[1].angles = level.camera2ang;
                self setmlgcameradefaults( 2, level.camera3pos, level.camera3ang );
                level.cameramapobjs[2].origin = level.camera3pos;
                level.cameramapobjs[2].angles = level.camera3ang;
                self setmlgcameradefaults( 3, level.camera4pos, level.camera4ang );
                level.cameramapobjs[3].origin = level.camera4pos;
                level.cameramapobjs[3].angles = level.camera4ang;
            }
        }
        else
        {
            for ( var_4 = 0; var_4 < 4; var_4++ )
                self setmlgcameradefaults( var_4, var_2.origin, var_2.angles );
        }
    }

    self setspectatedefaults( var_2.origin, var_2.angles );
    self spawn( var_2.origin, var_2.angles );
    checkpredictedspawnpointcorrectness( var_2.origin );
}

getspectatepoint()
{
    var_0 = getentarray( "mp_global_intermission", "classname" );
    var_1 = maps\mp\gametypes\_spawnlogic::getspawnpoint_random( var_0 );
    return var_1;
}

spawnintermission()
{
    self endon( "disconnect" );
    self notify( "spawned" );
    self notify( "end_respawn" );
    setspawnvariables();
    maps\mp\_utility::clearlowermessages();
    maps\mp\_utility::freezecontrolswrapper( 1 );
    self setclientdvar( "cg_everyoneHearsEveryone", 1 );
    var_0 = self.pers["postGameChallenges"];

    if ( !maps\mp\_utility::is_aliens() && level.rankedmatch && ( self.postgamepromotion || isdefined( var_0 ) && var_0 ) )
    {
        if ( self.postgamepromotion )
            self playlocalsound( "mp_level_up" );
        else if ( isdefined( var_0 ) )
            self playlocalsound( "mp_challenge_complete" );

        if ( self.postgamepromotion > level.postgamenotifies )
            level.postgamenotifies = 1;

        if ( isdefined( var_0 ) && var_0 > level.postgamenotifies )
            level.postgamenotifies = var_0;

        var_1 = 7.0;

        if ( isdefined( var_0 ) )
            var_1 = 4.0 + min( var_0, 3 );

        while ( var_1 )
        {
            wait 0.25;
            var_1 -= 0.25;
        }
    }

    if ( isdefined( level.finalkillcam_winner ) && level.finalkillcam_winner != "none" && isdefined( level.match_end_delay ) && maps\mp\_utility::waslastround() )
        wait(level.match_end_delay);

    maps\mp\_utility::updatesessionstate( "intermission" );
    maps\mp\_utility::clearkillcamstate();
    self.friendlydamage = undefined;
    var_2 = getentarray( "mp_global_intermission", "classname" );
    var_2 = maps\mp\gametypes\_spawnscoring::checkdynamicspawns( var_2 );
    var_3 = var_2[0];

    if ( !isdefined( level.custom_ending ) )
    {
        self spawn( var_3.origin, var_3.angles );
        checkpredictedspawnpointcorrectness( var_3.origin );
        self setdepthoffield( 0, 128, 512, 4000, 6, 1.8 );
    }
    else
        level notify( "scoreboard_displaying" );
}

spawnendofgame()
{
    if ( 1 )
    {
        if ( isdefined( level.custom_ending ) && maps\mp\_utility::waslastround() )
            level notify( "start_custom_ending" );

        maps\mp\_utility::freezecontrolswrapper( 1 );
        spawnspectator();
        maps\mp\_utility::freezecontrolswrapper( 1 );
        return;
    }

    self notify( "spawned" );
    self notify( "end_respawn" );
    setspawnvariables();
    maps\mp\_utility::clearlowermessages();
    self setclientdvar( "cg_everyoneHearsEveryone", 1 );
    maps\mp\_utility::updatesessionstate( "dead" );
    maps\mp\_utility::clearkillcamstate();
    self.friendlydamage = undefined;
    var_0 = getspectatepoint();
    spawnspectator( var_0.origin, var_0.angles );
    checkpredictedspawnpointcorrectness( var_0.origin );
    maps\mp\_utility::freezecontrolswrapper( 1 );
    self setdepthoffield( 0, 0, 512, 512, 4, 0 );
}

setspawnvariables()
{
    self stopshellshock();
    self stoprumble( "damage_heavy" );
    self.deathposition = undefined;
}

notifyconnecting()
{
    waittillframeend;

    if ( isdefined( self ) )
        level notify( "connecting", self );
}

callback_playerdisconnect( var_0 )
{
    if ( !isdefined( self.connected ) )
        return;

    var_1 = getmatchdata( "gameLength" );
    var_1 += int( maps\mp\_utility::getsecondspassed() );
    setmatchdata( "players", self.clientid, "disconnectTime", var_1 );
    setmatchdata( "players", self.clientid, "disconnectReason", var_0 );

    if ( maps\mp\_utility::rankingenabled() && !maps\mp\_utility::is_aliens() )
        maps\mp\_matchdata::logfinalstats();

    if ( isdefined( self.pers["confirmed"] ) )
        maps\mp\_matchdata::logkillsconfirmed();

    if ( isdefined( self.pers["denied"] ) )
        maps\mp\_matchdata::logkillsdenied();

    removeplayerondisconnect();
    maps\mp\gametypes\_spawnlogic::removefromparticipantsarray();
    maps\mp\gametypes\_spawnlogic::removefromcharactersarray();
    var_2 = self getentitynumber();

    if ( !level.teambased )
        game["roundsWon"][self.guid] = undefined;

    if ( level.splitscreen )
    {
        var_3 = level.players;

        if ( var_3.size <= 1 )
            level thread maps\mp\gametypes\_gamelogic::forceend();
    }

    if ( isdefined( self.score ) && isdefined( self.pers["team"] ) )
    {
        var_4 = self.score;

        if ( maps\mp\_utility::getminutespassed() )
            var_4 = self.score / maps\mp\_utility::getminutespassed();

        setplayerteamrank( self, self.clientid, int( var_4 ) );
    }

    var_5 = self getentitynumber();
    var_6 = self.guid;
    logprint( "Q;" + var_6 + ";" + var_5 + ";" + self.name + "\n" );
    thread maps\mp\_events::disconnected();

    if ( level.gameended )
        maps\mp\gametypes\_gamescore::removedisconnectedplayerfromplacement();

    if ( isdefined( self.team ) )
        removefromteamcount();

    if ( self.sessionstate == "playing" && !( isdefined( self.fauxdead ) && self.fauxdead ) )
        removefromalivecount( 1 );
    else if ( self.sessionstate == "spectator" || self.sessionstate == "dead" )
        level thread maps\mp\gametypes\_gamelogic::updategameevents();
}

removeplayerondisconnect()
{
    var_0 = 0;

    for ( var_1 = 0; var_1 < level.players.size; var_1++ )
    {
        if ( level.players[var_1] == self )
        {
            for ( var_0 = 1; var_1 < level.players.size - 1; var_1++ )
                level.players[var_1] = level.players[var_1 + 1];

            level.players[var_1] = undefined;
            break;
        }
    }
}

initclientdvarssplitscreenspecific()
{
}

initclientdvars()
{
    setdvar( "cg_drawTalk", 1 );
    setdvar( "cg_drawCrosshair", 1 );
    setdvar( "cg_drawCrosshairNames", 1 );
    setdvar( "cg_hudGrenadeIconMaxRangeFrag", 250 );

    if ( level.hardcoremode )
    {
        setdvar( "cg_drawTalk", 3 );
        setdvar( "cg_drawCrosshair", 0 );
        setdvar( "cg_drawCrosshairNames", 1 );
        setdvar( "cg_hudGrenadeIconMaxRangeFrag", 0 );
    }

    if ( isdefined( level.alwaysdrawfriendlynames ) && level.alwaysdrawfriendlynames )
        setdvar( "cg_drawFriendlyNamesAlways", 1 );
    else
        setdvar( "cg_drawFriendlyNamesAlways", 0 );

    initclientdvarssplitscreenspecific();

    self setclientdvar( "ui_altscene", 0 );

    if ( getdvarint( "scr_hitloc_debug" ) )
    {
        for ( var_0 = 0; var_0 < 6; var_0++ )
            self setclientdvar( "ui_hitloc_" + var_0, "" );

        self.hitlocinited = 1;
    }
}

getlowestavailableclientid()
{
    var_0 = 0;

    for ( var_1 = 0; var_1 < 30; var_1++ )
    {
        foreach ( var_3 in level.players )
        {
            if ( !isdefined( var_3 ) )
                continue;

            if ( var_3.clientid == var_1 )
            {
                var_0 = 1;
                break;
            }

            var_0 = 0;
        }

        if ( !var_0 )
            return var_1;
    }
}

setupsavedactionslots()
{
    self.saved_actionslotdata = [];

    for ( var_0 = 1; var_0 <= 4; var_0++ )
    {
        self.saved_actionslotdata[var_0] = spawnstruct();
        self.saved_actionslotdata[var_0].type = "";
        self.saved_actionslotdata[var_0].item = undefined;
    }

    if ( !level.console )
    {
        for ( var_0 = 5; var_0 <= 8; var_0++ )
        {
            self.saved_actionslotdata[var_0] = spawnstruct();
            self.saved_actionslotdata[var_0].type = "";
            self.saved_actionslotdata[var_0].item = undefined;
        }
    }
}

callback_playerconnect()
{
    thread notifyconnecting();
    self.statusicon = "hud_status_connecting";
    self waittill( "begin" );
    self.statusicon = "";
    self.connecttime = undefined;
    level notify( "connected", self );
    self.connected = 1;

    if ( self ishost() )
        level.player = self;

    if ( !level.splitscreen && !isdefined( self.pers["score"] ) )
        iprintln( &"MP_CONNECTED", self );

    self.usingonlinedataoffline = self isusingonlinedataoffline();
    initclientdvars();

    if ( !maps\mp\_utility::is_aliens() )
        initplayerstats();

    if ( getdvar( "r_reflectionProbeGenerate" ) == "1" )
        level waittill( "eternity" );

    self.guid = maps\mp\_utility::getuniqueid();
    var_0 = 0;

    if ( !isdefined( self.pers["clientid"] ) )
    {
        if ( game["clientid"] >= 30 )
            self.pers["clientid"] = getlowestavailableclientid();
        else
            self.pers["clientid"] = game["clientid"];

        if ( game["clientid"] < 30 )
            game["clientid"]++;

        var_0 = 1;
    }

    if ( var_0 )
        maps\mp\killstreaks\_killstreaks::resetadrenaline();

    self.clientid = self.pers["clientid"];
    self.pers["teamKillPunish"] = 0;
    logprint( "J;" + self.guid + ";" + self getentitynumber() + ";" + self.name + "\n" );

    if ( game["clientid"] <= 30 && game["clientid"] != getmatchdata( "playerCount" ) )
    {
        var_1 = 0;
        var_2 = 0;

        if ( !isai( self ) && maps\mp\_utility::matchmakinggame() )
            self registerparty( self.clientid );

        setmatchdata( "playerCount", game["clientid"] );
        setmatchdata( "players", self.clientid, "playerID", "xuid", self getxuid() );
        setmatchdata( "players", self.clientid, "playerID", "ucdIDHigh", self getucdidhigh() );
        setmatchdata( "players", self.clientid, "playerID", "ucdIDLow", self getucdidlow() );
        setmatchdata( "players", self.clientid, "playerID", "clanIDHigh", self getclanidhigh() );
        setmatchdata( "players", self.clientid, "playerID", "clanIDLow", self getclanidlow() );
        setmatchdata( "players", self.clientid, "gamertag", self.name );
        var_2 = self getcommonplayerdata( "connectionIDChunkLow", 0 );
        var_1 = self getcommonplayerdata( "connectionIDChunkHigh", 0 );
        setmatchdata( "players", self.clientid, "connectionIDChunkLow", var_2 );
        setmatchdata( "players", self.clientid, "connectionIDChunkHigh", var_1 );
        setmatchclientip( self, self.clientid );
        var_3 = getmatchdata( "gameLength" );
        var_3 += int( maps\mp\_utility::getsecondspassed() );
        setmatchdata( "players", self.clientid, "joinType", self getjointype() );
        setmatchdata( "players", self.clientid, "connectTime", var_3 );

        if ( maps\mp\_utility::rankingenabled() && !maps\mp\_utility::is_aliens() )
            maps\mp\_matchdata::loginitialstats();

        if ( isdefined( self.pers["isBot"] ) && self.pers["isBot"] || isai( self ) )
            var_4 = 1;
        else
            var_4 = 0;

        if ( maps\mp\_utility::matchmakinggame() && maps\mp\_utility::allowteamchoice() && !var_4 )
            setmatchdata( "players", self.clientid, "team", self.sessionteam );
    }

    if ( !level.teambased )
        game["roundsWon"][self.guid] = 0;

    self.leaderdialogqueue = [];
    self.leaderdialoglocqueue = [];
    self.leaderdialogactive = "";
    self.leaderdialoggroups = [];
    self.leaderdialoggroup = "";

    if ( !isdefined( self.pers["cur_kill_streak"] ) )
        self.pers["cur_kill_streak"] = 0;

    if ( !isdefined( self.pers["cur_death_streak"] ) )
        self.pers["cur_death_streak"] = 0;

    if ( !isdefined( self.pers["assistsToKill"] ) )
        self.pers["assistsToKill"] = 0;

    if ( !isdefined( self.pers["cur_kill_streak_for_nuke"] ) )
        self.pers["cur_kill_streak_for_nuke"] = 0;

    if ( !isdefined( self.pers["objectivePointStreak"] ) )
        self.pers["objectivePointStreak"] = 0;

    if ( maps\mp\_utility::rankingenabled() && !maps\mp\_utility::is_aliens() )
        self.kill_streak = maps\mp\gametypes\_persistence::statget( "killStreak" );

    self.lastgrenadesuicidetime = -1;
    self.teamkillsthisround = 0;
    self.hasspawned = 0;
    self.waitingtospawn = 0;
    self.wantsafespawn = 0;
    self.wasaliveatmatchstart = 0;
    self.movespeedscaler = 1;
    self.killstreakscaler = 1;
    self.xpscaler = 1;
    self.objectivescaler = 1;
    self.issniper = 0;

    if ( !maps\mp\_utility::is_aliens() )
        setrestxpgoal();

    setupsavedactionslots();
    thread maps\mp\_flashgrenades::monitorflash();
    resetuidvarsonconnect();
    waittillframeend;
    level.players[level.players.size] = self;
    maps\mp\gametypes\_spawnlogic::addtoparticipantsarray();
    maps\mp\gametypes\_spawnlogic::addtocharactersarray();

    if ( level.teambased )
        self updatescores();

    if ( game["state"] == "postgame" )
    {
        self.connectedpostgame = 1;
        spawnintermission();
    }
    else
    {
        if ( isai( self ) && isdefined( level.bot_funcs ) && isdefined( level.bot_funcs["think"] ) )
            self thread [[ level.bot_funcs["think"] ]]();

        level endon( "game_ended" );

        if ( isdefined( level.hostmigrationtimer ) )
            thread maps\mp\gametypes\_hostmigration::hostmigrationtimerthink();

        if ( isdefined( level.onplayerconnectaudioinit ) )
            [[ level.onplayerconnectaudioinit ]]();

        if ( maps\mp\_utility::bot_is_fireteam_mode() && !isai( self ) )
        {
            thread spawnspectator();
            self setclientomnvar( "ui_options_menu", 0 );
        }
        else if ( !isdefined( self.pers["team"] ) )
        {
            if ( maps\mp\_utility::matchmakinggame() && self.sessionteam != "none" )
            {
                thread spawnspectator();
                thread maps\mp\gametypes\_menus::setteam( self.sessionteam );

                if ( maps\mp\_utility::allowclasschoice() || maps\mp\_utility::showfakeloadout() && !isai( self ) )
                    self setclientomnvar( "ui_options_menu", 2 );

                thread kickifdontspawn();
                return;
            }
            else if ( issquadsmode() && getdvarint( "onlinegame" ) == 0 && level.gametype != "horde" && isbot( self ) == 0 )
            {
                thread spawnspectator();
                thread maps\mp\gametypes\_menus::setteam( "allies" );
                self setclientomnvar( "ui_options_menu", 2 );
            }
            else if ( maps\mp\_utility::allowteamchoice() && isbot( self ) == 0 )
            {
                maps\mp\gametypes\_menus::menuspectator();
                self setclientomnvar( "ui_options_menu", 1 );
            }
            else
            {
                thread spawnspectator();
                maps\mp\gametypes\_menus::autoassign();

                if ( maps\mp\_utility::showfakeloadout() && !isai( self ) )
                    self setclientomnvar( "ui_options_menu", 2 );

                if ( maps\mp\_utility::matchmakinggame() )
                    thread kickifdontspawn();

                return;
            }
        }
        else
        {
            maps\mp\gametypes\_menus::addtoteam( self.pers["team"], 1 );

            if ( maps\mp\_utility::isvalidclass( self.pers["class"] ) )
            {
                thread spawnclient();
                return;
            }

            thread spawnspectator();

            if ( self.pers["team"] == "spectator" )
            {
                if ( isdefined( self.pers["mlgSpectator"] ) && self.pers["mlgSpectator"] )
                {
                    self setmlgspectator( 1 );
                    thread maps\mp\gametypes\_spectating::setmlgcamvisibility( 1 );
                    thread maps\mp\gametypes\_spectating::setspectatepermissions();
                }

                if ( maps\mp\_utility::allowteamchoice() )
                {
                    maps\mp\gametypes\_menus::beginteamchoice();
                    return;
                }

                self [[ level.autoassign ]]();
                return;
                return;
            }

            maps\mp\gametypes\_menus::beginclasschoice();
        }
    }
}

callback_playermigrated()
{
    if ( isdefined( self.connected ) && self.connected )
    {
        maps\mp\_utility::updateobjectivetext();
        maps\mp\_utility::updatemainmenu();

        if ( level.teambased )
            self updatescores();
    }

    if ( self ishost() )
        initclientdvarssplitscreenspecific();

    var_0 = 0;

    foreach ( var_2 in level.players )
    {
        if ( !isdefined( var_2.pers["isBot"] ) || var_2.pers["isBot"] == 0 )
            var_0++;
    }

    if ( !isdefined( self.pers["isBot"] ) || self.pers["isBot"] == 0 )
    {
        level.hostmigrationreturnedplayercount++;

        if ( level.hostmigrationreturnedplayercount >= var_0 * 2 / 3 )
            level notify( "hostmigration_enoughplayers" );
    }
}

addlevelstoexperience( var_0, var_1 )
{
    var_2 = maps\mp\gametypes\_rank::getrankforxp( var_0 );
    var_3 = maps\mp\gametypes\_rank::getrankinfominxp( var_2 );
    var_4 = maps\mp\gametypes\_rank::getrankinfomaxxp( var_2 );
    var_2 += ( var_0 - var_3 ) / ( var_4 - var_3 );
    var_2 += var_1;

    if ( var_2 < 0 )
    {
        var_2 = 0;
        var_5 = 0.0;
    }
    else if ( var_2 >= level.maxrank + 1.0 )
    {
        var_2 = level.maxrank;
        var_5 = 1.0;
    }
    else
    {
        var_5 = var_2 - floor( var_2 );
        var_2 = int( floor( var_2 ) );
    }

    var_3 = maps\mp\gametypes\_rank::getrankinfominxp( var_2 );
    var_4 = maps\mp\gametypes\_rank::getrankinfomaxxp( var_2 );
    return int( var_5 * ( var_4 - var_3 ) ) + var_3;
}

getrestxpcap( var_0 )
{
    var_1 = getdvarfloat( "scr_restxp_cap" );
    return addlevelstoexperience( var_0, var_1 );
}

setrestxpgoal()
{
    if ( !maps\mp\_utility::rankingenabled() )
        return;

    if ( !getdvarint( "scr_restxp_enable" ) )
    {
        self setrankedplayerdata( "restXPGoal", 0 );
        return;
    }

    var_0 = self getrestedtime();
    var_1 = var_0 / 3600;
    var_2 = self getrankedplayerdata( "experience" );
    var_3 = getdvarfloat( "scr_restxp_minRestTime" );
    var_4 = getdvarfloat( "scr_restxp_levelsPerDay" ) / 24.0;
    var_5 = getrestxpcap( var_2 );
    var_6 = self getrankedplayerdata( "restXPGoal" );

    if ( var_6 < var_2 )
        var_6 = var_2;

    var_7 = var_6;
    var_8 = 0;

    if ( var_1 > var_3 )
    {
        var_8 = var_4 * var_1;
        var_6 = addlevelstoexperience( var_6, var_8 );
    }

    var_9 = "";

    if ( var_6 >= var_5 )
    {
        var_6 = var_5;
        var_9 = " (hit cap)";
    }

    self setrankedplayerdata( "restXPGoal", var_6 );
}

forcespawn()
{
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "spawned" );
    wait 60.0;

    if ( self.hasspawned )
        return;

    if ( self.pers["team"] == "spectator" )
        return;

    if ( !maps\mp\_utility::isvalidclass( self.pers["class"] ) )
    {
        self.pers["class"] = "CLASS_CUSTOM1";
        self.class = self.pers["class"];
    }

    thread spawnclient();
}

kickifdontspawn()
{
    self endon( "death" );
    self endon( "disconnect" );
    self endon( "spawned" );
    self endon( "attempted_spawn" );
    var_0 = getdvarfloat( "scr_kick_time", 90 );
    var_1 = getdvarfloat( "scr_kick_mintime", 45 );
    var_2 = getdvarfloat( "scr_kick_hosttime", 120 );
    var_3 = gettime();

    if ( self ishost() )
        kickwait( var_2 );
    else
        kickwait( var_0 );

    var_4 = ( gettime() - var_3 ) / 1000;

    if ( var_4 < var_0 - 0.1 && var_4 < var_1 )
        return;

    if ( self.hasspawned )
        return;

    if ( self.pers["team"] == "spectator" )
        return;

    kick( self getentitynumber(), "EXE_PLAYERKICKED_INACTIVE" );
    level thread maps\mp\gametypes\_gamelogic::updategameevents();
}

kickwait( var_0 )
{
    level endon( "game_ended" );
    maps\mp\gametypes\_hostmigration::waitlongdurationwithhostmigrationpause( var_0 );
}

initplayerstats()
{
    maps\mp\gametypes\_persistence::initbufferedstats();
    self.pers["lives"] = maps\mp\_utility::getgametypenumlives();

    if ( !isdefined( self.pers["deaths"] ) )
    {
        maps\mp\_utility::initpersstat( "deaths" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "deaths", 0 );
    }

    self.deaths = maps\mp\_utility::getpersstat( "deaths" );

    if ( !isdefined( self.pers["score"] ) )
    {
        maps\mp\_utility::initpersstat( "score" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "score", 0 );
    }

    self.score = maps\mp\_utility::getpersstat( "score" );

    if ( !isdefined( self.pers["suicides"] ) )
        maps\mp\_utility::initpersstat( "suicides" );

    self.suicides = maps\mp\_utility::getpersstat( "suicides" );

    if ( !isdefined( self.pers["kills"] ) )
    {
        maps\mp\_utility::initpersstat( "kills" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "kills", 0 );
    }

    self.kills = maps\mp\_utility::getpersstat( "kills" );

    if ( !isdefined( self.pers["headshots"] ) )
        maps\mp\_utility::initpersstat( "headshots" );

    self.headshots = maps\mp\_utility::getpersstat( "headshots" );

    if ( !isdefined( self.pers["assists"] ) )
    {
        maps\mp\_utility::initpersstat( "assists" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "assists", 0 );
    }

    self.assists = maps\mp\_utility::getpersstat( "assists" );

    if ( !isdefined( self.pers["captures"] ) )
    {
        maps\mp\_utility::initpersstat( "captures" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "captures", 0 );
    }

    self.captures = maps\mp\_utility::getpersstat( "captures" );

    if ( !isdefined( self.pers["returns"] ) )
    {
        maps\mp\_utility::initpersstat( "returns" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "returns", 0 );
    }

    self.returns = maps\mp\_utility::getpersstat( "returns" );

    if ( !isdefined( self.pers["defends"] ) )
    {
        maps\mp\_utility::initpersstat( "defends" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "defends", 0 );
    }

    self.defends = maps\mp\_utility::getpersstat( "defends" );

    if ( !isdefined( self.pers["plants"] ) )
    {
        maps\mp\_utility::initpersstat( "plants" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "plants", 0 );
    }

    self.plants = maps\mp\_utility::getpersstat( "plants" );

    if ( !isdefined( self.pers["defuses"] ) )
    {
        maps\mp\_utility::initpersstat( "defuses" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "defuses", 0 );
    }

    self.defuses = maps\mp\_utility::getpersstat( "defuses" );

    if ( !isdefined( self.pers["destructions"] ) )
    {
        maps\mp\_utility::initpersstat( "destructions" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "destructions", 0 );
    }

    self.destructions = maps\mp\_utility::getpersstat( "destructions" );

    if ( !isdefined( self.pers["confirmed"] ) )
    {
        maps\mp\_utility::initpersstat( "confirmed" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "confirmed", 0 );
    }

    self.confirmed = maps\mp\_utility::getpersstat( "confirmed" );

    if ( !isdefined( self.pers["denied"] ) )
    {
        maps\mp\_utility::initpersstat( "denied" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "denied", 0 );
    }

    self.denied = maps\mp\_utility::getpersstat( "denied" );

    if ( !isdefined( self.pers["rescues"] ) )
    {
        maps\mp\_utility::initpersstat( "rescues" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "rescues", 0 );
    }

    self.rescues = maps\mp\_utility::getpersstat( "rescues" );

    if ( !isdefined( self.pers["killChains"] ) )
    {
        maps\mp\_utility::initpersstat( "killChains" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "killChains", 0 );
    }

    self.killchains = maps\mp\_utility::getpersstat( "killChains" );

    if ( !isdefined( self.pers["killsAsSurvivor"] ) )
    {
        maps\mp\_utility::initpersstat( "killsAsSurvivor" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "killsAsSurvivor", 0 );
    }

    self.killsassurvivor = maps\mp\_utility::getpersstat( "killsAsSurvivor" );

    if ( !isdefined( self.pers["killsAsInfected"] ) )
    {
        maps\mp\_utility::initpersstat( "killsAsInfected" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "killsAsInfected", 0 );
    }

    self.killsasinfected = maps\mp\_utility::getpersstat( "killsAsInfected" );

    if ( !isdefined( self.pers["teamkills"] ) )
        maps\mp\_utility::initpersstat( "teamkills" );

    if ( !isdefined( self.pers["extrascore0"] ) )
        maps\mp\_utility::initpersstat( "extrascore0" );

    if ( !isdefined( self.pers["hordeKills"] ) )
    {
        maps\mp\_utility::initpersstat( "hordeKills" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "squardKills", 0 );
    }

    if ( !isdefined( self.pers["hordeRevives"] ) )
    {
        maps\mp\_utility::initpersstat( "hordeRevives" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "squardRevives", 0 );
    }

    if ( !isdefined( self.pers["hordeCrates"] ) )
    {
        maps\mp\_utility::initpersstat( "hordeCrates" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "squardCrates", 0 );
    }

    if ( !isdefined( self.pers["hordeRound"] ) )
    {
        maps\mp\_utility::initpersstat( "hordeRound" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "sguardWave", 0 );
    }

    if ( !isdefined( self.pers["hordeWeapon"] ) )
    {
        maps\mp\_utility::initpersstat( "hordeWeapon" );
        maps\mp\gametypes\_persistence::statsetchild( "round", "sguardWeaponLevel", 0 );
    }

    if ( !isdefined( self.pers["teamKillPunish"] ) )
        self.pers["teamKillPunish"] = 0;

    maps\mp\_utility::initpersstat( "longestStreak" );
    self.pers["lives"] = maps\mp\_utility::getgametypenumlives();
    maps\mp\gametypes\_persistence::statsetchild( "round", "killStreak", 0 );
    maps\mp\gametypes\_persistence::statsetchild( "round", "loss", 0 );
    maps\mp\gametypes\_persistence::statsetchild( "round", "win", 0 );
    maps\mp\gametypes\_persistence::statsetchild( "round", "scoreboardType", "none" );
    maps\mp\gametypes\_persistence::statsetchildbuffered( "round", "timePlayed", 0 );
}

addtoteamcount()
{
    level.teamcount[self.team]++;

    if ( !isdefined( level.teamlist ) )
        level.teamlist = [];

    if ( !isdefined( level.teamlist[self.team] ) )
        level.teamlist[self.team] = [];

    level.teamlist[self.team][level.teamlist[self.team].size] = self;
    maps\mp\gametypes\_gamelogic::updategameevents();
}

removefromteamcount()
{
    level.teamcount[self.team]--;

    if ( isdefined( level.teamlist ) && isdefined( level.teamlist[self.team] ) )
    {
        var_0 = [];

        foreach ( var_2 in level.teamlist[self.team] )
        {
            if ( !isdefined( var_2 ) || var_2 == self )
                continue;

            var_0[var_0.size] = var_2;
        }

        level.teamlist[self.team] = var_0;
    }
}

addtoalivecount()
{
    var_0 = self.team;

    if ( !( isdefined( self.alreadyaddedtoalivecount ) && self.alreadyaddedtoalivecount ) )
    {
        level.hasspawned[var_0]++;
        incrementalivecount( var_0 );
    }

    self.alreadyaddedtoalivecount = undefined;

    if ( level.alivecount["allies"] + level.alivecount["axis"] > level.maxplayercount )
        level.maxplayercount = level.alivecount["allies"] + level.alivecount["axis"];
}

incrementalivecount( var_0 )
{
    level.alivecount[var_0]++;
}

removefromalivecount( var_0 )
{
    if ( maps\mp\_utility::is_aliens() )
        return;

    var_1 = self.team;

    if ( isdefined( self.switching_teams ) && self.switching_teams && isdefined( self.joining_team ) && self.joining_team == self.team )
        var_1 = self.leaving_team;

    if ( isdefined( var_0 ) )
        removeallfromlivescount();
    else if ( isdefined( self.switching_teams ) )
        self.pers["lives"]--;

    decrementalivecount( var_1 );
    return maps\mp\gametypes\_gamelogic::updategameevents();
}

decrementalivecount( var_0 )
{
    level.alivecount[var_0]--;
}

addtolivescount()
{
    level.livescount[self.team] += self.pers["lives"];
}

removefromlivescount()
{
    level.livescount[self.team]--;
    level.livescount[self.team] = int( max( 0, level.livescount[self.team] ) );
}

removeallfromlivescount()
{
    level.livescount[self.team] -= self.pers["lives"];
    level.livescount[self.team] = int( max( 0, level.livescount[self.team] ) );
}

resetuidvarsonspawn()
{
    self setclientomnvar( "ui_carrying_bomb", 0 );
    self setclientomnvar( "ui_dom_securing", 0 );
    self setclientomnvar( "ui_securing", 0 );
    self setclientomnvar( "ui_bomb_planting_defusing", 0 );
    self setclientomnvar( "ui_light_armor", 0 );
    self setclientdvar( "ui_juiced_end_milliseconds", 0 );
    self setclientomnvar( "ui_killcam_end_milliseconds", 0 );
    self setclientomnvar( "ui_cranked_bomb_timer_end_milliseconds", 0 );
}

resetuidvarsonconnect()
{
    self setclientomnvar( "ui_carrying_bomb", 0 );
    self setclientomnvar( "ui_dom_securing", 0 );
    self setclientomnvar( "ui_securing", 0 );
    self setclientomnvar( "ui_bomb_planting_defusing", 0 );
    self setclientomnvar( "ui_light_armor", 0 );
    self setclientomnvar( "ui_killcam_end_milliseconds", 0 );
    self setclientdvar( "ui_juiced_end_milliseconds", 0 );
    self setclientdvar( "ui_eyes_on_end_milliseconds", 0 );
    self setclientomnvar( "ui_cranked_bomb_timer_end_milliseconds", 0 );
}

resetuidvarsonspectate()
{
    self setclientomnvar( "ui_carrying_bomb", 0 );
    self setclientomnvar( "ui_dom_securing", 0 );
    self setclientomnvar( "ui_securing", 0 );
    self setclientomnvar( "ui_bomb_planting_defusing", 0 );
    self setclientomnvar( "ui_light_armor", 0 );
    self setclientomnvar( "ui_killcam_end_milliseconds", 0 );
    self setclientdvar( "ui_juiced_end_milliseconds", 0 );
    self setclientdvar( "ui_eyes_on_end_milliseconds", 0 );
    self setclientomnvar( "ui_cranked_bomb_timer_end_milliseconds", 0 );
}

resetuidvarsondeath()
{

}

watchforslide()
{
    self endon( "death" );
    self endon( "disconnect" );

    for (;;)
    {
        self waittill( "sprint_slide_begin" );
        self playfx( level._effect["slide_dust"], self geteye() );
    }
}
