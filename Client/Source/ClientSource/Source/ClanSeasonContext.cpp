#include "pch.h"
#include "ClanSeasonContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

void	CLAN_SEASON_INFO::ParseData( const CLAN_MATCH_SEASON_EXT & ClanMatchSeason )
{
	_wstrSeasonName = ClanMatchSeason.m_szSeasonName;
//	_SeasonName = ClanMatchSeason.m_szSeasonName;
	_StartDate = TimeUtil::ToSystemTime(ClanMatchSeason.m_ui32StartDate);
	_EndDate = TimeUtil::ToSystemTime(ClanMatchSeason.m_ui32EndDate);
}


const bool	CLAN_SEASON_INFO::IsValidSeason(void) const
{
	// 시작&종료 월/일이 있어야 유효하다..
	if( _StartDate.wMonth > 0 && _StartDate.wDay > 0 && _EndDate.wMonth > 0 && _EndDate.wDay > 0 )
		return true;

	return false;
}

/************************************************************************/
/*						   클랜 이전 시즌 관련			                */
/************************************************************************/

void CLAN_PRESEASON_INFO::ParseData( const CLAN_MATCH_SEASON_EXT & ClanMatchSeason )
{
	_StartDate = TimeUtil::ToSystemTime(ClanMatchSeason.m_ui32PreSeasonStartDate);
	_EndDate = TimeUtil::ToSystemTime(ClanMatchSeason.m_ui32PreSeasonEndDate);
}

const CLAN_PRESEASON_RANK_INFO * CLAN_PRESEASON_INFO::GetSeasonRankInfo(const INT32 Ranking)
{
	i3::vector<CLAN_PRESEASON_RANK_INFO>::iterator iter = nullptr;

	size_t count = _vecSeasonRank.size();

	for(size_t idx = 0 ; idx < count ; ++idx)
	{
		iter = &_vecSeasonRank.at(idx);
		if( iter->_Ranking == Ranking )
			return iter;
	}

	return nullptr;
}

const CLAN_PRESEASON_RANK_INFO * CLAN_PRESEASON_INFO::GetSeasonRankInfo( const UINT32 ui32ClanIdx, const TTCHAR * szClanName )
{
	i3::vector<CLAN_PRESEASON_RANK_INFO>::iterator iter = nullptr;

	size_t count = _vecSeasonRank.size();

	for( size_t idx = 0 ; idx < count ; ++idx)
	{
		iter = &_vecSeasonRank.at(idx);
		if( iter->IsSameClan(ui32ClanIdx, szClanName) )
			return iter;
	}

	return nullptr;
}

/************************************************************************/

namespace 
{
	UINT32 g_check_time[SN_TYPE_MAX] = { 3600, 1800, 300 };
};

SeasonChecker::SeasonChecker(void) : m_eNoticeType(SN_TYPE_NONE)
{

}

SeasonChecker::~SeasonChecker(void)
{

}

UINT32 SeasonChecker::__SetNoticeData( const SYSTEMTIME & CurTime, const SYSTEMTIME & EndTime)
{
	UINT32 diff_sec = TimeUtil::CalcTimeDifference(CurTime, EndTime);
	
	for( INT32 idx = 0 ; idx < SN_TYPE_MAX ; ++idx)
	{
		if( __InCheckTimeOffset(diff_sec, g_check_time[idx]) )
		{
			m_eNoticeType = static_cast<SEASONEND_NOTICE>(idx);
			break;
		}
	}

	return diff_sec;
}

bool SeasonChecker::__InCheckTimeOffset(const UINT32 diff_sec, const UINT32 chk_time)
{
	// offset 값
	UINT32 max = chk_time + 60;
	return ( diff_sec < max) && ( diff_sec >= chk_time);
}

SEASONEND_NOTICE SeasonChecker::CheckToEndNotice(const SYSTEMTIME & EndTime)
{
	SYSTEMTIME	LocalTime;
	GetLocalTime(&LocalTime);

	UINT32 diff_sec = __SetNoticeData(LocalTime, EndTime);
	SEASONEND_NOTICE type = SN_TYPE_NONE;

	if(m_eNoticeType != SN_TYPE_NONE)
	{
		if( diff_sec < g_check_time[m_eNoticeType] )
		{
			type = m_eNoticeType;
			m_eNoticeType = SN_TYPE_NONE;
		}
	}

	return type;
}

bool SeasonChecker::IsPossibleMatch(const SYSTEMTIME & EndTime, const UINT32 chk_time)
{
	SYSTEMTIME	LocalTime;
	GetLocalTime(&LocalTime);

	UINT32 diff_sec = TimeUtil::CalcTimeDifference(LocalTime, EndTime);
	return (diff_sec > chk_time) ? true : false;
}

/************************************************************************/

ClanSeasonContext::ClanSeasonContext()
{
	m_PreSeaonInfo.Reset();
}

ClanSeasonContext::~ClanSeasonContext()
{
	m_PreSeaonInfo.Reset();
}

void ClanSeasonContext::SetPreSeasonInfo( const i3::rc_wstring & wstr, CLAN_BASIC_INFO * pInfo, INT32 i32Count )
{
	m_PreSeaonInfo.SetSeasonName(wstr);

	for(INT32 idx = 0 ; idx < i32Count ; ++idx)
	{
		const CLAN_PRESEASON_RANK_INFO * pPreSeasonInfo = m_PreSeaonInfo.GetSeasonRankInfo(pInfo[idx]._idx, pInfo[idx]._name);
		if( pPreSeasonInfo == nullptr)
			m_PreSeaonInfo.SetSeasonRankInfo( CLAN_PRESEASON_RANK_INFO(pInfo[idx], idx + 1) );
	}
}

bool ClanSeasonContext::CheckToSeasonEndNotice(i3::rc_wstring & out_str)
{
	SEASONEND_NOTICE type = m_SeasonChecker.CheckToEndNotice(m_CurrentSeason._EndDate); 

	if( type != SN_TYPE_NONE )
	{
		UINT32 time = 0;

		switch( type )
		{
		case SN_TYPE_1HOUR :
			{
				time = ConvertSecToUseHour(g_check_time[type]);
				i3::sprintf( out_str, GAME_RCSTRING("STR_SEASON_END_HOUR"), m_CurrentSeason.GetSeasonName(), time);
			}
			break;
		case SN_TYPE_30MINUTE :
		case SN_TYPE_5MINUTE :	
			{
				time = ConvertSecToUseMinute(g_check_time[type]);
				i3::sprintf( out_str, GAME_RCSTRING("STR_SEASON_END_MINUTE"), m_CurrentSeason.GetSeasonName(), time);
			}	
			break;
		}

		return true;
	}

	return false;
}

bool ClanSeasonContext::IsPossibleSeasonMatch( void)
{
	return m_SeasonChecker.IsPossibleMatch(m_CurrentSeason._EndDate, g_check_time[SN_TYPE_5MINUTE] );
}

const CLAN_PRESEASON_RANK_INFO * ClanSeasonContext::GetMyClanPreSeasonInfo(void)
{
	USER_INFO_BASIC MyInfo;
	UserInfoContext::i()->GetMyUserInfoBasic( &MyInfo);
	
	return m_PreSeaonInfo.GetSeasonRankInfo( MyInfo.m_ui32Clanidx, MyInfo.m_strClanName );
}

const CLAN_PRESEASON_RANK_INFO * ClanSeasonContext::GetClanPreSeasonInfo(INT32 i32Ranking)
{
	return m_PreSeaonInfo.GetSeasonRankInfo(i32Ranking);
}

bool ClanSeasonContext::IsMyClanInRanking(void)
{
	USER_INFO_BASIC MyInfo;
	UserInfoContext::i()->GetMyUserInfoBasic( &MyInfo);
	return m_PreSeaonInfo.GetSeasonRankInfo( MyInfo.m_ui32Clanidx, MyInfo.m_strClanName ) != nullptr;
}