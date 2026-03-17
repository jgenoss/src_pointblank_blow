#include "ControlServerContext.h"
#include "ControlSessionManager.h"
#include "i3IniParser.h"
#include <cstdio>
#include <cstring>
#include <ctime>

// Global
ControlServerContext* g_pControlServerContext = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlServerStatistics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlServerStatistics::LogStatistics()
{
	DWORD dwNow = GetTickCount();
	if (dwNow - dwLastLogTime < 60000)
		return;
	dwLastLogTime = dwNow;

	DWORD uptimeSec = (dwNow - dwStartTime) / 1000;
	int hours = uptimeSec / 3600;
	int mins = (uptimeSec % 3600) / 60;
	int secs = uptimeSec % 60;

	LONG64 pktsIn = l64TotalPacketsIn;
	LONG64 pktsOut = l64TotalPacketsOut;
	LONG64 bytesIn = l64TotalBytesIn;
	LONG64 bytesOut = l64TotalBytesOut;

	double fKBIn = (double)bytesIn / 1024.0;
	double fKBOut = (double)bytesOut / 1024.0;

	printf("[ControlServer I/O] Uptime=%dh%dm%ds | PktsIn=%lld PktsOut=%lld | "
		   "BytesIn=%.1fKB BytesOut=%.1fKB | Cmds=%ld\n",
		hours, mins, secs,
		pktsIn, pktsOut,
		fKBIn, fKBOut,
		lCommandsProcessed);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlServerContext
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(ControlServerContext, i3NetworkServerContext);

ControlServerContext::ControlServerContext()
	: m_pControlSessionManager(nullptr)
	, m_i32AdminAccountCount(0)
	, m_i32LogHead(0)
	, m_i32LogCount(0)
{
	memset(m_GameServers, 0, sizeof(m_GameServers));
	memset(m_LogBuffer, 0, sizeof(m_LogBuffer));
}

ControlServerContext::~ControlServerContext()
{
}

BOOL ControlServerContext::OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
									UINT8* pTimeOut, INT32 WorkCount,
									i3NetworkSessionManager* pSessionManager)
{
	m_pControlSessionManager = static_cast<ControlSessionManager*>(pSessionManager);

	InitializeCriticalSection(&m_csGameServers);
	InitializeCriticalSection(&m_csLogs);

	if (!i3NetworkServerContext::OnCreate(SocketCount, pAddress, pPort, pTimeOut, WorkCount, pSessionManager))
		return FALSE;

	g_pControlServerContext = this;
	m_Statistics.Reset();

	// Push initial log
	PushLog(0, "ControlServer", "Server started successfully");

	printf("[ControlServerContext] Created. Listening for admin connections.\n");
	return TRUE;
}

void ControlServerContext::OnUpdate(INT32 Command)
{
	// Check for dead GameServer connections (heartbeat timeout)
	EnterCriticalSection(&m_csGameServers);
	DWORD dwNow = GetTickCount();
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected)
		{
			if (dwNow - m_GameServers[i].dwLastHeartbeat > 60000) // 60s timeout
			{
				printf("[ControlServerContext] GameServer ID=%d heartbeat timeout\n",
					m_GameServers[i].i32ServerId);

				char szMsg[128];
				snprintf(szMsg, sizeof(szMsg), "GameServer ID=%d lost connection (heartbeat timeout)",
					m_GameServers[i].i32ServerId);
				PushLog(1, "ControlServer", szMsg);

				m_GameServers[i].Reset();
			}
		}
	}
	LeaveCriticalSection(&m_csGameServers);

	LogStatsPeriodic();
	m_Statistics.LogStatistics();
}

void ControlServerContext::LogStatsPeriodic()
{
	static DWORD s_dwLastLog = 0;
	DWORD dwNow = GetTickCount();
	if (dwNow - s_dwLastLog < 60000)
		return;
	s_dwLastLog = dwNow;

	DWORD uptimeSec = (dwNow - m_Statistics.dwStartTime) / 1000;
	int hours = uptimeSec / 3600;
	int mins = (uptimeSec % 3600) / 60;

	int activeSessions = m_pControlSessionManager ? m_pControlSessionManager->GetActiveCount() : 0;
	int gsCount = GetConnectedGameServerCount();
	int totalCCU = GetTotalCCU();

	printf("[ControlServer Stats] Uptime=%dh%dm | AdminSessions=%d | GameServers=%d | "
		   "TotalCCU=%d | Commands=%ld\n",
		hours, mins, activeSessions, gsCount, totalCCU,
		m_Statistics.lCommandsProcessed);
}

BOOL ControlServerContext::OnDestroy()
{
	g_pControlServerContext = nullptr;

	DeleteCriticalSection(&m_csGameServers);
	DeleteCriticalSection(&m_csLogs);

	return i3NetworkServerContext::OnDestroy();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Admin Account Management
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ControlServerContext::ValidateAdmin(const char* pszUsername, const char* pszPassword, uint8_t* pOutLevel)
{
	if (!pszUsername || !pszPassword)
		return false;

	for (int i = 0; i < m_i32AdminAccountCount; ++i)
	{
		if (strcmp(m_AdminAccounts[i].szUsername, pszUsername) == 0 &&
			strcmp(m_AdminAccounts[i].szPassword, pszPassword) == 0)
		{
			if (pOutLevel)
				*pOutLevel = m_AdminAccounts[i].ui8Level;
			return true;
		}
	}
	return false;
}

void ControlServerContext::AddAdminAccount(const char* pszUsername, const char* pszPassword, uint8_t ui8Level)
{
	if (m_i32AdminAccountCount >= MAX_CONTROL_ADMIN_ACCOUNTS)
	{
		printf("[ControlServerContext] WARNING: Max admin accounts reached (%d)\n", MAX_CONTROL_ADMIN_ACCOUNTS);
		return;
	}

	ControlAdminAccount& acct = m_AdminAccounts[m_i32AdminAccountCount];
	strncpy(acct.szUsername, pszUsername, MAX_ADMIN_USERNAME - 1);
	acct.szUsername[MAX_ADMIN_USERNAME - 1] = '\0';
	strncpy(acct.szPassword, pszPassword, MAX_ADMIN_PASSWORD - 1);
	acct.szPassword[MAX_ADMIN_PASSWORD - 1] = '\0';
	acct.ui8Level = ui8Level;
	m_i32AdminAccountCount++;

	printf("[ControlServerContext] Admin account added: '%s' (level=%d)\n", pszUsername, ui8Level);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GameServer Connection Tracking
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlServerContext::RegisterGameServer(int i32ServerId, int i32SessionIdx)
{
	EnterCriticalSection(&m_csGameServers);
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (!m_GameServers[i].bConnected)
		{
			m_GameServers[i].bConnected = true;
			m_GameServers[i].i32ServerId = i32ServerId;
			m_GameServers[i].i32SessionIdx = i32SessionIdx;
			m_GameServers[i].dwLastHeartbeat = GetTickCount();
			m_GameServers[i].i32CCU = 0;
			m_GameServers[i].i32RoomCount = 0;
			m_GameServers[i].i32BattleCount = 0;

			printf("[ControlServerContext] GameServer ID=%d registered (session=%d)\n",
				i32ServerId, i32SessionIdx);
			break;
		}
	}
	LeaveCriticalSection(&m_csGameServers);
}

void ControlServerContext::UnregisterGameServer(int i32ServerId)
{
	EnterCriticalSection(&m_csGameServers);
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected && m_GameServers[i].i32ServerId == i32ServerId)
		{
			printf("[ControlServerContext] GameServer ID=%d unregistered\n", i32ServerId);
			m_GameServers[i].Reset();
			break;
		}
	}
	LeaveCriticalSection(&m_csGameServers);
}

void ControlServerContext::UpdateGameServerStatus(int i32ServerId, int i32CCU, int i32Rooms, int i32Battles)
{
	EnterCriticalSection(&m_csGameServers);
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected && m_GameServers[i].i32ServerId == i32ServerId)
		{
			m_GameServers[i].i32CCU = i32CCU;
			m_GameServers[i].i32RoomCount = i32Rooms;
			m_GameServers[i].i32BattleCount = i32Battles;
			m_GameServers[i].dwLastHeartbeat = GetTickCount();
			break;
		}
	}
	LeaveCriticalSection(&m_csGameServers);
}

GameServerConnection* ControlServerContext::GetGameServerConnection(int i32ServerId)
{
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected && m_GameServers[i].i32ServerId == i32ServerId)
			return &m_GameServers[i];
	}
	return nullptr;
}

int ControlServerContext::GetGameServerSessionIdx(int i32ServerId)
{
	EnterCriticalSection(&m_csGameServers);
	int idx = -1;
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected && m_GameServers[i].i32ServerId == i32ServerId)
		{
			idx = m_GameServers[i].i32SessionIdx;
			break;
		}
	}
	LeaveCriticalSection(&m_csGameServers);
	return idx;
}

int ControlServerContext::GetTotalCCU() const
{
	int total = 0;
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected)
			total += m_GameServers[i].i32CCU;
	}
	return total;
}

int ControlServerContext::GetTotalRooms() const
{
	int total = 0;
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected)
			total += m_GameServers[i].i32RoomCount;
	}
	return total;
}

int ControlServerContext::GetTotalBattles() const
{
	int total = 0;
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected)
			total += m_GameServers[i].i32BattleCount;
	}
	return total;
}

int ControlServerContext::GetConnectedGameServerCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_GAMESERVER_CONNECTIONS; ++i)
	{
		if (m_GameServers[i].bConnected)
			count++;
	}
	return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log Ring Buffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlServerContext::PushLog(uint8_t ui8Level, const char* pszSource, const char* pszMessage)
{
	EnterCriticalSection(&m_csLogs);

	LogEntry& entry = m_LogBuffer[m_i32LogHead];
	entry.ui8Level = ui8Level;
	entry.ui32Timestamp = (uint32_t)time(nullptr);
	strncpy(entry.szSource, pszSource ? pszSource : "Unknown", MAX_LOG_SOURCE_LEN - 1);
	entry.szSource[MAX_LOG_SOURCE_LEN - 1] = '\0';
	strncpy(entry.szMessage, pszMessage ? pszMessage : "", MAX_LOG_MESSAGE_LEN - 1);
	entry.szMessage[MAX_LOG_MESSAGE_LEN - 1] = '\0';

	m_i32LogHead = (m_i32LogHead + 1) % MAX_LOG_ENTRIES;
	if (m_i32LogCount < MAX_LOG_ENTRIES)
		m_i32LogCount++;

	LeaveCriticalSection(&m_csLogs);
}

int ControlServerContext::GetRecentLogs(LogEntry* pOutEntries, int i32MaxCount) const
{
	if (!pOutEntries || i32MaxCount <= 0)
		return 0;

	// Note: caller should hold lock or accept slight inconsistency
	int count = (m_i32LogCount < i32MaxCount) ? m_i32LogCount : i32MaxCount;

	// Read from most recent backward
	int readIdx = (m_i32LogHead - 1 + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
	for (int i = 0; i < count; ++i)
	{
		pOutEntries[i] = m_LogBuffer[readIdx];
		readIdx = (readIdx - 1 + MAX_LOG_ENTRIES) % MAX_LOG_ENTRIES;
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlServer (BaseServer wrapper)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ControlServer::ControlServer()
	: m_pControlContext(nullptr)
{
}

ControlServer::~ControlServer()
{
}

bool ControlServer::OnLoadConfig(const char* pszConfigPath)
{
	// Defaults
	strcpy(m_ControlConfig.szBindIP, "127.0.0.1");	// Only local by default for security
	m_ControlConfig.ui16BindPort			= 40500;
	m_ControlConfig.i32MaxSessions			= 16;
	m_ControlConfig.i32WorkerThreadCount	= 2;
	m_ControlConfig.ui8SocketTimeout		= 120;		// Admin sessions can be longer
	strcpy(m_ControlConfig.szGameServerIP, "127.0.0.1");
	m_ControlConfig.ui16GameServerPort		= 40002;
	m_ControlConfig.i32AdminAccountCount	= 0;

	// Load from INI file
	i3IniParser ini;
	if (ini.Load(pszConfigPath))
	{
		const char* pszBindIP = ini.GetString("ControlServer", "BindIP", "127.0.0.1");
		strncpy_s(m_ControlConfig.szBindIP, pszBindIP, _TRUNCATE);
		m_ControlConfig.ui16BindPort		= (uint16_t)ini.GetInt("ControlServer", "BindPort", 40500);
		m_ControlConfig.i32MaxSessions		= ini.GetInt("ControlServer", "MaxSessions", 16);
		m_ControlConfig.i32WorkerThreadCount = ini.GetInt("ControlServer", "WorkerThreads", 2);
		m_ControlConfig.ui8SocketTimeout	= (uint8_t)ini.GetInt("ControlServer", "SocketTimeout", 120);

		const char* pszGSIP = ini.GetString("GameServer", "IP", "127.0.0.1");
		strncpy_s(m_ControlConfig.szGameServerIP, pszGSIP, _TRUNCATE);
		m_ControlConfig.ui16GameServerPort	= (uint16_t)ini.GetInt("GameServer", "Port", 40002);
	}
	else
	{
		printf("[ControlServer] WARNING: Cannot load config '%s', using defaults\n", pszConfigPath);
	}

	// Copy to base config
	m_Config = m_ControlConfig;

	printf("[ControlServer] Config loaded: Bind=%s:%d, MaxSessions=%d\n",
		m_ControlConfig.szBindIP, m_ControlConfig.ui16BindPort, m_ControlConfig.i32MaxSessions);
	printf("  GameServer: %s:%d\n",
		m_ControlConfig.szGameServerIP, m_ControlConfig.ui16GameServerPort);

	return true;
}

bool ControlServer::OnInitialize()
{
	if (!m_pControlContext)
		return false;

	// Load admin accounts from config
	i3IniParser ini;
	if (ini.Load(m_szConfigPath))
	{
		// Load admin accounts: [Admin.0], [Admin.1], ...
		for (int i = 0; i < MAX_CONTROL_ADMIN_ACCOUNTS; ++i)
		{
			char szSection[32];
			snprintf(szSection, sizeof(szSection), "Admin.%d", i);

			const char* pszUsername = ini.GetString(szSection, "Username", "");
			const char* pszPassword = ini.GetString(szSection, "Password", "");
			int level = ini.GetInt(szSection, "Level", -1);

			if (pszUsername[0] != '\0' && pszPassword[0] != '\0' && level >= 0)
			{
				m_pControlContext->AddAdminAccount(pszUsername, pszPassword, (uint8_t)level);
			}
			else
			{
				break; // No more accounts
			}
		}
	}

	// If no admin accounts were loaded, add a default one (dev mode)
	uint8_t dummyLevel = 0;
	if (!m_pControlContext->ValidateAdmin("admin", "admin", &dummyLevel))
	{
		printf("[ControlServer] WARNING: No admin accounts found, adding default admin/admin (level=3)\n");
		m_pControlContext->AddAdminAccount("admin", "admin", 3);
	}

	return true;
}

i3NetworkServerContext* ControlServer::OnCreateContext()
{
	m_pControlContext = new ControlServerContext();
	return m_pControlContext;
}

i3NetworkSessionManager* ControlServer::OnCreateSessionManager()
{
	ControlSessionManager* pManager = new ControlSessionManager();
	return pManager;
}

void ControlServer::OnUpdate()
{
	// Additional per-tick update if needed
}

void ControlServer::OnShutdown()
{
	m_pControlContext = nullptr;
}
