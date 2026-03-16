#include "pch.h"
#include "ModuleDataServer.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameServerContext.h"
#include "ClanDef.h"

// ============================================================================
// Request operations - Clan
// ============================================================================

void ModuleDataServer::RequestClanCreate(int64_t i64MasterUID, int i32SessionIdx,
										  const char* pszClanName, const char* pszMasterNick)
{
	IS_CLAN_CREATE_REQ req;
	memset(&req, 0, sizeof(req));
	req.i64MasterUID = i64MasterUID;
	req.i32SessionIdx = i32SessionIdx;
	if (pszClanName)
		strncpy_s(req.szClanName, pszClanName, _TRUNCATE);
	if (pszMasterNick)
		strncpy_s(req.szMasterNickname, pszMasterNick, _TRUNCATE);
	SendRequest(PROTOCOL_IS_CLAN_CREATE_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestClanDisband(int i32ClanId, int64_t i64MasterUID)
{
	IS_CLAN_DISBAND_REQ req;
	req.i32ClanId = i32ClanId;
	req.i64MasterUID = i64MasterUID;
	SendRequest(PROTOCOL_IS_CLAN_DISBAND_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestClanJoin(int i32ClanId, int64_t i64UID, int i32SessionIdx,
										const char* pszNickname, uint8_t ui8Level)
{
	IS_CLAN_JOIN_REQ req;
	memset(&req, 0, sizeof(req));
	req.i32ClanId = i32ClanId;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	if (pszNickname)
		strncpy_s(req.szNickname, pszNickname, _TRUNCATE);
	req.ui8MemberLevel = ui8Level;
	SendRequest(PROTOCOL_IS_CLAN_JOIN_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestClanLeave(int i32ClanId, int64_t i64UID)
{
	IS_CLAN_LEAVE_REQ req;
	req.i32ClanId = i32ClanId;
	req.i64UID = i64UID;
	SendRequest(PROTOCOL_IS_CLAN_LEAVE_REQ, &req, sizeof(req));
}

// ============================================================================
// Request operations - Friends
// ============================================================================

void ModuleDataServer::RequestFriendAdd(int64_t i64UID, int64_t i64FriendUID, int i32SessionIdx)
{
	IS_FRIEND_ADD_REQ req;
	req.i64UID = i64UID;
	req.i64FriendUID = i64FriendUID;
	req.i32SessionIdx = i32SessionIdx;
	SendRequest(PROTOCOL_IS_FRIEND_ADD_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestFriendRemove(int64_t i64UID, int64_t i64FriendUID)
{
	IS_FRIEND_REMOVE_REQ req;
	req.i64UID = i64UID;
	req.i64FriendUID = i64FriendUID;
	SendRequest(PROTOCOL_IS_FRIEND_REMOVE_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestFriendList(int64_t i64UID, int i32SessionIdx)
{
	IS_FRIEND_LIST_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	SendRequest(PROTOCOL_IS_FRIEND_LIST_REQ, &req, sizeof(req));
}

// ============================================================================
// Request operations - Block
// ============================================================================

void ModuleDataServer::RequestBlockAdd(int64_t i64UID, int64_t i64BlockedUID, int i32SessionIdx)
{
	IS_BLOCK_ADD_REQ req;
	req.i64UID = i64UID;
	req.i64BlockedUID = i64BlockedUID;
	req.i32SessionIdx = i32SessionIdx;
	SendRequest(PROTOCOL_IS_BLOCK_ADD_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestBlockRemove(int64_t i64UID, int64_t i64BlockedUID)
{
	IS_BLOCK_REMOVE_REQ req;
	req.i64UID = i64UID;
	req.i64BlockedUID = i64BlockedUID;
	SendRequest(PROTOCOL_IS_BLOCK_REMOVE_REQ, &req, sizeof(req));
}

void ModuleDataServer::RequestBlockList(int64_t i64UID, int i32SessionIdx)
{
	IS_BLOCK_LIST_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	SendRequest(PROTOCOL_IS_BLOCK_LIST_REQ, &req, sizeof(req));
}

// ============================================================================
// Response handlers - Clan
// ============================================================================

void ModuleDataServer::OnClanCreateAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_CLAN_CREATE_ACK))
		return;

	IS_CLAN_CREATE_ACK* pAck = (IS_CLAN_CREATE_ACK*)pData;

	printf("[ModuleDataServer] Clan create result - UID=%lld, ClanId=%d, Result=%d\n",
		pAck->i64MasterUID, pAck->i32ClanId, pAck->i32Result);

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64MasterUID)
	{
		pSession->OnClanCreateResult(pAck->i32ClanId, pAck->i32Result);
	}
}

void ModuleDataServer::OnClanDisbandAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_CLAN_DISBAND_ACK))
		return;

	IS_CLAN_DISBAND_ACK* pAck = (IS_CLAN_DISBAND_ACK*)pData;

	printf("[ModuleDataServer] Clan disband result - ClanId=%d, Result=%d\n",
		pAck->i32ClanId, pAck->i32Result);
}

void ModuleDataServer::OnClanJoinAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_CLAN_JOIN_ACK))
		return;

	IS_CLAN_JOIN_ACK* pAck = (IS_CLAN_JOIN_ACK*)pData;

	printf("[ModuleDataServer] Clan join result - ClanId=%d, UID=%lld, Result=%d\n",
		pAck->i32ClanId, pAck->i64UID, pAck->i32Result);

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		pSession->OnClanJoinResult(pAck->i32ClanId, pAck->i32Result);
	}
}

void ModuleDataServer::RequestClanLoad(int i32ClanId)
{
	IS_CLAN_LOAD_REQ req;
	req.i32ClanId = i32ClanId;
	SendRequest(PROTOCOL_IS_CLAN_LOAD_REQ, &req, sizeof(req));
}

void ModuleDataServer::OnClanLoadAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_CLAN_LOAD_ACK))
		return;

	IS_CLAN_LOAD_ACK* pAck = (IS_CLAN_LOAD_ACK*)pData;

	printf("[ModuleDataServer] Clan load result - ClanId=%d, Name='%s', Result=%d, Members=%d\n",
		pAck->i32ClanId, pAck->szName, pAck->i32Result, pAck->i32MemberCount);

	if (pAck->i32Result != 0)
		return;

	// Load clan data into GameClanManager
	extern GameClanManager* g_pClanManager;
	if (!g_pClanManager)
		return;

	GameClanInfo* pClan = g_pClanManager->FindClan(pAck->i32ClanId);
	if (!pClan)
	{
		// Clan not in memory yet - find empty slot and populate
		// For now we just log; the clan create flow already adds to manager
		printf("[ModuleDataServer] Clan %d not found in local manager, skipping load\n",
			pAck->i32ClanId);
		return;
	}

	// Update clan info from DB
	strncpy_s(pClan->szName, pAck->szName, _TRUNCATE);
	strncpy_s(pClan->szNotice, pAck->szNotice, _TRUNCATE);
	strncpy_s(pClan->szIntro, pAck->szIntro, _TRUNCATE);
	pClan->i64MasterUID = pAck->i64MasterUID;
	strncpy_s(pClan->szMasterNickname, pAck->szMasterNickname, _TRUNCATE);
	pClan->i32MaxMembers = pAck->i32MaxMembers;
	pClan->i32ClanExp = pAck->i32ClanExp;
	pClan->i32ClanRank = pAck->i32ClanRank;
	pClan->i32Wins = pAck->i32Wins;
	pClan->i32Losses = pAck->i32Losses;
	pClan->ui8Unit = pAck->ui8Unit;
	pClan->ui16MarkId = pAck->ui16MarkId;
	pClan->ui8MarkColor = pAck->ui8MarkColor;

	// Load members
	if (pAck->i32MemberCount > 0)
	{
		IS_CLAN_MEMBER_INFO* pMembers = (IS_CLAN_MEMBER_INFO*)(pData + sizeof(IS_CLAN_LOAD_ACK));
		for (int i = 0; i < pAck->i32MemberCount && i < MAX_CLAN_MEMBERS; i++)
		{
			// Check if member already exists
			GameClanMember* pExisting = pClan->FindMember(pMembers[i].i64UID);
			if (!pExisting)
			{
				pClan->AddMember(pMembers[i].i64UID, pMembers[i].szNickname,
								 pMembers[i].ui8MemberLevel, 0, 0);
			}
		}
	}
}

void ModuleDataServer::OnClanLeaveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_CLAN_LEAVE_ACK))
		return;

	IS_CLAN_LEAVE_ACK* pAck = (IS_CLAN_LEAVE_ACK*)pData;

	printf("[ModuleDataServer] Clan leave result - ClanId=%d, UID=%lld, Result=%d\n",
		pAck->i32ClanId, pAck->i64UID, pAck->i32Result);
}

// ============================================================================
// Response handlers - Friends
// ============================================================================

void ModuleDataServer::OnFriendAddAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_FRIEND_ADD_ACK))
		return;

	IS_FRIEND_ADD_ACK* pAck = (IS_FRIEND_ADD_ACK*)pData;

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		pSession->OnFriendAddResult(pAck->i64FriendUID, pAck->i32Result);
	}
}

void ModuleDataServer::OnFriendRemoveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_FRIEND_REMOVE_ACK))
		return;

	IS_FRIEND_REMOVE_ACK* pAck = (IS_FRIEND_REMOVE_ACK*)pData;

	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Friend remove failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::OnFriendListAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_FRIEND_LIST_ACK))
		return;

	IS_FRIEND_LIST_ACK* pAck = (IS_FRIEND_LIST_ACK*)pData;

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		IS_FRIEND_ENTRY* pEntries = nullptr;
		if (pAck->i32Count > 0)
			pEntries = (IS_FRIEND_ENTRY*)(pData + sizeof(IS_FRIEND_LIST_ACK));

		pSession->OnFriendListLoaded(pEntries, pAck->i32Count);
	}
}

// ============================================================================
// Response handlers - Block
// ============================================================================

void ModuleDataServer::OnBlockAddAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_BLOCK_ADD_ACK))
		return;

	IS_BLOCK_ADD_ACK* pAck = (IS_BLOCK_ADD_ACK*)pData;

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		pSession->OnBlockAddResult(pAck->i64BlockedUID, pAck->i32Result);
	}
}

void ModuleDataServer::OnBlockRemoveAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_BLOCK_REMOVE_ACK))
		return;

	IS_BLOCK_REMOVE_ACK* pAck = (IS_BLOCK_REMOVE_ACK*)pData;

	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Block remove failed - UID=%lld\n", pAck->i64UID);
}

void ModuleDataServer::OnBlockListAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_BLOCK_LIST_ACK))
		return;

	IS_BLOCK_LIST_ACK* pAck = (IS_BLOCK_LIST_ACK*)pData;

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		IS_BLOCK_ENTRY* pEntries = nullptr;
		if (pAck->i32Count > 0)
			pEntries = (IS_BLOCK_ENTRY*)(pData + sizeof(IS_BLOCK_LIST_ACK));

		pSession->OnBlockListLoaded(pEntries, pAck->i32Count);
	}
}
