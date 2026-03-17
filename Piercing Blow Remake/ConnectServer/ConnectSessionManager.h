#ifndef __CONNECTSESSIONMANAGER_H__
#define __CONNECTSESSIONMANAGER_H__

#pragma once
#include <windows.h>
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
	virtual ULONG_PTR	ConnectSession(SOCKET Socket, struct sockaddr_in* pAddr) override;

	// Accessors
	int					GetActiveCount() const			{ return (int)m_lActiveCount; }
	int					GetMaxCount() const				{ return MAX_CONNECT_SESSIONS; }
	int					GetTotalConnections() const		{ return (int)m_lTotalConnections; }
	int					GetTotalTimeouts() const		{ return (int)m_lTotalTimeouts; }
	int					GetPeakActive() const			{ return (int)m_lPeakActive; }

	// Session lookup
	class ConnectSession*	GetSession(int i32Idx);

private:
	void				CheckTimeouts();

private:
	i3List				m_SessionList;
	class ConnectSession*	m_pSessions;		// Pool pre-allocado
	DWORD				m_dwLastTimeoutCheck;

	// Thread-safe counters
	volatile LONG		m_lActiveCount;
	volatile LONG		m_lTotalConnections;
	volatile LONG		m_lTotalTimeouts;
	volatile LONG		m_lPeakActive;
};

#endif // __CONNECTSESSIONMANAGER_H__
