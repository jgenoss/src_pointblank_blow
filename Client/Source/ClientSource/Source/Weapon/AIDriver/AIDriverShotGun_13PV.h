#if !defined( __ShotGun_13PV_AIDRIVER_H)
#define __ShotGun_13PV_AIDRIVER_H

#include "i3Framework.h"

#define AI_SHOTGUN_13PV_FIRE	0x00000000
#define AI_SHOTGUN_13PV_IDLE	0x00000001
#define AI_SHOTGUN_13PV_LOADBULLET1PV	0x00000002
#define AI_SHOTGUN_13PV_LOADBULLET3PV	0x00000003
#define AI_SHOTGUN_13PV_LOADMAG1PV	0x00000004
#define AI_SHOTGUN_13PV_LOADMAG3PV	0x00000005

class AIDriverShotGun_13PV : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverShotGun_13PV, i3AIDriver);
protected:
public:
	AIDriverShotGun_13PV(void);
	virtual ~AIDriverShotGun_13PV(void);
};

#endif
