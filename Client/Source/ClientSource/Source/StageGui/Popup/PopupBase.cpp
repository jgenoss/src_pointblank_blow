#include "pch.h"
#include "PopupBase.h"
#include "GlobalVariables.h"

I3_ABSTRACT_CLASS_INSTANCE(CPopupBase, CGuiNotifyReceiver);

CPopupBase::CPopupBase()
{
}

CPopupBase::~CPopupBase()
{
}

void CPopupBase::OnOpenPopup(void * pArg1, void * pArg2)
{
	if( !isEnable())
	{
		PlaySound(GUI_SND_POPUP_OPEN);
	}

	SetEnable( TRUE);
}

void CPopupBase::OnClosePopup(void * pArg1, void * pArg2)
{
	if( isEnable())
	{
		PlaySound(GUI_SND_POPUP_CLOSE);
	}

	SetEnable( FALSE);
}

void CPopupBase::OnInitControl(void)
{
	CGuiNotifyReceiver::OnInitControl();

}

BOOL CPopupBase::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				OnExitKey();
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
