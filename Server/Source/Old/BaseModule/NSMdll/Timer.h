#ifndef __NSM_TIMER_H
#define __NSM_TIMER_H

class EXPORT_BASE NSM_cTimer
{	
protected:
	BOOL		m_bTimerInit;
	INT64		m_OldTick;
	
public:
	NSM_cTimer(void);
	virtual ~NSM_cTimer(void);

	REAL32	GetDeltaSec(void);
};

#endif
