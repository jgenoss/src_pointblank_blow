#if !defined( __I3_EVENT_INPUT_H)
#define __I3_EVENT_INPUT_H

#include "i3Base.h"
#include "i3Event.h"

class i3EventInput : public i3Event
{
	I3_CLASS_DEFINE( i3EventInput);

public:
	i3EventInput(void);
	virtual ~i3EventInput(void);
};

#endif