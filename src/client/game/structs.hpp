#pragma once
#include <d3d11.h>

#define PROTOCOL 1

namespace game
{
	typedef float vec_t;
	typedef vec_t vec2_t[2];
	typedef vec_t vec3_t[3];
	typedef vec_t vec4_t[4];

	enum
	{
		FL_GODMODE = 0x1,
		FL_DEMI_GODMODE = 0x2,
		FL_NOTARGET = 0x4,
		FL_NO_KNOCKBACK = 0x8,
		FL_NO_RADIUS_DAMAGE = 0x10,
		FL_SUPPORTS_LINKTO = 0x20,
		FL_NO_AUTO_ANIM_UPDATE = 0x40,
		FL_GRENADE_TOUCH_DAMAGE = 0x80,
		FL_STABLE_MISSILES = 0x100,
		FL_REPEAT_ANIM_UPDATE = 0x200,
		FL_VEHICLE_TARGET = 0x400,
		FL_GROUND_ENT = 0x800,
		FL_CURSOR_HINT = 0x1000,
		FL_MISSILE_ATTRACTOR_OR_REPULSOR = 0x2000,
		FL_WEAPON_BEING_GRABBED = 0x4000,
		FL_DELETE = 0x8000,
		FL_BOUNCE = 0x10000,
		FL_MOVER_SLIDE = 0x20000,
		FL_MOVING = 0x40000,
		FL_DONT_AUTOBOLT_MISSILE_EFFECTS = 0x80000,
		FL_DISABLE_MISSILE_STICK = 0x100000,
		FL_NO_MELEE_TARGET = 0x2000000,
		FL_DYNAMICPATH = 0x8000000,
		FL_AUTO_BLOCKPATHS = 0x10000000,
		FL_OBSTACLE = 0x20000000,
		FL_BADPLACE_VOLUME = 0x80000000,
	};

	enum XAssetType
	{
		ASSET_TYPE_PHYSPRESET = 0x0,
		ASSET_TYPE_PHYSCOLLMAP = 0x1,
		ASSET_TYPE_XANIMPARTS = 0x2,
		ASSET_TYPE_XMODEL_SURFS = 0x3,
		ASSET_TYPE_XMODEL = 0x4,
		ASSET_TYPE_MATERIAL = 0x5,
		ASSET_TYPE_COMPUTESHADER = 0x6,
		ASSET_TYPE_VERTEXSHADER = 0x7,
		ASSET_TYPE_HULLSHADER = 0x8,
		ASSET_TYPE_DOMAINSHADER = 0x9,
		ASSET_TYPE_PIXELSHADER = 0xA,
		ASSET_TYPE_VERTEXDECL = 0xB,
		ASSET_TYPE_TECHNIQUE_SET = 0xC,
		ASSET_TYPE_IMAGE = 0xD,
		ASSET_TYPE_SOUND = 0xE,
		ASSET_TYPE_SOUND_CURVE = 0xF,
		ASSET_TYPE_LPF_CURVE = 0x10,
		ASSET_TYPE_REVERB_CURVE = 0x11,
		ASSET_TYPE_LOADED_SOUND = 0x12,
		ASSET_TYPE_CLIPMAP = 0x13,
		ASSET_TYPE_COMWORLD = 0x14,
		ASSET_TYPE_GLASSWORLD = 0x15,
		ASSET_TYPE_PATHDATA = 0x16,
		ASSET_TYPE_VEHICLE_TRACK = 0x17,
		ASSET_TYPE_MAP_ENTS = 0x18,
		ASSET_TYPE_FXWORLD = 0x19,
		ASSET_TYPE_GFXWORLD = 0x1A,
		ASSET_TYPE_LIGHT_DEF = 0x1B,
		ASSET_TYPE_UI_MAP = 0x1C,
		ASSET_TYPE_FONT = 0x1D,
		ASSET_TYPE_MENULIST = 0x1E,
		ASSET_TYPE_MENU = 0x1F,
		ASSET_TYPE_ANIMCLASS = 0x20,
		ASSET_TYPE_LOCALIZE_ENTRY = 0x21,
		ASSET_TYPE_ATTACHMENT = 0x22,
		ASSET_TYPE_WEAPON = 0x23,
		ASSET_TYPE_SNDDRIVER_GLOBALS = 0x24,
		ASSET_TYPE_FX = 0x25,
		ASSET_TYPE_IMPACT_FX = 0x26,
		ASSET_TYPE_SURFACE_FX = 0x27,
		ASSET_TYPE_AITYPE = 0x28,
		ASSET_TYPE_MPTYPE = 0x29,
		ASSET_TYPE_CHARACTER = 0x2A,
		ASSET_TYPE_XMODELALIAS = 0x2B,
		ASSET_TYPE_RAWFILE = 0x2C,
		ASSET_TYPE_SCRIPTFILE = 0x2D,
		ASSET_TYPE_STRINGTABLE = 0x2E,
		ASSET_TYPE_LEADERBOARD = 0x2F,
		ASSET_TYPE_STRUCTURED_DATA_DEF = 0x30,
		ASSET_TYPE_TRACER = 0x31,
		ASSET_TYPE_VEHICLE = 0x32,
		ASSET_TYPE_ADDON_MAP_ENTS = 0x33,
		ASSET_TYPE_NET_CONST_STRINGS = 0x34,
		ASSET_TYPE_REVERB_PRESET = 0x35,
		ASSET_TYPE_LUA_FILE = 0x36,
		ASSET_TYPE_SCRIPTABLE = 0x37,
		ASSET_TYPE_COLORIZATION = 0x38,
		ASSET_TYPE_COLORIZATIONSET = 0x39,
		ASSET_TYPE_TONEMAPPING = 0x3A,
		ASSET_TYPE_EQUIPMENT_SND_TABLE = 0x3B,
		ASSET_TYPE_VECTORFIELD = 0x3C,
		ASSET_TYPE_DOPPLER_PRESET = 0x3D,
		ASSET_TYPE_PARTICLE_SIM_ANIMATION = 0x3E,
		ASSET_TYPE_COUNT = 0x3F,
		ASSET_TYPE_STRING = 0x3F,
		ASSET_TYPE_ASSETLIST = 0x40,
	};

	enum DBSyncMode
	{
		DB_LOAD_ASYNC = 0x0,
		DB_LOAD_SYNC = 0x1,
		DB_LOAD_ASYNC_WAIT_ALLOC = 0x2,
		DB_LOAD_ASYNC_FORCE_FREE = 0x3,
		DB_LOAD_ASYNC_NO_SYNC_THREADS = 0x4,
		DB_LOAD_SYNC_SKIP_ALWAYS_LOADED = 0x5,
	};

	enum
	{
		THREAD_CONTEXT_MAIN = 0x0,
		THREAD_CONTEXT_BACKEND = 0x1,
		THREAD_CONTEXT_WORKER0 = 0x2,
		THREAD_CONTEXT_WORKER1 = 0x3,
		THREAD_CONTEXT_WORKER2 = 0x4,
		THREAD_CONTEXT_WORKER3 = 0x5,
		THREAD_CONTEXT_WORKER4 = 0x6,
		THREAD_CONTEXT_WORKER5 = 0x7,
		THREAD_CONTEXT_WORKER6 = 0x8,
		THREAD_CONTEXT_WORKER7 = 0x9,
		THREAD_CONTEXT_SERVER = 0xA,
		THREAD_CONTEXT_TRACE_COUNT = 0xB,
		THREAD_CONTEXT_TRACE_LAST = 0xA,
		THREAD_CONTEXT_CINEMATIC = 0xB,
		THREAD_CONTEXT_DATABASE = 0xC,
		THREAD_CONTEXT_STREAM = 0xD,
		THREAD_CONTEXT_SNDSTREAMPACKETCALLBACK = 0xE,
		THREAD_CONTEXT_STATS_WRITE = 0xF,
		THREAD_CONTEXT_COUNT = 0x10,
	};

	union Weapon
	{
		struct
		{
			unsigned int weaponIdx : 8;
			unsigned int weaponVariation : 6;
			unsigned int weaponScopes : 3;
			unsigned int weaponUnderBarrels : 2;
			unsigned int weaponOthers : 7;
			unsigned int scopeVariation : 6;
		} __s0;
		unsigned int data;
	};

	enum PMem_Source
	{
		PMEM_SOURCE_EXTERNAL = 0x0,
		PMEM_SOURCE_DATABASE = 0x1,
		PMEM_SOURCE_DEFAULT_LOW = 0x2,
		PMEM_SOURCE_DEFAULT_HIGH = 0x3,
		PMEM_SOURCE_MOVIE = 0x4,
		PMEM_SOURCE_SCRIPT = 0x5,
	};

	enum errorParm
	{
		ERR_FATAL = 0,
		ERR_DROP = 1,
		ERR_SERVERDISCONNECT = 2,
		ERR_DISCONNECT = 3,
		ERR_SCRIPT = 4,
		ERR_SCRIPT_DROP = 5,
		ERR_LOCALIZATION = 6,
		ERR_MAPLOADERRORSUMMARY = 7,
	};

	enum CodPlayMode
	{
		CODPLAYMODE_NONE = 0x0,
		CODPLAYMODE_SP = 0x1,
		CODPLAYMODE_CORE = 0x2,
		CODPLAYMODE_ALIENS = 0x3,
		CODPLAYMODE_SQUADVSSQUAD = 0x4,
		CODPLAYMODE_COUNT = 0x5,
	};

	enum DWOnlineStatus
	{
		DW_LIVE_DISCONNECTED = 0x0,
		DW_LIVE_CONNECTING = 0x1,
		DW_LIVE_CONNECTED = 0x2,
	};

	enum bdLobbyErrorCode : uint32_t
	{
		BD_NO_ERROR = 0x0,
		BD_TOO_MANY_TASKS = 0x1,
		BD_NOT_CONNECTED = 0x2,
		BD_SEND_FAILED = 0x3,
		BD_HANDLE_TASK_FAILED = 0x4,
		BD_START_TASK_FAILED = 0x5,
		BD_RESULT_EXCEEDS_BUFFER_SIZE = 0x64,
		BD_ACCESS_DENIED = 0x65,
		BD_EXCEPTION_IN_DB = 0x66,
		BD_MALFORMED_TASK_HEADER = 0x67,
		BD_INVALID_ROW = 0x68,
		BD_EMPTY_ARG_LIST = 0x69,
		BD_PARAM_PARSE_ERROR = 0x6A,
		BD_PARAM_MISMATCHED_TYPE = 0x6B,
		BD_SERVICE_NOT_AVAILABLE = 0x6C,
		BD_CONNECTION_RESET = 0x6D,
		BD_INVALID_USER_ID = 0x6E,
		BD_LOBBY_PROTOCOL_VERSION_FAILURE = 0x6F,
		BD_LOBBY_INTERNAL_FAILURE = 0x70,
		BD_LOBBY_PROTOCOL_ERROR = 0x71,
		BD_LOBBY_FAILED_TO_DECODE_UTF8 = 0x72,
		BD_LOBBY_ASCII_EXPECTED = 0x73,
		BD_ASYNCHRONOUS_ERROR = 0xC8,
		BD_STREAMING_COMPLETE = 0xC9,
		BD_MEMBER_NO_PROPOSAL = 0x12C,
		BD_TEAMNAME_ALREADY_EXISTS = 0x12D,
		BD_MAX_TEAM_MEMBERSHIPS_LIMITED = 0x12E,
		BD_MAX_TEAM_OWNERSHIPS_LIMITED = 0x12F,
		BD_NOT_A_TEAM_MEMBER = 0x130,
		BD_INVALID_TEAM_ID = 0x131,
		BD_INVALID_TEAM_NAME = 0x132,
		BD_NOT_A_TEAM_OWNER = 0x133,
		BD_NOT_AN_ADMIN_OR_OWNER = 0x134,
		BD_MEMBER_PROPOSAL_EXISTS = 0x135,
		BD_MEMBER_EXISTS = 0x136,
		BD_TEAM_FULL = 0x137,
		BD_VULGAR_TEAM_NAME = 0x138,
		BD_TEAM_USERID_BANNED = 0x139,
		BD_TEAM_EMPTY = 0x13A,
		BD_INVALID_TEAM_PROFILE_QUERY_ID = 0x13B,
		BD_TEAMNAME_TOO_SHORT = 0x13C,
		BD_UNIQUE_PROFILE_DATA_EXISTS_ALREADY = 0x13D,
		BD_INVALID_LEADERBOARD_ID = 0x190,
		BD_INVALID_STATS_SET = 0x191,
		BD_EMPTY_STATS_SET_IGNORED = 0x193,
		BD_NO_DIRECT_ACCESS_TO_ARBITRATED_LBS = 0x194,
		BD_STATS_WRITE_PERMISSION_DENIED = 0x195,
		BD_STATS_WRITE_TYPE_DATA_TYPE_MISMATCH = 0x196,
		BD_NO_STATS_FOR_USER = 0x197,
		BD_INVALID_ACCESS_TO_UNRANKED_LB = 0x198,
		BD_INVALID_EXTERNAL_TITLE_ID = 0x199,
		BD_DIFFERENT_LEADERBOARD_SCHEMAS = 0x19A,
		BD_TOO_MANY_LEADERBOARDS_REQUESTED = 0x19B,
		BD_ENTITLEMENTS_ERROR = 0x19C,
		BD_ENTITLEMENTS_INVALID_TITLEID = 0x19D,
		BD_ENTITLEMENTS_INVALID_LEADERBOARDID = 0x19E,
		BD_ENTITLEMENTS_INVALID_GET_MODE_FOR_TITLE = 0x19F,
		BD_ENTITLEMENTS_URL_CONNECTION_ERROR = 0x1A0,
		BD_ENTITLEMENTS_CONFIG_ERROR = 0x1A1,
		BD_ENTITLEMENTS_NAMED_PARENT_ERROR = 0x1A2,
		BD_ENTITLEMENTS_NAMED_KEY_ERROR = 0x1A3,
		BD_TOO_MANY_ENTITY_IDS_REQUESTED = 0x1A4,
		BD_STATS_READ_FAILED = 0x1A5,
		BD_INVALID_TITLE_ID = 0x1F4,
		BD_MESSAGING_INVALID_MAIL_ID = 0x258,
		BD_SELF_BLOCK_NOT_ALLOWED = 0x259,
		BD_GLOBAL_MESSAGE_ACCESS_DENIED = 0x25A,
		BD_GLOBAL_MESSAGES_USER_LIMIT_EXCEEDED = 0x25B,
		BD_MESSAGING_SENDER_DOES_NOT_EXIST = 0x25C,
		BD_AUTH_NO_ERROR = 0x2BC,
		BD_AUTH_BAD_REQUEST = 0x2BD,
		BD_AUTH_SERVER_CONFIG_ERROR = 0x2BE,
		BD_AUTH_BAD_TITLE_ID = 0x2BF,
		BD_AUTH_BAD_ACCOUNT = 0x2C0,
		BD_AUTH_ILLEGAL_OPERATION = 0x2C1,
		BD_AUTH_INCORRECT_LICENSE_CODE = 0x2C2,
		BD_AUTH_CREATE_USERNAME_EXISTS = 0x2C3,
		BD_AUTH_CREATE_USERNAME_ILLEGAL = 0x2C4,
		BD_AUTH_CREATE_USERNAME_VULGAR = 0x2C5,
		BD_AUTH_CREATE_MAX_ACC_EXCEEDED = 0x2C6,
		BD_AUTH_MIGRATE_NOT_SUPPORTED = 0x2C7,
		BD_AUTH_TITLE_DISABLED = 0x2C8,
		BD_AUTH_ACCOUNT_EXPIRED = 0x2C9,
		BD_AUTH_ACCOUNT_LOCKED = 0x2CA,
		BD_AUTH_UNKNOWN_ERROR = 0x2CB,
		BD_AUTH_INCORRECT_PASSWORD = 0x2CC,
		BD_AUTH_IP_NOT_IN_ALLOWED_RANGE = 0x2CD,
		BD_AUTH_WII_TOKEN_VERIFICATION_FAILED = 0x2CE,
		BD_AUTH_WII_AUTHENTICATION_FAILED = 0x2CF,
		BD_AUTH_IP_KEY_LIMIT_REACHED = 0x2D0,
		BD_AUTH_INVALID_GSPID = 0x2D1,
		BD_AUTH_INVALID_IP_RANGE_ID = 0x2D2,
		BD_AUTH_3DS_TOKEN_VERIFICATION_FAILED = 0x2D1,
		BD_AUTH_3DS_AUTHENTICATION_FAILED = 0x2D2,
		BD_AUTH_STEAM_APP_ID_MISMATCH = 0x2D3,
		BD_AUTH_ABACCOUNTS_APP_ID_MISMATCH = 0x2D4,
		BD_AUTH_CODO_USERNAME_NOT_SET = 0x2D5,
		BD_AUTH_WIIU_TOKEN_VERIFICATION_FAILED = 0x2D6,
		BD_AUTH_WIIU_AUTHENTICATION_FAILED = 0x2D7,
		BD_AUTH_CODO_USERNAME_NOT_BASE64 = 0x2D8,
		BD_AUTH_CODO_USERNAME_NOT_UTF8 = 0x2D9,
		BD_AUTH_TENCENT_TICKET_EXPIRED = 0x2DA,
		BD_AUTH_PS3_SERVICE_ID_MISMATCH = 0x2DB,
		BD_AUTH_CODOID_NOT_WHITELISTED = 0x2DC,
		BD_AUTH_PLATFORM_TOKEN_ERROR = 0x2DD,
		BD_AUTH_JSON_FORMAT_ERROR = 0x2DE,
		BD_AUTH_REPLY_CONTENT_ERROR = 0x2DF,
		BD_AUTH_THIRD_PARTY_TOKEN_EXPIRED = 0x2E0,
		BD_AUTH_CONTINUING = 0x2E1,
		BD_AUTH_PLATFORM_DEVICE_ID_ERROR = 0x2E4,
		BD_NO_PROFILE_INFO_EXISTS = 0x320,
		BD_FRIENDSHIP_NOT_REQUSTED = 0x384,
		BD_NOT_A_FRIEND = 0x385,
		BD_SELF_FRIENDSHIP_NOT_ALLOWED = 0x387,
		BD_FRIENDSHIP_EXISTS = 0x388,
		BD_PENDING_FRIENDSHIP_EXISTS = 0x389,
		BD_USERID_BANNED = 0x38A,
		BD_FRIENDS_FULL = 0x38C,
		BD_FRIENDS_NO_RICH_PRESENCE = 0x38D,
		BD_RICH_PRESENCE_TOO_LARGE = 0x38E,
		BD_NO_FILE = 0x3E8,
		BD_PERMISSION_DENIED = 0x3E9,
		BD_FILESIZE_LIMIT_EXCEEDED = 0x3EA,
		BD_FILENAME_MAX_LENGTH_EXCEEDED = 0x3EB,
		BD_EXTERNAL_STORAGE_SERVICE_ERROR = 0x3EC,
		BD_CHANNEL_DOES_NOT_EXIST = 0x44D,
		BD_CHANNEL_ALREADY_SUBSCRIBED = 0x44E,
		BD_CHANNEL_NOT_SUBSCRIBED = 0x44F,
		BD_CHANNEL_FULL = 0x450,
		BD_CHANNEL_SUBSCRIPTIONS_FULL = 0x451,
		BD_CHANNEL_NO_SELF_WHISPERING = 0x452,
		BD_CHANNEL_ADMIN_REQUIRED = 0x453,
		BD_CHANNEL_TARGET_NOT_SUBSCRIBED = 0x454,
		BD_CHANNEL_REQUIRES_PASSWORD = 0x455,
		BD_CHANNEL_TARGET_IS_SELF = 0x456,
		BD_CHANNEL_PUBLIC_BAN_NOT_ALLOWED = 0x457,
		BD_CHANNEL_USER_BANNED = 0x458,
		BD_CHANNEL_PUBLIC_PASSWORD_NOT_ALLOWED = 0x459,
		BD_CHANNEL_PUBLIC_KICK_NOT_ALLOWED = 0x45A,
		BD_CHANNEL_MUTED = 0x45B,
		BD_EVENT_DESC_TRUNCATED = 0x4B0,
		BD_CONTENT_UNLOCK_UNKNOWN_ERROR = 0x514,
		BD_UNLOCK_KEY_INVALID = 0x515,
		BD_UNLOCK_KEY_ALREADY_USED_UP = 0x516,
		BD_SHARED_UNLOCK_LIMIT_REACHED = 0x517,
		BD_DIFFERENT_HARDWARE_ID = 0x518,
		BD_INVALID_CONTENT_OWNER = 0x519,
		BD_CONTENT_UNLOCK_INVALID_USER = 0x51A,
		BD_CONTENT_UNLOCK_INVALID_CATEGORY = 0x51B,
		BD_KEY_ARCHIVE_INVALID_WRITE_TYPE = 0x5DC,
		BD_KEY_ARCHIVE_EXCEEDED_MAX_IDS_PER_REQUEST = 0x5DD,
		BD_BANDWIDTH_TEST_TRY_AGAIN = 0x712,
		BD_BANDWIDTH_TEST_STILL_IN_PROGRESS = 0x713,
		BD_BANDWIDTH_TEST_NOT_PROGRESS = 0x714,
		BD_BANDWIDTH_TEST_SOCKET_ERROR = 0x715,
		BD_INVALID_SESSION_NONCE = 0x76D,
		BD_ARBITRATION_FAILURE = 0x76F,
		BD_ARBITRATION_USER_NOT_REGISTERED = 0x771,
		BD_ARBITRATION_NOT_CONFIGURED = 0x772,
		BD_CONTENTSTREAMING_FILE_NOT_AVAILABLE = 0x7D0,
		BD_CONTENTSTREAMING_STORAGE_SPACE_EXCEEDED = 0x7D1,
		BD_CONTENTSTREAMING_NUM_FILES_EXCEEDED = 0x7D2,
		BD_CONTENTSTREAMING_UPLOAD_BANDWIDTH_EXCEEDED = 0x7D3,
		BD_CONTENTSTREAMING_FILENAME_MAX_LENGTH_EXCEEDED = 0x7D4,
		BD_CONTENTSTREAMING_MAX_THUMB_DATA_SIZE_EXCEEDED = 0x7D5,
		BD_CONTENTSTREAMING_DOWNLOAD_BANDWIDTH_EXCEEDED = 0x7D6,
		BD_CONTENTSTREAMING_NOT_ENOUGH_DOWNLOAD_BUFFER_SPACE = 0x7D7,
		BD_CONTENTSTREAMING_SERVER_NOT_CONFIGURED = 0x7D8,
		BD_CONTENTSTREAMING_INVALID_APPLE_RECEIPT = 0x7DA,
		BD_CONTENTSTREAMING_APPLE_STORE_NOT_AVAILABLE = 0x7DB,
		BD_CONTENTSTREAMING_APPLE_RECEIPT_FILENAME_MISMATCH = 0x7DC,
		BD_CONTENTSTREAMING_HTTP_ERROR = 0x7E4,
		BD_CONTENTSTREAMING_FAILED_TO_START_HTTP = 0x7E5,
		BD_CONTENTSTREAMING_LOCALE_INVALID = 0x7E6,
		BD_CONTENTSTREAMING_LOCALE_MISSING = 0x7E7,
		BD_VOTERANK_ERROR_EMPTY_RATING_SUBMISSION = 0x7EE,
		BD_VOTERANK_ERROR_MAX_VOTES_EXCEEDED = 0x7EF,
		BD_VOTERANK_ERROR_INVALID_RATING = 0x7F0,
		BD_MAX_NUM_TAGS_EXCEEDED = 0x82A,
		BD_TAGGED_COLLECTION_DOES_NOT_EXIST = 0x82B,
		BD_EMPTY_TAG_ARRAY = 0x82C,
		BD_INVALID_QUERY_ID = 0x834,
		BD_NO_ENTRY_TO_UPDATE = 0x835,
		BD_SESSION_INVITE_EXISTS = 0x836,
		BD_INVALID_SESSION_ID = 0x837,
		BD_ATTACHMENT_TOO_LARGE = 0x838,
		BD_INVALID_GROUP_ID = 0xAF0,
		BD_MAIL_INVALID_MAIL_ID_ERROR = 0xB55,
		BD_UCD_SERVICE_ERROR = 0xC80,
		BD_UCD_SERVICE_DISABLED = 0xC81,
		BD_UCD_UNINTIALIZED_ERROR = 0xC82,
		BD_UCD_ACCOUNT_ALREADY_REGISTERED = 0xC83,
		BD_UCD_ACCOUNT_NOT_REGISTERED = 0xC84,
		BD_UCD_AUTH_ATTEMPT_FAILED = 0xC85,
		BD_UCD_ACCOUNT_LINKING_ERROR = 0xC86,
		BD_UCD_ENCRYPTION_ERROR = 0xC87,
		BD_UCD_ACCOUNT_DATA_INVALID = 0xC88,
		BD_UCD_ACCOUNT_DATA_INVALID_FIRSTNAME = 0xC89,
		BD_UCD_ACCOUNT_DATA_INVALID_LASTNAME = 0xC8A,
		BD_UCD_ACCOUNT_DATA_INVALID_DOB = 0xC8B,
		BD_UCD_ACCOUNT_DATA_INVALID_EMAIL = 0xC8C,
		BD_UCD_ACCOUNT_DATA_INVALID_COUNTRY = 0xC8D,
		BD_UCD_ACCOUNT_DATA_INVALID_POSTCODE = 0xC8E,
		BD_UCD_ACCOUNT_DATA_INVALID_PASSWORD = 0xC8F,
		BD_UCD_ACCOUNT_NAME_ALREADY_RESISTERED = 0xC94,
		BD_UCD_ACCOUNT_EMAIL_ALREADY_RESISTERED = 0xC95,
		BD_UCD_GUEST_ACCOUNT_AUTH_CONFLICT = 0xC96,
		BD_TWITCH_SERVICE_ERROR = 0xC1D,
		BD_TWITCH_ACCOUNT_ALREADY_LINKED = 0xC1E,
		BD_TWITCH_NO_LINKED_ACCOUNT = 0xC1F,
		BD_YOUTUBE_SERVICE_ERROR = 0xCE5,
		BD_YOUTUBE_SERVICE_COMMUNICATION_ERROR = 0xCE6,
		BD_YOUTUBE_USER_DENIED_AUTHORIZATION = 0xCE7,
		BD_YOUTUBE_AUTH_MAX_TIME_EXCEEDED = 0xCE8,
		BD_YOUTUBE_USER_UNAUTHORIZED = 0xCE9,
		BD_YOUTUBE_UPLOAD_MAX_TIME_EXCEEDED = 0xCEA,
		BD_YOUTUBE_DUPLICATE_UPLOAD = 0xCEB,
		BD_YOUTUBE_FAILED_UPLOAD = 0xCEC,
		BD_YOUTUBE_ACCOUNT_ALREADY_REGISTERED = 0xCED,
		BD_YOUTUBE_ACCOUNT_NOT_REGISTERED = 0xCEE,
		BD_YOUTUBE_CONTENT_SERVER_ERROR = 0xCEF,
		BD_YOUTUBE_UPLOAD_DOES_NOT_EXIST = 0xCF0,
		BD_YOUTUBE_NO_LINKED_ACCOUNT = 0xCF1,
		BD_YOUTUBE_DEVELOPER_TAGS_INVALID = 0xCF2,
		BD_TWITTER_AUTH_ATTEMPT_FAILED = 0xDAD,
		BD_TWITTER_AUTH_TOKEN_INVALID = 0xDAE,
		BD_TWITTER_UPDATE_LIMIT_REACHED = 0xDAF,
		BD_TWITTER_UNAVAILABLE = 0xDB0,
		BD_TWITTER_ERROR = 0xDB1,
		BD_TWITTER_TIMED_OUT = 0xDB2,
		BD_TWITTER_DISABLED_FOR_USER = 0xDB3,
		BD_TWITTER_ACCOUNT_AMBIGUOUS = 0xDB4,
		BD_TWITTER_MAXIMUM_ACCOUNTS_REACHED = 0xDB5,
		BD_TWITTER_ACCOUNT_NOT_REGISTERED = 0xDB6,
		BD_TWITTER_DUPLICATE_STATUS = 0xDB7,
		BD_TWITTER_ACCOUNT_ALREADY_REGISTERED = 0xE1C,
		BD_FACEBOOK_AUTH_ATTEMPT_FAILED = 0xE11,
		BD_FACEBOOK_AUTH_TOKEN_INVALID = 0xE12,
		BD_FACEBOOK_PHOTO_DOES_NOT_EXIST = 0xE13,
		BD_FACEBOOK_PHOTO_INVALID = 0xE14,
		BD_FACEBOOK_PHOTO_ALBUM_FULL = 0xE15,
		BD_FACEBOOK_UNAVAILABLE = 0xE16,
		BD_FACEBOOK_ERROR = 0xE17,
		BD_FACEBOOK_TIMED_OUT = 0xE18,
		BD_FACEBOOK_DISABLED_FOR_USER = 0xE19,
		BD_FACEBOOK_ACCOUNT_AMBIGUOUS = 0xE1A,
		BD_FACEBOOK_MAXIMUM_ACCOUNTS_REACHED = 0xE1B,
		BD_FACEBOOK_INVALID_NUM_PICTURES_REQUESTED = 0xE1C,
		BD_FACEBOOK_VIDEO_DOES_NOT_EXIST = 0xE1D,
		BD_FACEBOOK_ACCOUNT_ALREADY_REGISTERED = 0xE1E,
		BD_APNS_INVALID_PAYLOAD = 0xE74,
		BD_APNS_INVALID_TOKEN_LENGTH_ERROR = 0xE76,
		BD_MAX_CONSOLEID_LENGTH_EXCEEDED = 0xEE1,
		BD_MAX_WHITELIST_LENGTH_EXCEEDED = 0xEE2,
		BD_USERGROUP_NAME_ALREADY_EXISTS = 0x1770,
		BD_INVALID_USERGROUP_ID = 0x1771,
		BD_USER_ALREADY_IN_USERGROUP = 0x1772,
		BD_USER_NOT_IN_USERGROUP = 0x1773,
		BD_INVALID_USERGROUP_MEMBER_TYPE = 0x1774,
		BD_TOO_MANY_MEMBERS_REQUESTED = 0x1775,
		BD_USERGROUP_NAME_TOO_SHORT = 0x1776,
		BD_RICH_PRESENCE_DATA_TOO_LARGE = 0x1A90,
		BD_RICH_PRESENCE_TOO_MANY_USERS = 0x1A91,
		BD_PRESENCE_DATA_TOO_LARGE = 0x283C,
		BD_PRESENCE_TOO_MANY_USERS = 0x283D,
		BD_USER_LOGGED_IN_OTHER_TITLE = 0x283E,
		BD_USER_NOT_LOGGED_IN = 0x283F,
		BD_SUBSCRIPTION_TOO_MANY_USERS = 0x1B58,
		BD_SUBSCRIPTION_TICKET_PARSE_ERROR = 0x1B59,
		BD_CODO_ID_INVALID_DATA = 0x1BBC,
		BD_INVALID_MESSAGE_FORMAT = 0x1BBD,
		BD_TLOG_TOO_MANY_MESSAGES = 0x1BBE,
		BD_CODO_ID_NOT_IN_WHITELIST = 0x1BBF,
		BD_TLOG_MESSAGE_TRANSFORMATION_ERROR = 0x1BC0,
		BD_REWARDS_NOT_ENABLED = 0x1BC1,
		BD_MARKETPLACE_ERROR = 0x1F40,
		BD_MARKETPLACE_RESOURCE_NOT_FOUND = 0x1F41,
		BD_MARKETPLACE_INVALID_CURRENCY = 0x1F42,
		BD_MARKETPLACE_INVALID_PARAMETER = 0x1F43,
		BD_MARKETPLACE_RESOURCE_CONFLICT = 0x1F44,
		BD_MARKETPLACE_STORAGE_ERROR = 0x1F45,
		BD_MARKETPLACE_INTEGRITY_ERROR = 0x1F46,
		BD_MARKETPLACE_INSUFFICIENT_FUNDS_ERROR = 0x1F47,
		BD_MARKETPLACE_MMP_SERVICE_ERROR = 0x1F48,
		BD_MARKETPLACE_PRECONDITION_REQUIRED = 0x1F49,
		BD_MARKETPLACE_ITEM_MULTIPLE_PURCHASE_ERROR = 0x1F4A,
		BD_MARKETPLACE_MISSING_REQUIRED_ENTITLEMENT = 0x1F4B,
		BD_MARKETPLACE_VALIDATION_ERROR = 0x1F4C,
		BD_MARKETPLACE_TENCENT_PAYMENT_ERROR = 0x1F4D,
		BD_MARKETPLACE_SKU_NOT_COUPON_ENABLED_ERROR = 0x1F4E,
		BD_LEAGUE_INVALID_TEAM_SIZE = 0x1FA4,
		BD_LEAGUE_INVALID_TEAM = 0x1FA5,
		BD_LEAGUE_INVALID_SUBDIVISION = 0x1FA6,
		BD_LEAGUE_INVALID_LEAGUE = 0x1FA7,
		BD_LEAGUE_TOO_MANY_RESULTS_REQUESTED = 0x1FA8,
		BD_LEAGUE_METADATA_TOO_LARGE = 0x1FA9,
		BD_LEAGUE_TEAM_ICON_TOO_LARGE = 0x1FAA,
		BD_LEAGUE_TEAM_NAME_TOO_LONG = 0x1FAB,
		BD_LEAGUE_ARRAY_SIZE_MISMATCH = 0x1FAC,
		BD_LEAGUE_SUBDIVISION_MISMATCH = 0x2008,
		BD_LEAGUE_INVALID_WRITE_TYPE = 0x2009,
		BD_LEAGUE_INVALID_STATS_DATA = 0x200A,
		BD_LEAGUE_SUBDIVISION_UNRANKED = 0x200B,
		BD_LEAGUE_CROSS_TEAM_STATS_WRITE_PREVENTED = 0x200C,
		BD_LEAGUE_INVALID_STATS_SEASON = 0x200D,
		BD_COMMERCE_ERROR = 0x206C,
		BD_COMMERCE_RESOURCE_NOT_FOUND = 0x206D,
		BD_COMMERCE_STORAGE_INVALID_PARAMETER = 0x206E,
		BD_COMMERCE_APPLICATION_INVALID_PARAMETER = 0x206F,
		BD_COMMERCE_RESOURCE_CONFLICT = 0x2070,
		BD_COMMERCE_STORAGE_ERROR = 0x2071,
		BD_COMMERCE_INTEGRITY_ERROR = 0x2072,
		BD_COMMERCE_MMP_SERVICE_ERROR = 0x2073,
		BD_COMMERCE_PERMISSION_DENIED = 0x2074,
		BD_COMMERCE_INSUFFICIENT_FUNDS_ERROR = 0x2075,
		BD_COMMERCE_UNKNOWN_CURRENCY = 0x2076,
		BD_COMMERCE_INVALID_RECEIPT = 0x2077,
		BD_COMMERCE_RECEIPT_USED = 0x2078,
		BD_COMMERCE_TRANSACTION_ALREADY_APPLIED = 0x2079,
		BD_COMMERCE_INVALID_CURRENCY_TYPE = 0x207A,
		BD_CONNECTION_COUNTER_ERROR = 0x20D0,
		BD_LINKED_ACCOUNTS_INVALID_CONTEXT = 0x2198,
		BD_LINKED_ACCOUNTS_INVALID_PLATFORM = 0x2199,
		BD_LINKED_ACCOUNTS_LINKED_ACCOUNTS_FETCH_ERROR = 0x219A,
		BD_LINKED_ACCOUNTS_INVALID_ACCOUNT = 0x219B,
		BD_GMSG_INVALID_CATEGORY_ID = 0x27D8,
		BD_GMSG_CATEGORY_MEMBERSHIPS_LIMIT = 0x27D9,
		BD_GMSG_NONMEMBER_POST_DISALLOWED = 0x27DA,
		BD_GMSG_CATEGORY_DISALLOWS_CLIENT_TYPE = 0x27DB,
		BD_GMSG_PAYLOAD_TOO_BIG = 0x27DC,
		BD_GMSG_MEMBER_POST_DISALLOWED = 0x27DD,
		BD_GMSG_OVERLOADED = 0x27DE,
		BD_GMSG_USER_PERCATEGORY_POST_RATE_EXCEEDED = 0x27DF,
		BD_GMSG_USER_GLOBAL_POST_RATE_EXCEEDED = 0x27E0,
		BD_GMSG_GROUP_POST_RATE_EXCEEDED = 0x27E1,
		BD_MAX_ERROR_CODE = 0x27E2,
	};

	enum bdNATType : uint8_t
	{
		BD_NAT_UNKNOWN = 0x0,
		BD_NAT_OPEN = 0x1,
		BD_NAT_MODERATE = 0x2,
		BD_NAT_STRICT = 0x3,
	};

#pragma pack(push, 1)
	struct bdAuthTicket
	{
		unsigned int m_magicNumber;
		char m_type;
		unsigned int m_titleID;
		unsigned int m_timeIssued;
		unsigned int m_timeExpires;
		unsigned long long m_licenseID;
		unsigned long long m_userID;
		char m_username[64];
		char m_sessionKey[24];
		char m_usingHashMagicNumber[3];
		char m_hash[4];
	};
#pragma pack(pop)

	enum keyNum_t
	{
		K_NONE = 0x0,
		K_FIRSTGAMEPADBUTTON_RANGE_1 = 0x1,
		K_BUTTON_A = 0x1,
		K_BUTTON_B = 0x2,
		K_BUTTON_X = 0x3,
		K_BUTTON_Y = 0x4,
		K_BUTTON_LSHLDR = 0x5,
		K_BUTTON_RSHLDR = 0x6,
		K_LASTGAMEPADBUTTON_RANGE_1 = 0x6,
		K_BS = 0x8,
		K_TAB = 0x9,
		K_ENTER = 0xD,
		K_FIRSTGAMEPADBUTTON_RANGE_2 = 0xE,
		K_BUTTON_START = 0xE,
		K_BUTTON_BACK = 0xF,
		K_BUTTON_LSTICK = 0x10,
		K_BUTTON_RSTICK = 0x11,
		K_BUTTON_LTRIG = 0x12,
		K_BUTTON_RTRIG = 0x13,
		K_DPAD_UP = 0x14,
		K_FIRSTDPAD = 0x14,
		K_DPAD_DOWN = 0x15,
		K_DPAD_LEFT = 0x16,
		K_DPAD_RIGHT = 0x17,
		K_BUTTON_LSTICK_ALTIMAGE2 = 0x10,
		K_BUTTON_RSTICK_ALTIMAGE2 = 0x11,
		K_BUTTON_LSTICK_ALTIMAGE = 0xBC,
		K_BUTTON_RSTICK_ALTIMAGE = 0xBD,
		K_LASTDPAD = 0x17,
		K_LASTGAMEPADBUTTON_RANGE_2 = 0x17,
		K_ESCAPE = 0x1B,
		K_FIRSTGAMEPADBUTTON_RANGE_3 = 0x1C,
		K_APAD_UP = 0x1C,
		K_FIRSTAPAD = 0x1C,
		K_APAD_DOWN = 0x1D,
		K_APAD_LEFT = 0x1E,
		K_APAD_RIGHT = 0x1F,
		K_LASTAPAD = 0x1F,
		K_LASTGAMEPADBUTTON_RANGE_3 = 0x1F,
		K_SPACE = 0x20,
		K_GRAVE = 0x60,
		K_TILDE = 0x7E,
		K_BACKSPACE = 0x7F,
		K_ASCII_FIRST = 0x80,
		K_ASCII_181 = 0x80,
		K_ASCII_191 = 0x81,
		K_ASCII_223 = 0x82,
		K_ASCII_224 = 0x83,
		K_ASCII_225 = 0x84,
		K_ASCII_228 = 0x85,
		K_ASCII_229 = 0x86,
		K_ASCII_230 = 0x87,
		K_ASCII_231 = 0x88,
		K_ASCII_232 = 0x89,
		K_ASCII_233 = 0x8A,
		K_ASCII_236 = 0x8B,
		K_ASCII_241 = 0x8C,
		K_ASCII_242 = 0x8D,
		K_ASCII_243 = 0x8E,
		K_ASCII_246 = 0x8F,
		K_ASCII_248 = 0x90,
		K_ASCII_249 = 0x91,
		K_ASCII_250 = 0x92,
		K_ASCII_252 = 0x93,
		K_END_ASCII_CHARS = 0x94,
		K_COMMAND = 0x96,
		K_CAPSLOCK = 0x97,
		K_POWER = 0x98,
		K_PAUSE = 0x99,
		K_UPARROW = 0x9A,
		K_DOWNARROW = 0x9B,
		K_LEFTARROW = 0x9C,
		K_RIGHTARROW = 0x9D,
		K_ALT = 0x9E,
		K_CTRL = 0x9F,
		K_SHIFT = 0xA0,
		K_INS = 0xA1,
		K_DEL = 0xA2,
		K_PGDN = 0xA3,
		K_PGUP = 0xA4,
		K_HOME = 0xA5,
		K_END = 0xA6,
		K_F1 = 0xA7,
		K_F2 = 0xA8,
		K_F3 = 0xA9,
		K_F4 = 0xAA,
		K_F5 = 0xAB,
		K_F6 = 0xAC,
		K_F7 = 0xAD,
		K_F8 = 0xAE,
		K_F9 = 0xAF,
		K_F10 = 0xB0,
		K_F11 = 0xB1,
		K_F12 = 0xB2,
		K_F13 = 0xB3,
		K_F14 = 0xB4,
		K_F15 = 0xB5,
		K_KP_HOME = 0xB6,
		K_KP_UPARROW = 0xB7,
		K_KP_PGUP = 0xB8,
		K_KP_LEFTARROW = 0xB9,
		K_KP_5 = 0xBA,
		K_KP_RIGHTARROW = 0xBB,
		K_KP_END = 0xBC,
		K_KP_DOWNARROW = 0xBD,
		K_KP_PGDN = 0xBE,
		K_KP_ENTER = 0xBF,
		K_KP_INS = 0xC0,
		K_KP_DEL = 0xC1,
		K_KP_SLASH = 0xC2,
		K_KP_MINUS = 0xC3,
		K_KP_PLUS = 0xC4,
		K_KP_NUMLOCK = 0xC5,
		K_KP_STAR = 0xC6,
		K_KP_EQUALS = 0xC7,
		K_MOUSE1 = 0xC8,
		K_MOUSE2 = 0xC9,
		K_MOUSE3 = 0xCA,
		K_MOUSE4 = 0xCB,
		K_MOUSE5 = 0xCC,
		K_MWHEELDOWN = 0xCD,
		K_MWHEELUP = 0xCE,
		K_AUX1 = 0xCF,
		K_AUX2 = 0xD0,
		K_AUX3 = 0xD1,
		K_AUX4 = 0xD2,
		K_AUX5 = 0xD3,
		K_AUX6 = 0xD4,
		K_AUX7 = 0xD5,
		K_AUX8 = 0xD6,
		K_AUX9 = 0xD7,
		K_AUX10 = 0xD8,
		K_AUX11 = 0xD9,
		K_AUX12 = 0xDA,
		K_AUX13 = 0xDB,
		K_AUX14 = 0xDC,
		K_AUX15 = 0xDD,
		K_AUX16 = 0xDE,
		K_LAST_KEY = 0xDF
	};

	enum ConfigString
	{
		CS_FIRST = 0x0,
		CS_SERVERINFO = 0x0,
		CS_SYSTEMINFO = 0x1,
		CS_SUNLIGHT = 0x2,
		CS_SERVERID = 0x3,
		CS_SCORES1 = 0x4,
		CS_SCORES2 = 0x5,
		CS_FOGVARS = 0x6,
		CS_GAMEENDTIME = 0x7,
		CS_MAPCENTER = 0x8,
		CS_SESSIONNONCE = 0x9,
		CS_TIMESCALE = 0xA,
		CS_CODINFO = 0xB,
		CS_CODINFO_LAST = 0x8A,
		CS_CODINFO_VALUE = 0x8B,
		CS_CODINFO_VALUE_LAST = 0x10A,
		CS_PLAYERINFOS = 0x10B,
		CS_PLAYERINFOS_LAST = 0x116,
		CS_ENABLE_HUD_LIGHTING = 0x117,
		CS_USE_TRIG_STRINGS = 0x118,
		CS_USE_TRIG_STRINGS_LAST = 0x216,
		CS_LOCALIZED_STRINGS = 0x217,
		CS_LOCALIZED_STRINGS_LAST = 0x415,
		CS_MATCHID = 0x416,
		CS_CASE_INSENSITIVE_BEGIN = 0x417,
		CS_AMBIENT = 0x417,
		CS_AMBIENT_LAST = 0x418,
		CS_AMBIENT_AC130 = 0x419,
		CS_RUMBLES = 0x41A,
		CS_RUMBLE_LAST = 0x439,
		CS_NORTHYAW = 0x43A,
		CS_MINIMAP = 0x43B,
		CS_MATERIAL_THERMALBODY = 0x43C,
		CS_VISIONSET_NORMAL = 0x43D,
		CS_VISIONSET_NIGHT = 0x43E,
		CS_VISIONSET_MISSILECAM = 0x43F,
		CS_VISIONSET_THERMAL = 0x440,
		CS_VISIONSET_PAIN = 0x441,
		CS_VISIONSET_POST_APPLY = 0x442,
		CS_LOC_SEL_MTLS = 0x443,
		CS_LOC_SEL_MTLS_LAST = 0x445,
		CS_MODELS = 0x446,
		CS_MODELS_LAST = 0x845,
		CS_VEHICLE_DEFS = 0x846,
		CS_VEHICLE_DEFS_LAST = 0x865,
		CS_SOUNDALIASES = 0x866,
		CS_SOUNDALIASES_LAST = 0x965,
		CS_EFFECT_NAMES = 0x966,
		CS_EFFECT_NAMES_LAST = 0xA65,
		CS_SHELLSHOCKS = 0xA66,
		CS_SHELLSHOCKS_LAST = 0xA75,
		CS_SCRIPT_MENUS = 0xA76,
		CS_SCRIPT_MENUS_LAST = 0xAA0,
		CS_SERVER_MATERIALS = 0xAA1,
		CS_SERVER_MATERIALS_LAST = 0xBFE,
		CS_CLIENT_TAGS = 0xBFF,
		CS_CLIENT_TAGS_LAST = 0xC0E,
		CS_HEAD_ICONS = 0xC0F,
		CS_HEAD_ICONS_LAST = 0xC1D,
		CS_WEAPONFILES = 0xC1E,
		CS_WEAPONFILES_LAST = 0xCBD,
		CS_STATUS_ICONS = 0xCBE,
		CS_STATUS_ICONS_LAST = 0xCC5,
		CS_MINIMAP_ICONS = 0xCC6,
		CS_MINIMAP_ICONS_LAST = 0xCD4,
		CS_NAMEPLATE_MTLS = 0xCD5,
		CS_NAMEPLATE_MTLS_LAST = 0xCE2,
		CS_MP_ANIMS = 0xCE3,
		CS_MP_ANIMS_LAST = 0xD21,
		CS_ITEMS = 0xD22,
		CS_LEADERBOARDS = 0xD23,
		MAX_CONFIGSTRINGS = 0xD24,
	};

	enum LiveClientDropType
	{
		SV_LIVE_DROP_NONE = 0x0,
		SV_LIVE_DROP_DISCONNECT = 0x1,
		SV_LIVE_DROP_RMSG_LOST = 0x2,
		SV_LIVE_DROP_INACTIVE = 0x3,
		SV_LIVE_DROP_P2P_AUTH = 0x4,
		SV_LIVE_DROP_COUNT = 0x5,
	};

	enum TestClientType
	{
		TC_NONE = 0x0,
		TC_TEST_CLIENT = 0x1,
		TC_BOT = 0x2,
		TC_COUNT = 0x3,
	};

	enum StatsGroup
	{
		STATSGROUP_RANKED = 0x0,
		STATSGROUP_PRIVATE = 0x1,
		STATSGROUP_COOP = 0x2,
		STATSGROUP_COMMON = 0x3,
		STATSGROUP_COUNT = 0x4,
		STATSGROUP_IGNORE = 0x5,
	};

	enum StatsSource
	{
		STATS_ONLINE = 0x0,
		STATS_COUNT = 0x1,
	};

	enum StructuredDataTypeCategory
	{
		DATA_INT = 0x0,
		DATA_BYTE = 0x1,
		DATA_BOOL = 0x2,
		DATA_STRING = 0x3,
		DATA_ENUM = 0x4,
		DATA_STRUCT = 0x5,
		DATA_INDEXED_ARRAY = 0x6,
		DATA_ENUM_ARRAY = 0x7,
		DATA_FLOAT = 0x8,
		DATA_SHORT = 0x9,
		DATA_COUNT = 0xA,
	};

	enum StructuredData_SetResult
	{
		SETRESULT_SUCCESS_CHANGED = 0x0,
		SETRESULT_SUCCESS_NOCHANGE = 0x1,
		SETRESULT_WRONG_DATA_TYPE = 0x2,
		SETRESULT_ERROR_INT_TOO_LARGE_FOR_BYTE = 0x3,
		SETRESULT_ERROR_STRING_TOO_LONG = 0x4,
		SETRESULT_ERROR_INVALID_ENUM_VALUE = 0x5,
		SETRESULT_COUNT = 0x6,
	};

	enum LookupError
	{
		LOOKUP_ERROR_NONE = 0x0,
		LOOKUP_ERROR_WRONG_DATA_TYPE = 0x1,
		LOOKUP_ERROR_INDEX_OUTSIDE_BOUNDS = 0x2,
		LOOKUP_ERROR_INVALID_STRUCT_PROPERTY = 0x3,
		LOOKUP_ERROR_INVALID_ENUM_VALUE = 0x4,
		LOOKUP_ERROR_COUNT = 0x5,
	};

	struct StructuredDataEnumEntry
	{
		unsigned int name;
		unsigned __int16 index;
	};

	struct StructuredDataEnum
	{
		int entryCount;
		int reservedEntryCount;
		StructuredDataEnumEntry* entries;
	};

	union StructuredDataTypeUnion
	{
		unsigned int stringDataLength;
		int enumIndex;
		int structIndex;
		int indexedArrayIndex;
		int enumedArrayIndex;
	};

	struct StructuredDataType
	{
		StructuredDataTypeCategory type;
		StructuredDataTypeUnion u;
	};

	struct StructuredDataStructProperty
	{
		unsigned int name;
		StructuredDataType item;
		int offset;
		int validation;
	};

	struct StructuredDataStruct
	{
		int propertyCount;
		StructuredDataStructProperty* properties;
		int size;
		unsigned int bitOffset;
	};

	struct StructuredDataIndexedArray
	{
		int arraySize;
		StructuredDataType elementType;
		unsigned int elementSize;
	};

	struct StructuredDataEnumedArray
	{
		int enumIndex;
		StructuredDataType elementType;
		unsigned int elementSize;
	};

	struct StructuredDataDef
	{
		int version;
		unsigned int formatChecksum;
		int enumCount;
		StructuredDataEnum* enums;
		int structCount;
		StructuredDataStruct* structs;
		int indexedArrayCount;
		StructuredDataIndexedArray* indexedArrays;
		int enumedArrayCount;
		StructuredDataEnumedArray* enumedArrays;
		StructuredDataType rootType;
		unsigned int size;
	};

	struct StructuredDataDefSet
	{
		const char* name;
		unsigned int defCount;
		StructuredDataDef* defs;
	};

	struct StructuredDataLookup
	{
		StructuredDataDef* def;
		StructuredDataType* rootType;
		unsigned int offset;
		LookupError error;
	};

	struct StructuredDataBuffer
	{
		char* data;
		unsigned int size;
	};

	struct StringTableCell
	{
		const char* string;
		int hash;
	};

	struct StringTable
	{
		const char* name;
		int columnCount;
		int rowCount; 
		StringTableCell* values; 
	};

	struct Material
	{
		const char* name;
	};

	struct Glyph
	{
		unsigned short letter;
		char x0;
		char y0;
		char dx;
		char pixelWidth;
		char pixelHeight;
		float s0;
		float t0;
		float s1;
		float t1;
	};

	struct Font_s
	{
		const char* fontName;
		int pixelHeight;
		int glyphCount;
		Material* material;
		Material* glowMaterial;
		Glyph* glyphs;
	};

	enum connstate_t
	{
		CA_DISCONNECTED = 0x0,
		CA_CINEMATIC = 0x1,
		CA_LOGO = 0x2,
		CA_CONNECTING = 0x3,
		CA_CHALLENGING = 0x4,
		CA_CONNECTED = 0x5,
		CA_SENDINGSTATS = 0x6,
		CA_SYNCHRONIZING_DATA = 0x7,
		CA_LOADING = 0x8,
		CA_PRIMED = 0x9,
		CA_ACTIVE = 0xA,
	};

	enum DvarSetSource : std::uint32_t
	{
		DVAR_SOURCE_INTERNAL = 0x0,
		DVAR_SOURCE_EXTERNAL = 0x1,
		DVAR_SOURCE_SCRIPT = 0x2,
		DVAR_SOURCE_UISCRIPT = 0x3,
		DVAR_SOURCE_SERVERCMD = 0x4,
		DVAR_SOURCE_NUM = 0x5,
	};

	enum DvarFlags : std::uint32_t
	{
		DVAR_FLAG_NONE = 0,
		DVAR_FLAG_SAVED = 0x1,
		DVAR_FLAG_LATCHED = 0x2,
		DVAR_FLAG_CHEAT = 0x4,
		DVAR_FLAG_REPLICATED = 0x8,
		DVAR_FLAG_WRITE = 0x800,
		DVAR_FLAG_READ = 0x2000,
	};

	enum dvar_type : std::int8_t
	{
		boolean = 0,
		value = 1,
		vec2 = 2,
		vec3 = 3,
		vec4 = 4,
		integer = 5,
		enumeration = 6,
		string = 7,
		color = 8,
		rgb = 9 // Color without alpha
	};

	union dvar_value
	{
		bool enabled;
		int integer;
		unsigned int unsignedInt;
		float value;
		float vector[4];
		const char* string;
		char color[4];
	};

	struct $A37BA207B3DDD6345C554D4661813EDD
	{
		int stringCount;
		const char* const* strings;
	};

	struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
	{
		int min;
		int max;
	};

	struct $251C2428A496074035CACA7AAF3D55BD
	{
		float min;
		float max;
	};

	union dvar_limits
	{
		$A37BA207B3DDD6345C554D4661813EDD enumeration;
		$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
		$251C2428A496074035CACA7AAF3D55BD value;
		$251C2428A496074035CACA7AAF3D55BD vector;
	};

	struct dvar_t
	{
		const char* name; //00
		unsigned int flags; //08
		dvar_type type; //0C
		bool modified; //0D
		dvar_value current; //10
		dvar_value latched;
		dvar_value reset;
		dvar_limits domain;
	};

	struct CmdArgs
	{
		int nesting;
		int localClientNum[8];
		int controllerIndex[8];
		int argc[8];
		const char** argv[8];
	};

	struct cmd_function_s
	{
		cmd_function_s* next;
		const char* name;
		void (__cdecl* function)();
	};

	struct KeyState
	{
		int down;
		int repeats;
		int binding;
	};

	struct PlayerKeyState
	{
		int overstrikeMode;
		int anyKeyDown;
		KeyState keys[256];
	};

	struct ScreenPlacement
	{
		vec2_t scaleVirtualToReal;
		vec2_t scaleVirtualToFull;
		vec2_t scaleRealToVirtual;
		vec2_t realViewportPosition;
		vec2_t realViewportSize;
		vec2_t virtualViewableMin;
		vec2_t virtualViewableMax;
		vec2_t realViewableMin;
		vec2_t realViewableMax;
		vec2_t virtualAdjustableMin;
		vec2_t virtualAdjustableMax;
		vec2_t realAdjustableMin;
		vec2_t realAdjustableMax;
		vec2_t subScreenLeft;
	};

	struct Bounds
	{
		vec3_t midPoint;
		vec3_t halfSize;
	};

	enum PlayerHandIndex
	{
		WEAPON_HAND_RIGHT = 0x0,
		WEAPON_HAND_LEFT = 0x1,
		NUM_WEAPON_HANDS = 0x2,
		WEAPON_HAND_DEFAULT = 0x0,
	};

	struct usercmd_s
	{
		int serverTime;
		unsigned int buttons;
		int angles[3];
		int weapon;
		int offHand;
		char forwardmove;
		char rightmove;
		unsigned short airburstMarkDistance;
		unsigned short meleeChargeEnt;
		char meleeChargeDist;
		char selectedLoc[2];
		char selectedLocAngle;
		char remoteControlAngles[2];
		char remoteControlMove[3];
		unsigned int sightedClientsMask;
		unsigned __int16 spawnTraceEntIndex;
		unsigned int sightedSpawnsMask;
		unsigned int partialSightedSpawnsMask;
	};

	struct EntityEvent
	{
		int eventType;
		int eventParm;
	};

	struct playerEvents_t
	{
		int eventSequence;
		EntityEvent events[4];
		int oldEventSequence;
		int timeADSCameUp;
	};

	struct PlayerVehicleState
	{
		int entity;
		int flags;
		int targetEntity;
		vec3_t origin;
		vec3_t angles;
		vec3_t velocity;
		vec3_t angVelocity;
		vec2_t tilt;
		vec2_t tiltVelocity;
		vec2_t gunAngles;
		unsigned int splineId;
		unsigned int splineNodeIndex;
		float splineLambda;
		vec2_t corridorSpeeds;
	};

	struct PlayerActiveWeaponState
	{
		int weapAnim;
		int weaponTime;
		int weaponDelay;
		int weaponRestrictKickTime;
		int weaponState;
		int weapHandFlags;
		unsigned int weaponShotCount;
	};

	struct PlayerEquippedWeaponState
	{
		bool usedBefore;
		bool dualWielding;
		bool inAltMode;
		bool needsRechamber[2];
		int zoomLevelIndex;
		bool thermalEnabled;
		bool hybridScope;
	};

	struct compressedAnimData_s
	{
		int flags;
		int animRate;
		int distanceIn2D;
		int distanceOut2D;
		int distanceInZ;
		int distanceOutZ;
		int endScriptAnimTableIndex;
	};

	struct MantleState
	{
		float yaw;
		int startPitch;
		int transIndex;
		int flags;
		int startTime;
		float startPosition[3];
		compressedAnimData_s compressedAnimData;
	};

	struct SlideState
	{
		int flags;
		int noFricTime;
	};

	struct SprintState_s
	{
		int sprintButtonUpRequired;
		int sprintDelay;
		int lastSprintStart;
		int lastSprintEnd;
		int sprintStartMaxLength;
	};

	struct GlobalAmmo
	{
		uint64_t ammoType;
		int ammoCount;
		int pad;
	};

	struct ClipAmmo
	{
		uint64_t clipIndex;
		int ammoCount[2];
	};

	struct trajectory_t
	{
		int type;
		int time;
		int duration;
		vec3_t vBase;
		vec3_t vDelta;
	};

	enum clientstate_t
	{
		CS_FREE = 0x0,
		CS_ZOMBIE = 0x1,
		CS_RECONNECTING = 0x2,
		CS_CONNECTED = 0x3,
		CS_CLIENTLOADING = 0x4,
		CS_ACTIVE = 0x5,
	};

	enum netadrtype_t
	{
		NA_BOT = 0x0,
		NA_BAD = 0x1,
		NA_LOOPBACK = 0x2,
		NA_BROADCAST = 0x3,
		NA_IP = 0x4,
	};

	enum netsrc_t
	{
		NS_CLIENT1 = 0x0,
		NS_MAXCLIENTS = 0x1,
		NS_SERVER = 0x2,
		NS_PACKET = 0x3,
		NS_INVALID_NETSRC = 0x4,
	};

	struct netadr_s
	{
		netadrtype_t type;
		unsigned char ip[4];
		unsigned __int16 port;
		netsrc_t localNetID;
		unsigned int addrHandleIndex;
	};

	struct msg_t
	{
		int overflowed;
		int readOnly;
		char* data;
		char* splitData;
		int maxsize;
		int cursize;
		int splitSize;
		int readcount;
		int bit;
		int lastEntityRef;
		netsrc_t targetLocalNetID;
		int useZlib;
	};

	struct HsvColor
	{
		unsigned char h;
		unsigned char s;
		unsigned char v;
	};

	enum class he_type_t
	{
		HE_TYPE_FREE = 0x0,
		HE_TYPE_TEXT = 0x1,
		HE_TYPE_VALUE = 0x2,
		HE_TYPE_PLAYERNAME = 0x3,
		HE_TYPE_MATERIAL = 0x4,
		HE_TYPE_TIMER_DOWN = 0x5,
		HE_TYPE_TIMER_UP = 0x6,
		HE_TYPE_TIMER_STATIC = 0x7,
		HE_TYPE_TENTHS_TIMER_DOWN = 0x8,
		HE_TYPE_TENTHS_TIMER_UP = 0x9,
		HE_TYPE_TENTHS_TIMER_STATIC = 0xA,
		HE_TYPE_CLOCK_DOWN = 0xB,
		HE_TYPE_CLOCK_UP = 0xC,
		HE_TYPE_WAYPOINT = 0xD,
		HE_TYPE_COUNT = 0xE,
	};

	struct $C96EA5EC2ACBB9C0BF22693F316ACC67
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	union hudelem_color_t
	{
		$C96EA5EC2ACBB9C0BF22693F316ACC67 _s0;
		unsigned int rgba;
	};

	struct hudelem_s
	{
		he_type_t type;
		float x;
		float y;
		float z;
		int targetEntNum;
		float fontScale;
		float fromFontScale;
		int fontScaleStartTime;
		int fontScaleTime;
		int font;
		int alignOrg;
		int alignScreen;
		hudelem_color_t color;
		hudelem_color_t fromColor;
		int fadeStartTime;
		int fadeTime;
		int label;
		int width;
		int height;
		int materialIndex;
		int fromWidth;
		int fromHeight;
		int scaleStartTime;
		int scaleTime;
		float fromX;
		float fromY;
		int fromAlignOrg;
		int fromAlignScreen;
		int moveStartTime;
		int moveTime;
		int time;
		int duration;
		float value;
		int text;
		float sort;
		hudelem_color_t glowColor;
		int fxBirthTime;
		int fxLetterTime;
		int fxDecayStartTime;
		int fxDecayDuration;
		int soundID;
		int flags;
	};

	struct game_hudelem_s
	{
		hudelem_s elem;
		int clientNum;
		int team;
		int archived;
		int currentShowInKillcam;
	};

	enum
	{
		VAR_UNDEFINED = 0x0,
		VAR_BEGIN_REF = 0x1,
		VAR_POINTER = 0x1,
		VAR_STRING = 0x2,
		VAR_ISTRING = 0x3,
		VAR_VECTOR = 0x4,
		VAR_END_REF = 0x5,
		VAR_FLOAT = 0x5,
		VAR_INTEGER = 0x6,
		VAR_CODEPOS = 0x7,
		VAR_PRECODEPOS = 0x8,
		VAR_FUNCTION = 0x9,
		VAR_BUILTIN_FUNCTION = 0xA,
		VAR_BUILTIN_METHOD = 0xB,
		VAR_STACK = 0xC,
		VAR_ANIMATION = 0xD,
		VAR_PRE_ANIMATION = 0xE,
		VAR_THREAD = 0xF,
		VAR_NOTIFY_THREAD = 0x10,
		VAR_TIME_THREAD = 0x11,
		VAR_CHILD_THREAD = 0x12,
		VAR_OBJECT = 0x13,
		VAR_DEAD_ENTITY = 0x14,
		VAR_ENTITY = 0x15,
		VAR_ARRAY = 0x16,
		VAR_DEAD_THREAD = 0x17,
		VAR_COUNT = 0x18,
		VAR_FREE = 0x18,
		VAR_THREAD_LIST = 0x19,
		VAR_ENDON_LIST = 0x1A,
		VAR_TOTAL_COUNT = 0x1B,
	};

	struct VariableStackBuffer
	{
		const char* pos;
		unsigned __int16 size;
		unsigned __int16 bufLen;
		unsigned __int16 localId;
		char time;
		char buf[1];
	};

	union VariableUnion
	{
		int intValue;
		unsigned int uintValue;
		float floatValue;
		unsigned int stringValue;
		const float* vectorValue;
		const char* codePosValue;
		unsigned int pointerValue;
		VariableStackBuffer* stackValue;
		unsigned int entityOffset;
	};

	struct VariableValue
	{
		VariableUnion u;
		int type;
	};

	struct scr_entref_t
	{
		unsigned short entnum;
		unsigned short classnum;
	};

	typedef void(*BuiltinMethod)(scr_entref_t);
	typedef void(*BuiltinFunction)();

	struct function_stack_t
	{
		const char* pos;
		unsigned int localId;
		unsigned int localVarCount;
		VariableValue* top;
		VariableValue* startTop;
	};

	struct function_frame_t
	{
		function_stack_t fs;
		int topType;
	};

	struct scrVmPub_t
	{
		unsigned int* localVars;
		VariableValue* maxstack;
		int function_count;
		function_frame_t* function_frame;
		VariableValue* top;
		unsigned int inparamcount;
		unsigned int outparamcount;
		function_frame_t function_frame_start[32];
		VariableValue stack[2048];
	};

	struct ObjectVariableChildren
	{
		unsigned __int16 firstChild;
		unsigned __int16 lastChild;
	};

	struct ObjectVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ObjectVariableValue_u_o_u
	{
		unsigned __int16 size;
		unsigned __int16 entnum;
		unsigned __int16 nextEntId;
		unsigned __int16 self;
	};

	struct ObjectVariableValue_u_o
	{
		unsigned __int16 refCount;
		ObjectVariableValue_u_o_u u;
	};

	union ObjectVariableValue_w
	{
		unsigned int type;
		unsigned int classnum;
		unsigned int notifyName;
		unsigned int waitTime;
		unsigned int parentLocalId;
	};

	struct ChildVariableValue_u_f
	{
		unsigned __int16 prev;
		unsigned __int16 next;
	};

	union ChildVariableValue_u
	{
		ChildVariableValue_u_f f;
		VariableUnion u;
	};

	struct ChildBucketMatchKeys_keys
	{
		unsigned __int16 name_hi;
		unsigned __int16 parentId;
	};

	union ChildBucketMatchKeys
	{
		ChildBucketMatchKeys_keys keys;
		unsigned int match;
	};

	struct ChildVariableValue
	{
		ChildVariableValue_u u;
		unsigned __int16 next;
		char type;
		char name_lo;
		ChildBucketMatchKeys k;
		unsigned __int16 nextSibling;
		unsigned __int16 prevSibling;
	};

	union ObjectVariableValue_u
	{
		ObjectVariableValue_u_f f;
		ObjectVariableValue_u_o o;
	};

	struct ObjectVariableValue
	{
		ObjectVariableValue_u u;
		ObjectVariableValue_w w;
	};

	struct scrVarGlob_t
	{
		ObjectVariableValue objectVariableValue[36864];
		ObjectVariableChildren objectVariableChildren[36864];
		unsigned __int16 childVariableBucket[65536];
		ChildVariableValue childVariableValue[102400];
	};

	struct scr_classStruct_t
	{
		unsigned __int16 id;
		unsigned __int16 entArrayId;
		char charId;
		const char* name;
	};

	struct LuaFile
	{
		const char* name;
		int len;
		char strippingType;
		const char* buffer;
	};

	struct GfxImageLoadDef
	{
		char levelCount;
		char numElements;
		char pad[2];
		int flags;
		int format;
		int resourceSize;
		char data[1];
	};

	union $3FA29451CE6F1FA138A5ABAB84BE9676
	{
		ID3D11Texture1D* linemap;
		ID3D11Texture2D* map;
		ID3D11Texture3D* volmap;
		ID3D11Texture2D* cubemap;
		GfxImageLoadDef* loadDef;
	};

	struct GfxTexture
	{
		$3FA29451CE6F1FA138A5ABAB84BE9676 ___u0;
		ID3D11ShaderResourceView* shaderView;
		ID3D11ShaderResourceView* shaderViewAlternate;
	};

	struct Picmip
	{
		char platform[2];
	};

	struct CardMemory
	{
		int platform[2];
	};

	struct GfxImage
	{
		GfxTexture textures;
		int flags;
		int imageFormat;
		int resourceSize;
		char mapType;
		char semantic;
		char category;
		char flags2;
		Picmip picmip;
		char track;
		//CardMemory cardMemory;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned short numElements;
		char pad3[4];
		void* pixelData;
		//GfxImageLoadDef *loadDef;
		uint64_t streams[4];
		const char* name;
	};

	struct StreamFileNameRaw
	{
		const char* dir;
		const char* name;
	};

	struct StreamFileNamePacked
	{
		unsigned __int64 offset;
		unsigned __int64 length;
	};

	union StreamFileInfo
	{
		StreamFileNameRaw raw;
		StreamFileNamePacked packed;
	};

	struct StreamFileName
	{
		unsigned __int16 isLocalized;
		unsigned __int16 fileIndex;
		StreamFileInfo info;
	};

	struct StreamedSound
	{
		StreamFileName filename;
		unsigned int totalMsec;
	};

	struct ScriptFile
	{
		const char* name;
		int compressedLen;
		int len;
		int bytecodeLen;
		const char* buffer;
		unsigned char* bytecode;
	};

	enum weapType_t
	{
		WEAPTYPE_NONE = 0x0,
		WEAPTYPE_BULLET = 0x1,
		WEAPTYPE_GRENADE = 0x2,
		WEAPTYPE_PROJECTILE = 0x3,
		WEAPTYPE_RIOTSHIELD = 0x4,
		WEAPTYPE_NUM = 0x5,
	};

	enum weapClass_t
	{
		WEAPCLASS_RIFLE = 0x0,
		WEAPCLASS_SNIPER = 0x1,
		WEAPCLASS_MG = 0x2,
		WEAPCLASS_SMG = 0x3,
		WEAPCLASS_SPREAD = 0x4,
		WEAPCLASS_PISTOL = 0x5,
		WEAPCLASS_GRENADE = 0x6,
		WEAPCLASS_ROCKETLAUNCHER = 0x7,
		WEAPCLASS_TURRET = 0x8,
		WEAPCLASS_THROWINGKNIFE = 0x9,
		WEAPCLASS_NON_PLAYER = 0xA,
		WEAPCLASS_ITEM = 0xB,
		WEAPCLASS_NUM = 0xC,
	};

	enum PenetrateType
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,
		PENETRATE_TYPE_COUNT = 0x4,
	};

	enum weapInventoryType_t
	{
		WEAPINVENTORY_PRIMARY = 0x0,
		WEAPINVENTORY_OFFHAND = 0x1,
		WEAPINVENTORY_ITEM = 0x2,
		WEAPINVENTORY_ALTMODE = 0x3,
		WEAPINVENTORY_EXCLUSIVE = 0x4,
		WEAPINVENTORY_SCAVENGER = 0x5,
		WEAPINVENTORYCOUNT = 0x6,
	};

	struct WeaponDef
	{
		const char* szOverlayName;
		void** gunXModel;
		void* handXModel;
		void* camoWorldModel;
		void* camoViewModel;
		Material* camoWorldModelMaterialOverride;
		Material* camoViewModelMaterialOverride;
		Material** camoMaterialTarget;
		void** szXAnimsRightHanded;
		void** szXAnimsLeftHanded;
		const char* szModeName;
		unsigned int* notetrackSoundMapKeys;
		unsigned int* notetrackSoundMapValues;
		unsigned int* notetrackRumbleMapKeys;
		unsigned int* notetrackRumbleMapValues;
		unsigned int* notetrackFXMapKeys;
		const void** notetrackFXMapValues;
		unsigned int* notetrackFXMapTagValues;
		int playerAnimType;
		weapType_t weapType;
		weapClass_t weapClass;
		PenetrateType penetrateType;
		weapInventoryType_t inventoryType;
	}; // Incomplete

	struct RawFile
	{
		const char* name;
		int compressedLen;
		int len;
		const char* buffer;
	};

	union XAssetHeader
	{
		void* data;
		/*PhysPreset *physPreset;
		PhysCollmap *physCollmap;
		XAnimParts *parts;
		XModelSurfs *modelSurfs;
		XModel *model;*/
		Material* material;
		/*ComputeShader *computeShader;
		MaterialVertexShader *vertexShader;
		MaterialHullShader *hullShader;
		MaterialDomainShader *domainShader;
		MaterialPixelShader *pixelShader;
		MaterialVertexDeclaration *vertexDecl;
		MaterialTechniqueSet *techniqueSet;
		snd_alias_list_t *sound;
		SndCurve *sndCurve;
		SndCurve *lpfCurve;
		SndCurve *reverbCurve;
		LoadedSound *loadSnd;
		clipMap_t *clipMap;
		ComWorld *comWorld;
		GlassWorld *glassWorld;
		PathData *pathData;
		VehicleTrack *vehicleTrack;
		MapEnts *mapEnts;
		FxWorld *fxWorld;
		GfxWorld *gfxWorld;
		GfxLightDef *lightDef;*/
		Font_s* font;
		/*MenuList *menuList;
		menuDef_t *menu;
		AnimationClass *animClass;
		LocalizeEntry *localize;
		WeaponAttachment *attachment;
		WeaponCompleteDef *weapon;
		SndDriverGlobals *sndDriverGlobals;
		FxEffectDef *fx;
		FxImpactTable *impactFx;
		SurfaceFxTable *surfaceFx;*/
		RawFile* rawfile;
		ScriptFile* scriptfile;
		/*StringTable *stringTable;
		LeaderboardDef *leaderboardDef;
		StructuredDataDefSet *structuredDataDefSet;
		TracerDef *tracerDef;
		VehicleDef *vehDef;
		AddonMapEnts *addonMapEnts;
		NetConstStrings *netConstStrings;
		ReverbPreset *reverbPreset;*/
		LuaFile* luaFile;
		/*ScriptableDef *scriptable;
		Colorization *colorization;
		ColorizationSet *colorizationSet;
		ToneMapping *toneMapping;
		EquipmentSoundTable *equipSndTable;
		VectorField *vectorField;
		DopplerPreset *dopplerPreset;
		FxParticleSimAnimation *particleSimAnimation;*/
		GfxImage* image;
	};

	struct XZoneInfo
	{
		const char* name;
		int allocFlags;
		int freeFlags;
	};

	struct XZoneInfoInternal
	{
		char name[64];
		int flags;
		int isBaseMap;
	};

	struct XAsset
	{
		XAssetType type;
		XAssetHeader header;
	};

	struct XAssetEntry
	{
		XAsset asset;
		unsigned __int16 zoneIndex;
		volatile int inuseMask;
		unsigned int nextHash;
		unsigned int nextOverride;
		unsigned int nextPoolEntry;
	};

	struct weaponParms
	{
		float forward[3];
		float right[3];
		float up[3];
		float muzzleTrace[3];
		float gunForward[3];
		Weapon weapon;
		bool isAlternate;
		const WeaponDef* weapDef;
		const void* weapCompleteDef;
	};

	namespace sp
	{
		// very shit structures for the moment cuz i cba mapping the whole thing out right now...
		struct gclient_s
		{
			char __0x00[0xB6DC];
			int flags;
		};

		struct gentity_s
		{
			char __0x00[0x110];
			gclient_s* client;
			char __0x118[0x4C];
			int flags;
		};

		struct playerState_s
		{
		};

		struct XZone
		{
			char name[64];
			char gap2[24];
			XAssetEntry* entry;
			char gap4[121];
			char gap5;
		};
	}

	namespace mp
	{
#pragma pack(push, 4)
		struct playerState_s
		{
			int commandTime;
			int pm_type;
			int pm_time;
			int pm_flags;
			int otherFlags;
			int linkFlags;
			int bobCycle;
			vec3_t origin;
			vec3_t velocity;
			int grenadeTimeLeft;
			int throwbackGrenadeOwner;
			int throwbackGrenadeTimeLeft;
			int throwbackWeapon;
			int movingPlatformEntity;
			int remoteEyesEnt;
			int remoteEyesTagname;
			int remoteControlEnt;
			int remoteTurretEnt;
			int foliageSoundTime;
			int gravity;
			int speed;
			vec3_t delta_angles;
			int groundEntityNum;
			vec3_t vLadderVec;
			int jumpTime;
			float jumpOriginZ;
			int legsTimer;
			int legsAnim;
			int torsoTimer;
			int torsoAnim;
			int animMoveType;
			int damageTimer;
			int damageDuration;
			int flinch;
			int movementDir;
			int turnStartTime;
			int turnDirection;
			int turnRemaining;
			int corpseIndex;
			PlayerVehicleState vehicleState;
			int eFlags;
			playerEvents_t pe;
			int unpredictableEventSequence;
			int unpredictableEventSequenceOld;
			EntityEvent unpredictableEvents[4];
			int clientNum;
			int viewmodelIndex;
			vec3_t viewangles;
			char _0x190[0xAC];
			int locationSelectionInfo;
			SprintState_s sprintState;
			float holdBreathScale;
			int holdBreathTimer;
			float moveSpeedScaleMultiplier;
			float grenadeCookScale;
			MantleState mantleState;
			SlideState slideState;
			float leanf;
			PlayerActiveWeaponState weapState[2];
			int weaponsEquipped[15];
			PlayerEquippedWeaponState weapEquippedData[15];
			int offHand;
			int offhandPrimaryClass;
			int offhandSecondaryClass;
			int weapon;
			int weapFlags;
			float fWeaponPosFrac;
			float aimSpreadScale;
			int adsDelayTime;
			int spreadOverride;
			int spreadOverrideState;
			float fAimSpreadMovementScale;
			PlayerHandIndex lastWeaponHand;
			GlobalAmmo ammoNotInClip[15];
			ClipAmmo ammoInClip[15];
			int weapLockFlags;
			int weapLockedEntnum;
			vec3_t weapLockedPos;
			int weaponIdleTime;
			int lastStowedWeapon;
			char __0x638[0x2CE8];
		};
#pragma pack(pop)

		struct ClientCustomizationInfo
		{
			unsigned int modelIndex[3];
		};

		struct ScoreInfo
		{
			unsigned short ping;
			char status;
			char pad;
			unsigned short score;
			unsigned short kills;
			unsigned short deaths;
			unsigned short assists;
			unsigned short extrascore0;
			unsigned short extrascore1;
			unsigned short adrenaline;
		};

		union OmnvarValue
		{
			bool enabled;
			int integer;
			unsigned int time;
			float value;
			unsigned int ncsString;
		};

		struct OmnvarData
		{
			unsigned int timeModified;
			OmnvarValue current;
		};

		enum team_t
		{
			TEAM_FREE = 0x0,
			TEAM_BAD = 0x0,
			TEAM_AXIS = 0x1,
			TEAM_ALLIES = 0x2,
			TEAM_SPECTATOR = 0x3,
			TEAM_NUM_TEAMS = 0x4,
		};

		struct clientState_s
		{
			int clientIndex;
			team_t team;
			int modelindex;
			int dualWielding;
			char _0x10[0x30];
			char name[0x10];
			int rank;
			int prestige;
			char _0x00[0x10];
			int nameplateFriendlyIndex;
			int nameplateEnemyIndex;
			unsigned int perks[2];
			char _0x78[0x80];
		};

		struct clientSession_t
		{
			int sessionState;
			int forceSpectatorClient;
			int killCamEntity;
			int isInKillcam;
			char __0x332C[0x14];
			usercmd_s cmd;
			usercmd_s oldcmd;
			int localClient;
			char __0x33C4[0x24];
			float moveSpeedScaleMultiplier;
			int viewmodelIndex;
			int noSpectate;
			clientState_s cs;
			ScoreInfo scores;
			char __pad[0x1B0];
		};

		struct gclient_s
		{
			playerState_s ps;
			clientSession_t sess;
			int flags;
			int spectatorClient;
			char __0x36B4[0x3B0];
		};

		struct LerpEntityState
		{
			int entFlags;
			trajectory_t positionTrajectory;
			trajectory_t angleTrajectory;
			char _0x4C[0x10];
			int primaryWeapon;
			int secondaryWeapon;
			char _0x64[0x4];
		};

		struct EntityState
		{
			int number;
			int eType;
			LerpEntityState lerp;
			char _0x70[0x8];
			int otherEntityNum;
			int attackerEntityNum;
			int groundEntityNum;
			char _0x84[0xC];
			int clientNum;
			char _0x94[0x4];
			int solid;
			int eventParam;
			int eventSequence;
			EntityEvent entityEvent[4];
			int weapon;
			int inAltWeaponMode;
			char _0xCC[0x8];
			int eventParam2;
			char _0xD8[0x30];
		};

		struct EntityShared
		{
			char _0x108[0x38];
			vec3_t origin;
			vec3_t angles;
			char _0x14C[0xC];
		};

		struct gentity_s
		{
			EntityState s;
			EntityShared r;
			gclient_s* client;
			char _0x170[0x50];
			int flags;
			char _0x1C4[0x18];
			int health;
			int maxHealth;
			char _0x1E4[0x10C];
		};

		struct snapshot_s
		{
			char _0x0[0x3C78];
			int ping;
			int serverTime;
			int numEntities;
			int numClients;
		};

		struct cg_s
		{
			playerState_s ps;
			char _0x3324[0x64];
			snapshot_s* snap;
			snapshot_s* nextSnap; // theres alot after this cba rn
		};

		struct netProfilePacket_t
		{
			int iTime;
			int iSize;
			int bFragment;
		};

		struct netProfileStream_t
		{
			netProfilePacket_t packets[60];
			int iCurrPacket;
			int iBytesPerSecond;
			int iLastBPSCalcTime;
			int iCountedPackets;
			int iCountedFragments;
			int iFragmentPercentage;
			int iLargestPacket;
			int iSmallestPacket;
		};

		struct netProfileInfo_t
		{
			netProfileStream_t send;
			netProfileStream_t recieve;
		};

		struct __declspec(align(8)) netchan_t
		{
			int outgoingSequence;
			netsrc_t sock;
			int dropped;
			int incomingSequence;
			netadr_s remoteAddress;
			int fragmentSequence;
			int fragmentLength;
			char* fragmentBuffer;
			int fragmentBufferSize;
			int unsentFragments;
			int unsentFragmentStart;
			int unsentLength;
			char* unsentBuffer;
			int unsentBufferSize;
			netProfileInfo_t prof;
		};

		struct __declspec(align(8)) clientHeader_t
		{
			int state;
			int sendAsActive;
			int deltaMessage;
			int rateDelayed;
			int hasAckedBaselineData;
			int hugeSnapshotSent;
			netchan_t netchan;
			float predictedOrigin[3];
			int predictedOriginServerTime;
			int migrationState;
			unsigned int predictedVehicleSplineId;
			int predictedVehicleTargetEntity;
			float predictedVehicleOrigin[3];
			int predictedVehicleServerTime;
			int ackedMessage[32];
			unsigned int ackedMessageCount;
			int sentMessage[32];
			int wasKillcam[32];
			unsigned int sendMessageCount;
			bool overrideDeltaMessage;
			int overrideSequenceNumber;
			int sequenceResume;
			int isInKillcam;
		};

		struct client_t
		{
			clientHeader_t header;
			const char* dropReason;
			char userinfo[0x400];
			int reliableSequence;
			int reliableAcknowledge;
			int reliableSent;
			int messageAcknowledge;
			char _0xC30[0x41238];
			gentity_s* gentity;
			char name[16];
			int lastPacketTime;
			int lastConnectTime;
			int nextSnapshotTime;
			int timeoutCount;
			int ping;
			int minPing;
			int currFramePing;
			int snapshotBackoffCount;
			int rate;
			int pureAuthentic;
			unsigned int streamSyncWaitBits;
			unsigned int streamSyncWaitTimeout;
			LiveClientDropType liveDropRequest;
			char playerGuid[17];
			unsigned short scriptId;
			int bIsSplitscreenClient;
			TestClientType testClient;
			char _0x41E94[0x416A0];
		};

		struct XZone
		{
			char name[64];
			char gap2[24];
			XAssetEntry* entry;
			char gap4[399];
			char gap5;
		};

		struct missileFireParms
		{
			gentity_s* target;
			float targetPosOrOffset[3];
			float autoDetonateTime;
			bool lockon;
			bool topFire;
		};

		static_assert(sizeof(missileFireParms) == 0x20);
	}

	static_assert(sizeof(mp::client_t) == 0x83570);

	union playerState_s
	{
		sp::playerState_s* sp;
		mp::playerState_s* mp;
	};

	enum GfxDrawSceneMethod
	{
		GFX_DRAW_SCENE_STANDARD = 0x0,
	};

	enum MaterialTechniqueType
	{
		TECHNIQUE_DEPTH_PREPASS = 0x0,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x1,
		TECHNIQUE_UNLIT = 0x2,
		TECHNIQUE_EMISSIVE = 0x3,
		TECHNIQUE_EMISSIVE_DFOG = 0x4,
		TECHNIQUE_EMISSIVE_SHADOW = 0x5,
		TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x6,
		TECHNIQUE_LIT = 0x7,
		LIT_FIRST_ONE_LIGHT_TECHNIQUE = 0x8,
		TECHNIQUE_WIREFRAME_SOLID = 0x61,
		TECHNIQUE_WIREFRAME_SHADED = 0x62,
		TECHNIQUE_THERMAL = 0x63,
		TECHNIQUE_VELOCITY_RIGID = 0x64,
		TECHNIQUE_VELOCITY_SKINNED = 0x65,
		TECHNIQUE_DEBUG_BUMPMAP = 0x66,
		TECHNIQUE_NO_DISPLACEMENT_DEBUG_BUMPMAP = 0x134,
		TECHNIQUE_COUNT = 0x135,
		TECHNIQUE_TOTAL_COUNT = 0x136,
		TECHNIQUE_NONE = 0x137,
	};

	struct GfxDrawMethod_s
	{
		int drawScene;
		int baseTechType;
		int emissiveTechType;
		int forceTechType;
	};

	enum TraceHitType
	{
		TRACE_HITTYPE_NONE,
		TRACE_HITTYPE_ENTITY,
		TRACE_HITTYPE_DYNENT_MODEL,
		TRACE_HITTYPE_DYNENT_BRUSH,
		TRACE_HITTYPE_GLASS
	};

	struct trace_t
	{
		float fraction;
		float normal[3];
		int surfaceFlags;
		int contents;
		TraceHitType hitType;
		unsigned __int16 hitId;
		unsigned __int16 modelIndex;
		unsigned int partName;
		unsigned __int16 partGroup;
		bool allsolid;
		bool startsolid;
		bool walkable;
		bool getPenetration;
		bool removePitchAndRollRotations;
	};

	struct pmove_t
	{
		playerState_s* ps;
		usercmd_s cmd;
		usercmd_s oldcmd;
		int tracemask;
		int numtouch;
		int touchents[32];
		Bounds bounds;
		float speed;
		int contactEntity;
		int proneChange;
		bool mantleStarted;
		float mantleEndPos[3];
		int mantleDuration;
		float meleeEntOrigin[3];
		float meleeEntVelocity[3];
		int viewChangeTime;
		float viewChange;
		float fTorsoPitch;
		float fWaistPitch;
		int remoteTurretFireTime;
		int lastUpdateCMDServerTime;
		unsigned int groundSurfaceType;
		unsigned char handler;
	};

	struct WinVars_t
	{
		HINSTANCE reflib_library;
		int reflib_active;
		HWND hWnd;
		HINSTANCE hInstance;
		int activeApp;
		int isMinimized;
		int hasFocus;
		int activationStateChanged;
		int recenterMouse;
		HHOOK lowLevelKeyboardHook;
		unsigned int sysMsgTime;
	};

	static_assert(sizeof(WinVars_t) == 0x48);

	struct WinMouseVars_t
	{
		int oldButtonState;
		tagPOINT oldPos;
		bool mouseActive;
		bool mouseInitialized;
	};

	static_assert(sizeof(WinMouseVars_t) == 0x10);

	struct displayFreq_t
	{
		unsigned int numerator;
		unsigned int denominator;
	};

	struct vidConfig_t
	{
		unsigned int sceneWidth;
		unsigned int sceneHeight;
		unsigned int displayWidth;
		unsigned int displayHeight;
		char omitRTFlags;
		unsigned __int16 shadowTileResSmall;
		unsigned __int16 shadowTileResLarge;
		displayFreq_t displayFrequency;
		unsigned int monitorWidth;
		unsigned int monitorHeight;
		int displayMode;
		float windowAspectRatio;
		float displayAspectRatio;
		float sceneAspectRatio;
		float aspectRatioScenePixel;
		float aspectRatioDisplayPixel;
	};

	namespace hks
	{
		struct lua_State;
		struct HashTable;
		struct cclosure;

		struct GenericChunkHeader
		{
			unsigned __int64 m_flags;
		};

		struct ChunkHeader : GenericChunkHeader
		{
			ChunkHeader* m_next;
		};

		struct UserData : ChunkHeader
		{
			unsigned __int64 m_envAndSizeOffsetHighBits;
			unsigned __int64 m_metaAndSizeOffsetLowBits;
			char m_data[8];
		};

		struct InternString
		{
			unsigned __int64 m_flags;
			unsigned __int64 m_lengthbits;
			unsigned int m_hash;
			char m_data[30];
		};

		union HksValue
		{
			cclosure* cClosure;
			void* closure;
			UserData* userData;
			HashTable* table;
			void* tstruct;
			InternString* str;
			void* thread;
			void* ptr;
			float number;
			unsigned int native;
			bool boolean;
		};

		enum HksObjectType
		{
			TANY = 0xFFFFFFFE,
			TNONE = 0xFFFFFFFF,
			TNIL = 0x0,
			TBOOLEAN = 0x1,
			TLIGHTUSERDATA = 0x2,
			TNUMBER = 0x3,
			TSTRING = 0x4,
			TTABLE = 0x5,
			TFUNCTION = 0x6,  // idk
			TUSERDATA = 0x7,
			TTHREAD = 0x8,
			TIFUNCTION = 0x9, // Lua function
			TCFUNCTION = 0xA, // C function
			TUI64 = 0xB,
			TSTRUCT = 0xC,
			NUM_TYPE_OBJECTS = 0xE,
		};

		struct HksObject
		{
			HksObjectType t;
			HksValue v;
		};

		const struct hksInstruction
		{
			unsigned int code;
		};

		struct ActivationRecord
		{
			HksObject* m_base;
			const hksInstruction* m_returnAddress;
			__int16 m_tailCallDepth;
			__int16 m_numVarargs;
			int m_numExpectedReturns;
		};

		struct CallStack
		{
			ActivationRecord* m_records;
			ActivationRecord* m_lastrecord;
			ActivationRecord* m_current;
			const hksInstruction* m_current_lua_pc;
			const hksInstruction* m_hook_return_addr;
			int m_hook_level;
		};

		struct ApiStack
		{
			HksObject* top;
			HksObject* base;
			HksObject* alloc_top;
			HksObject* bottom;
		};

		struct UpValue : ChunkHeader
		{
			HksObject m_storage;
			HksObject* loc;
			UpValue* m_next;
		};

		struct CallSite
		{
			_SETJMP_FLOAT128 m_jumpBuffer[16];
			CallSite* m_prev;
		};

		enum Status
		{
			NEW = 0x1,
			RUNNING = 0x2,
			YIELDED = 0x3,
			DEAD_ERROR = 0x4,
		};

		enum HksError
		{
			HKS_NO_ERROR = 0,
			HKS_ERRSYNTAX = -4,
			HKS_ERRFILE = -5,
			HKS_ERRRUN = -100,
			HKS_ERRMEM = -200,
			HKS_ERRERR = -300,
			HKS_THROWING_ERROR = -500,
			HKS_GC_YIELD = 1,
		};

		struct lua_Debug
		{
			int event;
			const char* name;
			const char* namewhat;
			const char* what;
			const char* source;
			int currentline;
			int nups;
			int nparams;
			int ishksfunc;
			int linedefined;
			int lastlinedefined;
			char short_src[512];
			int callstack_level;
			int is_tail_call;
		};

		using lua_function = int(__fastcall*)(lua_State*);

		struct luaL_Reg
		{
			const char* name;
			lua_function function;
		};

		struct Node
		{
			HksObject m_key;
			HksObject m_value;
		};

		struct Metatable
		{
		};

		struct HashTable : ChunkHeader
		{
			Metatable* m_meta;
			unsigned int m_version;
			unsigned int m_mask;
			Node* m_hashPart;
			HksObject* m_arrayPart;
			unsigned int m_arraySize;
			Node* m_freeNode;
		};

		struct cclosure : ChunkHeader
		{
			lua_function m_function;
			HashTable* m_env;
			__int16 m_numUpvalues;
			__int16 m_flags;
			InternString* m_name;
			HksObject m_upvalues[1];
		};

		enum HksCompilerSettings_BytecodeSharingFormat
		{
			BYTECODE_DEFAULT = 0x0,
			BYTECODE_INPLACE = 0x1,
			BYTECODE_REFERENCED = 0x2,
		};

		enum HksCompilerSettings_IntLiteralOptions
		{
			INT_LITERALS_NONE = 0x0,
			INT_LITERALS_LUD = 0x1,
			INT_LITERALS_32BIT = 0x1,
			INT_LITERALS_UI64 = 0x2,
			INT_LITERALS_64BIT = 0x2,
			INT_LITERALS_ALL = 0x3,
		};

		struct HksCompilerSettings
		{
			int m_emitStructCode;
			const char** m_stripNames;
			int m_emitGlobalMemoization;
			int _m_isHksGlobalMemoTestingMode;
			HksCompilerSettings_BytecodeSharingFormat m_bytecodeSharingFormat;
			HksCompilerSettings_IntLiteralOptions m_enableIntLiterals;
			int(*m_debugMap)(const char*, int);
		};

		enum HksBytecodeSharingMode
		{
			HKS_BYTECODE_SHARING_OFF = 0x0,
			HKS_BYTECODE_SHARING_ON = 0x1,
			HKS_BYTECODE_SHARING_SECURE = 0x2,
		};

		struct HksGcWeights
		{
			int m_removeString;
			int m_finalizeUserdataNoMM;
			int m_finalizeUserdataGcMM;
			int m_cleanCoroutine;
			int m_removeWeak;
			int m_markObject;
			int m_traverseString;
			int m_traverseUserdata;
			int m_traverseCoroutine;
			int m_traverseWeakTable;
			int m_freeChunk;
			int m_sweepTraverse;
		};

		struct GarbageCollector_Stack
		{
			void* m_storage;
			unsigned int m_numEntries;
			unsigned int m_numAllocated;
		};

		struct ProtoList
		{
			void** m_protoList;
			unsigned __int16 m_protoSize;
			unsigned __int16 m_protoAllocSize;
		};

		struct GarbageCollector
		{
			int m_target;
			int m_stepsLeft;
			int m_stepLimit;
			HksGcWeights m_costs;
			int m_unit;
			_SETJMP_FLOAT128(*m_jumpPoint)[16];
			lua_State* m_mainState;
			lua_State* m_finalizerState;
			void* m_memory;
			int m_phase;
			GarbageCollector_Stack m_resumeStack;
			GarbageCollector_Stack m_greyStack;
			GarbageCollector_Stack m_remarkStack;
			GarbageCollector_Stack m_weakStack;
			int m_finalizing;
			HksObject m_safeTableValue;
			lua_State* m_startOfStateStackList;
			lua_State* m_endOfStateStackList;
			lua_State* m_currentState;
			HksObject m_safeValue;
			void* m_compiler;
			void* m_bytecodeReader;
			void* m_bytecodeWriter;
			int m_pauseMultiplier;
			int m_stepMultiplier;
			bool m_stopped;
			int(*m_gcPolicy)(lua_State*);
			unsigned __int64 m_pauseTriggerMemoryUsage;
			int m_stepTriggerCountdown;
			unsigned int m_stringTableIndex;
			unsigned int m_stringTableSize;
			UserData* m_lastBlackUD;
			UserData* m_activeUD;
		};

		enum MemoryManager_ChunkColor
		{
			RED = 0x0,
			BLACK = 0x1,
		};

		struct ChunkList
		{
			ChunkHeader m_prevToStart;
		};

		enum Hks_DeleteCheckingMode
		{
			HKS_DELETE_CHECKING_OFF = 0x0,
			HKS_DELETE_CHECKING_ACCURATE = 0x1,
			HKS_DELETE_CHECKING_SAFE = 0x2,
		};

		struct MemoryManager
		{
			void* (*m_allocator)(void*, void*, unsigned __int64, unsigned __int64);
			void* m_allocatorUd;
			MemoryManager_ChunkColor m_chunkColor;
			unsigned __int64 m_used;
			unsigned __int64 m_highwatermark;
			ChunkList m_allocationList;
			ChunkList m_sweepList;
			ChunkHeader* m_lastKeptChunk;
			lua_State* m_state;
			ChunkList m_deletedList;
			int m_deleteMode;
			Hks_DeleteCheckingMode m_deleteCheckingMode;
		};

		struct StaticStringCache
		{
			HksObject m_objects[41];
		};

		enum HksBytecodeEndianness
		{
			HKS_BYTECODE_DEFAULT_ENDIAN = 0x0,
			HKS_BYTECODE_BIG_ENDIAN = 0x1,
			HKS_BYTECODE_LITTLE_ENDIAN = 0x2,
		};

		struct RuntimeProfileData_Stats
		{
			unsigned __int64 hksTime;
			unsigned __int64 callbackTime;
			unsigned __int64 gcTime;
			unsigned __int64 cFinalizerTime;
			unsigned __int64 compilerTime;
			unsigned int hkssTimeSamples;
			unsigned int callbackTimeSamples;
			unsigned int gcTimeSamples;
			unsigned int compilerTimeSamples;
			unsigned int num_newuserdata;
			unsigned int num_tablerehash;
			unsigned int num_pushstring;
			unsigned int num_pushcfunction;
			unsigned int num_newtables;
		};

		struct RuntimeProfileData
		{
			__int64 stackDepth;
			__int64 callbackDepth;
			unsigned __int64 lastTimer;
			RuntimeProfileData_Stats frameStats;
			unsigned __int64 gcStartTime;
			unsigned __int64 finalizerStartTime;
			unsigned __int64 compilerStartTime;
			unsigned __int64 compilerStartGCTime;
			unsigned __int64 compilerStartGCFinalizerTime;
			unsigned __int64 compilerCallbackStartTime;
			__int64 compilerDepth;
			void* outFile;
			lua_State* rootState;
		};

		struct HksGlobal
		{
			MemoryManager m_memory;
			GarbageCollector m_collector;
			StringTable m_stringTable;
			HksBytecodeSharingMode m_bytecodeSharingMode;
			unsigned int m_tableVersionInitializer;
			HksObject m_registry;
			ProtoList m_protoList;
			HashTable* m_structProtoByName;
			ChunkList m_userDataList;
			lua_State* m_root;
			StaticStringCache m_staticStringCache;
			void* m_debugger;
			void* m_profiler;
			RuntimeProfileData m_runProfilerData;
			HksCompilerSettings m_compilerSettings;
			int(*m_panicFunction)(lua_State*);
			void* m_luaplusObjectList;
			int m_heapAssertionFrequency;
			int m_heapAssertionCount;
			void (*m_logFunction)(lua_State*, const char*, ...);
			HksBytecodeEndianness m_bytecodeDumpEndianness;
		};

		struct lua_State : ChunkHeader
		{
			HksGlobal* m_global;
			CallStack m_callStack;
			ApiStack m_apistack;
			UpValue* pending;
			HksObject globals;
			HksObject m_cEnv;
			CallSite* m_callsites;
			int m_numberOfCCalls;
			void* m_context;
			InternString* m_name;
			lua_State* m_nextState;
			lua_State* m_nextStateStack;
			Status m_status;
			HksError m_error;
		};
	}
}
