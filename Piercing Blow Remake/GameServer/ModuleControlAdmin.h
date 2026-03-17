#ifndef __MODULECONTROLADMIN_H__
#define __MODULECONTROLADMIN_H__

#pragma once
#include "i3Thread.h"
#include "i3NetworkPacket.h"

// ============================================================================
// Integrated Admin Control Module for GameServer
// ============================================================================
// Text-based TCP admin interface that runs as a module inside GameServer.
// External admin tools (web panels, CLI tools) connect here to manage
// the server without needing a separate ControlServer process.
//
// Protocol: Line-based text commands terminated by \n
// Responses are multi-line, terminated by "END\n"
//
// Commands:
//   LOGIN <username> <password>
//   STATUS
//   KICK <uid>
//   BAN <uid> <duration_seconds> [reason]
//   ANNOUNCE <message>
//   RELOAD [flags]
//   PLAYERINFO <uid>
//   BOOST <type> <exp_mult> <gp_mult> <start_unix> <end_unix>
//   ROOMLIST [channel]
//   LOGS [count]
//   SHUTDOWN [delay] [reason]
//   HELP
//   QUIT
// ============================================================================

#define MAX_ADMIN_SESSIONS			8
#define MAX_ADMIN_ACCOUNTS			16
#define MAX_ADMIN_USERNAME			32
#define MAX_ADMIN_PASSWORD			64
#define MAX_ADMIN_LINE_SIZE			2048
#define MAX_ADMIN_RESPONSE_SIZE		8192
#define ADMIN_SESSION_TIMEOUT		300		// 5 minutes idle timeout
#define MAX_ADMIN_LOG_ENTRIES		256
#define MAX_ADMIN_LOG_MESSAGE_LEN	256
#define MAX_ADMIN_LOG_SOURCE_LEN	32

// Admin permission levels
#define ADMIN_LEVEL_VIEWER			0
#define ADMIN_LEVEL_MODERATOR		1
#define ADMIN_LEVEL_ADMIN			2
#define ADMIN_LEVEL_SUPERADMIN		3

// ============================================================================
// Admin account entry
// ============================================================================
struct AdminAccount
{
	char		szUsername[MAX_ADMIN_USERNAME];
	char		szPassword[MAX_ADMIN_PASSWORD];
	uint8_t		ui8Level;

	AdminAccount()
		: ui8Level(0)
	{
		szUsername[0] = '\0';
		szPassword[0] = '\0';
	}
};

// ============================================================================
// Log entry for admin log ring buffer
// ============================================================================
struct AdminLogEntry
{
	uint8_t		ui8Level;
	uint32_t	ui32Timestamp;
	char		szSource[MAX_ADMIN_LOG_SOURCE_LEN];
	char		szMessage[MAX_ADMIN_LOG_MESSAGE_LEN];
};

// ============================================================================
// Per-admin connection state
// ============================================================================
enum AdminSessionState
{
	ADMIN_STATE_NONE = 0,
	ADMIN_STATE_CONNECTED,
	ADMIN_STATE_AUTHENTICATED,
	ADMIN_STATE_DISCONNECTING,
};

struct AdminSessionInfo
{
	SOCKET				hSocket;
	AdminSessionState	eState;
	char				szUsername[MAX_ADMIN_USERNAME];
	uint8_t				ui8Level;
	DWORD				dwConnectTime;
	DWORD				dwLastActivityTime;
	char				szLineBuffer[MAX_ADMIN_LINE_SIZE];
	int					i32LineBufferPos;
	char				szIPString[32];
	bool				bActive;

	void Reset()
	{
		hSocket = INVALID_SOCKET;
		eState = ADMIN_STATE_NONE;
		szUsername[0] = '\0';
		ui8Level = 0;
		dwConnectTime = 0;
		dwLastActivityTime = 0;
		szLineBuffer[0] = '\0';
		i32LineBufferPos = 0;
		szIPString[0] = '\0';
		bActive = false;
	}
};

// ============================================================================
// ModuleControlAdmin - runs as a thread inside GameServer
// ============================================================================
class ModuleControlAdmin : public i3Thread
{
public:
	ModuleControlAdmin();
	virtual ~ModuleControlAdmin();

	// Initialization
	bool		Initialize(const char* pszBindIP, uint16_t ui16Port);
	void		Shutdown();

	// Admin account management
	void		AddAdminAccount(const char* pszUsername, const char* pszPassword, uint8_t ui8Level);
	bool		ValidateAdmin(const char* pszUsername, const char* pszPassword, uint8_t* pOutLevel);

	// Log management (called from GameServer systems to push events)
	void		PushLog(uint8_t ui8Level, const char* pszSource, const char* pszMessage);
	int			GetRecentLogs(AdminLogEntry* pOutEntries, int i32MaxCount) const;

	// State
	bool		IsRunning() const		{ return m_bRunning; }
	int			GetActiveCount() const;

protected:
	// i3Thread overrides
	virtual void		BeforeRunning(void* pUserData) override;
	virtual UINT32		OnRunning(void* pUserData) override;
	virtual void		AfterRunning(void* pUserData) override;

private:
	// Accept loop
	void		AcceptNewConnections();
	void		ProcessSessions();
	void		CleanTimedOutSessions();

	// Per-session processing
	void		ProcessSessionData(AdminSessionInfo* pSession);
	void		ProcessTextLine(AdminSessionInfo* pSession, const char* pszLine);
	void		DisconnectSession(AdminSessionInfo* pSession);

	// Command handlers
	void		CmdLogin(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdStatus(AdminSessionInfo* pSession);
	void		CmdKick(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdBan(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdAnnounce(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdReload(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdPlayerInfo(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdBoost(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdRoomList(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdLogs(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdShutdown(AdminSessionInfo* pSession, const char* pszArgs);
	void		CmdHelp(AdminSessionInfo* pSession);
	void		CmdQuit(AdminSessionInfo* pSession);

	// Permission helpers
	bool		RequireAuth(AdminSessionInfo* pSession);
	bool		RequireLevel(AdminSessionInfo* pSession, uint8_t ui8MinLevel, const char* pszCmd);

	// Response helpers
	void		SendResponse(AdminSessionInfo* pSession, const char* pszFormat, ...);
	void		SendResponseEnd(AdminSessionInfo* pSession);
	void		SendRaw(AdminSessionInfo* pSession, const char* pszText, int i32Len);

private:
	// Listen socket
	SOCKET				m_hListenSocket;
	char				m_szBindIP[32];
	uint16_t			m_ui16Port;
	bool				m_bRunning;
	HANDLE				m_hKillEvent;

	// Admin sessions
	AdminSessionInfo	m_Sessions[MAX_ADMIN_SESSIONS];
	CRITICAL_SECTION	m_csSessions;

	// Admin accounts
	AdminAccount		m_Accounts[MAX_ADMIN_ACCOUNTS];
	int					m_i32AccountCount;

	// Log ring buffer
	AdminLogEntry		m_LogBuffer[MAX_ADMIN_LOG_ENTRIES];
	int					m_i32LogHead;
	int					m_i32LogCount;
	CRITICAL_SECTION	m_csLogs;
};

extern ModuleControlAdmin* g_pModuleControlAdmin;

#endif // __MODULECONTROLADMIN_H__
