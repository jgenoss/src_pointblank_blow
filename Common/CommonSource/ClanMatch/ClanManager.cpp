#include "pch.h"
#include "ClanManager.h"


CClanManager::CClanManager()
{
}


CClanManager::~CClanManager()
{
}


void CClanManager::InsertSeason(SEASON_TYPE eSeasonType, CLAN_MATCH_SEASON* pMatchSeason)
{
	if( eSeasonType <= SEASON_TYPE_NONE || eSeasonType <= SEASON_TYPE_COUNT )
		return;

	m_Seasons[eSeasonType].SetSeason(pMatchSeason);
}