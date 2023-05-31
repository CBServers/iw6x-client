if (game:issingleplayer() or not Engine.InFrontend()) then
	return
end

game:addlocalizedstring("LUA_MENU_UNLOCKALL", "UNLOCK ALL")
game:addlocalizedstring("LUA_MENU_UNLOCKALL_DESC",
	"Whether all items should be unlocked.")

function UnlockAllAction( f1_arg0, f1_arg1 )
	Engine.Exec("unlockstats")
end

function StatsButtonHelper( f2_arg0, f2_arg1 )
	f2_arg0:processEvent( LUI.ButtonHelperText.CommonEvents.addBackButton )
end

function BarrackPrestigeResetStatsAction( f3_arg0, f3_arg1 )
	LUI.FlowManager.RequestAddMenu( f3_arg0, "prestige_reset", true, f3_arg1.controller )
end

function menu_stats()
	return {
		type = "UIElement",
		id = "menu_stats_root",
		states = {
			default = {
				topAnchor = true,
				bottomAnchor = true,
				leftAnchor = true,
				rightAnchor = true,
				top = 0,
				bottom = 0,
				left = 0,
				right = 0
			}
		},
		handlers = {
			menu_create = StatsButtonHelper,
		},
		children = {
			{
				type = "generic_menu_title",
				id = "stats_title_text_id",
				properties = {
					menu_title = Engine.Localize( "@LUA_MENU_BARRACKS_CAPS" )
				}
			},
			{
				type = "UIVerticalList",
				id = "stats_options_vlist_id",
				states = {
					default = {
						alignment = LUI.Alignment.Top,
						leftAnchor = true,
						rightAnchor = false,
						topAnchor = true,
						bottomAnchor = false,
						left = GenericMenuDims.menu_left,
						right = GenericMenuDims.menu_right,
						top = GenericMenuDims.menu_top,
						bottom = GenericMenuDims.menu_bottom
					}
				},
				childrenFeeder = StatsOptionsFeeder
			},
			{
				type = "button_helper_text_main",
				id = "stats_button_helper_text_id"
			},
			{
				type = "UIBindButton",
				id = "stats_bind_buttons_id",
				handlers = {
					button_secondary = MBh.LeaveMenu()
				}
			},
		}
	}
end

function StatsOptionsFeeder( f8_arg0 )
	local f8_local4 = {}
	local f8_local0 = Engine.IsAliensMode()
	local f8_local2 = Engine.InLobby()
	local f8_local3 = f8_arg0.exclusiveController
	f8_local4[#f8_local4 + 1] = {
		type = "UIGenericButton",
		id = "unlockall_items_button_id",
		properties = {
			button_text = Engine.Localize( "@LUA_MENU_UNLOCKALL" ),
			button_action_func = UnlockAllAction,
			desc_text = Engine.Localize( "@LUA_MENU_UNLOCKALL_DESC" )
		}
	}
	if not f8_local2 then
		local f8_local5 = #f8_local4 + 1
		local f8_local6 = {
			type = "UIGenericButton",
			id = "reset_stats_button_id"
		}
		local f8_local7
		if Cac.GetPrestigeLevel( f8_local3, Cac.GetSquadLoc() ) == 10 then
			f8_local7 = Engine.IsUserAGuest( f8_local3 )
		else
			f8_local7 = true
		end
		f8_local6.disabled = f8_local7
		f8_local6.properties = {
			button_text = Engine.Localize( "@LUA_MENU_MP_RESET_STATS_CAPS" ),
			button_action_func = BarrackPrestigeResetStatsAction,
			desc_text = Engine.Localize( "@LUA_MENU_MP_RESET_STATS_DESC" )
		}
		f8_local6.handlers = {
			element_refresh = function ( f12_arg0, f12_arg1 )
				local f12_local0
				if Cac.GetPrestigeLevel( f8_local3, Cac.GetSquadLoc() ) == 10 then
					f12_local0 = Engine.IsUserAGuest( f8_local3 )
				else
					f12_local0 = true
				end
				local f12_local1 = f12_arg0
				local f12_local2 = f12_arg0.processEvent
				local f12_local3 = {}
				local f12_local4
				if f12_local0 then
					f12_local4 = "disable"
					if not f12_local4 then
						
					else
						f12_local3.name = f12_local4
						f12_local2( f12_local1, f12_local3 )
					end
				end
				f12_local4 = "enable"
			end
		}
		f8_local4[f8_local5] = f8_local6
	end
	f8_local4[#f8_local4 + 1] = {
		type = "button_desc_text",
		id = "stats_button_description_id"
	}
	return f8_local4
end

function stats_options_vlist()
	return {
		type = "UIVerticalList",
		focusable = true,
		states = {
			default = {
				alignment = LUI.Alignment.Top,
				leftAnchor = true,
				rightAnchor = false,
				topAnchor = true,
				bottomAnchor = false,
				left = GenericMenuDims.menu_left,
				right = GenericMenuDims.menu_right,
				top = GenericMenuDims.menu_top,
				bottom = GenericMenuDims.menu_bottom
			}
		}
	}
end

LUI.MenuBuilder.registerDef( "menu_stats", menu_stats )
LUI.MenuBuilder.registerDef( "stats_options_vlist", stats_options_vlist )
