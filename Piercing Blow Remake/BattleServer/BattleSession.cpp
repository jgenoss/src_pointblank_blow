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
	SendPacketMessage(&packet);
}

void BattleSession::OnBattleCreateReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_BATTLE_CREATE_REQ))
		return;

	IS_BATTLE_CREATE_REQ* pReq = (IS_BATTLE_CREATE_REQ*)pData;

	printf("[BattleSession:%d] BATTLE_CREATE_REQ: Room=%d, Channel=%d, Mode=%d, Map=%d, Players=%d\n",
		m_SessionIdx, pReq->i32RoomIdx, pReq->i32ChannelNum,
		pReq->ui8GameMode, pReq->ui8MapIndex, pReq->i32PlayerCount);

	// Validate parameters before using them
	if (pReq->ui8GameMode == 0 || pReq->ui8GameMode >= 18)		// 0=STAGE_MODE_NA, 18=STAGE_MODE_MAX
	{
		printf("[BattleSession:%d] BATTLE_CREATE_REQ rejected: invalid GameMode=%d\n",
			m_SessionIdx, pReq->ui8GameMode);
		SendBattleCreateAck(pReq->i32RoomIdx, 2, -1, "", 0);
		return;
	}
	if (pReq->ui8MapIndex >= 64)	// DS_MAX_MAP = 64
	{
		printf("[BattleSession:%d] BATTLE_CREATE_REQ rejected: invalid MapIndex=%d\n",
			m_SessionIdx, pReq->ui8MapIndex);
		SendBattleCreateAck(pReq->i32RoomIdx, 3, -1, "", 0);
		return;
	}
	if (pReq->i32PlayerCount <= 0 || pReq->i32PlayerCount > 16)
	{
		printf("[BattleSession:%d] BATTLE_CREATE_REQ rejected: invalid PlayerCount=%d\n",
			m_SessionIdx, pReq->i32PlayerCount);
		SendBattleCreateAck(pReq->i32RoomIdx, 4, -1, "", 0);
		return;
	}

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

	// Get the public IP from global config (set by BattleServer::OnInitialize)
	extern char g_szBattlePublicIP[MAX_SERVER_IP_LENGTH];
	const char* pszUdpIP = g_szBattlePublicIP[0] != '\0' ? g_szBattlePublicIP : "127.0.0.1";

	SendBattleCreateAck(pReq->i32RoomIdx, 0, battleRoomIdx, pszUdpIP, pRoom->GetUdpPort());
}

void BattleSession::OnPlayerMigrateReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_PLAYER_MIGRATE_REQ))
		return;

	IS_PLAYER_MIGRATE_REQ* pReq = (IS_PLAYER_MIGRATE_REQ*)pData;

	printf("[BattleSession:%d] PLAYER_MIGRATE_REQ: UID=%lld, Room=%d, Slot=%d, Team=%d\n",
		m_SessionIdx, pReq->i64UID, pReq->i32BattleRoomIdx, pReq->i32SlotIdx, pReq->i32Team);

	// Find the battle room
	BattleRoom* pRoom = g_pBattleRoomManager ? g_pBattleRoomManager->GetRoom(pReq->i32BattleRoomIdx) : nullptr;
	if (!pRoom || !pRoom->IsActive())
	{
		printf("[BattleSession:%d] PLAYER_MIGRATE failed: room %d not found/active\n",
			m_SessionIdx, pReq->i32BattleRoomIdx);
		SendPlayerMigrateAck(pReq->i64UID, 1);	// Error: room not found
		return;
	}

	// Add member to room - IP/port will be resolved when first UDP packet arrives
	// (UdpRelay auto-registers members by matching UID from header)
	int slotIdx = pReq->i32SlotIdx;
	if (slotIdx < 0 || slotIdx >= BATTLE_SLOT_MAX)
	{
		// Auto-assign slot
		slotIdx = pRoom->FindFreeSlot();
		if (slotIdx < 0)
		{
			printf("[BattleSession:%d] PLAYER_MIGRATE failed: room %d full\n",
				m_SessionIdx, pReq->i32BattleRoomIdx);
			SendPlayerMigrateAck(pReq->i64UID, 2);	// Error: room full
			return;
		}
	}

	// Add with IP/Port from request (may be 0 - will be filled by UdpRelay on first packet)
	pRoom->AddMember(pReq->i64UID, pReq->ui32ClientIP, pReq->ui16ClientPort, pReq->i32Team, slotIdx);

	printf("[BattleSession:%d] PLAYER_MIGRATE success: UID=%lld -> Room=%d Slot=%d\n",
		m_SessionIdx, pReq->i64UID, pReq->i32BattleRoomIdx, slotIdx);
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
	SendPacketMessage(&packet);
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
	SendPacketMessage(&packet);
}

void BattleSession::SendBattleEndNotify(BattleResult* pResult)
{
	if (!pResult)
		return;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_END_NOTIFY);
	packet.WriteData(pResult, sizeof(BattleResult));
	SendPacketMessage(&packet);
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
	SendPacketMessage(&packet);
}

// ============================================================================
// Battle Live Event Senders (direct to GameServer, replaces CastServer)
// ============================================================================

void BattleSession::SendKillNotify(int i32RoomIdx, int i32ChannelNum,
									uint8_t ui8RoundNum,
									const IS_BATTLE_KILL_INFO* pKills, uint8_t ui8KillCount)
{
	if (!pKills || ui8KillCount == 0)
		return;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_KILL_NOTIFY);

	IS_BATTLE_KILL_NOTIFY notify;
	notify.i32RoomIdx = i32RoomIdx;
	notify.i32ChannelNum = i32ChannelNum;
	notify.ui8RoundNum = ui8RoundNum;
	notify.ui8KillCount = ui8KillCount;
	notify.ui8Pad[0] = 0;
	notify.ui8Pad[1] = 0;

	packet.WriteData(&notify, sizeof(notify));
	packet.WriteData(pKills, sizeof(IS_BATTLE_KILL_INFO) * ui8KillCount);
	SendPacketMessage(&packet);
}

void BattleSession::SendRoundStartNotify(int i32RoomIdx, int i32ChannelNum,
										   uint8_t ui8RoundNum, uint8_t ui8GameMode)
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_ROUND_START_NOTIFY);

	IS_BATTLE_ROUND_START_NOTIFY notify;
	notify.i32RoomIdx = i32RoomIdx;
	notify.i32ChannelNum = i32ChannelNum;
	notify.ui8RoundNum = ui8RoundNum;
	notify.ui8GameMode = ui8GameMode;
	notify.ui8Pad[0] = 0;
	notify.ui8Pad[1] = 0;

	packet.WriteData(&notify, sizeof(notify));
	SendPacketMessage(&packet);
}

void BattleSession::SendRoundEndNotify(int i32RoomIdx, int i32ChannelNum,
										uint8_t ui8RoundNum, uint8_t ui8EndType,
										uint8_t ui8WinTeam,
										int i32RedScore, int i32BlueScore)
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_ROUND_END_NOTIFY);

	IS_BATTLE_ROUND_END_NOTIFY notify;
	notify.i32RoomIdx = i32RoomIdx;
	notify.i32ChannelNum = i32ChannelNum;
	notify.ui8RoundNum = ui8RoundNum;
	notify.ui8RoundEndType = ui8EndType;
	notify.ui8WinTeam = ui8WinTeam;
	notify.ui8Pad = 0;
	notify.i32RedScore = i32RedScore;
	notify.i32BlueScore = i32BlueScore;

	packet.WriteData(&notify, sizeof(notify));
	SendPacketMessage(&packet);
}

void BattleSession::SendHackNotify(int i32RoomIdx, int i32ChannelNum,
									uint32_t ui32SlotIdx, int64_t i64UID,
									uint8_t ui8HackType, uint8_t ui8Severity,
									const char* pszDescription)
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_HACK_NOTIFY);

	IS_BATTLE_HACK_NOTIFY notify;
	notify.i32RoomIdx = i32RoomIdx;
	notify.i32ChannelNum = i32ChannelNum;
	notify.ui32SlotIdx = ui32SlotIdx;
	notify.i64UID = i64UID;
	notify.ui8HackType = ui8HackType;
	notify.ui8Severity = ui8Severity;
	notify.ui8Pad[0] = 0;
	notify.ui8Pad[1] = 0;
	memset(notify.szDescription, 0, sizeof(notify.szDescription));
	if (pszDescription)
		strncpy_s(notify.szDescription, pszDescription, _TRUNCATE);

	packet.WriteData(&notify, sizeof(notify));
	SendPacketMessage(&packet);
}

void BattleSession::SendMissionNotify(int i32RoomIdx, int i32ChannelNum,
										uint8_t ui8EventType, uint32_t ui32SlotIdx,
										int64_t i64UID, int32_t i32Param1, int32_t i32Param2)
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BATTLE_MISSION_NOTIFY);

	IS_BATTLE_MISSION_NOTIFY notify;
	notify.i32RoomIdx = i32RoomIdx;
	notify.i32ChannelNum = i32ChannelNum;
	notify.ui8EventType = ui8EventType;
	notify.ui8Pad[0] = 0;
	notify.ui8Pad[1] = 0;
	notify.ui8Pad[2] = 0;
	notify.ui32SlotIdx = ui32SlotIdx;
	notify.i64UID = i64UID;
	notify.i32Param1 = i32Param1;
	notify.i32Param2 = i32Param2;

	packet.WriteData(&notify, sizeof(notify));
	SendPacketMessage(&packet);
}
