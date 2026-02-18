#include "pch.h"
#include "SpeedState.h"
#include "hms.h"

CSpeedState::CSpeedState()
{
	Reset();
}
CSpeedState::~CSpeedState()
{
}

void CSpeedState::Reset()
{
	m_ui32Count = 0;
	i3mem::FillZero(m_ar32SpeedData, sizeof(m_ar32SpeedData));
}

void CSpeedState::InsertSpeed( REAL32 r32Speed )
{
	// 파라미터 check 하고 들어옴 
	UINT32 idx = m_ui32Count % ( sizeof(m_ar32SpeedData) / sizeof(m_ar32SpeedData[0]) );
	m_ar32SpeedData[idx] = r32Speed;
	m_ui32Count++; // overflow safe
}

BOOL CSpeedState::IsSpeedHack(REAL32* pr32AvgSpeed)
{
	I3ASSERT( pr32AvgSpeed != NULL);

	UINT32 ui32Size = sizeof(m_ar32SpeedData) / sizeof(m_ar32SpeedData[0]);
	I3ASSERT( ui32Size >= 3);
	if( m_ui32Count < ui32Size ) return FALSE;

	double dSumOfSpeed = 0.0f;
	double dMaxSpeed   = 0.0f;
	for( UINT32 i = 0; i < ui32Size; i++ )
	{
		if( m_ar32SpeedData[i] > dMaxSpeed ) dMaxSpeed = m_ar32SpeedData[i];
		dSumOfSpeed += m_ar32SpeedData[i];
	}

	dSumOfSpeed -= dMaxSpeed; // max값 제외하고 평균을 구한다
	double dAvgSpeed = dSumOfSpeed / ( ui32Size - 1 );
	//I3TRACE( "%d, avg speed = %f\n", m_ui32Count, dAvgSpeed );
	if(  dAvgSpeed > MAX_SPEED )
	{
		*pr32AvgSpeed = (REAL32)dAvgSpeed;
		return TRUE;
	}

	return FALSE;
}