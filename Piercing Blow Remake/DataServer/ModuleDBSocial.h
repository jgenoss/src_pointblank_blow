#ifndef __MODULEDBSOCIAL_H__
#define __MODULEDBSOCIAL_H__

#pragma once
#include "InterServerProtocol.h"

class DBConnectionPool;
class DataServerContext;
class DataSession;

// Modulo de operaciones sociales de DB
// Maneja: clans, friends, blocks
// Patron sincronico v1 (sin i3Thread)
class ModuleDBSocial
{
public:
	ModuleDBSocial(DBConnectionPool* pPool);
	~ModuleDBSocial();

	// Clan operations
	int  CreateClan(IS_CLAN_CREATE_REQ* pReq);			// Returns clan_id, 0 on error
	bool DisbandClan(int i32ClanId, int64_t i64MasterUID);
	bool JoinClan(IS_CLAN_JOIN_REQ* pReq);
	bool LeaveClan(int i32ClanId, int64_t i64UID);

	// Friend operations
	int  AddFriend(int64_t i64UID, int64_t i64FriendUID);		// 0=OK, 1=exists, 2=not_found
	bool RemoveFriend(int64_t i64UID, int64_t i64FriendUID);
	int  LoadFriendList(int64_t i64UID, IS_FRIEND_ENTRY* pOut, int i32MaxCount);

	// Block operations
	int  AddBlock(int64_t i64UID, int64_t i64BlockedUID);		// 0=OK, 1=exists, 2=not_found
	bool RemoveBlock(int64_t i64UID, int64_t i64BlockedUID);
	int  LoadBlockList(int64_t i64UID, IS_BLOCK_ENTRY* pOut, int i32MaxCount);

	// Placeholder para futuro async
	void ProcessResponses(DataServerContext* pContext);

private:
	DBConnectionPool*	m_pPool;
};

#endif // __MODULEDBSOCIAL_H__
