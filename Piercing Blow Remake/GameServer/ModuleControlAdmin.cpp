#include "pch.h"
#include "ModuleControlAdmin.h"
#include "GameSessionManager.h"
#include "GameServerContext.h"
#include "GameContextMain.h"
#include "RoomManager.h"
#include "Room.h"
#include "ModuleDataServer.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>

// Global
ModuleControlAdmin* g_pModuleControlAdmin = nullptr;

// ============================================================================
// Construction / Destruction
// ============================================================================

ModuleControlAdmin::ModuleControlAdmin()
	: m_hListenSocket(INVALID_SOCKET)
	, m_ui16Port(0)
	, m_bRunning(false)
	, m_hKillEvent(nullptr)
	, m_i32AccountCount(0)
	, m_i32LogHead(0)
	, m_i32LogCount(0)
{
	m_szBindIP[0] = '\0';
	memset(m_Sessions, 0, sizeof(m_Sessions));
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
		m_Sessions[i].Reset();
	memset(m_LogBuffer, 0, sizeof(m_LogBuffer));
}

ModuleControlAdmin::~ModuleControlAdmin()
{
	Shutdown();
}

// ============================================================================
// Initialize
// ============================================================================

bool ModuleControlAdmin::Initialize(const char* pszBindIP, uint16_t ui16Port)
{
	if (m_bRunning)
		return false;

	strncpy(m_szBindIP, pszBindIP ? pszBindIP : "127.0.0.1", sizeof(m_szBindIP) - 1);
	m_szBindIP[sizeof(m_szBindIP) - 1] = '\0';
	m_ui16Port = ui16Port;

	InitializeCriticalSection(&m_csSessions);
	InitializeCriticalSection(&m_csLogs);

	// Create listen socket
	m_hListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_hListenSocket == INVALID_SOCKET)
	{
		printf("[ModuleControlAdmin] Failed to create listen socket\n");
		return false;
	}

	// Allow reuse
	int optval = 1;
	setsockopt(m_hListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

	// Bind
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_ui16Port);
	addr.sin_addr.s_addr = inet_addr(m_szBindIP);

	if (bind(m_hListenSocket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		printf("[ModuleControlAdmin] Failed to bind to %s:%d\n", m_szBindIP, m_ui16Port);
		closesocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		return false;
	}

	if (listen(m_hListenSocket, 8) == SOCKET_ERROR)
	{
		printf("[ModuleControlAdmin] Failed to listen\n");
		closesocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
		return false;
	}

	// Set non-blocking
	u_long nonBlocking = 1;
	ioctlsocket(m_hListenSocket, FIONBIO, &nonBlocking);

	m_hKillEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_bRunning = true;

	g_pModuleControlAdmin = this;

	// Start thread
	Start(nullptr);

	PushLog(0, "AdminModule", "Control admin module started");
	printf("[ModuleControlAdmin] Listening on %s:%d\n", m_szBindIP, m_ui16Port);

	return true;
}

void ModuleControlAdmin::Shutdown()
{
	if (!m_bRunning)
		return;

	m_bRunning = false;
	if (m_hKillEvent)
		SetEvent(m_hKillEvent);

	// Wait for thread to finish
	Stop();

	// Close all sessions
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
	{
		if (m_Sessions[i].bActive && m_Sessions[i].hSocket != INVALID_SOCKET)
		{
			closesocket(m_Sessions[i].hSocket);
			m_Sessions[i].Reset();
		}
	}

	// Close listen socket
	if (m_hListenSocket != INVALID_SOCKET)
	{
		closesocket(m_hListenSocket);
		m_hListenSocket = INVALID_SOCKET;
	}

	if (m_hKillEvent)
	{
		CloseHandle(m_hKillEvent);
		m_hKillEvent = nullptr;
	}

	DeleteCriticalSection(&m_csSessions);
	DeleteCriticalSection(&m_csLogs);

	g_pModuleControlAdmin = nullptr;
	printf("[ModuleControlAdmin] Shutdown complete\n");
}

// ============================================================================
// Admin Account Management
// ============================================================================

void ModuleControlAdmin::AddAdminAccount(const char* pszUsername, const char* pszPassword, uint8_t ui8Level)
{
	if (m_i32AccountCount >= MAX_ADMIN_ACCOUNTS)
		return;

	AdminAccount& acct = m_Accounts[m_i32AccountCount];
	strncpy(acct.szUsername, pszUsername, MAX_ADMIN_USERNAME - 1);
	acct.szUsername[MAX_ADMIN_USERNAME - 1] = '\0';
	strncpy(acct.szPassword, pszPassword, MAX_ADMIN_PASSWORD - 1);
	acct.szPassword[MAX_ADMIN_PASSWORD - 1] = '\0';
	acct.ui8Level = ui8Level;
	m_i32AccountCount++;

	printf("[ModuleControlAdmin] Admin account added: '%s' (level=%d)\n", pszUsername, ui8Level);
}

bool ModuleControlAdmin::ValidateAdmin(const char* pszUsername, const char* pszPassword, uint8_t* pOutLevel)
{
	if (!pszUsername || !pszPassword)
		return false;

	for (int i = 0; i < m_i32AccountCount; ++i)
	{
		if (strcmp(m_Accounts[i].szUsername, pszUsername) == 0 &&
			strcmp(m_Accounts[i].szPassword, pszPassword) == 0)
		{
			if (pOutLevel)
				*pOutLevel = m_Accounts[i].ui8Level;
			return true;
		}
	}
	return false;
}

int ModuleControlAdmin::GetActiveCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
	{
		if (m_Sessions[i].bActive)
			count++;
	}
	return count;
}

// ============================================================================
// Log Ring Buffer
// ============================================================================

void ModuleControlAdmin::PushLog(uint8_t ui8Level, const char* pszSource, const char* pszMessage)
{
	EnterCriticalSection(&m_csLogs);

	AdminLogEntry& entry = m_LogBuffer[m_i32LogHead];
	entry.ui8Level = ui8Level;
	entry.ui32Timestamp = (uint32_t)time(nullptr);
	strncpy(entry.szSource, pszSource ? pszSource : "Unknown", MAX_ADMIN_LOG_SOURCE_LEN - 1);
	entry.szSource[MAX_ADMIN_LOG_SOURCE_LEN - 1] = '\0';
	strncpy(entry.szMessage, pszMessage ? pszMessage : "", MAX_ADMIN_LOG_MESSAGE_LEN - 1);
	entry.szMessage[MAX_ADMIN_LOG_MESSAGE_LEN - 1] = '\0';

	m_i32LogHead = (m_i32LogHead + 1) % MAX_ADMIN_LOG_ENTRIES;
	if (m_i32LogCount < MAX_ADMIN_LOG_ENTRIES)
		m_i32LogCount++;

	LeaveCriticalSection(&m_csLogs);

	// Forward to any authenticated admin sessions in real-time
	static const char* s_pszLevels[] = { "INFO", "WARN", "ERROR", "CRIT" };
	const char* pszLevel = (ui8Level <= 3) ? s_pszLevels[ui8Level] : "???";

	EnterCriticalSection(&m_csSessions);
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
	{
		if (m_Sessions[i].bActive && m_Sessions[i].eState == ADMIN_STATE_AUTHENTICATED)
		{
			char szBuf[512];
			int len = snprintf(szBuf, sizeof(szBuf), "[LOG] [%s] [%s] %s\n",
				pszLevel, pszSource ? pszSource : "?", pszMessage ? pszMessage : "");
			if (len > 0)
				SendRaw(&m_Sessions[i], szBuf, len);
		}
	}
	LeaveCriticalSection(&m_csSessions);
}

int ModuleControlAdmin::GetRecentLogs(AdminLogEntry* pOutEntries, int i32MaxCount) const
{
	if (!pOutEntries || i32MaxCount <= 0)
		return 0;

	int count = (m_i32LogCount < i32MaxCount) ? m_i32LogCount : i32MaxCount;
	int readIdx = (m_i32LogHead - 1 + MAX_ADMIN_LOG_ENTRIES) % MAX_ADMIN_LOG_ENTRIES;
	for (int i = 0; i < count; ++i)
	{
		pOutEntries[i] = m_LogBuffer[readIdx];
		readIdx = (readIdx - 1 + MAX_ADMIN_LOG_ENTRIES) % MAX_ADMIN_LOG_ENTRIES;
	}
	return count;
}

// ============================================================================
// Thread Implementation
// ============================================================================

void ModuleControlAdmin::BeforeRunning(void* pUserData)
{
	printf("[ModuleControlAdmin] Thread started\n");
}

UINT32 ModuleControlAdmin::OnRunning(void* pUserData)
{
	while (m_bRunning)
	{
		if (WaitForSingleObject(m_hKillEvent, 50) == WAIT_OBJECT_0)
			break;

		AcceptNewConnections();
		ProcessSessions();
		CleanTimedOutSessions();
	}
	return 0;
}

void ModuleControlAdmin::AfterRunning(void* pUserData)
{
	printf("[ModuleControlAdmin] Thread stopped\n");
}

// ============================================================================
// Accept & Session Processing
// ============================================================================

void ModuleControlAdmin::AcceptNewConnections()
{
	if (m_hListenSocket == INVALID_SOCKET)
		return;

	struct sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	SOCKET hClient = accept(m_hListenSocket, (struct sockaddr*)&clientAddr, &addrLen);

	if (hClient == INVALID_SOCKET)
		return;

	// Find free slot
	EnterCriticalSection(&m_csSessions);
	int freeIdx = -1;
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
	{
		if (!m_Sessions[i].bActive)
		{
			freeIdx = i;
			break;
		}
	}

	if (freeIdx < 0)
	{
		LeaveCriticalSection(&m_csSessions);
		const char* msg = "ERROR: Server full\n";
		send(hClient, msg, (int)strlen(msg), 0);
		closesocket(hClient);
		return;
	}

	AdminSessionInfo& session = m_Sessions[freeIdx];
	session.Reset();
	session.hSocket = hClient;
	session.eState = ADMIN_STATE_CONNECTED;
	session.dwConnectTime = GetTickCount();
	session.dwLastActivityTime = session.dwConnectTime;
	session.bActive = true;

	// Set non-blocking
	u_long nonBlocking = 1;
	ioctlsocket(hClient, FIONBIO, &nonBlocking);

	// Format IP
	uint32_t ip = ntohl(clientAddr.sin_addr.s_addr);
	snprintf(session.szIPString, sizeof(session.szIPString), "%d.%d.%d.%d",
		(ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

	LeaveCriticalSection(&m_csSessions);

	printf("[ModuleControlAdmin] Admin client connected from %s (slot=%d)\n",
		session.szIPString, freeIdx);

	// Send welcome
	SendResponse(&session, "PIERCING BLOW REMAKE - Admin Control Module");
	SendResponse(&session, "Type LOGIN <username> <password> to authenticate.");
	SendResponseEnd(&session);
}

void ModuleControlAdmin::ProcessSessions()
{
	EnterCriticalSection(&m_csSessions);
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
	{
		if (!m_Sessions[i].bActive)
			continue;

		if (m_Sessions[i].eState == ADMIN_STATE_DISCONNECTING)
		{
			DisconnectSession(&m_Sessions[i]);
			continue;
		}

		ProcessSessionData(&m_Sessions[i]);
	}
	LeaveCriticalSection(&m_csSessions);
}

void ModuleControlAdmin::ProcessSessionData(AdminSessionInfo* pSession)
{
	if (!pSession || pSession->hSocket == INVALID_SOCKET)
		return;

	char recvBuf[2048];
	int received = recv(pSession->hSocket, recvBuf, sizeof(recvBuf) - 1, 0);

	if (received == 0)
	{
		// Connection closed
		DisconnectSession(pSession);
		return;
	}

	if (received == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK)
			DisconnectSession(pSession);
		return;
	}

	// Accumulate into line buffer, process complete lines
	for (int i = 0; i < received; ++i)
	{
		char c = recvBuf[i];

		if (c == '\r')
			continue;

		if (c == '\n')
		{
			pSession->szLineBuffer[pSession->i32LineBufferPos] = '\0';
			if (pSession->i32LineBufferPos > 0)
			{
				pSession->dwLastActivityTime = GetTickCount();
				ProcessTextLine(pSession, pSession->szLineBuffer);
			}
			pSession->i32LineBufferPos = 0;
		}
		else
		{
			if (pSession->i32LineBufferPos < MAX_ADMIN_LINE_SIZE - 1)
				pSession->szLineBuffer[pSession->i32LineBufferPos++] = c;
			else
			{
				SendResponse(pSession, "ERROR: Line too long");
				SendResponseEnd(pSession);
				pSession->i32LineBufferPos = 0;
			}
		}
	}
}

void ModuleControlAdmin::CleanTimedOutSessions()
{
	DWORD dwNow = GetTickCount();
	EnterCriticalSection(&m_csSessions);
	for (int i = 0; i < MAX_ADMIN_SESSIONS; ++i)
	{
		if (!m_Sessions[i].bActive)
			continue;
		if (m_Sessions[i].eState == ADMIN_STATE_NONE)
			continue;

		if (dwNow - m_Sessions[i].dwLastActivityTime > (ADMIN_SESSION_TIMEOUT * 1000))
		{
			printf("[ModuleControlAdmin] Session %d timed out (%s)\n", i, m_Sessions[i].szIPString);
			SendResponse(&m_Sessions[i], "ERROR: Session timed out");
			SendResponseEnd(&m_Sessions[i]);
			DisconnectSession(&m_Sessions[i]);
		}
	}
	LeaveCriticalSection(&m_csSessions);
}

void ModuleControlAdmin::DisconnectSession(AdminSessionInfo* pSession)
{
	if (!pSession)
		return;

	if (pSession->szUsername[0] != '\0')
	{
		printf("[ModuleControlAdmin] Admin '%s' disconnected (%s)\n",
			pSession->szUsername, pSession->szIPString);
	}

	if (pSession->hSocket != INVALID_SOCKET)
	{
		closesocket(pSession->hSocket);
	}
	pSession->Reset();
}

// ============================================================================
// Text Command Dispatch
// ============================================================================

void ModuleControlAdmin::ProcessTextLine(AdminSessionInfo* pSession, const char* pszLine)
{
	if (!pszLine || pszLine[0] == '\0')
		return;

	// Parse command
	char szCmd[64] = {};
	const char* pszArgs = nullptr;

	int cmdLen = 0;
	while (pszLine[cmdLen] != '\0' && pszLine[cmdLen] != ' ' && cmdLen < 63)
	{
		szCmd[cmdLen] = pszLine[cmdLen];
		cmdLen++;
	}
	szCmd[cmdLen] = '\0';

	pszArgs = pszLine + cmdLen;
	while (*pszArgs == ' ')
		pszArgs++;

	// Uppercase for matching
	for (int i = 0; szCmd[i]; ++i)
	{
		if (szCmd[i] >= 'a' && szCmd[i] <= 'z')
			szCmd[i] -= 32;
	}

	if (strcmp(szCmd, "LOGIN") == 0)			CmdLogin(pSession, pszArgs);
	else if (strcmp(szCmd, "STATUS") == 0)	CmdStatus(pSession);
	else if (strcmp(szCmd, "KICK") == 0)		CmdKick(pSession, pszArgs);
	else if (strcmp(szCmd, "BAN") == 0)		CmdBan(pSession, pszArgs);
	else if (strcmp(szCmd, "ANNOUNCE") == 0)	CmdAnnounce(pSession, pszArgs);
	else if (strcmp(szCmd, "RELOAD") == 0)	CmdReload(pSession, pszArgs);
	else if (strcmp(szCmd, "PLAYERINFO") == 0) CmdPlayerInfo(pSession, pszArgs);
	else if (strcmp(szCmd, "BOOST") == 0)	CmdBoost(pSession, pszArgs);
	else if (strcmp(szCmd, "ROOMLIST") == 0)	CmdRoomList(pSession, pszArgs);
	else if (strcmp(szCmd, "LOGS") == 0)		CmdLogs(pSession, pszArgs);
	else if (strcmp(szCmd, "SHUTDOWN") == 0)	CmdShutdown(pSession, pszArgs);
	else if (strcmp(szCmd, "HELP") == 0)		CmdHelp(pSession);
	else if (strcmp(szCmd, "QUIT") == 0 || strcmp(szCmd, "EXIT") == 0)
		CmdQuit(pSession);
	else
	{
		SendResponse(pSession, "ERROR: Unknown command '%s'. Type HELP for usage.", szCmd);
		SendResponseEnd(pSession);
	}
}

// ============================================================================
// Permission Helpers
// ============================================================================

bool ModuleControlAdmin::RequireAuth(AdminSessionInfo* pSession)
{
	if (pSession->eState != ADMIN_STATE_AUTHENTICATED)
	{
		SendResponse(pSession, "ERROR: Not authenticated. Use LOGIN <username> <password> first.");
		SendResponseEnd(pSession);
		return false;
	}
	return true;
}

bool ModuleControlAdmin::RequireLevel(AdminSessionInfo* pSession, uint8_t ui8MinLevel, const char* pszCmd)
{
	if (!RequireAuth(pSession))
		return false;

	if (pSession->ui8Level < ui8MinLevel)
	{
		SendResponse(pSession, "ERROR: Insufficient permissions for '%s'. Required: %d, yours: %d",
			pszCmd, ui8MinLevel, pSession->ui8Level);
		SendResponseEnd(pSession);
		return false;
	}
	return true;
}

// ============================================================================
// Command Implementations (direct GameServer access - no inter-server needed)
// ============================================================================

void ModuleControlAdmin::CmdLogin(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (pSession->eState == ADMIN_STATE_AUTHENTICATED)
	{
		SendResponse(pSession, "WARN: Already authenticated as '%s'", pSession->szUsername);
		SendResponseEnd(pSession);
		return;
	}

	char szUser[MAX_ADMIN_USERNAME] = {};
	char szPass[MAX_ADMIN_PASSWORD] = {};
	if (sscanf(pszArgs, "%31s %63s", szUser, szPass) != 2)
	{
		SendResponse(pSession, "ERROR: Usage: LOGIN <username> <password>");
		SendResponseEnd(pSession);
		return;
	}

	uint8_t ui8Level = 0;
	if (ValidateAdmin(szUser, szPass, &ui8Level))
	{
		pSession->eState = ADMIN_STATE_AUTHENTICATED;
		pSession->ui8Level = ui8Level;
		strncpy(pSession->szUsername, szUser, MAX_ADMIN_USERNAME - 1);

		static const char* s_pszLevelNames[] = { "Viewer", "Moderator", "Admin", "SuperAdmin" };
		const char* pszLevelName = (ui8Level <= 3) ? s_pszLevelNames[ui8Level] : "Unknown";

		printf("[ModuleControlAdmin] Admin '%s' authenticated (level=%d) from %s\n",
			szUser, ui8Level, pSession->szIPString);

		char szMsg[128];
		snprintf(szMsg, sizeof(szMsg), "Admin '%s' logged in from %s (level=%d)",
			szUser, pSession->szIPString, ui8Level);
		PushLog(0, "AdminModule", szMsg);

		SendResponse(pSession, "OK: Authenticated as '%s' (level=%d, %s)", szUser, ui8Level, pszLevelName);
		SendResponse(pSession, "Type HELP for available commands.");
	}
	else
	{
		printf("[ModuleControlAdmin] Failed login: user='%s' from %s\n",
			szUser, pSession->szIPString);
		SendResponse(pSession, "ERROR: Invalid credentials");
	}
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdStatus(AdminSessionInfo* pSession)
{
	if (!RequireAuth(pSession))
		return;

	// Direct access to GameServer data
	int ccu = g_pGameSessionManager ? g_pGameSessionManager->GetActiveCount() : 0;
	int rooms = g_pRoomManager ? g_pRoomManager->GetTotalUseRoomCount() : 0;

	DWORD dwUptime = 0;
	if (g_pContextMain)
		dwUptime = (GetTickCount() - g_pContextMain->m_dwServerStartTime) / 1000;
	int hours = dwUptime / 3600;
	int mins = (dwUptime % 3600) / 60;
	int secs = dwUptime % 60;

	int channels = g_pContextMain ? g_pContextMain->m_i32ChannelCount : 0;

	SendResponse(pSession, "=== Server Status ===");
	SendResponse(pSession, "Uptime: %dh %dm %ds", hours, mins, secs);
	SendResponse(pSession, "CCU: %d", ccu);
	SendResponse(pSession, "Channels: %d", channels);
	SendResponse(pSession, "Total Rooms: %d", rooms);
	SendResponse(pSession, "Admin Sessions: %d", GetActiveCount());
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdKick(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_MODERATOR, "KICK"))
		return;

	int64_t i64UID = 0;
	if (sscanf(pszArgs, "%lld", &i64UID) != 1 || i64UID <= 0)
	{
		SendResponse(pSession, "ERROR: Usage: KICK <uid>");
		SendResponseEnd(pSession);
		return;
	}

	if (!g_pGameSessionManager)
	{
		SendResponse(pSession, "ERROR: Session manager unavailable");
		SendResponseEnd(pSession);
		return;
	}

	// Find and kick directly
	GameSession* pTarget = nullptr;
	int maxSessions = g_pContextMain ? g_pContextMain->m_i32SessionCount : 0;
	for (int i = 0; i < maxSessions; i++)
	{
		GameSession* pSess = g_pGameSessionManager->GetSession(i);
		if (pSess && pSess->GetUID() == i64UID && pSess->GetTask() >= GAME_TASK_CHANNEL)
		{
			pTarget = pSess;
			break;
		}
	}

	if (pTarget)
	{
		printf("[ModuleControlAdmin] Admin '%s' kicking UID=%lld (%s)\n",
			pSession->szUsername, (long long)i64UID, pTarget->GetNickname());

		pTarget->OnDisconnect(TRUE);
		SendResponse(pSession, "OK: Player UID=%lld kicked", (long long)i64UID);

		char szMsg[128];
		snprintf(szMsg, sizeof(szMsg), "Admin '%s' kicked UID=%lld",
			pSession->szUsername, (long long)i64UID);
		PushLog(0, "AdminModule", szMsg);
	}
	else
	{
		SendResponse(pSession, "WARN: Player UID=%lld not found online", (long long)i64UID);
	}
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdBan(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_ADMIN, "BAN"))
		return;

	int64_t i64UID = 0;
	int i32Duration = 0;
	if (sscanf(pszArgs, "%lld %d", &i64UID, &i32Duration) < 2 || i64UID <= 0)
	{
		SendResponse(pSession, "ERROR: Usage: BAN <uid> <duration_seconds> [reason]");
		SendResponse(pSession, "  duration 0 = permanent ban");
		SendResponseEnd(pSession);
		return;
	}

	// Extract reason after the two numbers
	char szReason[128] = {};
	const char* pReason = pszArgs;
	// Skip uid
	while (*pReason == ' ') pReason++;
	while (*pReason && *pReason != ' ') pReason++;
	// Skip duration
	while (*pReason == ' ') pReason++;
	while (*pReason && *pReason != ' ') pReason++;
	while (*pReason == ' ') pReason++;
	if (*pReason)
		strncpy(szReason, pReason, sizeof(szReason) - 1);
	else
		snprintf(szReason, sizeof(szReason), "Banned by admin '%s'", pSession->szUsername);

	// Persist to DataServer
	if (g_pModuleDataServer && g_pModuleDataServer->IsConnected())
		g_pModuleDataServer->RequestPlayerBan(i64UID, 0, i32Duration, szReason);

	// Kick if online
	if (g_pGameSessionManager)
	{
		int maxSessions = g_pContextMain ? g_pContextMain->m_i32SessionCount : 0;
		for (int i = 0; i < maxSessions; i++)
		{
			GameSession* pSess = g_pGameSessionManager->GetSession(i);
			if (pSess && pSess->GetUID() == i64UID && pSess->GetTask() >= GAME_TASK_CHANNEL)
			{
				pSess->OnDisconnect(TRUE);
				break;
			}
		}
	}

	if (i32Duration == 0)
		SendResponse(pSession, "OK: Permanent ban set for UID=%lld", (long long)i64UID);
	else
		SendResponse(pSession, "OK: Ban set for UID=%lld (duration=%d sec)", (long long)i64UID, i32Duration);

	char szMsg[256];
	snprintf(szMsg, sizeof(szMsg), "Admin '%s' banned UID=%lld for %d sec: %s",
		pSession->szUsername, (long long)i64UID, i32Duration, szReason);
	PushLog(0, "AdminModule", szMsg);
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdAnnounce(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_ADMIN, "ANNOUNCE"))
		return;

	if (!pszArgs || pszArgs[0] == '\0')
	{
		SendResponse(pSession, "ERROR: Usage: ANNOUNCE <message>");
		SendResponseEnd(pSession);
		return;
	}

	if (g_pGameSessionManager)
	{
		g_pGameSessionManager->BroadcastAnnounce(pszArgs, (uint16_t)strlen(pszArgs));
		SendResponse(pSession, "OK: Announcement sent to all players");

		char szMsg[256];
		snprintf(szMsg, sizeof(szMsg), "Admin '%s' announced: %.200s",
			pSession->szUsername, pszArgs);
		PushLog(0, "AdminModule", szMsg);
	}
	else
	{
		SendResponse(pSession, "ERROR: Session manager unavailable");
	}
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdReload(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_SUPERADMIN, "RELOAD"))
		return;

	extern GameServer* g_pGameServer;
	if (g_pGameServer && g_pGameServer->ReloadEconomyConfig())
	{
		SendResponse(pSession, "OK: Config reloaded successfully");
		PushLog(0, "AdminModule", "Config reloaded by admin");
	}
	else
	{
		SendResponse(pSession, "ERROR: Config reload failed");
	}
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdPlayerInfo(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_MODERATOR, "PLAYERINFO"))
		return;

	int64_t i64UID = 0;
	if (sscanf(pszArgs, "%lld", &i64UID) != 1 || i64UID <= 0)
	{
		SendResponse(pSession, "ERROR: Usage: PLAYERINFO <uid>");
		SendResponseEnd(pSession);
		return;
	}

	if (!g_pGameSessionManager)
	{
		SendResponse(pSession, "ERROR: Session manager unavailable");
		SendResponseEnd(pSession);
		return;
	}

	GameSession* pTarget = nullptr;
	int maxSessions = g_pContextMain ? g_pContextMain->m_i32SessionCount : 0;
	for (int i = 0; i < maxSessions; i++)
	{
		GameSession* pSess = g_pGameSessionManager->GetSession(i);
		if (pSess && pSess->GetUID() == i64UID && pSess->GetTask() >= GAME_TASK_CHANNEL)
		{
			pTarget = pSess;
			break;
		}
	}

	if (!pTarget)
	{
		SendResponse(pSession, "WARN: Player UID=%lld not found online", (long long)i64UID);
		SendResponseEnd(pSession);
		return;
	}

	DWORD sessionSec = (GetTickCount() - pTarget->GetConnectTime()) / 1000;
	int hours = sessionSec / 3600;
	int mins = (sessionSec % 3600) / 60;

	SendResponse(pSession, "=== Player Info: UID=%lld ===", (long long)i64UID);
	SendResponse(pSession, "  Nickname: %s", pTarget->GetNickname());
	SendResponse(pSession, "  Level: %d, Rank: %d", pTarget->GetLevel(), pTarget->GetRankId());
	SendResponse(pSession, "  GP: %d", pTarget->GetGP());
	SendResponse(pSession, "  K/D: %d/%d", pTarget->GetKills(), pTarget->GetDeaths());
	SendResponse(pSession, "  Channel: %d, Room: %d", pTarget->GetChannelNum(), pTarget->GetRoomIdx());
	SendResponse(pSession, "  Session: %dh %dm", hours, mins);
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdBoost(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_ADMIN, "BOOST"))
		return;

	int i32Type = 0, i32ExpMult = 100, i32GPMult = 100;
	uint32_t ui32Start = 0, ui32End = 0;

	if (sscanf(pszArgs, "%d %d %d %u %u", &i32Type, &i32ExpMult, &i32GPMult, &ui32Start, &ui32End) < 5)
	{
		SendResponse(pSession, "ERROR: Usage: BOOST <type> <exp_mult> <gp_mult> <start_unix> <end_unix>");
		SendResponse(pSession, "  type: 0=EXP, 1=GP, 2=both");
		SendResponse(pSession, "  multipliers in percent (100=normal, 200=double)");
		SendResponseEnd(pSession);
		return;
	}

	// Apply boost directly to GameContextMain
	if (g_pContextMain)
	{
		g_pContextMain->m_BoostExpMultiplier = i32ExpMult;
		g_pContextMain->m_BoostGPMultiplier = i32GPMult;
		g_pContextMain->m_BoostStartTime = ui32Start;
		g_pContextMain->m_BoostEndTime = ui32End;

		SendResponse(pSession, "OK: Boost set: type=%d EXP=%d%% GP=%d%% start=%u end=%u",
			i32Type, i32ExpMult, i32GPMult, ui32Start, ui32End);

		char szMsg[256];
		snprintf(szMsg, sizeof(szMsg), "Admin '%s' set boost: EXP=%d%% GP=%d%%",
			pSession->szUsername, i32ExpMult, i32GPMult);
		PushLog(0, "AdminModule", szMsg);
	}
	else
	{
		SendResponse(pSession, "ERROR: Context unavailable");
	}
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdRoomList(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireAuth(pSession))
		return;

	int i32Channel = -1;
	if (pszArgs && pszArgs[0] != '\0')
		i32Channel = atoi(pszArgs);

	if (!g_pRoomManager)
	{
		SendResponse(pSession, "ERROR: Room manager unavailable");
		SendResponseEnd(pSession);
		return;
	}

	int totalChannels = g_pContextMain ? g_pContextMain->m_i32ChannelCount : 1;
	int startCh = (i32Channel >= 0) ? i32Channel : 0;
	int endCh = (i32Channel >= 0) ? i32Channel + 1 : totalChannels;

	int totalShown = 0;
	SendResponse(pSession, "=== Room List ===");

	for (int ch = startCh; ch < endCh && totalShown < 50; ch++)
	{
		int roomCount = g_pRoomManager->GetChannelUseRoomCount(ch);
		if (roomCount == 0)
			continue;

		for (int r = 0; r < 200 && totalShown < 50; r++)
		{
			Room* pRoom = g_pRoomManager->GetRoom(ch, r);
			if (!pRoom || !pRoom->IsCreated())
				continue;

			const char* pszState = (pRoom->GetRoomState() == ROOM_STATE_BATTLE) ? "Playing" : "Waiting";
			SendResponse(pSession, "  [%d] Ch=%d Mode=%d Map=%d Players=%d/%d State=%s Name='%s'",
				r, ch, pRoom->GetGameMode(), pRoom->GetMapIndex(),
				pRoom->GetPlayerCount(), pRoom->GetMaxPlayers(),
				pszState, pRoom->GetTitle());
			totalShown++;
		}
	}

	SendResponse(pSession, "=== Total: %d rooms shown ===", totalShown);
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdLogs(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireAuth(pSession))
		return;

	int i32Count = 20;
	if (pszArgs && pszArgs[0] != '\0')
	{
		i32Count = atoi(pszArgs);
		if (i32Count <= 0) i32Count = 20;
		if (i32Count > MAX_ADMIN_LOG_ENTRIES) i32Count = MAX_ADMIN_LOG_ENTRIES;
	}

	AdminLogEntry entries[MAX_ADMIN_LOG_ENTRIES];
	int count = GetRecentLogs(entries, i32Count);

	static const char* s_pszLevelStr[] = { "INFO", "WARN", "ERROR", "CRIT" };

	SendResponse(pSession, "=== Recent Logs (last %d entries) ===", count);
	for (int i = count - 1; i >= 0; --i)
	{
		const char* pszLevel = (entries[i].ui8Level <= 3) ? s_pszLevelStr[entries[i].ui8Level] : "???";
		time_t ts = (time_t)entries[i].ui32Timestamp;
		struct tm tmInfo;
		localtime_s(&tmInfo, &ts);
		char szTime[32];
		strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &tmInfo);

		SendResponse(pSession, "[%s] [%s] [%s] %s",
			szTime, pszLevel, entries[i].szSource, entries[i].szMessage);
	}
	SendResponse(pSession, "=== End of Logs ===");
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdShutdown(AdminSessionInfo* pSession, const char* pszArgs)
{
	if (!RequireLevel(pSession, ADMIN_LEVEL_SUPERADMIN, "SHUTDOWN"))
		return;

	int i32Delay = 30;
	if (pszArgs && pszArgs[0] != '\0')
	{
		int parsed = sscanf(pszArgs, "%d", &i32Delay);
		if (parsed != 1)
			i32Delay = 30;
	}

	// Broadcast shutdown notice
	char notice[128];
	snprintf(notice, sizeof(notice), "[SERVER] Server shutting down in %d seconds...", i32Delay);
	if (g_pGameSessionManager)
		g_pGameSessionManager->BroadcastAnnounce(notice, (uint16_t)strlen(notice));

	SendResponse(pSession, "OK: Shutdown initiated (delay=%d sec)", i32Delay);

	char szMsg[128];
	snprintf(szMsg, sizeof(szMsg), "Admin '%s' initiated shutdown (delay=%ds)",
		pSession->szUsername, i32Delay);
	PushLog(3, "AdminModule", szMsg);

	// Signal main loop to stop
	extern volatile bool g_bRunning;
	g_bRunning = false;

	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdHelp(AdminSessionInfo* pSession)
{
	if (!RequireAuth(pSession))
		return;

	SendResponse(pSession, "=== Admin Control Commands ===");
	SendResponse(pSession, "  LOGIN <username> <password>  - Authenticate");
	SendResponse(pSession, "  STATUS                       - View server status");
	SendResponse(pSession, "  PLAYERINFO <uid>             - View player details [Moderator+]");
	SendResponse(pSession, "  ROOMLIST [channel]           - List active rooms");
	SendResponse(pSession, "  LOGS [count]                 - View recent log entries");
	SendResponse(pSession, "  KICK <uid>                   - Kick a player [Moderator+]");
	SendResponse(pSession, "  BAN <uid> <seconds> [reason] - Ban a player (0=permanent) [Admin+]");
	SendResponse(pSession, "  ANNOUNCE <message>           - Send server announcement [Admin+]");
	SendResponse(pSession, "  BOOST <type> <exp%%> <gp%%> <start> <end> - Set boost event [Admin+]");
	SendResponse(pSession, "  RELOAD [flags]               - Reload server config [SuperAdmin]");
	SendResponse(pSession, "  SHUTDOWN [delay]             - Graceful server shutdown [SuperAdmin]");
	SendResponse(pSession, "  HELP                         - Show this help");
	SendResponse(pSession, "  QUIT                         - Disconnect");
	SendResponse(pSession, "");
	SendResponse(pSession, "  Permission Levels: 0=Viewer, 1=Moderator, 2=Admin, 3=SuperAdmin");
	SendResponse(pSession, "  Your Level: %d", pSession->ui8Level);
	SendResponseEnd(pSession);
}

void ModuleControlAdmin::CmdQuit(AdminSessionInfo* pSession)
{
	SendResponse(pSession, "Goodbye.");
	SendResponseEnd(pSession);
	pSession->eState = ADMIN_STATE_DISCONNECTING;
}

// ============================================================================
// Response Sending
// ============================================================================

void ModuleControlAdmin::SendResponse(AdminSessionInfo* pSession, const char* pszFormat, ...)
{
	char szBuffer[MAX_ADMIN_RESPONSE_SIZE];
	va_list args;
	va_start(args, pszFormat);
	int len = vsnprintf(szBuffer, sizeof(szBuffer) - 2, pszFormat, args);
	va_end(args);

	if (len < 0)
		return;

	if (len < (int)sizeof(szBuffer) - 2)
	{
		szBuffer[len] = '\n';
		szBuffer[len + 1] = '\0';
		len++;
	}

	SendRaw(pSession, szBuffer, len);
}

void ModuleControlAdmin::SendResponseEnd(AdminSessionInfo* pSession)
{
	SendRaw(pSession, "END\n", 4);
}

void ModuleControlAdmin::SendRaw(AdminSessionInfo* pSession, const char* pszText, int i32Len)
{
	if (!pSession || pSession->hSocket == INVALID_SOCKET || !pszText || i32Len <= 0)
		return;

	send(pSession->hSocket, pszText, i32Len, 0);
}
