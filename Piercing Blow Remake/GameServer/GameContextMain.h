#ifndef __GAMECONTEXTMAIN_H__
#define __GAMECONTEXTMAIN_H__

#pragma once
#include "i3ElementBase.h"
#include "EventDef.h"

class MedalLoader;

// Channel type enum (from CommonDef_System.h ChannelType)
enum GameChannelType
{
	GAME_CHANNEL_TYPE_NONE = 0,
	GAME_CHANNEL_TYPE_NORMAL,			// Normal channel
	GAME_CHANNEL_TYPE_BEGIN1,			// Beginner - Kill/Death restriction
	GAME_CHANNEL_TYPE_BEGIN2,			// Beginner - Ranking restriction
	GAME_CHANNEL_TYPE_CLAN,				// Clan channel
	GAME_CHANNEL_TYPE_EXPERT,			// Expert - Min level
	GAME_CHANNEL_TYPE_EXPERT2,			// Expert - Min rank
	GAME_CHANNEL_TYPE_CHAMPIONSHIP,		// Championship
	GAME_CHANNEL_TYPE_NORMAL2,			// Normal 2
	GAME_CHANNEL_TYPE_NATIONAL,			// National
	GAME_CHANNEL_TYPE_MIDDLE,			// Middle
	GAME_CHANNEL_TYPE_PCCAFE_SILVER,	// PC Cafe Silver
	GAME_CHANNEL_TYPE_PCCAFE_GOLD,		// PC Cafe Gold
	GAME_CHANNEL_TYPE_PCCAFE_CLAN,		// PC Cafe Clan
	GAME_CHANNEL_TYPE_MAX,
};

#define MAX_GAME_CHANNELS	20

// Per-channel configuration
struct GameChannelInfo
{
	GameChannelType	eType;
	char			szName[32];
	uint16_t		ui16MaxUsers;
	uint16_t		ui16CurrentUsers;
	int				i32MinLevel;		// Min level for expert channels
	int				i32MaxLevel;		// Max level for beginner channels

	void Reset()
	{
		eType = GAME_CHANNEL_TYPE_NORMAL;
		szName[0] = '\0';
		ui16MaxUsers = 200;
		ui16CurrentUsers = 0;
		i32MinLevel = 0;
		i32MaxLevel = 0;
	}
};

// Version simplificada de CContextMain del original
// Solo contiene datos globales necesarios para el GameServer remake
class GameContextMain : public i3ElementBase
{
	I3_CLASS_DEFINE(GameContextMain);

public:
	GameContextMain();
	~GameContextMain();

	BOOL	Create();
	void	OnUpdate();

	// Channel helpers
	const GameChannelInfo&	GetChannelInfo(int idx) const	{ return m_Channels[idx]; }
	GameChannelInfo&		GetChannelInfoMut(int idx)		{ return m_Channels[idx]; }

public:
	// Server identity
	int			m_i32ServerId;
	char		m_szServerName[64];

	// Session/channel config
	int			m_i32SessionCount;
	uint8_t		m_ui8ChannelCount;
	uint16_t	m_ui16MaxUsersPerChannel;
	uint16_t	m_ui16MaxRoomsPerChannel;

	// Per-channel info
	GameChannelInfo	m_Channels[MAX_GAME_CHANNELS];

	// RSA key (from S2MO)
	int16_t		m_i16RSAKeySize;
	int8_t		m_pRSAKey[512];

	// Timers
	int			m_i32UpdateTimeSS;
	int			m_i32LastUpdateTime;

	// Economy config (from [Economy] section)
	int			m_i32KillGPReward;
	int			m_i32WinGPReward;
	int			m_i32LoseGPReward;
	int			m_i32KillExpReward;
	int			m_i32WinExpReward;
	int			m_i32LoseExpReward;
	int			m_i32StartingGP;
	int			m_i32StartingCash;

	// Battle config (from [Battle] section)
	int			m_i32DefaultTimeLimit;
	int			m_i32DefaultMaxRound;
	int			m_i32MinPlayersToStart;
	int			m_i32AFKTimeout;

	// Random map config (Phase 14D)
	int			m_i32RandomMapCount;		// Number of random maps per channel (default 6)

	// Rank/EXP table (Phase 10)
	// 52 military ranks: Trainee(0) through General(51)
	static const int MAX_RANK_COUNT = 52;
	int64_t		m_aRankExpTable[MAX_RANK_COUNT];	// EXP required per rank

	// Map/Stage data (Phase 4B)
	static const int MAX_MAP_COUNT = 64;
	static const int MAX_STAGE_MODE_COUNT = 17;

	struct MapInfo
	{
		uint32_t	ui32StageId;
		char		szName[32];
		uint8_t		ui8SupportedModes;		// Bitmask of supported STAGE_MODEs (low byte)
		uint8_t		ui8SupportedModes2;		// High byte
		uint8_t		ui8MinPlayers;
		uint8_t		ui8MaxPlayers;
		bool		bActive;

		void Reset()
		{
			ui32StageId = 0;
			szName[0] = '\0';
			ui8SupportedModes = 0xFF;
			ui8SupportedModes2 = 0xFF;
			ui8MinPlayers = 2;
			ui8MaxPlayers = 16;
			bActive = false;
		}
	};

	MapInfo		m_Maps[MAX_MAP_COUNT];
	int			m_i32MapCount;
	uint32_t	m_ui32MapVersion;

	// Boost events (Phase 14B)
	GameBoostEvent	m_BoostEvents[MAX_BOOST_EVENTS];
	int				m_i32BoostEventCount;

	// Helper methods
	int64_t		GetRankExp(int rankId) const;
	int			GetRankForExp(int64_t exp) const;
	bool		IsMapValidForMode(int mapIdx, int mode) const;

	// Boost event helpers (Phase 14B)
	uint16_t	GetCurrentExpMultiplier() const;		// Returns 100-based multiplier (100 = 1x)
	uint16_t	GetCurrentPointMultiplier() const;
	bool		IsBoostEventActive(int idx) const;

	// Performance metrics
	struct ServerMetrics
	{
		// Current state
		int			i32CCU;					// Concurrent connected users
		int			i32ActiveRooms;			// Active rooms across all channels
		int			i32ActiveClans;			// Active clans
		int			i32ActiveBattles;		// Rooms in BATTLE state

		// Cumulative counters (since server start)
		int64_t		i64TotalPacketsIn;		// Total packets received
		int64_t		i64TotalPacketsOut;		// Total packets sent
		int64_t		i64TotalLogins;			// Total login attempts
		int64_t		i64TotalBattlesPlayed;	// Total battles completed

		// Peak tracking
		int			i32PeakCCU;				// Peak CCU since start
		int			i32PeakRooms;			// Peak room count

		// Timing
		DWORD		dwStartTime;			// Server start time (GetTickCount)
		DWORD		dwLastMetricsLog;		// Last time metrics were printed

		void Reset()
		{
			i32CCU = i32ActiveRooms = i32ActiveClans = i32ActiveBattles = 0;
			i64TotalPacketsIn = i64TotalPacketsOut = 0;
			i64TotalLogins = i64TotalBattlesPlayed = 0;
			i32PeakCCU = i32PeakRooms = 0;
			dwStartTime = GetTickCount();
			dwLastMetricsLog = dwStartTime;
		}
	};

	ServerMetrics	m_Metrics;

	void			UpdateMetrics();		// Gather and optionally log metrics
	void			IncrementPacketsIn()	{ m_Metrics.i64TotalPacketsIn++; }
	void			IncrementPacketsOut()	{ m_Metrics.i64TotalPacketsOut++; }
	void			IncrementLogins()		{ m_Metrics.i64TotalLogins++; }
	void			IncrementBattles()		{ m_Metrics.i64TotalBattlesPlayed++; }

	// Medal/Quest system
	MedalLoader*	m_pMedalLoader;
	MedalLoader*	GetMedalLoader()		{ return m_pMedalLoader; }
};

extern GameContextMain* g_pContextMain;

#endif // __GAMECONTEXTMAIN_H__
