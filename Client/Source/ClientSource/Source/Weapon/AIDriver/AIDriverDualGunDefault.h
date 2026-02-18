#if !defined( __DualGunDefault_AIDRIVER_H)
#define __DualGunDefault_AIDRIVER_H

#include "i3Framework.h"

#define AI_DUALGUNDEFAULT_EXTENSIONACTIVATE	0x00000000
#define AI_DUALGUNDEFAULT_EXTENSIONDEACTIVATE	0x00000001
#define AI_DUALGUNDEFAULT_FIRE	0x00000002
#define AI_DUALGUNDEFAULT_IDLE	0x00000003
#define AI_DUALGUNDEFAULT_LOADBULLET	0x00000004
#define AI_DUALGUNDEFAULT_LOADMAG	0x00000005
#define AI_DUALGUNDEFAULT_LOADMAGDUALGUN	0x00000006

class AIDriverDualGunDefault : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverDualGunDefault, i3AIDriver);
protected:
public:
	AIDriverDualGunDefault(void);
	virtual ~AIDriverDualGunDefault(void);
};

#endif
