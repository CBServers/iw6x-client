if (game:issingleplayer() or not Engine.InFrontend()) then
	return
end

local SquadsModeButtonAction = function( f4_arg1 , f4_arg0) 
	controller = f4_arg0.controller
	Engine.ExecNow( "forcenosplitscreencontrol main_XBOXLIVE_1", controller )
	local f4_local0, f4_local1 = Engine.UserCanPlayOnline( controller )
	if not f4_local0 then
		Engine.Exec( "xrequirelivesignin", controller )
		Engine.Exec( "forcenosplitscreencontrol main_XBOXLIVE_2", controller )
	else
		if SvS.IsSvS() then
			Engine.Exec( "profile_SetHasEverPlayed_SP", controller )
		end
		Engine.ExecNow( "resetSplitscreenSignIn", controller )
		Engine.ExecNow( "forcenosplitscreencontrol main_XBOXLIVE_3", controller )
		Engine.SetDvarBool( "systemlink", false )
		Engine.SetDvarBool( "splitscreen", false )
		Engine.SetDvarBool( "onlinegame", true )
		Engine.SetDvarBool( "xblive_privatematch", false )
		Engine.SetDvarBool( "ui_opensummary", false )
		Engine.SetDvarBool( "squad_match", false )
		Engine.SetDvarInt( "allow_online_squads", 1 )
		if Engine.GetDvarBool( "xblive_competitionmatch" ) then
			Engine.SetDvarBool( "xblive_competitionmatch", false )
			Engine.Exec( "set remove_mlg_rules 1" )
		end
		Engine.ExecNow( MPConfig.default_xboxlive, controller )
		if not SvS.IsSvS() then
			Engine.ExecNow( "xstartprivateparty", controller )
		end
		Engine.Exec( "startentitlements", controller )
		Engine.CacheUserDataForController( controller )
		if SvS.IsSvS() then
			LUI.FlowManager.RequestAddMenu( nil, "squads_mode_select_menu", false, controller, false )
		else
			LUI.FlowManager.RequestAddMenu( nil, "menu_xboxlive", false, controller, false )
		end
	end
end

local main_menu_options_feeder = function( f17_arg0 )
	local f17_local0 = Engine.IsAliensMode()
	local f17_local1 = SvS.IsSvS()
	local f17_local2 = false
	if Engine.GetDvarInt( "allow_online_squads" ) == 1 or not Engine.IsConsoleGame() then
		f17_local2 = true
	end
	local f17_local3 = Engine.DoWeNeedCompatibilityPacks()
	if f17_local1 then
		local f17_local4 = f17_local2
	end
	local f17_local5 = f17_local4 or not f17_local1
	local f17_local6 = {}
	if Engine.AllowOnline() and f17_local5 then
		local f17_local7, f17_local8 = nil
		if f17_local1 then
			f17_local8 = Engine.Localize( "@PLATFORM_PLAY_ONLINE_SQUADS_CAPS" )
			f17_local7 = Engine.Localize( "@LUA_MENU_SQUADS_INTRO" )
		elseif f17_local0 then
			f17_local8 = Engine.Localize( "@PLATFORM_PLAY_ONLINE_CAPS" )
			f17_local7 = Engine.Localize( "@LUA_MENU_PLAY_EXTINCTION_ONLINE_DESC" )
		else
			f17_local8 = Engine.Localize( "@PLATFORM_PLAY_ONLINE_CAPS" )
			f17_local7 = Engine.Localize( "@PLATFORM_PLAY_ONLINE_DESC" )
		end
		f17_local6[#f17_local6 + 1] = {
			type = "UIGenericButton",
			id = "btn_MPMain_0",
			disabled = f17_local3,
			disabledFunc = Engine.DoWeNeedCompatibilityPacks,
			properties = {
				button_text = f17_local8,
				button_action_func = LUI.mp_menus.MPMainMenu.xboxLiveButtonAction,
				desc_text = f17_local7,
				button_over_func = function ( f18_arg0, f18_arg1 )
					PersistentBackground.SetToDefault()
				end
			}
		}
	end
	if SvS.IsSvS() then
		local f17_local7 = "@PLATFORM_PLAY_ONLINE_SQUADS_CAPS"
		local f17_local8 = #f17_local6 + 1
		local f17_local9 = {
			type = "UIGenericButton",
			id = "btn_MPMain_1",
			disabled = f17_local3,
			disabledFunc = Engine.DoWeNeedCompatibilityPacks
		}
		local f17_local10 = {
			button_text = Engine.Localize( f17_local7 ),
			button_action_func = SquadsModeButtonAction
		}
		local f17_local11
		if f17_local1 then
			f17_local11 = Engine.Localize( "@LUA_MENU_SQUADS_INTRO" )
			if not f17_local11 then
			
			else
				f17_local10.desc_text = f17_local11
				f17_local10.button_over_func = function ( f19_arg0, f19_arg1 )
					PersistentBackground.SetToDefault()
				end
				
				f17_local9.properties = f17_local10
				f17_local6[f17_local8] = f17_local9
			end
		end
		f17_local11 = Engine.Localize( "@LUA_MENU_SPLITSCREEN_DESC" )
	end
	f17_local6[#f17_local6 + 1] = {
		type = "UIGenericButton",
		id = "btn_MPMain_6",
		properties = {
			button_text = Engine.Localize( "@LUA_MENU_OPTIONS_CAPS" ),
			button_action_func = LUI.mp_menus.MPMainMenu.optionsButtonAction,
			desc_text = Engine.Localize( "@LUA_MENU_OPTIONS_DESC" ),
			button_over_func = function ( f22_arg0, f22_arg1 )
				PersistentBackground.SetToDefault()
			end
		}
	}
	f17_local6[#f17_local6 + 1] = {
		type = "generic_separator",
		id = "main_menu_spacer_id"
	}
	if not Engine.IsCoreMode() then
		f17_local6[#f17_local6 + 1] = {
			type = "UIGenericButton",
			id = "btn_MPMain_7",
			properties = {
				text = Engine.Localize( "@LUA_MENU_MULTIPLAYER_CAPS" ),
				button_action_func = function ( f23_arg0, f23_arg1 )
					Engine.StopMusic( 200 )
					Engine.SwitchToCoreMode()
					Engine.PlayMusic( CoD.Music.MainMPMusic )
					Engine.SetActiveMenu( ActiveMenus.None )
					Engine.SetActiveMenu( ActiveMenus.Main )
				end,
				button_over_func = function ( f24_arg0, f24_arg1 )
					PersistentBackground.Set( PersistentBackground.Variants.MPBackground )
				end,
				desc_text = Engine.Localize( "@PLATFORM_PLAY_ONLINE_DESC" )
			}
		}
	end
	if not SvS.IsSvS() then
		f17_local6[#f17_local6 + 1] = {
			type = "UIGenericButton",
			id = "btn_MPMain_8",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_SQUAD_MODE_CAP" ),
				button_action_func = function ( f25_arg0, f25_arg1 )
					Engine.StopMusic( 200 )
					Engine.SwitchToSquadVsSquadMode()
					Engine.PlayMusic( CoD.Music.MainSquadMusic )
					Engine.SetActiveMenu( ActiveMenus.None )
					Engine.SetActiveMenu( ActiveMenus.Main )
				end,
				button_over_func = function ( f26_arg0, f26_arg1 )
					PersistentBackground.Set( PersistentBackground.Variants.SvSBackground )
				end,
				desc_text = Engine.Localize( "@LUA_MENU_SVS_MAIN_MENU_DESC" )
			}
		}
	end
	if not Engine.IsAliensMode() and Engine.UnlockedAliens() then
		f17_local6[#f17_local6 + 1] = {
			type = "UIGenericButton",
			id = "btn_MPMain_9",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_ALIENS_CAPS" ),
				button_action_func = function ( f27_arg0, f27_arg1 )
					Engine.StopMusic( 200 )
					Engine.SwitchToAliensMode()
					Engine.PlayMusic( CoD.Music.MainExtinctMusic )
					Engine.SetActiveMenu( ActiveMenus.None )
					Engine.SetActiveMenu( ActiveMenus.Main )
				end,
				button_over_func = function ( f28_arg0, f28_arg1 )
					PersistentBackground.Set( PersistentBackground.Variants.AliensBackground )
				end,
				desc_text = Engine.Localize( "@LUA_MENU_ALIENS_MAIN_MENU_DESC" ),
				additional_handlers = {
					menu_create = LUI.mp_menus.MPMainMenu.AddExtinctionGlowBackground
				}
			}
		}
	end
	f17_local6[#f17_local6 + 1] = {
		type = "button_desc_text",
		id = "mp_menu_button_description_id",
		properties = {
			lines = SvS.IsSvS() and 8 or nil
		}
	}
	return f17_local6
end

-- Remove social button
LUI.MenuBuilder.m_definitions["online_friends_widget"] = function()
	return {
		type = "UIElement"
	}
end

f0_local0 = function ()
	return {
		type = "UIVerticalList",
		id = "mp_main_menu_id",
		childrenFeeder = main_menu_options_feeder,
		states = {
			default = {
				topAnchor = true,
				bottomAnchor = false,
				leftAnchor = true,
				rightAnchor = false,
				top = GenericMenuDims.menu_top,
				bottom = GenericMenuDims.menu_bottom,
				left = GenericMenuDims.menu_left,
				right = GenericMenuDims.menu_right,
				alignment = LUI.Alignment.Top
			}
		},
		handlers = {
			dlc_mount_complete = function ( f30_arg0, f30_arg1 )
				f30_arg0:processEvent( {
					name = "menu_refresh"
				} )
			end
			,
			dlc_mount_fail = function ( f31_arg0, f31_arg1 )
				f31_arg0:processEvent( {
					name = "menu_refresh"
				} )
			end
			,
			dlc_download_fail = function ( f32_arg0, f32_arg1 )
				f32_arg0:processEvent( {
					name = "menu_refresh"
				} )
			end
			
		}
	}
end

LUI.MenuBuilder.m_definitions[ "main_mp_menu_options"] = f0_local0 
