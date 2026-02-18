#pragma once

#include "ClanMatchDefine.h"

class CClanMatchSeason
{
public:
	void				SetSeason(CLAN_MATCH_SEASON* pMatchSeason);
	void				MakePacket(CLAN_MATCH_SEASON* pMatchSeason);

	INT32				GetSeasonIndex()								{ return m_i32SeasonIndex; }
	TTCHAR*				GetSeasonName()									{ return m_szSeasonName; }
	UINT32				GetSeasonStartDate()							{ return m_ui32StartDate; }
	UINT32				GetSeasonEndDate()								{ return m_ui32EndDate; }


public:
	CClanMatchSeason();
	virtual ~CClanMatchSeason();

private:
	INT32				m_i32SeasonIndex;
	TTCHAR				m_szSeasonName[SEASON_NAME_LENGTH];
	UINT32				m_ui32StartDate;
	UINT32				m_ui32EndDate;
};
