#include "pch.h"
#include "CastSession.h"
#include "CastServerContext.h"
#include "ServerLog.h"

// ============================================================================
// CastSession Implementation
// ============================================================================
// Handles both GameServer connections (event sources) and Cast client
// connections (event consumers / web viewers / streaming tools).
// ============================================================================

I3_CLASS_INSTANCE(CastSession);

#define CAST_SESSION_TIMEOUT	120		// 120 seconds

// ============================================================================
// Constructor / Destructor
// ============================================================================

CastSession::CastSession()
	: m_eSessionType(CAST_SESSION_NONE)
	, m_i32GameServerId(0)
	, m_dwConnectTime(0)
	, m_dwLastPacketTime(0)
{
	m_szGameServerName[0] = '\0';
	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
		m_Subscriptions[i].Reset();
}

CastSession::~CastSession()
{
}

// ============================================================================
// i3NetworkSession overrides
// ============================================================================

BOOL CastSession::Create()
{
	return i3NetworkSession::Create();
}

void CastSession::Destroy()
{
	i3NetworkSession::Destroy();
}

BOOL CastSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	m_eSessionType = CAST_SESSION_NONE;
	m_i32GameServerId = 0;
	m_szGameServerName[0] = '\0';
	m_dwConnectTime = GetTickCount();
	m_dwLastPacketTime = m_dwConnectTime;

	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
		m_Subscriptions[i].Reset();

	printf("[CastSession] New connection from %s:%d (idx=%d)\n",
		inet_ntoa(pAddr->sin_addr), ntohs(pAddr->sin_port), GetIndex());

	return TRUE;
}

BOOL CastSession::OnDisconnect(BOOL bForceMainThread)
{
	if (m_eSessionType == CAST_SESSION_GAMESERVER)
	{
		printf("[CastSession] GameServer '%s' (id=%d) disconnected\n",
			m_szGameServerName, m_i32GameServerId);

		// Notify CastServerContext to remove tracked rooms for this GameServer
		if (g_pCastServerContext)
			g_pCastServerContext->OnGameServerDisconnect(m_i32GameServerId);
	}
	else if (m_eSessionType == CAST_SESSION_CLIENT)
	{
		printf("[CastSession] Cast client disconnected (idx=%d)\n", GetIndex());
	}

	m_eSessionType = CAST_SESSION_NONE;
	m_i32GameServerId = 0;
	m_szGameServerName[0] = '\0';

	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
		m_Subscriptions[i].Reset();

	return i3NetworkSession::OnDisconnect(bForceMainThread);
}

// ============================================================================
// PacketParsing
// ============================================================================

INT32 CastSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize < SOCKET_HEAD_SIZE)
		return 0;

	m_dwLastPacketTime = GetTickCount();

	// Parse header: uint16_t size + uint16_t protocol
	uint16_t ui16Size = 0;
	uint16_t ui16Protocol = 0;
	memcpy(&ui16Size, pPacket, sizeof(uint16_t));
	memcpy(&ui16Protocol, pPacket + sizeof(uint16_t), sizeof(uint16_t));

	if (ui16Size > (uint16_t)iSize)
		return 0;

	char* pData = pPacket + SOCKET_HEAD_SIZE;
	INT32 i32DataSize = (INT32)ui16Size - SOCKET_HEAD_SIZE;

	switch (ui16Protocol)
	{
	// GameServer registration
	case PROTOCOL_IS_CAST_REGISTER_REQ:
		OnCastRegisterReq(pData, i32DataSize);
		break;

	// Client subscriptions
	case PROTOCOL_IS_CAST_SUBSCRIBE_REQ:
		OnCastSubscribeReq(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_UNSUBSCRIBE_REQ:
		OnCastUnsubscribeReq(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_ROOM_LIST_REQ:
		OnCastRoomListReq(pData, i32DataSize);
		break;

	// GameServer event forwarding
	case PROTOCOL_IS_CAST_KILL_NOTIFY:
		OnCastKillNotify(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_ROUND_START_NOTIFY:
		OnCastRoundStartNotify(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_ROUND_END_NOTIFY:
		OnCastRoundEndNotify(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_SCORE_UPDATE_NOTIFY:
		OnCastScoreUpdateNotify(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_POSITION_UPDATE_NOTIFY:
		OnCastPositionUpdateNotify(pData, i32DataSize);
		break;
	case PROTOCOL_IS_CAST_BATTLE_END_NOTIFY:
		OnCastBattleEndNotify(pData, i32DataSize);
		break;

	// Heartbeat
	case PROTOCOL_IS_CAST_HEARTBEAT_REQ:
		OnCastHeartbeatReq(pData, i32DataSize);
		break;

	default:
		printf("[CastSession] Unknown protocol 0x%04X from session %d\n",
			ui16Protocol, GetIndex());
		break;
	}

	return (INT32)ui16Size;
}

// ============================================================================
// Packet Handlers - Registration
// ============================================================================

void CastSession::OnCastRegisterReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_REGISTER_REQ))
		return;

	CAST_REGISTER_REQ req;
	memcpy(&req, pData, sizeof(req));

	m_eSessionType = CAST_SESSION_GAMESERVER;
	m_i32GameServerId = req.i32ServerId;
	strncpy_s(m_szGameServerName, req.szServerName, _TRUNCATE);

	printf("[CastSession] GameServer registered: id=%d, name='%s'\n",
		m_i32GameServerId, m_szGameServerName);

	// Send ACK
	SendSimpleAck(PROTOCOL_IS_CAST_REGISTER_ACK, 0);
}

// ============================================================================
// Packet Handlers - Client Subscriptions
// ============================================================================

void CastSession::OnCastSubscribeReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_SUBSCRIBE_REQ))
		return;

	CAST_SUBSCRIBE_REQ req;
	memcpy(&req, pData, sizeof(req));

	// Mark as cast client if not already typed
	if (m_eSessionType == CAST_SESSION_NONE)
		m_eSessionType = CAST_SESSION_CLIENT;

	// Find empty subscription slot
	int i32Slot = -1;
	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
	{
		if (!m_Subscriptions[i].bActive)
		{
			i32Slot = i;
			break;
		}
	}

	if (i32Slot < 0)
	{
		// No empty slot
		SendSimpleAck(PROTOCOL_IS_CAST_SUBSCRIBE_ACK, 2);	// 2 = max subscriptions
		return;
	}

	// Check if already subscribed
	if (IsSubscribedTo(req.i32ServerId, req.i32RoomIdx, req.i32ChannelNum))
	{
		SendSimpleAck(PROTOCOL_IS_CAST_SUBSCRIBE_ACK, 0);	// Already subscribed, OK
		return;
	}

	m_Subscriptions[i32Slot].i32ServerId = req.i32ServerId;
	m_Subscriptions[i32Slot].i32RoomIdx = req.i32RoomIdx;
	m_Subscriptions[i32Slot].i32ChannelNum = req.i32ChannelNum;
	m_Subscriptions[i32Slot].bActive = true;

	printf("[CastSession] Client %d subscribed to Server=%d Room=%d Ch=%d\n",
		GetIndex(), req.i32ServerId, req.i32RoomIdx, req.i32ChannelNum);

	// Send ACK
	char buf[256];
	int offset = 0;

	uint16_t pktSize = (uint16_t)(SOCKET_HEAD_SIZE + sizeof(CAST_SUBSCRIBE_ACK));
	memcpy(buf + offset, &pktSize, sizeof(uint16_t)); offset += sizeof(uint16_t);
	uint16_t proto = PROTOCOL_IS_CAST_SUBSCRIBE_ACK;
	memcpy(buf + offset, &proto, sizeof(uint16_t)); offset += sizeof(uint16_t);

	CAST_SUBSCRIBE_ACK ack;
	ack.i32Result = 0;
	ack.i32ServerId = req.i32ServerId;
	ack.i32RoomIdx = req.i32RoomIdx;
	ack.i32ChannelNum = req.i32ChannelNum;
	memcpy(buf + offset, &ack, sizeof(ack)); offset += sizeof(ack);

	Send(buf, offset);
}

void CastSession::OnCastUnsubscribeReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_UNSUBSCRIBE_REQ))
		return;

	CAST_UNSUBSCRIBE_REQ req;
	memcpy(&req, pData, sizeof(req));

	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
	{
		if (m_Subscriptions[i].bActive &&
			m_Subscriptions[i].i32ServerId == req.i32ServerId &&
			m_Subscriptions[i].i32RoomIdx == req.i32RoomIdx &&
			m_Subscriptions[i].i32ChannelNum == req.i32ChannelNum)
		{
			m_Subscriptions[i].Reset();
			printf("[CastSession] Client %d unsubscribed from Server=%d Room=%d Ch=%d\n",
				GetIndex(), req.i32ServerId, req.i32RoomIdx, req.i32ChannelNum);
			break;
		}
	}

	SendSimpleAck(PROTOCOL_IS_CAST_UNSUBSCRIBE_ACK, 0);
}

void CastSession::OnCastRoomListReq(char* pData, INT32 i32Size)
{
	// Delegate to CastServerContext which tracks active rooms
	if (g_pCastServerContext)
		g_pCastServerContext->SendRoomListTo(this);
}

// ============================================================================
// Packet Handlers - Event Forwarding (GameServer -> CastServer -> Clients)
// ============================================================================

void CastSession::OnCastKillNotify(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_KILL_EVENT))
		return;

	CAST_KILL_EVENT evt;
	memcpy(&evt, pData, sizeof(evt));

	// Update room info in context
	if (g_pCastServerContext)
	{
		g_pCastServerContext->BroadcastToSubscribers(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			pData, i32Size, PROTOCOL_IS_CAST_EVENT_NOTIFY);
	}
}

void CastSession::OnCastRoundStartNotify(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_ROUND_EVENT))
		return;

	CAST_ROUND_EVENT evt;
	memcpy(&evt, pData, sizeof(evt));

	if (g_pCastServerContext)
	{
		g_pCastServerContext->BroadcastToSubscribers(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			pData, i32Size, PROTOCOL_IS_CAST_EVENT_NOTIFY);
	}
}

void CastSession::OnCastRoundEndNotify(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_ROUND_EVENT))
		return;

	CAST_ROUND_EVENT evt;
	memcpy(&evt, pData, sizeof(evt));

	// Update scores in room tracking
	if (g_pCastServerContext)
	{
		g_pCastServerContext->UpdateRoomScore(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			evt.i32RedScore, evt.i32BlueScore);

		g_pCastServerContext->BroadcastToSubscribers(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			pData, i32Size, PROTOCOL_IS_CAST_EVENT_NOTIFY);
	}
}

void CastSession::OnCastScoreUpdateNotify(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_SCORE_EVENT))
		return;

	CAST_SCORE_EVENT evt;
	memcpy(&evt, pData, sizeof(evt));

	if (g_pCastServerContext)
	{
		g_pCastServerContext->UpdateRoomScore(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			evt.i32RedScore, evt.i32BlueScore);

		g_pCastServerContext->BroadcastToSubscribers(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			pData, i32Size, PROTOCOL_IS_CAST_EVENT_NOTIFY);
	}
}

void CastSession::OnCastPositionUpdateNotify(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(CAST_POSITION_UPDATE))
		return;

	CAST_POSITION_UPDATE evt;
	memcpy(&evt, pData, sizeof(evt));

	if (g_pCastServerContext)
	{
		g_pCastServerContext->BroadcastToSubscribers(
			evt.i32ServerId, evt.i32RoomIdx, evt.i32ChannelNum,
			pData, i32Size, PROTOCOL_IS_CAST_EVENT_NOTIFY);
	}
}

void CastSession::OnCastBattleEndNotify(char* pData, INT32 i32Size)
{
	// Battle ended - remove room from tracking
	if (i32Size < 12)	// At least serverId + roomIdx + channelNum
		return;

	int i32ServerId, i32RoomIdx, i32ChannelNum;
	memcpy(&i32ServerId, pData, sizeof(int));
	memcpy(&i32RoomIdx, pData + 4, sizeof(int));
	memcpy(&i32ChannelNum, pData + 8, sizeof(int));

	if (g_pCastServerContext)
	{
		g_pCastServerContext->BroadcastToSubscribers(
			i32ServerId, i32RoomIdx, i32ChannelNum,
			pData, i32Size, PROTOCOL_IS_CAST_EVENT_NOTIFY);

		g_pCastServerContext->RemoveRoom(i32ServerId, i32RoomIdx, i32ChannelNum);
	}
}

// ============================================================================
// Heartbeat
// ============================================================================

void CastSession::OnCastHeartbeatReq(char* pData, INT32 i32Size)
{
	SendSimpleAck(PROTOCOL_IS_CAST_HEARTBEAT_ACK, 0);
}

// ============================================================================
// Subscription queries
// ============================================================================

bool CastSession::IsSubscribedTo(int i32ServerId, int i32RoomIdx, int i32ChannelNum) const
{
	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
	{
		if (m_Subscriptions[i].bActive &&
			m_Subscriptions[i].i32ServerId == i32ServerId &&
			m_Subscriptions[i].i32RoomIdx == i32RoomIdx &&
			m_Subscriptions[i].i32ChannelNum == i32ChannelNum)
		{
			return true;
		}
	}
	return false;
}

int CastSession::GetSubscriptionCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_SUBSCRIPTIONS_PER_SESSION; ++i)
	{
		if (m_Subscriptions[i].bActive)
			count++;
	}
	return count;
}

void CastSession::ForwardEventIfSubscribed(int i32ServerId, int i32RoomIdx,
										   int i32ChannelNum,
										   char* pData, int i32Size,
										   uint16_t ui16Protocol)
{
	if (!IsCastClient())
		return;

	if (!IsSubscribedTo(i32ServerId, i32RoomIdx, i32ChannelNum))
		return;

	// Build packet: size + protocol + event data
	char buf[4096];
	int offset = 0;

	uint16_t pktSize = (uint16_t)(SOCKET_HEAD_SIZE + i32Size);
	if (pktSize > sizeof(buf))
		return;

	memcpy(buf + offset, &pktSize, sizeof(uint16_t)); offset += sizeof(uint16_t);
	memcpy(buf + offset, &ui16Protocol, sizeof(uint16_t)); offset += sizeof(uint16_t);
	memcpy(buf + offset, pData, i32Size); offset += i32Size;

	Send(buf, offset);
}

// ============================================================================
// Timeout
// ============================================================================

bool CastSession::IsTimedOut() const
{
	DWORD dwNow = GetTickCount();
	return (dwNow - m_dwLastPacketTime > (CAST_SESSION_TIMEOUT * 1000));
}

// ============================================================================
// Helpers
// ============================================================================

void CastSession::SendSimpleAck(uint16_t ui16Protocol, int32_t i32Result)
{
	char buf[16];
	int offset = 0;

	uint16_t pktSize = (uint16_t)(SOCKET_HEAD_SIZE + sizeof(int32_t));
	memcpy(buf + offset, &pktSize, sizeof(uint16_t)); offset += sizeof(uint16_t);
	memcpy(buf + offset, &ui16Protocol, sizeof(uint16_t)); offset += sizeof(uint16_t);
	memcpy(buf + offset, &i32Result, sizeof(int32_t)); offset += sizeof(int32_t);

	Send(buf, offset);
}
