if (game:issingleplayer()) then
	return
end

if (package.loaded["LUI.mp_hud.OptionsMenu"] == nil) then
	return
end

game:addlocalizedstring("LUA_MENU_QUIT_TO_DESKTOP_CAPS", "QUIT TO DESKTOP")

package.loaded["LUI.mp_hud.OptionsMenu"].options_def = function()
	local f14_local0 = GameX.GetGameMode()
	local f14_local1 = Engine.TableLookup( GameTypesTable.File, GameTypesTable.Cols.Ref, f14_local0, GameTypesTable.Cols.ClassChoice ) == "1"

	if not f14_local1 then
		f14_local1 = GameX.UsesFakeLoadout()
	end

	local f14_local2 = LUI.mp_hud.OptionsMenu.checkTeamChoice( f14_local0 )
	local f14_local3 = GameX.IsRankedMatch()
	local f14_local4 = Engine.GetDvarBool( "splitscreen_ingame" )
	local f14_local5 = Game.GetOmnvar( "ui_team_selected" )
	local f14_local6 = Game.GetOmnvar( "ui_loadout_selected" )
	local f14_local7 = LUI.mp_hud.OptionsMenu.chooseClassCheck( f14_local3, f14_local5, f14_local2 )
	local self = LUI.UIVerticalList.new()
	self.id = "pause_selections_Id"

	self:registerAnimationState("default", {
		topAnchor = true,
		leftAnchor = true,
		bottomAnchor = false,
		rightAnchor = false,
		top = GenericMenuDims.menu_top,
		left = GenericMenuDims.menu_left,
		bottom = GenericMenuDims.menu_bottom,
		right = GenericMenuDims.menu_right,
		alignment = LUI.Alignment.Top
	})

	self:animateToState( "default", 0 )
	self:makeFocusable()

	if f14_local0 ~= "aliens" and false == CoD.IsFireTeamMode() and GameX.IsSpectatingNotOnTeam() == false and f14_local1 == true and f14_local7 == true and not MLG.IsMLGSpectator() then
		LUI.MenuBuilder.BuildAddChild(self, {
			type = "UIGenericButton",
			id = "btn_MPPause_0",
			properties = {
				childNum = 1,
				button_text = Engine.Localize( "@LUA_MENU_CHOOSE_CLASS_CAPS" ),
				button_action_func = LUI.mp_hud.OptionsMenu.chooseClassButtonAction
			}
		})
	end

	if f14_local0 ~= "aliens" and false == CoD.IsFireTeamMode() and f14_local2 == true and not MLG.IsMLGSpectator() then
		LUI.MenuBuilder.BuildAddChild(self, {
			type = "UIGenericButton",
			id = "btn_MPPause_1",
			properties = {
				childNum = 2,
				button_text = Engine.Localize( "@LUA_MENU_CHANGE_TEAM_CAPS" ),
				button_action_func = LUI.mp_hud.OptionsMenu.changeTeamButtonAction
			}
		})
	end

	LUI.MenuBuilder.BuildAddChild(self, {
		type = "UIGenericButton",
		id = "btn_MPPause_2",
		disabledFunc = LUI.mp_hud.OptionsMenu.optionsLockedUpdate,
		properties = {
			childNum = 3,
			button_text = Engine.Localize( "@LUA_MENU_OPTIONS_CAPS" ),
			button_action_func = LUI.mp_hud.OptionsMenu.optionsButtonAction
		},
		handlers = {
			refresh_options_button = LUI.mp_hud.OptionsMenu.refreshOptionDisable
		}
	})

	if GameX.IsOnlineMatch() and (not Engine.IsAliensMode() or not Game.GetOmnvar( "ui_alien_is_solo" )) and not MLG.IsMLGSpectator() then
		LUI.MenuBuilder.BuildAddChild(self, {
			type = "UIGenericButton",
			id = "btn_MPPause_3",
			properties = {
				childNum = 4,
				button_text = Engine.Localize( "@LUA_MENU_MUTE_PLAYERS_CAPS" ),
				button_action_func = LUI.mp_hud.OptionsMenu.mutePlayersButtonAction
			}
		})
	end

	LUI.MenuBuilder.BuildAddChild(self, {
		type = "UIGenericButton",
		id = "btn_MPPause_4",
		properties = {
			childNum = 5,
			button_text = Engine.Localize( "@LUA_MENU_END_GAME_CAPS" ),
			button_action_func = LUI.mp_hud.OptionsMenu.endGameButtonAction
		}
	})

	LUI.MenuBuilder.BuildAddChild(self, {
		type = "UIGenericButton",
		id = "btn_MPPause_5",
		properties = {
			childNum = 6,
			button_text = Engine.Localize( "@LUA_MENU_QUIT_TO_DESKTOP_CAPS" ),
			button_action_func = MBh.PopupMenu( "quit_popmenu", false, false )
		}
	})

	return self
end
