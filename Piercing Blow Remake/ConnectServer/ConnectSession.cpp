#include "pch.h"
#include "ConnectSession.h"
#include "ConnectServerContext.h"
#include "ConnectSessionManager.h"
#include "GameServerRegistry.h"
#include "ModuleDataClient.h"
#include "ServerList.h"
#include "ProtocolDef.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

I3_CLASS_INSTANCE(ConnectSession, i3NetworkSession);

ConnectSession::ConnectSession()
	: m_eTask(CONNECT_TASK_NONE)
	, m_i64UID(0)
	, m_ui32AuthToken(0)
	, m_ui32XorKey(0)
	, m_dwConnectTime(0)
{
	m_szUsername[0] = '\0';
}

ConnectSession::~ConnectSession()
{
}

BOOL ConnectSession::Create()
{
	return i3NetworkSession::Create();
}

void ConnectSession::Destroy()
{
	m_eTask = CONNECT_TASK_NONE;
	m_i64UID = 0;
	m_ui32AuthToken = 0;
	m_ui32XorKey = 0;
	m_szUsername[0] = '\0';
	m_dwConnectTime = 0;

	i3NetworkSession::Destroy();
}

BOOL ConnectSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	if (!i3NetworkSession::OnConnect(Socket, pAddr))
		return FALSE;

	m_eTask = CONNECT_TASK_CONNECTED;
	m_dwConnectTime = GetTickCount();

	printf("[ConnectSession:%d] Client connected from %s\n", m_SessionIdx, m_strIPString);
	return TRUE;
}

BOOL ConnectSession::OnConnectInit()
{
	if (FALSE == i3NetworkSession::OnConnectInit())
		return FALSE;

	// PACKET_BASE_CONNECT_ACK fields (same layout as S2MO serialization):
	// m_i32SessionIdx(4) + m_i16PacketRandSeed(2) + SC_VERSION(3) + RSA_KEY(1024) + ChannelTypes(10)
	INT32 i32SessionIdx = m_SessionIdx;
	INT16 i16PacketRandSeed = (INT16)(GetTickCount() & 0x7FFF);

	// SC_VERSION: ui8VerGame(1) + ui16VerSC(2)
	UINT8 ui8VerGame = 3;		// VER_GAME
	UINT16 ui16VerSC = 9;		// VER_SC_PROTOCOL

	i3NetworkPacket packet(PROTOCOL_BASE_CONNECT_ACK);
	packet.WriteData(&i32SessionIdx, sizeof(INT32));
	packet.WriteData(&i16PacketRandSeed, sizeof(INT16));
	packet.WriteData(&ui8VerGame, sizeof(UINT8));
	packet.WriteData(&ui16VerSC, sizeof(UINT16));

	// RSA key (1024 bytes, zeroed = no encryption for now)
	char rsaKey[1024] = {0};
	packet.WriteData(rsaKey, 1024);

	// Channel types (10 channels, all type 1 = normal)
	UINT8 channelTypes[10];
	memset(channelTypes, 0x01, 10);
	packet.WriteData(channelTypes, 10);

	SendPacketMessage(&packet);

	printf("[ConnectSession:%d] CONNECT_ACK sent (sid=%d)\n", m_SessionIdx, i32SessionIdx);
	return TRUE;
}

BOOL ConnectSession::OnDisconnect(BOOL bForceMainThread)
{
	printf("[ConnectSession:%d] Client disconnected (task=%d)\n", m_SessionIdx, (int)m_eTask);

	m_eTask = CONNECT_TASK_NONE;
	m_i64UID = 0;
	m_ui32AuthToken = 0;
	m_ui32XorKey = 0;
	m_szUsername[0] = '\0';

	return i3NetworkSession::OnDisconnect(bForceMainThread);
}

INT32 ConnectSession::PacketParsing(char* pPacket, INT32 iSize)
{
	if (iSize < SOCKET_HEAD_SIZE)
		return iSize;

	// Leer protocol ID del header
	UINT16* pDataSize = (UINT16*)pPacket;
	UINT16* pProtocolID = (UINT16*)(pPacket + SOCKET_HEAD_DATASIZE_SIZE);

	UINT16 ui16DataSize = (*pDataSize) & 0x7FFF;
	INT32 i32PacketSize = ui16DataSize + SOCKET_HEAD_SIZE;

	if (iSize < i32PacketSize)
		return iSize; // Paquete incompleto

	PROTOCOL protocolID = *pProtocolID;

	// Despachar segun protocolo
	switch (protocolID)
	{
	case PROTOCOL_BASE_CONNECT_REQ:
		OnPacketConnectReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	case PROTOCOL_LOGIN_REQ:
	case PROTOCOL_LOGIN_WEBKEY_REQ:
		OnPacketLoginReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	case PROTOCOL_BASE_HEART_BIT_REQ:
		OnPacketHeartBitReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	case PROTOCOL_BASE_GET_CHANNELLIST_REQ:
		OnPacketServerListReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	case PROTOCOL_BASE_USER_LEAVE_REQ:
		OnPacketServerSelectReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	// Inter-server protocols (from GameServer connections)
	case PROTOCOL_IS_SERVER_REGISTER_REQ:
		OnISServerRegisterReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	case PROTOCOL_IS_HEARTBEAT_REQ:
		OnISHeartbeatReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	case PROTOCOL_IS_SERVER_STATUS_UPDATE_REQ:
		OnISServerStatusUpdateReq(pPacket + SOCKET_HEAD_SIZE, ui16DataSize);
		break;

	default:
		printf("[ConnectSession:%d] Unknown protocol: 0x%04X\n", m_SessionIdx, protocolID);
		break;
	}

	return i32PacketSize;
}

bool ConnectSession::IsTimedOut() const
{
	if (m_eTask == CONNECT_TASK_NONE)
		return false;

	DWORD dwElapsed = GetTickCount() - m_dwConnectTime;
	return (dwElapsed > (CONNECT_SESSION_TIMEOUT * 1000));
}

// -- Packet Handlers --

void ConnectSession::OnPacketConnectReq(char* pData, INT32 i32Size)
{
	printf("[ConnectSession:%d] CONNECT_REQ received\n", m_SessionIdx);

	// Generar XOR key para encriptacion
	m_ui32XorKey = (uint32_t)(GetTickCount() ^ (m_SessionIdx << 16));

	m_eTask = CONNECT_TASK_HANDSHAKE;
	SendConnectAck();
}

void ConnectSession::OnPacketLoginReq(char* pData, INT32 i32Size)
{
	if (m_eTask < CONNECT_TASK_HANDSHAKE)
	{
		printf("[ConnectSession:%d] LOGIN_REQ before handshake!\n", m_SessionIdx);
		return;
	}

	printf("[ConnectSession:%d] LOGIN_REQ received\n", m_SessionIdx);

	// Parse username and password from client packet
	// Client sends: PBVer(1) + Username(null-terminated, up to 65) + Password(null-terminated, up to 32) + ...
	// We extract just the username and password for auth
	char szUsername[64] = {};
	char szPassword[64] = {};

	if (i32Size >= 2)
	{
		int offset = 1; // Skip PBVer byte

		// Read username (length-prefixed string: uint16 len + data)
		if (offset + 2 <= i32Size)
		{
			uint16_t usernameLen = *(uint16_t*)(pData + offset);
			offset += 2;
			if (usernameLen > 0 && usernameLen < 64 && offset + usernameLen <= i32Size)
			{
				memcpy(szUsername, pData + offset, usernameLen);
				szUsername[usernameLen] = '\0';
				offset += usernameLen;
			}
		}

		// Read password (length-prefixed string: uint16 len + data)
		if (offset + 2 <= i32Size)
		{
			uint16_t passwordLen = *(uint16_t*)(pData + offset);
			offset += 2;
			if (passwordLen > 0 && passwordLen < 64 && offset + passwordLen <= i32Size)
			{
				memcpy(szPassword, pData + offset, passwordLen);
				szPassword[passwordLen] = '\0';
			}
		}
	}

	// Validate we got credentials
	if (szUsername[0] == '\0')
	{
		printf("[ConnectSession:%d] LOGIN_REQ: empty username\n", m_SessionIdx);
		SendLoginAck(1); // Error
		return;
	}

	// Store username for later
	strncpy(m_szUsername, szUsername, sizeof(m_szUsername) - 1);

	// Get client IP
	uint32_t ui32ClientIP = m_ConIp;

	// Send auth request to DataServer
	if (g_pConnectServerContext && g_pConnectServerContext->GetModuleDataClient() &&
		g_pConnectServerContext->GetModuleDataClient()->IsConnected())
	{
		g_pConnectServerContext->GetModuleDataClient()->RequestLogin(
			m_SessionIdx, szUsername, szPassword, ui32ClientIP);
		// Response will come via OnLoginResult callback
	}
	else
	{
		// DataServer not available - fallback to development mode
		printf("[ConnectSession:%d] DataServer not connected, using dev auth\n", m_SessionIdx);
		m_i64UID = 1000 + m_SessionIdx;
		m_eTask = CONNECT_TASK_AUTHENTICATED;
		SendLoginAck(0);
	}
}

void ConnectSession::OnLoginResult(int i32Result, int64_t i64UID, uint8_t ui8AuthLevel)
{
	if (i32Result == 0)
	{
		m_i64UID = i64UID;
		m_eTask = CONNECT_TASK_AUTHENTICATED;
		printf("[ConnectSession:%d] Auth success: UID=%lld\n", m_SessionIdx, (long long)i64UID);
	}
	else
	{
		printf("[ConnectSession:%d] Auth failed: result=%d\n", m_SessionIdx, i32Result);
	}

	SendLoginAck(i32Result);
}

void ConnectSession::OnPacketServerListReq(char* pData, INT32 i32Size)
{
	if (m_eTask < CONNECT_TASK_AUTHENTICATED)
	{
		printf("[ConnectSession:%d] SERVER_LIST_REQ before auth!\n", m_SessionIdx);
		return;
	}

	printf("[ConnectSession:%d] SERVER_LIST_REQ\n", m_SessionIdx);
	SendServerList();
}

void ConnectSession::OnPacketServerSelectReq(char* pData, INT32 i32Size)
{
	if (m_eTask < CONNECT_TASK_AUTHENTICATED)
	{
		printf("[ConnectSession:%d] SERVER_SELECT_REQ before auth!\n", m_SessionIdx);
		return;
	}

	printf("[ConnectSession:%d] SERVER_SELECT_REQ\n", m_SessionIdx);

	if (!g_pConnectServerContext || !g_pConnectServerContext->GetRegistry())
	{
		SendServerSelectAck(1, "", 0); // Error: no servers
		return;
	}

	GameServerRegistry* pRegistry = g_pConnectServerContext->GetRegistry();
	const ServerInfo* pServer = nullptr;

	// Read server ID from client packet (if provided)
	if (i32Size >= (INT32)sizeof(int))
	{
		int requestedServerId = 0;
		memcpy(&requestedServerId, pData, sizeof(int));
		pServer = pRegistry->GetServerById(requestedServerId);
	}

	// Fallback to least-loaded server (load balancing)
	if (!pServer)
		pServer = pRegistry->GetLeastLoadedServer();

	if (!pServer)
	{
		SendServerSelectAck(1, "", 0); // Error: no servers available
		return;
	}

	// Generate auth token and transfer to GameServer
	m_ui32AuthToken = GenerateAuthToken();

	// Send auth token to the selected GameServer
	SendAuthTransferToGameServer(pServer->id, m_i64UID, m_ui32AuthToken,
		m_szUsername, m_ConIp);

	m_eTask = CONNECT_TASK_SERVER_SELECT;
	SendServerSelectAck(0, pServer->ip, pServer->port);
}

void ConnectSession::OnPacketHeartBitReq(char* pData, INT32 i32Size)
{
	// Responder heartbeat
	i3NetworkPacket packet(PROTOCOL_BASE_HEART_BIT_ACK);
	SendPacketMessage(&packet);
}

// -- Send Helpers --

void ConnectSession::SendConnectAck()
{
	i3NetworkPacket packet(PROTOCOL_BASE_CONNECT_ACK);

	// Escribir XOR key en el ACK
	packet.WriteData(&m_ui32XorKey, sizeof(m_ui32XorKey));

	SendPacketMessage(&packet);
}

void ConnectSession::SendLoginAck(int i32Result)
{
	i3NetworkPacket packet(PROTOCOL_LOGIN_ACK);

	packet.WriteData(&i32Result, sizeof(i32Result));
	if (i32Result == 0)
	{
		packet.WriteData(&m_i64UID, sizeof(m_i64UID));
	}

	SendPacketMessage(&packet);
}

void ConnectSession::SendServerList()
{
	i3NetworkPacket packet(PROTOCOL_BASE_GET_CHANNELLIST_ACK);

	if (!g_pConnectServerContext)
		return;

	GameServerRegistry* pRegistry = g_pConnectServerContext->GetRegistry();
	if (!pRegistry)
		return;

	int i32Count = pRegistry->GetOnlineServerCount();
	packet.WriteData(&i32Count, sizeof(i32Count));

	// Escribir info de cada servidor
	for (int i = 0; i < pRegistry->GetServerCount(); ++i)
	{
		const ServerInfo* pInfo = pRegistry->GetServerByIndex(i);
		if (pInfo && pInfo->state == ServerOnlineState::Online)
		{
			packet.WriteData(&pInfo->id, sizeof(pInfo->id));

			uint8_t ui8NameLen = (uint8_t)strlen(pInfo->name);
			packet.WriteData(&ui8NameLen, sizeof(ui8NameLen));
			packet.WriteData(pInfo->name, ui8NameLen);

			packet.WriteData(&pInfo->currentPlayers, sizeof(pInfo->currentPlayers));
			packet.WriteData(&pInfo->maxPlayers, sizeof(pInfo->maxPlayers));
		}
	}

	SendPacketMessage(&packet);
}

void ConnectSession::SendServerSelectAck(int i32Result, const char* pszIP, uint16_t ui16Port)
{
	i3NetworkPacket packet(PROTOCOL_BASE_USER_LEAVE_ACK);

	packet.WriteData(&i32Result, sizeof(i32Result));
	if (i32Result == 0)
	{
		packet.WriteData(&m_ui32AuthToken, sizeof(m_ui32AuthToken));

		uint8_t ui8IPLen = (uint8_t)strlen(pszIP);
		packet.WriteData(&ui8IPLen, sizeof(ui8IPLen));
		packet.WriteData(pszIP, ui8IPLen);

		packet.WriteData(&ui16Port, sizeof(ui16Port));
	}

	SendPacketMessage(&packet);
}

uint32_t ConnectSession::GenerateAuthToken()
{
	uint32_t token = (uint32_t)GetTickCount();
	token ^= (uint32_t)(m_i64UID & 0xFFFFFFFF);
	token ^= (uint32_t)(m_SessionIdx << 8);
	token ^= 0xA5A5A5A5;
	return token;
}

// ============================================================================
// Inter-server handlers (from GameServer connections)
// ============================================================================

void ConnectSession::OnISServerRegisterReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_SERVER_REGISTER_REQ))
		return;

	IS_SERVER_REGISTER_REQ* pReq = (IS_SERVER_REGISTER_REQ*)pData;

	printf("[ConnectSession:%d] SERVER_REGISTER_REQ: ID=%d Name='%s' IP=%s:%d\n",
		m_SessionIdx, pReq->i32ServerId, pReq->szServerName,
		pReq->szPublicIP, pReq->ui16PublicPort);

	GameServerRegistry* pRegistry = g_pConnectServerContext ? g_pConnectServerContext->GetRegistry() : nullptr;
	int i32Result = 1; // Rejected by default
	int i32AssignedId = pReq->i32ServerId;

	if (pRegistry)
	{
		ServerInfo info;
		info.id = pReq->i32ServerId;
		info.type = (ServerType)pReq->i32ServerType;
		strncpy(info.name, pReq->szServerName, sizeof(info.name) - 1);
		strncpy(info.ip, pReq->szPublicIP, sizeof(info.ip) - 1);
		info.port = pReq->ui16PublicPort;
		info.state = ServerOnlineState::Online;
		info.maxPlayers = pReq->i32MaxPlayers;
		info.currentPlayers = 0;
		strncpy(info.region, pReq->szRegion, sizeof(info.region) - 1);

		if (pRegistry->RegisterServer(info))
		{
			pRegistry->SetServerSessionIdx(pReq->i32ServerId, m_SessionIdx);
			i32Result = 0;
		}
	}

	// Send ACK
	IS_SERVER_REGISTER_ACK ack;
	ack.i32Result = i32Result;
	ack.i32AssignedId = i32AssignedId;
	i3NetworkPacket packet(PROTOCOL_IS_SERVER_REGISTER_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendPacketMessage(&packet);
}

void ConnectSession::OnISHeartbeatReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_HEARTBEAT_REQ))
		return;

	IS_HEARTBEAT_REQ* pReq = (IS_HEARTBEAT_REQ*)pData;

	GameServerRegistry* pRegistry = g_pConnectServerContext ? g_pConnectServerContext->GetRegistry() : nullptr;
	if (pRegistry)
	{
		pRegistry->UpdateHeartbeat(pReq->i32ServerId);
		pRegistry->UpdateServerStatus(pReq->i32ServerId, ServerOnlineState::Online, pReq->i32CurrentPlayers);
	}

	// Send ACK
	IS_HEARTBEAT_ACK ack;
	ack.i32Result = 0;
	i3NetworkPacket packet(PROTOCOL_IS_HEARTBEAT_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendPacketMessage(&packet);//#define SendMessage  SendMessageW
}

void ConnectSession::OnISServerStatusUpdateReq(char* pData, INT32 i32Size)
{
	if (i32Size < (INT32)sizeof(IS_SERVER_STATUS_UPDATE_REQ))
		return;

	IS_SERVER_STATUS_UPDATE_REQ* pReq = (IS_SERVER_STATUS_UPDATE_REQ*)pData;

	GameServerRegistry* pRegistry = g_pConnectServerContext ? g_pConnectServerContext->GetRegistry() : nullptr;
	if (pRegistry)
	{
		pRegistry->UpdateServerStatus(pReq->i32ServerId,
			(ServerOnlineState)pReq->i32State, pReq->i32CurrentPlayers);
	}

	// Send ACK
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_IS_SERVER_STATUS_UPDATE_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	SendPacketMessage(&packet);
}

bool ConnectSession::SendAuthTransferToGameServer(int i32ServerId,
	int64_t i64UID, uint32_t ui32AuthToken,
	const char* pszUsername, uint32_t ui32ClientIP)
{
	if (!g_pConnectServerContext)
		return false;

	GameServerRegistry* pRegistry = g_pConnectServerContext->GetRegistry();
	if (!pRegistry)
		return false;

	int gsSessionIdx = pRegistry->GetServerSessionIdx(i32ServerId);
	if (gsSessionIdx < 0)
		return false;

	ConnectSessionManager* pMgr = g_pConnectServerContext->GetSessionManager();
	if (!pMgr)
		return false;

	ConnectSession* pGSSession = pMgr->GetSession(gsSessionIdx);
	if (!pGSSession)
		return false;

	// Build IS_PLAYER_AUTH_TRANSFER_REQ packet
	IS_PLAYER_AUTH_TRANSFER_REQ req;
	memset(&req, 0, sizeof(req));
	req.i64UID = i64UID;
	req.ui32AuthToken = ui32AuthToken;
	if (pszUsername)
		strncpy(req.szUsername, pszUsername, sizeof(req.szUsername) - 1);
	req.ui32ClientIP = ui32ClientIP;
	i3NetworkPacket packet(PROTOCOL_IS_PLAYER_AUTH_TRANSFER_REQ);
	packet.WriteData(&req, sizeof(req));
	pGSSession->SendPacketMessage(&packet);

	printf("[ConnectSession] Auth transfer sent to GameServer ID=%d for UID=%lld Token=0x%08X\n",
		i32ServerId, (long long)i64UID, ui32AuthToken);
	return true;
}
