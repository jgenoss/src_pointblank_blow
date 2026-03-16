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
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (teamIdx >= 0) ? 0 : teamIdx;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	if (teamIdx >= 0)
	{
		int32_t idx = teamIdx;
		memcpy(buffer + offset, &idx, sizeof(int32_t));		offset += sizeof(int32_t);
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	char buffer[2048];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint16_t teamCount = (uint16_t)count;
	memcpy(buffer + offset, &teamCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	for (int i = 0; i < count; i++)
	{
		const ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(indices[i]);
		if (!pTeam || offset + 135 > (int)sizeof(buffer))
			break;

		int32_t idx = indices[i];
		memcpy(buffer + offset, &idx, sizeof(int32_t));		offset += sizeof(int32_t);
		memcpy(buffer + offset, pTeam->szClanName, 64);		offset += 64;
		memcpy(buffer + offset, pTeam->szLeaderNick, 64);	offset += 64;
		memcpy(buffer + offset, &pTeam->ui8State, 1);		offset += 1;
		memcpy(buffer + offset, &pTeam->ui8MemberCount, 1); offset += 1;
		memcpy(buffer + offset, &pTeam->ui8MaxMembers, 1);	offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	// Mercenary list - return empty for now
	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CLAN_WAR_MERCENARY_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint16_t mercCount = 0;
	memcpy(buffer + offset, &mercCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
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
	char buffer[4096];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CLAN_WAR_RESULT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	uint16_t matchCount = (uint16_t)count;
	memcpy(buffer + offset, &matchCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

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

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// ============================================================================
// Clan War Mercenary Handlers (Protocol_Clan_War 0x1B00)
// ============================================================================

void GameSession::OnClanWarRegistMercenaryReq(char* pData, INT32 i32Size)
{
	// Register as available mercenary for clan wars
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK, -1);
		return;
	}

	// Player registers availability as mercenary (non-clan member joining clan wars)
	// For now, just acknowledge the registration
	int32_t result = 0;

	// Can't be mercenary if already in a clan war team
	if (m_i32ClanId > 0)
		result = -2;	// Clan members can't be mercenaries

	SendSimpleAck(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK, result);
}

void GameSession::OnClanWarRemoveMercenaryReq(char* pData, INT32 i32Size)
{
	// Unregister from mercenary pool
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK, -1);
		return;
	}

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
			// Find mercenary online
			GameSession* pMerc = g_pGameSessionManager->FindSessionByUID(mercUID);
			if (!pMerc)
				result = -5;	// Offline
			else
			{
				// Send invite to the mercenary
				i3NetworkPacket invPacket;
				char invBuf[128];
				int invOff = 0;

				uint16_t invSz = 0;
				uint16_t invProto = PROTOCOL_CLAN_WAR_INVITE_MERCENARY_RECEIVER_ACK;
				invOff += sizeof(uint16_t);
				memcpy(invBuf + invOff, &invProto, sizeof(uint16_t));	invOff += sizeof(uint16_t);
				memcpy(invBuf + invOff, &teamIdx, sizeof(int32_t));		invOff += sizeof(int32_t);
				memcpy(invBuf + invOff, &m_i32ClanId, sizeof(int32_t));	invOff += sizeof(int32_t);
				memcpy(invBuf + invOff, m_szClanName, MAX_CLAN_NAME_LEN);	invOff += MAX_CLAN_NAME_LEN;
				memcpy(invBuf + invOff, m_szNickname, 64);				invOff += 64;

				invSz = (uint16_t)invOff;
				memcpy(invBuf, &invSz, sizeof(uint16_t));

				invPacket.SetPacketData(invBuf, invOff);
				pMerc->SendMessage(&invPacket);
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
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));	offset += sizeof(uint16_t);

	int32_t result = pMerc ? 0 : -1;
	memcpy(buffer + offset, &result, sizeof(int32_t));	offset += sizeof(int32_t);

	if (pMerc)
	{
		memcpy(buffer + offset, &mercUID, sizeof(int64_t));		offset += sizeof(int64_t);

		char nick[64] = {};
		strncpy(nick, pMerc->GetNickname(), 63);
		memcpy(buffer + offset, nick, 64);						offset += 64;

		int32_t level = pMerc->GetLevel();
		int32_t rank = pMerc->GetRankId();
		int32_t kills = pMerc->GetKills();
		int32_t deaths = pMerc->GetDeaths();
		int32_t wins = pMerc->GetWins();
		int32_t losses = pMerc->GetLosses();
		memcpy(buffer + offset, &level, 4);		offset += 4;
		memcpy(buffer + offset, &rank, 4);		offset += 4;
		memcpy(buffer + offset, &kills, 4);		offset += 4;
		memcpy(buffer + offset, &deaths, 4);	offset += 4;
		memcpy(buffer + offset, &wins, 4);		offset += 4;
		memcpy(buffer + offset, &losses, 4);	offset += 4;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}
