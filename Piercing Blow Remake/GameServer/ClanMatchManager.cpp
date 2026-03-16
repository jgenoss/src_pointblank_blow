#include "pch.h"
#include "ClanMatchManager.h"

ClanMatchManager* g_pClanMatchManager = nullptr;

ClanMatchManager::ClanMatchManager()
	: m_i32ActiveCount(0)
{
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS; i++)
		m_Teams[i].Reset();
}

ClanMatchManager::~ClanMatchManager()
{
}

void ClanMatchManager::Update()
{
	DWORD dwNow = GetTickCount();

	// Clean up stale teams (no activity for 10 minutes)
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS; i++)
	{
		if (!m_Teams[i].IsActive())
			continue;

		// Clean empty teams
		if (m_Teams[i].ui8MemberCount == 0)
		{
			DestroyTeam(i);
			continue;
		}

		// Timeout: 10 minutes in WAITING state with no fight
		if (m_Teams[i].ui8State == CLAN_MATCH_TEAM_WAITING &&
			(dwNow - m_Teams[i].dwCreateTime) > 600000)
		{
			printf("[ClanMatch] Team %d timed out (Clan=%s)\n",
				i, m_Teams[i].szClanName);
			DestroyTeam(i);
		}
	}
}

int ClanMatchManager::CreateTeam(int clanId, const char* clanName,
	int64_t leaderUID, const char* leaderNick, int sessionIdx, uint8_t maxMembers)
{
	if (clanId <= 0 || !clanName || !leaderNick)
		return -1;

	// Check if clan already has an active team
	if (FindTeamByClan(clanId) != nullptr)
		return -2;	// Clan already has a team

	// Find empty slot
	int idx = -1;
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS; i++)
	{
		if (!m_Teams[i].IsActive())
		{
			idx = i;
			break;
		}
	}

	if (idx < 0)
		return -3;	// No free slots

	if (maxMembers < CLAN_MATCH_MIN_PLAYERS)
		maxMembers = CLAN_MATCH_MIN_PLAYERS;
	if (maxMembers > MAX_CLAN_MATCH_TEAM_SIZE)
		maxMembers = MAX_CLAN_MATCH_TEAM_SIZE;

	ClanMatchTeam& team = m_Teams[idx];
	team.Reset();
	team.i32TeamIdx = idx;
	team.i32ClanId = clanId;
	strncpy_s(team.szClanName, clanName, _TRUNCATE);
	team.i64LeaderUID = leaderUID;
	strncpy_s(team.szLeaderNick, leaderNick, _TRUNCATE);
	team.ui8State = CLAN_MATCH_TEAM_WAITING;
	team.ui8MaxMembers = maxMembers;
	team.dwCreateTime = GetTickCount();

	// Add leader as first member
	team.members[0].bActive = true;
	team.members[0].i64UID = leaderUID;
	strncpy_s(team.members[0].szNickname, leaderNick, _TRUNCATE);
	team.members[0].i32SessionIdx = sessionIdx;
	team.members[0].bReady = true;
	team.ui8MemberCount = 1;

	m_i32ActiveCount++;

	printf("[ClanMatch] Team created - Idx=%d, Clan=%s, Leader=%s, Max=%d\n",
		idx, clanName, leaderNick, maxMembers);

	return idx;
}

int ClanMatchManager::JoinTeam(int teamIdx, int64_t uid, const char* nickname,
	int sessionIdx, uint8_t level, uint8_t rank)
{
	if (teamIdx < 0 || teamIdx >= MAX_CLAN_MATCH_TEAMS)
		return -1;

	ClanMatchTeam& team = m_Teams[teamIdx];
	if (!team.IsActive())
		return -2;

	if (team.ui8State != CLAN_MATCH_TEAM_WAITING)
		return -3;	// Team not accepting members

	if (team.IsFull())
		return -4;

	// Check not already in team
	if (team.FindMemberByUID(uid) >= 0)
		return -5;

	int slot = team.FindEmptySlot();
	if (slot < 0)
		return -6;

	team.members[slot].bActive = true;
	team.members[slot].i64UID = uid;
	strncpy_s(team.members[slot].szNickname, nickname, _TRUNCATE);
	team.members[slot].i32SessionIdx = sessionIdx;
	team.members[slot].ui8Level = level;
	team.members[slot].ui8Rank = rank;
	team.members[slot].bReady = false;
	team.ui8MemberCount++;

	printf("[ClanMatch] Member joined - Team=%d, UID=%lld (%s), Count=%d/%d\n",
		teamIdx, uid, nickname, team.ui8MemberCount, team.ui8MaxMembers);

	return slot;
}

void ClanMatchManager::LeaveTeam(int teamIdx, int64_t uid)
{
	if (teamIdx < 0 || teamIdx >= MAX_CLAN_MATCH_TEAMS)
		return;

	ClanMatchTeam& team = m_Teams[teamIdx];
	if (!team.IsActive())
		return;

	int slot = team.FindMemberByUID(uid);
	if (slot < 0)
		return;

	team.members[slot].Reset();
	team.ui8MemberCount--;

	printf("[ClanMatch] Member left - Team=%d, UID=%lld, Count=%d/%d\n",
		teamIdx, uid, team.ui8MemberCount, team.ui8MaxMembers);

	// If leader left, destroy team
	if (team.IsLeader(uid) || team.ui8MemberCount == 0)
	{
		printf("[ClanMatch] Leader left or empty - destroying team %d\n", teamIdx);
		DestroyTeam(teamIdx);
	}
}

void ClanMatchManager::DestroyTeam(int teamIdx)
{
	if (teamIdx < 0 || teamIdx >= MAX_CLAN_MATCH_TEAMS)
		return;

	if (!m_Teams[teamIdx].IsActive())
		return;

	printf("[ClanMatch] Team destroyed - Idx=%d, Clan=%s\n",
		teamIdx, m_Teams[teamIdx].szClanName);

	// Cancel pending fight
	if (m_Teams[teamIdx].i32OpponentTeamIdx >= 0)
	{
		int oppIdx = m_Teams[teamIdx].i32OpponentTeamIdx;
		if (oppIdx >= 0 && oppIdx < MAX_CLAN_MATCH_TEAMS)
		{
			m_Teams[oppIdx].i32OpponentTeamIdx = -1;
			m_Teams[oppIdx].ui8FightState = CLAN_MATCH_FIGHT_NONE;
		}
	}

	m_Teams[teamIdx].Reset();
	m_i32ActiveCount--;
	if (m_i32ActiveCount < 0)
		m_i32ActiveCount = 0;
}

int ClanMatchManager::RequestFight(int attackerTeamIdx, int defenderTeamIdx)
{
	if (attackerTeamIdx < 0 || attackerTeamIdx >= MAX_CLAN_MATCH_TEAMS)
		return -1;
	if (defenderTeamIdx < 0 || defenderTeamIdx >= MAX_CLAN_MATCH_TEAMS)
		return -1;
	if (attackerTeamIdx == defenderTeamIdx)
		return -2;

	ClanMatchTeam& attacker = m_Teams[attackerTeamIdx];
	ClanMatchTeam& defender = m_Teams[defenderTeamIdx];

	if (!attacker.IsActive() || !defender.IsActive())
		return -3;

	if (attacker.ui8State != CLAN_MATCH_TEAM_WAITING ||
		defender.ui8State != CLAN_MATCH_TEAM_WAITING)
		return -4;

	// Both must not have pending fights
	if (attacker.ui8FightState != CLAN_MATCH_FIGHT_NONE ||
		defender.ui8FightState != CLAN_MATCH_FIGHT_NONE)
		return -5;

	attacker.i32OpponentTeamIdx = defenderTeamIdx;
	attacker.ui8FightState = CLAN_MATCH_FIGHT_REQUESTED;

	defender.i32OpponentTeamIdx = attackerTeamIdx;
	defender.ui8FightState = CLAN_MATCH_FIGHT_REQUESTED;

	printf("[ClanMatch] Fight requested - %s (Team %d) vs %s (Team %d)\n",
		attacker.szClanName, attackerTeamIdx, defender.szClanName, defenderTeamIdx);

	return 0;
}

int ClanMatchManager::AcceptFight(int teamIdx, bool accept)
{
	if (teamIdx < 0 || teamIdx >= MAX_CLAN_MATCH_TEAMS)
		return -1;

	ClanMatchTeam& team = m_Teams[teamIdx];
	if (!team.IsActive())
		return -2;

	if (team.ui8FightState != CLAN_MATCH_FIGHT_REQUESTED)
		return -3;

	int oppIdx = team.i32OpponentTeamIdx;
	if (oppIdx < 0 || oppIdx >= MAX_CLAN_MATCH_TEAMS)
		return -4;

	ClanMatchTeam& opponent = m_Teams[oppIdx];

	if (accept)
	{
		team.ui8FightState = CLAN_MATCH_FIGHT_ACCEPTED;
		opponent.ui8FightState = CLAN_MATCH_FIGHT_ACCEPTED;
		team.ui8State = CLAN_MATCH_TEAM_FIGHTING;
		opponent.ui8State = CLAN_MATCH_TEAM_FIGHTING;

		printf("[ClanMatch] Fight accepted - %s vs %s\n",
			team.szClanName, opponent.szClanName);
		return 0;
	}
	else
	{
		team.ui8FightState = CLAN_MATCH_FIGHT_NONE;
		team.i32OpponentTeamIdx = -1;
		opponent.ui8FightState = CLAN_MATCH_FIGHT_NONE;
		opponent.i32OpponentTeamIdx = -1;

		printf("[ClanMatch] Fight rejected - %s rejected %s\n",
			team.szClanName, opponent.szClanName);
		return 1;
	}
}

ClanMatchTeam* ClanMatchManager::GetTeam(int idx)
{
	if (idx < 0 || idx >= MAX_CLAN_MATCH_TEAMS)
		return nullptr;
	return &m_Teams[idx];
}

const ClanMatchTeam* ClanMatchManager::GetTeam(int idx) const
{
	if (idx < 0 || idx >= MAX_CLAN_MATCH_TEAMS)
		return nullptr;
	return &m_Teams[idx];
}

ClanMatchTeam* ClanMatchManager::FindTeamByClan(int clanId)
{
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS; i++)
	{
		if (m_Teams[i].IsActive() && m_Teams[i].i32ClanId == clanId)
			return &m_Teams[i];
	}
	return nullptr;
}

ClanMatchTeam* ClanMatchManager::FindTeamByLeader(int64_t leaderUID)
{
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS; i++)
	{
		if (m_Teams[i].IsActive() && m_Teams[i].i64LeaderUID == leaderUID)
			return &m_Teams[i];
	}
	return nullptr;
}

int ClanMatchManager::GetActiveTeamCount() const
{
	return m_i32ActiveCount;
}

int ClanMatchManager::GetTeamListForClan(int clanId, int* pOutIndices, int maxCount) const
{
	int count = 0;
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS && count < maxCount; i++)
	{
		if (m_Teams[i].IsActive() && m_Teams[i].i32ClanId == clanId)
			pOutIndices[count++] = i;
	}
	return count;
}

int ClanMatchManager::GetAllActiveTeams(int* pOutIndices, int maxCount) const
{
	int count = 0;
	for (int i = 0; i < MAX_CLAN_MATCH_TEAMS && count < maxCount; i++)
	{
		if (m_Teams[i].IsActive())
			pOutIndices[count++] = i;
	}
	return count;
}
