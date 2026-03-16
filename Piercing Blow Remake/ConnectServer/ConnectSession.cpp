#include "ConnectSession.h"
#include "ConnectServerContext.h"
#include "GameServerRegistry.h"
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
	return i3NetworkSession::OnConnectInit();
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

	// TODO: Extraer credenciales del paquete y validar via DataServer
	// Por ahora, autenticacion hardcoded para desarrollo
	// En fase posterior se implementara la comunicacion con DataServer

	m_i64UID = 1000 + m_SessionIdx; // UID temporal
	strncpy(m_szUsername, "TestUser", sizeof(m_szUsername) - 1);

	m_eTask = CONNECT_TASK_AUTHENTICATED;

	// Enviar resultado exitoso
	SendLoginAck(0); // 0 = success
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

	// Generar auth token para la transferencia
	m_ui32AuthToken = GenerateAuthToken();

	// TODO: Notificar al GameServer seleccionado sobre el token entrante
	// (Se implementara en la fase de integracion via inter-server protocol)

	m_eTask = CONNECT_TASK_SERVER_SELECT;
	SendServerSelectAck(0, pServer->ip, pServer->port);
}

void ConnectSession::OnPacketHeartBitReq(char* pData, INT32 i32Size)
{
	// Responder heartbeat
	i3NetworkPacket packet(PROTOCOL_BASE_HEART_BIT_ACK);
	SendMessage(&packet);
}

// -- Send Helpers --

void ConnectSession::SendConnectAck()
{
	i3NetworkPacket packet(PROTOCOL_BASE_CONNECT_ACK);

	// Escribir XOR key en el ACK
	packet.WriteData(&m_ui32XorKey, sizeof(m_ui32XorKey));

	SendMessage(&packet);
}

void ConnectSession::SendLoginAck(int i32Result)
{
	i3NetworkPacket packet(PROTOCOL_LOGIN_ACK);

	packet.WriteData(&i32Result, sizeof(i32Result));
	if (i32Result == 0)
	{
		packet.WriteData(&m_i64UID, sizeof(m_i64UID));
	}

	SendMessage(&packet);
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

	SendMessage(&packet);
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

	SendMessage(&packet);
}

uint32_t ConnectSession::GenerateAuthToken()
{
	// Generar token simple basado en tiempo + session + UID
	// TODO: Usar algo mas robusto (S2MO RSA/MD5)
	uint32_t token = (uint32_t)GetTickCount();
	token ^= (uint32_t)(m_i64UID & 0xFFFFFFFF);
	token ^= (uint32_t)(m_SessionIdx << 8);
	token ^= 0xA5A5A5A5;
	return token;
}
