if (game:issingleplayer() or not Engine.InFrontend()) then
	return
end

local Lobby = luiglobals.Lobby
local SystemLinkJoinMenu = LUI.mp_menus.SystemLinkJoinMenu

local controller = nil
local server = nil

COLUMN_0 = 0
COLUMN_1 = 460
COLUMN_2 = 660
COLUMN_3 = 800
COLUMN_4 = 985
COLUMN_5 = 500

SystemLinkJoinMenu.UpdateGameList = function(f3_arg0, f3_arg1)
	local f3_local0 = f3_arg1.controller
	if not f3_local0 then
		f3_local0 = Engine.GetFirstActiveController()
	end
	local f3_local1 = LUI.FlowManager.GetMenuScopedDataFromElement(f3_arg0)
	Lobby.UpdateServerDisplayList(f3_local0)
	local f3_local2 = Lobby.GetServerCount(f3_local0)
	if f3_local2 ~= f3_local1.serverCount then
		f3_local1.serverCount = f3_local2
		f3_arg0:processEvent({
			name = "menu_refresh",
			immediate = true
		})
		f3_arg0:processEvent({
			name = "gain_focus",
			immediate = true
		})
	end
end

SystemLinkJoinMenu.RefreshServers = function(f4_arg0, f4_arg1)
	local f4_local0 = LUI.FlowManager.GetMenuScopedDataFromElement(f4_arg0)
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

function CreateColumnImage(id, shader, leftAnchor, rightAnchor, left, alpha)
	return {
		type = "UIImage",
		id = id,
		states = {
			default = {
				font = CoD.TextSettings.NormalFont.Font,
				alignment = LUI.Alignment.right,
				height = 20,
				width = 20,
				left = left,
				red = Colors.cac_label_text.r,
				green = Colors.cac_label_text.g,
				blue = Colors.cac_label_text.b,
				alpha = alpha,
				material = RegisterMaterial(shader)
			},
		},
		handlers = {
			button_over = MBh.AnimateToState("over"),
			button_up = MBh.AnimateToState("default"),
			button_disable = MBh.AnimateToState("disabled")
		}
	}
end

SystemLinkJoinMenu.CreateHeaderDef = function()
	local header = {
		type = "UIElement",
		id = "header_row_id",
		states = {
			default = CoD.ColorizeState(Swatches.Overlay.Color, {
				topAnchor = true,
				bottomAnchor = false,
				leftAnchor = true,
				rightAnchor = true,
				top = 2,
				left = 2,
				right = -2,
				height = GenericTitleBarDims.TitleBarHeight,
				material = RegisterMaterial("white"),
				alpha = Swatches.Overlay.AlphaMore
			})
		},
	}

	local columns = {}
	table.insert(columns, SystemLinkJoinMenu.CreateRowBackground(SystemLinkJoinMenu.Colors.generic_menu_bg_color))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnShade("column_shade_rect_id", false, COLUMN_1, COLUMN_2))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnShade("column_shade_rect_2_id", true, COLUMN_3, -1))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnText("host_header", Engine.Localize("@MENU_HOST_NAME"), true, false, COLUMN_0, COLUMN_1))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnText("map_header", Engine.Localize("@MENU_MAP"), true, false, COLUMN_1, COLUMN_2))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnText("players_header", Engine.Localize("@MENU_NUMPLAYERS"), true, false, COLUMN_2, COLUMN_3))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnText("type_header", Engine.Localize("@MENU_TYPE1"), true, true, COLUMN_3, COLUMN_4))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnText("ping_header", Engine.Localize("@MENU_PING"), true, true, COLUMN_4, COLUMN_5))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnImage("protected_header", "icon_lock", true, true, COLUMN_5, 1))
	header.children = columns
	return header
end

SystemLinkJoinMenu.CreateRowDef = function(f6_arg0, f6_arg1, f6_arg2, f6_arg3)
	local option = {
		type = "UIButton",
		id = "row_" .. f6_arg1,
		disabled = f6_arg2,
		focusable = not f6_arg2,
		properties = {
			button_height = GenericButtonDims.button_height
		},
		states = {
			default = {
				topAnchor = true,
				bottomAnchor = false,
				leftAnchor = true,
				rightAnchor = true,
				top = 0,
				bottom = MBh.Property("button_height"),
				left = 0,
				right = 0
			}
		},
		handlers = {
			button_action = MBh.EmitEventToRoot({
				name = "select_game",
				idx = f6_arg1
			})
		}
	}

	local columns = {}
	table.insert(columns, SystemLinkJoinMenu.CreateRowBackground(f6_arg3))
	table.insert(columns, SystemLinkJoinMenu.CreateColumnShade("column_shade_rect_id", false, COLUMN_1, COLUMN_2))
	local shade = SystemLinkJoinMenu.CreateColumnShade("column_shade_rect_2_id", true, COLUMN_3, -1)
	table.insert(columns, shade)
	local hostname = SystemLinkJoinMenu.CreateColumnText("host_text", Lobby.GetServerData(f6_arg0, f6_arg1, SystemLinkJoinMenu.GameDataColumn.Host), true, false, COLUMN_0, COLUMN_1)
	table.insert(columns, hostname)
	local mapname = SystemLinkJoinMenu.CreateColumnText("map_text", Lobby.GetServerData(f6_arg0, f6_arg1, SystemLinkJoinMenu.GameDataColumn.Map), true, false, COLUMN_1, COLUMN_2)
	table.insert(columns, mapname)
	local players = SystemLinkJoinMenu.CreateColumnText("players_text", Lobby.GetServerData(f6_arg0, f6_arg1, SystemLinkJoinMenu.GameDataColumn.Clients), true, false, COLUMN_2, COLUMN_3)
	table.insert(columns, players)
	local gametype = SystemLinkJoinMenu.CreateColumnText("type_text", Lobby.GetServerData(f6_arg0, f6_arg1, SystemLinkJoinMenu.GameDataColumn.Game), true, true, COLUMN_3, COLUMN_4)
	table.insert(columns, gametype)
	local ping = SystemLinkJoinMenu.CreateColumnText("ping_text", Lobby.GetServerData(f6_arg0, f6_arg1, SystemLinkJoinMenu.GameDataColumn.Ping), true, true, COLUMN_4, COLUMN_5)
	table.insert(columns, ping)
	local is_private = SystemLinkJoinMenu.CreateColumnImage("protected_header", "icon_lock", true, true, COLUMN_5, Lobby.GetServerData(f6_arg0, f6_arg1, 5) == "1" and 1 or 0)
	table.insert(columns, is_private)
	option.children = columns
	return option
end

SystemLinkJoinMenu.OnCreate = function(f1_arg0, f1_arg1)
	local f1_local0 = LUI.FlowManager.GetMenuScopedDataFromElement(f1_arg0)
	f1_local0.serverCount = 0
	f1_arg0:processEvent(LUI.ButtonHelperText.CommonEvents.addBackButton)
	f1_arg0:processEvent({
		name = "add_button_helper_text",
		button_ref = "button_action",
		helper_text = Engine.Localize("@MENU_JOIN_GAME1"),
		side = "left",
		clickable = true
	})
	f1_arg0:processEvent({
		name = "add_button_helper_text",
		button_ref = "button_alt1",
		helper_text = Engine.Localize("@MENU_SB_TOOLTIP_BTN_REFRESH"),
		side = "left",
		clickable = true
	})
	f1_arg0:processEvent({
		name = "add_button_helper_text",
		button_ref = "button_alt2",
		helper_text = Engine.Localize("@LUA_MENU_FILTER"),
		side = "right",
		clickable = true
	})
	local f1_local1 = Lobby.BuildServerList
	local f1_local2 = f1_arg1.controller
	if not f1_local2 then
		f1_local2 = Engine.GetFirstActiveController()
	end
	f1_local1(f1_local2)
end

function ServerListBackground()
	if Engine.IsAliensMode() then
		return {
			image = "frontend_aliens_art",
			fill_color = {
				r = 1,
				g = 1,
				b = 1
			},
			fill_alpha = 1
		}
	end

	if Engine.IsCoreMode() then
		return {
			image = "white",
			fill_color = {
				r = 0.07,
				g = 0.1,
				b = 0.11
			},
			fill_alpha = 1
		}
	end
end

function JoinGame(f2_arg0, f2_arg1)
	server = f2_arg1
	controller = server.controller
	if not f2_local1 then
		controller = Engine.GetFirstActiveController()
	end

	local is_private = Lobby.GetServerData(controller, server.idx, 5)
	if is_private == "1" then
		LUI.FlowManager.RequestPopupMenu(server, "server_password_field", false, controller, false)
	else
		Lobby.JoinServer(controller, server.idx)
	end
end

SystemLinkJoinMenu.menu_systemlink_join = function()
	if Engine.IsCoreMode() then
		Engine.SetDvarString("ui_customModeName", "mp")
	elseif Engine.IsAliensMode() then
		Engine.SetDvarString("ui_customModeName", "aliens")
	end

	if Engine.GetDvarString("ui_mapvote_entrya_gametype") == nil
		or Engine.GetDvarString("ui_mapvote_entrya_gametype") then
		Engine.SetDvarString("ui_mapvote_entrya_gametype", "any")
	end

	if Engine.GetDvarString("ui_mapvote_entrya_mapname") == nil
		or Engine.GetDvarString("ui_mapvote_entrya_mapname") == ""
		or (string.match(Engine.GetDvarString("ui_mapvote_entrya_mapname"), "alien") == nil and Engine.IsAliensMode())
		or (string.match(Engine.GetDvarString("ui_mapvote_entrya_mapname"), "alien") == "alien" and Engine.IsCoreMode()) then
		Engine.SetDvarString("ui_mapvote_entrya_mapname", "any")
	end

	return {
		type = "UIElement",
		id = "menu_systemlink_join_root",
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
			menu_create = SystemLinkJoinMenu.OnCreate,
			select_game = JoinGame
		},
		children = {
			{
				type = "UIImage",
				states = {
					default = CoD.ColorizeState(Swatches.Overlay.Color, {
						topAnchor = true,
						bottomAnchor = true,
						leftAnchor = true,
						rightAnchor = true,
						top = 0,
						bottom = 0,
						left = 0,
						right = 0,
						material = RegisterMaterial("white"),
						alpha = Swatches.Overlay.AlphaMore,
					})
				}
			},
			{
				type = "UIElement",
				states = {
					default = {
						topAnchor = false,
						bottomAnchor = false,
						leftAnchor = true,
						rightAnchor = true,
						left = 200,
						right = -200,
						height = 550
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
						id = "menu_systemlink_join_title_id",
						properties = {
							fill_alpha = 1,
							font = CoD.TextSettings.BoldFont,
							title_bar_text = Engine.Localize("@PLATFORM_SYSTEM_LINK_TITLE"),
							title_bar_text_indent = GenericTitleBarDims.TitleBarLCapWidth,
							title_bar_alignment = LUI.Alignment.Left
						}
					},
					{
						type = "generic_menu_background",
						id = "menu_systemlink_join_bg_id",
						properties = {
							fill_alpha = Swatches.Overlay.AlphaMore,
						},
						children = {
							SystemLinkJoinMenu.CreateHeaderDef(),
							{
								type = "UIVerticalList",
								id = "menu_systemlink_join_game_list_id",
								states = {
									default = {
										topAnchor = true,
										bottomAnchor = true,
										leftAnchor = true,
										rightAnchor = true,
										top = GenericTitleBarDims.TitleBarHeight + 4,
										bottom = 600,
										left = 2,
										right = -2
									}
								},
								childrenFeeder = SystemLinkJoinMenu.LinkGamesFeeder,
								handlers = {
									update_game_list = SystemLinkJoinMenu.UpdateGameList,
									menu_create = function(f12_arg0, f12_arg1)
										local f12_local0 = LUI.FlowManager.GetMenuScopedDataFromElement(f12_arg0)
										f12_local0.serverList = f12_arg0
										SystemLinkJoinMenu.RefreshServers(f12_arg0, f12_arg1)
									end

								}
							},
							{
								type = "button_helper_text_main",
								id = "online_vault_helper_text_id",
								properties = {
									left_inset = 10,
									right_inset = -10,
									top_margin = GenericFooterDims.TopMargin_WithoutBackground,
									height = 42,
									spacing = 12,
									background_alpha = 0
								}
							}
						}
					}
				}
			},
			{
				type = "UITimer",
				id = "menu_systemlink_join_update_timer",
				properties = {
					event = "update_game_list",
					interval = 250,
					disposable = false,
					broadcastToRoot = true
				}
			},
			{
				type = "UIBindButton",
				id = "menu_systemlink_join_bind_button_id",
				handlers = {
					button_secondary = MBh.LeaveMenu(),
					button_alt1 = SystemLinkJoinMenu.RefreshServers,
					button_alt2 = OpenFiltersMenu
				}
			},
		}
	}
end


LUI.MenuBuilder.m_definitions["menu_systemlink_join"] = function()
	local menu = SystemLinkJoinMenu.menu_systemlink_join()

	local rows = menu.children[2].children[3].children
	local header = rows[1]


	-- Increase server list width
	menu.children[2].states.default.left = 100
	menu.children[2].states.default.right = -100

	menu.children[3].properties.interval = 10 -- 250

	return menu
end

ServerPasswordListFeeder = function()
	return {
		{
			type = "UIVerticalList",
			id = "password_field_items",
			states = {
				default = {
					topAnchor = true,
					leftAnchor = true,
					bottomAnchor = false,
					rightAnchor = true,
				}
			},
			children = {
				{
					type = "UIGenericButton",
					id = "password_button_id",
					properties = {
						variant = GenericButtonSettings.Variants.Plain,
						button_text = Engine.Localize("PATCH_MENU_CHANGE_PASSWORD_CAPS"),
						button_display_func = function()
							local f31_local0 = Engine.GetDvarString("password")
							if f31_local0 then
								f31_local0 = Engine.GetDvarString("password") ~= ""
							end
							local f31_local1
							if f31_local0 then
								f31_local1 = Engine.Localize("PATCH_MENU_PASSWORD_SET")
								if not f31_local1 then

								else
									return f31_local1
								end
							end
							f31_local1 = Engine.Localize("MENU_NONE")
						end,
						button_action_func = function(f32_arg0, f32_arg1)
							Engine.ExecNow("setfromdvar ui_password password")
							Engine.OpenScreenKeyboard(f32_arg1.controller, Engine.Localize("MENU_PASSWORD"), Engine.GetDvarString("ui_password") or "", Lobby.PasswordLength, false, false, CoD.KeyboardInputTypes.Password)
						end
					},
					handlers = {
						text_input_complete = function(f33_arg0, f33_arg1)
							if f33_arg1.text then
								Engine.SetDvarString("password", f33_arg1.text)
								f33_arg0:processEvent({
									name = "content_refresh"
								})
							end
						end
					}
				},
				{
					type = "UIGenericButton",
					id = "connect_button_id",
					properties = {
						variant = GenericButtonSettings.Variants.Plain,
						button_text = Engine.Localize("MENU_JOIN_GAME"), button_action_func = function()
							Lobby.JoinServer(controller, server.idx)
						end
					}

				}
			}
		}
	}
end

password_field = function(f50_arg0, f50_arg1)
	return {
		type = "UIElement",
		id = "server_popup_container_id",
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
						height = 145
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
							title_bar_text = Engine.Localize("@LUA_MENU_LOBBY_JOINING"),
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
						type = "UIElement",
						id = "mp_server_password_popup_page_id",
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
								type = "generic_border",
								properties = {
									thickness = 2,
									border_red = Colors.generic_menu_frame_color.r,
									border_green = Colors.generic_menu_frame_color.g,
									border_blue = Colors.generic_menu_frame_color.b
								},
								states = {
									default = {
										topAnchor = true,
										bottomAnchor = true,
										leftAnchor = true,
										rightAnchor = true,
										top = 0,
										bottom = 0,
										left = 0,
										right = 0,
										alpha = 0.6
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
										top = -90,
										bottom = 0,
										left = 0,
										right = 0,
										spacing = 5
									}
								},
								childrenFeeder = ServerPasswordListFeeder
							}
						}
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

LUI.MenuBuilder.registerDef("server_password_field", password_field)
