#if !defined( __I3_STATE_FINISH_H)
#define __I3_STATE_FINISH_H

#include "i3State.h"

class i3StateFinish : public i3State
{
	I3_CLASS_DEFINE( i3StateFinish);

public:
	i3StateFinish(void);
	virtual ~i3StateFinish(void);
};

#endif