#if !defined( __I3_EVENT_IO_H)
#define __I3_EVENT_IO_H

#include "i3Base.h"
#include "i3Event.h"

class i3EventIO : public i3Event
{
	I3_CLASS_DEFINE( i3EventIO);

public:
	i3EventIO(void);
	virtual ~i3EventIO(void);
};

#endif