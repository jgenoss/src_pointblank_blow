#if !defined( __Sentrygun_AIDRIVER_H)
#define __Sentrygun_AIDRIVER_H

#include "i3Framework.h"

#define AI_SENTRYGUN_ATTACK	0x00000000
#define AI_SENTRYGUN_DESTROY	0x00000001
#define AI_SENTRYGUN_NONE	0x00000002
#define AI_SENTRYGUN_SETUP	0x00000003
#define AI_SENTRYGUN_UNDER_SURVEILLANCE	0x00000004

class AIDriverSentrygun : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverSentrygun, i3AIDriver);
protected:
public:
	AIDriverSentrygun(void);
	virtual ~AIDriverSentrygun(void);
};

#endif
