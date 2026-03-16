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

private:
	ClanMatchTeam	m_Teams[MAX_CLAN_MATCH_TEAMS];
	int				m_i32ActiveCount;
};

extern ClanMatchManager* g_pClanMatchManager;

#endif // __CLANMATCHMANAGER_H__
