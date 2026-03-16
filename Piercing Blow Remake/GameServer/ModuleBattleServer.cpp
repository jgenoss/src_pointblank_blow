#include "pch.h"
#include "ModuleBattleServer.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameServerContext.h"
#include "GameContextMain.h"
#include "Room.h"
#include "RoomManager.h"

ModuleBattleServer::ModuleBattleServer()
	: m_i32ServerId(0)
	, m_ui16PublicPort(0)
	, m_i32MaxPlayers(0)
	, m_bRegistered(false)
{
	memset(m_szServerName, 0, sizeof(m_szServerName));
	memset(m_szPublicIP, 0, sizeof(m_szPublicIP));
}

ModuleBattleServer::~ModuleBattleServer()
{
}

void ModuleBattleServer::SetServerInfo(int i32ServerId, const char* pszName,
									   const char* pszPublicIP, uint16_t ui16PublicPort,
									   int i32MaxPlayers)
{
	m_i32ServerId = i32ServerId;
	m_ui16PublicPort = ui16PublicPort;
	m_i32MaxPlayers = i32MaxPlayers;

	if (pszName)
		strncpy_s(m_szServerName, pszName, _TRUNCATE);
	if (pszPublicIP)
		strncpy_s(m_szPublicIP, pszPublicIP, _TRUNCATE);
}

// ============================================================================
// Request operations
// ============================================================================

void ModuleBattleServer::RequestBattleCreate(int i32RoomIdx, int i32ChannelNum,
											 uint8_t ui8GameMode, uint8_t ui8MapIndex,
											 uint8_t ui8MaxPlayers, int i32PlayerCount)
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_BATTLE_CREATE_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_BATTLE_CREATE_REQ req;
	req.i32RoomIdx = i32RoomIdx;
	req.i32ChannelNum = i32ChannelNum;
	req.ui8GameMode = ui8GameMode;
	req.ui8MapIndex = ui8MapIndex;
	req.ui8MaxPlayers = ui8MaxPlayers;
	req.i32PlayerCount = i32PlayerCount;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);

	printf("[ModuleBattleServer] Sent BATTLE_CREATE_REQ - Room=%d, Channel=%d, Mode=%d\n",
		i32RoomIdx, i32ChannelNum, ui8GameMode);
}

void ModuleBattleServer::RequestPlayerMigrate(int64_t i64UID, int i32BattleRoomIdx,
											  int i32SlotIdx, int i32Team,
											  uint32_t ui32ClientIP, uint16_t ui16ClientPort)
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_PLAYER_MIGRATE_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_PLAYER_MIGRATE_REQ req;
	req.i64UID = i64UID;
	req.i32BattleRoomIdx = i32BattleRoomIdx;
	req.i32SlotIdx = i32SlotIdx;
	req.i32Team = i32Team;
	req.ui32ClientIP = ui32ClientIP;
	req.ui16ClientPort = ui16ClientPort;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

// ============================================================================
// ModuleBase overrides
// ============================================================================

bool ModuleBattleServer::OnConnect()
{
	printf("[ModuleBattleServer] Connected to BattleServer %s:%d\n",
		GetRemoteIP(), GetRemotePort());

	// Send registration request (reuse IS_SERVER_REGISTER_REQ)
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_BATTLE_REGISTER_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_SERVER_REGISTER_REQ req;
	req.i32ServerId = m_i32ServerId;
	req.i32ServerType = (int)ServerType::Game;
	strncpy_s(req.szServerName, m_szServerName, _TRUNCATE);
	strncpy_s(req.szPublicIP, m_szPublicIP, _TRUNCATE);
	req.ui16PublicPort = m_ui16PublicPort;
	req.i32MaxPlayers = m_i32MaxPlayers;
	memset(req.szRegion, 0, sizeof(req.szRegion));

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);

	m_bRegistered = false;

	return true;
}

void ModuleBattleServer::OnDisconnect()
{
	printf("[ModuleBattleServer] Disconnected from BattleServer\n");
	m_bRegistered = false;
}

void ModuleBattleServer::OnProcessPacket(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(uint16_t) * 2)
		return;

	uint16_t packetSize = *(uint16_t*)pData;
	uint16_t protocolId = *(uint16_t*)(pData + 2);
	char* pPayload = pData + 4;
	int payloadSize = packetSize - 4;

	switch (protocolId)
	{
	case PROTOCOL_IS_HEARTBEAT_ACK:
		break;

	case PROTOCOL_IS_BATTLE_REGISTER_ACK:
		OnBattleRegisterAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_BATTLE_CREATE_ACK:
		OnBattleCreateAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_BATTLE_END_NOTIFY:
		OnBattleEndNotify(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_PLAYER_MIGRATE_ACK:
		OnPlayerMigrateAck(pPayload, payloadSize);
		break;

	default:
		printf("[ModuleBattleServer] Unknown protocol 0x%04X\n", protocolId);
		break;
	}
}

void ModuleBattleServer::OnHeartbeat()
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_HEARTBEAT_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_HEARTBEAT_REQ req;
	req.i32ServerId = g_pContextMain ? g_pContextMain->m_i32ServerId : 0;
	req.i32ServerType = (int)ServerType::Game;
	req.i32CurrentPlayers = 0;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

// ============================================================================
// Response handlers
// ============================================================================

void ModuleBattleServer::OnBattleRegisterAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_SERVER_REGISTER_ACK))
		return;

	IS_SERVER_REGISTER_ACK* pAck = (IS_SERVER_REGISTER_ACK*)pData;

	if (pAck->i32Result == 0)
	{
		m_bRegistered = true;
		printf("[ModuleBattleServer] Registered with BattleServer successfully (AssignedId=%d)\n",
			pAck->i32AssignedId);
	}
	else
	{
		printf("[ModuleBattleServer] Registration rejected by BattleServer (Result=%d)\n",
			pAck->i32Result);
	}
}

void ModuleBattleServer::OnBattleCreateAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_BATTLE_CREATE_ACK))
		return;

	IS_BATTLE_CREATE_ACK* pAck = (IS_BATTLE_CREATE_ACK*)pData;

	printf("[ModuleBattleServer] BATTLE_CREATE_ACK - Room=%d, Result=%d, BattleRoom=%d, UDP=%s:%d\n",
		pAck->i32RoomIdx, pAck->i32Result, pAck->i32BattleRoomIdx,
		pAck->szUdpIP, pAck->ui16UdpPort);

	if (pAck->i32Result != 0)
	{
		printf("[ModuleBattleServer] Battle creation failed for room %d\n", pAck->i32RoomIdx);
		return;
	}

	// TODO: Notify the room that battle was created successfully
	// Store battle room info (UDP IP/port) in the Room so clients can connect
	// For now just log it
}

void ModuleBattleServer::OnBattleEndNotify(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_BATTLE_END_NOTIFY))
		return;

	IS_BATTLE_END_NOTIFY* pNotify = (IS_BATTLE_END_NOTIFY*)pData;

	printf("[ModuleBattleServer] BATTLE_END_NOTIFY - Room=%d, Channel=%d, Winner=%s, Score: R%d-B%d, Players=%d\n",
		pNotify->i32RoomIdx, pNotify->i32ChannelNum,
		pNotify->i32WinnerTeam == 0 ? "RED" :
		pNotify->i32WinnerTeam == 1 ? "BLUE" : "DRAW",
		pNotify->i32RedScore, pNotify->i32BlueScore,
		pNotify->i32PlayerCount);

	// Parse player results
	char* pPlayerData = pData + sizeof(IS_BATTLE_END_NOTIFY);
	int remainingSize = i32Size - sizeof(IS_BATTLE_END_NOTIFY);

	for (int i = 0; i < pNotify->i32PlayerCount && remainingSize >= (int)sizeof(IS_BATTLE_PLAYER_RESULT); i++)
	{
		IS_BATTLE_PLAYER_RESULT* pResult = (IS_BATTLE_PLAYER_RESULT*)pPlayerData;

		printf("  Player UID=%lld: K=%d D=%d H=%d Team=%d\n",
			pResult->i64UID, pResult->i32Kills, pResult->i32Deaths,
			pResult->i32Headshots, pResult->i32Team);

		pPlayerData += sizeof(IS_BATTLE_PLAYER_RESULT);
		remainingSize -= sizeof(IS_BATTLE_PLAYER_RESULT);
	}

	// TODO: Update room state back to READY, apply results to player stats,
	// save stats to DataServer via ModuleDataServer
}

void ModuleBattleServer::OnPlayerMigrateAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_MIGRATE_ACK))
		return;

	IS_PLAYER_MIGRATE_ACK* pAck = (IS_PLAYER_MIGRATE_ACK*)pData;

	if (pAck->i32Result != 0)
	{
		printf("[ModuleBattleServer] Player migrate failed - UID=%lld, Result=%d\n",
			pAck->i64UID, pAck->i32Result);
	}
}
