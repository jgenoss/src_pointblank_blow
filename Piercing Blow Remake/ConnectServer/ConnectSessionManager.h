#ifndef __CONNECTSESSIONMANAGER_H__
#define __CONNECTSESSIONMANAGER_H__

#pragma once
#include "i3NetworkSessionManager.h"
#include "i3List.h"

class ConnectSession;

#define MAX_CONNECT_SESSIONS	500

// Gestor de sesiones del ConnectServer
// Patron portado de CUserSessionManager pero simplificado:
// - Sin canales, rooms ni lobbies
// - Pool pre-allocado de ConnectSession
// - Verifica timeouts periodicamente
class ConnectSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE(ConnectSessionManager);

public:
	ConnectSessionManager();
	virtual ~ConnectSessionManager();

	// i3NetworkSessionManager overrides
	virtual BOOL		OnCreate(i3List* pSessionList) override;
	virtual void		OnUpdate() override;
	virtual BOOL		OnDestroy() override;
	virtual ULONG_PTR	ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr);

	// Accessors
	int					GetActiveCount() const			{ return m_ActiveSessionCount; }
	int					GetMaxCount() const				{ return MAX_CONNECT_SESSIONS; }

	// Session lookup
	ConnectSession*		GetSession(int i32Idx);

private:
	void				CheckTimeouts();

private:
	i3List				m_SessionList;
	ConnectSession*		m_pSessions;		// Pool pre-allocado
	DWORD				m_dwLastTimeoutCheck;
};

#endif // __CONNECTSESSIONMANAGER_H__
