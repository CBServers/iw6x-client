if (game:issingleplayer() or not Engine.InFrontend()) then
	return
end

package.loaded["LUI.mp_menus.MPLivePublicLobby"].LivePublicLobbyOptionsFeeder = function( f18_arg0 )
	local f18_local0 = Lobby.GetVoteMapName( MapVoting.MapA.refNum )
	local f18_local1 = Lobby.GetVoteMapName( MapVoting.MapB.refNum )
	local f18_local2 = Lobby.GetVoteMapImage( MapVoting.MapA.refNum )
	local f18_local3 = Lobby.GetVoteMapImage( MapVoting.MapB.refNum )
	local f18_local4 = LUI.mp_menus.Aliens.FixGameModeTextForChaos( Lobby.GetVoteMapGametype( MapVoting.MapA.refNum ) )
	local f18_local5 = LUI.mp_menus.Aliens.FixGameModeTextForChaos( Lobby.GetVoteMapGametype( MapVoting.MapB.refNum ) )
	local f18_local6 = LUI.mp_menus.MPLivePublicLobby.IsVotingFinished()
	local f18_local7 = not LUI.mp_menus.MPLivePublicLobby.GetLobbyLeaderboardStatus()
	local f18_local8 = Engine.IsAliensMode()
	local f18_local9 = SvS.IsSvS()
	if not f18_local9 then
		f18_local9 = Engine.GetDvarBool( "squad_match" )
	end
	local f18_local10 = 14
	local f18_local11 = {}
	if not f18_arg0.slot then
		DebugPrint( "[WARNING] CAC: using cac slot index 0, this is only ok if you are editing a menu" )
		local f18_local12 = 0
	end
	local f18_local13 = Cac.GetCustomClassLoc( f18_arg0.classLocation )
	

	-- Add Start Button to Public Match Menu
	function ReadyUpAction( f11_arg0, f11_arg1 )
		Engine.SetDvarInt( "party_minplayers", 1)
	end
	
	if not f18_local9 then
			f18_local11[#f18_local11 + 1] = {
				type = "UIGenericButton",
				id = "ready_up_button_id",
				disabledFunc = false,
				properties = {
					disabledFunc = false,
					button_text = Engine.Localize( "@LUA_MENU_START_GAME_CAPS" ),
					button_action_func = ReadyUpAction,
					desc_text = Engine.Localize( "@LUA_MENU_DESC_READY_UP" ),
			}
		}
	end
		
	if f18_local8 then
		local f18_local14 = Engine.GetDvarInt( "pt_AliensReadyUpPublicInUse" )
		if f18_local14 ~= nil and f18_local14 ~= 0 then
			f18_local11[#f18_local11 + 1] = {
				type = "UIGenericButton",
				id = "ready_up_button_id",
				disabledFunc = LUI.mp_menus.MPLivePublicLobby.GetReadyUpDisableValue,
				properties = {
					disabledFunc = LUI.mp_menus.MPLivePublicLobby.GetReadyUpDisableValue,
					button_text = "",
					button_action_func = LUI.mp_menus.MPLivePublicLobby.ReadyUpAction,
					desc_text = LUI.mp_menus.MPLivePublicLobby.GetReadyUpDescText,
					additional_handlers = {
						recheck_start_button_lock = LUI.mp_menus.MPLivePublicLobby.RefreshButtonDisable,
						element_refresh = LUI.mp_menus.MPLivePublicLobby.UpdateReadyUpText,
						menu_create = LUI.mp_menus.MPLivePublicLobby.UpdateReadyUpText,
						ready_up_button_refresh = LUI.mp_menus.MPLivePublicLobby.UpdateReadyUpText
					}
				},
				children = {
					{
						type = "UITimer",
						id = "ready_up_button_refresh_timer_id",
						properties = {
							event = "ready_up_button_refresh",
							interval = 100,
							disposable = false,
							broadcastToRoot = false
						}
					}
				}
			}
		end
	end
	if not f18_local8 then
		if not SvS.IsSvS() or SvS.GetCurrentSquadModeInfo() ~= SvS.SquadModes.SquadAssault then
			f18_local11[#f18_local11 + 1] = {
				type = "UIGenericButton",
				id = "create_squad_button_id",
				properties = {
					button_text = Engine.Localize( "@LUA_MENU_CREATE_A_CLASS_CAPS" ),
					button_action_func = LUI.mp_menus.MPLivePublicLobby.CreateSquadAction,
					desc_text = Engine.Localize( "@LUA_MENU_DESC_CREATE_A_CLASS" )
				}
			}
		end
	else
		f18_local11[#f18_local11 + 1] = LUI.mp_menus.AliensLoadout.GetAliensLoadoutButton()
	end
	if not f18_local8 and not f18_local9 then
		f18_local11[#f18_local11 + 1] = {
			type = "UIGenericButton",
			id = "operations_button_id",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_OPERATIONS_TITLE" ),
				button_action_func = LUI.mp_menus.MPBarracks.BarrackOperationsAction,
				desc_text = Engine.Localize( "@LUA_MENU_DESC_CHALLENGES" )
			}
		}
	end
	if not f18_local9 and not f18_local8 then
		f18_local11[#f18_local11 + 1] = {
			type = "UIGenericButton",
			id = "barracks_button_id",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_BARRACKS_CAPS" ),
				button_action_func = LUI.mp_menus.MPLivePublicLobby.BarracksAction,
				desc_text = Clan.IsEnabled() and Engine.Localize( "@LUA_MENU_DESC_BARRACKS" ) or Engine.Localize( "@LUA_MENU_DESC_BARRACKS_PRIVATE" )
			}
		}
	end
	-- Disable Leaderboard (Code Removed)
	if f18_local9 then
		local f18_local14 = SvS.GetCurrentSquadModeInfo()
		if f18_local14 and f18_local14.HasReports then
			f18_local11[#f18_local11 + 1] = {
				type = "UIGenericButton",
				id = "squad_reports_button_id",
				properties = {
					button_text = Engine.Localize( "@LUA_MENU_SQUAD_REPORTS" ),
					desc_text = Engine.Localize( "@LUA_MENU_SQUAD_REPORTS_DESC" ),
					button_action_func = function ( f19_arg0, f19_arg1 )
						LUI.FlowManager.RequestAddMenu( f19_arg0, "squad_reports_menu", false, f19_arg1.controller, false, {
							controller = f19_arg1.controller,
							fromLiveLobby = true
						} )
					end
				}
			}
		end
		if f18_local14 and f18_local14.HasCompare then
			f18_local11[#f18_local11 + 1] = {
				type = "UIGenericButton",
				id = "squad_compare_button_id",
				disabled = LUI.mp_menus.MPLivePublicLobby.CheckSquadCompareDisabled(),
				properties = {
					button_text = Engine.Localize( "@LUA_MENU_SQUAD_COMPARE" ),
					desc_text = Engine.Localize( "@LUA_MENU_SQUAD_COMPARE_DESC" ),
					button_action_func = function ( f20_arg0, f20_arg1 )
						if Squad.FoundMatch() then
							local f20_local0 = SvS.BuildSquadCompareData( f20_arg1.controller )
							if f20_local0 then
								LUI.FlowManager.RequestAddMenu( f20_arg0, "squad_report_detail_menu", false, f20_arg1.controller, false, {
									reportData = f20_local0,
									controller = f20_arg1.controller,
									isCompare = true
								} )
							end
						end
					end
				},
				handlers = {
					check_button_disable_status = function ( f21_arg0, f21_arg1 )
						local f21_local0 = f21_arg0
						local f21_local1 = f21_arg0.processEvent
						local f21_local2 = {}
						local f21_local3
						if not LUI.mp_menus.MPLivePublicLobby.CheckSquadCompareDisabled() then
							f21_local3 = "enable"
							if not f21_local3 then
							
							else
								f21_local2.name = f21_local3
								f21_local1( f21_local0, f21_local2 )
							end
						end
						f21_local3 = "disable"
					end
				}
			}
		end
	end
	if f18_local9 then
		local f18_local14 = SvS.GetCurrentSquadModeInfo()
		if not f18_local14.HasLeaderboard then
		
		else
			f18_local11[#f18_local11 + 1] = {
				type = "UIGenericButton",
				id = "leaderboards_button_id",
				properties = {
					button_text = Engine.Localize( "@LUA_MENU_LEADERBOARDS_CAPS" ),
					desc_text = Engine.Localize( "@LUA_MENU_DESC_LEADERBOARDS" ),
					button_action_func = function ( f22_arg0, f22_arg1 )
						if Engine.IsUserAGuest( f22_arg1.controller ) then
							LUI.FlowManager.RequestPopupMenu( f22_arg0, "popup_no_guest", true, f22_arg1.controller )
						else
							LUI.FlowManager.RequestAddMenu( f22_arg0, "leaderboards", true, f22_arg1.controller )
						end
					end
				}
			}
		end
	end
	if f18_local8 then
		f18_local11[#f18_local11 + 1] = {
			type = "UIGenericButton",
			id = "leaderboards_button_id",
			properties = {
				button_text = Engine.Localize( "@LUA_MENU_LEADERBOARDS_CAPS" ),
				desc_text = Engine.Localize( "@LUA_MENU_DESC_LEADERBOARDS" ),
				button_action_func = function ( f22_arg0, f22_arg1 )
					if Engine.IsUserAGuest( f22_arg1.controller ) then
						LUI.FlowManager.RequestPopupMenu( f22_arg0, "popup_no_guest", true, f22_arg1.controller )
					else
						LUI.FlowManager.RequestAddMenu( f22_arg0, "leaderboards", true, f22_arg1.controller )
					end
				end
			}
		}
	end
	if f18_local8 and LUI.mp_menus.Aliens.CanAccessIntelMenu() then
		f18_local11[#f18_local11 + 1] = LUI.mp_menus.AliensIntel.GetAliensIntelButton()
	end
	f18_local11[#f18_local11 + 1] = {
		type = "button_desc_text",
		id = "public_lobby_description_id",
		properties = {
			lines = 1
		}
	}
	f18_local11[#f18_local11 + 1] = {
		type = "UIElement",
		id = "just_a_spacer_id",
		focusable = false,
		states = {
			default = {
				leftAnchor = true,
				rightAnchor = true,
				topAnchor = true,
				bottomAnchor = false,
				left = 0,
				right = 0,
				top = 0,
				bottom = f18_local10
			}
		}
	}
	f18_local11[#f18_local11 + 1] = {
		type = "UITimer",
		id = "check_status_tmr",
		properties = {
			event = "check_button_disable_status",
			interval = 500,
			disposable = false,
			broadcastToRoot = true
		}
	}
	if not f18_local9 and not f18_local6 then
		f18_local11[#f18_local11 + 1] = {
			type = "map_button",
			id = "map_button_1",
			properties = {
				mapVoteIndex = MapVoting.MapA.refNum,
				mapImage = RegisterMaterial( f18_local2 ),
				mapName = f18_local0,
				gamemode = f18_local4,
				votesText = Engine.GetDvarString( "party_nextMapVoteStatus" ),
				votesDvarName = "party_nextMapVoteStatus",
				action = function ( f23_arg0, f23_arg1 )
					LUI.mp_menus.MPLivePublicLobby.VoteForMap( f23_arg0, f23_arg1.controller, MapVoting.MapA.voteIndex )
				end
			}
		}
		f18_local11[#f18_local11 + 1] = {
			type = "UIElement",
			id = "just_another_spacer_id",
			focusable = false,
			states = {
				default = {
					leftAnchor = true,
					rightAnchor = true,
					topAnchor = true,
					bottomAnchor = false,
					left = 0,
					right = 0,
					top = 0,
					bottom = 40
				}
			}
		}
		f18_local11[#f18_local11 + 1] = {
			type = "map_button",
			id = "map_button_2",
			properties = {
				mapVoteIndex = MapVoting.MapB.refNum,
				mapImage = RegisterMaterial( f18_local3 ),
				mapName = f18_local1,
				gamemode = f18_local5,
				votesText = Engine.GetDvarString( "party_alternateMapVoteStatus" ),
				votesDvarName = "party_alternateMapVoteStatus",
				action = function ( f24_arg0, f24_arg1 )
					LUI.mp_menus.MPLivePublicLobby.VoteForMap( f24_arg0, f24_arg1.controller, MapVoting.MapB.voteIndex )
				end
			}
		}
	end
	return f18_local11
end
