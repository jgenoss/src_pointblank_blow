#ifndef _SubStepChallenge_h
#define _SubStepChallenge_h

#include "SubStepBase.h"

class CSubStepChallenge: public CSubStepBase
{
	I3_CLASS_DEFINE(CSubStepChallenge);
public:
	CSubStepChallenge();
	virtual ~CSubStepChallenge();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3GuiControl* pControl);
	virtual void OnInitControl(void);

private:
};

#endif //_SubStepChallenge_h
