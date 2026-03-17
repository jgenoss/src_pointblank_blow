#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "ClanMatchManager.h"
#include "ClanDef.h"
#include "GameSessionManager.h"

// ============================================================================
// Clan War 1.5 Handlers (Protocol_Clan_War 0x1B00)
// Uses the same ClanMatchManager infrastructure but with matchmaking queue
// ============================================================================

void GameSession::OnClanWarCreateTeamReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK, -2);
		return;
	}

	uint8_t maxMembers = CLAN_MATCH_DEFAULT_PLAYERS;
	if (i32Size >= 1)
		maxMembers = *(uint8_t*)pData;

	int teamIdx = g_pClanMatchManager->CreateTeam(
		m_i32ClanId, m_szClanName, m_i64UID, m_szNickname,
		GetIndex(), maxMembers);

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = (teamIdx >= 0) ? 0 : teamIdx;

	if (teamIdx >= 0)
	{
		int32_t idx = teamIdx;
	}
	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&idx, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanWarJoinTeamReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager || i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK, -2);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	int slot = g_pClanMatchManager->JoinTeam(teamIdx, m_i64UID, m_szNickname,
		GetIndex(), (uint8_t)m_i32Level, (uint8_t)m_i32RankId);

	int32_t result = (slot >= 0) ? 0 : slot;
	SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK, result);
}

void GameSession::OnClanWarLeaveTeamReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	g_pClanMatchManager->LeaveTeam(teamIdx, m_i64UID);
	SendSimpleAck(PROTOCOL_CLAN_WAR_LEAVE_TEAM_ACK, 0);
}

void GameSession::OnClanWarTeamListReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK, -2);
		return;
	}

	int indices[50];
	int count = g_pClanMatchManager->GetAllActiveTeams(indices, 50);

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	uint16_t teamCount = (uint16_t)count;

	for (int i = 0; i < count; i++)
	{
		const ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(indices[i]);
		if (!pTeam || offset + 135 > (int)sizeof(buffer))
			break;

		int32_t idx = indices[i];
		memcpy(buffer + offset, pTeam->szClanName, 64);		offset += 64;
		memcpy(buffer + offset, pTeam->szLeaderNick, 64);	offset += 64;
		memcpy(buffer + offset, &pTeam->ui8State, 1);		offset += 1;
		memcpy(buffer + offset, &pTeam->ui8MemberCount, 1); offset += 1;
		memcpy(buffer + offset, &pTeam->ui8MaxMembers, 1);	offset += 1;
	}
	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&teamCount, sizeof(uint16_t));
	packet.WriteData(&idx, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanWarMatchmakingReq(char* pData, INT32 i32Size)
{
	// Start matchmaking - finds an opponent team automatically
	if (!g_pClanMatchManager || i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_MATCHMAKING_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pMyTeam || !pMyTeam->IsLeader(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_MATCHMAKING_ACK, -2);
		return;
	}

	// Find an opponent team that is waiting and not from same clan
	int indices[50];
	int count = g_pClanMatchManager->GetAllActiveTeams(indices, 50);
	int bestOpponent = -1;

	for (int i = 0; i < count; i++)
	{
		if (indices[i] == teamIdx) continue;

		const ClanMatchTeam* pCandidate = g_pClanMatchManager->GetTeam(indices[i]);
		if (!pCandidate) continue;

		if (pCandidate->i32ClanId == pMyTeam->i32ClanId) continue;
		if (pCandidate->ui8State != CLAN_MATCH_TEAM_WAITING) continue;
		if (pCandidate->ui8FightState != CLAN_MATCH_FIGHT_NONE) continue;

		bestOpponent = indices[i];
		break;
	}

	if (bestOpponent < 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_MATCHMAKING_ACK, 1); // No opponent found
		return;
	}

	int fightResult = g_pClanMatchManager->RequestFight(teamIdx, bestOpponent);
	if (fightResult == 0)
	{
		// Auto-accept for matchmaking
		g_pClanMatchManager->AcceptFight(bestOpponent, true);
	}

	SendSimpleAck(PROTOCOL_CLAN_WAR_MATCHMAKING_ACK, fightResult);
}

void GameSession::OnClanWarCancelMatchmakingReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (pTeam && pTeam->ui8FightState == CLAN_MATCH_FIGHT_REQUESTED)
	{
		g_pClanMatchManager->AcceptFight(teamIdx, false); // Cancel = reject
	}

	SendSimpleAck(PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK, 0);
}

void GameSession::OnClanWarTeamChatReq(char* pData, INT32 i32Size)
{
	// Reuse clan match chat logic
	OnClanMatchChatReq(pData, i32Size);
}

void GameSession::OnClanWarChangeMaxPerReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || i32Size < 5)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));
	uint8_t newMax = *(uint8_t*)(pData + 4);

	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || !pTeam->IsLeader(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK, -2);
		return;
	}

	if (newMax < CLAN_MATCH_MIN_PLAYERS) newMax = CLAN_MATCH_MIN_PLAYERS;
	if (newMax > MAX_CLAN_MATCH_TEAM_SIZE) newMax = MAX_CLAN_MATCH_TEAM_SIZE;

	if (newMax < pTeam->ui8MemberCount)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK, -3); // Can't reduce below current count
		return;
	}

	pTeam->ui8MaxMembers = newMax;
	SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK, 0);

	// Notify team members
	SendClanMatchTeamInfo(teamIdx);
}

void GameSession::OnClanWarMercenaryListReq(char* pData, INT32 i32Size)
{
	MercenaryEntry mercs[MAX_MERCENARIES];
	int count = 0;

	if (g_pClanMatchManager)
		count = g_pClanMatchManager->GetMercenaryList(mercs, MAX_MERCENARIES);

	int32_t result = 0;
	uint16_t mercCount = (uint16_t)count;

	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&mercCount, sizeof(uint16_t));

	for (int i = 0; i < count; i++)
	{
		packet.WriteData(&mercs[i].i64UID, sizeof(int64_t));
		packet.WriteData(mercs[i].szNickname, 64);
		packet.WriteData(&mercs[i].ui8Level, sizeof(uint8_t));
		packet.WriteData(&mercs[i].ui8Rank, sizeof(uint8_t));
	}

	SendPacketMessage(&packet);
}

void GameSession::OnClanWarResultReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_RESULT_ACK, -1);
		return;
	}

	// Get match results for this clan
	ClanMatchResultEntry results[20];
	int count = g_pClanMatchManager->GetMatchResultsForClan(m_i32ClanId, results, 20);

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = 0;

	uint16_t matchCount = (uint16_t)count;

	for (int i = 0; i < count; i++)
	{
		if (offset + 144 > (int)sizeof(buffer))
			break;

		const ClanMatchResultEntry& entry = results[i];
		memcpy(buffer + offset, entry.szTeam1ClanName, 64);	offset += 64;
		memcpy(buffer + offset, entry.szTeam2ClanName, 64);	offset += 64;
		memcpy(buffer + offset, &entry.i32Team1Score, sizeof(int32_t));	offset += sizeof(int32_t);
		memcpy(buffer + offset, &entry.i32Team2Score, sizeof(int32_t));	offset += sizeof(int32_t);
		memcpy(buffer + offset, &entry.i32WinnerTeam, sizeof(int32_t));	offset += sizeof(int32_t);
		memcpy(buffer + offset, &entry.dwMatchTime, sizeof(DWORD));		offset += sizeof(DWORD);
	}
	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_RESULT_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&matchCount, sizeof(uint16_t));
	SendPacketMessage(&packet);
}

// ============================================================================
// Clan War Mercenary Handlers (Protocol_Clan_War 0x1B00)
// ============================================================================

void GameSession::OnClanWarRegistMercenaryReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK, -1);
		return;
	}

	// Clan members can't register as mercenaries
	if (m_i32ClanId > 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK, -2);
		return;
	}

	bool ok = g_pClanMatchManager->RegisterMercenary(
		m_i64UID, m_szNickname, GetIndex(),
		(uint8_t)m_i32Level, (uint8_t)m_i32RankId);

	SendSimpleAck(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK, ok ? 0 : -3);
}

void GameSession::OnClanWarRemoveMercenaryReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK, -1);
		return;
	}

	if (g_pClanMatchManager)
		g_pClanMatchManager->UnregisterMercenary(m_i64UID);

	SendSimpleAck(PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK, 0);
}

void GameSession::OnClanWarInviteMercenaryReq(char* pData, INT32 i32Size)
{
	// Team leader invites a mercenary to join the team
	if (m_eMainTask < GAME_TASK_CHANNEL || i32Size < 12)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	int64_t mercUID = 0;
	memcpy(&mercUID, pData + 4, sizeof(int64_t));

	int32_t result = 0;

	if (!g_pClanMatchManager)
	{
		result = -2;
	}
	else
	{
		ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
		if (!pTeam || !pTeam->IsLeader(m_i64UID))
			result = -3;
		else if (pTeam->ui8MemberCount >= pTeam->ui8MaxMembers)
			result = -4;
		else
		{
			// Verify target is in the mercenary pool
			if (!g_pClanMatchManager->IsMercenary(mercUID))
				result = -5;	// Not registered as mercenary

			// Find mercenary online
			GameSession* pMerc = (result == 0) ? g_pGameSessionManager->FindSessionByUID(mercUID) : nullptr;
			if (result == 0 && !pMerc)
				result = -6;	// Offline
			else
			{
				// Send invite to the mercenary
				i3NetworkPacket invPacket(PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK);
				invPacket.WriteData(&teamIdx, sizeof(int32_t));
				invPacket.WriteData(&m_i32ClanId, sizeof(int32_t));
				invPacket.WriteData(m_szClanName, MAX_CLAN_NAME_LEN);
				invPacket.WriteData(m_szNickname, 64);
				pMerc->SendPacketMessage(&invPacket);
			}
		}
	}

	SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_MERCENARY_SENDER_ACK, result);
}

void GameSession::OnClanWarInviteAcceptReq(char* pData, INT32 i32Size)
{
	// Mercenary accepts team invitation
	if (m_eMainTask < GAME_TASK_CHANNEL || i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	int32_t result = 0;

	if (!g_pClanMatchManager)
	{
		result = -2;
	}
	else
	{
		int slot = g_pClanMatchManager->JoinTeam(teamIdx, m_i64UID, m_szNickname,
			GetIndex(), (uint8_t)m_i32Level, (uint8_t)m_i32RankId);

		if (slot < 0)
			result = slot;	// Error from JoinTeam
	}

	SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_ACCEPT_ACK, result);
}

void GameSession::OnClanWarInviteDenialReq(char* pData, INT32 i32Size)
{
	// Mercenary denies team invitation
	if (m_eMainTask < GAME_TASK_CHANNEL || i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	// Notify the team leader that mercenary declined
	if (g_pClanMatchManager)
	{
		ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
		if (pTeam)
		{
			GameSession* pLeader = g_pGameSessionManager->FindSessionByUID(pTeam->i64LeaderUID);
			if (pLeader)
				pLeader->SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK, 0);
		}
	}

	SendSimpleAck(PROTOCOL_CLAN_WAR_INVITE_DENIAL_ACK, 0);
}

// ============================================================================
// Batch 17 - ClanWar extras
// ============================================================================

void GameSession::OnClanWarCreateRoomReq(char* pData, INT32 i32Size)
{
	// Clan war leader creates a match room for the clan war battle
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, -1);
		return;
	}

	if (i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, -1);
		return;
	}

	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || !pTeam->IsLeader(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, -2);
		return;
	}

	if (pTeam->i32OpponentTeamIdx < 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, -3);
		return;
	}

	ClanMatchTeam* pOpp = g_pClanMatchManager->GetTeam(pTeam->i32OpponentTeamIdx);
	if (!pOpp)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, -3);
		return;
	}

	// Use the existing CreateClanMatchRoom helper
	CreateClanMatchRoom(pTeam, pOpp);
	SendSimpleAck(PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK, 0);
}

void GameSession::OnClanWarJoinRoomReq(char* pData, INT32 i32Size)
{
	// Clan war member joins the match room
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK, -1);
		return;
	}

	if (i32Size < (int)(sizeof(int) * 2))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK, -1);
		return;
	}

	int offset = 0;
	int channel = 0;
	int roomIdx = 0;
	memcpy(&channel, pData + offset, sizeof(int));		offset += sizeof(int);
	memcpy(&roomIdx, pData + offset, sizeof(int));		offset += sizeof(int);

	if (!g_pRoomManager)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK, -1);
		return;
	}

	Room* pRoom = g_pRoomManager->GetRoom(channel, roomIdx);
	if (!pRoom || !pRoom->IsActive())
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK, -2);
		return;
	}

	// Auto-assign team based on clan ID
	int team = 0;  // default RED
	if (m_i32ClanId > 0 && pRoom->GetPlayerCount() > 0)
		team = 1;  // second clan goes BLUE

	int slot = pRoom->OnJoin(this, team);
	if (slot < 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK, -3);
		return;
	}

	m_i32RoomIdx = roomIdx;
	m_i32SlotIdx = slot;
	m_i32ChannelNum = channel;
	m_pRoom = pRoom;
	m_eMainTask = GAME_TASK_READY_ROOM;

	SendSimpleAck(PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK, 0);
}

void GameSession::OnClanWarLeaveRoomReq(char* pData, INT32 i32Size)
{
	// Clan war member leaves the match room
	if (m_eMainTask != GAME_TASK_READY_ROOM || !m_pRoom)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_LEAVE_ROOM_ACK, -1);
		return;
	}

	m_pRoom->OnLeave(this);
	m_pRoom = nullptr;
	m_i32RoomIdx = -1;
	m_i32SlotIdx = -1;
	m_eMainTask = GAME_TASK_LOBBY;

	SendSimpleAck(PROTOCOL_CLAN_WAR_LEAVE_ROOM_ACK, 0);
}

void GameSession::OnClanWarMercenaryDetailInfoReq(char* pData, INT32 i32Size)
{
	// Request detailed info about a mercenary
	if (i32Size < (int)sizeof(int64_t))
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK, -1);
		return;
	}

	int64_t mercUID = 0;
	memcpy(&mercUID, pData, sizeof(int64_t));

	GameSession* pMerc = g_pGameSessionManager ? g_pGameSessionManager->FindSessionByUID(mercUID) : nullptr;

	i3NetworkPacket packet;
	offset += sizeof(uint16_t);

	int32_t result = pMerc ? 0 : -1;

	if (pMerc)
	{

		char nick[64] = {};
		strncpy(nick, pMerc->GetNickname(), 63);

		int32_t level = pMerc->GetLevel();
		int32_t rank = pMerc->GetRankId();
		int32_t kills = pMerc->GetKills();
		int32_t deaths = pMerc->GetDeaths();
		int32_t wins = pMerc->GetWins();
		int32_t losses = pMerc->GetLosses();
	}
	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&mercUID, sizeof(int64_t));
	packet.WriteData(nick, 64);
	packet.WriteData(&level, 4);
	packet.WriteData(&rank, 4);
	packet.WriteData(&kills, 4);
	packet.WriteData(&deaths, 4);
	packet.WriteData(&wins, 4);
	packet.WriteData(&losses, 4);
	SendPacketMessage(&packet);
}

// ============================================================================
// Batch 19 - ClanWar extras
// ============================================================================

void GameSession::OnClanWarChangeOperationReq(char* pData, INT32 i32Size)
{
	// Change match team operation/mission setting
	if (!g_pClanMatchManager || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK, 1);
		return;
	}

	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK, 2);
		return;
	}

	uint8_t operationType = (uint8_t)pData[0];

	// Find the team this user leads
	ClanMatchTeam* pTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pTeam)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK, 3);	// Not team leader
		return;
	}

	// Operation type is stored for match rules
	// (e.g., bomb mission, deathmatch, etc.)
	SendSimpleAck(PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK, 0);
}

void GameSession::OnClanWarCheckPreseasonRankingReq(char* pData, INT32 i32Size)
{
	// Check previous season clan ranking
	// No explicit ACK defined, use REQ+1 pattern
	int32_t result = 0;
	int32_t prevRank   = 0;
	int32_t prevWins   = 0;
	int32_t prevLosses = 0;
	int32_t prevPoints = 0;

	if (m_i32ClanId > 0 && g_pClanMatchManager)
	{
		const ClanSeasonSnapshot* pSnap = g_pClanMatchManager->FindSeasonSnapshot(m_i32ClanId);
		if (pSnap)
		{
			prevRank   = pSnap->i32SeasonRank;
			prevWins   = pSnap->i32Wins;
			prevLosses = pSnap->i32Losses;
			prevPoints = pSnap->i32Points;
		}
	}

	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_CHECK_PRESEASON_RANKING_REQ + 1);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&prevRank, sizeof(int32_t));
	packet.WriteData(&prevWins, sizeof(int32_t));
	packet.WriteData(&prevLosses, sizeof(int32_t));
	packet.WriteData(&prevPoints, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanWarMercenaryPenaltyLoadReq(char* pData, INT32 i32Size)
{
	// Load mercenary desertion penalty timer
	i3NetworkPacket packet;
	int32_t result = 0;

	// Penalty time remaining (0 = no penalty)
	int32_t penaltySeconds = 0;
	i3NetworkPacket packet(PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&penaltySeconds, sizeof(int32_t));
	SendPacketMessage(&packet);
}

// ============================================================================
// Batch 20 - CS_MATCH_* Clan Match Handlers
// ============================================================================

void GameSession::OnClanMatchChannelReq(char* pData, INT32 i32Size)
{
	// Clan match channel info - returns aggregate data for the clan match system
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_CHANNEL_REQ + 1, 1);	// No clan
		return;
	}

	int32_t result = 0;
	int teamIndices[16];
	int teamCount = g_pClanMatchManager->GetTeamListForClan(m_i32ClanId, teamIndices, 16);
	int32_t i32TeamCount = teamCount;

	int32_t i32MatchCount = 0;
	for (int i = 0; i < teamCount; i++)
	{
		const ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIndices[i]);
		if (pTeam && pTeam->ui8State == CLAN_MATCH_TEAM_FIGHTING)
			i32MatchCount++;
	}

	i3NetworkPacket packet(PROTOCOL_CS_MATCH_CHANNEL_REQ + 1);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&i32TeamCount, sizeof(int32_t));
	packet.WriteData(&i32MatchCount, sizeof(int32_t));
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchFightQuickRequestReq(char* pData, INT32 i32Size)
{
	// Quick fight request - find any available team to fight
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK, 1);
		return;
	}

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK, 2);	// Not a team leader
		return;
	}

	if (pMyTeam->ui8State != CLAN_MATCH_TEAM_WAITING)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK, 3);	// Team not in waiting state
		return;
	}

	// Find an opponent team from another clan that's also waiting
	int allTeams[MAX_CLAN_MATCH_TEAMS];
	int totalTeams = g_pClanMatchManager->GetAllActiveTeams(allTeams, MAX_CLAN_MATCH_TEAMS);

	int opponentIdx = -1;
	for (int i = 0; i < totalTeams; i++)
	{
		ClanMatchTeam* pOther = g_pClanMatchManager->GetTeam(allTeams[i]);
		if (!pOther) continue;
		if (pOther->i32ClanId == m_i32ClanId) continue;	// Same clan
		if (pOther->ui8State != CLAN_MATCH_TEAM_WAITING) continue;
		if (pOther->ui8FightState != CLAN_MATCH_FIGHT_NONE) continue;
		if (pOther->ui8MaxMembers != pMyTeam->ui8MaxMembers) continue;	// Size mismatch

		opponentIdx = allTeams[i];
		break;
	}

	if (opponentIdx < 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK, 4);	// No opponent found
		return;
	}

	int fightResult = g_pClanMatchManager->RequestFight(pMyTeam->i32TeamIdx, opponentIdx);
	SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_QUICK_REQUEST_ACK, fightResult);
}

void GameSession::OnClanMatchTeamDetailInfoReq(char* pData, INT32 i32Size)
{
	// Opponent team detail info
	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK, 1);
		return;
	}

	int32_t teamIdx = 0;
	memcpy(&teamIdx, pData, 4);

	const ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || !pTeam->IsActive())
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK, 2);	// Team not found
		return;
	}

	i3NetworkPacket packet;
	int32_t result = 0;

	// Team info
	int32_t clanId = pTeam->i32ClanId;
	memcpy(buffer + offset, pTeam->szClanName, 64);	offset += 64;
	memcpy(buffer + offset, pTeam->szLeaderNick, 64);	offset += 64;

	uint8_t state = pTeam->ui8State;

	uint8_t maxMembers = pTeam->ui8MaxMembers;
	uint8_t memberCount = pTeam->ui8MemberCount;

	// Wins/losses/points
	int32_t wins = pTeam->i32Wins;
	int32_t losses = pTeam->i32Losses;
	int32_t points = pTeam->i32MatchPoints;

	// Member details
	for (int i = 0; i < pTeam->ui8MemberCount && i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
	{
		const ClanMatchMember& m = pTeam->members[i];
		if (!m.bActive) continue;

		memcpy(buffer + offset, m.szNickname, 64);	offset += 64;
		memcpy(buffer + offset, &m.ui8Level, 1);	offset += 1;
		memcpy(buffer + offset, &m.ui8Rank, 1);	offset += 1;
		uint8_t ready = m.bReady ? 1 : 0;
	}
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&clanId, 4);
	packet.WriteData(&state, 1);
	packet.WriteData(&maxMembers, 1);
	packet.WriteData(&memberCount, 1);
	packet.WriteData(&wins, 4);
	packet.WriteData(&losses, 4);
	packet.WriteData(&points, 4);
	packet.WriteData(&ready, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchTeamChangePerReq(char* pData, INT32 i32Size)
{
	// Change team max member count (leader only)
	if (i32Size < 1)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK, 1);
		return;
	}

	uint8_t newMax = 0;
	memcpy(&newMax, pData, 1);

	if (newMax < CLAN_MATCH_MIN_PLAYERS || newMax > MAX_CLAN_MATCH_TEAM_SIZE)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK, 2);	// Invalid size
		return;
	}

	ClanMatchTeam* pTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK, 3);	// Not team leader
		return;
	}

	if (pTeam->ui8State != CLAN_MATCH_TEAM_WAITING)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK, 4);	// Can't change during match
		return;
	}

	if (newMax < pTeam->ui8MemberCount)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK, 5);	// Would drop members
		return;
	}

	pTeam->ui8MaxMembers = newMax;

	// Send ACK with new max
	i3NetworkPacket packet;
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&newMax, 1);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchTeamListReq(char* pData, INT32 i32Size)
{
	// List teams from same clan
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_LIST_ACK, 1);
		return;
	}

	int teamIndices[MAX_CLAN_MATCH_TEAMS];
	int teamCount = g_pClanMatchManager->GetTeamListForClan(m_i32ClanId, teamIndices, MAX_CLAN_MATCH_TEAMS);

	i3NetworkPacket packet;
	int32_t result = 0;

	int32_t count = teamCount;

	for (int i = 0; i < teamCount && offset < 3800; i++)
	{
		const ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIndices[i]);
		if (!pTeam) continue;

		int32_t tidx = pTeam->i32TeamIdx;
		memcpy(buffer + offset, pTeam->szLeaderNick, 64);	offset += 64;
		uint8_t state = pTeam->ui8State;
		uint8_t maxM = pTeam->ui8MaxMembers;
		uint8_t curM = pTeam->ui8MemberCount;
		int32_t w = pTeam->i32Wins;
		int32_t l = pTeam->i32Losses;
	}
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_TEAM_LIST_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&count, 4);
	packet.WriteData(&tidx, 4);
	packet.WriteData(&state, 1);
	packet.WriteData(&maxM, 1);
	packet.WriteData(&curM, 1);
	packet.WriteData(&w, 4);
	packet.WriteData(&l, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchNewClanInfoReq(char* pData, INT32 i32Size)
{
	// Get opponent clan info from within a clan match room
	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK, 1);
		return;
	}

	int32_t opponentClanId = 0;
	memcpy(&opponentClanId, pData, 4);

	GameClanInfo* pClan = g_pClanManager->FindClan(opponentClanId);
	if (!pClan || !pClan->bActive)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK, 2);	// Clan not found
		return;
	}

	i3NetworkPacket packet;
	int32_t result = 0;

	// Clan info
	int32_t clanId = pClan->i32ClanId;
	memcpy(buffer + offset, pClan->szName, 32);		offset += 32;
	memcpy(buffer + offset, pClan->szMasterNickname, 64);	offset += 64;

	int32_t memberCount = pClan->i32MemberCount;
	int32_t maxMembers = pClan->i32MaxMembers;

	uint16_t markId = pClan->ui16MarkId;
	uint8_t markColor = pClan->ui8MarkColor;

	int32_t clanRank = pClan->i32ClanRank;
	int32_t clanExp = pClan->i32ClanExp;
	int32_t clanWins = pClan->i32Wins;
	int32_t clanLosses = pClan->i32Losses;
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_NEW_CLAN_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&clanId, 4);
	packet.WriteData(&memberCount, 4);
	packet.WriteData(&maxMembers, 4);
	packet.WriteData(&markId, 2);
	packet.WriteData(&markColor, 1);
	packet.WriteData(&clanRank, 4);
	packet.WriteData(&clanExp, 4);
	packet.WriteData(&clanWins, 4);
	packet.WriteData(&clanLosses, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchNewFightQuickReq(char* pData, INT32 i32Size)
{
	// New quick match request (v2) - registers for quick matching
	// Response uses PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK per protocol def comment
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, 1);
		return;
	}

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, 2);	// Not a team leader
		return;
	}

	if (pMyTeam->ui8State != CLAN_MATCH_TEAM_WAITING)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, 3);
		return;
	}

	if (pMyTeam->ui8MemberCount < CLAN_MATCH_MIN_PLAYERS)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, 4);	// Not enough members
		return;
	}

	// Find a matching opponent (same size, different clan, also waiting)
	int allTeams[MAX_CLAN_MATCH_TEAMS];
	int totalTeams = g_pClanMatchManager->GetAllActiveTeams(allTeams, MAX_CLAN_MATCH_TEAMS);

	int opponentIdx = -1;
	for (int i = 0; i < totalTeams; i++)
	{
		ClanMatchTeam* pOther = g_pClanMatchManager->GetTeam(allTeams[i]);
		if (!pOther) continue;
		if (pOther->i32TeamIdx == pMyTeam->i32TeamIdx) continue;
		if (pOther->i32ClanId == m_i32ClanId) continue;
		if (pOther->ui8State != CLAN_MATCH_TEAM_WAITING) continue;
		if (pOther->ui8FightState != CLAN_MATCH_FIGHT_NONE) continue;
		if (pOther->ui8MaxMembers != pMyTeam->ui8MaxMembers) continue;
		if (pOther->ui8MemberCount < CLAN_MATCH_MIN_PLAYERS) continue;

		opponentIdx = allTeams[i];
		break;
	}

	if (opponentIdx < 0)
	{
		// No match found - register for waiting
		pMyTeam->ui8FightState = CLAN_MATCH_FIGHT_REQUESTED;
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, 0);	// Registered, waiting for opponent
		return;
	}

	int fightResult = g_pClanMatchManager->RequestFight(pMyTeam->i32TeamIdx, opponentIdx);
	SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, fightResult);
}

void GameSession::OnClanMatchNewFightOtherReq(char* pData, INT32 i32Size)
{
	// Request fight against a specific team (marked 사용안함 but dispatch exists)
	// ACK: PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK
	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK, 1);
		return;
	}

	int32_t targetTeamIdx = 0;
	memcpy(&targetTeamIdx, pData, 4);

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK, 2);
		return;
	}

	if (pMyTeam->ui8State != CLAN_MATCH_TEAM_WAITING)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK, 3);
		return;
	}

	ClanMatchTeam* pTarget = g_pClanMatchManager->GetTeam(targetTeamIdx);
	if (!pTarget || !pTarget->IsActive())
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK, 4);
		return;
	}

	if (pTarget->i32ClanId == m_i32ClanId)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK, 5);	// Same clan
		return;
	}

	int fightResult = g_pClanMatchManager->RequestFight(pMyTeam->i32TeamIdx, targetTeamIdx);
	SendSimpleAck(PROTOCOL_CS_MATCH_NEW_FIGHT_REQUEST_OTHER_ACK, fightResult);
}

void GameSession::OnClanMatchNewGetRoomInfoReq(char* pData, INT32 i32Size)
{
	// Clan channel aggregate info: waiting teams count, fighting teams count
	i3NetworkPacket packet;
	int32_t result = 0;

	// Count teams by state
	int allTeams[MAX_CLAN_MATCH_TEAMS];
	int total = g_pClanMatchManager->GetAllActiveTeams(allTeams, MAX_CLAN_MATCH_TEAMS);

	int32_t waitingCount = 0;
	int32_t fightingCount = 0;
	for (int i = 0; i < total; i++)
	{
		const ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(allTeams[i]);
		if (!pTeam) continue;
		if (pTeam->ui8State == CLAN_MATCH_TEAM_WAITING)
			waitingCount++;
		else if (pTeam->ui8State == CLAN_MATCH_TEAM_FIGHTING)
			fightingCount++;
	}
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&waitingCount, 4);
	packet.WriteData(&fightingCount, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchNewHonorReq(char* pData, INT32 i32Size)
{
	// Honor match request - request an honor fight against specific clan
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 1);
		return;
	}

	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 2);
		return;
	}

	int32_t targetTeamIdx = 0;
	memcpy(&targetTeamIdx, pData, 4);

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 3);	// Not team leader
		return;
	}

	if (pMyTeam->ui8State != CLAN_MATCH_TEAM_WAITING)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 4);
		return;
	}

	ClanMatchTeam* pTarget = g_pClanMatchManager->GetTeam(targetTeamIdx);
	if (!pTarget || !pTarget->IsActive())
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 5);	// Target not found
		return;
	}

	if (pTarget->ui8State != CLAN_MATCH_TEAM_WAITING)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 6);	// Target busy
		return;
	}

	// Send honor request to target team leader
	int fightResult = g_pClanMatchManager->RequestFight(pMyTeam->i32TeamIdx, targetTeamIdx);
	if (fightResult != 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 7);
		return;
	}

	// Notify the target leader about the honor request
	GameSession* pTargetLeader = g_pGameSessionManager->FindSessionByUID(pTarget->i64LeaderUID);
	if (pTargetLeader)
	{
		int32_t nresult = 0;
		int32_t reqTeamIdx = pMyTeam->i32TeamIdx;
		i3NetworkPacket notifyPacket(PROTOCOL_CS_MATCH_NEW_HONOR_REQUEST_ACK);
		notifyPacket.WriteData(&nresult, sizeof(int32_t));
		notifyPacket.WriteData(&reqTeamIdx, sizeof(int32_t));
		notifyPacket.WriteData(pMyTeam->szClanName, 64);
		notifyPacket.WriteData(pMyTeam->szLeaderNick, 64);
		pTargetLeader->SendPacketMessage(&notifyPacket);
	}

	SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACK, 0);
}

void GameSession::OnClanMatchNewHonorAcceptReq(char* pData, INT32 i32Size)
{
	// Accept or reject an honor fight request
	if (i32Size < 5)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK, 1);
		return;
	}

	int32_t requesterTeamIdx = 0;
	memcpy(&requesterTeamIdx, pData, 4);

	uint8_t accept = 0;
	memcpy(&accept, pData + 4, 1);

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK, 2);
		return;
	}

	if (pMyTeam->i32OpponentTeamIdx != requesterTeamIdx)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK, 3);	// No pending request from this team
		return;
	}

	int result = g_pClanMatchManager->AcceptFight(pMyTeam->i32TeamIdx, accept != 0);

	// Notify the requester about acceptance/rejection
	ClanMatchTeam* pReqTeam = g_pClanMatchManager->GetTeam(requesterTeamIdx);
	if (pReqTeam)
	{
		GameSession* pReqLeader = g_pGameSessionManager->FindSessionByUID(pReqTeam->i64LeaderUID);
		if (pReqLeader)
		{
			int32_t nresult = accept ? 0 : 10;	// 0=accepted, 10=rejected
			i3NetworkPacket notifyPacket(PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK);
			notifyPacket.WriteData(&nresult, sizeof(int32_t));
			pReqLeader->SendPacketMessage(&notifyPacket);
		}
	}

	SendSimpleAck(PROTOCOL_CS_MATCH_NEW_HONOR_ACCEPT_ACK, result);
}

void GameSession::OnClanMatchNewOperationReq(char* pData, INT32 i32Size)
{
	// Set operation/mission name for the match
	if (i32Size < 4)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_OPERATION_ACK, 1);
		return;
	}

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_NEW_OPERATION_ACK, 2);	// Not team leader
		return;
	}

	// Parse operation name (up to 64 chars)
	char szOperation[64] = {};
	int copyLen = (i32Size < 64) ? i32Size : 63;
	memcpy(szOperation, pData, copyLen);
	szOperation[copyLen] = '\0';

	// Broadcast operation name to all team members
	i3NetworkPacket packet;
	int32_t result = 0;
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_NEW_OPERATION_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(szOperation, 64);
	// Send to all team members
	for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
	{
		if (!pMyTeam->members[i].bActive) continue;
		GameSession* pMember = g_pGameSessionManager->FindSessionByUID(pMyTeam->members[i].i64UID);
		if (pMember)
			pMember->SendPacketMessage(&packet);
	}
}

void GameSession::OnClanMatchRoomCreateGsReq(char* pData, INT32 i32Size)
{
	// Create a clan match room (GameServer-side)
	// This is triggered after both teams accept the fight
	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK, 1);
		return;
	}

	ClanMatchTeam* pMyTeam = g_pClanMatchManager->FindTeamByLeader(m_i64UID);
	if (!pMyTeam)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK, 2);
		return;
	}

	if (pMyTeam->ui8FightState != CLAN_MATCH_FIGHT_ACCEPTED)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK, 3);	// Fight not accepted
		return;
	}

	if (GetMainTask() < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK, 4);	// Not in channel
		return;
	}

	// Create room via RoomManager
	GameRoomCreateInfo createInfo = {};
	snprintf(createInfo.szTitle, sizeof(createInfo.szTitle), "ClanMatch: %s", pMyTeam->szClanName);
	createInfo.ui8GameMode = 1;			// Bomb mode (typical for clan matches)
	createInfo.ui8MaxPlayers = pMyTeam->ui8MaxMembers * 2;
	createInfo.ui8RoundCount = 5;

	int roomIdx = g_pRoomManager->OnCreateRoom(this, &createInfo);
	if (roomIdx < 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK, 5);	// Room creation failed
		return;
	}

	// Send room create ACK with room info
	i3NetworkPacket packet;
	int32_t result = 0;

	int32_t rIdx = roomIdx;

	int32_t channel = m_i32ChannelNum;
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&rIdx, 4);
	packet.WriteData(&channel, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchRoomJoinGsReq(char* pData, INT32 i32Size)
{
	// Join an existing clan match room
	if (i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK, 1);
		return;
	}

	int32_t roomIdx = 0;
	int32_t channel = 0;
	memcpy(&roomIdx, pData, 4);
	memcpy(&channel, pData + 4, 4);

	if (GetMainTask() < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK, 2);
		return;
	}

	if (m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK, 3);
		return;
	}

	Room* pRoom = g_pRoomManager->GetRoom(channel, roomIdx);
	if (!pRoom || !pRoom->IsCreated())
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK, 4);	// Room not found
		return;
	}

	int slotResult = g_pRoomManager->OnJoinRoom(this, channel, roomIdx, nullptr);
	if (slotResult < 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK, 5);	// Join failed
		return;
	}

	i3NetworkPacket packet;
	int32_t result = 0;

	int32_t slot = slotResult;
	i3NetworkPacket packet(PROTOCOL_CS_MATCH_ROOM_JOIN_GS_ACK);
	packet.WriteData(&result, sizeof(int32_t));
	packet.WriteData(&slot, 4);
	SendPacketMessage(&packet);
}

void GameSession::OnClanMatchRoomLeaveGsReq(char* pData, INT32 i32Size)
{
	// Leave clan match room (GameServer-side leave request)
	if (GetMainTask() < GAME_TASK_READY_ROOM || m_i32RoomIdx < 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ + 1, 1);	// Not in room
		return;
	}

	Room* pRoom = m_pRoom;
	if (pRoom)
	{
		g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum);
	}

	SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_LEAVE_GS_REQ + 1, 0);
}

void GameSession::OnClanMatchRoomLeaveReq(char* pData, INT32 i32Size)
{
	// Leave clan match team room (client-side leave)
	// Also leave the clan match team if applicable
	ClanMatchTeam* pMyTeam = nullptr;

	// Find which team this player belongs to
	int allTeams[MAX_CLAN_MATCH_TEAMS];
	int total = g_pClanMatchManager->GetAllActiveTeams(allTeams, MAX_CLAN_MATCH_TEAMS);
	for (int i = 0; i < total; i++)
	{
		ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(allTeams[i]);
		if (pTeam && pTeam->FindMemberByUID(m_i64UID) >= 0)
		{
			pMyTeam = pTeam;
			break;
		}
	}

	if (pMyTeam)
	{
		g_pClanMatchManager->LeaveTeam(pMyTeam->i32TeamIdx, m_i64UID);
	}

	// Leave room if in one
	if (m_pRoom && m_i32RoomIdx >= 0)
	{
		g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum);
	}

	SendSimpleAck(PROTOCOL_CS_MATCH_ROOM_LEAVE_REQ + 1, 0);
}
