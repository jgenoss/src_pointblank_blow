#include "pch.h"
#include "PhaseLobby_dmy.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "GameContextScript.h"

I3_CLASS_INSTANCE(CPhaseLobbyDmy, CReadyPhaseBase);

CPhaseLobbyDmy::CPhaseLobbyDmy()
{
	m_pUIScene = NULL;
}

CPhaseLobbyDmy::~CPhaseLobbyDmy()
{
	#if defined( USE_LUA_UI)
	if( m_pUIScene != NULL)
	{
		m_pUIScene->Detach();

		i3UIManager * pUI = i3UI::getManager();

		if( pUI != NULL)	pUI->RemoveScene( m_pUIScene);

		I3_SAFE_RELEASE( m_pUIScene);
	}
	#endif
}

void CPhaseLobbyDmy::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{

}

void CPhaseLobbyDmy::OnGameEvent(INT32 event,INT32 arg)
{
	CReadyPhaseBase::OnGameEvent(event, arg);

	switch(event)
	{
	case EVENT_CREATE_NICK_NAME:
		/*if( EV_SUCCESSED(arg))
		{
			USER_INFO_BASIC* pInfoBasic = g_pGameContext->GetMyUserInfoBasic();
			USER_INFO_EQUIPMENT_WEAPON* pEquipWeapon	=g_pGameContext->GetMyUserInfoEquipmentWeapon();
			g_pGameContext->GetMyUserInfoRecord()->Reset();

			i3String::Copy(pInfoBasic->_nick, g_pGameContext->getUserNick());
			
#if defined(USE_LINK_SHOP)			
			if(pEquipWeapon->_noprim != 0)
			{
				ERROR_POPUP_C(STBL_IDX_EP_LOBBY_EQUIP_SUPPLIED, CbSuppliedConfirm, this);
			}
			else
			{
				(((CStageReady*)m_pParent)->TogglePopup(POPUP_LINK_SHOP, 0));
			}
#else

			ERROR_POPUP_C(STBL_IDX_EP_LOBBY_EQUIP_SUPPLIED, CbSuppliedConfirm, this);
#endif
			
			UpdateCharacter(VT_DEFAULT);
		}
		else
		{
			if( EV_IS_ERROR(arg,EVENT_ERROR_EVENT_DUPLICATE_NICK)) ERROR_POPUP_C(STBL_IDX_EP_LOBBY_SAME_NICK,CbSameNick, this);
			else SYSTEM_POPUP_C(STBL_IDX_EP_LOBBY_FAIL_NICK_E, CbExitNormal, this);
		}*/
		break;
	case EVENT_GET_FIRST_SUPPLY:
		//m_pEquipView->InitViewCharacter();
		break;
	case EVENT_JOIN_QUICK:
		/*if( EV_SUCCESSED(arg))
		{			
			((CStageReady*)m_pParent)->MovePhase(PHASE_READY_ROOM);
		}
		else
		{
			m_bAllowRefresh = TRUE;
			i3GuiRoot::setGuiDisableInput(FALSE);
			if( EV_IS_ERROR(arg,EVENT_ERROR_FAIL)) EhWrongQuickJoin();
			else EhStageError(event,arg);
		}*/
		break;
	case EVENT_CREATE_ROOM:
 		/*if( EV_SUCCESSED(arg))
		{
			if( g_pGameContext->GetBattleChannel() )
			{ 
				// 슬롯을 닫아줍니다.
				for( INT32 i = 10 ; i < SLOT_MAX_COUNT; i++ )
				{
					g_pGameContext->getSlotInfo(i)->_State = SLOT_STATE_CLOSE;
				}
			}

			// 4vs4 모드이면 뒤 8개 슬롯 닫기
			const ROOM_INFO_BASIC* pRoomInfo = g_pGameContext->getMyRoomInfo();

			if (STAGE_SLOT_MODE_4VS4 == getStageSlot(pRoomInfo->_StageID) )
			{
				for(INT32 i = 8 ; i < SLOT_MAX_COUNT; i++ )
				{
					g_pGameContext->getSlotInfo(i)->_State = SLOT_STATE_CLOSE;
				}
			}

			// 첼린지 모드일때 슬롯을 닫아 준다.
			if( g_pGameContext->IsStageMaskTraining() )
			{
				for( INT32 i = 8 ; i < SLOT_MAX_COUNT; i++ )
				{
					g_pGameContext->getSlotInfo(i)->_State = SLOT_STATE_CLOSE;
				}
			}

			CStageReady * pReady = ((CStageReady*)m_pParent);

			if( pReady->GetCurrentStepType() == STEP_CLAN_BATTLE_LOBBY)
				pReady->ChangeStep(STEP_NULL);

			pReady->MovePhase(PHASE_READY_ROOM);			
		}
		else
		{
			m_bAllowRefresh = TRUE;
			i3GuiRoot::setGuiDisableInput(FALSE);
			EhStageError(event,arg);
		}*/
		break;
	case EVENT_JOIN_ROOM:
		/*i3GuiRoot::setGuiDisableInput(FALSE);
		if (g_pCommunity->IsInviteMoving())
		{
			g_pCommunity->ComplateInviteMove();
		}

		if( EV_SUCCESSED(arg))
		{
			CStageReady * pReady = ((CStageReady*)m_pParent);

			if( pReady->GetCurrentStepType() == STEP_CLAN_BATTLE_LOBBY)
				pReady->ChangeStep(STEP_NULL);

			pReady->MovePhase(PHASE_READY_ROOM);
		}
		else
		{
			m_bAllowRefresh = TRUE;
			i3GuiRoot::setGuiDisableInput(FALSE);
			if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL))			EhSlotFull();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM))	EhNotExist();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD))	EhWrongPW();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER))	EhLimitInterenter();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_LEVEL))		EhLimitLever();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_TRAINING))		EhLimitTraining();
			else EhStageError(event,arg);
		}*/
		break;
	case EVENT_MATCH_TEAM_JOIN:			//	클랜 팀 참가
		/*{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
				i3GuiRoot::setGuiDisableInput(FALSE);
			}

  			if( EV_SUCCESSED(arg))
			{
				//	팀에 참가하였으므로 로비로 넘어간다.
				((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_BATTLE_LOBBY);

				g_pFramework->GetClanContext()->DeleteBattleTeamList();
			}
			else
			{
				//	에러는 ClanContext에서 처리함 komet
			}
		}*/
		break;
	case EVENT_LEAVE_LOBBY:
		/*if( EV_SUCCESSED(arg))
		{
			((CStageReady*)m_pParent)->MovePhase(PHASE_CHANNEL_LIST);
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			m_bAllowRefresh = TRUE;
			g_pGameContext->SetEvent(EVENT_DISCONNECT);
			ERROR_POPUP_C(STBL_IDX_EP_SERVER_FAIL_REFRESH_C, CbExitNormal, this);
		}*/
		break;
	case EVENT_GET_ROOM_ADDINFO_N:
		//m_bRequestRoomInfoAdd = FALSE;
#if defined( USE_LUA_UI)
		GameContextScript::OnGameEvent(m_pUIScene, EVENT_GET_ROOM_ADDINFO_N, arg);
#endif
		break;
	case EVENT_M_DESTROYED_NETWORK:	// 네트워크가 끊어졌다
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	case EVENT_INVITED_MESSAGE:
		CReadyPhaseBase::OnInvitedMessage( );
		break;
	case EVENT_INVITED_CLAN_MESSAGE:
		CReadyPhaseBase::OnInvitedMessageClan( );
		break;
	case EVENT_UPDATE_USER_LIST:
		//_BuildUserInfoList();
		#if defined( USE_LUA_UI)
			GameContextScript::OnGameEvent(m_pUIScene, EVENT_UPDATE_USER_LIST, arg);
		#endif
		break;
	default:			
		/* Do nothing */
		break;
	}
}

BOOL CPhaseLobbyDmy::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	#if defined( USE_LUA_UI)
		i3UIManager * pUI = i3UI::getManager();
		I3ASSERT( pUI != NULL);

		m_pUIScene = pUI->LoadUIScene( "UI/PointBlank.i3UIe");
		I3ASSERT( m_pUIScene != NULL);
		m_pUIScene->AddRef();
	#endif

	return TRUE;
}

void CPhaseLobbyDmy::OnInitControl(void)
{
	#if defined( USE_LUA_UI)
	if( m_pUIScene != NULL)
	{
		m_pUIScene->Attach(true);
	}
	#endif
}

void CPhaseLobbyDmy::OnPreGuiNotify(void)
{

}
/*
void CPhaseLobbyDmy::_AutoRefresh( REAL32 rDeltaSeconds)
{

}
*/
void CPhaseLobbyDmy::OnUpdate(REAL32 rDeltaSeconds)
{
#if 0
	CReadyPhaseBase::OnUpdate(rDeltaSeconds);

	if (m_bAllowRefresh)
	{
		_AutoRefresh(rDeltaSeconds);
	}

	// EnterLobby 시 RoomList를 받아오기까지 대기 시간
	if( m_rEnterLobbyWaitTime < ENTER_LOBBY_WAIT_TIME )
	{		
		m_rEnterLobbyWaitTime += rDeltaSeconds;
		return;
	}

	if( m_pWaitRoomListMassageBox )
	{
		g_pFramework->CloseMessageBox(m_pWaitRoomListMassageBox);
		m_pWaitRoomListMassageBox = NULL;

		_SortRoomList();
	}

	if( m_bFirstLogin)
	{
		_ProcessFirstLogin();
		//_ProcessSpecialMode();
	}
	else
	{
		_PocessCommon( rDeltaSeconds);
		_ProcessRoomInfo( rDeltaSeconds);
		_ProcessUserInfoList(rDeltaSeconds);
	}


	// 진급 포인트 시스템 - 하사이상 초기 보너스
	// 레벨업 체크
	if (g_pGameContext->IsInitialRankUp())
	{
		char rankUpMessage[MAX_STRING_COUNT];

		sprintf_s(rankUpMessage, STR_TBL_GUI(STR_TBL_GUI_LOBBY_NOTICE_UPGRADE_CHARA), g_InitialRankUpPoint);/*축하 드립니다!!\n \n진급 포인트 지급 이벤트로 {col:255,0,0,255}%d 포인트{/col}를\n진급 포인트로 지급합니다.\n \n(이후부터는 진급 시 포인트를 획득합니다.)*/
		STRING_POPUP(rankUpMessage);
		
		g_pGameContext->RemoveGameFlag( GAMEFLAG_INITIALRANKUP);
	}	

	if (g_pGameContext->IsGeneralRankUp())
	{
		char generalrankUpMessage[MAX_STRING_COUNT];

		UINT32 point = g_pGameContext->GetRankUpPoint();

		sprintf_s(generalrankUpMessage, STR_TBL_GUI(STR_TBL_GUI_LOBBY_NOTICE_GENERAL_RANK_UP), point);/*진급을 축하 드립니다!!\n \n({col:255,0,0,255}%d 포인트{/col}를\n진급 포인트로 지급합니다.)*/
		STRING_POPUP(generalrankUpMessage);
		
		g_pGameContext->RemoveGameFlag( GAMEFLAG_GENERALRANKUP);
	}

	//	버튼이 깜빡입니다.
	BOOL Update = ! m_pSpecialWarWin->isEnable();
	m_BtnEffector.Update(Update, rDeltaSeconds);

#endif
	static REAL32 elap = 0.0f;
	elap += rDeltaSeconds;

	if( elap > 2.0f)
	{
		g_pGameContext->SetEventOnce( EVENT_GET_ROOM_ADDINFO_N );

		g_pGameContext->SetEventOnce(EVENT_LOBBY_REFRESH_N);

		elap -= 2.0f;
	}
}

void CPhaseLobbyDmy::OnWheelUpdate(INT32 scrollby)
{

}

void CPhaseLobbyDmy::SetInputDisable(BOOL bDisable)
{

}

BOOL CPhaseLobbyDmy::GetInputDisable(void)
{
	return TRUE;
}

void CPhaseLobbyDmy::OnEntranceStart(void)
{

}

BOOL CPhaseLobbyDmy::OnEntrancing(RT_REAL32 rDeltaSeconds)
{
	return TRUE;
}

void CPhaseLobbyDmy::OnEntranceEnd(void)
{

}

void CPhaseLobbyDmy::OnExitStart(void)
{

}

BOOL CPhaseLobbyDmy::OnExiting(RT_REAL32 rDeltaSeconds)
{
	return TRUE;
}

void CPhaseLobbyDmy::OnExitEnd(void)
{

}

void CPhaseLobbyDmy::OnSlideOut(BOOL bRight)
{

}

void CPhaseLobbyDmy::OnSlideIn(BOOL bRight)
{

}

BOOL CPhaseLobbyDmy::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	return TRUE;
}

BOOL CPhaseLobbyDmy::OnSliding(RT_REAL32 rDeltaSeconds)
{
	return TRUE;
}
/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CPhaseLobbyDmy::ResetCreateRoomAddInfo(void)
{
	// 확장된 방생성 팝업
	if( g_pGameContext->GetBattleChannel())
	{		
		m_nStageID = getFirstStageId(STAGE_TYPE_BOMB);	// 폭파미션, 다운타운
		m_SubType = BATTLE_ROUND_TYPE_5 | BATTLE_TIME_MISSION_TYPE_3;
	}
	else
	{
		m_nStageID = getFirstStageId(STAGE_TYPE_DEATHMATCH);
		m_SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
	}

	m_LockWeapon = LOCK_BASIC;//LOCK_PRIMARY | LOCK_SECONDARY | LOCK_MELEE | LOCK_THROW;
	m_LockInterEnter = 0;
	m_LockObserver = 0;
	m_LockTeamBalance = 1;

	_BuildMapList();		//모드 변경에 따른 맵 리스트들을 구합니다.
}

void CPhaseLobbyDmy::_BuildMapList(void)
{
	INT32 count = 0;
	STAGE_TYPE type = getStageType(m_nStageID);

	i3Color::Set(&m_Color, (I3COLOR*)GameGUI::GameModeColor(type));

	m_MapList.Clear();
	for(INT32 i = STAGE_ORD_NA + 1; i < STAGE_ORD_MAX; i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)i);

		if ( isActiveStage(stageId))
		{
			if ((STAGE_TYPE_DEATHMATCH == getStageType(stageId) && type == STAGE_TYPE_ANNIHILATION) || type == getStageType(stageId))
			{
				m_MapList.Add((void*)stageID);
			}
		}
	}

	return; 
}

void CPhaseLobbyDmy::SelectNextStageType(void)
{
	m_nStageID = getNextStageType( (STAGE_ID)m_nStageID, TRUE);

	_BuildMapList();
}

void CPhaseLobbyDmy::SelectPrevStageType(void)
{
	m_nStageID = getNextStageType( (STAGE_ID)m_nStageID, FALSE);

	_BuildMapList();
}

void CPhaseLobbyDmy::SelectNextGoalType(bool bNext)
{
	UINT8 idxMin;
	UINT8 idxMax;

	UINT8 stageType	= (UINT8)getStageType( m_nStageID);

	// 시작/끝 인덱스 설정
	switch( stageType)
	{
	case STAGE_TYPE_ANNIHILATION	: 
	case STAGE_TYPE_BOMB		:
	case STAGE_TYPE_ESCAPE		:
	case STAGE_TYPE_DESTROY		:
	case STAGE_TYPE_DEFENCE		:
		idxMin = BATTLE_ROUND_TYPE_3;
		idxMax = BATTLE_ROUND_TYPE_9;
		break;
	default					:
		idxMin = BATTLE_KILL_TYPE_60;
		idxMax = BATTLE_KILL_TYPE_160;
		break;
	}

	{
		UINT8 gameTime = HINIBBLE(m_SubType);
		UINT8 gameType = LONIBBLE(m_SubType);

		// 타입 순환
		if (bNext)
			gameType = LOOP_INCREASE(gameType, idxMin, idxMax);
		else
			gameType = LOOP_DECREASE(gameType, idxMin, idxMax);

		m_SubType = MAKEBYTE(gameTime, gameType);
	}
}

void CPhaseLobbyDmy::_SelectNextTimeLimit(BOOL bNext)
{
	UINT8 idxMin;
	UINT8 idxMax;

	// 시작/끝 인덱스 설정
	STAGE_TYPE			stageType		= getStageType( m_nStageID);		
	switch( stageType)
	{
	case STAGE_TYPE_BOMB		:
	case STAGE_TYPE_ESCAPE		:
	case STAGE_TYPE_DESTROY		:
	case STAGE_TYPE_DEFENCE		:
	case STAGE_TYPE_ANNIHILATION		:
		idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
		idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_7);
		break;
	case STAGE_TYPE_DEATHMATCH	:
	default						:
		idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
		idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
		break;
	}

	UINT8 gameTime = HINIBBLE(m_SubType);
	UINT8 gameType = LONIBBLE(m_SubType);

	// 시간 순환
	if (bNext)
		gameTime = LOOP_INCREASE(gameTime, idxMin, idxMax);
	else
		gameTime = LOOP_DECREASE(gameTime, idxMin, idxMax);

	// 시간 플래그 설정
	m_SubType = MAKEBYTE(gameTime, gameType);
}

void CPhaseLobbyDmy::UpdateCreateMapInfo(void)
{
	if( m_nStageID == 0)return; //getStageDisplayName( (STAGE_ID) pInfo->_StageID) 에러가 맵이동시 나올수 있습니다. 

	STAGE_TYPE stageType = getStageType( m_nStageID);
	UINT32 gameType	= m_SubType & 0x0F;
	UINT32 gameTime	= m_SubType & 0xF0;
	
	//맵 이미지
	//맵 이미지를  Pef로 뺐기 때문에. 소스에서 불러 올 수 없다.
	//EquipShapeManager.cpp 파일 참조
	INT32 idxImage = GetGuiMapShapeIndex( getStageOrd(m_nStageID));

	// 맵 이름, 모드
	char	strMapName[MAX_STRING_COUNT];
	char	strModeName[MAX_STRING_COUNT];
	BOOL	bAllowChangeMode = FALSE;
	bool	bNewMap = false;
	
	if(m_pLobbyCheckRandomMap->getCheckBoxState())
		i3String::Copy( strMapName, STR_TBL_POPUP(STR_POPUP_RANDOMMAP ));
	else
		i3String::Copy( strMapName, getStageDisplayName( (STAGE_ID)m_nStageID));

	switch( stageType)
	{
	case STAGE_TYPE_DEATHMATCH:
		i3String::Copy( strModeName, STR_TBL(STBL_IDX_GAMEMODE_TEAM));
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_BOMB:
		i3String::Copy( strModeName, STR_TBL(STBL_IDX_GAMEMODE_MISSION));
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_DESTROY:
		i3String::Copy( strModeName, getStageTypeName( (STAGE_ID)m_nStageID));
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_DEFENCE :
		i3String::Copy( strModeName, getStageTypeName( (STAGE_ID)m_nStageID));
		bAllowChangeMode = FALSE;
		break;
	case STAGE_TYPE_ANNIHILATION:
		i3String::Copy( strModeName, getStageTypeName( (STAGE_ID)m_nStageID));
		bAllowChangeMode = TRUE;
		break;
	case STAGE_TYPE_ESCAPE:
		i3String::Copy( strModeName, getStageTypeName( (STAGE_ID)m_nStageID));
		bAllowChangeMode = FALSE;
		break;
	default:
		i3String::Copy(strModeName, "loading");
		break;
	}

	// NewMark
	if ( getNewMarkStageTable( getStageOrd(m_nStageID) == GOODS_MARK_NEW ))
	{
		bNewMap = true;
	}

	// 승리조건
	{
		const char * strType = TextSet::VictoryCondition(stageType, gameType);

		m_pGameTypeValue->SetText(strType);		
	}

	// 제한시간
	{
		m_pTimeValue->SetText(TextSet::GameTime(gameTime));
	}

	m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pLimitPrimaryKey->SetTextColor(GetColor(GCT_DEFAULT));;
	m_pLimitSecondary->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pLimitSecondaryKey->SetTextColor(GetColor(GCT_DEFAULT));;
	m_pLimitMelee->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pLimitMeleeKey->SetTextColor(GetColor(GCT_DEFAULT));;
	m_pLimitThrow->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pLimitThrowKey->SetTextColor(GetColor(GCT_DEFAULT));;
	m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pLimitRPG7Key->SetTextColor(GetColor(GCT_DEFAULT));


	// RPG7전 제한
	if ( g_pConfigEx->GetGame().Enable_RPG7 )
	{		
		m_pLimitRPG7Key->SetEnable(TRUE);
		m_pLimitRPG7->SetEnable(TRUE);

		BOOL bAvariableMode = (getStageSlot( m_nStageID) == STAGE_SLOT_MODE_4VS4);

		// UnlimitedForRPG7의 경우 파괴미션을 제외한 모든 스테이지 선택 가능
		if( getStageType(m_nStageID) != STAGE_TYPE_DESTROY &&
			getStageType(m_nStageID) != STAGE_TYPE_DEFENCE &&
			g_pConfigEx->GetGame().UnlimitedForRPG7)	
			bAvariableMode = TRUE;

		if( bAvariableMode && !m_bEnableSniperMode && !m_bEnableShotGunMode)
		{
			m_pLimitRPG7Key->SetTextColor(GetColor(GCT_DEFAULT));
			m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		}
		else
		{
			m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID));
			m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		}
	}
	else
	{
		m_pLimitRPG7Key->SetEnable(FALSE);
		m_pLimitRPG7->SetEnable(FALSE);
	}

	if(m_bEnableSniperMode || m_bEnableShotGunMode) {
		m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID));
	}

	if(getStageType(m_nStageID) == STAGE_TYPE_ESCAPE)
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);

		m_pLimitPrimary->setCheckBoxState( TRUE);
		m_pLimitPrimaryLock->SetEnable( TRUE);
	}
	else
	{
		if( !m_bEnableSniperMode) {
			if( !m_bEnableShotGunMode) {
				m_pLimitPrimaryLock->SetEnable( FALSE);
			}
		}
	}

}*/