#include "pch.h"
#include "GameSession.h"
#include "GameProtocol.h"
#include "ClanMatchManager.h"
#include "GameSessionManager.h"
#include "RoomManager.h"
#include "GameServerContext.h"

// ============================================================================
// Clan Match Handlers (Protocol_ClanMatch 0x0800)
// ============================================================================

// Client requests team context (when entering clan channel or clicking clan match)
void GameSession::OnClanMatchTeamContextReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK, -2);
		return;
	}

	// Check if clan has an existing team
	ClanMatchTeam* pTeam = g_pClanMatchManager->FindTeamByClan(m_i32ClanId);

	i3NetworkPacket packet;
	char buffer[256];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MATCH_TEAM_CONTEXT_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Has team?
	uint8_t hasTeam = (pTeam != nullptr) ? 1 : 0;
	memcpy(buffer + offset, &hasTeam, 1);					offset += 1;

	if (pTeam)
	{
		// Team idx
		int32_t teamIdx = pTeam->i32TeamIdx;
		memcpy(buffer + offset, &teamIdx, sizeof(int32_t));	offset += sizeof(int32_t);

		// State
		memcpy(buffer + offset, &pTeam->ui8State, 1);		offset += 1;

		// Member count / max
		memcpy(buffer + offset, &pTeam->ui8MemberCount, 1); offset += 1;
		memcpy(buffer + offset, &pTeam->ui8MaxMembers, 1);	offset += 1;

		// Leader nick
		memcpy(buffer + offset, pTeam->szLeaderNick, 64);	offset += 64;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);
}

// Client requests to create a clan match team
void GameSession::OnClanMatchTeamCreateReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CREATE_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_CREATE_ACK, -2);
		return;
	}

	// Parse: maxMembers(1)
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
	uint16_t proto = PROTOCOL_CS_MATCH_TEAM_CREATE_ACK;
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

// Client requests to join a clan match team
void GameSession::OnClanMatchTeamJoinReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL || m_i32ClanId <= 0)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_JOIN_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager || i32Size < (int)sizeof(int))
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_JOIN_ACK, -2);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || pTeam->i32ClanId != m_i32ClanId)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_JOIN_ACK, -3); // Wrong clan or invalid
		return;
	}

	int slot = g_pClanMatchManager->JoinTeam(teamIdx, m_i64UID, m_szNickname,
		GetIndex(), (uint8_t)m_i32Level, (uint8_t)m_i32RankId);

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MATCH_TEAM_JOIN_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = (slot >= 0) ? 0 : slot;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);
	SendMessage(&packet);

	// Notify team members about new member
	if (slot >= 0)
		SendClanMatchTeamInfo(teamIdx);
}

// Client requests to leave clan match team
void GameSession::OnClanMatchTeamLeaveReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager)
		return;

	if (i32Size < (int)sizeof(int))
		return;

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	g_pClanMatchManager->LeaveTeam(teamIdx, m_i64UID);

	SendSimpleAck(PROTOCOL_CS_MATCH_TEAM_LEAVE_ACK, 0);

	// Notify remaining members
	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (pTeam && pTeam->IsActive())
		SendClanMatchTeamInfo(teamIdx);
}

// Client requests list of all active teams (for matchmaking)
void GameSession::OnClanMatchAllTeamListReq(char* pData, INT32 i32Size)
{
	if (m_eMainTask < GAME_TASK_CHANNEL)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK, -1);
		return;
	}

	if (!g_pClanMatchManager)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK, -2);
		return;
	}

	int indices[50];
	int count = g_pClanMatchManager->GetAllActiveTeams(indices, 50);

	i3NetworkPacket packet;
	char buffer[2048];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MATCH_ALL_TEAM_LIST_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	// Team count
	uint16_t teamCount = (uint16_t)count;
	memcpy(buffer + offset, &teamCount, sizeof(uint16_t));	offset += sizeof(uint16_t);

	// Per team: teamIdx(4) + clanName(64) + leaderNick(64) + state(1) + members(1) + maxMembers(1) = 135 bytes
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

// Client requests fight with another team
void GameSession::OnClanMatchFightRequestReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || i32Size < 8)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, -1);
		return;
	}

	int myTeamIdx = 0, targetTeamIdx = 0;
	memcpy(&myTeamIdx, pData, sizeof(int));
	memcpy(&targetTeamIdx, pData + 4, sizeof(int));

	// Verify requester is leader
	ClanMatchTeam* pMyTeam = g_pClanMatchManager->GetTeam(myTeamIdx);
	if (!pMyTeam || !pMyTeam->IsLeader(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, -2);
		return;
	}

	int fightResult = g_pClanMatchManager->RequestFight(myTeamIdx, targetTeamIdx);

	SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_REQUEST_ACK, fightResult);

	// Notify defender team leader about fight request
	if (fightResult == 0)
	{
		ClanMatchTeam* pTarget = g_pClanMatchManager->GetTeam(targetTeamIdx);
		if (pTarget)
		{
			// Find leader session and send notification
			GameSession* pLeader = g_pGameSessionManager ?
				g_pGameSessionManager->FindSessionByUID(pTarget->i64LeaderUID) : nullptr;
			if (pLeader)
			{
				i3NetworkPacket notifyPacket;
				char nbuf[128];
				int noff = 0;

				uint16_t nsz = 0;
				uint16_t nproto = PROTOCOL_CS_MATCH_FIGHT_REQUEST_RESULT_ACK;
				noff += sizeof(uint16_t);
				memcpy(nbuf + noff, &nproto, sizeof(uint16_t));		noff += sizeof(uint16_t);

				int32_t nresult = 0;
				memcpy(nbuf + noff, &nresult, sizeof(int32_t));		noff += sizeof(int32_t);

				// Attacker info
				memcpy(nbuf + noff, pMyTeam->szClanName, 64);		noff += 64;
				int32_t atkIdx = myTeamIdx;
				memcpy(nbuf + noff, &atkIdx, sizeof(int32_t));		noff += sizeof(int32_t);

				nsz = (uint16_t)noff;
				memcpy(nbuf, &nsz, sizeof(uint16_t));

				notifyPacket.SetPacketData(nbuf, noff);
				pLeader->SendMessage(&notifyPacket);
			}
		}
	}
}

// Client accepts or rejects a fight request
void GameSession::OnClanMatchFightAcceptReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || i32Size < 5)
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK, -1);
		return;
	}

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));
	uint8_t accept = *(uint8_t*)(pData + 4);

	// Verify requester is leader
	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || !pTeam->IsLeader(m_i64UID))
	{
		SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK, -2);
		return;
	}

	int result = g_pClanMatchManager->AcceptFight(teamIdx, accept != 0);
	SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_ACCECT_ACK, result);

	// If accepted, both teams are now FIGHTING - create clan match room
	if (result == 0 && accept != 0)
	{
		// Notify opponent about acceptance
		int oppIdx = pTeam->i32OpponentTeamIdx;
		ClanMatchTeam* pOpp = g_pClanMatchManager->GetTeam(oppIdx);
		if (pOpp)
		{
			GameSession* pOppLeader = g_pGameSessionManager ?
				g_pGameSessionManager->FindSessionByUID(pOpp->i64LeaderUID) : nullptr;
			if (pOppLeader)
				pOppLeader->SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK, 0);
		}

		// Auto-create clan match room
		CreateClanMatchRoom(pTeam, pOpp);
	}
	else if (!accept)
	{
		// Notify opponent about rejection
		int oppIdx = pTeam->i32OpponentTeamIdx;
		ClanMatchTeam* pOpp = g_pClanMatchManager->GetTeam(oppIdx);
		if (pOpp)
		{
			GameSession* pOppLeader = g_pGameSessionManager ?
				g_pGameSessionManager->FindSessionByUID(pOpp->i64LeaderUID) : nullptr;
			if (pOppLeader)
				pOppLeader->SendSimpleAck(PROTOCOL_CS_MATCH_FIGHT_ACCECT_RESULT_ACK, 1);
		}
	}
}

// Clan match team chat
void GameSession::OnClanMatchChatReq(char* pData, INT32 i32Size)
{
	if (!g_pClanMatchManager || i32Size < 6)
		return;

	int teamIdx = 0;
	memcpy(&teamIdx, pData, sizeof(int));

	uint16_t msgLen = 0;
	memcpy(&msgLen, pData + 4, sizeof(uint16_t));
	if (msgLen == 0 || msgLen > 200 || (int)(6 + msgLen) > i32Size)
		return;

	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || pTeam->FindMemberByUID(m_i64UID) < 0)
		return;

	// Broadcast to all team members
	i3NetworkPacket packet;
	char buffer[400];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MATCH_CHATING_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// Sender nickname
	memcpy(buffer + offset, m_szNickname, 64);				offset += 64;

	// Message
	memcpy(buffer + offset, &msgLen, sizeof(uint16_t));		offset += sizeof(uint16_t);
	memcpy(buffer + offset, pData + 6, msgLen);				offset += msgLen;

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
	{
		if (!pTeam->members[i].bActive)
			continue;

		GameSession* pMember = g_pGameSessionManager ?
			g_pGameSessionManager->GetSession(pTeam->members[i].i32SessionIdx) : nullptr;
		if (pMember && pMember->GetUID() == pTeam->members[i].i64UID)
			pMember->SendMessage(&packet);
	}
}

// Helper: Send team info to all team members
void GameSession::SendClanMatchTeamInfo(int teamIdx)
{
	if (!g_pClanMatchManager || !g_pGameSessionManager)
		return;

	ClanMatchTeam* pTeam = g_pClanMatchManager->GetTeam(teamIdx);
	if (!pTeam || !pTeam->IsActive())
		return;

	i3NetworkPacket packet;
	char buffer[512];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MATCH_TEAM_INFO_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	// Team info
	int32_t idx = teamIdx;
	memcpy(buffer + offset, &idx, sizeof(int32_t));			offset += sizeof(int32_t);
	memcpy(buffer + offset, &pTeam->ui8State, 1);			offset += 1;
	memcpy(buffer + offset, &pTeam->ui8MemberCount, 1);	offset += 1;
	memcpy(buffer + offset, &pTeam->ui8MaxMembers, 1);		offset += 1;

	// Member list
	for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
	{
		if (offset + 74 > (int)sizeof(buffer))
			break;

		uint8_t active = pTeam->members[i].bActive ? 1 : 0;
		memcpy(buffer + offset, &active, 1);				offset += 1;
		if (!active) continue;

		memcpy(buffer + offset, pTeam->members[i].szNickname, 64);	offset += 64;
		memcpy(buffer + offset, &pTeam->members[i].ui8Level, 1);	offset += 1;
		memcpy(buffer + offset, &pTeam->members[i].ui8Rank, 1);	offset += 1;
		uint8_t ready = pTeam->members[i].bReady ? 1 : 0;
		memcpy(buffer + offset, &ready, 1);							offset += 1;

		// Is leader
		uint8_t isLeader = (pTeam->members[i].i64UID == pTeam->i64LeaderUID) ? 1 : 0;
		memcpy(buffer + offset, &isLeader, 1);						offset += 1;
	}

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	// Send to all team members
	for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
	{
		if (!pTeam->members[i].bActive)
			continue;

		GameSession* pMember = g_pGameSessionManager->GetSession(pTeam->members[i].i32SessionIdx);
		if (pMember && pMember->GetUID() == pTeam->members[i].i64UID)
			pMember->SendMessage(&packet);
	}
}

// ============================================================================
// Auto-create a clan match room when fight is accepted
// ============================================================================
void GameSession::CreateClanMatchRoom(ClanMatchTeam* pTeam1, ClanMatchTeam* pTeam2)
{
	if (!pTeam1 || !pTeam2)
		return;

	RoomManager* pRoomMgr = g_pGameServerContext ? g_pGameServerContext->GetRoomManager() : nullptr;
	if (!pRoomMgr)
		return;

	// Find the leader session to create the room
	GameSession* pLeader = g_pGameSessionManager ?
		g_pGameSessionManager->FindSessionByUID(pTeam1->i64LeaderUID) : nullptr;
	if (!pLeader)
		return;

	int channel = pLeader->GetChannelNum();
	if (channel < 0)
		channel = 0;

	// Build room create info for clan match
	GameRoomCreateInfo info;
	memset(&info, 0, sizeof(info));
	snprintf(info.szTitle, sizeof(info.szTitle), "[ClanMatch] %s vs %s",
		pTeam1->szClanName, pTeam2->szClanName);
	info.ui8GameMode = STAGE_MODE_DEATHMATCH;
	info.ui8MapIndex = 0;
	info.ui8MaxPlayers = pTeam1->ui8MaxMembers + pTeam2->ui8MaxMembers;
	if (info.ui8MaxPlayers > SLOT_MAX_COUNT)
		info.ui8MaxPlayers = SLOT_MAX_COUNT;
	info.ui8RoundType = BATTLE_ROUND_TYPE_5;
	info.ui8SubType = 0;
	info.ui8WeaponFlag = LOCK_BASIC;
	info.ui8InfoFlag = 0;

	// Create the room (channel derived from leader's session)
	int roomIdx = pRoomMgr->OnCreateRoom(pLeader, &info);
	if (roomIdx < 0)
	{
		printf("[ClanMatch] Failed to create match room for %s vs %s\n",
			pTeam1->szClanName, pTeam2->szClanName);
		return;
	}

	Room* pRoom = pRoomMgr->GetRoom(channel, roomIdx);
	if (pRoom)
		pRoom->SetClanMatch(true);

	printf("[ClanMatch] Room created - Idx=%d, Ch=%d, %s vs %s\n",
		roomIdx, channel, pTeam1->szClanName, pTeam2->szClanName);

	// Notify both teams about the room
	SendClanMatchRoomNotify(pTeam1, channel, roomIdx);
	SendClanMatchRoomNotify(pTeam2, channel, roomIdx);
}

void GameSession::SendClanMatchRoomNotify(ClanMatchTeam* pTeam, int channel, int roomIdx)
{
	if (!pTeam || !g_pGameSessionManager)
		return;

	i3NetworkPacket packet;
	char buffer[32];
	int offset = 0;

	uint16_t sz = 0;
	uint16_t proto = PROTOCOL_CS_MATCH_ROOM_CREATE_ACK;
	offset += sizeof(uint16_t);
	memcpy(buffer + offset, &proto, sizeof(uint16_t));		offset += sizeof(uint16_t);

	int32_t result = 0;
	memcpy(buffer + offset, &result, sizeof(int32_t));		offset += sizeof(int32_t);

	int32_t ch = channel;
	memcpy(buffer + offset, &ch, sizeof(int32_t));			offset += sizeof(int32_t);

	int32_t idx = roomIdx;
	memcpy(buffer + offset, &idx, sizeof(int32_t));			offset += sizeof(int32_t);

	sz = (uint16_t)offset;
	memcpy(buffer, &sz, sizeof(uint16_t));

	packet.SetPacketData(buffer, offset);

	for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
	{
		if (!pTeam->members[i].bActive)
			continue;

		GameSession* pMember = g_pGameSessionManager->GetSession(pTeam->members[i].i32SessionIdx);
		if (pMember && pMember->GetUID() == pTeam->members[i].i64UID)
			pMember->SendMessage(&packet);
	}
}
