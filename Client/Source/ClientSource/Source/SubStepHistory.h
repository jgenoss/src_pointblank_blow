#ifndef _SubStepHistory_h
#define _SubStepHistory_h

#include "SubStepBase.h"

class CSubStepHistory: public CSubStepBase
{
	I3_CLASS_DEFINE(CSubStepHistory);
public:
	CSubStepHistory();
	virtual ~CSubStepHistory();

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	virtual BOOL OnLinkControl(i3GuiControl* pControl);
	virtual void OnInitControl(void);

private:
};

#endif //_SubStepHistory_h
