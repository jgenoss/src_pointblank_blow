#include "pch.h"

#include "UIPopupDormantEvent.h"
#include "UIPopupMonthAttendance.h"

#include "ItemImageMgr.h"
#include "UIUtil.h"
#include "AttendanceContext.h"
#include "ShopContext.h"
#include "UIShopFunction.h"
#include "UIMainFrame.h"

I3_CLASS_INSTANCE(UIPopupDormantEvent);

extern "C" {
	int UIPopupDormantEvent_Confirm(LuaState * L)
	{
		UIPopupDormantEvent* p = static_cast<UIPopupDormantEvent*>(GameUI::GetPopup(UPW_DORMANT_EVENT));
		if (p != nullptr)
			p->Confirm();
		return 0;
	}

	int UIPopupDormantEvent_SelectItem(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UIPopupDormantEvent* p = static_cast<UIPopupDormantEvent*>(GameUI::GetPopup(UPW_DORMANT_EVENT));
		if (p != nullptr)
			p->SelectReward(idx);
		return 0;
	}
}

void	UIPopupDormantEvent::StopAnim()
{
	m_pFrmEffect->EnableCtrl(false);
	m_bAnimState = false;

	if (m_pBtnOK != nullptr)
		m_pBtnOK->enable_btn();
}

LuaFuncReg UIPopupDormantEvent_Glue[] = {
	{ "Confirm",		UIPopupDormantEvent_Confirm },
	{ "SelectItem",		UIPopupDormantEvent_SelectItem },
	{ nullptr,			nullptr }
};

UIPopupDormantEvent::UIPopupDormantEvent() : m_rDelayTime_bullet(0.f), m_pFrmEffect(nullptr), m_bAnimState(true), m_rDelayTime_back(0.f), m_RewardItemIdx(-1),
m_r32Bullet_PosX(-413), m_r32BulletBack_PosX(404), m_r32BulletBack_MoveStart_PosX(210), m_r32BulletSpeed(24.37f), m_r32AnimDuration(3), m_InputState(INPUT_NONE)
{
	m_vMonthItemBG.reserve(MONTH_MAX_COUNT);
}

UIPopupDormantEvent::~UIPopupDormantEvent()
{
}

void UIPopupDormantEvent::OnCreate(i3GameNode * pParent)
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Popup/PBRe_PopUp_DormantEvent.i3UIs", UIPopupDormantEvent_Glue);
}

bool UIPopupDormantEvent::OnEntranceStart(void * pArg1 /*= nullptr */, void * pArg2 /*= nullptr */)
{
	if (UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	m_TodayAttendance = AttendanceContext::i()->GetAttendanceUser(ATTENDANCE_TYPE_DORMANT)->m_ui8AttendanceDay;
	if (m_EventState == ATTENDANCE_STATE_COMPLETE)
	{
		m_TodayAttendance--;
		if (m_pBtnOK != nullptr)
			m_pBtnOK->enable_btn();
	}
	else
	{
		if (m_pBtnOK != nullptr)
			m_pBtnOK->disable_btn();
	}

	UIPopupBase::_AttachToTopScreen();

	ResetAnimation();
	SetEventData();

	return true;
}

void UIPopupDormantEvent::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);

	m_pFrmEffect = static_cast<i3UIFrameWnd*>(pScene->FindChildByName("Effect"));
	m_pSlideBack = static_cast<i3UIImageBox*>(pScene->FindChildByName("effect_slide_back"));
	m_pSlideBullet = static_cast<i3UIImageBox*>(pScene->FindChildByName("effect_slide"));
	m_pBtnOK = static_cast<i3UIButtonComposed3*>(pScene->FindChildByName("Btn_OK"));

	m_pImgCtrl[0] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "GiveItem1");
	m_pImgCtrl[1] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "GiveItem2");

	m_pRewardText[0] = (i3UIStaticText *)pScene->FindChildByName("GiveItemText1");
	m_pRewardText[1] = (i3UIStaticText *)pScene->FindChildByName("GiveItemText2");

	m_pRewardIcon[0] = (i3UIButtonImageSet *)pScene->FindChildByName("BtnIS_ItemBg_01");
	m_pRewardIcon[1] = (i3UIButtonImageSet *)pScene->FindChildByName("BtnIS_ItemBg_02");

	/*휴면 출석 확인*/
	i3UIStaticText* pText = static_cast<i3UIStaticText*>(pScene->FindChildByName("Static_Title"));
	if (pText != nullptr)		pText->SetText(GAME_RCSTRING("STR_TBL_UI_DORMANT_CHECK"));

	/*휴면 출석 확인 이벤트*/
	pText = static_cast<i3UIStaticText*>(pScene->FindChildByName("EventName"));
	if (pText != nullptr)		pText->SetText(GAME_RCSTRING("STR_TBL_UI_DORMANT_CHECK_EVENT"));

	/*휴면 출석체크 보상 아이템을 선택하세요*/
	pText = static_cast<i3UIStaticText*>(pScene->FindChildByName("Static_CheckItem"));
	if (pText != nullptr)		pText->SetText(GAME_RCSTRING("STR_TBL_UI_DORMANT_SELECT_ITEM"));

	m_EventState = (ATTENDANCE_STATE)AttendanceContext::i()->GetAttendanceUser(ATTENDANCE_TYPE_DORMANT)->m_ui8EventState;

	/*출석 확인*/
	if (m_pBtnOK != nullptr)
	{
		if (m_EventState == ATTENDANCE_STATE_COMPLETE)
			m_pBtnOK->SetText(GAME_STRING("STBL_IDX_BUTTON_CLOSE"));
		else
			m_pBtnOK->SetText(GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK"));
	}
}

void UIPopupDormantEvent::SetEventData()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo(ATTENDANCE_TYPE_DORMANT);
	if (pInfo == nullptr) return;

	{
		i3::rc_wstring wstr;
		i3::sprintf(wstr, L"%s", pInfo->m_strEventName);
		SetStaticText("EventName", wstr);
	}

	{
		i3::rc_wstring wstr;
		SetPeriod(wstr, pInfo->m_ui32StartDate, pInfo->m_ui32EndDate);
		SetStaticText("EventPeriod", wstr);
	}

	{
		i3::rc_wstring wstr;
		SetPeriod(wstr, pInfo->m_ui32NextStartDate, pInfo->m_ui32NextEndDate, true);
		SetStaticText("Next_Check", wstr);
	}

	// Daily Item - 31
	INT32 count = pInfo->m_ui8AttendanceDay;
	for (INT32 i = 0; i < count; i++)
	{
		i3::rc_wstring wstrString;
		i3::string controlname;

		// n일차
		i3::sprintf(wstrString, GAME_STRING("STR_TBL_UI_ATTENDANCE_DAY"), i + 1);
		i3::sprintf(controlname, "Day%02d", i + 1);
		SetStaticText(controlname, wstrString);

		// item Text 세팅
		ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[i];
		for (INT32 itemcount = 0; itemcount < (INT32)item_.m_ui8GoodsCount; itemcount++)
		{
			i3::sprintf(controlname, "DayItem%02d_%d", i + 1, itemcount + 1);

			INT32 itemID = CShopContext::i()->ConvertGoodsID(item_.m_ui32GoodsID[itemcount]);
			usf::GetShopItemName(wstrString, itemID);

			SetStaticText(controlname, wstrString);
		}

		INT32 shape_type = -1;

		if (m_TodayAttendance > i) shape_type = CLEAR_ATTENDANCE;
		else if (m_TodayAttendance == i) shape_type = TODAY_ATTENDANCE;
		else shape_type = NEXT_ATTENDANCE;

		SetCompletedDay(i, shape_type);
	}

	// Reward UI 
	SetViewTodayItem(); 

	// Set Default Reward
	SelectReward(0);
}

void UIPopupDormantEvent::SetViewTodayItem()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo(ATTENDANCE_TYPE_DORMANT);
	if (pInfo == nullptr) return;

	ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[m_TodayAttendance];
	UINT8 goodscount = item_.m_ui8GoodsCount;

	if (goodscount != 0)
	{
		m_ViewItemIdx = m_TodayAttendance;
		SetTodayItemIcon(item_, m_EventState == ATTENDANCE_STATE_COMPLETE);
	}
	else 
	{
		// Goods가 없을 경우 뒤에 내용을 체크합니다.
		// 가장 가까운 날짜의 정보를 출력하고 입력을 제한합니다.

		INT32 i = 0;
		for (i = m_TodayAttendance; i < (INT32)pInfo->m_ui8AttendanceDay; i++)
		{
			ATTENDANCE_CHOICE_ITEM temp = pInfo->m_ui32GiveGoodsID[i];

			if (temp.m_ui8GoodsCount != 0)
			{
				item_ = temp;
				break;
			}
		}
		m_ViewItemIdx = i;

		SetTodayItemIcon(item_, true);

		i3UIStaticText * UIText = (i3UIStaticText *)GetScene(0)->FindChildByName("Static_CheckItem");
		if (UIText != nullptr)
		{
			UIText->SetText(GAME_STRING("STR_TBL_UI_DORMANT_NON_ITEM"));
		}
	}
}


void UIPopupDormantEvent::SetTodayItemIcon(ATTENDANCE_CHOICE_ITEM& item_, bool isViewOnly)
{
	INT32 i32GoodsCount = item_.m_ui8GoodsCount;

	for (INT32 i = 0; i < i32GoodsCount; i++)
	{
		INT32 itemID = CShopContext::i()->ConvertGoodsID(item_.m_ui32GoodsID[i]);

		i3::rc_wstring itemstring;
		usf::GetShopItemName(itemstring, itemID);
		m_pRewardText[i]->SetText(itemstring);

		GetItemImageMgr()->SelectImage(m_pImgCtrl[i], itemID);

		if (isViewOnly)
		{
			m_pRewardIcon[i]->setInputDisable(true);
			m_pRewardIcon[i]->addStyle(I3UI_STYLE_IGNOREINPUT);
			m_pRewardIcon[i]->SetViewOnly(true);
		}
	}

	if (i32GoodsCount < 2)
	{
		m_pRewardIcon[1]->EnableCtrl(false);

		VEC3D * pos_ = m_pRewardIcon[0]->getPos();
		VEC3D * pos2_ = m_pRewardIcon[1]->getPos();
		REAL32 xpos = (pos2_->x - pos_->x) / 2.f;
		pos_->x += xpos;
		m_pRewardIcon[0]->setPos(pos_);
	}
}

void UIPopupDormantEvent::SetCompletedDay(INT32 idx, INT32 shape_type)
{
	if (shape_type == -1) return;

	i3::string controlname;
	i3::string bgControl;
	i3::sprintf(controlname, "DayFrame%02d", idx + 1);

	i3UIFrameWnd * pWnd = (i3UIFrameWnd *)GetScene(0)->FindChildByName(controlname.c_str());
	if (pWnd == nullptr) return;

	switch (shape_type)
	{
	case CLEAR_ATTENDANCE:
		i3::sprintf(bgControl, "V3_BtnIS_DayCheck_02");
		i3::sprintf(controlname, "V3_BtnIS_DayCheck_03");
		break;
	case TODAY_ATTENDANCE:
		if (m_EventState == ATTENDANCE_STATE_COMPLETE)
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

	VEC3D wndPos;
	wndPos.Set(0.0f, 0.0f, 0.0f);
	VEC2D * wndSize = pWnd->getSize();

	if (bgControl.empty() == false)
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

	if (shape_type != TODAY_ATTENDANCE || m_EventState == ATTENDANCE_STATE_COMPLETE)
	{
		UIButton->SetShapeIdx(1);
		UIButton->SetViewOnly(true);
	}
	else
	{
		i3::sprintf(controlname, "Day%02d", idx + 1);
		i3GameNode * title_text = pWnd->FindChildByName(controlname.c_str());
		i3GameNode * uiParent = title_text->getParent();
		if (uiParent != nullptr)
			title_text->ChangeParent(uiParent);
	}
}

void UIPopupDormantEvent::_ClearAtUnload(i3UIScene * pScene)
{
	INT32 count = m_vMonthItemBG.size();
	for (INT32 i = 0; i < count; i++)
	{
		i3GameNode * parent = m_vMonthItemBG[i]->getParent();
		parent->RemoveChild(m_vMonthItemBG[i]);
	}
	m_vMonthItemBG.clear();

	m_RewardItemIdx = -1;
	m_ViewItemIdx = -1;

	UIPopupBase::_ClearAtUnload(pScene);
}

void UIPopupDormantEvent::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	if (m_bAnimState == true)
	{	
		PlayAnimation(rDeltaSeconds);
	}
}

bool UIPopupDormantEvent::DisableBtnConfirm()
{
	if (m_InputState == INPUT_NON_RELATED)
		return true;

	if (GameUI::IsMousePressed(I3I_MOUSE_LBUTTON))
	{
		if (m_InputState == INPUT_NONE)
		{
			m_InputState = INPUT_FIRST_PRESS;
		}
	}
	else
	{
		if (m_InputState == INPUT_FIRST_PRESS)
		{
			m_InputState = INPUT_NON_RELATED;
			return true;
		}
	}

	return false;
}

void UIPopupDormantEvent::ReceiveGameEvent_Attendance_Check(INT32 arg, const i3::user_data &UserData)
{
	switch (arg)
	{
		case EVENT_ERROR_ATTENDANCE_SUCCESS:
			CompleteEvent();
			break;
		default:
			SetErrorMsg(arg); // Base
			break;
	}
}

void UIPopupDormantEvent::CompleteEvent()
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo(ATTENDANCE_TYPE_DORMANT);
	if (pInfo == nullptr) return;

	AttendanceContext::i()->SetAttendanceState(ATTENDANCE_STATE_COMPLETE, ATTENDANCE_TYPE_DORMANT);
	m_EventState = ATTENDANCE_STATE_COMPLETE;

	INT32 nCurSel = 0;
	ATTENDANCE_CHOICE_ITEM item_ = pInfo->m_ui32GiveGoodsID[m_TodayAttendance];

	if (item_.m_ui8GoodsCount > 1)
	{
		nCurSel = m_RewardItemIdx;
	}

	if (item_.m_ui8GoodsCount != 0)
	{
		GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CLEAR, pInfo, &nCurSel);
		AttendanceContext::i()->SetAttendanceSelectItem((UINT8)nCurSel, ATTENDANCE_TYPE_DORMANT);
	}
	else
	{
		// 오늘 받을 수 있는 아이템이 없을 경우 메시지 박스만 띄우고 끝냅니다.
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_UI_ATTENDANCE_CHECK_SUCCESS"));
	}

	GetMainFrame()->ClosePopup(UPW_DORMANT_EVENT);
}

bool UIPopupDormantEvent::OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, I3_EVT_CODE code)
{
	if (code == I3_EVT_CODE_ACTIVATE)
	{
		if (event == I3_EVT_INPUT)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();
			i3InputMouse* pMouse = pMgr->GetMouse();

			if ((pMouse->GetButtonState() & I3I_MOUSE_LBUTTON))
			{
				StopAnim();
			}

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				m_bAnimState = false; 
				// [확인] 버튼이 아닌 다른 방법으로 팝업 또는 게임이 종료되면 
				// 서버로 잘 받았다는 처리를 해주지 않습니다.
				GetMainFrame()->ClosePopup(UPW_DORMANT_EVENT);
			}
		}
	}

	return UIPopupBase::OnEvent(event, pObj, param2, code);
}

void UIPopupDormantEvent::Confirm()
{
	if (m_EventState == ATTENDANCE_STATE_COMPLETE)
	{
		OnKey_Escape();
	}
	else
	{
		const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo(ATTENDANCE_TYPE_DORMANT);
		if (pInfo != nullptr)
		{
			GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CHECK, &pInfo->m_ui32Version, &pInfo->m_ui8AttendanceType);
		}
		else
		{
			OnKey_Escape();
			GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_UNKNOWN"));
		}
	}
}

void UIPopupDormantEvent::SelectReward(INT32 idx)
{
	if (idx != m_RewardItemIdx && m_TodayAttendance == m_ViewItemIdx)
	{
		if (-1 < idx && idx < 2)
		{
			if (m_RewardItemIdx == -1) m_RewardItemIdx = 0;

			m_pRewardIcon[m_RewardItemIdx]->SetShapeIdx(0);
			m_pRewardIcon[idx]->SetShapeIdx(1);

			m_RewardItemIdx = idx;
		}
	}
	else if (m_RewardItemIdx == -1)
	{
		// 이 경우는 최초 한 번 뿐, Complate일 경우이다.
		// 여기서 내가 선택한 인덱스를 던지면 됩니다.
		if (m_TodayAttendance == m_ViewItemIdx)
		{
			m_RewardItemIdx = AttendanceContext::i()->GetAttendanceUser(ATTENDANCE_TYPE_DORMANT)->m_ui8ItemNum;

			if (m_RewardItemIdx != -1)
			{
				m_pRewardIcon[m_RewardItemIdx]->SetShapeIdx(1);
			}
		}
	}
}

void UIPopupDormantEvent::ResetAnimation()
{
	m_pSlideBack->setPos(m_r32BulletBack_PosX, m_pSlideBack->getPos()->y, m_pSlideBack->getPos()->z);
	m_pSlideBullet->setPos(m_r32Bullet_PosX, m_pSlideBullet->getPos()->y, m_pSlideBullet->getPos()->z);

	m_pFrmEffect->EnableCtrl(true);
	m_pBtnOK->disable_btn();

	m_rDelayTime_bullet = 0.f;
	m_rDelayTime_back = 0.f;

	if (m_bAnimState == true)
	{
		bool isFirstDay = AttendanceContext::i()->GetAttendanceUser(ATTENDANCE_TYPE_DORMANT)->m_ui8AttendanceDay == 0 ? true : false;
		if (!isFirstDay)
		{
			StopAnim();
			m_InputState = INPUT_NON_RELATED;
		}
	}
	else
	{
		StopAnim();
		m_InputState = INPUT_NON_RELATED;
	}
}

void UIPopupDormantEvent::PlayAnimation(REAL32 rDeltaSeconds)
{
	m_rDelayTime_bullet += rDeltaSeconds;

	if (m_pFrmEffect == nullptr || m_pSlideBullet == nullptr || m_pSlideBack == nullptr) return;

	// 애니메이션 재생 시간이 지나면 애니메이션을 종료하고 팝업 내용을 보여줌
	if (m_rDelayTime_bullet >= m_r32AnimDuration && m_pFrmEffect->isEnable())
	{
		m_InputState = INPUT_NON_RELATED;
		StopAnim();
	}
	else
	{
		if (m_pSlideBullet->getPos()->x < i3UI::getManager()->getWidth())
		{
			// 프레임 간 시간을 비율 계산해서 이미지를 이동시켜줍니다. 
			float DistanceBullet = ((float)m_rDelayTime_bullet / 0.016f) * m_r32BulletSpeed;

			m_pSlideBullet->setPos(m_r32Bullet_PosX + DistanceBullet, m_pSlideBullet->getPos()->y, m_pSlideBullet->getPos()->z);
			if (m_pSlideBullet->getPos()->x > m_r32BulletBack_MoveStart_PosX)
			{
				m_rDelayTime_back += rDeltaSeconds;
				float DistanceBack = ((float)m_rDelayTime_back / 0.016f) * m_r32BulletSpeed;

				m_pSlideBack->setPos(m_r32BulletBack_PosX + DistanceBack, m_pSlideBack->getPos()->y, m_pSlideBack->getPos()->z);
			}
		}
	}
}