#ifndef __I3_DATE_TIME__
#define __I3_DATE_TIME__

#include "i3Thread.h"
#include "DATE32.h"

class I3_EXPORT_BASE i3DateTime : public i3Thread
{
	I3_CLASS_DEFINE(i3DateTime);

	DATE32					m_dt32Local;			// 썸머타임 적용되는 로컬시(변환후)
	DATE32					m_dt32Stand;			// 썸머타임 무시하는 표준시(변환후)

	UINT32					m_ui32AccSecMilli;		// 초기값 10000000. 이후 1초에 1000씩 증가
	UINT32					m_ui32AccSec;			// 초기값 10000. 이후 1초에 1씩 증가
	UINT32					m_ui32BeginTime;

	BOOL					m_bIsRun;

public:
	i3DateTime(void);
	virtual ~i3DateTime(void);

	UINT32					GetServerTime(void)					{	return m_ui32AccSec;		}
	UINT32					GetServerTimeMilli(void)			{	return m_ui32AccSecMilli;	}

	DATE32					GetLocalTime()						{	return m_dt32Local;			}
	DATE32					GetStandTime()						{	return m_dt32Stand;			}

	void					GetSmallDate( char* pstrSmallDateTime, INT32 i32Lenth );

	virtual UINT32			OnRunning( void* pUserData );
};

namespace i3ThreadTimer
{
	I3_EXPORT_BASE BOOL Init(void);
	I3_EXPORT_BASE void Destroy(void);
	I3_EXPORT_BASE DATE32 LocalTime(void);
	I3_EXPORT_BASE DATE32 StandTime(void);
	I3_EXPORT_BASE UINT32 GetServerTime(void);
	I3_EXPORT_BASE UINT32 GetServerTimeMilli(void);

	// 1.초기화	i3ThreadTimer::Init();
	// 2.사용	i3ThreadTimer::GetServerTime(); i3ThreadTimer::GetLocalTime(), i3ThreadTimer::GetStandTime(), i3ThreadTimer::GetServerTimeMilli()
	// 3.종료	i3ThreadTimer::Destroy();
}

#endif