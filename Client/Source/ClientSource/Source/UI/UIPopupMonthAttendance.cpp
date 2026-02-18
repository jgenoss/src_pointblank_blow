#include "pch.h"
#include "UIPopupMonthAttendance.h"

#include "ItemImageMgr.h"
#include "UIUtil.h"
#include "AttendanceContext.h"
#include "ShopContext.h"
#include "UIShopFunction.h"
#include "UIMainFrame.h"

I3_CLASS_INSTANCE(UIPopupMonthAttendance);

extern "C" {
	int UIPopupMonthAttendance_ClickButton( LuaState * L)
	{
		UIPopupMonthAttendance* p = static_cast<UIPopupMonthAttendance*>(GameUI::GetPopup(UPW_MONTH_ATTENDANCE));
		if( p != nullptr)
			p->ClickButton();
		return 0;
	}

	int UIPopupMonthAttendance_ItemClick( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UIPopupMonthAttendance* p = static_cast<UIPopupMonthAttendance*>(GameUI::GetPopup(UPW_MONTH_ATTENDANCE));
		if( p != nullptr)
			p->SelectReward(idx);
		return 0;
	}
}
LuaFuncReg UIPopupMonthAttendance_Glue[] = {
	{"Click",		UIPopupMonthAttendance_ClickButton},
	{"ItemClick",	UIPopupMonthAttendance_ItemClick},
	{ nullptr,			nullptr}	
};

UIPopupMonthAttendance::UIPopupMonthAttendance() : m_RewardItemIdx(-1), m_TodayAttendance(-1), m_EventState(0)
, m_ViewItemIdx(-1)
{
	for(INT32 i = 0; i < 2; i++)
	{
		m_pImgCtrl[i] = nullptr;
		m_pRewardText[i] = nullptr;
		m_pRewardIcon[i] = nullptr;
	}

	m_vMonthItemBG.reserve(MONTH_MAX_COUNT);
}

UIPopupMonthAttendance::~UIPopupMonthAttendance()
{

}

void UIPopupMonthAttendance::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Attandence.i3UIs", UIPopupMonthAttendance_Glue);
}

bool UIPopupMonthAttendance::OnEntranceStart( void * pArg1 /*= nullptr */, void * pArg2 /*= nullptr */ )
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	// 데이터를 설정하기 전에 미리 세팅합니다.
	// -1을 하는 이유는 출석 체크를 하게 되면 AttendanceDay가 올라가서 Complate되면 1을 빼야
	// 현재 날짜가 나옵니다.
	m_TodayAttendance = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;
	if(m_EventState == ATTENDANCE_STATE_COMPLETE)
		m_TodayAttendance--;

	// 이벤트 데이터 설정
	_SetEventData();

	return true;
}

void UIPopupMonthAttendance::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pImgCtrl[0] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "GiveItem1");
	m_pImgCtrl[1] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "GiveItem2");

	m_pRewardText[0] = (i3UIStaticText *) pScene->FindChildByName("GiveItemText1");
	m_pRewardText[1] = (i3UIStaticText *) pScene->FindChildByName("GiveItemText2");

	m_pRewardIcon[0] = (i3UIButtonImageSet *) pScene->FindChildByName("BtnIS_ItemBg_01");
	m_pRewardIcon[1] = (i3UIButtonImageSet *) pScene->FindChildByName("BtnIS_ItemBg_02");

	// 많이 사용하므로 저장해둡니다.
	m_EventState = AttendanceContext::i()->GetAttendanceUser()->m_ui8EventState;

	_SetStaticText(pScene);
}

void UIPopupMonthAttendance::_ClearAtUnload( i3UIScene * pScene )
{
	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE(m_pImgCtrl[i]);
		m_pRewardText[i] = nullptr;
	}

	INT32 count = m_vMonthItemBG.size();
	for(INT32 i = 0; i < count; i++)
	{
		i3GameNode * parent = m_vMonthItemBG[i]->getParent();
		parent->RemoveChild(m_vMonthItemBG[i]);
		//I3_SAFE_RELEASE(m_vMonthItemBG[i]);
	}
	m_vMonthItemBG.clear();

	m_RewardItemIdx = -1;
	m_ViewItemIdx = -1;

	// 우선 BG부터 제거 합니다.
	UIPopupBase::_ClearAtUnload(pScene);
}

void UIPopupMonthAttendance::_SetEventData()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if(pInfo == nullptr) return;

	i3::rc_wstring wstrEventName;
	i3::sprintf(wstrEventName, L"%s", pInfo->m_strEventName);
	SetStaticText("EventName", wstrEventName);

	// 출석 이벤트 기간
	i3::rc_wstring wstrPeriod;
	SetPeriod(wstrPeriod, pInfo->m_ui32StartDate, pInfo->m_ui32EndDate);
	SetStaticText("EventPeriod", wstrPeriod);

	i3::rc_wstring wstrNextPeriod;
	SetPeriod(wstrNextPeriod, pInfo->m_ui32NextStartDate, pInfo->m_ui32NextEndDate, true);
	SetStaticText("Next_Check", wstrNextPeriod);

	// 달력 표시
	_SetViewMonthItem();

	// 오늘 아이템 표시
	_SetViewTodayItem();

	// 최초 선택을 표시합니다.
	SelectReward(0);
}	

void UIPopupMonthAttendance::_SetStaticText( i3UIScene * pScene )
{
	i3UIStaticText * UIText = nullptr;
	i3UIButton * UIButton = nullptr;

	UIText = (i3UIStaticText *) pScene->FindChildByName("Static_Title");
	if(UIText != nullptr)
	{
		UIText->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK_EVENT"));
	}

	UIText = (i3UIStaticText *) pScene->FindChildByName("Static_CheckItem");
	if(UIText != nullptr)
	{
		UIText->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_SELECT_ITEM"));
	}

	UIButton = (i3UIButton *) pScene->FindChildByName("Btn_OK");
	if(UIButton != nullptr)
	{
		if(m_EventState == ATTENDANCE_STATE_COMPLETE)
			UIButton->SetText(GAME_STRING("STBL_IDX_BUTTON_CLOSE"));
		else
			UIButton->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK"));
	}
}

void UIPopupMonthAttendance::_SetViewMonthItem()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if(pInfo == nullptr) return;
	
	// 날 별로 goods item을 세팅 합니다.
	INT32 count = pInfo->m_ui8AttendanceDay;
	/*while(pInfo->m_ui32GiveGoodsID[count].m_ui8GoodsCount > 0)
	{
		count++;
	}*/

	// 여기는 전부 데이터를 출력 합니다.
	for(INT32 i = 0; i < count; i++)
	{
		i3::rc_wstring wstrString;
		i3::string controlname;

		// 몇 일 차
		i3::sprintf(wstrString, GAME_STRING("STR_TBL_UI_ATTENDANCE_DAY"), i+1);
		i3::sprintf(controlname, "Day%02d", i+1);
		SetStaticText(controlname, wstrString);

		// item 세팅
		ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[i];
		for(INT32 itemcount = 0; itemcount < (INT32)item_.m_ui8GoodsCount; itemcount++)
		{
			i3::sprintf(controlname, "DayItem%02d_%d", i+1, itemcount+1);
			
			INT32 itemID = CShopContext::i()->ConvertGoodsID(item_.m_ui32GoodsID[itemcount]);
			usf::GetShopItemName(wstrString, itemID);

			SetStaticText(controlname, wstrString);
		}

		// 최 하위로 붙여야 합니다.
		//UINT8 attendanceday = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;
		INT32 shape_type = -1;

		if( m_TodayAttendance > i) shape_type = CLEAR_ATTENDANCE;
		else if(m_TodayAttendance == i) shape_type = TODAY_ATTENDANCE;
		else shape_type = NEXT_ATTENDANCE;

		_SetViewItemBG(i, shape_type);
	}
}

void UIPopupMonthAttendance::_SetViewTodayItem()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if(pInfo == nullptr) return;

	// 여기서 데이터를 꺼내옵니다.
	//UINT8 attendanceday = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;
	ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[m_TodayAttendance];
	
	UINT8 goodscount = item_.m_ui8GoodsCount;

	// Goods가 1개라도 있을 경우
	if(goodscount != 0)
	{
		m_ViewItemIdx = m_TodayAttendance;

		// 다음 아이템 세팅
		__SetTodayItemIcon(item_, m_EventState == ATTENDANCE_STATE_COMPLETE);
	}
	// Goods가 없을 경우 뒤에 내용을 체크합니다.
	else
	{
		// 가장 가까운 정보를 세팅 한 뒤 Input을 막습니다.
		INT32 i = 0;
		for(i = m_TodayAttendance; i < (INT32)pInfo->m_ui8AttendanceDay; i++)
		{
			ATTENDANCE_CHOICE_ITEM temp = pInfo->m_ui32GiveGoodsID[i];
			
			if(temp.m_ui8GoodsCount != 0)
			{
				item_ = temp;
				break;
			}
		}

		m_ViewItemIdx = i;
		
		// 다음 아이템 세팅
		__SetTodayItemIcon(item_, true);

		// UI Text를 바꿉니다.
		i3UIStaticText * UIText = (i3UIStaticText *) GetScene(0)->FindChildByName("Static_CheckItem");
		if(UIText != nullptr)
		{
			UIText->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_NON_ITEM"));
		}
	}
}

void UIPopupMonthAttendance::__SetTodayItemIcon(ATTENDANCE_CHOICE_ITEM& item_, bool IsviewOnly)
{
	INT32 goodscount = item_.m_ui8GoodsCount;

	// 아이템을 세팅합니다.
	for(INT32 i = 0; i < goodscount; i++)
	{
		INT32 itemID = CShopContext::i()->ConvertGoodsID(item_.m_ui32GoodsID[i]);

		i3::rc_wstring itemstring;
		usf::GetShopItemName(itemstring, itemID);
		m_pRewardText[i]->SetText(itemstring);

		GetItemImageMgr()->SelectImage(m_pImgCtrl[i], itemID);

		if(IsviewOnly)
		{
			m_pRewardIcon[i]->setInputDisable(true);
			m_pRewardIcon[i]->addStyle(I3UI_STYLE_IGNOREINPUT);
			m_pRewardIcon[i]->SetViewOnly(true);
		}
	}

	// 1개 일 경우
	if( goodscount < 2)
	{
		// 우측항목 끄기
		m_pRewardIcon[1]->EnableCtrl(false);

		VEC3D * pos_ = m_pRewardIcon[0]->getPos();
		VEC3D * pos2_ = m_pRewardIcon[1]->getPos();
		REAL32 xpos = (pos2_->x - pos_->x) / 2.f;
		pos_->x += xpos;
		m_pRewardIcon[0]->setPos(pos_);
	}
}

void UIPopupMonthAttendance::_SetViewItemBG( INT32 idx, INT32 shape_type )
{
	if( shape_type == -1) return;

	i3::string controlname;
	i3::string bgControl;
	i3::sprintf(controlname, "DayFrame%02d", idx+1);

	i3UIFrameWnd * pWnd = (i3UIFrameWnd * )GetScene(0)->FindChildByName(controlname.c_str());
	if(pWnd == nullptr) return;

	// Shape Template 찾기
	switch(shape_type)
	{
	case CLEAR_ATTENDANCE:
		i3::sprintf(bgControl, "V3_BtnIS_DayCheck_02");
		i3::sprintf(controlname, "V3_BtnIS_DayCheck_03");
		break;
	case TODAY_ATTENDANCE:
		if(m_EventState == ATTENDANCE_STATE_COMPLETE)
		{
			i3::sprintf(bgControl, "V3_BtnIS_DayCheck_02");
			i3::sprintf(controlname, "V3_BtnIS_DayCheck_03");
		}
		else
		{
			i3::sprintf(controlname, "V3_BtnIS_DayCheck_02");
		}
		break;
	case NEXT_ATTENDANCE:
		i3::sprintf(controlname, "V3_BtnIS_DayCheck_02");
		break;
	}

	// 위치 이동 / 크기 조절
	VEC3D wndPos;
	wndPos.Set(0.0f, 0.0f, 0.0f);
	VEC2D * wndSize = pWnd->getSize();

	if(bgControl.empty() == false)
	{
		i3UITemplate* BGTemplate = i3UI::getUILibrary()->FindByName(bgControl.c_str());
		i3UIButtonImageSet * UIButton = (i3UIButtonImageSet*)i3UI::CreateControl(BGTemplate, &wndPos, wndSize, pWnd);

		m_vMonthItemBG.push_back(UIButton);
		UIButton->SetShapeIdx(1);
		UIButton->SetViewOnly(true);
	}

	i3UITemplate* BGTemplate = i3UI::getUILibrary()->FindByName(controlname.c_str());
	i3UIButtonImageSet * UIButton = (i3UIButtonImageSet*)i3UI::CreateControl(BGTemplate, &wndPos, wndSize, pWnd);

	m_vMonthItemBG.push_back(UIButton);
	
	// Shape 변경
	if(shape_type != TODAY_ATTENDANCE || m_EventState == ATTENDANCE_STATE_COMPLETE)
	{
		UIButton->SetShapeIdx(1);
		UIButton->SetViewOnly(true);
	}
	else
	{
		// Title을 맨 위로 올립니다.
		// 구조가 바뀌어서 다시 세팅합니다.
		i3::sprintf(controlname, "Day%02d", idx + 1);
		i3GameNode * title_text = pWnd->FindChildByName(controlname.c_str());
		i3GameNode * uiParent = title_text->getParent();
		if(uiParent != nullptr)
			title_text->ChangeParent(uiParent);
	}
}

void UIPopupMonthAttendance::ClickButton()
{
	// Click 이벤트
	if(m_EventState == ATTENDANCE_STATE_COMPLETE)
	{	
		// 끄면 됩니다.
		OnKey_Escape();
	}
	else
	{
		const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
		GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CHECK, &pInfo->m_ui32Version, &pInfo->m_ui8AttendanceType);
	}
}

void UIPopupMonthAttendance::SelectReward( INT32 idx )
{
	if(idx != m_RewardItemIdx && m_TodayAttendance == m_ViewItemIdx)
	{
		if(-1 < idx && idx < 2)
		{
			if( m_RewardItemIdx == -1 ) m_RewardItemIdx = 0;

			m_pRewardIcon[m_RewardItemIdx]->SetShapeIdx(0);
			m_pRewardIcon[idx]->SetShapeIdx(1);

			m_RewardItemIdx = idx;
		}
	}
	else if(m_RewardItemIdx == -1)
	{
		// 이 경우는 최초 한 번 뿐, Complate일 경우이다.
		// 여기서 내가 선택한 인덱스를 던지면 됩니다.
		if( m_TodayAttendance == m_ViewItemIdx)
		{
			m_RewardItemIdx = AttendanceContext::i()->GetAttendanceUser()->m_ui8ItemNum;

			if(m_RewardItemIdx != -1)
			{
				m_pRewardIcon[m_RewardItemIdx]->SetShapeIdx(1);
			}
		}
	}
}

void UIPopupMonthAttendance::ReceiveGameEvent_Attendance_Check( INT32 arg, const i3::user_data &UserData )
{
	switch( arg)
	{
		// 성공
	case EVENT_ERROR_ATTENDANCE_SUCCESS:
		_SetSuccessUI();
		_SetSuccessEvent();
		break;
	default:
		SetErrorMsg(arg);
		break;
	}
}

void UIPopupMonthAttendance::_SetSuccessUI()
{
	// Check 변경
	// 서버에서 변경 안되게 전달되어서 강제 세팅
	AttendanceContext::i()->SetAttendanceState(ATTENDANCE_STATE_COMPLETE);
	m_EventState = ATTENDANCE_STATE_COMPLETE;

	// 생각해보니 끌껀데 왜 하고 있을까?
	//// 달력을 변경하기 전 BG를 지웁니다.
	//INT32 count = m_vMonthItemBG.size();
	//for(INT32 i = 0; i < count; i++)
	//{
	//	i3GameNode * parent = m_vMonthItemBG[i]->getParent();
	//	parent->RemoveChild(m_vMonthItemBG[i]);
	//	I3_SAFE_RELEASE(m_vMonthItemBG[i]);
	//}
	//m_vMonthItemBG.clear();
	//
	//// 달력 변경
	//_SetViewMonthItem();

	//// 버튼 텍스트 변경
	//i3UIButton * ok_btn = (i3UIButton*)GetScene(0)->FindChildByName("Btn_OK");
	//if(ok_btn != nullptr)
	//{
	//	ok_btn->SetText(GAME_STRING("STBL_IDX_BUTTON_CLOSE"));
	//}
}



void UIPopupMonthAttendance::_SetSuccessEvent()
{	
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
	if( pInfo == nullptr) return;


	INT32 nCurSel = 0;

	ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[m_TodayAttendance];

	if( item_.m_ui8GoodsCount > 1)
	{
		// 선택된 걸 저장합니다.
		nCurSel = m_RewardItemIdx;
	}

	// Item Count가 0이 아닐경우 즉 오늘 받을 수 있는 아이템이 있을 경우에만 보냅니다.
	if( item_.m_ui8GoodsCount != 0)
	{
		GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CLEAR, pInfo, &nCurSel);
		AttendanceContext::i()->SetAttendanceSelectItem((UINT8)nCurSel);
	}
	else
	{
		// 오늘 받을 수 있는 아이템이 없을 경우 메시지 박스만 띄우고 끝냅니다.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK_SUCCESS"));
	}

	// 팝업 종료
	GetMainFrame()->ClosePopup(UPW_MONTH_ATTENDANCE);
}
