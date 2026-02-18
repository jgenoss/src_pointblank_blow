#include "pch.h"
#include "UIPhaseReadyRoom_V10.h"

#include "UI/UIMainFrame.h"
#include "UI_V10/UITopMenu_V10.h"
#include "UI/UIPopup.h"
#include "UI/UIFloatReadyRoomInfo.h"
#include "UI/UIFloatMiniProfile.h"
#include "UI/UIFloatChat.h"
#include "UI/UIFloatUserList.h"
#include "UI/UIFloatPlayTimeAlarm.h"
#include "UI/UIFloatCharacterView.h"
#include "UI/UICharaView.h"
#include "UI/BreakIntoGame.h"
#include "UI/UIPhaseProfile.h"
#include "UI/UIUtil.h"
#include "../CommunityContext.h"
#include "../AwayChecker.h"
#include "../TextSet.h"

#include "GameContextUtil.h"

#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "InviteContext.h"
#include "RandomMapContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "QuickJoinContext.h"
#include "ClanSeasonContext.h"
#include "GameGUI.h"

#include "../StageBattle/NetworkContext.h"
#include "../StageBattle/AIModeContext.h"

#include "../RoomMasterKickManager.h"
#include "UI/UIShopFunction.h"

#include "MissionCard/MCard.h"
#include "MissionCard//MCardUtil.h"
#include "MissionCard/MCardSet.h"

#include "UI_V10/UIFloatReadyRoomUserInfo.h"

#include "ClientStageInfoUtil.h"
#include "ClientStageInfoContext.h"

#include "UI_V10\UIFloatReadyRoomChat_V10.h"

#define SLOT_UPDATETIME	1.f
#define	READY_BUTTON_ACTIVE_DELAY	1.0f

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UIPhaseReadyRoom_V10_ClickSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickSlot( idx);
		return 0;
	}

	int UIPhaseReadyRoom_V10_ClickRSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickRSlot( idx);
		return 0;
	}

	int UIPhaseReadyRoom_V10_CloseSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickCloseButton( idx);
		return 0;
	}

	int UIPhaseReadyRoom_V10_ClickButton( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);

		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if (pReadyRoom != nullptr)
		{
			pReadyRoom->ClickButton(btn);

			// 기존의도는 3초 딜레이가 맞지만, 1.0, 1.5는 각 UI쪽에서 
			// SetLockInput(false) 다시 호출하는 걸로 수정(PBRU-1033)
			if ((INT32)RBS_READY == btn &&
				true != pReadyRoom->IsReadyBtnPushed())
			{
				pReadyRoom->SetReadyBtnOn();
				pReadyRoom->InitReadyDelayTime(READY_BUTTON_ACTIVE_DELAY);
			}
		}

		return 0;
	}

	int UIPhaseReadyRoom_V10_ChangeGMObserver( LuaState * L)
	{
		bool bChecked = i3Lua::GetBooleanArg(L , 1);

		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ChangeGMObserver(bChecked);
		return 0;
	}

	int UIPhaseReadyRoom_V10_ClickRoomSetComboBox( LuaState *L )
	{
		INT32 mode = i3Lua::GetIntegerArg(L, 1);
		INT32 idx = i3Lua::GetIntegerArg(L, 2);

		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ChangeGameMode(mode, idx);
		return 0;		
	}

	int UIPhaseReadyRoom_V10_ApplyPassword( LuaState *L)
	{
		UIPhaseReadyRoom_V10 *pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if(pReadyRoom != nullptr)
			pReadyRoom->Apply_Password();
		return 0;
	}

	int UIPhaseReadyRoom_V10_ApplyTitle( LuaState *L)
	{
		UIPhaseReadyRoom_V10 *pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if(pReadyRoom != nullptr)
			pReadyRoom->Apply_Title();
		return 0;
	}

	int UIPhaseReadyRoom_V10_ClickExit( LuaState *L)
	{
		UIPhaseReadyRoom_V10 *pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if(pReadyRoom != nullptr)
			pReadyRoom->OnKey_Escape();
		return 0;
	}

	int UIPhaseReadyRoom_V10_ClickRandomMap( LuaState * L)
	{
		UIPhaseReadyRoom_V10 *pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if(pReadyRoom != nullptr)
			pReadyRoom->Apply_RandomMap();
		return 0;
	}

	int UIPhaseReadyRoom_V10_OnMouseOver( LuaState * L)
	{
		INT32 mode = i3Lua::GetIntegerArg(L, 1);
		UIPhaseReadyRoom_V10 *pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if(pReadyRoom != nullptr)
			pReadyRoom->SetModeAni(mode);
		return 0;
	}

	int UIPhaseReadyRoom_V10_ChangeMode( LuaState * L)
	{
		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
		{
			pReadyRoom->ChangeMode();
		}
		return 0;
	}

	int UIPhaseReadyRoom_V10_ClickStageComboBox(LuaState * L)
	{
		UIPhaseReadyRoom_V10 * pReadyRoom = static_cast<UIPhaseReadyRoom_V10*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if (pReadyRoom != nullptr)
		{
			bool isClickComboBox = i3Lua::GetBooleanArg(L, 1);
			pReadyRoom->FocusOnStageComboBox(isClickComboBox);
		}
		return 0;
	}
	
}

LuaFuncReg	PhaseReadyRoom_V10_Glue[] =
{
	{ "ClickSlot",			UIPhaseReadyRoom_V10_ClickSlot			},
	{ "ClickRSlot",			UIPhaseReadyRoom_V10_ClickRSlot			},
	{ "CloseSlot",			UIPhaseReadyRoom_V10_CloseSlot			},
	{ "ClickButton",		UIPhaseReadyRoom_V10_ClickButton		},
	{ "ChangeGMObserver",	UIPhaseReadyRoom_V10_ChangeGMObserver	},
	{ "ClickRoomSetCB",		UIPhaseReadyRoom_V10_ClickRoomSetComboBox },
	{ "ApplyPassword",		UIPhaseReadyRoom_V10_ApplyPassword		},
	{ "ApplyTitle",			UIPhaseReadyRoom_V10_ApplyTitle			},
	{ "ClickExit",			UIPhaseReadyRoom_V10_ClickExit			},
	{ "ClickRandomMap",		UIPhaseReadyRoom_V10_ClickRandomMap 	},
	{ "ChangeMode",			UIPhaseReadyRoom_V10_ClickRandomMap 	},
	{ "OnMouseOver",		UIPhaseReadyRoom_V10_OnMouseOver 	},
	{ "ChangeMode",			UIPhaseReadyRoom_V10_ChangeMode 	},
	{ "FoucsStageBox",			UIPhaseReadyRoom_V10_ClickStageComboBox },
	{ nullptr,		nullptr}
};

I3_CLASS_INSTANCE( UIPhaseReadyRoom_V10);//, UIPhaseBase);

UIPhaseReadyRoom_V10::UIPhaseReadyRoom_V10()
	: m_bReadyPushed(false)
	, m_rReadyDelay(0.f)
{
	i3mem::FillZero(m_nCurComboData, sizeof(m_nCurComboData));
}

UIPhaseReadyRoom_V10::~UIPhaseReadyRoom_V10()
{
}

/*virtual*/ void UIPhaseReadyRoom_V10::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	m_room_detail[ROOM_T_NORMAL] = new room_normal_V10(this);
	MEMDUMP_NEW( m_room_detail[ROOM_T_NORMAL], sizeof( room_normal_V10));

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_ReadyRoom_TeamSelect.i3UIs",  PhaseReadyRoom_V10_Glue, true, false);

	const char* soundFileName[COUNTDOWN_COUNT] =
	{
		"UI_Count_A.wav",
		"UI_Count_B.wav",
		"UI_Count_C.wav",
		"UI_Count_D.wav",
		"UI_Count_E.wav"
	};

	for( INT32 i = 0; i < COUNTDOWN_COUNT; i++)
	{
		i3GameResSound* pResSound = (i3GameResSound*)g_pFramework->QuaryResource( soundFileName[i]);
		I3ASSERT(pResSound != nullptr);
		I3_REF_CHANGE( m_pCountdownSound[i], pResSound->getSound());
	}

}

void UIPhaseReadyRoom_V10::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);
}

/*virtual*/ void UIPhaseReadyRoom_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_bReadyEnable = true;


	i3UIFrameWnd* wnd;
	wnd = (i3UIFrameWnd*)pScene->FindChildByName( "FrameWnd_ReadySlot");
	m_room_detail[ROOM_T_NORMAL]->set_frame_wnd( wnd);

	m_pGMObserver = (i3UICheckBox*) pScene->FindChildByName( "CkGMObserver");

	m_pBtnReady = (i3UIButton*) pScene->FindChildByName( "Btn_Ready");
	I3ASSERT( m_pBtnReady != nullptr);
	m_pBtnStart = (i3UIButton*) pScene->FindChildByName( "Btn_Start");
	I3ASSERT( m_pBtnStart != nullptr);
	m_btnRoomSet = (i3UIButton*)pScene->FindChildByName("Btn_RoomSet");
	I3ASSERT( m_btnRoomSet != nullptr);


	m_hostFmw = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd8");
	I3ASSERT( m_hostFmw != nullptr);
	m_pPwEditBox = (i3UIEditBox*)pScene->FindChildByName("textPassword");
	I3ASSERT( m_pPwEditBox != nullptr);

	wnd = (i3UIFrameWnd*)pScene->FindChildByName( "i3UIFrameChangeMode");
	m_ChangMode.SetControl(wnd);

	m_pStageComboBox = (i3UIComboBox*)pScene->FindChildByName("i3UIComboBox_Stage");
}

void UIPhaseReadyRoom_V10::OnEntranceEnd()
{
	UIPhaseReadyRoom::OnEntranceEnd();
	
	// 랜덤맵 개선 시스템을 사용하지 않으면 기존 랜덤맵 방식을 쓴다.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		// 서버로부터 받은 랜덤 스테이지 리스트 적용.
		RandomMapContext::i()->TakeOverStageList();
	}

	SetLuaPasswordOpt();
	
	//입장전에 이거 보내줘야합니다. FloatCharaView가 1.0에서는 없어요
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_TEAM_CHANGE );

	// 임무 보상 체크
	CheckBattleChallengeComplate();

	UITopMenu_V10 * pTop = GetMainFrame()->GetTopMenu_V10();
	if (pTop != nullptr)
		pTop->cb_tmenu_gauge(true);

	// 랜덤맵 부스트 이벤트 스트링 세팅.
	LuaState* L = _CallLuaFunction("InitToolTip");
	if (L != NULL)
	{
		const i3::rc_wstring& wstrRMEvent = ClientStageInfoContext::i()->GetRandomMapEventString();
		i3Lua::PushStringUTF16To8(L, wstrRMEvent);
		_EndLuaFunction(L, 1);
	}
}

void UIPhaseReadyRoom_V10::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseReadyRoom::OnUpdate(rDeltaSeconds);

	if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM) 
	{
		m_ChangMode.OnUpdate(rDeltaSeconds);	
	}

	if (true == m_bReadyPushed)
	{
		m_rReadyDelay -= rDeltaSeconds;
		if (m_rReadyDelay <= 0.0f)
		{
			m_bReadyPushed = false;
			GetMainFrame()->SetLockInput(false);
		}
	}
}

bool UIPhaseReadyRoom_V10::OnRevive( i3RenderContext * pCtx)
{
	if( UIPhaseBase::OnRevive( pCtx) == false)
		return false;

	return true;
}

void UIPhaseReadyRoom_V10::SetFrmWndPos()
{

}

void UIPhaseReadyRoom_V10::ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel)
{
	LuaState* L = _CallLuaFunction("InitGameMode");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, _cType);
		i3Lua::PushStringUTF16To8(L, wstrCondition);
		i3Lua::PushInteger(L, m_nCurComboIdx[_cType]);
		i3Lua::PushInteger(L, m_nCurComboData[_cType]);
		_EndLuaFunction(L, 4);
	}
}

void UIPhaseReadyRoom_V10::ApplyLua_auto_team_change_enable(UINT32 stageID)
{
	bool is_auto_change_enable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);
	bool is_auto_change_stage = StageID::IsAtkDefChangeStage(stageID);
	bool is_dino_mode = (	MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ||
							MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT));

	bool is_enable_ctrl = false;

	// 공룡모드는 콤보박스를 표시하지 않습니다.
	if (is_dino_mode == false)
	{
		// 공수교대가 가능한 스테이지는 콤보박스 표시.
		if (is_auto_change_enable && is_auto_change_stage)
			is_enable_ctrl = true;
	}

	LuaState* L = _CallLuaFunction("SetAutoTeamChangeEnable");
	if (L != nullptr)
	{
		i3Lua::PushBoolean(L, is_enable_ctrl ? true : false);
		_EndLuaFunction(L, 1);
	}
}

void UIPhaseReadyRoom_V10::SetLuaText()
{
//	if(!m_bIsHost)
		SetLuaPasswordOpt();

	// Room Title 출력 [서버이름 서버넘버 >> 방번호 : 방 제목]
	SetRoomTitle();
}

void UIPhaseReadyRoom_V10::Apply_Title()
{
	ROOM_INFO_ALL Info = GetCurRoonInfo();
	//MyRoomInfoContext::i()->getMyRoomInfo(&Info._Basic);

	LuaState* L = _CallLuaFunction("getTitle");
	const char* pszTitle = _ReturnLuaString(L, 0);
	
	i3::utf8_to_utf16(pszTitle, m_strTitle);

	if( i3::generic_is_iequal( Info._Basic._Title, m_strTitle.c_str()) == false)
	{
		INT32 Length = m_strTitle.size();
		if(Length <= 0 )
		{
			m_strTitle = L"";
			i3::safe_string_copy(Info._Basic._Title, m_strTitle, NET_ROOM_NAME_SIZE);
		}
		else
		{
			i3::safe_string_copy(Info._Basic._Title, m_strTitle, NET_ROOM_NAME_SIZE);
		}
		
		MyRoomInfoContext::i()->setMyRoomInfo(&Info._Basic);
		GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &Info );
	}
	
}

void UIPhaseReadyRoom_V10::FocusOnStageComboBox(bool enable)
{	
	UIFloatReadyRoomChat_V10 * curfloat = (UIFloatReadyRoomChat_V10*)(GetMainFrame()->GetFloating(UFW_READYROOM_CHAT));
	if (curfloat != nullptr)
		curfloat->SetInputDisable(enable);

}

void UIPhaseReadyRoom_V10::Apply_RandomMap(bool bChange/*= true*/)
{
	ROOM_INFO_ALL allinfo = GetCurRoonInfo();

	LuaState * L = _CallLuaFunction("isRandomMap");

	bool bRandom = false;
	if( nullptr != L )
	{
		bRandom = UILua::ReturnLuaBoolean(L, 0);
		allinfo._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(allinfo._Basic._InfoFlag, bRandom);
		MyRoomInfoContext::i()->setMyRoomInfo(&allinfo._Basic);
	}

	// 랜덤맵 개선 시스템을 사용하지 않으면 기존 랜덤맵 방식을 쓴다.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		RandomMapContext::i()->BackupStageList();
		RandomMapContext::i()->ClearStageList();

		if (bRandom == true)
		{
			CSI_STAGE_LIST mapList;
			mapList = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);
			for (INT32 i = 0; i < (INT32)mapList.GetStageListSize(); i++)
			{
				CSI_STAGE* Stage = mapList.GetStageData(i);

				if (m_nCurComboData[COMBO_RULE] == STAGE_MODE_DEATHMATCH ||
					m_nCurComboData[COMBO_RULE] == STAGE_MODE_ANNIHILATION)
				{
					if (CStageInfoUtil::GetSlotMode(*Stage) == STAGE_SLOT_MODE_4VS4) continue; //4 vs 4 모드 제외
				}

				RandomMapContext::i()->AddStage(Stage);
				SET_ENABLE_RANDOM_MAP(allinfo._Add._ui64OpenStage, Stage->GetStageUID(), TRUE);
			}
		}
		else
		{
			for (INT32 i = 0; i < MAX_OPEN_STAGE; ++i)
				allinfo._Add._ui64OpenStage[i] = 0;
		}
	}
	
	if(bChange)
		GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &allinfo );
}

void UIPhaseReadyRoom_V10::ChangeMode()
{
	for( size_t i = 0; i < m_cb_Detail[COMBO_RULE]._value.size(); i++)
	{
		if( (m_cb_Detail[COMBO_RULE].GetData(i)) == m_CurStageRule.m_i32RuleIdx )
		{

			ROOM_INFO_ALL Info = GetCurRoonInfo();
			INT32 _celidx = i;
			m_nCurComboIdx[COMBO_RULE] = _celidx;
			
			if ( !CStageInfoUtil::IsDiableChaneMode(m_CurStageRule) )
				ChangeGameMode(COMBO_RULE, m_nCurComboIdx[COMBO_RULE]);

		}
	}
}

void UIPhaseReadyRoom_V10::ChangeGameMode(INT32 _mode, INT32 _celidx)
{
	if(!m_bIsHost)
		return;

	bool bChange = false;		// 바뀐것이 있는지, 없는지. 없으면 이벤트를 보내지 않음
	ROOM_INFO_ALL Info = GetCurRoonInfo();
	CSI_STAGE_LIST mapList;
	UINT8 gameTime;UINT8 gameType;

	switch (_mode)
	{
	case COMBO_RULE:
		if (m_cb_Detail[_mode]._value.size() > 0)
		{
			INT32 iMax = m_cb_Detail[_mode]._value.size() - 1;
			bChange = true;
			_celidx += m_nCurComboIdx[_mode];

			if (_celidx < 0)
				_celidx = iMax;
			else if (_celidx > iMax)
				_celidx = 0;

			m_CurStageRule = *CStageInfoUtil::GetRuleData(m_cb_Detail[_mode].GetData(_celidx));
			CSI_STAGE_LIST temp;
			if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)	temp = CStageInfoUtil::GetStageListofEvent();
			else												temp = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

			m_nCurStageID = Info._Basic._StageID = temp.GetStageData(0)->m_ui32StageID;
			m_SubType = CStageInfoUtil::getMissionRoundType(*(temp.GetStageData(0)));
			Info._Add._SubType = m_SubType;

			switch (CStageInfoUtil::GetSlotMode(*temp.GetStageData(0)))
			{
			case STAGE_SLOT_MODE_4VS4:
				Info._Basic._PersonPlayerMax = 8;
				break;
			case STAGE_SLOT_MODE_5VS5:
				Info._Basic._PersonPlayerMax = 10;
				break;
			default:
				Info._Basic._PersonPlayerMax = 16;
				break;
		}
		}
		break;
	case COMBO_STAGE:	// STAGE ID 의 경우, 인덱스 만으로 본래의 값을 찾을 수 없기 때문에 리스트를 받아옴..
	{
		bChange = true;

		if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)
			mapList = CStageInfoUtil::GetStageListofEvent();
		else
			mapList = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

		for (INT32 i = 0; i < (INT32)mapList.GetStageListSize(); i++)
		{
			if (i == _celidx)
				Info._Basic._StageID = mapList.GetStageData(i)->m_ui32StageID;
			m_nCurStageID = mapList.GetStageData(i)->m_ui32StageID;
		}
	}
	break;
	case COMBO_ROUND:
	{
		INT32 idxMin, idxMax;
		if (CStageInfoUtil::IsModeRoundType(*MyRoomInfoContext::i()->getStage()))
		{
			idxMin = 0;
			idxMax = BATTLE_ROUND_TYPE_9 - BATTLE_ROUND_TYPE_3;
		}
		else
		{
			idxMin = BATTLE_KILL_TYPE_60;
			idxMax = BATTLE_KILL_TYPE_160;
		}

		bChange = true;
		_celidx += m_nCurComboIdx[_mode];

		if (_celidx < idxMin)
			_celidx = idxMax;
		else if (_celidx > idxMax)
			_celidx = idxMin;

		gameTime = HINIBBLE(m_SubType);
		gameType = LONIBBLE(m_cb_Detail[_mode].GetData_subType(_celidx));
		m_SubType = MAKEBYTE(gameTime, gameType);
		Info._Add._SubType = m_SubType;
	}
	break;
	case COMBO_TIME:
	{
		INT32 iMax = m_cb_Detail[_mode]._type.size() - 1;
		bChange = true;
		_celidx += m_nCurComboIdx[_mode];

		if (_celidx < 0)
			_celidx = iMax;
		else if (_celidx > iMax)
			_celidx = 0;

		m_nCurComboIdx[_mode] = _celidx;
		gameTime = LONIBBLE(m_cb_Detail[_mode].GetData_subType(_celidx));
		gameType = LONIBBLE(m_SubType);
		m_SubType = MAKEBYTE(gameTime, gameType);
		Info._Add._SubType = m_SubType;
	}
	break;
	case COMBO_TEAM_CHANGE:
	{
		INT32 iMax = m_cb_Detail[_mode]._value.size() - 1;
		_celidx += m_nCurComboIdx[_mode];

		if (_celidx < 0)
			_celidx = iMax;
		else if (_celidx > iMax)
			_celidx = 0;

		m_nCurComboIdx[_mode] = _celidx;

		// set auto team change flag
		bool is_auto_change_enable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);
		bool is_auto_change_stage = StageID::IsAtkDefChangeStage(Info._Basic._StageID);

		if (is_auto_change_enable && is_auto_change_stage && (_celidx == 1))
			Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 1);
		else
			Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 0);

		bChange = true;
	}
	break;
	case COMBO_AI_LEVEL:
		{
			m_nCurComboIdx[_mode] = _celidx;
			AIModeContext::i()->StartLevel = m_nCurComboIdx[_mode]+1;
			bChange = true;
		}
		break;
	case COMBO_AI_COUNT:
		{
			INT32 iMax = 8;
			bChange = true;
			_celidx += m_nCurComboIdx[_mode];

			if (_celidx < 1)
				_celidx = iMax;
			else if (_celidx > iMax)
				_celidx = 1;

			m_nCurComboIdx[_mode] = _celidx;
			AIModeContext::i()->NewEnemyCount = m_nCurComboIdx[_mode];
		}
		break;
	default:
		bChange = true;
		break;
	}

	if (bChange)
	{
	
		// Lua 최신화 - Stage Type이 바뀔때만 호출합니다
		if (_mode == COMBO_RULE)
		{
			// set auto team change flag
			{
				bool is_auto_change_enable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);
				bool is_auto_change_stage = CStageInfoUtil::GetStageData(Info._Basic._StageID)->IsAtkDefChangeStage();

				if (is_auto_change_enable && is_auto_change_stage)
					Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 1);
				else
					Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 0);
			}

			_SetAiModeConditon(m_nCurStageID);

			AdjustRandomMapData(Info);

			UpdateCBBoxInfo(COMBO_ROUND);
			UpdateCBBoxInfo(COMBO_STAGE);
			UpdateCBBoxInfo(COMBO_TIME);
			UpdateCBBoxInfo(COMBO_TEAM_CHANGE);
			UpdateCBBoxInfo(COMBO_AI_LEVEL);
			UpdateCBBoxInfo(COMBO_AI_COUNT);
		}
		else if (_mode == COMBO_STAGE && m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)
		{
			// set auto team change flag
			{
				bool is_auto_change_enable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);
				bool is_auto_change_stage = CStageInfoUtil::GetStageData(Info._Basic._StageID)->IsAtkDefChangeStage();

				if (is_auto_change_enable && is_auto_change_stage)
					Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 1);
				else
					Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 0);
			}

			_SetAiModeConditon(m_nCurStageID);

			AdjustRandomMapData(Info);

			UpdateCBBoxInfo(COMBO_ROUND);
			UpdateCBBoxInfo(COMBO_TIME);
			UpdateCBBoxInfo(COMBO_TEAM_CHANGE);
			UpdateCBBoxInfo(COMBO_AI_LEVEL);
			UpdateCBBoxInfo(COMBO_AI_COUNT);
		}

		MyRoomInfoContext::i()->setMyRoomInfo(&Info._Basic);
		MyRoomInfoContext::i()->setMyRoomInfoAdd(&Info._Add);

		GameEventSender::i()->SetEvent(EVENT_CHANGE_ROOM_ALLINFO_N, &Info);
		I3TRACE("SettingRoom EVENT_CHANGE_ROOM_ALLINFO_N\n");

	} // 	if (bChange)
}

void UIPhaseReadyRoom_V10::UpdateCBBoxInfo(COMBO_TYPE BoxType)
{
	if(m_bLeaveTeamState) return;

	i3::stack_wstring	wstrCondition;	i3::rc_wstring strCurCondition;
	INT32 idxMin = 0, idxMax = 0;
	INT32 i = 0;
	UINT32 gameTime; UINT32 gameType;

	ROOM_INFO_ALL info = GetCurRoonInfo();
	m_nCurStageID = (STAGE_ID)info._Basic._StageID;

	if (m_nCurStageID == 0) return;

	if (CStageInfoUtil::GetStageData(m_nCurStageID)->IsEventforEventType(STAGE_EVENT_SEASON))
		m_CurStageRule = *CStageInfoUtil::GetRuleData((INT32)0);
	else
		m_CurStageRule = *CStageInfoUtil::GetRuleData(m_nCurStageID);
	m_SubType = info._Add._SubType;

	// Stage ID 관련. 모드에 따라 출력할 수 있는 스테이지의 범위가 다르기 때문
	CSI_STAGE_LIST mapList;

	switch (BoxType)
	{
	case COMBO_RULE:								// 게임 모드
	{
		m_cb_Detail[BoxType].ClearData();

		if (!CStageInfoUtil::IsDiableChaneMode(*CStageInfoUtil::GetStageData(m_nCurStageID)))
		{
			bool custom_rule = CStageInfoUtil::GetStageData(m_nCurStageID)->IsSpecialWar();
			INT32 temp_rule_idx = m_CurStageRule.m_i32RuleIdx;

			const size_t numRule			= CStageInfoUtil::GetRuleList(custom_rule).size();
			i3::vector<CSI_RULE*> temp_list = CStageInfoUtil::GetRuleList(custom_rule);
			for (idxMin = 0; (size_t)idxMin < numRule; idxMin++)
			{
				CSI_RULE* test = temp_list.at(idxMin);
				if (
					(temp_rule_idx == test->m_i32RuleIdx) ||
					(custom_rule && (m_CurStageRule.GetWeaponRestriction() == test->GetWeaponRestriction()))
					)
				{
					if (test->m_i32RuleIdx > 0 && CStageInfoUtil::GetStageList(test->m_i32RuleIdx).GetStageListSize() > 0)
					{
						m_cb_Detail[BoxType].SetData(i, test->m_i32RuleIdx);
						++i;
					}
				}
			}
		}
		else
		{
			for (int rule_type = 0; rule_type < 2; rule_type++)
			{
				bool custom_rule = rule_type == 0 ? false : true;
				for (idxMin = 0; (size_t)idxMin < CStageInfoUtil::GetRuleList(custom_rule).size(); idxMin++)
				{
					CSI_RULE* temp_rule = CStageInfoUtil::GetRuleList(custom_rule).at(idxMin);
					INT32 temp_rule_idx = temp_rule->m_i32RuleIdx;

					//클랜매치 일 때는 클랜 매치 불가능한 모드는 아예 넣어 버리지 않는다.
					if (ChannelContext::i()->GetBattleChannel())
					{
						if (!temp_rule->IsClanMatchStage())
							continue;
					}

					if (!CStageInfoUtil::IsDiableChaneMode(*temp_rule))
						continue;

					CSI_STAGE_LIST temp;
					if (temp_rule_idx != RULE_ID_EVENT)	temp = CStageInfoUtil::GetStageList(temp_rule_idx);
					else								temp = CStageInfoUtil::GetStageListofEvent();

					if (temp.GetStageListSize() > 0)
					{
						m_cb_Detail[BoxType].SetData(i, temp_rule_idx);
						++i;
					}
				}
			}
		}

		for (int idx = 0; idx < i; idx++)
		{
			if (m_cb_Detail[BoxType].GetData(idx) == m_CurStageRule.m_i32RuleIdx)
			{
				m_nCurComboIdx[BoxType] = idx;
				strCurCondition = CStageInfoUtil::GetRuleData(m_CurStageRule.m_i32RuleIdx)->getRuleName();
				break;
			}
		}

		UI_MODE_TYPE mode = GameUI::GetStageModeType(CStageInfoUtil::GetStageData(info._Basic._StageID), false);
		m_nCurComboData[BoxType] = (INT32)mode;

		SetModeAni();
	}
	break;
	case COMBO_STAGE:								// 스테이지 설정
	{
		if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)
			mapList = CStageInfoUtil::GetStageListofEvent();
		else
			mapList = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

		for (int index = 0; (size_t)index < mapList.GetStageListSize(); index++)
		{
			i3::rc_wstring _name = CStageInfoUtil::GetMapData(mapList.GetStageData(index)->m_ui32StageID)->getDisplayName();
			i3::generic_string_cat(strCurCondition, _name);
			i3::generic_string_cat(strCurCondition, L";");

			m_cb_Detail[BoxType].SetData(index, mapList.GetStageData(index)->m_ui32StageID);

			if (mapList.GetStageData(index)->m_ui32StageID == m_nCurStageID)
				m_nCurComboIdx[BoxType] = index;
				
			i++;
		}
		
		bool bRandomMap = MyRoomInfoContext::i()->IsRandomMap();
		bool bRandomMapEventShow = false;

		//Stage Shape 
		if (bRandomMap)
		{
			m_nCurComboIdx[BoxType] = 0;
			m_nCurComboData[BoxType] = g_pFramework->GetRandomMapIdx();
			strCurCondition = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
			i3::sprintf(wstrCondition, L"%s;", GAME_RCSTRING("STR_POPUP_RANDOMMAP"));
			bRandomMapEventShow = ClientStageInfoContext::i()->IsRandomMapEventOpened();
		}
		else
		{
			m_nCurComboData[BoxType] = g_pFramework->GetMapShapeIdx((STAGE_ID)m_nCurStageID);
		}

		//Stage New Mark
		bool bNewEnable = false;
		if (TESTBIT(CStageInfoUtil::GetStageData(m_nCurStageID)->m_ui8Mark, GOODS_MARK_NEW))
			bNewEnable = true;
		
		//	공수 교대 & 무기 제한 여부
		INT32 mode_option = GameUI::GetModeOptionShapeIndex(info._Basic);
		
		LuaState* L = _CallLuaFunction("StageOption");
		if (L != nullptr)
		{
			i3Lua::PushBoolean(L, bNewEnable);
			i3Lua::PushBoolean(L, bRandomMapEventShow);
			i3Lua::PushInteger(L, bRandomMap ? 1 : 0);
			i3Lua::PushInteger(L, mode_option);	// 공수 교대 & 무기 제한 여부
			_EndLuaFunction(L, 4);
		}
		
		Apply_RandomMap(false);
	}
	break;
	case COMBO_ROUND:							// 승리 조건 설정
	{
		gameType = m_SubType & 0x0F;

		// 챌린지모드와 기타 모드에서 쓰이는 데이터가 다르기때문에 클리어 먼저 해줌
		m_cb_Detail[BoxType].ClearData();
		if (CStageInfoUtil::IsModeRoundType(*MyRoomInfoContext::i()->getStage()))
		{
			if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) ||
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
			{
				m_cb_Detail[BoxType].SetData_subType(0, 0);
				i3::generic_string_cat(wstrCondition, GAME_RCSTRING("STBL_IDX_MODE_TYPE1"));
				i3::generic_string_cat(wstrCondition, L";");
				strCurCondition = GAME_RCSTRING("STBL_IDX_MODE_TYPE1");
				m_nCurComboIdx[BoxType] = 0;
				break;
			}
			idxMin = BATTLE_ROUND_TYPE_3;
			idxMax = BATTLE_ROUND_TYPE_9;
			for (i = idxMin; i <= idxMax; i++)
			{
				i3::generic_string_cat(wstrCondition, TextSet::Round(i));
				i3::generic_string_cat(wstrCondition, L";");
				m_cb_Detail[BoxType].SetData_subType(i - idxMin, (UINT8)i);
			}

			strCurCondition = TextSet::Round(gameType);
		}
		else
		{
			idxMin = BATTLE_KILL_TYPE_60;
			idxMax = BATTLE_KILL_TYPE_160;
			for (i = idxMin; i <= idxMax; i++)
			{
				i3::generic_string_cat(wstrCondition, TextSet::KillCount(i));
				i3::generic_string_cat(wstrCondition, L";");
				m_cb_Detail[BoxType].SetData_subType(i - idxMin, (UINT8)i);
			}

			strCurCondition = TextSet::KillCount(gameType);
		}
		m_nCurComboIdx[BoxType] = gameType - idxMin;
	}
	break;
	case COMBO_TIME:						// 제한시간 설정
	{
		gameTime = m_SubType & 0xF0;
		
		m_cb_Detail[BoxType].ClearData();
		// 모드에 따른 최대/최소 시간 설정
		switch (StageID::GetStageMode(m_nCurStageID))
		{
		case STAGE_MODE_ANNIHILATION:
		case STAGE_MODE_BOMB:
		case STAGE_MODE_DESTROY:
		case STAGE_MODE_CONVOY:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_7);
			break;
		case STAGE_MODE_DEFENCE:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			break;
		case STAGE_MODE_ESCAPE:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
			break;
		case STAGE_MODE_CROSSCOUNT:
			idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
			idxMax = HINIBBLE(BATTLE_TIME_TYPE_15);
			break;
		case STAGE_MODE_DEATHMATCH:
		default:
			idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
			idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
			break;
		}

		if (StageID::GetAIMode(m_nCurStageID) == AI_USE_CHALLENGE ||
			StageID::GetAIMode(m_nCurStageID) == AI_USE_DIEHARD)
		{
			idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
			idxMax = HINIBBLE(BATTLE_TIME_TYPE_10);

			for (i = idxMin; i <= idxMax; i++)
			{
				i3::generic_string_cat(wstrCondition, TextSet::GameTime(MAKEBYTE(i, 0)));
				i3::generic_string_cat(wstrCondition, L";");
				m_cb_Detail[BoxType].SetData_subType(i - idxMin, (UINT8)i);
			}
			m_nCurComboIdx[BoxType] = HINIBBLE(gameTime) - idxMin;

		}
		else if (StageID::GetAIMode(m_nCurStageID) == AI_USE_MADNESS)
		{
			idxMin = HINIBBLE(BATTLE_TIME_TYPE_10);
			idxMax = HINIBBLE(BATTLE_TIME_TYPE_10);

			i3::generic_string_cat(wstrCondition, TextSet::GameTime(MAKEBYTE(idxMin, 0)));
			i3::generic_string_cat(wstrCondition, L";");
			m_cb_Detail[BoxType].SetData_subType(0, (UINT8)idxMin);
			m_nCurComboIdx[BoxType] = 0;

		}
		else
		{
			for (i = idxMin; i <= idxMax; i++)
			{
				i3::generic_string_cat(wstrCondition, TextSet::GameTime(MAKEBYTE(i, 0)));
				i3::generic_string_cat(wstrCondition, L";");
				m_cb_Detail[BoxType].SetData_subType(i - idxMin, (UINT8)i);
			}
			m_nCurComboIdx[BoxType] = HINIBBLE(gameTime) - idxMin;
			if (m_nCurComboIdx[BoxType] < 0) m_nCurComboIdx[BoxType] = 0;
		}
		strCurCondition = TextSet::GameTime(MAKEBYTE(HINIBBLE(gameTime), 0));
	}
	break;
	case COMBO_TEAM_CHANGE:
	{
		m_cb_Detail[BoxType].SetData(0, 0);
		i3::generic_string_cat(wstrCondition, GAME_RCSTRING("STR_OPT_DEFAULT_OFF"));
		i3::generic_string_cat(wstrCondition, L";");

		m_cb_Detail[BoxType].SetData(1, 1);
		i3::generic_string_cat(wstrCondition, GAME_RCSTRING("STR_OPT_DEFAULT_ON"));
		i3::generic_string_cat(wstrCondition, L";");

		if (GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(info._Basic._InfoFlag) == ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE)
		{
			strCurCondition = GAME_RCSTRING("STR_OPT_DEFAULT_ON");
			m_nCurComboIdx[BoxType] = 1;
		}
		else
		{
			strCurCondition = GAME_RCSTRING("STR_OPT_DEFAULT_OFF");
			m_nCurComboIdx[BoxType] = 0;
		}

		ApplyLua_auto_team_change_enable(info._Basic._StageID);
	}
	break;
	case COMBO_AI_LEVEL:
	{
		//레벨.. 로 쓰는것 같음.
		m_cb_Detail[BoxType].ClearData();
		if( m_nCurComboData[COMBO_RULE] == UMT_AI )
		{
			if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS))
			{
				i3::generic_string_cat(strCurCondition, TextSet::AIModeLevel(1));
				i3::generic_string_cat(strCurCondition, L";");
				m_cb_Detail[BoxType].SetData(0, 1);
			}
			else
			{
				idxMin = 1;
				idxMax = 10;

					for(INT32 j=idxMin; j <= idxMax ; ++j)
					{
						i3::generic_string_cat(strCurCondition, TextSet::AIModeLevel(j));
						i3::generic_string_cat(strCurCondition, L";");
						m_cb_Detail[BoxType].SetData(j-idxMin, j);
					}
				}

				m_nCurComboIdx[BoxType] = AIModeContext::i()->StartLevel-1;
			}
	}
	break;
	case COMBO_AI_COUNT:
	{
		//적군수
		m_cb_Detail[BoxType].ClearData();
		if (m_nCurComboData[COMBO_RULE] == UMT_AI)
		{
			i3::sprintf(strCurCondition, L"%d", AIModeContext::i()->NewEnemyCount);
			m_nCurComboIdx[BoxType] = AIModeContext::i()->NewEnemyCount;
		}
	}
	break;
	default:
		break;
	}

	if(m_bIsHost)
	{
		ApplyLua_cbbox(BoxType, strCurCondition, m_nCurComboIdx[BoxType]);
		m_btnRoomSet->enable_btn();
	}
	else
	{
		ApplyLua_cbbox(BoxType, strCurCondition, m_nCurComboIdx[BoxType]);
	}

}

void UIPhaseReadyRoom_V10::CheckBeHost(void)
{

	if (!_IsHost()) m_bIsHost = false;

	if (m_bIsHost)	return;										// 이미 방장이면 처리하지 않는다

	if (_IsHost())												// 방장이 되었다면 팝업 출력
	{
		if (!MyRoomInfoContext::i()->IsTeamChangeFlag())
		{
			i3::rc_wstring wstrBuf;
			i3::sprintf(wstrBuf, GAME_RCSTRING("STBL_IDX_EP_ROOM_BE_HOST"),
				BattleSlotContext::i()->getNickForSlot(BattleSlotContext::i()->getMySlotIdx()));

			m_pBeHostMessageBox = GameUI::MsgBox(MSG_TYPE_GAME_OK, wstrBuf, nullptr, MAKE_CALLBACK(UIPhaseReadyRoom::CbBeHost), this);

			m_bIsHost = true;
		}
	}

	for (UINT8 i = COMBO_RULE; i < COMBO_MAX; i++)
		UpdateCBBoxInfo((COMBO_TYPE)i);

	if (m_OldHostIndex != BattleSlotContext::i()->getMainSlotIdx())
	{
		//[domi room]
		//_SetButtonState();
		m_curr_room->set_button_state();						// 방장이 변경되면 버튼을 다시 설정

		if (_IsHost())
		{
			for (UINT8 i = COMBO_RULE; i < COMBO_MAX; i++)
				UpdateCBBoxInfo((COMBO_TYPE)i);
		}

		m_OldHostIndex = BattleSlotContext::i()->getMainSlotIdx();
	}

	m_hostFmw->setInputDisable(!m_bIsHost);
	bool bRandomMap = MyRoomInfoContext::i()->IsRandomMap();
	LuaState* L = _CallLuaFunction("SetHostmode");
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L, !_IsHost());
		i3Lua::PushBoolean(L, bRandomMap);
		_EndLuaFunction(L, 2);
	}
	m_pPwEditBox->setInputDisable(!_IsHost());
}

bool UIPhaseReadyRoom_V10::isTrainingMode( void)
{ 
	return (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE)); 
}

void UIPhaseReadyRoom_V10::SetModeAni(INT32 state) 
{
	if(m_nCurComboData[COMBO_RULE] == UMT_DEATHMATCH || m_nCurComboData[COMBO_RULE] == UMT_ANNIHILATION )
		m_ChangMode.SetStateAni(state);
	else
		m_ChangMode.SetStateAni(-1);
}

void UIPhaseReadyRoom_V10::CheckBattleChallengeComplate(void)
{

	const MCardSet* pCardSet = MCardUtil::GetSelectedMCardSet();

	if (pCardSet == nullptr) // 선택된 임무카드셋이 없으면 그냥 끝..
		return;

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	//임무카드 최종 보상 미리보기
	if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LCTRL ) &&
		g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT) )
	{
		const MCard* pCard = pCardSet->GetSelectedMCard();

		if (pCard)
		{
			const MCardInfo* pInfo_Test = pCard->GetMCardInfo();
			bool bIsCardFinal = true;
			if ( pInfo_Test )
			{
				GetMainFrame()->OpenPopup( UPW_REWARDITEM, const_cast<MCardInfo*>(pInfo_Test), &bIsCardFinal);
				return;
			}
		}
	}
#endif

	const MCardInfo* pInfo_Reward = nullptr;
	bool bIsAnyQuestCompleted = false;	bool bIsCardRewardExist = false;	bool bIsCardFinal = false;

	MCardUtil::PopRewardMark(pInfo_Reward, bIsAnyQuestCompleted, bIsCardRewardExist, bIsCardFinal);

	if (bIsAnyQuestCompleted == false)
		return;

	if (bIsCardRewardExist == false)
	{
		// 임무카드의 모든 퀘스트를 완료한 것이 아닌 몇개의 임무라도 완료했다면 팝업을 띄웁니다.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_EXIST_COMPLETE_MISSION"));//완료된 임무가 있습니다.
	}
	else
	{
		// 이 위치에서 분기를 나누어 커펌을 받고 나서 켜야 함.
		GetMainFrame()->OpenPopup( UPW_REWARDITEM , const_cast<MCardInfo*>(pInfo_Reward), &bIsCardFinal);
	}
}

void UIPhaseReadyRoom_V10::ClickSlot( INT32 idx)
{
	if( idx < 0 || idx >= SLOT_MAX_COUNT)
		return;

	if( m_nSelectedSlotIdx == idx)
		return;

	m_nSelectedSlotIdx = idx;

	const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo( idx);

	UIFloatReadyRoomUserInfo * pUserInfo = static_cast<UIFloatReadyRoomUserInfo*>( GetMainFrame()->GetFloating(UFW_READYROOM_USERINFO));
	pUserInfo->SetSelectedIdx(idx);
	
	INT32 nTeam;
	
	if (0 == idx % 2)
		nTeam = TEAM_RED;
	else
		nTeam = TEAM_BLUE;

	if( SLOT_STATE_EMPTY == pSlotInfo->_State)
	{
		if( !g_pFramework->GetClanContext()->InBattleTeam())				//	클랜전이 아닐 경우만 가능하도록
		{
			//[domi room]
			//_ChangeTeam(nTeam);
			m_curr_room->change_team( nTeam);
		}

	}
	else if( SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		// 상세정보 업데이트
		_GetUserInfo(idx);

		if( idx != BattleSlotContext::i()->getMySlotIdx())
		{	// 다른 유저의 슬롯이면 이벤트 요청
			GameEventSender::i()->SetEvent( EVENT_GET_USER_EQUIPMENT_INFO, &idx);
		}
		else
		{	// 자신의 캐릭터 업데이트
			CharaViewMgr::i()->getView()->UpdateCharacter(idx, true, WEAPON_SLOT_UNKNOWN, static_cast<TEAM_TYPE>(nTeam) );
		}
	}
}

void UIPhaseReadyRoom_V10::_GetUserInfo(INT32 idxGameSlot)
{
	if( BattleSlotContext::i()->getMySlotIdx() != idxGameSlot)
	{
		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_N, &idxGameSlot);
	}
	else
	{
		UIFloatReadyRoomUserInfo * floating = (UIFloatReadyRoomUserInfo*)GetMainFrame()->GetFloating(UFW_READYROOM_USERINFO);
		if(floating != nullptr)
		{
			floating->SetMyInfo();
		}
	}
}

void UIPhaseReadyRoom_V10::ClickRSlot(INT32 idx)
{
	UINT32 i;
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

	m_nSelectedSlotIdx = idx;

	const SLOT_INFO* pSlotInfo = BattleSlotContext::i()->getSlotInfo( idx);
	if(pSlotInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_NOTICE,"pSlotInfo Value is nullptr");
		return;
	}
	if (SLOT_STATE_EMPTY != pSlotInfo->_State &&
		SLOT_STATE_CLOSE != pSlotInfo->_State)
	{
		ClickSlot( idx);

		GetMainFrame()->ResetUserActionPopup();

		// 자신의 경우 출력할 컨텍스트메뉴가 없다.
		if( BattleSlotContext::i()->getMySlotIdx() != idx)
		{
			bool bFriend = false;
			FRIEND_STATE friendState = FRIEND_STATE_OFFLINE;
			for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
			{
				const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
				if(nullptr == node)	continue;
				if( i3::generic_is_iequal( node->_wstr_nick, BattleSlotContext::i()->getNickForSlot( idx)) == 0)
				{
					// 이미 친구이다.
					bFriend = true;
					friendState = (FRIEND_STATE) node->_state;
				}
			}

			// 방장 일 경우
			if( _IsHost())
			{
				// 방장 넘기기, 강제퇴장
				GetMainFrame()->AddUserActionCommand( UCM_TRANSFER_HOST);
				GetMainFrame()->AddUserActionCommand( UCM_KICK);
			}
	
			// 귓속말
			GetMainFrame()->AddUserActionCommand( UCM_WHISPER);

			// 친구가 아닐경우
			if(bFriend == false)
			{
				GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);
			}

			if( LocaleValue::Enable("EnableBlock") ) // 차단하기 버튼 활성화 여부
			{
				GetMainFrame()->AddUserActionCommand( UCM_ADD_BLOCK);		//	차단하기
			}

			if( CanClanInvite() )
			{
				//	자기가 클랜 초대 기능이 있을 경우만 나오게
				if( BattleSlotContext::i()->getSlotInfo( idx )->_clanstate == CLAN_MEMBER_LEVEL_UNKNOWN &&
					g_pFramework->GetClanContext()->IsPossibleClanInvite() )
				{
					GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);
				}
			}

			// GM 옵션창
			if ( UserInfoContext::i()->GetMyRank() == RANK_GAMEMASTER)
			{
				GetMainFrame()->AddUserActionCommand( UCM_GMOPTION);		// 클랜초대(CurrentTab)
			}
		}

		GetMainFrame()->OpenUserActionPopup( this, idx);
		PlaySound( UISND_ITEM_SELECTING);
	}
}