#ifndef __ROOMDEF_H__
#define __ROOMDEF_H__

#pragma once

// Room definitions para el GameServer remake
// Usa las definiciones de Common cuando sea posible

// Constantes de room (del original CommonDef.h)
#ifndef SLOT_MAX_COUNT
#define SLOT_MAX_COUNT			16
#endif

#ifndef TEAM_COUNT
#define TEAM_COUNT				2
#endif

#ifndef GAME_CHANNEL_COUNT
#define GAME_CHANNEL_COUNT		10
#endif

#define MAX_ROOM_COUNT_PER_CHANNEL	200
#define NET_ROOM_NAME_SIZE			64
#define NET_ROOM_PW_SIZE			16

// ============================================================================
// Room state enums (from CommonDef.h)
// ============================================================================

#ifndef SLOT_STATE_EMPTY
enum SlotState
{
	SLOT_STATE_EMPTY = 0,
	SLOT_STATE_CLOSE,
	SLOT_STATE_SHOP,
	SLOT_STATE_INFO,
	SLOT_STATE_CLAN,
	SLOT_STATE_INVENTORY,
	SLOT_STATE_GACHA,
	SLOT_STATE_GIFTSHOP,

	SLOT_STATE_NORMAL,			// In ready room
	SLOT_STATE_READY,			// Ready in ready room
	SLOT_STATE_LOAD,			// Loading game
	SLOT_STATE_RENDEZVOUS,		// Rendezvous server
	SLOT_STATE_PRESTART,		// Hole punching
	SLOT_STATE_BATTLE_LOADOK,	// Game sync done
	SLOT_STATE_BATTLE_READY,	// Waiting for game start
	SLOT_STATE_BATTLE,			// In battle
};
#endif

#ifndef ROOM_STATE_READY
enum RoomState
{
	ROOM_STATE_READY = 0,		// Ready room
	ROOM_STATE_COUNTDOWN_R,		// Start countdown
	ROOM_STATE_LOADING,			// Loading
	ROOM_STATE_RENDEZVOUS,		// Rendezvous
	ROOM_STATE_PRE_BATTLE,		// Pre-battle (hole punch)
	ROOM_STATE_COUNTDOWN_B,		// Battle countdown
	ROOM_STATE_BATTLE,			// Battle in progress
	ROOM_STATE_BATTLE_RESULT,	// Battle result screen
	ROOM_STATE_BATTLE_END,		// Battle ended
	ROOM_STATE_EMPTY,			// Empty/destroyed room
};
#endif

// Team types
#ifndef TEAM_RED
#define TEAM_RED	0
#define TEAM_BLUE	1
#endif

// ============================================================================
// Stage/Game mode enums (from CommonDef_Stage.h)
// ============================================================================

enum StageMode
{
	STAGE_MODE_NA = 0,
	STAGE_MODE_DEATHMATCH,		// Team Deathmatch
	STAGE_MODE_BOMB,			// Bomb plant/defuse
	STAGE_MODE_DESTROY,			// Destroy objectives
	STAGE_MODE_ANNIHILATION,	// Eliminate all enemies
	STAGE_MODE_DEFENCE,			// Defence mode
	STAGE_MODE_RESERVED1,
	STAGE_MODE_ESCAPE,			// Escape/VIP mode
	STAGE_MODE_RESERVED2,
	STAGE_MODE_RESERVED3,
	STAGE_MODE_TUTORIAL,
	STAGE_MODE_RESERVED4,
	STAGE_MODE_CROSSCOUNT,		// Dino deathmatch
	STAGE_MODE_CONVOY,			// Convoy mode
	STAGE_MODE_RUN_AWAY,		// Run away mode
	STAGE_MODE_RESERVED5,
	STAGE_MODE_CHAMPIONSHIP,	// Championship mode
	STAGE_MODE_ALL,
	STAGE_MODE_MAX = STAGE_MODE_ALL
};

// Weapon restrictions (from CommonDef_Stage.h)
enum WeaponRestriction
{
	WEAPON_ALL = 0,
	WEAPON_ASSULT_ONLY,
	WEAPON_SMG_ONLY,
	WEAPON_SNIPER_ONLY,
	WEAPON_SHOTGUN_ONLY,
	WEAPON_MELEE_ONLY,
	WEAPON_HANDGUN_ONLY,
	WEAPON_MG_ONLY,
	WEAPON_MAX
};

// Battle round types (from CommonDef_Room.h)
enum BattleRoundType
{
	BATTLE_ROUND_TYPE_1 = 0,
	BATTLE_ROUND_TYPE_2,
	BATTLE_ROUND_TYPE_3,
	BATTLE_ROUND_TYPE_5,
	BATTLE_ROUND_TYPE_7,
	BATTLE_ROUND_TYPE_9,
	BATTLE_ROUND_TYPE_ENDLESS,
	BATTLE_ROUND_TYPE_MAX,
};

// Kill count types for deathmatch (from CommonDef_Room.h)
#define BATTLE_KILL_TYPE_60		0x00
#define BATTLE_KILL_TYPE_80		0x01
#define BATTLE_KILL_TYPE_100	0x02
#define BATTLE_KILL_TYPE_120	0x03
#define BATTLE_KILL_TYPE_140	0x04
#define BATTLE_KILL_TYPE_160	0x05

// Time limit types (from CommonDef_Room.h)
#define BATTLE_TIME_TYPE_3		0x00
#define BATTLE_TIME_TYPE_5		0x30
#define BATTLE_TIME_TYPE_10		0x40
#define BATTLE_TIME_TYPE_15		0x50
#define BATTLE_TIME_TYPE_20		0x60
#define BATTLE_TIME_TYPE_25		0x70
#define BATTLE_TIME_TYPE_30		0x80

// Mission time types
#define BATTLE_TIME_MISSION_TYPE_3	0x00
#define BATTLE_TIME_MISSION_TYPE_5	0x10
#define BATTLE_TIME_MISSION_TYPE_7	0x20

// ============================================================================
// Room info flags (from CommonDef_Room.h)
// ============================================================================

#define ROOM_INFO_FLAG_INTER_ENTER			0x80
#define ROOM_INFO_FLAG_OBSERVER				0x40
#define ROOM_INFO_FLAG_TEAM_BALANCE			0x20
#define ROOM_INFO_FLAG_REAL_IP				0x10
#define ROOM_INFO_FLAG_OBSERVER_MODE		0x08
#define ROOM_INFO_FLAG_PRIVATE_MODE			0x04
#define ROOM_INFO_FLAG_RANDOM_MODE			0x02
#define ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE	0x01

// Weapon restriction flags
#define LOCK_KNUCKLE	0x80
#define LOCK_SHOTGUN	0x40
#define LOCK_SNIPER		0x20
#define LOCK_RPG7		0x10
#define LOCK_PRIMARY	0x08
#define LOCK_SECONDARY	0x04
#define LOCK_MELEE		0x02
#define LOCK_THROW		0x01
#define LOCK_BASIC		0x0F

// Mission timing (seconds)
#define MISSION_PREBATTLE_TIME		5
#define MISSION_TEAM_CHANGE_TIME	5
#define MISSION_FREE_TIME			5
#define MISSION_RESULT_TIME			3
#define COUNT_DOWN_TIME				3
#define LOADING_TIMEOUT				60		// Max loading time before kicking
#define BATTLE_RESULT_DISPLAY_TIME	5		// Result screen display time

// Bomb mode constants (from WeaponDefine.h & CommonDef_Room.h)
#define BOMB_INSTALL_TIME			5.5f		// Seconds to plant C4
#define BOMB_DISARM_TIME			7.0f		// Seconds to defuse C4
#define BOMB_EXPLOSION_DELAY_MS		40000		// 40 seconds after plant to explode

// Destroy mode constants
#define GENERATOR_MAX_HP			1000		// Default HP for generator/objective
#define GENERATOR_COUNT_MAX			2			// Max generators per map

// Escape/VIP mode constants
#define ESCAPE_VIP_EXTRA_HP			150			// VIP gets 150% effective HP
#define ESCAPE_TOUCHDOWN_RADIUS		5.0f		// Radius to touchdown zone
#define ESCAPE_VIP_SLOT_NONE		(-1)

// Bomb area
enum BombArea
{
	BOMB_AREA_A = 0,
	BOMB_AREA_B,
	BOMB_AREA_COUNT,
};

// Mission end types (from CommonDef_Room.h)
enum MissionEndType
{
	MISSION_END_TYPE_NONE = 0,
	MISSION_END_TIMEOUT = 1,
	MISSION_END_BOMBFIRE = 2,		// Bomb exploded (ATK wins)
	MISSION_END_UNINSTALL = 3,		// Bomb defused (DEF wins)
	MISSION_END_ALLDEATH = 4,		// All enemies dead
	MISSION_END_GENERATOR_DESTROY = 5,
	MISSION_END_DEFENCE_DESTROY = 6,
};

// Respawn types and their times (seconds)
enum RespawnType
{
	RESPAWN_TYPE_NO = 0,			// 6 seconds
	RESPAWN_TYPE_SHORT_20,			// 5 seconds
	RESPAWN_TYPE_SHORT_30,			// 4 seconds
	RESPAWN_TYPE_MID_30,			// 3 seconds
	RESPAWN_TYPE_MID_40,			// 2 seconds
	RESPAWN_TYPE_MID_50,			// 1 second
};

inline int GetRespawnTimeByType(uint8_t ui8Type)
{
	switch (ui8Type)
	{
	case RESPAWN_TYPE_NO:			return 6;
	case RESPAWN_TYPE_SHORT_20:		return 5;
	case RESPAWN_TYPE_SHORT_30:		return 4;
	case RESPAWN_TYPE_MID_30:		return 3;
	case RESPAWN_TYPE_MID_40:		return 2;
	case RESPAWN_TYPE_MID_50:		return 1;
	default: return 6;
	}
}

// Multi-kill types
enum MultiKillType
{
	MULTI_KILL_NONE = 0,
	MULTI_KILL_DOUBLE,				// 2 kills
	MULTI_KILL_TRIPLE,				// 3 kills
	MULTI_KILL_MULTI,				// 4 kills
	MULTI_KILL_ULTRA,				// 5 kills
	MULTI_KILL_UNBELIEVABLE,		// 6+ kills
};

inline uint8_t GetMultiKillType(int i32ConsecutiveKills)
{
	if (i32ConsecutiveKills >= 6) return MULTI_KILL_UNBELIEVABLE;
	if (i32ConsecutiveKills >= 5) return MULTI_KILL_ULTRA;
	if (i32ConsecutiveKills >= 4) return MULTI_KILL_MULTI;
	if (i32ConsecutiveKills >= 3) return MULTI_KILL_TRIPLE;
	if (i32ConsecutiveKills >= 2) return MULTI_KILL_DOUBLE;
	return MULTI_KILL_NONE;
}

// Hit part types
enum HitPartType
{
	HIT_PART_BODY = 0,
	HIT_PART_HEAD,
	HIT_PART_LEFT_ARM,
	HIT_PART_RIGHT_ARM,
	HIT_PART_LEFT_LEG,
	HIT_PART_RIGHT_LEG,
};

// Battle result event types (EXP/Point bonus categories)
#define TYPE_BATTLE_RESULT_EVENT_COUNT	8
enum BattleResultEventType
{
	BATTLE_RESULT_EVENT_BASE = 0,		// Base reward
	BATTLE_RESULT_EVENT_WIN,			// Win bonus
	BATTLE_RESULT_EVENT_KILL,			// Kill bonus
	BATTLE_RESULT_EVENT_HEADSHOT,		// Headshot bonus
	BATTLE_RESULT_EVENT_ASSIST,			// Assist bonus
	BATTLE_RESULT_EVENT_BOOST,			// Event boost
	BATTLE_RESULT_EVENT_ITEM,			// Item boost
	BATTLE_RESULT_EVENT_RANK,			// Rank bonus
};

// ============================================================================
// Structs
// ============================================================================

// Room creation info (expanded from original ROOM_INFO_BASIC)
struct GameRoomCreateInfo
{
	char		szTitle[NET_ROOM_NAME_SIZE];
	char		szPassword[NET_ROOM_PW_SIZE];
	uint32_t	ui32StageId;		// Encoded stage ID (mode|weapon|AI|flags|stageUID)
	uint8_t		ui8GameMode;		// StageMode enum
	uint8_t		ui8MapIndex;		// STAGE_UID
	uint8_t		ui8MaxPlayers;		// 2-16
	uint8_t		ui8RoundType;		// BattleRoundType enum
	uint8_t		ui8SubType;			// Kill count/time type (encoded in upper/lower nibble)
	uint8_t		ui8TeamBalance;		// Auto team balance
	uint8_t		ui8WeaponFlag;		// Weapon restriction flags
	uint8_t		ui8InfoFlag;		// Room info flags
	uint32_t	ui32OptionFlag;		// Extended options
	bool		bIsClanMatch;		// Clan match room

	GameRoomCreateInfo()
		: ui32StageId(0)
		, ui8GameMode(STAGE_MODE_DEATHMATCH)
		, ui8MapIndex(0)
		, ui8MaxPlayers(16)
		, ui8RoundType(BATTLE_ROUND_TYPE_3)
		, ui8SubType(BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_5)
		, ui8TeamBalance(1)
		, ui8WeaponFlag(LOCK_BASIC)
		, ui8InfoFlag(0)
		, ui32OptionFlag(0)
		, bIsClanMatch(false)
	{
		szTitle[0] = '\0';
		szPassword[0] = '\0';
	}
};

// Slot info (expanded)
struct GameSlotInfo
{
	uint8_t		ui8State;			// SLOT_STATE enum
	uint8_t		ui8Team;			// TEAM_TYPE (0=RED, 1=BLUE)
	int			i32SessionIdx;		// Session index (-1 = empty)
	uint8_t		ui8Rank;			// Player rank
	uint32_t	ui32ClanIdx;		// Clan ID
	uint32_t	ui32ClanMark;		// Clan mark
	uint32_t	ui32Item;			// Consumable item flags
	uint32_t	ui32CharaItemId;	// Character item ID

	GameSlotInfo()
		: ui8State(SLOT_STATE_EMPTY)
		, ui8Team(0)
		, i32SessionIdx(-1)
		, ui8Rank(0)
		, ui32ClanIdx(0)
		, ui32ClanMark(0)
		, ui32Item(0)
		, ui32CharaItemId(0)
	{
	}

	void Reset()
	{
		ui8State = SLOT_STATE_EMPTY;
		ui8Team = 0;
		i32SessionIdx = -1;
		ui8Rank = 0;
		ui32ClanIdx = 0;
		ui32ClanMark = 0;
		ui32Item = 0;
		ui32CharaItemId = 0;
	}
};

// Score tracking per team
struct GameRoomScore
{
	int			i32RedScore;
	int			i32BlueScore;
	int			i32NowRound;		// Current round (0-based)
	int			i32MaxRound;		// Max rounds
	uint16_t	ui16NowIngTime;		// Current elapsed time
	uint16_t	ui16MaxTime;		// Max time per round (seconds)

	GameRoomScore()
		: i32RedScore(0)
		, i32BlueScore(0)
		, i32NowRound(0)
		, i32MaxRound(3)
		, ui16NowIngTime(0)
		, ui16MaxTime(300)
	{
	}

	void Reset()
	{
		i32RedScore = i32BlueScore = 0;
		i32NowRound = 0;
		ui16NowIngTime = 0;
	}
};

// Round count lookup table
inline int GetRoundCount(uint8_t ui8RoundType)
{
	switch (ui8RoundType)
	{
	case BATTLE_ROUND_TYPE_1:		return 1;
	case BATTLE_ROUND_TYPE_2:		return 2;
	case BATTLE_ROUND_TYPE_3:		return 3;
	case BATTLE_ROUND_TYPE_5:		return 5;
	case BATTLE_ROUND_TYPE_7:		return 7;
	case BATTLE_ROUND_TYPE_9:		return 9;
	case BATTLE_ROUND_TYPE_ENDLESS:	return 999;
	default: return 3;
	}
}

// Time limit in seconds from SubType
inline uint16_t GetTimeLimit(uint8_t ui8SubType, uint8_t ui8GameMode)
{
	uint8_t timeType = ui8SubType & 0xF0;

	if (ui8GameMode == STAGE_MODE_BOMB || ui8GameMode == STAGE_MODE_ANNIHILATION ||
		ui8GameMode == STAGE_MODE_DESTROY || ui8GameMode == STAGE_MODE_DEFENCE)
	{
		// Mission modes use mission time
		switch (timeType)
		{
		case BATTLE_TIME_MISSION_TYPE_3:	return 180;
		case BATTLE_TIME_MISSION_TYPE_5:	return 300;
		case BATTLE_TIME_MISSION_TYPE_7:	return 420;
		default: return 300;
		}
	}
	else
	{
		// Deathmatch/other modes use battle time
		switch (timeType)
		{
		case BATTLE_TIME_TYPE_3:	return 180;
		case BATTLE_TIME_TYPE_5:	return 300;
		case BATTLE_TIME_TYPE_10:	return 600;
		case BATTLE_TIME_TYPE_15:	return 900;
		case BATTLE_TIME_TYPE_20:	return 1200;
		case BATTLE_TIME_TYPE_25:	return 1500;
		case BATTLE_TIME_TYPE_30:	return 1800;
		default: return 300;
		}
	}
}

// Kill limit from SubType (deathmatch only)
inline int GetKillLimit(uint8_t ui8SubType)
{
	uint8_t killType = ui8SubType & 0x0F;
	switch (killType)
	{
	case BATTLE_KILL_TYPE_60:	return 60;
	case BATTLE_KILL_TYPE_80:	return 80;
	case BATTLE_KILL_TYPE_100:	return 100;
	case BATTLE_KILL_TYPE_120:	return 120;
	case BATTLE_KILL_TYPE_140:	return 140;
	case BATTLE_KILL_TYPE_160:	return 160;
	default: return 100;
	}
}

// Check if a game mode uses rounds (mission-based)
inline bool IsMissionMode(uint8_t ui8GameMode)
{
	return (ui8GameMode == STAGE_MODE_BOMB ||
			ui8GameMode == STAGE_MODE_ANNIHILATION ||
			ui8GameMode == STAGE_MODE_DESTROY ||
			ui8GameMode == STAGE_MODE_DEFENCE ||
			ui8GameMode == STAGE_MODE_ESCAPE);
}

// Check if a game mode uses ATK/DEF teams with swap
inline bool IsAtkDefMode(uint8_t ui8GameMode)
{
	return (ui8GameMode == STAGE_MODE_BOMB ||
			ui8GameMode == STAGE_MODE_DESTROY ||
			ui8GameMode == STAGE_MODE_ESCAPE);
}

// QuickJoin constants
#define MAX_QUICK_JOIN_INFO_COUNT	3
#define QUICK_JOIN_SEARCH_FAIL_IDX	(-1)

#endif // __ROOMDEF_H__
