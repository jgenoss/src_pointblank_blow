#if !defined( __Gun_13PV_old_AIDRIVER_H)
#define __Gun_13PV_old_AIDRIVER_H

#include "i3Framework.h"

#define AI_GUN_13PV_OLD_CHANGE1PV	0x00000000
#define AI_GUN_13PV_OLD_CHANGE3PV	0x00000001
#define AI_GUN_13PV_OLD_EXTENSIONACTIVATE	0x00000002
#define AI_GUN_13PV_OLD_EXTENSIONDEACTIVATE	0x00000003
#define AI_GUN_13PV_OLD_FIRE	0x00000004
#define AI_GUN_13PV_OLD_IDLE	0x00000005
#define AI_GUN_13PV_OLD_LOADMAG1PV	0x00000006
#define AI_GUN_13PV_OLD_LOADMAG3PV	0x00000007

class AIDriverGun_13PV_old : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverGun_13PV_old, i3AIDriver);
protected:
public:
	AIDriverGun_13PV_old(void);
	virtual ~AIDriverGun_13PV_old(void);
};

#endif
