#include "pch.h"
#include "UIPopupVisitEvent.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "AttendanceContext.h"
#include "ShopContext.h"

I3_CLASS_INSTANCE(UIPopupVisitEvent);

extern "C" {

	int UIPopupVisitEvent_ReceiveItem( LuaState * L)
	{
		UIPopupVisitEvent * p = static_cast<UIPopupVisitEvent*>(GameUI::GetPopup(UPW_VISIT_EVENT));
		if( p != nullptr)
			p->ReceiveItem();
		return 0;
	}
	
}
LuaFuncReg UIPopupVisitEvent_Glue[] = {
	{"ReceiveItem",		UIPopupVisitEvent_ReceiveItem},
	{nullptr,	nullptr}	
};

UIPopupVisitEvent::UIPopupVisitEvent() : m_bRes(false)
{
	m_nItemCount = 0;
	i3mem::FillZero(m_pImgCtrl, sizeof(m_pImgCtrl));
}

UIPopupVisitEvent::~UIPopupVisitEvent()
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE( m_pImgCtrl[i]);
	}
}


/*virtual*/ void UIPopupVisitEvent::OnCreate ( i3GameNode * pParent)
{
	UIPopupBase::OnCreate(pParent);

	i3::string path = "";
	i3UIScene* scene = nullptr;

	path = "Scene/Popup/PBRe_PopUp_1DayCheckEvent.i3UIs";
	scene = AddScene( path.c_str(), UIPopupVisitEvent_Glue);
	m_bRes = (scene != nullptr);
}

/*virtual*/ void UIPopupVisitEvent::OnLoadAllScenes()
{
	i3UIScene * pScene = GetScene(0);

	m_pImgCtrl[0] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_Item1");
	m_pImgCtrl[1] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_Item2");
}

/*virtual*/ void UIPopupVisitEvent::OnUnloadAllScenes()
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE(m_pImgCtrl[i]);
	}
}

/*virtual*/ void UIPopupVisitEvent::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	_SetEventData();
}

/*vurtual*/ void UIPopupVisitEvent::OnExitEnd(void)
{
	UIPopupBase::OnExitEnd();
	
	//AttendanceContext::i()->SetAttendanceType(0, ATTENDANCE_TYPE_NONE);
}

void UIPopupVisitEvent::_SetEventData()
{
	return;
}

void UIPopupVisitEvent::ReceiveItem()
{
	//const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo(0);

	//I3ASSERT(pInfo);

	//UINT32 ui32Version = pInfo->m_ui32Version;
	//INT32 nCurSel = 0;

	//// 지급 아이템이 2종류 일때만, 선택한 라디오 버튼을 받아온다.
	//// 1개만 지급하는 경우는 0을 보냄
	//if(m_nItemCount > 1)
	//{
	//	LuaState * L = _CallLuaFunction("GetRadioIndex");
	//	if(L != nullptr)
	//	{
	//		nCurSel = _ReturnLuaInteger(L , 0);

	//		if(nCurSel < 0)
	//		{
	//			GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_ERROR_NONE_SELECT_ITEM"));
	//			return;
	//		}
	//	}
	//}

	//GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CLEAR, &ui32Version, &nCurSel);

	GetMainFrame()->ClosePopup(UPW_VISIT_EVENT);
}

void UIPopupVisitEvent::_SetItemInfo(i3::rc_wstring& out, UIImgSetCtrl *pCtrl, INT32 nGoodsID)
{
	INT32 nItemID = CShopContext::i()->ConvertGoodsID(nGoodsID);

	usf::GetShopItemName(out, nItemID);
	GetItemImageMgr()->SelectImage(pCtrl, nItemID);
}

void UIPopupVisitEvent::_set_default_selection()
{
	i3UIScene* pScene = GetScene(0);
	i3UIRadio* rb = (i3UIRadio*)pScene->FindChildByName( "i3UIRadio_Item1");

	I3ASSERT( rb != nullptr);
	if( rb)
		rb->setCheck( true);
}
