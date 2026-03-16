#ifndef __SERVERLIST_H__
#define __SERVERLIST_H__

#pragma once
#include "ServerInfo.h"

class ServerList
{
public:
	ServerList();
	~ServerList();

	bool				LoadFromConfig(const char* pszFilename);

	int					GetServerCount() const						{ return m_i32Count; }
	int					GetServerCount(ServerType type) const;
	const ServerInfo*	GetServerById(int id) const;
	const ServerInfo*	GetServerByIndex(int index) const;

	bool				AddServer(const ServerInfo& info);
	bool				RemoveServer(int id);
	bool				UpdateServerState(int serverId, ServerOnlineState newState, int currentPlayers);

	const ServerInfo*	GetServers() const							{ return m_Servers; }

private:
	ServerInfo			m_Servers[MAX_SERVER_COUNT];
	int					m_i32Count;
};

#endif // __SERVERLIST_H__
