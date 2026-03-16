#include "DataSession.h"
#include "DataServerContext.h"
#include "TaskProcessor.h"
#include "ModuleDBGameData.h"
#include "ModuleDBSocial.h"
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

// -- Game Data Handlers --

void DataSession::OnEquipmentSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_EQUIPMENT_SAVE_REQ))
		return;

	IS_EQUIPMENT_SAVE_REQ* pReq = (IS_EQUIPMENT_SAVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBGameData()->SaveEquipment(pReq->i64UID, pReq);

	IS_EQUIPMENT_SAVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_EQUIPMENT_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnMedalSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_MEDAL_SAVE_REQ))
		return;

	IS_MEDAL_SAVE_REQ* pReq = (IS_MEDAL_SAVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBGameData()->SaveMedal(pReq->i64UID, pReq);

	IS_MEDAL_SAVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_MEDAL_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnAttendanceSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_ATTENDANCE_SAVE_REQ))
		return;

	IS_ATTENDANCE_SAVE_REQ* pReq = (IS_ATTENDANCE_SAVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBGameData()->SaveAttendance(pReq->i64UID, pReq);

	IS_ATTENDANCE_SAVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_ATTENDANCE_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnSkillSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_SKILL_SAVE_REQ))
		return;

	IS_SKILL_SAVE_REQ* pReq = (IS_SKILL_SAVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBGameData()->SaveSkill(pReq->i64UID, pReq);

	IS_SKILL_SAVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_SKILL_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnQuestSaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_QUEST_SAVE_REQ))
		return;

	IS_QUEST_SAVE_REQ* pReq = (IS_QUEST_SAVE_REQ*)pData;
	const char* pQuestData = pData + sizeof(IS_QUEST_SAVE_REQ);
	int i32DataSize = (int)pReq->ui16DataSize;
	if (i32Size < (INT32)(sizeof(IS_QUEST_SAVE_REQ) + i32DataSize))
		return;

	bool bResult = g_pDataServerContext->GetModuleDBGameData()->SaveQuest(
		pReq->i64UID, pReq, pQuestData, i32DataSize);

	IS_QUEST_SAVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_QUEST_SAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

// -- Social Handlers --

void DataSession::OnClanCreateReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_CREATE_REQ))
		return;

	IS_CLAN_CREATE_REQ* pReq = (IS_CLAN_CREATE_REQ*)pData;
	int i32ClanId = g_pDataServerContext->GetModuleDBSocial()->CreateClan(pReq);

	IS_CLAN_CREATE_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i64MasterUID = pReq->i64MasterUID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32ClanId = i32ClanId;
	ack.i32Result = (i32ClanId > 0) ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_CLAN_CREATE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnClanDisbandReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_DISBAND_REQ))
		return;

	IS_CLAN_DISBAND_REQ* pReq = (IS_CLAN_DISBAND_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBSocial()->DisbandClan(
		pReq->i32ClanId, pReq->i64MasterUID);

	IS_CLAN_DISBAND_ACK ack;
	ack.i32ClanId = pReq->i32ClanId;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_CLAN_DISBAND_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnClanJoinReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_JOIN_REQ))
		return;

	IS_CLAN_JOIN_REQ* pReq = (IS_CLAN_JOIN_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBSocial()->JoinClan(pReq);

	IS_CLAN_JOIN_ACK ack;
	ack.i32ClanId = pReq->i32ClanId;
	ack.i64UID = pReq->i64UID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_CLAN_JOIN_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnClanLeaveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_LEAVE_REQ))
		return;

	IS_CLAN_LEAVE_REQ* pReq = (IS_CLAN_LEAVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBSocial()->LeaveClan(
		pReq->i32ClanId, pReq->i64UID);

	IS_CLAN_LEAVE_ACK ack;
	ack.i32ClanId = pReq->i32ClanId;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_CLAN_LEAVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnFriendAddReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_FRIEND_ADD_REQ))
		return;

	IS_FRIEND_ADD_REQ* pReq = (IS_FRIEND_ADD_REQ*)pData;
	int i32Result = g_pDataServerContext->GetModuleDBSocial()->AddFriend(
		pReq->i64UID, pReq->i64FriendUID);

	IS_FRIEND_ADD_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i64FriendUID = pReq->i64FriendUID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Result = i32Result;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_FRIEND_ADD_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnFriendRemoveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_FRIEND_REMOVE_REQ))
		return;

	IS_FRIEND_REMOVE_REQ* pReq = (IS_FRIEND_REMOVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBSocial()->RemoveFriend(
		pReq->i64UID, pReq->i64FriendUID);

	IS_FRIEND_REMOVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i64FriendUID = pReq->i64FriendUID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_FRIEND_REMOVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnFriendListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_FRIEND_LIST_REQ))
		return;

	IS_FRIEND_LIST_REQ* pReq = (IS_FRIEND_LIST_REQ*)pData;

	IS_FRIEND_ENTRY friends[100];
	memset(friends, 0, sizeof(friends));
	int i32Count = g_pDataServerContext->GetModuleDBSocial()->LoadFriendList(
		pReq->i64UID, friends, 100);

	IS_FRIEND_LIST_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Count = i32Count;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_FRIEND_LIST_ACK);
	packet.WriteData(&ack, sizeof(ack));
	if (i32Count > 0)
		packet.WriteData(friends, i32Count * sizeof(IS_FRIEND_ENTRY));
	SendMessage(&packet);
}

void DataSession::OnBlockAddReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_BLOCK_ADD_REQ))
		return;

	IS_BLOCK_ADD_REQ* pReq = (IS_BLOCK_ADD_REQ*)pData;
	int i32Result = g_pDataServerContext->GetModuleDBSocial()->AddBlock(
		pReq->i64UID, pReq->i64BlockedUID);

	IS_BLOCK_ADD_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i64BlockedUID = pReq->i64BlockedUID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Result = i32Result;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BLOCK_ADD_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnBlockRemoveReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_BLOCK_REMOVE_REQ))
		return;

	IS_BLOCK_REMOVE_REQ* pReq = (IS_BLOCK_REMOVE_REQ*)pData;
	bool bResult = g_pDataServerContext->GetModuleDBSocial()->RemoveBlock(
		pReq->i64UID, pReq->i64BlockedUID);

	IS_BLOCK_REMOVE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i64BlockedUID = pReq->i64BlockedUID;
	ack.i32Result = bResult ? 0 : 1;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BLOCK_REMOVE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

void DataSession::OnBlockListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_BLOCK_LIST_REQ))
		return;

	IS_BLOCK_LIST_REQ* pReq = (IS_BLOCK_LIST_REQ*)pData;

	IS_BLOCK_ENTRY blocks[100];
	memset(blocks, 0, sizeof(blocks));
	int i32Count = g_pDataServerContext->GetModuleDBSocial()->LoadBlockList(
		pReq->i64UID, blocks, 100);

	IS_BLOCK_LIST_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.i32Count = i32Count;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_BLOCK_LIST_ACK);
	packet.WriteData(&ack, sizeof(ack));
	if (i32Count > 0)
		packet.WriteData(blocks, i32Count * sizeof(IS_BLOCK_ENTRY));
	SendMessage(&packet);
}

// -- Shop Handler --

void DataSession::OnShopListReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;

	IS_SHOP_ITEM_ENTRY items[500];
	memset(items, 0, sizeof(items));
	int i32Count = g_pDataServerContext->GetModuleDBGameData()->LoadShopItems(items, 500);

	IS_SHOP_LIST_ACK ack;
	ack.i32Result = 0;
	ack.i32ItemCount = i32Count;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_SHOP_LIST_ACK);
	packet.WriteData(&ack, sizeof(ack));
	if (i32Count > 0)
		packet.WriteData(items, i32Count * sizeof(IS_SHOP_ITEM_ENTRY));
	SendMessage(&packet);

	printf("[DataSession:%d] SHOP_LIST_ACK: sent %d items\n", m_SessionIdx, i32Count);
}

// -- Shop Buy Handler --

void DataSession::OnShopBuyReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_SHOP_BUY_REQ))
		return;

	IS_SHOP_BUY_REQ* pReq = (IS_SHOP_BUY_REQ*)pData;

	printf("[DataSession:%d] SHOP_BUY_REQ: UID=%lld, GoodsId=%u, PayType=%d, Price=%d\n",
		m_SessionIdx, pReq->i64UID, pReq->ui32GoodsId, pReq->ui8PayType, pReq->i32Price);

	IS_SHOP_BUY_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i64UID = pReq->i64UID;
	ack.i32SessionIdx = pReq->i32SessionIdx;
	ack.ui32ItemId = pReq->ui32ItemId;

	g_pDataServerContext->GetModuleDBGameData()->BuyShopItem(pReq, &ack);

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_SHOP_BUY_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

// -- Inventory Update Handler --

void DataSession::OnInvenUpdateReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBGameData())
		return;
	if (i32Size < (INT32)sizeof(IS_INVEN_UPDATE_REQ))
		return;

	IS_INVEN_UPDATE_REQ* pReq = (IS_INVEN_UPDATE_REQ*)pData;

	bool bResult = g_pDataServerContext->GetModuleDBGameData()->UpdateInventory(pReq);

	IS_INVEN_UPDATE_ACK ack;
	ack.i64UID = pReq->i64UID;
	ack.i32Result = bResult ? 0 : 3;
	ack.ui32ItemId = pReq->ui32ItemId;
	ack.i32SlotIdx = pReq->i32SlotIdx;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_INVEN_UPDATE_ACK);
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}

// -- Clan Load Handler --

void DataSession::OnClanLoadReq(char* pData, INT32 i32Size)
{
	if (!g_pDataServerContext || !g_pDataServerContext->GetModuleDBSocial())
		return;
	if (i32Size < (INT32)sizeof(IS_CLAN_LOAD_REQ))
		return;

	IS_CLAN_LOAD_REQ* pReq = (IS_CLAN_LOAD_REQ*)pData;

	printf("[DataSession:%d] CLAN_LOAD_REQ: ClanId=%d\n", m_SessionIdx, pReq->i32ClanId);

	IS_CLAN_LOAD_ACK ack;
	memset(&ack, 0, sizeof(ack));
	ack.i32ClanId = pReq->i32ClanId;

	IS_CLAN_MEMBER_INFO members[50];
	memset(members, 0, sizeof(members));
	int memberCount = 0;

	bool bResult = g_pDataServerContext->GetModuleDBSocial()->LoadClan(
		pReq->i32ClanId, &ack, members, 50, &memberCount);

	ack.i32Result = bResult ? 0 : 1;
	ack.i32MemberCount = memberCount;

	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_CLAN_LOAD_ACK);
	packet.WriteData(&ack, sizeof(ack));
	if (memberCount > 0)
		packet.WriteData(members, memberCount * sizeof(IS_CLAN_MEMBER_INFO));
	SendMessage(&packet);
}

void DataSession::SendHeartbeatAck()
{
	i3NetworkPacket packet((PROTOCOL)PROTOCOL_IS_HEARTBEAT_ACK);
	IS_HEARTBEAT_ACK ack;
	ack.i32Result = 0;
	packet.WriteData(&ack, sizeof(ack));
	SendMessage(&packet);
}
