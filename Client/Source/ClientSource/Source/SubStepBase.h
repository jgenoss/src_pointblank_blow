#ifndef _SubStepBase_h
#define _SubStepBase_h

#include "GuiNotifyReceiver.h"

class CSubStepBase: public CGuiNotifyReceiver
{
	I3_ABSTRACT_CLASS_DEFINE(CSubStepBase);
public:
	CSubStepBase();
	~CSubStepBase();

private:
};

#endif //_SubStepBase_h
