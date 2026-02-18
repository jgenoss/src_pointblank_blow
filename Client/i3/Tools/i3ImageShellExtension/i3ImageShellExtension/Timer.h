#include "TypeDefine.h"

class Timer
{
protected:
	bool			m_bTimerInit;

	INT64			m_OldTick;

public:
	Timer(void);
	virtual ~Timer(void);

	double			GetDeltaSec(void);
	double			GetFixedDeltaSec(void);

	static	INT64	GetProfileTime(void);
	static	double	GetProfileResolution(void);
	static	INT64	GetPerformanceTime(void);

	static	void	Sleep(UINT32	ms);

private:
	UINT32	m_uiOldTime;
};
