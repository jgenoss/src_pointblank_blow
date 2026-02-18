#if !defined( __DinoCommon_AIDRIVER_H)
#define __DinoCommon_AIDRIVER_H

#include "i3Framework.h"

#define AI_DINOCOMMON_DEATH	0x00000000

class AIDriverDinoCommon : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverDinoCommon, i3AIDriver);
protected:
public:
	AIDriverDinoCommon(void);
	virtual ~AIDriverDinoCommon(void);
};

#endif
