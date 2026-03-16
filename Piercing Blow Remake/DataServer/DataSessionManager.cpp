#include "DataSessionManager.h"
#include "DataSession.h"
#include <cstdio>

I3_CLASS_INSTANCE(DataSessionManager, i3NetworkSessionManager);

DataSessionManager::DataSessionManager()
	: m_pSessions(nullptr)
{
}

DataSessionManager::~DataSessionManager()
{
}

BOOL DataSessionManager::OnCreate(i3List* pSessionList)
{
	// Crear pool de sesiones
	m_pSessions = new DataSession[MAX_DATA_SESSIONS];
	if (!m_pSessions)
		return FALSE;

	// Agregar al session list para i3NetworkSessionManager
	for (int i = 0; i < MAX_DATA_SESSIONS; ++i)
	{
		m_pSessions[i].Create();
		m_SessionList.Add(&m_pSessions[i]);
	}

	// Llamar al padre con la lista
	if (!i3NetworkSessionManager::OnCreate(&m_SessionList))
		return FALSE;

	printf("[DataSessionManager] Created with %d session slots\n", MAX_DATA_SESSIONS);
	return TRUE;
}

void DataSessionManager::OnUpdate()
{
	i3NetworkSessionManager::OnUpdate();
}

BOOL DataSessionManager::OnDestroy()
{
	BOOL bResult = i3NetworkSessionManager::OnDestroy();

	if (m_pSessions)
	{
		delete[] m_pSessions;
		m_pSessions = nullptr;
	}

	return bResult;
}

ULONG_PTR DataSessionManager::ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr)
{
	return i3NetworkSessionManager::ConnectSession(Socket, pAddr);
}

DataSession* DataSessionManager::GetSession(int i32Idx)
{
	if (i32Idx < 0 || i32Idx >= MAX_DATA_SESSIONS)
		return nullptr;
	return &m_pSessions[i32Idx];
}
