#ifndef __CLANMATCHMANAGER_H__
#define __CLANMATCHMANAGER_H__

#pragma once
#include "ClanMatchDef.h"

// Manages clan match teams and fight requests.
// Teams are created in the clan channel, members join, and when both
// teams accept a fight request a clan match room is created.
class ClanMatchManager
{
public:
	ClanMatchManager();
	~ClanMatchManager();

	void	Update();

	// Team operations
	int		CreateTeam(int clanId, const char* clanName, int64_t leaderUID, const char* leaderNick, int sessionIdx, uint8_t maxMembers);
	int		JoinTeam(int teamIdx, int64_t uid, const char* nickname, int sessionIdx, uint8_t level, uint8_t rank);
	void	LeaveTeam(int teamIdx, int64_t uid);
	void	DestroyTeam(int teamIdx);

	// Fight operations
	int		RequestFight(int attackerTeamIdx, int defenderTeamIdx);
	int		AcceptFight(int teamIdx, bool accept);

	// Queries
	ClanMatchTeam*			GetTeam(int idx);
	const ClanMatchTeam*	GetTeam(int idx) const;
	ClanMatchTeam*			FindTeamByClan(int clanId);
	ClanMatchTeam*			FindTeamByLeader(int64_t leaderUID);
	int						GetActiveTeamCount() const;
	int						GetTeamListForClan(int clanId, int* pOutIndices, int maxCount) const;
	int						GetAllActiveTeams(int* pOutIndices, int maxCount) const;

	// Match results
	void					AddMatchResult(int team1ClanId, const char* team1Name,
								int team2ClanId, const char* team2Name,
								int team1Score, int team2Score, int winnerTeam);
	int						GetMatchResults(const ClanMatchResultEntry** ppOut) const;
	int						GetMatchResultsForClan(int clanId, ClanMatchResultEntry* pOut, int maxCount) const;

	// Mercenary pool (in-memory, session-based)
	bool					RegisterMercenary(int64_t uid, const char* nick, int sessionIdx, uint8_t level, uint8_t rank);
	bool					UnregisterMercenary(int64_t uid);
	int						GetMercenaryList(MercenaryEntry* pOut, int maxCount) const;
	bool					IsMercenary(int64_t uid) const;

	// Preseason snapshot — populated at season end, queried by clanId
	void					SaveSeasonSnapshot(int clanId, const char* clanName,
								int seasonRank, int wins, int losses, int points);
	const ClanSeasonSnapshot* FindSeasonSnapshot(int clanId) const;
	int						GetSeasonSnapshotCount() const { return m_i32SnapshotCount; }
	void					ClearSeasonSnapshot();

private:
	ClanMatchTeam			m_Teams[MAX_CLAN_MATCH_TEAMS];
	int						m_i32ActiveCount;

	ClanMatchResultEntry	m_Results[MAX_CLAN_MATCH_RESULTS];
	int						m_i32ResultCount;
	int						m_i32ResultWriteIdx;	// Circular buffer index

	MercenaryEntry			m_Mercenaries[MAX_MERCENARIES];

	// Previous season snapshot (circular, max MAX_SEASON_SNAPSHOT_CLANS entries)
	ClanSeasonSnapshot		m_SeasonSnapshot[MAX_SEASON_SNAPSHOT_CLANS];
	int						m_i32SnapshotCount;
};

extern ClanMatchManager* g_pClanMatchManager;

#endif // __CLANMATCHMANAGER_H__
