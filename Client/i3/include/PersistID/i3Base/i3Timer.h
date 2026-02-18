#if !defined( __I3_TIMER_H)
#define __I3_TIMER_H

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

class I3_EXPORT_BASE i3Timer : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Timer);
protected:
	BOOL			m_bTimerInit;

#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WINCE)
	INT64		m_OldTick;
#elif defined( I3_PSP) || defined( I3_WIPI)
	UINT64		m_OldTick;
#endif

	
public:
	i3Timer(void);
	virtual ~i3Timer(void);

	REAL32			GetDeltaSec(void);

	static	INT64	GetProfileTime(void);
	static	double	GetProfileResolution(void);
	static	INT64	GetPerformanceTime( void);
};

#endif
