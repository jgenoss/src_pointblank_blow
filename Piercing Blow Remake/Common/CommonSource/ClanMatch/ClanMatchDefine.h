#pragma once


#define CLAN_NUM_PER_PAGE		10
#define SEASON_NAME_LENGTH		50




//enum MATCH_RESULT
//{
//	MATCH_DRAW				= 1,
//	MATCH_LOSE				,
//	MATCH_WIN				,
//
//	MATCH_RESULT_COUNT		,
//};


#pragma pack(push, 1)

struct CLAN_MATCH_SEASON
{
	// 현재 시즌 정보
	INT32				m_i32SeasonIndex;
	TTCHAR				m_szSeasonName[SEASON_NAME_LENGTH];
	UINT32				m_ui32StartDate;
	UINT32				m_ui32EndDate;
};

struct CLAN_MATCH_SEASON_EXT : public CLAN_MATCH_SEASON
{
	// 이전 시즌 날짜.
	UINT32				m_ui32PreSeasonStartDate;
	UINT32				m_ui32PreSeasonEndDate;
};

struct CLAN_RANKING
{
	INT32				m_i32Ranking;
	INT32				m_i32ClanID;
	UINT16				m_ui16OnlineMember;
	UINT16				m_ui16TotalMember;
	REAL64				m_r64RatingMean;
};
#pragma pack(pop)

