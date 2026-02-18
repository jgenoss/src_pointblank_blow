#if !defined( __TRIGGER_CALLBACK_H)
#define __TRIGGER_CALLBACK_H

#include "NxUserContactReport.h"

class CTriggerCallBack : public NxUserTriggerReport
{
private:
	REAL32 m_rOldTime;
public:
	void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);
}; 

#endif // #if !defined( __TRIGGER_CALLBACK_H)