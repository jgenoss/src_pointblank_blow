#if !defined( __I3_STATE_INIT_H)
#define __I3_STATE_INIT_H

#include "i3State.h"

class i3StateInit : public i3State
{
	I3_CLASS_DEFINE( i3StateInit);

public:
	i3StateInit(void);
	virtual ~i3StateInit(void);
};

#endif