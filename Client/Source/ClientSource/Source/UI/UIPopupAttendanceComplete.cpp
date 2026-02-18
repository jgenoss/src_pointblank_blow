#include "pch.h"
#include "UIPopupAttendanceComplete.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "UIPopupAttendanceCheck.h"

#include "ItemImageMgr.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "AttendanceContext.h"
#include "ShopContext.h"

I3_CLASS_INSTANCE(UIPopupAttendanceComplete);

extern "C" {

	int UIPopupAttendanceComplete_ReceiveItem( LuaState * L)
	{
		UIPopupAttendanceComplete * p = static_cast<UIPopupAttendanceComplete*>(GameUI::GetPopup(UPW_ATTENDANCE_COMPLETE));
		if( p != nullptr)
			p->ReceiveItem();
		return 0;
	}
	
}
LuaFuncReg UIPopupAttendanceComplete_Glue[] = {
	{"ReceiveItem",		UIPopupAttendanceComplete_ReceiveItem},
	{nullptr,	nullptr}	
};

UIPopupAttendanceComplete::UIPopupAttendanceComplete() : m_bRes(false)
{
	m_nItemCount = 0;
	i3mem::FillZero(m_pImgCtrl, sizeof(m_pImgCtrl));
}

UIPopupAttendanceComplete::~UIPopupAttendanceComplete()
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE( m_pImgCtrl[i]);
	}
}

/*virtual*/ void UIPopupAttendanceComplete::OnCreate ( i3GameNode * pParent)
{
	UIPopupBase::OnCreate(pParent);

	i3::string path = "";
	i3UIScene* scene = nullptr;

	path = "Scene/Popup/PBRe_PopUp_DayCheckEvent_Result.i3UIs";
	scene = AddScene( path.c_str(), UIPopupAttendanceComplete_Glue);

	m_bRes = (scene != nullptr);
}

/*virtual*/ void UIPopupAttendanceComplete::OnLoadAllScenes()
{
	i3UIScene * pScene = GetScene(0);

	m_pImgCtrl[0] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_Item1");
	m_pImgCtrl[1] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_Item2");
}

/*virtual*/ void UIPopupAttendanceComplete::OnUnloadAllScenes()
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE(m_pImgCtrl[i]);
	}
}

/*virtual*/ bool UIPopupAttendanceComplete::OnEntranceStart( void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false) return false;

	return true;
}

/*virtual*/ void UIPopupAttendanceComplete::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	_SetEventData();
}

/*virtual*/ void UIPopupAttendanceComplete::OnExitEnd(void)
{
	UIPopupBase::OnExitEnd();
	
	if(GetMainFrame()->IsOpenedPopup(UPW_ATTENDANCE_CHECK))
	{
		UIPopupAttendanceCheck * pPopup = static_cast<UIPopupAttendanceCheck*>(GameUI::GetPopup(UPW_ATTENDANCE_CHECK));
		pPopup->SetBtnEnable(true);
	}
	else
	{
		AttendanceContext::i()->SetAttendanceState( ATTENDANCE_STATE_COMPLETE );

		UIPhaseLobby * pLobby = static_cast<UIPhaseLobby*>(GetMainFrame()->GetPhase(UIPHASE_LOBBY));
		pLobby->ShowAttendancePopup();
	}
}


void UIPopupAttendanceComplete::_SetEventData()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();

	I3ASSERT(pInfo);

	// 기본적으로는 오늘 보상만 표시하지만
	// 오늘 보상 받을 아이템이 없을 경우 다음번 보상을 표시한다.
	INT32 nItemIdx = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceItem;
	while( 0 == pInfo->m_ui32GiveGoodsID[ nItemIdx ].m_ui8GoodsCount )
	{
		++nItemIdx;
		I3ASSERT( nItemIdx < pInfo->m_ui8AttendanceDay );
	}

	ATTENDANCE_CHOICE_ITEM Item = pInfo->m_ui32GiveGoodsID[nItemIdx];

	m_nItemCount = Item.m_ui8GoodsCount;

	i3::rc_wstring wstrItemName[ ATTENDANCE_CHOICE_MAX_COUNT ];	

	for( INT32 i = 0 ; i < m_nItemCount ; i++ )
	{
		_SetItemInfo(wstrItemName[i], m_pImgCtrl[i], static_cast<INT32>(Item.m_ui32GoodsID[i]));
	}

	LuaState * L = _CallLuaFunction("SetEventData");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L,	m_nItemCount);
		i3Lua::PushStringUTF16To8(L,	wstrItemName[0].c_str(), wstrItemName[0].size());
		i3Lua::PushStringUTF16To8(L,	wstrItemName[1].c_str(), wstrItemName[1].size());
		_EndLuaFunction(L, 3);
	}
}


void UIPopupAttendanceComplete::ReceiveItem()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	I3ASSERT(pInfo);

	INT32 nCurSel = 0;

	// 지급 아이템이 2종류 일때만, 선택한 라디오 버튼을 받아온다.
	// 1개만 지급하는 경우는 0을 보냄
	if(m_nItemCount > 1)
	{
		LuaState * L = _CallLuaFunction("GetRadioIndex");
		if(L != nullptr)
		{
			nCurSel = _ReturnLuaInteger(L , 0);

			if(nCurSel < 0)
			{
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_ERROR_NONE_SELECT_ITEM"));
				return;
			}
		}
	}

	GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CLEAR, &pInfo, &nCurSel);

	GetMainFrame()->ClosePopup(UPW_ATTENDANCE_COMPLETE);
}

void UIPopupAttendanceComplete::_SetItemInfo(i3::rc_wstring& out, UIImgSetCtrl *pCtrl, INT32 nGoodsID)
{
	INT32 nItemID = CShopContext::i()->ConvertGoodsID(nGoodsID);

	usf::GetShopItemName(out, nItemID);
	GetItemImageMgr()->SelectImage(pCtrl, nItemID);
}

void UIPopupAttendanceComplete::_set_default_selection()
{
	i3UIScene* pScene = GetScene(0);
	i3UIRadio* rb = (i3UIRadio*)pScene->FindChildByName( "i3UIRadio_Item1");

	I3ASSERT( rb != nullptr);
	if( rb)
		rb->setCheck( true);
}
