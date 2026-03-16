#ifndef __ROOM_H__
#define __ROOM_H__

#pragma once
#include "i3ElementBase.h"
#include "i3NetworkPacket.h"
#include "RoomDef.h"

class GameSession;

// Simplified port of CRoom from Server/Source/Game/Game/Room.h
// Manages a single game room with slots, teams, and battle state
class Room : public i3ElementBase
{
	I3_CLASS_DEFINE(Room);

public:
	Room();
	virtual ~Room();

	// Lifecycle
	bool		OnCreate(GameSession* pOwner, GameRoomCreateInfo* pInfo, int i32ChannelNum);
	void		OnDestroy();

	// Player management
	int			OnJoin(GameSession* pSession, int i32Team);		// Returns slot index, -1 on error
	void		OnLeave(GameSession* pSession);
	void		OnReady(GameSession* pSession, bool bReady);

	// Battle control
	bool		OnStartBattle();								// Returns false if not all ready
	void		OnEndBattle();

	// State queries
	bool		IsCreated() const			{ return m_bIsCreate; }
	int			GetChannelNum() const		{ return m_i32ChannelNum; }
	int			GetPlayerCount() const		{ return m_i32PlayerCount; }
	int			GetMaxPlayers() const		{ return m_i32MaxPlayers; }
	uint8_t		GetRoomState() const		{ return m_ui8RoomState; }
	const char*	GetTitle() const			{ return m_szTitle; }
	uint32_t	GetStageId() const			{ return m_ui32StageId; }
	int			GetRoomIdx() const			{ return m_i32RoomIdx; }
	void		SetRoomIdx(int idx)			{ m_i32RoomIdx = idx; }

	// Slot queries
	bool		IsSlotEmpty(int i32Slot) const;
	GameSession* GetSlotSession(int i32Slot) const;
	int			GetOwnerSlot() const		{ return m_i32OwnerSlot; }
	const GameSlotInfo& GetSlotInfo(int i32Slot) const { return m_Slots[i32Slot]; }

	// Broadcast
	void		SendToAll(i3NetworkPacket* pPacket);
	void		SendToTeam(int i32Team, i3NetworkPacket* pPacket);

	// Room info for lobby list (simplified ROOM_INFO_BASIC)
	void		FillRoomInfoBasic(char* pBuffer, int* pSize) const;

private:
	int			FindEmptySlot(int i32Team) const;
	void		UpdateOwner();

public:
	// Manager access
	bool				m_bIsCreate;
	int					m_i32ChannelNum;

private:
	int					m_i32RoomIdx;
	char				m_szTitle[64];
	char				m_szPassword[16];
	uint32_t			m_ui32StageId;
	uint8_t				m_ui8RoomState;		// ROOM_STATE enum
	int					m_i32MaxPlayers;
	int					m_i32PlayerCount;
	int					m_i32OwnerSlot;		// Slot del creador de la room
	uint8_t				m_ui8TeamBalance;

	// Slots
	GameSlotInfo		m_Slots[SLOT_MAX_COUNT];
	GameSession*		m_pSlotSession[SLOT_MAX_COUNT];
};

#endif // __ROOM_H__
