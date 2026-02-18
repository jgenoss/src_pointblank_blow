#include "pch.h"

#ifdef	DOMI_UTIL

#include "CoolTimeGuage.h"


//--------------------------------------------------------
// CCoolTimeGuage
//--------------------------------------------------------
CCoolTimeGuage::CCoolTimeGuage(void) :
	m_bEnable( false),
	m_rRate( 0.0f),
	m_rRemainCool( 0.0f),
	m_rSetCool( 0.0f)
{
}
CCoolTimeGuage::~CCoolTimeGuage(void)
{
}


bool CCoolTimeGuage::OnUpdate( REAL32 tm )
{
	if( m_rRemainCool <= 0.0f ) return false;

	m_rRemainCool	-= tm;
	if( m_rRemainCool < 0.0f ) m_rRemainCool	= 0.0f;

	m_rRate	= ( m_rSetCool <= 0.0f) ? 0.0f : (m_rRemainCool / m_rSetCool);

	return true;
}

bool CCoolTimeGuage::OnUpdate( REAL32 tm, REAL32 rRate)
{
	if( !m_bEnable) return false;
	
	// 전체 쿨타임의 경우 
	if( OnUpdate( tm ) )
	{
		if( m_rRate < rRate )
		{
			m_rRemainCool	= 0.0f;
			m_rRate			= rRate;
		}
	}
	else
	{
		m_rRate		= rRate;
	}

	return true;
}


void CCoolTimeGuage::SetClose(void)
{
	m_bEnable		= false;
	m_rRate			= 1.0f;
	m_rRemainCool	= 0.0f;
}

void CCoolTimeGuage::SetCoolTime( REAL32 rTime )
{
	m_bEnable		= true;
	m_rRemainCool	= m_rSetCool	= rTime;
}



//--------------------------------------------------------
// CCoolEffect
//--------------------------------------------------------
REAL32	CCoolEffect::m_rRateMin		= 1.0f;
REAL32	CCoolEffect::m_rRateMax		= 1.5f;
REAL32	CCoolEffect::m_rSpeed		= 3.0f;		// 초당 커지는 크기 - 크기 / 1초


CCoolEffect::CCoolEffect(void) :
	m_bEnable( false), m_State( STATE_NONE )
{
}
CCoolEffect::~CCoolEffect(void)
{
}


bool CCoolEffect::OnUpdate( REAL32 tm )
{
	if( !m_bEnable) return false;

	switch( m_State )
	{
	case	STATE_INC :
		m_rRate	+= m_rSpeed * tm;
		if( m_rRate >= m_rRateMax )
		{
			m_rRate		= m_rRateMax;
			m_State		= STATE_DEC;
		}
		break;

	case	STATE_DEC :
		m_rRate	-= m_rSpeed * tm;
		if( m_rRate <= m_rRateMin )
		{
			m_rRate		= m_rRateMax;
			m_State		= STATE_NONE;
			m_bEnable	= false;
		}
		break;

	default:
		m_State		= STATE_NONE;
		m_bEnable	= false;
		return false;
	}

	return true;
}


#endif