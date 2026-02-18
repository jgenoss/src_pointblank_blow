#include "pch.h"
#include "PhaseReadyRoom.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "ContextMenu.h"
#include "ClanDef.h"
#include "QuestTextDictionary.h"
#include "StepProfileDef.h"
#include "CommunityContext.h"
#include "TextSet.h"
#include "ItemDataBase.h"
#include "ServerInfo.h"
#include "Record.h"
#include "GameGUI.h"

I3_CLASS_INSTANCE(CPhaseReadyRoom, CReadyPhaseBase);

//이정우 : 2011년 3월 18일 미션 카드에 최종 보상을 미리 보기
//#define TEMP_QUEST_REWARD_ENABLE	//미리 보기 

//#define TEMP_QUEST_REWARD_ITEM	//최종 아이템 보상
//#define TEMP_QUEST_REWARD_MEDAL	//최종 메달 보상


CPhaseReadyRoom::CPhaseReadyRoom()
{
	m_pRoomGui = NULL;
	m_pRoomGui = NULL;
	m_pRoomWin = NULL;
	m_pRecordWin = NULL;
	m_pAdvencedWin = NULL;
	m_pInviteWin = NULL;
	m_pRoomCaption = NULL;
	m_pRecordCaption = NULL;
	m_pAdvencedCaption = NULL;
	m_pInviteCaption = NULL;
	m_pKey = NULL;
	m_pBattleMark = NULL;
	m_pStart = NULL;
	m_pReady = NULL;
	m_pCancelStart = NULL;
	m_pCancelReady = NULL;
	m_pInvite = NULL;
	m_pLeave = NULL;
	m_pSearchHost = NULL;
	m_pTeamChange = NULL;
	m_pRedBg = NULL;
	m_pBlueBg = NULL;
	m_pRedList = NULL;
	m_pBlueList = NULL;

	i3mem::FillZero(m_slot, sizeof(m_slot));

	m_pRedSelect = NULL;
	m_pRedFocus = NULL;
	m_pBlueSelect = NULL;
	m_pBlueFocus = NULL;

	m_pMapImage = NULL;
	m_pTypeImage = NULL;
	m_pMapNewMark = NULL;
	m_pMapXMasMark = NULL;
	m_pMapCircle = NULL;
	m_pMap = NULL;
	m_pMapCombo = NULL;
	m_pMapList = NULL;
	m_pTitleKey = NULL;
	m_pTitleValue = NULL;
	m_pTitleChange = NULL;
	m_pPasswordKey = NULL;
	m_pPasswordValue = NULL;
	m_pPasswordChange = NULL;
	m_pModeKey = NULL;
	m_pModeValue = NULL;
	m_pModePrev = NULL;
	m_pModeNext = NULL;
	m_pTypeKey = NULL;
	m_pTypeValue = NULL;
	m_pTypePrev = NULL;
	m_pTypeNext = NULL;
	m_pTimeKey = NULL;
	m_pTimeValue = NULL;
	m_pTimePrev = NULL;
	m_pTimeNext = NULL;
	m_pOptionKey = NULL;
	m_pOption = NULL;
	m_pTrainingTimeKey = NULL;
	m_pTrainingTimeValue = NULL;
	m_pTrainingTimePrev = NULL;
	m_pTrainingTimeNext = NULL;
	m_pTrainingEnemyKey = NULL;
	m_pTrainingEnemyValue = NULL;
	m_pTrainingEnemyPrev = NULL;
	m_pTrainingEnemyNext = NULL;
	m_pTrainingDifficultyKey = NULL;
	m_pTrainingDifficultyValue = NULL;
	m_pTrainingDifficultyCombo = NULL;
	m_pTrainingDifficultyList = NULL;

	m_pRecordNickname = NULL;
	m_pRecordRank = NULL;
	m_pRecordRank_Beginner = NULL;
	m_pSeasonKey = NULL;
	m_pTotalRecord = NULL;
	m_pTotalKillDeath = NULL;
	m_pTotalHeadshot = NULL;
	m_pTotalKey = NULL;
	m_pTotalKillDeath2 = NULL;
	m_pTotalDisconnect = NULL;
	m_pSeasonRecord = NULL;
	m_pSeasonKillDeath = NULL;
	m_pSeasonHeadshot = NULL;
	m_pSeasonKillDeath2 = NULL;
	m_pSeasonDisconnect = NULL;
	m_pClanKey = NULL;
	m_pClanNameKey = NULL;
	m_pClanNameValue = NULL;
	m_pClanRankKey = NULL;
	m_pClanRankValue = NULL;
	m_pClanMark = NULL;
	m_pClanRank = NULL;

	m_pAdvencedWin = NULL;
	m_pAdvencedCaption = NULL;
	m_pAdvencedConfirm = NULL;
	m_pAdvencedCancel = NULL;
	m_pLimitKey = NULL;

	m_pLimitPrimary = NULL;
	m_pLimitPrimaryKey = NULL;
	m_pLimitSecondary = NULL;
	m_pLimitSecondaryKey = NULL;
	m_pLimitMelee = NULL;
	m_pLimitMeleeKey = NULL;
	m_pLimitThrow = NULL;
	m_pLimitThrowKey = NULL;
	m_pLimitRPG7 = NULL;
	m_pLimitRPG7Key = NULL;

	m_pLimitJoinKey = NULL;
	m_pLimitJoinYesKey = NULL;
	m_pLimitJoinNoKey = NULL;
	m_pLimitJoinYes = NULL;
	m_pLimitJoinNo = NULL;
	m_pLimitObserverKey = NULL;
	//	m_pLimitObserverYesKey = NULL;
	//	m_pLimitObserverNoKey = NULL;
	//	m_pLimitObserverYes = NULL;
	//	m_pLimitObserverNo = NULL;
	m_pTeamBalanceKey = NULL;
	m_pTeamBalancePersonnelKey = NULL;
	m_pTeamBalanceAbilityKey = NULL;
	m_pTeamBalanceNoKey = NULL;
	m_pTeamBalancePersonnel = NULL;
	m_pTeamBalanceAbility = NULL;
	m_pTeamBalanceNo = NULL;

	m_pLimitObserver3PV_FreeKey = NULL;
	m_pLimitObserver3PV_Free = NULL;
	m_pLimitObserver1PV_FreeKey = NULL;
	m_pLimitObserver1PV_Free = NULL;
	m_pLimitObserverEnemy_FreeKey = NULL;
	m_pLimitObserverEnemy_Free = NULL;
	m_pLimitObserverEnemyHPKey = NULL;
	m_pLimitObserverEnemyHP = NULL;

	m_pInviteConfirm = NULL;
	m_pInviteCancel = NULL;

	i3mem::FillZero(m_inviteSlot, sizeof(m_inviteSlot));


	// CardComplateWin
	m_pCardComplateWin = NULL;
	m_pCardComplateCaption = NULL;
	m_pCardComplateConfirm = NULL;
	m_pCardComplateExp = NULL;
	m_pCardComplatePoint = NULL;
	m_cardComplateSlot._bg = NULL;
	m_cardComplateSlot._ribbonDummy = NULL;
	m_cardComplateSlot._medalDummy = NULL;
	m_pCardCompleteName = NULL;

	// FinalComplateWin
	m_pFinalComplateWin = NULL;
	m_pFinalComplateCaption = NULL;
	m_pFinalComplateConfirm = NULL;
	m_pFinalComplateName = NULL;
	m_pFinalComplateDescription = NULL;
	m_pFinalComplateBg = NULL;
	m_pFinalComplateDummy = NULL;
	m_pFinalComplateBg2 = NULL;
	m_pFinalComplateDummy2 = NULL;

	// FinalComplateExWin
	m_pFinalComplateExWin = NULL;
	m_pFinalComplateExCaption = NULL;
	m_pFinalComplateExConfirm = NULL;
	m_pFinalComplateExName = NULL;
	m_pFinalComplateExDescription = NULL;
	m_pFinalComplateExPrizeBg = NULL;
	m_pFinalComplateExRibbonDummy = NULL;
	m_pFinalComplateExMedalDummy = NULL;
	m_pFinalComplateExWeaponBg = NULL;
	m_pFinalComplateExWeaponDummy = NULL;

	// FinalComplatePrizeWin
	m_pFinalComplatePrizeWin = NULL;
	m_pFinalComplatePrizeCaption = NULL;
	m_pFinalComplatePrizeConfirm = NULL;
	m_pFinalComplatePrizeName = NULL;
	m_pFinalComplatePrizePoint = NULL;
	m_pFinalComplatePrizeExp = NULL;
	m_pFinalComplatePrizeDescription = NULL;
	m_pFinalComplatePrizeDummy = NULL;

	m_nItemIDForCardSetComplete = 0;
	m_nCountForItemIDForCardSetComplete = 0;

	// 진급 포인트 시스템 윈도우
	m_pRankUpWin = NULL;
	m_pRankUpCaption = NULL;
	m_pRankUpConfirm = NULL;
	m_pRankUpMessage = NULL;
	m_pRankUpMark = NULL;
	m_pRankUpName = NULL;

	//	일반 룸
	m_pNormalRoomInfoGui = NULL;
	m_pNormalRoomBottomGui = NULL;

	//	클랜전 룸
	m_pClanBattleInfoGui = NULL;
	m_pClanBattleInfoRedClanMark = NULL;
	m_pClanBattleInfoBlueClanMark = NULL;
	m_pClanBattleInfoRedClanName = NULL;
	m_pClanBattleInfoRedTeamName = NULL;
	m_pClanBattleInfoBlueClanName = NULL;
	m_pClanBattleInfoBlueTeamName = NULL;
	m_pClanBattleInfoModeKey = NULL;
	m_pClanBattleInfoModeValue = NULL;
	m_pClanBattleInfoModePrev = NULL;
	m_pClanBattleInfoModeNext = NULL;
	m_pClanBattleInfoTypeKey = NULL;
	m_pClanBattleInfoTypeValue = NULL;
	m_pClanBattleInfoTypePrev = NULL;
	m_pClanBattleInfoTypeNext = NULL;
	m_pClanBattleInfoTimeKey = NULL;
	m_pClanBattleInfoTimeValue = NULL;
	m_pClanBattleInfoTimePrev = NULL;
	m_pClanBattleInfoTimeNext = NULL;
	m_pClanBattleInfoOptionKey = NULL;
	m_pClanBattleInfoOptionButton = NULL;

	m_pClanBattleBottomGui = NULL;
	m_pClanBattleBottomTeamChange = NULL;


	i3mem::FillZero(m_bOpenRedSlot, sizeof(m_bOpenRedSlot));


	m_nOldViewSlot = -1;

	m_bChagingTeam = FALSE;
	m_bOnceChatType = FALSE;
	m_bOnceChatTypeAll = FALSE;

	// EXPORT_AVALIABLE_CODE_VALUE
	m_fViewPosX = 0.4f;
	m_fViewPosY = 0.8f;
	m_fViewPosZ = 3.0f;
	m_fViewAngleY = -30.0f;

	m_pWaitInviteMB = NULL;
	m_pBeHostMessageBox = NULL;

	m_bChangeOptionNotify = FALSE;

	m_pCharaAttachRoot = NULL;

	// 강퇴 중 파업 중복 호출 막기위한 플래그 - 킥시점 TRUE, LOBBY_ENTER응답시 FALSE
	m_bKickingState = FALSE;

	m_bRecordWindowSliding = FALSE;
	m_bInRecordWindow = TRUE;
	m_rRecordSlideTimeStamp = 0.0f;

	m_bInviteResult = FALSE;
	m_bInviteMessage = FALSE;
	m_transferHostSlotIdx = -1;

	m_rSearchHostDelay = 0.0f;
	m_rTeamChangeDelay = 0.0f;

	m_pChatBody = CGameChatBody::NewObject();

	m_circleTerm = 0.0f;
	m_circleIdx = 0;
	m_playCircleAnimation = FALSE;

	i3mem::FillZero(m_pCountdownSound, sizeof(m_pCountdownSound));


	m_teamChangePosX = 0.0f;

	m_nSelectedSlotIdx = -1;

	m_AiModeState = AI_MODE_STATE_ORIGINAL;

	m_IndexMap =0;
	m_CheckMap =0;
	
}

CPhaseReadyRoom::~CPhaseReadyRoom()
{
	for(INT32 i = 0; i < COUNTDOWN_COUNT; i++)
	{
		I3_SAFE_RELEASE(m_pCountdownSound[i]);
	}

	I3_SAFE_RELEASE( m_pChatBody);
}

void CPhaseReadyRoom::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CHK_OBSERVER:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			if(g_pGameContext->IsGameMaster() == false) {
				break;
			}

#if !defined( DEF_OBSERVER_MODE)
			if(m_pChkObserver->getCheckBoxState()) {
				g_pGameContext->AddGameFlag( GAMEFLAG_GMOBSERVER);
			}
			else {
				g_pGameContext->RemoveGameFlag( GAMEFLAG_GMOBSERVER);
			}
#endif
		}
		break;
	case GCI_RRP_E_RECORD:
		if (I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent) OnRecordWindow();
		break;
	case GCI_RRP_B_START:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnGameStartButtonClick();
		break;
	case GCI_RRP_B_INVITE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnInviteListButtonClick();
		break;
	case GCI_RRP_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnExitButtonClick();
		break;
	case GCI_RRP_B_SEARCH_HOST:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSearchHost();
		break;
	case GCI_RRP_B_TEAM_CHANGE:
	case GCI_RRP_B_CLANBATTLE_TEAM_CHANGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTeamChange();
		break;
	case GCI_RRP_L_RED:
		if (I3GUI_POPUPLIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnRedListFocus(pNotify->m_nPriParam);
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnRedListSelect(pNotify->m_nPriParam);
		if (I3GUI_POPUPLIST_NOTIFY_RCLICK == pNotify->m_nEvent)	OnRedListContext(pNotify->m_nPriParam);
		break;
	case GCI_RRP_L_BLUE:
		if (I3GUI_POPUPLIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnBlueListFocus(pNotify->m_nPriParam);
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnBlueListSelect(pNotify->m_nPriParam);
		if (I3GUI_POPUPLIST_NOTIFY_RCLICK == pNotify->m_nEvent) OnBlueListContext(pNotify->m_nPriParam);
		break;
	case GCI_RRP_B_KICK_1:
	case GCI_RRP_B_KICK_2:
	case GCI_RRP_B_KICK_3:
	case GCI_RRP_B_KICK_4:
	case GCI_RRP_B_KICK_5:
	case GCI_RRP_B_KICK_6:
	case GCI_RRP_B_KICK_7:
	case GCI_RRP_B_KICK_8:
	case GCI_RRP_B_KICK_9:
	case GCI_RRP_B_KICK_10:
	case GCI_RRP_B_KICK_11:
	case GCI_RRP_B_KICK_12:
	case GCI_RRP_B_KICK_13:
	case GCI_RRP_B_KICK_14:
	case GCI_RRP_B_KICK_15:
	case GCI_RRP_B_KICK_16:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnKickButton(pNotify->m_nID - GCI_RRP_B_KICK_1);
		break;
	case GCI_RRP_B_MAP_CHANGE:
		if (m_pMapCircle->isEnable())
		{
			if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)	OnMapChange();
			if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)	m_playCircleAnimation = TRUE;
		}
		break;
	case GCI_RRP_B_MAP_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnMapCombo();
		break;
	case GCI_RRP_L_MAP_COMBO:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnMapPopup(pNotify->m_nPriParam);
		break;
	case GCI_RRP_B_TITLE_CHANGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTitleChangeButtonClick();
		break;
	case GCI_RRP_B_PASSWORD_CHANGE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnPasswordChangeButtonClick();
		break;
	case GCI_RRP_B_MODE_PREV:
	case GCI_RRP_B_CLANBATTLE_MODE_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnModePrevButtonClick();
		break;
	case GCI_RRP_B_MODE_NEXT:
	case GCI_RRP_B_CLANBATTLE_MODE_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnModeNextButtonClick();
		break;
	case GCI_RRP_B_TYPE_PREV:
	case GCI_RRP_B_CLANBATTLE_TYPE_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnVictoryPrevButtonClick();
		break;
	case GCI_RRP_B_TYPE_NEXT:
	case GCI_RRP_B_CLANBATTLE_TYPE_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnVictoryNextButtonClick();
		break;
	case GCI_RRP_B_TIME_PREV:
	case GCI_RRP_B_CLANBATTLE_TIME_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnVictory2PrevButtonClick();
		break;
	case GCI_RRP_B_TIME_NEXT:
	case GCI_RRP_B_CLANBATTLE_TIME_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnVictory2NextButtonClick();
		break;
	case GCI_RRP_L_LIST:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) m_pChatBody->OnDragChatList();
		break;
	case GCI_RRP_B_CHAT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChatCombo();
		break;
	case GCI_RRP_L_CHAT:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChatPopup(pNotify->m_nPriParam);
		break;
	case GCI_RRP_B_OPTION:
	case GCI_RRP_B_CLANBATTLE_OPTION:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnOption();
		break;
	case GCI_RRP_B_RANDOMMAP:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRandomMap();
		break;
	case GCI_RRP_B_TRAINING_TIME_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingTimePrev();
		break;
	case GCI_RRP_B_TRAINING_TIME_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingTimeNext();
		break;
	case GCI_RRP_B_TRAINING_ENEMY_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingEnemyPrev();
		break;
	case GCI_RRP_B_TRAINING_ENEMY_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingEnemyNext();
		break;
	case GCI_RRP_B_DIFFICULTY_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingDifficultyCombo();
		break;
	case GCI_RRP_L_DIFFICULTY_COMBO:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnTrainingDifficultyPopup(pNotify->m_nPriParam);
		break;
	case GCI_RRP_B_ADVENCED_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAdvencedConfirm();
		break;
	case GCI_RRP_B_ADVENCED_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAdvencedCancel();
		break;
	case GCI_RRP_CHK_ADVENCED_PRIMARY:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			if(m_pLimitPrimary->getCheckBoxState())
			{
				m_pLimitRPG7->setCheckBoxState(FALSE);
			}
		}
		break;
	case GCI_RRP_CHK_ADVENCED_RPG7:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			if ( g_pConfigEx->GetGame().Enable_RPG7)
			{
				if(m_pLimitRPG7->getCheckBoxState())
				{
					m_pLimitPrimary->setCheckBoxState(FALSE);
				}
			}
			else
			{
				m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID_C));
				m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);
				m_pLimitRPG7->setCheckBoxState(FALSE);
			}
		}
		break;

	case GCI_RRP_B_INVITE_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnInviteConfirm();
		break;
	case GCI_RRP_B_INVITE_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnInviteCancel();
		break;
	case GCI_RRP_B_COMPLATE_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnComplateConfirm();
		break;
	case GCI_RRP_B_FINAL_COMPLATE_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			OnFinalComplateWinConfirm();
		}
		break;
	case GCI_RRP_B_FINAL_COMPLATE_CONFIRM_EX:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			OnFinalComplateExWinConfirm();
		}
		break;
	case GCI_RRP_B_FINAL_COMPLATE_CONFIRM_PRIZE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			OnFinalComplatePrizeWinConfirm();
		}
		break;
	case GCI_RRP_CHK_RANDOM :
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)OnCheckRandomMap();
		break;
	case GCI_RRP_B_RANK_UP_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRankUpConfirm();
		break;
	case GCI_RRP_B_JOIN_LIMIT_YES:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnJoinLimitCheck(TRUE);
		break;
	case GCI_RRP_B_JOIN_LIMIT_NO:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnJoinLimitCheck(FALSE);
		break;
		//	case GCI_RRP_B_OBSERVER_LIMIT_YES:
		//		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnObserverLimitCheck(TRUE);
		//		break;
		//	case GCI_RRP_B_OBSERVER_LIMIT_NO:
		//		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnObserverLimitCheck(FALSE);
		//		break;
	case GCI_RRP_B_TEAM_BALANCE_PERSONNEL:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent)		SetTooltip(TIP_TEAM_BALANCE);
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamBalanceCheck(1);
		break;
	case GCI_RRP_B_TEAM_BALANCE_ABILITY:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent)		SetTooltip(TIP_TEAM_BALANCE);
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamBalanceCheck(2);
		break;
	case GCI_RRP_B_TEAM_BALANCE_NO:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent)		SetTooltip(TIP_TEAM_BALANCE);
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamBalanceCheck(0);
		break;
	case GCI_RRP_L_CHAT_CHANNEL_TYPE1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pChatBody->OnChatChannel(0);
		break;
	case GCI_RRP_L_CHAT_CHANNEL_TYPE2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pChatBody->OnChatChannel(1);
		break;
	case GCI_RRP_L_CHAT_CHANNEL_TYPE3:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pChatBody->OnChatChannel(2);
		break;
	case GCI_RRP_L_CHAT_CHANNEL_TYPE4:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) m_pChatBody->OnChatChannel(3);
		break;
	}
}

void CPhaseReadyRoom::OnGameEvent(INT32 event,INT32 arg)
{
	I3NOTICE( "CPhaseReadyRoom::OnGameEvent() - event:%d, arg:%p Enter\n", event, arg);

	CReadyPhaseBase::OnGameEvent(event, arg);

	switch(event)
	{
	case EVENT_READY_BATTLE:
		if( SLOT_STATE_LOAD == arg )
		{
			OnExitEnd();
			
			UINT32 StageID		= g_pGameContext->GetMyRoomStageID();
			char szTemp[256];

			getStageName(szTemp, (STAGE_ID)  StageID);
			g_pGameContext->SetEvent(EVENT_HACKCHECK_STAGECODENAME, szTemp);
			getStageCodeName(szTemp, (STAGE_ID) StageID);
			g_pFramework->GotoNextStage( szTemp);
		}
		else
		{
			if (EV_IS_ERROR(arg, EVENT_ERROR_NO_REAL_IP))
				EhNoRealIP();
			else if (EV_IS_ERROR(arg, EVENT_ERROR_NO_READY_TEAM))
				EhNoReadyTeam();
			else if (EV_IS_ERROR(arg, EVENT_ERROR_NO_START_FOR_UNDER_NAT))
				ERROR_POPUP(GAME_STRING("STBL_IDX_EP_ROOM_NO_START_FOR_UNDER_NAT"));
			else if ( EV_IS_ERROR( arg, EVENT_ERROR_NO_CLAN_TEAM ) )
				ERROR_POPUP(GAME_STRING("STBL_IDX_EP_ROOM_NO_START_FOR_NO_CLAN_TEAM"));
			else if ( EV_IS_ERROR( arg, EVENT_ERROR_NO_CLAN_TEAM_NOTFULL ) )
				ERROR_POPUP(GAME_STRING("STBL_IDX_EP_ROOM_NO_START_FOR_TEAM_NOTFULL"));
			else if ( EV_IS_ERROR( arg, EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY ) )
				ERROR_POPUP(GAME_STRING("STBL_IDX_EP_ROOM_NO_START_FOR_NOT_ALL_READY"));
			else if ( EV_IS_ERROR( arg, EVENT_ERROR_EVENT_READY_WEAPON_EQUIP ) )
				ERROR_POPUP(GAME_STRING("STBL_IDX_EP_ROOM_ERROR_READY_WEAPON_EQUIP"));
			else
				EhStageError(event,arg);


			m_pRoomWin->setInputDisable(FALSE);
			((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
		}
		break;
	case EVENT_BATTLE_COUNTDOWN:
		{
			// 방장에 의한 게임 시작 Count 취소
			if (arg == -1)
			{				
				((CStageReady*)m_pParent)->StartCount(arg);
				((CStageReady*)m_pParent)->Set_bGameStart(FALSE);
				g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_COUNTDOWN_ABORTED_BY_ROOM_MASTER"));/*{col:255,120,0,255}방장이 카운트 다운을 취소 하였습니다.{/col}*/
				break;
			}

			// 상대팀 레디가 풀려 게임 시작 Count 취소
			if (arg <= -2)
			{
				g_pGameContext->getSlotInfo( g_pGameContext->getMainSlotIdx() )->_State = SLOT_STATE_NORMAL;

				((CStageReady*)m_pParent)->StartCount(arg);
				((CStageReady*)m_pParent)->Set_bGameStart(FALSE);
				g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_COUNTDOWN_ABORTED"));/*{col:255,120,0,255}준비상태가 모두 해제되어 카운트 다운이 취소되었습니다.{/col}*/
				break;
			}


			// 게임 시작 Count
			if (arg == 5)
			{
				// 방장이 게임시작 버튼을 눌렀을시 카운터를 종료하기 위해 사용한다.
				((CStageReady*)m_pParent)->Set_bGameStart(TRUE);
				((CStageReady*)m_pParent)->StartCount(arg);
				g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_START_COUNTDOWN"));/*{col:255,120,0,255}카운트 다운을 시작합니다.{/col}*/
			}

			if (0 < arg)
			{
				char countdownText[MAX_STRING_COUNT];
				i3String::Format(countdownText, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PRE_GAME_START_ALARM"), arg);/*{col:255,120,0,255}게임 시작 %d초 전{/col}*/
				g_pFramework->getChatBox()->PutSystemChatting(countdownText);
			}

			if (0 == arg)
			{
				g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_START_GAME"));/*{col:255,120,0,255}게임을 시작합니다.{/col}*/
			}


			if (0 < arg && arg <= COUNTDOWN_COUNT)
			{
				i3SoundContext* pSoundContext = g_pViewer->GetSoundContext();
				if( pSoundContext != NULL)
				{
					pSoundContext->Play(m_pCountdownSound[arg - 1]);
				}
			}
		}
		break;
	case EVENT_GET_LOBBY_USER:
		m_bInviteResult = EV_SUCCESSED(arg);
		m_bInviteMessage = TRUE;
		break;
	case EVENT_INVITE_LOBBY_USER:
		break;
	case EVENT_ENTER_LOBBY:
		if( EV_SUCCESSED(arg))
		{
			//	클랜전일 경우
			CClanContext * pClan = g_pFramework->GetClanContext();
			if(pClan->InBattleTeam())
			{
				((CStageReady*)m_pParent)->ReturnClanBattleLobby();	//	클랜전 로비로 나감
			}
			else
			{
				((CStageReady*)m_pParent)->MovePhase(PHASE_LOBBY);
			}
		}
		else
		{
			if (g_pCommunity->IsInviteMoving())
			{
				g_pCommunity->ComplateInviteMove();
			}

			m_bEnableUpdate = TRUE;
			i3GuiRoot::setGuiDisableInput(FALSE);
			EhStageError(event,arg);
		}

		m_bKickingState = FALSE;
		break;
	case EVENT_M_KICK_USER:
		if( EV_SUCCESSED(arg))
		{
			EhKicked();
		}
		break;
	case EVENT_CHANGE_SLOT_N:
		if( EV_SUCCESSED(arg))
		{
			// null;
			// 성공 시, 슬롯은 서버에서 보내주는 값으로 자동 업데이트 된다 - praptor
		}
		break;
	case EVENT_CHANGE_ROOM_OPTION_N:
		_NotifyChangeOption();
		break;
	case EVENT_UNREADY_4VS4_N:
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_NOTICE_4VS4_LIMIT_COUNT"));/*4vs4 인원제한에 걸려\n준비완료 상태를 해제합니다.*/
		}
		break;
	case EVENT_GET_ROOM_ADDINFO_N:
		// empty
		break;
	case EVENT_M_DESTROYED_NETWORK:	// 네트워크가 끊어졌다
		SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_GAME_DESTROYED_NETWORK_E"), EXIT_CALLBACK(CbExitNormal), this);
		break;
	case EVENT_INVITED_MESSAGE:
		CReadyPhaseBase::OnInvitedMessage();
		break;
	case EVENT_INVITED_CLAN_MESSAGE:
		CReadyPhaseBase::OnInvitedMessageClan();
		break;
	case EVENT_SEARCH_HOST:
		{
			char message[MAX_STRING_COUNT];

			if (EV_SUCCESSED(arg))
			{
				// 방장 외 RealIP 있다.
				sprintf_s(message, GAME_STRING("STR_TBL_GUI_READY_MESSAGE_CHANGE_ROOM_MASTER"), g_pGameContext->getNickForSlot(arg));/*최적 방장 검색 결과\n[%s]님\n방장을 변경 하시겠습니까?*/
			}
			else
			{
				if (g_pGameContext->isRealIp())
				{
					// 방장만 RealIP다.
					sprintf_s(message, GAME_STRING("STR_TBL_GUI_READY_NOTICE_CHANGE_ROOM_MASTER_RESULT2"), g_pGameContext->getNickForSlot(g_pGameContext->getMySlotIdx()));/*최적 방장 검색 결과\n[%s]님\n방장에 최적화된 다른 사용자가 없지만 변경 하시겠습니까?*/
				}
				else
				{
					// 아무도 RealIP가 아니다.
					sprintf_s(message, GAME_STRING("STR_TBL_GUI_READY_NOTICE_CHANGE_ROOM_MASTER_RESULT1"));/*최적 방장 검색 결과\n없습니다.\n방장에 최적화된 다른 사용자가 없지만 변경 하시겠습니까?*/
				}
			}

			m_transferHostSlotIdx = arg;
			STRING_POPUP_Q(message, MAKE_CALLBACK(CbSearchHost), this);
		}
		break;
	case EVENT_REQUEST_HOST_CHANGE:
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_TBL_GUI_READY_NOTICE_ROOM_MASTER_CHANGE_REQUEST"), g_pGameContext->getNickForSlot(arg));/*[%s]님이 방장검색을 요청하셨습니다.*/
			g_pFramework->getChatBox()->PutSystemChatting(message);
		}
		break;
	case EVENT_TRANSFER_HOST:
		{
			// 방장 변경 성공
			if (EV_SUCCESSED(arg))
			{
				char message[MAX_STRING_COUNT];

				sprintf_s(message, GAME_STRING("STR_TBL_GUI_READY_NOTICE_CHANGED_ROOM_MASTER"), g_pGameContext->getNickForSlot(arg));/*[%s]님이 방장이 되었습니다.*/
				g_pFramework->getChatBox()->PutSystemChatting(message);

				if (g_pGameContext->getMySlotIdx() != arg)
				{
					//m_bIsHost = FALSE;
					// 일단막습니다.
				}
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_NO_EXIST_TO_CHANGE_ROOM_MASTER"));/*방장을 넘길 적합한 사용자가 없습니다.*/
			}
		}
		break;
	case EVENT_ITEM_AUTH:	// 갯수제 아이템 인증
		{
			if (EV_SUCCESSED(arg))
			{
				OpenPopupUsingCashItem();
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_ITEM_IDENTYFY_FAIL_BY_NETWORK_PROBLEM"));/*네트워크 문제로 아이템 인증에 실패했습니다.\n 다시 시도해 주세요.*/
			}
		}
		break;
	case EVENT_QUEST_CHANGE_ACTIVE_IDX:
		//CheckBattleChallengeComplate();
		break;

	case EVENT_ITEM_AUTH_DATA:
		{
			if (EV_FAILED(arg))
			{
				// 아이템 사용 실패
				// 아이템을 사용을 실패하였습니다.
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_USE_ITEM_3"), GAME_STRING("STR_POPUP_CAPTION_FAILED_USE_ITEM"));
			}
		}
		break;

	case EVENT_VIEW_OTHER_USER_ITEM_INFO:
		{
			const char * UserName = g_pGameContext->GetViewOtherUserItemInfoUserName();

			string NameList;
			const i3List & OtherUserItemList = g_pGameContext->GetOtherUserItemInfoList();
			GetItemNameList(NameList, OtherUserItemList);

			char Msg[1024] = {};
			i3String::Format(Msg, 1024, GAME_STRING("STR_TBL_GUI_VIEW_OTHER_USER_ITEM_INFO_MSG"), UserName, NameList.c_str());

			g_pFramework->PutSystemChatting(Msg);
		}
		break;

	default:
		/* Do nothing */
		break;
	}

	I3NOTICE( "CPhaseReadyRoom::OnGameEvent() Leave < event : %d, arg : %d >\n", event, arg);
}

BOOL CPhaseReadyRoom::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "chkObserver", m_pChkObserver, GCI_CHK_OBSERVER);
	LinkControlEx(plist, "editObserver", m_pEditObserver, -1);

	LinkControlEx(plist, "ReadyRoom", m_pRoomGui, -1);
	LinkControlEx(plist, "rrRoomWin", m_pRoomWin, -1);
	LinkControlEx(plist, "rrRecordWin", m_pRecordWin, -1);
	LinkControlEx(plist, "rrAdvencedWin", m_pAdvencedWin, -1);
	LinkControlEx(plist, "rrInviteWin", m_pInviteWin, -1);
	LinkControlEx(plist, "rrRoomCaption", m_pRoomCaption, -1);
	LinkControlEx(plist, "rrRecordCaption", m_pRecordCaption, GCI_RRP_E_RECORD);
	LinkControlEx(plist, "rrAdvencedCaption", m_pAdvencedCaption, -1);
	LinkControlEx(plist, "rrInviteCaption", m_pInviteCaption, -1);

	LinkControlEx(plist, "rrKey", m_pKey, -1);
	LinkControlEx(plist, "rrBattleMark", m_pBattleMark, -1);
	LinkControlEx(plist, "rrStart", m_pStart, GCI_RRP_B_START);
	LinkControlEx(plist, "rrReady", m_pReady, GCI_RRP_B_START);
	LinkControlEx(plist, "rrCancelStart", m_pCancelStart, GCI_RRP_B_START);
	LinkControlEx(plist, "rrCancelReady", m_pCancelReady, GCI_RRP_B_START);
	LinkControlEx(plist, "rrInvite", m_pInvite, GCI_RRP_B_INVITE);
	LinkControlEx(plist, "rrLeave", m_pLeave, GCI_RRP_B_LEAVE);
	LinkControlEx(plist, "rrSearchHost", m_pSearchHost, GCI_RRP_B_SEARCH_HOST);
	LinkControlEx(plist, "rrChange", m_pTeamChange, GCI_RRP_B_TEAM_CHANGE);	// 팀바꾸기

	LinkControlEx(plist, "rrRed", m_pRedBg, -1);
	LinkControlEx(plist, "rrBlue", m_pBlueBg, -1);
	LinkControlEx(plist, "rrRedList", m_pRedList, GCI_RRP_L_RED);
	LinkControlEx(plist, "rrBlueList", m_pBlueList, GCI_RRP_L_BLUE);
	//RandomMap
	LinkControlEx(plist, "CheckRandomMap", m_pSelectRandomMap, GCI_RRP_CHK_RANDOM);
	

	for(UINT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		char strTeam[MAX_STRING_COUNT];
		char strName[MAX_STRING_COUNT];
		UINT32 number = (i / 2) + 1;

		if (0 == i % 2)
			sprintf_s(strTeam, "rrRed");
		else
			sprintf_s(strTeam, "rrBlue");

		sprintf_s(strName, "%sOpen%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pOpen, -1);

		sprintf_s(strName, "%sRank%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pRank, -1);

		sprintf_s(strName, "%sNickname%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pNickname, -1);

		sprintf_s(strName, "%sState%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pState, -1);

		sprintf_s(strName, "%sKick%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pKick, GCI_RRP_B_KICK_1 + i);

		sprintf_s(strName, "%sClose%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pClose, -1);

		sprintf_s(strName, "%sPc%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pPcCafe, -1);

		sprintf_s(strName, "%sClanRank%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pClanRank, -1);

		sprintf_s(strName, "%sClanMark%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pClanMark, -1);

		sprintf_s(strName, "%sRank_Beginner%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pRankBeginner, -1);

		sprintf_s(strName, "%sESportLeft%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pESportLeft, -1);

		sprintf_s(strName, "%sESportRight%d", strTeam, number);
		LinkControlEx(plist, strName, m_slot[i]._pESportRight, -1);
	}

	LinkControlEx(plist, "rrRedSelect", m_pRedSelect, -1);
	LinkControlEx(plist, "rrRedFocus", m_pRedFocus, -1);
	LinkControlEx(plist, "rrBlueSelect", m_pBlueSelect, -1);
	LinkControlEx(plist, "rrBlueFocus", m_pBlueFocus, -1);

	LinkControlEx(plist, "rrImageDummy", m_pMapImage, GCI_RRP_B_MAP_CHANGE);
	LinkControlEx(plist, "rrImageDummy2", m_pTypeImage, -1);
	LinkControlEx(plist, "rrImageDummy3", m_pMapNewMark, -1);
	LinkControlEx(plist, "rrImageDummy4", m_pExtraTypeImage, -1);
	LinkControlEx(plist, "rrImageCircle", m_pMapCircle, -1);
	LinkControlEx(plist, "rrImageXMas",	m_pMapXMasMark, -1);
	LinkControlEx(plist, "rrMap", m_pMap, -1);
	LinkControlEx(plist, "rrMapCombo", m_pMapCombo, GCI_RRP_B_MAP_COMBO);
	LinkControlEx(plist, "rrMapPopup", m_pMapList, GCI_RRP_L_MAP_COMBO);
	LinkControlEx(plist, "rrTitleKey", m_pTitleKey, -1);
	LinkControlEx(plist, "rrTitleValue", m_pTitleValue, -1);
	LinkControlEx(plist, "rrTitleChange", m_pTitleChange, GCI_RRP_B_TITLE_CHANGE);
	LinkControlEx(plist, "rrPasswordKey", m_pPasswordKey, -1);
	LinkControlEx(plist, "rrPasswordValue", m_pPasswordValue, -1);
	LinkControlEx(plist, "rrPasswordChange", m_pPasswordChange, GCI_RRP_B_PASSWORD_CHANGE);
	LinkControlEx(plist, "rrModeKey", m_pModeKey, -1);
	LinkControlEx(plist, "rrModeValue", m_pModeValue, -1);
	LinkControlEx(plist, "rrModePrev", m_pModePrev, GCI_RRP_B_MODE_PREV);
	LinkControlEx(plist, "rrModeNext", m_pModeNext, GCI_RRP_B_MODE_NEXT);
	LinkControlEx(plist, "rrTypeKey", m_pTypeKey, -1);
	LinkControlEx(plist, "rrTypeValue", m_pTypeValue, -1);
	LinkControlEx(plist, "rrTypePrev", m_pTypePrev, GCI_RRP_B_TYPE_PREV);
	LinkControlEx(plist, "rrTypeNext", m_pTypeNext, GCI_RRP_B_TYPE_NEXT);
	LinkControlEx(plist, "rrTimeKey", m_pTimeKey, -1);
	LinkControlEx(plist, "rrTimeValue", m_pTimeValue, -1);
	LinkControlEx(plist, "rrTimePrev", m_pTimePrev, GCI_RRP_B_TIME_PREV);
	LinkControlEx(plist, "rrTimeNext", m_pTimeNext, GCI_RRP_B_TIME_NEXT);
	LinkControlEx(plist, "rrOptionKey", m_pOptionKey, -1);
	LinkControlEx(plist, "rrOption", m_pOption, GCI_RRP_B_OPTION);
	LinkControlEx(plist, "rrRandomMap", m_pRandomMap, GCI_RRP_B_RANDOMMAP);//랜덤
	LinkControlEx(plist, "rrTrainingTimeKey", m_pTrainingTimeKey, -1);
	LinkControlEx(plist, "rrTrainingTimeValue", m_pTrainingTimeValue, -1);
	LinkControlEx(plist, "rrTrainingTimePrev", m_pTrainingTimePrev, GCI_RRP_B_TRAINING_TIME_PREV);
	LinkControlEx(plist, "rrTrainingTimeNext", m_pTrainingTimeNext, GCI_RRP_B_TRAINING_TIME_NEXT);
	LinkControlEx(plist, "rrTrainingEnemyKey", m_pTrainingEnemyKey, -1);
	LinkControlEx(plist, "rrTrainingEnemyValue", m_pTrainingEnemyValue, -1);
	LinkControlEx(plist, "rrTrainingEnemyPrev", m_pTrainingEnemyPrev, GCI_RRP_B_TRAINING_ENEMY_PREV);
	LinkControlEx(plist, "rrTrainingEnemyNext", m_pTrainingEnemyNext, GCI_RRP_B_TRAINING_ENEMY_NEXT);
	LinkControlEx(plist, "rrTrainingDifficultyKey", m_pTrainingDifficultyKey, -1);
	LinkControlEx(plist, "rrTrainingDifficultyValue", m_pTrainingDifficultyValue, -1);
	LinkControlEx(plist, "rrTrainingDifficultyCombo", m_pTrainingDifficultyCombo, GCI_RRP_B_DIFFICULTY_COMBO);
	LinkControlEx(plist, "rrTrainingDifficultyPopup", m_pTrainingDifficultyList, GCI_RRP_L_DIFFICULTY_COMBO);

	LinkControlEx(plist, "rrRecordNickname", m_pRecordNickname, -1);
	LinkControlEx(plist, "rrRecordRank", m_pRecordRank, -1);
	LinkControlEx(plist, "rrRecordRank_Beginner", m_pRecordRank_Beginner, -1);
	LinkControlEx(plist, "rrSeasonKey", m_pSeasonKey, -1);
	LinkControlEx(plist, "rrTotalRecord", m_pTotalRecord, -1);
	LinkControlEx(plist, "rrTotalKillDeath", m_pTotalKillDeath, -1);
	LinkControlEx(plist, "rrTotalHeadshot", m_pTotalHeadshot, -1);
	LinkControlEx(plist, "rrTotalKey", m_pTotalKey, -1);
	LinkControlEx(plist, "rrTotalKillDeath2", m_pTotalKillDeath2, -1);
	LinkControlEx(plist, "rrTotalDisconnect", m_pTotalDisconnect, -1);
	LinkControlEx(plist, "rrSeasonRecord", m_pSeasonRecord, -1);
	LinkControlEx(plist, "rrSeasonKillDeath", m_pSeasonKillDeath, -1);
	LinkControlEx(plist, "rrSeasonHeadshot", m_pSeasonHeadshot, -1);
	LinkControlEx(plist, "rrSeasonKillDeath2", m_pSeasonKillDeath2, -1);
	LinkControlEx(plist, "rrSeasonDisconnect", m_pSeasonDisconnect, -1);
	LinkControlEx(plist, "rrClanKey", m_pClanKey, -1);
	LinkControlEx(plist, "rrClanNameKey", m_pClanNameKey, -1);
	LinkControlEx(plist, "rrClanNameValue", m_pClanNameValue, -1);
	LinkControlEx(plist, "rrClanRankKey", m_pClanRankKey, -1);
	LinkControlEx(plist, "rrClanRankValue", m_pClanRankValue, -1);
	LinkControlEx(plist, "rrClanMark", m_pClanMark, -1);
	LinkControlEx(plist, "rrClanRank", m_pClanRank, -1);

	LinkControlEx(plist, "rrAdvencedConfirm", m_pAdvencedConfirm, GCI_RRP_B_ADVENCED_CONFIRM);
	LinkControlEx(plist, "rrAdvencedCancel", m_pAdvencedCancel, GCI_RRP_B_ADVENCED_CANCEL);
	LinkControlEx(plist, "rrLimitKey", m_pLimitKey, -1);

	LinkControlEx(plist, "rrLimitPrimary", m_pLimitPrimary, GCI_RRP_CHK_ADVENCED_PRIMARY);
	LinkControlEx(plist, "rrLimitPrimaryKey", m_pLimitPrimaryKey, -1);
	LinkControlEx(plist, "rrLimitPrimaryLock", m_pLimitPrimaryLock, -1);
	LinkControlEx(plist, "rrLimitSecondary", m_pLimitSecondary, -1);
	LinkControlEx(plist, "rrLimitSecondaryKey", m_pLimitSecondaryKey, -1);
	LinkControlEx(plist, "rrLimitSecondaryLock", m_pLimitSecondaryLock, -1);
	LinkControlEx(plist, "rrLimitMelee", m_pLimitMelee, -1);
	LinkControlEx(plist, "rrLimitMeleeKey", m_pLimitMeleeKey, -1);
	LinkControlEx(plist, "rrLimitMeleeLock", m_pLimitMeleeLock, -1);
	LinkControlEx(plist, "rrLimitThrow", m_pLimitThrow, -1);
	LinkControlEx(plist, "rrLimitThrowKey", m_pLimitThrowKey, -1);
	LinkControlEx(plist, "rrLimitThrowLock", m_pLimitThrowLock, -1);
	LinkControlEx(plist, "rrLimitRPG7", m_pLimitRPG7, GCI_RRP_CHK_ADVENCED_RPG7);
	LinkControlEx(plist, "rrLimitRPG7Key", m_pLimitRPG7Key, -1);
	LinkControlEx(plist, "rrLimitRPG7Lock", m_pLimitRPG7Lock, -1);

	LinkControlEx(plist, "rrLimitJoinKey", m_pLimitJoinKey, -1);
	LinkControlEx(plist, "rrLimitJoinYesKey", m_pLimitJoinYesKey, -1);
	LinkControlEx(plist, "rrLimitJoinNoKey", m_pLimitJoinNoKey, -1);
	LinkControlEx(plist, "rrLimitJoinYes", m_pLimitJoinYes, GCI_RRP_B_JOIN_LIMIT_YES);
	LinkControlEx(plist, "rrLimitJoinNo", m_pLimitJoinNo, GCI_RRP_B_JOIN_LIMIT_NO);
	LinkControlEx(plist, "rrTeamBalanceKey", m_pTeamBalanceKey, -1);
	LinkControlEx(plist, "rrTeamBalanceYesKey2", m_pTeamBalancePersonnelKey, -1);
	LinkControlEx(plist, "rrTeamBalanceYesKey", m_pTeamBalanceAbilityKey, -1);
	LinkControlEx(plist, "rrTeamBalanceNoKey", m_pTeamBalanceNoKey, -1);
	LinkControlEx(plist, "rrTeamBalanceYes2", m_pTeamBalancePersonnel, GCI_RRP_B_TEAM_BALANCE_PERSONNEL);
	LinkControlEx(plist, "rrTeamBalanceYes", m_pTeamBalanceAbility, GCI_RRP_B_TEAM_BALANCE_ABILITY);
	LinkControlEx(plist, "rrTeamBalanceNo", m_pTeamBalanceNo, GCI_RRP_B_TEAM_BALANCE_NO);

	LinkControlEx(plist, "rrLimitObserverKey", m_pLimitObserverKey, -1);
	//	LinkControlEx(plist, "rrLimitObserverYesKey", m_pLimitObserverYesKey, -1);
	//	LinkControlEx(plist, "rrLimitObserverNoKey", m_pLimitObserverNoKey, -1);
	//	LinkControlEx(plist, "rrLimitObserverYes", m_pLimitObserverYes, GCI_RRP_B_OBSERVER_LIMIT_YES);
	//	LinkControlEx(plist, "rrLimitObserverNo", m_pLimitObserverNo, GCI_RRP_B_OBSERVER_LIMIT_NO);

	LinkControlEx(plist, "rrReadyObserver3PV_Free", m_pLimitObserver3PV_Free, GCI_RRP_CHK_LIMIT_OBSERVER_3PV_FREE);			// ID
	LinkControlEx(plist, "rrReadyObserver3PV_FreeKey", m_pLimitObserver3PV_FreeKey, -1);
	LinkControlEx(plist, "rrReadyObserver1PV_Free", m_pLimitObserver1PV_Free, GCI_RRP_CHK_LIMIT_OBSERVER_1PV_FREE);			// ID
	LinkControlEx(plist, "rrReadyObserver1PV_FreeKey", m_pLimitObserver1PV_FreeKey, -1);
	LinkControlEx(plist, "rrReadyObserverEnemy_Free", m_pLimitObserverEnemy_Free, GCI_RRP_CHK_LIMIT_OBSERVER_ENEMY_FREE);			// ID
	LinkControlEx(plist, "rrReadyObserverEnemy_FreeKey", m_pLimitObserverEnemy_FreeKey, -1);
	LinkControlEx(plist, "rrReadyObserverEnemyHP_Free", m_pLimitObserverEnemyHP, GCI_RRP_CHK_LIMIT_OBSERVER_ENEMY_HP);			// ID
	LinkControlEx(plist, "rrReadyObserverEnemyHP_FreeKey", m_pLimitObserverEnemyHPKey, -1);

	LinkControlEx(plist, "rrInviteConfirm", m_pInviteConfirm, GCI_RRP_B_INVITE_CONFIRM);
	LinkControlEx(plist, "rrInviteCancel", m_pInviteCancel, GCI_RRP_B_INVITE_CANCEL);

	// 채팅의 경우 특별하게 링크합니다.
	{
		char strTemp[MAX_STRING_COUNT];

		LinkControlEx(plist, "rrList", m_pChatBody->m_pChatList, GCI_RRP_L_LIST);
		LinkControlEx(plist, "rrTextBox", m_pChatBody->m_pTextBox, -1);
		LinkControlEx(plist, "rrChatCombo", m_pChatBody->m_pChatCombo, -1);
		LinkControlEx(plist, "rrChatButton", m_pChatBody->m_pChatButton, GCI_RRP_B_CHAT);
		LinkControlEx(plist, "rrChatPopup", m_pChatBody->m_pChatPopup, GCI_RRP_L_CHAT);
		LinkControlEx(plist, "rrChatBox", m_pChatBody->m_pChatBox, -1);
		LinkControlEx(plist, "rrChatLang", m_pChatBody->m_pChatLang, -1);

		for(INT32 i=0; i<CHAT_CHANNEL_BUTTON_COUNT; i++)
		{
			sprintf_s(strTemp, "rrChatTab%d", i + 1);

			LinkControlEx(plist, strTemp, m_pChatBody->m_pChatChannelButton[i], GCI_RRP_L_CHAT_CHANNEL_TYPE1+i);
		}
	}

	for(UINT32 i = 0; i < MAX_INVITE_COUNT; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "rrInviteCheck%d", i + 1);
		LinkControlEx(plist, strName, m_inviteSlot[i]._pCheck, -1);

		sprintf_s(strName, "rrInviteRank%d", i + 1);
		LinkControlEx(plist, strName, m_inviteSlot[i]._pRank, -1);

		sprintf_s(strName, "rrInviteNickname%d", i + 1);
		LinkControlEx(plist, strName, m_inviteSlot[i]._pNickname, -1);

		sprintf_s(strName, "rrInviteRank_Beginner%d", i + 1);
		LinkControlEx(plist, strName, m_inviteSlot[i]._pRankBeginner, -1);
	}


	// CardComplateWin
	LinkControlEx(plist, "rrCardComplateWin", m_pCardComplateWin, -1);
	LinkControlEx(plist, "rrCardComplateCaption", m_pCardComplateCaption, -1);
	LinkControlEx(plist, "rrCardComplateConfirm", m_pCardComplateConfirm, GCI_RRP_B_COMPLATE_CONFIRM);
	LinkControlEx(plist, "rrCardComplateExp", m_pCardComplateExp, -1);
	LinkControlEx(plist, "rrCardComplatePoint", m_pCardComplatePoint, -1);

	LinkControlEx(plist, "rrCardComplateBg1", m_cardComplateSlot._bg, -1);
	LinkControlEx(plist, "rrCardComplateRibbonDummy1", m_cardComplateSlot._ribbonDummy, -1);
	LinkControlEx(plist, "rrCardComplateMedalDummy1", m_cardComplateSlot._medalDummy, -1);
	LinkControlEx(plist, "rrCardComplateName", m_pCardCompleteName, -1);

	// FinalComplateWin
	LinkControlEx(plist, "rrFinalComplateWin", m_pFinalComplateWin, -1);
	LinkControlEx(plist, "rrFinalComplateCaption", m_pFinalComplateCaption, -1);
	LinkControlEx(plist, "rrFinalComplateConfirm", m_pFinalComplateConfirm, GCI_RRP_B_FINAL_COMPLATE_CONFIRM);
	LinkControlEx(plist, "rrFinalComplateName", m_pFinalComplateName, -1);
	LinkControlEx(plist, "rrFinalComplateDescription", m_pFinalComplateDescription, -1);
	LinkControlEx(plist, "rrFinalComplateBg", m_pFinalComplateBg, -1);
	LinkControlEx(plist, "rrFinalComplateDummy", m_pFinalComplateDummy, -1);
	LinkControlEx(plist, "rrFinalComplateBg2", m_pFinalComplateBg2, -1);
	LinkControlEx(plist, "rrFinalComplateDummy2", m_pFinalComplateDummy2, -1);

	// FinalComplateExWin
	LinkControlEx(plist, "rrFinalComplateExWin", m_pFinalComplateExWin, -1);
	LinkControlEx(plist, "rrFinalComplateExCaption", m_pFinalComplateExCaption, -1);
	LinkControlEx(plist, "rrFinalComplateExConfirm", m_pFinalComplateExConfirm, GCI_RRP_B_FINAL_COMPLATE_CONFIRM_EX);
	LinkControlEx(plist, "rrFinalComplateExName", m_pFinalComplateExName, -1);
	LinkControlEx(plist, "rrFinalComplateExDescription", m_pFinalComplateExDescription, -1);
	LinkControlEx(plist, "rrFinalComplateExPrizeBg", m_pFinalComplateExPrizeBg, -1);
	LinkControlEx(plist, "rrFinalComplateExRibbonDummy", m_pFinalComplateExRibbonDummy, -1);
	LinkControlEx(plist, "rrFinalComplateExMedalDummy", m_pFinalComplateExMedalDummy, -1);
	LinkControlEx(plist, "rrFinalComplateExWeaponBg", m_pFinalComplateExWeaponBg, -1);
	LinkControlEx(plist, "rrFinalComplateExWeaponDummy", m_pFinalComplateExWeaponDummy, -1);

	// FinalComplatePrizeWin
	LinkControlEx(plist, "rrFinalComplatePrizeWin", m_pFinalComplatePrizeWin, -1);
	LinkControlEx(plist, "rrFinalComplatePrizeCaption", m_pFinalComplatePrizeCaption, -1);
	LinkControlEx(plist, "rrFinalComplatePrizeConfirm", m_pFinalComplatePrizeConfirm, GCI_RRP_B_FINAL_COMPLATE_CONFIRM_PRIZE);
	LinkControlEx(plist, "rrFinalComplatePrizeName", m_pFinalComplatePrizeName, -1);
	LinkControlEx(plist, "rrFinalComplatePrizePoint", m_pFinalComplatePrizePoint, -1);
	LinkControlEx(plist, "rrFinalComplatePrizeExp", m_pFinalComplatePrizeExp, -1);
	LinkControlEx(plist, "rrFinalComplatePrizeDescription", m_pFinalComplatePrizeDescription, -1);
	LinkControlEx(plist, "rrFinalComplatePrizeDummy", m_pFinalComplatePrizeDummy, -1);

	// 진급 포인트 시스템 윈도우
	LinkControlEx(plist, "rrRankUpWin", m_pRankUpWin, -1);
	LinkControlEx(plist, "rrRankUpCaption", m_pRankUpCaption, -1);
	LinkControlEx(plist, "rrRankUpConfirm", m_pRankUpConfirm, GCI_RRP_B_RANK_UP_CONFIRM);
	LinkControlEx(plist, "rrRankUpMessage", m_pRankUpMessage, -1);
	LinkControlEx(plist, "rrRankUpMark", m_pRankUpMark, -1);
	LinkControlEx(plist, "rrRankUpName", m_pRankUpName, -1);

	//	클랜 매치메이킹용
	{
		//	일반 룸
		LinkControlEx(plist, "rrNormalInfo", m_pNormalRoomInfoGui, GCI_UNKNOWN);
		LinkControlEx(plist, "rrNormalBottom", m_pNormalRoomBottomGui, GCI_UNKNOWN);

		//	클랜전 룸
		LinkControlEx(plist, "rrClanBattleInfo", m_pClanBattleInfoGui, GCI_UNKNOWN);

		LinkControlEx(plist, "rrRedClanMarkImage", m_pClanBattleInfoRedClanMark, GCI_UNKNOWN);
		LinkControlEx(plist, "rrBlueClanMarkImage", m_pClanBattleInfoBlueClanMark, GCI_UNKNOWN);
		LinkControlEx(plist, "rrRedClanNameValue", m_pClanBattleInfoRedClanName, GCI_UNKNOWN);
		LinkControlEx(plist, "rrRedTeamNameValue", m_pClanBattleInfoRedTeamName, GCI_UNKNOWN);
		LinkControlEx(plist, "rrBlueClanNameValue", m_pClanBattleInfoBlueClanName, GCI_UNKNOWN);
		LinkControlEx(plist, "rrBlueTeamNameValue", m_pClanBattleInfoBlueTeamName, GCI_UNKNOWN);

		LinkControlEx(plist, "rrClanBattleModeKey", m_pClanBattleInfoModeKey, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleModeValue", m_pClanBattleInfoModeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleModePrev", m_pClanBattleInfoModePrev, GCI_RRP_B_CLANBATTLE_MODE_PREV);
		LinkControlEx(plist, "rrClanBattleModeNext", m_pClanBattleInfoModeNext, GCI_RRP_B_CLANBATTLE_MODE_NEXT);
		LinkControlEx(plist, "rrClanBattleTypeKey", m_pClanBattleInfoTypeKey, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleTypeValue", m_pClanBattleInfoTypeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleTypePrev", m_pClanBattleInfoTypePrev, GCI_RRP_B_CLANBATTLE_TYPE_PREV);
		LinkControlEx(plist, "rrClanBattleTypeNext", m_pClanBattleInfoTypeNext, GCI_RRP_B_CLANBATTLE_TYPE_NEXT);
		LinkControlEx(plist, "rrClanBattleTimeKey", m_pClanBattleInfoTimeKey, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleTimeValue", m_pClanBattleInfoTimeValue, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleTimePrev", m_pClanBattleInfoTimePrev, GCI_RRP_B_CLANBATTLE_TIME_PREV);
		LinkControlEx(plist, "rrClanBattleTimeNext", m_pClanBattleInfoTimeNext, GCI_RRP_B_CLANBATTLE_TIME_NEXT);
		LinkControlEx(plist, "rrClanBattleOptionKey", m_pClanBattleInfoOptionKey, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleOption", m_pClanBattleInfoOptionButton, GCI_RRP_B_CLANBATTLE_OPTION);

		LinkControlEx(plist, "rrClanBattleBottom", m_pClanBattleBottomGui, GCI_UNKNOWN);
		LinkControlEx(plist, "rrClanBattleChange", m_pClanBattleBottomTeamChange, GCI_RRP_B_CLANBATTLE_TEAM_CHANGE);
	}

	return FALSE;
}

void CPhaseReadyRoom::OnInitControl(void)
{
	_InitCommon();
	_InitSlot();
	_InitInfo();
	_InitChat();
	_InitRecord();
	_InitAdvencedWin();
	_InitInviteWin();

	_InitCardComplateWin();
	_InitFinalComplateWin();

	_InitRankUpWin();

	_InitClanBattle();

	{
		const char* soundFileName[COUNTDOWN_COUNT] =
		{
			"UI_Count_A.wav",
				"UI_Count_B.wav",
				"UI_Count_C.wav",
				"UI_Count_D.wav",
				"UI_Count_E.wav"
		};

		for(INT32 i = 0; i < COUNTDOWN_COUNT; i++)
		{
			m_pCountdownSound[i] = i3SoundPlayInfo::NewObject();

			i3GameResSound* pResSound = (i3GameResSound*)g_pFramework->FindResourceByOnlyName(soundFileName[i]);
			I3ASSERT(pResSound != NULL);

			i3SoundPlayInfo* pSoundInfo = pResSound->getSound();
			pSoundInfo->CopyTo(m_pCountdownSound[i], I3_COPY_INSTANCE);
		}
	}

	CPhaseBase::OnInitControl();
}

void CPhaseReadyRoom::OnPreGuiNotify(void)
{
	CReadyPhaseBase::OnPreGuiNotify();

	m_pRedFocus->SetEnable(FALSE);
	m_pBlueFocus->SetEnable(FALSE);
}

void CPhaseReadyRoom::OnUpdate(REAL32 rDeltaSeconds)
{
	CReadyPhaseBase::OnUpdate(rDeltaSeconds);


	// 진급 포인트 시스템
	// 레벨업 체크
	if (0 < g_pGameContext->GetRankUpPoint())
	{
		_OpenRankUpPopup();
		g_pGameContext->ResetRankUp();
	}

	// 팀밸런스 메세지 출력
	if (g_pGameContext->IsTeamBalanceMessageFlag())
	{
		g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_AUTO_BALANCED"));/*{col:255,120,0,255}팀 밸런스 옵션에 따라 양 팀 인원수를 맞추기 위해 자동으로 조절되었습니다.{/col}*/
		g_pGameContext->RemoveGameFlag( GAMEFLAG_TEAMBALANCEMSG);
	}


	// 방장검색 딜레이 설정
	m_rSearchHostDelay = MAX(m_rSearchHostDelay - rDeltaSeconds, 0.0f);
	m_rTeamChangeDelay = MAX(m_rTeamChangeDelay - rDeltaSeconds, 0.0f);


	// 방장이 되었는지 확인
	_CheckBeHost();

	_ProcessCommon( rDeltaSeconds);


	// 상세정보창 슬라이드처리
	_SlideRecordWindow(rDeltaSeconds);


	// 초대하기 알림팝업 일정시간 유지
	_DelayInviteResponse(rDeltaSeconds);

	// 맵 모드 변경 원형 화살표 애니메이션
	if (m_playCircleAnimation)
	{
		m_circleTerm = m_circleTerm - rDeltaSeconds;

		if (m_circleTerm <= 0.0f)
		{
			m_circleIdx = (m_circleIdx + 1) % 7;
			if (0 == m_circleIdx) m_circleIdx++;

			m_pMapCircle->SetAllShapeEnable(FALSE);
			m_pMapCircle->SetShapeEnable(m_circleIdx, TRUE);
			m_circleTerm = 0.08f;
		}

		m_playCircleAnimation = FALSE;
	}
	else
	{
		m_circleIdx = 0;
		m_pMapCircle->SetAllShapeEnable(FALSE);
		m_pMapCircle->SetShapeEnable(m_circleIdx, TRUE);
	}

	AllUserReady();
	UpdateReadyButton();
	UpdateStartButton();

	//보급 상자 보급창
	ApplyRewardReadyBox();

	if(g_pGameContext->IsAbusing())
		OnExitButtonClick();
}

void CPhaseReadyRoom::OnEntranceStart(void)
{
	I3NOTICE( "CPhaseReadyRoom::OnEntranceStart() Enter\n");
	//20110322하동익 어뷰징 관련 테스트// 막아야함
	//g_pGameContext->SetAbuseInfo(ABUSING_LEVEL_STRONG,20);
	//if(g_pGameContext->IsAbusing())
	//	OnExitButtonClick();


#if defined(LOCALE_CHINA)

#endif
	//g_pSDOA_Link->
#if defined(USE_RNADOMMAP_MODE)
	m_pSelectRandomMap->setCheckBoxState(g_pGameContext->IsRandomMap());
#endif
	CReadyPhaseBase::OnEntranceStart();

	m_pChatBody->SetEntranceStart(this);

	g_pGameContext->RemoveGameFlag( GAMEFLAG_KICKBYHOST);
	g_pGameContext->ResetTouchDownCount();

	_InitSlideRecordWindow();

	m_bInviteResult = FALSE;
	m_bInviteMessage = FALSE;
	g_pGameContext->RemoveGameFlag( GAMEFLAG_MANUALREADY);

	m_transferHostSlotIdx = -1;
	m_nSelectedSlotIdx = -1;
	m_bChagingTeam = FALSE;
	m_nItemIDForCardSetComplete = 0;
	m_nCountForItemIDForCardSetComplete = 0;

	// Event Message
	EventMessage();

	// 클랜 모드일때
	if( g_pGameContext->GetBattleChannel() )
	{
		m_pTeamBalancePersonnel->setInputDisable(TRUE);
		m_pTeamBalanceAbility->setInputDisable(TRUE);
		m_pTeamBalanceNo->setInputDisable(TRUE);
		m_pLimitJoinYes->setInputDisable(TRUE);
		m_pLimitJoinNo->setInputDisable(TRUE);
	}
	else
	{
		m_pTeamBalancePersonnel->setInputDisable(FALSE);
		m_pTeamBalanceAbility->setInputDisable(FALSE);
		m_pTeamBalanceNo->setInputDisable(FALSE);
		m_pLimitJoinYes->setInputDisable(FALSE);
		m_pLimitJoinNo->setInputDisable(FALSE);
	}

	g_pFramework->getChatBox()->setWhisperEnable( TRUE);
	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	//	클랜전 셋팅
#if defined( TEST_CLAN_BATTLE)
	_SetClanBattleRoomControl();

#else
	//	일반 레디룸이냐 클랜전 레디룸이냐에 따라 GUI가 변한다.
	if( g_pFramework->GetClanContext()->InBattleTeam())
	{
		_SetClanBattleRoomControl();

		//	클랜 매치가 성공했으므로 신청 받았던 정보는 모두 제거
		g_pFramework->GetClanContext()->ClearRequestBattleTeam();
	}
	else
	{
		_SetNormalRoomControl();
	}
#endif

	//	레디룸으로 왔을때 필요한 팝업을 띄운다.
	OpenPopupUsingCashItem();


	// 훈련전 여부에 따른 UI 활성화
	BOOL isTrainingMode = g_pGameContext->IsStageMaskTraining();
	{
#ifndef USE_AI_MODE_DIEHARD
#if defined( USE_DUAL_AI_MODE)
		{
			if( isTrainingMode)
			{
				// 방 정보 사본 준비
				UINT8 AIState = g_pGameContext->getAiMode();

				switch( AIState)
				{
				case AI_MODE_STATE_ORIGINAL:
					{
						m_pTrainingEnemyKey->SetEnable( TRUE);
						m_pTrainingEnemyValue->SetEnable( TRUE);
						m_pTrainingEnemyPrev->SetEnable( TRUE);
						m_pTrainingEnemyNext->SetEnable( TRUE);
						m_pTrainingDifficultyKey->SetEnable( TRUE);
						m_pTrainingDifficultyValue->SetEnable( TRUE);
						m_pTrainingDifficultyCombo->SetEnable( TRUE);
						m_pTrainingTimeKey->SetEnable(TRUE);
						m_pTrainingTimeValue->SetEnable(TRUE);
					}
					break;
				case AI_MODE_STATE_DIEHARD:
					{
						m_pTrainingEnemyKey->SetEnable( FALSE);
						m_pTrainingEnemyValue->SetEnable( FALSE);
						m_pTrainingEnemyPrev->SetEnable( FALSE);
						m_pTrainingEnemyNext->SetEnable( FALSE);
						m_pTrainingDifficultyKey->SetEnable( FALSE);
						m_pTrainingDifficultyValue->SetEnable( FALSE);

		#if defined( USE_SELECTION_LEVEL_FOR_AI_MODE_DIEHARD)
						m_pTrainingDifficultyCombo->SetEnable( TRUE);
		#else
						m_pTrainingDifficultyCombo->SetEnable( FALSE);
		#endif
						m_pTrainingTimeKey->SetEnable(TRUE);
						m_pTrainingTimeValue->SetEnable(TRUE);
					}
					break;
				}

				m_pTrainingTimeKey->SetEnable(TRUE);
				m_pTrainingTimeValue->SetEnable(TRUE);
			}
			else
			{
				m_pTrainingTimeKey->SetEnable(isTrainingMode);
				m_pTrainingTimeValue->SetEnable(isTrainingMode);
				m_pTrainingEnemyKey->SetEnable(isTrainingMode);
				m_pTrainingEnemyValue->SetEnable(isTrainingMode);
				m_pTrainingDifficultyKey->SetEnable(isTrainingMode);
				m_pTrainingDifficultyValue->SetEnable(isTrainingMode);
			}
		}
#else
		m_pTrainingTimeKey->SetEnable(isTrainingMode);
		m_pTrainingTimeValue->SetEnable(isTrainingMode);
		m_pTrainingEnemyKey->SetEnable(isTrainingMode);
		m_pTrainingEnemyValue->SetEnable(isTrainingMode);
		m_pTrainingDifficultyKey->SetEnable(isTrainingMode);
		m_pTrainingDifficultyValue->SetEnable(isTrainingMode);
#endif

#else
	
		m_pTrainingTimeKey->SetEnable(isTrainingMode);
		m_pTrainingTimeValue->SetEnable(isTrainingMode);
		m_pTrainingEnemyKey->SetEnable(isTrainingMode);
		m_pTrainingEnemyValue->SetEnable(isTrainingMode);
		m_pTrainingDifficultyKey->SetEnable(isTrainingMode);
		m_pTrainingDifficultyValue->SetEnable(isTrainingMode);

	#if defined( USE_SELECTION_LEVEL_FOR_AI_MODE_DIEHARD)
		m_pTrainingDifficultyCombo->SetEnable( isTrainingMode);
	#endif
#endif

#if defined( USE_DUAL_AI_MODE)
		if( isTrainingMode)
		{
			m_pModeKey->SetEnable(TRUE); // 게임모드
			m_pModeValue->SetEnable(TRUE); // 게임모드
		}
		else
		{
			m_pModeKey->SetEnable(!isTrainingMode);
			m_pModeValue->SetEnable(!isTrainingMode);
		}
#else
		m_pModeKey->SetEnable(!isTrainingMode);
		m_pModeValue->SetEnable(!isTrainingMode);
#endif

		m_pTypeKey->SetEnable(!isTrainingMode);
		m_pTypeValue->SetEnable(!isTrainingMode);
		m_pTimeKey->SetEnable(!isTrainingMode);
		m_pTimeValue->SetEnable(!isTrainingMode);
		m_pOptionKey->SetEnable(!isTrainingMode);
		
		m_pInvite->SetEnable(!isTrainingMode);
		m_pSearchHost->SetEnable(!isTrainingMode);
	}

	// 저격적 여부에 따른 UI 활성화
	BOOL isSniperMode = g_pGameContext->IsSniperRoom();
	if( isSniperMode)
	{
		m_pModeKey->SetEnable(!isSniperMode);
		m_pModeValue->SetEnable(!isSniperMode);
	}

	BOOL isKnuckleMode = g_pGameContext->IsKnuckleRoom();
	if( isKnuckleMode)
	{
		m_pModeKey->SetEnable(!isKnuckleMode);
		m_pModeValue->SetEnable(!isKnuckleMode);
	}
	
	BOOL isEscapeMode = g_pGameContext->IsEscapeRoom();
	if( isEscapeMode)
	{
		m_pModeKey->SetEnable(true);
		m_pModeValue->SetEnable(true);
		m_pModePrev->SetEnable(false);
		m_pModeNext->SetEnable(false);
		m_pTypeKey->SetEnable(false);
		m_pTypeValue->SetEnable(false);
	}
	else
	{
		m_pModeNext->SetEnable(!isEscapeMode);
		m_pModePrev->SetEnable(!isEscapeMode);
	}

#if defined( CHAMPETITION_BUILD )
	m_pModeNext->SetEnable(FALSE);
	m_pModePrev->SetEnable(FALSE);
#endif

	if (isTrainingMode)
	{
		//_InitTrainingSlot();

		m_pTimeKey->setPositionY( m_pOptionKey->getPositionY());

#if defined( USE_DUAL_AI_MODE)
		m_pTrainingTimeKey->setPositionY( m_pOptionKey->getPositionY());
#endif

		REAL32 posX = m_pSearchHost->getPositionX();
		m_pTeamChange->setPositionX(posX);
	}
	else if( isSniperMode)
	{
		m_pTimeKey->setPositionY( m_pTypeKey->getPositionY());
		m_pTeamChange->setPositionX(m_teamChangePosX);
	}
	else if( isKnuckleMode)
	{
		m_pTimeKey->setPositionY( m_pTypeKey->getPositionY());
		m_pTeamChange->setPositionX(m_teamChangePosX);
	}
	else
	{
		m_pTimeKey->setPositionY( m_pOptionKey->getPositionY());

		m_pTeamChange->setPositionX(m_teamChangePosX);
	}

	BOOL	Rv = g_pGameContext->IsGameMaster();
	m_pChkObserver->SetEnable(Rv);
#if defined( DEF_OBSERVER_MODE)
	m_pChkObserver->setCheckBoxState(g_pGameContext->isObserverGM());
#else
	m_pChkObserver->setCheckBoxState(g_pGameContext->IsGM_Observer());
#endif
	m_pEditObserver->SetEnable(Rv);

	I3NOTICE( "CPhaseReadyRoom::OnEntranceStart() Leave\n");
}

BOOL CPhaseReadyRoom::OnEntrancing(RT_REAL32 rDeltaSeconds)
{
	return CPhaseBase::OnEntrancing(rDeltaSeconds);
}

void CPhaseReadyRoom::OnEntranceEnd(void)
{
	CReadyPhaseBase::OnEntranceEnd();

	// 방 입장시 자신이 방장인지 확인한다
	if (_BeHost())
	{
		m_bIsHost = TRUE;
	}
	else
	{
		m_bIsHost = FALSE;
	}

	m_bEnableUpdate = TRUE;

	// 제목, 패스워드 초기 표시
	_UpdateTitlePassword(TRUE);


	// 상세정보 초기화
	m_nOldViewSlot = -1;
	_InitUserInfo();

	m_bChangeOptionNotify = FALSE;

	m_pOption->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
#if defined (USE_RANDOMMAP_MODE)	
	m_pRandomMap->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	if(g_pGameContext->IsRandomMap() && g_pGameContext->GetRandomMapCount()<1)
	{
		//OnRandomMap();
		ROOM_INFO_BASIC Info;

		g_pGameContext->getMyRoomInfo(&Info);
		CReadyPhaseBase::createRandomMap( getStageType( Info._StageID ) );
	}
	//전투후 READYROOM으로 오는것이 아니라면(Lobby에서 오는것이라면)
	if(g_pFramework->GetState() != 7)//BattlePhase에서 Readyroom으로 올때 STATE_FADEIN을 이용
	{
		//Lobby에서 랜덤맵을 체크한경우 초기화 하지 않습니다
		if(!g_pGameContext->IsRandomMap())
			InitRandomCheck();
	}
	
#endif	

	m_pAdvencedWin->SetEnable(FALSE);
		
	m_pRoomWin->setInputDisable(FALSE);

	// 임무 완료 확인
	CheckBattleChallengeComplate();

	//보급 상자 채팅 메세지를 띈운다.
	ApplyMsgReadyBox();

	// Battle에서 에러팝업 후 퇴장 시 처리
	if( g_pFramework->IsMessageBoxEnabled() )
	{
		i3GuiRoot::setGuiDisableInput(TRUE);
	}

	//	배경 캐릭터 업데이트
	UpdateCharacter(VT_DEFAULT);

	//	저격전일 경우 저격 무기를 장착
	g_pFramework->EquipWeaponInGameMode();

	if (g_pCommunity->IsInviteMoving())
	{
		OnInviteMove();
	}

	//	클랜팀에 속해 있지 않으면 클랜 버튼을 활성화 한다.
	if( !g_pFramework->GetClanContext()->InBattleTeam())
	{
		SetPushedStateButton(MBT_CLAN, FALSE);
	}
		

	// 초보/고수 채널이동 팝업
	ChannelType channelType = g_pGameContext->GetConnectedChannelType();
	if (!g_pGameContext->IsValidChannelType(channelType))
	{
		switch(channelType)
		{
		case CHANNEL_TYPE_BEGIN1:
			STRING_POPUP_C(GAME_STRING("STR_TBL_GUI_READY_NOTICE_LEAVE_SERVER_BY_KILL_RATE"), MAKE_CALLBACK(CbMoveChannel), this);/*Kill / Death 비율이 40%가 넘어 서버 선택 화면으로 이동합니다. (K/D 비율 39%까지 입장 가능)*/
			break;
		case CHANNEL_TYPE_BEGIN2:
			STRING_POPUP_C(GAME_STRING("STR_TBL_GUI_READY_NOTICE_WRONG_GRADE_FOR_BEGINNER"), MAKE_CALLBACK(CbMoveChannel), this);/*초보 서버에 입장 가능한 계급과 맞지 않아 서버 선택 화면으로 이동합니다. (상병 계급까지만 입장 가능)*/
			break;
		case CHANNEL_TYPE_EXPERT:
			STRING_POPUP_C(GAME_STRING("STR_TBL_GUI_READY_NOTICE_WRONG_GRADE_FOR_VETERAN"), MAKE_CALLBACK(CbMoveChannel), this);/*고수 서버 입장 조건을 만족시키지 못하여 서버선택 화면으로 이동합니다.*/
			break;
		default:
			break;
		}
	}
}

void CPhaseReadyRoom::OnExitStart(void)
{
	CReadyPhaseBase::OnExitStart();

	if (m_pCardComplateWin->isEnable())
	{
		m_pCardComplateWin->SetEnable(FALSE);
		m_pRoomWin->setInputDisable(FALSE);
		((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
	}

	if (m_pBeHostMessageBox)
	{
		g_pFramework->CloseMessageBox(m_pBeHostMessageBox);
	}

	ContextMenuReset();
}

BOOL CPhaseReadyRoom::OnExiting(RT_REAL32 rDeltaSeconds)
{
	return CPhaseBase::OnExiting(rDeltaSeconds);
}

void CPhaseReadyRoom::OnExitEnd(void)
{
	CReadyPhaseBase::OnExitEnd();

	m_pChatBody->OnExitEnd();

	g_pGameContext->RemoveGameFlag( GAMEFLAG_KICKBYHOST);

	// 다음번 방 입장시, 이전방의 슬롯 정보가 잠깐 출력되는 것을 막기위해
	// 나갈때 슬롯 정보를 초기화한다.
	for(UINT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_slot[i]._pOpen->SetEnable(TRUE);
		m_slot[i]._pClose->SetEnable(FALSE);
		m_slot[i]._pRank->SetEnable(FALSE);
		m_slot[i]._pNickname->SetEnable(FALSE);
		m_slot[i]._pClanRank->SetEnable(FALSE);
		m_slot[i]._pPcCafe->SetAllShapeEnable(FALSE);
		m_slot[i]._pClanMark->SetEnable(FALSE);
		m_slot[i]._pRankBeginner->SetEnable(FALSE);

		m_slot[i]._pESportLeft->SetEnable(FALSE);
		m_slot[i]._pESportLeft->SetAllShapeEnable(FALSE);
		m_slot[i]._pESportLeft->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게

		m_slot[i]._pESportRight->SetEnable(FALSE);
		m_slot[i]._pESportRight->SetAllShapeEnable(FALSE);
		m_slot[i]._pESportRight->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게

		m_slot[i]._pNickname->SetTextColor(GetColor(GCT_DEFAULT));
		m_slot[i]._pClanRank->SetTextColor(GetColor(GCT_DEFAULT));
	}

	g_pFramework->getChatBox()->setWhisperEnable( FALSE);

	m_nItemIDForCardSetComplete = 0;
}

void CPhaseReadyRoom::OnSlideOut(BOOL bRight)
{
	CReadyPhaseBase::OnSlideOut(bRight);

	m_pChatBody->OnExitEnd();

	g_pGameContext->SetCurrentInfoSlot(-1);
	m_nOldViewSlot = -1;
}

void CPhaseReadyRoom::OnSlideIn(BOOL bRight)
{
	CReadyPhaseBase::OnSlideIn(bRight);

	m_pChatBody->SetEntranceStart(this);

	g_pGameContext->SetCurrentInfoSlot(-1);
	m_nOldViewSlot = -1;

	//	저격전일 경우 저격 무기를 장착
	g_pFramework->EquipWeaponInGameMode();

	//	배경 캐릭터 업데이트
	UpdateCharacter(VT_DEFAULT);
}

void CPhaseReadyRoom::OnExitKey(void)
{
	// 레디버튼을 눌러 게임이 시작되면, 나가기 기능을 막는다.
	INT32 slotIdx = g_pGameContext->GetMySlotIdxEx();

	if (-1 != slotIdx)
	{
		SLOT_INFO* info = g_pGameContext->getSlotInfo(slotIdx);

		if (info && SLOT_STATE_READY > info->_State)
		{
			OnExitButtonClick();
		}
	}
}

void CPhaseReadyRoom::OnWheelUpdate(INT32 scrollby)
{
	m_pChatBody->OnWheelUpdate(scrollby);
}

void CPhaseReadyRoom::SetInputDisable(BOOL bDisable)
{
	m_pRoomGui->setInputDisable(bDisable);
}

BOOL CPhaseReadyRoom::GetInputDisable(void)
{
	return m_pRoomGui->getInputDisable();
}

BOOL CPhaseReadyRoom::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		// ContextMenu Event
		if (pObj && pObj->IsExactTypeOf(CContextMenu::GetClassMeta()))
		{
			INT32 slotIdx = param2;

			switch(event)
			{
			case CMC_TRANSFER_HOST	:
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnTransferHost(slotIdx);
				break;
			case CMC_KICK			:
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnKickButton(slotIdx);
				break;
			case CMC_WHISPER		:
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnWhisper(slotIdx);
				break;
			case CMC_ADD_FRIEND		:
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnAddFriend(slotIdx);
				break;

#if defined( USE_COMMUNITY_BLOCK)
			case CMC_ADD_BLOCK		:
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnAddBlock( slotIdx);
				break;
#endif

			case CMC_CLAN_INVITE	:
				if ( ! CanClanInvite() )
					return FALSE;
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnClanInvite(slotIdx);
				break;

			case CMC_VIEW_OTHER_USER_ITEM_INFO:
				PlaySound(GUI_SND_ITEM_SELECTING);
				OnViewOtherUserItemInfo(slotIdx);
				break;

			default:
				return FALSE;
			}

			return TRUE;
		}

		if (I3_EVT_INPUT == event)
		{
			I3ASSERT(pObj != NULL);
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				if (m_pRankUpWin->isEnable())
				{
					OnRankUpConfirm();
					return TRUE;
				}

				if (m_pFinalComplateWin->isEnable())
				{
					OnFinalComplateWinConfirm();
					return TRUE;
				}

				if (m_pFinalComplateExWin->isEnable())
				{
					OnFinalComplateExWinConfirm();
					return TRUE;
				}

				if (m_pFinalComplatePrizeWin->isEnable())
				{
					OnFinalComplatePrizeWinConfirm();
					return TRUE;
				}

				if (m_pCardComplateWin->isEnable())
				{
					OnComplateConfirm();
					return TRUE;
				}

				if (m_pAdvencedWin->isEnable())
				{
					OnAdvencedCancel();
					return TRUE;
				}
				if (m_pInviteWin->isEnable())
				{
					OnInviteCancel();
					return TRUE;
				}
			}

			if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				if (m_pRankUpWin->isEnable())
				{
					OnRankUpConfirm();
					return TRUE;
				}

				if (m_pFinalComplateWin->isEnable())
				{
					OnFinalComplateWinConfirm();
					return TRUE;
				}

				if (m_pFinalComplateExWin->isEnable())
				{
					OnFinalComplateExWinConfirm();
					return TRUE;
				}

				if (m_pFinalComplatePrizeWin->isEnable())
				{
					OnFinalComplatePrizeWinConfirm();
					return TRUE;
				}

				if (m_pCardComplateWin->isEnable())
				{
					OnComplateConfirm();
					return TRUE;
				}

				if (m_pAdvencedWin->isEnable())
				{
					OnAdvencedConfirm();
					return TRUE;
				}
				

				if (m_pInviteWin->isEnable())
				{
					OnInviteConfirm();
					return TRUE;
				}
			}


			if( m_pRoomGui->isEnable() &&
				!((CStageReady*)this->getParent())->IsEnablePopup( POPUP_OPTION) &&
				!((CStageReady*)this->getParent())->IsEnablePopup( POPUP_COMMUNITY))
			{
				// 채팅 타입: 전체, 팀
				m_pChatBody->UpdateChatType();
			
				// 엔터시 포커스 처리
				m_pChatBody->UpdateChatFocus();
			}
		}
	}

	I3ASSERT(pObj != NULL);
	return CReadyPhaseBase::OnEvent(event, pObj, param2, code);
}

BOOL CPhaseReadyRoom::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CReadyPhaseBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;	// 변경사항이 없다.
	}


	// 등장/퇴장 시간에 따른 좌표 오프셋 얻어내기
	m_pRoomGui->setPositionX(CReadyPhaseBase::GetSlideX());


	// 퇴장 완료 시 비활성화. 그외 활성화.
	BOOL enableWindow = TRUE;

	if (SS_OUT == CReadyPhaseBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pRoomGui->SetEnable(enableWindow);

	return TRUE;
}

void CPhaseReadyRoom::OnRecordWindow(void)
{
	m_bRecordWindowSliding = TRUE;
	m_bInRecordWindow = !m_bInRecordWindow;

	if (m_bInRecordWindow)
	{
		m_pRecordCaption->SetText(GAME_STRING("STR_TBL_GUI_READY_DETAIL_INFO1"));/*▼ 상세정보 ▼*/
	}
	else
	{
		m_pRecordCaption->SetText(GAME_STRING("STR_TBL_GUI_READY_DETAIL_INFO2"));/*▲ 상세정보 ▲*/
	}
}


void CPhaseReadyRoom::OnGameStartButtonClick(void)
{
#if defined( I3_DEBUG )  && defined( TEMP_QUEST_REWARD_ENABLE )
	#if defined (TEMP_QUEST_REWARD_ITEM)
		// 카드셋 전체 완료 여부 확인 (개발 확인용. 실제로 DB에서 받지 않습니다.)
		//CCardSet* cardSet = g_pGameContext->getQuestManager()->_getActiveCardSet();
		CCardSet* cardSet = g_pGameContext->getQuestManager()->_getCardSet(MAX_CARDSET_PER_USER-1);

		m_nItemIDForCardSetComplete = g_pQuestTextDictionary->GetCardSetBonusItemID(
			QUEST_CARDSET_TYPE_HUMAN_2ND,	//임무 카드 번호를 적으면 된다.	ENUM_QUEST_CARDSET_TYPE
			0);

		m_nCountForItemIDForCardSetComplete = 0;

		if ( m_nItemIDForCardSetComplete != 0 )
		{
			OnOpenPopupRewardItemForCardSetComplete();
		}

	#else if ( TEMP_QUEST_REWARD_MEDAL )
		//	ENUM_QUEST_CARDSET_TYPE 에 대한 내용으로 미리보기 된다.
		_OpenFinalPrizeWindowUI( 
			QUEST_CARDSET_TYPE_DINO_3RD ////임무 카드 번호를 적으면 된다.	ENUM_QUEST_CARDSET_TYPE
			);
	#endif

	return;
#endif
	//레디 예외 처리
	if (g_pGameContext->IsStageMaskTraining() || g_pGameContext->IsStageMode(STAGE_TYPE_DOMINATION))
	{//훈련전
		if( _BeHost() )
		{
			if(FALSE == _IsReady())
			{
				((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_TRAINIG_TEAM_S"));
				return;
			}
		}
		else
		{
			//1. 방장이 아닐때는 레디를 할때만 체크 합니다.
			if( (g_pGameContext->getSlotInfo(g_pGameContext->getMySlotIdx())->_State) == SLOT_STATE_NORMAL)
			{
				if( (g_pGameContext->getMySlotIdx() % 2) != (g_pGameContext->getMainSlotIdx() % 2) )
				{
					((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_TRAINIG_TEAM_S"));
					return;
				}
			}
		}
	}
	else
	{//방장일때만 예외 처리 합니다.
		if( _BeHost() && (FALSE == _IsReady()) )
		{
			((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_READY_TEAM_S"));
			return;
		}
	}
#if defined (USE_RANDOMMAP_MODE)
	if(g_pGameContext->IsRandomMap()&& _BeHost())
		RunRandomMap();
#endif
	ROOM_INFO_BASIC	roomInfo;
	g_pGameContext->getMyRoomInfo(&roomInfo);
	
	if (STAGE_SLOT_MODE_4VS4 == getStageSlot(roomInfo._StageID) && roomInfo._State != ROOM_STATE_READY)
	{
		INT32 myTeamCount = 0;
		INT32 i = g_pGameContext->getMySlotIdx() % 2;

		while(i < SLOT_MAX_COUNT)
		{
			SLOT_INFO* pSlot = g_pGameContext->getSlotInfo(i);

			if (SLOT_STATE_READY <= pSlot->_State)
			{
				myTeamCount++;
			}

			i += 2;
		}

		if (4 <= myTeamCount)
		{
			((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_NOTICE_LIMIT_ENTER_4VS4"));/*4vs4 모드 인원수 제한으로 난입할 수 없습니다.*/
			return;
		}
	}
	
	if( m_pSelectRandomMap->getCheckBoxState())
		g_pGameContext->AddGameFlag( GAMEFLAG_RANDMAPCHECKED);
	else
		g_pGameContext->RemoveGameFlag( GAMEFLAG_RANDMAPCHECKED);

	g_pGameContext->SetEvent( EVENT_READY_BATTLE );

	return;

}

void CPhaseReadyRoom::OnExitButtonClick(void)
{
	//	클랜전일 경우 질문
#if defined( TEST_CLAN_BATTLE)
	STRING_POPUP_Q(GAME_STRING("STR_TBL_GUI_READY_CONFIRM_TO_EXIT1"), MAKE_CALLBACK(cbQueryLeaveClanBattle), this);/*나가기를 하신 이후에는 다시 팀에 참여할 수 없습니다.\n 그래도 나가시겠습니까?*/
#else
	if( g_pFramework->GetClanContext()->InBattleTeam())
	{
		STRING_POPUP_Q(GAME_STRING("STR_TBL_GUI_READY_CONFIRM_TO_EXIT2"), MAKE_CALLBACK(cbQueryLeaveClanBattle), this);/*나가기를 하신 이후에는 다시 게임에 참여할 수 없습니다.\n 그래도 나가시겠습니까?*/
	}
	else
	{
		g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		i3GuiRoot::setGuiDisableInput(TRUE);

		m_bEnableUpdate = FALSE;
	}
	
	InitRandomCheck();
#endif
}

void CPhaseReadyRoom::cbQueryLeaveClanBattle(void * pThis, INT32 nParam)
{
	//	클랜전에서 나가기 처리
	if( nParam == MB_OK)
	{
		CPhaseReadyRoom * pStage = (CPhaseReadyRoom*) pThis;

		g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		i3GuiRoot::setGuiDisableInput(TRUE);

		pStage->SetUpdateEnable( FALSE);

		((CStageReady*)pStage->getParent())->SetMenuClanInputDisable(FALSE);
	}
}

void CPhaseReadyRoom::CbMoveChannel(void* pThis,INT32 nParam)
{
	// 고수/초보 채널 조건을 만족하지 못하여 채널 이동
	if (nParam == MB_OK)
	{
		g_pGameContext->AddGameFlag( GAMEFLAG_MOVECHANNEL);

		g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		i3GuiRoot::setGuiDisableInput(TRUE);

		CPhaseReadyRoom* pReadyRoom = (CPhaseReadyRoom*)pThis;

		pReadyRoom->SetUpdateEnable(FALSE);
		((CStageReady*)pReadyRoom->getParent())->SetMenuClanInputDisable(FALSE);

		((CPhaseReadyRoom*)pThis)->m_bEnableUpdate = FALSE;
	}
}

void CPhaseReadyRoom::CbBlockUser(void* pThis, INT32 nParam)
{
	if (nParam == MB_OK)
	{
		//CPhaseReadyRoom* pReadyRoom = (CPhaseReadyRoom*)pThis;
	}
}

void CPhaseReadyRoom::OnPasswordChangeButtonClick(void)
{
	if (0 != memcmp(g_pGameContext->getMyRoomPW(), m_pPasswordValue->getIMETextBuf(), sizeof(char) * NET_ROOM_PW))
	{
		i3mem::FillZero(g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
		i3mem::Copy( g_pGameContext->getMyRoomPW(), m_pPasswordValue->getIMETextBuf(), sizeof(char)*i3String::Length(m_pPasswordValue->getIMETextBuf()));
		g_pGameContext->SetEvent(EVENT_CHANGE_ROOM_PASSWORD_N);
	}
}

void CPhaseReadyRoom::OnTitleChangeButtonClick(void)
{
	char szTemp[MAX_STRING_COUNT];
	i3String::Copy( szTemp, m_pTitleValue->getIMETextBuf(), MAX_STRING_COUNT );
	i3String::RemoveAllBlank( szTemp );
	if( i3String::Length( szTemp ) <= 0) {
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_PLEASE_ENTER_ROOM_NAME"));/*방 제목을 입력하여 주십시오*/
		return;
	}
#if defined(LOCALE_CHINA)
	CStringFilter* pStringFilter = CStringFilter::NewObject();
	CStringFilter* pStringFilter2 = CStringFilter::NewObject();
	pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);
	if( FALSE == g_pFramework->IsFilter( m_pTitleValue->getIMETextBuf() ) || FALSE == pStringFilter2->IsFilter( szTemp ) || FALSE == pStringFilter->IsFilterSame( szTemp ) ||
		i3String::ContainNoCase(szTemp, "{col:") >= 0)
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
		I3_SAFE_RELEASE( pStringFilter2 );
		I3_SAFE_RELEASE( pStringFilter );
		return;
	}
	I3_SAFE_RELEASE( pStringFilter2 );
	I3_SAFE_RELEASE( pStringFilter );
#else

	#if defined( LOCALE_NORTHAMERICA )
		if( FALSE == g_pFramework->IsFilter( m_pTitleValue->getIMETextBuf() ) 
			|| FALSE == g_pFramework->IsFilter2( m_pTitleValue->getIMETextBuf() )
			|| i3String::ContainNoCase(szTemp, "{col:") >= 0)
	#else
  		if( FALSE == g_pFramework->IsFilter( m_pTitleValue->getIMETextBuf() ) || i3String::ContainNoCase(szTemp, "{col:") >= 0)
	#endif
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
			return;
		}

#endif
	// 방 정보 사본 준비
	ROOM_INFO_BASIC infoBasic;
	g_pGameContext->getMyRoomInfo(&infoBasic);

	// 방제목이 변경되었다면 요청
	{
		// ! praptor - IME에 버그 있습니다. 초기화시 넘겨준 최대길이보다 큰 문자열이
		// 넘어 올 수 있습니다. 문자열복사시 반드시 확인해야합니다.

		// IME에 의해 최대길이보다 큰 문자열이 들어왔을 경우 잘라낸다.
		char strTitle[MAX_STRING_COUNT];
		i3String::Copy( strTitle, m_pTitleValue->getIMETextBuf(), MAX_STRING_COUNT );

		UINT32 titleLength = i3String::Length(strTitle);

		if (NET_ROOM_NAME_SIZE <= titleLength)
		{
			for(INT32 i = NET_ROOM_NAME_SIZE - 1; i >= 0; i--)
			{
				if (2 == i3String::WhatIsCode(strTitle, i))
				{
					strTitle[i] = '\0';
				}
				else
				{
					strTitle[i] = '\0';
					break;
				}
			}
		}

		if (0 != i3String::CompareCase(infoBasic._Title, strTitle))
		{
			i3String::Copy(infoBasic._Title, strTitle, NET_ROOM_NAME_SIZE );
			g_pGameContext->SetEvent(EVENT_CHANGE_ROOMINFO_N, &infoBasic );
		}
	}
}

void CPhaseReadyRoom::OnModePrevButtonClick(void)
{
#if defined( USE_DUAL_AI_MODE)
	if (g_pGameContext->IsStageMaskTraining())
	{
		// 방 정보 사본 준비
		ROOM_INFO_ALL info;

		g_pGameContext->getMyRoomInfo(&info._Basic);
		g_pGameContext->getMyRoomInfoAdd(&info._Add);

		m_AiModeState = (AI_MODE_STATE)info._Add._AIState;

		if( m_AiModeState == AI_MODE_STATE_ORIGINAL) m_AiModeState = AI_MODE_STATE_DIEHARD;
		else m_AiModeState = AI_MODE_STATE_ORIGINAL;

		switch( m_AiModeState)
		{
		case AI_MODE_STATE_ORIGINAL:
			{
				info._Add._AIState = AI_MODE_STATE_ORIGINAL;
				info._Basic._StageID = getFirstChallengeStage();
			}
			break;
		case AI_MODE_STATE_DIEHARD:
			{
				info._Add._AIState = AI_MODE_STATE_DIEHARD;
				info._Basic._StageID = getFirstChallengeDieHardStage();

				// DieHard모드에서는 레벨과 AI캐릭터 수를 고정합니다.
				g_pGameContext->setDifficultyLevel(10);
				g_pGameContext->setEnemyAiCount(8);
			}
			break;
		}

		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &info );
	}
	else
#endif
	{
		_RotateModeIndex(FALSE);
	}
}

void CPhaseReadyRoom::OnModeNextButtonClick(void)
{
#if defined( USE_DUAL_AI_MODE)
	if (g_pGameContext->IsStageMaskTraining())
	{
		// 방 정보 사본 준비
		ROOM_INFO_ALL info;

		g_pGameContext->getMyRoomInfo(&info._Basic);
		g_pGameContext->getMyRoomInfoAdd(&info._Add);

		m_AiModeState = (AI_MODE_STATE)info._Add._AIState;

		if( m_AiModeState == AI_MODE_STATE_ORIGINAL) m_AiModeState = AI_MODE_STATE_DIEHARD;
		else m_AiModeState = AI_MODE_STATE_ORIGINAL;

		switch( m_AiModeState)
		{
		case AI_MODE_STATE_ORIGINAL:
			{
				info._Add._AIState = AI_MODE_STATE_ORIGINAL;
				info._Basic._StageID = getFirstChallengeStage();
			}
			break;
		case AI_MODE_STATE_DIEHARD:
			{
				info._Add._AIState = AI_MODE_STATE_DIEHARD;

				// DieHard모드에서는 레벨과 AI캐릭터 수를 고정합니다.
				g_pGameContext->setDifficultyLevel(10);
				g_pGameContext->setEnemyAiCount(8);

				info._Basic._StageID = getFirstChallengeDieHardStage();
			}
			break;
		}

		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &info );
	}
	else
#endif
	{
		_RotateModeIndex(TRUE);
	}
}

void CPhaseReadyRoom::OnVictoryPrevButtonClick(void)
{
	_RotateTypeIndex(FALSE);
}

void CPhaseReadyRoom::OnVictoryNextButtonClick(void)
{
	_RotateTypeIndex(TRUE);
}

void CPhaseReadyRoom::OnVictory2PrevButtonClick(void)
{
	_RotateTimeIndex(FALSE);
}

void CPhaseReadyRoom::OnVictory2NextButtonClick(void)
{
	_RotateTimeIndex(TRUE);
}

void CPhaseReadyRoom::OnInviteListButtonClick(void)
{
	OnAdvencedCancel();		// 고급옵션 창과는 배타적으로 활성화되야한다.

	for(UINT32 i = 0; i < MAX_INVITE_COUNT; i++)
	{
		m_inviteSlot[i]._pCheck->setCheckBoxState(FALSE);
		m_inviteSlot[i]._pCheck->SetEnable(FALSE);
		m_inviteSlot[i]._pRank->SetEnable(FALSE);
		m_inviteSlot[i]._pRankBeginner->SetAllShapeEnable(FALSE);
		m_inviteSlot[i]._pNickname->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_inviteSlot[i]._pNickname->SetEnable(FALSE);
	}

	// 대기 메시지
	m_pWaitInviteMB = STRING_POPUP_W(GAME_STRING("STR_TBL_GUI_READY_GET_USERLIST_FROM_LOBBY1"));/*대기실에 위치한 사용자들의 목록을 받고 *//*있습니다. 잠시만 기다려 주십시오.*/

	// 서버에 요청
	g_pGameContext->SetEvent(EVENT_GET_LOBBY_USER);
}

void CPhaseReadyRoom::OnTransferHost(UINT32 idxSlot)
{	
	g_pGameContext->SetEvent(EVENT_TRANSFER_HOST, &idxSlot);
}

void CPhaseReadyRoom::OnKickButton(UINT32 idxSlot)
{
	// 훈련전 시 특정 슬롯이상은 동작 하지 않도록 합니다.
	if (g_pGameContext->IsStageMaskTraining())
	{
		if (8 <= idxSlot)
		{
			return;
		}
	}

	INT32 slotFlag = 0x00000000;
	SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(idxSlot);

	//	클랜전이 아닐 경우만 가능하도록
#if defined( TEST_CLAN_BATTLE)
	if(idxSlot >= 0)
		return;
#else
	if( g_pFramework->GetClanContext()->InBattleTeam())
		return;
#endif

#if defined( BUILD_RELEASE_QA_VERSION ) && defined( TEMP_QUEST_REWARD)
	// TODO: 4대4 모드에서는 아래 8개 슬롯을 닫아야하나, 일반모드에서 입장한
	// 사용자가 아래 슬롯을 차지하고 있을 경우 동작이 정의되어 있지 않다.
	// 명세가 나올때까지 동작을 막는다.

	// 훈련전 & 4vs4이면 뒤쪽 8개 슬롯은 작동 안함
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();

	if (STAGE_SLOT_MODE_4VS4 == getStageSlot(StageID) || g_pGameContext->IsStageMaskTraining())
	{
		if (8 <= idxSlot)
		{
			return;
		}
	}
#endif

	// 닫혀있으면 오픈 플레그 설정
	if (SLOT_STATE_CLOSE == pInfo->_State)
	{
		slotFlag = 0x10000000;
	}

	slotFlag |= idxSlot;
	g_pGameContext->SetEvent(EVENT_CHANGE_SLOT_N, &slotFlag);
}

void CPhaseReadyRoom::OnMapChange(void)
{
	ROOM_INFO_ALL info;

	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);

	if (STAGE_TYPE_DEATHMATCH == getStageType(info._Basic._StageID))
	{
		info._Basic._StageID = setStageType(info._Basic._StageID, STAGE_TYPE_ANNIHILATION);
	}
	else if (STAGE_TYPE_ANNIHILATION == getStageType(info._Basic._StageID))
	{
		info._Basic._StageID = setStageType(info._Basic._StageID, STAGE_TYPE_DEATHMATCH);
	}
	else if (STAGE_TYPE_HEAD_HUNTER == getStageType(info._Basic._StageID))
	{
		info._Basic._StageID = setStageType(info._Basic._StageID, STAGE_TYPE_HEAD_KILLER);
	}
	else if (STAGE_TYPE_HEAD_KILLER == getStageType(info._Basic._StageID))
	{
		info._Basic._StageID = setStageType(info._Basic._StageID, STAGE_TYPE_HEAD_HUNTER);
	}
	else
	{
		return;
	}

	_UpdateMapInfo( &info);
}

void CPhaseReadyRoom::OnMapCombo(void)
{
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();

	CReadyPhaseBase::InitEnableMapList(getStageType(StageID), m_pMapList);

	ExclusionPopup(m_pMapList);
}

void CPhaseReadyRoom::OnMapPopup(UINT32 idxSlot)
{
	ROOM_INFO_ALL	info;
	STAGE_TYPE		stageType;
	UINT32			nStageID;

	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);

	// 스테이지 ID가 데스매치일 경우 스테이지 타입을 현재와 동일하게 맞춰줘야 된다
	// 스테이지 타입이 섬멸전이나 폭파등 다를 수도 있기 때문에
	nStageID		= getStageId( m_EnableStageList[ idxSlot ].second );
	stageType		= getStageType( nStageID );

	if( stageType == STAGE_TYPE_DEATHMATCH )
	{
		nStageID	= setStageType( nStageID, getStageType( info._Basic._StageID ) );
	}

	info._Basic._StageID	= nStageID;

	_UpdateMapInfo( &info);
}

void CPhaseReadyRoom::OnRedListFocus(UINT32 idxRedSlot)
{
	if (FALSE == m_bChagingTeam)
	{
		SLOT_INFO* pSlotInfo = g_pGameContext->getSlotInfo(idxRedSlot * 2);

		if (SLOT_STATE_EMPTY != pSlotInfo->_State && SLOT_STATE_CLOSE != pSlotInfo->_State)
		{
			m_pRedFocus->setPositionY(idxRedSlot * 24.0f + 10.0f);
			m_pRedFocus->SetEnable(TRUE);
		}
	}
}

void CPhaseReadyRoom::OnRedListSelect(UINT32 idxRedSlot)
{
	_SlotClick(idxRedSlot * 2);

	if (m_slot[idxRedSlot * 2]._pOpen->isEnable())
	{
		SLOT_INFO* pSlotInfo = g_pGameContext->getSlotInfo(idxRedSlot * 2);

		if (SLOT_STATE_EMPTY != pSlotInfo->_State && SLOT_STATE_CLOSE != pSlotInfo->_State)
		{
			m_pBlueSelect->SetEnable(FALSE);

			m_pRedSelect->SetEnable(TRUE);
			m_pRedSelect->setPositionY(idxRedSlot * 24.0f + 10.0f);

			PlaySound(GUI_SND_ITEM_SELECTING);
		}
	}
}

void CPhaseReadyRoom::OnRedListContext(UINT32 idxRedSlot)
{
	if (m_slot[idxRedSlot * 2]._pOpen->isEnable())
	{
		SLOT_INFO* pSlotInfo = g_pGameContext->getSlotInfo(idxRedSlot * 2);

		if (SLOT_STATE_EMPTY != pSlotInfo->_State && SLOT_STATE_CLOSE != pSlotInfo->_State)
		{
			OnRedListSelect(idxRedSlot);

			_OpenContextMenu(idxRedSlot * 2);
		}
	}
}

void CPhaseReadyRoom::OnBlueListFocus(UINT32 idxBlueSlot)
{
	if (FALSE == m_bChagingTeam)
	{
		SLOT_INFO* pSlotInfo = g_pGameContext->getSlotInfo((idxBlueSlot * 2) + 1);

		if (SLOT_STATE_EMPTY != pSlotInfo->_State && SLOT_STATE_CLOSE != pSlotInfo->_State)
		{
			m_pBlueFocus->setPositionY(idxBlueSlot * 24.0f + 10.0f);
			m_pBlueFocus->SetEnable(TRUE);
		}
	}
}

void CPhaseReadyRoom::OnBlueListSelect(UINT32 idxBlueSlot)
{
	_SlotClick((idxBlueSlot * 2) + 1);

	if (m_slot[(idxBlueSlot * 2) + 1]._pOpen->isEnable())
	{
		SLOT_INFO* pSlotInfo = g_pGameContext->getSlotInfo((idxBlueSlot * 2) + 1);

		if (SLOT_STATE_EMPTY != pSlotInfo->_State && SLOT_STATE_CLOSE != pSlotInfo->_State)
		{
			m_pRedSelect->SetEnable(FALSE);

			m_pBlueSelect->SetEnable(TRUE);
			m_pBlueSelect->setPositionY(idxBlueSlot * 24.0f + 10.0f);

			PlaySound(GUI_SND_ITEM_SELECTING);
		}
	}
}

void CPhaseReadyRoom::OnBlueListContext(UINT32 idxBlueSlot)
{
	if (m_slot[(idxBlueSlot * 2) + 1]._pOpen->isEnable())
	{
		SLOT_INFO* pSlotInfo = g_pGameContext->getSlotInfo((idxBlueSlot * 2) + 1);

		if (SLOT_STATE_EMPTY != pSlotInfo->_State && SLOT_STATE_CLOSE != pSlotInfo->_State)
		{
			OnBlueListSelect(idxBlueSlot);

			_OpenContextMenu(idxBlueSlot * 2 + 1);
		}
	}
}

void CPhaseReadyRoom::OnChatCombo(void)
{
	ExclusionPopup(m_pChatBody->m_pChatPopup);
}

void CPhaseReadyRoom::OnChatPopup(UINT32 idxItem)
{
	m_pChatBody->OnChatPopup(idxItem);
}

void CPhaseReadyRoom::OnOption(void)
{
	OnInviteCancel();		// 초대하기 창과는 배타적으로 활성화되야한다.

	m_bChangeOptionNotify = FALSE;
	m_pOption->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);

	m_pLimitMelee->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	ROOM_INFO_BASIC	roomInfo;
	ROOM_INFO_ADD	roomAddInfo;
	g_pGameContext->getMyRoomInfo(&roomInfo);
	g_pGameContext->getMyRoomInfoAdd(&roomAddInfo);


	m_pLimitSecondary->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pLimitThrow->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	if (GET_ROOM_BASIC_WEAPON_SNIPER( roomInfo._WeaponFlag ))
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitPrimary->setCheckBoxState(TRUE);
		m_pLimitPrimaryLock->SetEnable( TRUE);
	}
	else if(GET_ROOM_BASIC_WEAPON_SHOTGUN( roomInfo._WeaponFlag ))
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitPrimary->setCheckBoxState(TRUE);
		m_pLimitPrimaryLock->SetEnable( TRUE);
	}
	else if(GET_ROOM_BASIC_WEAPON_KNUCKLE( roomInfo._WeaponFlag ))
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitPrimaryKey->SetTextColor(GetColor(GCT_INVALID_C));
		m_pLimitSecondary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitSecondaryKey->SetTextColor(GetColor(GCT_INVALID_C));
		m_pLimitThrow->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitThrowKey->SetTextColor(GetColor(GCT_INVALID_C));
	}
	else
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pLimitPrimaryLock->SetEnable( FALSE);
	}

	if( g_pGameContext->IsBossMissionMode() ||
		g_pGameContext->IsDefenceMissionMode() ||	// 방어전에서는 밸런스 문제로 칼전, 권총전 불가능하도록 변경
		g_pGameContext->IsDestroyMissionMode() )	
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);

		m_pLimitPrimary->setCheckBoxState( TRUE);
		m_pLimitPrimaryLock->SetEnable( TRUE);
	}
	else
	{
		if( !GET_ROOM_BASIC_WEAPON_SNIPER( roomInfo._WeaponFlag))	{
			if( !GET_ROOM_BASIC_WEAPON_SHOTGUN( roomInfo._WeaponFlag))	{
				m_pLimitPrimaryLock->SetEnable( FALSE);
			}
		}
	}


	if(_BeHost())
	{
		// RPG7 모드 체크 (파괴미션에서는 불가)
		if( g_pConfigEx->GetGame().Enable_RPG7 &&
			getStageType( roomInfo._StageID) != STAGE_TYPE_DESTROY &&
			getStageType( roomInfo._StageID) != STAGE_TYPE_DEFENCE &&
			(g_pConfigEx->GetGame().UnlimitedForRPG7 ||	getStageSlot( roomInfo._StageID) == STAGE_SLOT_MODE_4VS4) )
		{
			m_pLimitRPG7Key->SetTextColor(GetColor(GCT_DEFAULT));

			if (GET_ROOM_BASIC_WEAPON_SNIPER( roomInfo._WeaponFlag ))				//	저격전 모드
			{
				m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			}
			else if (GET_ROOM_BASIC_WEAPON_SHOTGUN( roomInfo._WeaponFlag ))		//	샷건전 모드
			{
				m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			}
			else
			{
				m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			}
		}
		else
		{
			m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID_C));
			m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		}

		m_pLimitObserver3PV_Free->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pLimitObserver1PV_Free->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pLimitObserverEnemy_Free->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pLimitObserverEnemyHP->setControlState(I3GUI_CONTROL_STATE_NORMAL);

		// 클랜 배틀 서버 땜빵 코드입니다 ( 20080422 김현우 )
		if( g_pGameContext->GetBattleChannel() || g_pFramework->GetClanContext()->InBattleTeam())
		{
			m_pTeamBalancePersonnel->setInputDisable(TRUE);
			m_pTeamBalanceAbility->setInputDisable(TRUE);
			m_pTeamBalanceNo->setInputDisable(TRUE);
			m_pLimitJoinYes->setInputDisable(TRUE);
			m_pLimitJoinNo->setInputDisable(TRUE);

			m_pLimitJoinYes->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pLimitJoinNo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTeamBalancePersonnel->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTeamBalanceAbility->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTeamBalanceNo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		}
		else
		{
			m_pTeamBalancePersonnel->setInputDisable(FALSE);
			m_pTeamBalanceAbility->setInputDisable(FALSE);
			m_pTeamBalanceNo->setInputDisable(FALSE);
			m_pLimitJoinYes->setInputDisable(FALSE);
			m_pLimitJoinNo->setInputDisable(FALSE);

			m_pLimitJoinYes->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pLimitJoinNo->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTeamBalancePersonnel->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTeamBalanceAbility->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTeamBalanceNo->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		}
	}
	else
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitSecondary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitThrow->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);

		// RPG7 모드 체크 (파괴미션에서는 불가)
		UINT32 StageID = roomInfo._StageID;
		if( getStageSlot(StageID) != STAGE_SLOT_MODE_4VS4)
		{
			if( g_pConfigEx->GetGame().Enable_RPG7 == FALSE ||
				getStageType( StageID) == STAGE_TYPE_DESTROY ||
				getStageType( StageID) == STAGE_TYPE_DEFENCE ||
				(g_pConfigEx->GetGame().Enable_RPG7 == TRUE && g_pConfigEx->GetGame().UnlimitedForRPG7 == FALSE))
			{
				m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID_C));
			}
		}

		m_pLimitJoinYes->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitJoinNo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitObserver3PV_Free->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitObserver1PV_Free->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitObserverEnemy_Free->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitObserverEnemyHP->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTeamBalancePersonnel->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTeamBalanceAbility->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTeamBalanceNo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	}

	if( g_pGameContext->IsSniperRoom() || g_pGameContext->IsShotGunRoom()  || g_pGameContext->IsKnuckleRoom()) {
		m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID_C));
	}


	// 무기 제한
	m_pLimitPrimary->setCheckBoxState( TESTBIT(roomInfo._WeaponFlag,		LOCK_PRIMARY));
	m_pLimitSecondary->setCheckBoxState( TESTBIT(roomInfo._WeaponFlag,	LOCK_SECONDARY));
	m_pLimitMeleeLock->SetEnable(TRUE);
	
	if( TESTBIT(roomInfo._WeaponFlag, LOCK_MELEE) )
	{
		m_pLimitMelee->setCheckBoxState( TESTBIT(roomInfo._WeaponFlag,		LOCK_MELEE));
	}
	else
	{
		m_pLimitMelee->setCheckBoxState( TESTBIT(roomInfo._WeaponFlag,		LOCK_KNUCKLE));
	}

	m_pLimitThrow->setCheckBoxState( TESTBIT(roomInfo._WeaponFlag,		LOCK_THROW));
	m_pLimitRPG7->setCheckBoxState( TESTBIT(roomInfo._WeaponFlag,			LOCK_RPG7));

	// 입장 제한
	OnJoinLimitCheck(1 == roomAddInfo._LockInterEnter);

	// 관전 기능
	BOOL val;
	val = TESTBIT(roomAddInfo._LockObserver, LOCK_OBSERVER_CHASE) != 0;
	m_pLimitObserver3PV_Free->setCheckBoxState(val);

	val = TESTBIT(roomAddInfo._LockObserver, LOCK_OBSERVER_FREE) != 0;
	m_pLimitObserver1PV_Free->setCheckBoxState( val);

	val = TESTBIT(roomAddInfo._LockObserver, LOCK_OBSERVER_ENEMY) != 0;
	m_pLimitObserverEnemy_Free->setCheckBoxState( val);

	val = TESTBIT(roomAddInfo._LockObserver, LOCK_OBSERVER_ENEMY_HP) != 0;
	m_pLimitObserverEnemyHP->setCheckBoxState( val);


	// 팀밸런스
	OnTeamBalanceCheck((INT32)roomAddInfo._LockTeamBalance);

	if ( g_pConfigEx->GetGame().Enable_RPG7)
	{
		m_pLimitRPG7Key->SetEnable(TRUE);
		m_pLimitRPG7->SetEnable(TRUE);
		m_pLimitRPG7->setInputDisable(FALSE);
	}
	else
	{
		m_pLimitRPG7Key->SetEnable(FALSE);
		m_pLimitRPG7->SetEnable(FALSE);
		m_pLimitRPG7->setInputDisable(TRUE);
	}

	m_pAdvencedWin->SetEnable(TRUE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);
	m_pRoomWin->setInputDisable(TRUE);
}


void CPhaseReadyRoom::RunRandomMap(void)
{
	if(!_BeHost() )	return;

	INT32	StageIndex = g_RandomMap.RandomizeMapIndex();
	if( StageIndex < 0 ) return;

	ROOM_INFO_ALL info;
	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);

	STAGE_ID stageId =	getStageId((STAGE_ORDINAL_ID)StageIndex);
	STAGE_TYPE type = getStageType(info._Basic._StageID);
	info._Basic._StageID = setStageType(stageId, type);

	_UpdateMapInfo( &info);
}

void CPhaseReadyRoom::OnTrainingTimePrev(void)
{
	_RotateTrainingTime();
}

void CPhaseReadyRoom::OnTrainingTimeNext(void)
{
	_RotateTrainingTime();
}

void CPhaseReadyRoom::OnTrainingEnemyPrev(void)
{
	_RotateTrainingEnemy(FALSE);
}

void CPhaseReadyRoom::OnTrainingEnemyNext(void)
{
	_RotateTrainingEnemy(TRUE);
}

void CPhaseReadyRoom::OnTrainingDifficultyCombo(void)
{
	ExclusionPopup(m_pTrainingDifficultyList);
}

void CPhaseReadyRoom::OnTrainingDifficultyPopup(UINT32 idxSlot)
{
	// 사본
	// 사본 준비
	ROOM_INFO_BASIC infoBasic;
	g_pGameContext->getMyRoomInfo(&infoBasic);

	// 계산
	INT32 difficultyLevel = idxSlot+1;

	// 적용
	g_pGameContext->setDifficultyLevel(difficultyLevel);
	g_pGameContext->setCurrentDifficultyLevel(difficultyLevel);
	g_pGameContext->SetEvent( EVENT_CHANGE_ROOMINFO_N, &infoBasic );
}

void CPhaseReadyRoom::OnRandomMap(void)
{
	if(!_BeHost() || !g_pGameContext->IsRandomMap())
		return;

	UINT32 StageID = g_pGameContext->GetMyRoomStageID();
	STAGE_TYPE	GameMode = getStageType(StageID);
	((CStageReady*)m_pParent)->TogglePopup(POPUP_RANDOM_MAP, reinterpret_cast<void*>(GameMode));
}
void CPhaseReadyRoom::OnCheckRandomMap()
{
	if(!_BeHost())
		return;
	ROOM_INFO_BASIC	roomInfo;
	g_pGameContext->getMyRoomInfo(&roomInfo);

	roomInfo._InfoFlag = SET_ROOM_INFO_RANDOM_MODE( roomInfo._InfoFlag, m_pSelectRandomMap->getCheckBoxState() );

	if(g_pGameContext->GetRandomMapCount()<1)
	{
		CReadyPhaseBase::createRandomMap( getStageType( roomInfo._StageID ) );
	}

	g_pGameContext->setMyRoomInfo(&roomInfo);
	g_pGameContext->SetEvent(EVENT_CHANGE_ROOMINFO_N, &roomInfo);

}
void CPhaseReadyRoom::OnAdvencedConfirm(void)
{
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
	m_pRoomWin->setInputDisable(FALSE);

	m_pAdvencedWin->SetEnable(FALSE);

	if(!_BeHost())return;

	//방장일때만 합니다.
	ROOM_INFO_BASIC InfoBasic, OldInfo;
	ROOM_INFO_ADD	infoAdd, OldInfoAdd;
	g_pGameContext->getMyRoomInfo(&InfoBasic);
	g_pGameContext->getMyRoomInfo(&OldInfo);
	g_pGameContext->getMyRoomInfoAdd(&infoAdd);
	g_pGameContext->getMyRoomInfoAdd(&OldInfoAdd);

	BOOL bLockSniper =		(BOOL) TESTBIT( InfoBasic._WeaponFlag,	LOCK_SNIPER);
	BOOL bLockShotGun =		(BOOL) TESTBIT( InfoBasic._WeaponFlag,	LOCK_SHOTGUN);
	BOOL bLockKnuckle =		(BOOL) TESTBIT( InfoBasic._WeaponFlag,	LOCK_KNUCKLE);

	// 무기 제한 세팅 기본 초기화로 만들어 준다.
	InfoBasic._WeaponFlag = 0;

	if(m_pLimitPrimary->getCheckBoxState())
		SETBIT( InfoBasic._WeaponFlag, LOCK_PRIMARY);
	else
		UNSETBIT(InfoBasic._WeaponFlag, LOCK_PRIMARY); 

	if(m_pLimitSecondary->getCheckBoxState())
		SETBIT( InfoBasic._WeaponFlag, LOCK_SECONDARY);
	else
		UNSETBIT(InfoBasic._WeaponFlag, LOCK_SECONDARY); 

	if(m_pLimitRPG7->getCheckBoxState())
		SETBIT( InfoBasic._WeaponFlag, LOCK_RPG7);
	else
		UNSETBIT( InfoBasic._WeaponFlag, LOCK_RPG7);

	if(m_pLimitThrow->getCheckBoxState())
		SETBIT( InfoBasic._WeaponFlag, LOCK_THROW);
	else
		UNSETBIT( InfoBasic._WeaponFlag, LOCK_THROW);

	if(bLockSniper)
		SETBIT( InfoBasic._WeaponFlag, LOCK_SNIPER);
	else
		UNSETBIT(InfoBasic._WeaponFlag, LOCK_SNIPER);

	if(bLockShotGun)
		SETBIT( InfoBasic._WeaponFlag, LOCK_SHOTGUN);
	else
		UNSETBIT(InfoBasic._WeaponFlag, LOCK_SHOTGUN);

	SETBIT( InfoBasic._WeaponFlag, LOCK_MELEE);

	if(bLockKnuckle)
	{
		SETBIT( InfoBasic._WeaponFlag, LOCK_KNUCKLE);
		UNSETBIT(InfoBasic._WeaponFlag, LOCK_MELEE);
	}


	// 무기 제한
	BOOL bLockPrimary =		(BOOL) TESTBIT( InfoBasic._WeaponFlag,	LOCK_PRIMARY);
	BOOL bLockSecondary =	(BOOL) TESTBIT( InfoBasic._WeaponFlag,	LOCK_SECONDARY);
	BOOL bLockRPG7 =		(BOOL) TESTBIT( InfoBasic._WeaponFlag,	LOCK_RPG7);

	//	칼전/권총전 여부
	UNSET_ROOM_BATTLE_MARK( m_pBattleMark );
	if(bLockSniper)
	{
		SET_ROOM_BATTLE_MARK_SNIPER(m_pBattleMark);
	}
	else if(bLockShotGun) {
		SET_ROOM_BATTLE_MARK_SHOTGUN(m_pBattleMark);
	}
	else if( !bLockPrimary)		//	주무기 lock
	{
		if( !bLockSecondary)	//	보조무기 lock
		{
			if( bLockRPG7)
			{
				SET_ROOM_BATTLE_MARK_RPG7( m_pBattleMark);
			}
			else if( bLockKnuckle )	//너클전
			{
				SET_ROOM_BATTLE_MARK_KNUCKLE( m_pBattleMark);
			}
			else	//칼전
			{
				SET_ROOM_BATTLE_MARK_KNIFE( m_pBattleMark);
			}
		}
		else if(bLockRPG7)									//	-------------	RPG7전
		{
			SET_ROOM_BATTLE_MARK_RPG7( m_pBattleMark);
		}
		else												//	-------------	권총전
		{
			SET_ROOM_BATTLE_MARK_HANDGUN( m_pBattleMark);
		}
	}

	// 관전기능
	if(m_pLimitObserver3PV_Free->getCheckBoxState())	SETBIT( infoAdd._LockObserver, LOCK_OBSERVER_CHASE);
	else												UNSETBIT(infoAdd._LockObserver, LOCK_OBSERVER_CHASE);

	if(m_pLimitObserver1PV_Free->getCheckBoxState())	SETBIT( infoAdd._LockObserver, LOCK_OBSERVER_FREE);
	else												UNSETBIT(infoAdd._LockObserver, LOCK_OBSERVER_FREE);

	if(m_pLimitObserverEnemy_Free->getCheckBoxState())	SETBIT( infoAdd._LockObserver, LOCK_OBSERVER_ENEMY);
	else												UNSETBIT(infoAdd._LockObserver, LOCK_OBSERVER_ENEMY);

	if(m_pLimitObserverEnemyHP->getCheckBoxState())		SETBIT( infoAdd._LockObserver, LOCK_OBSERVER_ENEMY_HP);
	else												UNSETBIT(infoAdd._LockObserver, LOCK_OBSERVER_ENEMY_HP);

	// 난입제한
	infoAdd._LockInterEnter = m_pLimitJoinYes->getCheckBoxState() ? 1 : 0;

	// 팀밸런스
	if(m_pTeamBalancePersonnel->getCheckBoxState() )
		infoAdd._LockTeamBalance = 0x01;
	else if(m_pTeamBalanceAbility->getCheckBoxState())
		infoAdd._LockTeamBalance = 0x02;
	else
		infoAdd._LockTeamBalance = 0x00;



	// 비교후 변경된것이 있으면 보낸다.
	// 총이 바뀌었을떄
	if( InfoBasic._WeaponFlag != OldInfo._WeaponFlag )
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOMINFO_N, &InfoBasic );
	}

	// 총 이외에 다른 것이 바뀌었을때
	if (infoAdd._LockInterEnter != OldInfoAdd._LockInterEnter ||
		infoAdd._LockObserver != OldInfoAdd._LockObserver ||
		infoAdd._LockTeamBalance != OldInfoAdd._LockTeamBalance
		)
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_OPTION_N, &infoAdd);
	}
}

void CPhaseReadyRoom::OnAdvencedCancel(void)
{
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
	m_pRoomWin->setInputDisable(FALSE);

	m_pAdvencedWin->SetEnable(FALSE);
}

void CPhaseReadyRoom::OnJoinLimitCheck(BOOL check)
{
	m_pLimitJoinYes->setCheckBoxState(check);
	m_pLimitJoinNo->setCheckBoxState(!check);
}

void CPhaseReadyRoom::OnObserverLimitCheck(BOOL check)
{
	m_pLimitObserverYes->setCheckBoxState(check);
	m_pLimitObserverNo->setCheckBoxState(!check);
}

void CPhaseReadyRoom::OnTeamBalanceCheck(INT32 check)
{
	switch(check)
	{
		case 1:
			m_pTeamBalancePersonnel->setCheckBoxState(true);
			m_pTeamBalanceAbility->setCheckBoxState(false);
			m_pTeamBalanceNo->setCheckBoxState(false);
			break;
		case 2:
			m_pTeamBalancePersonnel->setCheckBoxState(false);
			m_pTeamBalanceAbility->setCheckBoxState(true);
			m_pTeamBalanceNo->setCheckBoxState(false);
			break;
		default:
			m_pTeamBalancePersonnel->setCheckBoxState(false);
			m_pTeamBalanceAbility->setCheckBoxState(false);
			m_pTeamBalanceNo->setCheckBoxState(true);
			break;
	}
}

void CPhaseReadyRoom::OnInviteConfirm(void)
{
	UINT32 inviteCount = 0;

	// Check 확인하여 0-4 만큼 초대하기
	for(UINT32 i = 0; i < MAX_INVITE_COUNT; i++)
	{
		if (m_inviteSlot[i]._pCheck->getCheckBoxState())
		{
			g_pGameContext->CheckInviteUserInfo(i);
			inviteCount++;
		}
	}

	if (0 < inviteCount)
	{
		g_pGameContext->SetEvent(EVENT_INVITE_LOBBY_USER);

		char inviteMessage[MAX_STRING_COUNT];

		sprintf_s(inviteMessage, GAME_STRING("STR_TBL_GUI_READY_NOTICE_SEND_INVITE"), inviteCount);/*%d명에게 초대메시지를 전송하였습니다.*/
		STRING_POPUP(inviteMessage);
	}

	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
	m_pRoomWin->setInputDisable(FALSE);

	m_pInviteWin->SetEnable(FALSE);
}

void CPhaseReadyRoom::OnInviteCancel(void)
{
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
	m_pRoomWin->setInputDisable(FALSE);

	m_pInviteWin->SetEnable(FALSE);
}

void CPhaseReadyRoom::OnComplateConfirm(void)
{
	i3Error::Log( "최종 아이템 보상창1 [ CPhaseReadyRoom::OnComplateConfirm ] " );
	// 보상 윈도우를 닫는다.
	m_pCardComplateWin->SetEnable(FALSE);
	m_pRoomWin->setInputDisable(FALSE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);

		i3Error::Log( "최종 아이템 보상창2 [ g_pGameContext->IsFinalPrizeFlag ] %x ", 
			g_pGameContext->GetGameFlag() & GAMEFLAG_FINALPRIZE );

	// 최종보상이 이루어졌는지 확인한다.
	if (g_pGameContext->IsFinalPrizeFlag())
	{
		_OpenFinalPrizeWindow();
	}

	i3Error::Log( "최종 아이템 보상창100 [ CPhaseReadyRoom::OnComplateConfirm ] " );
}

void CPhaseReadyRoom::OnButtonConfirm(void)
{
 	// GUI 복귀
	m_pRoomWin->setInputDisable(FALSE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
}

void CPhaseReadyRoom::CbFinalComplateConfirm(void* pThis,INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		CPhaseReadyRoom* pReadyRoom = (CPhaseReadyRoom*)pThis;

		pReadyRoom->OnButtonConfirm();
	}
}

void CPhaseReadyRoom::CbMoveShop(void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		CPhaseReadyRoom* pReadyRoom = (CPhaseReadyRoom*)pThis;
		CStageReady * pStageReady = ((CStageReady*)pReadyRoom->getParent());
		pStageReady->ChangeStep(STEP_SHOP);
	}
}

void CPhaseReadyRoom::OnRankUpConfirm(void)
{
	m_pRoomWin->setInputDisable(FALSE);
	m_pRankUpWin->SetEnable(FALSE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);

	// 캐릭터 세레머니 추가 2008.07.03 dfly79
	((CStageReady*)m_pParent)->PlayCeremonyCharacter();
}

void CPhaseReadyRoom::OnSearchHost(void)
{
	// 연속 입력 무시
	if (0.0f < m_rSearchHostDelay)
	{
		return;
	}

	m_rSearchHostDelay = 3.0f;	// 3초


	// 방장여부에 따라 기능이 달라진다.
	if (_BeHost())
	{
		// 적합한 방장후보를 찾는다.
		g_pGameContext->SetEvent(EVENT_SEARCH_HOST);
	}
	else
	{
		// 방장 변경요청 메시지를 전달한다.
		g_pGameContext->SetEvent(EVENT_REQUEST_HOST_CHANGE);
	}
}

void CPhaseReadyRoom::OnTeamChange(void)
{
#if defined( BUILD_RELEASE_QA_VERSION ) && defined( TEMP_QUEST_REWARD)

	STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_UNDER_CONSTRUCTION"));/*차후 구현 예정입니다.*/

#else

	// TODO : 팀바꾸기.

	if (0.0f < m_rTeamChangeDelay)
	{
		return;
	}

	m_rTeamChangeDelay = 2.0f;	// 2초

	if (_BeHost())
	{
		g_pGameContext->SetEvent(EVENT_TEAM_CHANGE);
	}

#endif
}

//virtual
BOOL CPhaseReadyRoom::OnInviteMove(void)
{
	BOOL bMove = FALSE;
	if( g_pCommunity->GetMovingMatch() > 0 )
	{
		bMove = TRUE;
	}
	else
	{
		const CUserLocationInfo & UserLocInfo = g_pCommunity->GetMovingLocInfo();
		bMove = (g_pGameContext->IsSameRoom(UserLocInfo) == FALSE ? TRUE : FALSE);
	}

	if ( bMove )
	{
		i3GuiRoot::setGuiDisableInput(TRUE);

		// 스탭, 팝업을 닫는다.
		{
			CStageReady * pStageReady = ((CStageReady*)m_pParent);

			pStageReady->TogglePopup(POPUP_NULL);

			if (STEP_NULL != pStageReady->GetCurrentStepType())
			{
				pStageReady->ChangeStep( pStageReady->GetCurrentStepType());
			}
		}

		// 나간다.
		g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		m_bEnableUpdate = FALSE;

		return TRUE;
	}


	STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/

	g_pCommunity->ComplateInviteMove();
	return TRUE;
}

void CPhaseReadyRoom::OnAddFriend(INT32 slotIdx)
{
	char nick[MAX_STRING_COUNT] = "";

	m_nSelectedSlotIdx = slotIdx;

	i3String::Copy( nick, g_pGameContext->getNickForSlot(slotIdx), MAX_STRING_COUNT );


	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_NO_MORE_ADD_FRIEND"));/*더이상 친구를 추가할 수 없습니다.*/
		return;
	}

	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_TBL_GUI_READY_NOTICE_ALREADY_FRIEND"), nick);/*[%s]님은 이미 친구목록에 등록되어 있습니다.*/
			STRING_POPUP(message);
			return;
		}
	}

	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_NOTICE_NO_ADD_SELF"));/*자신을 추가할 수 없습니다.*/
		return;
	}

	g_pCommunity->InsertFriend(nick);
}

void CPhaseReadyRoom::OnAddBlock(INT32 slotIdx)
{
	char message[MAX_STRING_COUNT] = "";
	char nick[MAX_STRING_COUNT] = "";

	m_nSelectedSlotIdx = slotIdx;

	i3String::Copy( nick, g_pGameContext->getNickForSlot(slotIdx), MAX_STRING_COUNT );

	if (g_pGameContext->IsMyNickName(nick))
	{
		return;
	}

	//	"[ %s ]님을 차단하시겠습니까?"
	sprintf_s( message, GAME_STRING("STR_TBL_GUI_LOBBY_Q_BLOCK_USER"), nick);

	STRING_POPUP_Q( message, MAKE_CALLBACK(CbBlockUser), this);
}

void CPhaseReadyRoom::OnWhisper(INT32 slotIdx)
{
	m_nSelectedSlotIdx = slotIdx;

	m_pChatBody->OnWhisper(slotIdx);
}

void CPhaseReadyRoom::OnViewOtherUserItemInfo(INT32 slotIndex)
{
	m_nSelectedSlotIdx = slotIndex;

	INT32 ItemViewItemInfo = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_VIEW_OTHER_USER_ITEM_INFO, CASHITEM_DAY_NO);

	if (g_pFramework->IsUsingCashItem(ItemViewItemInfo))
	{
		UINT8 Type = 0;
		UINT32 Arg = 0;
		g_pFramework->GetCashItemTypeByID(ItemViewItemInfo, &Type, &Arg);

		INT64 i64UID = g_pFramework->GetInvenList()->GetCashItemWareDBIdx(ItemViewItemInfo);
		UINT32 Size = sizeof(UINT8);

		g_pGameContext->SetViewOtherUserItemInfoUserName( g_pGameContext->getNickForSlot(slotIndex) );

		g_pGameContext->SetEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO, &i64UID, &Size, &slotIndex);
		return;
	}
	else
	{
		// 알림
		// {col:255,152,52,255}상대방 정보 보기{/col} 아이템이 없습니다. 상점으로 이동하시겠습니까?
		CAPTION_POPUP_Q(GAME_STRING("STR_TBL_GUI_VIEW_OTHER_USER_ITEM_INFO"), GAME_STRING("STR_TBL_GUI_READY_NOTICE"), MAKE_CALLBACK(CbMoveShop), this);
	}

}

void CPhaseReadyRoom::OnClanInvite(INT32 slotIdx)
{
	m_nSelectedSlotIdx = slotIdx;

	g_pFramework->GetClanContext()->SendClanInvite( slotIdx, 1 );
}

void CPhaseReadyRoom::OpenPopupUsingCashItem(void)
{
	if( ! g_pFramework->GetInvenList()->IsUseBattleGiveupCashItem())
		return;

	char szMessage[MAX_STRING_COUNT] = INVALID_TEXT;
	char szRemainTeam[MAX_STRING_COUNT] = INVALID_TEXT;
	UINT32 itemTerm = 0;
	UINT8 itemAttr = 0;
	UINT32 itemArg = 0;


	//	현재 '자유 이동' 아이템을 사용하고 있으면 아이템 사용 알림 팝업창을 띄운다.
	INT32 itemFreeMove = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MAINTENANCE, CASHITEM_GROUP_FREE_MOVE, CASHITEM_DAY_NO);
	INT32 itemBattleMove = MAKE_CASH_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_EXPENDABLE, CASHITEM_GROUP_BATTLEFIELD_MOVE, CASHITEM_DAY_NO);

	//	'자유 이동' 아이템 우선
	if( g_pFramework->IsUsingCashItem(itemFreeMove))
	{
		INT32 ItemID = GetCashItemByGroup(itemFreeMove);


		g_pFramework->GetCashItemTypeByID(ItemID, &itemAttr, &itemArg);

		//	현재 사용중인 남은 기간을 알아온다.
		if( itemArg > 0)
		{
			itemTerm = g_pGameContext->GetInventorySecTime(itemArg);
			if( itemTerm > 0)
			{
				//	기간 보정 - komet
				if( itemTerm >= 120)
					itemTerm -= 120;

				itemTerm = ConvertSecToUseDay(itemTerm);
			}
		}
		else
		{
			itemTerm = 0;
		}

		if (1 < itemTerm)
		{
			//	남은 기간 : %d일
			i3String::Format(szRemainTeam, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_REMAINING_DAYS1"),
				itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));/*남은 기간 : {col:255,255,255,255}%d%s{/col}*/
		}
		else if( itemTerm == 0)
		{
			//	남은 기간 : %d일
			i3String::Format(szRemainTeam, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_REMAINING_DAYS2"),
				itemTerm, GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"));/*남은 기간 : {col:255,255,255,255}%d%s{/col}*/
		}
		else
		{
			//	남은 기간 : 1일 미만
			i3String::Format(szRemainTeam, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_REMAINING_DAYS2"),
				GAME_STRING("STBL_IDX_ITEM_TERM_UNIT"), GAME_STRING("STBL_IDX_ITEM_LESS_THEN"));/*남은 기간 : {col:210,0,0,255}1%s %s{/col}*/
		}


		char Name[MAX_STRING_COUNT] = {};
		g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);

		/*{col:255,153,51,255}%s{/col} 아이템이 적용되었습니다.\n \n {col: 51,204,255,255}경험치 및 포인트를 획득했습니다.\n 이탈 패널티가 적용되지 않습니다.\n 전적 및 킬/데스가 기록되지 않습니다.{/col}\n \n %s*/
		i3String::Format(szMessage, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), Name, szRemainTeam);

		STRING_POPUP( szMessage);

		g_pFramework->GetInvenList()->SetUseBattleGiveupCashItem(FALSE);
	}
	//	'전장 이동' 아이템을 사용 결과가 날라오면
	//	인게임에서 '전장 이동' 아이템을 사용하여 퇴장할 경우 레디룸에 아이템 사용 결과를 서버로부터 받고 알림창을 띄운다. komet
	else if( g_pFramework->GetInvenList()->GetLatestUsedCashItemFlag() == itemBattleMove)
	{
		INT32 ItemID = GetCashItemByGroup(itemBattleMove);


		if( g_pFramework->GetCashItemTypeByID(ItemID, &itemAttr, &itemArg))
		{
			itemTerm = itemArg;	//	아이템 갯수

			i3String::Format(szRemainTeam, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_REMAINING_COUNT"), itemTerm);/*남은 갯수 : {col:255,255,255,255}%d개{/col}*/
		}

		char Name[MAX_STRING_COUNT] = {};
		g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);

		/*{col:255,153,51,255}%s{/col} 아이템이 적용되었습니다.\n \n {col: 51,204,255,255}경험치 및 포인트를 획득했습니다.\n 이탈 패널티가 적용되지 않습니다.\n 전적 및 킬/데스가 기록되지 않습니다.{/col}\n \n %s*/
		i3String::Format(szMessage, MAX_STRING_COUNT, GAME_STRING("STR_TBL_GUI_READY_ITEM_APPLYED_LEAVE_GAME"), Name, szRemainTeam);

		STRING_POPUP( szMessage);

		g_pFramework->GetInvenList()->SetUseBattleGiveupCashItem(FALSE);
	}

}

void CPhaseReadyRoom::_ProcessCommon( REAL32 rDeltaSeconds)
{
	// 업데이트 처리가 허용되지 않았다면 처리하지 않는다
	if(FALSE == m_bEnableUpdate)	return;


	// ! praptor - EVENT_CHANGE_TEAM_N이 실패할 경우 화면 업데이트가 멈출 수 있다.
	// ! praptor - 게임 이벤트는 정상 처리 된다.
	if( m_bChagingTeam)
	{
		if(g_pGameContext->getMySlotIdx() % 2 == m_nOldTeam)
		{
			// 팀 변경 중이면, 팀 변경이 완료될때까지 슬롯 업데이트를 하지 않는다.
			return;
		}
		else
		{
			m_nOldViewSlot = -1;
			_InitUserInfo();
			m_bChagingTeam = FALSE;
		}
	}


	// Chat 정보 갱신
	_ProcessChat( rDeltaSeconds);

	// 캐릭터 정보 갱신
	_ProcessCharaInfo( rDeltaSeconds);

	// 맵 정보 갱신
	_ProcessMapInfo( rDeltaSeconds);

	// 슬롯 갱신
	_ProcessSlot( rDeltaSeconds);

	//	클랜전 업데이트
	_ProcessClanBattle( rDeltaSeconds);
}

void CPhaseReadyRoom::_ProcessCharaInfo( REAL32 rDeltaSeconds)
{
	// 캐릭터 상세정보 갱신
	_ProcessCharaDetailInfo( rDeltaSeconds);

	if( -1 == g_pGameContext->GetCurrentInfoSlot())
	{
		_InitUserInfo();
	}
	else
	{
		_Update3DCharacter();
	}
}

void CPhaseReadyRoom::_ProcessCharaDetailInfo( REAL32 rDeltaSeconds)
{
	INT32 totalWidth = 0;
	INT32 nicknameWidth = 0;
	REAL32 frontWidth = 0.0f;
	REAL32 nicknameStart = 0.0f;

	// 유저 정보 불러오기
	const USER_INFO_BASIC* pBasic = g_pGameContext->GetUserInfoBasic();
	const USER_INFO_RECORD* pRecord = g_pGameContext->GetUserInfoRecord();

	// 닉네임
	SetNameUsingColorTable(m_pRecordNickname, pBasic->_nick, pBasic->_nickcolor);

	// 계급 (초보 채널에 대한 처리)
	// 초보채널이라 할지라도 자기 자신의 계급은 원래의 계급을 보여줍니다.
	if(g_pGameContext->IsBeginnerChannel1() && g_pGameContext->GetCurrentInfoSlot() != g_pGameContext->getMySlotIdx())
	{
		m_pRecordRank->SetEnable(FALSE);
		m_pRecordRank_Beginner->SetAllShapeEnable(TRUE);

		// 계급 마크를 닉네임 바로 앞에 붙인다.
		totalWidth = m_pRecordNickname->getWidth();
		nicknameWidth = m_pRecordNickname->getTextNodeDX()->getTextWidth();
		frontWidth = (REAL32)(((totalWidth - nicknameWidth) / 2) - m_pRecordRank_Beginner->getWidth());

		frontWidth -= (8 * i3GuiRoot::getGuiResolutionWidth());

		nicknameStart = m_pRecordNickname->getPositionX();

		// setPositionX()의 이자값은 기본해상도를 기준으로 작동한다.
		// 따라서 변경된 해상도를 통해 얻은 실제 픽셀길이는 해상도 비율만큼
		// 줄여줘야한다.
		frontWidth = frontWidth / i3GuiRoot::getGuiResolutionWidth();
		m_pRecordRank_Beginner->setPositionX(nicknameStart + frontWidth);
	}
	else
	{
		m_pRecordRank_Beginner->SetAllShapeEnable(FALSE);
		m_pRecordRank->SetEnable(TRUE);
		m_pRecordRank->SetShapeUVIndex(pBasic->_rank);

		// 계급 마크를 닉네임 바로 앞에 붙인다.
		totalWidth = m_pRecordNickname->getWidth();
		nicknameWidth = m_pRecordNickname->getTextNodeDX()->getTextWidth();
		frontWidth = (REAL32)(((totalWidth - nicknameWidth) / 2) - m_pRecordRank->getWidth());

		frontWidth -= (8 * i3GuiRoot::getGuiResolutionWidth());

		nicknameStart = m_pRecordNickname->getPositionX();

		// setPositionX()의 이자값은 기본해상도를 기준으로 작동한다.
		// 따라서 변경된 해상도를 통해 얻은 실제 픽셀길이는 해상도 비율만큼
		// 줄여줘야한다.
		frontWidth = frontWidth / i3GuiRoot::getGuiResolutionWidth();
		m_pRecordRank->setPositionX(nicknameStart + frontWidth);
	}

	// 시즌전적
	{
		char strRecord[MAX_STRING_COUNT];

		#ifdef USE_MODE_RECORD
		sprintf_s(strRecord, "%d%s %d%s %d%s", pRecord->_season_record._match, GAME_STRING("STBL_IDX_BATTLE_COUNT"),
				pRecord->_season_record._win, GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),
				pRecord->_season_record._lose, GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));	// 0전 0승 0패
		#else
		sprintf_s(strRecord, "%d%s %d%s %d%s", pRecord->_smatch, GAME_STRING("STBL_IDX_BATTLE_COUNT"),
				pRecord->_swin, GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),
				pRecord->_slose, GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));	// 0전 0승 0패
		#endif
		m_pSeasonRecord->SetText(strRecord);
	}

	// 시즌 킬/데스
	{
		char strKillDeath[MAX_STRING_COUNT];
		#ifdef USE_MODE_RECORD
		INT32 killRate = g_pGameContext->getKillRate(pRecord->_season_record._killcount, pRecord->_season_record._death);

		sprintf_s(strKillDeath, GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killRate);
		m_pSeasonKillDeath->SetText(strKillDeath);

		sprintf_s(strKillDeath, "%d / %d", pRecord->_season_record._killcount, pRecord->_season_record._death);
		m_pSeasonKillDeath2->SetText(strKillDeath);
		#else
		INT32 killRate = g_pGameContext->getKillRate(pRecord->_skillcount, pRecord->_sdeath);

		sprintf_s(strKillDeath, GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killRate);
		m_pSeasonKillDeath->SetText(strKillDeath);

		sprintf_s(strKillDeath, "%d / %d", pRecord->_skillcount, pRecord->_sdeath);
		m_pSeasonKillDeath2->SetText(strKillDeath);
		#endif
	}

	// 시즌 헤드샷
	{
		//INT32 nHeadshot = 0;
		//char strHeadshot[MAX_STRING_COUNT];

		//#ifdef USE_MODE_RECORD
		//if (0 != pRecord->_season_record._dkillcount)
		//{
		//	nHeadshot = (INT32)((pRecord->_season_record._headshot * 100.0f / pRecord->_season_record._dkillcount) + 0.5f);
		//}
		//#else
		//if (0 != pRecord->_sdkillcount)
		//{
		//	nHeadshot = (INT32)((pRecord->_sheadshot * 100.0f / pRecord->_sdkillcount) + 0.5f);
		//}
		//#endif

		INT32 nHeadshot = SeasonHeadShotPercent(pRecord);

		char strHeadshot[MAX_STRING_COUNT];
		sprintf_s(strHeadshot, GAME_STRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), nHeadshot);
		m_pSeasonHeadshot->SetText(strHeadshot);
	}

	// 시즌 전장이탈률
	{
		char disconnect[MAX_STRING_COUNT];

		#ifdef USE_MODE_RECORD
		sprintf_s(disconnect, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_season_record._dis);
		#else
		sprintf_s(disconnect, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_sdis);
		#endif
		
		m_pSeasonDisconnect->SetText(disconnect);
	}


	// 통산전적
	{
		char strRecord[MAX_STRING_COUNT];
		#ifdef USE_MODE_RECORD
		sprintf_s(strRecord, "%d%s %d%s %d%s", pRecord->_total_record._match, GAME_STRING("STBL_IDX_BATTLE_COUNT"),
				pRecord->_total_record._win, GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),
				pRecord->_total_record._lose, GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));	// 0전 0승 0패
		#else
		sprintf_s(strRecord, "%d%s %d%s %d%s", pRecord->_match, GAME_STRING("STBL_IDX_BATTLE_COUNT"),
				pRecord->_win, GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),
				pRecord->_lose, GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));	// 0전 0승 0패
		#endif
		
		m_pTotalRecord->SetText(strRecord);
	}

	// 통산 킬/데스
	{
		char strKillDeath[MAX_STRING_COUNT];

		#ifdef USE_MODE_RECORD
		INT32 killRate = g_pGameContext->getKillRate(pRecord->_total_record._killcount, pRecord->_total_record._death);

		sprintf_s(strKillDeath, GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killRate);
		m_pTotalKillDeath->SetText(strKillDeath);

		sprintf_s(strKillDeath, "%d / %d", pRecord->_total_record._killcount, pRecord->_total_record._death);
		m_pTotalKillDeath2->SetText(strKillDeath);
		#else
		INT32 killRate = g_pGameContext->getKillRate(pRecord->_killcount, pRecord->_death);

		sprintf_s(strKillDeath, GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE_MODE1"), killRate);
		m_pTotalKillDeath->SetText(strKillDeath);

		sprintf_s(strKillDeath, "%d / %d", pRecord->_killcount, pRecord->_death);
		m_pTotalKillDeath2->SetText(strKillDeath);
		#endif
	}

	// 통산 헤드샷
	{
		INT32 nHeadshot = TotalHeadShotPercent(pRecord);

		char strHeadshot[MAX_STRING_COUNT];
		sprintf_s(strHeadshot, GAME_STRING("STR_TBL_PROFILE_HEADSHOT_RATE_MODE1"), nHeadshot);
		m_pTotalHeadshot->SetText(strHeadshot);
	}

	// 통산 전장 이탈률
	{
		char disconnect[MAX_STRING_COUNT];

		//INT32 disconnectRate = 0;
		//REAL32 total = (REAL32)(pBasic->_dis + pRecord->_dmatch);
		//if (0 < total)
		//{
		//	disconnectRate = (INT32)(((pBasic->_dis / total) * 100.0f) + 0.5f);
		//}

		#ifdef USE_MODE_RECORD
		sprintf_s(disconnect, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_total_record._dis);
		#else
		sprintf_s(disconnect, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_dis);
		#endif

		m_pTotalDisconnect->SetText(disconnect);
	}


	// 클랜에 가입되어있으면 가입정보를 출력한다.
	if (0 < pBasic->_clanidx && CLAN_MEMBER_LEVEL_UNKNOWN < pBasic->_clanstate )
	{
		SetNameUsingColorTable( m_pClanNameValue, pBasic->_clanName, pBasic->_clanNamecolor );
		m_pClanRankValue->SetTextColor( GetColor(GCT_VALUE));

		//	클랜 마크
		m_pClanMark->SetEnable(TRUE);

#if defined( USE_CLAN_MARK)
		::SetClanMarkTexEx(m_pClanMark, CLAN_MARK_SMALL, pBasic->_clanMark);
#endif

		//	클랜 등급, 클랜 등급 이미지 표시
		::SetClanRank2(m_pClanRankValue, m_pClanRank, pBasic->_clanExpRank, pBasic->_clanPerRank, pBasic->_clanExpRank);
	}
	else
	{
		I3COLOR color;
		i3Color::Set( &color, (UINT8) 80, 80, 80, 255);

		m_pClanNameValue->SetText(GAME_STRING("STR_TBL_GUI_READY_NO_JOINING"));/*미가입*/
		m_pClanNameValue->SetTextColor( &color);
		m_pClanRankValue->SetText(GAME_STRING("STR_TBL_GUI_READY_NO_JOINING"));/*미가입*/
		m_pClanRankValue->SetTextColor( &color);
		m_pClanMark->SetEnable(FALSE);
		m_pClanRank->SetAllShapeEnable(FALSE);
	}
}
/*
void CPhaseReadyRoom::RefreshRandomMap()
{	
	UINT32 StageID = g_pGameContext->GetMyRoomStageID();
	STAGE_TYPE stageType		= getStageType( StageID);

	g_pGameContext->ClearRandomMapIndexList();
	g_pGameContext->ClearRandomMapList();
	
	for(INT32 i = STAGE_ORD_NA + 1; i < STAGE_ORD_MAX; i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)i);

		if ( isActiveStage(stageId))
		{
			if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && stageType == STAGE_TYPE_ANNIHILATION||stageType == getStageType(stageId))
			{
				m_IndexMap = i;

				g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (stageType == STAGE_TYPE_ANNIHILATION)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_ANNIHILATION);
				}
			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && stageType == STAGE_TYPE_SUDDEN_DEATH||stageType == getStageType(stageId))
			{
				m_IndexMap = i;

				g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (stageType == STAGE_TYPE_SUDDEN_DEATH)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_SUDDEN_DEATH);
				}
			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && stageType == STAGE_TYPE_HEAD_HUNTER||stageType == getStageType(stageId))
			{
				m_IndexMap = i;

				g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (stageType == STAGE_TYPE_HEAD_HUNTER)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_HEAD_HUNTER);
				}
			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && stageType == STAGE_TYPE_HEAD_KILLER||stageType == getStageType(stageId))
			{
				m_IndexMap = i;

				g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (stageType == STAGE_TYPE_HEAD_KILLER)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_HEAD_KILLER);
				}
			}
		}
	}

	for(INT32 i =0; i<g_pGameContext->GetRandomMapIndexCount(); i++)
	{
		m_CheckMap = g_pGameContext->GetRandomMapIndex(i);
		g_pGameContext->AddRandomMap(m_CheckMap);
	}
}
*/
void CPhaseReadyRoom::InitRandomCheck()
{
	//랜덤맵 체크상태를 초기화합니다
	/*for(INT32 i =0; i < 20; i++)
	{
		g_pGameContext->setRandomMap(i,TRUE);
	}*/
	g_pGameContext->InitRandomCheck( true );
}
void CPhaseReadyRoom::_ProcessMapInfo( REAL32 rDeltaSeconds)
{
	// 방정보를 불러온다
	ROOM_INFO_BASIC Info;
	ROOM_INFO_ADD	InfoAdd;

	g_pGameContext->getMyRoomInfo(&Info);
	g_pGameContext->getMyRoomInfoAdd(&InfoAdd);

	if( Info._StageID == 0)return; //getStageDisplayName( (STAGE_ID) Info._StageID) 에러가 맵이동시 나올수 있습니다.

	STAGE_TYPE stageType		= getStageType( Info._StageID);
	UINT32 gameType				= InfoAdd._SubType & 0x0F;
	UINT32 gameTime				= InfoAdd._SubType & 0xF0;

#if defined (USE_RANDOMMAP_MODE)	
	
	m_pSelectRandomMap->SetEnable(TRUE);

	//모드가 바뀌면 팝업정보를 갱신합니다.
	//if(g_pGameContext->GetRandomStageType() != stageType && m_pSelectRandomMap->getCheckBoxState() && _BeHost() )
		//RefreshRandomMap();

	if( stageType != g_pGameContext->GetRandomStageType()
		&& m_pSelectRandomMap->getCheckBoxState() && _BeHost() )
	{		
		CReadyPhaseBase::createRandomMap( stageType );
	}
		
	m_pSelectRandomMap->setCheckBoxState(g_pGameContext->IsRandomMap());
	
//	 맵 이미지
	if(g_pGameContext->IsRandomMap())
	{
		g_pFramework->SetRandomMapShape(m_pMapImage);
		m_pRandomMap->setInputDisable(FALSE);
		m_pMapCombo->SetEnable(FALSE);
	}
	else
	{
		g_pFramework->SetMapShape( m_pMapImage, (STAGE_ID)Info._StageID);
		m_pRandomMap->setInputDisable(TRUE);
		m_pMapCombo->SetEnable(TRUE);
	}
#else
	g_pFramework->SetMapShape( m_pMapImage, (STAGE_ID)Info._StageID);
	m_pSelectRandomMap->SetEnable(FALSE);
#endif	
	// 맵 이름, 모드
	{
		std::string strMapName;
		std::string strModeName;
		BOOL bAllowChangeMode = FALSE;

		GetStageMapName(g_pGameContext->IsRandomMap(), (STAGE_ID)Info._StageID, strMapName);
		GetStageModeName(stageType, strModeName, bAllowChangeMode);

		m_pMap->SetText(strMapName.c_str());
		m_pClanBattleInfoModeValue->SetText( strModeName.c_str() );

#if defined( USE_DUAL_AI_MODE)
		if (g_pGameContext->IsStageMaskTraining())
		{
			//m_pSelectRandomMap->SetEnable(FALSE);//랜덤
			switch(InfoAdd._AIState)
			{
			case AI_MODE_STATE_ORIGINAL:
				m_pModeValue->SetText("Mode1");
				m_pTrainingEnemyKey->SetEnable( TRUE);
				m_pTrainingEnemyValue->SetEnable( TRUE);
				m_pTrainingEnemyPrev->SetEnable( TRUE);
				m_pTrainingEnemyNext->SetEnable( TRUE);
				m_pTrainingDifficultyKey->SetEnable( TRUE);
				m_pTrainingDifficultyValue->SetEnable( TRUE);
				m_pTrainingDifficultyCombo->SetEnable( TRUE);
				break;
			case AI_MODE_STATE_DIEHARD:
				m_pModeValue->SetText("Mode2");
				m_pTrainingEnemyKey->SetEnable( FALSE);
				m_pTrainingEnemyValue->SetEnable( FALSE);
				m_pTrainingEnemyPrev->SetEnable( FALSE);
				m_pTrainingEnemyNext->SetEnable( FALSE);
				m_pTrainingDifficultyKey->SetEnable( FALSE);
				m_pTrainingDifficultyValue->SetEnable( FALSE);
				m_pTrainingDifficultyCombo->SetEnable( FALSE);
				break;
			}
			
		}
		else
		{
				m_pModeValue->SetText( strModeName.c_str() );
		}
#else
		m_pModeValue->SetText( strModeName.c_str() );
#endif

		m_pExtraTypeImage->SetEnable(FALSE);
		if( Info._StageMask ==  STAGE_MASK_TYPE_HEADHUNTER )
		{
			m_pExtraTypeImage->SetEnable(TRUE);
		}


		INT32 mapTypeIdx = GetMapImageShapeIndex(stageType);

		if (g_pGameContext->IsStageMaskTraining())
		{
			mapTypeIdx = 4;
		}

		m_pTypeImage->SetAllShapeEnable(FALSE);
		
		if( mapTypeIdx == -1 )
		{
			m_pTypeImage->SetVisible( FALSE );
		}
		else
		{
			m_pTypeImage->SetVisible( TRUE );
			m_pTypeImage->SetShapeEnable(mapTypeIdx, TRUE);
		}

		if (g_pGameContext->IsSniperRoom())
		{
			m_pTypeImage->SetAllShapeEnable(FALSE);
			m_pSelectRandomMap->SetEnable(FALSE);//랜덤

		}
		if (g_pGameContext->IsKnuckleRoom())
		{
			m_pTypeImage->SetAllShapeEnable(FALSE);
			m_pSelectRandomMap->SetEnable(FALSE);//랜덤

		}

		BOOL bRandomMap = g_pGameContext->IsRandomMap();
		
		m_pMapNewMark->SetEnable(FALSE);

		if ( TESTBIT(getNewMarkStageTable( getStageOrd(Info._StageID)) , GOODS_MARK_NEW) && bRandomMap == FALSE)
		{
			m_pMapNewMark->SetEnable(TRUE);
		}

		// X-Mas Mark
		m_pMapXMasMark->SetEnable(FALSE);
		if( TESTBIT(getNewMarkStageTable( getStageOrd(Info._StageID)), GOODS_MARK_XMAS) && bRandomMap == FALSE)
		{
			m_pMapXMasMark->SetEnable(TRUE);
		}

		if (g_pGameContext->IsStageMaskTraining())
		{
			bAllowChangeMode = FALSE;
		}

		if (g_pGameContext->IsSniperRoom())
		{
			bAllowChangeMode = FALSE;
		}

		if (g_pGameContext->IsKnuckleRoom())
		{
			bAllowChangeMode = FALSE;
		}

		m_pMapCircle->SetEnable(bAllowChangeMode);
	}

	// 승리조건
	{
		const char * strType = TextSet::VictoryCondition(stageType, gameType);

		m_pTypeValue->SetText(strType);
		m_pClanBattleInfoTypeValue->SetText(strType);
	}

	// 제한시간
	{
		const char * strGameTime = TextSet::GameTime(gameTime);

		m_pTimeValue->SetText(strGameTime);
		m_pClanBattleInfoTimeValue->SetText(strGameTime);
	}

	// 훈련전 제한시간
	{
		m_pTrainingTimeValue->SetText(TextSet::GameTime(gameTime));
	}

	// 훈련전 적군 수
	{
		char enemyCountText[MAX_STRING_COUNT];

		sprintf_s(enemyCountText, "%d", g_pGameContext->getEnemyAiCount());
		m_pTrainingEnemyValue->SetText(enemyCountText);
	}

	// 훈련전 난이도
	{
		char difficultyText[MAX_STRING_COUNT] = {0,};

		switch(g_pGameContext->getDifficultyLevel())
		{
		case 10:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_A"), MAX_STRING_COUNT );	break;
		case 9:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_B"), MAX_STRING_COUNT );	break;
		case 8:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_C"), MAX_STRING_COUNT );	break;
		case 7:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_D"), MAX_STRING_COUNT );	break;
		case 6:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_E"), MAX_STRING_COUNT );	break;
		case 5:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_F"), MAX_STRING_COUNT );	break;
		case 4:	i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_G"), MAX_STRING_COUNT );	break;
		case 3: i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_H"), MAX_STRING_COUNT );	break;
		case 2: i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_I"), MAX_STRING_COUNT );	break;
		case 1: i3String::Copy(difficultyText, GAME_STRING("STR_TBL_GUI_READY_GRADE_J"), MAX_STRING_COUNT );	break;
		}

		m_pTrainingDifficultyValue->SetText(difficultyText);
	}


	// 방장 권한 컨트롤 설정
	if (_BeHost())
	{
		m_pTitleValue->setInputDisable(FALSE);
		m_pPasswordValue->setInputDisable(FALSE);

		m_pMapCombo->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pTitleChange->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pMapImage->setInputDisable(FALSE);
		m_pSelectRandomMap->setInputDisable(FALSE);

		// 현재 스테이지타입에 따라 컨트롤의 비활성화를 결정합니다.
		if( g_pGameContext->IsStageMaskTraining())
		{
			m_pTitleValue->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTitleChange->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			//			m_pPasswordValue->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			//			m_pPasswordChange->setControlState(I3GUI_CONTROL_STATE_DISABLED);

			m_pTypePrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTypeNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);

			m_pLimitJoinYes->setCheckBoxState( TRUE);
			m_pLimitJoinNo->setCheckBoxState( FALSE);
			//			m_pLimitObserverYes->setCheckBoxState( TRUE);
			//			m_pLimitObserverNo->setCheckBoxState( FALSE);
			m_pTeamBalancePersonnel->setCheckBoxState( FALSE);
			m_pTeamBalanceAbility->setCheckBoxState( FALSE);
			m_pTeamBalanceNo->setCheckBoxState( TRUE);

			m_pLimitJoinYes->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pLimitJoinNo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pLimitObserver3PV_Free->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pLimitObserver1PV_Free->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pLimitObserverEnemy_Free->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pLimitObserverEnemyHP->setControlState(I3GUI_CONTROL_STATE_DISABLED);

			m_pTeamBalancePersonnel->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTeamBalanceAbility->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pTeamBalanceNo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		
			
#if defined( USE_DUAL_AI_MODE)
			m_pModePrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pModeNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
#endif
			m_pTrainingTimePrev->setControlState( I3GUI_CONTROL_STATE_NORMAL);
			m_pTrainingTimeNext->setControlState( I3GUI_CONTROL_STATE_NORMAL);
		}
		else
		{
			m_pTitleValue->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTitleChange->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pPasswordValue->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pPasswordChange->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pModePrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pModeNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTypePrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTypeNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);

			m_pLimitJoinYes->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pLimitJoinNo->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pLimitObserver3PV_Free->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pLimitObserver1PV_Free->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pLimitObserverEnemy_Free->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pLimitObserverEnemyHP->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTeamBalancePersonnel->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTeamBalanceAbility->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pTeamBalanceNo->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		}

		m_pTimePrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pTimeNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);


		if (Info._State == ROOM_STATE_COUNTDOWN)
		{
			m_pCancelStart->SetEnable(TRUE);

			m_pStart->SetEnable(FALSE);
			m_pReady->SetEnable(FALSE);
			m_pCancelReady->SetEnable(FALSE);
		}
		else
		{
			m_pStart->SetEnable(TRUE);

			m_pReady->SetEnable(FALSE);
			m_pCancelStart->SetEnable(FALSE);
			m_pCancelReady->SetEnable(FALSE);
		}


		m_pTrainingEnemyPrev->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pTrainingEnemyNext->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pTrainingDifficultyCombo->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}
	else
	{
		m_pTitleValue->setInputDisable(TRUE);
		m_pPasswordValue->setInputDisable(TRUE);

		m_pMapCombo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTitleChange->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pPasswordChange->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pModePrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pModeNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTypePrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTypeNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTimePrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTimeNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTrainingEnemyPrev->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTrainingEnemyNext->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTrainingDifficultyCombo->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pTrainingTimePrev->setControlState( I3GUI_CONTROL_STATE_DISABLED);
		m_pTrainingTimeNext->setControlState( I3GUI_CONTROL_STATE_DISABLED);

		m_pMapImage->setInputDisable(TRUE);
		m_pSelectRandomMap->setInputDisable(TRUE);

		// 게임이 시작되면, 준비버튼을 시작버튼으로 대체한다

		INT32 mySlotIdx = g_pGameContext->getMySlotIdx();
		SLOT_INFO* mySlotInfo = g_pGameContext->getSlotInfo(mySlotIdx);

		if (Info._State == ROOM_STATE_COUNTDOWN && mySlotInfo->_State == SLOT_STATE_READY)
		{
			m_pCancelReady->SetEnable(TRUE);

			m_pStart->SetEnable(FALSE);
			m_pReady->SetEnable(FALSE);
			m_pCancelStart->SetEnable(FALSE);
		}
		else if (ROOM_STATE_LOADING <= Info._State)
		{
			m_pStart->SetEnable(TRUE);
			m_pReady->SetEnable(FALSE);
			m_pCancelStart->SetEnable(FALSE);
			m_pCancelReady->SetEnable(FALSE);
		}
		else
		{
			m_pStart->SetEnable(FALSE);
			m_pReady->SetEnable(TRUE);
			m_pCancelStart->SetEnable(FALSE);
			m_pCancelReady->SetEnable(FALSE);
		}
	}


	// 비밀방 표시
	BOOL bSecretRoom = (BOOL)GET_ROOM_INFO_PRIVATE_MODE( Info._InfoFlag );
	if (bSecretRoom)
	{
		m_pKey->SetEnable(TRUE);
	}
	else
	{
		m_pKey->SetEnable(FALSE);
	}

	// 무기 제한
	BOOL bLockPrimary =		(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_PRIMARY);
	BOOL bLockSecondary =	(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_SECONDARY);
	BOOL bLockRPG7 =		(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_RPG7);
	BOOL bLockMelee =		(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_MELEE);
	BOOL bLockSniper =		(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_SNIPER);
	BOOL bLockShotGun =		(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_SHOTGUN);
	BOOL bLockKnuckle =		(BOOL) TESTBIT(Info._WeaponFlag,	LOCK_KNUCKLE);

	//	칼전/권총전 여부
	UNSET_ROOM_BATTLE_MARK( m_pBattleMark);
	if(bLockSniper) {
		SET_ROOM_BATTLE_MARK_SNIPER( m_pBattleMark);
	}
	else if(bLockShotGun) {
		SET_ROOM_BATTLE_MARK_SHOTGUN( m_pBattleMark);
	}
	else if( !bLockPrimary)		//	주무기 lock
	{
		if( !bLockSecondary)	//	보조무기 lock
		{
			if( bLockRPG7)											//	-------------	RPG7전
			{
				SET_ROOM_BATTLE_MARK_RPG7( m_pBattleMark);
			}
			else if( bLockMelee)									//	-------------	칼전
			{
				SET_ROOM_BATTLE_MARK_KNIFE( m_pBattleMark);
			}
			else if( bLockKnuckle)									//	-------------	너클전
			{
				SET_ROOM_BATTLE_MARK_KNUCKLE( m_pBattleMark);
			}
			else
			{
				I3ASSERT_0;
			}
		}
		else if(bLockRPG7)											//	-------------	RPG7전
		{
			SET_ROOM_BATTLE_MARK_RPG7( m_pBattleMark);
		}
		else														//	-------------	권총전
		{
			SET_ROOM_BATTLE_MARK_HANDGUN( m_pBattleMark);
		}
	}

	// 옵션 변경 알림
	_ProcessNotifyChangeOption(rDeltaSeconds);
}

void CPhaseReadyRoom::_ProcessSlot( REAL32 rDeltaSeconds)
{
	// 방 제목, 비밀번호 업데이트
	_UpdateTitlePassword();

	// 방장이면 킥버튼 활성화, 아니면 비활성화
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (_BeHost())
			m_slot[i]._pKick->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		else
			m_slot[i]._pKick->setControlState(I3GUI_CONTROL_STATE_DISABLED);


		// 방장이면 방장 마크 표시
		if (g_pGameContext->getMainSlotIdx() == i)
		{
			m_slot[i]._pKick->SetUserDefine(I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 4);
			m_slot[i]._pKick->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		}
		else
		{
			m_slot[i]._pKick->SetUserDefine(I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 3);
		}
	}


	// 슬롯 초기화
	for(UINT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_slot[i]._pOpen->SetEnable(TRUE);
		m_slot[i]._pClose->SetEnable(FALSE);
		m_slot[i]._pRank->SetEnable(FALSE);
		m_slot[i]._pNickname->SetEnable(FALSE);
		m_slot[i]._pClanRank->SetEnable(FALSE);
		m_slot[i]._pPcCafe->SetAllShapeEnable(FALSE);
		m_slot[i]._pClanMark->SetEnable(FALSE);
		m_slot[i]._pRankBeginner->SetEnable(FALSE);
		m_slot[i]._pESportLeft->SetAllShapeEnable(FALSE);
		m_slot[i]._pESportRight->SetAllShapeEnable(FALSE);

		m_slot[i]._pNickname->SetTextColor(GetColor(GCT_DEFAULT));
		m_slot[i]._pClanRank->SetTextColor(GetColor(GCT_DEFAULT));
	}


	// 슬롯 상태 확인
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		SLOT_INFO* pSlot = g_pGameContext->getSlotInfo(i);
		I3COLOR* pNickNameColor = 0;

		// 슬롯에 누군가가 있다.
		if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
		{
			// 상태표시
			_ShowSlotState(i);

			pNickNameColor = (COLOR*)&g_ColorCashItem[ g_pGameContext->getNickColorForSlot(i) ];

			// 닉네임
			m_slot[i]._pNickname->SetTextEllipsis(g_pGameContext->getNickForSlot(i));
			m_slot[i]._pNickname->SetTextColor( (COLOR*)pNickNameColor );
			m_slot[i]._pNickname->SetEnable(TRUE);

			// 계급을 표시합니다.
			_ShowSlotRankMark(i);

			// 자기 슬롯 강조
			if (g_pGameContext->getMySlotIdx() == i)
			{
				m_slot[i]._pNickname->SetTextColor( (COLOR*)pNickNameColor );
				m_slot[i]._pClanRank->SetTextColor( (COLOR*)pNickNameColor );
			}

			// 클랜 가입시 클랜마크 표시
			_SetClanInfoInSlot( i, pSlot);

			// PC방 표시
			if (pSlot->_pcCafe != PC_NOT_CAFE)
			{
				switch(pSlot->_pcCafe)
				{
				case PC_NORMAL_CAFE:
					m_slot[i]._pPcCafe->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
					break;
				case PC_PREMIUM_CAFE:
					m_slot[i]._pPcCafe->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
					break;
				case PC_VACATION_EVENT_ITEM:
					m_slot[i]._pPcCafe->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
					break;
				case PC_INDO_TELCOM_ITEM:
					// I3ASSERT_0 피함.
					break;
				case 0xFF:
					break;
				default:
					I3ASSERT_0;
					break;
				}
			}


			//	유저 E-Sport 태그

			m_slot[i]._pESportLeft->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게
			m_slot[i]._pESportRight->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게

			switch( pSlot->_eSportID)
			{

			case ESPORT_ID_LEVEL_BEGINNER1:	// 유저의 E-Sport 태그
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
					}
				}
				break;
			case ESPORT_ID_LEVEL_BEGINNER2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
					}
				}
				break;
			case ESPORT_ID_LEVEL_BEGINNER3:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_SUBSTITUTE1:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_SUBSTITUTE2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_STARTER1:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 5, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 5, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_STARTER2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 6, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 6, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_STAR1:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 7, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 7, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_STAR2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 8, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 8, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_STAR3:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 9, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 9, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_SUPERSTAR1:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 10, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 10, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_SUPERSTAR2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 11, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 11, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_SUPERSTAR3:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 12, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 12, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_SUPERSTAR4:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 13, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 13, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_WORLDCLASS1:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 14, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 14, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_WORLDCLASS2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 15, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 15, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_WORLDCLASS3:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 16, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 16, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_LEGEND1:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 17, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 17, TRUE);
					}
				}
				break;

			case ESPORT_ID_LEVEL_LEGEND2:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 18, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 18, TRUE);
					}
				}
				break;


			case ESPORT_ID_LEVEL_HALLOFFAME:
				{

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(GUI_SHAPE_OFFSET + 19, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{	
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(GUI_SHAPE_OFFSET + 19, TRUE);
					}
				}
				break;






				/*
			case ESPORT_ID:	// 유저의 E-Sport 태그
				{
					m_slot[i]._pESportLeft->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게
					m_slot[i]._pESportRight->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게

					// PC방 태그가 없으므로 오른편에 붙어있는 ESport Right 태그를 사용한다.
					if( pSlot->_pcCafe == PC_NOT_CAFE)
					{
						m_slot[i]._pESportLeft->SetEnable( FALSE);

						m_slot[i]._pESportRight->SetEnable( TRUE);
						m_slot[i]._pESportRight->SetShapeEnable(0, TRUE);
					}
					// PC방 태그가 있으므로 왼편에 붙어있는 ESport Left 태그를 사용한다.
					else
					{							
						m_slot[i]._pESportLeft->SetEnable( TRUE);
						m_slot[i]._pESportLeft->SetShapeEnable(0, TRUE);

						m_slot[i]._pESportRight->SetEnable( FALSE);
					}
				}
				break;*/
			}

		}
		else
		{
			// 상태표시
			m_slot[i]._pState->SetEnable(FALSE);

			// 선택된 슬롯이 비거나 킥당하면 출력 정보 갱신
			if (g_pGameContext->GetCurrentInfoSlot() == i)
			{
				_InitUserInfo();
			}

			// 닫혀있는 슬롯 처리
			if (SLOT_STATE_CLOSE == pSlot->_State)
			{
				m_slot[i]._pOpen->SetEnable(FALSE);
				m_slot[i]._pClose->SetEnable(TRUE);
			}
		}
	}


	// 방정검색 기능을 막는다.
	BOOL bEnableSearchHost;

	// 게임 중이면 방장검색 기능을 막는다.
	INT32 hostIdx = g_pGameContext->getMainSlotIdx();

	SLOT_INFO* info = g_pGameContext->getSlotInfo(hostIdx);
	if (SLOT_STATE_READY < info->_State)
	{
		bEnableSearchHost = FALSE;
	}
	else
	{
		bEnableSearchHost = TRUE;
	}


	// 팀변경은 혼자 있어도 가능
	if (_BeHost())
	{
		m_pTeamChange->setControlDisable(!bEnableSearchHost);
		m_pClanBattleBottomTeamChange->setControlDisable(!bEnableSearchHost);
	}
	else
	{
		m_pTeamChange->setControlDisable(TRUE);
		m_pClanBattleBottomTeamChange->setControlDisable(TRUE);
	}


	// 게임 중이 아니라도, 혼자 있으면 방장검색 기능을 막는다.
	if (bEnableSearchHost)
	{
		INT32 count = 0;
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			SLOT_INFO* info = g_pGameContext->getSlotInfo(i);

			if (SLOT_STATE_CLOSE < info->_State)
			{
				count++;
			}
		}

		if (1 < count)
		{
			bEnableSearchHost = TRUE;
		}
		else
		{
			bEnableSearchHost = FALSE;
		}
	}

	m_pSearchHost->setControlDisable(!bEnableSearchHost);
}

void  CPhaseReadyRoom::_ProcessChat( REAL32 rDeltaSeconds)
{
	m_pChatBody->OnUpdate( rDeltaSeconds, this);

}

void CPhaseReadyRoom::_ProcessClanBattle(REAL32 rDeltaSeconds)
{
	char szRedTeam[MAX_STRING_COUNT] = "";
	char szBlueTeam[MAX_STRING_COUNT] = "";
	const char * pszRedClan = "";
	const char * pszBlueClan = "";
	INT32 nRedCount = 0, nBlueCount = 0;

#if defined( TEST_CLAN_BATTLE)
	pszRedClan = "RED DUMMY";
	i3String::Copy(szRedTeam, GAME_STRING("STR_TBL_GUI_READY_ALPHA_TEAM"));/*알파 팀*/

	pszBlueClan = "BLUE DUMMY";
	i3String::Copy(szBlueTeam, GAME_STRING("STR_TBL_GUI_READY_BRAVO_TEAM"));/*브라보 팀*/

#else
	//	클랜전이 아니면 처리안한다.
	if( !g_pFramework->GetClanContext()->InBattleTeam())
		return;

	const CLAN_BATTLE_TEAM_INFO * pRedTeam = NULL, * pBlueTeam = NULL;

	const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	CLAN_BATTLE_TEAM_INFO * pReqTeam = g_pFramework->GetClanContext()->GetClanBattleReqTeamInfo();
	I3ASSERT( pMyTeam != NULL && pReqTeam != NULL);

	//	슬롯에 유저가 있는지 판단
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		SLOT_INFO* pSlot = g_pGameContext->getSlotInfo(i);

		// 슬롯에 누군가가 있다.
		if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
		{
			if (0 == i % 2)		//	red
				nRedCount++;
			else
				nBlueCount++;
		}
	}

	if (0 == g_pGameContext->getMySlotIdx() % 2)		//	red
	{
		pRedTeam = pMyTeam;
		pBlueTeam = pReqTeam;
	}
	else												//	blue
	{
		pRedTeam = pReqTeam;
		pBlueTeam = pMyTeam;
	}

	if( nRedCount > 0)
	{
		pszRedClan = pRedTeam->_Clan._ClanName;
		GetBattleTeamName(szRedTeam, MAX_STRING_COUNT, pRedTeam->_Team._Naming);
	}

	if( nBlueCount > 0)
	{
		pszBlueClan = pBlueTeam->_Clan._ClanName;
		GetBattleTeamName(szBlueTeam, MAX_STRING_COUNT, pBlueTeam->_Team._Naming);
	}

#if defined( USE_CLAN_MARK)
	//	Red
	if( pRedTeam->_Clan._ClanIdx > 0)
	{
		m_pClanBattleInfoRedClanMark->SetEnable(TRUE);

		SetClanMarkTexEx(m_pClanBattleInfoRedClanMark, CLAN_MARK_MEDIUM, pRedTeam->_Clan._ClanMark);
	}
	else
	{
		m_pClanBattleInfoRedClanMark->SetEnable(FALSE);
	}

	// Blue
	if( pBlueTeam->_Clan._ClanIdx > 0)
	{
		m_pClanBattleInfoBlueClanMark->SetEnable(TRUE);

		SetClanMarkTexEx(m_pClanBattleInfoBlueClanMark, CLAN_MARK_MEDIUM, pBlueTeam->_Clan._ClanMark);
	}
	else
	{
		m_pClanBattleInfoBlueClanMark->SetEnable(FALSE);
	}
#endif

#endif

	m_pClanBattleInfoRedClanName->SetTextEllipsis(pszRedClan);
	m_pClanBattleInfoRedTeamName->SetTextEllipsis(szRedTeam);

	m_pClanBattleInfoBlueClanName->SetTextEllipsis(pszBlueClan);
	m_pClanBattleInfoBlueTeamName->SetTextEllipsis(szBlueTeam);

}

void CPhaseReadyRoom::_InitUserInfo(void)
{
	if (-1 == m_nOldViewSlot || g_pGameContext->GetCurrentInfoSlot() != m_nOldViewSlot)
	{
		USER_INFO_BASIC MyUserInfoBasic;
		g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

		i3mem::Copy(g_pGameContext->SetUserInfoBasic(),		&MyUserInfoBasic,							sizeof(USER_INFO_BASIC));
		i3mem::Copy(g_pGameContext->SetUserInfoRecord(),	g_pGameContext->GetMyUserInfoRecord(),		sizeof(USER_INFO_RECORD));

		g_pGameContext->SetCurrentInfoSlot( g_pGameContext->getMySlotIdx());
		_Update3DCharacter();

		if (0 == g_pGameContext->getMySlotIdx() % 2)
		{
			UINT32 idxRedSlot = g_pGameContext->getMySlotIdx() / 2;

			m_pRedSelect->setPositionY(idxRedSlot * 24.0f + 10.0f);
			m_pRedSelect->SetEnable(TRUE);
			m_pBlueSelect->SetEnable(FALSE);
		}
		else
		{
			UINT32 idxBlueSlot = (g_pGameContext->getMySlotIdx() / 2);
			m_pBlueSelect->setPositionY(idxBlueSlot * 24.0f + 10.0f);
			m_pBlueSelect->SetEnable(TRUE);
			m_pRedSelect->SetEnable(FALSE);
		}
	}
}

void CPhaseReadyRoom::_ShowSlotState(INT32 idxSlot)
{
	SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(idxSlot);

	m_slot[idxSlot]._pState->SetAllShapeEnable(FALSE);

	if (SLOT_STATE_READY < pInfo->_State)
	{
		m_slot[idxSlot]._pState->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
	}
	else
	{
		switch(pInfo->_State)
		{
		case SLOT_STATE_CLAN:		m_slot[idxSlot]._pState->SetShapeEnable(GUI_SHAPE_OFFSET + 5, TRUE);	break;
		case SLOT_STATE_INVENTORY:	m_slot[idxSlot]._pState->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);	break;
		case SLOT_STATE_INFO:		m_slot[idxSlot]._pState->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);	break;
		case SLOT_STATE_SHOP:		m_slot[idxSlot]._pState->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);	break;
		case SLOT_STATE_READY:		m_slot[idxSlot]._pState->SetShapeEnable(GUI_SHAPE_OFFSET, TRUE);		break;
		}
	}

	m_slot[idxSlot]._pState->SetEnable(TRUE);
}

void CPhaseReadyRoom::_ShowSlotRankMark(INT32 idxSlot)
{
	//초보 채널에 대한 처리
	if( g_pGameContext->IsBeginnerChannel1() )
	{/*
		m_slot[idxSlot]._pRank->SetEnable(FALSE);
		m_slot[idxSlot]._pRankBeginner->SetEnable(TRUE);
		*/
		UINT32 Rank = RANK_NEWBIE;
		m_slot[idxSlot]._pRank->SetShapeUVIndex(Rank);
		m_slot[idxSlot]._pRank->SetEnable(TRUE);
	
	}
	else
	{
		m_slot[idxSlot]._pRankBeginner->SetEnable(FALSE);

		UINT32 Rank = g_pGameContext->getSlotInfo(idxSlot)->_Rank;
		m_slot[idxSlot]._pRank->SetShapeUVIndex(Rank);
		m_slot[idxSlot]._pRank->SetEnable(TRUE);
	}
}

BOOL CPhaseReadyRoom::_IsReady(void)
{
	if (g_pGameContext->IsStageMaskTraining())
	{
		// 훈련전은 상대팀이 없어야한다.
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(i);

			if (pInfo->_State != SLOT_STATE_CLOSE && pInfo->_State != SLOT_STATE_EMPTY)
			{
				INT32 myTeam = g_pGameContext->getMySlotIdx() % 2;
				INT32 slotTeam = i % 2;

				if (myTeam != slotTeam)
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}
	else
	{
		BOOL bBeRed = FALSE;
		BOOL bBeBlue = FALSE;

		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(i);

			if( SLOT_STATE_EMPTY != pInfo->_State && SLOT_STATE_CLOSE != pInfo->_State)
			{
				if( 0 == i % 2)
				{
					bBeRed = TRUE;
				}
				else
				{
					bBeBlue = TRUE;
				}
			}
		}

#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE) //클라이언트 혼자 실행하기
		return TRUE;
#else
		return bBeRed && bBeBlue;
#endif
	}
}

void CPhaseReadyRoom::_SlotClick(INT32 idxSlot)
{
	SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(idxSlot);

	if (SLOT_STATE_EMPTY == pInfo->_State)
	{
		//	클랜전이 아닐 경우만 가능하도록
		if( !g_pFramework->GetClanContext()->InBattleTeam())
		{
			INT32 nTeam;

			if (0 == idxSlot % 2)
				nTeam = TEAM_RED;
			else
				nTeam = TEAM_BLUE;

			_ChangeTeam(nTeam);
		}

	}
	else if (SLOT_STATE_CLOSE != pInfo->_State)
	{
		_GetUserInfo(idxSlot);
	}


	// 관물대 캐릭터 처리
	if (g_pGameContext->GetCurrentInfoSlot() == idxSlot)
	{
		UpdateCharacter(VT_SLOT);
	}
}

void CPhaseReadyRoom::_ChangeTeam(INT32 nTeamType)
{
	INT32 idxMySlot = g_pGameContext->getMySlotIdx();
	SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(idxMySlot);


	// 현재 팀과 같으면 이동할 필요없다
	if (nTeamType == idxMySlot % 2)
	{
		return;
	}

	// Ready 중엔 팀 변경 금지
	if (SLOT_STATE_READY == pInfo->_State)
	{
		((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_CHANGE_SLOT_S"));
		return;
	}

	m_bChagingTeam = TRUE;
	m_nOldTeam = g_pGameContext->getMySlotIdx() % 2;
	g_pGameContext->SetEvent(EVENT_CHANGE_TEAM_N , &nTeamType);
}

void CPhaseReadyRoom::_GetUserInfo(INT32 idxGameSlot)
{
	if( g_pGameContext->getMySlotIdx() == idxGameSlot)
	{
		if (g_pGameContext->GetCurrentInfoSlot() != idxGameSlot)
		{
			m_nOldViewSlot = -1;
			_InitUserInfo();
		}
	}
	else
	{
		if( g_pGameContext->GetCurrentInfoSlot() != idxGameSlot)
		{
			g_pGameContext->SetEvent(EVENT_GET_USERINFO_N, &idxGameSlot);
		}
	}
}

void CPhaseReadyRoom::EhKicked(void)
{
	if (FALSE == m_bKickingState)
	{
		m_bKickingState = TRUE;

		if (false == g_pGameContext->IsKicking())
		{
			ERROR_POPUP(GAME_STRING("STBL_IDX_EP_ROOM_KICKED"));
			g_pGameContext->AddGameFlag( GAMEFLAG_KICKBYHOST);
		}

		i3GuiRoot::setGuiDisableInput(TRUE);
		g_pGameContext->SetEventOnce(EVENT_ENTER_LOBBY);
		m_bEnableUpdate = FALSE;
	}
}

void CPhaseReadyRoom::EhNoRealIP(void)
{
	((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_REAL_IP_S"));
}

void CPhaseReadyRoom::EhNoReadyTeam(void)
{
	((CGameFramework*)g_pFramework)->PutSystemChatting(GAME_STRING("STBL_IDX_EP_ROOM_NO_READY_TEAM_S"));
}

void CPhaseReadyRoom::EhStageError(INT32 Event,INT32 Arg)
{
	if( EV_IS_ERROR(Arg, EVENT_ERROR_GAME_WRONG_POSITION))
	{
		((CStageReady*)m_pParent)->MovePhase(PHASE_LOBBY);
	}
}

void CPhaseReadyRoom::CbBeHost(void* pThis,INT32 nParam)
{
	((CPhaseReadyRoom*)pThis)->m_pBeHostMessageBox = NULL;
}

void CPhaseReadyRoom::CbSearchHost(void* pThis,INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		INT32 slotIdx = ((CPhaseReadyRoom*)pThis)->m_transferHostSlotIdx;
		g_pGameContext->SetEvent(EVENT_TRANSFER_HOST, &slotIdx);
	}
}

void CPhaseReadyRoom::_InitCommon(void)
{
	m_pRoomGui->SetEnable(FALSE);

#if defined(ENABLE_BOLD_CAPTION)
	m_pRoomCaption->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_CAPTION_SIZE, FALSE, NULL, FW_BOLD);
#endif
	m_pRoomCaption->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRoomCaption->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRoomCaption->SetOffsetPos(15.0f, 0.0f);

	m_pKey->SetEnable(FALSE);
	m_pBattleMark->SetEnable(FALSE);

	INIT_BUTTON_EX(m_pStart);
	INIT_BUTTON_EX(m_pReady);
	INIT_BUTTON_EX(m_pCancelStart);
	INIT_BUTTON_EX(m_pCancelReady);
	INIT_BUTTON_EX(m_pInvite);
	INIT_BUTTON_EX(m_pLeave);
	INIT_BUTTON_EX(m_pSearchHost);
	INIT_BUTTON_EX(m_pTeamChange);
	
	m_teamChangePosX = m_pTeamChange->getPositionX();
}

void CPhaseReadyRoom::_InitSlot(void)
{
	// 상자의 한쪽 모서리만 4개인 shape를 회전시켜 아귀를 맞춘다
	m_pRedBg->SetShapeFlip(GUI_SHAPE_OFFSET + 1, TRUE, FALSE);
	m_pRedBg->SetShapeFlip(GUI_SHAPE_OFFSET + 2, FALSE, TRUE);
	m_pRedBg->SetShapeFlip(GUI_SHAPE_OFFSET + 3, TRUE, TRUE);
	m_pBlueBg->SetShapeFlip(GUI_SHAPE_OFFSET + 0, TRUE, FALSE);
	m_pBlueBg->SetShapeFlip(GUI_SHAPE_OFFSET + 2, TRUE, TRUE);
	m_pBlueBg->SetShapeFlip(GUI_SHAPE_OFFSET + 3, FALSE, TRUE);

	m_pRedList->CreateTextEx(8,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pRedList->SetSelColor(0, 0, 0, 0);
	m_pRedList->SetFixedElementCount(TRUE);
	m_pBlueList->CreateTextEx(8,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pBlueList->SetSelColor(0, 0, 0, 0);
	m_pBlueList->SetFixedElementCount(TRUE);

	for(UINT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_slot[i]._pClose->SetEnable(FALSE);
		GameGUI::SetRankStatic(m_slot[i]._pRank);
		m_slot[i]._pRankBeginner->SetEnable(FALSE);
		m_slot[i]._pState->SetEnable(FALSE);

		m_slot[i]._pNickname->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_slot[i]._pNickname->SetTextColor(GetColor(GCT_DEFAULT));
		m_slot[i]._pNickname->SetOffsetPos(2.0f, 0.0f);

		INIT_BUTTON_EX(m_slot[i]._pKick);

		m_slot[i]._pPcCafe->SetAllShapeEnable(FALSE);
		m_slot[i]._pClanRank->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
#ifdef LOCALE_CHINA
		m_slot[i]._pClanRank->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
		m_slot[i]._pClanRank->ReCreateTextEx( GetDefaultFontName(), m_slot[i]._pClanRank->getMaxTextCount(), 8, FALSE, NULL);
#endif

		m_slot[i]._pClanRank->SetTextColor(GetColor(GCT_DEFAULT));
		m_slot[i]._pClanMark->SetEnable(FALSE);

		m_slot[i]._pESportLeft->SetAllShapeEnable(FALSE);
		m_slot[i]._pESportRight->SetAllShapeEnable(FALSE);
	}

	m_pRedSelect->SetEnable(FALSE);
	m_pRedFocus->SetEnable(FALSE);
	m_pBlueSelect->SetEnable(FALSE);
	m_pBlueFocus->SetEnable(FALSE);
}

void CPhaseReadyRoom::_InitInfo(void)
{
	g_pFramework->SetMapShape(m_pMapImage, STAGE_ID_NA);
	m_pTypeImage->SetAllShapeEnable(FALSE);
	m_pTypeImage->setInputDisable(TRUE);
	m_pMapNewMark->setInputDisable(TRUE);
	m_pMapXMasMark->setInputDisable(TRUE);
	m_pMapCircle->SetAllShapeEnable(FALSE);
	m_pMapCircle->SetShapeEnable(0, TRUE);
	m_pMapCircle->setInputDisable(TRUE);
	m_pMapCircle->SetEnable(FALSE);



	INIT_COMBO_EX(m_pMapCombo);

	m_pMapList->CreateTextEx( STAGE_ORD_MAX,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pMapList->SetSelColor(100, 100, 100, 100);
	m_pMapList->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMapList->SetExclusiveEnableControl(TRUE);
	m_pMapList->SetFixedElementCount(FALSE);
	m_pMapList->SetEnable(FALSE);
	m_pMapList->AddElement("none");

	// 맵 선택
	m_pMap->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMap->SetTextColor(GetColor(GCT_DEFAULT));

	// 방제목
	m_pTitleKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTitleKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTitleKey->SetText(GAME_STRING("STR_TBL_GUI_READY_ROOM_SUBJECT"));/*방제목*/

	INIT_COMBO_EX(m_pTitleChange);

	// 비밀번호
	m_pPasswordKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPasswordKey->SetTextColor(GetColor(GCT_STATIC));
	m_pPasswordKey->SetText(GAME_STRING("STR_TBL_GUI_READY_PASSWORD"));/*비밀번호*/
	INIT_COMBO_EX(m_pPasswordChange);

	// 게임모드
	m_pModeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pModeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pModeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_GAME_MODE"));/*게임모드*/
	m_pModeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pModeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pModePrev);
	INIT_COMBO_EX(m_pModeNext);

	// 승리조건
	m_pTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTypeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_VICTORY_CONDITION"));/*승리조건*/
	m_pTypeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTypeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pTypePrev);
	INIT_COMBO_EX(m_pTypeNext);

	// 제한시간
	m_pTimeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTimeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTimeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_LIMIT_TIME2"));/*제한시간*/
	m_pTimeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTimeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pTimePrev);
	INIT_COMBO_EX(m_pTimeNext);
	// 랜덤맵
	INIT_CAPTION_BUTTON(m_pRandomMap, GAME_STRING("STR_POPUP_RANDOMMAP_CONFIG"));
	// 고급옵션
	m_pOptionKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pOptionKey->SetTextColor(GetColor(GCT_STATIC));
	m_pOptionKey->SetText(GAME_STRING("STR_TBL_GUI_READY_HIGHRANK_OPTION"));/*고급옵션*/

	const char * OptionText = GAME_STRING("STR_TBL_GUI_READYROOM_OPTION");

	INIT_CAPTION_BUTTON(m_pOption, OptionText);

	m_pLimitMelee->SetUserDefine(I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 3);


	// 훈련전 제한시간
	m_pTrainingTimeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingTimeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTrainingTimeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_LIMIT_TIME2"));/*제한시간*/
	m_pTrainingTimeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingTimeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pTrainingTimePrev);
	INIT_COMBO_EX(m_pTrainingTimeNext);

	// 훈련전 적군 수
	m_pTrainingEnemyKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingEnemyKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTrainingEnemyKey->SetText(GAME_STRING("STR_TBL_GUI_READY_ENEMY_COUNT"));/*적군 수*/
	m_pTrainingEnemyValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingEnemyValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pTrainingEnemyPrev);
	INIT_COMBO_EX(m_pTrainingEnemyNext);

#if defined(USE_AI_MODE_DIEHARD)
	m_pTrainingEnemyKey->SetEnable( FALSE);
	m_pTrainingEnemyValue->SetEnable( FALSE);
	m_pTrainingEnemyPrev->SetEnable( FALSE);
	m_pTrainingEnemyNext->SetEnable( FALSE);
#endif

	// 훈련전 난이도
	m_pTrainingDifficultyKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingDifficultyKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTrainingDifficultyKey->SetText(GAME_STRING("STR_TBL_GUI_READY_DIFFICULTY"));/*난이도*/
	m_pTrainingDifficultyValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingDifficultyValue->SetTextColor(GetColor(GCT_DEFAULT));


	INIT_COMBO_EX(m_pTrainingDifficultyCombo);

#if defined(USE_AI_MODE_DIEHARD)
	m_pTrainingDifficultyKey->SetEnable( FALSE);
	m_pTrainingDifficultyValue->SetEnable( FALSE);
	m_pTrainingDifficultyCombo->SetEnable( FALSE);
#endif

	m_pTrainingDifficultyList->CreateTextEx( TRAINING_DIFFICULTY_LEVEL_HIGH,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pTrainingDifficultyList->SetSelColor(100, 100, 100, 100);
	m_pTrainingDifficultyList->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTrainingDifficultyList->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTrainingDifficultyList->SetExclusiveEnableControl(TRUE);
	m_pTrainingDifficultyList->SetFixedElementCount(FALSE);
	m_pTrainingDifficultyList->SetEnable(FALSE);

	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_J"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_I"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_H"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_G"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_F"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_E"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_D"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_C"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_B"));
	m_pTrainingDifficultyList->AddElement(GAME_STRING("STR_TBL_GUI_READY_GRADE_A"));


	// 방 제목 IME
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_ROOM_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pTitleValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pTitleValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTitleValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTitleValue->SetIMEMode( IME_MODE_NATIVE);
	m_pTitleValue->SetTextAutoWrap( FALSE);

	m_pTitleValue->SetOffsetPos(3.0f, 0);


	// 비밀번호 IME
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_ROOM_PW;	// NULL종료 문자열이 아니다! 4byte 바이너리!
	ime._StyleIME = IME_STYLE_NUMERIC_ONLY | IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
	m_pPasswordValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pPasswordValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPasswordValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPasswordValue->SetAlphaNumericOnly( true);
	m_pPasswordValue->SetTextAutoWrap( FALSE);

	m_pChkObserver->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pChkObserver->SetEnable(FALSE);

	m_pEditObserver->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pEditObserver->SetTextColor(GetColor(GCT_DEFAULT));
	m_pEditObserver->SetText(GAME_STRING("STR_TBL_GUI_OBSERVER"));/*관전기능*/
	m_pEditObserver->SetEnable(FALSE);
}

void CPhaseReadyRoom::_InitChat(void)
{
	m_pChatBody->InitChatBody( CHAT_BODY_STATE_READYROOM, MAX_CHAT_LINE_FOR_ROOM);
}

void CPhaseReadyRoom::_CheckBeHost(void)
{
	if (!_BeHost()) m_bIsHost = FALSE;

	// 이미 방장이면 처리하지 않는다
	if (m_bIsHost)	return;

	// 방장이 되었다면 팝업 출력
	if (_BeHost())
	{	
#ifdef USE_ROOM_MASTER_KICK
		if (!g_pGameContext->IsTeamChangeFlag())
		{
			char strBuf[MAX_STRING_COUNT];
			sprintf_s(strBuf, GAME_STRING("STBL_IDX_EP_ROOM_BE_HOST"), g_pGameContext->getNickForSlot(g_pGameContext->getMySlotIdx()));

			m_pBeHostMessageBox = STRING_POPUP_C(strBuf, MAKE_CALLBACK(CbBeHost), this);

			m_bIsHost = TRUE;
			((CStageReady*)m_pParent)->Set_bBeHostMessageOK(TRUE);
		}
#else
		if (!g_pGameContext->IsTeamChangeFlag())
		{
			char strBuf[MAX_STRING_COUNT];
			sprintf_s(strBuf, GAME_STRING("STBL_IDX_EP_ROOM_BE_HOST"), g_pGameContext->getNickForSlot(g_pGameContext->getMySlotIdx()));

			m_pBeHostMessageBox = STRING_POPUP_C(strBuf, MAKE_CALLBACK(CbBeHost), this);

			m_bIsHost = TRUE;
		}
#endif
	}
}

BOOL CPhaseReadyRoom::_BeHost(void)
{
	return g_pGameContext->getMainSlotIdx() == g_pGameContext->getMySlotIdx();
}

void CPhaseReadyRoom::_UpdateMapInfo( ROOM_INFO_ALL * pInfo)
{
	ROOM_INFO_ALL Info;
	i3mem::Copy( &Info._Basic,	&pInfo->_Basic, sizeof( ROOM_INFO_BASIC ));
	i3mem::Copy( &Info._Add,	&pInfo->_Add,	sizeof( ROOM_INFO_ADD ));

	BOOL bProcess = TRUE;
	
	// 변경된 모드에 따른 기본 설정 변경
	STAGE_TYPE type = getStageType( Info._Basic._StageID);
	
#if defined (USE_RANDOMMAP_MODE)
	//랜덤맵일 경우 맵타입이 바뀌지않는한 맵설정이 초기화 대지 않도록 합니다.
	if(!g_pGameContext->IsRandomMap() || g_pGameContext->GetPrevStageType() != type && g_pGameContext->IsRandomMap() )
		bProcess = TRUE;
	else
		bProcess = FALSE;
#else
	bProcess = TRUE;
#endif
	
	if( bProcess)
	{
		switch( type)
		{
		case STAGE_TYPE_ANNIHILATION :
		case STAGE_TYPE_SUDDEN_DEATH :
		case STAGE_TYPE_HEAD_KILLER :
		case STAGE_TYPE_BOMB :
			Info._Add._SubType	= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;
			break;
		case STAGE_TYPE_ESCAPE :

			Info._Add._SubType	= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;
			break;
		case STAGE_TYPE_DESTROY :
			Info._Add._SubType	= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
			break;

		case STAGE_TYPE_DEFENCE :	// 방어전은 주무기는 기본으로 설정
			/*Info._Basic._WeaponFlag = */SETBIT(Info._Basic._WeaponFlag, LOCK_PRIMARY);	
			Info._Add._SubType	= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
			break;
	
		default :
			Info._Add._SubType	= BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
			break;
		}
		g_pGameContext->SetPrevStageType(type);
	}
	
	// RPG7 모드 제거
	if( getStageSlot(Info._Basic._StageID) != STAGE_SLOT_MODE_4VS4)
	{
		if( g_pConfigEx->GetGame().Enable_RPG7 == FALSE || type == STAGE_TYPE_DESTROY || type == STAGE_TYPE_DEFENCE ||
			(g_pConfigEx->GetGame().Enable_RPG7 == TRUE && g_pConfigEx->GetGame().UnlimitedForRPG7 == FALSE))
		{
			if(TESTBIT(Info._Basic._WeaponFlag ,	LOCK_RPG7))
			{
				Info._Basic._WeaponFlag |= LOCK_PRIMARY;
				Info._Basic._WeaponFlag &= ~LOCK_RPG7;
			}
		}
	}
	
	g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &Info );
}

void CPhaseReadyRoom::_RotateModeIndex(BOOL bNext,BOOL bRotateMap)
{
	// 방 정보 사본 준비
	ROOM_INFO_ALL info;
	
	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);

	STAGE_TYPE type = getStageType( info._Basic._StageID);
	g_pGameContext->SetPrevStageType(type);
	info._Basic._StageID = getNextStageType( (STAGE_ID) info._Basic._StageID, bNext == TRUE);	

	_UpdateMapInfo( &info);
}

void CPhaseReadyRoom::_RotateTypeIndex(BOOL bNext)
{
	// 방 정보 사본 준비
	ROOM_INFO_ALL info;
	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);

	{
		UINT8 idxMin;
		UINT8 idxMax;

		UINT8 stageType	= (UINT8)getStageType( info._Basic._StageID);

		// 시작/끝 인덱스 설정
		switch( stageType)
		{
		case STAGE_TYPE_ANNIHILATION	:
		case STAGE_TYPE_SUDDEN_DEATH	:
		case STAGE_TYPE_HEAD_KILLER	:
		case STAGE_TYPE_BOMB	:
		case STAGE_TYPE_DESTROY :
		case STAGE_TYPE_DEFENCE	:
			idxMin = BATTLE_ROUND_TYPE_3;
			idxMax = BATTLE_ROUND_TYPE_9;
			break;
		case STAGE_TYPE_ESCAPE	:
			idxMin = BATTLE_TIME_MISSION_TYPE_5;
			idxMax = BATTLE_TIME_MISSION_TYPE_5;
			break;
		default					:
			idxMin = BATTLE_KILL_TYPE_60;
			idxMax = BATTLE_KILL_TYPE_160;
			break;
		}

		{
			UINT8 gameTime = HINIBBLE(info._Add._SubType);
			UINT8 gameType = LONIBBLE(info._Add._SubType);

			// 타입 순환
			if (bNext)
				gameType = LOOP_INCREASE(gameType, idxMin, idxMax);
			else
				gameType = LOOP_DECREASE(gameType, idxMin, idxMax);

			info._Add._SubType = MAKEBYTE(gameTime, gameType);
		}
	}

	g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &info);
}

void CPhaseReadyRoom::_RotateTimeIndex(BOOL bNext)
{
	// 방정보 사본 준비
	ROOM_INFO_ALL info;
	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);

	STAGE_TYPE stageType = getStageType( info._Basic._StageID );
	
	info._Add._SubType = CReadyPhaseBase::_RotateTimeIndex(stageType, info._Add._SubType, bNext);

	g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &info);
}

void CPhaseReadyRoom::_RotateTrainingTime(void)
{
	// 사본 준비
	ROOM_INFO_ALL info;
	g_pGameContext->getMyRoomInfo(&info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&info._Add);


	// 값 설정 - 토글
	UINT8 gameTime = HINIBBLE(info._Add._SubType);
	UINT8 gameType = LONIBBLE(info._Add._SubType);

	if (gameTime == HINIBBLE(BATTLE_TIME_TYPE_5))
	{
		gameTime = HINIBBLE(BATTLE_TIME_TYPE_30);
	}
	else
	{
		gameTime = HINIBBLE(BATTLE_TIME_TYPE_5);
	}

	info._Add._SubType = MAKEBYTE(gameTime, gameType);


	// 적용 요청
	g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &info);
}

void CPhaseReadyRoom::_RotateTrainingEnemy(BOOL bNext)
{
	// 사본 준비
	// 사본 준비
	ROOM_INFO_BASIC infoBasic;
	g_pGameContext->getMyRoomInfo(&infoBasic);

	INT32 enemyCount = g_pGameContext->getEnemyAiCount();


	// 값 설정
	if (bNext)
	{
		if (MAX_SLOT_PER_TEAM <= enemyCount)
		{
			enemyCount = 1;
		}
		else
		{
			enemyCount++;
		}
	}
	else
	{
		if (enemyCount <= 1)
		{
			enemyCount = MAX_SLOT_PER_TEAM;
		}
		else
		{
			enemyCount--;
		}
	}


	// 적용 요청
	g_pGameContext->setEnemyAiCount(enemyCount);
	g_pGameContext->SetEvent( EVENT_CHANGE_ROOMINFO_N, &infoBasic );
}

void CPhaseReadyRoom::_RotateTrainingDifficulty(BOOL bNext)
{
	// 사본
	// 사본 준비
	ROOM_INFO_BASIC infoBasic;
	g_pGameContext->getMyRoomInfo(&infoBasic);

	INT32 difficultyLevel = g_pGameContext->getDifficultyLevel();

	// 계산
	if (bNext)
	{
		if (difficultyLevel <= TRAINING_DIFFICULTY_LEVEL_HIGH)
		{
			difficultyLevel = TRAINING_DIFFICULTY_LEVEL_LOW;
		}
		else
		{
			difficultyLevel--;
		}
	}
	else
	{
		if (TRAINING_DIFFICULTY_LEVEL_LOW <= difficultyLevel)
		{
			difficultyLevel = TRAINING_DIFFICULTY_LEVEL_HIGH;
		}
		else
		{
			difficultyLevel++;
		}
	}

	// 적용
	g_pGameContext->setDifficultyLevel(difficultyLevel);
	g_pGameContext->setCurrentDifficultyLevel(difficultyLevel);
	g_pGameContext->SetEvent( EVENT_CHANGE_ROOMINFO_N, &infoBasic );
}

void CPhaseReadyRoom::_UpdateTitlePassword(BOOL bInit)
{
	ROOM_INFO_BASIC roomInfo;
	g_pGameContext->getMyRoomInfo(&roomInfo);

	// 방이름 설정
	{
		char szRoom[MAX_STRING_COUNT] = "";
		char szServer[MAX_STRING_COUNT] = "";
		char strTitle[MAX_STRING_COUNT] = "";

		//	현재 서버 위치 가져오기
		((CStageReady*)this->getParent())->GetCurrentSeverInfo( szServer, MAX_STRING_COUNT);

		//	몇번방
		sprintf_s( szRoom, GAME_STRING("STBL_IDX_ROOM_TEXT1"), roomInfo._Idx + 1);

		// 방 윈도우 캡션 - 방번호 포함
		sprintf_s(strTitle, "%s - %s: %s", szServer, szRoom, roomInfo._Title);
		m_pRoomCaption->SetText(strTitle);

		// 방 제목 변경창 - 방번호 제외
		if (bInit || FALSE == _BeHost())
			m_pTitleValue->SetIMEText(roomInfo._Title);
	}

	// 비밀번호 설정
	{
		char strPassword[NET_ROOM_PW + 1];

		// 비밀번호 값 설정 - NULL종료 문자열이 아니다! 4byte 바이너리!
		i3mem::FillZero(strPassword, sizeof(char) * (NET_ROOM_PW + 1));
		i3String::NCopy(strPassword, g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

		if (bInit || FALSE == _BeHost())
			m_pPasswordValue->SetIMEText(strPassword);
	}

	// 비밀방 표시
	if (GET_ROOM_INFO_PRIVATE_MODE( roomInfo._InfoFlag ) )
	{
		m_pKey->SetEnable(TRUE);
	}
	else
	{
		m_pKey->SetEnable(FALSE);
	}
}

void CPhaseReadyRoom::_InitRecord(void)
{
	// 윈도우 초기위치 기억해둔다.
	INIT_WIN_CAPTION(m_pRecordCaption, GAME_STRING("STR_TBL_GUI_READY_DETAIL_INFO1"));/*▼ 상세정보 ▼*/

	m_pRecordNickname->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRecordNickname->SetTextColor(GetColor(GCT_DEFAULT));
	GameGUI::SetRankStatic(m_pRecordRank);
	m_pRecordRank_Beginner->SetAllShapeEnable(FALSE);

	m_pSeasonKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonKey->SetText(GAME_STRING("STR_TBL_GUI_READY_SEASON_BATTLE_HISTORY"));/*시즌전적*/
	m_pTotalKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TOTAL_BATTLE_HISTORY"));/*통산전적*/

	m_pSeasonRecord->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonKillDeath->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonHeadshot->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonKillDeath2->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonDisconnect->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonRecord->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonKillDeath->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonHeadshot->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonKillDeath2->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonDisconnect->SetTextColor(GetColor(GCT_DEFAULT));

	m_pTotalRecord->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalKillDeath->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalHeadshot->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalKillDeath2->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalDisconnect->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalRecord->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalKillDeath->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalHeadshot->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalKillDeath2->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalDisconnect->SetTextColor(GetColor(GCT_DEFAULT));

	// 클랜 정보
	m_pClanKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanNameKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanNameValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanRankKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanRankValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanNameKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanNameValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanRankKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanRankValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanKey->SetText(GAME_STRING("STR_TBL_GUI_READY_CLAN_BATTLE_HISTORY"));/*클랜정보*/
	m_pClanNameKey->SetText(GAME_STRING("STR_TBL_GUI_READY_CLAN_NAME"));/*클랜명:*/
	m_pClanRankKey->SetText(GAME_STRING("STR_TBL_GUI_READY_CLAN_GRADE"));/*클랜등급:*/


	m_pClanMark->SetEnable(FALSE);
	m_pClanRank->SetAllShapeEnable(FALSE);
}

void CPhaseReadyRoom::_Update3DCharacter(void)
{
	if (g_pGameContext->GetCurrentInfoSlot() != m_nOldViewSlot)
	{
		m_nOldViewSlot = g_pGameContext->GetCurrentInfoSlot();
		UpdateCharacter(VT_SLOT, TRUE);
	}
}

void CPhaseReadyRoom::_InitAdvencedWin(void)
{
	m_pAdvencedWin->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pAdvencedCaption, GAME_STRING("STR_TBL_GUI_READY_HIGHRANK_OPTION"));/*고급옵션*/

	INIT_CAPTION_BUTTON(m_pAdvencedConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/
	INIT_CAPTION_BUTTON(m_pAdvencedCancel, GAME_STRING("STR_TBL_GUI_READY_CANCEL"));/*취소*/

	m_pLimitKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitKey->SetText(GAME_STRING("STR_TBL_GUI_READY_WEAPON_LIMIT"));/*무기제한*/

	INIT_CHECK_EX(m_pLimitPrimary);
	INIT_CHECK_EX(m_pLimitSecondary);
	INIT_CHECK_EX(m_pLimitMelee);
	INIT_CHECK_EX(m_pLimitThrow);
	INIT_CHECK_EX(m_pLimitRPG7);
	INIT_CHECK_EX(m_pLimitObserver3PV_Free);
	INIT_CHECK_EX(m_pLimitObserver1PV_Free);
	INIT_CHECK_EX(m_pLimitObserverEnemy_Free);
	INIT_CHECK_EX(m_pLimitObserverEnemyHP);

	m_pLimitPrimaryLock->SetEnable(FALSE);
	m_pLimitSecondaryLock->SetEnable(FALSE);
	m_pLimitMeleeLock->SetEnable(FALSE);
	m_pLimitThrowLock->SetEnable(FALSE);
	m_pLimitRPG7Lock->SetEnable(FALSE);

	m_pLimitPrimary->setCheckBoxState(FALSE);
	m_pLimitSecondary->setCheckBoxState(FALSE);
	m_pLimitMelee->setCheckBoxState(FALSE);
	m_pLimitThrow->setCheckBoxState(FALSE);
	m_pLimitRPG7->setCheckBoxState(FALSE);
	m_pLimitObserver3PV_Free->setCheckBoxState(FALSE);
	m_pLimitObserver1PV_Free->setCheckBoxState(FALSE);
	m_pLimitObserverEnemy_Free->setCheckBoxState(FALSE);
	m_pLimitObserverEnemyHP->setCheckBoxState(FALSE);

	m_pLimitPrimaryKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitSecondaryKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitMeleeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitThrowKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitRPG7Key->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitObserver3PV_FreeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitObserver1PV_FreeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitObserverEnemy_FreeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitObserverEnemyHPKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pLimitPrimaryKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitSecondaryKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitMeleeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitThrowKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitRPG7Key->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitObserver3PV_FreeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitObserver1PV_FreeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitObserverEnemy_FreeKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitObserverEnemyHPKey->SetTextColor(GetColor(GCT_DEFAULT));

	m_pLimitPrimaryKey->SetText(GAME_STRING("STR_TBL_GUI_READY_PRIMARY_WEAPON"));/*주무기*/
	m_pLimitSecondaryKey->SetText(GAME_STRING("STR_TBL_GUI_READY_SECONDRY_WEAPON"));/*보조무기*/
	m_pLimitMeleeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_MELLE_WEAPON"));/*근접무기*/
	m_pLimitThrowKey->SetText(GAME_STRING("STR_TBL_GUI_READY_THROW_WEAPON"));/*투척무기*/
	m_pLimitRPG7Key->SetText(GAME_STRING("STR_TBL_GUI_READY_RPG7_WEAPON"));/*RPG7*/
	m_pLimitObserver3PV_FreeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_OBSERVER_LIMIT1"));/*3인칭 자유시점*/
	m_pLimitObserver1PV_FreeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_OBSERVER_LIMIT2"));/*자유 관전시점*/
	m_pLimitObserverEnemy_FreeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_OBSERVER_LIMIT3"));/*상대진영 시점*/
	m_pLimitObserverEnemyHPKey->SetText(GAME_STRING("STR_TBL_GUI_READY_OBSERVER_LIMIT4"));/*상대진영HP*/

	m_pLimitJoinKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitJoinYesKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitJoinNoKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitObserverKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTeamBalanceKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTeamBalancePersonnelKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTeamBalanceAbilityKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTeamBalanceNoKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pLimitJoinKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitJoinYesKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitJoinNoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitObserverKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTeamBalanceKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTeamBalancePersonnelKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTeamBalanceAbilityKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTeamBalanceNoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitJoinKey->SetText(GAME_STRING("STR_TBL_GUI_READY_LIMIT_JOINING"));/*난입제한*/
	m_pLimitObserverKey->SetText(GAME_STRING("STR_TBL_GUI_READY_OBSERVER_LIMIT"));/*관전기능*/
	m_pTeamBalanceKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE"));/*팀밸런스*/
	m_pLimitJoinYesKey->SetText(GAME_STRING("STR_TBL_GUI_READY_BEING"));/*있음*/
	m_pTeamBalancePersonnelKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON"));/*있음*/
	m_pTeamBalanceAbilityKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"));/*있음*/
	m_pLimitJoinNoKey->SetText(GAME_STRING("STR_TBL_GUI_READY_NONE"));/*없음*/
	m_pTeamBalanceNoKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_OFF"));/*없음*/

	INIT_CHECK_EX(m_pLimitJoinYes);
	INIT_CHECK_EX(m_pLimitJoinNo);
	INIT_CHECK_EX(m_pTeamBalancePersonnel);
	INIT_CHECK_EX(m_pTeamBalanceAbility);
	INIT_CHECK_EX(m_pTeamBalanceNo);

	m_pLimitJoinYes->setCheckBoxState(FALSE);
	m_pLimitJoinNo->setCheckBoxState(FALSE);
	m_pTeamBalancePersonnel->setCheckBoxState(FALSE);
	m_pTeamBalanceAbility->setCheckBoxState(FALSE);
	m_pTeamBalanceNo->setCheckBoxState(FALSE);

}

void CPhaseReadyRoom::_InitInviteWin(void)
{
	m_pInviteWin->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pInviteCaption, GAME_STRING("STR_TBL_GUI_READY_INVITE1"));/*초대하기*/

	INIT_CAPTION_BUTTON(m_pInviteConfirm, GAME_STRING("STR_TBL_GUI_READY_INVITE2"));/*초대*/
	INIT_CAPTION_BUTTON(m_pInviteCancel, GAME_STRING("STR_TBL_GUI_READY_CANCEL"));/*취소*/

	for(UINT32 i = 0; i < MAX_INVITE_COUNT; i++)
	{
		m_inviteSlot[i]._pCheck->setCheckBoxState(FALSE);
		GameGUI::SetRankStatic(m_inviteSlot[i]._pRank);
		m_inviteSlot[i]._pRank->SetEnable(FALSE);
		m_inviteSlot[i]._pRankBeginner->SetAllShapeEnable(FALSE);
		m_inviteSlot[i]._pNickname->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_inviteSlot[i]._pNickname->SetTextColor(GetColor(GCT_DEFAULT));
	}
}

void CPhaseReadyRoom::_InitCardComplateWin(void)
{
	// 윈도우 초기화
	m_pCardComplateWin->SetEnable(FALSE);
	INIT_WIN_CAPTION(m_pCardComplateCaption, GAME_STRING("STR_TBL_GUI_READY_MISSION_REWARD"));/*임무 보상*/

	// 버튼 초기화
	INIT_CAPTION_BUTTON(m_pCardComplateConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	// 에디트박스 초기화
	m_pCardComplateExp->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardComplatePoint->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardComplateExp->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCardComplatePoint->SetTextColor(GetColor(GCT_DEFAULT));

	// 보상
	m_cardComplateSlot._ribbonDummy->SetEnable(FALSE);
	m_cardComplateSlot._medalDummy->SetEnable(FALSE);

	m_pCardCompleteName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCardCompleteName->SetTextColor(GetColor(GCT_DEFAULT));
}

void CPhaseReadyRoom::_InitFinalComplateWin(void)
{
	// TODO: 임무카드 최종 보상창
	// 윈도우 초기화
	m_pFinalComplateWin->SetEnable(FALSE);
	INIT_WIN_CAPTION(m_pFinalComplateCaption, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));/*알림*/

	// 버튼 초기화
	INIT_CAPTION_BUTTON(m_pFinalComplateConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	// 에디트박스 초기화
	m_pFinalComplateName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalComplateDescription->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pFinalComplateName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pFinalComplateDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pFinalComplateDescription->SetTextSpace(0, 3);
	m_pFinalComplateDescription->SetText(GAME_STRING("STR_TBL_GUI_READY_NOTICE_REWARD_ITEM1"));/*{col:255,152,52,255}축하 드립니다!!{/col}\n \n인벤토리를 확인하시면 보상으로 아이템을 받을 수 있으며, 모으신 카드는 삭제됩니다. (이미 보유중인 아이템일 경우에는 남은 기간이 증가합니다.)*/


	// 표장+총기보상 팝업 초기화
	m_pFinalComplateExWin->SetEnable(FALSE);
	INIT_WIN_CAPTION(m_pFinalComplateExCaption, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));/*알림*/
	INIT_CAPTION_BUTTON(m_pFinalComplateExConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	m_pFinalComplateExName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalComplateExName->SetTextColor(GetColor(GCT_DEFAULT));

	m_pFinalComplateExDescription->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pFinalComplateExDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pFinalComplateExDescription->SetTextSpace(0, 3);
	m_pFinalComplateExDescription->SetText(GAME_STRING("STR_TBL_GUI_READY_NOTICE_COMPLETE_MISSION"));/*{col:255,152,52,255}축하 드립니다!!{/col}\n \n완료한 임무카드는 삭제되며 획득하신 훈장은 호칭과 교환할 수 있습니다.*/


	// 표장 보상 팝업 초기화
	m_pFinalComplatePrizeWin->SetEnable(FALSE);
	INIT_WIN_CAPTION(m_pFinalComplatePrizeCaption, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));/*알림*/
	INIT_CAPTION_BUTTON(m_pFinalComplatePrizeConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	m_pFinalComplatePrizeName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalComplatePrizeName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pFinalComplatePrizePoint->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalComplatePrizePoint->SetTextColor(GetColor(GCT_NOTIFY));
	m_pFinalComplatePrizeExp->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pFinalComplatePrizeExp->SetTextColor(GetColor(GCT_NOTIFY));
	m_pFinalComplatePrizeDescription->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pFinalComplatePrizeDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pFinalComplatePrizeDescription->SetTextSpace(0, 3);
	m_pFinalComplatePrizeDescription->SetText(GAME_STRING("STR_TBL_GUI_READY_NOTICE_COMPLETE_MISSION"));/*{col:255,152,52,255}축하 드립니다!!{/col}\n \n완료한 임무카드는 삭제되며 획득하신 훈장은 호칭과 교환할 수 있습니다.*/
}

void CPhaseReadyRoom::_InitRankUpWin(void)
{
	m_pRankUpWin->SetEnable(FALSE);
	INIT_WIN_CAPTION(m_pRankUpCaption, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));/*알림*/

	INIT_CAPTION_BUTTON(m_pRankUpConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	m_pRankUpMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRankUpMessage->SetTextSpace(0, 3);
	m_pRankUpMessage->SetTextColor(GetColor(GCT_DEFAULT));
	m_pRankUpName->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRankUpName->SetTextColor(GetColor(GCT_FOCUS));

	GameGUI::SetRankStatic(m_pRankUpMark);
}

void CPhaseReadyRoom::_InitClanBattle(void)
{
	m_pClanBattleInfoGui->SetEnable(FALSE);

	// 게임모드
	m_pClanBattleInfoModeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoModeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pClanBattleInfoModeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_GAME_MODE"));/*게임모드*/
	m_pClanBattleInfoModeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoModeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pClanBattleInfoModePrev);
	INIT_COMBO_EX(m_pClanBattleInfoModeNext);

	// 승리조건
	m_pClanBattleInfoTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pClanBattleInfoTypeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_VICTORY_CONDITION"));/*승리조건*/
	m_pClanBattleInfoTypeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoTypeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pClanBattleInfoTypePrev);
	INIT_COMBO_EX(m_pClanBattleInfoTypeNext);

	// 제한시간
	m_pClanBattleInfoTimeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoTimeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pClanBattleInfoTimeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_LIMIT_TIME2"));/*제한시간*/
	m_pClanBattleInfoTimeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoTimeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pClanBattleInfoTimePrev);
	INIT_COMBO_EX(m_pClanBattleInfoTimeNext);

	// 고급옵션
	m_pClanBattleInfoOptionKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pClanBattleInfoOptionKey->SetTextColor(GetColor(GCT_STATIC));
	m_pClanBattleInfoOptionKey->SetText(GAME_STRING("STR_TBL_GUI_READY_HIGHRANK_OPTION"));/*고급옵션*/
	INIT_CAPTION_BUTTON(m_pClanBattleInfoOptionButton, GAME_STRING("STR_TBL_GUI_READYROOM_OPTION"));

	INIT_STATIC_CAPTION2(m_pClanBattleInfoRedClanName, "");
	INIT_STATIC_CAPTION2(m_pClanBattleInfoRedTeamName, "");

	INIT_STATIC_CAPTION(m_pClanBattleInfoBlueClanName, "");
	INIT_STATIC_CAPTION(m_pClanBattleInfoBlueTeamName, "");

	m_pClanBattleBottomGui->SetEnable(FALSE);
	INIT_BUTTON_EX(m_pClanBattleBottomTeamChange);
}

void CPhaseReadyRoom::_SetNormalRoomControl(void)
{	// 일반 룸 인터페이스로
	m_pNormalRoomInfoGui->SetEnable(TRUE);
	m_pNormalRoomBottomGui->SetEnable(TRUE);
	m_pClanBattleInfoGui->SetEnable(FALSE);
	m_pClanBattleBottomGui->SetEnable(FALSE);

	((CStageReady*)m_pParent)->SetMenuClanInputDisable(FALSE);
}

void CPhaseReadyRoom::_SetClanBattleRoomControl(void)
{	//	클랜전 인터페이스로
	m_pNormalRoomInfoGui->SetEnable(FALSE);
	m_pNormalRoomBottomGui->SetEnable(FALSE);
	m_pClanBattleInfoGui->SetEnable(TRUE);
	m_pClanBattleBottomGui->SetEnable(TRUE);

	((CStageReady*)m_pParent)->SetMenuClanInputDisable(TRUE);
}

void CPhaseReadyRoom::_InitTrainingSlot(void)
{
	INT32 closeFlag = 0x00000000;

	for(INT32 i = 8; i < SLOT_MAX_COUNT; i++)
	{
		INT32 slotFlag = closeFlag | i;

		g_pGameContext->SetEvent(EVENT_CHANGE_SLOT_N, &slotFlag);
	}
}

void CPhaseReadyRoom::_NotifyChangeOption(void)
{
	STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_CHANGED_WEAPON_LIMIT"));/*방의 무기제한이 변경되어\n준비완료 상태를 해제합니다.*/
	g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_TBL_GUI_READY_CHANGED_OPTION"));/*방의 고급옵션이 변경되었습니다.*/

	m_nOldViewSlot = -1;
	_Update3DCharacter();

	if (_BeHost())	return;		// 방장은 처리하지 않는다

	m_bChangeOptionNotify = TRUE;

	if (m_pAdvencedWin->isEnable())
	{
		OnOption();
	}
}

void CPhaseReadyRoom::_ProcessNotifyChangeOption(REAL32 rDeltaSeconds)
{
	if (m_bChangeOptionNotify)
	{
		static BOOL toggleState = FALSE;
		static REAL32 rToggleTerm = 0.0f;

		rToggleTerm -= rDeltaSeconds;

		if (0.0f > rToggleTerm)
		{
			toggleState = !toggleState;

			if (toggleState)
			{
				rToggleTerm = 0.2f;
			}
			else
			{
				rToggleTerm = 0.5f;
			}
		}

		if (toggleState)
		{
			m_pOption->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 3);
		}
		else
		{
			m_pOption->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 0);
		}
	}
}

void CPhaseReadyRoom::_OpenContextMenu(INT32 slotIdx)
{
	ContextMenuReset();

	// 자신의 경우 출력할 컨텍스트메뉴가 없다.
	if (g_pGameContext->getMySlotIdx() == slotIdx)
	{
		return;
	}

	if (_BeHost())
	{
		ContextMenuAddCommand(CMC_TRANSFER_HOST);
		ContextMenuAddCommand(CMC_KICK);
	}

	ContextMenuAddCommand(CMC_WHISPER);
	ContextMenuAddCommand(CMC_ADD_FRIEND);

#if defined( USE_COMMUNITY_BLOCK)
	ContextMenuAddCommand(CMC_ADD_BLOCK);		//	차단
#endif

	if ( CanClanInvite() )
	{
		//	자기가 클랜 초대 기능이 있을 경우만 나오게
		if( g_pGameContext->getSlotInfo( slotIdx )->_clanstate == CLAN_MEMBER_LEVEL_UNKNOWN &&
			g_pFramework->GetClanContext()->IsPossibleClanInvite() )
		{
			ContextMenuAddCommand(CMC_CLAN_INVITE);
		}
	}

	if ( CanUse_ViewOtherUserItemInfo() )
		ContextMenuAddCommand(CMC_VIEW_OTHER_USER_ITEM_INFO);

	ContextMenuOpen(this, slotIdx);
}

void CPhaseReadyRoom::_SlideRecordWindow(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == m_bRecordWindowSliding) return;


	m_rRecordSlideTimeStamp += (rDeltaSeconds * 4.0f);
	m_rRecordSlideTimeStamp = MIN(1.0f, m_rRecordSlideTimeStamp);

	// 상세정보창 출현
	if (m_bInRecordWindow)
	{
		REAL32 x = 686.0f - ((686.0f - 436.0f) * m_rRecordSlideTimeStamp);
		m_pRecordWin->setPositionY(x);
	}
	else
	{
		REAL32 x = 436.0f + ((686.0f - 436.0f) * m_rRecordSlideTimeStamp);
		m_pRecordWin->setPositionY(x);
	}

	if (1.0f <= m_rRecordSlideTimeStamp)
	{
		m_bRecordWindowSliding = FALSE;
		m_rRecordSlideTimeStamp = 0.0f;
	}
}

void CPhaseReadyRoom::_InitSlideRecordWindow(void)
{
	m_bRecordWindowSliding = FALSE;
	m_bInRecordWindow = TRUE;
	m_pRecordWin->setPositionY(436.0f);
	m_pRecordCaption->SetText(GAME_STRING("STR_TBL_GUI_READY_DETAIL_INFO1"));/*▼ 상세정보 ▼*/
	m_rRecordSlideTimeStamp = 0.0f;
}

void CPhaseReadyRoom::_DelayInviteResponse(RT_REAL32 rDeltaSeconds)
{
	static REAL32 rDelayTime = 0.0f;

	if (m_pWaitInviteMB)
	{
		if (m_bInviteMessage && 1.0f <= rDelayTime)
		{
			// 딜레이용 값 초기화
			g_pFramework->CloseMessageBox(m_pWaitInviteMB);
			m_pWaitInviteMB = NULL;
			rDelayTime = 0.0f;
			m_bInviteMessage = FALSE;

			// 초대하기 결과 처리
			if (m_bInviteResult)
			{
				UINT32 nCount = g_pGameContext->GetUserInfoInviteCount();

				nCount = MIN(nCount, MAX_INVITE_COUNT);

				for(UINT32 i = 0; i < nCount; i++)
				{
					const USER_INFO_INVITE* pInfo = g_pGameContext->GetUserInfoInvite(i);

					m_inviteSlot[i]._pNickname->SetTextEllipsis(pInfo->_nick);
					m_inviteSlot[i]._pNickname->SetEnable(TRUE);
					m_inviteSlot[i]._pCheck->SetEnable(TRUE);

					if( g_pGameContext->IsBeginnerChannel1() )
					{
						m_inviteSlot[i]._pRank->SetEnable( FALSE);
						m_inviteSlot[i]._pRankBeginner->SetEnable( TRUE);
						m_inviteSlot[i]._pRankBeginner->SetAllShapeEnable( TRUE);
						m_inviteSlot[i]._pRankBeginner->SetShapeUVIndex(RANK_NEWBIE);
					}
					else
					{
						m_inviteSlot[i]._pRankBeginner->SetEnable( FALSE);
						m_inviteSlot[i]._pRankBeginner->SetAllShapeEnable( FALSE);
						m_inviteSlot[i]._pRank->SetEnable( TRUE);
						m_inviteSlot[i]._pRank->SetShapeUVIndex(pInfo->_rank);
					}
				}

				((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);
				m_pRoomWin->setInputDisable(TRUE);

				m_pInviteWin->SetEnable(TRUE);
			}
			else
			{
				// 비활성화 풀기
				STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_NO_USER_AT_LOBBY"));/*대기실에 사용자가 없습니다.*/
			}
		}
		else
		{
			rDelayTime += rDeltaSeconds;
		}
	}
}

void CPhaseReadyRoom::_OpenFinalPrizeWindow(void)
{
	g_pGameContext->RemoveGameFlag( GAMEFLAG_FINALPRIZE);

	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	
	if( pQuestManager == NULL )
	{
		i3Error::Log( "최종 아이템 보상창 3 [ pQuestManager == NULL ] " );
	}

	if( pQuestManager->isQuestActivated() == false )
	{
		i3Error::Log( "최종 아이템 보상창 4 [ pQuestManager->isQuestActivated() == false ] " );
	}

	if( pQuestManager == NULL || (pQuestManager->isQuestActivated() == false))
		return;

 	// 활성화된 카드셋이 없다면 체크하지 않는다.
	i3Error::Log( "최종 아이템 보상창 5 [ pQuestManager->IsCompleteActiveCardSet() ] 검사하러감 " );
 	if (! pQuestManager->IsCompleteActiveCardSet())
	{
 		return;
	}

	i3Error::Log( "최종 아이템 보상창 5 [ 보상 될 임무 카드 %d ] ", 
		pQuestManager->GetActiveCardsetID() );

	_OpenFinalPrizeWindowUI(pQuestManager->GetActiveCardsetID());


	m_pRoomWin->setInputDisable(TRUE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);

 	
	bool success = pQuestManager->RemoveActiveCardset();
	if ( ! success )
	{
		I3ASSERT_0;
	}
}


void CPhaseReadyRoom::_OpenPopupRewardItemForCardSetComplete( INT32 ItemID )
{
	string name;
	GetItemName(name, ItemID);

	// 획득 보상 출력
	m_pFinalComplateName->SetText(name.c_str());
	m_pFinalComplateName->SetEnable(TRUE);


	m_pFinalComplateBg->SetEnable(FALSE);
	m_pFinalComplateBg2->SetEnable(FALSE);

	i3GuiStatic * pStatic = NULL;
	switch(GET_ITEM_FLAG_TYPE(ItemID))
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		pStatic = m_pFinalComplateDummy;
		m_pFinalComplateBg->SetEnable(TRUE);
		break;
	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_EXPENDABLE:
	case ITEM_TYPE_POINT:
		pStatic = m_pFinalComplateDummy2;
		m_pFinalComplateBg2->SetEnable(TRUE);
		break;
	}

	i3Error::Log( "최종 아이템 보상창 10 [ Name = %s ] ", name.c_str() );

	g_pFramework->SetSlotImage(pStatic, ItemID);

	/*{col:255,152,52,255}축하 드립니다!!{/col}\n \n인벤토리를 확인하시면 보상으로 아이템을 받을 수 있으며, 모으신 카드는 삭제됩니다. (이미 보유중인 아이템일 경우에는 남은 기간이 증가합니다.)*/
	m_pFinalComplateDescription->SetText(GAME_STRING("STR_TBL_GUI_READY_NOTICE_REWARD_ITEM1"));
	m_pFinalComplateDescription->SetEnable(TRUE);

	// 보상교환 팝업 출력
	m_pFinalComplateWin->SetEnable(TRUE);
}

void CPhaseReadyRoom::CheckBattleChallengeComplate(void)
{
	CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
	if( pQuestManager == NULL || (pQuestManager->isQuestActivated() == false)) return;

	// 전투중 완료된 퀘스트가 없으면 처리하지 않는다.
	if (FALSE == pQuestManager->BeGainedPrizeByBattle())
	{
		if (g_pGameContext->IsFinalPrizeFlag())
		{
			_OpenFinalPrizeWindow();
		}
		else if (pQuestManager->BeCompleteQuestByBattle())
		{
			// 임무카드의 모든 퀘스트를 완료한 것이 아닌
			// 몇개의 임무라도 완료했다면 팝업을 띄웁니다.
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_READY_EXIST_COMPLETE_MISSION"));//완료된 임무가 있습니다.
		}

		pQuestManager->ResetGainedPrizeByBattle();
		return;
	}

	//
	// 기록된 보상을 출력합니다.
	//

	// 보상 표장이미지를 선택합니다.
	char	szName[MAX_STRING_COUNT] = {};
	BOOL	EnableRibbon	= FALSE;
	BOOL	EnableMedal		= FALSE;
	UINT8		prize		= static_cast<UINT8>(pQuestManager->GetGainedPrizeByBattle());
	BADGE_TYPE	BadgeType	= PrizeToBadgeType(prize);

	switch (BadgeType)
	{
	case BADGE_TYPE_RIBBON:
		EnableRibbon = TRUE;

		g_pFramework->SetRibbonShape( m_cardComplateSlot._ribbonDummy, prize);
		m_cardComplateSlot._medalDummy->SetEnable(FALSE);
		i3String::Copy( szName, GAME_STRING("STR_TBL_PROFILE_RIBBON_TEXT2"), MAX_STRING_COUNT );	//약장
		break;

	case BADGE_TYPE_ENSIGN:
		prize -= 50;
		EnableMedal = TRUE;

		g_pFramework->SetEnsignShape( m_cardComplateSlot._medalDummy, prize);
		i3String::Copy( szName, GAME_STRING("STR_TBL_PROFILE_ENSIGN_TEXT2"), MAX_STRING_COUNT );	//휘장
		break;

	case BADGE_TYPE_MEDAL:
	case BADGE_TYPE_MASTER:
		prize -= 100;
		EnableMedal = TRUE;

		g_pFramework->SetMedalShape( m_cardComplateSlot._medalDummy, prize);
		i3String::Copy( szName, GAME_STRING("STR_TBL_PROFILE_MEDALS_TEXT2"), MAX_STRING_COUNT );	//훈장
		break;
	}

	m_cardComplateSlot._ribbonDummy->SetEnable(EnableRibbon);
	m_cardComplateSlot._medalDummy->SetEnable(EnableMedal);


	// 보상 내용에 대해서 출력합니다.
	char expText[MAX_STRING_COUNT];
	char pointText[MAX_STRING_COUNT];

	INT32 totalExp		= pQuestManager->getGainedTotalExp();
	INT32 totalPoint	= pQuestManager->getGainedTotalPoint();
	
	sprintf_s(expText, GAME_STRING("STR_TBL_GUI_READY_REWARD_EXP"), totalExp );/*보상 경험치: %d*/
	sprintf_s(pointText, GAME_STRING("STR_TBL_GUI_READY_REWARD_POINT"), totalPoint );/*보상 Point: %d*/
	m_pCardComplateExp->SetText(expText);
	m_pCardComplatePoint->SetText(pointText);

	m_pCardCompleteName->SetText( szName);

	// 보상 윈도우를 활성화한다.
	m_pCardComplateWin->SetEnable(TRUE);
	m_pRoomWin->setInputDisable(TRUE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);

	// 개별 카드의 보상을 출력해준다.
	{
		CARDSET_ID CardSetID = pQuestManager->GetActiveCardsetID();
		UINT32 CardIndex = static_cast<UINT32>(pQuestManager->getCompletedCardIndex());
		UINT32 PrizeItem = g_pQuestTextDictionary->GetQuestCardPrize(CardSetID, CardIndex);
		CString		str;
		str.Format("------------------\n %d / %d / %d \n-----------------------------------\n", CardSetID, CardIndex, PrizeItem);
		OutputDebugString(str);
		if ( PrizeItem != 0 )
		{
			string	itemName;
			GetItemName(itemName, PrizeItem);

			char str[1024];
			StringCchPrintf(str, 1024, GAME_STRING("STR_TBL_GUI_READY_NOTICE_GET_SOMETHING1"), itemName.c_str());
			STRING_POPUP(str, GAME_STRING("STR_TBL_GUI_READY_MISSION_REWARD"));
		}

	}

	pQuestManager->ResetGainedPrizeByBattle();
}


void CPhaseReadyRoom::_OpenPopupReadyBoxRewardItem( INT32 ItemID )
{
	string name;
	GetItemName(name, ItemID);

	// 획득 보상 출력
	m_pFinalComplateName->SetText(name.c_str());
	m_pFinalComplateName->SetEnable(TRUE);

	m_pFinalComplateBg->SetEnable(FALSE);
	m_pFinalComplateBg2->SetEnable(FALSE);

	i3GuiStatic * pStatic = NULL;
	switch(GET_ITEM_FLAG_TYPE(ItemID))
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		pStatic = m_pFinalComplateDummy;
		m_pFinalComplateBg->SetEnable(TRUE);
		break;
	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_EXPENDABLE:
	case ITEM_TYPE_POINT:
		pStatic = m_pFinalComplateDummy2;
		m_pFinalComplateBg2->SetEnable(TRUE);
		break;
	}

	g_pFramework->SetSlotImage(pStatic, ItemID);

	char strBuf[MAX_STRING_COUNT] = {0,};
	_snprintf(strBuf, MAX_STRING_COUNT-1, 
		GAME_STRING("STR_READYBOX_REWARD"), name.c_str() );	//{col:240, 30, 40, 255}%s{/col} 를 획득 하셨습니다.

	m_pFinalComplateDescription->SetText( strBuf );
	m_pFinalComplateDescription->SetEnable(TRUE);

	// 보상교환 팝업 출력
	m_pFinalComplateWin->SetEnable(TRUE);
}



void CPhaseReadyRoom::ApplyRewardReadyBox()
{
	//보상 창이 이미 떠 있으면 안 된다.
	if( m_pCardComplateWin->isEnable() )
		return;

	//보상 받을게 없다.
	if( g_pGameContext->GetReadyBoxReward_Type() == SUPPLY_BOX_NONE )
		return;

	UINT32 ItemID = g_pGameContext->GetReadyBoxReward_ItemID();

	//보상창을 띄운다.
	_OpenPopupReadyBoxRewardItem( ItemID );

	//초기화 한다.
	g_pGameContext->ClearReadyBoxReward();
}

INT32 CPhaseReadyRoom::_GetRewardMedalByCardSet(UINT32 cardSetId)
{	//	임무카드 보상 메달 (없으면 -1 반환)
	switch( cardSetId)
	{
#if defined( BUILD_RELEASE_QA_VERSION ) && defined( TEMP_QUEST_REWARD)
		// 예전방식의 임무카드 최종보상의 표장인덱스인것 같습니다. (영권 09.08.04)
	case QUEST_CARDSET_TYPE_ASSAULT:		return 16; break;
	case QUEST_CARDSET_TYPE_BACKUP:			return 17; break;
	case QUEST_CARDSET_TYPE_INFILTRATION:	return 18; break;
	case QUEST_CARDSET_TYPE_SPECIAL:		return 19; break;
	case QUEST_CARDSET_TYPE_DEFCON:			return 20; break;
	case QUEST_CARDSET_TYPE_COMMISSIONED_O:	return 18; break;
	case QUEST_CARDSET_TYPE_COMPANY_O:		return 19; break;
	case QUEST_CARDSET_TYPE_FIELD_O:		return 20; break;
#else
		// 새로운 임무카드의 최종 보상은 무조건 마스터 훈장입니다. (영권 09.08.04)
	case QUEST_CARDSET_TYPE_ASSAULT:
	case QUEST_CARDSET_TYPE_BACKUP:
	case QUEST_CARDSET_TYPE_INFILTRATION:
	case QUEST_CARDSET_TYPE_SPECIAL:
	case QUEST_CARDSET_TYPE_DEFCON:
	case QUEST_CARDSET_TYPE_COMMISSIONED_O:
	case QUEST_CARDSET_TYPE_COMPANY_O:
	case QUEST_CARDSET_TYPE_FIELD_O:
		return FIXED_BADGE_NUM_MASTER;
		break;
#endif

		// 러시아 새해 2011 이벤트 전용. 임시로 넣습니다. 나중에 수정 필요. 성민
/*#if defined( LOCALE_RUSSIA)
	case QUEST_CARDSET_TYPE_EVENT:			return FIXED_BADGE_NUM_MASTER; break;	// 러시아는 마스터 훈장
#else*/
	case QUEST_CARDSET_TYPE_EVENT:			return EVENT_CARDSET_PRIZE_MEDAL_TYPE; break;
//#endif
	}

	return -1;
}

void CPhaseReadyRoom::_GetRewardRibbonByCardSet(UINT32 cardSetId, i3GuiStatic * pStatic, char * pszOutString, const size_t sizeOutStrMax )
{
	switch( cardSetId)
	{
	case QUEST_CARDSET_TYPE_DINO_TUTORIAL:
	case QUEST_CARDSET_TYPE_DINO_2ND:
	case QUEST_CARDSET_TYPE_DINO_3RD:
		{
			g_pFramework->SetRibbonShape(pStatic, 21);

			if( pszOutString)
				g_pQuestTextDictionary->GetRibbonNameString( pszOutString, sizeOutStrMax, 21);
		}
		break;
	case QUEST_CARDSET_TYPE_HUMAN_TUTORIAL:
	case QUEST_CARDSET_TYPE_HUMAN_2ND:
	case QUEST_CARDSET_TYPE_HUMAN_3RD:
		{
			g_pFramework->SetRibbonShape(pStatic, 22);

			if( pszOutString)
				g_pQuestTextDictionary->GetRibbonNameString( pszOutString, sizeOutStrMax, 21);
		}
		break;
	}
}

BOOL CPhaseReadyRoom::_SetClanInfoInSlot( INT32 slotNum, SLOT_INFO* pInfo)
{
	I3_BOUNDCHK( slotNum, SLOT_MAX_COUNT);

	if( pInfo)
	{
#if defined( USE_CLAN_MARK)
		if( SetClanMarkTex(m_slot[slotNum]._pClanMark, pInfo))
#else
		if( pInfo->_clanidx > 0 && pInfo->_clanstate != CLAN_MEMBER_LEVEL_UNKNOWN)
#endif
			//	클랜 마크

		{
			char temp[MAX_STRING_COUNT] = "";

			m_slot[slotNum]._pClanMark->SetEnable(TRUE);

			// 클랜등급 표시
			GetClanRankPreName2(temp, MAX_STRING_COUNT, pInfo->_clanExpRank);

			m_slot[slotNum]._pClanRank->SetText(temp);
			m_slot[slotNum]._pClanRank->SetEnable(TRUE);

			return TRUE;
		}
	}

	m_slot[slotNum]._pClanMark->SetEnable(FALSE);
	m_slot[slotNum]._pClanRank->SetText("");
	m_slot[slotNum]._pClanRank->SetEnable(FALSE);

	return FALSE;
}

void CPhaseReadyRoom::_OpenRankUpPopup(void)
{
	// 진급 포인트 정보
	UINT32 rank = g_pGameContext->GetRankUpRank();
	UINT32 point = g_pGameContext->GetRankUpPoint();

	// 계급장과 계급명
	m_pRankUpMark->SetShapeUVIndex(rank);

	const char * RankText = GetCharaRank(rank);
	m_pRankUpName->SetText( RankText );

	// 진급 축하 메시지
	char rankUpPopupText[MAX_STRING_COUNT];

	// 브라질일때만
	// 레벨이 10미만일때만 STR_TBL_GUI_READY_NOTICE_PROMOTION3으로 출력

#if defined (LOCALE_BRAZIL)
	if (rank <= 10)	// 계급이 10미만일때만
		sprintf_s(rankUpPopupText, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION3"), point);
	else
		sprintf_s(rankUpPopupText, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION2"), point);/*{col:215,211,210,255}진급을 축하합니다!{/col}\n( {col:255,148,52,255}%d 포인트{/col}를 진급 포인트로 지급합니다. )*/
#else
	sprintf_s(rankUpPopupText, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION2"), point);/*{col:215,211,210,255}진급을 축하합니다!{/col}\n( {col:255,148,52,255}%d 포인트{/col}를 진급 포인트로 지급합니다. )*/
#endif
	
	m_pRankUpMessage->SetText(rankUpPopupText);

	// 채팅창에도 출력
	char rankUpChattingText[MAX_STRING_COUNT];
	
	// 닉네임 복사해서 사용
	char szMyNick[NET_NICK_NAME_SIZE] = {0};
	i3String::Copy(szMyNick, g_pGameContext->GetMyNickName(), NET_NICK_NAME_SIZE);

	sprintf_s(rankUpChattingText, GAME_STRING("STR_TBL_GUI_READY_NOTICE_PROMOTION1"), szMyNick, RankText);/*{col:255,120,0,255}%s님이 %s으로 진급하셨습니다. 축하 드립니다.!!{/col}*/
	g_pFramework->getChatBox()->PutSystemChatting(rankUpChattingText);


	// 팝업 출력
	m_pRoomWin->setInputDisable(TRUE);
	m_pRankUpWin->SetEnable(TRUE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);
}

void CPhaseReadyRoom::EventMessage(void)
{
	for( INT32 i=0; i<g_pFramework->getNoticeChatCount(); i++)
	{
		g_pFramework->getChatBox()->PutSystemChatting( g_pFramework->getNoticeChatList(i));
	}
}

BOOL CPhaseReadyRoom::OnOpenPopupRewardItemForCardSetComplete()
{
	//ItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_FACEGEAR, CHARACTER_CLASS_FACEGEAR,		MASK_ID_ALIEN_RED_EYES);
	if ( m_nItemIDForCardSetComplete != 0 )
	{				
		char szTemp[MAX_STRING_COUNT] = "";
		sprintf_s( szTemp, "CardSet Complete!! reward item : %d\n", m_nItemIDForCardSetComplete);
		I3NOTICE( szTemp);

		_OpenPopupRewardItemForCardSetComplete( m_nItemIDForCardSetComplete);

		m_nItemIDForCardSetComplete = 0;

		return TRUE;
	}

	return FALSE;
}

void CPhaseReadyRoom::OnFinalComplateWinConfirm(void)
{
	m_pFinalComplateWin->SetEnable(FALSE);
	OnButtonConfirm();

	m_nCountForItemIDForCardSetComplete++;

	// 다음 보상 아이템을 체크하고 가져온다. (최대 MAX_QUEST_REWARD_ITEM_COUNT 갯수만큼 보상 아이템 지정가능)
	if( m_nCountForItemIDForCardSetComplete < MAX_QUEST_REWARD_ITEM_COUNT)
	{
		CQuestManager * pQuestManager = g_pGameContext->getQuestManager();
		I3ASSERT( pQuestManager != NULL);

		CARDSET_ID CarsetID = pQuestManager->GetActiveCardsetID();

		if(CarsetID != QUEST_CARDSET_TYPE_NONE)
			m_nItemIDForCardSetComplete = g_pQuestTextDictionary->GetCardSetBonusItemID(CarsetID, m_nCountForItemIDForCardSetComplete);
	}

	if( OnOpenPopupRewardItemForCardSetComplete() == TRUE)
	{
		// 팝업이 열려서 다시 뒷배경 GUI 입력을 막는다.
		m_pRoomWin->setInputDisable(TRUE);
		((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);
	}
}

void CPhaseReadyRoom::OnFinalComplateExWinConfirm(void)
{
	m_pFinalComplateExWin->SetEnable(FALSE);
	OnButtonConfirm();
}

// 최종 보상 팝업후 닫으면 보상 아이템 팝업이 뜬다.
void CPhaseReadyRoom::OnFinalComplatePrizeWinConfirm(void)
{
	m_pFinalComplatePrizeWin->SetEnable(FALSE);
	OnButtonConfirm();

	if( OnOpenPopupRewardItemForCardSetComplete() == TRUE)
	{
		// 팝업이 열려서 다시 뒷배경 GUI 입력을 막는다.
		m_pRoomWin->setInputDisable(TRUE);
		((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////
//Room Master GameStart Timer After All User Ready
void CPhaseReadyRoom::AllUserReady()
{
	#ifdef USE_ROOM_MASTER_KICK
	if (_BeHost())
	{
		INT32 OtherUserCount = 0;

		if(NULL == m_pBeHostMessageBox)
		{
			((CStageReady*)m_pParent)->Set_bBeHostMessageOK(FALSE);
		}

		if(TRUE == ((CStageReady*)m_pParent)->Get_bBeHostMessageOK())
		{
			((CStageReady*)m_pParent)->PauseRoomMasterKick();
		}		

		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			// 방장이면 방장 마크 표시
			if (g_pGameContext->getMainSlotIdx() == i)		continue;
			SLOT_INFO* pSlot = g_pGameContext->getSlotInfo(i);

			// 슬롯에 누군가가 있다.
			if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
			{
				if(SLOT_STATE_READY != pSlot->_State)
				{
					((CStageReady*)m_pParent)->Set_bPS_PLAYING(TRUE);
					return;
				}			

				OtherUserCount++;
			}			
		}

		// 상대팀의 유저가 하나도 없는지 검사
		if(0 < OtherUserCount)
		{
			if(FALSE == ((CStageReady*)m_pParent)->Get_bRoomMsterAwayCountDown())
			{
				// 모두 레디했으면 제한 시간 활성화
				((CStageReady*)m_pParent)->Set_bAllUserReady(TRUE);
			}
		}
		else
		{
			((CStageReady*)m_pParent)->StopRoomMasterKick();
		}
	}
	else
	{
		//((CStageReady*)m_pParent)->Set_bPS_PLAYING(TRUE);
		((CStageReady*)m_pParent)->StopRoomMasterKick();
	}
	#endif
}

void CPhaseReadyRoom::_OpenFinalPrizeWindowUI( CARDSET_ID CardsetID )
{
	// 보상 확인
	switch(CardsetID)
	{
	case QUEST_CARDSET_TYPE_TUTORIAL:
		{
#if defined( LOCALE_KOREA) || defined( LOCALE_RUSSIA)
			// 튜토리얼 임무카드 최종보상은 데저트 이글 무기
			INT32 ItemID = g_pQuestTextDictionary->GetCardSetBonusItemID(CardsetID);
			_OpenPopupRewardItemForCardSetComplete(ItemID);
#else	
			//	데저트 이글 무기대신 포인트만 지급. 예외처리함.
			char bonusPointText[MAX_STRING_COUNT];
			sprintf_s(bonusPointText, GAME_STRING("STR_TBL_GUI_READY_GET_POINT_BONUS"), g_pQuestTextDictionary->GetCardSetBonusPoint(CardsetID));

			// 보상교환 팝업 출력
			CAPTION_POPUP_C( bonusPointText, GAME_STRING("STR_TBL_GUI_READY_NOTICE"), MAKE_CALLBACK(CbFinalComplateConfirm), this);
#endif
		}
		break;
	case QUEST_CARDSET_TYPE_DINO_TUTORIAL:
	case QUEST_CARDSET_TYPE_HUMAN_TUTORIAL:
	case QUEST_CARDSET_TYPE_DINO_2ND:
	case QUEST_CARDSET_TYPE_HUMAN_2ND:
	case QUEST_CARDSET_TYPE_DINO_3RD:
	case QUEST_CARDSET_TYPE_HUMAN_3RD:
		{
			INT32 ItemID = g_pQuestTextDictionary->GetCardSetBonusItemID(CardsetID);

			i3Error::Log( "최종 아이템 보상창100 [ g_pQuestTextDictionary->GetCardSetBonusItemID(CardsetID) = %d ] ", ItemID );

			_OpenPopupRewardItemForCardSetComplete(ItemID);
		}
		break;
	case QUEST_CARDSET_TYPE_ASSAULT			:
	case QUEST_CARDSET_TYPE_BACKUP			:
	case QUEST_CARDSET_TYPE_INFILTRATION	:
	case QUEST_CARDSET_TYPE_SPECIAL			:
	case QUEST_CARDSET_TYPE_DEFCON			:
	case QUEST_CARDSET_TYPE_COMMISSIONED_O	:
	case QUEST_CARDSET_TYPE_COMPANY_O		:
	case QUEST_CARDSET_TYPE_FIELD_O			:
	case QUEST_CARDSET_TYPE_EVENT			:
		{
			INT32 iGetMedal = _GetRewardMedalByCardSet( CardsetID );

			INT32 bonusPoint = g_pQuestTextDictionary->GetCardSetBonusPoint(CardsetID);	// 보너스 포인트
			INT32 bonusExp = g_pQuestTextDictionary->GetCardSetBonusExp(CardsetID);		// 보너스 경험치

			if( iGetMedal != -1)	g_pFramework->SetMedalShape(m_pFinalComplatePrizeDummy, iGetMedal);
			else					m_pFinalComplatePrizeDummy->SetEnable( FALSE);

			char prizeText[MAX_STRING_COUNT] = {0, };
			i3String::Copy( prizeText, GAME_STRING("STR_TBL_PROFILE_MASTER_MEDALS_TEXT"), MAX_STRING_COUNT );

			// 획득 보상 출력
			char captionText[MAX_STRING_COUNT];
			sprintf_s(captionText, GAME_STRING("STR_TBL_GUI_READY_NOTICE_GET_SOMETHING1"), prizeText);/*[%s] 획득*/

			if( iGetMedal != -1 )
				m_pFinalComplatePrizeName->SetText(captionText);
			else
				m_pFinalComplatePrizeName->SetText("");

			m_pFinalComplatePrizeName->SetEnable(TRUE);

			char bonusPointText[MAX_STRING_COUNT];
			sprintf_s(bonusPointText, GAME_STRING("STR_TBL_GUI_READY_GET_POINT_BONUS"), bonusPoint);/*+ %d 포인트 획득*/
			m_pFinalComplatePrizePoint->SetText(bonusPointText);
			m_pFinalComplatePrizePoint->SetEnable(TRUE);

			char bonusExpText[MAX_STRING_COUNT];
			sprintf_s(bonusExpText, GAME_STRING("STR_TBL_GUI_READY_GET_EXP_BONUS"), bonusExp);/*+ %d 경험치 획득*/
			m_pFinalComplatePrizeExp->SetText(bonusExpText);
			m_pFinalComplatePrizeExp->SetEnable(TRUE);

			if( QUEST_CARDSET_TYPE_EVENT == CardsetID)
				m_pFinalComplatePrizeDescription->SetEnable(FALSE);
			else
				m_pFinalComplatePrizeDescription->SetEnable(TRUE);

			// 완료시 인벤에 지급받는 최종 보상 아이템 
			m_nItemIDForCardSetComplete = g_pQuestTextDictionary->GetCardSetBonusItemID(CardsetID, 0);
			m_nCountForItemIDForCardSetComplete = 0;

			// 최종 보상 메달, 포인트, 경험치 출력
			if( iGetMedal != -1 || bonusPoint > 0 || bonusExp > 0) 
			{
				m_pFinalComplatePrizeWin->SetEnable(TRUE);
			}
			else
			{
				// 완료시 인벤에 지급받는 최종 보상 아이템 
				OnOpenPopupRewardItemForCardSetComplete();
			}			
		}
		break;

	default:
		break;
	}
}

void CPhaseReadyRoom::UpdateReadyButton()
{
	if(m_pReady->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)//마우스 오버 -> 밝은상태로 고정
	{
		m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET+1);
	}
	else if((g_pGameContext->getSlotInfo(g_pGameContext->getMySlotIdx())->_State) == SLOT_STATE_NORMAL)//레디체크X 마우스 아웃 -> 깜빡
	{
		if(((CStageReady*)m_pParent)->GetButtonEffect())
		{
			m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
		else
		{
			m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
	}
	else//레디체크O 마우스 아웃 -> 밝은상태로 고정
	{
		m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pReady->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
	}
}

void CPhaseReadyRoom::UpdateStartButton()
{
	if(m_pStart->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)//마우스 오버 -> 밝은상태로 고정
	{
		m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET+1);
	}
	else if( _BeHost() && _CheckReady())
	{
		if(((CStageReady*)m_pParent)->GetButtonEffect())
		{
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
		else
		{
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
	}
	else if( !_BeHost()) //방장이 아닐때
	{
		if(((CStageReady*)m_pParent)->GetButtonEffect())
		{
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
		else
		{
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
	}
	else//마우스 아웃 && 게임시작 불가능 -> 비활성화
	{
		m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pStart->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
	}
}

BOOL CPhaseReadyRoom::_CheckReady()
{
	INT32 Other = 0;
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		// 방장이면 방장 마크 표시
		if (g_pGameContext->getMainSlotIdx() == i)		continue;
		SLOT_INFO* pSlot = g_pGameContext->getSlotInfo(i);

		// 슬롯에 누군가가 있다.
		if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
		{
			Other++;
			if(SLOT_STATE_READY != pSlot->_State)
			{
				return FALSE;
			}			
		}
	}
	if(Other > 0)
		return TRUE;
	else
		return FALSE;
}
//void CPhaseReadyRoom::AllUserReady()
//{
//#ifdef USE_ROOM_MASTER_KICK
//	if (_BeHost())
//	{
//		INT32 OtherTeamUserCount = 0;
//
//		INT32 MasterTeam = g_pGameContext->getMainSlotIdx() % 2;
//
//		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
//		{
//			// 방장이면 방장 마크 표시
//			if (g_pGameContext->getMainSlotIdx() == i)		continue;
//
//			// 방장과 다른 팀만 검사
//			if(MasterTeam != i%2)
//			{
//				SLOT_INFO* pSlot = g_pGameContext->getSlotInfo(i);
//
//				// 슬롯에 누군가가 있다.
//				if (SLOT_STATE_CLOSE != pSlot->_State && SLOT_STATE_EMPTY != pSlot->_State)
//				{
//					if(SLOT_STATE_READY != pSlot->_State)
//					{
//						((CStageReady*)m_pParent)->Set_bPS_PLAYING(TRUE);
//						return;
//					}			
//
//					OtherTeamUserCount++;
//				}
//			}
//		}
//
//		// 상대팀의 유저가 하나도 없는지 검사
//		if(0 < OtherTeamUserCount)
//		{
//			if(FALSE == ((CStageReady*)m_pParent)->Get_bRoomMsterAwayCountDown())
//			{
//				// 모두 레디했으면 제한 시간 활성화
//				((CStageReady*)m_pParent)->Set_bAllUserReady(TRUE);
//			}
//		}
//	}
//	else
//	{
//		((CStageReady*)m_pParent)->StopRoomMasterKick();
//	}
//#endif
//}
