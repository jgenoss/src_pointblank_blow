#if !defined( __GrenadeDefault_AIDRIVER_H)
#define __GrenadeDefault_AIDRIVER_H

#include "i3Framework.h"

#define AI_GRENADEDEFAULT_FIRE	0x00000000
#define AI_GRENADEDEFAULT_IDLE	0x00000001
#define AI_GRENADEDEFAULT_LOADBULLET	0x00000002
#define AI_GRENADEDEFAULT_LOADMAG	0x00000003
#define AI_GRENADEDEFAULT_SECONDARYFIRE	0x00000004

class AIDriverGrenadeDefault : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverGrenadeDefault, i3AIDriver);
protected:
public:
	AIDriverGrenadeDefault(void);
	virtual ~AIDriverGrenadeDefault(void);
};

#endif
