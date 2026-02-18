#include "pch.h"
#include "UIPhaseTemp_V10.h"

#include "UITabProfileDesignation_V10.h"
#include "UITabProfileMissionCard_V10.h"
#include "UIFloatChat_V10.h"
#include "UITopMenu_V10.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UI/UIShopFunction.h"
#include "UserInfoContext.h"

#include "UI/UIMainFrame_V10.h"


I3_CLASS_INSTANCE( UIPhaseTemp_V10);

UIPhaseTemp_V10::UIPhaseTemp_V10() 
{

}

UIPhaseTemp_V10::~UIPhaseTemp_V10()
{
	
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
/*virtual*/ void UIPhaseTemp_V10::OnCreate( i3GameNode * pParent)
{
	UIPhaseSubBase::OnCreate( pParent);

	AddScene("Scene/Main/temp.i3UIs", nullptr);
}

/*virtual*/ void UIPhaseTemp_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPhaseBase::_InitializeAtLoad(pScene);

	
	 			
}

/*virtual*/ void UIPhaseTemp_V10::_ClearAtUnload( i3UIScene * pScene )
{
	UIPhaseBase::_ClearAtUnload(pScene);

}

/*virtual*/ void UIPhaseTemp_V10::SetInputDisable(bool bDisable)
{
	UIBase::SetInputDisable(bDisable);

}



/*virtual*/ bool UIPhaseTemp_V10::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	//재정의한 탭 ProcessGameEvent는 PhaseProfile에서 case별로 이벤트 호출 
	UIPhaseBase::OnEvent(event,pObj,param2, code);
	

	return true;
}

bool	UIPhaseTemp_V10::OnExitStart()
{


	return UIPhaseSubBase::OnExitStart();
}

void UIPhaseTemp_V10::OnExitEnd( void)
{
	UIPhaseSubBase::OnExitEnd();

	
	//m_pWeaponRecord->SetEnable( false);
}

/*virtual*/ void UIPhaseTemp_V10::Leave( void)
{
	if ( i3::same_of<UIMainFrame_V10*>(g_pFramework->GetUIMainframe()))
	{
		UIMainFrame_V10 * pUIMainFrame = ((UIMainFrame_V10*)g_pFramework->GetUIMainframe());

		I3ASSERT( i3::kind_of<UIMainFrame_V10*>(pUIMainFrame));

		pUIMainFrame->GetTopMenu_V10()->AllMenuBtnShapeToNormal();
		
	}

	// 차후 연동 작업 시 로비로 돌아갈 때 사용
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_LEAVE_TEMP);
}

/*virtual*/ bool UIPhaseTemp_V10::OnClick( i3UIControl * pControl)
{
	if( UIPhaseSubBase::OnClick( pControl) == false)
		return false;

	return true;
}


/*virtual*/ void UIPhaseTemp_V10::OnLoadAllScenes()
{
	UIPhaseBase::OnLoadAllScenes();


}
bool UIPhaseTemp_V10::OnEntranceStart(void * pArg1, void * pArg2 )
{
	bool res = UIPhaseBase::OnEntranceStart();
	I3ASSERT(res);

	m_slide.ForceEnd();
	return false;
}
void UIPhaseTemp_V10::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();

	//UIPOPUPWIN popup = UPW_CLAN_MEMBER;
	//if(GetMainFrame()->IsOpenedPopup(UPW_CLAN_NON_MEMBER) == true )
	//	popup = UPW_CLAN_NON_MEMBER;
	//EnterPopup(GetMainFrame(), popup, true);

  	UIPOPUPWIN popup = UPW_CLAN_MEMBER;
  
 	if(GetMainFrame()->IsOpenedPopup(UPW_CLAN_NON_MEMBER) == true )
  		popup = UPW_CLAN_NON_MEMBER;
//  	GetMainFrame()->TogglePopup( popup);

	UIMainFrame* pFrame = GetMainFrame();
	if( pFrame->IsOpenedPopup(popup) )
	{
		//if(delay)
		{
			pFrame->ExclusedEnterPopup( UPW_NULL );
		}
		//else 		
		{
			//pFrame->ClosePopup(popup);
			//g_last_popup_tab = None; // OnMainFrameClosePopupAfter로 옮겨감
		}
	}
	else
	{
		//g_last_popup_tab = get_tab_idx(popup);

		//if(delay)
		{
// 			if( g_last_phase_tab != None )	//popup 이 활성화 될 때 바탕이 되는 subphase는 나간다.
// 			{
// 				if( pFrame->GetCurrentSubPhase() != nullptr)
// 					pFrame->GetCurrentSubPhase()->Leave();
// 			}

			pFrame->ExclusedEnterPopup( popup );
		}
		//else 	pFrame->OpenPopup(popup);		
	}
}

/*virtual*/ bool UIPhaseTemp_V10::OnKey_Escape()
{
	if( i3UI::getDisableUIInput() == true)
		return false;
	
	// 차후 연동 작업 시 로비로 돌아갈 때 사용
	GetMainFrame()->ExclusedEnterSubPhase( USP_TEMP);		

	return true;
}


