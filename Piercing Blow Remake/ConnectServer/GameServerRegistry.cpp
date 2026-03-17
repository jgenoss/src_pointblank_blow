#include "GameServerRegistry.h"
#include <cstdio>
#include <cstring>

GameServerRegistry::GameServerRegistry()
{
	memset(m_dwLastHeartbeat, 0, sizeof(m_dwLastHeartbeat));
	memset(m_i32SessionIdx, -1, sizeof(m_i32SessionIdx));
	InitializeCriticalSectionAndSpinCount(&m_csRegistry, 4000);
	m_lTotalRegistrations = 0;
	m_lTotalUnregistrations = 0;
	m_lTotalHeartbeats = 0;
	m_lTotalDeadRemoved = 0;
	m_lTotalConnections = 0;
	m_lTotalAuths = 0;
	m_lAuthFailures = 0;
	m_lPeakServers = 0;
}

GameServerRegistry::~GameServerRegistry()
{
	DeleteCriticalSection(&m_csRegistry);
}

bool GameServerRegistry::RegisterServer(const ServerInfo& info)
{
	EnterCriticalSection(&m_csRegistry);

	if (!m_ServerList.AddServer(info))
	{
		LeaveCriticalSection(&m_csRegistry);
		printf("[GameServerRegistry] Failed to register server ID=%d '%s'\n", info.id, info.name);
		return false;
	}

	// Marcar heartbeat inicial
	if (info.id >= 0 && info.id < MAX_REGISTERED_SERVERS)
		m_dwLastHeartbeat[info.id] = GetTickCount();

	LeaveCriticalSection(&m_csRegistry);

	InterlockedIncrement(&m_lTotalRegistrations);

	// Update peak server count via CAS loop
	{
		EnterCriticalSection(&m_csRegistry);
		LONG currentCount = (LONG)m_ServerList.GetServerCount();
		LeaveCriticalSection(&m_csRegistry);

		LONG peak = m_lPeakServers;
		while (currentCount > peak) {
			LONG old = InterlockedCompareExchange(&m_lPeakServers, currentCount, peak);
			if (old == peak) break;
			peak = old;
		}
	}

	printf("[GameServerRegistry] Registered: ID=%d Name='%s' IP=%s:%d MaxPlayers=%d\n",
		info.id, info.name, info.ip, info.port, info.maxPlayers);

	return true;
}

bool GameServerRegistry::UnregisterServer(int serverId)
{
	EnterCriticalSection(&m_csRegistry);

	if (m_ServerList.RemoveServer(serverId))
	{
		if (serverId >= 0 && serverId < MAX_REGISTERED_SERVERS)
		{
			m_dwLastHeartbeat[serverId] = 0;
			m_i32SessionIdx[serverId] = -1;
		}

		LeaveCriticalSection(&m_csRegistry);

		InterlockedIncrement(&m_lTotalUnregistrations);

		printf("[GameServerRegistry] Unregistered server ID=%d\n", serverId);
		return true;
	}

	LeaveCriticalSection(&m_csRegistry);
	return false;
}

bool GameServerRegistry::UpdateServerStatus(int serverId, ServerOnlineState state, int currentPlayers)
{
	EnterCriticalSection(&m_csRegistry);
	bool result = m_ServerList.UpdateServerState(serverId, state, currentPlayers);
	LeaveCriticalSection(&m_csRegistry);
	return result;
}

bool GameServerRegistry::UpdateHeartbeat(int serverId)
{
	EnterCriticalSection(&m_csRegistry);

	if (serverId < 0 || serverId >= MAX_REGISTERED_SERVERS)
	{
		LeaveCriticalSection(&m_csRegistry);
		return false;
	}

	const ServerInfo* pInfo = m_ServerList.GetServerById(serverId);
	if (!pInfo)
	{
		LeaveCriticalSection(&m_csRegistry);
		return false;
	}

	m_dwLastHeartbeat[serverId] = GetTickCount();

	LeaveCriticalSection(&m_csRegistry);

	InterlockedIncrement(&m_lTotalHeartbeats);
	return true;
}

int GameServerRegistry::GetOnlineServerCount() const
{
	EnterCriticalSection(&m_csRegistry);

	int count = 0;
	for (int i = 0; i < m_ServerList.GetServerCount(); ++i)
	{
		const ServerInfo* pInfo = m_ServerList.GetServerByIndex(i);
		if (pInfo && pInfo->state == ServerOnlineState::Online)
			++count;
	}

	LeaveCriticalSection(&m_csRegistry);
	return count;
}

const ServerInfo* GameServerRegistry::GetFirstAvailableServer() const
{
	EnterCriticalSection(&m_csRegistry);

	const ServerInfo* pResult = nullptr;
	for (int i = 0; i < m_ServerList.GetServerCount(); ++i)
	{
		const ServerInfo* pInfo = m_ServerList.GetServerByIndex(i);
		if (pInfo && pInfo->state == ServerOnlineState::Online &&
			pInfo->currentPlayers < pInfo->maxPlayers)
		{
			pResult = pInfo;
			break;
		}
	}

	LeaveCriticalSection(&m_csRegistry);
	return pResult;
}

const ServerInfo* GameServerRegistry::GetLeastLoadedServer() const
{
	EnterCriticalSection(&m_csRegistry);

	const ServerInfo* pBest = nullptr;
	float fBestLoad = 1.0f;

	for (int i = 0; i < m_ServerList.GetServerCount(); ++i)
	{
		const ServerInfo* pInfo = m_ServerList.GetServerByIndex(i);
		if (!pInfo || pInfo->state != ServerOnlineState::Online)
			continue;
		if (pInfo->maxPlayers <= 0)
			continue;

		float fLoad = (float)pInfo->currentPlayers / (float)pInfo->maxPlayers;
		if (fLoad < fBestLoad)
		{
			fBestLoad = fLoad;
			pBest = pInfo;
		}
	}

	LeaveCriticalSection(&m_csRegistry);
	return pBest;
}

void GameServerRegistry::SetServerSessionIdx(int serverId, int sessionIdx)
{
	EnterCriticalSection(&m_csRegistry);
	if (serverId >= 0 && serverId < MAX_REGISTERED_SERVERS)
		m_i32SessionIdx[serverId] = sessionIdx;
	LeaveCriticalSection(&m_csRegistry);
}

int GameServerRegistry::GetServerSessionIdx(int serverId) const
{
	EnterCriticalSection(&m_csRegistry);
	int result = -1;
	if (serverId >= 0 && serverId < MAX_REGISTERED_SERVERS)
		result = m_i32SessionIdx[serverId];
	LeaveCriticalSection(&m_csRegistry);
	return result;
}

void GameServerRegistry::Update()
{
	RemoveDeadServers();
}

void GameServerRegistry::RemoveDeadServers()
{
	EnterCriticalSection(&m_csRegistry);

	DWORD dwNow = GetTickCount();

	for (int i = m_ServerList.GetServerCount() - 1; i >= 0; --i)
	{
		const ServerInfo* pInfo = m_ServerList.GetServerByIndex(i);
		if (!pInfo)
			continue;

		int serverId = pInfo->id;
		if (serverId < 0 || serverId >= MAX_REGISTERED_SERVERS)
			continue;

		// Verificar si el heartbeat expiro
		if (m_dwLastHeartbeat[serverId] > 0 &&
			(dwNow - m_dwLastHeartbeat[serverId]) > SERVER_HEARTBEAT_TIMEOUT)
		{
			printf("[GameServerRegistry] Server ID=%d '%s' timed out (no heartbeat)\n",
				serverId, pInfo->name);

			// Inline unregister to avoid double-locking
			m_ServerList.RemoveServer(serverId);
			m_dwLastHeartbeat[serverId] = 0;
			m_i32SessionIdx[serverId] = -1;

			InterlockedIncrement(&m_lTotalDeadRemoved);
			InterlockedIncrement(&m_lTotalUnregistrations);
		}
	}

	LeaveCriticalSection(&m_csRegistry);
}

void GameServerRegistry::GetStats(LONG* pRegs, LONG* pUnregs, LONG* pHeartbeats, LONG* pDeadRemoved) const
{
	if (pRegs)			*pRegs = m_lTotalRegistrations;
	if (pUnregs)		*pUnregs = m_lTotalUnregistrations;
	if (pHeartbeats)	*pHeartbeats = m_lTotalHeartbeats;
	if (pDeadRemoved)	*pDeadRemoved = m_lTotalDeadRemoved;
}

void GameServerRegistry::GetStatistics(RegistryStatistics* pOut) const
{
	if (!pOut)
		return;

	pOut->lRegistrations	= m_lTotalRegistrations;
	pOut->lUnregistrations	= m_lTotalUnregistrations;
	pOut->lHeartbeats		= m_lTotalHeartbeats;
	pOut->lDeadRemoved		= m_lTotalDeadRemoved;
	pOut->lTotalConnections	= m_lTotalConnections;
	pOut->lTotalAuths		= m_lTotalAuths;
	pOut->lAuthFailures		= m_lAuthFailures;
	pOut->lPeakServers		= m_lPeakServers;

	EnterCriticalSection(&m_csRegistry);
	pOut->iCurrentServers	= m_ServerList.GetServerCount();
	LeaveCriticalSection(&m_csRegistry);

	pOut->iOnlineServers	= GetOnlineServerCount();
}
