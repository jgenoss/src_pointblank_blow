#ifndef __SYSTEM_TIMER_H__
#define __SYSTEM_TIMER_H__

class CSystemTimer
{
	INT64				m_i64QPFTicksPerSec;
	INT64				m_i64LastElapsedTime;

public:
	CSystemTimer();
	~CSystemTimer();

	bool					Create();
	void					Destroy();
	REAL32					RenewElapsedTime();
};

#endif