#if !defined( ___Default_AIDRIVER_H)
#define ___Default_AIDRIVER_H

#include "i3Framework.h"

#define AI__DEFAULT_ATTACK	0x00000000
#define AI__DEFAULT_ATTACKIDLE	0x00000001
#define AI__DEFAULT_DAMAGE	0x00000002
#define AI__DEFAULT_DEATH	0x00000003
#define AI__DEFAULT_IDLE	0x00000004

class AIDriver_Default : public i3AIDriver
{
	I3_CLASS_DEFINE( AIDriver_Default, i3AIDriver);
protected:
public:
	AIDriver_Default(void);
	virtual ~AIDriver_Default(void);
};

#endif
