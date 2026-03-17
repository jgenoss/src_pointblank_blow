#include "GameServerRegistry.h"
#include <cstdio>
#include <cstring>

GameServerRegistry::GameServerRegistry()
{
	memset(m_dwLastHeartbeat, 0, sizeof(m_dwLastHeartbeat));
	memset(m_i32SessionIdx, -1, sizeof(m_i32SessionIdx));
}

GameServerRegistry::~GameServerRegistry()
{
}

bool GameServerRegistry::RegisterServer(const ServerInfo& info)
{
	if (!m_ServerList.AddServer(info))
	{
		printf("[GameServerRegistry] Failed to register server ID=%d '%s'\n", info.id, info.name);
		return false;
	}

	// Marcar heartbeat inicial
	if (info.id >= 0 && info.id < MAX_REGISTERED_SERVERS)
		m_dwLastHeartbeat[info.id] = GetTickCount();

	printf("[GameServerRegistry] Registered: ID=%d Name='%s' IP=%s:%d MaxPlayers=%d\n",
		info.id, info.name, info.ip, info.port, info.maxPlayers);

	return true;
}

bool GameServerRegistry::UnregisterServer(int serverId)
{
	if (m_ServerList.RemoveServer(serverId))
	{
		if (serverId >= 0 && serverId < MAX_REGISTERED_SERVERS)
		{
			m_dwLastHeartbeat[serverId] = 0;
			m_i32SessionIdx[serverId] = -1;
		}

		printf("[GameServerRegistry] Unregistered server ID=%d\n", serverId);
		return true;
	}
	return false;
}

bool GameServerRegistry::UpdateServerStatus(int serverId, ServerOnlineState state, int currentPlayers)
{
	return m_ServerList.UpdateServerState(serverId, state, currentPlayers);
}

bool GameServerRegistry::UpdateHeartbeat(int serverId)
{
	if (serverId < 0 || serverId >= MAX_REGISTERED_SERVERS)
		return false;

	const ServerInfo* pInfo = m_ServerList.GetServerById(serverId);
	if (!pInfo)
		return false;

	m_dwLastHeartbeat[serverId] = GetTickCount();
	return true;
}

int GameServerRegistry::GetOnlineServerCount() const
{
	int count = 0;
	for (int i = 0; i < m_ServerList.GetServerCount(); ++i)
	{
		const ServerInfo* pInfo = m_ServerList.GetServerByIndex(i);
		if (pInfo && pInfo->state == ServerOnlineState::Online)
			++count;
	}
	return count;
}

const ServerInfo* GameServerRegistry::GetFirstAvailableServer() const
{
	for (int i = 0; i < m_ServerList.GetServerCount(); ++i)
	{
		const ServerInfo* pInfo = m_ServerList.GetServerByIndex(i);
		if (pInfo && pInfo->state == ServerOnlineState::Online &&
			pInfo->currentPlayers < pInfo->maxPlayers)
		{
			return pInfo;
		}
	}
	return nullptr;
}

const ServerInfo* GameServerRegistry::GetLeastLoadedServer() const
{
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

	return pBest;
}

void GameServerRegistry::SetServerSessionIdx(int serverId, int sessionIdx)
{
	if (serverId >= 0 && serverId < MAX_REGISTERED_SERVERS)
		m_i32SessionIdx[serverId] = sessionIdx;
}

int GameServerRegistry::GetServerSessionIdx(int serverId) const
{
	if (serverId >= 0 && serverId < MAX_REGISTERED_SERVERS)
		return m_i32SessionIdx[serverId];
	return -1;
}

void GameServerRegistry::Update()
{
	RemoveDeadServers();
}

void GameServerRegistry::RemoveDeadServers()
{
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
			UnregisterServer(serverId);
		}
	}
}
