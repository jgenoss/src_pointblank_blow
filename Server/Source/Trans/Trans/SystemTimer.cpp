#include "pch.h"
#include "SystemTimer.h"

CSystemTimer::CSystemTimer()
{
}

CSystemTimer::~CSystemTimer()
{
}

bool CSystemTimer::Create()
{
	LARGE_INTEGER qwTicksPerSec, qwTime; 

	if( FALSE == QueryPerformanceFrequency( &qwTicksPerSec ) )	return false;

	m_i64QPFTicksPerSec = qwTicksPerSec.QuadPart; 

	QueryPerformanceCounter( &qwTime ); 

	m_i64LastElapsedTime = qwTime.QuadPart;

	return true;
}

REAL32 CSystemTimer::RenewElapsedTime()
{
	LARGE_INTEGER qwTime; 

	QueryPerformanceCounter( &qwTime );

	REAL32 fElapsedTime = (REAL32)((double) ( qwTime.QuadPart - m_i64LastElapsedTime ) / (double) m_i64QPFTicksPerSec); 

	m_i64LastElapsedTime = qwTime.QuadPart; 

	return fElapsedTime;
}