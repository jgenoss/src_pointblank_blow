#include "pch.h"
#include "UIPopupCreateRoom.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UserGuideMgr.h"

#include "MyRoomInfoContext.h"

#include "UIPhaseReadyRoom.h"
#include "ClientStageInfoContext.h"

I3_CLASS_INSTANCE(UIPopupCreateRoom)

//===================================================================================
extern "C" {

	//최종 버튼
	int UIPopupCreateRoom_ClickOK(LuaState * L)
	{
		UIPopupCreateRoom * p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		if (p != nullptr)
			p->ClickOk();
		return 0;
	}

	int UIPopupCreateRoom_ClickCancel(LuaState * L)
	{
		UIPopupCreateRoom * p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		if (p != nullptr)
			p->ClickCancel();
		return 0;
	}

	//rule
	int UIPopupCreateRoom_SelectModeType(LuaState * L)
	{
		INT32 nMode = i3Lua::GetIntegerArg(L, 1);
		bool bSpecial = i3Lua::GetBooleanArg(L, 2);

		UIPopupCreateRoom * p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_Rule*  r_base = static_cast<CreateRoom_Rule*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_RULE));

		if (r_base != nullptr)
			r_base->SelectRule(nMode, bSpecial ? true : false);
		
		p->UpdateforRule();

		return 0;
	}

	//stage
	int UIPopupCreateRoom_MapListScroll(LuaState* L)
	{
		UIPopupCreateRoom * p		= static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_Stage*  s_base	= static_cast<CreateRoom_Stage*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
		if (s_base != nullptr)
			s_base->MapListScroll();
		return 0;
	}

	int UIPopupCreateRoom_MapListWheel(LuaState* L)
	{
		INT32 zDelta = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom * p		= static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_Stage*  s_base	= static_cast<CreateRoom_Stage*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
		if (s_base != nullptr)
			s_base->MapListWheel(zDelta);
		return 0;
	}

	int UIPopupCreateRoom_MapListDBClick(LuaState* L)
	{
		INT32 nSlot = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom * p		= static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_Stage*  s_base	= static_cast<CreateRoom_Stage*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
		if (s_base != nullptr)
		{
			s_base->MapListDBClick(nSlot);
			p->UpdateforStage();
			p->ClickOk();
		}
		return 0;
	}

	int UIPopupCreateRoom_ClickMapSlot(LuaState * L)
	{
		INT32 nSlot = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom * p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_Stage*  s_base = static_cast<CreateRoom_Stage*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
		if (s_base != nullptr)
		{
			s_base->ClickMapSlot(nSlot);
			if(!MyRoomInfoContext::i()->IsRandomMap()) p->UpdateforStage();
		}
		return 0;
	}

	int UIPopupCreateRoom_OnMouseOver(LuaState * L)
	{
		INT32 nSlot = i3Lua::GetIntegerArg(L, 1);
		bool bMouseOver = i3Lua::GetBooleanArg(L, 2);
		UIPopupCreateRoom * p		= static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_Stage*  s_base	= static_cast<CreateRoom_Stage*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
		if (s_base != nullptr)
			s_base->OnMouseOverSlot(nSlot, bMouseOver ? true : false);
		return 0;
	}

	int UIPopupCreateRoom_ClickRandomMap(LuaState * L)
	{
		UIPopupCreateRoom * p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		if (!p)
			return 0;

		CreateRoom_Stage*  s_base = static_cast<CreateRoom_Stage*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
		if (s_base)
			s_base->ClickRandomMap();
		return 0;
	}

	//roominfo_basic
	int UIPopupCreateRoom_ClickPwCheckBox(LuaState * L)
	{
		UIPopupCreateRoom * p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		if (!p)
			return 0;

		CreateRoom_RoomInfo*  ri_base = static_cast<CreateRoom_RoomInfo*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_ROOMINFO));
		if (ri_base)
			ri_base->ResetPasswordEditbox();
		return 0;
	}

	//main_cd
	int UIPopupCreateRoom_ChangeObjectCondition(LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom*  p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_MainCD*  mc_base = static_cast<CreateRoom_MainCD*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_MAIN_CD));
		if (mc_base != nullptr)
			mc_base->ChangeWinConditon(cur);
		return 0;
	}

	int UIPopupCreateRoom_ChangeTimeLimit(LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom*  p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_MainCD*  mc_base = static_cast<CreateRoom_MainCD*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_MAIN_CD));
		if (mc_base != nullptr)
			mc_base->ChangeTimeLimit(cur);
		return 0;
	}

	int UIPopupCreateRoom_ChangeAICount(LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom*  p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_MainCD*  mc_base = static_cast<CreateRoom_MainCD*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_MAIN_CD));
		if (mc_base != nullptr)
			mc_base->ChangeAiCnt(cur);
		return 0;
	}

	int UIPopupCreateRoom_ChangeAILevel(LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg(L, 1);
		UIPopupCreateRoom*  p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_MainCD*  mc_base = static_cast<CreateRoom_MainCD*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_MAIN_CD));
		if (mc_base != nullptr)
			mc_base->ChangeAiLevel(cur);
		return 0;
	}

	//sub_cd
	int UIPopupCreateRoom_ClickPushedBtn(LuaState * L)
	{
		INT32 nSlot = i3Lua::GetIntegerArg(L, 1);	// 버튼 타입 ( PushButtonInfo::BTN_TYPE )
		INT32 _btn = i3Lua::GetIntegerArg(L, 2);	// 눌려진 버튼 인덱스

		UIPopupCreateRoom*  p = static_cast<UIPopupCreateRoom*>(GameUI::GetPopup(UPW_CREATEROOM_EX));
		CreateRoom_SubCD*	sc_base = static_cast<CreateRoom_SubCD*>(p->GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_SUB_CD));
		if (sc_base != nullptr)
			sc_base->SetPushedButton(nSlot, _btn);
		return 0;
	}
}

LuaFuncReg UIPopupCreateRoom_Glue[] = {
	
	{ "pbClickCreate",			UIPopupCreateRoom_ClickOK },
	{ "pbClickCreateCancel",	UIPopupCreateRoom_ClickCancel },
	{ "ChangeObjectCondition",	UIPopupCreateRoom_ChangeObjectCondition },
	{ "ChangeTimeLimit",		UIPopupCreateRoom_ChangeTimeLimit },
	{ "ChangeAICount",			UIPopupCreateRoom_ChangeAICount },
	{ "ChangeAILevel",			UIPopupCreateRoom_ChangeAILevel },
	{ "MapListScroll",			UIPopupCreateRoom_MapListScroll },
	{ "MapListWheel",			UIPopupCreateRoom_MapListWheel },
	{ "MapListDBClick",			UIPopupCreateRoom_MapListDBClick },
	{ "ClickRandomMap",			UIPopupCreateRoom_ClickRandomMap },
	{ "ClickMapSlot",			UIPopupCreateRoom_ClickMapSlot },
	{ "SelectModeType",			UIPopupCreateRoom_SelectModeType },
	{ "OnMouseOver",			UIPopupCreateRoom_OnMouseOver },
	{ "ClickPushedBtn",			UIPopupCreateRoom_ClickPushedBtn },
	{ "ClickPasswordCheckBox",	UIPopupCreateRoom_ClickPwCheckBox },

	{ nullptr,						nullptr }
};
//===================================================================================



UIPopupCreateRoom::UIPopupCreateRoom()
{
	for (int idx = 0; idx < CR_BASE_MAX; idx++)
		m_pCreateRoomBase[idx] = nullptr;
}

UIPopupCreateRoom::~UIPopupCreateRoom()
{
	static_cast<CreateRoom_Rule*>(m_pCreateRoomBase[CR_BASE_RULE])->~CreateRoom_Rule();
	static_cast<CreateRoom_Stage*>(m_pCreateRoomBase[CR_BASE_STAGE])->~CreateRoom_Stage();
	static_cast<CreateRoom_RoomInfo*>(m_pCreateRoomBase[CR_BASE_ROOMINFO])->~CreateRoom_RoomInfo();
	static_cast<CreateRoom_MainCD*>(m_pCreateRoomBase[CR_BASE_MAIN_CD])->~CreateRoom_MainCD();
	static_cast<CreateRoom_SubCD*>(m_pCreateRoomBase[CR_BASE_SUB_CD])->~CreateRoom_SubCD();
	static_cast<CreateRoom_ObserverCD*>(m_pCreateRoomBase[CR_BASE_OBSERVER_CD])->~CreateRoom_ObserverCD();
	static_cast<CreateRoom_WeaponCD*>(m_pCreateRoomBase[CR_BASE_WEAPON_CD])->~CreateRoom_WeaponCD();

	for (int idx = 0; idx < CR_BASE_MAX; idx++)
		I3_SAFE_DELETE(m_pCreateRoomBase[idx]);
}

//===================================================================================
void	UIPopupCreateRoom::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);
	
	// 랜덤맵 부스트 이벤트 UI 세팅.
	i3UIControl* pImgRandMapEvent = static_cast<i3UIControl*>(pScene->FindChildByName("i3UIImgRMEvent"));

	if (nullptr != pImgRandMapEvent)
	{
		if (true != ClientStageInfoContext::i()->IsRandomMapEventOpened())
		{
			pImgRandMapEvent->EnableCtrl(false);
		}
		else
		{
			const i3::rc_wstring& wstrRMEvent = ClientStageInfoContext::i()->GetRandomMapEventString();
			pImgRandMapEvent->SetToolTipMsg(wstrRMEvent.c_str());
		}
	}	
	
}
//===================================================================================

//===================================================================================
void	UIPopupCreateRoom::OnCreate(i3GameNode * pParent)
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Main/PBRe_Lobby_CreatRoom.i3UIs", UIPopupCreateRoom_Glue);

	m_pCreateRoomBase[CR_BASE_RULE]			= new CreateRoom_Rule(GetScene(0));
	m_pCreateRoomBase[CR_BASE_STAGE]		= new CreateRoom_Stage(GetScene(0));
	m_pCreateRoomBase[CR_BASE_ROOMINFO]		= new CreateRoom_RoomInfo(GetScene(0));
	m_pCreateRoomBase[CR_BASE_MAIN_CD]		= new CreateRoom_MainCD(GetScene(0));
	m_pCreateRoomBase[CR_BASE_SUB_CD]		= new CreateRoom_SubCD(GetScene(0));
	m_pCreateRoomBase[CR_BASE_OBSERVER_CD]	= new CreateRoom_ObserverCD(GetScene(0));
	m_pCreateRoomBase[CR_BASE_WEAPON_CD]	= new CreateRoom_WeaponCD(GetScene(0));
}

bool	UIPopupCreateRoom::OnEntranceStart(void * pArg1, void * pArv2)
{
	m_bCreateRoom = *static_cast<bool*>(pArg1) ? false : true;

	bool bRes = UIPopupBase::OnEntranceStart(pArg1, pArv2);

	GetSlide()->ForceEnd();
	return bRes;
}

void	UIPopupCreateRoom::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	_InitializeCreateRoomInfo();

	// Key : User Guide System
	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	// 이미 생성된 방에서의 옵션에서 맵 선택이 초기화되는 버그 수정
	static_cast<CreateRoom_Stage*>(GetCreateRoomBase(CR_BASE_STAGE))->FocuseStageWheel();
}

bool	UIPopupCreateRoom::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	if (i3UI::getGlobalIme()->isFocused())
	{
		i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
		i3InputMouse* pMouse = pInputMng->GetMouse();

		if (I3I_MOUSE_LBUTTON != pMouse->GetButtonState() &&
			(pKeyboard->GetStrokeState(I3I_KEY_ESC) || pKeyboard->GetStrokeState(I3I_KEY_ENTER)))
		{
			i3UI::SetFocusControl(nullptr);
		}

		i3UIEditBox* p_pw_box = (i3UIEditBox*)m_pScene->FindChildByName("i3UIEditBox_Password");
		bool usePassword = false;
		if ( p_pw_box->getEditState() == EDIT_STATE_CHANGE_INPUT_CHAR ||
			 p_pw_box->getTextLength() > 0)
			usePassword = true;

		LuaState* L = _CallLuaFunction("SetCheckPassword");
		if (L != nullptr)
		{
			i3Lua::PushInteger(L, usePassword);
			_EndLuaFunction(L, 1);
		}

		return true;
	}

	return UIPopupBase::OnKeyInput(pInputMng);
}

bool	UIPopupCreateRoom::OnKey_Enter()
{
	ClickOk();
	return true;
}

bool	UIPopupCreateRoom::OnKey_Escape()
{
	ClickCancel();
	return true;
}

bool	UIPopupCreateRoom::OnPlayClickSound(i3UIControl * pCtrl)
{
	if (i3::same_of<i3UIButtonComposed3*>(pCtrl))
	{
		if (i3::generic_is_iequal(pCtrl->GetName(), "i3UIButtonComposed3_LCR"))
		{
			// 확인 버튼
			return PlaySound(UISND_ITEM_CLICK_OK);
		}
		else if (i3::generic_is_iequal(pCtrl->GetName(), "i3UIButtonComposed30_LCR"))
		{
			// 취소 버튼
			return PlaySound(UISND_ITEM_CLICK_CANCEL);
		}
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}
//===================================================================================

//===================================================================================
void	UIPopupCreateRoom::_InitializeCreateRoomInfo(void)
{
	for (int idx = 0; idx < CR_BASE_MAX; idx++)
	{
		m_pCreateRoomBase[idx]->SetScene(GetScene(0));
		m_pCreateRoomBase[idx]->InitData(m_bCreateRoom);
	}
	
	UpdateforRule();
}
//===================================================================================

//===================================================================================
void	UIPopupCreateRoom::UpdateforRule()
{
	CreateRoom_Rule*	r_base = static_cast<CreateRoom_Rule*>(GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_RULE));
	for (int idx = CR_BASE_STAGE ; idx < CR_BASE_MAX; idx++)
		GetCreateRoomBase((CreateRoomBase)idx)->UpdateforRule(r_base->GetSelectedRule());

	UpdateforStage();
}

void	UIPopupCreateRoom::UpdateforStage()
{
	CreateRoom_Stage*	s_base = static_cast<CreateRoom_Stage*>(GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
	for (int idx = CR_BASE_STAGE + 1; idx < CR_BASE_MAX; idx++)
		GetCreateRoomBase((CreateRoomBase)idx)->UpdateforStage(s_base->GetSelectedStage());
}

CreateRoom_Base*	UIPopupCreateRoom::GetCreateRoomBase(CreateRoomBase base_enum)
{
	return m_pCreateRoomBase[base_enum];
}

void	UIPopupCreateRoom::ClickOk()
{
	ROOM_INFO_ALL info;

	for (int idx = 0; idx < CR_BASE_MAX; idx++)
	{
		if (!m_pCreateRoomBase[idx]->ConfimRoomInfo(info)) 
			return;
	}

	SetInputDisable(true);

	if (g_pFramework->GetClanContext()->InBattleTeam())
		info._Basic._IsClanMatch = true;
	else
		info._Basic._IsClanMatch = false;

	MyRoomInfoContext::i()->setMyRoomInfo(&info._Basic);
	MyRoomInfoContext::i()->setMyRoomInfoAdd(&info._Add);

	if (!m_bCreateRoom)
		GameEventSender::i()->SetEvent(EVENT_CREATE_ROOM);
	else	
		GameEventSender::i()->SetEvent(EVENT_CHANGE_ROOM_ALLINFO_N, &info);
	
	i3UI::setDisableUIInput(false);
	GetMainFrame()->TogglePopup(UPW_CREATEROOM_EX, false);
}

void	UIPopupCreateRoom::ClickCancel()
{
	CreateRoom_Stage*	s_base		= static_cast<CreateRoom_Stage*>(GetCreateRoomBase(UIPopupCreateRoom::CR_BASE_STAGE));
	UIPhaseReadyRoom*	ready_room	= (UIPhaseReadyRoom*)g_pFramework->GetUIMainframe()->GetPhase(UIPHASE_READY_ROOM);

	if( !m_bCreateRoom )	s_base->CancelRandomMap();
	if (ready_room)			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CHANGE_READYROOM_INFO);

	GetMainFrame()->ClosePopup(UPW_CREATEROOM_EX);
}
//===================================================================================