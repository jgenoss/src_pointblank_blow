#include "pch.h"
#include "ObserverSystem.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "Room.h"

// ============================================================================
// Custom protocol IDs for observer spectator feed
// These are server-side only (GameServer -> Client for observer data).
// We define them as offsets from PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK
// to keep them in the observer protocol region.
// ============================================================================
#define PROTOCOL_OBSERVER_POSITION_UPDATE_NTF		(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 2)
#define PROTOCOL_OBSERVER_KILLFEED_NTF				(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 4)
#define PROTOCOL_OBSERVER_SCORE_UPDATE_NTF			(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 6)
#define PROTOCOL_OBSERVER_ROUND_EVENT_NTF			(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 8)
#define PROTOCOL_OBSERVER_VIEW_MODE_ACK				(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 10)
#define PROTOCOL_OBSERVER_FOLLOW_PLAYER_ACK			(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 12)
#define PROTOCOL_OBSERVER_SCOREBOARD_ACK			(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 14)
#define PROTOCOL_OBSERVER_JOIN_NTF					(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 16)
#define PROTOCOL_OBSERVER_LEAVE_NTF					(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 18)

// ============================================================================
// Construction / Initialization
// ============================================================================

ObserverSystem::ObserverSystem()
	: m_pRoom(nullptr)
	, m_i32ObserverCount(0)
	, m_dwAntiGhostDelayMs(OBSERVER_ANTIGHOST_DELAY_MS)
	, m_i32PositionWriteIdx(0)
	, m_i32KillFeedWriteIdx(0)
	, m_i32KillFeedReadIdx(0)
	, m_i32PendingRedScore(0)
	, m_i32PendingBlueScore(0)
	, m_i32PendingRound(0)
	, m_bScoreDirty(false)
	, m_i32RoundEventWriteIdx(0)
	, m_i32RoundEventReadIdx(0)
	, m_dwLastPositionBroadcast(0)
{
	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
		m_Observers[i].Reset();

	memset(m_PositionBuffer, 0, sizeof(m_PositionBuffer));
	memset(m_KillFeedBuffer, 0, sizeof(m_KillFeedBuffer));
	memset(m_RoundEventBuffer, 0, sizeof(m_RoundEventBuffer));
}

ObserverSystem::~ObserverSystem()
{
}

void ObserverSystem::Init(Room* pRoom)
{
	m_pRoom = pRoom;
	Reset();
}

void ObserverSystem::Reset()
{
	m_i32ObserverCount = 0;
	m_i32PositionWriteIdx = 0;
	m_i32KillFeedWriteIdx = 0;
	m_i32KillFeedReadIdx = 0;
	m_i32RoundEventWriteIdx = 0;
	m_i32RoundEventReadIdx = 0;
	m_bScoreDirty = false;
	m_i32PendingRedScore = 0;
	m_i32PendingBlueScore = 0;
	m_i32PendingRound = 0;
	m_dwLastPositionBroadcast = 0;

	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
		m_Observers[i].Reset();

	memset(m_PositionBuffer, 0, sizeof(m_PositionBuffer));
	memset(m_KillFeedBuffer, 0, sizeof(m_KillFeedBuffer));
	memset(m_RoundEventBuffer, 0, sizeof(m_RoundEventBuffer));
}

// ============================================================================
// Observer Join / Leave
// ============================================================================

bool ObserverSystem::OnObserverJoin(GameSession* pSession, int i32SlotIdx)
{
	if (!pSession || !m_pRoom)
		return false;

	if (m_i32ObserverCount >= MAX_OBSERVERS_PER_ROOM)
	{
		printf("[ObserverSystem] Room=%d: observer slots full (%d/%d)\n",
			m_pRoom->GetRoomIdx(), m_i32ObserverCount, MAX_OBSERVERS_PER_ROOM);
		return false;
	}

	// Check if already an observer
	if (FindObserver(i32SlotIdx) != nullptr)
	{
		printf("[ObserverSystem] Room=%d: slot %d already an observer\n",
			m_pRoom->GetRoomIdx(), i32SlotIdx);
		return false;
	}

	int emptyIdx = FindEmptyObserverSlot();
	if (emptyIdx < 0)
		return false;

	DWORD dwNow = GetTickCount();

	ObserverState& obs = m_Observers[emptyIdx];
	obs.i32SessionIdx = pSession->GetIndex();
	obs.i32SlotIdx = i32SlotIdx;
	obs.i32FollowSlot = -1;
	obs.eViewMode = OBSERVER_VIEW_FREE;
	obs.bShowScoreboard = false;
	obs.bShowMinimap = true;
	obs.bActive = true;
	obs.dwLastUpdateTime = dwNow;
	obs.dwJoinTime = dwNow;

	m_i32ObserverCount++;

	// Notify all players in room that an observer joined
	{
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_OBSERVER_JOIN_NTF;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

		uint8_t slotByte = (uint8_t)i32SlotIdx;
		memcpy(buffer + offset, &slotByte, sizeof(uint8_t));	offset += sizeof(uint8_t);

		uint8_t obsCount = (uint8_t)m_i32ObserverCount;
		memcpy(buffer + offset, &obsCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		m_pRoom->SendToAll(&packet);
	}

	// Send current score state to the newly joined observer
	if (m_pRoom->GetRoomState() >= ROOM_STATE_BATTLE)
	{
		const GameRoomScore& score = m_pRoom->GetScore();
		i3NetworkPacket scorePacket;
		char scoreBuf[32];
		int sOff = 0;

		uint16_t sSize = 0;
		uint16_t sProto = PROTOCOL_OBSERVER_SCORE_UPDATE_NTF;
		sOff += sizeof(uint16_t);
		memcpy(scoreBuf + sOff, &sProto, sizeof(uint16_t));	sOff += sizeof(uint16_t);

		int32_t redScore = score.i32RedScore;
		int32_t blueScore = score.i32BlueScore;
		int32_t round = score.i32NowRound;
		memcpy(scoreBuf + sOff, &redScore, sizeof(int32_t));	sOff += sizeof(int32_t);
		memcpy(scoreBuf + sOff, &blueScore, sizeof(int32_t));	sOff += sizeof(int32_t);
		memcpy(scoreBuf + sOff, &round, sizeof(int32_t));		sOff += sizeof(int32_t);

		sSize = (uint16_t)sOff;
		memcpy(scoreBuf, &sSize, sizeof(uint16_t));

		scorePacket.SetPacketData(scoreBuf, sOff);
		SendToObserver(i32SlotIdx, &scorePacket);
	}

	printf("[ObserverSystem] Room=%d: observer joined slot=%d, count=%d\n",
		m_pRoom->GetRoomIdx(), i32SlotIdx, m_i32ObserverCount);

	return true;
}

void ObserverSystem::OnObserverLeave(GameSession* pSession)
{
	if (!pSession || !m_pRoom)
		return;

	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		if (m_Observers[i].bActive && m_Observers[i].i32SessionIdx == pSession->GetIndex())
		{
			int slotIdx = m_Observers[i].i32SlotIdx;
			m_Observers[i].Reset();
			m_i32ObserverCount--;
			if (m_i32ObserverCount < 0)
				m_i32ObserverCount = 0;

			// Notify room
			{
				i3NetworkPacket packet;
				char buffer[32];
				int offset = 0;

				uint16_t size = 0;
				uint16_t proto = PROTOCOL_OBSERVER_LEAVE_NTF;
				offset += sizeof(uint16_t);
				memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

				uint8_t slotByte = (uint8_t)slotIdx;
				memcpy(buffer + offset, &slotByte, sizeof(uint8_t));	offset += sizeof(uint8_t);

				uint8_t obsCount = (uint8_t)m_i32ObserverCount;
				memcpy(buffer + offset, &obsCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

				size = (uint16_t)offset;
				memcpy(buffer, &size, sizeof(uint16_t));

				packet.SetPacketData(buffer, offset);
				m_pRoom->SendToAll(&packet);
			}

			printf("[ObserverSystem] Room=%d: observer left slot=%d, count=%d\n",
				m_pRoom->GetRoomIdx(), slotIdx, m_i32ObserverCount);
			return;
		}
	}
}

// ============================================================================
// View Mode / Follow
// ============================================================================

bool ObserverSystem::OnViewModeChange(int i32SlotIdx, ObserverViewMode eMode)
{
	if (eMode < 0 || eMode >= OBSERVER_VIEW_MAX)
		return false;

	ObserverState* pObs = FindObserver(i32SlotIdx);
	if (!pObs)
		return false;

	pObs->eViewMode = eMode;

	// If switching to free or overhead, clear follow target
	if (eMode == OBSERVER_VIEW_FREE || eMode == OBSERVER_VIEW_OVERHEAD)
		pObs->i32FollowSlot = -1;

	// Send ACK back to observer
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_OBSERVER_VIEW_MODE_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

		uint8_t modeVal = (uint8_t)eMode;
		memcpy(buffer + offset, &modeVal, sizeof(uint8_t));		offset += sizeof(uint8_t);

		int8_t followSlot = (int8_t)pObs->i32FollowSlot;
		memcpy(buffer + offset, &followSlot, sizeof(int8_t));	offset += sizeof(int8_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToObserver(i32SlotIdx, &packet);
	}

	printf("[ObserverSystem] Room=%d: slot=%d view mode -> %d\n",
		m_pRoom ? m_pRoom->GetRoomIdx() : -1, i32SlotIdx, (int)eMode);

	return true;
}

bool ObserverSystem::OnFollowPlayer(int i32SlotIdx, int i32TargetSlot)
{
	ObserverState* pObs = FindObserver(i32SlotIdx);
	if (!pObs || !m_pRoom)
		return false;

	int32_t result = 0;

	// Validate target slot
	if (i32TargetSlot < -1 || i32TargetSlot >= SLOT_MAX_COUNT)
	{
		result = 1;		// Invalid slot
	}
	else if (i32TargetSlot >= 0)
	{
		// Check that target is an active player (not empty, not observer)
		if (m_pRoom->IsSlotEmpty(i32TargetSlot))
		{
			result = 2;	// Empty slot
		}
		else if (m_pRoom->GetSlotInfo(i32TargetSlot).bIsObserver)
		{
			result = 3;	// Cannot follow another observer
		}
		else
		{
			pObs->i32FollowSlot = i32TargetSlot;

			// Auto-switch to follow mode if in free cam
			if (pObs->eViewMode == OBSERVER_VIEW_FREE || pObs->eViewMode == OBSERVER_VIEW_OVERHEAD)
				pObs->eViewMode = OBSERVER_VIEW_FOLLOW;
		}
	}
	else
	{
		// i32TargetSlot == -1 means unfollow (go to free cam)
		pObs->i32FollowSlot = -1;
		pObs->eViewMode = OBSERVER_VIEW_FREE;
	}

	// Send ACK
	{
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_OBSERVER_FOLLOW_PLAYER_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

		int8_t targetByte = (int8_t)pObs->i32FollowSlot;
		memcpy(buffer + offset, &targetByte, sizeof(int8_t));	offset += sizeof(int8_t);

		uint8_t modeVal = (uint8_t)pObs->eViewMode;
		memcpy(buffer + offset, &modeVal, sizeof(uint8_t));		offset += sizeof(uint8_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToObserver(i32SlotIdx, &packet);
	}

	return (result == 0);
}

bool ObserverSystem::OnToggleScoreboard(int i32SlotIdx, bool bShow)
{
	ObserverState* pObs = FindObserver(i32SlotIdx);
	if (!pObs)
		return false;

	pObs->bShowScoreboard = bShow;

	// Send ACK with full scoreboard data if showing
	{
		i3NetworkPacket packet;
		char buffer[256];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_OBSERVER_SCOREBOARD_ACK;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

		int32_t result = 0;
		memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

		uint8_t showByte = bShow ? 1 : 0;
		memcpy(buffer + offset, &showByte, sizeof(uint8_t));	offset += sizeof(uint8_t);

		if (bShow && m_pRoom)
		{
			// Send per-slot K/D/A stats
			for (int i = 0; i < SLOT_MAX_COUNT; i++)
			{
				const Room::SlotBattleStats& stats = m_pRoom->GetSlotBattleStats(i);
				int16_t kills = (int16_t)stats.i32Kills;
				int16_t deaths = (int16_t)stats.i32Deaths;
				int16_t assists = (int16_t)stats.i32Assists;
				uint8_t alive = stats.bAlive ? 1 : 0;

				memcpy(buffer + offset, &kills, sizeof(int16_t));	offset += sizeof(int16_t);
				memcpy(buffer + offset, &deaths, sizeof(int16_t));	offset += sizeof(int16_t);
				memcpy(buffer + offset, &assists, sizeof(int16_t));	offset += sizeof(int16_t);
				memcpy(buffer + offset, &alive, sizeof(uint8_t));	offset += sizeof(uint8_t);
			}
		}

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToObserver(i32SlotIdx, &packet);
	}

	return true;
}

// ============================================================================
// Event Enqueue (called from Room game logic)
// ============================================================================

void ObserverSystem::EnqueuePositionUpdate(int i32Slot, float fX, float fY, float fZ,
										   float fYaw, uint8_t ui8Team, bool bAlive)
{
	if (m_i32ObserverCount <= 0)
		return;

	ObserverPositionEntry& entry = m_PositionBuffer[m_i32PositionWriteIdx];
	entry.dwTimestamp = GetTickCount();
	entry.i32Slot = i32Slot;
	entry.fX = fX;
	entry.fY = fY;
	entry.fZ = fZ;
	entry.fYaw = fYaw;
	entry.ui8Team = ui8Team;
	entry.bAlive = bAlive;

	m_i32PositionWriteIdx = (m_i32PositionWriteIdx + 1) % OBSERVER_POSITION_BUFFER_SIZE;
}

void ObserverSystem::EnqueueKillFeed(int i32KillerSlot, int i32DeadSlot,
									 uint32_t ui32WeaponId, uint8_t ui8HitPart,
									 uint8_t ui8MultiKill)
{
	if (m_i32ObserverCount <= 0)
		return;

	ObserverKillFeedEntry& entry = m_KillFeedBuffer[m_i32KillFeedWriteIdx];
	entry.dwTimestamp = GetTickCount();
	entry.i32KillerSlot = i32KillerSlot;
	entry.i32DeadSlot = i32DeadSlot;
	entry.ui32WeaponId = ui32WeaponId;
	entry.ui8HitPart = ui8HitPart;
	entry.ui8MultiKill = ui8MultiKill;

	m_i32KillFeedWriteIdx = (m_i32KillFeedWriteIdx + 1) % 64;

	// Prevent overrun: advance read index if writer catches up
	if (m_i32KillFeedWriteIdx == m_i32KillFeedReadIdx)
		m_i32KillFeedReadIdx = (m_i32KillFeedReadIdx + 1) % 64;
}

void ObserverSystem::EnqueueScoreUpdate(int i32RedScore, int i32BlueScore, int i32Round)
{
	m_i32PendingRedScore = i32RedScore;
	m_i32PendingBlueScore = i32BlueScore;
	m_i32PendingRound = i32Round;
	m_bScoreDirty = true;
}

void ObserverSystem::EnqueueRoundEvent(uint8_t ui8EventType, int i32WinnerTeam,
									   int i32ActorSlot, uint8_t ui8Extra)
{
	if (m_i32ObserverCount <= 0)
		return;

	ObserverRoundEventEntry& entry = m_RoundEventBuffer[m_i32RoundEventWriteIdx];
	entry.dwTimestamp = GetTickCount();
	entry.ui8EventType = ui8EventType;
	entry.i32WinnerTeam = i32WinnerTeam;
	entry.i32ActorSlot = i32ActorSlot;
	entry.ui8Extra = ui8Extra;

	m_i32RoundEventWriteIdx = (m_i32RoundEventWriteIdx + 1) % 16;

	if (m_i32RoundEventWriteIdx == m_i32RoundEventReadIdx)
		m_i32RoundEventReadIdx = (m_i32RoundEventReadIdx + 1) % 16;
}

// ============================================================================
// Periodic Update (called from Room::OnUpdateRoom)
// ============================================================================

void ObserverSystem::Update(DWORD dwNow)
{
	if (m_i32ObserverCount <= 0 || !m_pRoom)
		return;

	// Only send updates during active battle states
	uint8_t roomState = m_pRoom->GetRoomState();
	if (roomState < ROOM_STATE_BATTLE)
		return;

	// Position updates at OBSERVER_UPDATE_INTERVAL_MS rate
	if (dwNow - m_dwLastPositionBroadcast >= OBSERVER_UPDATE_INTERVAL_MS)
	{
		SendPositionUpdate(dwNow);
		m_dwLastPositionBroadcast = dwNow;
	}

	// Process delayed kill feed
	SendKillFeedUpdate(dwNow);

	// Process delayed round events
	SendRoundEvent(dwNow);

	// Score updates (sent immediately when dirty, no delay needed)
	if (m_bScoreDirty)
	{
		SendScoreUpdate();
		m_bScoreDirty = false;
	}

	// If an observer is following a dead player, auto-switch to killer or next alive
	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		ObserverState& obs = m_Observers[i];
		if (!obs.bActive || obs.i32FollowSlot < 0)
			continue;

		if (obs.eViewMode == OBSERVER_VIEW_FOLLOW || obs.eViewMode == OBSERVER_VIEW_FIRST_PERSON)
		{
			if (!m_pRoom->IsPlayerAlive(obs.i32FollowSlot))
			{
				// Try to find next alive player on same team
				int startSlot = obs.i32FollowSlot;
				uint8_t team = m_pRoom->GetSlotInfo(obs.i32FollowSlot).ui8Team;
				int nextSlot = -1;

				for (int s = 1; s < SLOT_MAX_COUNT; s++)
				{
					int checkSlot = (startSlot + s) % SLOT_MAX_COUNT;
					if (!m_pRoom->IsSlotEmpty(checkSlot) &&
						!m_pRoom->GetSlotInfo(checkSlot).bIsObserver &&
						m_pRoom->IsPlayerAlive(checkSlot) &&
						m_pRoom->GetSlotInfo(checkSlot).ui8Team == team)
					{
						nextSlot = checkSlot;
						break;
					}
				}

				if (nextSlot >= 0)
				{
					obs.i32FollowSlot = nextSlot;

					// Notify observer of auto-switch
					i3NetworkPacket pkt;
					char buf[16];
					int off = 0;

					uint16_t sz = 0;
					uint16_t pr = PROTOCOL_OBSERVER_FOLLOW_PLAYER_ACK;
					off += sizeof(uint16_t);
					memcpy(buf + off, &pr, sizeof(uint16_t));		off += sizeof(uint16_t);

					int32_t res = 0;
					memcpy(buf + off, &res, sizeof(int32_t));		off += sizeof(int32_t);

					int8_t tgt = (int8_t)nextSlot;
					memcpy(buf + off, &tgt, sizeof(int8_t));		off += sizeof(int8_t);

					uint8_t mode = (uint8_t)obs.eViewMode;
					memcpy(buf + off, &mode, sizeof(uint8_t));		off += sizeof(uint8_t);

					sz = (uint16_t)off;
					memcpy(buf, &sz, sizeof(uint16_t));

					pkt.SetPacketData(buf, off);
					SendToObserver(obs.i32SlotIdx, &pkt);
				}
			}
		}
	}
}

// ============================================================================
// Internal: Position Update Broadcast
// ============================================================================

void ObserverSystem::SendPositionUpdate(DWORD dwNow)
{
	if (!m_pRoom || m_i32ObserverCount <= 0)
		return;

	// Determine the cutoff timestamp for anti-ghosting delay
	DWORD dwDelayCutoff = dwNow - m_dwAntiGhostDelayMs;

	// Collect the latest delayed position for each active slot
	// We scan the circular buffer and keep the most recent entry per slot
	// that is old enough (past the anti-ghost delay)
	struct SlotPos
	{
		bool	bValid;
		float	fX, fY, fZ, fYaw;
		uint8_t	ui8Team;
		bool	bAlive;
	};

	SlotPos latestPos[SLOT_MAX_COUNT];
	memset(latestPos, 0, sizeof(latestPos));

	for (int i = 0; i < OBSERVER_POSITION_BUFFER_SIZE; i++)
	{
		const ObserverPositionEntry& entry = m_PositionBuffer[i];
		if (entry.dwTimestamp == 0)
			continue;

		// Anti-ghosting: only send positions older than the delay
		if (entry.dwTimestamp > dwDelayCutoff)
			continue;

		int slot = entry.i32Slot;
		if (slot < 0 || slot >= SLOT_MAX_COUNT)
			continue;

		// Keep the newest valid (delayed) entry per slot
		if (!latestPos[slot].bValid || entry.dwTimestamp > 0)
		{
			latestPos[slot].bValid = true;
			latestPos[slot].fX = entry.fX;
			latestPos[slot].fY = entry.fY;
			latestPos[slot].fZ = entry.fZ;
			latestPos[slot].fYaw = entry.fYaw;
			latestPos[slot].ui8Team = entry.ui8Team;
			latestPos[slot].bAlive = entry.bAlive;
		}
	}

	// Build position update packet
	// Format: proto(2) + slotCount(1) + [slotIdx(1) + team(1) + alive(1) + x(4) + y(4) + z(4) + yaw(4)] * N
	i3NetworkPacket packet;
	char buffer[1024];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_OBSERVER_POSITION_UPDATE_NTF;
	offset += sizeof(uint16_t);		// placeholder for size
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// Count valid slots first
	uint8_t slotCount = 0;
	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (latestPos[i].bValid)
			slotCount++;
	}

	memcpy(buffer + offset, &slotCount, sizeof(uint8_t));	offset += sizeof(uint8_t);

	for (int i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (!latestPos[i].bValid)
			continue;

		uint8_t slotIdx = (uint8_t)i;
		memcpy(buffer + offset, &slotIdx, sizeof(uint8_t));				offset += sizeof(uint8_t);
		memcpy(buffer + offset, &latestPos[i].ui8Team, sizeof(uint8_t));	offset += sizeof(uint8_t);

		uint8_t alive = latestPos[i].bAlive ? 1 : 0;
		memcpy(buffer + offset, &alive, sizeof(uint8_t));					offset += sizeof(uint8_t);

		memcpy(buffer + offset, &latestPos[i].fX, sizeof(float));			offset += sizeof(float);
		memcpy(buffer + offset, &latestPos[i].fY, sizeof(float));			offset += sizeof(float);
		memcpy(buffer + offset, &latestPos[i].fZ, sizeof(float));			offset += sizeof(float);
		memcpy(buffer + offset, &latestPos[i].fYaw, sizeof(float));		offset += sizeof(float);

		// Safety check
		if (offset > (int)(sizeof(buffer) - 20))
			break;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	if (slotCount > 0)
	{
		packet.SetPacketData(buffer, offset);
		SendToObservers(&packet);
	}
}

// ============================================================================
// Internal: Kill Feed Update
// ============================================================================

void ObserverSystem::SendKillFeedUpdate(DWORD dwNow)
{
	if (!m_pRoom || m_i32ObserverCount <= 0)
		return;

	DWORD dwDelayCutoff = dwNow - m_dwAntiGhostDelayMs;

	// Process all pending kill feed entries that are old enough
	while (m_i32KillFeedReadIdx != m_i32KillFeedWriteIdx)
	{
		const ObserverKillFeedEntry& entry = m_KillFeedBuffer[m_i32KillFeedReadIdx];

		// Anti-ghosting delay
		if (entry.dwTimestamp > dwDelayCutoff)
			break;

		// Build kill feed packet
		// Format: proto(2) + killerSlot(1) + deadSlot(1) + weaponId(4) + hitPart(1) + multiKill(1)
		i3NetworkPacket packet;
		char buffer[32];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_OBSERVER_KILLFEED_NTF;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

		int8_t killerSlot = (int8_t)entry.i32KillerSlot;
		int8_t deadSlot = (int8_t)entry.i32DeadSlot;
		memcpy(buffer + offset, &killerSlot, sizeof(int8_t));		offset += sizeof(int8_t);
		memcpy(buffer + offset, &deadSlot, sizeof(int8_t));			offset += sizeof(int8_t);
		memcpy(buffer + offset, &entry.ui32WeaponId, sizeof(uint32_t));	offset += sizeof(uint32_t);
		memcpy(buffer + offset, &entry.ui8HitPart, sizeof(uint8_t));	offset += sizeof(uint8_t);
		memcpy(buffer + offset, &entry.ui8MultiKill, sizeof(uint8_t));	offset += sizeof(uint8_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToObservers(&packet);

		m_i32KillFeedReadIdx = (m_i32KillFeedReadIdx + 1) % 64;
	}
}

// ============================================================================
// Internal: Score Update
// ============================================================================

void ObserverSystem::SendScoreUpdate()
{
	if (!m_pRoom || m_i32ObserverCount <= 0)
		return;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_OBSERVER_SCORE_UPDATE_NTF;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

	int32_t redScore = m_i32PendingRedScore;
	int32_t blueScore = m_i32PendingBlueScore;
	int32_t round = m_i32PendingRound;
	memcpy(buffer + offset, &redScore, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &blueScore, sizeof(int32_t));		offset += sizeof(int32_t);
	memcpy(buffer + offset, &round, sizeof(int32_t));			offset += sizeof(int32_t);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendToObservers(&packet);
}

// ============================================================================
// Internal: Round Event
// ============================================================================

void ObserverSystem::SendRoundEvent(DWORD dwNow)
{
	if (!m_pRoom || m_i32ObserverCount <= 0)
		return;

	DWORD dwDelayCutoff = dwNow - m_dwAntiGhostDelayMs;

	while (m_i32RoundEventReadIdx != m_i32RoundEventWriteIdx)
	{
		const ObserverRoundEventEntry& entry = m_RoundEventBuffer[m_i32RoundEventReadIdx];

		if (entry.dwTimestamp > dwDelayCutoff)
			break;

		// Build round event packet
		// Format: proto(2) + eventType(1) + winnerTeam(1) + actorSlot(1) + extra(1)
		i3NetworkPacket packet;
		char buffer[16];
		int offset = 0;

		uint16_t size = 0;
		uint16_t proto = PROTOCOL_OBSERVER_ROUND_EVENT_NTF;
		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &proto, sizeof(uint16_t));			offset += sizeof(uint16_t);

		memcpy(buffer + offset, &entry.ui8EventType, sizeof(uint8_t));	offset += sizeof(uint8_t);

		int8_t winner = (int8_t)entry.i32WinnerTeam;
		memcpy(buffer + offset, &winner, sizeof(int8_t));				offset += sizeof(int8_t);

		int8_t actor = (int8_t)entry.i32ActorSlot;
		memcpy(buffer + offset, &actor, sizeof(int8_t));				offset += sizeof(int8_t);

		memcpy(buffer + offset, &entry.ui8Extra, sizeof(uint8_t));		offset += sizeof(uint8_t);

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		packet.SetPacketData(buffer, offset);
		SendToObservers(&packet);

		m_i32RoundEventReadIdx = (m_i32RoundEventReadIdx + 1) % 16;
	}
}

// ============================================================================
// Internal: Packet Sending Helpers
// ============================================================================

void ObserverSystem::SendToObservers(i3NetworkPacket* pPacket)
{
	if (!pPacket || !m_pRoom)
		return;

	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		if (!m_Observers[i].bActive)
			continue;

		int slotIdx = m_Observers[i].i32SlotIdx;
		if (slotIdx < 0 || slotIdx >= SLOT_MAX_COUNT)
			continue;

		GameSession* pSession = m_pRoom->GetSlotSession(slotIdx);
		if (pSession)
			pSession->SendMessage(pPacket);
	}
}

void ObserverSystem::SendToObserver(int i32SlotIdx, i3NetworkPacket* pPacket)
{
	if (!pPacket || !m_pRoom)
		return;

	if (i32SlotIdx < 0 || i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	GameSession* pSession = m_pRoom->GetSlotSession(i32SlotIdx);
	if (pSession)
		pSession->SendMessage(pPacket);
}

// ============================================================================
// Queries
// ============================================================================

bool ObserverSystem::IsObserver(int i32SlotIdx) const
{
	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		if (m_Observers[i].bActive && m_Observers[i].i32SlotIdx == i32SlotIdx)
			return true;
	}
	return false;
}

const ObserverState* ObserverSystem::GetObserverBySlot(int i32SlotIdx) const
{
	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		if (m_Observers[i].bActive && m_Observers[i].i32SlotIdx == i32SlotIdx)
			return &m_Observers[i];
	}
	return nullptr;
}

ObserverState* ObserverSystem::FindObserver(int i32SlotIdx)
{
	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		if (m_Observers[i].bActive && m_Observers[i].i32SlotIdx == i32SlotIdx)
			return &m_Observers[i];
	}
	return nullptr;
}

int ObserverSystem::FindEmptyObserverSlot() const
{
	for (int i = 0; i < MAX_OBSERVERS_PER_ROOM; i++)
	{
		if (!m_Observers[i].bActive)
			return i;
	}
	return -1;
}
