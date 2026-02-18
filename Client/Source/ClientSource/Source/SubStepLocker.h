#ifndef _SubStepLocker_h
#define _SubStepLocker_h

#include "SubStepBase.h"

class CSubStepLocker: public CSubStepBase
{
	I3_CLASS_DEFINE(CSubStepLocker);
public:
	CSubStepLocker();
	virtual ~CSubStepLocker();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3GuiControl* pControl);
	virtual void OnInitControl(void);

private:
};

#endif //_SubStepLocker_h
