#include "pch.h"
#include "CapsuleRand.h"

I3_CLASS_INSTANCE(CCapsuleRand, i3ElementBase);

extern	boost::BOOST_RANDOM_ENGINE		g_RandomEngine;

CCapsuleRand::CCapsuleRand()
{
	m_ui8Capsule	= NULL;
	m_i32CountMax	= 0;
}

CCapsuleRand::CCapsuleRand( INT32 i32Count )
{
	m_ui8Capsule	= NULL;
	m_i32CountMax	= 0;

	Create( i32Count );
}

CCapsuleRand::~CCapsuleRand()
{
	I3MEM_SAFE_FREE( m_ui8Capsule );
}

BOOL	CCapsuleRand::Create( INT32 i32Count )
{
	if( m_ui8Capsule )	return FALSE;
	if( 0 == i32Count )	return FALSE;

	m_ui8Capsule	= (UINT8*)i3MemAlloc( sizeof(UINT8) * i32Count ); 
	m_i32CountMax	= 0;

	return TRUE;
}

void	CCapsuleRand::Insert( INT32 iCount, UINT8 ui8Value )
{
	if( NULL == m_ui8Capsule )	return;

	for( INT32 i = 0 ; i < iCount ; i++ )
	{
		m_ui8Capsule[ m_i32CountMax ] = ui8Value;
		m_i32CountMax++;
	}
}

void	CCapsuleRand::ShuffleMix()
{
	if( NULL == m_ui8Capsule )	return;

	for( INT32 i = 0 ; i < m_i32CountMax ; i++ )
	{	// 임의로 하나를 정해서 두개의 위치를 바꿔준다. 랜덤유도
		INT32 i32Idx = rand() % m_i32CountMax;

		UINT8 ui8Temp = m_ui8Capsule[ i32Idx ];
		m_ui8Capsule[ i32Idx ] = m_ui8Capsule[ i ];
		m_ui8Capsule[ i ] = ui8Temp;
	}
}

UINT8	CCapsuleRand::Rand()
{
	if( NULL == m_ui8Capsule )	return 0;

	boost::uniform_int<>	Range( 0, m_i32CountMax - 1 );
	boost::variate_generator< boost::BOOST_RANDOM_ENGINE&, boost::uniform_int<> > GenerateRandomNumber( g_RandomEngine, Range );

	return m_ui8Capsule[ GenerateRandomNumber() ];
}