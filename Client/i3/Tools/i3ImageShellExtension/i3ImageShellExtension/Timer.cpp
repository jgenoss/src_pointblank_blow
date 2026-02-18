#include <Windows.h>
#include "Timer.h"

#pragma warning( push)
#pragma warning( disable : 4201)
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

Timer::Timer(void)
{
	m_bTimerInit = false;
	m_OldTick = 0;
	m_uiOldTime = 0;
}

Timer::~Timer(void)
{
}

double Timer::GetDeltaSec(void)
{
	double dt;
	UINT32 uiTime = timeGetTime();

	if (m_bTimerInit == false)
	{
		dt = 0.0f;
		m_bTimerInit = true;
	}
	else
	{
		dt = (double)((uiTime - m_uiOldTime) * 0.001f);
	}

	m_uiOldTime = uiTime;

	return dt;
}

double Timer::GetFixedDeltaSec(void)
{
	double dt;
	UINT32 uiTime = timeGetTime();

	if (m_bTimerInit == false)
	{
		dt = 0.0f;
		m_bTimerInit = true;
	}
	else
	{
		dt = (double)((uiTime - m_uiOldTime) * 0.001f);
		if (dt < 0.0166f)
		{
			double tt = (0.0166f - dt) * 1000;
			Sleep((UINT32)tt);
			uiTime = timeGetTime();
			dt = (double)((uiTime - m_uiOldTime) * 0.001f);
		}
	}

	m_uiOldTime = uiTime;

	return dt;
}

INT64 Timer::GetProfileTime(void)
{
	INT64	time;

	QueryPerformanceCounter((LARGE_INTEGER *)&time);

	return time;
}

double Timer::GetProfileResolution(void)
{
	if (s_bResRetrevied == false)
	{
		INT64 res;

		QueryPerformanceFrequency((LARGE_INTEGER *)&res);

		s_dSecRes = 1.0 / res;
		s_dRes = 1.0E+3 / res;
		s_bResRetrevied = true;
	}

	return s_dRes;
}

INT64 Timer::GetPerformanceTime(void)	//	by KOMET
{
	INT64 time = 0;

#if defined( I3G_DX) || defined( I3G_XBOX)
	QueryPerformanceCounter((LARGE_INTEGER *)&time);
#endif

	return time;
}

void Timer::Sleep(UINT32 ms)
{
#if defined( I3_WINDOWS)
	SleepEx(ms, true);
#elif defined( I3_LINUX)
	struct timespec tv;

	tv.tv_sec = 0;
	tv.tv_nsec = ms * 1000000;

	nanosleep(&tv, &tv);
#endif
}