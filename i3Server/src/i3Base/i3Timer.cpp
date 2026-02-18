#include "i3CommonType.h"
#include "i3Timer.h"
#include "i3Debug.h"
#include "i3Macro.h"

#if defined( I3_WINDOWS)
	#if defined( I3_COMPILER_VC)
		#pragma warning( push)
		#pragma warning( disable : 4201)
	#endif

	#include "Mmsystem.h"

	#if defined( I3_COMPILER_VC)
		#pragma warning( pop)
	#endif
#endif

// AMD Dual-Core 시스템에서 나타나는 버그로 QueryPerformanceCounter 함수에서 균일한 값을 반환하지 못하는 경우가 있음.
//
// http://support.microsoft.com/kb/909944
// http://msdn2.microsoft.com/en-us/library/bb173458.aspx
//
// 관련하여 AMD에서 아래의 링크로 CPU Driver를 배포하고 있음.
//
// http://www.amd.com/us-en/Processors/TechnicalResources/0,,30_182_871_13118,00.html



#if defined( I3_PSP)

#elif defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WINCE )
static BOOL			s_bResRetrevied = FALSE;
static double		s_dSecRes;
static double		s_dRes;
#endif

I3_CLASS_INSTANCE( i3Timer, i3ElementBase);

i3Timer::i3Timer(void)
{
	m_bTimerInit = FALSE;
}

i3Timer::~i3Timer(void)
{
}

REAL32 i3Timer::GetDeltaSec(void)
{
	REAL32 dt;
	UINT32 uiTime = timeGetTime();

	if( m_bTimerInit == FALSE)
	{
		dt = 0.0f;
		m_bTimerInit = TRUE;
	}
	else
	{
		dt = (REAL32)((uiTime - m_uiOldTime) * 0.001f);
	}

	m_uiOldTime = uiTime;

	return dt;
}

/*
REAL32 i3Timer::GetDeltaSec(void)
{
	REAL32 dt;

#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3_WINCE )
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
	
#elif defined( I3G_PSP)
	SceRtcTick		Tick;

	sceRtcGetCurrentTick( &Tick);

	if( m_bTimerInit == FALSE)
	{
		dt = 0.0f;
		m_bTimerInit = TRUE;
	}
	else
	{
		UINT32 dTime;

		if( (Tick.tick - m_OldTick) <= 0xFFFFFFFFLL)
		{
			dTime = (UINT32)((INT64)Tick.tick - m_OldTick);
		}
		else
		{
			// Time Overflow
			dTime = 0;
		}

        dt = (REAL32)dTime * 1.0E-6f;						// microsecond를 secons로 변환
	}

	m_OldTick = Tick.tick;

#elif defined( I3_WIPI)		//	05.06.20 by KOMET
	if( m_bTimerInit == FALSE)
	{
		dt = 0.0f;
		m_bTimerInit = TRUE;
	}
	else
	{
		INT32 dTime;

		dTime = (INT32)((INT64)MC_knlCurrentTime() - m_OldTick);
		dt = (REAL32)dTime * 1.0E-3f; //0.001f;						// millisecond를 secons로 변환
	}

	m_OldTick = MC_knlCurrentTime();
#endif

	return dt;
}
*/

INT64 i3Timer::GetProfileTime(void)
{
#if defined( I3G_DX) || defined( I3G_XBOX) || defined( I3_WINCE )
	INT64	time;

	QueryPerformanceCounter( (LARGE_INTEGER *) &time);

	return time;
	
#elif defined( I3G_PSP)
	SceRtcTick		Tick;

	sceRtcGetCurrentTick( &Tick);

	return Tick.tick * 1000L;			// microsecond를 nanosecond로 환산

#elif defined( I3_WIPI)		//	05.06.20 by KOMET
	return MC_knlCurrentTime() * 1000000L;	// millisecond를 nanosecond로 환산

#endif
}

double i3Timer::GetProfileResolution(void)
{
	if( s_bResRetrevied == FALSE)
	{
		INT64 res;

		QueryPerformanceFrequency( (LARGE_INTEGER *) &res);

		s_dSecRes = 1.0 / res;
		s_dRes = 1.0E+3 / res;
		s_bResRetrevied = TRUE;
	}

	return s_dRes;
}

INT64 i3Timer::GetPerformanceTime( void)	//	by KOMET
{
	INT64 time = 0;

#if defined( I3G_DX) || defined( I3G_XBOX)
	QueryPerformanceCounter( (LARGE_INTEGER *) &time);
#endif

	return time;
}

void i3Timer::Sleep( UINT32 ms)
{
#if defined( I3_WINDOWS)
	SleepEx( ms, TRUE);
#elif defined( I3_LINUX)
	struct timespec tv;

	tv.tv_sec = 0;
	tv.tv_nsec = ms * 1000000;

	nanosleep( &tv, &tv);
#endif
}