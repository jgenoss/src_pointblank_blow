#ifndef __CASTSESSION_H__
#define __CASTSESSION_H__

#pragma once
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "InterServerProtocol.h"

// ============================================================================
// Cast Server Protocol
// ============================================================================

enum Protocol_InterServer_Cast
{
	PROTOCOL_IS_CAST = 0x7500,

	// GameServer -> CastServer: Register game server
	PROTOCOL_IS_CAST_REGISTER_REQ,
	PROTOCOL_IS_CAST_REGISTER_ACK,

	// GameServer -> CastServer: Forward battle events
	PROTOCOL_IS_CAST_KILL_NOTIFY,
	PROTOCOL_IS_CAST_ROUND_START_NOTIFY,
	PROTOCOL_IS_CAST_ROUND_END_NOTIFY,
	PROTOCOL_IS_CAST_SCORE_UPDATE_NOTIFY,
	PROTOCOL_IS_CAST_POSITION_UPDATE_NOTIFY,
	PROTOCOL_IS_CAST_BATTLE_END_NOTIFY,

	// Cast client -> CastServer: Subscribe/Unsubscribe
	PROTOCOL_IS_CAST_SUBSCRIBE_REQ,
	PROTOCOL_IS_CAST_SUBSCRIBE_ACK,
	PROTOCOL_IS_CAST_UNSUBSCRIBE_REQ,
	PROTOCOL_IS_CAST_UNSUBSCRIBE_ACK,

	// Cast client -> CastServer: Get room list
	PROTOCOL_IS_CAST_ROOM_LIST_REQ,
	PROTOCOL_IS_CAST_ROOM_LIST_ACK,

	// CastServer -> Cast client: Event distribution
	PROTOCOL_IS_CAST_EVENT_NOTIFY,

	// Heartbeat
	PROTOCOL_IS_CAST_HEARTBEAT_REQ,
	PROTOCOL_IS_CAST_HEARTBEAT_ACK,
};

// ============================================================================
// Cast Session Types
// ============================================================================

enum CastSessionType
{
	CAST_SESSION_NONE = 0,
	CAST_SESSION_GAMESERVER,		// GameServer feeding events
	CAST_SESSION_CLIENT,			// Cast viewer client (web/streaming tool)
};

// ============================================================================
// Cast Protocol Structures
// ============================================================================

#pragma pack(push, 1)

struct CAST_REGISTER_REQ
{
	int			i32ServerId;
	char		szServerName[64];
};

struct CAST_REGISTER_ACK
{
	int			i32Result;			// 0 = OK
};

struct CAST_SUBSCRIBE_REQ
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
};

struct CAST_SUBSCRIBE_ACK
{
	int			i32Result;			// 0 = OK, 1 = room not found
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
};

struct CAST_UNSUBSCRIBE_REQ
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
};

struct CAST_UNSUBSCRIBE_ACK
{
	int			i32Result;
};

struct CAST_ROOM_LIST_REQ
{
	int			i32ServerId;		// 0 = all servers
};

struct CAST_ROOM_INFO
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
	uint8_t		ui8RoomState;		// ROOM_STATE enum
	int			i32SubscriberCount;	// Number of viewers watching
};

struct CAST_KILL_EVENT
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint32_t	ui32KillerSlot;
	uint32_t	ui32VictimSlot;
	int64_t		i64KillerUID;
	int64_t		i64VictimUID;
	char		szKillerNick[64];
	char		szVictimNick[64];
	uint32_t	ui32WeaponID;
	uint8_t		ui8Headshot;
};

struct CAST_ROUND_EVENT
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8RoundNum;
	uint8_t		ui8GameMode;
	uint8_t		ui8EventType;		// 0 = start, 1 = end
	uint8_t		ui8WinTeam;			// For round end
	int			i32RedScore;
	int			i32BlueScore;
};

struct CAST_SCORE_EVENT
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
	int			i32RedScore;
	int			i32BlueScore;
	uint8_t		ui8NowRound;
};

struct CAST_POSITION_UPDATE
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
	uint8_t		ui8SlotCount;
	// Followed by CAST_SLOT_POSITION[ui8SlotCount]
};

struct CAST_SLOT_POSITION
{
	uint8_t		ui8Slot;
	float		fX;
	float		fY;
	float		fZ;
	float		fYaw;
	uint8_t		ui8Team;
	uint8_t		ui8Alive;
};

#pragma pack(pop)

// ============================================================================
// Subscription tracking per room
// ============================================================================

#define MAX_SUBSCRIPTIONS_PER_SESSION	8

struct CastSubscription
{
	int			i32ServerId;
	int			i32RoomIdx;
	int			i32ChannelNum;
	bool		bActive;

	void Reset()
	{
		i32ServerId = 0;
		i32RoomIdx = -1;
		i32ChannelNum = -1;
		bActive = false;
	}
};

// ============================================================================
// CastSession
// ============================================================================

class CastSession : public i3NetworkSession
{
	I3_CLASS_DEFINE(CastSession);

public:
	CastSession();
	virtual ~CastSession();

	// i3NetworkSession overrides
	virtual BOOL		Create() override;
	virtual void		Destroy() override;
	virtual BOOL		OnConnect(SOCKET Socket, struct sockaddr_in* pAddr) override;
	virtual BOOL		OnDisconnect(BOOL bForceMainThread = FALSE) override;
	virtual INT32		PacketParsing(char* pPacket, INT32 iSize) override;

	// Session type
	CastSessionType		GetSessionType() const { return m_eSessionType; }
	bool				IsGameServer() const { return m_eSessionType == CAST_SESSION_GAMESERVER; }
	bool				IsCastClient() const { return m_eSessionType == CAST_SESSION_CLIENT; }
	int					GetGameServerId() const { return m_i32GameServerId; }
	const char*			GetGameServerName() const { return m_szGameServerName; }

	// Subscription management
	bool				IsSubscribedTo(int i32ServerId, int i32RoomIdx, int i32ChannelNum) const;
	int					GetSubscriptionCount() const;

	// Forward event to this client if subscribed
	void				ForwardEventIfSubscribed(int i32ServerId, int i32RoomIdx,
												  int i32ChannelNum,
												  char* pData, int i32Size,
												  uint16_t ui16Protocol);

	// Timeout
	bool				IsTimedOut() const;

private:
	// Packet handlers - GameServer registration
	void				OnCastRegisterReq(char* pData, INT32 i32Size);

	// Packet handlers - Client subscriptions
	void				OnCastSubscribeReq(char* pData, INT32 i32Size);
	void				OnCastUnsubscribeReq(char* pData, INT32 i32Size);
	void				OnCastRoomListReq(char* pData, INT32 i32Size);

	// Packet handlers - GameServer event forwarding
	void				OnCastKillNotify(char* pData, INT32 i32Size);
	void				OnCastRoundStartNotify(char* pData, INT32 i32Size);
	void				OnCastRoundEndNotify(char* pData, INT32 i32Size);
	void				OnCastScoreUpdateNotify(char* pData, INT32 i32Size);
	void				OnCastPositionUpdateNotify(char* pData, INT32 i32Size);
	void				OnCastBattleEndNotify(char* pData, INT32 i32Size);

	// Heartbeat
	void				OnCastHeartbeatReq(char* pData, INT32 i32Size);

	// Helpers
	void				SendSimpleAck(uint16_t ui16Protocol, int32_t i32Result);

private:
	CastSessionType		m_eSessionType;
	int					m_i32GameServerId;
	char				m_szGameServerName[64];
	CastSubscription	m_Subscriptions[MAX_SUBSCRIPTIONS_PER_SESSION];
	DWORD				m_dwConnectTime;
	DWORD				m_dwLastPacketTime;
};

#endif // __CASTSESSION_H__
