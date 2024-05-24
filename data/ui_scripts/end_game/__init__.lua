if (game:issingleplayer() or Engine.InFrontend()) then
	return
end

f0_local0 = function ( f1_arg0, f1_arg1 )
	LUI.FlowManager.RequestLeaveMenu( f1_arg0 )
end

f0_local1 = function ( f2_arg0, f2_arg1 )
	f2_arg0:setText( f2_arg1.message_text )
	f2_arg0:dispatchEventToRoot( {
		name = "resize_popup"
	} )
end

f0_local2 = function ( f3_arg0 )
	Engine.ExecFirstClient( "xpartybackout" )
	Engine.ExecFirstClient( "disconnect" )
end

local f0_local3 = function ( f4_arg0 )
	Engine.ExecFirstClient( "xpartydisbandafterround" )
	Engine.ExecFirstClient( "disconnect" )
end

local f0_local4 = function ( f5_arg0 )
	return Engine.GetDvarBool( "onlinegame" )
end

local f0_local5 = function ( f6_arg0 )
	if f0_local4( f6_arg0 ) then
		Engine.ExecFirstClient( "xstopprivateparty" )
		Engine.ExecFirstClient( "disconnect" )
		Engine.ExecFirstClient( "xblive_privatematch 0" )
		Engine.ExecFirstClient( "onlinegame 1" )
		Engine.ExecFirstClient( "xstartprivateparty" )
	else
		Engine.ExecFirstClient( "disconnect" )
	end
end

local f0_local6 = function ( f7_arg0 )
	Engine.ExecFirstClient( "xstopprivateparty" )
	Engine.ExecFirstClient( "xpartydisbandafterround" )

	if Engine.GetDvarBool( "sv_running" ) then
		Engine.NotifyServer( "end_game", 1 )
		Engine.ExecFirstClient( "xblive_privatematch 0" )
		Engine.ExecFirstClient( "onlinegame 1" )
		Engine.ExecFirstClient( "xstartprivateparty" )
	else
		f0_local5( f7_arg0 )
	end
end

local f0_local7 = function ( f8_arg0, f8_arg1 )
	if Engine.GetDvarBool( "sv_running" ) then
		Engine.NotifyServer( "end_game", 1 )
	else
		f0_local5( f8_arg0 )
	end
	LUI.FlowManager.RequestCloseAllMenus( f8_arg0 )
end

local f0_local8 = function ( f9_arg0, f9_arg1 )
	LUI.FlowManager.RequestLeaveMenu( f9_arg0 )
	Engine.Exec( "onPlayerQuit" )
	if Engine.GetDvarBool( "sv_running" ) then
		f0_local3( f9_arg0 )
	else
		f0_local2( f9_arg0 )
	end
	LUI.FlowManager.RequestCloseAllMenus( f9_arg0 )
end

local f0_local9 = function ( f10_arg0, f10_arg1 )
	LUI.FlowManager.RequestLeaveMenu( f10_arg0 )
	Engine.Exec( "onPlayerQuit" )
	if Engine.GetDvarBool( "sv_running" ) then
		f0_local6( f10_arg0 )
	else
		f0_local5( f10_arg0 )
	end
	LUI.FlowManager.RequestCloseAllMenus( f10_arg0 )
end

local f0_local10 = function ( f11_arg0 )
	local f11_local0 = Lobby.IsInPrivateParty()
	if f11_local0 then
		f11_local0 = Lobby.IsPrivatePartyHost()
		if f11_local0 then
			f11_local0 = not Lobby.IsAloneInPrivateParty()
		end
	end
	return f11_local0
end

local f0_local11 = function ( f12_arg0, f12_arg1 )
	local f12_local0 = Engine.GetDvarBool( "squad_match" )
	local f12_local1 = Engine.GetDvarBool( "squad_use_hosts_squad" )
	if f12_local0 and Lobby.IsInPrivateParty() and Lobby.IsPrivatePartyHost() then
		Engine.NotifyServer( "end_game", 1 )
	elseif f12_local1 and Lobby.IsPrivatePartyHost() and Lobby.IsAloneInPrivateParty() and Lobby.IsAlone() then
		Engine.NotifyServer( "end_game", 1 )
	elseif f0_local10( f12_arg0 ) then
		LUI.FlowManager.RequestLeaveMenu( f12_arg0, true )
		LUI.FlowManager.RequestPopupMenu( f12_arg0, "popup_pull_party", false )
	else
		Engine.Exec( "onPlayerQuit" )
		if Engine.GetDvarBool( "sv_running" ) then
			f0_local6( f12_arg0 )
		else
			f0_local5( f12_arg0 )
		end
		LUI.FlowManager.RequestCloseAllMenus( f12_arg0 )
	end
end

local f0_local12 = function ()
	local self = LUI.UIElement.new()
	self.id = "end_game_id"
	self:registerAnimationState( "default", {
		topAnchor = true,
		leftAnchor = true,
		bottomAnchor = true,
		rightAnchor = true,
		top = -50,
		left = 0,
		bottom = 0,
		right = 0,
		alpha = 1
	} )
	self:animateToState( "default", 0 )
	local f13_local1 = Engine.Localize( "@LUA_MENU_END_GAME_DESC" )
	local f13_local2 = Engine.Localize( "@LUA_MENU_LEAVE_GAME_TITLE" )
	if Engine.IsAliensMode() and Game.GetOmnvar( "ui_alien_is_solo" ) then
		f13_local1 = Engine.Localize( "@ALIENS_LEAVE_GAME_DESC" )
		f13_local2 = Engine.Localize( "@MENU_NOTICE" )
	end
	LUI.MenuBuilder.BuildAddChild( self, {
		type = "generic_yesno_popup",
		id = "privateGame_options_list_id",
		properties = {
			message_text_alignment = LUI.Alignment.Center,
			message_text = f13_local1,
			popup_title = f13_local2,
			padding_top = 12,
			yes_action = f0_local7
		}
	} )
	local f13_local3 = LUI.UIBindButton.new()
	f13_local3.id = "endBackToGameStartButton"
	f13_local3:registerEventHandler( "button_start", f0_local0 )
	self:addElement( f13_local3 )
	return self
end

local f0_local13 = function ()
	local self = LUI.UIElement.new()
	self.id = "leave_game_id"
	self:registerAnimationState( "default", {
		topAnchor = true,
		leftAnchor = true,
		bottomAnchor = true,
		rightAnchor = true,
		top = -50,
		left = 0,
		bottom = 0,
		right = 0,
		alpha = 1
	} )
	self:animateToState( "default", 0 )
	LUI.MenuBuilder.BuildAddChild( self, {
		type = "generic_yesno_popup",
		id = "publicGame_options_list_id",
		properties = {
			message_text_alignment = LUI.Alignment.Center,
			message_text = Engine.IsAliensMode() and Engine.Localize( "@ALIENS_LEAVE_GAME_DESC" ) or Engine.Localize( "@LUA_MENU_LEAVE_GAME_DESC" ),
			popup_title = Engine.IsAliensMode() and Engine.Localize( "@MENU_NOTICE" ) or Engine.Localize( "@LUA_MENU_LEAVE_GAME_TITLE" ),
			padding_top = 12,
			yes_action = f0_local11
		}
	} )
	local f14_local1 = LUI.UIBindButton.new()
	f14_local1.id = "leaveBackToGameStartButton"
	f14_local1:registerEventHandler( "button_start", f0_local0 )
	self:addElement( f14_local1 )
	return self
end

local f0_local14 = function ()
	local self = LUI.UIElement.new()
	self.id = "pull_party_out_id"
	self:registerAnimationState( "default", {
		topAnchor = true,
		leftAnchor = true,
		bottomAnchor = true,
		rightAnchor = true,
		top = -50,
		left = 0,
		bottom = 0,
		right = 0,
		alpha = 1
	} )
	self:animateToState( "default", 0 )
	LUI.MenuBuilder.BuildAddChild( self, {
		type = "generic_yesno_popup",
		id = "party_pullout_list_id",
		properties = {
			message_text_alignment = LUI.Alignment.Center,
			message_text = Engine.Localize( "@LUA_MENU_PULL_PARTY_DESC" ),
			popup_title = Engine.Localize( "@LUA_MENU_LEAVE_GAME_TITLE" ),
			padding_top = 12,
			yes_action = f0_local8,
			no_action = f0_local9,
			cancel_means_no = false
		}
	} )
	local f15_local1 = LUI.UIBindButton.new()
	f15_local1.id = "leavePullPartyButton"
	f15_local1:registerEventHandler( "button_start", f0_local0 )
	self:addElement( f15_local1 )
	return self
end

LUI.MenuBuilder.m_types["popup_end_game"] = f0_local12
LUI.MenuBuilder.m_types["popup_leave_game"] = f0_local13
LUI.MenuBuilder.m_types["popup_pull_party"] = f0_local14
