#include "ModuleDBSocial.h"
#include "DBConnectionPool.h"
#include <cstdio>
#include <cstring>

ModuleDBSocial::ModuleDBSocial(DBConnectionPool* pPool)
	: m_pPool(pPool)
{
}

ModuleDBSocial::~ModuleDBSocial()
{
}

// ============================================================================
// Clan Operations
// ============================================================================

int ModuleDBSocial::CreateClan(IS_CLAN_CREATE_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	if (!pConn->BeginTransaction())
	{
		m_pPool->ReleaseConnection(pConn);
		return 0;
	}

	char szMasterUID[32];
	snprintf(szMasterUID, sizeof(szMasterUID), "%lld", pReq->i64MasterUID);

	const char* paramValues[3] = { pReq->szClanName, szMasterUID, pReq->szMasterNickname };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_clans (name, master_uid, master_nickname) "
		"VALUES ($1, $2, $3) RETURNING clan_id",
		3, paramValues);

	if (!result.IsSuccess() || result.GetRowCount() == 0)
	{
		pConn->RollbackTransaction();
		m_pPool->ReleaseConnection(pConn);
		printf("[ModuleDBSocial] ERROR: CreateClan failed for '%s'\n", pReq->szClanName);
		return 0;
	}

	int i32ClanId = result.GetInt32(0, 0);

	// Add master as member
	char szClanId[16], szLevel[8];
	snprintf(szClanId, sizeof(szClanId), "%d", i32ClanId);
	snprintf(szLevel, sizeof(szLevel), "%d", 1);	// CLAN_MEMBER_MASTER

	const char* memberParams[3] = { szClanId, szMasterUID, szLevel };
	DBResult memberResult = pConn->ExecuteParams(
		"INSERT INTO pb_clan_members (clan_id, uid, member_level) VALUES ($1, $2, $3)",
		3, memberParams);

	if (!memberResult.IsSuccess())
	{
		pConn->RollbackTransaction();
		m_pPool->ReleaseConnection(pConn);
		return 0;
	}

	// Update user's clan_id
	const char* updateParams[2] = { szClanId, szMasterUID };
	pConn->ExecuteParams(
		"UPDATE pb_users SET clan_id = $1 WHERE uid = $2",
		2, updateParams);

	pConn->CommitTransaction();
	m_pPool->ReleaseConnection(pConn);

	printf("[ModuleDBSocial] Clan created: id=%d name='%s' master=%lld\n",
		i32ClanId, pReq->szClanName, pReq->i64MasterUID);
	return i32ClanId;
}

bool ModuleDBSocial::DisbandClan(int i32ClanId, int64_t i64MasterUID)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	if (!pConn->BeginTransaction())
	{
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	char szClanId[16], szMasterUID[32];
	snprintf(szClanId, sizeof(szClanId), "%d", i32ClanId);
	snprintf(szMasterUID, sizeof(szMasterUID), "%lld", i64MasterUID);

	// Clear clan_id for all members
	const char* clearParams[1] = { szClanId };
	pConn->ExecuteParams(
		"UPDATE pb_users SET clan_id = 0 WHERE uid IN "
		"(SELECT uid FROM pb_clan_members WHERE clan_id = $1)",
		1, clearParams);

	// Delete members
	pConn->ExecuteParams(
		"DELETE FROM pb_clan_members WHERE clan_id = $1",
		1, clearParams);

	// Mark clan as inactive
	const char* disbandParams[2] = { szClanId, szMasterUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_clans SET is_active = FALSE WHERE clan_id = $1 AND master_uid = $2",
		2, disbandParams);

	if (!result.IsSuccess())
	{
		pConn->RollbackTransaction();
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	pConn->CommitTransaction();
	m_pPool->ReleaseConnection(pConn);

	printf("[ModuleDBSocial] Clan disbanded: id=%d\n", i32ClanId);
	return true;
}

bool ModuleDBSocial::JoinClan(IS_CLAN_JOIN_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	if (!pConn->BeginTransaction())
	{
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	char szClanId[16], szUID[32], szLevel[8];
	snprintf(szClanId, sizeof(szClanId), "%d", pReq->i32ClanId);
	snprintf(szUID, sizeof(szUID), "%lld", pReq->i64UID);
	snprintf(szLevel, sizeof(szLevel), "%d", pReq->ui8MemberLevel);

	// Insert member
	const char* memberParams[3] = { szClanId, szUID, szLevel };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_clan_members (clan_id, uid, member_level) VALUES ($1, $2, $3) "
		"ON CONFLICT DO NOTHING",
		3, memberParams);

	if (!result.IsSuccess())
	{
		pConn->RollbackTransaction();
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	// Update user's clan_id
	const char* updateParams[2] = { szClanId, szUID };
	pConn->ExecuteParams(
		"UPDATE pb_users SET clan_id = $1 WHERE uid = $2",
		2, updateParams);

	// Increment member count
	pConn->ExecuteParams(
		"UPDATE pb_clans SET member_count = member_count + 1 WHERE clan_id = $1",
		1, &szClanId);

	pConn->CommitTransaction();
	m_pPool->ReleaseConnection(pConn);

	printf("[ModuleDBSocial] Clan join: clan=%d uid=%lld\n", pReq->i32ClanId, pReq->i64UID);
	return true;
}

bool ModuleDBSocial::LeaveClan(int i32ClanId, int64_t i64UID)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	if (!pConn->BeginTransaction())
	{
		m_pPool->ReleaseConnection(pConn);
		return false;
	}

	char szClanId[16], szUID[32];
	snprintf(szClanId, sizeof(szClanId), "%d", i32ClanId);
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	// Remove member
	const char* delParams[2] = { szClanId, szUID };
	pConn->ExecuteParams(
		"DELETE FROM pb_clan_members WHERE clan_id = $1 AND uid = $2",
		2, delParams);

	// Clear user's clan_id
	const char* clearParams[1] = { szUID };
	pConn->ExecuteParams(
		"UPDATE pb_users SET clan_id = 0 WHERE uid = $1",
		1, clearParams);

	// Decrement member count
	const char* decParams[1] = { szClanId };
	pConn->ExecuteParams(
		"UPDATE pb_clans SET member_count = GREATEST(member_count - 1, 0) WHERE clan_id = $1",
		1, decParams);

	pConn->CommitTransaction();
	m_pPool->ReleaseConnection(pConn);

	printf("[ModuleDBSocial] Clan leave: clan=%d uid=%lld\n", i32ClanId, i64UID);
	return true;
}

// ============================================================================
// Friend Operations
// ============================================================================

int ModuleDBSocial::AddFriend(int64_t i64UID, int64_t i64FriendUID)
{
	if (!m_pPool)
		return 2;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 2;

	// Check friend exists
	char szFriendUID[32];
	snprintf(szFriendUID, sizeof(szFriendUID), "%lld", i64FriendUID);

	const char* checkParams[1] = { szFriendUID };
	DBResult checkResult = pConn->ExecuteParams(
		"SELECT uid FROM pb_users WHERE uid = $1",
		1, checkParams);

	if (!checkResult.IsSuccess() || checkResult.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		return 2;	// Not found
	}

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* insertParams[2] = { szUID, szFriendUID };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_friends (uid, friend_uid, accepted) VALUES ($1, $2, TRUE) "
		"ON CONFLICT DO NOTHING",
		2, insertParams);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 2;

	return 0;
}

bool ModuleDBSocial::RemoveFriend(int64_t i64UID, int64_t i64FriendUID)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szFriendUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szFriendUID, sizeof(szFriendUID), "%lld", i64FriendUID);

	// Remove both directions
	const char* params[2] = { szUID, szFriendUID };
	pConn->ExecuteParams(
		"DELETE FROM pb_friends WHERE (uid = $1 AND friend_uid = $2) "
		"OR (uid = $2 AND friend_uid = $1)",
		2, params);

	m_pPool->ReleaseConnection(pConn);
	return true;
}

int ModuleDBSocial::LoadFriendList(int64_t i64UID, IS_FRIEND_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT f.friend_uid, u.nickname, u.level, f.accepted "
		"FROM pb_friends f "
		"JOIN pb_users u ON u.uid = f.friend_uid "
		"WHERE f.uid = $1 "
		"ORDER BY f.created_at",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int i32Count = result.GetRowCount();
	if (i32Count > i32MaxCount)
		i32Count = i32MaxCount;

	for (int i = 0; i < i32Count; i++)
	{
		pOut[i].i64FriendUID = result.GetInt64(i, 0);
		const char* pszNick = result.GetString(i, 1);
		if (pszNick)
			strncpy(pOut[i].szNickname, pszNick, sizeof(pOut[i].szNickname) - 1);
		pOut[i].i32Level = result.GetInt32(i, 2);
		pOut[i].bAccepted = result.GetBool(i, 3);
	}

	return i32Count;
}

// ============================================================================
// Block Operations
// ============================================================================

int ModuleDBSocial::AddBlock(int64_t i64UID, int64_t i64BlockedUID)
{
	if (!m_pPool)
		return 2;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 2;

	// Check blocked user exists
	char szBlockedUID[32];
	snprintf(szBlockedUID, sizeof(szBlockedUID), "%lld", i64BlockedUID);

	const char* checkParams[1] = { szBlockedUID };
	DBResult checkResult = pConn->ExecuteParams(
		"SELECT uid FROM pb_users WHERE uid = $1",
		1, checkParams);

	if (!checkResult.IsSuccess() || checkResult.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		return 2;	// Not found
	}

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* insertParams[2] = { szUID, szBlockedUID };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_blocks (uid, blocked_uid) VALUES ($1, $2) "
		"ON CONFLICT DO NOTHING",
		2, insertParams);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 2;

	return 0;
}

bool ModuleDBSocial::RemoveBlock(int64_t i64UID, int64_t i64BlockedUID)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szBlockedUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szBlockedUID, sizeof(szBlockedUID), "%lld", i64BlockedUID);

	const char* params[2] = { szUID, szBlockedUID };
	pConn->ExecuteParams(
		"DELETE FROM pb_blocks WHERE uid = $1 AND blocked_uid = $2",
		2, params);

	m_pPool->ReleaseConnection(pConn);
	return true;
}

int ModuleDBSocial::LoadBlockList(int64_t i64UID, IS_BLOCK_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT b.blocked_uid, u.nickname "
		"FROM pb_blocks b "
		"JOIN pb_users u ON u.uid = b.blocked_uid "
		"WHERE b.uid = $1 "
		"ORDER BY b.created_at",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int i32Count = result.GetRowCount();
	if (i32Count > i32MaxCount)
		i32Count = i32MaxCount;

	for (int i = 0; i < i32Count; i++)
	{
		pOut[i].i64BlockedUID = result.GetInt64(i, 0);
		const char* pszNick = result.GetString(i, 1);
		if (pszNick)
			strncpy(pOut[i].szNickname, pszNick, sizeof(pOut[i].szNickname) - 1);
	}

	return i32Count;
}

// ============================================================================
// Clan Load
// ============================================================================

bool ModuleDBSocial::LoadClan(int i32ClanId, IS_CLAN_LOAD_ACK* pOut,
							  IS_CLAN_MEMBER_INFO* pMembers, int i32MaxMembers, int* pMemberCount)
{
	if (!m_pPool || !pOut || !pMembers || !pMemberCount)
		return false;

	*pMemberCount = 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szClanId[16];
	snprintf(szClanId, sizeof(szClanId), "%d", i32ClanId);

	// Load clan info
	const char* clanParams[1] = { szClanId };
	DBResult clanResult = pConn->ExecuteParams(
		"SELECT name, notice, intro, master_uid, master_nickname, "
		"member_count, max_members, clan_exp, clan_rank, wins, losses, "
		"unit, mark_id, mark_color "
		"FROM pb_clans WHERE clan_id = $1 AND is_active = TRUE",
		1, clanParams);

	if (!clanResult.IsSuccess() || clanResult.GetRowCount() == 0)
	{
		m_pPool->ReleaseConnection(pConn);
		printf("[ModuleDBSocial] LoadClan: clan %d not found\n", i32ClanId);
		return false;
	}

	strncpy_s(pOut->szName, clanResult.GetValue(0, 0), _TRUNCATE);
	strncpy_s(pOut->szNotice, clanResult.GetValue(0, 1), _TRUNCATE);
	strncpy_s(pOut->szIntro, clanResult.GetValue(0, 2), _TRUNCATE);
	pOut->i64MasterUID = _atoi64(clanResult.GetValue(0, 3));
	strncpy_s(pOut->szMasterNickname, clanResult.GetValue(0, 4), _TRUNCATE);
	pOut->i32MemberCount = atoi(clanResult.GetValue(0, 5));
	pOut->i32MaxMembers = atoi(clanResult.GetValue(0, 6));
	pOut->i32ClanExp = atoi(clanResult.GetValue(0, 7));
	pOut->i32ClanRank = atoi(clanResult.GetValue(0, 8));
	pOut->i32Wins = atoi(clanResult.GetValue(0, 9));
	pOut->i32Losses = atoi(clanResult.GetValue(0, 10));
	pOut->ui8Unit = (uint8_t)atoi(clanResult.GetValue(0, 11));
	pOut->ui16MarkId = (uint16_t)atoi(clanResult.GetValue(0, 12));
	pOut->ui8MarkColor = (uint8_t)atoi(clanResult.GetValue(0, 13));

	// Load members
	DBResult memberResult = pConn->ExecuteParams(
		"SELECT cm.uid, u.nickname, cm.member_level "
		"FROM pb_clan_members cm "
		"JOIN pb_users u ON cm.uid = u.uid "
		"WHERE cm.clan_id = $1 ORDER BY cm.member_level ASC",
		1, clanParams);

	m_pPool->ReleaseConnection(pConn);

	if (memberResult.IsSuccess())
	{
		int count = 0;
		for (int i = 0; i < memberResult.GetRowCount() && count < i32MaxMembers; i++)
		{
			pMembers[count].i64UID = _atoi64(memberResult.GetValue(i, 0));
			strncpy_s(pMembers[count].szNickname, memberResult.GetValue(i, 1), _TRUNCATE);
			pMembers[count].ui8MemberLevel = (uint8_t)atoi(memberResult.GetValue(i, 2));
			count++;
		}
		*pMemberCount = count;
	}

	printf("[ModuleDBSocial] LoadClan: loaded clan %d '%s' with %d members\n",
		i32ClanId, pOut->szName, *pMemberCount);
	return true;
}

// ============================================================================
// Note/Mail Operations
// ============================================================================

bool ModuleDBSocial::SaveNote(IS_NOTE_SEND_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szSenderUID[32], szReceiverUID[32], szType[8];
	snprintf(szSenderUID, sizeof(szSenderUID), "%lld", pReq->i64SenderUID);
	snprintf(szReceiverUID, sizeof(szReceiverUID), "%lld", pReq->i64ReceiverUID);
	snprintf(szType, sizeof(szType), "%d", pReq->ui8Type);

	const char* params[6] = { szSenderUID, szReceiverUID, pReq->szSenderNick,
							   pReq->szSubject, pReq->szBody, szType };
	DBResult result = pConn->ExecuteParams(
		"INSERT INTO pb_notes (sender_uid, receiver_uid, sender_nick, subject, body, note_type) "
		"VALUES ($1, $2, $3, $4, $5, $6)",
		6, params);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

int ModuleDBSocial::LoadNotes(int64_t i64UID, IS_NOTE_ENTRY* pOut, int i32MaxCount)
{
	if (!m_pPool || !pOut)
		return 0;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return 0;

	char szUID[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);

	const char* params[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"SELECT id, sender_uid, sender_nick, subject, body, "
		"EXTRACT(EPOCH FROM created_at)::INTEGER, note_type, is_read "
		"FROM pb_notes WHERE receiver_uid = $1 "
		"ORDER BY created_at DESC LIMIT 50",
		1, params);

	m_pPool->ReleaseConnection(pConn);

	if (!result.IsSuccess())
		return 0;

	int i32Count = result.GetRowCount();
	if (i32Count > i32MaxCount)
		i32Count = i32MaxCount;

	for (int i = 0; i < i32Count; i++)
	{
		memset(&pOut[i], 0, sizeof(IS_NOTE_ENTRY));
		pOut[i].i64NoteId = result.GetInt64(i, 0);
		pOut[i].i64SenderUID = result.GetInt64(i, 1);
		const char* pszNick = result.GetString(i, 2);
		if (pszNick)
			strncpy(pOut[i].szSenderNick, pszNick, sizeof(pOut[i].szSenderNick) - 1);
		const char* pszSubject = result.GetString(i, 3);
		if (pszSubject)
			strncpy(pOut[i].szSubject, pszSubject, sizeof(pOut[i].szSubject) - 1);
		const char* pszBody = result.GetString(i, 4);
		if (pszBody)
			strncpy(pOut[i].szBody, pszBody, sizeof(pOut[i].szBody) - 1);
		pOut[i].ui32Timestamp = (uint32_t)result.GetInt32(i, 5);
		pOut[i].ui8Type = (uint8_t)result.GetInt32(i, 6);
		pOut[i].ui8Read = result.GetBool(i, 7) ? 1 : 0;
	}

	return i32Count;
}

bool ModuleDBSocial::DeleteNote(int64_t i64UID, int64_t i64NoteId)
{
	if (!m_pPool)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szNoteId[32];
	snprintf(szUID, sizeof(szUID), "%lld", i64UID);
	snprintf(szNoteId, sizeof(szNoteId), "%lld", i64NoteId);

	const char* params[2] = { szNoteId, szUID };
	DBResult result = pConn->ExecuteParams(
		"DELETE FROM pb_notes WHERE id = $1 AND receiver_uid = $2",
		2, params);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

// ============================================================================
// Ban Operations
// ============================================================================

bool ModuleDBSocial::BanPlayer(IS_PLAYER_BAN_REQ* pReq)
{
	if (!m_pPool || !pReq)
		return false;

	DBConnection* pConn = m_pPool->AcquireConnection();
	if (!pConn)
		return false;

	char szUID[32], szBannedByUID[32], szDuration[32];
	snprintf(szUID, sizeof(szUID), "%lld", pReq->i64UID);
	snprintf(szBannedByUID, sizeof(szBannedByUID), "%lld", pReq->i64BannedByUID);
	snprintf(szDuration, sizeof(szDuration), "%d", pReq->i32Duration);

	// Insert ban record
	const char* params1[4] = { szUID, pReq->szReason, szDuration, szBannedByUID };
	pConn->ExecuteParams(
		"INSERT INTO pb_ban_list (uid, reason, duration, banned_by_uid) "
		"VALUES ($1, $2, $3, $4)",
		4, params1);

	// Update user banned flag
	const char* params2[1] = { szUID };
	DBResult result = pConn->ExecuteParams(
		"UPDATE pb_users SET is_banned = TRUE WHERE uid = $1",
		1, params2);

	m_pPool->ReleaseConnection(pConn);
	return result.IsSuccess();
}

void ModuleDBSocial::ProcessResponses(DataServerContext* pContext)
{
	// Placeholder para futuro async con ring buffers
}
