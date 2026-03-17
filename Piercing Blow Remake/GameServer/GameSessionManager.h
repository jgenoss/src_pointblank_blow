#ifndef __GAMESESSIONMANAGER_H__
#define __GAMESESSIONMANAGER_H__

#pragma once
#include "i3NetworkSessionManager.h"
#include "i3List.h"
#include "i3Mutex.h"

class GameSession;

#define MAX_GAME_SESSIONS	1000

// Port simplificado de CUserSessionManager
// Gestiona pool de GameSession, listas por canal y lobby
class GameSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE(GameSessionManager);

public:
	GameSessionManager();
	virtual ~GameSessionManager();

	// i3NetworkSessionManager overrides
	virtual BOOL		OnCreate(i3List* pSessionList) override;
	virtual void		OnUpdate() override;
	virtual BOOL		OnDestroy() override;
	virtual ULONG_PTR	ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr);

	// Session lookup
	GameSession*		GetSession(int i32Idx);
	GameSession*		GetSession(int i32Idx, int64_t i64UID);
	GameSession*		FindSessionByUID(int64_t i64UID);
	GameSession*		FindSessionByNickname(const char* szNickname);

	// Channel management
	int					OnEnterChannel(GameSession* pSession, uint32_t ui32Channel);
	void				OnLeaveChannel(GameSession* pSession, uint32_t ui32Channel);
	int					GetChannelUserCount(uint32_t ui32Channel);

	// Lobby management
	void				OnEnterLobby(GameSession* pSession, uint32_t ui32Channel);
	void				OnLeaveLobby(GameSession* pSession, uint32_t ui32Channel);

	// Broadcast
	void				OnSendLobbyChatting(GameSession* pSender, char* pMessage, uint16_t ui16Size);
	void				OnSendChannelUser(int i32ChannelNum, i3NetworkPacket* pPacket);
	void				BroadcastToChannel(int i32ChannelNum, i3NetworkPacket* pPacket);
	void				BroadcastToAll(i3NetworkPacket* pPacket);
	void				BroadcastAnnounce(const char* pszMessage, uint16_t ui16MsgLen);

	// Stats
	int					GetActiveCount() const		{ return (int)m_lActiveCount; }
	int					GetPeakActive() const		{ return (int)m_lPeakActive; }
	int					GetTotalConnections() const	{ return (int)m_lTotalConnections; }

private:
	void				CheckTimeouts();

private:
	// Session pool
	i3List				m_SessionList;
	GameSession*		m_pSessions;

	// Per-channel user lists
	i3List**			m_pChannelUserList;
	i3Mutex**			m_pcsChannelUser;

	// Per-channel lobby user lists
	i3List**			m_pLobbyUserList;
	i3Mutex**			m_pcsLobbyUser;

	// Config
	uint32_t			m_ui32ChannelCount;

	// Stats
	volatile LONG		m_lActiveCount;
	volatile LONG		m_lPeakActive;
	volatile LONG		m_lTotalConnections;
	volatile LONG		m_lSessionCheckIdx;
	DWORD				m_dwLastTimeoutCheck;
};

extern GameSessionManager* g_pGameSessionManager;

#endif // __GAMESESSIONMANAGER_H__
