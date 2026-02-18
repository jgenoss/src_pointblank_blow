#pragma once

#include "ClanMatchSeason.h"
#define MAX_CLAN_NUMBER			10



class CClan
{

};

class CClanRanking
{
private:
	CClan*				m_pClanRanking[MAX_CLAN_NUMBER];
};


class CClanManager
{
public:
	void				InsertSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason);

public:
	CClanManager();
	virtual ~CClanManager();

private:
	CClanMatchSeason	m_Seasons[SEASON_TYPE_COUNT];

	INT32				m_i32ClanCount;
	CClan				m_Clans[MAX_CLAN_NUMBER];
	CClanRanking		m_ClanRanking;
};

