#include "i3CommonType.h"
#include "i3IniParser.h"
#include "ServerList.h"
#include <cstdio>
#include <cstring>

ServerList::ServerList()
	: m_i32Count(0)
{
	memset(m_Servers, 0, sizeof(m_Servers));
}

ServerList::~ServerList()
{
}

bool ServerList::LoadFromConfig(const char* pszFilename)
{
	i3IniParser ini;
	if (!ini.OpenFromFile(pszFilename))
		return false;

	if (!ini.ReadSection("ServerList"))
		return false;

	INT32 nCount = 0;
	ini.GetValue("Count", &nCount);
	if (nCount <= 0)
		return false;

	for (int i = 1; i <= nCount && m_i32Count < MAX_SERVER_COUNT; ++i)
	{
		char szSection[32];
		snprintf(szSection, sizeof(szSection), "Server_%d", i);
		if (!ini.ReadSection(szSection))
			continue;

		ServerInfo info;

		ini.GetValue("ID", &info.id);

		INT32 nType = (INT32)ServerType::Game;
		ini.GetValue("Type", &nType);
		info.type = (ServerType)nType;

		ini.GetValue("Name", info.name, MAX_SERVER_NAME_LENGTH);
		ini.GetValue("IP", info.ip, MAX_SERVER_IP_LENGTH);

		INT32 nPort = 0;
		ini.GetValue("Port", &nPort);
		info.port = (uint16_t)nPort;

		ini.GetValue("MaxPlayers", &info.maxPlayers);
		ini.GetValue("Region", info.region, MAX_SERVER_REGION_LENGTH);

		info.state = ServerOnlineState::Offline;
		info.currentPlayers = 0;

		AddServer(info);
	}

	return m_i32Count > 0;
}

int ServerList::GetServerCount(ServerType type) const
{
	int count = 0;
	for (int i = 0; i < m_i32Count; ++i)
	{
		if (m_Servers[i].type == type)
			++count;
	}
	return count;
}

const ServerInfo* ServerList::GetServerById(int id) const
{
	for (int i = 0; i < m_i32Count; ++i)
	{
		if (m_Servers[i].id == id)
			return &m_Servers[i];
	}
	return nullptr;
}

const ServerInfo* ServerList::GetServerByIndex(int index) const
{
	if (index < 0 || index >= m_i32Count)
		return nullptr;
	return &m_Servers[index];
}

bool ServerList::AddServer(const ServerInfo& info)
{
	if (m_i32Count >= MAX_SERVER_COUNT)
		return false;

	// Verificar duplicado
	if (GetServerById(info.id) != nullptr)
		return false;

	m_Servers[m_i32Count] = info;
	++m_i32Count;
	return true;
}

bool ServerList::RemoveServer(int id)
{
	for (int i = 0; i < m_i32Count; ++i)
	{
		if (m_Servers[i].id == id)
		{
			// Mover el ultimo al lugar del removido
			if (i < m_i32Count - 1)
				m_Servers[i] = m_Servers[m_i32Count - 1];
			--m_i32Count;
			return true;
		}
	}
	return false;
}

bool ServerList::UpdateServerState(int serverId, ServerOnlineState newState, int currentPlayers)
{
	for (int i = 0; i < m_i32Count; ++i)
	{
		if (m_Servers[i].id == serverId)
		{
			m_Servers[i].state = newState;
			m_Servers[i].currentPlayers = currentPlayers;
			return true;
		}
	}
	return false;
}
