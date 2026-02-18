#include "stdafx.h"
#include "Timer.h"

#pragma warning( push)
#pragma warning( disable : 4201)
#include "Mmsystem.h"
#pragma warning( pop)

// AMD Dual-Core 시스템에서 나타나는 버그로 QueryPerformanceCounter 함수에서 균일한 값을 반환하지 못하는 경우가 있음.
//
// http://support.microsoft.com/kb/909944
// http://msdn2.microsoft.com/en-us/library/bb173458.aspx
//
// 관련하여 AMD에서 아래의 링크로 CPU Driver를 배포하고 있음.
//
// http://www.amd.com/us-en/Processors/TechnicalResources/0,,30_182_871_13118,00.html

static BOOL			s_bResRetrevied = FALSE;
static double		s_dSecRes;
static double		s_dRes;

NSM_cTimer::NSM_cTimer(void)
{
	m_bTimerInit	= FALSE;
	m_OldTick		= 0; 
}

NSM_cTimer::~NSM_cTimer(void)
{

}

REAL32 NSM_cTimer::GetDeltaSec(void)
{
	REAL32 dt;
	INT64	time;

	QueryPerformanceCounter( (LARGE_INTEGER *) &time);

	if( s_bResRetrevied == FALSE)
	{
		INT64 res;

		QueryPerformanceFrequency( (LARGE_INTEGER *) &res);

		s_dSecRes = 1.0 / res;
		s_dRes = 1.0E+3 / res;
		s_bResRetrevied = TRUE;
	}

	if( m_bTimerInit == FALSE)
	{
		dt = 0.0f;
		m_bTimerInit = TRUE;
	}
	else
	{
		dt = (REAL32)((time - m_OldTick) * s_dSecRes);
	}

	m_OldTick = time;	

	return dt;
}

