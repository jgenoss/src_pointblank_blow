#include "ControlSession.h"
#include "ControlServerContext.h"
#include "ControlSessionManager.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include <ctime>

I3_CLASS_INSTANCE(ControlSession, i3NetworkSession);

ControlSession::ControlSession()
	: m_eState(CONTROL_STATE_NONE)
	, m_ui8AdminLevel(0)
	, m_dwConnectTime(0)
	, m_dwLastActivityTime(0)
	, m_i32LineBufferPos(0)
	, m_i32PendingRequestId(0)
{
	m_szAdminUsername[0] = '\0';
	m_szLineBuffer[0] = '\0';
}

ControlSession::~ControlSession()
{
}

BOOL ControlSession::Create()
{
	return i3NetworkSession::Create();
}

void ControlSession::Destroy()
{
	m_eState = CONTROL_STATE_NONE;
	m_ui8AdminLevel = 0;
	m_szAdminUsername[0] = '\0';
	m_dwConnectTime = 0;
	m_dwLastActivityTime = 0;
	m_i32LineBufferPos = 0;
	m_szLineBuffer[0] = '\0';
	m_i32PendingRequestId = 0;

	i3NetworkSession::Destroy();
}

BOOL ControlSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	if (!i3NetworkSession::OnConnect(Socket, pAddr))
		return FALSE;

	m_eState = CONTROL_STATE_CONNECTED;
	m_dwConnectTime = GetTickCount();
	m_dwLastActivityTime = m_dwConnectTime;
	m_i32LineBufferPos = 0;

	printf("[ControlSession:%d] Admin client connected from %s\n", m_SessionIdx, m_strIPString);

	// Send welcome banner
	SendResponse("PIERCING BLOW REMAKE - Control Server");
	SendResponse("Type LOGIN <username> <password> to authenticate.");
	SendResponseEnd();

	return TRUE;
}

BOOL ControlSession::OnConnectInit()
{
	return i3NetworkSession::OnConnectInit();
}

BOOL ControlSession::OnDisconnect(BOOL bForceMainThread)
{
	if (m_szAdminUsername[0] != '\0')
	{
		printf("[ControlSession:%d] Admin '%s' disconnected\n", m_SessionIdx, m_szAdminUsername);

		if (g_pControlServerContext)
		{
			char szMsg[128];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' disconnected from %s",
				m_szAdminUsername, m_strIPString);
			g_pControlServerContext->PushLog(0, "ControlServer", szMsg);
		}
	}
	else
	{
		printf("[ControlSession:%d] Unauthenticated client disconnected\n", m_SessionIdx);
	}

	m_eState = CONTROL_STATE_NONE;
	m_ui8AdminLevel = 0;
	m_szAdminUsername[0] = '\0';
	m_i32LineBufferPos = 0;

	return i3NetworkSession::OnDisconnect(bForceMainThread);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PacketParsing - Text-based line protocol
// Accumulates data into line buffer, processes complete lines (terminated by \n)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
INT32 ControlSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize <= 0)
		return iSize;

	// Check for inter-server binary packets (protocol ID >= 0x7000)
	// These come from GameServer connections, not admin clients
	if (iSize >= SOCKET_HEAD_SIZE)
	{
		UINT16 ui16DataSize = (*(UINT16*)pPacket) & 0x7FFF;
		UINT16 ui16Protocol = *(UINT16*)(pPacket + SOCKET_HEAD_DATASIZE_SIZE);

		if (ui16Protocol >= 0x7000)
		{
			INT32 i32PacketSize = ui16DataSize + SOCKET_HEAD_SIZE;
			if (iSize < i32PacketSize)
				return iSize; // Incomplete binary packet

			char* pData = pPacket + SOCKET_HEAD_SIZE;
			INT32 i32DataLen = ui16DataSize;

			switch (ui16Protocol)
			{
			case PROTOCOL_IS_HEARTBEAT_REQ:
				// GameServer heartbeat - handle directly
				{
					if (i32DataLen >= (INT32)sizeof(IS_HEARTBEAT_REQ))
					{
						IS_HEARTBEAT_REQ* pReq = (IS_HEARTBEAT_REQ*)pData;
						if (g_pControlServerContext)
						{
							g_pControlServerContext->UpdateGameServerStatus(
								pReq->i32ServerId, pReq->i32CurrentPlayers, 0, 0);
						}
						// Send ACK
						char buffer[32];
						int offset = 0;
						uint16_t size = 0;
						uint16_t proto = PROTOCOL_IS_HEARTBEAT_ACK;
						memcpy(buffer + offset, &size, sizeof(uint16_t));		offset += sizeof(uint16_t);
						memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);
						IS_HEARTBEAT_ACK ack;
						ack.i32Result = 0;
						memcpy(buffer + offset, &ack, sizeof(ack));				offset += sizeof(ack);
						size = (uint16_t)offset;
						memcpy(buffer, &size, sizeof(uint16_t));
						i3NetworkPacket packet;
						packet.SetPacketData(buffer, offset);
						SendMessage(&packet);
					}
				}
				break;

			case PROTOCOL_IS_CONTROL_STATUS_ACK:
				OnISStatusAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_KICK_ACK:
				OnISKickAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_BAN_ACK:
				OnISBanAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_ANNOUNCE_ACK:
				OnISAnnounceAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_RELOAD_ACK:
				OnISReloadAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_PLAYERINFO_ACK:
				OnISPlayerInfoAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_BOOST_SET_ACK:
				OnISBoostSetAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_ROOMLIST_ACK:
				OnISRoomListAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_SHUTDOWN_ACK:
				OnISShutdownAck(pData, i32DataLen);
				break;
			case PROTOCOL_IS_CONTROL_LOG_NOTIFY:
				OnISLogNotify(pData, i32DataLen);
				break;

			default:
				printf("[ControlSession:%d] Unknown IS protocol: 0x%04X\n", m_SessionIdx, ui16Protocol);
				break;
			}

			return i32PacketSize;
		}
	}

	// Text protocol: accumulate bytes into line buffer, process on \n
	int consumed = 0;
	for (int i = 0; i < iSize; ++i)
	{
		char c = pPacket[i];
		consumed++;

		if (c == '\r')
			continue; // Skip CR

		if (c == '\n')
		{
			// Complete line received
			m_szLineBuffer[m_i32LineBufferPos] = '\0';
			if (m_i32LineBufferPos > 0)
			{
				RefreshActivity();
				ProcessTextLine(m_szLineBuffer);

				if (g_pControlServerContext)
					g_pControlServerContext->GetStatistics()->IncrementCommands();
			}
			m_i32LineBufferPos = 0;
		}
		else
		{
			if (m_i32LineBufferPos < MAX_CONTROL_LINE_SIZE - 1)
			{
				m_szLineBuffer[m_i32LineBufferPos++] = c;
			}
			else
			{
				// Line too long, discard
				SendResponse("ERROR: Line too long (max %d chars)", MAX_CONTROL_LINE_SIZE - 1);
				SendResponseEnd();
				m_i32LineBufferPos = 0;
			}
		}
	}

	return consumed;
}

bool ControlSession::IsTimedOut() const
{
	if (m_eState == CONTROL_STATE_NONE)
		return false;

	DWORD dwElapsed = GetTickCount() - m_dwLastActivityTime;
	return (dwElapsed > (CONTROL_SESSION_TIMEOUT * 1000));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Text Command Processing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlSession::ProcessTextLine(const char* pszLine)
{
	if (!pszLine || pszLine[0] == '\0')
		return;

	// Parse command (first word)
	char szCmd[64] = {};
	const char* pszArgs = nullptr;

	// Extract command token
	int cmdLen = 0;
	while (pszLine[cmdLen] != '\0' && pszLine[cmdLen] != ' ' && cmdLen < 63)
	{
		szCmd[cmdLen] = pszLine[cmdLen];
		cmdLen++;
	}
	szCmd[cmdLen] = '\0';

	// Skip whitespace to find args
	pszArgs = pszLine + cmdLen;
	while (*pszArgs == ' ')
		pszArgs++;

	// Convert command to uppercase for case-insensitive matching
	for (int i = 0; szCmd[i]; ++i)
	{
		if (szCmd[i] >= 'a' && szCmd[i] <= 'z')
			szCmd[i] -= 32;
	}

	// Dispatch command
	if (strcmp(szCmd, "LOGIN") == 0)
		CmdLogin(pszArgs);
	else if (strcmp(szCmd, "STATUS") == 0)
		CmdStatus(pszArgs);
	else if (strcmp(szCmd, "KICK") == 0)
		CmdKick(pszArgs);
	else if (strcmp(szCmd, "BAN") == 0)
		CmdBan(pszArgs);
	else if (strcmp(szCmd, "ANNOUNCE") == 0)
		CmdAnnounce(pszArgs);
	else if (strcmp(szCmd, "RELOAD") == 0)
		CmdReload(pszArgs);
	else if (strcmp(szCmd, "PLAYERINFO") == 0)
		CmdPlayerInfo(pszArgs);
	else if (strcmp(szCmd, "BOOST") == 0)
		CmdBoost(pszArgs);
	else if (strcmp(szCmd, "ROOMLIST") == 0)
		CmdRoomList(pszArgs);
	else if (strcmp(szCmd, "LOGS") == 0)
		CmdLogs(pszArgs);
	else if (strcmp(szCmd, "SHUTDOWN") == 0)
		CmdShutdown(pszArgs);
	else if (strcmp(szCmd, "HELP") == 0)
		CmdHelp();
	else if (strcmp(szCmd, "QUIT") == 0 || strcmp(szCmd, "EXIT") == 0)
		CmdQuit();
	else
	{
		SendResponse("ERROR: Unknown command '%s'. Type HELP for usage.", szCmd);
		SendResponseEnd();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Permission helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ControlSession::RequireAuth()
{
	if (m_eState != CONTROL_STATE_AUTHENTICATED)
	{
		SendResponse("ERROR: Not authenticated. Use LOGIN <username> <password> first.");
		SendResponseEnd();
		return false;
	}
	return true;
}

bool ControlSession::RequireLevel(uint8_t ui8MinLevel, const char* pszCommand)
{
	if (!RequireAuth())
		return false;

	if (m_ui8AdminLevel < ui8MinLevel)
	{
		SendResponse("ERROR: Insufficient permissions for '%s'. Required level: %d, your level: %d",
			pszCommand, ui8MinLevel, m_ui8AdminLevel);
		SendResponseEnd();
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Command Implementations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlSession::CmdLogin(const char* pszArgs)
{
	if (m_eState == CONTROL_STATE_AUTHENTICATED)
	{
		SendResponse("WARN: Already authenticated as '%s'", m_szAdminUsername);
		SendResponseEnd();
		return;
	}

	// Parse: LOGIN <username> <password>
	char szUsername[MAX_ADMIN_USERNAME] = {};
	char szPassword[MAX_ADMIN_PASSWORD] = {};

	if (sscanf(pszArgs, "%31s %63s", szUsername, szPassword) != 2)
	{
		SendResponse("ERROR: Usage: LOGIN <username> <password>");
		SendResponseEnd();
		return;
	}

	if (!g_pControlServerContext)
	{
		SendResponse("ERROR: Server not initialized");
		SendResponseEnd();
		return;
	}

	uint8_t ui8Level = 0;
	if (g_pControlServerContext->ValidateAdmin(szUsername, szPassword, &ui8Level))
	{
		m_eState = CONTROL_STATE_AUTHENTICATED;
		m_ui8AdminLevel = ui8Level;
		strncpy(m_szAdminUsername, szUsername, MAX_ADMIN_USERNAME - 1);
		m_szAdminUsername[MAX_ADMIN_USERNAME - 1] = '\0';

		printf("[ControlSession:%d] Admin '%s' authenticated (level=%d) from %s\n",
			m_SessionIdx, szUsername, ui8Level, m_strIPString);

		char szMsg[128];
		snprintf(szMsg, sizeof(szMsg), "Admin '%s' logged in from %s (level=%d)",
			szUsername, m_strIPString, ui8Level);
		g_pControlServerContext->PushLog(0, "ControlServer", szMsg);

		static const char* s_pszLevelNames[] = { "Viewer", "Moderator", "Admin", "SuperAdmin" };
		const char* pszLevelName = (ui8Level <= 3) ? s_pszLevelNames[ui8Level] : "Unknown";

		SendResponse("OK: Authenticated as '%s' (level=%d, %s)", szUsername, ui8Level, pszLevelName);
		SendResponse("Type HELP for available commands.");
		SendResponseEnd();
	}
	else
	{
		printf("[ControlSession:%d] Failed login attempt: user='%s' from %s\n",
			m_SessionIdx, szUsername, m_strIPString);

		if (g_pControlServerContext)
		{
			char szMsg[128];
			snprintf(szMsg, sizeof(szMsg), "Failed login attempt: user='%s' from %s",
				szUsername, m_strIPString);
			g_pControlServerContext->PushLog(1, "ControlServer", szMsg);
		}

		SendResponse("ERROR: Invalid credentials");
		SendResponseEnd();
	}
}

void ControlSession::CmdStatus(const char* pszArgs)
{
	if (!RequireAuth())
		return;

	if (!g_pControlServerContext)
	{
		SendResponse("ERROR: Server context unavailable");
		SendResponseEnd();
		return;
	}

	// Gather local status info from tracked GameServer data
	DWORD dwUptime = (GetTickCount() - g_pControlServerContext->GetStatistics()->dwStartTime) / 1000;
	int hours = dwUptime / 3600;
	int mins = (dwUptime % 3600) / 60;
	int secs = dwUptime % 60;

	int totalCCU = g_pControlServerContext->GetTotalCCU();
	int totalRooms = g_pControlServerContext->GetTotalRooms();
	int totalBattles = g_pControlServerContext->GetTotalBattles();
	int gsCount = g_pControlServerContext->GetConnectedGameServerCount();

	SendResponse("=== Server Status ===");
	SendResponse("ControlServer Uptime: %dh %dm %ds", hours, mins, secs);
	SendResponse("Connected GameServers: %d", gsCount);
	SendResponse("Total CCU: %d", totalCCU);
	SendResponse("Total Rooms: %d", totalRooms);
	SendResponse("Total Battles: %d", totalBattles);
	SendResponse("Commands Processed: %ld", g_pControlServerContext->GetStatistics()->lCommandsProcessed);

	// Also request fresh status from each connected GameServer
	if (gsCount > 0)
	{
		IS_CONTROL_STATUS_REQ req;
		req.i32RequestId = ++m_i32PendingRequestId;
		SendToAllGameServers(PROTOCOL_IS_CONTROL_STATUS_REQ, &req, sizeof(req));
		SendResponse("(Requesting live status from GameServers...)");
	}

	SendResponseEnd();
}

void ControlSession::CmdKick(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_MODERATOR, "KICK"))
		return;

	int64_t i64UID = 0;
	if (sscanf(pszArgs, "%lld", &i64UID) != 1 || i64UID <= 0)
	{
		SendResponse("ERROR: Usage: KICK <uid>");
		SendResponseEnd();
		return;
	}

	IS_CONTROL_KICK_REQ req;
	memset(&req, 0, sizeof(req));
	req.i64UID = i64UID;
	snprintf(req.szReason, sizeof(req.szReason), "Kicked by admin '%s'", m_szAdminUsername);

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_KICK_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Kick request sent for UID=%lld", (long long)i64UID);

		if (g_pControlServerContext)
		{
			char szMsg[128];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' kicked UID=%lld",
				m_szAdminUsername, (long long)i64UID);
			g_pControlServerContext->PushLog(0, "ControlServer", szMsg);
		}
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdBan(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_ADMIN, "BAN"))
		return;

	int64_t i64UID = 0;
	int i32Duration = 0;
	char szReason[128] = {};

	// Parse: BAN <uid> <duration_seconds> <reason...>
	int argsRead = sscanf(pszArgs, "%lld %d", &i64UID, &i32Duration);
	if (argsRead < 2 || i64UID <= 0)
	{
		SendResponse("ERROR: Usage: BAN <uid> <duration_seconds> [reason]");
		SendResponse("  duration 0 = permanent ban");
		SendResponseEnd();
		return;
	}

	// Extract reason (everything after second number)
	const char* pReason = pszArgs;
	// Skip uid
	while (*pReason == ' ') pReason++;
	while (*pReason && *pReason != ' ') pReason++;
	// Skip duration
	while (*pReason == ' ') pReason++;
	while (*pReason && *pReason != ' ') pReason++;
	// Skip space before reason
	while (*pReason == ' ') pReason++;

	if (*pReason != '\0')
		strncpy(szReason, pReason, sizeof(szReason) - 1);
	else
		snprintf(szReason, sizeof(szReason), "Banned by admin '%s'", m_szAdminUsername);

	IS_CONTROL_BAN_REQ req;
	memset(&req, 0, sizeof(req));
	req.i64UID = i64UID;
	req.i32Duration = i32Duration;
	strncpy(req.szReason, szReason, sizeof(req.szReason) - 1);
	req.i64BannedByUID = 0; // Admin system UID

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_BAN_REQ, &req, sizeof(req)))
	{
		if (i32Duration == 0)
			SendResponse("OK: Permanent ban request sent for UID=%lld", (long long)i64UID);
		else
			SendResponse("OK: Ban request sent for UID=%lld (duration=%d seconds)", (long long)i64UID, i32Duration);

		if (g_pControlServerContext)
		{
			char szMsg[256];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' banned UID=%lld for %d sec: %s",
				m_szAdminUsername, (long long)i64UID, i32Duration, szReason);
			g_pControlServerContext->PushLog(0, "ControlServer", szMsg);
		}
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdAnnounce(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_ADMIN, "ANNOUNCE"))
		return;

	if (!pszArgs || pszArgs[0] == '\0')
	{
		SendResponse("ERROR: Usage: ANNOUNCE <message>");
		SendResponseEnd();
		return;
	}

	IS_CONTROL_ANNOUNCE_REQ req;
	memset(&req, 0, sizeof(req));
	strncpy(req.szMessage, pszArgs, sizeof(req.szMessage) - 1);
	req.ui8RepeatCount = 0;
	req.ui16IntervalSec = 0;

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_ANNOUNCE_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Announcement sent to all GameServers");

		if (g_pControlServerContext)
		{
			char szMsg[256];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' announced: %.200s",
				m_szAdminUsername, pszArgs);
			g_pControlServerContext->PushLog(0, "ControlServer", szMsg);
		}
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdReload(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_SUPERADMIN, "RELOAD"))
		return;

	uint8_t ui8Flags = 0xFF; // Reload all by default
	if (pszArgs && pszArgs[0] != '\0')
	{
		ui8Flags = (uint8_t)atoi(pszArgs);
		if (ui8Flags == 0)
			ui8Flags = 0xFF;
	}

	IS_CONTROL_RELOAD_REQ req;
	req.ui8ReloadFlags = ui8Flags;

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_RELOAD_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Reload request sent (flags=0x%02X)", ui8Flags);
		SendResponse("  Flags: 1=config, 2=shop, 4=maps, 8=notices, 16=boosts");

		if (g_pControlServerContext)
		{
			char szMsg[128];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' requested reload (flags=0x%02X)",
				m_szAdminUsername, ui8Flags);
			g_pControlServerContext->PushLog(0, "ControlServer", szMsg);
		}
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdPlayerInfo(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_MODERATOR, "PLAYERINFO"))
		return;

	int64_t i64UID = 0;
	if (sscanf(pszArgs, "%lld", &i64UID) != 1 || i64UID <= 0)
	{
		SendResponse("ERROR: Usage: PLAYERINFO <uid>");
		SendResponseEnd();
		return;
	}

	IS_CONTROL_PLAYERINFO_REQ req;
	req.i64UID = i64UID;

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_PLAYERINFO_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Player info request sent for UID=%lld", (long long)i64UID);
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdBoost(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_ADMIN, "BOOST"))
		return;

	int i32Type = 0, i32ExpMult = 100, i32GPMult = 100;
	uint32_t ui32Start = 0, ui32End = 0;

	int argsRead = sscanf(pszArgs, "%d %d %d %u %u",
		&i32Type, &i32ExpMult, &i32GPMult, &ui32Start, &ui32End);

	if (argsRead < 5)
	{
		SendResponse("ERROR: Usage: BOOST <type> <exp_mult> <gp_mult> <start_unix> <end_unix>");
		SendResponse("  type: 0=EXP, 1=GP, 2=both");
		SendResponse("  multipliers in percent (100=normal, 200=double)");
		SendResponse("  start/end as Unix timestamps");
		SendResponseEnd();
		return;
	}

	IS_CONTROL_BOOST_SET_REQ req;
	req.i32EventType = i32Type;
	req.i32ExpMultiplier = i32ExpMult;
	req.i32GPMultiplier = i32GPMult;
	req.ui32StartTime = ui32Start;
	req.ui32EndTime = ui32End;

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_BOOST_SET_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Boost event set: type=%d EXP=%d%% GP=%d%% start=%u end=%u",
			i32Type, i32ExpMult, i32GPMult, ui32Start, ui32End);

		if (g_pControlServerContext)
		{
			char szMsg[256];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' set boost: type=%d EXP=%d%% GP=%d%%",
				m_szAdminUsername, i32Type, i32ExpMult, i32GPMult);
			g_pControlServerContext->PushLog(0, "ControlServer", szMsg);
		}
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdRoomList(const char* pszArgs)
{
	if (!RequireAuth())
		return;

	int i32Channel = -1;
	if (pszArgs && pszArgs[0] != '\0')
		i32Channel = atoi(pszArgs);

	IS_CONTROL_ROOMLIST_REQ req;
	req.i32ChannelNum = i32Channel;
	req.i32Offset = 0;
	req.i32Limit = 50;

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_ROOMLIST_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Room list request sent (channel=%d)", i32Channel);
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdLogs(const char* pszArgs)
{
	if (!RequireAuth())
		return;

	int i32Count = 20; // Default
	if (pszArgs && pszArgs[0] != '\0')
	{
		i32Count = atoi(pszArgs);
		if (i32Count <= 0) i32Count = 20;
		if (i32Count > MAX_LOG_ENTRIES) i32Count = MAX_LOG_ENTRIES;
	}

	if (!g_pControlServerContext)
	{
		SendResponse("ERROR: Server context unavailable");
		SendResponseEnd();
		return;
	}

	LogEntry entries[MAX_LOG_ENTRIES];
	int count = g_pControlServerContext->GetRecentLogs(entries, i32Count);

	static const char* s_pszLevelStr[] = { "INFO", "WARN", "ERROR", "CRIT" };

	SendResponse("=== Recent Logs (last %d entries) ===", count);
	for (int i = count - 1; i >= 0; --i) // Oldest first
	{
		const char* pszLevel = (entries[i].ui8Level <= 3) ? s_pszLevelStr[entries[i].ui8Level] : "???";

		// Format timestamp
		time_t ts = (time_t)entries[i].ui32Timestamp;
		struct tm tmInfo;
		localtime_s(&tmInfo, &ts);
		char szTime[32];
		strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &tmInfo);

		SendResponse("[%s] [%s] [%s] %s",
			szTime, pszLevel, entries[i].szSource, entries[i].szMessage);
	}
	SendResponse("=== End of Logs ===");
	SendResponseEnd();
}

void ControlSession::CmdShutdown(const char* pszArgs)
{
	if (!RequireLevel(ADMIN_LEVEL_SUPERADMIN, "SHUTDOWN"))
		return;

	int i32Delay = 30; // 30 seconds default
	char szReason[128] = "Server maintenance";

	// Parse: SHUTDOWN [delay] [reason...]
	if (pszArgs && pszArgs[0] != '\0')
	{
		// Try to parse delay first
		int parsed = sscanf(pszArgs, "%d", &i32Delay);
		if (parsed == 1)
		{
			// Find reason after delay number
			const char* p = pszArgs;
			while (*p == ' ') p++;
			while (*p && *p != ' ') p++;
			while (*p == ' ') p++;
			if (*p != '\0')
				strncpy(szReason, p, sizeof(szReason) - 1);
		}
		else
		{
			// No delay, entire arg is reason
			i32Delay = 30;
			strncpy(szReason, pszArgs, sizeof(szReason) - 1);
		}
	}

	IS_CONTROL_SHUTDOWN_REQ req;
	memset(&req, 0, sizeof(req));
	req.i32DelaySec = i32Delay;
	strncpy(req.szReason, szReason, sizeof(req.szReason) - 1);

	if (SendToAllGameServers(PROTOCOL_IS_CONTROL_SHUTDOWN_REQ, &req, sizeof(req)))
	{
		SendResponse("OK: Shutdown request sent. Delay=%d seconds. Reason: %s", i32Delay, szReason);

		if (g_pControlServerContext)
		{
			char szMsg[256];
			snprintf(szMsg, sizeof(szMsg), "Admin '%s' initiated shutdown (delay=%ds): %s",
				m_szAdminUsername, i32Delay, szReason);
			g_pControlServerContext->PushLog(3, "ControlServer", szMsg);
		}
	}
	else
	{
		SendResponse("ERROR: No GameServers connected");
	}
	SendResponseEnd();
}

void ControlSession::CmdHelp()
{
	if (!RequireAuth())
		return;

	SendResponse("=== ControlServer Commands ===");
	SendResponse("  LOGIN <username> <password>  - Authenticate");
	SendResponse("  STATUS                       - View server status (CCU, rooms, battles)");
	SendResponse("  PLAYERINFO <uid>             - View player details [Moderator+]");
	SendResponse("  ROOMLIST [channel]           - List active rooms");
	SendResponse("  LOGS [count]                 - View recent log entries");
	SendResponse("  KICK <uid>                   - Kick a player [Moderator+]");
	SendResponse("  BAN <uid> <seconds> [reason] - Ban a player (0=permanent) [Admin+]");
	SendResponse("  ANNOUNCE <message>           - Send server announcement [Admin+]");
	SendResponse("  BOOST <type> <exp%%> <gp%%> <start> <end> - Set boost event [Admin+]");
	SendResponse("  RELOAD [flags]               - Reload server config [SuperAdmin]");
	SendResponse("  SHUTDOWN [delay] [reason]    - Graceful server shutdown [SuperAdmin]");
	SendResponse("  HELP                         - Show this help");
	SendResponse("  QUIT                         - Disconnect");
	SendResponse("");
	SendResponse("  Permission Levels: 0=Viewer, 1=Moderator, 2=Admin, 3=SuperAdmin");
	SendResponse("  Your Level: %d (%s)", m_ui8AdminLevel,
		m_ui8AdminLevel == 0 ? "Viewer" :
		m_ui8AdminLevel == 1 ? "Moderator" :
		m_ui8AdminLevel == 2 ? "Admin" : "SuperAdmin");
	SendResponseEnd();
}

void ControlSession::CmdQuit()
{
	SendResponse("Goodbye.");
	SendResponseEnd();

	// Schedule disconnect
	m_eState = CONTROL_STATE_DISCONNECTING;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inter-Server Response Handlers (ACK from GameServer)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlSession::OnISStatusAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_STATUS_ACK))
		return;

	IS_CONTROL_STATUS_ACK* pAck = (IS_CONTROL_STATUS_ACK*)pData;

	DWORD uptimeSec = pAck->dwUptime;
	int hours = uptimeSec / 3600;
	int mins = (uptimeSec % 3600) / 60;

	SendResponse("--- GameServer Live Status ---");
	SendResponse("  CCU: %d / %d", pAck->i32CCU, pAck->i32MaxPlayers);
	SendResponse("  Rooms: %d, Battles: %d, Channels: %d", pAck->i32RoomCount, pAck->i32BattleCount, pAck->i32ChannelCount);
	SendResponse("  Uptime: %dh %dm", hours, mins);
	SendResponseEnd();
}

void ControlSession::OnISKickAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_KICK_ACK))
		return;

	IS_CONTROL_KICK_ACK* pAck = (IS_CONTROL_KICK_ACK*)pData;

	if (pAck->i32Result == 0)
		SendResponse("OK: Player UID=%lld kicked successfully", (long long)pAck->i64UID);
	else
		SendResponse("WARN: Player UID=%lld not found on this GameServer", (long long)pAck->i64UID);
	SendResponseEnd();
}

void ControlSession::OnISBanAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_BAN_ACK))
		return;

	IS_CONTROL_BAN_ACK* pAck = (IS_CONTROL_BAN_ACK*)pData;

	if (pAck->i32Result == 0)
		SendResponse("OK: Player UID=%lld banned successfully", (long long)pAck->i64UID);
	else if (pAck->i32Result == 1)
		SendResponse("WARN: Player UID=%lld not found", (long long)pAck->i64UID);
	else
		SendResponse("ERROR: Ban failed for UID=%lld (result=%d)", (long long)pAck->i64UID, pAck->i32Result);
	SendResponseEnd();
}

void ControlSession::OnISAnnounceAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_ANNOUNCE_ACK))
		return;

	IS_CONTROL_ANNOUNCE_ACK* pAck = (IS_CONTROL_ANNOUNCE_ACK*)pData;

	if (pAck->i32Result == 0)
		SendResponse("OK: Announcement delivered");
	else
		SendResponse("ERROR: Announcement delivery failed (result=%d)", pAck->i32Result);
	SendResponseEnd();
}

void ControlSession::OnISReloadAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_RELOAD_ACK))
		return;

	IS_CONTROL_RELOAD_ACK* pAck = (IS_CONTROL_RELOAD_ACK*)pData;

	if (pAck->i32Result == 0)
		SendResponse("OK: Reload completed (reloaded flags=0x%02X)", pAck->ui8ReloadedFlags);
	else
		SendResponse("ERROR: Reload failed (result=%d)", pAck->i32Result);
	SendResponseEnd();
}

void ControlSession::OnISPlayerInfoAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_PLAYERINFO_ACK))
		return;

	IS_CONTROL_PLAYERINFO_ACK* pAck = (IS_CONTROL_PLAYERINFO_ACK*)pData;

	if (pAck->i32Result != 0)
	{
		SendResponse("WARN: Player UID=%lld not found (offline or not on this server)", (long long)pAck->i64UID);
		SendResponseEnd();
		return;
	}

	// Format client IP
	uint32_t ip = pAck->ui32ClientIP;
	char szIP[32];
	snprintf(szIP, sizeof(szIP), "%d.%d.%d.%d",
		(ip & 0xFF), ((ip >> 8) & 0xFF), ((ip >> 16) & 0xFF), ((ip >> 24) & 0xFF));

	DWORD sessionSec = pAck->dwSessionTime;
	int hours = sessionSec / 3600;
	int mins = (sessionSec % 3600) / 60;

	SendResponse("=== Player Info: UID=%lld ===", (long long)pAck->i64UID);
	SendResponse("  Nickname: %s", pAck->szNickname);
	SendResponse("  Level: %d, Rank: %d", pAck->i32Level, pAck->i32RankId);
	SendResponse("  EXP: %lld", (long long)pAck->i64Exp);
	SendResponse("  Cash: %d, GP: %d", pAck->i32Cash, pAck->i32GP);
	SendResponse("  K/D: %d/%d (%.2f)", pAck->i32Kills, pAck->i32Deaths,
		pAck->i32Deaths > 0 ? (float)pAck->i32Kills / (float)pAck->i32Deaths : (float)pAck->i32Kills);
	SendResponse("  Clan: %d", pAck->i32ClanId);
	SendResponse("  Room: %d, Channel: %d", pAck->i32RoomIdx, pAck->i32ChannelIdx);
	SendResponse("  IP: %s", szIP);
	SendResponse("  Session: %dh %dm", hours, mins);
	SendResponseEnd();
}

void ControlSession::OnISBoostSetAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_BOOST_SET_ACK))
		return;

	IS_CONTROL_BOOST_SET_ACK* pAck = (IS_CONTROL_BOOST_SET_ACK*)pData;

	if (pAck->i32Result == 0)
		SendResponse("OK: Boost event applied on GameServer");
	else
		SendResponse("ERROR: Boost event failed (result=%d)", pAck->i32Result);
	SendResponseEnd();
}

void ControlSession::OnISRoomListAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_ROOMLIST_ACK))
		return;

	IS_CONTROL_ROOMLIST_ACK* pAck = (IS_CONTROL_ROOMLIST_ACK*)pData;

	if (pAck->i32Result != 0)
	{
		SendResponse("ERROR: Room list request failed (result=%d)", pAck->i32Result);
		SendResponseEnd();
		return;
	}

	static const char* s_pszStates[] = { "Waiting", "Playing" };

	SendResponse("=== Room List (Total: %d, Showing: %d) ===", pAck->i32TotalRooms, pAck->i32Count);

	// Parse room entries following the ACK header
	char* pEntries = pData + sizeof(IS_CONTROL_ROOMLIST_ACK);
	int remainingSize = i32Size - sizeof(IS_CONTROL_ROOMLIST_ACK);

	for (int i = 0; i < pAck->i32Count; ++i)
	{
		if (remainingSize < (int)sizeof(IS_CONTROL_ROOM_ENTRY))
			break;

		IS_CONTROL_ROOM_ENTRY* pEntry = (IS_CONTROL_ROOM_ENTRY*)pEntries;
		const char* pszState = (pEntry->ui8State <= 1) ? s_pszStates[pEntry->ui8State] : "???";

		SendResponse("  [%d] Ch=%d Mode=%d Map=%d Players=%d/%d State=%s Name='%s'",
			pEntry->i32RoomIdx, pEntry->i32ChannelNum,
			pEntry->ui8GameMode, pEntry->ui8MapIndex,
			pEntry->ui8PlayerCount, pEntry->ui8MaxPlayers,
			pszState, pEntry->szRoomName);

		pEntries += sizeof(IS_CONTROL_ROOM_ENTRY);
		remainingSize -= sizeof(IS_CONTROL_ROOM_ENTRY);
	}

	SendResponse("=== End of Room List ===");
	SendResponseEnd();
}

void ControlSession::OnISShutdownAck(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_SHUTDOWN_ACK))
		return;

	IS_CONTROL_SHUTDOWN_ACK* pAck = (IS_CONTROL_SHUTDOWN_ACK*)pData;

	if (pAck->i32Result == 0)
		SendResponse("OK: GameServer shutdown initiated");
	else
		SendResponse("ERROR: Shutdown request failed (result=%d)", pAck->i32Result);
	SendResponseEnd();
}

void ControlSession::OnISLogNotify(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_CONTROL_LOG_NOTIFY))
		return;

	IS_CONTROL_LOG_NOTIFY* pNotify = (IS_CONTROL_LOG_NOTIFY*)pData;

	// Store in log buffer
	if (g_pControlServerContext)
	{
		g_pControlServerContext->PushLog(pNotify->ui8LogLevel,
			pNotify->szSource, pNotify->szMessage);
	}

	// If this admin session is authenticated, forward the log in real-time
	if (m_eState == CONTROL_STATE_AUTHENTICATED)
	{
		static const char* s_pszLevels[] = { "INFO", "WARN", "ERROR", "CRIT" };
		const char* pszLevel = (pNotify->ui8LogLevel <= 3) ? s_pszLevels[pNotify->ui8LogLevel] : "???";
		SendResponse("[LOG] [%s] [%s] %s", pszLevel, pNotify->szSource, pNotify->szMessage);
		SendResponseEnd();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Response Sending Helpers
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ControlSession::SendResponse(const char* pszFormat, ...)
{
	char szBuffer[MAX_CONTROL_RESPONSE_SIZE];
	va_list args;
	va_start(args, pszFormat);
	int len = vsnprintf(szBuffer, sizeof(szBuffer) - 2, pszFormat, args);
	va_end(args);

	if (len < 0)
		return;

	// Append newline
	if (len < (int)sizeof(szBuffer) - 2)
	{
		szBuffer[len] = '\n';
		szBuffer[len + 1] = '\0';
		len++;
	}

	SendRawText(szBuffer, len);
}

void ControlSession::SendResponseEnd()
{
	SendRawText("END\n", 4);
}

void ControlSession::SendResponseLine(const char* pszFormat, ...)
{
	char szBuffer[MAX_CONTROL_RESPONSE_SIZE];
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

	SendRawText(szBuffer, len);
}

void ControlSession::SendRawText(const char* pszText, int i32Len)
{
	if (!pszText || i32Len <= 0)
		return;

	// Build a binary packet wrapping the text data
	// We use the standard i3 packet format so the i3 network layer handles transmission
	i3NetworkPacket packet;
	// Protocol ID 0 for raw text data (admin protocol is text-based)
	packet.SetProtocol(0);
	packet.WriteData(pszText, (UINT32)i32Len);
	SendMessage(&packet);

	if (g_pControlServerContext)
	{
		g_pControlServerContext->GetStatistics()->IncrementPacketsOut();
		g_pControlServerContext->GetStatistics()->AddBytesOut(i32Len);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inter-Server Packet Sending
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ControlSession::SendToGameServer(uint16_t ui16Protocol, const void* pData, int i32DataSize)
{
	if (!g_pControlServerContext)
		return false;

	// For now, we look for the first connected GameServer
	// In production, you'd specify which GS to target
	ControlSessionManager* pMgr = g_pControlServerContext->GetSessionManager();
	if (!pMgr)
		return false;

	// Find a connected GameServer session
	for (int gs = 0; gs < MAX_GAMESERVER_CONNECTIONS; ++gs)
	{
		GameServerConnection* pGS = g_pControlServerContext->GetGameServerConnection(gs);
		if (!pGS || !pGS->bConnected)
			continue;

		int gsIdx = pGS->i32SessionIdx;
		ControlSession* pGSSession = static_cast<ControlSession*>(pMgr->GetSession(gsIdx));
		if (!pGSSession)
			continue;

		// Build binary packet
		char buffer[2048];
		int offset = 0;

		uint16_t size = 0;
		memcpy(buffer + offset, &size, sizeof(uint16_t));		offset += sizeof(uint16_t);
		memcpy(buffer + offset, &ui16Protocol, sizeof(uint16_t));	offset += sizeof(uint16_t);

		if (pData && i32DataSize > 0 && offset + i32DataSize < (int)sizeof(buffer))
		{
			memcpy(buffer + offset, pData, i32DataSize);
			offset += i32DataSize;
		}

		size = (uint16_t)offset;
		memcpy(buffer, &size, sizeof(uint16_t));

		i3NetworkPacket packet;
		packet.SetPacketData(buffer, offset);
		pGSSession->SendMessage(&packet);
		return true;
	}

	return false;
}

bool ControlSession::SendToAllGameServers(uint16_t ui16Protocol, const void* pData, int i32DataSize)
{
	if (!g_pControlServerContext)
		return false;

	ControlSessionManager* pMgr = g_pControlServerContext->GetSessionManager();
	if (!pMgr)
		return false;

	bool bSentAny = false;

	// Build binary packet once
	char buffer[2048];
	int offset = 0;

	uint16_t size = 0;
	memcpy(buffer + offset, &size, sizeof(uint16_t));			offset += sizeof(uint16_t);
	memcpy(buffer + offset, &ui16Protocol, sizeof(uint16_t));	offset += sizeof(uint16_t);

	if (pData && i32DataSize > 0 && offset + i32DataSize < (int)sizeof(buffer))
	{
		memcpy(buffer + offset, pData, i32DataSize);
		offset += i32DataSize;
	}

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	for (int gs = 0; gs < MAX_GAMESERVER_CONNECTIONS; ++gs)
	{
		GameServerConnection* pGS = &g_pControlServerContext->m_GameServers[gs];
		if (!pGS->bConnected)
			continue;

		int gsIdx = pGS->i32SessionIdx;
		ControlSession* pGSSession = static_cast<ControlSession*>(pMgr->GetSession(gsIdx));
		if (!pGSSession)
			continue;

		i3NetworkPacket packet;
		packet.SetPacketData(buffer, offset);
		pGSSession->SendMessage(&packet);
		bSentAny = true;
	}

	return bSentAny;
}
