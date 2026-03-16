#include "ServerList.h"
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
	// TODO: Implementar carga desde archivo INI usando i3IniParser
	// Por ahora retorna true para que el servidor pueda iniciar
	return true;
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
