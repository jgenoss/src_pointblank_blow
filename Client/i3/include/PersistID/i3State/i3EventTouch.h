#if !defined( __I3_EVENT_TOUCH_H)
#define __I3_EVENT_TOUCH_H

#include "i3Base.h"
#include "i3Event.h"

class i3EventTouch : public i3Event
{
	I3_CLASS_DEFINE( i3EventTouch);

public:
	i3EventTouch(void);
	virtual ~i3EventTouch(void);
};

#endif