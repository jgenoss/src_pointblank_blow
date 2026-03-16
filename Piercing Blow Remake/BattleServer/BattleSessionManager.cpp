#include "pch.h"
#include "BattleSessionManager.h"
#include "BattleSession.h"

I3_CLASS_INSTANCE(BattleSessionManager, i3NetworkSessionManager);

BattleSessionManager::BattleSessionManager()
	: m_pSessions(nullptr)
{
}

BattleSessionManager::~BattleSessionManager()
{
}

BOOL BattleSessionManager::OnCreate(i3List* pSessionList)
{
	m_pSessions = new BattleSession[MAX_BATTLE_SESSIONS];
	if (!m_pSessions)
		return FALSE;

	for (int i = 0; i < MAX_BATTLE_SESSIONS; ++i)
	{
		m_pSessions[i].Create();
		m_SessionList.Add(&m_pSessions[i]);
	}

	if (!i3NetworkSessionManager::OnCreate(&m_SessionList))
		return FALSE;

	printf("[BattleSessionManager] Created with %d session slots\n", MAX_BATTLE_SESSIONS);
	return TRUE;
}

void BattleSessionManager::OnUpdate()
{
	i3NetworkSessionManager::OnUpdate();
}

BOOL BattleSessionManager::OnDestroy()
{
	BOOL bResult = i3NetworkSessionManager::OnDestroy();

	if (m_pSessions)
	{
		delete[] m_pSessions;
		m_pSessions = nullptr;
	}

	return bResult;
}

ULONG_PTR BattleSessionManager::ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr)
{
	return i3NetworkSessionManager::ConnectSession(Socket, pAddr);
}

BattleSession* BattleSessionManager::GetSession(int i32Idx)
{
	if (i32Idx < 0 || i32Idx >= MAX_BATTLE_SESSIONS)
		return nullptr;
	return &m_pSessions[i32Idx];
}

int BattleSessionManager::GetRegisteredCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_BATTLE_SESSIONS; ++i)
	{
		if (m_pSessions[i].IsRegistered())
			count++;
	}
	return count;
}
