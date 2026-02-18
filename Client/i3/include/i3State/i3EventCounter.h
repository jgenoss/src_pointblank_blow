#if !defined( __I3_EVENT_COUNTER_H)
#define __I3_EVENT_COUNTER_H

#include "i3Base.h"
#include "i3Event.h"

class i3EventCounter : public i3Event
{
	I3_CLASS_DEFINE( i3EventCounter);

public:
	i3EventCounter(void);
	virtual ~i3EventCounter(void);
};

#endif