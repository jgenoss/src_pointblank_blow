#if !defined( __I3_TIMER_H)
#define __I3_TIMER_H

#include "i3ElementBase.h"


class I3_EXPORT_BASE i3Timer : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Timer, i3ElementBase);
protected:
	bool			m_bTimerInit = false;

#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WINCE)
	INT64		m_OldTick = 0;
#elif defined( I3_PSP) || defined( I3_WIPI)
	UINT64		m_OldTick = 0;
#endif

	
public:

	REAL32			GetDeltaSec(void);
	REAL32			GetFixedDeltaSec(void);

	static	INT64	GetProfileTime(void);
	static	double	GetProfileResolution(void);
	static	INT64	GetPerformanceTime( void);

	static	void	Sleep( UINT32	ms);

private:
	UINT32	m_uiOldTime = 0;
};

#endif
