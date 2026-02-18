#include "i3FrameworkPCH.h"
#include "i3UIModalDriver.h"
#include "i3UIDefines.h"
#include "i3UIControl.h"
#include "i3UIManager.h"

I3_CLASS_INSTANCE( i3UIModalDriver);


bool i3UIModalDriver::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pInfo = (I3UI_CONTROL_NOTIFY *) param2;
		i3UIControl * pCtrl = pInfo->m_pCtrl;

		switch( pInfo->m_nEvent)
		{
			case I3UI_EVT_CLICKED :
				if( i3::generic_is_iequal( pCtrl->GetNameString(), "Yes")  || i3::generic_is_iequal( pCtrl->GetNameString(), "OK") )
				{
					i3UI::EndModal( pInfo->m_pFrameWnd, pCtrl, I3UI_MODALRESULT_YES);
				}
				else if( i3::generic_is_iequal( pCtrl->GetNameString(), "No") )
				{
					i3UI::EndModal( pInfo->m_pFrameWnd, pCtrl, I3UI_MODALRESULT_NO);
				}
				else if( i3::generic_is_iequal( pCtrl->GetNameString(), "Cancel") )
				{
					i3UI::EndModal( pInfo->m_pFrameWnd, pCtrl, I3UI_MODALRESULT_CANCEL);
				}
				break;
		}
	}

	return true;
}