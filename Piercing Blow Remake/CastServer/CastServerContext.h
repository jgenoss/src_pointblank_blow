#ifndef __CASTSERVERCONTEXT_H__
#define __CASTSERVERCONTEXT_H__

#pragma once
#include "BaseServer.h"
#include "CastSession.h"

class CastSession;

// ============================================================================
// CastServer Room Tracking
// ============================================================================

#define MAX_CAST_ROOMS		256
#define MAX_CAST_SESSIONS	128

struct CastRoomInfo
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
	char		szRoomTitle[64];
	uint8_t		ui8GameMode;
	uint8_t		ui8MapIndex;
	uint8_t		ui8PlayerCount;
	uint8_t		ui8MaxPlayers;
	int			i32RedScore;
	int			i32BlueScore;
	uint8_t		ui8RoomState;
	bool		bActive;

	void Reset()
	{
		i32ServerId = 0;
		i32RoomIdx = -1;
		i32ChannelNum = -1;
		szRoomTitle[0] = '\0';
		ui8GameMode = 0;
		ui8MapIndex = 0;
		ui8PlayerCount = 0;
		ui8MaxPlayers = 0;
		i32RedScore = 0;
		i32BlueScore = 0;
		ui8RoomState = 0;
		bActive = false;
	}
};

// ============================================================================
// CastServer Config
// ============================================================================

struct CastServerConfig : public BaseServerConfig
{
	int			i32ServerId;
	char		szServerName[64];

	CastServerConfig()
		: i32ServerId(1)
	{
		szServerName[0] = '\0';
	}
};

// ============================================================================
// CastServerContext
// ============================================================================
// Streaming/spectator relay server.
// Receives battle events from GameServers and distributes them to connected
// cast clients (web viewers, streaming tools).
// Follows the BaseServer pattern used by ConnectServer, GameServer, etc.

class CastSessionManager;

class CastServerContext : public i3NetworkServerContext
{
	I3_CLASS_DEFINE(CastServerContext);

public:
	CastServerContext();
	virtual ~CastServerContext();

	// i3NetworkServerContext overrides
	virtual BOOL OnCreate(UINT8 SocketCount, UINT32* pAddress, UINT16* pPort,
						  UINT8* pTimeOut, INT32 WorkCount,
						  i3NetworkSessionManager* pSessionManager) override;
	virtual void OnUpdate(INT32 Command) override;
	virtual BOOL OnDestroy() override;

	// Accessors
	CastSessionManager*	GetSessionManager() { return m_pCastSessionManager; }

	// Room tracking
	void		RegisterRoom(int i32ServerId, int i32RoomIdx, int i32ChannelNum,
							 const char* pszTitle, uint8_t ui8Mode, uint8_t ui8Map,
							 uint8_t ui8Players, uint8_t ui8MaxPlayers);
	void		RemoveRoom(int i32ServerId, int i32RoomIdx, int i32ChannelNum);
	void		UpdateRoomScore(int i32ServerId, int i32RoomIdx, int i32ChannelNum,
								int i32RedScore, int i32BlueScore);

	// Called when a GameServer disconnects - remove all its rooms
	void		OnGameServerDisconnect(int i32ServerId);

	// Broadcast event to all clients subscribed to a specific room
	void		BroadcastToSubscribers(int i32ServerId, int i32RoomIdx,
									   int i32ChannelNum,
									   char* pData, int i32Size,
									   uint16_t ui16Protocol);

	// Send room list to a specific client
	void		SendRoomListTo(CastSession* pSession);

	// Get subscriber count for a room
	int			GetSubscriberCount(int i32ServerId, int i32RoomIdx, int i32ChannelNum);

private:
	CastRoomInfo*	FindRoom(int i32ServerId, int i32RoomIdx, int i32ChannelNum);
	CastRoomInfo*	FindFreeRoomSlot();

private:
	CastSessionManager*	m_pCastSessionManager;
	CastRoomInfo		m_Rooms[MAX_CAST_ROOMS];
};

extern CastServerContext* g_pCastServerContext;

// ============================================================================
// CastSessionManager
// ============================================================================

class CastSessionManager : public i3NetworkSessionManager
{
	I3_CLASS_DEFINE(CastSessionManager);

public:
	CastSessionManager();
	virtual ~CastSessionManager();

	// i3NetworkSessionManager overrides
	virtual BOOL		OnCreate(i3List* pSessionList) override;
	virtual void		OnUpdate() override;
	virtual BOOL		OnDestroy() override;
	virtual ULONG_PTR	ConnectSession_v(SOCKET Socket, struct sockaddr_in* pAddr);

	// Accessors
	CastSession*		GetSession(int i32Idx);
	int					GetActiveCount() const { return (int)m_lActiveCount; }

	// Iterate all sessions (for broadcasting)
	int					GetMaxSessions() const { return MAX_CAST_SESSIONS; }

private:
	i3List				m_SessionList;
	CastSession*		m_pSessions;
	volatile LONG		m_lActiveCount;
};

extern CastSessionManager* g_pCastSessionManager;

// ============================================================================
// CastServer (BaseServer wrapper)
// ============================================================================

class CastServer : public BaseServer
{
public:
	CastServer();
	virtual ~CastServer();

	// IServer
	virtual ServerType	GetType() const override		{ return ServerType::Control; }	// Reuse Control type
	virtual const char*	GetName() const override		{ return "CastServer"; }

	// Accessors
	CastServerContext*		GetCastContext() { return m_pCastContext; }
	const CastServerConfig&	GetCastConfig() const { return m_CastConfig; }

protected:
	// BaseServer overrides
	virtual bool		OnLoadConfig(const char* pszConfigPath) override;
	virtual bool		OnInitialize() override;
	virtual i3NetworkServerContext*		OnCreateContext() override;
	virtual i3NetworkSessionManager*	OnCreateSessionManager() override;
	virtual void		OnUpdate() override;
	virtual void		OnShutdown() override;

private:
	CastServerContext*	m_pCastContext;
	CastServerConfig	m_CastConfig;
};

#endif // __CASTSERVERCONTEXT_H__
