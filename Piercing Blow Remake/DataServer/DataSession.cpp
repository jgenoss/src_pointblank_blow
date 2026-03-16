#include "DataSession.h"
#include "DataServerContext.h"
#include "TaskProcessor.h"
#include <cstdio>
#include <cstring>

I3_CLASS_INSTANCE(DataSession, i3NetworkSession);

DataSession::DataSession()
	: m_eType(DATA_SESSION_UNKNOWN)
	, m_i32RemoteServerId(0)
{
}

DataSession::~DataSession()
{
}

BOOL DataSession::Create()
{
	return i3NetworkSession::Create();
}

void DataSession::Destroy()
{
	m_eType = DATA_SESSION_UNKNOWN;
	m_i32RemoteServerId = 0;
	i3NetworkSession::Destroy();
}

BOOL DataSession::OnConnect(SOCKET Socket, struct sockaddr_in* pAddr)
{
	if (!i3NetworkSession::OnConnect(Socket, pAddr))
		return FALSE;

	printf("[DataSession:%d] Server connected from %s\n", m_SessionIdx, m_strIPString);
	return TRUE;
}

BOOL DataSession::OnDisconnect(BOOL bForceMainThread)
{
	printf("[DataSession:%d] Server disconnected (type=%d, serverId=%d)\n",
		m_SessionIdx, (int)m_eType, m_i32RemoteServerId);

	m_eType = DATA_SESSION_UNKNOWN;
	m_i32RemoteServerId = 0;

	return i3NetworkSession::OnDisconnect(bForceMainThread);
}

INT32 DataSession::PacketParsing(char* pPacket, INT32 iSize)
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
	// Heartbeat
	case PROTOCOL_IS_HEARTBEAT_REQ:
		OnHeartbeatReq(pData, ui16DataSize);
		break;

	// Auth (from ConnectServer)
	case PROTOCOL_IS_AUTH_LOGIN_REQ:
		OnAuthLoginReq(pData, ui16DataSize);
		break;

	// Player data (from GameServer)
	case PROTOCOL_IS_PLAYER_LOAD_REQ:
		OnPlayerLoadReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_PLAYER_SAVE_REQ:
		OnPlayerSaveReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_PLAYER_CREATE_NICK_REQ:
		OnPlayerCreateNickReq(pData, ui16DataSize);
		break;

	case PROTOCOL_IS_PLAYER_CHECK_NICK_REQ:
		OnPlayerCheckNickReq(pData, ui16DataSize);
		break;

	// Stats (from GameServer/BattleServer)
	case PROTOCOL_IS_STATS_SAVE_REQ:
		OnStatsSaveReq(pData, ui16DataSize);
		break;

	default:
		printf("[DataSession:%d] Unknown protocol: 0x%04X\n", m_SessionIdx, protocolID);
		break;
	}

	return i32PacketSize;
}

// -- Packet Handlers --

void DataSession::OnHeartbeatReq(char* pData, INT32 i32Size)
{
	if (i32Size >= (INT32)sizeof(IS_HEARTBEAT_REQ))
	{
		IS_HEARTBEAT_REQ* pReq = (IS_HEARTBEAT_REQ*)pData;
		m_i32RemoteServerId = pReq->i32ServerId;

		// Detectar tipo de servidor por el campo type
		switch (pReq->i32ServerType)
		{
		case (int)ServerType::Connect:	m_eType = DATA_SESSION_CONNECT_SERVER; break;
		case (int)ServerType::Game:		m_eType = DATA_SESSION_GAME_SERVER; break;
		case (int)ServerType::Battle:	m_eType = DATA_SESSION_BATTLE_SERVER; break;
		default: break;
		}
	}

	SendHeartbeatAck();
}

void DataSession::OnAuthLoginReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_AUTH_LOGIN_REQ))
		return;

	IS_AUTH_LOGIN_REQ* pReq = (IS_AUTH_LOGIN_REQ*)pData;

	printf("[DataSession:%d] AUTH_LOGIN_REQ: user='%s' sessionIdx=%d\n",
		m_SessionIdx, pReq->szUsername, pReq->i32SessionIdx);

	g_pDataServerContext->GetTaskProcessor()->ProcessAuthLogin(this, pReq);
}

void DataSession::OnPlayerLoadReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_PLAYER_LOAD_REQ))
		return;

	IS_PLAYER_LOAD_REQ* pReq = (IS_PLAYER_LOAD_REQ*)pData;

	printf("[DataSession:%d] PLAYER_LOAD_REQ: UID=%lld sessionIdx=%d\n",
		m_SessionIdx, pReq->i64UID, pReq->i32SessionIdx);

	g_pDataServerContext->GetTaskProcessor()->ProcessPlayerLoad(this, pReq);
}

void DataSession::OnPlayerSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_PLAYER_SAVE_REQ))
		return;

	IS_PLAYER_SAVE_REQ* pReq = (IS_PLAYER_SAVE_REQ*)pData;

	printf("[DataSession:%d] PLAYER_SAVE_REQ: UID=%lld\n", m_SessionIdx, pReq->i64UID);

	g_pDataServerContext->GetTaskProcessor()->ProcessPlayerSave(this, pReq);
}

void DataSession::OnPlayerCreateNickReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_PLAYER_CREATE_NICK_REQ))
		return;

	IS_PLAYER_CREATE_NICK_REQ* pReq = (IS_PLAYER_CREATE_NICK_REQ*)pData;

	printf("[DataSession:%d] PLAYER_CREATE_NICK_REQ: UID=%lld nick='%s'\n",
		m_SessionIdx, pReq->i64UID, pReq->szNickname);

	g_pDataServerContext->GetTaskProcessor()->ProcessPlayerCreateNick(this, pReq);
}

void DataSession::OnPlayerCheckNickReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_PLAYER_CHECK_NICK_REQ))
		return;

	IS_PLAYER_CHECK_NICK_REQ* pReq = (IS_PLAYER_CHECK_NICK_REQ*)pData;

	printf("[DataSession:%d] PLAYER_CHECK_NICK_REQ: nick='%s'\n",
		m_SessionIdx, pReq->szNickname);

	g_pDataServerContext->GetTaskProcessor()->ProcessPlayerCheckNick(this, pReq);
}

void DataSession::OnStatsSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_STATS_SAVE_REQ))
		return;

	IS_STATS_SAVE_REQ* pReq = (IS_STATS_SAVE_REQ*)pData;

	printf("[DataSession:%d] STATS_SAVE_REQ: UID=%lld\n", m_SessionIdx, pReq->i64UID);

	g_pDataServerContext->GetTaskProcessor()->ProcessStatsSave(this, pReq);
}

void DataSession::SendHeartbeatAck()
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_HEARTBEAT_ACK);
	IS_HEARTBEAT_ACK ack;
	ack.i32Result = 0;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}
