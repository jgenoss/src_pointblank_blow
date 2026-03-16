#include "pch.h"
#include "ModuleDataServer.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameContextMain.h"
#include "GameServerContext.h"

ModuleDataServer::ModuleDataServer()
{
}

ModuleDataServer::~ModuleDataServer()
{
}

// ============================================================================
// Request operations
// ============================================================================

void ModuleDataServer::RequestPlayerLoad(int i32SessionIdx, int64_t i64UID)
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_PLAYER_LOAD_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_PLAYER_LOAD_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

void ModuleDataServer::RequestPlayerSave(int64_t i64UID, int i32Level, int64_t i64Exp,
										  int i32Cash, int i32GP)
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_PLAYER_SAVE_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_PLAYER_SAVE_REQ req;
	req.i64UID = i64UID;
	req.i32Level = i32Level;
	req.i64Exp = i64Exp;
	req.i32Cash = i32Cash;
	req.i32GP = i32GP;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

void ModuleDataServer::RequestStatsSave(int64_t i64UID, int i32Kills, int i32Deaths,
										 int i32Headshots, int i32Wins, int i32Losses)
{
	if (!IsConnected())
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_STATS_SAVE_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_STATS_SAVE_REQ req;
	req.i64UID = i64UID;
	req.i32Kills = i32Kills;
	req.i32Deaths = i32Deaths;
	req.i32Headshots = i32Headshots;
	req.i32Wins = i32Wins;
	req.i32Losses = i32Losses;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

void ModuleDataServer::RequestCreateNick(int64_t i64UID, int i32SessionIdx, const char* pszNickname)
{
	if (!IsConnected() || !pszNickname)
		return;

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_PLAYER_CREATE_NICK_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_PLAYER_CREATE_NICK_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	strncpy_s(req.szNickname, pszNickname, _TRUNCATE);

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

void ModuleDataServer::RequestCheckNick(int i32SessionIdx, const char* pszNickname)
{
	if (!IsConnected() || !pszNickname)
		return;

	i3NetworkPacket packet;
	char buffer[128];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_PLAYER_CHECK_NICK_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_PLAYER_CHECK_NICK_REQ req;
	req.i32SessionIdx = i32SessionIdx;
	strncpy_s(req.szNickname, pszNickname, _TRUNCATE);

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

// ============================================================================
// ModuleBase overrides
// ============================================================================

bool ModuleDataServer::OnConnect()
{
	printf("[ModuleDataServer] Connected to DataServer %s:%d\n",
		GetRemoteIP(), GetRemotePort());
	return true;
}

void ModuleDataServer::OnDisconnect()
{
	printf("[ModuleDataServer] Disconnected from DataServer\n");
}

void ModuleDataServer::OnProcessPacket(char* pData, int i32Size)
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

	case PROTOCOL_IS_PLAYER_LOAD_ACK:
		OnPlayerLoadAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_PLAYER_SAVE_ACK:
		OnPlayerSaveAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_PLAYER_CREATE_NICK_ACK:
		OnCreateNickAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_PLAYER_CHECK_NICK_ACK:
		OnCheckNickAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_STATS_SAVE_ACK:
		OnStatsSaveAck(pPayload, payloadSize);
		break;

	default:
		printf("[ModuleDataServer] Unknown protocol 0x%04X\n", protocolId);
		break;
	}
}

void ModuleDataServer::OnHeartbeat()
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

void ModuleDataServer::OnPlayerLoadAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_LOAD_ACK))
		return;

	IS_PLAYER_LOAD_ACK* pAck = (IS_PLAYER_LOAD_ACK*)pData;

	if (pAck->i32Result != 0)
	{
		printf("[ModuleDataServer] Player load failed - UID=%lld, Result=%d\n",
			pAck->i64UID, pAck->i32Result);
		return;
	}

	// Find the session and load player data
	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx, pAck->i64UID);
	if (!pSession)
	{
		printf("[ModuleDataServer] Session not found for loaded data - Idx=%d, UID=%lld\n",
			pAck->i32SessionIdx, pAck->i64UID);
		return;
	}

	// The payload after IS_PLAYER_LOAD_ACK contains IS_PLAYER_LOAD_DATA + inventory
	char* pPayload = pData + sizeof(IS_PLAYER_LOAD_ACK);
	int payloadSize = i32Size - sizeof(IS_PLAYER_LOAD_ACK);

	pSession->OnPlayerDataLoaded(pPayload, payloadSize);
}

void ModuleDataServer::OnPlayerSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_SAVE_ACK))
		return;

	IS_PLAYER_SAVE_ACK* pAck = (IS_PLAYER_SAVE_ACK*)pData;

	if (pAck->i32Result != 0)
	{
		printf("[ModuleDataServer] Player save failed - UID=%lld, Result=%d\n",
			pAck->i64UID, pAck->i32Result);
	}
}

void ModuleDataServer::OnCreateNickAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_CREATE_NICK_ACK))
		return;

	IS_PLAYER_CREATE_NICK_ACK* pAck = (IS_PLAYER_CREATE_NICK_ACK*)pData;

	printf("[ModuleDataServer] Create nick result - UID=%lld, Result=%d\n",
		pAck->i64UID, pAck->i32Result);

	// TODO: Notify the session of nick creation result
}

void ModuleDataServer::OnCheckNickAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_CHECK_NICK_ACK))
		return;

	IS_PLAYER_CHECK_NICK_ACK* pAck = (IS_PLAYER_CHECK_NICK_ACK*)pData;

	printf("[ModuleDataServer] Check nick result - SessionIdx=%d, Result=%d\n",
		pAck->i32SessionIdx, pAck->i32Result);

	// TODO: Notify the session of nick check result
}

void ModuleDataServer::OnStatsSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_STATS_SAVE_ACK))
		return;

	IS_STATS_SAVE_ACK* pAck = (IS_STATS_SAVE_ACK*)pData;

	if (pAck->i32Result != 0)
	{
		printf("[ModuleDataServer] Stats save failed - UID=%lld, Result=%d\n",
			pAck->i64UID, pAck->i32Result);
	}
}
