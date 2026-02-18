#include "pch.h"

#ifdef	DOMI_UTIL

#include "UIHUD_Domination_MessageTimer.h"


CMsgTimer::CMsgTimer(void) : m_bEnable( false), m_rRemainTime( 0.0f), m_rLimitTime( 0.0f), 
	m_nCount( 0), m_nLastCount( 0 )
{
}

CMsgTimer::~CMsgTimer(void)
{
}


TIMER_STATE CMsgTimer::OnUpdate( REAL32 rDeltaSec)
{
	if( IsEnable() == false ) return TIMER_STATE_ENDED;

	m_rRemainTime	-= rDeltaSec;

	if( m_rRemainTime <= 0.0f)
	{
		return (this->*m_pfUpdater)( rDeltaSec);
	}

	return TIMER_STATE_PROCESS;
}

TIMER_STATE	CMsgTimer::_UpdateCount( REAL32 rDeltaSec)
{
	m_nCount--;

	if( m_nCount > m_nLastCount )
	{
		m_rRemainTime += m_rLimitTime;
		return TIEMR_STATE_COUNTEND;
	}
	
	m_bEnable	= false;
	return TIMER_STATE_END;
}

TIMER_STATE	CMsgTimer::_UpdateOnce( REAL32 rDeltaSec)
{
	m_bEnable	= false;
	return TIMER_STATE_END;
}


void CMsgTimer::SetTimer( REAL32 rLimitTime, INT32 nCount, bool bNonCountZero )
{
	m_rRemainTime	= m_rLimitTime	= rLimitTime;	
	m_nCount		= nCount;

	if( nCount > 0 )
	{
		m_pfUpdater	= &CMsgTimer::_UpdateCount;
	}
	else
	{
		m_pfUpdater	= &CMsgTimer::_UpdateOnce;

		// 카운트가 0 까지 셀지 세지 않을지 설정
		if( bNonCountZero )	m_nLastCount	= 0;
		else				m_nLastCount	= -1;
	}

	m_bEnable		= true;
}

#endif