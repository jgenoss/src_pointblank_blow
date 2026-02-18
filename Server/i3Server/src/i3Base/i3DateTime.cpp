#include "i3CommonType.h"
#include "i3DateTime.h"
#include <mmsystem.h>

#define SERVER_BEGIN_TIME	10000

static i3DateTime*	s_pDateTime;

I3_CLASS_INSTANCE(i3DateTime, i3Thread);

i3DateTime::i3DateTime(void)
{
	m_bIsRun			= FALSE;
	m_ui32AccSec		= SERVER_BEGIN_TIME;	// 계산을 위해서 이 값부터 시작... UINT32 때문
	m_ui32AccSecMilli	= SERVER_BEGIN_TIME*1000;
	m_ui32BeginTime		= timeGetTime() - (SERVER_BEGIN_TIME*1000);
}

i3DateTime::~i3DateTime(void)
{
	m_bIsRun	= FALSE; 
	i3Thread::WaitForFinish();
}

UINT32 i3DateTime::OnRunning( void * pUserData)
{
	SYSTEMTIME stSystemTime;

	m_bIsRun = TRUE;
	while( m_bIsRun )
	{
		// 1. 10000부터 시작하는 누적초,누적밀리초 업데이트
		m_ui32AccSecMilli = timeGetTime() - m_ui32BeginTime;
		m_ui32AccSec = m_ui32AccSecMilli/1000;

		// 2. 썸머타임 영향받는 로컬 년월일시분초 업데이트		
		::GetLocalTime( &stSystemTime );
		m_dt32Local = stSystemTime;

		// 3. 썸머타임 관계없는 표준 년월일시분초 업데이트
		SYSTEMTIME UTCTime;
		TIME_ZONE_INFORMATION TZInfo;
		::GetSystemTime(&UTCTime);
		::GetTimeZoneInformation(&TZInfo);
		TZInfo.DaylightBias = 0;			// 타임존 정보에서 썸머타임을 적용하지 않도록 보정값 변경
		::SystemTimeToTzSpecificLocalTime( &TZInfo, &UTCTime, &stSystemTime );
		m_dt32Stand = stSystemTime;

		// 4. 슬립
		::WaitForSingleObject( m_Handle, 1);
	}
	return 0;
}

I3_EXPORT_BASE BOOL i3ThreadTimer::Init(void)
{
	s_pDateTime = i3DateTime::NewObject();
	return s_pDateTime->Create( "ThreadTimer", 0, 4096, 0, PRIORITY_NORMAL );
}

I3_EXPORT_BASE DATE32 i3ThreadTimer::LocalTime(void)
{
	return s_pDateTime->GetLocalTime();
}

I3_EXPORT_BASE DATE32 i3ThreadTimer::StandTime(void)
{
	return s_pDateTime->GetStandTime();
}

I3_EXPORT_BASE UINT32 i3ThreadTimer::GetServerTime(void)
{
	return s_pDateTime->GetServerTime();
}

I3_EXPORT_BASE UINT32 i3ThreadTimer::GetServerTimeMilli(void)
{
	return s_pDateTime->GetServerTimeMilli();
}

I3_EXPORT_BASE void i3ThreadTimer::Destroy(void)
{
	I3_SAFE_RELEASE(s_pDateTime);
}