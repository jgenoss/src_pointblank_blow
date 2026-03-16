#ifndef __DATASESSIONMANAGER_H__
#define __DATASESSIONMANAGER_H__

#pragma once
#include "i3NetworkSessionManager.h"
#include "i3List.h"

class DataSession;

#define MAX_DATA_SESSIONS	32		// Solo servidores se conectan, no clientes

// Gestor de sesiones del DataServer
// Pool pre-allocado de DataSession para ConnectServer, GameServer y BattleServer
class DataSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE(DataSessionManager);

public:
	DataSessionManager();
	virtual ~DataSessionManager();

	// i3NetworkSessionManager overrides
	virtual BOOL		OnCreate(i3List* pSessionList) override;
	virtual void		OnUpdate() override;
	virtual BOOL		OnDestroy() override;
	virtual ULONG_PTR	ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr);

	// Accessors
	int					GetActiveCount() const			{ return m_ActiveSessionCount; }
	int					GetMaxCount() const				{ return MAX_DATA_SESSIONS; }

	// Session lookup
	DataSession*		GetSession(int i32Idx);

private:
	i3List				m_SessionList;
	DataSession*		m_pSessions;		// Pool pre-allocado
};

#endif // __DATASESSIONMANAGER_H__
