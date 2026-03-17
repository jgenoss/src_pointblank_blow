#ifndef __GAMESERVERREGISTRY_H__
#define __GAMESERVERREGISTRY_H__

#pragma once
#include <windows.h>
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

	// Session mapping (ConnectSession index that owns this GameServer connection)
	void				SetServerSessionIdx(int serverId, int sessionIdx);
	int					GetServerSessionIdx(int serverId) const;

	// Mantenimiento (llamar desde OnUpdate)
	void				Update();

	// Statistics
	void				GetStats(LONG* pRegs, LONG* pUnregs, LONG* pHeartbeats, LONG* pDeadRemoved) const;

private:
	void				RemoveDeadServers();

private:
	ServerList			m_ServerList;
	DWORD				m_dwLastHeartbeat[MAX_REGISTERED_SERVERS];		// Ultimo heartbeat por server ID slot
	int					m_i32SessionIdx[MAX_REGISTERED_SERVERS];		// ConnectSession index per server

	// Thread safety
	mutable CRITICAL_SECTION	m_csRegistry;

	// Statistics counters
	volatile LONG		m_lTotalRegistrations;
	volatile LONG		m_lTotalUnregistrations;
	volatile LONG		m_lTotalHeartbeats;
	volatile LONG		m_lTotalDeadRemoved;
	volatile LONG		m_lTotalConnections;
	volatile LONG		m_lTotalAuths;
	volatile LONG		m_lAuthFailures;
	volatile LONG		m_lPeakServers;

public:
	// Extended statistics
	struct RegistryStatistics
	{
		LONG lRegistrations;
		LONG lUnregistrations;
		LONG lHeartbeats;
		LONG lDeadRemoved;
		LONG lTotalConnections;
		LONG lTotalAuths;
		LONG lAuthFailures;
		LONG lPeakServers;
		int  iCurrentServers;
		int  iOnlineServers;
	};

	void				GetStatistics(RegistryStatistics* pOut) const;
	void				IncrementConnections()		{ InterlockedIncrement(&m_lTotalConnections); }
	void				IncrementAuths()			{ InterlockedIncrement(&m_lTotalAuths); }
	void				IncrementAuthFailures()		{ InterlockedIncrement(&m_lAuthFailures); }
};

#endif // __GAMESERVERREGISTRY_H__
