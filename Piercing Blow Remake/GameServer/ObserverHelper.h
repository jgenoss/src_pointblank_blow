#ifndef __OBSERVER_HELPER_H__
#define __OBSERVER_HELPER_H__

#pragma once

class Room;
class GameSession;
class ObserverSystem;

// ============================================================================
// ObserverRegistry - Global accessor for per-room ObserverSystem instances
// ============================================================================
namespace ObserverRegistry
{
	ObserverSystem*	GetObserverSystem(int i32ChannelNum, int i32RoomIdx);
	ObserverSystem*	GetObserverSystemForRoom(Room* pRoom);
	void			InitForRoom(Room* pRoom);
	void			ResetForRoom(Room* pRoom);
}

// ============================================================================
// ObserverHelper - Static integration helpers called from existing code paths
//
// These functions are called from Room.cpp and GameSessionRoom.cpp to hook
// into the observer system without modifying Room.h.
// ============================================================================
namespace ObserverHelper
{
	// Room lifecycle
	void	OnRoomCreated(Room* pRoom);
	void	OnRoomDestroyed(Room* pRoom);

	// Observer slot transitions
	void	OnObserverJoined(Room* pRoom, GameSession* pSession, int slotIdx);
	void	OnObserverLeft(Room* pRoom, GameSession* pSession);

	// Periodic update (call from Room::OnUpdateRoom during battle)
	void	Update(Room* pRoom, DWORD dwNow);

	// Battle event notifications
	void	NotifyKillFeed(Room* pRoom, int killerSlot, int deadSlot,
						   uint32_t weaponId, uint8_t hitPart, uint8_t multiKill);
	void	NotifyScoreUpdate(Room* pRoom, int redScore, int blueScore, int round);
	void	NotifyRoundStart(Room* pRoom, int round);
	void	NotifyRoundEnd(Room* pRoom, int winnerTeam);
	void	NotifyBombPlant(Room* pRoom, int installerSlot, uint8_t bombArea);
	void	NotifyBombDefuse(Room* pRoom, int defuserSlot);
	void	NotifyBombExplode(Room* pRoom);
	void	NotifyVIPTouchdown(Room* pRoom, int vipSlot);

	// Position tracking (call when player positions are updated)
	void	EnqueuePosition(Room* pRoom, int slot, float x, float y, float z,
							float yaw, uint8_t team, bool alive);
}

#endif // __OBSERVER_HELPER_H__
