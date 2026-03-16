#ifndef __BATTLEROOMMANAGER_H__
#define __BATTLEROOMMANAGER_H__

#pragma once
#include "i3ElementBase.h"
#include "BattleDef.h"
#include "InterServerProtocol.h"

class BattleRoom;

// Gestor de battle rooms
// Pool pre-allocado de BattleRoom, asignacion de puertos UDP
class BattleRoomManager : public i3ElementBase
{
	I3_CLASS_DEFINE(BattleRoomManager);

public:
	BattleRoomManager();
	virtual ~BattleRoomManager();

	// Lifecycle
	void			OnCreate();
	void			OnUpdate();
	void			OnDestroy();

	// Room management
	int				CreateRoom(IS_BATTLE_CREATE_REQ* pReq, int i32SessionIdx);	// Returns room index, -1 = error
	BattleRoom*		GetRoom(int i32Idx);
	void			DestroyRoom(int i32Idx);

	// Queries
	int				GetActiveRoomCount() const		{ return m_i32ActiveCount; }
	int				GetMaxRoomCount() const			{ return MAX_BATTLE_ROOMS; }

private:
	uint16_t		AllocateUdpPort();
	int				FindFreeRoomSlot() const;

private:
	BattleRoom*		m_pRooms;
	int				m_i32ActiveCount;
	uint16_t		m_ui16NextUdpPort;
};

extern BattleRoomManager* g_pBattleRoomManager;

#endif // __BATTLEROOMMANAGER_H__
