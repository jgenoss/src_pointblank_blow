#pragma once
#include "TimeUtil.h"

//----------------------------------------------------------------------//
//								클랜 시즌 관련							//
//----------------------------------------------------------------------//

struct CLAN_SEASON_INFO
{
	i3::rc_wstring	_wstrSeasonName;
	SYSTEMTIME	_StartDate;
	SYSTEMTIME	_EndDate;

	void				ParseData( const CLAN_MATCH_SEASON_EXT & ClanMatchSeason );

	const i3::rc_wstring&	GetSeasonName( void ) const					{ return _wstrSeasonName; }
	void					SetSeasonName( const i3::rc_wstring& SeasonName )		{ _wstrSeasonName = SeasonName; }

	const SYSTEMTIME	GetSeasonStartDate( void ) const			{ return _StartDate; }
	void				SetSeasonStartDate( SYSTEMTIME StartDate )	{ _StartDate = StartDate; }

	const SYSTEMTIME	GetSeasonEndDate( void ) const				{ return _EndDate; }
	void				SetSeasonEndDate( SYSTEMTIME EndDate )		{ _EndDate = EndDate; }

	const bool			IsValidSeason(void) const;

};

//----------------------------------------------------------------------//
//							클랜 이전 시즌 관련							//
//----------------------------------------------------------------------//

struct CLAN_PRESEASON_RANK_INFO
{
	CLAN_BASIC_INFO	_ClanInfo;
	INT32			_Ranking = 0;

	CLAN_PRESEASON_RANK_INFO() { }

	CLAN_PRESEASON_RANK_INFO(const CLAN_BASIC_INFO & info, const INT32 Ranking)
	{
		_ClanInfo = info;
		_Ranking = Ranking;
	}

	bool IsSameClan( const UINT32 ui32ClanIdx, const TTCHAR * szClanName )
	{
		if( _ClanInfo._idx != ui32ClanIdx )
			return false;

		if( i3::generic_compare(_ClanInfo._name, szClanName) != 0 )
			return false;

		return true;
	}
};

struct CLAN_PRESEASON_INFO : public CLAN_SEASON_INFO
{
	i3::vector<CLAN_PRESEASON_RANK_INFO>	_vecSeasonRank;

	void Reset(void)
	{
		_wstrSeasonName = L"";
		_vecSeasonRank.clear();
		i3mem::FillZero(&_StartDate, sizeof(SYSTEMTIME));
		i3mem::FillZero(&_EndDate, sizeof(SYSTEMTIME));

	}
	void								ParseData( const CLAN_MATCH_SEASON_EXT & ClanMatchSeason );

	const CLAN_PRESEASON_RANK_INFO *	GetSeasonRankInfo( const INT32 Ranking);
	const CLAN_PRESEASON_RANK_INFO *	GetSeasonRankInfo( const UINT32 ui32ClanIdx, const TTCHAR * szClanName );

	void								SetSeasonRankInfo( const CLAN_PRESEASON_RANK_INFO & info)
	{
		_vecSeasonRank.push_back(info);
	}

	bool								IsShowRankingInfo(void)		{ return _vecSeasonRank.size() > 0; }
};

/************************************************************************/

enum SEASONEND_NOTICE
{
	SN_TYPE_NONE		= -1,
	SN_TYPE_1HOUR,		// 종료 1시간 전 안내
	SN_TYPE_30MINUTE,	// 종료 30분 전 안내
	SN_TYPE_5MINUTE,	// 종료 5분 전 안내

	SN_TYPE_MAX
};

class SeasonChecker
{
public:
	SeasonChecker(void);
	~SeasonChecker(void);

	SEASONEND_NOTICE	CheckToEndNotice(const SYSTEMTIME & EndTime);
	bool				IsPossibleMatch(const SYSTEMTIME & EndTime, const UINT32 chk_time);

private:
	UINT32				__SetNoticeData( const SYSTEMTIME & CurTime, const SYSTEMTIME & EndTime);
	bool				__InCheckTimeOffset( const UINT32 diff_sec, const UINT32 chk_time);

private:
	SEASONEND_NOTICE	m_eNoticeType;
};

class ClanSeasonContext : public i3::shared_ginst<ClanSeasonContext>
{
public:
	ClanSeasonContext();
	~ClanSeasonContext();

	void						SetSeasonInfo( const CLAN_MATCH_SEASON_EXT & ClanMatchSeason )
	{
		m_CurrentSeason.ParseData(ClanMatchSeason);
		m_PreSeaonInfo.ParseData(ClanMatchSeason);
	}
	const CLAN_SEASON_INFO *	GetCurrentSeason( void ) const		{ return &m_CurrentSeason; }

	bool						CheckToSeasonEndNotice( i3::rc_wstring & out_str );
	bool						IsPossibleSeasonMatch( void);

	//----------------------------------------------------------------------//
	//							클랜 이전 시즌 관련							//
	//----------------------------------------------------------------------//
	void								SetPreSeasonInfo( const i3::rc_wstring & wstr, CLAN_BASIC_INFO * pInfo, INT32 i32Count );
	const CLAN_PRESEASON_INFO *			GetPreSeasonInfo( void ) const { return &m_PreSeaonInfo; } 

	const i3::rc_wstring &				GetPreSeasonName(void)	const		{ return m_PreSeaonInfo.GetSeasonName(); }
	const CLAN_PRESEASON_RANK_INFO * 	GetMyClanPreSeasonInfo(void);
	const CLAN_PRESEASON_RANK_INFO * 	GetClanPreSeasonInfo(INT32 i32Ranking);
	
	bool								IsShowPreSeaonRanking(void)			{ return m_PreSeaonInfo.IsShowRankingInfo(); }
	bool								IsMyClanInRanking(void);

private:

	CLAN_SEASON_INFO				m_CurrentSeason;
	SeasonChecker					m_SeasonChecker;

	CLAN_PRESEASON_INFO				m_PreSeaonInfo;
};
