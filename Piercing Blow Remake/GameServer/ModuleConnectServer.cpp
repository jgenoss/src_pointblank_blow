#include "pch.h"
#include "ModuleConnectServer.h"
#include "GameSessionManager.h"
#include "GameContextMain.h"

ModuleConnectServer::ModuleConnectServer()
	: m_i32ServerId(0)
	, m_ui16PublicPort(0)
	, m_i32MaxPlayers(0)
	, m_bRegistered(false)
	, m_i32TokenWriteIdx(0)
{
	m_szServerName[0] = '\0';
	m_szPublicIP[0] = '\0';
	memset(m_AuthTokens, 0, sizeof(m_AuthTokens));
}

ModuleConnectServer::~ModuleConnectServer()
{
}

void ModuleConnectServer::SetServerInfo(int i32ServerId, const char* pszName,
										const char* pszPublicIP, uint16_t ui16PublicPort,
										int i32MaxPlayers)
{
	m_i32ServerId = i32ServerId;
	strncpy_s(m_szServerName, pszName, _TRUNCATE);
	strncpy_s(m_szPublicIP, pszPublicIP, _TRUNCATE);
	m_ui16PublicPort = ui16PublicPort;
	m_i32MaxPlayers = i32MaxPlayers;
}

bool ModuleConnectServer::ValidateToken(uint32_t ui32Token, int64_t* pUID, char* pUsername)
{
	DWORD dwNow = GetTickCount();

	for (int i = 0; i < MAX_PENDING_AUTH_TOKENS; i++)
	{
		if (m_AuthTokens[i].bActive && m_AuthTokens[i].ui32Token == ui32Token)
		{
			if (dwNow < m_AuthTokens[i].dwExpireTime)
			{
				if (pUID)
					*pUID = m_AuthTokens[i].i64UID;
				if (pUsername)
					strncpy_s(pUsername, 64, m_AuthTokens[i].szUsername, _TRUNCATE);

				// Consume the token
				m_AuthTokens[i].bActive = false;
				return true;
			}
			else
			{
				// Expired
				m_AuthTokens[i].bActive = false;
				return false;
			}
		}
	}

	return false;
}

void ModuleConnectServer::SendStatusUpdate(int i32CurrentPlayers)
{
	if (!IsConnected() || !m_bRegistered)
		return;

	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_SERVER_STATUS_UPDATE_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_SERVER_STATUS_UPDATE_REQ req;
	req.i32ServerId = m_i32ServerId;
	req.i32State = 1;	// Online
	req.i32CurrentPlayers = i32CurrentPlayers;

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

// ============================================================================
// ModuleBase overrides
// ============================================================================

bool ModuleConnectServer::OnConnect()
{
	printf("[ModuleConnectServer] Connected to ConnectServer %s:%d\n",
		GetRemoteIP(), GetRemotePort());

	// Send registration request
	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_SERVER_REGISTER_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_SERVER_REGISTER_REQ req;
	req.i32ServerId = m_i32ServerId;
	req.i32ServerType = (int)ServerType::Game;
	strncpy_s(req.szServerName, m_szServerName, _TRUNCATE);
	strncpy_s(req.szPublicIP, m_szPublicIP, _TRUNCATE);
	req.ui16PublicPort = m_ui16PublicPort;
	req.i32MaxPlayers = m_i32MaxPlayers;
	req.szRegion[0] = '\0';

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);

	return true;
}

void ModuleConnectServer::OnDisconnect()
{
	printf("[ModuleConnectServer] Disconnected from ConnectServer\n");
	m_bRegistered = false;
}

void ModuleConnectServer::OnProcessPacket(char* pData, int i32Size)
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
		// Heartbeat response received
		break;

	case PROTOCOL_IS_SERVER_REGISTER_ACK:
		OnServerRegisterAck(pPayload, payloadSize);
		break;

	case PROTOCOL_IS_PLAYER_AUTH_TRANSFER_REQ:
		OnPlayerAuthTransferReq(pPayload, payloadSize);
		break;

	default:
		printf("[ModuleConnectServer] Unknown protocol 0x%04X\n", protocolId);
		break;
	}
}

void ModuleConnectServer::OnHeartbeat()
{
	if (!IsConnected())
		return;

	// Send heartbeat
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_HEARTBEAT_REQ;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_HEARTBEAT_REQ req;
	req.i32ServerId = m_i32ServerId;
	req.i32ServerType = (int)ServerType::Game;
	req.i32CurrentPlayers = 0;	// TODO: Get from session manager

	memcpy(buffer + offset, &req, sizeof(req));			offset += sizeof(req);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);

	// Clean expired tokens
	CleanExpiredTokens();
}

// ============================================================================
// Packet handlers
// ============================================================================

void ModuleConnectServer::OnServerRegisterAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_SERVER_REGISTER_ACK))
		return;

	IS_SERVER_REGISTER_ACK* pAck = (IS_SERVER_REGISTER_ACK*)pData;

	if (pAck->i32Result == 0)
	{
		m_bRegistered = true;
		printf("[ModuleConnectServer] Registered with ConnectServer - AssignedId=%d\n",
			pAck->i32AssignedId);
	}
	else
	{
		printf("[ModuleConnectServer] Registration rejected by ConnectServer\n");
	}
}

void ModuleConnectServer::OnPlayerAuthTransferReq(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_AUTH_TRANSFER_REQ))
		return;

	IS_PLAYER_AUTH_TRANSFER_REQ* pReq = (IS_PLAYER_AUTH_TRANSFER_REQ*)pData;

	// Store the auth token
	int idx = m_i32TokenWriteIdx % MAX_PENDING_AUTH_TOKENS;

	m_AuthTokens[idx].bActive = true;
	m_AuthTokens[idx].ui32Token = pReq->ui32AuthToken;
	m_AuthTokens[idx].i64UID = pReq->i64UID;
	strncpy_s(m_AuthTokens[idx].szUsername, pReq->szUsername, _TRUNCATE);
	m_AuthTokens[idx].ui32ClientIP = pReq->ui32ClientIP;
	m_AuthTokens[idx].dwExpireTime = GetTickCount() + AUTH_TOKEN_EXPIRE_TIME;

	m_i32TokenWriteIdx++;

	printf("[ModuleConnectServer] Auth token received - UID=%lld, Token=0x%08X\n",
		pReq->i64UID, pReq->ui32AuthToken);

	// Send ACK back to ConnectServer
	i3NetworkPacket packet;
	char buffer[64];
	int offset = 0;

	uint16_t size = 0;
	uint16_t proto = PROTOCOL_IS_PLAYER_AUTH_TRANSFER_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	IS_PLAYER_AUTH_TRANSFER_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = 0;	// Accepted

	memcpy(buffer + offset, &ack, sizeof(ack));			offset += sizeof(ack);

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

void ModuleConnectServer::CleanExpiredTokens()
{
	DWORD dwNow = GetTickCount();

	for (int i = 0; i < MAX_PENDING_AUTH_TOKENS; i++)
	{
		if (m_AuthTokens[i].bActive && dwNow >= m_AuthTokens[i].dwExpireTime)
			m_AuthTokens[i].bActive = false;
	}
}
