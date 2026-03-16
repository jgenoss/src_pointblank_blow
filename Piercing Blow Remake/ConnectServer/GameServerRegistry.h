#ifndef __GAMESERVERREGISTRY_H__
#define __GAMESERVERREGISTRY_H__

#pragma once
#include "ServerInfo.h"
#include "ServerList.h"

#define MAX_REGISTERED_SERVERS		16
#define SERVER_HEARTBEAT_TIMEOUT	30000	// 30 segundos sin heartbeat = muerto

// Registro de GameServers conectados al ConnectServer
// Los GameServers se registran al iniciar y reportan estado periodicamente.
// Similar al patron de MU Online donde ConnectServer mantiene la lista maestra.
class GameServerRegistry
{
public:
	GameServerRegistry();
	~GameServerRegistry();

	// Registro/desregistro
	bool				RegisterServer(const ServerInfo& info);
	bool				UnregisterServer(int serverId);

	// Actualizacion de estado
	bool				UpdateServerStatus(int serverId, ServerOnlineState state, int currentPlayers);
	bool				UpdateHeartbeat(int serverId);

	// Queries
	int					GetServerCount() const					{ return m_ServerList.GetServerCount(); }
	int					GetOnlineServerCount() const;
	const ServerInfo*	GetServerById(int id) const				{ return m_ServerList.GetServerById(id); }
	const ServerInfo*	GetServerByIndex(int index) const		{ return m_ServerList.GetServerByIndex(index); }
	const ServerInfo*	GetFirstAvailableServer() const;
	const ServerInfo*	GetLeastLoadedServer() const;

	// Mantenimiento (llamar desde OnUpdate)
	void				Update();

private:
	void				RemoveDeadServers();

private:
	ServerList			m_ServerList;
	DWORD				m_dwLastHeartbeat[MAX_REGISTERED_SERVERS];		// Ultimo heartbeat por server ID slot
};

#endif // __GAMESERVERREGISTRY_H__
