#if !defined( __I3_EVENT_STATE_H)
#define __I3_EVENT_STATE_H

#include "i3Base.h"
#include "i3Event.h"

class i3EventState : public i3Event
{
	I3_CLASS_DEFINE( i3EventState);

public:
	i3EventState(void);
	virtual ~i3EventState(void);
};

#endif