#include "DataSession.h"
#include "DataServerContext.h"
#include "TaskProcessor.h"
#include "i3NetworkPacket.h"
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

	// Game data
	case PROTOCOL_IS_EQUIPMENT_SAVE_REQ:
		OnEquipmentSaveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_MEDAL_SAVE_REQ:
		OnMedalSaveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_ATTENDANCE_SAVE_REQ:
		OnAttendanceSaveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_SKILL_SAVE_REQ:
		OnSkillSaveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_QUEST_SAVE_REQ:
		OnQuestSaveReq(pData, ui16DataSize);
		break;

	// Social
	case PROTOCOL_IS_CLAN_CREATE_REQ:
		OnClanCreateReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_CLAN_DISBAND_REQ:
		OnClanDisbandReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_CLAN_JOIN_REQ:
		OnClanJoinReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_CLAN_LEAVE_REQ:
		OnClanLeaveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_FRIEND_ADD_REQ:
		OnFriendAddReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_FRIEND_REMOVE_REQ:
		OnFriendRemoveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_FRIEND_LIST_REQ:
		OnFriendListReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_BLOCK_ADD_REQ:
		OnBlockAddReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_BLOCK_REMOVE_REQ:
		OnBlockRemoveReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_BLOCK_LIST_REQ:
		OnBlockListReq(pData, ui16DataSize);
		break;

	// Shop
	case PROTOCOL_IS_SHOP_LIST_REQ:
		OnShopListReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_SHOP_BUY_REQ:
		OnShopBuyReq(pData, ui16DataSize);
		break;

	// Inventory
	case PROTOCOL_IS_INVEN_UPDATE_REQ:
		OnInvenUpdateReq(pData, ui16DataSize);
		break;

	// Clan load
	case PROTOCOL_IS_CLAN_LOAD_REQ:
		OnClanLoadReq(pData, ui16DataSize);
		break;

	// Notes/Mail
	case PROTOCOL_IS_NOTE_SEND_REQ:
		OnNoteSendReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_NOTE_LIST_REQ:
		OnNoteListReq(pData, ui16DataSize);
		break;
	case PROTOCOL_IS_NOTE_DELETE_REQ:
		OnNoteDeleteReq(pData, ui16DataSize);
		break;

	// Ban
	case PROTOCOL_IS_PLAYER_BAN_REQ:
		OnPlayerBanReq(pData, ui16DataSize);
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

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_AUTH_LOGIN, m_SessionIdx, PROTOCOL_IS_AUTH_LOGIN_REQ,
		pReq, sizeof(IS_AUTH_LOGIN_REQ));
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

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_PLAYER_LOAD, m_SessionIdx, PROTOCOL_IS_PLAYER_LOAD_REQ,
		pReq, sizeof(IS_PLAYER_LOAD_REQ));
}

void DataSession::OnPlayerSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_PLAYER_SAVE_REQ))
		return;

	IS_PLAYER_SAVE_REQ* pReq = (IS_PLAYER_SAVE_REQ*)pData;

	printf("[DataSession:%d] PLAYER_SAVE_REQ: UID=%lld\n", m_SessionIdx, pReq->i64UID);

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_PLAYER_SAVE, m_SessionIdx, PROTOCOL_IS_PLAYER_SAVE_REQ,
		pReq, sizeof(IS_PLAYER_SAVE_REQ));
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

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_PLAYER_CREATE_NICK, m_SessionIdx, PROTOCOL_IS_PLAYER_CREATE_NICK_REQ,
		pReq, sizeof(IS_PLAYER_CREATE_NICK_REQ));
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

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_PLAYER_CHECK_NICK, m_SessionIdx, PROTOCOL_IS_PLAYER_CHECK_NICK_REQ,
		pReq, sizeof(IS_PLAYER_CHECK_NICK_REQ));
}

void DataSession::OnStatsSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	if (i32Size < (INT32)sizeof(IS_STATS_SAVE_REQ))
		return;

	IS_STATS_SAVE_REQ* pReq = (IS_STATS_SAVE_REQ*)pData;

	printf("[DataSession:%d] STATS_SAVE_REQ: UID=%lld\n", m_SessionIdx, pReq->i64UID);

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_STATS_SAVE, m_SessionIdx, PROTOCOL_IS_STATS_SAVE_REQ,
		pReq, sizeof(IS_STATS_SAVE_REQ));
}

// -- Game Data Handlers (all async via TaskProcessor) --

void DataSession::OnEquipmentSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_EQUIPMENT_SAVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_EQUIPMENT_SAVE_REQ,
		pData, sizeof(IS_EQUIPMENT_SAVE_REQ));
}

void DataSession::OnMedalSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_MEDAL_SAVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_MEDAL_SAVE_REQ,
		pData, sizeof(IS_MEDAL_SAVE_REQ));
}

void DataSession::OnAttendanceSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_ATTENDANCE_SAVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_ATTENDANCE_SAVE_REQ,
		pData, sizeof(IS_ATTENDANCE_SAVE_REQ));
}

void DataSession::OnSkillSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_SKILL_SAVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_SKILL_SAVE_REQ,
		pData, sizeof(IS_SKILL_SAVE_REQ));
}

void DataSession::OnQuestSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_QUEST_SAVE_REQ))
		return;

	IS_QUEST_SAVE_REQ* pReq = (IS_QUEST_SAVE_REQ*)pData;
	int i32TotalSize = (int)sizeof(IS_QUEST_SAVE_REQ) + (int)pReq->ui16DataSize;
	if (i32Size < i32TotalSize)
		return;

	// Queue the full data including trailing quest payload
	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_QUEST_SAVE_REQ,
		pData, i32TotalSize);
}

// -- Social Handlers (all async via TaskProcessor) --

void DataSession::OnClanCreateReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_CREATE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_CLAN_CREATE_REQ,
		pData, sizeof(IS_CLAN_CREATE_REQ));
}

void DataSession::OnClanDisbandReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_DISBAND_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_CLAN_DISBAND_REQ,
		pData, sizeof(IS_CLAN_DISBAND_REQ));
}

void DataSession::OnClanJoinReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_JOIN_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_CLAN_JOIN_REQ,
		pData, sizeof(IS_CLAN_JOIN_REQ));
}

void DataSession::OnClanLeaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_LEAVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_CLAN_LEAVE_REQ,
		pData, sizeof(IS_CLAN_LEAVE_REQ));
}

void DataSession::OnFriendAddReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_FRIEND_ADD_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_FRIEND_ADD_REQ,
		pData, sizeof(IS_FRIEND_ADD_REQ));
}

void DataSession::OnFriendRemoveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_FRIEND_REMOVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_FRIEND_REMOVE_REQ,
		pData, sizeof(IS_FRIEND_REMOVE_REQ));
}

void DataSession::OnFriendListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_FRIEND_LIST_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_FRIEND_LIST_REQ,
		pData, sizeof(IS_FRIEND_LIST_REQ));
}

void DataSession::OnBlockAddReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_BLOCK_ADD_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_BLOCK_ADD_REQ,
		pData, sizeof(IS_BLOCK_ADD_REQ));
}

void DataSession::OnBlockRemoveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_BLOCK_REMOVE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_BLOCK_REMOVE_REQ,
		pData, sizeof(IS_BLOCK_REMOVE_REQ));
}

void DataSession::OnBlockListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_BLOCK_LIST_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_BLOCK_LIST_REQ,
		pData, sizeof(IS_BLOCK_LIST_REQ));
}

// -- Shop Handlers (async via TaskProcessor) --

void DataSession::OnShopListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;

	// ShopListReq has no meaningful payload, but queue with protocol ID for routing
	IS_SHOP_LIST_REQ req;
	req.i32SessionIdx = m_SessionIdx;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_SHOP_LIST_REQ,
		&req, sizeof(req));
}

void DataSession::OnShopBuyReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_SHOP_BUY_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_SHOP_BUY_REQ,
		pData, sizeof(IS_SHOP_BUY_REQ));
}

// -- Inventory Update Handler (async via TaskProcessor) --

void DataSession::OnInvenUpdateReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_INVEN_UPDATE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_GAME_DATA, m_SessionIdx, PROTOCOL_IS_INVEN_UPDATE_REQ,
		pData, sizeof(IS_INVEN_UPDATE_REQ));
}

// -- Clan Load Handler (async via TaskProcessor) --

void DataSession::OnClanLoadReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_LOAD_REQ))
		return;

	printf("[DataSession:%d] CLAN_LOAD_REQ: ClanId=%d\n",
		m_SessionIdx, ((IS_CLAN_LOAD_REQ*)pData)->i32ClanId);

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_CLAN_LOAD_REQ,
		pData, sizeof(IS_CLAN_LOAD_REQ));
}

// ============================================================================
// Note/Mail Handlers (async via TaskProcessor)
// ============================================================================

void DataSession::OnNoteSendReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_NOTE_SEND_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_NOTE_SEND_REQ,
		pData, sizeof(IS_NOTE_SEND_REQ));
}

void DataSession::OnNoteListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_NOTE_LIST_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_NOTE_LIST_REQ,
		pData, sizeof(IS_NOTE_LIST_REQ));
}

void DataSession::OnNoteDeleteReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_NOTE_DELETE_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_NOTE_DELETE_REQ,
		pData, sizeof(IS_NOTE_DELETE_REQ));
}

// ============================================================================
// Ban Handler (async via TaskProcessor)
// ============================================================================

void DataSession::OnPlayerBanReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetTaskProcessor())
		return;
	if (i32Size < (INT32)sizeof(IS_PLAYER_BAN_REQ))
		return;

	g_pDataServerContext->GetTaskProcessor()->QueueTask(
		TASK_SOCIAL, m_SessionIdx, PROTOCOL_IS_PLAYER_BAN_REQ,
		pData, sizeof(IS_PLAYER_BAN_REQ));
}

// ============================================================================
// Helpers
// ============================================================================

void DataSession::SendHeartbeatAck()
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_HEARTBEAT_ACK);
	IS_HEARTBEAT_ACK ack;
	ack.i32Result = 0;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}
