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
	int			i32RankId;
	int			i32ClanId;
	// Stats
	int			i32Kills;
	int			i32Deaths;
	int			i32Headshots;
	int			i32Wins;
	int			i32Losses;
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

#pragma pack(pop)

#endif // __INTERSERVERPROTOCOL_H__
