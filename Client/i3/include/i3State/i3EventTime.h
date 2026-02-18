#if !defined( __I3_EVENT_TIME_H)
#define __I3_EVENT_TIME_H

#include "i3Base.h"
#include "i3Event.h"

class i3EventTime : public i3Event
{
	I3_CLASS_DEFINE( i3EventTime);

public:
	i3EventTime(void);
	virtual ~i3EventTime(void);
};

#endif