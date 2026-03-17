#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "Room.h"
#include "RoomManager.h"
#include "ObserverSystem.h"

// ============================================================================
// Observer/Spectator Packet Handlers
//
// These extend the existing observer slot support (bIsObserver, ROOM_INFO_FLAG_OBSERVER)
// with real-time spectator feed functionality during battles.
//
// Protocol IDs for observer requests use offsets from the existing
// PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT range.
// The same IDs are defined in ObserverSystem.cpp for the ACK/NTF side.
// ============================================================================

// Request protocol IDs (client -> server)
// We use odd offsets from PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_REQ for requests,
// matching the REQ/ACK pattern used throughout the protocol enum.
#define PROTOCOL_OBSERVER_VIEW_MODE_REQ			(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 9)
#define PROTOCOL_OBSERVER_FOLLOW_PLAYER_REQ		(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 11)
#define PROTOCOL_OBSERVER_SCOREBOARD_REQ		(PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK + 13)

// ============================================================================
// OnObserverViewModeReq
//
// Client requests a change in spectator view mode.
// Packet format: ui8ViewMode(1)
// ============================================================================
void GameSession::OnObserverViewModeReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 1)
		return;

	// Must be in a room and be an observer
	if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
	if (!mySlot.bIsObserver)
	{
		printf("[GameSession] ObserverViewMode: slot %d is not an observer\n", m_i32SlotIdx);
		return;
	}

	// Must be in battle state to change view modes
	if (m_pRoom->GetRoomState() < ROOM_STATE_BATTLE)
		return;

	uint8_t ui8ViewMode = *(uint8_t*)pData;

	// Validate range
	if (ui8ViewMode >= OBSERVER_VIEW_MAX)
		ui8ViewMode = OBSERVER_VIEW_FREE;

	ObserverViewMode eMode = (ObserverViewMode)ui8ViewMode;

	// Get the room's observer system
	// For this implementation, ObserverSystem is accessed via a per-room instance.
	// We create a static map keyed by room index for simplicity, or the Room
	// should own the ObserverSystem. Since we cannot modify Room.h per the task,
	// we use a static registry approach.
	static ObserverSystem s_ObserverSystems[GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL];

	int obsIdx = m_pRoom->GetChannelNum() * MAX_ROOM_COUNT_PER_CHANNEL + m_pRoom->GetRoomIdx();
	if (obsIdx < 0 || obsIdx >= GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL)
		return;

	ObserverSystem& obsSys = s_ObserverSystems[obsIdx];
	obsSys.OnViewModeChange(m_i32SlotIdx, eMode);

	printf("[GameSession] ObserverViewMode: slot=%d mode=%d, UID=%lld\n",
		m_i32SlotIdx, (int)eMode, m_i64UID);
}

// ============================================================================
// OnObserverFollowPlayerReq
//
// Client requests to follow a specific player.
// Packet format: i8TargetSlot(1)
//   -1 = unfollow (free camera)
//   0..15 = follow that slot's player
// ============================================================================
void GameSession::OnObserverFollowPlayerReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 1)
		return;

	if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
	if (!mySlot.bIsObserver)
	{
		printf("[GameSession] ObserverFollow: slot %d is not an observer\n", m_i32SlotIdx);
		return;
	}

	if (m_pRoom->GetRoomState() < ROOM_STATE_BATTLE)
		return;

	int8_t i8TargetSlot = *(int8_t*)pData;

	static ObserverSystem s_ObserverSystems[GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL];

	int obsIdx = m_pRoom->GetChannelNum() * MAX_ROOM_COUNT_PER_CHANNEL + m_pRoom->GetRoomIdx();
	if (obsIdx < 0 || obsIdx >= GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL)
		return;

	ObserverSystem& obsSys = s_ObserverSystems[obsIdx];
	obsSys.OnFollowPlayer(m_i32SlotIdx, (int)i8TargetSlot);

	printf("[GameSession] ObserverFollow: slot=%d follow=%d, UID=%lld\n",
		m_i32SlotIdx, (int)i8TargetSlot, m_i64UID);
}

// ============================================================================
// OnObserverScoreboardReq
//
// Client requests to show or hide the observer scoreboard overlay.
// Packet format: ui8Show(1)
//   0 = hide scoreboard, 1 = show scoreboard
// ============================================================================
void GameSession::OnObserverScoreboardReq(char* pData, INT32 i32Size)
{
	if (!m_pRoom || i32Size < 1)
		return;

	if (m_i32SlotIdx < 0 || m_i32SlotIdx >= SLOT_MAX_COUNT)
		return;

	const GameSlotInfo& mySlot = m_pRoom->GetSlotInfo(m_i32SlotIdx);
	if (!mySlot.bIsObserver)
		return;

	uint8_t ui8Show = *(uint8_t*)pData;
	bool bShow = (ui8Show != 0);

	static ObserverSystem s_ObserverSystems[GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL];

	int obsIdx = m_pRoom->GetChannelNum() * MAX_ROOM_COUNT_PER_CHANNEL + m_pRoom->GetRoomIdx();
	if (obsIdx < 0 || obsIdx >= GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL)
		return;

	ObserverSystem& obsSys = s_ObserverSystems[obsIdx];
	obsSys.OnToggleScoreboard(m_i32SlotIdx, bShow);
}

// ============================================================================
// ObserverSystem Global Registry
//
// Since Room.h cannot be modified to embed ObserverSystem directly,
// we provide a global accessor so that Room.cpp, GameSessionRoom.cpp, and
// other files can access the observer system for a given room.
// ============================================================================

namespace ObserverRegistry
{
	static ObserverSystem s_Systems[GAME_CHANNEL_COUNT * MAX_ROOM_COUNT_PER_CHANNEL];
	static bool s_bInitialized = false;

	ObserverSystem* GetObserverSystem(int i32ChannelNum, int i32RoomIdx)
	{
		if (i32ChannelNum < 0 || i32ChannelNum >= GAME_CHANNEL_COUNT)
			return nullptr;
		if (i32RoomIdx < 0 || i32RoomIdx >= MAX_ROOM_COUNT_PER_CHANNEL)
			return nullptr;

		int idx = i32ChannelNum * MAX_ROOM_COUNT_PER_CHANNEL + i32RoomIdx;
		return &s_Systems[idx];
	}

	ObserverSystem* GetObserverSystemForRoom(Room* pRoom)
	{
		if (!pRoom)
			return nullptr;
		return GetObserverSystem(pRoom->GetChannelNum(), pRoom->GetRoomIdx());
	}

	void InitForRoom(Room* pRoom)
	{
		ObserverSystem* pSys = GetObserverSystemForRoom(pRoom);
		if (pSys)
			pSys->Init(pRoom);
	}

	void ResetForRoom(Room* pRoom)
	{
		ObserverSystem* pSys = GetObserverSystemForRoom(pRoom);
		if (pSys)
			pSys->Reset();
	}
}

// ============================================================================
// Integration helpers - called from existing code paths
//
// These static functions can be called from Room.cpp or GameSessionRoom.cpp
// to hook into the observer system without modifying Room.h.
//
// Example integration in Room::OnPlayerDeath():
//   ObserverHelper::NotifyKillFeed(this, killerSlot, deadSlot, weaponId, hitPart, multiKill);
//
// Example integration in Room::OnUpdateRoom():
//   ObserverHelper::Update(this, dwNow);
//
// Example integration in Room::OnRoundEnd():
//   ObserverHelper::NotifyRoundEnd(this, winnerTeam);
// ============================================================================

namespace ObserverHelper
{
	void OnRoomCreated(Room* pRoom)
	{
		ObserverRegistry::InitForRoom(pRoom);
	}

	void OnRoomDestroyed(Room* pRoom)
	{
		ObserverRegistry::ResetForRoom(pRoom);
	}

	void OnObserverJoined(Room* pRoom, GameSession* pSession, int slotIdx)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys)
			pSys->OnObserverJoin(pSession, slotIdx);
	}

	void OnObserverLeft(Room* pRoom, GameSession* pSession)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys)
			pSys->OnObserverLeave(pSession);
	}

	void Update(Room* pRoom, DWORD dwNow)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->Update(dwNow);
	}

	void NotifyKillFeed(Room* pRoom, int killerSlot, int deadSlot,
						uint32_t weaponId, uint8_t hitPart, uint8_t multiKill)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueKillFeed(killerSlot, deadSlot, weaponId, hitPart, multiKill);
	}

	void NotifyScoreUpdate(Room* pRoom, int redScore, int blueScore, int round)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueScoreUpdate(redScore, blueScore, round);
	}

	void NotifyRoundStart(Room* pRoom, int round)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueRoundEvent(0, -1, -1, (uint8_t)round);
	}

	void NotifyRoundEnd(Room* pRoom, int winnerTeam)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueRoundEvent(1, winnerTeam, -1, 0);
	}

	void NotifyBombPlant(Room* pRoom, int installerSlot, uint8_t bombArea)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueRoundEvent(2, -1, installerSlot, bombArea);
	}

	void NotifyBombDefuse(Room* pRoom, int defuserSlot)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueRoundEvent(3, -1, defuserSlot, 0);
	}

	void NotifyBombExplode(Room* pRoom)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueRoundEvent(4, -1, -1, 0);
	}

	void NotifyVIPTouchdown(Room* pRoom, int vipSlot)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueueRoundEvent(5, -1, vipSlot, 0);
	}

	void EnqueuePosition(Room* pRoom, int slot, float x, float y, float z,
						  float yaw, uint8_t team, bool alive)
	{
		ObserverSystem* pSys = ObserverRegistry::GetObserverSystemForRoom(pRoom);
		if (pSys && pSys->HasObservers())
			pSys->EnqueuePositionUpdate(slot, x, y, z, yaw, team, alive);
	}
}
