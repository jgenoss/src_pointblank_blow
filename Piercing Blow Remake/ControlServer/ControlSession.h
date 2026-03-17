#ifndef __CONTROLSESSION_H__
#define __CONTROLSESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "InterServerProtocol.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlSession states
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum ControlSessionState
{
	CONTROL_STATE_NONE = 0,
	CONTROL_STATE_CONNECTED,			// TCP connected, awaiting LOGIN
	CONTROL_STATE_AUTHENTICATED,		// Admin authenticated, can issue commands
	CONTROL_STATE_DISCONNECTING,
};

// Admin permission levels
#define ADMIN_LEVEL_VIEWER		0		// Can only view status/logs
#define ADMIN_LEVEL_MODERATOR	1		// Can kick, view player info
#define ADMIN_LEVEL_ADMIN		2		// Can ban, announce, manage boosts
#define ADMIN_LEVEL_SUPERADMIN	3		// Can reload, shutdown

#define CONTROL_SESSION_TIMEOUT		300		// 5 minutes idle timeout
#define MAX_CONTROL_LINE_SIZE		2048	// Max text command line size
#define MAX_CONTROL_RESPONSE_SIZE	8192	// Max text response size

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ControlSession - Admin panel session using text-based protocol over TCP
//
// Protocol: Line-based text commands terminated by \n
// Each command is a single line. Responses are multi-line terminated by "END\n".
//
// Commands:
//   LOGIN <username> <password>
//   STATUS
//   KICK <uid>
//   BAN <uid> <duration_seconds> <reason>
//   ANNOUNCE <message>
//   RELOAD [flags]
//   PLAYERINFO <uid>
//   BOOST <type> <exp_mult> <gp_mult> <start_unix> <end_unix>
//   ROOMLIST [channel]
//   LOGS [count]
//   SHUTDOWN <reason>
//   HELP
//   QUIT
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ControlSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(ControlSession);

public:
	ControlSession();
	virtual ~ControlSession();

	// i3NetworkSession overrides
	virtual BOOL		Create() override;
	virtual void		Destroy() override;
	virtual BOOL		OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL		OnConnectInit() override;
	virtual BOOL		OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32		PacketParsing(char* pPacket, INT32 iSize) override;

	// State
	ControlSessionState	GetState() const					{ return m_eState; }
	void				SetState(ControlSessionState e)		{ m_eState = e; }
	uint8_t				GetAdminLevel() const				{ return m_ui8AdminLevel; }
	const char*			GetAdminUsername() const				{ return m_szAdminUsername; }

	// Timeout check
	bool				IsTimedOut() const;
	void				RefreshActivity()					{ m_dwLastActivityTime = GetTickCount(); }

	// Inter-server response callbacks (from GameServer)
	void				OnISStatusAck(char* pData, INT32 i32Size);
	void				OnISKickAck(char* pData, INT32 i32Size);
	void				OnISBanAck(char* pData, INT32 i32Size);
	void				OnISAnnounceAck(char* pData, INT32 i32Size);
	void				OnISReloadAck(char* pData, INT32 i32Size);
	void				OnISPlayerInfoAck(char* pData, INT32 i32Size);
	void				OnISBoostSetAck(char* pData, INT32 i32Size);
	void				OnISRoomListAck(char* pData, INT32 i32Size);
	void				OnISShutdownAck(char* pData, INT32 i32Size);
	void				OnISLogNotify(char* pData, INT32 i32Size);

private:
	// Text command processing
	void				ProcessTextLine(const char* pszLine);

	// Command handlers
	void				CmdLogin(const char* pszArgs);
	void				CmdStatus(const char* pszArgs);
	void				CmdKick(const char* pszArgs);
	void				CmdBan(const char* pszArgs);
	void				CmdAnnounce(const char* pszArgs);
	void				CmdReload(const char* pszArgs);
	void				CmdPlayerInfo(const char* pszArgs);
	void				CmdBoost(const char* pszArgs);
	void				CmdRoomList(const char* pszArgs);
	void				CmdLogs(const char* pszArgs);
	void				CmdShutdown(const char* pszArgs);
	void				CmdHelp();
	void				CmdQuit();

	// Send text response to admin client
	void				SendResponse(const char* pszFormat, ...);
	void				SendResponseEnd();
	void				SendResponseLine(const char* pszFormat, ...);
	void				SendRawText(const char* pszText, int i32Len);

	// Send inter-server packet to GameServer
	bool				SendToGameServer(uint16_t ui16Protocol, const void* pData, int i32DataSize);
	bool				SendToAllGameServers(uint16_t ui16Protocol, const void* pData, int i32DataSize);

	// Permission check
	bool				RequireAuth();
	bool				RequireLevel(uint8_t ui8MinLevel, const char* pszCommand);

private:
	ControlSessionState	m_eState;
	char				m_szAdminUsername[MAX_ADMIN_USERNAME];
	uint8_t				m_ui8AdminLevel;
	DWORD				m_dwConnectTime;
	DWORD				m_dwLastActivityTime;

	// Text line buffer for accumulating partial lines
	char				m_szLineBuffer[MAX_CONTROL_LINE_SIZE];
	int					m_i32LineBufferPos;

	// Pending request tracking (for matching async responses)
	int					m_i32PendingRequestId;
};

#endif // __CONTROLSESSION_H__
