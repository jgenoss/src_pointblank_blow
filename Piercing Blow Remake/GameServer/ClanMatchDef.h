#ifndef __CLANMATCHDEF_H__
#define __CLANMATCHDEF_H__

#pragma once

#define MAX_CLAN_MATCH_TEAMS		100
#define MAX_CLAN_MATCH_TEAM_SIZE	8
#define CLAN_MATCH_MIN_PLAYERS		4
#define CLAN_MATCH_DEFAULT_PLAYERS	5

// Team state
enum ClanMatchTeamState
{
	CLAN_MATCH_TEAM_NONE		= 0,
	CLAN_MATCH_TEAM_WAITING		= 1,	// Waiting for members
	CLAN_MATCH_TEAM_READY		= 2,	// All members ready
	CLAN_MATCH_TEAM_FIGHTING	= 3,	// In match
	CLAN_MATCH_TEAM_ENDED		= 4,	// Match ended
};

// Fight request state
enum ClanMatchFightState
{
	CLAN_MATCH_FIGHT_NONE		= 0,
	CLAN_MATCH_FIGHT_REQUESTED	= 1,	// Fight request sent
	CLAN_MATCH_FIGHT_ACCEPTED	= 2,	// Fight accepted
	CLAN_MATCH_FIGHT_REJECTED	= 3,	// Fight rejected
};

// Team member info
struct ClanMatchMember
{
	int64_t		i64UID;
	char		szNickname[64];
	int			i32SessionIdx;
	uint8_t		ui8Level;
	uint8_t		ui8Rank;
	bool		bReady;
	bool		bActive;

	void Reset()
	{
		i64UID = 0;
		szNickname[0] = '\0';
		i32SessionIdx = -1;
		ui8Level = 0;
		ui8Rank = 0;
		bReady = false;
		bActive = false;
	}
};

// Team info
struct ClanMatchTeam
{
	int					i32TeamIdx;
	int					i32ClanId;
	char				szClanName[64];
	int64_t				i64LeaderUID;
	char				szLeaderNick[64];
	uint8_t				ui8State;			// ClanMatchTeamState
	uint8_t				ui8MaxMembers;		// 4-8
	uint8_t				ui8MemberCount;
	ClanMatchMember		members[MAX_CLAN_MATCH_TEAM_SIZE];

	// Fight request
	int					i32OpponentTeamIdx;	// -1 = none
	uint8_t				ui8FightState;		// ClanMatchFightState

	// Match result
	int					i32Wins;
	int					i32Losses;
	int					i32MatchPoints;

	DWORD				dwCreateTime;

	void Reset()
	{
		i32TeamIdx = -1;
		i32ClanId = 0;
		szClanName[0] = '\0';
		i64LeaderUID = 0;
		szLeaderNick[0] = '\0';
		ui8State = CLAN_MATCH_TEAM_NONE;
		ui8MaxMembers = CLAN_MATCH_DEFAULT_PLAYERS;
		ui8MemberCount = 0;
		i32OpponentTeamIdx = -1;
		ui8FightState = CLAN_MATCH_FIGHT_NONE;
		i32Wins = 0;
		i32Losses = 0;
		i32MatchPoints = 0;
		dwCreateTime = 0;

		for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
			members[i].Reset();
	}

	int FindMemberByUID(int64_t uid) const
	{
		for (int i = 0; i < MAX_CLAN_MATCH_TEAM_SIZE; i++)
		{
			if (members[i].bActive && members[i].i64UID == uid)
				return i;
		}
		return -1;
	}

	int FindEmptySlot() const
	{
		for (int i = 0; i < ui8MaxMembers; i++)
		{
			if (!members[i].bActive)
				return i;
		}
		return -1;
	}

	bool IsLeader(int64_t uid) const { return i64LeaderUID == uid; }
	bool IsFull() const { return ui8MemberCount >= ui8MaxMembers; }
	bool IsActive() const { return ui8State != CLAN_MATCH_TEAM_NONE; }
};

#endif // __CLANMATCHDEF_H__
