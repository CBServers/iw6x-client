#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(unsigned int id)> AddRefToObject{0, 0x1404326D0};
	WEAK symbol<void(int type, VariableUnion u)> AddRefToValue{0x1403D7740, 0x1404326E0};
	WEAK symbol<unsigned int(unsigned int id)> AllocThread{0, 0x1404329B0};
	WEAK symbol<ObjectVariableValue*(unsigned int* index)> AllocVariable{0x1403D7A70, 0x140432A10};
	WEAK symbol<void(int type, VariableUnion u)> RemoveRefToValue{0x1403D90F0, 0x1404340C0};
	WEAK symbol<void(unsigned int id)> RemoveRefToObject{0, 0x140433FB0};
	WEAK symbol<void(unsigned int parentId, unsigned int index)> RemoveVariableValue{0x1403D91C0, 0x140434190};

	WEAK symbol<void(void*, void*)> AimAssist_AddToTargetList{0, 0x140139D80};

	WEAK symbol<void(unsigned int weapon, bool isAlternate, char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{0, 0x140239370};

	WEAK symbol<void()> Com_Frame_Try_Block_Function{0x1403BC980, 0x1404131A0};
	WEAK symbol<const char*(char const**)> Com_Parse{0x1404313E0, 0x1404F50E0};
	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x1403BBFF0, 0x140412740};
	WEAK symbol<void()> Com_Quit{0x1403BDDD0, 0x140414920};
	WEAK symbol<CodPlayMode()> Com_GetCurrentCoDPlayMode{0, 0x1404f6140};
	WEAK symbol<void(float, float, int)> Com_SetSlowMotion{0, 0x1404158C0};
	WEAK symbol<void(const char* text_in)> Com_TokenizeString{0x1403B4150, 0x1403F7CC0};
	WEAK symbol<void()> Com_EndTokenizeString{0x1403B37C0, 0x1403F7330};

	WEAK symbol<void(const char* message)> Conbuf_AppendText{0x14043DDE0, 0x1405028C0};

	WEAK symbol<char*(int start)> ConcatArgs{0x14030AF10, 0x140392880};

	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{0x1403B3050, 0x1403F6B50};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer, void (int, int, const char*))> Cbuf_ExecuteBufferInternal{0x1403B3160, 0x1403F6C60};

	WEAK symbol<bool()> CL_IsCgameInitialized{0x140234DA0, 0x1402B9A70};

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{0x1401F2E20, 0x140271320};
	WEAK symbol<void(int localClientNum, mp::cg_s* cg, const char* dvar, const char* value)> CG_SetClientDvarFromServer{0x0, 0x14028A2C0};

	WEAK symbol<void(const char* cmdName, void (), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x1403B3570, 0x1403F7070};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x1403B3B10, 0x1403F7680};

	WEAK symbol<void (XAssetType type, void (__cdecl *func)(XAssetHeader, void*), void* inData, bool includeOverride)> DB_EnumXAssets_FastFile{0x140271F50, 0x14031EF90};
	WEAK symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{0x140271FC0, 0x14031F000};
	WEAK symbol<XAssetEntry*(XAssetType type, const char* name)> DB_FindXAssetEntry{0x140272230, 0x14031F2D0};
	WEAK symbol<const char* (const XAsset* asset)> DB_GetXAssetName{0x14024FB10, 0x1402FB160};
	WEAK symbol<int(XAssetType type)> DB_GetXAssetTypeSize{0x14024FB30, 0x1402FB180};
	WEAK symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, DBSyncMode syncMode)> DB_LoadXAssets{0x140273FD0, 0x140320F20};
	WEAK symbol<void(void* buffer, int size)> DB_ReadXFileUncompressed{0x140250FB0, 0x1402FC9C0};
	WEAK symbol<XAssetHeader(XAssetType type, const char* name, int allowCreateDefault)> DB_FindXAssetHeader{0x140272300, 0x14031F3A0};
	WEAK symbol<int(XAssetType type, const char* name)> DB_XAssetExists{0x140276200, 0x1403245E0};
	WEAK symbol<int(XAssetType type, const char* name)> DB_IsXAssetDefault{0x140273480 , 0x1403204D0};
	WEAK symbol<int(const RawFile* rawfile)> DB_GetRawFileLen{0x0140272E80, 0x14031FF80};
	WEAK symbol<void(const RawFile* rawfile, char* buf, int size)> DB_GetRawBuffer{0x140272D50, 0x14031FE50};

	WEAK symbol<void*(unsigned int size, unsigned int alignment, unsigned int type, PMem_Source source)> PMem_AllocFromSource_NoDebug{0x0140430B80, 0x001404F46C0};
	WEAK symbol<void*(unsigned int size)> Hunk_AllocateTempMemoryHighInternal{0x140423C70, 0x1404E4E20};

	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x140429E70, 0x1404ECB60};
	WEAK symbol<void (char* buffer, int index)> Dvar_GetCombinedString{0x1403BFD80, 0x140416B30};
	WEAK symbol<bool(const char* dvarName)> Dvar_GetBool{0x140429FC0, 0x1404ECCB0};
	WEAK symbol<int(const char* dvarName)> Dvar_GetInt{0x14042A0A0, 0x1404ECD90};
	WEAK symbol<const char*(const char* dvarName, const char* defaultValue)> Dvar_GetVariantStringWithDefault{0x14042A240, 0x1404ECF90};
	WEAK symbol<dvar_t*(const char* dvarName, bool value, unsigned int flags, const char* description)> Dvar_RegisterBool{0x14042AF10, 0x1404EDD60};
	WEAK symbol<dvar_t*(const char* dvarName, const char** valueList, int defaultIndex, unsigned int flags,
	                    const char* description)> Dvar_RegisterEnum{0x14042B220, 0x1404EE070};
	WEAK symbol<dvar_t*(const char* dvarName, float value, float min, float max, unsigned int flags,
	                    const char* description)> Dvar_RegisterFloat{0x14042B330, 0x1404EE180};
	WEAK symbol<dvar_t*(const char* dvarName, int value, int min, int max, unsigned int flags, const char* desc)> Dvar_RegisterInt{0x14042B420, 0x1404EE270};
	WEAK symbol<dvar_t*(const char* dvarName, const char* value, unsigned int flags, const char* description)> Dvar_RegisterString{0x14042B7A0, 0x1404EE660};
	WEAK symbol<dvar_t*(const char* dvarName, float x, float y, float min, float max, unsigned int flags,
	                    const char* description)> Dvar_RegisterVec2{0x14042B880, 0x1404EE740};
	WEAK symbol<dvar_t*(const char* dvarName, float x, float y, float z, float w, float min, float max,
	                    unsigned int flags, const char* description)> Dvar_RegisterVec4{0x14042BC10, 0x1404EEA50};
	WEAK symbol<void(dvar_t* dvar, DvarSetSource source)> Dvar_Reset{0x14042C150, 0x1404EF020};
	WEAK symbol<void(const dvar_t* dvar, bool value)> Dvar_SetBool{0x14042C370, 0x1404EF1A0};
	WEAK symbol<void(const char* dvar, const char* buffer)> Dvar_SetCommand{0x14042C8E0, 0x1404EF790};
	WEAK symbol<void(const dvar_t* dvar, const char* string)> Dvar_SetString{0x14042D6E0, 0x1404F08E0};
	WEAK symbol<void(const char*, const char*, DvarSetSource)> Dvar_SetFromStringByNameFromSource{0x14042D000, 0x1404F00B0};
	WEAK symbol<void()> Dvar_Sort{0x14042DEF0, 0x1404F1210};
	WEAK symbol<const char*(dvar_t* dvar, dvar_value value)> Dvar_ValueToString{0x14042E710, 0x1404F1A30};

	WEAK symbol<long long (const char* qpath, char** buffer)> FS_ReadFile{0x14041D0B0, 0x1404DE900};
	WEAK symbol<void(void* buffer)> FS_FreeFile{0x14041D0A0, 0x1404DE8F0};
	WEAK symbol<void(const char *gameName)> FS_Startup{0x14041C660, 0x1404DDEB0};
	WEAK symbol<void(const char *path, const char *dir, int bLanguageDirectory, int iLanguage)> FS_AddGameDirectory{0x14041A120, 0x1404DC570};
	WEAK symbol<void(const char *path, const char *dir)> FS_AddLocalizedGameDirectory{0x14041A2F0, 0x1404DC760};

	WEAK symbol<Weapon(const char* pickupName, int model)> G_FindItem{0x140462490, 0x14021B7E0};
	WEAK symbol<int(playerState_s* ps, Weapon weapon, int dualWield, int startInAltMode, int usedBefore)> G_GivePlayerWeapon{0x140359E20, 0x1403DA5E0};
	WEAK symbol<Weapon(const char* name)> G_GetWeaponForName{0x140359890, 0x1403DA060};
	WEAK symbol<void()> G_Glass_Update{0x14030E680, 0x140397450};
	WEAK symbol<void (playerState_s* ps, Weapon weapon, int hadWeapon)> G_InitializeAmmo{0x140311F00, 0x14039AEA0};
	WEAK symbol<void(int clientNum, Weapon weapon)> G_SelectWeapon{0x14035A200, 0x1403DA880};
	WEAK symbol<int(playerState_s* ps, Weapon weapon)> G_TakePlayerWeapon{0x14035A350, 0x1403DA9C0};
	WEAK symbol<unsigned int (const char* name, /*ConfigString*/ unsigned int start, unsigned int max, int create,
	                          const char* errormsg)> G_FindConfigstringIndex{0x0, 0x140161F90};
	WEAK symbol<int(int server_time)> G_RunFrame{0x0, 0x1403A05E0};

	WEAK symbol<game_hudelem_s*(int clientNum, int teamNum)> HudElem_Alloc{0x0, 0x1403997E0};

	WEAK symbol<char*(char* string)> I_CleanStr{0x140432460, 0x1404F63C0};

	WEAK symbol<char*(GfxImage* image, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipCount, 
		uint32_t imageFlags, DXGI_FORMAT imageFormat, const char* name, const void* initData)>
	Image_Setup{0x140517910, 0x1405E4380};

	WEAK symbol<const char*(int, int, int)> Key_KeynumToString{0x14023D9A0, 0x1402C40E0};

	WEAK symbol<unsigned int (int)> Live_SyncOnlineDataFlags{0, 0x1405ABF70};

	WEAK symbol<bool(int controllerIndex, unsigned int name, int value, StatsGroup statsGroup)> LiveStorage_PlayerDataSetIntByName{0x1403B8C20, 0x140404730};
	WEAK symbol<bool(std::uint8_t* persistentData, const char* lookupString, int value, std::uint8_t* modifiedFlags, StatsGroup statsGroup)> LiveStorage_PlayerDataSetReservedInt{0x1403B8D00, 0x140404820};
	WEAK symbol<std::int64_t(std::uint8_t* persistentData, const char* lookupString, const StatsGroup statsGroup)> LiveStorage_PlayerDataGetReservedInt{0x1403B84F0, 0x140403CF0};
	WEAK symbol<bool(int controllerIndex, const unsigned int* navStrings, int navStringCount, int value, StatsGroup statsGroup)> LiveStorage_PlayerDataSetIntByNameArray{0, 0x140404640};
	WEAK symbol<int(int controllerIndex, const unsigned int* navStrings, int navStringCount, StatsGroup statsGroup)> LiveStorage_PlayerDataGetIntByNameArray{0, 0x140403B80};
	WEAK symbol<int(int controllerIndex, unsigned int name, StatsGroup statsGroup)> LiveStorage_PlayerDataGetIntByName{0x1403B8460, 0x140403C40};
	WEAK symbol<std::uint8_t*(int controllerIndex)>LiveStorage_GetPersistentDataBuffer{0x1403B6F80, 0x140400170};
	WEAK symbol<void(int controllerIndex)> LiveStorage_StatsWriteNeeded{0x1403BA400, 0x1404090E0};

	WEAK symbol<BOOL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> MainWndProc{0x14043E6A0, 0x140504A00};
	WEAK symbol<void()> IN_MouseMove{0x140438840, 0x1404FC7B0};
	WEAK symbol<void()> IN_RecenterMouse{0x1404389E0, 0x1404FC950};
	WEAK symbol<int(int x, int y, int dx, int dy)> CL_MouseEvent{0x14023B8E0, 0x1402C12A0};

	WEAK symbol<StructuredDataDef*(const char* filename, unsigned int maxSize)>StructuredDataDef_GetAsset{0, 0x1404E6560};
	WEAK symbol<StringTable*(const char* fileName, const StringTable** tablePtr)>StringTable_GetAsset{0, 0x1404E6170};
	WEAK symbol<const char*(const StringTable* table, const int row, const int column)> StringTable_GetColumnValueForRow{0, 0x1404E61A0};
	WEAK symbol<int(const StringTable* table, const int comparisonColumn, const char* value)> StringTable_LookupRowNumForValue{0, 0x1404E6260};

	WEAK symbol<void(int localClientNum, const char* menuName, int isPopup, int isModal, unsigned int isExclusive)> LUI_OpenMenu{0x1403FD460, 0x1404B3610};
	// Made up name, replaced by ScopedCriticalSection on Black Ops 3
	WEAK symbol<void()> LUI_EnterCriticalSection{0x1401AE940, 0x1401CD040};
	WEAK symbol<void()> LUI_LeaveCriticalSection{0x1401B0AA0, 0x1401CF1A0};

	WEAK symbol<bool(int localClientNum, const char* menuName)> Menu_IsMenuOpenAndVisible{0x0, 0x1404B38A0};

	WEAK symbol<Material*(const char* material)> Material_RegisterHandle{0x140523D90, 0x1405F0E20};

	WEAK symbol<void(netsrc_t, netadr_s*, const char*)> NET_OutOfBandPrint{0, 0x14041D5C0};
	WEAK symbol<void(netsrc_t sock, int length, const void* data, const netadr_s* to)> NET_SendLoopPacket{0, 0x14041D780};
	WEAK symbol<bool(const char* s, netadr_s* a)> NET_StringToAdr{0, 0x14041D870};
	WEAK symbol<void(netadr_s*, sockaddr*)> NetadrToSockadr{0, 0x1404E53D0};

	WEAK symbol<void(float x, float y, float width, float height, float s0, float t0, float s1, float t1,
	                  float* color, Material* material)> R_AddCmdDrawStretchPic{0x140234460, 0x140600BE0};
	WEAK symbol<void(const char*, int, Font_s*, float, float, float, float, float, float*, int)> R_AddCmdDrawText{0x140533E40, 0x140601070};
	WEAK symbol<void(const char*, int, Font_s*, float, float, float, float, float, const float*, int, int, char)> R_AddCmdDrawTextWithCursor{0x140534170, 0x1406013A0};
	WEAK symbol<Font_s*(const char* font)> R_RegisterFont{0x1405130B0, 0x1405DFAC0};
	WEAK symbol<void()> R_SyncRenderThread{0x140535AF0, 0x140602D30};
	WEAK symbol<int(const char* text, int maxChars, Font_s* font)> R_TextWidth{0x140513390, 0x1405DFDB0};

	WEAK symbol<ScreenPlacement*()> ScrPlace_GetViewPlacement{0x14024D150, 0x1402F6D40};

	WEAK symbol<void()> GScr_LoadConsts{0x140367AA0, 0x1403E0420};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int name)> FindVariable{0x1403D84F0, 0x1404334A0};
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> FindEntityId{0, 0x1404333A0};
	WEAK symbol<unsigned int(unsigned int parentId, unsigned int id)> GetVariableName{0x1403D8E90, 0x140433E60};
	WEAK symbol<void (VariableValue* result, unsigned int classnum, int entnum, int offset)> GetEntityFieldValue{0x1403DC810, 0x140437860};
	WEAK symbol<const float*(const float* v)> Scr_AllocVector{0x1403D9AF0, 0x140434A10};
	WEAK symbol<const char*(unsigned int index)> Scr_GetString{0, 0x140439160};
	WEAK symbol<void(int value)> Scr_AddInt{0x0, 0x140437E70};
	WEAK symbol<int(unsigned int index)> Scr_GetInt{0x0, 0x140438E10};
	WEAK symbol<float(unsigned int index)> Scr_GetFloat{0, 0x140438D60};
	WEAK symbol<unsigned int()> Scr_GetNumParam{0x1403DDF60, 0x140438EC0};
	WEAK symbol<void()> Scr_ClearOutParams{0x1403DD500, 0x140438600};
	WEAK symbol<scr_entref_t(unsigned int entId)> Scr_GetEntityIdRef{0x1403DBDC0, 0x140436E10};
	WEAK symbol<int(unsigned int classnum, int entnum, int offset)> Scr_SetObjectField{0x140350E70, 0x1403D3FE0};
	WEAK symbol<void(unsigned int id, unsigned int stringValue, unsigned int paramcount)> Scr_NotifyId{0x1403DE730, 0x140439700};
	WEAK symbol<unsigned int(int entnum, unsigned int classnum)> Scr_GetEntityId{0x0, 0x140436D60};
	WEAK symbol<bool(VariableValue* value)> Scr_CastString{0x0, 0x140434AC0};

	WEAK symbol<unsigned __int16(int handle, unsigned int paramcount)> Scr_ExecThread{0x1403DD600, 0x1404386E0};
	WEAK symbol<unsigned int(const char* name)> Scr_LoadScript{0x1403D3C50, 0x14042EAA0};
	WEAK symbol<unsigned int(const char* script, unsigned int name)> Scr_GetFunctionHandle{0x1403D3AD0 , 0x14042E920};
	WEAK symbol<unsigned int(void* func, int type, unsigned int name)> Scr_RegisterFunction{0x1403D3530, 0x14042E330};
	WEAK symbol<void()> Scr_ErrorInternal{0x0, 0x140438660};

	WEAK symbol<int(unsigned int)> GetObjectType{0x0, 0x140433CF0};

	WEAK symbol<unsigned int(unsigned int localId, const char* pos, unsigned int paramcount)> VM_Execute{0, 0x14043A280};

	WEAK symbol<const char*(const char*)> SEH_StringEd_GetString{0, 0x1404A5F60};

	WEAK symbol<const char*(unsigned int stringValue)> SL_ConvertToString{0x1403D6870, 0x1404317F0};
	WEAK symbol<unsigned int(const char* str, unsigned int user)> SL_GetString{0x1403D6CD0, 0x140431C70};
	WEAK symbol<unsigned int(int)> SL_GetStringForInt{0x1403D6D50, 0x140431CF0};
	WEAK symbol<unsigned int(const char* str)> SL_FindString{0x1403D6AF0, 0x140431A90};
	WEAK symbol<unsigned int(char const* str)> SL_GetCanonicalString{0x1403D36F0, 0x14042E4F0};

	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{0x1403B4560, 0x1403F80D0};
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{0, 0x1403F8150};
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{0, 0x1403F8110};
	WEAK symbol<void()> SV_MatchEnd{0x0, 0x14047A090};

	WEAK symbol<void(netadr_s* from)> SV_DirectConnect{0, 0x140471390};
	WEAK symbol<void(int, int, const char*)> SV_GameSendServerCommand{0x140490F40, 0x1404758C0};
	WEAK symbol<bool()> SV_Loaded{0x140491820, 0x1404770C0};
	WEAK symbol<void(int localClientNum, const char* map, bool mapIsPreloaded)> SV_StartMap{0, 0x140470170};
	WEAK symbol<void(int localClientNum, const char* map, bool mapIsPreloaded, bool migrate)> SV_StartMapForParty{0, 0x1404702F0};

	WEAK symbol<mp::gentity_s*(const char*, unsigned int, unsigned int, unsigned int)> SV_AddBot{0, 0x140470920};
	WEAK symbol<bool(int clientNum)> SV_BotIsBot{0, 0x140461340};
	WEAK symbol<const char*()> SV_BotGetRandomName{0, 0x140460B80};
	WEAK symbol<void(mp::gentity_s*)> SV_SpawnTestClient{0, 0x1404740A0};

	WEAK symbol<void(mp::client_t*, const char*, int)> SV_ExecuteClientCommand{0, 0x140472430};
	WEAK symbol<void()> SV_FastRestart{0x14048B890, 0x14046F440};
	WEAK symbol<playerState_s*(int num)> SV_GetPlayerstateForClientNum{0x140490F80, 0x140475A10};
	WEAK symbol<const char*(int clientNum)> SV_GetGuid{0, 0x140475990};
	WEAK symbol<void(int clientNum, const char* reason)> SV_KickClientNum{0, 0x14046F730};
	WEAK symbol<void(int index, const char* string)> SV_SetConfigstring{0, 0x140477450};

	WEAK symbol<void(const char* error, ...)> Sys_Error{0x14043AC20, 0x1404FF510};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x1403C2D40, 0x140423920};
	WEAK symbol<int()> Sys_Milliseconds{0x14043D2A0, 0x140501CA0};
	WEAK symbol<void()> Sys_ShowConsole{0x14043E650, 0x140503130};
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{0x14043D000, 0x140501A00};
	WEAK symbol<void*(int valueIndex)> Sys_GetValue{0x1403C2C30, 0x1404237D0};
	WEAK symbol<bool()> Sys_IsMainThread{0x1478FC470, 0x140423950};

	WEAK symbol<void()> SwitchToCoreMode{0, 0x1401FA4A0};
	WEAK symbol<void()> SwitchToAliensMode{0, 0x1401FA4D0};
	WEAK symbol<void()> SwitchToSquadVsSquadMode{0, 0x1401FA500};

	WEAK symbol<const char*(const char*)> UI_LocalizeMapname{0, 0x1404B96D0};
	WEAK symbol<const char*(const char*)> UI_LocalizeGametype{0, 0x1404B90F0};

	WEAK symbol<DWOnlineStatus(int)> dwGetLogOnStatus{0, 0x140589490};

	WEAK symbol<void(pmove_t* move, trace_t*, const float*, const float*,
		const Bounds*, int, int)> PM_playerTrace{0x14046C910, 0x140225C20};
	WEAK symbol<void(const pmove_t* move, trace_t* trace, const float*,
		const float*, const Bounds*, int, int)> PM_trace{0, 0x140225DB0};

	WEAK symbol<void(void* ps)> Jump_ClearState{0x0, 0x140213120};

	WEAK symbol<void*(jmp_buf* Buf, int Value)> longjmp{0x14062E030, 0x140738060};
	WEAK symbol<int (jmp_buf* Buf)> _setjmp{0x14062F030, 0x140739060};

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<int> keyCatchers{0x1417CF6E0, 0x1419E1ADC};

	WEAK symbol<CmdArgs> cmd_args{0x144CE7F70, 0x144518480};
	WEAK symbol<CmdArgs> sv_cmd_args{0x144CE8020, 0x144518530};
	WEAK symbol<cmd_function_s*> cmd_functions{0x144CE80C8, 0x1445185D8};

	WEAK symbol<int> dvarCount{0x1458CBA3C, 0x1478EADF4};
	WEAK symbol<dvar_t*> sortedDvars{0x1458CBA60, 0x1478EAE10};

	WEAK symbol<PlayerKeyState> playerKeys{0x14164138C, 0x1419DEABC};

	WEAK symbol<SOCKET> query_socket{0, 0x147AD1A78};

	WEAK symbol<const char*> command_whitelist{0x14086AA70, 0x1409E3AB0};

	WEAK symbol<unsigned int> levelEntityId{0x1452A9F30, 0x144A43020};
	WEAK symbol<int> g_script_error_level{0x1455B1F98, 0x144D535C4};
	WEAK symbol<jmp_buf> g_script_error{0x1455BA5E0, 0x144D536E0};
	WEAK symbol<scr_classStruct_t> g_classMap{0x140873E20, 0x1409EBFC0};

	WEAK symbol<void*> DB_XAssetPool{0x14086DCB0, 0x1409E4F20};
	// db_hashTable
	WEAK symbol<int> g_poolSize{0x14086DBB0, 0x1409E4E20};
	// g_assetEntryPool
	WEAK symbol<const char*> g_assetNames{0x14086CA40, 0x1409E40C0};

	WEAK symbol<WinVars_t> g_wv{0x145A7BAD0, 0x147AD2630};
	WEAK symbol<WinMouseVars_t> s_wmv{0x145A73750, 0x147AC9D78};

	WEAK symbol<int> window_center_x{0x145A73760, 0x147AC9D74};
	WEAK symbol<int> window_center_y{0x145A73764, 0x147AC9D88};
	
	WEAK symbol<vidConfig_t> vidConfigOut{0x141644464, 0x141D1ACE0};

	WEAK symbol<scrVarGlob_t> scr_VarGlob{0x1452CDF80, 0x144A67080};
	WEAK symbol<scrVmPub_t> scr_VmPub{0x1455B1FA0, 0x144D4B090};
	WEAK symbol<function_stack_t> scr_function_stack{0x1455BE708, 0x144D57808};
	WEAK symbol<unsigned int> scr_levelEntityId{0x1452A9F30, 0x144A43020};

	WEAK symbol<int> level_time{0x0, 0x1443F4B6C};
	WEAK symbol<int> level_finished{0x0, 0x1443F6FAC};
	WEAK symbol<int> level_savepersist{0x0, 0x1443F5710};

	WEAK symbol<DWORD> threadIds{0x144DE6640, 0x1446B4960};

	WEAK symbol<GfxDrawMethod_s> gfxDrawMethod{0x145F525A8, 0x1480350D8};

	namespace sp
	{
		WEAK symbol<gentity_s> g_entities{0x143C91600, 0};

		WEAK symbol<XZone> g_zones_0{0x1434892D8, 0};
	}

	namespace mp
	{
		WEAK symbol<cg_s> cgArray{0, 0x14176EC00};

		WEAK symbol<gentity_s> g_entities{0, 0x14427A0E0};

		WEAK symbol<int> svs_clientCount{0, 0x14647B28C};
		WEAK symbol<client_t> svs_clients{0, 0x14647B290};

		WEAK symbol<std::uint32_t> sv_serverId_value{0, 0x144DF9478};

		WEAK symbol<int> gameTime{0, 0x1443F4B6C};
		WEAK symbol<int> serverTime{0, 0x14647B280};

		WEAK symbol<XZone> g_zones_0{0, 0x143A46498};
	}

	namespace hks
	{
		WEAK symbol<lua_State*> lua_state{0, 0x141640DA0};
		WEAK symbol<void(lua_State* s, const char* str, unsigned int l)> hksi_lua_pushlstring{0, 0x14019DEC0};
		WEAK symbol<HksObject*(HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_getfield{0, 0x1401994B0};
		WEAK symbol<void(lua_State* s, const HksObject* tbl, const HksObject* key, const HksObject* val)> hks_obj_settable{0, 0x14019A570};
		WEAK symbol<HksObject* (HksObject* result, lua_State* s, const HksObject* table, const HksObject* key)> hks_obj_gettable{0, 0x1401998F0};
		WEAK symbol<void(lua_State* s, int nargs, int nresults, const unsigned int* pc)> vm_call_internal{0, 0x1401C6420};
		WEAK symbol<HashTable*(lua_State* s, unsigned int arraySize, unsigned int hashSize)> Hashtable_Create{0, 0x140186BD0};
		WEAK symbol<cclosure*(lua_State* s, lua_function function, int num_upvalues, int internal_, int profilerTreatClosureAsFunc)> cclosure_Create{0, 0x140186DF0};
		WEAK symbol<int(lua_State* s, int t)> hksi_luaL_ref{0, 0x14019C5C0};
		WEAK symbol<void(lua_State* s, int t, int ref)> hksi_luaL_unref{0, 0x14019C750};
		WEAK symbol<int(lua_State* s, int what, int data)> hks_lua_gc{0, 0x1401A4790};
		WEAK symbol<int(lua_State* s, const char* filename)> hksi_hks_memorystats{0, 0x14019B580};

		WEAK symbol<int(lua_State* s, const HksCompilerSettings* options, const char* buff, unsigned __int64 sz, const char* name)> hksi_hksL_loadbuffer{0, 0x14019AD90};
		WEAK symbol<int(lua_State* s, const char* what, lua_Debug* ar)> hksi_lua_getinfo{0, 0x14019CCF0};
		WEAK symbol<int(lua_State* s, int level, lua_Debug* ar)> hksi_lua_getstack{0, 0x14019CFB0};
		WEAK symbol<void(lua_State* s, const char* fmt, ...)> hksi_luaL_error{0, 0x14019C4C0};
		WEAK symbol<const char*> s_compilerTypeName{0, 0x1409DE040};
	}
}
