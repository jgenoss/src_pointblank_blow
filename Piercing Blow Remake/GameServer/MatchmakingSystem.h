#ifndef __MATCHMAKINGSYSTEM_H__
#define __MATCHMAKINGSYSTEM_H__

#pragma once
#include "RoomDef.h"

// ============================================================================
// Matchmaking System
// ============================================================================
// Advanced matchmaking beyond the basic QuickJoin.
// Supports:
// - ELO/skill-based matching
// - Latency-aware grouping
// - Party support (keep groups together)
// - Expanding search radius over time
// - Automatic room creation when match found
// ============================================================================

class GameSession;
class Room;

// ============================================================================
// Matchmaking Request
// ============================================================================

struct MatchmakingRequest
{
	int64_t		i64UID;
	int			i32SessionIdx;
	int			i32Level;
	int			i32RankId;
	float		fKDRatio;
	int			i32ELO;					// Skill rating (default 1000)
	uint32_t	ui32Ping;				// Latency to server in ms
	uint8_t		ui8PreferredMode;		// StageMode preference (0 = any)
	int64_t		i64PartyLeaderUID;		// 0 = solo player
	int			i32PartySize;			// 1 = solo
	DWORD		dwQueueStartTime;		// GetTickCount() when queued
	bool		bActive;				// Is this request in the queue

	void Reset()
	{
		i64UID = 0;
		i32SessionIdx = -1;
		i32Level = 0;
		i32RankId = 0;
		fKDRatio = 1.0f;
		i32ELO = 1000;
		ui32Ping = 0;
		ui8PreferredMode = 0;
		i64PartyLeaderUID = 0;
		i32PartySize = 1;
		dwQueueStartTime = 0;
		bActive = false;
	}
};

// ============================================================================
// Matchmaking Config
// ============================================================================

struct MatchmakingConfig
{
	bool		bEnabled;
	int			i32MinPlayersToMatch;		// Minimum players to form a match (default 4)
	int			i32MaxPlayersPerMatch;		// Maximum players per match (default 16)
	int			i32InitialELORange;			// Initial ELO search range (default 100)
	int			i32MaxELORange;				// Maximum ELO search range (default 500)
	int			i32ELOExpandInterval_Sec;	// Expand search every N seconds (default 15)
	int			i32ELOExpandStep;			// ELO range expansion step (default 50)
	int			i32MaxPingDifference;		// Max ping difference in ms (default 100)
	int			i32MaxLevelDifference;		// Max level difference (default 10)
	int			i32QueueTimeout_Sec;		// Max time in queue before forced match (default 120)
	uint8_t		ui8DefaultGameMode;			// Default mode if no preference (STAGE_MODE_DEATHMATCH)
	uint8_t		ui8DefaultMapIndex;			// Default map (0 = random)

	MatchmakingConfig()
		: bEnabled(true)
		, i32MinPlayersToMatch(4)
		, i32MaxPlayersPerMatch(16)
		, i32InitialELORange(100)
		, i32MaxELORange(500)
		, i32ELOExpandInterval_Sec(15)
		, i32ELOExpandStep(50)
		, i32MaxPingDifference(100)
		, i32MaxLevelDifference(10)
		, i32QueueTimeout_Sec(120)
		, ui8DefaultGameMode(STAGE_MODE_DEATHMATCH)
		, ui8DefaultMapIndex(0)
	{
	}
};

// ============================================================================
// Match Result (formed match ready for room creation)
// ============================================================================

#define MAX_MATCH_PLAYERS	16

struct MatchResult
{
	int			i32PlayerCount;
	int			i32SessionIndices[MAX_MATCH_PLAYERS];
	int64_t		i64UIDs[MAX_MATCH_PLAYERS];
	int			i32Teams[MAX_MATCH_PLAYERS];		// 0=RED, 1=BLUE
	uint8_t		ui8GameMode;
	uint8_t		ui8MapIndex;
	int			i32AverageELO;
	bool		bValid;

	void Reset()
	{
		i32PlayerCount = 0;
		memset(i32SessionIndices, -1, sizeof(i32SessionIndices));
		memset(i64UIDs, 0, sizeof(i64UIDs));
		memset(i32Teams, 0, sizeof(i32Teams));
		ui8GameMode = STAGE_MODE_DEATHMATCH;
		ui8MapIndex = 0;
		i32AverageELO = 1000;
		bValid = false;
	}
};

// ============================================================================
// Matchmaking Queue Statistics
// ============================================================================

struct MatchmakingStats
{
	volatile LONG	lQueuedPlayers;			// Currently in queue
	volatile LONG	lTotalMatchesFormed;	// Total matches created
	volatile LONG	lTotalPlayersMatched;	// Total players matched
	volatile LONG	lTotalTimeouts;			// Players who timed out
	volatile LONG	lTotalCancellations;	// Players who cancelled
	DWORD			dwAvgWaitTime;			// Average wait time in ms

	void Reset()
	{
		lQueuedPlayers = 0;
		lTotalMatchesFormed = 0;
		lTotalPlayersMatched = 0;
		lTotalTimeouts = 0;
		lTotalCancellations = 0;
		dwAvgWaitTime = 0;
	}
};

// ============================================================================
// MatchmakingSystem
// ============================================================================

#define MAX_MATCHMAKING_QUEUE	256

class MatchmakingSystem
{
public:
	MatchmakingSystem();
	~MatchmakingSystem();

	// Initialize
	bool		Initialize(const MatchmakingConfig& config);

	// Queue management
	bool		EnqueuePlayer(GameSession* pSession);
	bool		EnqueueParty(GameSession** ppSessions, int i32Count, int64_t i64PartyLeaderUID);
	bool		DequeuePlayer(int i32SessionIdx);
	bool		DequeueParty(int64_t i64PartyLeaderUID);

	// Check if player is in queue
	bool		IsPlayerQueued(int i32SessionIdx) const;
	bool		IsPlayerQueued(int64_t i64UID) const;

	// Get queue position (1-based, 0 = not found)
	int			GetQueuePosition(int i32SessionIdx) const;

	// Periodic update - attempts to form matches
	// Should be called from GameServerContext::OnUpdate every second
	void		Update();

	// Configuration
	const MatchmakingConfig& GetConfig() const { return m_Config; }
	void		SetConfig(const MatchmakingConfig& config) { m_Config = config; }
	bool		IsEnabled() const { return m_Config.bEnabled; }

	// Statistics
	const MatchmakingStats& GetStats() const { return m_Stats; }
	int			GetQueuedCount() const { return (int)m_Stats.lQueuedPlayers; }

private:
	// Try to form a match from the current queue
	bool		TryFormMatch(MatchResult* pResult);

	// Check if two requests are compatible for matching
	bool		AreCompatible(const MatchmakingRequest& a, const MatchmakingRequest& b,
							  DWORD dwNow) const;

	// Get current ELO range for a request (expands over time)
	int			GetCurrentELORange(const MatchmakingRequest& req, DWORD dwNow) const;

	// Assign teams using skill-balanced approach
	void		AssignTeams(MatchResult* pResult);

	// Create a room for a formed match
	bool		CreateMatchRoom(MatchResult* pResult);

	// Select map for the match
	uint8_t		SelectMap(uint8_t ui8GameMode) const;

	// Notify players in queue about status
	void		NotifyQueueStatus(int i32SessionIdx, int i32Position, int i32EstimatedWait);
	void		NotifyMatchFound(int i32SessionIdx, int i32RoomIdx, int i32ChannelNum);

	// Cleanup timed-out entries
	void		CleanupTimeouts(DWORD dwNow);

	// Find request in queue
	MatchmakingRequest*	FindRequest(int i32SessionIdx);
	MatchmakingRequest*	FindRequest(int64_t i64UID);
	MatchmakingRequest*	FindFreeSlot();

private:
	MatchmakingConfig	m_Config;
	MatchmakingRequest	m_Queue[MAX_MATCHMAKING_QUEUE];
	MatchmakingStats	m_Stats;
	DWORD				m_dwLastUpdateTime;
	DWORD				m_dwLastStatsLogTime;
};

extern MatchmakingSystem* g_pMatchmaking;

#endif // __MATCHMAKINGSYSTEM_H__
