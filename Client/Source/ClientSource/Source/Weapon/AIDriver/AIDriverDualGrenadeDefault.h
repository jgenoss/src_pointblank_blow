#if !defined( __DualGrenadeDefault_AIDRIVER_H)
#define __DualGrenadeDefault_AIDRIVER_H

#include "i3Framework.h"

#define AI_DUALGRENADEDEFAULT_FIRE_RIGHT	0x00000000
#define AI_DUALGRENADEDEFAULT_FIRE_LEFT	0x00000001
#define AI_DUALGRENADEDEFAULT_IDLE_RIGHT	0x00000002
#define AI_DUALGRENADEDEFAULT_IDLE_LEFT	0x00000003
#define AI_DUALGRENADEDEFAULT_LOADBULLET	0x00000004
#define AI_DUALGRENADEDEFAULT_LOADMAG	0x00000005

class AIDriverDualGrenadeDefault : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverDualGrenadeDefault, i3AIDriver);
protected:
public:
	AIDriverDualGrenadeDefault(void);
	virtual ~AIDriverDualGrenadeDefault(void);
};

#endif
