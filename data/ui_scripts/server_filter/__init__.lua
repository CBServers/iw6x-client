if (game:issingleplayer() or not Engine.InFrontend()) then
    return
end

local Lobby = luiglobals.Lobby

game:addlocalizedstring("LUA_MENU_SERVER_FILTER_POPUP_INSTR", "Change how the servers are filtered")

function FiltersPopupClose(f1_arg0, f1_arg1)
	local f1_local0 = LUI.FlowManager.GetMenuScopedDataFromElement(f1_arg0)
	local f1_local1 = LUI.FlowManager.GetMenuScopedDataByMenuName("mp_leaderboard_main")
	local f1_local2 = f1_local1.leaderboardType
	if f1_local2 and f1_local2 ~= "" then
		Leaderboards.OpenLeaderboard(f1_arg0, f1_local2, f1_local0.filterKey, f1_local1.filterDurationKey, f1_local1.isHardcore)
	end
end

function mp_server_filters_popup()
	return {
		type = "UIElement",
		id = "mp_leaderboard_filters_popup_container_id",
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
		children = {
			{
				type = "UIImage",
				id = "darken_bg",
				states = {
					default = CoD.ColorizeState(Swatches.Overlay.Color, {
						topAnchor = true,
						bottomAnchor = true,
						leftAnchor = true,
						rightAnchor = true,
						left = 0,
						right = 0,
						top = 0,
						bottom = 0,
						material = RegisterMaterial("white"),
						alpha = Swatches.Overlay.AlphaMore
					})
				}
			},
			{
				type = "UIElement",
				id = "mp_server_filters_popup_id",
				states = {
					default = {
						topAnchor = false,
						bottomAnchor = false,
						leftAnchor = false,
						rightAnchor = false,
						top = -1 * Leaderboards.Layout.FilterHeight * 0.6,
						width = Leaderboards.Layout.FilterWidth,
						height = 170
					}
				},
				children = {
					{
						type = "generic_drop_shadow",
						properties = {
							offset_shadow = 0
						}
					},
					{
						type = "generic_menu_titlebar",
						id = "server_filters_popup_title_bar_id",
						properties = {
							title_bar_text = Engine.Localize("@LUA_MENU_FILTER_CAPS"),
							fill_alpha = 1
						}
					},
					{
						type = "generic_menu_background",
						id = "server_filters_popup_background",
						properties = {
							fill_alpha = 1
						}
					},
					{
						type = "mp_server_filters_popup_page"
					},
					{
						type = "UIBindButton",
						id = "filters_popup_back_button",
						handlers = {
							button_secondary = MBh.DoMultiple({
								MBh.LeaveMenu(),
								FiltersPopupClose,
								ForceRefreshServers
							})
						}
					}
				}
			}
		}
	}
end

function ForceRefreshServers(f4_arg0, f4_arg1)
	local f4_local0 = LUI.FlowManager.GetMenuScopedDataByMenuName("menu_systemlink_join")
	f4_local0.serverCount = 0
	if f4_local0.serverList then
		local f4_local1 = Lobby.RefreshServerList
		local f4_local2 = f4_arg1.controller
		if not f4_local2 then
			f4_local2 = Engine.GetFirstActiveController()
		end
		f4_local1(f4_local2)
		f4_local0.serverList:processEvent({
			name = "lose_focus",
			immediate = true
		})
		f4_local0.serverList:clearSavedState()
		f4_local0.serverList:processEvent({
			name = "menu_refresh",
			dispatchChildren = true
		})
	end
end

function OpenFiltersMenu(f36_arg0, f36_arg1)
	LUI.FlowManager.RequestPopupMenu(f36_arg0, "mp_server_filters_popup", true, f36_arg1.controller, false, {})
end

LUI.MenuBuilder.registerDef("mp_server_filters_popup", mp_server_filters_popup)

GameTypeRefCol = 0
GameTypeNameCol = 1
GameTypeDescCol = 2
GameTypeImageCol = 3

function GetGametypes()
	if Engine.IsCoreMode() then
		return { "any", "dm", "war", "sd", "dom", "conf", "sr", "infect", "blitz", "grind", "cranked", "sotf", "sotf_ffa",
			"horde", "gun", "grnd", "siege" }
	end
	return { "aliens" }
end

function GetLocalizedStringFromGametype(gametype)
	if gametype == "any" then
		return string.upper(Engine.Localize("LUA_MENU_LB_FILTER_GROUP_ALL"))
	end
	local gametype_name = Engine.TableLookup("mp/gameTypesTable.csv", GameTypeRefCol, gametype, GameTypeNameCol)
	if gametype_name ~= "" then
		local token = "@"
		local caps_token = "_CAPS"
		localized = Engine.Localize(token .. gametype_name .. caps_token)
		return localized
	end
	return "UNKNOWN"
end

function GetMapnameFromID(id)
	if id == "any" then
		return string.upper(Engine.Localize("LUA_MENU_LB_FILTER_GROUP_ALL"))
	end

	if id == "mp_descent_new" then
		return Engine.Localize("@LUA_MENU_MAPNAME_DESCENT_CAPS")
	elseif id == "mp_favela_iw6" then
		return string.upper(Engine.Localize("@PRESENCE_MP_FAVELA_IW6"))
	elseif id == "mp_conflict" then
		return string.upper(Engine.Localize("@PRESENCE_MP_CONFLICT"))
	elseif id == "mp_mine" then
		return string.upper(Engine.Localize("@PRESENCE_MP_MINE"))
	elseif id == "mp_shipment_ns" then
		return string.upper(Engine.Localize("@PRESENCE_MP_SHIPMENT_NS"))
	elseif id == "mp_zerosub" then
		return string.upper(Engine.Localize("@PRESENCE_MP_ZEROSUB"))
	elseif id == "mp_ca_red_river" then
		return string.upper(Engine.Localize("@PRESENCE_MP_CA_RED_RIVER"))
	elseif id == "mp_ca_rumble" then
		return string.upper(Engine.Localize("@PRESENCE_MP_CA_RUMBLE"))
	end

	if id == "mp_alien_town" then
		return string.upper(Engine.Localize("@MPUI_ALIEN_TOWN"))
	elseif id == "mp_alien_armory" then
		return string.upper(Engine.Localize("@MPUI_ALIEN_ARMORY"))
	elseif id == "mp_alien_beacon" then
		return string.upper(Engine.Localize("@MPUI_ALIEN_BEACON"))
	elseif id == "mp_alien_dlc3" then
		return string.upper(Engine.Localize("@PRESENCE_MP_ALIEN_DLC3"))
	elseif id == "mp_alien_last" then
		return string.upper(Engine.Localize("@PRESENCE_MP_ALIEN_LAST"))
	end

	return Engine.Localize("@LUA_MENU_MAPNAME_" .. id:sub(4) .. "_CAPS")
end

function GetMaps()
	if Engine.IsCoreMode() then
		return {
			"any",
			"mp_prisonbreak",
			"mp_dart",
			"mp_lonestar",
			"mp_frag",
			"mp_snow",
			"mp_fahrenheit",
			"mp_hashima",
			"mp_warhawk",
			"mp_sovereign",
			"mp_zebra",
			"mp_skeleton",
			"mp_chasm",
			"mp_flooded",
			"mp_strikezone",
			"mp_descent_new",
			"mp_ca_red_river",
			"mp_ca_rumble",
			"mp_swamp",
			"mp_boneyard_ns",
			"mp_dome_ns",
			"mp_battery3",
			"mp_ca_impact",
			"mp_ca_behemoth",
			"mp_dig",
			"mp_zulu",
			"mp_pirate",
			"mp_favela_iw6",
			"mp_zerosub",
			"mp_conflict",
			"mp_mine",
			"mp_shipment_ns",
		}
	elseif Engine.IsAliensMode() then
		return {
			"any",
			"mp_alien_town",
			"mp_alien_armory",
			"mp_alien_beacon",
			"mp_alien_dlc3",
			"mp_alien_last"
		}
	end

	return { "any" }
end

function ServerFiltersPopupItems(f7_arg0)

	local rules = {}
	if Engine.IsCoreMode() then
		local gametype = {
			type = "UIGenericButton",
			id = "server_filters_gametype",
			properties = {
				style = GenericButtonSettings.Styles.GlassButton,
				substyle = GenericButtonSettings.Styles.GlassButton.SubStyles.SubMenu,
				variant = GenericButtonSettings.Variants.Select,
				content_width = 258,
				side = "left",
				button_text = Engine.Localize("@LUA_MENU_MODE_CAPS"),
				button_display_func = function(f8_arg0, f8_arg1)
					local focusedElement = LUI.FlowManager.GetMenuScopedDataFromElement(f8_arg0)
					if focusedElement.filterGametypeKey == nil then
						focusedElement.filterGametypeKey = Engine.GetDvarString("ui_mapvote_entrya_gametype")
					end
					Engine.SetDvarString("ui_mapvote_entrya_gametype", focusedElement.filterGametypeKey)
					return Engine.Localize(GetLocalizedStringFromGametype(focusedElement.filterGametypeKey))
				end,
				button_left_func = function(f9_arg0, f9_arg1)
					local focusedElement = LUI.FlowManager.GetMenuScopedDataFromElement(f9_arg0)
					local keys = GetGametypes()
					local defaultKeysI = 1
					for i = 1, #keys, 1 do
						if keys[i] == focusedElement.filterGametypeKey then
							defaultKeysI = i
							break
						end
					end
					defaultKeysI = defaultKeysI - 1
					if defaultKeysI < 1 then
						focusedElement.filterGametypeKey = keys[#keys]
					else
						focusedElement.filterGametypeKey = keys[defaultKeysI]
					end
				end,
				button_right_func = function(f10_arg0, f10_arg1)
					local focusedElement = LUI.FlowManager.GetMenuScopedDataFromElement(f10_arg0)
					local keys = GetGametypes()
					local defaultKeysI = 1
					for i = 1, #keys, 1 do
						if keys[i] == focusedElement.filterGametypeKey then
							defaultKeysI = i
							break
						end
					end
					defaultKeysI = defaultKeysI + 1
					if #keys < defaultKeysI then
						focusedElement.filterGametypeKey = keys[1]
					else
						focusedElement.filterGametypeKey = keys[defaultKeysI]
					end
				end
			}
		}
		table.insert(rules, gametype)
	end

	local maps = {
		type = "UIGenericButton",
		id = "server_filters_map",
		properties = {
			style = GenericButtonSettings.Styles.GlassButton,
			substyle = GenericButtonSettings.Styles.GlassButton.SubStyles.SubMenu,
			variant = GenericButtonSettings.Variants.Select,
			content_width = 258,
			side = "left",
			button_text = Engine.Localize("@LUA_MENU_MAPS_CAPS"),
			button_display_func = function(f8_arg0, f8_arg1)
				local focusedElement = LUI.FlowManager.GetMenuScopedDataFromElement(f8_arg0)
				if focusedElement.filterMapnameKey == nil then
					focusedElement.filterMapnameKey = Engine.GetDvarString("ui_mapvote_entrya_mapname")
				end
				Engine.SetDvarString("ui_mapvote_entrya_mapname", focusedElement.filterMapnameKey)
				return GetMapnameFromID(focusedElement.filterMapnameKey)
			end,
			button_left_func = function(f9_arg0, f9_arg1)
				local focusedElement = LUI.FlowManager.GetMenuScopedDataFromElement(f9_arg0)
				local keys = GetMaps()
				local defaultKeysI = 1
				for i = 1, #keys, 1 do
					if keys[i] == focusedElement.filterMapnameKey then
						defaultKeysI = i
						break
					end
				end
				defaultKeysI = defaultKeysI - 1
				if defaultKeysI < 1 then
					focusedElement.filterMapnameKey = keys[#keys]
				else
					focusedElement.filterMapnameKey = keys[defaultKeysI]
				end
			end,
			button_right_func = function(f10_arg0, f10_arg1)
				local focusedElement = LUI.FlowManager.GetMenuScopedDataFromElement(f10_arg0)
				local keys = GetMaps()
				local defaultKeysI = 1
				for i = 1, #keys, 1 do
					if keys[i] == focusedElement.filterMapnameKey then
						defaultKeysI = i
						break
					end
				end
				defaultKeysI = defaultKeysI + 1
				if #keys < defaultKeysI then
					focusedElement.filterMapnameKey = keys[1]
				else
					focusedElement.filterMapnameKey = keys[defaultKeysI]
				end
			end
		}
	}
	table.insert(rules, maps)

	return rules
end

function mp_server_filters_popup_page()
	return {
		type = "UIElement",
		id = "mp_server_filters_popup_page_id",
		properties = {},
		states = {
			default = {
				topAnchor = true,
				bottomAnchor = true,
				leftAnchor = true,
				rightAnchor = true,
				top = AAR.Layout.TitleBarHeight + 1,
				bottom = -1,
				left = 1,
				right = -1,
			}
		},
		children = {
			{
				type = "UIText",
				id = "leaderboard_filters_instruction",
				properties = {
					text = Engine.Localize("@LUA_MENU_SERVER_FILTER_POPUP_INSTR")
				},
				states = {
					default = {
						topAnchor = true,
						bottomAnchor = false,
						leftAnchor = true,
						rightAnchor = true,
						top = 10,
						bottom = 10 + CoD.TextSettings.NormalFont.Height,
						left = 75,
						right = -75,
						alignment = LUI.Alignment.Center,
						font = CoD.TextSettings.NormalFont.Font,
						red = Colors.white.r,
						green = Colors.white.g,
						blue = Colors.white.b
					}
				}
			},
			{
				type = "UIVerticalList",
				id = "server_filters_popup_vlist",
				states = {
					default = {
						topAnchor = false,
						bottomAnchor = true,
						leftAnchor = true,
						rightAnchor = true,
						top = -75,
						bottom = 0,
						left = 2,
						right = -2,
						spacing = 5
					}
				},
				childrenFeeder = ServerFiltersPopupItems
			}
		}
	}
end

LUI.MenuBuilder.registerDef("mp_server_filters_popup_page", mp_server_filters_popup_page)
