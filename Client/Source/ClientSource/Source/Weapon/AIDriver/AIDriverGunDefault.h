#if !defined( __GunDefault_AIDRIVER_H)
#define __GunDefault_AIDRIVER_H

#include "i3Framework.h"

#define AI_GUNDEFAULT_EXTENSIONACTIVATE	0x00000000
#define AI_GUNDEFAULT_EXTENSIONDEACTIVATE	0x00000001
#define AI_GUNDEFAULT_FIRE	0x00000002
#define AI_GUNDEFAULT_IDLE	0x00000003
#define AI_GUNDEFAULT_LOADBULLET	0x00000004
#define AI_GUNDEFAULT_LOADMAG	0x00000005

class AIDriverGunDefault : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverGunDefault, i3AIDriver);
protected:
public:
	AIDriverGunDefault(void);
	virtual ~AIDriverGunDefault(void);
};

#endif
