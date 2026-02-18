#include "pch.h"
#include "UI_V15/UIPhaseReadyRoom_V15.h"

#include "UI/UIMainFrame.h"
#include "UI/UITopMenu_V15.h"
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

#include "../CommunityContext.h"
#include "../AwayChecker.h"
#include "../TextSet.h"

#include "GameContextUtil.h"

#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "InviteContext.h"
#include "ClanGameContext.h"
#include "ChannelContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "QuickJoinContext.h"
#include "ClanSeasonContext.h"
#include "GameGUI.h"

#include "StageBattle/AIModeContext.h"

#include "../StageBattle/NetworkContext.h"

#include "../RoomMasterKickManager.h"
#include "UI/UIShopFunction.h"

#include "ClientStageInfoUtil.h"

#define SLOT_UPDATETIME	1.f
#define	READY_BUTTON_ACTIVE_DELAY	1.0f	

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UIPhaseReadyRoom_ClickSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V15 * pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickSlot( idx);
		return 0;
	}

	int UIPhaseReadyRoom_ClickRSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V15 * pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickRSlot( idx);
		return 0;
	}

	int UIPhaseReadyRoom_CloseSlot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIPhaseReadyRoom_V15 * pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ClickCloseButton( idx);
		return 0;
	}

	int UIPhaseReadyRoom_ClickButton( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);

		UIPhaseReadyRoom_V15 * pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
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

	int UIPhaseReadyRoom_ChangeGMObserver( LuaState * L)
	{
		bool bChecked = i3Lua::GetBooleanArg(L , 1);

		UIPhaseReadyRoom_V15 * pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ChangeGMObserver(bChecked);
		return 0;
	}

	int UIPhaseReadyRoom_ClickRoomSetComboBox( LuaState *L )
	{
		INT32 mode = i3Lua::GetIntegerArg(L, 1);
		INT32 idx = i3Lua::GetIntegerArg(L, 2);

		UIPhaseReadyRoom_V15 * pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if( pReadyRoom != nullptr)
			pReadyRoom->ChangeGameMode(mode, idx);
		return 0;		
	}

	int UIPhaseReadyRoom_ApplyPassword( LuaState *L)
	{
		UIPhaseReadyRoom_V15 *pReadyRoom = static_cast<UIPhaseReadyRoom_V15*>(GameUI::GetPhase(UIPHASE_READY_ROOM));
		if(pReadyRoom != nullptr)
			pReadyRoom->Apply_Password();
		return 0;
	}
}

LuaFuncReg	PhaseReadyRoom_Glue[] =
{
	{ "ClickSlot",			UIPhaseReadyRoom_ClickSlot			},
	{ "ClickRSlot",			UIPhaseReadyRoom_ClickRSlot			},
	{ "CloseSlot",			UIPhaseReadyRoom_CloseSlot			},
	{ "ClickButton",		UIPhaseReadyRoom_ClickButton		},
	{ "ChangeGMObserver",	UIPhaseReadyRoom_ChangeGMObserver	},
	{ "ClickRoomSetCB",		UIPhaseReadyRoom_ClickRoomSetComboBox },
	{ "ApplyPassword",		UIPhaseReadyRoom_ApplyPassword		},
	{ nullptr,		nullptr}
};


I3_CLASS_INSTANCE( UIPhaseReadyRoom_V15);//, UIPhaseBase);

UIPhaseReadyRoom_V15::UIPhaseReadyRoom_V15()
	: m_bReadyPushed(false)
	, m_rReadyDelay(0.f)
{
}

UIPhaseReadyRoom_V15::~UIPhaseReadyRoom_V15()
{
}

/*virtual*/ void UIPhaseReadyRoom_V15::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	m_room_detail[ROOM_T_NORMAL] = new room_normal(this);
	MEMDUMP_NEW( m_room_detail[ROOM_T_NORMAL], sizeof( room_normal));

	m_room_detail[ROOM_T_DOMI] = new room_domi(this);
	MEMDUMP_NEW( m_room_detail[ROOM_T_DOMI], sizeof( room_domi));

	m_curr_room = m_room_detail[ROOM_T_NORMAL];

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_ReadyRoom_TeamSelect.i3UIs",  PhaseReadyRoom_Glue, true, false);
	//AddScene( "Scene/Domination/PBRe_Domi_TeamSelect.i3UIs",  PhaseReady_DomiRoom_Glue, true, false);

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

void UIPhaseReadyRoom_V15::OnLoadAllScenes()
{

}

/*virtual*/ void UIPhaseReadyRoom_V15::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	i3UIFrameWnd* wnd;
	i3::string scene_name = "PointBlankRe_ReadyRoom_TeamSelect";
	if( scene_name.find( pScene->GetName()) != -1)
	{
		wnd = (i3UIFrameWnd*)pScene->FindChildByName( "FrameWnd_ReadySlot");
		m_room_detail[ROOM_T_NORMAL]->set_frame_wnd( wnd);

		m_pGMObserver = (i3UICheckBox*) pScene->FindChildByName( "CkGMObserver");
		I3ASSERT( m_pGMObserver);

		m_pMainWnd[0] = wnd;
		m_pMainWnd[1] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd_BtnGameSet");
		m_pMainWnd[2] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd_GMObserver");
		m_pMainWnd[3] = nullptr;
		m_pMainWnd[4] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd_BtnRoomSet");
		m_pMainWnd[5] = (i3UIFrameWnd*)pScene->FindChildByName("Fmw_bg");

		m_pBtnReady = (i3UIButton*) pScene->FindChildByName( "Btn_Ready");
		I3ASSERT( m_pBtnReady != nullptr);
		m_pBtnStart = (i3UIButton*) pScene->FindChildByName( "Btn_Start");
		I3ASSERT( m_pBtnStart != nullptr);

		m_pTitleWnd = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd_RoomTitle");
		I3ASSERT( m_pTitleWnd != nullptr);

		m_cbBox[COMBO_RULE] = (i3UIComboBox*)pScene->FindChildByName("i3UIComboBox_Mode");
		I3ASSERT( m_cbBox[COMBO_RULE] != nullptr);
		m_cbBox[COMBO_STAGE] = (i3UIComboBox*)pScene->FindChildByName("i3UIComboBox_Stage");
		I3ASSERT( m_cbBox[COMBO_STAGE] != nullptr);
		m_cbBox[COMBO_ROUND] = (i3UIComboBox*)pScene->FindChildByName("i3UIComboBox_Round");
		I3ASSERT( m_cbBox[COMBO_ROUND] != nullptr);
		m_cbBox[COMBO_TIME] = (i3UIComboBox*)pScene->FindChildByName("i3UIComboBox_Time");
		I3ASSERT( m_cbBox[COMBO_TIME] != nullptr);
		m_cbBox[COMBO_TEAM_CHANGE] = (i3UIComboBox*)pScene->FindChildByName("i3UIComboBox_AutoTeamChange");
		I3ASSERT(m_cbBox[COMBO_TEAM_CHANGE] != nullptr);

		m_btnRoomSet = (i3UIButton*)pScene->FindChildByName("Btn_RoomSet");
		m_hostFmw = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd_BtnRoomSet");
		I3ASSERT( m_hostFmw != nullptr);
	}

	scene_name = "PBRe_Domi_TeamSelect";
	if( scene_name.find( pScene->GetName()) != -1)
	{
		//wnd = (i3UIFrameWnd*)pScene->FindChildByName( "domi_ReadySlot");
		//m_room_detail[ROOM_T_DOMI]->set_frame_wnd( wnd);
		//m_pMainWnd[3] = wnd;
	}

	m_bReadyEnable = true;

}

void UIPhaseReadyRoom_V15::OnEntranceEnd()
{
	UIPhaseReadyRoom::OnEntranceEnd();

	UITopMenu_V15 * pTop = GetMainFrame()->GetTopMenu_V15();
	if( pTop != nullptr)
		pTop->cb_tmenu_gauge( true);
}

void UIPhaseReadyRoom_V15::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseReadyRoom::OnUpdate(rDeltaSeconds);

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

void UIPhaseReadyRoom_V15::SetFrmWndPos()
{
	enum { Right_Dec = 8 };

	for(size_t idx = 0; idx < UserListMaxFrm; ++idx)
	{
		if (m_pMainWnd[idx] == nullptr)
			continue;

		REAL32 offset = 0.0f;

		// GM Observer Frame은 right 좌표가 맨 끝이 아니기 때문에 별도 값 추가- sanghun.han(2014. 12. 19)
		if (idx == 2)
			offset = 193.0f;	
	
		VEC3D* pos = m_pMainWnd[idx]->getPos();
		VEC2D* size = m_pMainWnd[idx]->getSize();

		m_pMainWnd[idx]->setPos( (REAL32) (GameUI::AutoPosition::get_max_right() - size->x - Right_Dec - offset), pos->y );
	}
}

void UIPhaseReadyRoom_V15::ApplyLua_cbbox(COMBO_TYPE _cType, const i3::wliteral_range& wstrCondition, INT32 _cel)
{
	LuaState* L = _CallLuaFunction("InitGameMode");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, _cType);
		i3Lua::PushStringUTF16To8(L, wstrCondition);
		i3Lua::PushInteger(L, _cel);
		_EndLuaFunction(L, 3);
	}
}

void UIPhaseReadyRoom_V15::ApplyLua_auto_team_change_enable(UINT32 stageID)
{
	bool is_auto_change_enable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);
	bool is_auto_change_stage = StageID::IsAtkDefChangeStage(stageID);
	bool is_dino_mode		  = (	MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || 
									MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT)	);

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

void UIPhaseReadyRoom_V15::SetLuaText()
{
	SetLuaPasswordOpt();
}

void UIPhaseReadyRoom_V15::ChangeGameMode(INT32 _mode, INT32 _celidx)
{
	if(!m_bIsHost)
		return;

	bool bChange = false;		// 바뀐것이 있는지, 없는지. 없으면 이벤트를 보내지 않음
	ROOM_INFO_ALL Info = GetCurRoonInfo();
	CSI_STAGE_LIST mapList;
	UINT8 gameTime;UINT8 gameType;

	if(m_nCurComboIdx[_mode] != _celidx)
		bChange = true;
	else
		return;		// 바뀐것이 없으면 실행할 필요가 없는 함수이므로 리턴

	m_nCurComboIdx[_mode] = _celidx;

	switch(_mode)
	{
	case COMBO_RULE:
	{
		m_CurStageRule		 = *CStageInfoUtil::GetRuleData(m_cb_Detail[_mode].GetData(_celidx));
		CSI_STAGE_LIST temp;
		if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)	temp = CStageInfoUtil::GetStageListofEvent();
		else												temp = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

		m_nCurStageID		= Info._Basic._StageID = temp.GetStageData(0)->m_ui32StageID;

		m_SubType = CStageInfoUtil::getMissionRoundType(*(temp.GetStageData(0)));
		Info._Add._SubType = m_SubType;
	}
	break;
	case COMBO_STAGE:	
		if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)
			mapList = CStageInfoUtil::GetStageListofEvent();
		else
			mapList = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

		for(INT32 i=0; i < (INT32)mapList.GetStageListSize(); i++)
		{
			if( i == _celidx )
				Info._Basic._StageID = mapList.GetStageData(i)->m_ui32StageID;			
			m_nCurStageID = mapList.GetStageData(i)->m_ui32StageID;
		}
		break;
	case COMBO_ROUND:
		// 챌린지모드는 난이도 조정
		if (MyRoomInfoContext::i()->getStage()->IsAiMode())
		{
			AIModeContext::i()->StartLevel = m_cb_Detail[_mode].GetData(_celidx);
			break;
		}
		gameTime = HINIBBLE(m_SubType);
		gameType = LONIBBLE(m_cb_Detail[_mode].GetData_subType(_celidx));
		m_SubType = MAKEBYTE(gameTime, gameType);
		Info._Add._SubType = m_SubType;
		break;
	case COMBO_TIME:
		{
			gameTime = LONIBBLE(m_cb_Detail[_mode].GetData_subType(_celidx));
			gameType = LONIBBLE(m_SubType);
			m_SubType = MAKEBYTE(gameTime, gameType);
			Info._Add._SubType = m_SubType;
		}
		break;
	case COMBO_TEAM_CHANGE :
		{
			// set auto team change flag
			bool is_auto_change_enable = UserInfoContext::i()->IsContentsEndabled(CONTENTS_TEAM_CHANGE);
			bool is_auto_change_stage  = StageID::IsAtkDefChangeStage(Info._Basic._StageID);

			if (is_auto_change_enable && is_auto_change_stage && (_celidx == 1) )
				Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 1);
			else
				Info._Basic._InfoFlag = SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(Info._Basic._InfoFlag, 0);
		}
		break;
	default:
		break;
	}

	if (bChange)
	{
		
		// Lua 최신화 - Stage Type이 바뀔때만 호출합니다
		if( _mode == COMBO_RULE )
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
		}

		MyRoomInfoContext::i()->setMyRoomInfo(&Info._Basic);
		MyRoomInfoContext::i()->setMyRoomInfoAdd(&Info._Add);

		GameEventSender::i()->SetEvent(EVENT_CHANGE_ROOM_ALLINFO_N, &Info);
		I3TRACE("SettingRoom EVENT_CHANGE_ROOM_ALLINFO_N\n");

	} // 	if (bChange)
}

void UIPhaseReadyRoom_V15::UpdateCBBoxInfo(COMBO_TYPE BoxType)
{
	// 터지는 문제가 아직 해소되지 않아 일단 다음의 코드를 임시 추가합니다..(2015.07.17.수빈)
	if (m_cbBox[BoxType] == nullptr)
		return;

	if(m_bLeaveTeamState) return;

	i3::stack_wstring	wstrCondition;	i3::rc_wstring strCurCondition;
	INT32 idxMin=0, idxMax=0;
	INT32 i= 0;
	UINT32 gameTime; UINT32 gameType;

	ROOM_INFO_ALL info	= GetCurRoonInfo();
	m_nCurStageID		= (STAGE_ID)info._Basic._StageID;

	if (m_nCurStageID == 0) return;

	if(CStageInfoUtil::GetStageData(m_nCurStageID)->IsEventforEventType(STAGE_EVENT_SEASON))
		m_CurStageRule = *CStageInfoUtil::GetRuleData((INT32)0);
	else
		m_CurStageRule = *CStageInfoUtil::GetRuleData(m_nCurStageID);
	m_SubType			= info._Add._SubType;

	// Stage ID 관련. 모드에 따라 출력할 수 있는 스테이지의 범위가 다르기 때문
	CSI_STAGE_LIST mapList;

	switch (BoxType)
	{
	case COMBO_RULE:									// 게임 룰
	{
		m_cb_Detail[BoxType].ClearData();

		if (!CStageInfoUtil::IsDiableChaneMode(*CStageInfoUtil::GetStageData(m_nCurStageID)))
		{
			bool custom_rule	= CStageInfoUtil::GetStageData(m_nCurStageID)->IsSpecialWar();
			INT32 temp_rule_idx = m_CurStageRule.m_i32RuleIdx;

			const size_t numRule = CStageInfoUtil::GetRuleList(custom_rule).size();
			i3::vector<CSI_RULE*> temp_list = CStageInfoUtil::GetRuleList(custom_rule);
			for (idxMin = 0; (size_t)idxMin < numRule; idxMin++)
			{
				CSI_RULE* test = temp_list.at(idxMin);

				if (
						(temp_rule_idx == test->m_i32RuleIdx) ||
						(custom_rule && (m_CurStageRule.GetWeaponRestriction() == test->GetWeaponRestriction()))
				   )
				{
					if (test->m_i32RuleIdx > 0 && CStageInfoUtil::GetStageList(test->m_i32RuleIdx).GetStageListSize() > 0 )
					{
						i3::rc_wstring tempStr = test->getRuleName();
						wstrCondition += tempStr;
						wstrCondition += L';';
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
						i3::rc_wstring tempStr = CStageInfoUtil::GetRuleData(temp_rule_idx)->getRuleName();
						i3::generic_string_cat(wstrCondition, tempStr);
						i3::generic_string_cat(wstrCondition, L";");
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
	}
	break;
	case COMBO_STAGE:								// 스테이지 설정
	{
		if (m_CurStageRule.m_i32RuleIdx == RULE_ID_EVENT)
			mapList = CStageInfoUtil::GetStageListofEvent();
		else
			mapList = CStageInfoUtil::GetStageList(m_CurStageRule.m_i32RuleIdx);

		bool bRandomMap = MyRoomInfoContext::i()->IsRandomMap();

		if (bRandomMap)
		{
			m_nCurComboIdx[BoxType] = 0;
			m_cb_Detail[BoxType].SetData(0, g_pFramework->GetRandomMapIdx());
			strCurCondition = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
			i3::sprintf(wstrCondition, L"%s;", GAME_RCSTRING("STR_POPUP_RANDOMMAP"));
		}
		else
		{
			for (int index = 0; (size_t)index < mapList.GetStageListSize(); index++)
			{
				i3::rc_wstring _name = CStageInfoUtil::GetMapData(mapList.GetStageData(index)->m_ui32StageID)->getDisplayName();
				i3::generic_string_cat(wstrCondition, _name);
				i3::generic_string_cat(wstrCondition, L";");

				m_cb_Detail[BoxType].SetData(index, mapList.GetStageData(index)->m_ui32StageID);

				if (mapList.GetStageData(index)->m_ui32StageID == m_nCurStageID)
				{
					m_nCurComboIdx[BoxType] = index;
					strCurCondition = CStageInfoUtil::GetMapData(mapList.GetStageData(index)->m_ui32StageID)->getDisplayName();
				}
				i++;
			}

			GameUI::GetStageMapName(false, m_nCurStageID, strCurCondition);
		}
	}
	break;
	case COMBO_ROUND:							// 승리 조건 설정
	{
		gameType = m_SubType & 0x0F;

		// 챌린지모드와 기타 모드에서 쓰이는 데이터가 다르기때문에 클리어 먼저 해줌
		m_cb_Detail[BoxType].ClearData();

		if (StageID::GetAIMode(m_nCurStageID) != AI_USE_NO )
		{
			if(StageID::GetAIMode(m_nCurStageID) == AI_USE_MADNESS )
			{
				i3::generic_string_cat( wstrCondition, TextSet::AIModeLevel(1));	
				i3::generic_string_cat( wstrCondition, L";");
				m_cb_Detail[BoxType].SetData(0, 1);
			}
			else
			{
				idxMin = 1;
				idxMax = 10;

				for (INT32 j = idxMin; j <= idxMax; ++j)
				{
					i3::generic_string_cat(wstrCondition, TextSet::AIModeLevel(j));
					i3::generic_string_cat(wstrCondition, L";");
					m_cb_Detail[BoxType].SetData(j - idxMin, j);
				}
			}

			strCurCondition = TextSet::AIModeLevel(AIModeContext::i()->StartLevel);
			m_nCurComboIdx[BoxType] = AIModeContext::i()->StartLevel -1 ;
			if (m_nCurComboIdx[BoxType] < 0) m_nCurComboIdx[BoxType] = 0;

			break;
		}

		if( CStageInfoUtil::IsModeRoundType(*MyRoomInfoContext::i()->getStage()) )
		{
			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) ||
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
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
			for(i = idxMin; i<=idxMax; i++)
			{
				i3::generic_string_cat(wstrCondition, TextSet::Round(i));
				i3::generic_string_cat(wstrCondition, L";");
				m_cb_Detail[BoxType].SetData_subType(i-idxMin, (UINT8)i);
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
		m_nCurComboIdx[BoxType] =gameType - idxMin;
	}
	break;
	case COMBO_TIME:						// 제한시간 설정
	{
		gameTime = m_SubType & 0xF0;

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

		if ( StageID::GetAIMode(m_nCurStageID) == AI_USE_CHALLENGE || 
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
		else if ( StageID::GetAIMode(m_nCurStageID) == AI_USE_MADNESS)
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

	case COMBO_TEAM_CHANGE :
		{
			m_cb_Detail[BoxType].SetData(0, 0);
			i3::generic_string_cat(wstrCondition, GAME_RCSTRING("STR_OPT_DEFAULT_OFF"));
			i3::generic_string_cat(wstrCondition, L";");

			m_cb_Detail[BoxType].SetData(1, 1);
			i3::generic_string_cat(wstrCondition, GAME_RCSTRING("STR_OPT_DEFAULT_ON"));
			i3::generic_string_cat(wstrCondition, L";");

			if ( GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(info._Basic._InfoFlag) == ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE )
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

	default:
		break;
	}

	if(m_bIsHost)
	{
		ApplyLua_cbbox(BoxType, wstrCondition, m_nCurComboIdx[BoxType]);
		m_btnRoomSet->enable_btn();
	}
	else
	{
		if(m_cbBox[BoxType])
			m_cbBox[BoxType]->getEditBox()->SetText(strCurCondition);
		m_btnRoomSet->disable_btn();
	}
}

void UIPhaseReadyRoom_V15::CheckBeHost(void)
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
	m_pTitleWnd->setInputDisable(!m_bIsHost);

}