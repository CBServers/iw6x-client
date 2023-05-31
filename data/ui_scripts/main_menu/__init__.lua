if (game:issingleplayer() or not Engine.InFrontend()) then
	return
end

package.loaded["LUI.mp_menus.MPMainMenu"].main_menu_options_feeder = function( f17_arg0 )
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
	if Engine.IsConsoleGame() then
		local f17_local7 = "@LUA_MENU_SPLITSCREEN_CAPS"
		if f17_local0 then
			f17_local7 = "@LUA_MENU_LOCAL_CAPS"
		elseif f17_local1 then
			f17_local7 = "@LUA_MENU_LOCAL_CAPS"
		end
		local f17_local8 = #f17_local6 + 1
		local f17_local9 = {
			type = "UIGenericButton",
			id = "btn_MPMain_1",
			disabled = f17_local3,
			disabledFunc = Engine.DoWeNeedCompatibilityPacks
		}
		local f17_local10 = {
			button_text = Engine.Localize( f17_local7 ),
			button_action_func = splitScreenButtonAction
		}
		local f17_local11
		if f17_local1 then
			f17_local11 = Engine.Localize( "@LUA_MENU_SQUAD_LOCAL_PLAY_DESC" )
			if not f17_local11 then
			
			else
				f17_local10.desc_text = f17_local11
				f17_local10.button_over_func = function ( f19_arg0, f19_arg1 )
					PersistentBackground.SetToDefault()
				end
				
				f17_local9.properties = f17_local10
				f17_local6[f17_local8] = f17_local9
				if not f17_local1 then
					f17_local6[#f17_local6 + 1] = {
						type = "UIGenericButton",
						id = "btn_MPMain_2",
						disabled = f17_local3,
						disabledFunc = Engine.DoWeNeedCompatibilityPacks,
						properties = {
							button_text = Engine.Localize( "@PLATFORM_SYSTEM_LINK_CAPS" ),
							button_action_func = LUI.mp_menus.MPMainMenu.systemLinkButtonAction,
							desc_text = Engine.Localize( "@PLATFORM_SYSTEM_LINK_DESC" ),
							button_over_func = function ( f20_arg0, f20_arg1 )
								PersistentBackground.SetToDefault()
							end
						}
					}
				end
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
					menu_create = AddExtinctionGlowBackground
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
	local f29_local5 = {
		type = "UIGenericButton",
		id = "leaderboards_button_id",
		properties = {
			button_text = Engine.Localize( "@LUA_MENU_LEADERBOARDS_CAPS" ),
			desc_text = Engine.Localize( "@LUA_MENU_DESC_LEADERBOARDS" ),
			button_action_func = function ( f31_arg0, f31_arg1 )
				if Engine.IsUserAGuest( f31_arg1.controller ) then
					LUI.FlowManager.RequestPopupMenu( f31_arg0, "popup_no_guest", true, f31_arg1.controller )
				else
					LUI.FlowManager.RequestAddMenu( f31_arg0, "leaderboards", true, f31_arg1.controller )
				end
			end
		}
	}
	if f29_local1 and f29_local2.HasLeaderboard then
		f29_local3[#f29_local3 + 1] = f29_local5
	end
	if f29_local1 and f29_local2 and f29_local2.HasReports then
		f29_local3[#f29_local3 + 1] = {
			type = "UIGenericButton",
			id = "squad_reports_button_id",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_SQUAD_REPORTS" ),
				desc_text = Engine.Localize( "@LUA_MENU_SQUAD_REPORTS_DESC" ),
				button_action_func = function ( f32_arg0, f32_arg1 )
					LUI.FlowManager.RequestAddMenu( f32_arg0, "squad_reports_menu", false, f32_arg1.controller, false, {
						controller = f32_arg1.controller
					} )
				end
			}
		}
	end
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
		if f29_local1 then
			if f29_local2 == SvS.SquadModes.SquadAssault then
				f29_local3[#f29_local3 + 1] = {
					type = "UIGenericButton",
					id = "squad_match_button_id",
					disabled = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons(),
					properties = {
						button_text = Engine.Localize( "LUA_MENU_CHALLENGE_FRIEND_CAPS" ),
						button_action_func = function ( f35_arg0, f35_arg1 )
							if IsFirstTimeFlowRequired( f35_arg1.controller ) then
								LUI.FlowManager.RequestAddMenu( f35_arg0, "cac_member_select_main", true, f35_arg1.controller, false, {
									next_screen = "cac_edit_main",
									squad_location = "squadMembers",
									class_location = "loadouts",
									findMatch = true
								} )
							elseif CheckHasRequiredDLC( f35_arg0 ) then
								LUI.FlowManager.RequestPopupMenu( f35_arg0, "popup_friends", true, f35_arg1.controller, false, {
									challengeMode = true
								} )
							end
						end,
						desc_text = Engine.Localize( "LUA_MENU_CHALLENGE_FRIEND_DESC" ),
						disabledFunc = LUI.mp_menus.MPXboxLiveMenu.disableCreateGameButtons,
						additional_handlers = {
							check_buttons = LUI.mp_menus.MPLivePrivateLobby.RefreshButtonDisable
						}
					}
				}
			end
			if f29_local2 and not f29_local2.RequiresMatchmaking then
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
	if not (not SvS.IsSvS() or f29_local2 ~= SvS.SquadModes.SquadAssault) or Engine.IsAliensMode() or Engine.IsCoreMode() then
		f29_local9 = 1
		if not f29_local9 then
		
		else
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

function FindMatchAction( f5_arg0, f5_arg1 )
	if Lobby.EnteringLobby() == true then
		LUI.FlowManager.RequestPopupMenu( f5_arg0, "popup_throttling", true, f5_arg1.controller, false, {
			eventData = f5_arg1
		} )
	else
		FindMatchAfterThrottleEvent( f5_arg0, f5_arg1 )
	end
end

function FindMatchAfterThrottleEvent( f4_arg0, f4_arg1 )
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

function BarracksAction( f9_arg0, f9_arg1 )
	LUI.FlowManager.RequestAddMenu( f9_arg0, "menu_stats", true, f9_arg1.controller ) -- custom stats menu
end

-- Remove social button
LUI.MenuBuilder.m_definitions["online_friends_widget"] = function()
	return {
		type = "UIElement"
	}
end
