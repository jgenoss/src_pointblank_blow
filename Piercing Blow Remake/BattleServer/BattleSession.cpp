#include "pch.h"
#include "BattleSession.h"
#include "BattleServerContext.h"
#include "BattleRoomManager.h"
#include "BattleRoom.h"

I3_CLASS_INSTANCE(BattleSession, i3NetworkSession);

BattleSession::BattleSession()
	: m_bRegistered(false)
	, m_i32GameServerId(0)
{
	memset(m_szGameServerName, 0, sizeof(m_szGameServerName));
}

BattleSession::~BattleSession()
{
}

BOOL BattleSession::Create()
{
	return i3NetworkSession::Create();
}

void BattleSession::Destroy()
{
	m_bRegistered = false;
	m_i32GameServerId = 0;
	memset(m_szGameServerName, 0, sizeof(m_szGameServerName));
	i3NetworkSession::Destroy();
}

BOOL BattleSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	if (!i3NetworkSession::OnConnect(Socket, pAddr))
		return FALSE;

	printf("[BattleSession:%d] GameServer connected from %s\n", m_SessionIdx, m_strIPString);
	return TRUE;
}

BOOL BattleSession::OnDisconnect(BOOL bForceMainThread)
{
	printf("[BattleSession:%d] GameServer disconnected (id=%d, name='%s')\n",
		m_SessionIdx, m_i32GameServerId, m_szGameServerName);

	m_bRegistered = false;
	m_i32GameServerId = 0;
	memset(m_szGameServerName, 0, sizeof(m_szGameServerName));

	return i3NetworkSession::OnDisconnect(bForceMainThread);
}

INT32 BattleSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize < SOCKET_HEAD_SIZE)
		return iSize;

	UINT16* pDataSize = (UINT16*)pPacket;
	UINT16* pProtocolID = (UINT16*)(pPacket + SOCKET_HEAD_DATASIZE_SIZE);

	UINT16 ui16DataSize = (*pDataSize) & 0x7FFF;
	INT32 i32PacketSize = ui16DataSize + SOCKET_HEAD_SIZE;

	if (iSize < i32PacketSize)
		return iSize;

	PROTOCOL protocolID = *pProtocolID;
	char* pData = pPacket + SOCKET_HEAD_SIZE;

	switch (protocolID)
	{
	case PROTOCOL_IS_HEARTBEAT_REQ:
		OnHeartbeatReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_BATTLE_REGISTER_REQ:
		OnBattleRegisterReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_BATTLE_CREATE_REQ:
		OnBattleCreateReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_PLAYER_MIGRATE_REQ:
		OnPlayerMigrateReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_BATTLE_STATUS_UPDATE:
		OnBattleStatusUpdate(pData, ui16DataSize);
		break;

	default:
		printf("[BattleSession:%d] Unknown protocol: 0x%04X\n", m_SessionIdx, protocolID);
		break;
	}

	return i32PacketSize;
}

// ============================================================================
// Packet Handlers
// ============================================================================

void BattleSession::OnHeartbeatReq(char* pData, INT32 i32Size)
{
	if (i32Size >= (INT32)sizeof(IS_HEARTBEAT_REQ))
	{
		IS_HEARTBEAT_REQ* pReq = (IS_HEARTBEAT_REQ*)pData;
		m_i32GameServerId = pReq->i32ServerId;
	}

	SendHeartbeatAck();
}

void BattleSession::OnBattleRegisterReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_SERVER_REGISTER_REQ))
		return;

	IS_SERVER_REGISTER_REQ* pReq = (IS_SERVER_REGISTER_REQ*)pData;

	m_i32GameServerId = pReq->i32ServerId;
	strncpy_s(m_szGameServerName, pReq->szServerName, _TRUNCATE);
	m_bRegistered = true;

	printf("[BattleSession:%d] GameServer registered - ID=%d, Name='%s', IP=%s:%d\n",
		m_SessionIdx, m_i32GameServerId, m_szGameServerName,
		pReq->szPublicIP, pReq->ui16PublicPort);

	// Send ACK
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_REGISTER_ACK);
	IS_SERVER_REGISTER_ACK ack;
	ack.i32Result = 0;			// OK
	ack.i32AssignedId = m_i32GameServerId;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void BattleSession::OnBattleCreateReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_BATTLE_CREATE_REQ))
		return;

	IS_BATTLE_CREATE_REQ* pReq = (IS_BATTLE_CREATE_REQ*)pData;

	printf("[BattleSession:%d] BATTLE_CREATE_REQ: Room=%d, Channel=%d, Mode=%d, Map=%d, Players=%d\n",
		m_SessionIdx, pReq->i32RoomIdx, pReq->i32ChannelNum,
		pReq->ui8GameMode, pReq->ui8MapIndex, pReq->i32PlayerCount);

	if (!g_pBattleRoomManager)
	{
		SendBattleCreateAck(pReq->i32RoomIdx, 1, -1, "", 0);	// Error
		return;
	}

	int battleRoomIdx = g_pBattleRoomManager->CreateRoom(pReq, m_SessionIdx);
	if (battleRoomIdx < 0)
	{
		SendBattleCreateAck(pReq->i32RoomIdx, 1, -1, "", 0);	// No room available
		return;
	}

	BattleRoom* pRoom = g_pBattleRoomManager->GetRoom(battleRoomIdx);
	if (!pRoom)
	{
		SendBattleCreateAck(pReq->i32RoomIdx, 1, -1, "", 0);
		return;
	}

	// Get the public IP from config
	const char* pszUdpIP = "127.0.0.1";
	if (g_pBattleServerContext)
	{
		// Use the BattleServer's public IP
		// Will be set from config in BattleServerContext
	}

	SendBattleCreateAck(pReq->i32RoomIdx, 0, battleRoomIdx, pszUdpIP, pRoom->GetUdpPort());
}

void BattleSession::OnPlayerMigrateReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_PLAYER_MIGRATE_REQ))
		return;

	IS_PLAYER_MIGRATE_REQ* pReq = (IS_PLAYER_MIGRATE_REQ*)pData;

	printf("[BattleSession:%d] PLAYER_MIGRATE_REQ: UID=%lld\n",
		m_SessionIdx, pReq->i64UID);

	// TODO: Add player to the battle room when full migrate protocol is implemented
	SendPlayerMigrateAck(pReq->i64UID, 0);
}

void BattleSession::OnBattleStatusUpdate(char* pData, INT32 i32Size)
{
	// Status update from GameServer - informational only in v1
}

// ============================================================================
// Helpers
// ============================================================================

void BattleSession::SendHeartbeatAck()
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_HEARTBEAT_ACK);
	IS_HEARTBEAT_ACK ack;
	ack.i32Result = 0;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

// ============================================================================
// Response Senders
// ============================================================================

void BattleSession::SendBattleCreateAck(int i32RoomIdx, int i32Result,
										int i32BattleRoomIdx, const char* pszUdpIP, uint16_t ui16UdpPort)
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_CREATE_ACK);
	IS_BATTLE_CREATE_ACK ack;
	ack.i32RoomIdx = i32RoomIdx;
	ack.i32Result = i32Result;
	ack.i32BattleRoomIdx = i32BattleRoomIdx;
	memset(ack.szUdpIP, 0, sizeof(ack.szUdpIP));
	if (pszUdpIP)
		strncpy_s(ack.szUdpIP, pszUdpIP, _TRUNCATE);
	ack.ui16UdpPort = ui16UdpPort;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void BattleSession::SendBattleEndNotify(BattleResult* pResult)
{
	if (!pResult)
		return;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_END_NOTIFY);
	packet.WriteData(pResult, sizeof(BattleResult));
	SendMessage(&packet);
}

void BattleSession::SendPlayerMigrateAck(int64_t i64UID, int i32Result)
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_PLAYER_MIGRATE_ACK);

	// Simple inline struct for the ack
	struct
	{
		int64_t i64UID;
		int		i32Result;
	} ack;

	ack.i64UID = i64UID;
	ack.i32Result = i32Result;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}
