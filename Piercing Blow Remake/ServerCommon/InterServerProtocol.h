#ifndef __INTERSERVERPROTOCOL_H__
#define __INTERSERVERPROTOCOL_H__

#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inter-Server Protocol
// Rango 0x7000+ para evitar conflicto con protocolos de cliente (ProtocolDef.h usa 0x0100-0x1C00)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum Protocol_InterServer_Base
{
	PROTOCOL_IS_BASE = 0x7000,

	//----------------------------------------------------------
	// Heartbeat (todos los servidores)
	PROTOCOL_IS_HEARTBEAT_REQ,
	PROTOCOL_IS_HEARTBEAT_ACK,
};

enum Protocol_InterServer_Connect
{
	PROTOCOL_IS_CONNECT = 0x7100,

	//----------------------------------------------------------
	// ConnectServer <-> GameServer: Registro de servidor
	PROTOCOL_IS_SERVER_REGISTER_REQ,			// GameServer se registra con ConnectServer
	PROTOCOL_IS_SERVER_REGISTER_ACK,			// ConnectServer confirma registro

	// ConnectServer <-> GameServer: Estado del servidor
	PROTOCOL_IS_SERVER_STATUS_UPDATE_REQ,		// GameServer reporta su estado (players, state)
	PROTOCOL_IS_SERVER_STATUS_UPDATE_ACK,

	// ConnectServer -> GameServer: Transferencia de autenticacion
	PROTOCOL_IS_PLAYER_AUTH_TRANSFER_REQ,		// ConnectServer notifica token de jugador entrante
	PROTOCOL_IS_PLAYER_AUTH_TRANSFER_ACK,		// GameServer confirma recepcion del token

	// ConnectServer <-> GameServer: Server list
	PROTOCOL_IS_SERVER_LIST_REQ,
	PROTOCOL_IS_SERVER_LIST_ACK,
};

enum Protocol_InterServer_Data
{
	PROTOCOL_IS_DATA = 0x7200,

	//----------------------------------------------------------
	// Auth operations (ConnectServer -> DataServer)
	PROTOCOL_IS_AUTH_LOGIN_REQ,					// Validar credenciales
	PROTOCOL_IS_AUTH_LOGIN_ACK,

	// Player data operations (GameServer <-> DataServer)
	PROTOCOL_IS_PLAYER_LOAD_REQ,				// Cargar datos de jugador
	PROTOCOL_IS_PLAYER_LOAD_ACK,

	PROTOCOL_IS_PLAYER_SAVE_REQ,				// Guardar datos de jugador
	PROTOCOL_IS_PLAYER_SAVE_ACK,

	PROTOCOL_IS_PLAYER_CREATE_NICK_REQ,			// Crear nickname
	PROTOCOL_IS_PLAYER_CREATE_NICK_ACK,

	PROTOCOL_IS_PLAYER_CHECK_NICK_REQ,			// Verificar nickname
	PROTOCOL_IS_PLAYER_CHECK_NICK_ACK,

	// Shop operations (GameServer -> DataServer)
	PROTOCOL_IS_SHOP_BUY_REQ,
	PROTOCOL_IS_SHOP_BUY_ACK,

	PROTOCOL_IS_INVEN_UPDATE_REQ,
	PROTOCOL_IS_INVEN_UPDATE_ACK,

	// Stats operations (GameServer/BattleServer -> DataServer)
	PROTOCOL_IS_STATS_SAVE_REQ,
	PROTOCOL_IS_STATS_SAVE_ACK,

	// Equipment operations (GameServer <-> DataServer)
	PROTOCOL_IS_EQUIPMENT_SAVE_REQ,
	PROTOCOL_IS_EQUIPMENT_SAVE_ACK,

	// Medal operations (GameServer <-> DataServer)
	PROTOCOL_IS_MEDAL_SAVE_REQ,
	PROTOCOL_IS_MEDAL_SAVE_ACK,

	// Attendance operations (GameServer <-> DataServer)
	PROTOCOL_IS_ATTENDANCE_SAVE_REQ,
	PROTOCOL_IS_ATTENDANCE_SAVE_ACK,

	// Skill operations (GameServer <-> DataServer)
	PROTOCOL_IS_SKILL_SAVE_REQ,
	PROTOCOL_IS_SKILL_SAVE_ACK,

	// Quest operations (GameServer <-> DataServer)
	PROTOCOL_IS_QUEST_SAVE_REQ,
	PROTOCOL_IS_QUEST_SAVE_ACK,

	// Clan operations (GameServer <-> DataServer)
	PROTOCOL_IS_CLAN_CREATE_REQ,
	PROTOCOL_IS_CLAN_CREATE_ACK,
	PROTOCOL_IS_CLAN_DISBAND_REQ,
	PROTOCOL_IS_CLAN_DISBAND_ACK,
	PROTOCOL_IS_CLAN_JOIN_REQ,
	PROTOCOL_IS_CLAN_JOIN_ACK,
	PROTOCOL_IS_CLAN_LEAVE_REQ,
	PROTOCOL_IS_CLAN_LEAVE_ACK,
	PROTOCOL_IS_CLAN_LOAD_REQ,
	PROTOCOL_IS_CLAN_LOAD_ACK,

	// Friend operations (GameServer <-> DataServer)
	PROTOCOL_IS_FRIEND_ADD_REQ,
	PROTOCOL_IS_FRIEND_ADD_ACK,
	PROTOCOL_IS_FRIEND_REMOVE_REQ,
	PROTOCOL_IS_FRIEND_REMOVE_ACK,
	PROTOCOL_IS_FRIEND_LIST_REQ,
	PROTOCOL_IS_FRIEND_LIST_ACK,

	// Block operations (GameServer <-> DataServer)
	PROTOCOL_IS_BLOCK_ADD_REQ,
	PROTOCOL_IS_BLOCK_ADD_ACK,
	PROTOCOL_IS_BLOCK_REMOVE_REQ,
	PROTOCOL_IS_BLOCK_REMOVE_ACK,
	PROTOCOL_IS_BLOCK_LIST_REQ,
	PROTOCOL_IS_BLOCK_LIST_ACK,

	// Shop catalog (GameServer -> DataServer)
	PROTOCOL_IS_SHOP_LIST_REQ,
	PROTOCOL_IS_SHOP_LIST_ACK,

	// Note/Mail operations (GameServer <-> DataServer)
	PROTOCOL_IS_NOTE_SEND_REQ,
	PROTOCOL_IS_NOTE_SEND_ACK,
	PROTOCOL_IS_NOTE_LIST_REQ,
	PROTOCOL_IS_NOTE_LIST_ACK,
	PROTOCOL_IS_NOTE_DELETE_REQ,
	PROTOCOL_IS_NOTE_DELETE_ACK,

	// Ban operations (GameServer -> DataServer)
	PROTOCOL_IS_PLAYER_BAN_REQ,
	PROTOCOL_IS_PLAYER_BAN_ACK,

	// Account Cosmetics (GameServer <-> DataServer)
	PROTOCOL_IS_COSMETICS_LOAD_REQ,
	PROTOCOL_IS_COSMETICS_LOAD_ACK,
	PROTOCOL_IS_COSMETICS_SAVE_REQ,
	PROTOCOL_IS_COSMETICS_SAVE_ACK,

	// Mode Records (GameServer <-> DataServer)
	PROTOCOL_IS_MODE_RECORDS_LOAD_REQ,
	PROTOCOL_IS_MODE_RECORDS_LOAD_ACK,
	PROTOCOL_IS_MODE_RECORDS_SAVE_REQ,
	PROTOCOL_IS_MODE_RECORDS_SAVE_ACK,

	// Daily Records (GameServer <-> DataServer)
	PROTOCOL_IS_DAILY_RECORD_LOAD_REQ,
	PROTOCOL_IS_DAILY_RECORD_LOAD_ACK,
	PROTOCOL_IS_DAILY_RECORD_SAVE_REQ,
	PROTOCOL_IS_DAILY_RECORD_SAVE_ACK,

	// Map System (GameServer <-> DataServer)
	PROTOCOL_IS_MAP_LIST_REQ,
	PROTOCOL_IS_MAP_LIST_ACK,
	PROTOCOL_IS_MAP_STATS_SAVE_REQ,
	PROTOCOL_IS_MAP_STATS_SAVE_ACK,

	// Rankings (GameServer <-> DataServer)
	PROTOCOL_IS_RANKING_INDIVIDUAL_REQ,
	PROTOCOL_IS_RANKING_INDIVIDUAL_ACK,
	PROTOCOL_IS_RANKING_CLAN_REQ,
	PROTOCOL_IS_RANKING_CLAN_ACK,
	PROTOCOL_IS_RANKING_CALCULATE_REQ,
	PROTOCOL_IS_RANKING_CALCULATE_ACK,

	// Medal Sets (GameServer <-> DataServer)
	PROTOCOL_IS_MEDAL_SET_LOAD_REQ,
	PROTOCOL_IS_MEDAL_SET_LOAD_ACK,
	PROTOCOL_IS_MEDAL_SET_SAVE_REQ,
	PROTOCOL_IS_MEDAL_SET_SAVE_ACK,

	// Server Config (GameServer <-> DataServer)
	PROTOCOL_IS_SERVER_CONFIG_REQ,
	PROTOCOL_IS_SERVER_CONFIG_ACK,
	PROTOCOL_IS_NOTICES_REQ,
	PROTOCOL_IS_NOTICES_ACK,
	PROTOCOL_IS_BOOST_EVENTS_REQ,
	PROTOCOL_IS_BOOST_EVENTS_ACK,

	// Gift Delivery (GameServer <-> DataServer)
	PROTOCOL_IS_GIFT_SEND_REQ,
	PROTOCOL_IS_GIFT_SEND_ACK,
	PROTOCOL_IS_GIFT_LIST_REQ,
	PROTOCOL_IS_GIFT_LIST_ACK,
	PROTOCOL_IS_GIFT_RECEIVE_REQ,
	PROTOCOL_IS_GIFT_RECEIVE_ACK,

	// Clan Extended (GameServer <-> DataServer)
	PROTOCOL_IS_CLAN_REQUEST_SEND_REQ,
	PROTOCOL_IS_CLAN_REQUEST_SEND_ACK,
	PROTOCOL_IS_CLAN_REQUEST_LIST_REQ,
	PROTOCOL_IS_CLAN_REQUEST_LIST_ACK,
	PROTOCOL_IS_CLAN_REQUEST_RESPOND_REQ,
	PROTOCOL_IS_CLAN_REQUEST_RESPOND_ACK,
	PROTOCOL_IS_CLAN_ITEMS_REQ,
	PROTOCOL_IS_CLAN_ITEMS_ACK,
	PROTOCOL_IS_CLAN_SEASON_REQ,
	PROTOCOL_IS_CLAN_SEASON_ACK,
	PROTOCOL_IS_CLAN_MATCH_RESULT_REQ,
	PROTOCOL_IS_CLAN_MATCH_RESULT_ACK,

	// Mercenary (GameServer <-> DataServer)
	PROTOCOL_IS_MERCENARY_HIRE_REQ,
	PROTOCOL_IS_MERCENARY_HIRE_ACK,
	PROTOCOL_IS_MERCENARY_LIST_REQ,
	PROTOCOL_IS_MERCENARY_LIST_ACK,
	PROTOCOL_IS_MERCENARY_DISMISS_REQ,
	PROTOCOL_IS_MERCENARY_DISMISS_ACK,
	PROTOCOL_IS_MERCENARY_RESULT_REQ,
	PROTOCOL_IS_MERCENARY_RESULT_ACK,

	// Shop Extended (GameServer <-> DataServer)
	PROTOCOL_IS_SHOP_COUPON_REQ,
	PROTOCOL_IS_SHOP_COUPON_ACK,
};

enum Protocol_InterServer_Battle
{
	PROTOCOL_IS_BATTLE = 0x7300,

	//----------------------------------------------------------
	// GameServer <-> BattleServer
	PROTOCOL_IS_BATTLE_REGISTER_REQ,			// BattleServer se registra con GameServer
	PROTOCOL_IS_BATTLE_REGISTER_ACK,

	PROTOCOL_IS_BATTLE_CREATE_REQ,				// Crear batalla
	PROTOCOL_IS_BATTLE_CREATE_ACK,

	PROTOCOL_IS_BATTLE_END_NOTIFY,				// Notificar fin de batalla con resultados

	PROTOCOL_IS_PLAYER_MIGRATE_REQ,				// Migrar jugador a BattleServer
	PROTOCOL_IS_PLAYER_MIGRATE_ACK,

	PROTOCOL_IS_BATTLE_STATUS_UPDATE,			// Estado de batalla en progreso

	//----------------------------------------------------------
	// Battle live events (BattleServer -> GameServer)
	// Reemplaza ModuleCast del original - comunicacion directa sin CastServer
	PROTOCOL_IS_BATTLE_KILL_NOTIFY,				// Kill event durante la batalla
	PROTOCOL_IS_BATTLE_ROUND_START_NOTIFY,		// Inicio de ronda
	PROTOCOL_IS_BATTLE_ROUND_END_NOTIFY,		// Fin de ronda (con equipo ganador)
	PROTOCOL_IS_BATTLE_HACK_NOTIFY,				// Hack detectado
	PROTOCOL_IS_BATTLE_MISSION_NOTIFY,			// Evento de mision (bomba, destruccion, defensa)
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inter-Server Packet Structures
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

// Heartbeat
struct IS_HEARTBEAT_REQ
{
	int			i32ServerId;
	int			i32ServerType;			// ServerType enum
	int			i32CurrentPlayers;
};

struct IS_HEARTBEAT_ACK
{
	int			i32Result;				// 0 = OK
};

// Server Registration
struct IS_SERVER_REGISTER_REQ
{
	int			i32ServerId;
	int			i32ServerType;			// ServerType enum
	char		szServerName[MAX_SERVER_NAME_LENGTH];
	char		szPublicIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16PublicPort;
	int			i32MaxPlayers;
	char		szRegion[MAX_SERVER_REGION_LENGTH];
};

struct IS_SERVER_REGISTER_ACK
{
	int			i32Result;				// 0 = OK, 1 = Rejected
	int			i32AssignedId;			// ID asignado por ConnectServer
};

// Server Status Update
struct IS_SERVER_STATUS_UPDATE_REQ
{
	int			i32ServerId;
	int			i32State;				// ServerOnlineState enum
	int			i32CurrentPlayers;
};

// Auth Transfer (ConnectServer -> GameServer)
struct IS_PLAYER_AUTH_TRANSFER_REQ
{
	int64_t		i64UID;					// User database ID
	uint32_t	ui32AuthToken;			// Token de autenticacion temporal
	char		szUsername[64];
	uint32_t	ui32ClientIP;			// IP del cliente para verificacion
};

struct IS_PLAYER_AUTH_TRANSFER_ACK
{
	int64_t		i64UID;
	int			i32Result;				// 0 = Accepted, 1 = Rejected
};

// Auth Login (ConnectServer -> DataServer)
struct IS_AUTH_LOGIN_REQ
{
	int			i32SessionIdx;			// Session index en ConnectServer
	char		szUsername[64];
	char		szPassword[64];
	uint32_t	ui32ClientIP;
};

struct IS_AUTH_LOGIN_ACK
{
	int			i32SessionIdx;
	int			i32Result;				// 0 = OK, error codes
	int64_t		i64UID;					// User ID if successful
	uint8_t		ui8AuthLevel;			// Authority level
};

// Player Data Load (GameServer -> DataServer)
struct IS_PLAYER_LOAD_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;			// Session index en GameServer
};

struct IS_PLAYER_LOAD_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;				// 0 = OK
	// El resto de los datos del jugador se envia como payload adicional
};

// Player Save (GameServer -> DataServer)
struct IS_PLAYER_SAVE_REQ
{
	int64_t		i64UID;
	int			i32Level;
	int64_t		i64Exp;
	int			i32Cash;
	int			i32GP;
};

struct IS_PLAYER_SAVE_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;				// 0 = OK
};

// Nick Create (GameServer -> DataServer)
struct IS_PLAYER_CREATE_NICK_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
	char		szNickname[64];
};

struct IS_PLAYER_CREATE_NICK_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;				// 0=OK, 1=exists, 2=invalid
	char		szNickname[64];			// Nickname created (echoed back)
};

// Nick Check (GameServer -> DataServer)
struct IS_PLAYER_CHECK_NICK_REQ
{
	int			i32SessionIdx;
	char		szNickname[64];
};

struct IS_PLAYER_CHECK_NICK_ACK
{
	int			i32SessionIdx;
	int			i32Result;				// 0=available, 1=taken
};

// Stats Save (GameServer/BattleServer -> DataServer)
struct IS_STATS_SAVE_REQ
{
	int64_t		i64UID;
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32Wins;
	int			i32Losses;
};

struct IS_STATS_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;				// 0 = OK
};

// Player data payload para PLAYER_LOAD_ACK
struct IS_PLAYER_LOAD_DATA
{
	int64_t		i64UID;
	char		szNickname[64];
	int			i32Level;
	int64_t		i64Exp;
	int			i32Cash;
	int			i32GP;
	int			i32Coin;
	int			i32RankId;
	int			i32KeepRank;
	int			i32ClanId;
	uint8_t		ui8AuthLevel;
	// Stats
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32Wins;
	int			i32Losses;
	int			i32Draws;
	int			i32Disconnects;
	// Extended user info
	int			i32ConnectCount;
	int			i32ConnectTime;
	int			i32TotalBattleTime;
	uint8_t		ui8TutorialDone;
	uint64_t	ui64GuideComplete;
	// Cosmetics
	uint8_t		ui8NicknameColor;
	uint8_t		ui8CrosshairColor;
	uint8_t		ui8ChattingColor;
	int			i32DisguiseRank;
	// Inventory count (IS_PLAYER_INVENTORY_ITEM items follow after this struct)
	int			i32InventoryCount;
};

struct IS_PLAYER_INVENTORY_ITEM
{
	int			i32ItemId;
	int			i32ItemCount;
	int			i32SlotIdx;
	uint8_t		ui8IsEquipped;
};

// Equipment Save (GameServer -> DataServer)
struct IS_EQUIPMENT_SAVE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8CharaSlot;			// 0-4
	uint8_t		ui8SlotState;			// MultiSlotState
	uint32_t	ui32CharaId;
	uint32_t	ui32WeaponIds[5];		// CHAR_EQUIP_WEAPON_COUNT
	uint32_t	ui32PartsIds[10];		// CHAR_EQUIP_PARTS_COUNT
};

struct IS_EQUIPMENT_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// Medal Save (GameServer -> DataServer)
struct IS_MEDAL_SAVE_REQ
{
	int64_t		i64UID;
	uint16_t	ui16MedalIdx;
	uint8_t		ui8ActionId;
	uint16_t	ui16Count;
	uint8_t		ui8GetReward;			// Bitmask
};

struct IS_MEDAL_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// Attendance Save (GameServer -> DataServer)
struct IS_ATTENDANCE_SAVE_REQ
{
	int64_t		i64UID;
	int			i32TotalDays;
	int			i32CurrentStreak;
	uint32_t	ui32LastAttendDate;		// YYYYMMDD
	uint8_t		ui8Days[30];			// MAX_ATTENDANCE_DAYS
};

struct IS_ATTENDANCE_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// Skill Save (GameServer -> DataServer)
struct IS_SKILL_SAVE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8ClassId;				// GameCharaClass (0-4)
	uint8_t		ui8MainLevels[5];		// MAX_MAIN_SKILLS
	uint8_t		ui8AssistLevels[4];		// MAX_ASSIST_SKILLS
	uint8_t		ui8CommonLevels[5];		// MAX_COMMON_SKILLS
	uint16_t	ui16SkillPoints;
};

struct IS_SKILL_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// Quest Save (GameServer -> DataServer)
struct IS_QUEST_SAVE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8SetIndex;			// 0-3
	uint8_t		ui8SetType;				// GameQuestCardSetType
	uint8_t		ui8ActiveCard;			// Active card (0-9)
	uint16_t	ui16DataSize;			// Size of quest_data payload following
	// Followed by quest_data bytes (serialized quest progress)
};

struct IS_QUEST_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// Clan Create (GameServer -> DataServer)
struct IS_CLAN_CREATE_REQ
{
	int64_t		i64MasterUID;
	int			i32SessionIdx;
	char		szClanName[32];
	char		szMasterNickname[64];
};

struct IS_CLAN_CREATE_ACK
{
	int64_t		i64MasterUID;
	int			i32SessionIdx;
	int			i32ClanId;				// Assigned ID, 0 = error
	int			i32Result;				// 0=OK, 1=name_taken, 2=error
};

// Clan Disband (GameServer -> DataServer)
struct IS_CLAN_DISBAND_REQ
{
	int			i32ClanId;
	int64_t		i64MasterUID;
};

struct IS_CLAN_DISBAND_ACK
{
	int			i32ClanId;
	int			i32Result;
};

// Clan Join (GameServer -> DataServer)
struct IS_CLAN_JOIN_REQ
{
	int			i32ClanId;
	int64_t		i64UID;
	int			i32SessionIdx;
	char		szNickname[64];
	uint8_t		ui8MemberLevel;			// GameClanMemberLevel
};

struct IS_CLAN_JOIN_ACK
{
	int			i32ClanId;
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;
};

// Clan Leave (GameServer -> DataServer)
struct IS_CLAN_LEAVE_REQ
{
	int			i32ClanId;
	int64_t		i64UID;
};

struct IS_CLAN_LEAVE_ACK
{
	int			i32ClanId;
	int64_t		i64UID;
	int			i32Result;
};

// Clan Load (DataServer -> GameServer on startup/request)
struct IS_CLAN_LOAD_REQ
{
	int			i32ClanId;
};

struct IS_CLAN_LOAD_ACK
{
	int			i32ClanId;
	int			i32Result;
	char		szName[32];
	char		szNotice[256];
	char		szIntro[128];
	int64_t		i64MasterUID;
	char		szMasterNickname[64];
	int			i32MemberCount;
	int			i32MaxMembers;
	int			i32ClanExp;
	int			i32ClanRank;
	int			i32Wins;
	int			i32Losses;
	uint8_t		ui8Unit;
	uint16_t	ui16MarkId;
	uint8_t		ui8MarkColor;
	// Followed by i32MemberCount * IS_CLAN_MEMBER_INFO
};

struct IS_CLAN_MEMBER_INFO
{
	int64_t		i64UID;
	char		szNickname[64];
	uint8_t		ui8MemberLevel;
};

// Friend Add (GameServer -> DataServer)
struct IS_FRIEND_ADD_REQ
{
	int64_t		i64UID;
	int64_t		i64FriendUID;
	int			i32SessionIdx;
};

struct IS_FRIEND_ADD_ACK
{
	int64_t		i64UID;
	int64_t		i64FriendUID;
	int			i32SessionIdx;
	int			i32Result;				// 0=OK, 1=already_friends, 2=not_found
};

// Friend Remove
struct IS_FRIEND_REMOVE_REQ
{
	int64_t		i64UID;
	int64_t		i64FriendUID;
};

struct IS_FRIEND_REMOVE_ACK
{
	int64_t		i64UID;
	int64_t		i64FriendUID;
	int			i32Result;
};

// Friend List
struct IS_FRIEND_LIST_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
};

struct IS_FRIEND_LIST_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Count;
	// Followed by i32Count * IS_FRIEND_ENTRY
};

struct IS_FRIEND_ENTRY
{
	int64_t		i64FriendUID;
	char		szNickname[64];
	int			i32Level;
	bool		bAccepted;
};

// Block Add
struct IS_BLOCK_ADD_REQ
{
	int64_t		i64UID;
	int64_t		i64BlockedUID;
	int			i32SessionIdx;
};

struct IS_BLOCK_ADD_ACK
{
	int64_t		i64UID;
	int64_t		i64BlockedUID;
	int			i32SessionIdx;
	int			i32Result;
};

// Block Remove
struct IS_BLOCK_REMOVE_REQ
{
	int64_t		i64UID;
	int64_t		i64BlockedUID;
};

struct IS_BLOCK_REMOVE_ACK
{
	int64_t		i64UID;
	int64_t		i64BlockedUID;
	int			i32Result;
};

// Block List
struct IS_BLOCK_LIST_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
};

struct IS_BLOCK_LIST_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Count;
	// Followed by i32Count * IS_BLOCK_ENTRY
};

struct IS_BLOCK_ENTRY
{
	int64_t		i64BlockedUID;
	char		szNickname[64];
};

// Shop List (GameServer -> DataServer)
struct IS_SHOP_LIST_REQ
{
	int			i32SessionIdx;			// 0 = server startup request
};

struct IS_SHOP_LIST_ACK
{
	int			i32Result;
	int			i32ItemCount;
	// Followed by i32ItemCount * IS_SHOP_ITEM_ENTRY
};

struct IS_SHOP_ITEM_ENTRY
{
	uint32_t	ui32GoodsId;
	uint32_t	ui32ItemId;
	uint8_t		ui8ItemType;
	int			i32PriceGP;
	int			i32PriceCash;
	uint32_t	ui32Duration;
	uint8_t		ui8Category;
};

// Shop Buy (GameServer -> DataServer)
struct IS_SHOP_BUY_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
	uint32_t	ui32GoodsId;
	uint32_t	ui32ItemId;
	uint8_t		ui8PayType;				// 0=GP, 1=Cash
	int			i32Price;
};

struct IS_SHOP_BUY_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;				// 0=OK, 1=insufficient_funds, 2=item_not_found, 3=error
	uint32_t	ui32ItemId;
	int			i32RemainingGP;
	int			i32RemainingCash;
};

// Inventory Update (GameServer -> DataServer)
struct IS_INVEN_UPDATE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8Operation;			// 0=add, 1=remove, 2=update_count
	uint32_t	ui32ItemId;
	int			i32ItemCount;
	int			i32SlotIdx;
	uint8_t		ui8IsEquipped;
};

struct IS_INVEN_UPDATE_ACK
{
	int64_t		i64UID;
	int			i32Result;				// 0=OK, 1=inventory_full, 2=item_not_found, 3=error
	uint32_t	ui32ItemId;
	int			i32SlotIdx;
};

// Battle Create (GameServer -> BattleServer)
struct IS_BATTLE_CREATE_REQ
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8GameMode;
	uint8_t		ui8MapIndex;
	uint8_t		ui8MaxPlayers;
	int			i32PlayerCount;
};

struct IS_BATTLE_CREATE_ACK
{
	int			i32RoomIdx;
	int			i32Result;				// 0 = OK
	int			i32BattleRoomIdx;		// Index en BattleServer
	char		szUdpIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16UdpPort;
};

// Player Migrate (GameServer -> BattleServer)
struct IS_PLAYER_MIGRATE_REQ
{
	int64_t		i64UID;
	int			i32BattleRoomIdx;		// Battle room index en BattleServer
	int			i32SlotIdx;				// Slot en la room
	int			i32Team;				// 0=RED, 1=BLUE
	uint32_t	ui32ClientIP;			// IP del cliente para UDP relay
	uint16_t	ui16ClientPort;			// Puerto UDP del cliente
};

struct IS_PLAYER_MIGRATE_ACK
{
	int64_t		i64UID;
	int			i32Result;				// 0 = OK
};

// Battle End Notify (BattleServer -> GameServer)
struct IS_BATTLE_END_NOTIFY
{
	int			i32RoomIdx;				// Room index en GameServer
	int			i32ChannelNum;
	int			i32WinnerTeam;			// 0=RED, 1=BLUE, -1=Draw
	int			i32RedScore;
	int			i32BlueScore;
	int			i32PlayerCount;
	// Followed by IS_BATTLE_PLAYER_RESULT[i32PlayerCount]
};

struct IS_BATTLE_PLAYER_RESULT
{
	int64_t		i64UID;
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32Team;
};

// Note/Mail (GameServer <-> DataServer)
struct IS_NOTE_SEND_REQ
{
	int64_t		i64SenderUID;
	int64_t		i64ReceiverUID;
	int			i32SessionIdx;
	char		szSenderNick[64];
	char		szSubject[64];
	char		szBody[256];
	uint8_t		ui8Type;			// 0=normal, 1=system, 2=gift
};

struct IS_NOTE_SEND_ACK
{
	int			i32SessionIdx;
	int			i32Result;			// 0=OK, 1=target_not_found, 2=mailbox_full, 3=error
};

struct IS_NOTE_LIST_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
};

struct IS_NOTE_LIST_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Count;
	// Followed by IS_NOTE_ENTRY[i32Count]
};

struct IS_NOTE_ENTRY
{
	int64_t		i64NoteId;
	int64_t		i64SenderUID;
	char		szSenderNick[64];
	char		szSubject[64];
	char		szBody[256];
	uint32_t	ui32Timestamp;		// Unix timestamp
	uint8_t		ui8Type;
	uint8_t		ui8Read;			// 0=unread, 1=read
};

struct IS_NOTE_DELETE_REQ
{
	int64_t		i64UID;
	int64_t		i64NoteId;
};

struct IS_NOTE_DELETE_ACK
{
	int64_t		i64UID;
	int64_t		i64NoteId;
	int			i32Result;
};

// ============================================================================
// Battle Live Events (BattleServer -> GameServer, direct - no CastServer)
// ============================================================================

// Kill notification (replaces CModuleCast::Send_PROTOCOL_BATTLE_KILL_ACK)
struct IS_BATTLE_KILL_NOTIFY
{
	int			i32RoomIdx;				// Room index en GameServer
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint8_t		ui8KillCount;			// Number of IS_BATTLE_KILL_INFO following
	uint8_t		ui8Pad[2];
	// Followed by IS_BATTLE_KILL_INFO[ui8KillCount]
};

struct IS_BATTLE_KILL_INFO
{
	uint32_t	ui32KillerSlot;
	uint32_t	ui32VictimSlot;
	int64_t		i64KillerUID;
	int64_t		i64VictimUID;
	uint32_t	ui32WeaponID;
	uint8_t		ui8HitPart;				// CharaHitPart
	uint8_t		ui8Headshot;
	uint8_t		ui8Pad[2];
};

// Round start notification
struct IS_BATTLE_ROUND_START_NOTIFY
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint8_t		ui8GameMode;
	uint8_t		ui8Pad[2];
};

// Round end notification (replaces CModuleCast::Send_PROTOCOL_BATTLE_RoundEnd)
struct IS_BATTLE_ROUND_END_NOTIFY
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint8_t		ui8RoundEndType;		// 0=time, 1=elimination, 2=objective
	uint8_t		ui8WinTeam;				// 0=RED, 1=BLUE, 0xFF=draw
	uint8_t		ui8Pad;
	int			i32RedScore;
	int			i32BlueScore;
};

// Hack detection notification (replaces CModuleCast::Send_PROTOCOL_BATTLE_HACK_USER_ACK)
struct IS_BATTLE_HACK_NOTIFY
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint32_t	ui32SlotIdx;
	int64_t		i64UID;
	uint8_t		ui8HackType;			// HackType enum
	uint8_t		ui8Severity;			// HackSeverity enum
	uint8_t		ui8Pad[2];
	char		szDescription[128];
};

// Mission event notification (bomb, destruction, defence, touchdown)
enum BattleMissionEventType
{
	MISSION_EVENT_BOMB_INSTALL = 0,
	MISSION_EVENT_BOMB_UNINSTALL,
	MISSION_EVENT_BOMB_EXPLODE,
	MISSION_EVENT_DESTRUCTION_HP,
	MISSION_EVENT_DEFENCE,
	MISSION_EVENT_TOUCHDOWN,
};

struct IS_BATTLE_MISSION_NOTIFY
{
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8EventType;			// BattleMissionEventType
	uint8_t		ui8Pad[3];
	uint32_t	ui32SlotIdx;			// Player who triggered
	int64_t		i64UID;
	int32_t		i32Param1;				// Object index or HP value
	int32_t		i32Param2;				// Extra param
};

// Player Ban (GameServer -> DataServer)
struct IS_PLAYER_BAN_REQ
{
	int64_t		i64UID;
	int64_t		i64BannedByUID;
	int			i32Duration;		// Seconds, 0 = permanent
	char		szReason[128];
};

struct IS_PLAYER_BAN_ACK
{
	int64_t		i64UID;
	int			i32Result;			// 0=OK, 1=not_found, 2=error
};

// ============================================================================
// Account Cosmetics
// ============================================================================

struct IS_COSMETICS_LOAD_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
};

struct IS_COSMETICS_LOAD_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;
	uint8_t		ui8NicknameColor;
	uint8_t		ui8CrosshairColor;
	uint8_t		ui8ChattingColor;
	int			i32DisguiseRank;
	char		szFakeNick[50];
};

struct IS_COSMETICS_SAVE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8NicknameColor;
	uint8_t		ui8CrosshairColor;
	uint8_t		ui8ChattingColor;
	int			i32DisguiseRank;
	char		szFakeNick[50];
};

struct IS_COSMETICS_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// ============================================================================
// Mode Records (per game mode stats)
// ============================================================================

struct IS_MODE_RECORD_ENTRY
{
	uint8_t		ui8RecordType;		// Game mode (0-9)
	int			i32Matches;
	int			i32Wins;
	int			i32Losses;
	int			i32Draws;
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32Disconnects;
	int			i32DailyMatches;
	int			i32DailyKills;
};

struct IS_MODE_RECORDS_SAVE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8RecordType;		// Which mode to update
	int			i32DeltaMatches;	// Additive delta
	int			i32DeltaWins;
	int			i32DeltaLosses;
	int			i32DeltaDraws;
	int			i32DeltaKills;
	int			i32DeltaDeaths;
	int			i32DeltaHeadshots;
	int			i32DeltaDisconnects;
};

struct IS_MODE_RECORDS_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// ============================================================================
// Daily Records
// ============================================================================

struct IS_DAILY_RECORD_ENTRY
{
	int			i32RecordDate;		// YYYYMMDD
	int			i32Wins;
	int			i32Losses;
	int			i32Draws;
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32ExpEarned;
	int			i32GPEarned;
	int			i32PlayTime;
	int			i32GiftPlayTime;
	uint8_t		ui8GotDailyItem;
};

struct IS_DAILY_RECORD_SAVE_REQ
{
	int64_t		i64UID;
	int			i32DeltaWins;
	int			i32DeltaLosses;
	int			i32DeltaDraws;
	int			i32DeltaKills;
	int			i32DeltaDeaths;
	int			i32DeltaHeadshots;
	int			i32DeltaExp;
	int			i32DeltaGP;
	int			i32DeltaPlayTime;
	uint8_t		ui8GotDailyItem;	// Set to 1 when claimed
};

struct IS_DAILY_RECORD_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// ============================================================================
// Map System
// ============================================================================

struct IS_MAP_MODE_ENTRY
{
	uint16_t	ui16StageKey;
	uint16_t	ui16StageUID;
	uint8_t		ui8Mode;
	uint8_t		ui8MaxPlayers;
	uint8_t		ui8Mark;
	uint8_t		ui8SpecialMode;
};

struct IS_MAP_LIST_ACK
{
	int			i32Result;
	int			i32MapCount;
	// Followed by IS_MAP_MODE_ENTRY[i32MapCount]
};

struct IS_MAP_STATS_SAVE_REQ
{
	int			i32MapId;
	int			i32StageType;		// Game mode
	int			i32PlayTime;		// Seconds
};

struct IS_MAP_STATS_SAVE_ACK
{
	int			i32Result;
};

// ============================================================================
// Rankings
// ============================================================================

struct IS_RANKING_INDIVIDUAL_ENTRY
{
	int			i32RankPosition;
	int64_t		i64UID;
	char		szNickname[50];
	int			i32Level;
	int64_t		i64Exp;
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
};

struct IS_RANKING_INDIVIDUAL_ACK
{
	int			i32Result;
	int			i32Count;
	uint8_t		ui8TermType;		// 0=daily, 1=weekly, 2=monthly
	// Followed by IS_RANKING_INDIVIDUAL_ENTRY[i32Count]
};

struct IS_RANKING_CLAN_ENTRY
{
	int			i32RankPosition;
	int			i32ClanId;
	char		szClanName[50];
	int64_t		i64ClanExp;
	int			i32Wins;
	int			i32Losses;
	int			i32MemberCount;
};

struct IS_RANKING_CLAN_ACK
{
	int			i32Result;
	int			i32Count;
	uint8_t		ui8TermType;
	// Followed by IS_RANKING_CLAN_ENTRY[i32Count]
};

// ============================================================================
// Medal Sets
// ============================================================================

struct IS_MEDAL_SET_CURRENT_ENTRY
{
	uint8_t		ui8SetType;
	int16_t		i16MedalSetIdx;
	int16_t		i16Medal1Count;
	int16_t		i16Medal2Count;
	int16_t		i16Medal3Count;
	int16_t		i16Medal4Count;
	int16_t		i16Medal5Count;
	int16_t		i16Medal6Count;
	uint8_t		ui8GetReward;
};

struct IS_MEDAL_SET_SAVE_REQ
{
	int64_t		i64UID;
	uint8_t		ui8SetType;
	int16_t		i16MedalSetIdx;
	int16_t		i16Medal1Count;
	int16_t		i16Medal2Count;
	int16_t		i16Medal3Count;
	int16_t		i16Medal4Count;
	int16_t		i16Medal5Count;
	int16_t		i16Medal6Count;
	uint8_t		ui8GetReward;
	uint8_t		ui8IsComplete;		// 1 = move to complete table
};

struct IS_MEDAL_SET_SAVE_ACK
{
	int64_t		i64UID;
	int			i32Result;
};

// ============================================================================
// Server Config / Notices / Boost Events
// ============================================================================

struct IS_SERVER_CONFIG_ENTRY
{
	char		szKey[64];
	char		szValue[255];
};

struct IS_SERVER_CONFIG_ACK
{
	int			i32Result;
	int			i32Count;
	// Followed by IS_SERVER_CONFIG_ENTRY[i32Count]
};

struct IS_NOTICE_ENTRY
{
	int			i32Id;
	int			i32RepeatInterval;	// Seconds
	char		szText[255];
};

struct IS_NOTICES_ACK
{
	int			i32Result;
	int			i32Count;
	// Followed by IS_NOTICE_ENTRY[i32Count]
};

struct IS_BOOST_EVENT_ENTRY
{
	int			i32EventType;
	int			i32ExpMultiplier;	// Percentage (100=normal)
	int			i32GPMultiplier;
	uint32_t	ui32StartTime;		// Unix timestamp
	uint32_t	ui32EndTime;
};

struct IS_BOOST_EVENTS_ACK
{
	int			i32Result;
	int			i32Count;
	// Followed by IS_BOOST_EVENT_ENTRY[i32Count]
};

// ============================================================================
// Gift Delivery System
// ============================================================================

struct IS_GIFT_SEND_REQ
{
	int64_t		i64SenderUID;
	int64_t		i64ReceiverUID;
	char		szSenderNick[50];
	int			i32GoodsId;
	int			i32ItemId;
	uint8_t		ui8GiftType;		// 0=player, 1=system, 2=web, 3=event
	char		szMessage[125];
};

struct IS_GIFT_SEND_ACK
{
	int64_t		i64SenderUID;
	int			i32SessionIdx;
	int			i32Result;			// 0=OK, 1=target_not_found, 2=error
};

struct IS_GIFT_ENTRY
{
	int64_t		i64GiftId;
	int64_t		i64SenderUID;
	char		szSenderNick[50];
	int			i32GoodsId;
	int			i32ItemId;
	uint8_t		ui8GiftType;
	char		szMessage[125];
	uint32_t	ui32Timestamp;
};

struct IS_GIFT_LIST_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;
	int			i32Count;
	// Followed by IS_GIFT_ENTRY[i32Count]
};

struct IS_GIFT_RECEIVE_REQ
{
	int64_t		i64UID;
	int64_t		i64GiftId;
	int			i32SessionIdx;
};

struct IS_GIFT_RECEIVE_ACK
{
	int64_t		i64UID;
	int64_t		i64GiftId;
	int			i32SessionIdx;
	int			i32Result;			// 0=OK, 1=not_found, 2=already_received, 3=error
	int			i32ItemId;			// Item received
	int			i32GoodsId;
};

// ============================================================================
// Clan Extended: Join Requests
// ============================================================================

struct IS_CLAN_REQUEST_SEND_REQ
{
	int			i32ClanId;
	int64_t		i64UID;
	char		szNickname[64];
	int			i32Level;
	int			i32RankId;
	char		szMessage[128];
};

struct IS_CLAN_REQUEST_SEND_ACK
{
	int64_t		i64UID;
	int			i32ClanId;
	int			i32Result;			// 0=OK, 1=already_requested, 2=clan_full, 3=error
};

struct IS_CLAN_REQUEST_ENTRY
{
	int64_t		i64RequestId;
	int64_t		i64UID;
	char		szNickname[64];
	int			i32Level;
	int			i32RankId;
	char		szMessage[128];
	uint32_t	ui32Timestamp;
};

struct IS_CLAN_REQUEST_LIST_ACK
{
	int			i32ClanId;
	int			i32Result;
	int			i32Count;
	// Followed by IS_CLAN_REQUEST_ENTRY[i32Count]
};

struct IS_CLAN_REQUEST_RESPOND_REQ
{
	int			i32ClanId;
	int64_t		i64MasterUID;		// Who is responding
	int64_t		i64RequestId;
	int64_t		i64ApplicantUID;
	uint8_t		ui8Accept;			// 0=reject, 1=accept
};

struct IS_CLAN_REQUEST_RESPOND_ACK
{
	int			i32ClanId;
	int64_t		i64ApplicantUID;
	int			i32Result;
};

// ============================================================================
// Clan Items
// ============================================================================

struct IS_CLAN_ITEM_ENTRY
{
	int			i32Id;
	int			i32ItemId;
	int			i32ItemCount;
	uint8_t		ui8ItemType;
};

struct IS_CLAN_ITEMS_ACK
{
	int			i32ClanId;
	int			i32Result;
	int			i32Count;
	// Followed by IS_CLAN_ITEM_ENTRY[i32Count]
};

// ============================================================================
// Clan Match Result (update clan stats after clan war)
// ============================================================================

struct IS_CLAN_MATCH_RESULT_REQ
{
	int			i32ClanId;
	int			i32OpponentClanId;
	uint8_t		ui8Result;			// 0=win, 1=loss, 2=draw
	int			i32ExpGained;
	int			i32RatingDelta;		// +/- rating change
};

struct IS_CLAN_MATCH_RESULT_ACK
{
	int			i32ClanId;
	int			i32Result;
};

// ============================================================================
// Clan Season
// ============================================================================

struct IS_CLAN_SEASON_ENTRY
{
	int			i32SeasonId;
	int			i32FinalRank;
	int			i32FinalRating;
	int			i32Wins;
	int			i32Losses;
	int			i32Draws;
	int			i32SeasonExp;
};

struct IS_CLAN_SEASON_ACK
{
	int			i32ClanId;
	int			i32Result;
	int			i32Count;
	// Followed by IS_CLAN_SEASON_ENTRY[i32Count]
};

// ============================================================================
// Mercenary System
// ============================================================================

struct IS_MERCENARY_HIRE_REQ
{
	int			i32ClanId;
	int64_t		i64UID;
	char		szNickname[64];
	int			i32DurationSec;		// Contract duration in seconds
};

struct IS_MERCENARY_HIRE_ACK
{
	int			i32ClanId;
	int64_t		i64UID;
	int			i32Result;			// 0=OK, 1=already_hired, 2=clan_full, 3=error
};

struct IS_MERCENARY_ENTRY
{
	int64_t		i64UID;
	char		szNickname[64];
	int			i32Kills;
	int			i32Deaths;
	int			i32Wins;
	int			i32Losses;
	int			i32Disconnects;
	uint32_t	ui32HireTime;		// Unix timestamp
	uint32_t	ui32ExpireTime;
};

struct IS_MERCENARY_LIST_ACK
{
	int			i32ClanId;
	int			i32Result;
	int			i32Count;
	// Followed by IS_MERCENARY_ENTRY[i32Count]
};

struct IS_MERCENARY_DISMISS_REQ
{
	int			i32ClanId;
	int64_t		i64UID;
};

struct IS_MERCENARY_DISMISS_ACK
{
	int			i32ClanId;
	int64_t		i64UID;
	int			i32Result;
};

struct IS_MERCENARY_RESULT_REQ
{
	int			i32ClanId;
	int64_t		i64UID;
	int			i32DeltaKills;
	int			i32DeltaDeaths;
	int			i32DeltaWins;
	int			i32DeltaLosses;
	int			i32DeltaDisconnects;
};

struct IS_MERCENARY_RESULT_ACK
{
	int			i32Result;
};

// ============================================================================
// Shop Coupon
// ============================================================================

struct IS_SHOP_COUPON_REQ
{
	int64_t		i64UID;
	int			i32SessionIdx;
	char		szCouponCode[32];
};

struct IS_SHOP_COUPON_ACK
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Result;			// 0=OK, 1=invalid, 2=expired, 3=already_used, 4=limit_reached
	int			i32DiscountValue;
	uint8_t		ui8DiscountType;	// 0=fixed, 1=percent
};

#pragma pack(pop)

#endif // __INTERSERVERPROTOCOL_H__
