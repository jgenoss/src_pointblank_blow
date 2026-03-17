#ifndef __MODULEDBSOCIAL_H__
#define __MODULEDBSOCIAL_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;
class DataSession;

// Modulo de operaciones sociales de DB
// Maneja: clans (extendido), friends, blocks, notes, bans,
//         clan requests, clan items, clan season, mercenary
class ModuleDBSocial
{
public:
	ModuleDBSocial(DBConnectionPool* pPool);
	~ModuleDBSocial();

	// Clan operations (base)
	int  CreateClan(IS_CLAN_CREATE_REQ* pReq);			// Returns clan_id, 0 on error
	bool DisbandClan(int i32ClanId, int64_t i64MasterUID);
	bool JoinClan(IS_CLAN_JOIN_REQ* pReq);
	bool LeaveClan(int i32ClanId, int64_t i64UID);
	bool LoadClan(int i32ClanId, IS_CLAN_LOAD_ACK* pOut,
				  IS_CLAN_MEMBER_INFO* pMembers, int i32MaxMembers, int* pMemberCount);

	// Clan Join Requests
	bool SendClanRequest(IS_CLAN_REQUEST_SEND_REQ* pReq, IS_CLAN_REQUEST_SEND_ACK* pAck);
	int  LoadClanRequests(int i32ClanId, IS_CLAN_REQUEST_ENTRY* pOut, int i32MaxCount);
	bool RespondClanRequest(IS_CLAN_REQUEST_RESPOND_REQ* pReq, IS_CLAN_REQUEST_RESPOND_ACK* pAck);

	// Clan Items
	int  LoadClanItems(int i32ClanId, IS_CLAN_ITEM_ENTRY* pOut, int i32MaxCount);

	// Clan Match Result (update stats after clan war)
	bool UpdateClanMatchResult(IS_CLAN_MATCH_RESULT_REQ* pReq);

	// Clan Season
	int  LoadClanSeason(int i32ClanId, IS_CLAN_SEASON_ENTRY* pOut, int i32MaxCount);

	// Mercenary operations
	bool HireMercenary(IS_MERCENARY_HIRE_REQ* pReq, IS_MERCENARY_HIRE_ACK* pAck);
	int  LoadMercenaries(int i32ClanId, IS_MERCENARY_ENTRY* pOut, int i32MaxCount);
	bool DismissMercenary(int i32ClanId, int64_t i64UID);
	bool UpdateMercenaryResult(IS_MERCENARY_RESULT_REQ* pReq);

	// Friend operations
	int  AddFriend(int64_t i64UID, int64_t i64FriendUID);		// 0=OK, 1=exists, 2=not_found
	bool RemoveFriend(int64_t i64UID, int64_t i64FriendUID);
	int  LoadFriendList(int64_t i64UID, IS_FRIEND_ENTRY* pOut, int i32MaxCount);

	// Block operations
	int  AddBlock(int64_t i64UID, int64_t i64BlockedUID);		// 0=OK, 1=exists, 2=not_found
	bool RemoveBlock(int64_t i64UID, int64_t i64BlockedUID);
	int  LoadBlockList(int64_t i64UID, IS_BLOCK_ENTRY* pOut, int i32MaxCount);

	// Note/Mail operations
	bool SaveNote(IS_NOTE_SEND_REQ* pReq);
	int  LoadNotes(int64_t i64UID, IS_NOTE_ENTRY* pOut, int i32MaxCount);
	bool DeleteNote(int64_t i64UID, int64_t i64NoteId);

	// Ban operations
	bool BanPlayer(IS_PLAYER_BAN_REQ* pReq);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBSOCIAL_H__
