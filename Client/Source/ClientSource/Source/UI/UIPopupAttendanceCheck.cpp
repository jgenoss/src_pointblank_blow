#include "pch.h"
#include "UIPopupAttendanceCheck.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"

#include "UIUtil.h"
#include "UIMath.h"
#include "AttendanceContext.h"
#include "ShopContext.h"

UIAttendanceStampSlot::UIAttendanceStampSlot(void)
{
	m_pStampCtrl = i3UIButtonImageSet::new_object();
	I3ASSERT(m_pStampCtrl);

	m_pStampCtrl->SetTemplateByName( "BtnIS_Stamp");
	m_pStampCtrl->setSize( STAMP_WIDTH, STAMP_HEIGHT);
}

UIAttendanceStampSlot::~UIAttendanceStampSlot(void)
{
	if( m_pStampCtrl )
	{
		i3UIFrameWnd * pParent = i3::same_cast<i3UIFrameWnd*>(m_pStampCtrl->getParent());

		if( pParent)
			pParent->RemoveChild(m_pStampCtrl);

		I3_SAFE_RELEASE( m_pStampCtrl);
	}
}

void UIAttendanceStampSlot::SetStampData(i3UIFrameWnd * pParent, const INT32 idx, const INT32 count)
{
	I3ASSERT( m_pStampCtrl != nullptr );
	pParent->AddChild(m_pStampCtrl);

	REAL32 width = pParent->getWidth();
	REAL32 height = pParent->getHeight();

	REAL32 offset_x = static_cast<REAL32>( (width - (STAMP_WIDTH * count) ) / (count + 1) );

	REAL32 pos_x = offset_x + ( (offset_x + STAMP_WIDTH) * idx); 
	REAL32 pos_y = (height - STAMP_HEIGHT) * 0.5f;
	
	m_pStampCtrl->setPos(pos_x, pos_y);
}

I3_CLASS_INSTANCE(UIPopupAttendanceCheck);

extern "C" {
	int UIPopupAttendanceCheck_CheckStamp( LuaState * L)
	{
		UIPopupAttendanceCheck* p = static_cast<UIPopupAttendanceCheck*>(GameUI::GetPopup(UPW_ATTENDANCE_CHECK));
		if( p != nullptr)
			p->CheckStamp();
		return 0;
	}
}
LuaFuncReg UIPopupAttendanceCheck_Glue[] = {
	{"CheckStamp",		UIPopupAttendanceCheck_CheckStamp},
	{nullptr,	nullptr}	
};

UIPopupAttendanceCheck::UIPopupAttendanceCheck() : m_bRes(false)
{
	i3mem::FillZero(m_pImgCtrl, sizeof(m_pImgCtrl));
	m_pStampCtrl = nullptr;

	m_nItemCount = 0;
	m_bPlayStampAnim = false;
	m_bIsCheckComplete = false;

	m_rAnimEndTime = 0.0f;
	m_rAccumTime = 0.0f;
	
	i3Vector::Zero(&m_vTargetPos);
	i3Vector::Zero(&m_vTargetSize);

	_ClearStampSlotInfo();
}

UIPopupAttendanceCheck::~UIPopupAttendanceCheck()
{
	_ClearStampSlotInfo(); 

	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE(m_pImgCtrl[i]);
	}

	I3_SAFE_RELEASE(m_pStampCtrl);
}


/*virtual*/ void UIPopupAttendanceCheck::OnCreate ( i3GameNode * pParent)
{
	UIPopupBase::OnCreate(pParent);

	i3::string path = "";
	i3UIScene* scene = nullptr;

	path = "Scene/Popup/PBRe_PopUp_DayCheckEvent.i3UIs";
	scene = AddScene( path.c_str(), UIPopupAttendanceCheck_Glue);

	m_bRes = (scene != nullptr);
}

/*virtual*/ void UIPopupAttendanceCheck::OnLoadAllScenes()
{
	i3UIScene * pScene = GetScene(0);

	m_pImgCtrl[0] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_Item1");
	m_pImgCtrl[1] = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet_Item2");

	m_pStampCtrl = reinterpret_cast<i3UIButtonImageSet*>(pScene->FindChildByName("i3UIButtonImageSet_Stamp"));
	m_pStampCtrl->EnableCtrl(false);

	// Init Stamp Slot Data
	i3UIFrameWnd * pStampFrame = static_cast<i3UIFrameWnd*>(pScene->FindChildByName("i3UIFrameWnd_Stamp"));
	_InitStampSlotInfo(pStampFrame);
}

/*virtual*/ void UIPopupAttendanceCheck::OnUnloadAllScenes()
{
	_ClearStampSlotInfo();

	for(INT32 i = 0 ; i < 2 ; i++)
	{
		I3_SAFE_RELEASE(m_pImgCtrl[i]);
	}

	m_pStampCtrl = nullptr;
}

/*virtual*/ bool UIPopupAttendanceCheck::OnEntranceStart(void * pArg1, void * pArg2)
{
	I3TRACE("UIPopupAttendanceComplete::OnEntranceStart\n");
	bool resPopup = UIPopupBase::OnEntranceStart(pArg1,pArg2);
	I3ASSERT(resPopup);
	
	m_nItemCount = 0;
	m_bPlayStampAnim = false;
	m_bIsCheckComplete = false;

	m_rAnimEndTime = 0.0f;
	m_rAccumTime = 0.0f;
		
	return true;
}

/*virtual*/ void UIPopupAttendanceCheck::OnEntranceEnd(void)
{
	UIPopupBase::OnEntranceEnd();

	_SetEventData();
}

/*virtual*/ void UIPopupAttendanceCheck::OnExitEnd(void)
{
	UIPopupBase::OnExitEnd();

	AttendanceContext::i()->SetAttendanceState( ATTENDANCE_STATE_COMPLETE);

	UIPhaseLobby * pLobby = static_cast<UIPhaseLobby*>(GetMainFrame()->GetPhase(UIPHASE_LOBBY));
	pLobby->ShowAttendancePopup();
}

/*virtual*/ void UIPopupAttendanceCheck::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate(rDeltaSeconds);

	if(m_bPlayStampAnim)
	{
		_PlayStampAnim(rDeltaSeconds);
	}

}

/*virtual*/ bool UIPopupAttendanceCheck::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		if( i3::same_of<i3UIManager*>(pObj))
		{
			I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;

			if( pNotify->m_nEvent == I3UI_EVT_CLICKED )
			{
				// Stamp Slot Click 처리..
				{
					i3UIButtonImageSet * pCtrl = i3::same_cast<i3UIButtonImageSet*>(pNotify->m_pCtrl);
					for( size_t idx = 0 ; idx < m_vecStampSlotList.size(); ++idx)
					{
						if( m_vecStampSlotList[idx]->IsSameCtrl(pCtrl) )
						{
							UpdateItemView(idx);
							return true;
						}
					}
				}
			}
		}
	}

	return UIPopupBase::OnEvent( event, pObj, param2, code);
}

void UIPopupAttendanceCheck::CheckStamp()
{
	// 이미 출석체크가 완료됬다면, 출석체크 번튼을 닫기 버튼으로 교체,
	// 출석체크 전이라면 서버로 출석 정보를 보낸다.
	if(m_bIsCheckComplete)
		ClosePopup();
	else
	{
		const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();
		GameEventSender::i()->SetEvent(EVENT_ATTENDANCE_CHECK, &pInfo->m_ui32Version, &pInfo->m_ui8AttendanceType);
	}
}

void UIPopupAttendanceCheck::ClosePopup()
{
	GetMainFrame()->ClosePopup(UPW_ATTENDANCE_CHECK);
}

void UIPopupAttendanceCheck::UpdateItemView(INT32 nItemIdx)
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();

	I3ASSERT(pInfo);

	// 기본적으로는 오늘 보상만 표시하지만
	// 오늘 보상 받을 아이템이 없을 경우 다음번 보상을 표시한다.	
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

	LuaState * L = _CallLuaFunction("SetItemView");

	if( L != nullptr)
	{
		i3Lua::PushInteger(L , m_nItemCount);
		i3Lua::PushStringUTF16To8(L, wstrItemName[0].c_str(), wstrItemName[0].size());
		i3Lua::PushStringUTF16To8(L, wstrItemName[1].c_str(), wstrItemName[1].size());
		_EndLuaFunction(L , 3);
	}
}


void UIPopupAttendanceCheck::SetBtnEnable(bool bFlag)
{
	LuaState * L = _CallLuaFunction("SetBtnEnable");

	if(L != nullptr)
	{
		i3Lua::PushBoolean(L , bFlag);
		_EndLuaFunction(L , 1);
	}
}	

void UIPopupAttendanceCheck::_InitStampSlotInfo(i3UIFrameWnd * pFrame)
{
	_ClearStampSlotInfo();

	INT32 count = AttendanceContext::i()->GetAttendanceInfo()->m_ui8AttendanceDay;

	for( INT32 idx = 0 ; idx < count ; ++idx )
	{
		UIAttendanceStampSlot * pStampSlot = new UIAttendanceStampSlot();
		pStampSlot->SetStampData(pFrame, idx, count);
		m_vecStampSlotList.push_back( pStampSlot);
	}
}

void UIPopupAttendanceCheck::_ClearStampSlotInfo(void)
{
	i3::cu::for_each_delete_clear(m_vecStampSlotList);
}

void UIPopupAttendanceCheck::_SetEventData(void)
{
	const ATTENDANCE_INFO * pInfo = AttendanceContext::i()->GetAttendanceInfo();

	I3ASSERT(pInfo);

	i3::wstring wstrPeriod;
	_SetPeriod(wstrPeriod, pInfo->m_ui32StartDate, pInfo->m_ui32EndDate);

	i3::wstring wstrMission;
	_SetMission(wstrMission, pInfo->m_ui8AttendanceDay);

	LuaState * L = _CallLuaFunction("SetEventData");

	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, pInfo->m_strEventName);
		i3Lua::PushStringUTF16To8(L, wstrPeriod);
		i3Lua::PushStringUTF16To8(L, wstrMission);
		_EndLuaFunction(L , 3);
	}

	UpdateItemView(AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay);

	_SetStampShape();
}

void UIPopupAttendanceCheck::_SetItemInfo(i3::rc_wstring& out, UIImgSetCtrl *pCtrl, INT32 nGoodsID)
{
	INT32 nItemID = CShopContext::i()->ConvertGoodsID(nGoodsID);

	usf::GetShopItemName(out, nItemID);
	GetItemImageMgr()->SelectImage(pCtrl, nItemID);
}

void UIPopupAttendanceCheck::_SetStampShape()
{
	INT32 nDay = AttendanceContext::i()->GetAttendanceInfo()->m_ui8AttendanceDay;
	INT32 nCheckDay = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay;

	if(nDay == nCheckDay)
	{
		// 반복 이벤트로 인한 데이터 초기화
		// 현재 출석을 완료 하였고, 아이템까지 전부 받았다면, 
		//스템프 데이터를 다시 0번째 날부터 시작한다.
		INT32 nItemIdx = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceItem;

		if(nCheckDay == nItemIdx)
		{
			nCheckDay = 0;
		}
	}

	I3ASSERT( static_cast<size_t>(nCheckDay) <= m_vecStampSlotList.size() );

	for( INT32 idx = 0 ; idx < nCheckDay ; ++idx)
	{
		m_vecStampSlotList[idx]->SetShapeIdx(2);
	}
}

void UIPopupAttendanceCheck::_SetStampAnim()
{
	// 버튼 비활성화
	SetBtnEnable(false);

	_SetStampControl();

	m_bIsCheckComplete = true;
	m_bPlayStampAnim = true;

	_SetButtonText();
}

void UIPopupAttendanceCheck::_SetStampControl(void)
{
	// Target Stamp Pos & Size
	{
		INT32 nDay = AttendanceContext::i()->GetAttendanceInfo()->m_ui8AttendanceDay;
		INT32 nCheckDay = AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay - 1;

		if(nCheckDay < 0 || nCheckDay >= nDay)
			nCheckDay = 0;

		m_vecStampSlotList[nCheckDay]->GetCtrlPos(&m_vTargetPos);
		m_vecStampSlotList[nCheckDay]->GetCtrlSize(&m_vTargetSize);
	}

	VEC2D * vStampSize = m_pStampCtrl->getSize();

	// 컨트롤의 중심을 찾아, StampCtrl의 위치를 잡아주기 위한 함수
	// 이미지 사이즈별로 보정값이 들어간다.
	// StampSize > CtrlSize == 음수, StampSize < CtrlSize == 양수
	VEC2D vCenterOffset;
	vCenterOffset.x = (m_vTargetSize.x - vStampSize->x) * 0.5f;
	vCenterOffset.y = (m_vTargetSize.y - vStampSize->y) * 0.5f;

	m_pStampCtrl->setPos(m_vTargetPos.x + vCenterOffset.x, m_vTargetPos.y + vCenterOffset.y);
	m_pStampCtrl->EnableCtrl(true);
	m_pStampCtrl->SetShapeIdx(1);

	m_rAnimEndTime = 1.0f;
}

void UIPopupAttendanceCheck::_SetButtonText(void)
{
	LuaState * L = _CallLuaFunction("SetButtonText");
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L , m_bIsCheckComplete);
		_EndLuaFunction(L , 1);
	}
}

void UIPopupAttendanceCheck::_PlayStampAnim(REAL32 rDeltaSeconds)
{
	VEC2D * vStampSize = m_pStampCtrl->getSize();

	m_rAccumTime += rDeltaSeconds;

	REAL32 rFactor = uiMath::inverse_interpolate(m_rAccumTime, 0.0f , m_rAnimEndTime);
	rFactor = 1.0f - rFactor;

	vStampSize->x *= rFactor;
	vStampSize->y *= rFactor;

	VEC2D vCenterOffset;
	vCenterOffset.x = (m_vTargetSize.x - vStampSize->x) * 0.5f;
	vCenterOffset.y = (m_vTargetSize.y - vStampSize->y) * 0.5f;

	m_pStampCtrl->setPos(m_vTargetPos.x + vCenterOffset.x, m_vTargetPos.y + vCenterOffset.y);
	m_pStampCtrl->setSize(vStampSize);

	if(vStampSize->x <= m_vTargetSize.x && vStampSize->y <= m_vTargetSize.y)
	{
		_StopStampAnim();
	}
}

void UIPopupAttendanceCheck::_StopStampAnim()
{
	m_bPlayStampAnim = false;	
	m_pStampCtrl->EnableCtrl(false);

	m_rAnimEndTime = 0.0f;
	m_rAccumTime = 0.0f;

	_SetStampShape();
	_OpenComplatePopup();
}

void UIPopupAttendanceCheck::_OpenComplatePopup()
{
	INT32 ui32UserCheckDay	= static_cast<INT32>(AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceDay);
	INT32 ui32UserCheckItem	= static_cast<INT32>(AttendanceContext::i()->GetAttendanceUser()->m_ui8AttendanceItem);
	
	
	// 최종 보상, 혹은 매일 보상의 경우에는 보상 팝업을 띄우고.
	// 아닐 경우에는 메세지박스로 출석 확인메세지를 출력해준다.
	if(ui32UserCheckItem < ui32UserCheckDay)
	{
		GetMainFrame()->OpenPopup(UPW_ATTENDANCE_COMPLETE);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_CHECK_SUCCESS"), 
			nullptr, MAKE_CALLBACK(cbSetBtnEnable), this);
	}
}

void UIPopupAttendanceCheck::_SetPeriod(i3::wstring& out, UINT32 ui32StartDate, UINT32 ui32EndDate)
{
	INT32 nStartMonth, nStartDay, nEndMonth, nEndDay;
	i3::stack_wstring wstrStartMonth;
	i3::stack_wstring wstrStartDay;
	i3::stack_wstring wstrEndMonth;
	i3::stack_wstring wstrEndDay;

	// 시작 날짜
	{
		nStartMonth = ui32StartDate / 10000;
		nStartMonth = nStartMonth % 10000;
		nStartDay = nStartMonth % 100;
		nStartMonth = nStartMonth / 100;

		i3::sprintf(wstrStartMonth, L"%d", nStartMonth);
		i3::sprintf(wstrStartDay, L"%d", nStartDay);
	}

	// 종료 날짜
	{
		nEndMonth = ui32EndDate / 10000;
		nEndMonth = nEndMonth % 10000;
		nEndDay = nEndMonth % 100;
		nEndMonth = nEndMonth / 100;

		i3::sprintf(wstrEndMonth, L"%d", nEndMonth);
		i3::sprintf(wstrEndDay, L"%d", nEndDay);
	}


	i3::sprintf(out , GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_PERIOD_SCREEN"),
		wstrStartMonth, 
		wstrStartDay,
		wstrEndMonth, 
		wstrEndDay);

	//sprintf_s(&szOut, MAX_STRING_COUNT, "%1 %2 ~ %3 %4", szStartMonth, szStartDay, szEndMonth, szEndDay);
}

void UIPopupAttendanceCheck::_SetMission(i3::wstring& out, UINT8 ui8CheckDay)
{
	//if(ui8Type == ATTENDANCE_TYPE_CONSECUTIVELY)
	//{
	//	sprintf_s(&szOut, MAX_STRING_COUNT, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_MISSION1"), ui8CheckDay);
	//}
	//else
	//{
	//	char szDaily[64] = {0};
	//	sprintf_s(szDaily, sizeof(szDaily), GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_MISSION1"), ui8CheckDay);

	//	sprintf_s(&szOut, MAX_STRING_COUNT, "%s + %s", GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_MISSION2"), szDaily);
	//}
	out = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_CHECK_EVENT");
}

void UIPopupAttendanceCheck::_SetErrorMsgBox(const i3::rc_wstring& wstrError)
{
	m_bIsCheckComplete = true;
	_SetButtonText();
	SetBtnEnable(false);

	GameUI::MsgBox( MSG_TYPE_ERROR, wstrError, nullptr, MAKE_CALLBACK(cbSetBtnEnable), this);
}

void UIPopupAttendanceCheck::cbSetBtnEnable(void * pThis, INT32 nParam)
{
	reinterpret_cast<UIPopupAttendanceCheck*>(pThis)->SetBtnEnable(true);
}

void UIPopupAttendanceCheck::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupBase::ProcessGameEvent( evt, arg, UserData);

	if( evt == EVENT_ATTENDANCE_CHECK)
	{
		ReceiveGameEvent_Attendance_Check( arg, UserData);
	}
}

void UIPopupAttendanceCheck::ReceiveGameEvent_Attendance_Check( INT32 arg, const i3::user_data&)
{
	switch( arg)
	{
	case EVENT_ERROR_ATTENDANCE_SUCCESS:
		_SetStampAnim();
		break;
	case EVENT_ERROR_ATTENDANCE_PARAMETER_USER:
		_SetErrorMsgBox(GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_PARAMETER_USER"));
		break;
	case EVENT_ERROR_ATTENDANCE_NOT_ENOUGH:
		_SetErrorMsgBox(GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_NOT_ENOUGH"));
		break;
	case EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED:
		_SetErrorMsgBox(GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_ALREADY_CHECK"));
		break;
	case EVENT_ERROR_ATTENDANCE_WRONG_VERSION:
		_SetErrorMsgBox(GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_WRONG_VERSION"));
		break;
	case EVENT_ERROR_ATTENDANCE_UNKWON:
	default:
		_SetErrorMsgBox(GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_UNKNOWN"));
		break;
	}
}
