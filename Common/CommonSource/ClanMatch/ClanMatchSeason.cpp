#include "pch.h"
#include "ClanMatchSeason.h"


CClanMatchSeason::CClanMatchSeason()
: m_i32SeasonIndex(0)
, m_ui32StartDate(0)
, m_ui32EndDate(0)
{
	ZeroMemory(m_szSeasonName, sizeof(TTCHAR) * SEASON_NAME_LENGTH);
}


CClanMatchSeason::~CClanMatchSeason()
{
}

void CClanMatchSeason::SetSeason(CLAN_MATCH_SEASON* pMatchSeason)
{
	m_i32SeasonIndex	= pMatchSeason->m_i32SeasonIndex;
	m_ui32StartDate		= pMatchSeason->m_ui32StartDate;
	m_ui32EndDate		= pMatchSeason->m_ui32EndDate;

	i3mem::Copy(m_szSeasonName, pMatchSeason->m_szSeasonName, sizeof(TTCHAR) * SEASON_NAME_LENGTH);
}

void CClanMatchSeason::MakePacket(CLAN_MATCH_SEASON* pMatchSeason)
{
	pMatchSeason->m_i32SeasonIndex	= m_i32SeasonIndex;
	pMatchSeason->m_ui32StartDate	= m_ui32StartDate;
	pMatchSeason->m_ui32EndDate		= m_ui32EndDate;

	i3mem::Copy(pMatchSeason->m_szSeasonName, m_szSeasonName, sizeof(TTCHAR) * SEASON_NAME_LENGTH);
}
