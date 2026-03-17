#include "pch.h"
#include "MatchmakingSystem.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "RoomManager.h"
#include "Room.h"
#include "GameContextMain.h"
#include "GameProtocol.h"
#include "ServerLog.h"

// ============================================================================
// Matchmaking System Implementation
// ============================================================================

MatchmakingSystem* g_pMatchmaking = nullptr;

// Protocol for matchmaking notifications to client
#define PROTOCOL_MATCHMAKING_STATUS_ACK		0x0F20
#define PROTOCOL_MATCHMAKING_FOUND_ACK		0x0F21
#define PROTOCOL_MATCHMAKING_CANCEL_ACK		0x0F22

#pragma pack(push, 1)
struct MM_STATUS_PACKET
{
	uint16_t	ui16Size;
	uint16_t	ui16Protocol;
	int32_t		i32Result;
	int32_t		i32QueuePosition;
	int32_t		i32EstimatedWait;	// Seconds
	int32_t		i32QueuedPlayers;
};

struct MM_FOUND_PACKET
{
	uint16_t	ui16Size;
	uint16_t	ui16Protocol;
	int32_t		i32Result;
	int32_t		i32RoomIdx;
	int32_t		i32ChannelNum;
	uint8_t		ui8GameMode;
	uint8_t		ui8MapIndex;
	uint8_t		ui8Team;
	uint8_t		ui8Pad;
};
#pragma pack(pop)

// ============================================================================
// Constructor / Destructor
// ============================================================================

MatchmakingSystem::MatchmakingSystem()
	: m_dwLastUpdateTime(0)
	, m_dwLastStatsLogTime(0)
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
		m_Queue[i].Reset();
	m_Stats.Reset();
}

MatchmakingSystem::~MatchmakingSystem()
{
}

// ============================================================================
// Initialize
// ============================================================================

bool MatchmakingSystem::Initialize(const MatchmakingConfig& config)
{
	m_Config = config;
	m_dwLastUpdateTime = GetTickCount();
	m_dwLastStatsLogTime = m_dwLastUpdateTime;

	printf("[Matchmaking] Initialized: Enabled=%s, MinPlayers=%d, ELO Range=%d-%d\n",
		m_Config.bEnabled ? "YES" : "NO",
		m_Config.i32MinPlayersToMatch,
		m_Config.i32InitialELORange,
		m_Config.i32MaxELORange);

	return true;
}

// ============================================================================
// EnqueuePlayer
// ============================================================================

bool MatchmakingSystem::EnqueuePlayer(GameSession* pSession)
{
	if (!m_Config.bEnabled || !pSession)
		return false;

	int i32SessionIdx = pSession->GetIndex();

	// Check if already queued
	if (IsPlayerQueued(i32SessionIdx))
		return false;

	MatchmakingRequest* pSlot = FindFreeSlot();
	if (!pSlot)
	{
		printf("[Matchmaking] Queue full, cannot enqueue player UID=%lld\n",
			pSession->GetUID());
		return false;
	}

	pSlot->i64UID = pSession->GetUID();
	pSlot->i32SessionIdx = i32SessionIdx;
	pSlot->i32Level = pSession->GetLevel();
	pSlot->i32RankId = pSession->GetRankId();

	// Calculate KD ratio
	int deaths = pSession->GetDeaths();
	pSlot->fKDRatio = (deaths > 0) ? (float)pSession->GetKills() / (float)deaths : (float)pSession->GetKills();

	// ELO calculation: base 1000 + level*10 + (kills-deaths)*2
	pSlot->i32ELO = 1000 + pSession->GetLevel() * 10 +
					(pSession->GetKills() - pSession->GetDeaths()) * 2;
	if (pSlot->i32ELO < 100) pSlot->i32ELO = 100;
	if (pSlot->i32ELO > 3000) pSlot->i32ELO = 3000;

	pSlot->ui32Ping = 0;			// Ping will be updated from heartbeat
	pSlot->ui8PreferredMode = 0;	// Any mode
	pSlot->i64PartyLeaderUID = 0;
	pSlot->i32PartySize = 1;
	pSlot->dwQueueStartTime = GetTickCount();
	pSlot->bActive = true;

	InterlockedIncrement(&m_Stats.lQueuedPlayers);

	printf("[Matchmaking] Player UID=%lld queued (ELO=%d, Level=%d)\n",
		pSession->GetUID(), pSlot->i32ELO, pSlot->i32Level);

	// Send initial queue status
	int pos = GetQueuePosition(i32SessionIdx);
	NotifyQueueStatus(i32SessionIdx, pos, 30);	// Estimated 30 seconds

	return true;
}

// ============================================================================
// EnqueueParty
// ============================================================================

bool MatchmakingSystem::EnqueueParty(GameSession** ppSessions, int i32Count,
									  int64_t i64PartyLeaderUID)
{
	if (!m_Config.bEnabled || !ppSessions || i32Count <= 0)
		return false;

	// Check if we have enough queue slots
	int freeSlots = 0;
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (!m_Queue[i].bActive) freeSlots++;
	}

	if (freeSlots < i32Count)
	{
		printf("[Matchmaking] Not enough queue slots for party of %d\n", i32Count);
		return false;
	}

	DWORD dwNow = GetTickCount();

	for (int i = 0; i < i32Count; ++i)
	{
		GameSession* pSession = ppSessions[i];
		if (!pSession) continue;

		if (IsPlayerQueued(pSession->GetIndex()))
			continue;

		MatchmakingRequest* pSlot = FindFreeSlot();
		if (!pSlot) break;

		pSlot->i64UID = pSession->GetUID();
		pSlot->i32SessionIdx = pSession->GetIndex();
		pSlot->i32Level = pSession->GetLevel();
		pSlot->i32RankId = pSession->GetRankId();

		int deaths = pSession->GetDeaths();
		pSlot->fKDRatio = (deaths > 0) ? (float)pSession->GetKills() / (float)deaths : (float)pSession->GetKills();

		pSlot->i32ELO = 1000 + pSession->GetLevel() * 10 +
						(pSession->GetKills() - pSession->GetDeaths()) * 2;
		if (pSlot->i32ELO < 100) pSlot->i32ELO = 100;
		if (pSlot->i32ELO > 3000) pSlot->i32ELO = 3000;

		pSlot->ui32Ping = 0;
		pSlot->ui8PreferredMode = 0;
		pSlot->i64PartyLeaderUID = i64PartyLeaderUID;
		pSlot->i32PartySize = i32Count;
		pSlot->dwQueueStartTime = dwNow;
		pSlot->bActive = true;

		InterlockedIncrement(&m_Stats.lQueuedPlayers);
	}

	printf("[Matchmaking] Party of %d (leader UID=%lld) queued\n",
		i32Count, i64PartyLeaderUID);

	return true;
}

// ============================================================================
// DequeuePlayer / DequeueParty
// ============================================================================

bool MatchmakingSystem::DequeuePlayer(int i32SessionIdx)
{
	MatchmakingRequest* pReq = FindRequest(i32SessionIdx);
	if (!pReq)
		return false;

	printf("[Matchmaking] Player UID=%lld dequeued\n", pReq->i64UID);

	pReq->Reset();
	InterlockedDecrement(&m_Stats.lQueuedPlayers);
	InterlockedIncrement(&m_Stats.lTotalCancellations);

	return true;
}

bool MatchmakingSystem::DequeueParty(int64_t i64PartyLeaderUID)
{
	int removed = 0;
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (m_Queue[i].bActive && m_Queue[i].i64PartyLeaderUID == i64PartyLeaderUID)
		{
			m_Queue[i].Reset();
			InterlockedDecrement(&m_Stats.lQueuedPlayers);
			removed++;
		}
	}

	if (removed > 0)
	{
		InterlockedIncrement(&m_Stats.lTotalCancellations);
		printf("[Matchmaking] Party leader UID=%lld dequeued (%d members)\n",
			i64PartyLeaderUID, removed);
	}

	return (removed > 0);
}

// ============================================================================
// IsPlayerQueued / GetQueuePosition
// ============================================================================

bool MatchmakingSystem::IsPlayerQueued(int i32SessionIdx) const
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (m_Queue[i].bActive && m_Queue[i].i32SessionIdx == i32SessionIdx)
			return true;
	}
	return false;
}

bool MatchmakingSystem::IsPlayerQueued(int64_t i64UID) const
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (m_Queue[i].bActive && m_Queue[i].i64UID == i64UID)
			return true;
	}
	return false;
}

int MatchmakingSystem::GetQueuePosition(int i32SessionIdx) const
{
	int pos = 0;
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (!m_Queue[i].bActive)
			continue;

		pos++;
		if (m_Queue[i].i32SessionIdx == i32SessionIdx)
			return pos;
	}
	return 0;
}

// ============================================================================
// Update - called periodically to form matches
// ============================================================================

void MatchmakingSystem::Update()
{
	if (!m_Config.bEnabled)
		return;

	DWORD dwNow = GetTickCount();

	// Update every 2 seconds
	if (dwNow - m_dwLastUpdateTime < 2000)
		return;

	m_dwLastUpdateTime = dwNow;

	// Cleanup timed-out entries
	CleanupTimeouts(dwNow);

	// Try to form matches
	MatchResult result;
	while (TryFormMatch(&result))
	{
		if (result.bValid)
		{
			if (CreateMatchRoom(&result))
			{
				InterlockedIncrement(&m_Stats.lTotalMatchesFormed);
				InterlockedExchangeAdd(&m_Stats.lTotalPlayersMatched,
					(LONG)result.i32PlayerCount);
			}
		}
		result.Reset();
	}

	// Log stats periodically (every 60 seconds)
	if (dwNow - m_dwLastStatsLogTime >= 60000)
	{
		m_dwLastStatsLogTime = dwNow;
		if (m_Stats.lQueuedPlayers > 0)
		{
			printf("[Matchmaking] Stats: Queued=%ld, Matches=%ld, Matched=%ld, Timeouts=%ld\n",
				m_Stats.lQueuedPlayers, m_Stats.lTotalMatchesFormed,
				m_Stats.lTotalPlayersMatched, m_Stats.lTotalTimeouts);
		}
	}
}

// ============================================================================
// TryFormMatch
// ============================================================================

bool MatchmakingSystem::TryFormMatch(MatchResult* pResult)
{
	if (!pResult)
		return false;

	pResult->Reset();

	DWORD dwNow = GetTickCount();
	int i32QueuedCount = (int)m_Stats.lQueuedPlayers;

	if (i32QueuedCount < m_Config.i32MinPlayersToMatch)
		return false;

	// Find the oldest request as anchor
	int i32AnchorIdx = -1;
	DWORD dwOldest = dwNow;

	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (!m_Queue[i].bActive)
			continue;

		if (m_Queue[i].dwQueueStartTime < dwOldest)
		{
			dwOldest = m_Queue[i].dwQueueStartTime;
			i32AnchorIdx = i;
		}
	}

	if (i32AnchorIdx < 0)
		return false;

	const MatchmakingRequest& anchor = m_Queue[i32AnchorIdx];

	// Collect compatible players
	int matchIndices[MAX_MATCH_PLAYERS];
	int matchCount = 0;

	// Add anchor
	matchIndices[matchCount++] = i32AnchorIdx;

	// Find compatible players
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (i == i32AnchorIdx || !m_Queue[i].bActive)
			continue;

		if (matchCount >= m_Config.i32MaxPlayersPerMatch)
			break;

		if (AreCompatible(anchor, m_Queue[i], dwNow))
		{
			// If this player is part of a party, include all party members
			if (m_Queue[i].i64PartyLeaderUID != 0)
			{
				// Check if all party members can fit
				int partyCount = 0;
				int partyIndices[MAX_MATCH_PLAYERS];

				for (int j = 0; j < MAX_MATCHMAKING_QUEUE; ++j)
				{
					if (m_Queue[j].bActive &&
						m_Queue[j].i64PartyLeaderUID == m_Queue[i].i64PartyLeaderUID)
					{
						if (partyCount < MAX_MATCH_PLAYERS)
							partyIndices[partyCount++] = j;
					}
				}

				// Only add if entire party fits
				if (matchCount + partyCount <= m_Config.i32MaxPlayersPerMatch)
				{
					for (int p = 0; p < partyCount; ++p)
					{
						// Avoid duplicates
						bool dup = false;
						for (int m = 0; m < matchCount; ++m)
						{
							if (matchIndices[m] == partyIndices[p])
							{
								dup = true;
								break;
							}
						}
						if (!dup)
							matchIndices[matchCount++] = partyIndices[p];
					}
				}
			}
			else
			{
				matchIndices[matchCount++] = i;
			}
		}
	}

	// Check if we have enough players
	if (matchCount < m_Config.i32MinPlayersToMatch)
	{
		// Check if anchor has been waiting too long (forced match with fewer players)
		DWORD dwWaitTime = (dwNow - anchor.dwQueueStartTime) / 1000;
		if ((int)dwWaitTime < m_Config.i32QueueTimeout_Sec)
			return false;

		// Force match if at least 2 players
		if (matchCount < 2)
			return false;
	}

	// Build match result
	pResult->i32PlayerCount = matchCount;
	int totalELO = 0;

	for (int i = 0; i < matchCount; ++i)
	{
		int qIdx = matchIndices[i];
		pResult->i32SessionIndices[i] = m_Queue[qIdx].i32SessionIdx;
		pResult->i64UIDs[i] = m_Queue[qIdx].i64UID;
		totalELO += m_Queue[qIdx].i32ELO;
	}

	pResult->i32AverageELO = totalELO / matchCount;

	// Determine game mode (use anchor's preference, or default)
	pResult->ui8GameMode = (anchor.ui8PreferredMode != 0) ?
		anchor.ui8PreferredMode : m_Config.ui8DefaultGameMode;

	// Select map
	pResult->ui8MapIndex = SelectMap(pResult->ui8GameMode);

	// Assign teams (skill-balanced)
	AssignTeams(pResult);

	pResult->bValid = true;

	// Remove matched players from queue
	for (int i = 0; i < matchCount; ++i)
	{
		int qIdx = matchIndices[i];
		m_Queue[qIdx].Reset();
		InterlockedDecrement(&m_Stats.lQueuedPlayers);
	}

	printf("[Matchmaking] Match formed: %d players, mode=%d, map=%d, avgELO=%d\n",
		matchCount, pResult->ui8GameMode, pResult->ui8MapIndex, pResult->i32AverageELO);

	return true;
}

// ============================================================================
// AreCompatible
// ============================================================================

bool MatchmakingSystem::AreCompatible(const MatchmakingRequest& a,
									   const MatchmakingRequest& b,
									   DWORD dwNow) const
{
	// Game mode preference check
	if (a.ui8PreferredMode != 0 && b.ui8PreferredMode != 0 &&
		a.ui8PreferredMode != b.ui8PreferredMode)
		return false;

	// ELO range check (expands over time)
	int eloRangeA = GetCurrentELORange(a, dwNow);
	int eloRangeB = GetCurrentELORange(b, dwNow);
	int eloRange = (eloRangeA > eloRangeB) ? eloRangeA : eloRangeB;

	int eloDiff = abs(a.i32ELO - b.i32ELO);
	if (eloDiff > eloRange)
		return false;

	// Level difference check
	int levelDiff = abs(a.i32Level - b.i32Level);
	if (levelDiff > m_Config.i32MaxLevelDifference)
	{
		// Allow if both have been waiting long enough
		DWORD waitA = (dwNow - a.dwQueueStartTime) / 1000;
		DWORD waitB = (dwNow - b.dwQueueStartTime) / 1000;
		if (waitA < (DWORD)(m_Config.i32QueueTimeout_Sec / 2) &&
			waitB < (DWORD)(m_Config.i32QueueTimeout_Sec / 2))
			return false;
	}

	// Ping difference check (if both have valid ping)
	if (a.ui32Ping > 0 && b.ui32Ping > 0)
	{
		int pingDiff = abs((int)a.ui32Ping - (int)b.ui32Ping);
		if (pingDiff > m_Config.i32MaxPingDifference)
		{
			// Allow if waiting long enough
			DWORD waitA = (dwNow - a.dwQueueStartTime) / 1000;
			if (waitA < (DWORD)(m_Config.i32QueueTimeout_Sec / 2))
				return false;
		}
	}

	return true;
}

// ============================================================================
// GetCurrentELORange
// ============================================================================

int MatchmakingSystem::GetCurrentELORange(const MatchmakingRequest& req, DWORD dwNow) const
{
	int waitSeconds = (int)((dwNow - req.dwQueueStartTime) / 1000);
	int expansions = waitSeconds / m_Config.i32ELOExpandInterval_Sec;
	int range = m_Config.i32InitialELORange + expansions * m_Config.i32ELOExpandStep;

	if (range > m_Config.i32MaxELORange)
		range = m_Config.i32MaxELORange;

	return range;
}

// ============================================================================
// AssignTeams - skill-balanced team assignment
// ============================================================================

void MatchmakingSystem::AssignTeams(MatchResult* pResult)
{
	if (!pResult || pResult->i32PlayerCount <= 0)
		return;

	// Simple greedy algorithm: sort by ELO descending, alternately assign
	// This produces reasonably balanced teams

	// Gather ELOs
	struct PlayerELO
	{
		int idx;
		int elo;
	};

	PlayerELO players[MAX_MATCH_PLAYERS];
	for (int i = 0; i < pResult->i32PlayerCount; ++i)
	{
		players[i].idx = i;
		// Look up ELO from the session
		players[i].elo = pResult->i32AverageELO;	// Default

		// Try to get actual ELO from the queue (already removed, so use average)
		if (g_pGameSessionManager)
		{
			GameSession* pSession = g_pGameSessionManager->GetSession(pResult->i32SessionIndices[i]);
			if (pSession)
			{
				int deaths = pSession->GetDeaths();
				float kd = (deaths > 0) ? (float)pSession->GetKills() / (float)deaths : (float)pSession->GetKills();
				players[i].elo = 1000 + pSession->GetLevel() * 10 +
								(pSession->GetKills() - pSession->GetDeaths()) * 2;
			}
		}
	}

	// Sort by ELO descending (simple insertion sort for small N)
	for (int i = 1; i < pResult->i32PlayerCount; ++i)
	{
		PlayerELO key = players[i];
		int j = i - 1;
		while (j >= 0 && players[j].elo < key.elo)
		{
			players[j + 1] = players[j];
			j--;
		}
		players[j + 1] = key;
	}

	// Alternate assignment: strongest to RED, 2nd strongest to BLUE, etc.
	int redELO = 0, blueELO = 0;
	for (int i = 0; i < pResult->i32PlayerCount; ++i)
	{
		int playerIdx = players[i].idx;

		// Greedy: assign to team with lower total ELO
		if (redELO <= blueELO)
		{
			pResult->i32Teams[playerIdx] = TEAM_RED;
			redELO += players[i].elo;
		}
		else
		{
			pResult->i32Teams[playerIdx] = TEAM_BLUE;
			blueELO += players[i].elo;
		}
	}
}

// ============================================================================
// CreateMatchRoom
// ============================================================================

bool MatchmakingSystem::CreateMatchRoom(MatchResult* pResult)
{
	if (!pResult || !pResult->bValid)
		return false;

	if (!g_pGameSessionManager || !g_pRoomManager)
		return false;

	// Get first valid session as room creator
	GameSession* pCreator = nullptr;
	for (int i = 0; i < pResult->i32PlayerCount; ++i)
	{
		pCreator = g_pGameSessionManager->GetSession(pResult->i32SessionIndices[i]);
		if (pCreator)
			break;
	}

	if (!pCreator)
		return false;

	// Create room info
	GameRoomCreateInfo roomInfo;
	_snprintf_s(roomInfo.szTitle, _TRUNCATE, "Ranked Match #%ld",
		m_Stats.lTotalMatchesFormed + 1);
	roomInfo.szPassword[0] = '\0';
	roomInfo.ui8GameMode = pResult->ui8GameMode;
	roomInfo.ui8MapIndex = pResult->ui8MapIndex;
	roomInfo.ui8MaxPlayers = (uint8_t)pResult->i32PlayerCount;
	roomInfo.ui8RoundType = BATTLE_ROUND_TYPE_5;	// Best of 5 for ranked
	roomInfo.ui8SubType = BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_5;
	roomInfo.ui8TeamBalance = 0;		// Teams already balanced by matchmaking
	roomInfo.ui8WeaponFlag = LOCK_BASIC;
	roomInfo.ui8InfoFlag = 0;

	// Create the room
	int i32RoomIdx = g_pRoomManager->OnCreateRoom(pCreator, &roomInfo);
	if (i32RoomIdx < 0)
	{
		printf("[Matchmaking] Failed to create match room\n");
		return false;
	}

	int i32ChannelNum = pCreator->GetChannelNum();

	// Join all players to the room
	for (int i = 0; i < pResult->i32PlayerCount; ++i)
	{
		GameSession* pSession = g_pGameSessionManager->GetSession(pResult->i32SessionIndices[i]);
		if (!pSession)
			continue;

		// Skip creator (already in room)
		if (pSession == pCreator)
			continue;

		int team = pResult->i32Teams[i];
		g_pRoomManager->OnJoinRoom(pSession, i32ChannelNum, i32RoomIdx, team);
	}

	// Notify all matched players
	for (int i = 0; i < pResult->i32PlayerCount; ++i)
	{
		NotifyMatchFound(pResult->i32SessionIndices[i], i32RoomIdx, i32ChannelNum);
	}

	printf("[Matchmaking] Created match room: idx=%d, ch=%d, %d players\n",
		i32RoomIdx, i32ChannelNum, pResult->i32PlayerCount);

	return true;
}

// ============================================================================
// SelectMap
// ============================================================================

uint8_t MatchmakingSystem::SelectMap(uint8_t ui8GameMode) const
{
	if (m_Config.ui8DefaultMapIndex != 0)
		return m_Config.ui8DefaultMapIndex;

	// Random map selection from valid maps for this mode
	if (g_pContextMain)
	{
		int validMaps[64];
		int validCount = 0;

		for (int i = 0; i < g_pContextMain->m_i32MapCount; ++i)
		{
			if (g_pContextMain->IsMapValidForMode(i, ui8GameMode))
			{
				validMaps[validCount++] = i;
				if (validCount >= 64)
					break;
			}
		}

		if (validCount > 0)
		{
			return (uint8_t)validMaps[GetTickCount() % validCount];
		}
	}

	return 0;	// Default map
}

// ============================================================================
// Notifications
// ============================================================================

void MatchmakingSystem::NotifyQueueStatus(int i32SessionIdx, int i32Position, int i32EstimatedWait)
{
	if (!g_pGameSessionManager)
		return;

	GameSession* pSession = g_pGameSessionManager->GetSession(i32SessionIdx);
	if (!pSession)
		return;

	MM_STATUS_PACKET pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.ui16Size = sizeof(MM_STATUS_PACKET);
	pkt.ui16Protocol = PROTOCOL_MATCHMAKING_STATUS_ACK;
	pkt.i32Result = 0;
	pkt.i32QueuePosition = i32Position;
	pkt.i32EstimatedWait = i32EstimatedWait;
	pkt.i32QueuedPlayers = (int32_t)m_Stats.lQueuedPlayers;

	pSession->SendGamePacket((char*)&pkt, sizeof(pkt));
}

void MatchmakingSystem::NotifyMatchFound(int i32SessionIdx, int i32RoomIdx, int i32ChannelNum)
{
	if (!g_pGameSessionManager)
		return;

	GameSession* pSession = g_pGameSessionManager->GetSession(i32SessionIdx);
	if (!pSession)
		return;

	MM_FOUND_PACKET pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.ui16Size = sizeof(MM_FOUND_PACKET);
	pkt.ui16Protocol = PROTOCOL_MATCHMAKING_FOUND_ACK;
	pkt.i32Result = 0;
	pkt.i32RoomIdx = i32RoomIdx;
	pkt.i32ChannelNum = i32ChannelNum;

	pSession->SendGamePacket((char*)&pkt, sizeof(pkt));
}

// ============================================================================
// CleanupTimeouts
// ============================================================================

void MatchmakingSystem::CleanupTimeouts(DWORD dwNow)
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (!m_Queue[i].bActive)
			continue;

		DWORD dwWait = (dwNow - m_Queue[i].dwQueueStartTime) / 1000;
		if ((int)dwWait > m_Config.i32QueueTimeout_Sec * 2)	// Double timeout = force remove
		{
			printf("[Matchmaking] Player UID=%lld timed out in queue\n", m_Queue[i].i64UID);

			// Send cancel notification
			if (g_pGameSessionManager)
			{
				GameSession* pSession = g_pGameSessionManager->GetSession(m_Queue[i].i32SessionIdx);
				if (pSession)
				{
					MM_STATUS_PACKET pkt;
					memset(&pkt, 0, sizeof(pkt));
					pkt.ui16Size = sizeof(MM_STATUS_PACKET);
					pkt.ui16Protocol = PROTOCOL_MATCHMAKING_CANCEL_ACK;
					pkt.i32Result = 1;	// Timed out
					pkt.i32QueuePosition = 0;
					pkt.i32EstimatedWait = 0;
					pkt.i32QueuedPlayers = (int32_t)m_Stats.lQueuedPlayers;
					pSession->SendGamePacket((char*)&pkt, sizeof(pkt));
				}
			}

			m_Queue[i].Reset();
			InterlockedDecrement(&m_Stats.lQueuedPlayers);
			InterlockedIncrement(&m_Stats.lTotalTimeouts);
		}
	}
}

// ============================================================================
// Find helpers
// ============================================================================

MatchmakingRequest* MatchmakingSystem::FindRequest(int i32SessionIdx)
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (m_Queue[i].bActive && m_Queue[i].i32SessionIdx == i32SessionIdx)
			return &m_Queue[i];
	}
	return nullptr;
}

MatchmakingRequest* MatchmakingSystem::FindRequest(int64_t i64UID)
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (m_Queue[i].bActive && m_Queue[i].i64UID == i64UID)
			return &m_Queue[i];
	}
	return nullptr;
}

MatchmakingRequest* MatchmakingSystem::FindFreeSlot()
{
	for (int i = 0; i < MAX_MATCHMAKING_QUEUE; ++i)
	{
		if (!m_Queue[i].bActive)
			return &m_Queue[i];
	}
	return nullptr;
}
