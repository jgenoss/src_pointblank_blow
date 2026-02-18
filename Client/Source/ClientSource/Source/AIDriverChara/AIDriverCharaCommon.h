#if !defined( __CharaCommon_AIDRIVER_H)
#define __CharaCommon_AIDRIVER_H

#include "i3Framework.h"

#define AI_CHARACOMMON_CEREMONY	0x00000000

class AIDriverCharaCommon : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriverCharaCommon, i3AIDriver);
protected:
public:
	AIDriverCharaCommon(void);
	virtual ~AIDriverCharaCommon(void);
};

#endif
