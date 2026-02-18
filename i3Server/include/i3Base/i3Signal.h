#if !defined( __I3_SIGNAL_H)
#define __I3_SIGNAL_H

#include "i3ElementBase.h"

class I3_EXPORT_BASE i3Signal : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Signal);
protected:

#if defined( I3_WINDOWS)
	HANDLE			m_hEvent;
#endif

public:
	i3Signal(void);
	virtual ~i3Signal(void);

	bool		Create( bool bManualReset = false, bool bInitState = false);


#if defined( I3_WINDOWS)
	HANDLE		getEventObject(void)				{ return m_hEvent; }
#endif

	bool		Raise(void);
	bool		Reset(void);
	bool		WaitFor( UINT32 timeout = 0xFFFFFFFF);
};

#endif