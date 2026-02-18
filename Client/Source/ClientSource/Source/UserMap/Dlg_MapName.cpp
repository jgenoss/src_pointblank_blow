#include "PCH.h"
#include "Dlg_MapName.h"

I3_CLASS_INSTANCE( CDlgMapName, i3UIModalDriver);


BOOL CDlgMapName::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pInfo = (I3UI_CONTROL_NOTIFY *) param2;
		i3UIControl * pCtrl = pInfo->m_pCtrl;

		switch( pInfo->m_nEvent)
		{
			case I3UI_EVT_CLICKED :
				if( (i3String::Compare( pCtrl->GetName(), "Yes") == 0) || (i3String::Compare( pCtrl->GetName(), "OK") == 0))
				{
					// 이름이 입력되었는가를 확인한다.
					// 만약 충분히 입력되지 않았다면 Dialog를 닫지 않는다.
					i3UIEditBox * pEditBox = (i3UIEditBox *) pInfo->m_pFrameWnd->FindCtrlByName( "Name");

					if( (pEditBox->getText() == NULL) || (pEditBox->getText()[0] == 0))
					{
						// 이름이 입력되지 않았다.
						i3UI::MsgBox( I3UI_MSGBOX_OK, NULL, "맵의 이름이 있어야만 저장할 수 있습니다.");
						return TRUE;
					}
				}

				break;
		}
	}

	return i3UIModalDriver::OnEvent( event, pObj, param2, code);
}
