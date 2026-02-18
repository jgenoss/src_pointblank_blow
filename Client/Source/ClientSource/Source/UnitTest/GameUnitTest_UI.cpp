#include "pch.h"
#if ENABLE_UNIT_TEST

#include "GlobalVariables.h"
#include "AllStage.h"
#include "GameCharaInputContext.h"

#include "UI/UIMainFrame.h"
#include "UI/UITopMenu.h"
#include "UI/UIPhaseSelectServer.h"
#include "UI/UIPhaseCreateChara.h"

#include "UI/UIPopupNickNameSetting.h"

#include "UI/UIUtil.h"
#include "UtilBattle.h"
#include "GameLoginContext.h"

#include "GameUnitTest_UI.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( CGameUnitTest_UI);

#define UNIT_TEST_UI_RANDOM_EVENT_SEND_TIME	1.0f
#define UINT_TEST_UI_STEP_CHANGE_TIME		3.0f

#define SUBPHASE_LOOP_COUNT					3

CGameUnitTest_UI::CGameUnitTest_UI(void)
{
	m_rChangeStepTime = 0.0f;
	m_rEventSendTime = 0.0f;

	m_bRandomEvent = false;

	m_bEnterSubPhase = false;

	m_eCurrentStep = UT_UI_STEP_START;
	m_eOldStep = UT_UI_STEP_START;

	m_i32SubPhaseLoopCnt = 0;

	m_bBattleProcess = false;
	
	_RegisterPopupList();
}

CGameUnitTest_UI::~CGameUnitTest_UI(void)
{
	for( size_t cnt = UIPHASE_SERVER_CHANNEL_LIST; cnt < UIPHASE_MAX; ++cnt)
		m_PhasePopupList[cnt].Clear();

	for( size_t cnt = USP_SHOP; cnt < USP_MAX; ++cnt)
		m_SubPhasePopupList[cnt].Clear();
}

void CGameUnitTest_UI::OnUpdate(REAL32 rDeltaSeconds)
{
	m_rEventSendTime += rDeltaSeconds;
	m_rChangeStepTime += rDeltaSeconds;

	g_pConfig->m_bNetworking = true;

	// LoginPhase인 경우 UIMainFrame으로 이동
	if( !i3::same_of<UIMainFrame*>(g_pFramework->GetUIMainframe()) )
	{
		if(m_rEventSendTime > UNIT_TEST_UI_RANDOM_EVENT_SEND_TIME)
		{
			GAME_SERVER_INFO ServerInfo;
			ServerInfo._IsActive = 1;
			ServerInfo._Arg0 = 0;
			/*ServerInfo.m_ui8ChannelCount = 10;
			ServerInfo.m_ui16EachChannelMaxCount = 200;*/

			g_pServerInfo->CreateServerInfo(1, 1);
			g_pServerInfo->UpdateServerInfo(1, &ServerInfo);

			_SetStage();
			JumpToReady(true);
		}
	}

	_OnUpdate_PhaseTest(rDeltaSeconds);
}

void CGameUnitTest_UI::JumpToReady(bool bStartPhase)
{
	if(bStartPhase)
	{
		g_pFramework->GetUIMainframe()->SetStartPhase(UIPHASE_START);

		if( (i3Math::Rand() % 3) > 0)
		{
			char InitNick[NET_NICK_NAME_SIZE] = {0};

			CCharaInfoContext::i()->ClearAll();
			UserInfoContext::i()->SetMyNickName(InitNick);
			memset(UserInfoContext::i()->SetMyUICharaInfo(), 0, sizeof(UI_CHARA_INFO) );
		}
	}

	CGameFramework::JumpToReady(); 
	m_rEventSendTime = 0.0f;
	m_rChangeStepTime = 0.0f;
}

void CGameUnitTest_UI::_ChangeUIPhase(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pUIFrame != nullptr);

	INT32 i32PhaseState = i3Math::Rand() % 6;

	switch(pUIFrame->GetCurrentPhaseType())
	{
	case UIPHASE_SERVER_CHANNEL_LIST:
		{
			g_pServerInfo->SetGameServer(0, S_CHANNEL_TYPE_ALL);
			g_pServerInfo->SetCurChannelNumber(0);

			if( false == CCharaInfoContext::i()->HasBasicChara() )
			{
				pUIFrame->MovePhase(UIPHASE_CREATE_CHARA);
			}
			else
			{
				if( (i32PhaseState % 2) == 0)
					pUIFrame->MovePhase(UIPHASE_LOBBY);
				else
					pUIFrame->MovePhase(UIPHASE_CLANLOBBY);
			}
		}
		break;
	case UIPHASE_CREATE_CHARA:
		{
			if( pUIFrame->IsOpenedPopup(UPW_CREATENICKNAME) )
			{
				UIPopupNickNameSetting * pPopup = static_cast<UIPopupNickNameSetting*>(GameUI::GetPopup(UPW_CREATENICKNAME));
				pPopup->ChangeTextMsg(0);
				pPopup->OnOKButton();
				pPopup->OnExitStart();
			}

			if( (i32PhaseState % 2) == 0)
				pUIFrame->MovePhase(UIPHASE_LOBBY);
			else
				pUIFrame->MovePhase(UIPHASE_CLANLOBBY);
		}
		break;
	case UIPHASE_LOBBY:
		{
			switch(i32PhaseState)
			{
			case 0:	 pUIFrame->MovePhase(UIPHASE_SERVER_CHANNEL_LIST);	break;
			case 1:	 pUIFrame->MovePhase(UIPHASE_CLANLOBBY);			break;
			default: pUIFrame->MovePhase(UIPHASE_READY_ROOM);			break;
			}
		}
		break;
	case UIPHASE_CLANLOBBY:
		{
			switch(i32PhaseState)
			{
			case 0:	 pUIFrame->MovePhase(UIPHASE_SERVER_CHANNEL_LIST);	break;
			case 1:	 pUIFrame->MovePhase(UIPHASE_READY_ROOM);			break;
			default: pUIFrame->MovePhase(UIPHASE_LOBBY);				break;
			}
		}
		break;
	case UIPHASE_READY_ROOM:
		{
			switch(i32PhaseState)
			{
			case 0:	 pUIFrame->MovePhase(UIPHASE_CLANLOBBY);		break;
			case 1:	 pUIFrame->MovePhase(UIPHASE_LOBBY);			break;
			default: m_bBattleProcess = true;						break;
			}			
		}
		break;
	default:
		break;
	}

	m_rEventSendTime = 0.0f;
	m_rChangeStepTime = 0.0f;
	m_bRandomEvent = false;
	m_eCurrentStep = UT_UI_STEP_START;
}

void CGameUnitTest_UI::_SendRandomEvent(void)
{
	GAME_EVENT eEvent = (GAME_EVENT)(i3Math::Rand()); 
	GameEventReceiver::i()->PushReceivedGameEvent(eEvent);
}

///////////////////////////// UI Phase Test에 사용되는 함수 //////////////////////////////
void CGameUnitTest_UI::_OnUpdate_PhaseTest(REAL32 rDeltaSeconds)
{
	if(m_rEventSendTime > UNIT_TEST_UI_RANDOM_EVENT_SEND_TIME)
	{
		_SendGameEvent();
	}

	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	if( pUIFrame)
	{
		UIPHASE phase = pUIFrame->GetCurrentPhaseType();
		UISUBPHASE sub_phase = pUIFrame->GetCurrentSubPhaseType();

		if( m_PhasePopupList[phase].IsOpenPopup() || m_SubPhasePopupList[sub_phase].IsOpenPopup())
			return;
	}

	if(m_rChangeStepTime > UINT_TEST_UI_STEP_CHANGE_TIME)
	{
		_ChangeNextStep();
	}
}

void CGameUnitTest_UI::_SendGameEvent(void)
{
	// 첫 페이지 진입은 어떤 페이지는 관계 없이 랜덤이벤트를 보냅니다.
	if(m_bRandomEvent == true || m_eCurrentStep == UT_UI_STEP_START)
	{
		_SendRandomEvent();
	}
	else 
	{
		UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
		I3ASSERT(pUIFrame != nullptr);

		// 서브 페이즈 테스트인 경우
		if( m_bEnterSubPhase == true)
		{
			switch(pUIFrame->GetCurrentSubPhaseType())
			{
			case USP_INVENTORY:	_ChangeNextStep_SubPhaseInven();	break;
			case USP_SHOP:		_ChangeNextStep_SubPhaseShop();		break;
			default:			break;
			}
		}
		else
		{
			switch(pUIFrame->GetCurrentPhaseType())
			{
			case UIPHASE_SERVER_CHANNEL_LIST:	_ChangeNextStep_PhaseServerList();	break;
			case UIPHASE_CREATE_CHARA:			_ChangeNextStep_PhaseCreateChara();	break;
			case UIPHASE_LOBBY:					_ChangeNextStep_PhaseLobby();		break;
			case UIPHASE_CLANLOBBY:				_ChangeNextStep_PhaseClanLobby();	break;
			case UIPHASE_READY_ROOM:			_ChangeNextStep_PhaseReadyRoom();	break;
			default:							break;
			}
		}
	}
	m_rEventSendTime = 0.0f;
}

void CGameUnitTest_UI::_ChangeNextStep(void)
{
	switch(m_eCurrentStep)
	{
	case UT_UI_STEP_START:	m_eCurrentStep = UT_UI_STEP0;	break;
	case UT_UI_STEP0:		m_eCurrentStep = UT_UI_STEP1;	break;
	case UT_UI_STEP1:		m_eCurrentStep = UT_UI_STEP2;	break;
	case UT_UI_STEP2:		m_eCurrentStep = UT_UI_STEP3;	break;
	case UT_UI_STEP3:		m_eCurrentStep = UT_UI_STEP4;	break;
	default:				break;
	}

	m_rEventSendTime = 0.0f;
	m_rChangeStepTime = 0.0f;
	m_bRandomEvent = false;
}

void CGameUnitTest_UI::_ChangeNextStep_PhaseServerList(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pUIFrame != nullptr);

	UIPhaseSelectServer * pPhase = i3::same_cast<UIPhaseSelectServer*>(pUIFrame->GetCurrentPhase());
	I3ASSERT(pPhase != nullptr);

	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		{
			// 로딩 프로그래스바 제거 
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_SYSTEM_INFO);
			
			// target server index 
			pPhase->m_TargetServerIndex = 0;	//ASSERT 제거.

			// 채널리스트 활성화
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_CHANNEL_LIST);
			m_bRandomEvent = true;
		}
		break;
	case UT_UI_STEP1:
		{
			_ChangeUIPhase();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_ChangeNextStep_PhaseCreateChara(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pUIFrame != nullptr);

	UIPhaseCreateChara * pPhase = i3::same_cast<UIPhaseCreateChara*>(pUIFrame->GetCurrentPhase());
	I3ASSERT(pPhase != nullptr);

	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		{
			UIPopupNickNameSetting * pPopup = static_cast<UIPopupNickNameSetting*>(GameUI::GetPopup(UPW_CREATENICKNAME));
			I3ASSERT_RETURN(pPopup);
			pPopup->ChangeTextMsg(0);
			pPopup->OnOKButton();
			pPopup->OnExitStart();

			GameLoginContext::i()->setUserNick(L"UnitTest");
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CREATE_NICK_NAME);

			m_bRandomEvent = true;
		}
		break;
	case UT_UI_STEP1:
		{
			pPhase->Click_Next();
			m_bRandomEvent = true;
		}
		break;
	case UT_UI_STEP2:
		{
			pPhase->Click_Next();
			
			CHARA_WEAPON_INFO * pWeaponInfo = const_cast<CHARA_WEAPON_INFO*>(CCharaInfoContext::i()->GetCharaWeaponInfo(0));
			pWeaponInfo->Reset();
			
			for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
				CCharaInfoContext::i()->SetMainCharaIdx(static_cast<TEAM_TYPE>(idx), 0 );
	
			_ChangeUIPhase();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_ChangeNextStep_PhaseLobby(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pUIFrame != nullptr);

	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		{
			GameUI::CloseAllMsgBox();
			m_bRandomEvent = true;
		}
		break;
	case UT_UI_STEP1:
		{//1초에 한번씩 호출.
			GameUI::CloseAllMsgBox();

			if( m_PhasePopupList[UIPHASE_LOBBY].IsOpenPopup())
				m_PhasePopupList[UIPHASE_LOBBY].ProcessPopup();
			else
				m_PhasePopupList[UIPHASE_LOBBY].OpenPopup();
			
			m_bRandomEvent = false;
			_SendRandomEvent();
		}
		break;
	case UT_UI_STEP2:
		{
			_StartSubPhaseTest();
		}
		break;
	case UT_UI_STEP3:
		{
			_ChangeUIPhase();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_ChangeNextStep_PhaseClanLobby(void)
{
	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		{
			_StartSubPhaseTest();
		}
		break;

	case UT_UI_STEP1:
		{//1초에 한번씩 호출.
			GameUI::CloseAllMsgBox();

			if( m_PhasePopupList[UIPHASE_CLANLOBBY].IsOpenPopup())
				m_PhasePopupList[UIPHASE_CLANLOBBY].ProcessPopup();
			else
				m_PhasePopupList[UIPHASE_CLANLOBBY].OpenPopup();

			m_bRandomEvent = false;
			_SendRandomEvent();
		}
		break;
	case UT_UI_STEP2:
		{
			GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLAN_WAR_CREATE_TEAM);
			m_bRandomEvent = true;
		}
		break;
	case UT_UI_STEP3:
		{
			_ChangeUIPhase();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_ChangeNextStep_PhaseReadyRoom(void)
{
	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		GameUI::CloseAllMsgBox();

		if( m_PhasePopupList[UIPHASE_READY_ROOM].IsOpenPopup())
			m_PhasePopupList[UIPHASE_READY_ROOM].ProcessPopup();
		else
			m_PhasePopupList[UIPHASE_READY_ROOM].OpenPopup();

		m_bRandomEvent = false;
		_SendRandomEvent();
		break;

	case UT_UI_STEP1:
		{
			_StartSubPhaseTest();
		}
		break;

	case UT_UI_STEP2:
		{
			_ChangeUIPhase();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_ChangeNextStep_SubPhaseInven(void)
{
	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		GameUI::CloseAllMsgBox();

		if( m_SubPhasePopupList[USP_INVENTORY].IsOpenPopup())
			m_SubPhasePopupList[USP_INVENTORY].ProcessPopup();
		else
			m_SubPhasePopupList[USP_INVENTORY].OpenPopup();

		m_bRandomEvent = false;
		_SendRandomEvent();
		break;
	case UT_UI_STEP1:
		{
			_ChangSubPhaseTest();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_ChangeNextStep_SubPhaseShop(void)
{
	switch(m_eCurrentStep)
	{
	case UT_UI_STEP0:
		GameUI::CloseAllMsgBox();

		if( m_SubPhasePopupList[USP_SHOP].IsOpenPopup())
			m_SubPhasePopupList[USP_SHOP].ProcessPopup();
		else
			m_SubPhasePopupList[USP_SHOP].OpenPopup();
		
		m_bRandomEvent = false;
		_SendRandomEvent();
		break;

	case UT_UI_STEP1:
		{
			_ChangSubPhaseTest();
		}
		break;
	default:
		break;
	}
}

void CGameUnitTest_UI::_StartSubPhaseTest(void)
{
	INT32 i32SubPhase = i3Math::Rand() % 3;

	if( i32SubPhase == 0)
	{
		m_bRandomEvent = true;
		return;
	}
	else if(i32SubPhase == 1)
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_INVENTORY);
	}
	else
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_SHOP);
	}

	m_eOldStep = m_eCurrentStep;
	m_eCurrentStep = UT_UI_STEP_START;
	m_bRandomEvent = false;
	m_bEnterSubPhase = true;
	m_i32SubPhaseLoopCnt = 0;
}

void CGameUnitTest_UI::_ChangSubPhaseTest(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	I3ASSERT(pUIFrame != nullptr);

	INT32 i32SubPhase = 0;
	
	// 서브페이지 전환은 3회를 넘지 않습니다.
	if(m_i32SubPhaseLoopCnt < SUBPHASE_LOOP_COUNT)
		i32SubPhase = i3Math::Rand() % 3;

	if(i32SubPhase == 0)
	{
		switch(pUIFrame->GetCurrentSubPhaseType())
		{
		case USP_INVENTORY:
			{
				GameEventReceiver::i()->PushReceivedGameEvent(EVENT_LEAVE_INVENTORY);
			}
			break;
		case USP_SHOP:
			{
				GameEventReceiver::i()->PushReceivedGameEvent(EVENT_LEAVE_SHOP);
			}
			break;
		default:
			break;
		}

		m_i32SubPhaseLoopCnt = 0;
		m_bEnterSubPhase = false;
		m_eCurrentStep = m_eOldStep;
		m_eOldStep = UT_UI_STEP_START;

		m_rEventSendTime = 0.0f;
		m_rChangeStepTime = 0.0f;
		m_bRandomEvent = true;
	}
	else
	{
		switch(pUIFrame->GetCurrentSubPhaseType())
		{
		case USP_INVENTORY:
			{
				GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_SHOP);
			}
			break;
		case USP_SHOP:
			{
				GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_INVENTORY);
			}
			break;
		default:
			break;
		}

		m_eCurrentStep = UT_UI_STEP_START;
		++m_i32SubPhaseLoopCnt;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

void CGameUnitTest_UI::_RegisterPopupList()
{
	//Server Channel List Popup

	//Create Character Popup

	//Lobby Popup
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_CREATEROOM_EX);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_OPTION);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_MAILREV);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_MAILBOX);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_PROFILE);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_USERPROFILE);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_QUERY_PASSWORD);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_CLAN_CREATE);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_CLAN_NAME_CHECK);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_CLAN_JOIN);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_PCCAFE);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_ADDBLOCK);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_COUPON_INPUT);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_DISCOUNT_COUPON);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_EVENT);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_VISIT_EVENT);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_ATTENDANCE_CHECK);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_ATTENDANCE_COMPLETE);
	
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_CLAN_NON_MEMBER);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_CLAN_MEMBER);
	m_PhasePopupList[UIPHASE_LOBBY].AddPopup( UPW_MEDAL_REWARD);

	//ReadyRoom Popup
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_INVITELOBBYUSER);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_RANK_UP);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_EXIT_POPUP);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_TS_REWARD);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_CLANHONOR);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_BREAK_INTO_GAME);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_WEB_BOX);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_DOMI_SCORE);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_DOMI_ROUND);
	m_PhasePopupList[UIPHASE_READY_ROOM].AddPopup( UPW_DOMI_ENTER);
	
	//sub shop
	m_SubPhasePopupList[USP_SHOP].AddPopup( UPW_BUYITEM);
	m_SubPhasePopupList[USP_SHOP].AddPopup( UPW_SHOPEXTENSION);
	m_SubPhasePopupList[USP_SHOP].AddPopup( UPW_QUERY_ITEM);
	m_SubPhasePopupList[USP_SHOP].AddPopup( UPW_RESULT_USEITEM);
	m_SubPhasePopupList[USP_SHOP].AddPopup( UPW_SKILLOPEN_ITEM);
	m_SubPhasePopupList[USP_SHOP].AddPopup( UPW_SKILLOPEN_RESULT);
	
	//sub inven 
	m_SubPhasePopupList[USP_INVENTORY].AddPopup( UPW_CHANGENICK);

	#if legacy_gui_scene
	m_SubPhasePopupList[USP_INVENTORY].AddPopup( UPW_CHANGE_CLAN_NAME);
	#endif

	m_SubPhasePopupList[USP_INVENTORY].AddPopup( UPW_CHANGE_COLOR_NICK);
	m_SubPhasePopupList[USP_INVENTORY].AddPopup( UPW_CHANGE_CROSSHAIR_COLOR);
	m_SubPhasePopupList[USP_INVENTORY].AddPopup( UPW_DISGUISE_RANK);
	m_SubPhasePopupList[USP_INVENTORY].AddPopup( UPW_CHANGE_CLAN_MARK);

}

void CGameUnitTest_UI::_SetStage()
{
}

#endif //end of ENABLE_UNIT_TEST