#include "i3CommonType.h"
#include "i3Timer.h"
#include "i3Debug.h"

#if !defined( I3_NO_PROFILE)
#define			MAX_PROFILERECORD				100

typedef void	(*PF_CALLBACK)( INT32 slot, const char * pszName, INT64	callCount, REAL64 elapsed, REAL64 elapseMin, REAL64 elapseMax, REAL64 elapseAvg, void * pUserData);

struct PROFILE_INFO
{
	bool		m_bOnceOut = false;

	char		m_szTitle[128] = { 0 };
	INT64		m_CallCount = 0;
	INT64		m_tmElapsed = 0;

	INT64		m_TempCallCount = 0;
	INT64		m_tmStart = 0;
	INT64		m_tmAccm = 0;
	INT64		m_tmTempMin = 0;
	INT64		m_tmTempMax = 0;

	INT64		m_TotalCallCount = 0;
	INT64		m_TotalElapsed = 0;
	INT64		m_tmMin = 0;
	INT64		m_tmMax = 0;
	INT64		m_tmAvg = 0;
} ;

static PROFILE_INFO	g_PfInfo[ MAX_PROFILERECORD];

I3_EXPORT_BASE
void SET_PROFILE( INT32 slot, const char * pszStr)
{
	strncpy( g_PfInfo[slot].m_szTitle, pszStr, sizeof(g_PfInfo[slot].m_szTitle) - 1);
}

I3_EXPORT_BASE
void START_PROFILE( INT32 slot)
{
	g_PfInfo[slot].m_tmStart = i3Timer::GetProfileTime();;
}

I3_EXPORT_BASE
void END_PROFILE( INT32 slot)
{
	INT64 gap;

	g_PfInfo[slot].m_TempCallCount++;

	gap = i3Timer::GetProfileTime() - g_PfInfo[slot].m_tmStart;
	g_PfInfo[slot].m_tmAccm += gap;

	if( g_PfInfo[slot].m_bOnceOut == false)
	{
		g_PfInfo[slot].m_tmTempMin = g_PfInfo[slot].m_tmTempMax = gap;
	}
	else
	{
		g_PfInfo[slot].m_tmTempMin = MIN( g_PfInfo[slot].m_tmTempMin, gap);
		g_PfInfo[slot].m_tmTempMax = MAX( g_PfInfo[slot].m_tmTempMax, gap);
	}

	g_PfInfo[slot].m_bOnceOut = true;
}

I3_EXPORT_BASE
bool CHECK_PROFILE( INT32 slot, double sec)
{
	double timeRes = i3Timer::GetProfileResolution();

	return ((g_PfInfo[slot].m_tmAccm * timeRes) >= sec);
}

I3_EXPORT_BASE
void RESET_PROFILE( bool bClearHistory)
{
	INT32 i;

	for( i = 0; i < MAX_PROFILERECORD; i++)
	{
		g_PfInfo[i].m_CallCount = g_PfInfo[i].m_TempCallCount;
		g_PfInfo[i].m_TotalCallCount += g_PfInfo[i].m_TempCallCount;
		g_PfInfo[i].m_tmElapsed = g_PfInfo[i].m_tmAccm;
		g_PfInfo[i].m_TotalElapsed += g_PfInfo[i].m_tmAccm;

		if( bClearHistory)
		{
			g_PfInfo[i].m_tmMin = 0;
			g_PfInfo[i].m_tmMax = 0;
			g_PfInfo[i].m_tmAvg = 0;
			g_PfInfo[i].m_bOnceOut = false;
		}
		else
		{
			g_PfInfo[i].m_tmMin = MIN( g_PfInfo[i].m_tmMin, g_PfInfo[i].m_tmAccm);
			g_PfInfo[i].m_tmMax = MAX( g_PfInfo[i].m_tmMax, g_PfInfo[i].m_tmAccm);

			if( g_PfInfo[i].m_TempCallCount > 0)
				g_PfInfo[i].m_tmAvg	= g_PfInfo[i].m_TotalElapsed / g_PfInfo[i].m_TotalCallCount;
			else
				g_PfInfo[i].m_tmAvg	=  0;
		}

		g_PfInfo[i].m_TempCallCount = 0;
		g_PfInfo[i].m_tmStart = 0;
		g_PfInfo[i].m_tmAccm = 0;
		g_PfInfo[i].m_tmTempMin = 0x7FFFFFFFFFFFFFFF;	//	가장 큰수 
		g_PfInfo[i].m_tmTempMax = 0x8000000000000000;	//	가장 작은수
	}
}

#define		SPTR( str)			if( pszDest == nullptr)	I3TRACE( str);	else	strcat( pszDest, str)

I3_EXPORT_BASE
void REPORT_PROFILE( char * pszDest)
{
	INT32 i;
	double	timeRes = i3Timer::GetProfileResolution();
	char conv[256];

	sprintf( conv, "----------------------- Recorded Profile (ms) -----------------------------\n");
	SPTR( conv);

	for( i = 0; i < MAX_PROFILERECORD; i++)
	{
		if( g_PfInfo[i].m_bOnceOut) 
		{
			#if defined( I3_COMPILER_VC)
			sprintf( conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n", 
					i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount, 
					(timeRes * g_PfInfo[i].m_tmElapsed),
					(timeRes * g_PfInfo[i].m_tmMin),
					(timeRes * g_PfInfo[i].m_tmMax),
					(timeRes * g_PfInfo[i].m_tmAvg));

			

			#elif defined( I3_COMPILER_GCC)
				#if defined( I3_WIPI)
					sprintf( conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n", 
						i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount, 
						(timeRes * g_PfInfo[i].m_tmElapsed),
						(timeRes * g_PfInfo[i].m_tmMin),
						(timeRes * g_PfInfo[i].m_tmMax),
						(timeRes * g_PfInfo[i].m_tmAvg));
				#else
					sprintf( conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n", 
						i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount, 
						(timeRes * g_PfInfo[i].m_tmElapsed),
						(timeRes * g_PfInfo[i].m_tmMin),
						(timeRes * g_PfInfo[i].m_tmMax),
						(timeRes * g_PfInfo[i].m_tmAvg));
				#endif
			#endif

			SPTR( conv);
		}
	}

	sprintf( conv, "---------------------------------------------------------------------------\n");
	SPTR( conv);
}


I3_EXPORT_BASE 
void REPORT_PROFILE( i3::string& inout)
{

	INT32 i;
	double	timeRes = i3Timer::GetProfileResolution();
	char conv[256];

	sprintf( conv, "----------------------- Recorded Profile (ms) -----------------------------\n");
	inout += conv;

	for( i = 0; i < MAX_PROFILERECORD; i++)
	{
		if( g_PfInfo[i].m_bOnceOut) 
		{
#if defined( I3_COMPILER_VC)
			sprintf( conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n", 
				i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount, 
				(timeRes * g_PfInfo[i].m_tmElapsed),
				(timeRes * g_PfInfo[i].m_tmMin),
				(timeRes * g_PfInfo[i].m_tmMax),
				(timeRes * g_PfInfo[i].m_tmAvg));

#elif defined( I3_COMPILER_GCC)
#if defined( I3_WIPI)
			sprintf( conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n", 
				i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount, 
				(timeRes * g_PfInfo[i].m_tmElapsed),
				(timeRes * g_PfInfo[i].m_tmMin),
				(timeRes * g_PfInfo[i].m_tmMax),
				(timeRes * g_PfInfo[i].m_tmAvg));
#else
			sprintf( conv, "[%02d] %s - call:%05I64d,  elapsed:%f ms (min:%f, max:%f, avg:%f)\n", 
				i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount, 
				(timeRes * g_PfInfo[i].m_tmElapsed),
				(timeRes * g_PfInfo[i].m_tmMin),
				(timeRes * g_PfInfo[i].m_tmMax),
				(timeRes * g_PfInfo[i].m_tmAvg));
#endif
#endif
			inout += conv;
		}
	}

	sprintf( conv, "---------------------------------------------------------------------------\n");
	inout += conv;

}



I3_EXPORT_BASE
void REPORT_PROFILE_CALLBACK( PF_CALLBACK pUserProc, void * pData)
{
	INT32 i;
	double	timeRes = i3Timer::GetProfileResolution();

	for( i = 0; i < MAX_PROFILERECORD; i++)
	{
		if( g_PfInfo[i].m_bOnceOut) 
		{
			pUserProc( i, g_PfInfo[i].m_szTitle, g_PfInfo[i].m_CallCount,
				(timeRes * g_PfInfo[i].m_tmElapsed),
					(timeRes * g_PfInfo[i].m_tmMin),
					(timeRes * g_PfInfo[i].m_tmMax),
					(timeRes * g_PfInfo[i].m_tmAvg), pData);
		}
	}
}

I3_EXPORT_BASE INT64	PF_GetCallCount( INT32 slot)
{
	return g_PfInfo[ slot].m_CallCount;
}

I3_EXPORT_BASE REAL64	PF_GetElapsed( INT32 slot)
{
	REAL64	timeRes = i3Timer::GetProfileResolution();

	return timeRes * g_PfInfo[ slot].m_tmElapsed;
}

#endif
