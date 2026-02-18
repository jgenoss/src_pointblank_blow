#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <pdh.h>
#include <pdhmsg.h>

class CSystem : public i3ElementBase
{
	I3_CLASS_DEFINE( CSystem );

	//Time
	INT32				m_i32Year;
	INT32				m_i32Month;
	INT32				m_i32Day;
	INT32				m_i32Hour;
	INT32				m_i32Minute;
	INT32				m_i32Second;

	i3Timer				m_Timer;
	SYSTEMTIME			m_SystemTime;	
	REAL32				m_rLocalTime; 

	UINT32				m_ui32DateTime;
	UINT32				m_ui32ServerTime; 
	
	INT32				m_i32CpuQuery;
	HQUERY				m_hCpuQuery;
	HCOUNTER			m_hCpuCounter;

	INT32				m_i32HandleQuery;
	HQUERY				m_hHandleQuery;
	HCOUNTER			m_hHandleCounter;

public:
	CSystem();
	virtual ~CSystem();

	BOOL				OnCreate();
	void				OnDestroy();

	void				OnUpdate();

	void				RenewTime();

	INT32				GetYear()					{ return m_i32Year;			}
	INT32				GetMonth()					{ return m_i32Month;		}
	INT32				GetDay()					{ return m_i32Day;			}
	INT32				GetHour()					{ return m_i32Hour;			}
	INT32				GetMinute()					{ return m_i32Minute;		}
	INT32				GetSecond()					{ return m_i32Second;		}

	INT32				GetCpuQuery()				{ return m_i32CpuQuery;		}
	INT32				GetHandleQuery()			{ return m_i32HandleQuery;	}

	UINT32				GetServerTime()				{ return m_ui32ServerTime;	}	
	UINT32				GetDateTime()				{ return m_ui32DateTime;	}
};

extern CSystem*	g_pSystem;
#endif