if (game:issingleplayer() or not Engine.InFrontend()) then
	return
end

local Lobby = luiglobals.Lobby

local FindMatchAfterThrottleEvent = function( f4_arg0, f4_arg1 )
	local f4_local0 = false
	local f4_local1 = -1
	for f4_local2 = 0, Engine.GetMaxControllerCount() - 1, 1 do
		if Engine.HasActiveLocalClient( f4_local2 ) and IsFirstTimeFlowRequired( f4_local2 ) then
			f4_local0 = true
			if f4_local1 < 0 then
				f4_local1 = f4_local2
			end
		end
	end
	if f4_local0 then
		LUI.FlowManager.RequestAddMenu( f4_arg0, "cac_member_select_main", true, f4_local1, false, {
			next_screen = "cac_edit_main",
			squad_location = "squadMembers",
			class_location = "loadouts",
			findMatch = true
		} )
	elseif not LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons() then
		Engine.Exec( "xblive_privatematch 0" )
		if Engine.IsAliensMode() then
			LUI.mp_menus.Aliens.AliensRunConfig( f4_arg1.controller )
		end
		if LUI.mp_menus.MPXboxLiveMenu.CheckHasRequiredDLC( f4_arg0 ) then
			if LUI.mp_menus.MPXboxLiveMenu.DisplayLowRepWarning( f4_arg0, f4_arg1 ) then
				return 
			elseif SvS.IsSvS() then
				local f4_local3 = SvS.GetCurrentSquadModeInfo()
				local f4_local4, f4_local5 = SvS.GetPlaylistFromSquadMode( f4_local3 )
				local f4_local6 = false
				if f4_arg1.squadsPlayNow then
					f4_local6 = true
				end
				if not f4_arg1.squadsPlayNow and f4_local3.DynamicMatchmaking then
					Playlist.DoAction( f4_local4, f4_local5, true, f4_local6 )
				else
					Playlist.DoAction( f4_local4, f4_local5, false, f4_local6 )
				end
				if Engine.GetDvarBool( "squad_match" ) then
					Squad.StartMatch( f4_arg1.controller, true )
					Engine.SetDvarBool( "squad_find_match", true )
				end
				LUI.FlowManager.RequestAddMenu( f4_arg0, "menu_xboxlive_lobby", false, f4_arg1.controller, false )
			else
				LUI.FlowManager.RequestPopupMenu( f4_arg0, "menu_systemlink_join" ) -- open server list instead of playlist_main
			end
		end
	end
end

local FindMatchAction = function( f5_arg0, f5_arg1 )
	if Lobby.EnteringLobby() == true then
		LUI.FlowManager.RequestPopupMenu( f5_arg0, "popup_throttling", true, f5_arg1.controller, false, {
			eventData = f5_arg1
		} )
	else
		FindMatchAfterThrottleEvent( f5_arg0, f5_arg1 )
	end
end

local BarracksAction = function( f9_arg0, f9_arg1 )
	LUI.FlowManager.RequestAddMenu( f9_arg0, "menu_stats", true, f9_arg1.controller ) -- custom stats menu
end

package.loaded["LUI.mp_menus.MPXboxLiveMenu"].XboxLiveOptionsFeeder = function( f29_arg0 )
	local f29_local0 = Engine.IsAliensMode()
	local f29_local1 = SvS.IsSvS()
	local f29_local2 = SvS.IsSvS()
	if f29_local2 then
		f29_local2 = SvS.GetCurrentSquadModeInfo()
	end
	local f29_local3 = {}
	local f29_local4 = nil
	if f29_local0 then
		f29_local4 = Engine.Localize( "@LUA_MENU_STORE_CAPS" ) -- Orginally @LUA_MENU_PUBLIC_MATCH_CAPS but we need to use @LUA_MENU_STORE_CAPS
	elseif f29_local1 then
		f29_local4 = Engine.Localize( "@PLATFORM_FIND_GAME_CAPS" )
	else
		f29_local4 = Engine.Localize( "@LUA_MENU_STORE_CAPS" ) -- Orginally @PLATFORM_FIND_GAME_CAPS but we need to use @LUA_MENU_STORE_CAPS
	end
	f29_local3[#f29_local3 + 1] = {
		type = "UIGenericButton",
		id = "find_match_button_id",
		disabled = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons(),
		properties = {
			button_text = f29_local4,
			button_action_func = FindMatchAction,
			desc_text = SvS.IsSvS() and Engine.Localize( "@LUA_MENU_SQUADS_FIND_MATCH_DESC" ) or Engine.Localize( "@LUA_MENU_STORE_DESC" ), -- Orginally @PLATFORM_DESC_FIND_GAME but we need to use @LUA_MENU_STORE_DESC
			disabledFunc = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons,
			additional_handlers = {
				check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
			}
		}
	}
	if f29_local0 then
		f29_local3[#f29_local3 + 1] = {
			type = "UIGenericButton",
			id = "solo_match_button_id",
			disabled = LUI.mp_menus.MPXboxLiveMenu.shouldDisableSoloMatch(),
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_SOLO_MATCH_CAPS" ),
				button_action_func = LUI.mp_menus.MPXboxLiveMenu.SoloMatchAction,
				desc_text = Engine.Localize( "@LUA_MENU_SOLO_MATCH_DESC" ),
				disabledFunc = LUI.mp_menus.MPXboxLiveMenu.shouldDisableSoloMatch,
				additional_handlers = {
					check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
				}
			}
		}
		f29_local3[#f29_local3 + 1] = {
			type = "UIGenericButton",
			id = "private_match_button_id",
			disabled = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons(),
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_CUSTOM_MATCH_CAPS" ),
				button_action_func = LUI.mp_menus.MPXboxLiveMenu.PrivateMatchAction,
				desc_text = Engine.Localize( "@LUA_MENU_DESC_PRIVATE_MATCH" ),
				disabledFunc = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons,
				additional_handlers = {
					check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
				}
			}
		}
	end
	if not f29_local0 then
		f29_local3[#f29_local3 + 1] = {
			type = "UIGenericButton",
			id = "create_squad_button_id",
			disabled = false,
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_CREATE_A_CLASS_CAPS" ),
				button_action_func = LUI.mp_menus.MPXboxLiveMenu.CreateSquadAction,
				desc_text = Engine.Localize( "@LUA_MENU_DESC_CREATE_A_CLASS" ),
				additional_handlers = {
					refresh_new_icons = function ( f30_arg0, f30_arg1 )
						if Cac.AnyUnseenMDLCItems( Engine.GetFirstActiveController(), NewIconsTable.CACItemTypes ) then
							f30_arg0:processEvent( {
								name = "show_new_icon"
							} )
						end
					end
				}
			}
		}
	else
		f29_local3[#f29_local3 + 1] = LUI.mp_menus.AliensLoadout.GetAliensLoadoutButton()
	end

	-- Disable Leaderboard (Code Removed)
	-- Disable Squad Reports (Code Removed)

	if not f29_local0 and not f29_local1 then
		f29_local3[#f29_local3 + 1] = {
			type = "UIGenericButton",
			id = "operations_button_id",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_OPERATIONS_TITLE" ),
				button_action_func = LUI.mp_menus.MPBarracks.BarrackOperationsAction,
				desc_text = Engine.Localize( "@LUA_MENU_DESC_CHALLENGES" )
			}
		}
	end
	if not f29_local0 then
		if not f29_local1 then
			f29_local3[#f29_local3 + 1] = {
				type = "UIGenericButton",
				id = "barracks_button_id",
				disabled = false,
				properties = {
					button_text = Engine.Localize( "@LUA_MENU_BARRACKS_CAPS" ),
					button_action_func = BarracksAction,
					desc_text = Clan.IsEnabled() and Engine.Localize( "@LUA_MENU_DESC_BARRACKS" ) or Engine.Localize( "@LUA_MENU_DESC_BARRACKS_PRIVATE" )
				}
			}
		end
		if not f29_local1 or f29_local2 ~= SvS.SquadModes.SquadVsSquad then
			f29_local3[#f29_local3 + 1] = {
				type = "generic_separator"
			}
		end
		if not f29_local1 then
			f29_local3[#f29_local3 + 1] = {
				type = "UIGenericButton",
				id = "private_match_button_id",
				disabled = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons(),
				properties = {
					button_text = Engine.Localize( "@LUA_MENU_PRIVATE_MATCH_CAPS" ),
					button_action_func = LUI.mp_menus.MPXboxLiveMenu.PrivateMatchAction,
					desc_text = Engine.Localize( "@LUA_MENU_DESC_PRIVATE_MATCH" ),
					disabledFunc = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons,
					additional_handlers = {
						check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
					}
				}
			}
		end

		-- Add Combat Training Button
		if not f29_local0 and not f29_local1 then
			game:addlocalizedstring("LUA_MENU_COMBAT", "COMBAT TRAINING");
			game:addlocalizedstring("LUA_MENU_COMBAT_DESC", "Rank up offline with bots.");

			f29_local3[#f29_local3 + 1] = {
				type = "UIGenericButton",
				id = "find_match_button_id2",
				disabled = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons(),
				properties = {
					button_text = Engine.Localize("@LUA_MENU_COMBAT"), -- Engine.Localize( "@PLATFORM_FIND_GAME_CAPS" ),
					button_action_func = LUI.mp_menus.MPXboxLiveMenu.FindMatchAction,
					desc_text = Engine.Localize( "@LUA_MENU_COMBAT_DESC" ),  -- Engine.Localize( "@LUA_MENU_SQUADS_FIND_MATCH_DESC" ) 
					disabledFunc = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons,
					additional_handlers = {
						check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
					}
				}
			}
	    end
		
		if f29_local1 then
			-- Disable Challenge Friend Button (Code Removed)
			if f29_local2 then
				f29_local3[#f29_local3 + 1] = {
					type = "UIGenericButton",
					id = "play_now_button_id",
					disabled = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons(),
					properties = {
						button_text = Engine.Localize( "LUA_MENU_PLAY_NOW_CAPS" ),
						button_action_func = function ( f36_arg0, f36_arg1 )
							f36_arg1.squadsPlayNow = true
							FindMatchAction( f36_arg0, f36_arg1 )
						end,
						desc_text = Engine.Localize( "LUA_MENU_PLAY_NOW_DESC" ),
						disabledFunc = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons,
						additional_handlers = {
							check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
						}
					}
				}
			end
		end
	end
	local f29_local6 = #f29_local3 + 1
	local f29_local7 = {
		type = "button_desc_text",
		id = "prelobby_description_id"
	}
	local f29_local8 = {}
	local f29_local9
	if (SvS.IsSvS() or f29_local2 ~= SvS.SquadModes.SquadAssault) or Engine.IsAliensMode() or Engine.IsCoreMode() then
		f29_local9 = 1
		if f29_local9 then
			f29_local8.lines = f29_local9
			f29_local7.properties = f29_local8
			f29_local3[f29_local6] = f29_local7
			f29_local3[#f29_local3 + 1] = {
				type = "UITimer",
				id = "bnt_lock_tmr",
				properties = {
					event = "check_buttons",
					interval = 500,
					disposable = false,
					broadcastToRoot = true
				}
			}
			return f29_local3
		end
	end
	f29_local9 = nil
end
