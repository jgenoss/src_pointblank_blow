#include "Profiler.h"
#include "Timer.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>

#define			MAX_PROFILERECORD				100

typedef void(*PF_CALLBACK)(INT32 slot, const char * pszName, INT64	callCount, double elapsed, double elapseMin, double elapseMax, double elapseAvg, void * pUserData);

typedef struct _tagProfileRec
{
	bool		m_bOnceOut;

	char		m_szTitle[128];
	INT64		m_CallCount;
	INT64		m_tmElapsed;

	INT64		m_TempCallCount;
	INT64		m_tmStart;
	INT64		m_tmAccm;
	INT64		m_tmTempMin;
	INT64		m_tmTempMax;

	INT64		m_TotalCallCount;
	INT64		m_TotalElapsed;
	INT64		m_tmMin;
	INT64		m_tmMax;
	INT64		m_tmAvg;
} PROFILE_INFO;

static PROFILE_INFO	g_PfInfo[MAX_PROFILERECORD];


void INIT_PROFILE(void)
{
	memset(g_PfInfo, 0, sizeof(g_PfInfo));
}


void SET_PROFILE(INT32 slot, const char * pszStr)
{
	strncpy_s(g_PfInfo[slot].m_szTitle, pszStr, sizeof(g_PfInfo[slot].m_szTitle) - 1);
}


void START_PROFILE(INT32 slot)
{
	g_PfInfo[slot].m_tmStart = Timer::GetProfileTime();;
}


void END_PROFILE(INT32 slot)
{
	INT64 gap;

	g_PfInfo[slot].m_TempCallCount++;

	gap = Timer::GetProfileTime() - g_PfInfo[slot].m_tmStart;
	g_PfInfo[slot].m_tmAccm += gap;

	if (g_PfInfo[slot].m_bOnceOut == false)
	{
		g_PfInfo[slot].m_tmTempMin = g_PfInfo[slot].m_tmTempMax = gap;
	}
	else
	{
		g_PfInfo[slot].m_tmTempMin = MIN(g_PfInfo[slot].m_tmTempMin, gap);
		g_PfInfo[slot].m_tmTempMax = MAX(g_PfInfo[slot].m_tmTempMax, gap);
	}

	g_PfInfo[slot].m_bOnceOut = true;
}


bool CHECK_PROFILE(INT32 slot, double sec)
{
	double timeRes = Timer::GetProfileResolution();

	return ((g_PfInfo[slot].m_tmAccm * timeRes) >= sec);
}


void RESET_PROFILE(bool bClearHistory)
{
	INT32 i;

	for (i = 0; i < MAX_PROFILERECORD; i++)
	{
		g_PfInfo[i].m_CallCount = g_PfInfo[i].m_TempCallCount;
		g_PfInfo[i].m_TotalCallCount += g_PfInfo[i].m_TempCallCount;
		g_PfInfo[i].m_tmElapsed = g_PfInfo[i].m_tmAccm;
		g_PfInfo[i].m_TotalElapsed += g_PfInfo[i].m_tmAccm;

		if (bClearHistory)
		{
			g_PfInfo[i].m_tmMin = 0;
			g_PfInfo[i].m_tmMax = 0;
			g_PfInfo[i].m_tmAvg = 0;
			g_PfInfo[i].m_bOnceOut = false;
		}
		else
		{
			g_PfInfo[i].m_tmMin = MIN(g_PfInfo[i].m_tmMin, g_PfInfo[i].m_tmAccm);
			g_PfInfo[i].m_tmMax = MAX(g_PfInfo[i].m_tmMax, g_PfInfo[i].m_tmAccm);

			if (g_PfInfo[i].m_TempCallCount > 0)
				g_PfInfo[i].m_tmAvg = g_PfInfo[i].m_TotalElapsed / g_PfInfo[i].m_TotalCallCount;
			else
				g_PfInfo[i].m_tmAvg = 0;
		}

		g_PfInfo[i].m_TempCallCount = 0;
		g_PfInfo[i].m_tmStart = 0;
		g_PfInfo[i].m_tmAccm = 0;
		g_PfInfo[i].m_tmTempMin = 0x7FFFFFFFFFFFFFFF;	//	가장 큰수 
		g_PfInfo[i].m_tmTempMax = 0x8000000000000000;	//	가장 작은수
	}
}

static char tempstr[0xFFFF];
static char tempstr2[0xFFFF];

void I3PRINTF(const char *format, ...)
{
	va_list marker;

	va_start(marker, format);

	vsprintf_s(tempstr, format, marker);

	va_end(marker);

	OutputDebugStringA(tempstr);
}

#define		SPTR( str)			if( pszDest == NULL)	I3PRINTF( str);	else	strcat_s( pszDest, MAX_PATH, str)


void REPORT_PROFILE(char * pszDest)
{
	INT32 i;
	double	timeRes = Timer::GetProfileResolution();
	char conv[256];

	sprintf_s(conv, "----------------------- Recorded Profile (ms) -----------------------------\n");
	SPTR(conv);

	for (i = 0; i < MAX_PROFILERECORD; i++)
	{
		if (g_PfInfo[i].m_bOnceOut)
		{
			sprintf_s(conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n",
				i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount,
				(timeRes * g_PfInfo[i].m_tmElapsed),
				(timeRes * g_PfInfo[i].m_tmMin),
				(timeRes * g_PfInfo[i].m_tmMax),
				(timeRes * g_PfInfo[i].m_tmAvg));

			SPTR(conv);
		}
	}

	sprintf_s(conv, "---------------------------------------------------------------------------\n");
	SPTR(conv);
}


void REPORT_PROFILE_CALLBACK(PF_CALLBACK pUserProc, void * pData)
{
	INT32 i;
	double	timeRes = Timer::GetProfileResolution();

	for (i = 0; i < MAX_PROFILERECORD; i++)
	{
		if (g_PfInfo[i].m_bOnceOut)
		{
			pUserProc(i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount,
				(timeRes * g_PfInfo[i].m_tmElapsed),
				(timeRes * g_PfInfo[i].m_tmMin),
				(timeRes * g_PfInfo[i].m_tmMax),
				(timeRes * g_PfInfo[i].m_tmAvg), pData);
		}
	}
}

 INT64	PF_GetCallCount(INT32 slot)
{
	return g_PfInfo[slot].m_CallCount;
}

 double	PF_GetElapsed(INT32 slot)
{
	double	timeRes = Timer::GetProfileResolution();

	return timeRes * g_PfInfo[slot].m_tmElapsed;
}