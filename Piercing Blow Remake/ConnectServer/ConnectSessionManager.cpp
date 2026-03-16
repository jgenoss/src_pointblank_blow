#include "ConnectSessionManager.h"
#include "ConnectSession.h"
#include <cstdio>

#define TIMEOUT_CHECK_INTERVAL	5000	// Verificar timeouts cada 5 segundos

I3_CLASS_INSTANCE(ConnectSessionManager, i3NetworkSessionManager);

ConnectSessionManager::ConnectSessionManager()
	: m_pSessions(nullptr)
	, m_dwLastTimeoutCheck(0)
{
}

ConnectSessionManager::~ConnectSessionManager()
{
}

BOOL ConnectSessionManager::OnCreate(i3List* pSessionList)
{
	// Crear pool de sesiones
	m_pSessions = new ConnectSession[MAX_CONNECT_SESSIONS];
	if (!m_pSessions)
		return FALSE;

	// Agregar al session list para i3NetworkSessionManager
	for (int i = 0; i < MAX_CONNECT_SESSIONS; ++i)
	{
		m_pSessions[i].Create();
		m_SessionList.Add(&m_pSessions[i]);
	}

	// Llamar al padre con la lista
	if (!i3NetworkSessionManager::OnCreate(&m_SessionList))
		return FALSE;

	m_dwLastTimeoutCheck = GetTickCount();

	printf("[ConnectSessionManager] Created with %d session slots\n", MAX_CONNECT_SESSIONS);
	return TRUE;
}

void ConnectSessionManager::OnUpdate()
{
	i3NetworkSessionManager::OnUpdate();

	// Verificar timeouts periodicamente
	DWORD dwNow = GetTickCount();
	if (dwNow - m_dwLastTimeoutCheck >= TIMEOUT_CHECK_INTERVAL)
	{
		m_dwLastTimeoutCheck = dwNow;
		CheckTimeouts();
	}
}

BOOL ConnectSessionManager::OnDestroy()
{
	BOOL bResult = i3NetworkSessionManager::OnDestroy();

	if (m_pSessions)
	{
		delete[] m_pSessions;
		m_pSessions = nullptr;
	}

	return bResult;
}

ULONG_PTR ConnectSessionManager::ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr)
{
	return i3NetworkSessionManager::ConnectSession(Socket, pAddr);
}

ConnectSession* ConnectSessionManager::GetSession(int i32Idx)
{
	if (i32Idx < 0 || i32Idx >= MAX_CONNECT_SESSIONS)
		return nullptr;
	return &m_pSessions[i32Idx];
}

void ConnectSessionManager::CheckTimeouts()
{
	for (int i = 0; i < MAX_CONNECT_SESSIONS; ++i)
	{
		ConnectSession* pSession = &m_pSessions[i];
		if (pSession->GetIsActive() && pSession->IsTimedOut())
		{
			printf("[ConnectSessionManager] Session %d timed out\n", i);
			DisConnectSession(pSession, FALSE);
		}
	}
}
