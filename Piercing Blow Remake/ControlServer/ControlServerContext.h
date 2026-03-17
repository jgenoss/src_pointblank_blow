#ifndef __CONTROLSERVERCONTEXT_H__
#define __CONTROLSERVERCONTEXT_H__

#pragma once
#include "BaseServer.h"

class ControlSessionManager;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Admin account entry for authentication
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_CONTROL_ADMIN_ACCOUNTS	16
#define MAX_ADMIN_USERNAME			32
#define MAX_ADMIN_PASSWORD			64

struct ControlAdminAccount
{
	char		szUsername[MAX_ADMIN_USERNAME];
	char		szPassword[MAX_ADMIN_PASSWORD];
	uint8_t		ui8Level;				// 0=viewer, 1=moderator, 2=admin, 3=superadmin

	ControlAdminAccount()
	{
		szUsername[0] = '\0';
		szPassword[0] = '\0';
		ui8Level = 0;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Server-wide statistics for the ControlServer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ControlServerStatistics
{
	volatile LONG64		l64TotalPacketsIn;
	volatile LONG64		l64TotalPacketsOut;
	volatile LONG64		l64TotalBytesIn;
	volatile LONG64		l64TotalBytesOut;
	volatile LONG		lCommandsProcessed;
	DWORD				dwStartTime;
	DWORD				dwLastLogTime;

	void Reset()
	{
		l64TotalPacketsIn = 0;
		l64TotalPacketsOut = 0;
		l64TotalBytesIn = 0;
		l64TotalBytesOut = 0;
		lCommandsProcessed = 0;
		dwStartTime = GetTickCount();
		dwLastLogTime = dwStartTime;
	}

	void IncrementPacketsIn()				{ InterlockedIncrement64(&l64TotalPacketsIn); }
	void IncrementPacketsOut()				{ InterlockedIncrement64(&l64TotalPacketsOut); }
	void AddBytesIn(LONG64 bytes)			{ InterlockedExchangeAdd64(&l64TotalBytesIn, bytes); }
	void AddBytesOut(LONG64 bytes)			{ InterlockedExchangeAdd64(&l64TotalBytesOut, bytes); }
	void IncrementCommands()				{ InterlockedIncrement(&lCommandsProcessed); }

	void LogStatistics();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GameServer connection tracking for forwarding control commands
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_GAMESERVER_CONNECTIONS	8

struct GameServerConnection
{
	bool		bConnected;
	int			i32ServerId;
	int			i32SessionIdx;				// Session index in ControlServer for this GS connection
	int			i32CCU;
	int			i32RoomCount;
	int			i32BattleCount;
	DWORD		dwLastHeartbeat;

	GameServerConnection()
	{
		Reset();
	}

	void Reset()
	{
		bConnected = false;
		i32ServerId = -1;
		i32SessionIdx = -1;
		i32CCU = 0;
		i32RoomCount = 0;
		i32BattleCount = 0;
		dwLastHeartbeat = 0;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log ring buffer for storing recent log entries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_LOG_ENTRIES				256
#define MAX_LOG_MESSAGE_LEN			256
#define MAX_LOG_SOURCE_LEN			32

struct LogEntry
{
	uint8_t		ui8Level;
	uint32_t	ui32Timestamp;
	char		szSource[MAX_LOG_SOURCE_LEN];
	char		szMessage[MAX_LOG_MESSAGE_LEN];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configuration specific to ControlServer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ControlServerConfig : public BaseServerConfig
{
	// GameServer connection info
	char		szGameServerIP[MAX_SERVER_IP_LENGTH];
	uint16_t	ui16GameServerPort;

	// Admin account count (loaded from INI)
	int			i32AdminAccountCount;

	ControlServerConfig()
		: ui16GameServerPort(0)
		, i32AdminAccountCount(0)
	{
		szGameServerIP[0] = '\0';
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlServerContext - i3NetworkServerContext implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ControlServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE(ControlServerContext);

public:
	ControlServerContext();
	virtual ~ControlServerContext();

	// i3NetworkServerContext overrides
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
						  UINT8* pTimeOut, INT32 WorkCount,
						  i3NetworkSessionManager* pSessionManager) override;
	virtual void OnUpdate(INT32 Command) override;
	virtual BOOL OnDestroy() override;

	// Accessors
	ControlSessionManager*	GetSessionManager()					{ return m_pControlSessionManager; }
	ControlServerStatistics* GetStatistics()					{ return &m_Statistics; }

	// Admin account management
	bool					ValidateAdmin(const char* pszUsername, const char* pszPassword, uint8_t* pOutLevel);
	void					AddAdminAccount(const char* pszUsername, const char* pszPassword, uint8_t ui8Level);

	// GameServer connection tracking
	void					RegisterGameServer(int i32ServerId, int i32SessionIdx);
	void					UnregisterGameServer(int i32ServerId);
	void					UpdateGameServerStatus(int i32ServerId, int i32CCU, int i32Rooms, int i32Battles);
	GameServerConnection*	GetGameServerConnection(int i32ServerId);
	int						GetGameServerSessionIdx(int i32ServerId);
	int						GetTotalCCU() const;
	int						GetTotalRooms() const;
	int						GetTotalBattles() const;
	int						GetConnectedGameServerCount() const;

	// Log management
	void					PushLog(uint8_t ui8Level, const char* pszSource, const char* pszMessage);
	int						GetRecentLogs(LogEntry* pOutEntries, int i32MaxCount) const;

	// Periodic stats
	void					LogStatsPeriodic();

private:
	ControlSessionManager*		m_pControlSessionManager;
	ControlServerStatistics		m_Statistics;

	// Admin accounts
	ControlAdminAccount			m_AdminAccounts[MAX_CONTROL_ADMIN_ACCOUNTS];
	int							m_i32AdminAccountCount;

	// GameServer connections
	GameServerConnection		m_GameServers[MAX_GAMESERVER_CONNECTIONS];
	CRITICAL_SECTION			m_csGameServers;

	// Log ring buffer
	LogEntry					m_LogBuffer[MAX_LOG_ENTRIES];
	int							m_i32LogHead;
	int							m_i32LogCount;
	CRITICAL_SECTION			m_csLogs;
};

extern ControlServerContext* g_pControlServerContext;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlServer - BaseServer wrapper
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ControlServer : public BaseServer
{
public:
	ControlServer();
	virtual ~ControlServer();

	// IServer
	virtual ServerType	GetType() const override		{ return ServerType::Control; }
	virtual const char*	GetName() const override		{ return "ControlServer"; }

	// Accessors
	ControlServerContext*		GetControlContext()					{ return m_pControlContext; }
	const ControlServerConfig&	GetControlConfig() const			{ return m_ControlConfig; }

protected:
	// BaseServer overrides
	virtual bool		OnLoadConfig(const char* pszConfigPath) override;
	virtual bool		OnInitialize() override;
	virtual i3NetworkServerContext*		OnCreateContext() override;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() override;
	virtual void		OnUpdate() override;
	virtual void		OnShutdown() override;

private:
	ControlServerContext*	m_pControlContext;
	ControlServerConfig		m_ControlConfig;
};

#endif // __CONTROLSERVERCONTEXT_H__
