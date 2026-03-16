#ifndef __BATTLESESSIONMANAGER_H__
#define __BATTLESESSIONMANAGER_H__

#pragma once
#include "i3NetworkSessionManager.h"
#include "i3List.h"
#include "BattleDef.h"

class BattleSession;

// Gestor de sesiones del BattleServer
// Solo GameServers se conectan al BattleServer (MAX_BATTLE_SESSIONS = 32)
class BattleSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE(BattleSessionManager);

public:
	BattleSessionManager();
	virtual ~BattleSessionManager();

	// i3NetworkSessionManager overrides
	virtual BOOL		OnCreate(i3List* pSessionList) override;
	virtual void		OnUpdate() override;
	virtual BOOL		OnDestroy() override;
	virtual ULONG_PTR	ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr);

	// Accessors
	BattleSession*		GetSession(int i32Idx);
	int					GetRegisteredCount() const;

private:
	i3List				m_SessionList;
	BattleSession*		m_pSessions;
};

#endif // __BATTLESESSIONMANAGER_H__
