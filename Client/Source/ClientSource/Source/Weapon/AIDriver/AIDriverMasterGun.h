#if !defined( __MasterGun_AIDRIVER_H)
#define __MasterGun_AIDRIVER_H

#include "i3Framework.h"

#define AI_MASTERGUN_EXTENSIONACTIVATE	0x00000000
#define AI_MASTERGUN_EXTENSIONDEACTIVATE	0x00000001
#define AI_MASTERGUN_FIRE	0x00000002
#define AI_MASTERGUN_IDLE	0x00000003
#define AI_MASTERGUN_LOADBULLET	0x00000004
#define AI_MASTERGUN_LOADMAG	0x00000005
#define AI_MASTERGUN_LOADMAGMASTER1PV	0x00000006
#define AI_MASTERGUN_LOADMAGMASTER3PV	0x00000007

class AIDriverMasterGun : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverMasterGun, i3AIDriver);
protected:
public:
	AIDriverMasterGun(void);
	virtual ~AIDriverMasterGun(void);
};

#endif
