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
// Helper: build header (size+proto) + body and send
// ============================================================================

void ModuleDataServer::SendRequest(uint16_t ui16Protocol, const void* pStruct, int i32StructSize)
{
	if (!IsConnected())
		return;

	char buffer[8192];
	int offset = 0;

	uint16_t size = 0;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &ui16Protocol, sizeof(uint16_t));	offset += sizeof(uint16_t);
	memcpy(buffer + offset, pStruct, i32StructSize);			offset += i32StructSize;

	size = (uint16_t)offset;
	memcpy(buffer, &size, sizeof(uint16_t));

	i3NetworkPacket packet;
	packet.SetPacketData(buffer, offset);
	SendPacket(&packet);
}

// ============================================================================
// Request operations - Player core
// ============================================================================

void ModuleDataServer::RequestPlayerLoad(int i32SessionIdx, int64_t i64UID)
{
	IS_PLAYER_LOAD_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	SendRequest(PROTOCOL_IS_PLAYER_LOAD_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestPlayerSave(int64_t i64UID, int i32Level, int64_t i64Exp,
										  int i32Cash, int i32GP)
{
	IS_PLAYER_SAVE_REQ req;
	req.i64UID = i64UID;
	req.i32Level = i32Level;
	req.i64Exp = i64Exp;
	req.i32Cash = i32Cash;
	req.i32GP = i32GP;
	SendRequest(PROTOCOL_IS_PLAYER_SAVE_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestStatsSave(int64_t i64UID, int i32Kills, int i32Deaths,
										 int i32Headshots, int i32Wins, int i32Losses)
{
	IS_STATS_SAVE_REQ req;
	req.i64UID = i64UID;
	req.i32Kills = i32Kills;
	req.i32Deaths = i32Deaths;
	req.i32Headshots = i32Headshots;
	req.i32Wins = i32Wins;
	req.i32Losses = i32Losses;
	SendRequest(PROTOCOL_IS_STATS_SAVE_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestCreateNick(int64_t i64UID, int i32SessionIdx, const char* pszNickname)
{
	if (!pszNickname)
		return;

	IS_PLAYER_CREATE_NICK_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	strncpy_s(req.szNickname, pszNickname, _TRUNCATE);
	SendRequest(PROTOCOL_IS_PLAYER_CREATE_NICK_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestCheckNick(int i32SessionIdx, const char* pszNickname)
{
	if (!pszNickname)
		return;

	IS_PLAYER_CHECK_NICK_REQ req;
	req.i32SessionIdx = i32SessionIdx;
	strncpy_s(req.szNickname, pszNickname, _TRUNCATE);
	SendRequest(PROTOCOL_IS_PLAYER_CHECK_NICK_REQ, &req, sizeof(req));
}

// ============================================================================
// ModuleBase overrides
// ============================================================================

bool ModuleDataServer::OnConnect()
{
	printf("[ModuleDataServer] Connected to DataServer %s:%d\n",
		GetRemoteIP(), GetRemotePort());

	// Request shop catalog on connect
	RequestShopList();

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

	// Player core
	case PROTOCOL_IS_PLAYER_LOAD_ACK:		OnPlayerLoadAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_PLAYER_SAVE_ACK:		OnPlayerSaveAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_PLAYER_CREATE_NICK_ACK: OnCreateNickAck(pPayload, payloadSize);	break;
	case PROTOCOL_IS_PLAYER_CHECK_NICK_ACK:	OnCheckNickAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_STATS_SAVE_ACK:		OnStatsSaveAck(pPayload, payloadSize);		break;

	// Game data
	case PROTOCOL_IS_EQUIPMENT_SAVE_ACK:	OnEquipmentSaveAck(pPayload, payloadSize);	break;
	case PROTOCOL_IS_MEDAL_SAVE_ACK:		OnMedalSaveAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_ATTENDANCE_SAVE_ACK:	OnAttendanceSaveAck(pPayload, payloadSize);	break;
	case PROTOCOL_IS_SKILL_SAVE_ACK:		OnSkillSaveAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_QUEST_SAVE_ACK:		OnQuestSaveAck(pPayload, payloadSize);		break;

	// Clan
	case PROTOCOL_IS_CLAN_CREATE_ACK:		OnClanCreateAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_CLAN_DISBAND_ACK:		OnClanDisbandAck(pPayload, payloadSize);	break;
	case PROTOCOL_IS_CLAN_JOIN_ACK:			OnClanJoinAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_CLAN_LEAVE_ACK:		OnClanLeaveAck(pPayload, payloadSize);		break;

	// Social
	case PROTOCOL_IS_FRIEND_ADD_ACK:		OnFriendAddAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_FRIEND_REMOVE_ACK:		OnFriendRemoveAck(pPayload, payloadSize);	break;
	case PROTOCOL_IS_FRIEND_LIST_ACK:		OnFriendListAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_BLOCK_ADD_ACK:			OnBlockAddAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_BLOCK_REMOVE_ACK:		OnBlockRemoveAck(pPayload, payloadSize);	break;
	case PROTOCOL_IS_BLOCK_LIST_ACK:		OnBlockListAck(pPayload, payloadSize);		break;

	// Shop
	case PROTOCOL_IS_SHOP_LIST_ACK:			OnShopListAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_SHOP_BUY_ACK:			OnShopBuyAck(pPayload, payloadSize);		break;

	// Inventory
	case PROTOCOL_IS_INVEN_UPDATE_ACK:		OnInvenUpdateAck(pPayload, payloadSize);	break;

	// Clan load
	case PROTOCOL_IS_CLAN_LOAD_ACK:			OnClanLoadAck(pPayload, payloadSize);		break;

	// Notes/Mail
	case PROTOCOL_IS_NOTE_SEND_ACK:			OnNoteSendAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_NOTE_LIST_ACK:			OnNoteListAck(pPayload, payloadSize);		break;
	case PROTOCOL_IS_NOTE_DELETE_ACK:		OnNoteDeleteAck(pPayload, payloadSize);		break;

	// Ban
	case PROTOCOL_IS_PLAYER_BAN_ACK:		OnPlayerBanAck(pPayload, payloadSize);		break;

	default:
		printf("[ModuleDataServer] Unknown protocol 0x%04X\n", protocolId);
		break;
	}
}

void ModuleDataServer::OnHeartbeat()
{
	if (!IsConnected())
		return;

	IS_HEARTBEAT_REQ req;
	req.i32ServerId = g_pContextMain ? g_pContextMain->m_i32ServerId : 0;
	req.i32ServerType = (int)ServerType::Game;
	req.i32CurrentPlayers = 0;
	SendRequest(PROTOCOL_IS_HEARTBEAT_REQ, &req, sizeof(req));
}

// ============================================================================
// Response handlers - Player core
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
		printf("[ModuleDataServer] Player save failed - UID=%lld, Result=%d\n",
			pAck->i64UID, pAck->i32Result);
}

void ModuleDataServer::OnCreateNickAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_CREATE_NICK_ACK))
		return;

	IS_PLAYER_CREATE_NICK_ACK* pAck = (IS_PLAYER_CREATE_NICK_ACK*)pData;

	printf("[ModuleDataServer] Create nick result - UID=%lld, Result=%d\n",
		pAck->i64UID, pAck->i32Result);

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		pSession->OnCreateNickResult(pAck->i32Result, pAck->szNickname);
	}
}

void ModuleDataServer::OnCheckNickAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_PLAYER_CHECK_NICK_ACK))
		return;

	IS_PLAYER_CHECK_NICK_ACK* pAck = (IS_PLAYER_CHECK_NICK_ACK*)pData;

	printf("[ModuleDataServer] Check nick result - SessionIdx=%d, Result=%d\n",
		pAck->i32SessionIdx, pAck->i32Result);

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession)
	{
		pSession->OnCheckNickResult(pAck->i32Result);
	}
}

void ModuleDataServer::OnStatsSaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_STATS_SAVE_ACK))
		return;

	IS_STATS_SAVE_ACK* pAck = (IS_STATS_SAVE_ACK*)pData;

	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Stats save failed - UID=%lld, Result=%d\n",
			pAck->i64UID, pAck->i32Result);
}
