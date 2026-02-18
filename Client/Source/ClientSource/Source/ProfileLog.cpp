#include "pch.h"
#include "ProfileLog.h"

I3_CLASS_INSTANCE( CProfileLog);//, i3ElementBase);

struct PFLOG_TABLE
{
	char						m_szName[16];
	I3_VIEWER_PROFILE_SLOT		m_Slot;

};

static PFLOG_TABLE s_PFLogTable[ PFLOG_COUNT] =
{
	{	"Udt",		I3_VIEWER_PROFILE_UPDATE },
	{	"Trc",		I3_VIEWER_PROFILE_TRACE_3D },
	{	"Rnd",		I3_VIEWER_PROFILE_RENDER_3D },
	{	"Phs",		I3_VIEWER_PROFILE_PHX_SIMULATE },
	{	"End",		I3_VIEWER_PROFILE_END },
	{	"EfR",		I3_VIEWER_PROFILE_EFFECT },
	{	"sgCtx",	I3_VIEWER_PROFILE_SGCONTEXT }
};

CProfileLog::CProfileLog(void)
{
	// Total
	m_Total		= 0.0f;
	m_Total_Max	= 0.0f;

	i3mem::FillZero( m_timeAll, sizeof(m_timeAll));
	i3mem::FillZero( m_timeSec, sizeof(m_timeSec));
	i3mem::FillZero( m_timeMax, sizeof(m_timeMax));

	m_FrameCnt		= 0;
	m_SecCnt		= 0;

	m_timeLocal		= 0.0f;
	
	m_bStart		= false;
	m_timeStart		= 0.0f;
}

CProfileLog::~CProfileLog(void)
{
	Log();
}

void CProfileLog::UpdateFrame( REAL32 tm)
{
	REAL32 temp;
	INT32 i;

	if( m_bStart)
	{
		m_timeLocal += tm;
		m_FrameCnt++;

		// OnUpdate
		for( i = 0; i < PFLOG_COUNT; i++)
		{
			temp = g_pViewer->getProfileData( s_PFLogTable[i].m_Slot);

			// 1초간 Elapsed Time 누적
			m_timeSec[ i] += temp;

			// 최대치 갱신
			m_timeMax[ i]	=	max( temp, m_timeMax[ i]);
		}

		if( m_timeLocal < 1.0f)
			return;


		////////////////////////////////////////////////////////////
		// 1초마다 Update

		// Total
		temp = g_pViewer->getProfileData( I3_VIEWER_PROFILE_TOTAL);
		m_Total += temp;
		m_Total_Max	=	max( temp, m_Total_Max);

		for( i = 0; i < PFLOG_COUNT; i++)
		{
			m_timeAll[i]	+= m_timeSec[i] / m_FrameCnt;			// 전체 시간 누적.
			m_timeSec[i]	= 0.0f;
		}

		m_SecCnt ++;
		m_FrameCnt = 0;
		m_timeLocal = 0.0f;
	}
	else
	{
		m_timeStart += tm;

		// Stage를 시작하고 앞부분은 많이 느릴 수 있기 때문에
		// Profile에 반영하지 않는다.

		if( m_timeStart > 5.0f)
			m_bStart = true;
	}
}

void CProfileLog::Log(void)
{
	INT32 i;

	I3PRINTLOG(I3LOG_NOTICE, "^^^^^^^^^^^^^^^^ Pf Log ^^^^^^^^^^^^^^^^^^");

	if( m_SecCnt <= 0)
	{
		I3PRINTLOG(I3LOG_NOTICE, "SecCnt is zero.");
		return;
	}

	I3PRINTLOG(I3LOG_NOTICE, " * Total : %.1f (%.1f)",		m_Total / m_SecCnt, m_Total_Max);

	for( i = 0; i < PFLOG_COUNT; i++)
	{
		I3TRACE( "%f %d\n",  m_timeAll[i], m_SecCnt);

		I3PRINTLOG(I3LOG_NOTICE, " * %10s\t%.1f (%.1f)",		s_PFLogTable[i].m_szName, m_timeAll[i] / m_SecCnt, m_timeMax[i]);
	}
}
