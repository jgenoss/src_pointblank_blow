#if !defined( __Dummy_AIDRIVER_H)
#define __Dummy_AIDRIVER_H

#include "i3Framework.h"

#define AI_DUMMY_DESTROY	0x00000000
#define AI_DUMMY_IDLE	0x00000001
#define AI_DUMMY_NULL	0x00000002

class AIDriverDummy : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverDummy, i3AIDriver);
protected:
public:
	AIDriverDummy(void);
	virtual ~AIDriverDummy(void);
};

#endif
