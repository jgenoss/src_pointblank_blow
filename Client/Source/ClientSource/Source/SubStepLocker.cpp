#include "pch.h"
#include "SubStepLocker.h"

I3_CLASS_INSTANCE(CSubStepLocker, CSubStepBase);

CSubStepLocker::CSubStepLocker()
{
}

CSubStepLocker::~CSubStepLocker()
{
}

void CSubStepLocker::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
}

BOOL CSubStepLocker::OnLinkControl(i3GuiControl* pControl)
{
	return FALSE;
}

void CSubStepLocker::OnInitControl(void)
{
}
