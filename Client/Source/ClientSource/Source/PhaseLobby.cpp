#include "pch.h"
#include "PhaseLobby.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "StringFilter.h"
#include "PhaseReadyRoom.h"
#include "ContextMenu.h"
#include "CommunityContext.h"
#include "ClanMark.h"
#include "TextSet.h"
#include "StageGui/Popup/PopupHeader.h"
#include "ServerInfo.h"
#include "SecurityHeader.h"
#include "ItemDataBase.h"

#if defined (TEST_LOBBY_USER_LIST)
static const INT32 g_nLobbyUserListDummyCount = 100;
static LOBBY_USER_INFO_LIST * g_pLobbyUserListDummy = NULL;
#endif

I3_CLASS_INSTANCE(CPhaseLobby, CReadyPhaseBase);

BOOL		CPhaseLobby::sm_bAwayInput					= FALSE;
SORT_ORDER	CPhaseLobby::sm_SortOrder[MAX_SORT_TYPE]	= {{ROOM_SORT_NUMBER, TRUE},};
BOOL		CPhaseLobby::m_bPcCafeNotice				= FALSE;
BOOL		CPhaseLobby::m_bEventCardSetNotice			= FALSE;
BOOL		CPhaseLobby::m_bEventNotice					= FALSE;



CPhaseLobby::CPhaseLobby()
: CReadyPhaseBase()
//, m_BtnEffector(1.0f) ,m_BtnEffector1(1.0f)
{
	m_pLobbyGui = NULL;
	m_pInfoWin = NULL;
	m_pRoomWin = NULL;
	m_pCreateWin = NULL;
	m_pNicknameWin = NULL;

    m_pInfoCaption = NULL;
	m_pRoomCaption = NULL;
	m_pServerInfoValue = NULL;

	m_pbtnQuickStart = NULL;

	// 특수전
	m_pbtnSpecialWar = NULL;
	m_pSpecialWarWin = NULL;
	m_pSpecialWarCaption = NULL;
	m_pSpecialWarCancel = NULL;

	m_pDinoWin =NULL;
	m_pDinoCancel =NULL;

	i3mem::FillZero(m_SpecialWar, sizeof(m_SpecialWar));

	// 여기까지 (특수전)

	m_pServer = NULL;
	m_pEnter = NULL;
	m_pRefresh = NULL;
	m_pCreate = NULL;
	m_pTeamCaption = NULL;
	m_pServerCaption = NULL;
	m_pEnterCaption = NULL;
	m_pRefreshCaption = NULL;
	m_pCreateCaption = NULL;
	m_pList = NULL;
	

	i3mem::FillZero(m_roomSlot, sizeof(m_roomSlot));

	m_pSelect = NULL;
	m_pFocus = NULL;
	m_pImageDummy = NULL;
	m_pImageDummy2 = NULL;
	m_pImageDummy3 = NULL;
	m_pImageDummy4 = NULL;
	m_pImageXMas = NULL;
	m_pMapKey = NULL;
	m_pMapValue = NULL;
	m_pModeKey = NULL;
	m_pModeValue = NULL;
	m_pTypeKey = NULL;
	m_pTypeValue = NULL;
	m_pHostKey = NULL;
	m_pHostValue = NULL;
	m_pStateKey = NULL;
	m_pStateValue = NULL;
	m_pJoinLimitKey = NULL;
	m_pJoinLimitValue = NULL;
	m_pWeaponLimitKey = NULL;
	m_pWeaponLimitValue = NULL;
	m_pObserverLimitKey = NULL;
	m_pObserverLimitValue = NULL;
	m_pTeamBalanceKey = NULL;
	m_pTeamBalanceValue = NULL;

	m_pIdxCaption = NULL;
	m_pModeCaption = NULL;
	m_pMapCaption = NULL;
	m_pTitleCaption = NULL;
	m_pStateCaption = NULL;
	m_pPersonCaption = NULL;
	m_pPingCatpion = NULL;
	
	m_pNicknameCaption = NULL;
	m_pNicknameMessage = NULL;
	m_pNicknameInput = NULL;
	m_pNicknameConfirm = NULL;
	
	m_pCreateWinCaption = NULL;
	m_pCreateTitleKey = NULL;
	m_pCreateTitleValue = NULL;
	m_pCreatePasswordKey = NULL;
	m_pCreatePasswordValue = NULL;
	m_pCreateModeKey = NULL;
	m_pCreateModeValue = NULL;
	m_pModeCombo = NULL;
	m_pModePopup = NULL;
	m_pCreateConfirm = NULL;
	m_pCreateCancel = NULL;
	m_pCreatePasswordCheck = NULL;

	m_pAbuseWindow = NULL;
	m_pAbuseEditbox = NULL;
	m_pAbuseConfirm = NULL;

	///////////////////////////////////////////////////////////////////////////////////////////
	// 확장된 방생성 팝업
	m_pMapImage = NULL;
	m_pTypeImage = NULL;
	m_pMapNewMark = NULL;
	m_pMapXMasMark = NULL;
	m_pMap = NULL;
	m_pMapCombo = NULL;
	m_pMapList = NULL;
	m_pGameModeKey = NULL;
	m_pGameModeValue = NULL;
	m_pGameModePrev = NULL;
	m_pGameModeNext = NULL;
	m_pGameTypeKey = NULL;
	m_pGameTypeValue = NULL;
	m_pGameTypePrev = NULL;
	m_pGameTypeNext = NULL;
	m_pTimeKey = NULL;
	m_pTimeValue = NULL;
	m_pTimePrev = NULL;
	m_pTimeNext = NULL;
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

	m_pLimitObserver3PV_FreeKey = NULL;
	m_pLimitObserver3PV_Free = NULL;
	m_pLimitObserver1PV_FreeKey = NULL;
	m_pLimitObserver1PV_Free = NULL;
	m_pLimitObserverEnemy_FreeKey = NULL;
	m_pLimitObserverEnemy_Free = NULL;
	m_pLimitObserverEnemyHPKey = NULL;
	m_pLimitObserverEnemyHP = NULL;

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
	m_pGameTeamBalanceKey = NULL;
	m_pGameTeamBalancePersonnelKey = NULL;
	m_pGameTeamBalanceAbilityKey = NULL;
	m_pGameTeamBalanceNoKey = NULL;
	m_pGameTeamBalancePersonnel = NULL;
	m_pGameTeamBalanceAbility = NULL;
	m_pGameTeamBalanceNo = NULL;
	//
	///////////////////////////////////////////////////////////////////////////////////////////

	// PC방 알림 팝업
	m_pPcCafeWin = NULL;
	m_pPcCafeCaption = NULL;
	m_pPcCafeConfirm = NULL;
	m_pPcCafeMessage = NULL;
	m_pPcCafeExpKey = NULL;
	m_pPcCafePointKey = NULL;
	m_pPcCafeItemKey = NULL;
	m_pPcCafeExpValue = NULL;
	m_pPcCafePointValue = NULL;
	m_pPcCafeItemValue = NULL;

	m_pUserListWin = NULL;
	m_pUserListCaption = NULL;
	m_pUserListScroll = NULL;	
	m_pUserListSelect = NULL;
	m_pUserListFocus = NULL;

	i3mem::FillZero(m_pUserListSlot, sizeof(m_pUserListSlot));

	m_nRoomPage = 0;
	m_rEnterLobbyWaitTime = 0.0f;
	m_pWaitRoomListMassageBox = NULL;

	m_bCheckInvite = TRUE;
	m_pInviteMessageBox = NULL;

	m_bAllowRefresh = FALSE;

	m_bEnableNicknameMsgBox = FALSE;
	m_bEnableCreateRoomMsgBox = FALSE;
	m_CreateGameMode = STAGE_TYPE_DEATHMATCH;

	m_SelectedElement		= -1;

	m_fRefreshElapsedTime	= 0.0f;
	ButtonTermTime			= 0.0f;

	m_bFirstLogin = TRUE;

	m_bLogin = FALSE;
	m_bRequestRoomInfoAdd	= FALSE;
	m_LastRefreshTime		= 0.0f;

	m_nChatType = 0;
	m_nOldChatType = 0;

	m_nUserInfoListStartIdx = 0;
	m_nUserInfoListSelectIdx = -1;

	m_pChatBody = CGameChatBody::NewObject();

	g_pGameContext->ClearPreStageOption();

	// 확장된 방생성 팝업
	ResetCreateRoomAddInfo();

//#ifdef USE_ABUSING_CHECK
	m_bAbuseFlag = FALSE;
	m_rAbusingTotalTime = 0.f;
	m_bNickSpace = false;	
//#endif

	InitStageLimitTimeList();
}

CPhaseLobby::~CPhaseLobby()
{
	// Room List
	m_listRoom.SafeFreeAll();

	I3_SAFE_RELEASE( m_pChatBody);

	m_ViewUserInfoList.Clear();
}
//EscapeMission :공룡모드 창
void CPhaseLobby::SetStateDinoButton(DINO_ESCAPE_MODE mode, bool bOnMouse)
{

	I3ASSERT( mode < DINO_MODE_MAX);

	for(INT32 shape=0; shape < 6; shape++)
	{
		if( bOnMouse)
		{
			if( shape < 3)
				m_Dino[ mode].pButton->SetShapeEnable( shape, TRUE);
			else
				m_Dino[ mode].pButton->SetShapeEnable( shape, FALSE);
		}
		else
		{
			if( shape < 3)
				m_Dino[ mode].pButton->SetShapeEnable( shape, FALSE);
			else
				m_Dino[ mode].pButton->SetShapeEnable( shape, TRUE);
		}
	}



}
void CPhaseLobby::SetStateSpecialWarButton(SP_MODE_SPECIAL_WAR mode, bool bOnMouse)
{
	I3ASSERT( mode < SP_MODE_MAX);

	for(INT32 shape=0; shape < 6; shape++)
	{
		if( bOnMouse)
		{
			if( shape < 3)
				m_SpecialWar[ mode].pButton->SetShapeEnable( shape, TRUE);
			else
				m_SpecialWar[ mode].pButton->SetShapeEnable( shape, FALSE);
		}
		else
		{
			if( shape < 3)
				m_SpecialWar[ mode].pButton->SetShapeEnable( shape, FALSE);
			else
				m_SpecialWar[ mode].pButton->SetShapeEnable( shape, TRUE);
		}
	}
}

void CPhaseLobby::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	//	소트용 캡션. 디폴트 셋팅
	{
		m_pIdxCaption->SetTextColor(GetColor(GCT_DEFAULT));
		m_pModeCaption->SetTextColor(GetColor(GCT_DEFAULT));
		m_pMapCaption->SetTextColor(GetColor(GCT_DEFAULT));
		m_pTitleCaption->SetTextColor(GetColor(GCT_DEFAULT));
		m_pStateCaption->SetTextColor(GetColor(GCT_DEFAULT));
		m_pPersonCaption->SetTextColor(GetColor(GCT_DEFAULT));
		m_pPingCatpion->SetTextColor(GetColor(GCT_DEFAULT));
	}

	//특수전 버튼 비활성
	for (int i=0; i < SP_MODE_MAX; ++i)
	{
		SetStateSpecialWarButton( (SP_MODE_SPECIAL_WAR)i, false);
	}

	//공룡전 버튼 비활성
	for (int i=0; i < DINO_MODE_MAX; ++i)
	{
		SetStateDinoButton( (DINO_ESCAPE_MODE)i, false);
	}


	switch(pNotify->m_nID)
	{
	case GCI_LBP_B_QUICK_START:	//빠른 시작
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnQuickStart();
			}
		}
		break;
	case GCI_LBP_B_SPECIAL_WAR:	//특수전
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnSpecialWar(TRUE);
				PlaySound( GUI_SND_POPUP_OPEN);
			}
		}
		break;	
	case GCI_LBP_B_SPECIAL_WAR_CANCEL:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			OnSpecialWar(FALSE);
			PlaySound( GUI_SND_POPUP_CLOSE);
		}
		break;
		break;	
	case GCI_LBP_B_DINO_ESCAPE_CANCEL://EscapeMission
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			OnDino(FALSE);
			PlaySound( GUI_SND_POPUP_CLOSE);
		}
		break;
	case  GCI_LBP_B_DINO : //EscapeMission 
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnDino(TRUE);
				PlaySound( GUI_SND_POPUP_OPEN);
			}
		}
		break;	

	//탈출 미션(Escape Mission)
	case	GCI_LBP_S_DINO_ESCAPE_BUTTON:
	case	GCI_LBP_S_DINO_ESCAPE_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnDinoEscape();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateDinoButton( DINO_ESCAPE, true);
		}
		break;
	case GCI_LBP_E_DINO_ESCAPE_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_DINO_ESCAPE_TITLE_DESCRIPTION:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnDinoEscape();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateDinoButton( DINO_ESCAPE, true);
		}
		break;

		//제압 미션(Domination Mission)
	case	GCI_LBP_S_DINO_DOMINATION_BUTTON:
	case	GCI_LBP_S_DINO_DOMINATION_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnDinoDomination();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateDinoButton( DINO_DOMINATION, true);
		}
		break;
	case GCI_LBP_E_DINO_DOMINATION_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_DINO_DOMINATION_TITLE_DESCRIPTION:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnDinoDomination();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateDinoButton( DINO_DOMINATION, true);
		}
		break;
	////	특수전 챌린지 버튼
	case GCI_LBP_S_SPECIAL_WAR_CHALLENGE_BUTTON:
	case GCI_LBP_S_SPECIAL_WAR_CHALLENGE_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnSpecialWarChallenge();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_CHALLENGE, true);
		}
		break;
	case GCI_LBP_E_SPECIAL_WAR_CHALLENGE_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_CHALLENGE_TITLE_DESCRIPTION:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnSpecialWarChallenge();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_CHALLENGE, true);
		}
		break;
	////	특수전 샷건 버튼
	case GCI_LBP_S_SPECIAL_WAR_SHOTGUN_BUTTON:
	case GCI_LBP_S_SPECIAL_WAR_SHOTGUN_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnSpecialWarShotgun();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_SHOTGUN, true);
		}
		break;
	case GCI_LBP_E_SPECIAL_WAR_SHOTGUN_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_SHOTGUN_TITLE_DESCRIPTION:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnSpecialWarShotgun();	

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_SHOTGUN, true);
		}
		break;
	////	특수전 저격 버튼
	case GCI_LBP_S_SPECIAL_WAR_SNIPER_BUTTON:
	case GCI_LBP_S_SPECIAL_WAR_SNIPER_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnSpecialWarSniper();		

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_SNIPER, true);
		}
		break;
	case GCI_LBP_E_SPECIAL_WAR_SNIPER_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_SNIPER_TITLE_DESCRIPTION:		
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnSpecialWarSniper();		

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_SNIPER, true);
		}
		break;
	////	특수전 헤드헌터 버튼
	case GCI_LBP_S_SPECIAL_WAR_HEADHUNTER_BUTTON:
	case GCI_LBP_S_SPECIAL_WAR_HEADHUNTER_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnSpecialWarHeadHunter();		

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_HEADHUNTER, true);
		}
		break;
	case GCI_LBP_E_SPECIAL_WAR_HEADHUNTER_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_HEADHUNTER_TITLE_DESCRIPTION:		
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnSpecialWarHeadHunter();			

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_HEADHUNTER, true);
		}
		break;
	////	특수전 너클전 버튼
	case GCI_LBP_S_SPECIAL_WAR_KNUCKLE_BUTTON:
	case GCI_LBP_S_SPECIAL_WAR_KNUCKLE_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			OnSpecialWarKnuckle();		

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_KNUCKLE, true);
		}
		break;
	case GCI_LBP_E_SPECIAL_WAR_KNUCKLE_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_KNUCKLE_TITLE_DESCRIPTION:		
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			OnSpecialWarKnuckle();			

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_KNUCKLE, true);
		}
		break;
	////	특수전 튜토리얼 버튼
	case GCI_LBP_S_SPECIAL_WAR_TUTORIAL_BUTTON:
	case GCI_LBP_S_SPECIAL_WAR_TUTORIAL_ARROW:
		if(pNotify->m_nEvent == I3GUI_STATIC_NOTIFY_CLICKED) {
			StartTutorialMode();		

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_TUTORIAL, true);
		}
		break;
	case GCI_LBP_E_SPECIAL_WAR_TUTORIAL_BUTTON_DESCRIPTION:
	case GCI_LBP_E_BUTTON_TUTORIAL_TITLE_DESCRIPTION:		
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) {		
			StartTutorialMode();			

			PlaySound( GUI_SND_ITEM_SELECTING);
		}
		else {
			SetStateSpecialWarButton( SP_MODE_TUTORIAL, true);
		}
		break;
	case GCI_LBP_B_SERVER:	//서버 변경
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE)		SetTooltip(TIP_CHANGE_SERVER);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{		
				OnServer();
			}
		}
		break;
	case GCI_LBP_B_ENTER:	//입장하기
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE)		SetTooltip(TIP_ENTER_ROOM);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnEnter();
			}
		}
		break;
	case GCI_LBP_B_REFRESH:	//새로고침
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE)		SetTooltip(TIP_REFRESH);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnRefresh();
			}
		}
		break;
	case GCI_LBP_B_CREATE:	//방 만들기
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE)		SetTooltip(TIP_CREATE_ROOM);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 	
		{
			if( !m_bNickSpace )
			{
				OnCreateRoom();
			}
		}
		break;
	case GCI_LBP_L_LIST:		
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent)			OnSlotOnMouse(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)			OnSlotDrag();
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent)			OnSlotClick(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)		OnEnter(pNotify->m_nPriParam);
		break;
	case GCI_LBP_B_NICKNAME:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 		OnCreateNickname();
		break;
	case GCI_LBP_B_CREATE_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 		OnCreateRoomConfirmButtonClick();
		break;
	case GCI_LBP_B_CREATE_CANCEL:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnCreateRoomCancelButtonClick();
		break;
	case GCI_LBP_MAP_B_COMBO:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnMapCombo();
		break;
	case GCI_LBP_MAP_L_COMBO:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnMapPopup(pNotify->m_nPriParam);
		break;
	case GCI_LBP_B_MODE_PREV:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnModePrevButtonClick();
		break;
	case GCI_LBP_B_MODE_NEXT:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnModeNextButtonClick();
		break;
	case GCI_LBP_B_TYPE_PREV:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnVictoryPrevButtonClick();
		break;
	case GCI_LBP_B_TYPE_NEXT:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnVictoryNextButtonClick();
		break;
	case GCI_LBP_B_TIME_PREV:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnVictory2PrevButtonClick();
		break;
	case GCI_LBP_B_TIME_NEXT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnVictory2NextButtonClick();
		break;
	case GCI_LBP_CHK_ADVENCED_PRIMARY:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			if(m_pLimitPrimary->getCheckBoxState())		
			{
				m_pLimitRPG7->setCheckBoxState(FALSE);
			}
		}
		break;
	case GCI_LBP_CHK_ADVENCED_RPG7:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			if(m_pLimitRPG7->getCheckBoxState())			
			{
				m_pLimitPrimary->setCheckBoxState(FALSE);
			}
		}
		break;
	case GCI_LBP_B_JOIN_LIMIT_YES:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnJoinLimitCheck(TRUE);
		break;
	case GCI_LBP_B_JOIN_LIMIT_NO:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnJoinLimitCheck(FALSE);
		break;
//	case GCI_LBP_B_OBSERVER_LIMIT_YES:
//		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnObserverLimitCheck(TRUE);
//		break;
//	case GCI_LBP_B_OBSERVER_LIMIT_NO:
//		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnObserverLimitCheck(FALSE);
//		break;
	case GCI_LBP_B_TEAM_BALANCE_PERSONNEL:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent)		SetTooltip(TIP_TEAM_BALANCE);
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamBalanceCheck(1);
		break;
	case GCI_LBP_B_TEAM_BALANCE_ABILITY:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent)		SetTooltip(TIP_TEAM_BALANCE);
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamBalanceCheck(2);
		break;
	case GCI_LBP_B_TEAM_BALANCE_NO:
		if (I3GUI_BUTTON_NOTIFY_ONMOUSE == pNotify->m_nEvent)		SetTooltip(TIP_TEAM_BALANCE);
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamBalanceCheck(0);
		break;
	case GCI_LP_CHK_RANDOM:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnCheckRandomMap();
		break;
	case GCI_LP_B_RANDOM:
			if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) OnRandomMap();
		break;
		break;

	case GCI_LBP_B_PC_CAFE_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnPcCafeConfirm();
		break;
	case GCI_LBP_E_IDX:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) 		OnSortNumber();
		break;
	case GCI_LBP_E_MODE:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) 		OnSortMode();
		break;
	case GCI_LBP_E_MAP:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) 		OnSortMap();
		break;
	case GCI_LBP_E_TITLE:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) 		OnSortName();
		break;
	case GCI_LBP_E_STATE:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED)			OnSortState();
		break;
	case GCI_LBP_E_PERSON:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED)			OnSortPerson();
		break;
	case GCI_LBP_E_PING:
		if( pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED)			OnSortPing();
		break;
	case GCI_LBP_L_CHATLIST:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)			m_pChatBody->OnDragChatList();
		break;
	case GCI_LBP_L_CHAT_CHANNEL_TYPE1:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 		m_pChatBody->OnChatChannel(0);
		break;
	case GCI_LBP_L_CHAT_CHANNEL_TYPE2:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 		m_pChatBody->OnChatChannel(1);
		break;
	case GCI_LBP_L_CHAT_CHANNEL_TYPE3:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 		m_pChatBody->OnChatChannel(2);
		break;
	case GCI_LBP_L_CHAT_CHANNEL_TYPE4:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 		m_pChatBody->OnChatChannel(3);
		break;
	case GCI_LBP_B_CHAT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnChatCombo();
		break;
	case GCI_LBP_L_CHAT:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChatPopup(pNotify->m_nPriParam);
		break;
	case GCI_LBP_L_USERLIST_SCROLL:
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent)			OnUserListFocus(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent)			OnUserListClick(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_RCLICK == pNotify->m_nEvent)			OnUserListSelect(pNotify->m_nPriParam);	
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)			OnUserListScroll();
		if (I3GUI_LIST_NOTIFY_DOUBLECLICK == pNotify->m_nEvent)		
		{
			PlaySound(GUI_SND_ITEM_SELECTING);
			
			OnUserDetail(pNotify->m_nPriParam);
		}
		break;		
//#ifdef USE_ABUSING_CHECK
	case GCI_LBP_A_BUTTON_ABUSING_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnAbusingConfirm();
		break;
//#endif
	case GCI_LP_ITEMWIN_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnItemConfirm();
		break;
	case GCI_LP_EVENTWIN_CONFIRM:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)		OnEventConfirm();
		break;
	}

}

void CPhaseLobby::OnGameEvent(INT32 event,INT32 arg)
{
	CReadyPhaseBase::OnGameEvent(event, arg);

	switch(event)
	{
	case EVENT_READY_BATTLE:
		if( SLOT_STATE_LOAD == arg )
		{
			OnExitEnd();

			UINT32 StageID		= g_pGameContext->GetMyRoomStageID();
			char szTemp[256];
			
			getStageName(szTemp, (STAGE_ID) StageID);
			g_pGameContext->SetEvent(EVENT_HACKCHECK_STAGECODENAME, szTemp);

			getStageCodeName( szTemp, (STAGE_ID) StageID);
			g_pFramework->GotoNextStage( szTemp);
		}
		break;
	case EVENT_CREATE_NICK_NAME:
		if( EV_SUCCESSED(arg))
		{
			g_pGameContext->SetMyUserInfoRecord()->Reset();
			g_pGameContext->SetMyNickName(g_pGameContext->getUserNick());

#if defined (_FROST_)
			frostSetCharName(g_pGameContext->GetMyNickName());
#endif
			
#if defined(USE_LINK_SHOP)
			USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
			g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);
			if(InfoEquipWeapon._noprim != 0)
			{
				ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_EQUIP_SUPPLIED"), MAKE_CALLBACK(CbSuppliedConfirm), this);
			}
			else
			{
				(((CStageReady*)m_pParent)->TogglePopup(POPUP_LINK_SHOP, 0));
			}
#else

			ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_EQUIP_SUPPLIED"), MAKE_CALLBACK(CbSuppliedConfirm), this);
			
#endif

			UpdateCharacter(VT_DEFAULT);

			m_bNickSpace = false;
			((CStageReady*)m_pParent)->OutroNickSpace();
		}
		else
		{
			if( EV_IS_ERROR(arg,EVENT_ERROR_EVENT_DUPLICATE_NICK))
			{
				_LOG_("Same NickName");
				ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_SAME_NICK"), MAKE_CALLBACK(CbSameNick), this);
			}
			else
			{
				_LOG_("NickName Fail");
				SYSTEM_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_FAIL_NICK_E"), EXIT_CALLBACK(CbExitNormal), this);
			}
		}
		break;
	case EVENT_GET_FIRST_SUPPLY:
		//m_pEquipView->InitViewCharacter();
		break;
	case EVENT_JOIN_QUICK:
		if( EV_SUCCESSED(arg))
		{			
			((CStageReady*)m_pParent)->MovePhase(PHASE_READY_ROOM);
		}
		else
		{
			m_bAllowRefresh = TRUE;
			i3GuiRoot::setGuiDisableInput(FALSE);
			if( EV_IS_ERROR(arg,EVENT_ERROR_FAIL)) EhWrongQuickJoin();
			else EhStageError(event,arg);
		}
		break;
	case EVENT_CREATE_ROOM:
 		if( EV_SUCCESSED(arg))
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
			UINT32 StageID = g_pGameContext->GetMyRoomStageID();

			if (STAGE_SLOT_MODE_4VS4 == getStageSlot(StageID) )
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
		}
		break;
	case EVENT_JOIN_ROOM:
		i3GuiRoot::setGuiDisableInput(FALSE);
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
#if defined(EVENT_ERROR_ROOM_JOIN_VOTE)
			else if (EV_IS_ERROR(arg, EVENT_ERROR_ROOM_JOIN_VOTE))					EhKickedByVote();
#endif
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM))	EhNotExist();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD))	EhWrongPW();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER))	EhLimitInterenter();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_LEVEL))		EhLimitLever();
			else if( EV_IS_ERROR(arg, EVENT_ERROR_EVENT_JOIN_ROOM_TRAINING))		EhLimitTraining();
			else EhStageError(event,arg);
		}
		break;
	case EVENT_MATCH_TEAM_JOIN:			//	클랜 팀 참가
		{
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
		}
		break;
	case EVENT_LEAVE_LOBBY:
		if( EV_SUCCESSED(arg))
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
			ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_SERVER_FAIL_REFRESH_C"), EXIT_CALLBACK(CbExitNormal), this);
		}
		break;
	case EVENT_GET_ROOM_ADDINFO_N:
		m_bRequestRoomInfoAdd = FALSE;
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
		_BuildUserInfoList();
		break;
	case EVENT_READYBOX_MESSAGE:
		//보급 상자 채팅 메세지를 띈운다.
		ApplyMsgReadyBox();
		break;

	default:			
		/* Do nothing */
		break;
	}
}

BOOL CPhaseLobby::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strTemp[MAX_STRING_COUNT];

	LinkControlEx(plist, "Lobby", m_pLobbyGui, -1);
	LinkControlEx(plist, "lbInfoWin", m_pInfoWin, -1);
	LinkControlEx(plist, "lbRoomWin", m_pRoomWin, -1);
	LinkControlEx(plist, "lbCreateWin", m_pCreateWin, -1);
	LinkControlEx(plist, "lbNicknameWin", m_pNicknameWin, -1);
    LinkControlEx(plist, "lbInfoWinCaption", m_pInfoCaption, -1);
	LinkControlEx(plist, "lbRoomWinCaption", m_pRoomCaption, -1);
	LinkControlEx(plist, "lbRoomServerInfo", m_pServerInfoValue, -1);	

	/*LinkControlEx(plist, "lbTeam", m_pTeam, GCI_LBP_B_TEAM);
	LinkControlEx(plist, "lbMission", m_pMission, GCI_LBP_B_MISSION);*/

	//EscapeMission : 공룡 모드 창 
	LinkControlEx(plist, "lbDinoWin", m_pDinoWin, -1);
	LinkControlEx(plist, "lbDinoCaption", m_pDinoCaption, -1);
	LinkControlEx(plist, "lbDinoCancel", m_pDinoCancel, GCI_LBP_B_DINO_ESCAPE_CANCEL);
	
	// 탈출 미션
	LinkControlEx(plist, "DinoEscapeButton", m_Dino[ DINO_ESCAPE].pButton, GCI_LBP_S_DINO_ESCAPE_BUTTON);
	LinkControlEx(plist, "lbDinoButtonEscapeDescription", m_Dino[ DINO_ESCAPE].pButtonDesc, GCI_LBP_E_DINO_ESCAPE_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowDinoEscape", m_Dino[ DINO_ESCAPE].pArrow, GCI_LBP_S_DINO_ESCAPE_ARROW);
	LinkControlEx(plist, "lbBtnDinoEscapeTitleDesc", m_Dino[ DINO_ESCAPE].pBtnSTitleDesc, GCI_LBP_E_BUTTON_DINO_ESCAPE_TITLE_DESCRIPTION);
	LinkControlEx(plist, "DinoEscapeEmpty", m_Dino[ DINO_ESCAPE].pEmpty, -1);

	// 제압 미션
	LinkControlEx(plist, "DinoDominationButton", m_Dino[ DINO_DOMINATION].pButton, GCI_LBP_S_DINO_DOMINATION_BUTTON);
	LinkControlEx(plist, "lbDinoButtonDominationDescription", m_Dino[ DINO_DOMINATION].pButtonDesc, GCI_LBP_E_DINO_DOMINATION_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowDinoDomination", m_Dino[ DINO_DOMINATION].pArrow, GCI_LBP_S_DINO_DOMINATION_ARROW);
	LinkControlEx(plist, "lbBtnDinoDominationTitleDesc", m_Dino[ DINO_DOMINATION].pBtnSTitleDesc, GCI_LBP_E_BUTTON_DINO_DOMINATION_TITLE_DESCRIPTION);
	LinkControlEx(plist, "DinoDominationEmpty", m_Dino[ DINO_DOMINATION].pEmpty, -1);
	
	
	// 특수전
	LinkControlEx(plist, "lbSpecialWarWin", m_pSpecialWarWin, -1);
	LinkControlEx(plist, "lbSpecialWarCaption", m_pSpecialWarCaption, -1);
	LinkControlEx(plist, "lbSpecialWarCancel", m_pSpecialWarCancel, GCI_LBP_B_SPECIAL_WAR_CANCEL);


	//AI전
	LinkControlEx(plist, "SpecialWarChallengeButton", m_SpecialWar[ SP_MODE_CHALLENGE].pButton, GCI_LBP_S_SPECIAL_WAR_CHALLENGE_BUTTON);
	LinkControlEx(plist, "lbSpecialWarButtonChallengeDescription", m_SpecialWar[ SP_MODE_CHALLENGE].pButtonDesc, GCI_LBP_E_SPECIAL_WAR_CHALLENGE_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowChallenge", m_SpecialWar[ SP_MODE_CHALLENGE].pArrow, GCI_LBP_S_SPECIAL_WAR_CHALLENGE_ARROW);
	LinkControlEx(plist, "lbBtnChallengeTitleDesc", m_SpecialWar[ SP_MODE_CHALLENGE].pBtnSTitleDesc, GCI_LBP_E_BUTTON_CHALLENGE_TITLE_DESCRIPTION);
	LinkControlEx(plist, "SpecialWarChallengeEmpty", m_SpecialWar[ SP_MODE_CHALLENGE].pEmpty, -1);


	//샷건전
	LinkControlEx(plist, "SpecialWarShotgunButton", m_SpecialWar[ SP_MODE_SHOTGUN].pButton, GCI_LBP_S_SPECIAL_WAR_SHOTGUN_BUTTON);
	LinkControlEx(plist, "lbSpecialWarButtonShotgunDescription", m_SpecialWar[ SP_MODE_SHOTGUN].pButtonDesc, GCI_LBP_E_SPECIAL_WAR_SHOTGUN_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowShotgun", m_SpecialWar[ SP_MODE_SHOTGUN].pArrow, GCI_LBP_S_SPECIAL_WAR_SHOTGUN_ARROW);
	LinkControlEx(plist, "lbBtnShotgunTitleDesc", m_SpecialWar[ SP_MODE_SHOTGUN].pBtnSTitleDesc, GCI_LBP_E_BUTTON_SHOTGUN_TITLE_DESCRIPTION);
	LinkControlEx(plist, "SpecialWarShotgunEmpty", m_SpecialWar[ SP_MODE_SHOTGUN].pEmpty, -1);


	//스나전
	LinkControlEx(plist, "SpecialWarSniperButton", m_SpecialWar[ SP_MODE_SNIPER].pButton, GCI_LBP_S_SPECIAL_WAR_SNIPER_BUTTON);
	LinkControlEx(plist, "lbSpecialWarButtonSniperDescription", m_SpecialWar[ SP_MODE_SNIPER].pButtonDesc, GCI_LBP_E_SPECIAL_WAR_SNIPER_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowSniper", m_SpecialWar[ SP_MODE_SNIPER].pArrow, GCI_LBP_S_SPECIAL_WAR_SNIPER_ARROW);
	LinkControlEx(plist, "lbBtnSniperTitleDesc", m_SpecialWar[ SP_MODE_SNIPER].pBtnSTitleDesc, GCI_LBP_E_BUTTON_SNIPER_TITLE_DESCRIPTION);
	LinkControlEx(plist, "SpecialWarSniperEmpty", m_SpecialWar[ SP_MODE_SNIPER].pEmpty, -1);


	//헤드헌터전
	LinkControlEx(plist, "SpecialWarHeadHunterButton", m_SpecialWar[ SP_MODE_HEADHUNTER].pButton, GCI_LBP_S_SPECIAL_WAR_HEADHUNTER_BUTTON);
	LinkControlEx(plist, "lbSpecialWarButtonHeadHunterDescription", m_SpecialWar[ SP_MODE_HEADHUNTER].pButtonDesc, GCI_LBP_E_BUTTON_HEADHUNTER_TITLE_DESCRIPTION);
	LinkControlEx(plist, "lbArrowHeadHunter", m_SpecialWar[ SP_MODE_HEADHUNTER].pArrow, GCI_LBP_S_SPECIAL_WAR_HEADHUNTER_ARROW);
	LinkControlEx(plist, "lbBtnHeadHunterTitleDesc", m_SpecialWar[ SP_MODE_HEADHUNTER].pBtnSTitleDesc, GCI_LBP_E_BUTTON_HEADHUNTER_TITLE_DESCRIPTION);
	LinkControlEx(plist, "SpecialWarHeadHunterEmpty", m_SpecialWar[ SP_MODE_HEADHUNTER].pEmpty, -1);


	//너클전
	LinkControlEx(plist, "SpecialWarKnuckleButton", m_SpecialWar[ SP_MODE_KNUCKLE].pButton, GCI_LBP_S_SPECIAL_WAR_KNUCKLE_BUTTON);
	LinkControlEx(plist, "lbSpecialWarButtonKnuckleDescription", m_SpecialWar[ SP_MODE_KNUCKLE].pButtonDesc, GCI_LBP_E_SPECIAL_WAR_KNUCKLE_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowKnuckle", m_SpecialWar[ SP_MODE_KNUCKLE].pArrow, GCI_LBP_S_SPECIAL_WAR_KNUCKLE_ARROW);
	LinkControlEx(plist, "lbBtnKnuckleTitleDesc", m_SpecialWar[ SP_MODE_KNUCKLE].pBtnSTitleDesc, GCI_LBP_E_BUTTON_KNUCKLE_TITLE_DESCRIPTION);
	LinkControlEx(plist, "SpecialWarKnuckleEmpty", m_SpecialWar[ SP_MODE_KNUCKLE].pEmpty, -1);


	//튜토리얼전
	LinkControlEx(plist, "SpecialWarTutorialButton", m_SpecialWar[ SP_MODE_TUTORIAL].pButton, GCI_LBP_S_SPECIAL_WAR_TUTORIAL_BUTTON);
	LinkControlEx(plist, "lbSpecialWarButtonTutorialDescription", m_SpecialWar[ SP_MODE_TUTORIAL].pButtonDesc, GCI_LBP_E_SPECIAL_WAR_TUTORIAL_BUTTON_DESCRIPTION);
	LinkControlEx(plist, "lbArrowTutorial", m_SpecialWar[ SP_MODE_TUTORIAL].pArrow, GCI_LBP_S_SPECIAL_WAR_TUTORIAL_ARROW);
	LinkControlEx(plist, "lbBtnTutorialTitleDesc", m_SpecialWar[ SP_MODE_TUTORIAL].pBtnSTitleDesc, GCI_LBP_E_BUTTON_TUTORIAL_TITLE_DESCRIPTION);
	LinkControlEx(plist, "SpecialWarTutorialEmpty", m_SpecialWar[ SP_MODE_TUTORIAL].pEmpty, -1);
	

	// 여기까지 (특수전)

	LinkControlEx(plist, "lbQuickStart", m_pbtnQuickStart, GCI_LBP_B_QUICK_START);
	LinkControlEx(plist, "lbSpecialWar", m_pbtnSpecialWar, GCI_LBP_B_SPECIAL_WAR);
	LinkControlEx(plist, "lbDino", m_pbtnDino, GCI_LBP_B_DINO);

	LinkControlEx(plist, "lbServer", m_pServer, GCI_LBP_B_SERVER);
	LinkControlEx(plist, "lbEnter", m_pEnter, GCI_LBP_B_ENTER);
	LinkControlEx(plist, "lbRefresh", m_pRefresh, GCI_LBP_B_REFRESH);
	LinkControlEx(plist, "lbCreate", m_pCreate, GCI_LBP_B_CREATE);
	LinkControlEx(plist, "lbTeamCaption", m_pTeamCaption, -1);
	LinkControlEx(plist, "lbServerCaption", m_pServerCaption, -1);
	LinkControlEx(plist, "lbEnterCaption", m_pEnterCaption, -1);
	LinkControlEx(plist, "lbRefreshCaption", m_pRefreshCaption, -1);
	LinkControlEx(plist, "lbCreateCaption", m_pCreateCaption, -1);



	
	// 아이템 지급 팝업

	LinkControlEx(plist, "lbItemWin", m_pItemWin, -1);
	LinkControlEx(plist, "lbItemCaption", m_pItemCaption, -1);
	LinkControlEx(plist, "lbItemConfirm", m_pItemConfirm, GCI_LP_ITEMWIN_CONFIRM);
	LinkControlEx(plist, "lbItemName", m_pItemName, -1);
	LinkControlEx(plist, "lbItemDescription", m_pItemDescription, -1);
	LinkControlEx(plist, "lbItemBg", m_pItemBg, -1);
	LinkControlEx(plist, "lbItemDummy", m_pItemDummy, -1);
	LinkControlEx(plist, "lbItemBg2", m_pItemBg2, -1);
	LinkControlEx(plist, "lbItemDummy2", m_pItemDummy2, -1);


	// 아이템 지급 팝업

	LinkControlEx(plist, "lbEventWin", m_pEventWin, -1);

	LinkControlEx(plist, "lbEventLayout", m_pEventLayout, -1);

	
	LinkControlEx(plist, "lbEventTitle", m_pEventTitle, -1);
	LinkControlEx(plist, "lbEventCaption", m_pEventCaption, -1);
	LinkControlEx(plist, "lbEventConfirm", m_pEventConfirm, GCI_LP_EVENTWIN_CONFIRM);
	LinkControlEx(plist, "lbEventName", m_pEventName, -1);
	LinkControlEx(plist, "lbEventDescription", m_pEventDescription, -1);
	LinkControlEx(plist, "lbEventBg", m_pEventBg, -1);
	LinkControlEx(plist, "lbEventDummy", m_pEventDummy, -1);
	LinkControlEx(plist, "lbEventBg2", m_pEventBg2, -1);
	LinkControlEx(plist, "lbEventDummy2", m_pEventDummy2, -1);
	LinkControlEx(plist, "lbEventBg3", m_pEventBg3, -1);
	LinkControlEx(plist, "lbEventDummy3", m_pEventDummy3, -1);




	LinkControlEx(plist, "lbList", m_pList, GCI_LBP_L_LIST);
		
	for(UINT32 i = 0; i < MAX_ROOM_PER_PAGE_EX; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "lbSlot%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlot, GCI_LBP_S_SLOT_1 + i);

		sprintf_s(strName, "lbSlotIdx%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotIdx, -1);

		sprintf_s(strName, "lbSlotMode%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotMode, -1);

		sprintf_s(strName, "lbSlotMap%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotMap, -1);

		sprintf_s(strName, "lbSlotTitle%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotTitle, -1);

		sprintf_s(strName, "lbSlotState%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotState, -1);

		sprintf_s(strName, "lbSlotPerson%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotPerson, -1);

		sprintf_s(strName, "lbBattleMarkFront%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotBattleMarkFront, -1);

		sprintf_s(strName, "lbBattleMarkRear%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotBattleMarkRear, -1);		

		sprintf_s(strName, "lbKey%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotKey, -1);

		sprintf_s(strName, "lbPing%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotPing, -1);

		sprintf_s(strName, "lbNonRealIP%d", i + 1);
		LinkControlEx(plist, strName, m_roomSlot[i]._pSlotNonRealIP, -1);
		
	}

	LinkControlEx(plist, "lbSelect", m_pSelect, -1);
	LinkControlEx(plist, "lbFocus", m_pFocus, -1);

	LinkControlEx(plist, "lbImageDummy", m_pImageDummy, -1);
	LinkControlEx(plist, "lbImageDummy2", m_pImageDummy2, -1);
	LinkControlEx(plist, "lbImageDummy3", m_pImageDummy3, -1);
	LinkControlEx(plist, "lbImageDummy4", m_pImageDummy4, -1);
	LinkControlEx(plist, "lbImageXMas", m_pImageXMas, -1);
	LinkControlEx(plist, "lbMapKey", m_pMapKey, -1);
	LinkControlEx(plist, "lbMapValue", m_pMapValue, -1);
	LinkControlEx(plist, "lbModeKey", m_pModeKey, -1);
	LinkControlEx(plist, "lbModeValue", m_pModeValue, -1);
	LinkControlEx(plist, "lbTypeKey", m_pTypeKey, -1);
	LinkControlEx(plist, "lbTypeValue", m_pTypeValue, -1);
	LinkControlEx(plist, "lbHostKey", m_pHostKey, -1);
	LinkControlEx(plist, "lbHostValue", m_pHostValue, -1);
	LinkControlEx(plist, "lbStateKey", m_pStateKey, -1);
	LinkControlEx(plist, "lbStateValue", m_pStateValue, -1);
	LinkControlEx(plist, "lbJoinLimitKey", m_pJoinLimitKey, -1);
	LinkControlEx(plist, "lbJoinLimitValue", m_pJoinLimitValue, -1);
	LinkControlEx(plist, "lbWeaponLimitKey", m_pWeaponLimitKey, -1);
	LinkControlEx(plist, "lbWeaponLimitValue", m_pWeaponLimitValue, -1);
	LinkControlEx(plist, "lbObserverLimitKey", m_pObserverLimitKey, -1);
	LinkControlEx(plist, "lbObserverLimitValue", m_pObserverLimitValue, -1);
	LinkControlEx(plist, "lbTeamBalanceKey", m_pTeamBalanceKey, -1);
	LinkControlEx(plist, "lbTeamBalanceValue", m_pTeamBalanceValue, -1);

	LinkControlEx(plist, "lbIdxCaption", m_pIdxCaption, GCI_LBP_E_IDX);
	LinkControlEx(plist, "lbModeCaption", m_pModeCaption, GCI_LBP_E_MODE);
	LinkControlEx(plist, "lbMapCaption", m_pMapCaption, GCI_LBP_E_MAP);
	LinkControlEx(plist, "lbTitleCaption", m_pTitleCaption, GCI_LBP_E_TITLE);
	LinkControlEx(plist, "lbStateCaption", m_pStateCaption, GCI_LBP_E_STATE);
	LinkControlEx(plist, "lbPersonCaption", m_pPersonCaption, GCI_LBP_E_PERSON);
	LinkControlEx(plist, "lbPingCaption", m_pPingCatpion, GCI_LBP_E_PING);

	LinkControlEx(plist, "lbNicknameCaption", m_pNicknameCaption, -1);
	LinkControlEx(plist, "lbNicknameMessage", m_pNicknameMessage, -1);
	LinkControlEx(plist, "lbNicknameInput", m_pNicknameInput, -1);
	LinkControlEx(plist, "lbNicknameConfirm", m_pNicknameConfirm, GCI_LBP_B_NICKNAME);
	
	LinkControlEx(plist, "lbCreateWinCaption", m_pCreateWinCaption, -1);
	LinkControlEx(plist, "lbCreateTitleKey", m_pCreateTitleKey, -1);
	LinkControlEx(plist, "lbCreateTitleValue", m_pCreateTitleValue, -1);
	LinkControlEx(plist, "lbCreatePasswordKey", m_pCreatePasswordKey, -1);
	LinkControlEx(plist, "lbCreatePasswordValue", m_pCreatePasswordValue, -1);

	LinkControlEx(plist, "rrCreateImageDummy", m_pMapImage, -1);
	LinkControlEx(plist, "rrCreateImageDummy2", m_pTypeImage, -1);
	LinkControlEx(plist, "rrCreateImageDummy3", m_pMapNewMark, -1);
	LinkControlEx(plist, "rrCreateImageDummy4", m_pExtraTypeImage, -1);
	LinkControlEx(plist, "rrCreateImageXMas", m_pMapXMasMark, -1);
	LinkControlEx(plist, "rrCreateMap", m_pMap, -1);
	LinkControlEx(plist, "rrCreateMapCombo", m_pMapCombo, GCI_LBP_MAP_B_COMBO);	// ID
	LinkControlEx(plist, "rrCreateMapPopup", m_pMapList, GCI_LBP_MAP_L_COMBO);		// ID

	LinkControlEx(plist, "rrCreateModeKey", m_pGameModeKey, -1);
	LinkControlEx(plist, "rrCreateModeValue", m_pGameModeValue, -1);
	LinkControlEx(plist, "rrCreateModePrev", m_pGameModePrev, GCI_LBP_B_MODE_PREV);	// ID
	LinkControlEx(plist, "rrCreateModeNext", m_pGameModeNext, GCI_LBP_B_MODE_NEXT);	// ID

	#if defined( CHAMPETITION_BUILD )
		m_pGameModePrev->SetEnable( FALSE );
		m_pGameModeNext->SetEnable( FALSE );
	#endif

	LinkControlEx(plist, "rrCreateTypeKey", m_pGameTypeKey, -1);
	LinkControlEx(plist, "rrCreateTypeValue", m_pGameTypeValue, -1);
	LinkControlEx(plist, "rrCreateTypePrev", m_pGameTypePrev, GCI_LBP_B_TYPE_PREV);	// ID
	LinkControlEx(plist, "rrCreateTypeNext", m_pGameTypeNext, GCI_LBP_B_TYPE_NEXT);	// ID

	LinkControlEx(plist, "rrCreateTimeKey", m_pTimeKey, -1);
	LinkControlEx(plist, "rrCreateTimeValue", m_pTimeValue, -1);
	LinkControlEx(plist, "rrCreateTimePrev", m_pTimePrev, GCI_LBP_B_TIME_PREV);	// ID
	LinkControlEx(plist, "rrCreateTimeNext", m_pTimeNext, GCI_LBP_B_TIME_NEXT);	// ID

	LinkControlEx(plist, "rrCreateLimitKey", m_pLimitKey, -1);
	LinkControlEx(plist, "rrCreateLimitPrimary", m_pLimitPrimary, GCI_LBP_CHK_ADVENCED_PRIMARY);		// ID
	LinkControlEx(plist, "rrCreateLimitPrimaryKey", m_pLimitPrimaryKey, -1);
	LinkControlEx(plist, "rrCreateLimitPrimary_Lock", m_pLimitPrimaryLock, -1);
	LinkControlEx(plist, "rrCreateLimitSecondary", m_pLimitSecondary, -1);
	LinkControlEx(plist, "rrCreateLimitSecondaryKey", m_pLimitSecondaryKey, -1);
	LinkControlEx(plist, "rrCreateLimitSecondary_Lock", m_pLimitSecondaryLock, -1);
	LinkControlEx(plist, "rrCreateLimitMelee", m_pLimitMelee, -1);
	LinkControlEx(plist, "rrCreateLimitMeleeKey", m_pLimitMeleeKey, -1);
	LinkControlEx(plist, "rrCreateLimitMelee_Lock", m_pLimitMeleeLock, -1);
	LinkControlEx(plist, "rrCreateLimitThrow", m_pLimitThrow, -1);
	LinkControlEx(plist, "rrCreateLimitThrowKey", m_pLimitThrowKey, -1);
	LinkControlEx(plist, "rrCreateLimitThrow_Lock", m_pLimitThrowLock, -1);
	LinkControlEx(plist, "rrCreateLimitRPG7", m_pLimitRPG7, GCI_LBP_CHK_ADVENCED_RPG7);			// ID
	LinkControlEx(plist, "rrCreateLimitRPG7Key", m_pLimitRPG7Key, -1);
	LinkControlEx(plist, "rrCreateLimitRPG7_Lock", m_pLimitRPG7Lock, -1);

	LinkControlEx(plist, "rrObserver3PV_Free", m_pLimitObserver3PV_Free, GCI_LBP_CHK_LIMIT_OBSERVER_3PV_FREE);			// ID
	LinkControlEx(plist, "rrObserver3PV_FreeKey", m_pLimitObserver3PV_FreeKey, -1);
	LinkControlEx(plist, "rrObserver1PV_Free", m_pLimitObserver1PV_Free, GCI_LBP_CHK_LIMIT_OBSERVER_1PV_FREE);			// ID
	LinkControlEx(plist, "rrObserver1PV_FreeKey", m_pLimitObserver1PV_FreeKey, -1);
	LinkControlEx(plist, "rrObserverEnemy_Free", m_pLimitObserverEnemy_Free, GCI_LBP_CHK_LIMIT_OBSERVER_ENEMY_FREE);			// ID
	LinkControlEx(plist, "rrObserverEnemy_FreeKey", m_pLimitObserverEnemy_FreeKey, -1);
	LinkControlEx(plist, "rrObserverEnemyHP_Free", m_pLimitObserverEnemyHP, GCI_LBP_CHK_LIMIT_OBSERVER_ENEMY_HP);			// ID
	LinkControlEx(plist, "rrObserverEnemyHP_FreeKey", m_pLimitObserverEnemyHPKey, -1);
/////////////////////////////////////////////////////////////////////////////////////////////
//						Ranmdom MapPopup
/////////////////////////////////////////////////////////////////////////////////////////////	
	LinkControlEx(plist, "rrCheckRandomMap", m_pLobbyCheckRandomMap, GCI_LP_CHK_RANDOM);
	LinkControlEx(plist, "rrCreateRandomMap", m_pLobbyRandomMapEditBox, -1);
	LinkControlEx(plist, "rrLobbyRandomMapButton", m_pLobbyRandomMapButton, GCI_LP_B_RANDOM);

/////////////////////////////////////////////////////////////////////////////////////////////
	LinkControlEx(plist, "rrCreateLimitJoinKey", m_pLimitJoinKey, -1);
	LinkControlEx(plist, "rrCreateLimitJoinYesKey", m_pLimitJoinYesKey, -1);
	LinkControlEx(plist, "rrCreateLimitJoinNoKey", m_pLimitJoinNoKey, -1);
	LinkControlEx(plist, "rrCreateLimitJoinYes", m_pLimitJoinYes, GCI_LBP_B_JOIN_LIMIT_YES);		// ID
	LinkControlEx(plist, "rrCreateLimitJoinNo", m_pLimitJoinNo, GCI_LBP_B_JOIN_LIMIT_NO);		// ID
	LinkControlEx(plist, "rrCreateLimitObserverKey", m_pLimitObserverKey, -1);
//	LinkControlEx(plist, "rrCreateLimitObserverYesKey", m_pLimitObserverYesKey, -1);
//	LinkControlEx(plist, "rrCreateLimitObserverNoKey", m_pLimitObserverNoKey, -1);
//	LinkControlEx(plist, "rrCreateLimitObserverYes", m_pLimitObserverYes, GCI_LBP_B_OBSERVER_LIMIT_YES);	// ID
//	LinkControlEx(plist, "rrCreateLimitObserverNo", m_pLimitObserverNo, GCI_LBP_B_OBSERVER_LIMIT_NO);	// ID
	LinkControlEx(plist, "rrCreateTeamBalanceKey", m_pGameTeamBalanceKey, -1);
	LinkControlEx(plist, "rrCreateTeamBalanceYesKey2", m_pGameTeamBalancePersonnelKey, -1);
	LinkControlEx(plist, "rrCreateTeamBalanceYesKey", m_pGameTeamBalanceAbilityKey, -1);
	LinkControlEx(plist, "rrCreateTeamBalanceNoKey", m_pGameTeamBalanceNoKey, -1);
	LinkControlEx(plist, "rrCreateTeamBalanceYes2", m_pGameTeamBalancePersonnel, GCI_LBP_B_TEAM_BALANCE_PERSONNEL);		// ID
	LinkControlEx(plist, "rrCreateTeamBalanceYes", m_pGameTeamBalanceAbility, GCI_LBP_B_TEAM_BALANCE_ABILITY);		// ID
	LinkControlEx(plist, "rrCreateTeamBalanceNo", m_pGameTeamBalanceNo, GCI_LBP_B_TEAM_BALANCE_NO);		// ID

	LinkControlEx(plist, "lbCreateConfirm", m_pCreateConfirm, GCI_LBP_B_CREATE_CONFIRM);
	LinkControlEx(plist, "lbCreateCancel", m_pCreateCancel, GCI_LBP_B_CREATE_CANCEL);
	LinkControlEx(plist, "lbPasswordCheck", m_pCreatePasswordCheck, -1);
	// PC방 알림 팝업
	LinkControlEx(plist, "lbPcCafeWin", m_pPcCafeWin, -1);
	LinkControlEx(plist, "lbPcCafeCaption", m_pPcCafeCaption, -1);
	LinkControlEx(plist, "lbPcCafeConfirm", m_pPcCafeConfirm, GCI_LBP_B_PC_CAFE_CONFIRM);
	LinkControlEx(plist, "lbPcCafeMessage", m_pPcCafeMessage, -1);
	LinkControlEx(plist, "lbPcCafeExpKey", m_pPcCafeExpKey, -1);
	LinkControlEx(plist, "lbPcCafePointKey", m_pPcCafePointKey, -1);
	LinkControlEx(plist, "lbPcCafeItemKey", m_pPcCafeItemKey, -1);
	LinkControlEx(plist, "lbPcCafeExpValue", m_pPcCafeExpValue, -1);
	LinkControlEx(plist, "lbPcCafePointValue", m_pPcCafePointValue, -1);
	LinkControlEx(plist, "lbPcCafeItemValue", m_pPcCafeItemValue, -1);

	//i3GameNode *temp = m_pPcCafeItemValue->getParent()

	// 어뷰징 알림 팝업 : 20110331 하동익
	LinkControlEx(plist, "lbAbuseWindow", m_pAbuseWindow, -1);
	LinkControlEx(plist, "lbAbuseConfirm", m_pAbuseConfirm, GCI_LBP_A_BUTTON_ABUSING_CONFIRM);
	LinkControlEx(plist, "lbAbuseEditbox", m_pAbuseEditbox, -1);

	//	유저 목록
	LinkControlEx(plist, "lbUserListWin", m_pUserListWin, -1);
	LinkControlEx(plist, "lbUserListWinCaption", m_pUserListCaption, -1);
	LinkControlEx(plist, "lbUserListScroll", m_pUserListScroll, GCI_LBP_L_USERLIST_SCROLL);
	LinkControlEx(plist, "lbUserListSlotSelect", m_pUserListSelect, -1);
	LinkControlEx(plist, "lbUserListSlotFocus", m_pUserListFocus, -1);
	

	for(INT32 i=0; i<MAX_LOBBY_USER_SLOT; i++)
	{
		sprintf_s(strTemp, "lbUserListSlot%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pSlot, -1);

		sprintf_s(strTemp, "lbUserListClanMark%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pClanMark, -1);

		sprintf_s(strTemp, "lbUserListSlotRank%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pRank, -1);

		sprintf_s(strTemp, "lbUserListSlotRank_Beginner%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pBeginner, -1);

		sprintf_s(strTemp, "lbUserListSlotESport%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pESport, -1);
		
		sprintf_s(strTemp, "lbUserListSlotNick%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pNick, -1);

		sprintf_s(strTemp, "lbUserListSlotFilter%d", i + 1);
		LinkControlEx(plist, strTemp, m_pUserListSlot[ i]._pFilter, -1);
	}

	LinkControlEx(plist, "lbChatList", m_pChatBody->m_pChatList, GCI_LBP_L_CHATLIST);
	LinkControlEx(plist, "lbTextBox", m_pChatBody->m_pTextBox, -1);
	LinkControlEx(plist, "lbChatCombo", m_pChatBody->m_pChatCombo, -1);
	LinkControlEx(plist, "lbChatButton", m_pChatBody->m_pChatButton, GCI_LBP_B_CHAT);
	LinkControlEx(plist, "lbChatPopup", m_pChatBody->m_pChatPopup, GCI_LBP_L_CHAT);
	LinkControlEx(plist, "lbChatBox", m_pChatBody->m_pChatBox, -1);
	LinkControlEx(plist, "lbChatLang", m_pChatBody->m_pChatLang, -1);

	for(INT32 i=0; i<CHAT_CHANNEL_BUTTON_COUNT; i++)
	{
		sprintf_s(strTemp, "lbChatTab%d", i + 1);
		LinkControlEx(plist, strTemp, m_pChatBody->m_pChatChannelButton[i], GCI_LBP_L_CHAT_CHANNEL_TYPE1+i);
	}

	return FALSE;
}

void CPhaseLobby::OnInitControl(void)
{	
	// LobbyGui 기본 숨김
	m_pLobbyGui->SetEnable(FALSE);
	m_pCreateWin->SetEnable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);
	m_pSpecialWarWin->SetEnable(FALSE);

	//위치 이동
	{
		m_pCreateWin->setPositionY(
			m_pCreateWin->getPositionY() +
			LocaleValue::GetInt32("WinPosition/CreateRoom_Y")
			);
	}

	m_pDinoWin->SetEnable(FALSE); //EscapeMission

	INIT_WIN_CAPTION(m_pInfoCaption, GAME_STRING("STR_TBL_GUI_LOBBY_GAME_INFO_TEXT"));/*게임정보*/
	INIT_WIN_CAPTION(m_pRoomCaption, GAME_STRING("STR_TBL_GUI_LOBBY_ROOM_LIST_TEXT"));/*방목록*/

	//	서버 정보
	INIT_STATIC_CAPTION(m_pServerInfoValue, "");
	
	// 채팅 초기화
	_InitChat();

	// 버튼 초기화
	INIT_BUTTON_EX(m_pbtnQuickStart);
	INIT_BUTTON_EX(m_pbtnSpecialWar);
	INIT_BUTTON_EX(m_pbtnDino);//EscapeMission

	// 랜덤맵
#if defined (USE_RANDOMMAP_MODE)
	INIT_CAPTION_BUTTON(m_pLobbyRandomMapButton, GAME_STRING("STR_POPUP_RANDOMMAP_CONFIG"));
	m_pLobbyCheckRandomMap->SetEnable(TRUE);
#else
	m_pLobbyCheckRandomMap->SetEnable(FALSE);
#endif

	if( IsAvailableSpecialWar())
		m_pbtnSpecialWar->SetEnable( TRUE);
	else
		m_pbtnSpecialWar->SetEnable( FALSE);

	//m_BtnEffector.Set(m_pbtnSpecialWar);
	//m_BtnEffector1.Set(m_pCreate);
	//m_BtnEffector1.Set(m_pCreate);

	INIT_BUTTON_EX(m_pServer);
	INIT_BUTTON_EX(m_pEnter);
	INIT_BUTTON_EX(m_pRefresh);
	INIT_BUTTON_EX(m_pCreate);
	
//	m_pTeamCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
//	m_pServerCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
//	m_pEnterCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
//	m_pRefreshCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
//	m_pCreateCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
//	m_pTeamCaption->SetTextColor(GetColor(GCT_BLUE));
//	m_pServerCaption->SetTextColor(GetColor(GCT_BLUE));
//	m_pEnterCaption->SetTextColor(GetColor(GCT_BLUE));
//	m_pRefreshCaption->SetTextColor(GetColor(GCT_BLUE));
//	m_pCreateCaption->SetTextColor(GetColor(GCT_BLUE));
//	m_pTeamCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_QUICK_START_TEXT"));/*빠른시작*/
//	m_pServerCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_CHANGE_SERVER"));/*서버변경*/
//	m_pEnterCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ENTER_ROOM_TEXT"));/*입장하기*/
//	m_pRefreshCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_REFRESH_TEXT"));/*새로고침*/
//	m_pCreateCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_CREATE_ROOM_TEXT"));/*방만들기*/
	


	// GuiList 초기화
	m_pList->CreateTextEx(MAX_ROOM_PER_PAGE_EX,  GetDefaultFontName(), FONT_COUNT_256);
	m_pList->SetSelColor(0, 0, 0, 0);
	m_pList->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	

	for(UINT32 i = 0; i < MAX_ROOM_PER_PAGE_EX; i++)
	{
		m_roomSlot[i]._pSlotIdx->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotMode->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);		
		m_roomSlot[i]._pSlotMap->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotTitle->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotPerson->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotMap->SetOffsetPos(3.0f, 0.0f);
		m_roomSlot[i]._pSlotTitle->SetOffsetPos(3.0f, 0.0f);

		m_roomSlot[i]._pSlotMode->SetTextSpace( 0, -3);	// 러시아 방 모드 예외처리(타국가에서 문제 없어 보임)


		_SetRoomListColor( i, GetColor( GCT_INVALID_C));

		m_roomSlot[i]._pSlotBattleMarkFront->SetEnable(FALSE);
		m_roomSlot[i]._pSlotBattleMarkRear->SetEnable(FALSE);
		m_roomSlot[i]._pSlotKey->SetEnable(FALSE);
		m_roomSlot[i]._pSlotPing->SetEnable(FALSE);
		m_roomSlot[i]._pSlotNonRealIP->SetEnable(FALSE);
		
		m_roomSlot[i]._pSlotIdx->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotMode->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotMap->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotTitle->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotState->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotPerson->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotIdx->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotMode->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotMap->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotTitle->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotState->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotPerson->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotBattleMarkFront->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotBattleMarkRear->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotKey->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotPing->setInputDisable(TRUE);
		m_roomSlot[i]._pSlotNonRealIP->setInputDisable(TRUE);		
	}


	const UINT32 MaxCaption = 7;
	i3GuiEditBox* pCaption[MaxCaption] = {m_pIdxCaption, m_pModeCaption, m_pMapCaption, m_pTitleCaption,
						 	  m_pStateCaption, m_pPersonCaption, m_pPingCatpion};

	for(UINT32 i = 0; i < MaxCaption; i++)
	{
		pCaption[i]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		pCaption[i]->SetTextColor(GetColor(GCT_DEFAULT));
	}
	
	m_pIdxCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_NUMBER_TEXT"));/*번호*/
	m_pModeCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_GAME_MODE_TEXT"));/*게임모드*/
	m_pMapCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_MAP_INFO_TEXT"));/*맵정보*/
	m_pTitleCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ROOMNAME_TEXT2"));/*방이름*/
	m_pStateCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_STATE_TEXT"));/*상태*/
	m_pPersonCaption->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_PEOPLE_COUNT_TEXT"));/*인원수*/
	m_pPingCatpion->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_PING_TEXT"));/*핑*/
	

	// 리스트 셀럭트, 포커스 초기화
	m_pSelect->SetEnable(FALSE);
	m_pFocus->SetEnable(FALSE);

#if defined(LOCALE_INDONESIA) && defined(ENABLE_BOLD_CAPTION)
	{
		i3GuiEditBox* pKey[] = {m_pMapKey, m_pModeKey, m_pTypeKey, m_pHostKey, m_pStateKey,
								m_pJoinLimitKey, m_pWeaponLimitKey, m_pObserverLimitKey, m_pTeamBalanceKey};
		INT32 Count = GET_ARRAY_COUNT(pKey);
		for ( INT32 i = 0 ; i < Count ; ++i )
			pKey[i]->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FALSE, NULL, FW_BOLD);
	}
#endif
	// 방 정보
	const UINT32 MaxInfo = 18;
	i3GuiEditBox* pInfo[MaxInfo] = {m_pMapKey, m_pMapValue, m_pModeKey, m_pModeValue,
							 m_pTypeKey, m_pTypeValue, m_pHostKey, m_pHostValue,
							 m_pStateKey, m_pStateValue, m_pJoinLimitKey, m_pJoinLimitValue,
							 m_pWeaponLimitKey, m_pWeaponLimitValue, m_pObserverLimitKey,
							 m_pObserverLimitValue, m_pTeamBalanceKey, m_pTeamBalanceValue};

	for(UINT32 i = 0; i < MaxInfo; i++)
	{
		if (0 == i % 2)
		{
			pInfo[i]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			pInfo[i]->SetTextColor(GetColor(GCT_STATIC));
		}
		else
		{
			pInfo[i]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			pInfo[i]->SetTextColor(GetColor(GCT_FOCUS));
		}		
	}

	m_pMapKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_MAP_INFO_TEXT"));/*맵정보*/
	m_pModeKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_GAME_MODE_TEXT"));/*게임모드*/
	m_pTypeKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_GAME_SETTING_TEXT"));/*게임설정*/
	m_pHostKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ROOM_MASTER_TEXT"));/*방장*/
	m_pStateKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_PROGRESS_TEXT"));/*진행상황*/
	m_pJoinLimitKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_ENTER_TEXT"));/*난입제한*/
	m_pWeaponLimitKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_WEAPON_TEXT"));/*무기제한*/
	m_pObserverLimitKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_OBSERVER"));/*관전기능*/
	m_pTeamBalanceKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_TEAM_BALANCE_TEXT"));/*팀밸런스*/
	
	// 방 정보 텍스트 비움
	_ClearRoomInfo();


	// 닉네임 설정 창
	INIT_WIN_CAPTION(m_pNicknameCaption, GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"));/*알림*/

	m_pNicknameMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameMessage->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNicknameMessage->SetText(GAME_STRING("STBL_IDX_EP_LOBBY_SET_NICK"));


	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pNicknameInput->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNicknameInput->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameInput->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNicknameInput->SetText("");

	INIT_CAPTION_BUTTON(m_pNicknameConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/
	

	// 방생성창
	g_pFramework->SetMapShape(m_pMapImage, STAGE_ID_PORT_AKABA);
	m_pTypeImage->SetAllShapeEnable(FALSE);
	m_pTypeImage->setInputDisable(TRUE);
	m_pMapNewMark->SetEnable(FALSE);
	m_pMapNewMark->setInputDisable(TRUE);
	m_pMapXMasMark->SetEnable(FALSE);
	m_pMapXMasMark->setInputDisable(TRUE);

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
	// 랜덤 맵
	InitRandomMap();
	// 게임모드
	m_pGameModeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameModeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pGameModeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_GAME_MODE"));/*게임모드*/
	m_pGameModeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameModeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pGameModePrev);
	INIT_COMBO_EX(m_pGameModeNext);

	// 승리조건
	m_pGameTypeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameTypeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pGameTypeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_VICTORY_CONDITION"));/*승리조건*/
	m_pGameTypeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameTypeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pGameTypePrev);
	INIT_COMBO_EX(m_pGameTypeNext);

	// 제한시간
	m_pTimeKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTimeKey->SetTextColor(GetColor(GCT_STATIC));
	m_pTimeKey->SetText(GAME_STRING("STR_TBL_GUI_READY_LIMIT_TIME"));/*제한시간*/
	m_pTimeValue->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTimeValue->SetTextColor(GetColor(GCT_DEFAULT));
	INIT_COMBO_EX(m_pTimePrev);
	INIT_COMBO_EX(m_pTimeNext);

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
//	m_pLimitObserverYesKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
//	m_pLimitObserverNoKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameTeamBalanceKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameTeamBalancePersonnelKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameTeamBalanceAbilityKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pGameTeamBalanceNoKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	
	m_pLimitJoinKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitJoinYesKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitJoinNoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitObserverKey->SetTextColor(GetColor(GCT_DEFAULT));
//	m_pLimitObserverYesKey->SetTextColor(GetColor(GCT_DEFAULT));
//	m_pLimitObserverNoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pGameTeamBalanceKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pGameTeamBalancePersonnelKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pGameTeamBalanceAbilityKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pGameTeamBalanceNoKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pLimitJoinKey->SetText(GAME_STRING("STR_TBL_GUI_READY_LIMIT_JOINING"));/*난입제한*/
	m_pLimitObserverKey->SetText(GAME_STRING("STR_TBL_GUI_READY_OBSERVER_LIMIT"));/*관전기능*/
	m_pGameTeamBalanceKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE"));/*팀밸런스*/
	m_pLimitJoinYesKey->SetText(GAME_STRING("STR_TBL_GUI_READY_BEING"));/*있음*/
//	m_pLimitObserverYesKey->SetText(GAME_STRING("STR_TBL_GUI_READY_BEING"));/*있음*/
	m_pGameTeamBalancePersonnelKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON"));/*있음*/
	m_pGameTeamBalanceAbilityKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"));/*있음*/
	m_pLimitJoinNoKey->SetText(GAME_STRING("STR_TBL_GUI_READY_NONE"));/*없음*/
//	m_pLimitObserverNoKey->SetText(GAME_STRING("STR_TBL_GUI_READY_NONE"));/*없음*/
	m_pGameTeamBalanceNoKey->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_OFF"));/*없음*/

	INIT_CHECK_EX(m_pLimitJoinYes);
	INIT_CHECK_EX(m_pLimitJoinNo);	
//	INIT_CHECK_EX(m_pLimitObserverYes);
//	INIT_CHECK_EX(m_pLimitObserverNo);
	INIT_CHECK_EX(m_pGameTeamBalancePersonnel);
	INIT_CHECK_EX(m_pGameTeamBalanceAbility);
	INIT_CHECK_EX(m_pGameTeamBalanceNo);
	
	m_pLimitJoinYes->setCheckBoxState(FALSE);
	m_pLimitJoinNo->setCheckBoxState(FALSE);	
//	m_pLimitObserverYes->setCheckBoxState(FALSE);
//	m_pLimitObserverNo->setCheckBoxState(FALSE);
	m_pGameTeamBalancePersonnel->setCheckBoxState(FALSE);
	m_pGameTeamBalanceAbility->setCheckBoxState(FALSE);
	m_pGameTeamBalanceNo->setCheckBoxState(FALSE);

	INIT_CAPTION_BUTTON(m_pCreateConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/
	INIT_CAPTION_BUTTON(m_pCreateCancel, GAME_STRING("STR_TBL_GUI_LOBBY_CANCEL_TEXT"));/*취소*/
	INIT_WIN_CAPTION(m_pCreateWinCaption, GAME_STRING("STR_TBL_GUI_LOBBY_CREATE_ROOM_TEXT"));/*방만들기*/

	const UINT32 MaxCreate = 4;
	i3GuiEditBox* pCreate[MaxCreate] = { m_pCreateTitleKey, m_pCreateTitleValue, 
										 m_pCreatePasswordKey, m_pCreatePasswordValue};

	for(UINT32 i = 0; i < MaxCreate; i++)
	{
		pCreate[i]->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		pCreate[i]->SetTextColor(GetColor(GCT_DEFAULT));
	}


	m_pCreateTitleKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ROOMNAME_TEXT1"));/*방제목*/
	m_pCreatePasswordKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_PASSWORD_TEXT"));/*비밀번호*/

	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_ROOM_PW;
	ime._StyleIME = IME_STYLE_NUMERIC_ONLY | IME_STYLE_IGNORE_KEYINPUT_IME_MODE;
	m_pCreatePasswordValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pCreatePasswordValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCreatePasswordValue->SetAlphaNumericOnly( true);

	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_ROOM_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pCreateTitleValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pCreateTitleValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pCreateTitleValue->SetIMEMode( IME_MODE_NATIVE);
	m_pCreateTitleValue->SetTextAutoWrap( FALSE);
	m_pCreateTitleValue->SetOffsetPos(3.0f, 0.0f);

	

	_InitItemWin();
	_InitEventWin();
	// EscapeMission
	INIT_WIN_CAPTION(m_pDinoCaption, GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_CAPTION"));
	INIT_CAPTION_BUTTON(m_pDinoCancel, GAME_STRING("STR_TBL_GUI_LOBBY_CANCEL_TEXT"));/*취소*/


	for(INT32 i=0; i<DINO_MODE_MAX; i++)
	{
		const char * pszButton = UNKNOWN_TEXT;
		const char * pszTitle = UNKNOWN_TEXT;

		// 공룡모드 활성화를 위해 추가 (pEmpty = !bActivate, Ect = bActivate)
		bool bActivate = true;

		m_Dino[i].pBtnSTitleDesc->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FALSE, NULL, FW_SEMIBOLD, TRUE, FONT_SHADOW_QUALITY_HIGH);

		switch( i)
		{
		case DINO_ESCAPE:
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_ESCAPE_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_ESCAPE_TEXT");
			break;
		case DINO_DOMINATION:
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_DOMINATION_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_DOMINATION_TEXT");
#if !defined (DEV_BUILD)
			// Dev 빌드가 아닌 국가는 제압미션 비활성화 
			bActivate = false;
#endif
			break;
		}

		INIT_STATIC_CAPTION(m_Dino[ i].pButtonDesc, pszButton);
		INIT_NORMAL_CAPTION(m_Dino[ i].pBtnSTitleDesc, pszTitle);
		
		m_Dino[ i].pEmpty->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 100);	
		m_Dino[ i].pEmpty->SetEnable( !bActivate);
		
		m_Dino[ i].pBtnSTitleDesc->SetEnable(bActivate);
		m_Dino[ i].pButtonDesc->SetEnable(bActivate);
		m_Dino[ i].pButton->SetEnable(bActivate);
		m_Dino[ i].pArrow->SetEnable(bActivate);
	}


	// 특수전
	INIT_WIN_CAPTION(m_pSpecialWarCaption, GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_CAPTION"));
	INIT_CAPTION_BUTTON(m_pSpecialWarCancel, GAME_STRING("STR_TBL_GUI_LOBBY_CANCEL_TEXT"));/*취소*/

	for(INT32 i=0; i<SP_MODE_MAX; i++)
	{
		const char * pszButton = UNKNOWN_TEXT;
		const char * pszTitle = UNKNOWN_TEXT;

		m_SpecialWar[i].pBtnSTitleDesc->ReCreateTextEx( GetDefaultFontName(), 
			FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FALSE, NULL, FW_SEMIBOLD, TRUE, FONT_SHADOW_QUALITY_HIGH);

		switch( i)
		{
		case SP_MODE_CHALLENGE:
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_AI_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_TRANING_TEXT");
			break;
		case SP_MODE_SHOTGUN:		
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_SHOTGUN_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_SHOTGUNMODE_TITLE");
			break;
		case SP_MODE_SNIPER:		
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_SNIPER_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_SNIPERMODE_TITLE");
			break;
		case SP_MODE_HEADHUNTER:		
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_HEADHUNTER_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_HEADHUNTERMODE_TITLE");
			break;
		case SP_MODE_KNUCKLE:		
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_KNUCKLE_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_KNUCKLEMODE_TITLE");
			break;
		case SP_MODE_TUTORIAL:		
			pszButton = GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_WAR_TUTORIAL_BUTTON_DESCRIPTION");
			pszTitle = GAME_STRING("STR_TBL_GUI_LOBBY_TUTORIALMODE_TITLE");
			break;
		}

		INIT_STATIC_CAPTION(m_SpecialWar[ i].pButtonDesc, pszButton);
		INIT_NORMAL_CAPTION(m_SpecialWar[ i].pBtnSTitleDesc, pszTitle);

		m_SpecialWar[ i].pEmpty->SetColor( I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 100);	
		m_SpecialWar[ i].pEmpty->SetEnable( TRUE);
		m_SpecialWar[ i].pButton->SetEnable( FALSE);
		m_SpecialWar[ i].pButtonDesc->SetEnable( FALSE);
		m_SpecialWar[ i].pBtnSTitleDesc->SetEnable( FALSE);
		m_SpecialWar[ i].pArrow->SetEnable( FALSE);
	}

	// PC방 알림 팝업
	m_pPcCafeWin->SetEnable(FALSE);	
	INIT_WIN_CAPTION(m_pPcCafeCaption, GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_TEXT"));/*알림*/
	
	INIT_CAPTION_BUTTON(m_pPcCafeConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/

	m_pPcCafeMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafeMessage->SetTextColor(GetColor(GCT_DEFAULT));	
	m_pPcCafeExpKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafeExpKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPcCafeExpKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_EXP_PER_ROUND_TEXT"));/*경험치 획득량:*/
	m_pPcCafePointKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafePointKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPcCafePointKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_GAIN_POINT"));/*포인트 획득량:*/
	m_pPcCafeItemKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafeItemKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPcCafeItemKey->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_SPECIAL_ITEM"));/*전　용 아이템:*/	// 자간을 맞추기위해 특수문자공백을 포함
	m_pPcCafeExpValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafeExpValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPcCafePointValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafePointValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPcCafeItemValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPcCafeItemValue->SetTextColor(GetColor(GCT_DEFAULT));

	//#ifdef USE_ABUSING_CHECK
	// 어뷰징 알림 팝업 : 20110331 하동익
	m_pAbuseWindow->SetEnable(FALSE);
	m_pAbuseConfirm->SetEnable(FALSE);
	m_pAbuseConfirm->setClickAction(FALSE);
	m_pAbuseEditbox->SetEnable(FALSE);
	m_bAbuseFlag = FALSE;
	m_rAbusingTotalTime = 0.f;
	m_pAbuseEditbox->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pAbuseEditbox->SetTextColor(GetColor(GCT_DEFAULT));
	m_pAbuseEditbox->SetText("");
	//#else
	//m_pAbuseConfirm->SetEnable(FALSE);
	//m_pAbuseWindow->SetEnable(FALSE);
	//m_pAbuseEditbox->SetEnable(FALSE);
	//#endif

	//	유저 리스트
	m_pUserListWin->SetEnable(TRUE);

	INIT_WIN_CAPTION( m_pUserListCaption, GAME_STRING("STR_TBL_GUI_LOBBY_USER_LIST"));/*유저 리스트*/

	m_pUserListSelect->SetEnable(FALSE);
	m_pUserListFocus->SetEnable(FALSE);

	m_pUserListScroll->CreateTextEx(MAX_LOBBY_USER_SLOT,  GetDefaultFontName());
	m_pUserListScroll->SetSelColor(0, 0, 0, 0);

	for(INT32 i=0; i<MAX_LOBBY_USER_SLOT; i++)
	{
		m_pUserListSlot[ i]._pSlot->setInputDisable(TRUE);	

		m_pUserListSlot[ i]._pClanMark->SetEnable(TRUE);
		m_pUserListSlot[ i]._pClanMark->setInputDisable(TRUE);

		GameGUI::SetRankStatic(m_pUserListSlot[ i]._pRank);
		m_pUserListSlot[ i]._pRank->setInputDisable(TRUE);

		m_pUserListSlot[ i]._pBeginner->SetEnable(FALSE);
		m_pUserListSlot[ i]._pBeginner->setInputDisable(TRUE);

		m_pUserListSlot[ i]._pESport->SetEnable(FALSE);
		m_pUserListSlot[ i]._pESport->setInputDisable(TRUE);
		m_pUserListSlot[ i]._pESport->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게

		INIT_VALUE_CAPTION(m_pUserListSlot[ i]._pNick, "");
		m_pUserListSlot[ i]._pNick->setInputDisable(TRUE);
	}

	_ModifyStaticGui();
	
	CPhaseBase::OnInitControl();
}

//이정우(4월 1일) 특정 Gui를 비활성화 한다.
void CPhaseLobby::_ModifyStaticGui()
{
#if defined( LOCALE_TURKEY )
	//이정우(4월 1일) - 터키 요청으로 PCCafe(프리미엄 아이템 지급)을 수정한다.
	// PC방 알림 팝업중에 Static Gui
	i3GuiEditBox* pItemParent = (i3GuiEditBox*)m_pPcCafeItemKey->getParent();
	pItemParent->SetEnable(FALSE);
#endif
}


void CPhaseLobby::OnPreGuiNotify(void)
{
	CReadyPhaseBase::OnPreGuiNotify();

	m_pFocus->SetEnable(FALSE);
	m_pUserListFocus->SetEnable(FALSE);
	INT32 idxSlot = m_SelectedElement - m_pList->GetCurrentTop();

	if (idxSlot < 0 || idxSlot >= MAX_ROOM_PER_PAGE_EX)
	{
		m_pSelect->SetEnable(FALSE);
	}	
	else
	{
		m_pSelect->setPositionY(idxSlot * 33.f + 1.0f);
	}

	//	유저 목록 선택 바
	BOOL bEnable = FALSE;

	INT32 idxUserListSlot = m_nUserInfoListSelectIdx - m_pUserListScroll->GetCurrentTop();
	if (idxUserListSlot < 0 || idxUserListSlot >= MAX_LOBBY_USER_SLOT)
	{
		bEnable = FALSE;
	}	
	else
	{

		I3_BOUNDCHK( idxUserListSlot, MAX_LOBBY_USER_SLOT);

		if( m_pUserListSlot[idxUserListSlot]._pFilter->isEnable())
		{
			bEnable = FALSE;	

			//	포커스된 사람이 선택되었던 사람인지 비교한다.
			if( m_nUserInfoListSelectIdx < m_ViewUserInfoList.GetCount())
			{
				LOBBY_USER_INFO_LIST * pCurUserInfo = (LOBBY_USER_INFO_LIST*) m_ViewUserInfoList.GetItem(m_nUserInfoListSelectIdx);

				if( pCurUserInfo != NULL && m_SelectUserInfo._Session == pCurUserInfo->_Session)
					bEnable = TRUE;	
			}

			if( bEnable)
			{
				REAL32 fPosY = m_pUserListSlot[idxUserListSlot]._pSlot->getPositionY() + m_pUserListSlot[idxUserListSlot]._pFilter->getPositionY();
				REAL32 fPosX = m_pUserListSlot[idxUserListSlot]._pSlot->getPositionX();
				
				m_pUserListSelect->setPositionY( fPosY);
				m_pUserListSelect->setPositionX( fPosX);
			}
		}
		else
		{
			bEnable = FALSE;					
		}			
	}

	m_pUserListSelect->SetEnable(bEnable);
}

void CPhaseLobby::OnUpdate(REAL32 rDeltaSeconds)
{
	CReadyPhaseBase::OnUpdate(rDeltaSeconds);

	if (m_bAllowRefresh)
	{
		_AutoRefresh(rDeltaSeconds);
	}

	// 어뷰징 관련 시간 함수
//#ifdef USE_ABUSING_CHECK
	if(g_pGameContext->IsAbusing())
	{
		_ProcessAbusing(rDeltaSeconds);
	}
//#endif

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
		if(g_pGameContext->IsShowTutorialMessage())
		{
			((CStageReady*)m_pParent)->SetEffectBegin();
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_BASE_FINISH_TUTORIAL_MODE"));
			g_pGameContext->SetShowedMessage(true);
		}

		if (g_pGameContext->IsKickedByVote() )
		{
			USER_INFO_BASIC basic;
			g_pGameContext->GetMyUserInfoBasic(&basic);

			char szMsg[MAX_STRING_COUNT];
			StringCchPrintf(szMsg, MAX_STRING_COUNT, 
				GAME_STRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), basic._nick);
			//
			g_pFramework->PutSystemChatting(szMsg);
			//
			g_pGameContext->RemoveKickedByVoteFlag();
		}

	}
	else
	{
		//#ifdef USE_ABUSING_CHECK
		if(ButtonTermTime > 1.5f && !g_pGameContext->IsAbusing())
		{
			((CStageReady*)m_pParent)->SetEffectBegin();
		}
		else
		{
			ButtonTermTime += rDeltaSeconds;
		}
		//#else
		//if(ButtonTermTime > 1.5f)
		//{
		//	((CStageReady*)m_pParent)->SetEffectBegin();
		//}
		//else
	//	{
		//	ButtonTermTime += rDeltaSeconds;
	//	}
	//	#endif
	}

	if( g_pGameContext->IsFirstLogin())
	{
		_ProcessFirstLogin();
	}
	else
	{
		_ProcessCommon( rDeltaSeconds);
		_ProcessRoomInfo( rDeltaSeconds);
		_ProcessUserInfoList(rDeltaSeconds);
	}


	// 진급 포인트 시스템 - 하사이상 초기 보너스
	// 레벨업 체크
	if (g_pGameContext->IsInitialRankUp())
	{
		char rankUpMessage[MAX_STRING_COUNT];

		sprintf_s(rankUpMessage, GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_UPGRADE_CHARA"), g_InitialRankUpPoint);/*축하 드립니다!!\n \n진급 포인트 지급 이벤트로 {col:255,0,0,255}%d 포인트{/col}를\n진급 포인트로 지급합니다.\n \n(이후부터는 진급 시 포인트를 획득합니다.)*/
		STRING_POPUP(rankUpMessage);
		
	}	

	if (g_pGameContext->IsGeneralRankUp())
	{
		char generalrankUpMessage[MAX_STRING_COUNT];

		UINT32 point = g_pGameContext->GetRankUpPoint();

		sprintf_s(generalrankUpMessage, GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_GENERAL_RANK_UP"), point);/*진급을 축하 드립니다!!\n \n({col:255,0,0,255}%d 포인트{/col}를\n진급 포인트로 지급합니다.)*/
		STRING_POPUP(generalrankUpMessage);
		
		g_pGameContext->ResetRankUp();
		g_pGameContext->RemoveGameFlag( GAMEFLAG_GENERALRANKUP);
	}

	//	버튼이 깜빡입니다.
	/*BOOL Update = ! m_pSpecialWarWin->isEnable();
	m_BtnEffector.Update(Update, rDeltaSeconds);*/

	//m_BtnEffector.Update(Update, rDeltaSeconds);
	//m_BtnEffector1.Update(Update, rDeltaSeconds);
	//m_BtnEffector1.Update(Update, rDeltaSeconds);
}

void CPhaseLobby::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			if( m_pList->getIsFocus())
			{
				m_pList->MovePrev();
			}
			else if( m_pUserListScroll->getIsFocus())
			{
				m_pUserListScroll->MovePrev();

				m_bRefrashUserInfoList = TRUE;
			}
			else if( m_pChatBody->m_pChatList->getIsFocus())
			{
				m_pChatBody->m_pChatList->MovePrev();
			}
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
 			if( m_pList->getIsFocus())
			{
				m_pList->MoveNext();
			}
			else if( m_pUserListScroll->getIsFocus())
			{
				m_pUserListScroll->MoveNext();

				m_bRefrashUserInfoList = TRUE;
			}
			else if( m_pChatBody->m_pChatList->getIsFocus())
			{
				m_pChatBody->m_pChatList->MoveNext();
			}
		}			
	}


	if( m_pList->getIsFocus())
	{
		INT32 idxSlot = m_SelectedElement - m_pList->GetCurrentTop();

		if (0 <= idxSlot && idxSlot < MAX_ROOM_PER_PAGE_EX)
		{
			m_pSelect->setPositionY(idxSlot * 33.f + 1.0f);
			m_pSelect->SetEnable(TRUE);
		}
	}

	if( m_pChatBody->m_pChatList->getIsFocus())
	{
		g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatBody->m_pChatList->getCurValue()*0.01f, MAX_CHAT_LINE_FOR_LOBBY);	
	}
}



void CPhaseLobby::_ProcessSpecialMode( REAL32 rDeltaSeconds) // 특수전 활성화 처리
{
	g_pGameContext->ClearPreStageOption();

	// 서버에서 내려받은 리스트에서 각 스테이지의 속성을 체크하여 하나라도 해당 모드 속성을 가진 스테이지가 있으면 해당 특수전 GUI를 활성화한다.
	// 해당 모드를 가진 스테이지가 하나도 없을 경우 해당 특수전 버튼은 비활성화 된다.
	for(INT32 i=0; i<SP_MODE_MAX; i++)
	{
		BOOL bActive = FALSE;

		switch( i)
		{
		case SP_MODE_CHALLENGE:		bActive = IsSpecialMode(STAGE_OPTION_AI);			break;		//	특수전 AI모드(챌런지)이 on/off
		case SP_MODE_SHOTGUN:		bActive = IsSpecialMode(STAGE_OPTION_SHOTGUN);		break;		//	특수전 샷건이 on/off
		case SP_MODE_SNIPER:		bActive = IsSpecialMode(STAGE_OPTION_SNIPER);		break;		//	특수전 저격이 on/off
		case SP_MODE_HEADHUNTER:	bActive = IsSpecialMode(STAGE_OPTION_HEADHUNTER);	break;		//	특수전 헤드헌터 on/off
		case SP_MODE_KNUCKLE:	bActive = IsSpecialMode(STAGE_OPTION_KNUCKLE);	break;		//	특수전 너클전 on/off
		
		//튜토리얼 모드는 서버에서 내려준 맵 리스트에 튜토리얼 맵 유무에 따라 활성화 비 활성화 된다.
		case SP_MODE_TUTORIAL:	bActive = IsAvailableStageMode(STAGE_TYPE_TUTORIAL);	break;		//	특수전 튜토리얼 on/off
		}

		m_SpecialWar[ i].pEmpty->SetEnable( !bActive);
		m_SpecialWar[ i].pButton->SetEnable( bActive);
		m_SpecialWar[ i].pButtonDesc->SetEnable( bActive);
		m_SpecialWar[ i].pBtnSTitleDesc->SetEnable( bActive);
		m_SpecialWar[ i].pArrow->SetEnable( bActive);
	}
}

void CPhaseLobby::OnEntranceStart(void)
{
#ifdef NC_BUILD
	__ProhibitFile_AreYouCheating();
#endif

	if( IsAvailableStageMode(STAGE_TYPE_ESCAPE) )
	{
		m_pbtnDino->SetEnable(TRUE);
	}
	else
	{
		m_pbtnDino->SetEnable(FALSE);
		m_pbtnQuickStart->setPosition(57.0f,  585.0f);
		m_pbtnSpecialWar->setPosition(168.0f, 585.0f);
	}

	m_pChatBody->SetEntranceStart(this);

	if(getFirstChallengeStage() == STAGE_ID_NA)
	{
		m_SpecialWar[ SP_MODE_CHALLENGE].pEmpty->SetEnable( TRUE);
		m_SpecialWar[ SP_MODE_CHALLENGE].pButton->SetEnable( FALSE);
		m_SpecialWar[ SP_MODE_CHALLENGE].pButtonDesc->SetEnable( FALSE);
		m_SpecialWar[ SP_MODE_CHALLENGE].pBtnSTitleDesc->SetEnable( FALSE);
		m_SpecialWar[ SP_MODE_CHALLENGE].pArrow->SetEnable( FALSE);
	}

	//	클랜전 예외처리
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		CReadyPhaseBase::OnEntranceStart();

		// Event Message
		EventMessage();
	}

	g_pFramework->getChatBox()->setWhisperEnable( TRUE);
	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	//	현재 서버 위치 가져오기
	{
		char szServer[MAX_STRING_COUNT] = "";
		((CStageReady*)this->getParent())->GetCurrentSeverInfo( szServer, MAX_STRING_COUNT);

		m_pServerInfoValue->SetText( szServer);
	}

	//	유저 리스트 초기화
	_ResetUserInfoList();

#if defined (TEST_LOBBY_USER_LIST)
	_BuildUserInfoListDummy();
#endif

	m_ViewUserInfoList.Clear();

	//	로비에 들어오면 제일 먼저 서버에 상품 리스트를 요청한다. 
	//	커뮤니티에서 선물 쪽지 확인하기 전에 서버로부터 선물 리스트를 정상적으로 모두 받아야 선물 쪽지가 정상 작동하게 됩니다.  komet
	//	(상품 리스트와 선물 대조를 위해)
	g_pGameContext->SetEvent( EVENT_REQUEST_GOODS_LIST);	
}

BOOL CPhaseLobby::OnEntrancing(RT_REAL32 rDeltaSeconds)
{
	//	클랜전 예외처리

	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		return CReadyPhaseBase::OnEntrancing(rDeltaSeconds);
	}

	return TRUE;
}

void CPhaseLobby::OnEntranceEnd(void)
{
	CReadyPhaseBase::OnEntranceEnd();

	g_pGameContext->ClearPreStageOption();


	m_rEnterLobbyWaitTime = 0.0f;
	m_bAllowRefresh = TRUE;
	
	g_pGameContext->ClearInviteInfo();
	m_bCheckInvite = TRUE;

	//	클랜전 예외처리
	if( g_pFramework->GetClanContext()->GetVisibleLobbyPhase())
	{
		//  어뷰징 관련 하동익 20110402
		//	GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_LOADING_ROOM_LIST")
		//#ifdef USE_ABUSING_CHECK
		if(g_pGameContext->IsAbusing())
		{
			((CStageReady*)m_pParent)->SetEffectButtonStop(); //버튼깜박이 임시코드
			m_rAbusingTotalTime = 0.f;

			//서버에서 받은 어뷰징 레벨에 맞게 텍스트 설정 각 국가 번역 필요
			switch(g_pGameContext->GetAbuseLevel())
			{
			case ABUSING_LEVEL_NORMAL:
				m_pAbuseEditbox->SetText(GAME_STRING("STR_ABUSING_LEVEL_NORMAL"));
				break;
			case ABUSING_LEVEL_STRONG:
				m_pAbuseEditbox->SetText(GAME_STRING("STR_ABUSING_LEVEL_STRONG"));
				break;
			case ABUSING_LEVEL_STRONGEST:
				m_pAbuseEditbox->SetText(GAME_STRING("STR_ABUSING_LEVEL_STRONGEST"));
				break;
			}

			m_pAbuseWindow->SetEnable(TRUE);
			m_pAbuseEditbox->SetEnable(TRUE);
			m_pAbuseConfirm->SetEnable(TRUE);
		}
		else
		{
		
			if ( !g_pGameContext->IsKickedByVote() )
				m_pWaitRoomListMassageBox = ERROR_POPUP_W(GAME_STRING("STBL_IDX_EP_LOBBY_WAIT_ROOM_LIST_W"));/*방 리스트를 불러오고 있습니다.*/
			else	// 강퇴자의 경우 대기메세지 대신 강퇴되었다고 알려준다..
			{
				USER_INFO_BASIC basic;
				g_pGameContext->GetMyUserInfoBasic(&basic);

				char szMsg[MAX_STRING_COUNT];
				StringCchPrintf(szMsg, MAX_STRING_COUNT, 
					GAME_STRING("STR_ACK_FORCIBLYREMOVE_KICKED_BY_VOTE"), basic._nick);

				m_pWaitRoomListMassageBox = ERROR_POPUP_W(szMsg);/*방 리스트를 불러오고 있습니다.*/
			}
		}
	//	#else
	//	m_pWaitRoomListMassageBox = ERROR_POPUP_W(GAME_STRING("STBL_IDX_EP_LOBBY_WAIT_ROOM_LIST_W"));/*방 리스트를 불러오고 있습니다.*/
	//	#endif
	}
 
	// 클랜 체널에서는 챌린지 버튼 비활성

	if( IsAvailableSpecialWar())
	{
		if( g_pGameContext->GetBattleChannel() )
		{
#if defined (LOCALE_KOREA)
			m_pbtnSpecialWar->SetEnable( FALSE);		//	한국은 클랜 채널에서 특수전을 비활성화
#else
			m_pbtnSpecialWar->SetEnable( TRUE);
#endif
		}
		else
		{
			m_pbtnSpecialWar->SetEnable( TRUE);
		}
	}	
	else
	{
		m_pbtnSpecialWar->SetEnable( FALSE);
	}	

	m_pList->Clear();

	for(UINT32 i = 0; i < MAX_ROOM_PER_PAGE_EX; i++)
	{
		m_roomSlot[i]._pSlotIdx->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotMode->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotMap->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotTitle->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_roomSlot[i]._pSlotPerson->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

		_SetRoomListColor( i, GetColor( GCT_INVALID_C));

		m_roomSlot[i]._pSlotIdx->SetText("");
		m_roomSlot[i]._pSlotMode->SetText("");
		m_roomSlot[i]._pSlotMap->SetText("");
		m_roomSlot[i]._pSlotTitle->SetText("");
		m_roomSlot[i]._pSlotState->SetText("");
		m_roomSlot[i]._pSlotPerson->SetText("");

		m_roomSlot[i]._pSlotBattleMarkFront->SetEnable(FALSE);
		m_roomSlot[i]._pSlotBattleMarkRear->SetEnable(FALSE);
		m_roomSlot[i]._pSlotKey->SetEnable(FALSE);
		m_roomSlot[i]._pSlotPing->SetEnable(FALSE);
		m_roomSlot[i]._pSlotNonRealIP->SetEnable(FALSE);
	}


	// 클랜 모드일때 폭파 미션 모드로.
	if( g_pGameContext->GetBattleChannel() )
	{
		g_pGameContext->setQuickJoinType( STAGE_TYPE_BOMB);
	}
	else
	{
		g_pGameContext->setQuickJoinType( STAGE_TYPE_DEATHMATCH);
	}

	m_bRequestRoomInfoAdd = FALSE;
	m_SelectedElement = -1;

	// Battle에서 에러팝업 후 퇴장 시 처리
	if( g_pFramework->IsMessageBoxEnabled() )
	{
		i3GuiRoot::setGuiDisableInput(TRUE);
	}

	UpdateCharacter(VT_DEFAULT);

	// 이하 블록내 순서 중요합니다. - praptor
	{
		if (g_pCommunity->IsInviteMoving())
		{
			OnInviteMove();
			return;
		}

		if (g_pGameContext->IsMoveChannel() )
		{
			m_bAllowRefresh = FALSE;
			g_pGameContext->RemoveGameFlag( GAMEFLAG_MOVECHANNEL);

			i3GuiRoot::setGuiDisableInput(TRUE);
			g_pGameContext->SetEventOnce(EVENT_LEAVE_LOBBY);
			return;
		}

		//	클랜팀에 속해 있지 않으면 클랜 버튼을 활성화 한다.
		if( !g_pFramework->GetClanContext()->InBattleTeam())
		{
			SetPushedStateButton(MBT_CLAN, FALSE);  
		}
	}
	//#ifdef USE_ABUSING_CHECK
	if(g_pGameContext->IsAbusing()) // 하동익20110402 어뷰징 Processing중 입력 강제 막기
	{
		_SetInputDisableGUI(TRUE);
	}
//	#endif
}

void CPhaseLobby::OnExitStart(void)
{
	CPhaseBase::OnExitStart();
}

BOOL CPhaseLobby::OnExiting(RT_REAL32 rDeltaSeconds)
{
	return CPhaseBase::OnExiting(rDeltaSeconds);
}

void CPhaseLobby::OnExitEnd(void)
{
	CPhaseBase::OnExitEnd();

	m_pChatBody->OnExitEnd();

	if (m_pInviteMessageBox)
	{
		g_pFramework->CloseMessageBox(m_pInviteMessageBox);
	}

	if (m_pWaitRoomListMassageBox)
	{
		g_pFramework->CloseMessageBox(m_pWaitRoomListMassageBox);

		if (g_pGameContext->IsKickedByVote() )
			g_pGameContext->RemoveKickedByVoteFlag();
	}

	g_pFramework->getChatBox()->setWhisperEnable( FALSE);
	
	m_ViewUserInfoList.Clear();
}

void CPhaseLobby::OnSlideOut(BOOL bRight)
{
	CReadyPhaseBase::OnSlideOut(bRight);

	m_pChatBody->OnExitEnd();
}

void CPhaseLobby::OnSlideIn(BOOL bRight)
{
	CReadyPhaseBase::OnSlideIn(bRight);

	m_pChatBody->SetEntranceStart(this);
}

void CPhaseLobby::SetInputDisable(BOOL bDisable)
{
	m_pLobbyGui->setInputDisable(bDisable);
}

BOOL CPhaseLobby::GetInputDisable(void)
{
	return m_pLobbyGui->getInputDisable();
}

BOOL CPhaseLobby::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		// ContextMenu Event
		if (pObj && pObj->IsExactTypeOf(CContextMenu::GetClassMeta()))
		{ 
			switch(event)
			{
				case CMC_VIEW_DETAIL	:
					PlaySound(GUI_SND_ITEM_SELECTING);
					OnUserDetail( &m_SelectUserInfo);
					break;
				case CMC_WHISPER		:
					PlaySound(GUI_SND_ITEM_SELECTING);
					OnWhisper( &m_SelectUserInfo);
					break;
				case CMC_ADD_FRIEND		:
					PlaySound(GUI_SND_ITEM_SELECTING);
					OnAddFriend( &m_SelectUserInfo);
					break;

#if defined( USE_COMMUNITY_BLOCK)
				case CMC_ADD_BLOCK		:
					PlaySound(GUI_SND_ITEM_SELECTING);
					OnAddBlock( &m_SelectUserInfo);
					break;
#endif

				case CMC_CLAN_INVITE:
					if ( ! CanClanInvite() )
						return FALSE;

					PlaySound(GUI_SND_ITEM_SELECTING);
					OnClanInvite( &m_SelectUserInfo);
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
				if (m_pPcCafeWin->isEnable())
				{
					OnPcCafeConfirm();
					return TRUE;
				}
				if (m_pCreateWin->isEnable())
				{						
					OnCreateRoomCancelButtonClick();
					return TRUE;
				}
			
			}

			if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				if (m_pPcCafeWin->isEnable())
				{
					OnPcCafeConfirm();
					return TRUE;
				}
				//g_pSDOA_Link->SetSidabar(TRUE);
			}

			//if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			if( InputKeyboardEnter())
			{
				if (m_pCreateWin->isEnable())
				{
					BOOL bPasswordFocus = m_pCreatePasswordValue->getIsFocus();
					BOOL bTitleFocus = m_pCreateTitleValue->getIsFocus();

					if (FALSE == bPasswordFocus && FALSE == bTitleFocus)
					{
						// ENTER키는 focus상태의 IME control에서 처리하기때문에,
						// focus상태의 IME control이 없을 경우에만 처리한다.

						OnCreateRoomConfirmButtonClick();
						return TRUE;
					}
				}
			}

			if( m_pLobbyGui->isEnable() && !m_pNicknameWin->isEnable() &&
				!((CStageReady*)this->getParent())->IsEnablePopup( POPUP_OPTION) &&
				!((CStageReady*)this->getParent())->IsEnablePopup( POPUP_COMMUNITY) )
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

BOOL CPhaseLobby::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CReadyPhaseBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;	// 변경사항이 없다.
	}

	
	// 등장/퇴장 시간에 따른 좌표 오프셋 얻어내기
	m_pLobbyGui->setPositionX(CPhaseBase::GetSlideX());	


	// 퇴장 완료 시 비활성화. 그외 활성화.
	BOOL enableWindow = TRUE;

	if (SS_OUT == CReadyPhaseBase::GetSlideState())
	{
		enableWindow = FALSE;
	}
	
	m_pLobbyGui->SetEnable(enableWindow);

	return TRUE;
}

void CPhaseLobby::OnQuickStart(void)
{
	m_bAllowRefresh = FALSE;

	g_pGameContext->setQuickJoinType( STAGE_TYPE_DEATHMATCH);
	g_pGameContext->SetEventOnce(EVENT_JOIN_QUICK);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::StartTutorialMode()
{
	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	i3String::Copy(MyRoomInfo._Title, "Tutorial", NET_ROOM_NAME_SIZE );/*훈련전*/

	MyRoomInfo._StageID		= STAGE_ID_TUTORIAL;
	// MyRoomInfo._StageMask		= SET_STAGEMASK_TRAINING( MyRoomInfo._StageMask, 1 );
	MyRoomInfo._StageMask		= STAGE_MASK_TYPE_NONE;
	MyRoomInfo._WeaponFlag		= 0x0F;	
	
	// 16개의 슬롯을 연다
	MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT;

	// Add
	MyRoomInfoAdd._SubType				= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10;
	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x01;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance		= 0x00;	// 기본 밸런스 없음

	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);

	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::OnAdjustSpecialWarShape()
{
	REAL32 scn_height  = g_pEnvSet->m_nScreenHeight * 0.5f;	//게임 창 크기
	REAL32 height = m_pSpecialWarWin->getHeight() * 0.5f;	//특수전 창 크기

	m_pSpecialWarWin->setPositionY( scn_height - height );
}

void CPhaseLobby::OnSpecialWar(BOOL bFlag)
{
	m_pSpecialWarWin->SetEnable(bFlag);
	_SetInputDisableGUI( bFlag);
}
void CPhaseLobby::OnDino(BOOL bFlag)//EscapeMission 창
{
	m_pDinoWin->SetEnable(bFlag);
	_SetInputDisableGUI( bFlag);
}

void CPhaseLobby::On4vs4(void)
{
	m_bAllowRefresh = FALSE;

	g_pGameContext->setQuickJoinType(  STAGE_TYPE_ANNIHILATION);
	g_pGameContext->SetEventOnce(EVENT_JOIN_QUICK);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::OnSpecialWarChallenge(void)
{
	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	m_pSpecialWarWin->SetEnable(FALSE);
	m_pDinoWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);

	i3String::Copy(MyRoomInfo._Title, GAME_STRING("STR_TBL_GUI_LOBBY_TRAINING_ROOM_TITLE"), NET_ROOM_NAME_SIZE );/*훈련전*/

#if defined( USE_DUAL_AI_MODE)
	switch(MyRoomInfoAdd._AIState)
	{
	case AI_MODE_STATE_ORIGINAL:
		MyRoomInfo._StageID		= getFirstChallengeStage();
		break;
	case AI_MODE_STATE_DIEHARD:
		MyRoomInfo._StageID		= getFirstChallengeDieHardStage();
		break;
	}
#else
	MyRoomInfo._StageID		= getFirstChallengeStage();
#endif

	MyRoomInfo._StageMask		= STAGE_MASK_TYPE_TRAINING;
	MyRoomInfo._WeaponFlag		= 0x0F;	
	
	// 8개의 슬롯을 염
	MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT / 2;

	// Add
	switch( MyRoomInfo._StageID )
	{
		case STAGE_TYPE_DEATHMATCH	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
		case STAGE_TYPE_HEAD_HUNTER	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
		case STAGE_TYPE_SUDDEN_DEATH : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 	break; 
		case STAGE_TYPE_HEAD_KILLER : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 	break; 
		case STAGE_TYPE_ANNIHILATION: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
		case STAGE_TYPE_BOMB		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
		case STAGE_TYPE_DESTROY		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
		case STAGE_TYPE_DEFENCE		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
		case STAGE_TYPE_ESCAPE		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
		default						:		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10;			break; 
	}
	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance	= 0x00;	// 기본 밸런스 없음

	g_pGameContext->setEnemyAiCount(8);
	g_pGameContext->setDifficultyLevel(TRAINING_DIFFICULTY_LEVEL_LOW);
	g_pGameContext->setCurrentDifficultyLevel(TRAINING_DIFFICULTY_LEVEL_LOW);
	
	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bAllowRefresh = FALSE;
	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::OnSpecialWarShotgun(void)
{
	g_pGameContext->AddPreStageOption( STAGE_OPTION_SHOTGUN );
	OnCreateRoom();
}
void CPhaseLobby::OnSpecialWarHeadHunter(void)
{	
	g_pGameContext->AddPreStageOption( STAGE_OPTION_HEADHUNTER );
	OnCreateRoom();
}
void CPhaseLobby::OnSpecialWarKnuckle(void)
{	
	g_pGameContext->AddPreStageOption( STAGE_OPTION_KNUCKLE );

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	m_pSpecialWarWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);

	i3String::Copy(MyRoomInfo._Title, GAME_STRING("STR_TBL_GUI_LOBBY_KNUCKLEMODE_ROOM_TITLE"), NET_ROOM_NAME_SIZE );/*저격방 설명*/

	MyRoomInfo._StageID = getFirstKnuckleStage();
	
	// 16개의 슬롯을 염
	MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT;

	// Add
	switch( MyRoomInfo._StageID )
	{
	case STAGE_TYPE_DEATHMATCH	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_HEAD_HUNTER	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_SUDDEN_DEATH : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 	break; 
	case STAGE_TYPE_HEAD_KILLER : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 	break; 
	case STAGE_TYPE_ANNIHILATION: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_BOMB		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_DESTROY		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_DEFENCE		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_ESCAPE		: 	MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
	default						:		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10;			break; 
	}
	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance	= 0x01;	// 기본 밸런스 있음

	if( g_pGameContext->GetStageOption() == STAGE_OPTION_KNUCKLE ) 
	{
		MyRoomInfo._WeaponFlag = LOCK_KNUCKLE;
		g_pGameContext->RemovePreStageOption( STAGE_OPTION_KNUCKLE );
	}

	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bAllowRefresh = FALSE;
	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}
void CPhaseLobby::OnDinoEscape(void)
{

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	m_pSpecialWarWin->SetEnable(FALSE);
	m_pDinoWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);


	INT32 nCreateRoomRnd = i3Math::Rand()%3;

	// Room Name
	switch(nCreateRoomRnd)
	{
	case 0:	

		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1"), NET_ROOM_NAME_SIZE -1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1")) < (sizeof(char) * NET_ROOM_NAME_SIZE) );
		break;
	case 1:	
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2"), NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2")) < (sizeof(char) * NET_ROOM_NAME_SIZE) );
		break;
	case 2:	
#if defined (LOCALE_NORTHAMERICA) //NORTHAMERICA STBL_IDX_ESCAPE_ROOMNAME_3 길이가 23을 넘습니다.
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2"), NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2")) < (sizeof(char) * NET_ROOM_NAME_SIZE));
#else 
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_3"), NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_3")) < (sizeof(char) * NET_ROOM_NAME_SIZE));
#endif
		break;
	default: 
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1") ,NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1"))> (sizeof(char) * NET_ROOM_NAME_SIZE));
		break;
	}

	MyRoomInfo._StageID = getFirstDinoEscapeStage();
	MyRoomInfo._StageMask = STAGE_MASK_TYPE_ESCAPE;
	
	// 클랜채널에서는 슬롯을 10개만 연다
	// 그 외에는 16개의 슬롯을 연다
	if(g_pGameContext->GetBattleChannel())
		MyRoomInfo._PersonPlayerMax = 10;
	else
		MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT;

	// Add
	switch( MyRoomInfo._StageID )
	{
	case STAGE_TYPE_DEATHMATCH	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_HEAD_HUNTER	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_SUDDEN_DEATH : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 			break; 
	case STAGE_TYPE_HEAD_KILLER : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 			break; 
	case STAGE_TYPE_ANNIHILATION: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_BOMB		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_DESTROY		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_DEFENCE		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_ESCAPE		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_DOMINATION	: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
	default						:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;			break; 
	}
	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance	= 0x01;	// 기본 밸런스 있음


	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);
	
	m_bAllowRefresh = FALSE;
	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::OnDinoDomination(void)
{

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	m_pSpecialWarWin->SetEnable(FALSE);
	m_pDinoWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);


	INT32 nCreateRoomRnd = i3Math::Rand()%3;

	// Room Name
	switch(nCreateRoomRnd)
	{
	case 0:	
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1"), NET_ROOM_NAME_SIZE -1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1")) < (sizeof(char) * NET_ROOM_NAME_SIZE) );
		break;
	case 1:	
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2"), NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2")) < (sizeof(char) * NET_ROOM_NAME_SIZE) );
		break;
	case 2:	
#if defined (LOCALE_NORTHAMERICA) //NORTHAMERICA STBL_IDX_ESCAPE_ROOMNAME_3 길이가 23을 넘습니다.
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2"), NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_2")) < (sizeof(char) * NET_ROOM_NAME_SIZE));
#else 
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_3"), NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_3")) < (sizeof(char) * NET_ROOM_NAME_SIZE));
#endif
		break;
	default: 
		i3String::NCopy(MyRoomInfo._Title, GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1") ,NET_ROOM_NAME_SIZE-1);
		I3ASSERT(i3String::Length( GAME_STRING("STBL_IDX_ESCAPE_ROOMNAME_1"))> (sizeof(char) * NET_ROOM_NAME_SIZE));
		break;
	}


	MyRoomInfo._StageID = STAGE_ID_DEADTOWN;
	
	// 16개의 슬롯을 염
	MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT;

	//MyRoomInfo._StageMask = SET_STAGEMASK_ESCAPE( MyRoomInfo._StageMask, 1 );


	// Add
	switch( MyRoomInfo._StageID )
	{
	case STAGE_TYPE_DEATHMATCH	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_HEAD_HUNTER	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_SUDDEN_DEATH : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 			break; 
	case STAGE_TYPE_HEAD_KILLER : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 			break; 
	case STAGE_TYPE_ANNIHILATION: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_BOMB		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_DESTROY		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_DEFENCE		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_ESCAPE		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_DOMINATION	: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
	default						:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;			break; 
	}
	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance	= 0x01;	// 기본 밸런스 있음


	g_pGameContext->setEnemyAiCount(8);
	g_pGameContext->setDifficultyLevel(TRAINING_DIFFICULTY_LEVEL_LOW);
	g_pGameContext->setCurrentDifficultyLevel(TRAINING_DIFFICULTY_LEVEL_LOW);

	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bAllowRefresh = FALSE;
	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}


void CPhaseLobby::OnSpecialWarSniper(void)
{
	g_pGameContext->AddPreStageOption( STAGE_OPTION_SNIPER );

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	m_pSpecialWarWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);

	i3String::Copy(MyRoomInfo._Title, GAME_STRING("STR_TBL_GUI_LOBBY_SNIPERMODE_ROOM_TITLE"), NET_ROOM_NAME_SIZE );/*저격방 설명*/

	MyRoomInfo._StageID = getFirstSniperStage();
	
	// 16개의 슬롯을 염
	MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT;

	// Add
	switch( MyRoomInfo._StageID )
	{
	case STAGE_TYPE_DEATHMATCH	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_HEAD_HUNTER	: 		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10; 			break; 
	case STAGE_TYPE_SUDDEN_DEATH : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 	break; 
	case STAGE_TYPE_HEAD_KILLER : 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5; 	break; 
	case STAGE_TYPE_ANNIHILATION: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_BOMB		: 		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;		break; 
	case STAGE_TYPE_DESTROY		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
	case STAGE_TYPE_DEFENCE		:		MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;		break; 
		case STAGE_TYPE_ESCAPE		: 	MyRoomInfoAdd._SubType		= BATTLE_TIME_MISSION_TYPE_5 | BATTLE_ROUND_TYPE_3;		break; 
	default						:		MyRoomInfoAdd._SubType		= BATTLE_KILL_TYPE_100 | BATTLE_TIME_TYPE_10;			break; 
	}
	MyRoomInfoAdd._NowRoundCount		= 0;
	MyRoomInfoAdd._NowIngTime			= 0;
	MyRoomInfoAdd._LockInterEnter		= 0x00;	// 기본 가능함
	MyRoomInfoAdd._LockTeamBalance	= 0x01;	// 기본 밸런스 있음
	MyRoomInfo._WeaponFlag = SET_ROOM_BASIC_WEAPON_SNIPER( MyRoomInfo._WeaponFlag );

	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bAllowRefresh = FALSE;
	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::OnCreateRoom(void)
{
	char	szRoomName[MAX_STRING_COUNT] = {0,};

	//((CStageReady*)m_pParent)->SetEffectButtonStop();

	m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);

	ResetCreateRoomAddInfo();
	
	m_nStageID = s_StageInfoTableList.GetFirstStageId( 
		STAGE_TYPE_DEATHMATCH, 
		g_pGameContext->GetStageOption() );

	_UpdateMapInfo();

	m_bEnableCreateRoomMsgBox = TRUE;
	m_pCreateWin->SetEnable(TRUE);
	m_pSpecialWarWin->SetEnable(FALSE);
	m_pDinoWin->SetEnable(FALSE);
	_SetInputDisableGUI(TRUE);
	InitRandomMap();

	INT32 nCreateRoomRnd = i3Math::Rand()%FIXED_ROOMNAME_COUNT;

	// Room Name
	switch(nCreateRoomRnd)
	{
	case 0:	
		i3String::Copy(szRoomName, GAME_STRING("STBL_IDX_ROOMNAME_1"), MAX_STRING_COUNT );
		break;
	case 1:	
		i3String::Copy(szRoomName, GAME_STRING("STBL_IDX_ROOMNAME_2"), MAX_STRING_COUNT );
		break;
	case 2:	
		i3String::Copy(szRoomName, GAME_STRING("STBL_IDX_ROOMNAME_3"), MAX_STRING_COUNT );
		break;
	case 3:	
		i3String::Copy(szRoomName, GAME_STRING("STBL_IDX_ROOMNAME_4"), MAX_STRING_COUNT );
		break;
	default: 
		i3String::Copy(szRoomName, GAME_STRING("STBL_IDX_ROOMNAME_1"), MAX_STRING_COUNT );
		break;
	}

    m_pCreateTitleValue->SetIMEText( szRoomName);
	m_pCreateTitleValue->SetIMEMode( IME_MODE_NATIVE);
	m_pCreateTitleValue->SetFocus(TRUE);

	m_pCreatePasswordValue->SetIMEText("");

  	m_CreateGameMode = g_pGameContext->getQuickJoinType();

	//모든 특수전 및 방 만들기 공통
	m_pLimitMeleeLock->SetEnable(TRUE);	//자물쇠 표시

	//모든 체크 박스 활성화 한다.
	m_pLimitPrimary->setControlDisable( FALSE );
	m_pLimitSecondary->setControlDisable( FALSE );
	m_pLimitMelee->setControlDisable( FALSE );
	m_pLimitThrow->setControlDisable( FALSE );
	m_pLimitRPG7->setControlDisable( FALSE );

	//무기 제한 - 체크 박스
	m_pLimitPrimary->setCheckBoxState( TESTBIT(m_LockWeapon,	LOCK_PRIMARY));
	m_pLimitSecondary->setCheckBoxState( TESTBIT(m_LockWeapon,	LOCK_SECONDARY));

	if( TESTBIT(m_LockWeapon, LOCK_MELEE) )
	{
		m_pLimitMelee->setCheckBoxState( TESTBIT(m_LockWeapon, LOCK_MELEE) );
	}
	else
	{
		m_pLimitMelee->setCheckBoxState( TESTBIT(m_LockWeapon, LOCK_KNUCKLE));
	}

	m_pLimitThrow->setCheckBoxState( TESTBIT(m_LockWeapon,	LOCK_THROW));
	m_pLimitRPG7->setCheckBoxState( TESTBIT(m_LockWeapon,	LOCK_RPG7));


	//무기 제한 - 체크 박스 제한
	if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) || 
		g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) )	//스나이퍼 전
	{	
		m_pLimitPrimary->setCheckBoxState(TRUE);		//주무기 체크 표시
		m_pLimitPrimary->setControlDisable( TRUE);		//주무기 제한
		m_pLimitPrimaryLock->SetEnable( TRUE);		//주무기 자물쇠

		m_pLimitRPG7->setControlDisable( TRUE);		//RPG 제한
	}
	else if( g_pGameContext->IsPreStageOption( STAGE_OPTION_KNUCKLE ))
	{	
		m_pLimitPrimary->setControlDisable( TRUE);
		m_pLimitSecondary->setControlDisable( TRUE);
		//m_pLimitThrow->setControlDisable( TRUE);
		m_pLimitRPG7->setControlDisable( TRUE);

		m_pLimitPrimary->setCheckBoxState( FALSE );
		m_pLimitSecondary->setCheckBoxState( FALSE );
		m_pLimitThrow->setCheckBoxState( FALSE );
	}
	else	//방 만들기 모드
	{
		m_pLimitPrimary->setControlDisable( FALSE);
		m_pLimitSecondary->setControlDisable( FALSE );
		m_pLimitRPG7->setControlDisable( FALSE);

		//m_pLimitPrimaryLock->SetEnable( FALSE);
	}

	// 관전 기능
	m_pLimitObserver3PV_Free->setCheckBoxState( TESTBIT(m_LockObserver,	LOCK_OBSERVER_CHASE));
	m_pLimitObserver1PV_Free->setCheckBoxState( TESTBIT(m_LockObserver,	LOCK_OBSERVER_FREE));
	m_pLimitObserverEnemy_Free->setCheckBoxState( TESTBIT(m_LockObserver,	LOCK_OBSERVER_ENEMY));
	m_pLimitObserverEnemyHP->setCheckBoxState( TESTBIT(m_LockObserver,	LOCK_OBSERVER_ENEMY_HP));

	// 입장 제한
	OnJoinLimitCheck(1 == m_LockInterEnter);
	
	// 팀밸런스
	OnTeamBalanceCheck((INT32)m_LockTeamBalance);
	
	m_pLimitMelee->setControlState(I3GUI_CONTROL_STATE_DISABLED);
}

void CPhaseLobby::OnRefresh(void)
{
	// 선택되어 있는 방의 정보를 갱신하는 이벤트입니다.
	// 룸리스트를 갱신하는 이벤트가 아닙니다...
	// 왜 이런지 모르겠습니다.

	if ( m_SelectedElement == -1 )
		return;

	// 반복해서 눌러도 2초간격으로 되게 합니다.
	if ( g_pGameContext->GetAccTime() - m_LastRefreshTime <= g_pConfigEx->GetNetwork().RefreshInterval_RoomInfo)
		return;
	m_LastRefreshTime = g_pGameContext->GetAccTime();


	g_pGameContext->SetEvent( EVENT_GET_ROOM_ADDINFO_N );
	m_bRequestRoomInfoAdd = TRUE;
	I3TRACE0("EVENT_GET_ROOM_ADDINFO_N [CPhaseLobby::OnRefresh]\n");


	_SortRoomList();
	m_SelectedElement = -1;
}

void CPhaseLobby::OnServer(void)
{
	m_bAllowRefresh = FALSE;

	i3GuiRoot::setGuiDisableInput(TRUE);
	
	g_pGameContext->SetEventOnce(EVENT_LEAVE_LOBBY);
}

void CPhaseLobby::OnEnter(void)
{
	if (0 <= m_SelectedElement && m_SelectedElement < m_listRoom.GetCount())
	{
		OnEnter(m_SelectedElement);
	}
}

void CPhaseLobby::OnCreateRoomConfirmButtonClick(void)
{ 
	char szTemp[MAX_STRING_COUNT] = "";
	const char * pszRoomTitle = m_pCreateTitleValue->getIMETextBuf();
	BOOL bNeedFilter = TRUE;

	m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
	//((CStageReady*)m_pParent)->SetEffectButtonStop();

	// 빈 단어입력
	i3String::Copy( szTemp, pszRoomTitle, MAX_STRING_COUNT );
	i3String::RemoveAllBlank( szTemp );
	if( i3String::Length( szTemp ) <= 0) { 
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_PLEASE_WRITE_ROOM_NAME"));/*방 제목을 입력하여 주십시오*/
		return;
	}

	// 방생성 이름이 기본 제공하는 제목이라면 필터링하지 않는다. 기본 방제에서 필터에 걸려 방 생성이 안되는 문제를 해결함. komet
	for(INT32 i=0; i<4; i++)
	{
		i3String::Format(szTemp, MAX_STRING_COUNT, "STBL_IDX_ROOMNAME_%d", i+1);	// 1 ~ 4

		if( i3String::Compare( GAME_STRING( szTemp), pszRoomTitle) == 0)
		{
			bNeedFilter = FALSE;	// 기본 제공하는 방제목이므로 필터할 필요없다.
			break;
		}
	}	
	
	if( bNeedFilter)
	{
#if defined(LOCALE_CHINA)
		CStringFilter* pStringFilter = CStringFilter::NewObject();
		CStringFilter* pStringFilter2 = CStringFilter::NewObject();
		pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
		pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);

  		if( FALSE == g_pFramework->IsFilter( pszRoomTitle ) || i3String::ContainNoCase(szTemp, "{col:") >= 0 || FALSE == pStringFilter2->IsFilter( pszRoomTitle )
			|| FALSE == pStringFilter->IsFilterSame( pszRoomTitle ))

		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
			I3_SAFE_RELEASE( pStringFilter2 );
			I3_SAFE_RELEASE( pStringFilter );
			return;
		}	
		I3_SAFE_RELEASE( pStringFilter2 );
		I3_SAFE_RELEASE( pStringFilter );
#else

  	#if defined( LOCALE_NORTHAMERICA )
		if( FALSE == g_pFramework->IsFilter( pszRoomTitle ) 
			|| FALSE == g_pFramework->IsFilter2( pszRoomTitle )
			|| i3String::ContainNoCase(szTemp, "{col:") >= 0)
	#else
  		if( FALSE == g_pFramework->IsFilter( pszRoomTitle ) || i3String::ContainNoCase(szTemp, "{col:") >= 0)
	#endif
		{
			STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
			return;
		}

#endif

	}

	ROOM_INFO_BASIC		MyRoomInfo;
	ROOM_INFO_ADD		MyRoomInfoAdd;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	g_pGameContext->getMyRoomInfoAdd(&MyRoomInfoAdd);

	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);

	// 방생성 정보를 셋합니다!
	
	// 1.방이름
	{
		// ! praptor - IME에 버그 있습니다. 초기화시 넘겨준 최대길이보다 큰 문자열이
		// 넘어 올 수 있습니다. 문자열복사시 반드시 확인해야합니다.

		// IME에 의해 최대길이보다 큰 문자열이 들어왔을 경우 잘라낸다.
		char strTitle[MAX_STRING_COUNT];
		i3String::Copy(strTitle, m_pCreateTitleValue->getIMETextBuf(), MAX_STRING_COUNT );		

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

		i3String::Copy(MyRoomInfo._Title, strTitle, NET_ROOM_NAME_SIZE );
	}

	// 2.게임모드
	{			
		MyRoomInfo._StageID = m_nStageID;			
	}

	// 3.비밀방 여부
	if( i3String::Length(m_pCreatePasswordValue->getIMETextBuf()) > 0)
	{
		MyRoomInfo._InfoFlag = SET_ROOM_INFO_PRIVATE_MODE( MyRoomInfo._InfoFlag, 1 );
	}
		// 4. 비밀번호
		i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
		i3mem::Copy( g_pGameContext->getMyRoomPW(), m_pCreatePasswordValue->getIMETextBuf(), sizeof(char)*i3String::Length( m_pCreatePasswordValue->getIMETextBuf()));

	// 4. 게임 고급 설정 메뉴
	{			
		//
		//// 무기 제한	
		m_LockWeapon = 0; 

		if(m_pLimitPrimary->getCheckBoxState())	
			SETBIT( m_LockWeapon, LOCK_PRIMARY); 
		else
			UNSETBIT(m_LockWeapon, LOCK_PRIMARY); 

		if(m_pLimitSecondary->getCheckBoxState())
			SETBIT( m_LockWeapon, LOCK_SECONDARY); 
		else 
			UNSETBIT(m_LockWeapon, LOCK_SECONDARY); 

		if(m_pLimitRPG7->getCheckBoxState())
			SETBIT( m_LockWeapon, LOCK_RPG7); 
		else 
			UNSETBIT(m_LockWeapon, LOCK_RPG7); 

		if(m_pLimitThrow->getCheckBoxState())	
			SETBIT( m_LockWeapon, LOCK_THROW); 
		else 
			UNSETBIT(m_LockWeapon, LOCK_THROW); 

		//너클전, 칼전 구분
		if( g_pGameContext->IsPreStageOption( STAGE_OPTION_KNUCKLE ) )
			SETBIT( m_LockWeapon, LOCK_KNUCKLE);
		else
			SETBIT( m_LockWeapon, LOCK_MELEE);

		MyRoomInfo._WeaponFlag = m_LockWeapon;

		//
		// 관전기능
		//m_LockObserver = m_pLimitObserverYes->getCheckBoxState() ? 1 : 0;
		if(m_pLimitObserver3PV_Free->getCheckBoxState())	SETBIT( m_LockObserver, LOCK_OBSERVER_CHASE); 
		else												UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE); 

		if(m_pLimitObserver1PV_Free->getCheckBoxState())	SETBIT( m_LockObserver, LOCK_OBSERVER_FREE); 
		else												UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE); 

		if(m_pLimitObserverEnemy_Free->getCheckBoxState())	SETBIT( m_LockObserver, LOCK_OBSERVER_ENEMY); 
		else												UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY); 

		if(m_pLimitObserverEnemyHP->getCheckBoxState())		SETBIT( m_LockObserver, LOCK_OBSERVER_ENEMY_HP); 
		else												UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP); 

		// 난입제한
		m_LockInterEnter = m_pLimitJoinYes->getCheckBoxState() ? 1 : 0;			

		// 팀밸런스
		if(m_pGameTeamBalancePersonnel->getCheckBoxState())
			m_LockTeamBalance = 0x01;
		else if(m_pGameTeamBalanceAbility->getCheckBoxState())
			m_LockTeamBalance = 0x02;
		else
			m_LockTeamBalance = 0x00;

		MyRoomInfoAdd._SubType = m_SubType;
		//MyRoomInfoAdd._LockWeapon = m_LockWeapon;
		MyRoomInfoAdd._LockInterEnter = m_LockInterEnter;
		MyRoomInfoAdd._LockObserver = m_LockObserver;
		MyRoomInfoAdd._LockTeamBalance = m_LockTeamBalance;
	}

//	MyRoomInfo._StageMask = SET_STAGEMASK_TRAINING( MyRoomInfo._StageMask, 0 );

	if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) ) 
	{
		MyRoomInfo._StageMask = STAGE_MASK_TYPE_SHOTGUN;
		MyRoomInfo._WeaponFlag = SET_ROOM_BASIC_WEAPON_SHOTGUN( MyRoomInfo._WeaponFlag );
		//m_bEnableShotGunMode = FALSE;
		g_pGameContext->RemovePreStageOption( STAGE_OPTION_SHOTGUN );
	}
	else if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) ) 
	{
		MyRoomInfo._StageMask = STAGE_MASK_TYPE_SNIPER;
		MyRoomInfo._WeaponFlag = SET_ROOM_BASIC_WEAPON_SNIPER( MyRoomInfo._WeaponFlag );
		g_pGameContext->RemovePreStageOption( STAGE_OPTION_SNIPER );
	}
	else if( g_pGameContext->IsPreStageOption( STAGE_OPTION_HEADHUNTER ) ) 
	{
		MyRoomInfo._StageMask = STAGE_MASK_TYPE_HEADHUNTER;
		g_pGameContext->RemovePreStageOption( STAGE_OPTION_HEADHUNTER );
	}
	else if( g_pGameContext->IsPreStageOption( STAGE_OPTION_KNUCKLE ) ) 
	{
		MyRoomInfo._StageMask = STAGE_MASK_TYPE_KNUCKLE;
		MyRoomInfo._WeaponFlag = LOCK_KNUCKLE;
		g_pGameContext->RemovePreStageOption( STAGE_OPTION_KNUCKLE );
	}

	// 클랜채널에서는 슬롯을 10개만 연다
	// 그 외에는 16개의 슬롯을 연다
	if(g_pGameContext->GetBattleChannel())
		MyRoomInfo._PersonPlayerMax = 10;
	else
		MyRoomInfo._PersonPlayerMax = SLOT_MAX_COUNT;

	m_bAllowRefresh = FALSE;

	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	g_pGameContext->setMyRoomInfoAdd(&MyRoomInfoAdd);

	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);
	i3GuiRoot::setGuiDisableInput(TRUE);
}

void CPhaseLobby::OnCreateRoomCancelButtonClick(void)
{
	if( g_pGameContext->GetBattleChannel() )
	{
		g_pGameContext->setQuickJoinType( STAGE_TYPE_BOMB);
	}
	else
	{
		g_pGameContext->setQuickJoinType( STAGE_TYPE_DEATHMATCH);
	}
	m_bEnableCreateRoomMsgBox = FALSE;
	m_pCreateWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);

	m_pCreatePasswordValue->SetIMEText("");
	i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

	g_pGameContext->ClearPreStageOption();


	((CStageReady*)m_pParent)->SetEffectButtonBegin();
	//20110408 하동익 SetAnnounce 테스트
	//g_pFramework->SetAnnounceMessage2(GAME_STRING("STR_CHINA_PLAYTIME_1HOURS"));
}

void CPhaseLobby::OnEnter(INT32 idxRoom)
{
	if( m_SelectedElement == idxRoom)
	{
		_Join();
	}
	else
	{
		OnSlotClick(idxRoom);
	}
}

void CPhaseLobby::OnSlotClick(INT32 idxRoom)
{
	if (idxRoom >= m_listRoom.GetCount())
		return;

	ROOM_INFO_BASIC * pLobbyRoomInfo = (ROOM_INFO_BASIC*)m_listRoom.GetItem(idxRoom);
	if( ! pLobbyRoomInfo )
		return;

	// 같은 방의 슬롯을 반복해서 눌러도 2초간격으로 되게 합니다.
	if ( g_pGameContext->GetAccTime() - m_LastRefreshTime > g_pConfigEx->GetNetwork().RefreshInterval_RoomInfo)
	{
		m_LastRefreshTime = g_pGameContext->GetAccTime();

		m_SelectedElement	= idxRoom;
		UINT32 idxSlot		= idxRoom - m_pList->GetCurrentTop();

		m_pSelect->setPositionY(idxSlot * 33.f + 1.0f);
		m_pSelect->SetEnable(TRUE);

		g_pGameContext->setSelectedRoomIdx( pLobbyRoomInfo->_Idx);

		I3TRACE0("EVENT_GET_ROOM_ADDINFO_N [CPhaseLobby::OnSlotClick]\n");
		g_pGameContext->SetEvent( EVENT_GET_ROOM_ADDINFO_N );
		m_bRequestRoomInfoAdd = TRUE;
	}

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSlotOnMouse(INT32 idxSlot)
{
	INT32 idxRoom = idxSlot + m_pList->GetCurrentTop();

	if (idxRoom < m_listRoom.GetCount())
	{
		ROOM_INFO_BASIC* pLobbyRoomInfo = (ROOM_INFO_BASIC*)m_listRoom.GetItem(idxRoom);
		
		if (ROOM_STATE_EMPTY != pLobbyRoomInfo->_State)
		{
			m_pFocus->setPositionY(idxSlot * 33.f + 1.0f);
			m_pFocus->SetEnable(TRUE);
		}
	}
}

void CPhaseLobby::OnSlotDrag(void)
{
	if (-1 == m_SelectedElement) return;

	INT32 idxSlot = m_SelectedElement - m_pList->GetCurrentTop();

	if (0 <= idxSlot && idxSlot < MAX_ROOM_PER_PAGE_EX)
	{
		m_pSelect->setPositionY(idxSlot * 33.f + 1.0f);
		m_pSelect->SetEnable(TRUE);
	}
}

void CPhaseLobby::OnCreateRoomModePopup(UINT32 idxItem)
{
	switch(idxItem)
	{
	case 1:		m_CreateGameMode = STAGE_TYPE_BOMB;			break;
	case 2:		m_CreateGameMode = STAGE_TYPE_DESTROY;		break;	
	case 3:		m_CreateGameMode = STAGE_TYPE_ANNIHILATION;	break;
	case 4:		m_CreateGameMode = STAGE_TYPE_ESCAPE;			break;
	case 5 :	m_CreateGameMode = STAGE_TYPE_DEFENCE;		break;
	case 6 :	m_CreateGameMode = STAGE_TYPE_SUDDEN_DEATH;		break;
	case 7 :	m_CreateGameMode = STAGE_TYPE_HEAD_HUNTER;		break;
	case 8 :	m_CreateGameMode = STAGE_TYPE_HEAD_KILLER;		break;
	default:	m_CreateGameMode = STAGE_TYPE_DEATHMATCH;	break;
	}

	g_pGameContext->setQuickJoinType( m_CreateGameMode);
}

void CPhaseLobby::OnCreateRoomModeCombo(void)
{
	ExclusionPopup(m_pModePopup);
}


void CPhaseLobby::OnPcCafeConfirm(void)
{
	m_pPcCafeWin->SetEnable(FALSE);
	_SetInputDisableGUI(FALSE);

	ShowNoticePopup();
}

void CPhaseLobby::_AutoRefresh(REAL32 rDeltaSeconds)
{
	// 현재 스텝이 STEP_NULL이 아니면 방리스트를 보고 있는상태가 아니므로
	// 방리스트를 요청하지 않도록 한다.
	// 스텝, 페이지구분 등 명확히 안되어 있어서 이런식으로 코딩할 수 밖에 없다.
	CStageReady * pStageReady = ((CStageReady*)m_pParent);
	if ( STEP_NULL != pStageReady->GetCurrentStepType() )
		return;

	m_fRefreshElapsedTime -= rDeltaSeconds;
	if( m_fRefreshElapsedTime <= 0.0f)
	{		
#if defined(I3_DEBUG) && 0
		{
			static UINT32 a = 0;
			I3TRACE1("[%d]Refresh Room List....\n", a++);
		}
#endif // _DEBUG

		g_pGameContext->SetEvent(EVENT_LOBBY_REFRESH_N);

		_UpdateRoomList();
		m_fRefreshElapsedTime = g_pConfigEx->GetNetwork().RefreshInterval_RoomList;
	}
}

void CPhaseLobby::_ProcessFirstLogin(void)
{


	//상점바로가기 기능 관련 시간 저장
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	char szTemp[30];
	sprintf_s(szTemp,"%dMon%dDay",sysTime.wMonth,sysTime.wDay);
	
	// 처음 닉네임 만들기.
	// 처음 로그인.
	if( ! g_pGameContext->HaveNickName() )
	{ 
		_ShowNicknamePopup();
		m_bLogin =true;
	}
	else
	{
		FirstLoninPopup();

	}
#if defined(USE_LINK_SHOP)
	//현재 화면 모드 정보를 설정한뒤 날짜를 저장합니다.
	i3String::Copy(g_pEnvSet->m_szDay, szTemp, 12 );
	g_pEnvSet->m_bFullScreen = g_pViewer->isFullScreen();
	g_pEnvSet->Save();
#endif
	g_pGameContext->RemoveGameFlag( GAMEFLAG_FIRSTLOGIN);
}


void CPhaseLobby::OpenEventWin(INT32 ItemId)
{

	string name;
	m_pEventWin->SetEnable(true);
	m_pEventTitle->SetEnable(true);
	m_pEventLayout->SetEnable(true);

	i3GuiStatic * pStatic = NULL;
	pStatic = m_pEventDummy;
	m_pEventBg->SetEnable(TRUE);
	g_pFramework->SetSlotImage(pStatic, ItemId);

	GetItemName(name, ItemId);

	// 획득 보상 출력
	m_pEventName->SetText(name.c_str());

	m_pEventDescription->SetText(GAME_STRING("STBL_IDX_EP_CHRISTMAS_POPUP"));
	m_pEventDescription->SetEnable(TRUE);

	m_bEventNotice = TRUE;

}



void CPhaseLobby::OpenEventWin2(INT32 ItemId1,INT32 ItemId2)
{
	string name;
	m_pEventWin->SetEnable(true);
	m_pEventTitle->SetEnable(true);
	m_pEventLayout->SetEnable(true);

	i3GuiStatic * pStatic = NULL;
	i3GuiStatic * pStatic2 = NULL;
	pStatic = m_pEventDummy2;
	m_pEventBg2->SetEnable(TRUE);
	g_pFramework->SetSlotImage(pStatic, ItemId1);


	pStatic2 = m_pEventDummy3;
	m_pEventBg3->SetEnable(TRUE);
	g_pFramework->SetSlotImage(pStatic2, ItemId2);

	m_pEventDescription->SetText(GAME_STRING("STBL_IDX_EP_TELKOM_POPUP"));
	m_pEventDescription->SetEnable(TRUE);



}
void CPhaseLobby::OnEventConfirm()
{

	m_pEventWin->SetEnable(FALSE);

}
void CPhaseLobby::OpenItemWin(INT32 ItemId)
{

	string name;
	m_pItemWin->SetEnable(true);

	i3GuiStatic * pStatic = NULL;
	pStatic = m_pItemDummy2;
	m_pItemBg2->SetEnable(TRUE);
	g_pFramework->SetSlotImage(pStatic, ItemId);

	GetItemName(name, ItemId);

	// 획득 보상 출력
	m_pItemName->SetText(name.c_str());

	m_pItemDescription->SetText(GAME_STRING("STBL_IDX_EP_TELKOM_POPUP"));
	m_pItemDescription->SetEnable(TRUE);

}
void CPhaseLobby::OnItemConfirm()
{

	m_pItemWin->SetEnable(FALSE);

}
void CPhaseLobby::_InitItemWin(void)
{
	m_pItemWin->SetEnable(FALSE);


	INIT_WIN_CAPTION(m_pItemCaption, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));/*알림*/
	// 버튼 초기화
	INIT_CAPTION_BUTTON(m_pItemConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	// 에디트박스 초기화
	m_pItemName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pItemDescription->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pItemName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pItemDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pItemDescription->SetTextSpace(0, 3);
	m_pItemDescription->SetText(GAME_STRING("STR_TBL_GUI_READY_NOTICE_REWARD_ITEM1"));


	m_pItemBg->SetEnable(false);
	m_pItemDummy->SetEnable(false);


}
void CPhaseLobby::_InitEventWin(void)
{
	m_pEventWin->SetEnable(FALSE);
	m_pEventTitle->SetEnable(FALSE);
	m_pEventLayout->SetEnable(FALSE);


	INIT_WIN_CAPTION(m_pEventCaption, GAME_STRING("STR_TBL_GUI_READY_NOTICE"));/*알림*/
	// 버튼 초기화
	INIT_CAPTION_BUTTON(m_pEventConfirm, GAME_STRING("STR_TBL_GUI_READY_CONFIRM"));/*확인*/

	// 에디트박스 초기화
	m_pEventName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pEventDescription->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
	m_pEventName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pEventDescription->SetTextColor(GetColor(GCT_DEFAULT));
	m_pEventDescription->SetTextSpace(0, 3);
	m_pEventDescription->SetText(GAME_STRING("STR_TBL_GUI_READY_NOTICE_REWARD_ITEM1"));


	m_pEventBg->SetEnable(false);
	m_pEventDummy->SetEnable(false);
	m_pEventBg2->SetEnable(false);
	m_pEventDummy2->SetEnable(false);
	m_pEventBg3->SetEnable(false);
	m_pEventDummy3->SetEnable(false);


}

void CPhaseLobby::_ShowNicknamePopup(void)
{
	m_pNicknameWin->SetEnable(TRUE);
	m_pNicknameInput->SetFocus(TRUE);

	m_bEnableNicknameMsgBox = TRUE;
	_SetInputDisableGUI(TRUE);
}

void CPhaseLobby::_ProcessNickname(REAL32 rDeltaSeconds)
{
	// IME 에디트 입력받는 부분은 받드시 상태를 체크해서 처리해야 됩니다. komet 09.06
	// IME를 처리하는 Windows API Proc은 메시지 큐잉 방식으로 처리하기 때문입니다.
	if (GUI_IME_CONFIRM(m_pNicknameInput))
	{
		OnCreateNickname();
		m_pNicknameInput->InputDone();
	}
	else
	{
		m_bNickSpace = true;
		((CStageReady*)m_pParent)->EnterNickSpace();
	}
}

void CPhaseLobby::_ProcessCheckInvite(REAL32 rDeltaSeconds)
{
	i3TStack<INVITE_INFO>* pInfo = g_pGameContext->getStackInviteInfo();

	// 초대 메시지 검사
	if (pInfo->Pop(&m_InviteInfo))
	{	
		BOOL bPass = FALSE;

		// 옵션에 따라 초대하기 창을 활성/비활성 상태를 결정한다.
		switch( g_pEnvSet->m_nInviteType)
		{
		case COMMUNITY_PERMISSION_ALL		: bPass = TRUE;			break;		// 전체 허용	
		case COMMUNITY_PERMISSION_NO		: bPass = FALSE;		break;		// 전체 차단	
		case COMMUNITY_PERMISSION_FRIEND	: bPass = FALSE;		break;		// 친구/클랜 허용	
		default:bPass = TRUE;		break;			
		}

		// 친구/클랜 허용상태라면, 조건을 검색합니다.
		if( g_pEnvSet->m_nInviteType == COMMUNITY_PERMISSION_FRIEND)
		{
			INT32 i = 0;
			INT32 nFriendCount = g_pCommunity->GetFriendCount();
			for( i=0; i<nFriendCount; i++)
			{
				const FRIEND_NODE* pNode = g_pCommunity->GetFriend(i);
				if(pNode->_uid == m_InviteInfo._iUID)
				{
					bPass = TRUE;
					break;
				}
			}

			if( !bPass)
			{
				INT32 nClanMemberCount = g_pCommunity->GetClanMemberCount();
				for( i=0; i<nClanMemberCount; i++)
				{
					const CLAN_MEMBER_NODE* pNode = g_pCommunity->GetClanMember(i);
					if(pNode->_uid == m_InviteInfo._iUID)
					{
						bPass = TRUE;
						break;
					}
				}
			}
		}
		
		if( bPass)
		{
			// 한번에 하나씩 처리하기위한 메시지 검사 중지
			m_bCheckInvite = FALSE;

			char strBuf[MAX_STRING_COUNT] = "";

			// 수락 여부 확인		
			const char * str = GAME_STRING("STR_TBL_GUI_LOBBY_INVITE_MESSAGE1");		/*%d번 방의 %s님으로 부터 초대 요청 메시지가 *//*도착하였습니다.\n수락 하시겠습니까?*/
			std::string fmt = BOOST_FORMAT2( str, m_InviteInfo._idxRoom + 1, m_InviteInfo._nick);
			i3String::Copy( strBuf, fmt.c_str(), MAX_STRING_COUNT );
			
			m_pInviteMessageBox = STRING_POPUP_Q(strBuf, MAKE_CALLBACK(CbBeInvited), this);
		}
	}
}

void CPhaseLobby::OnCreateNickname(void)
{
	char strBuf[MAX_STRING_COUNT];

	i3String::Copy(strBuf, m_pNicknameInput->getIMETextBuf(), MAX_STRING_COUNT );

	if( 0 == i3String::Length(strBuf) )
	{
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_NICK_EMPTY"), MAKE_CALLBACK(CbSameNick), this);
		
		m_bEnableNicknameMsgBox = FALSE;
        m_pNicknameWin->SetEnable( FALSE);
		_SetInputDisableGUI(FALSE);
		return;
	}

	// NickName
	if (FALSE == CStringFilter::IsValidNickname(strBuf))
	{
		// ! praptor - 최소 글자수 포함되도록 내용 수정해야합니다.
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_NICK_LENGTH"), MAKE_CALLBACK(CbSameNick), this);
		m_bEnableNicknameMsgBox = FALSE;
        m_pNicknameWin->SetEnable( FALSE);
		_SetInputDisableGUI(FALSE);
		return;
	}


	if ( ! g_pFramework->CheckValidNickname(strBuf) )
	{
		ERROR_POPUP_C(GAME_STRING("STBL_IDX_EP_LOBBY_WRONG_NICK"), MAKE_CALLBACK(CbSameNick), this);
		m_bEnableNicknameMsgBox = FALSE; 
		m_pNicknameWin->SetEnable( FALSE);
		_SetInputDisableGUI(FALSE);
		return;
	}

	m_bEnableNicknameMsgBox = FALSE;
	m_pNicknameWin->SetEnable( FALSE);
	_SetInputDisableGUI(FALSE);

	g_pGameContext->setUserNick(strBuf);
	
	g_pGameContext->SetEvent(EVENT_CREATE_NICK_NAME);
}

void CPhaseLobby::OnSortNumber(void)
{
	_ChangeSortOrder(ROOM_SORT_NUMBER);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pIdxCaption->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSortMode(void)
{
	_ChangeSortOrder(ROOM_SORT_MODE);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pModeCaption->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSortMap(void)
{
	_ChangeSortOrder(ROOM_SORT_MAP);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pMapCaption->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSortName(void)
{
	//_ChangeSortOrder(ROOM_SORT_PRIVATE);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pTitleCaption->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSortState(void)
{
	_ChangeSortOrder(ROOM_SORT_STATE);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pStateCaption->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSortPerson(void)
{
	_ChangeSortOrder(ROOM_SORT_PERSON);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pPersonCaption->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPhaseLobby::OnSortPing(void)
{
	//_ChangeSortOrder(ROOM_SORT_PING);
	_SortRoomList();
	m_SelectedElement = -1;

	m_pPingCatpion->SetTextColor(GetColor(GCT_INVERSE));

	PlaySound(GUI_SND_ITEM_SELECTING);
}

//virtual
BOOL CPhaseLobby::OnInviteMove(void)
{
	m_bAllowRefresh = FALSE;


	if (g_pCommunity->GetMovingServer() != (UINT32)g_pServerInfo->GetCurGameServerIndex())
	{
		i3GuiRoot::setGuiDisableInput(TRUE);

		// 스탭, 팝업 닫는다.
		{
			CStageReady * pStageReady = ((CStageReady*)m_pParent);
			
			pStageReady->TogglePopup(POPUP_NULL);

			if (STEP_NULL != pStageReady->GetCurrentStepType())
			{
				pStageReady->ChangeStep( pStageReady->GetCurrentStepType());
			}
		}
		
		// 로비를 나간다
		g_pGameContext->SetEventOnce(EVENT_LEAVE_LOBBY);

		return TRUE;
	}
	
 	if (g_pCommunity->GetMovingChannel() != (UINT32)g_pServerInfo->GetCurChannelNumber())
	{
		i3GuiRoot::setGuiDisableInput(TRUE);		

		// 스탭, 팝업 닫는다.
		{
			CStageReady * pStageReady = ((CStageReady*)m_pParent);
			
			pStageReady->TogglePopup(POPUP_NULL);

			if (STEP_NULL != pStageReady->GetCurrentStepType())
			{
				pStageReady->ChangeStep( pStageReady->GetCurrentStepType());
			}
		}

		// 로비를 나간다
		g_pGameContext->SetEventOnce(EVENT_LEAVE_LOBBY);

		return TRUE;
	}

	{
		CStageReady * pStageReady = ((CStageReady*)m_pParent);
		
		pStageReady->TogglePopup(POPUP_NULL);

		if (STEP_NULL != pStageReady->GetCurrentStepType())
		{
			pStageReady->ChangeStep( pStageReady->GetCurrentStepType());
		}
	}

	// 방만들기 창 닫기
	OnCreateRoomCancelButtonClick();

	if( g_pCommunity->GetMovingMatch() > 0 )
	{
		UINT32	wMatch = g_pCommunity->GetMovingMatch() - 1;
		UINT8	cNaming = 1;
		g_pGameContext->SetEvent( EVENT_MATCH_TEAM_JOIN, &wMatch, &cNaming );
	}
	else
	{
		g_pGameContext->setMyRoomIndex(g_pCommunity->GetMovingRoom());

		m_SelectedElement = g_pCommunity->GetMovingRoom();

		BOOL bInvite = !g_pCommunity->IsVisitFriend();
		ROOM_INFO_BASIC	roomInfo;
		g_pGameContext->getMyRoomInfo(&roomInfo);
//		roomInfo._StageMask = SET_STAGEMASK_TRAINING( roomInfo._StageMask, 0 );
		g_pGameContext->setMyRoomInfo(&roomInfo);

		g_pGameContext->SetEventOnce(EVENT_JOIN_ROOM, &bInvite);
	}

	i3GuiRoot::setGuiDisableInput(TRUE);
	return TRUE;
}

void CPhaseLobby::OnChatCombo(void)
{
	ExclusionPopup(m_pChatBody->m_pChatPopup);
}

void CPhaseLobby::OnChatPopup(UINT32 idxItem)
{
	m_pChatBody->OnChatPopup(idxItem);
}

void CPhaseLobby::OnWhisper( LOBBY_USER_INFO_LIST * pUser)
{		
	if( pUser == NULL) return;

	if( pUser->_Session < 0 || pUser->_Nick[0] == 0)
	{
		return;
	}

	m_pChatBody->OnWhisper( pUser->_Nick);	
}

void CPhaseLobby::OnAddFriend( LOBBY_USER_INFO_LIST * pUser)
{
	if( pUser == NULL) return;

	if( pUser->_Session < 0 || pUser->_Nick[0] == 0) 
		return;
	
	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_NO_MORE_ADD_FRIEND"));/*더이상 친구를 추가할 수 없습니다.*/
		return;
	}

	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare( pUser->_Nick, node->_nick))
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_ALREADY_FRIEND"), pUser->_Nick);/*[%s]님은 이미 친구목록에 등록되어 있습니다.*/
			STRING_POPUP(message);
			return;
		}
	}

	if (g_pGameContext->IsMyNickName(pUser->_Nick))
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_ADD_CHARA"));/*자신을 추가할 수 없습니다.*/
		return;
	}

	g_pCommunity->InsertFriend( pUser->_Nick);
}

void CPhaseLobby::OnAddBlock( LOBBY_USER_INFO_LIST * pUser)
{
	char message[MAX_STRING_COUNT] = "";

	if (g_pGameContext->IsMyNickName(pUser->_Nick))
	{
		return;
	}

	//	"[ %s ]님을 차단하시겠습니까?"
	sprintf_s( message, GAME_STRING("STR_TBL_GUI_LOBBY_Q_BLOCK_USER"), pUser->_Nick);
	
	STRING_POPUP_Q( message, MAKE_CALLBACK(CbBlockUser), this);
}

void CPhaseLobby::DoBlockUser(void)
{
}

void CPhaseLobby::OnClanInvite( LOBBY_USER_INFO_LIST * pUser)
{
	char message[MAX_STRING_COUNT] = "";

	if( pUser == NULL) return;

	if( pUser->_Session < 0 || pUser->_Nick[0] == 0) 
		return;

	//	자기가 클랜 초대 기능이 있을 경우
	if( !g_pFramework->GetClanContext()->IsPossibleClanInvite())
	{	
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_NO_HAVE_CLAN_INVITE"));/*클랜 초대 권한이 없습니다.*/
		return;
	}

	if (g_pGameContext->IsMyNickName(pUser->_Nick)) 
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_ADD_CHARA"));/*자신을 추가할 수 없습니다.*/
		return;
	}

	//	"[ %s ]님에게 클랜 초대를 하시겠습니까?"
	sprintf_s( message, GAME_STRING("STR_TBL_GUI_LOBBY_Q_CLAN_INVITE_USER"), pUser->_Nick);
	
	STRING_POPUP_Q( message, MAKE_CALLBACK(CbClanInvite), this);
}

void CPhaseLobby::DoClanInvite(void)
{
	//	클랜 초대를 보낸다.
	g_pFramework->GetClanContext()->SendClanInvite( m_SelectUserInfo._Session, 2);
}

void CPhaseLobby::_ClearRoomInfo(void)
{
	g_pFramework->SetMapShape(m_pImageDummy, STAGE_ID_NA);	// Unknown
	m_pImageDummy2->SetAllShapeEnable(FALSE);
	m_pImageDummy3->SetEnable(FALSE);
	m_pImageDummy4->SetEnable(FALSE);
	m_pImageXMas->SetEnable(FALSE);

	m_pMapValue->SetText("");
	m_pModeValue->SetText("");
	m_pTypeValue->SetText("");
	m_pHostValue->SetText("");
	m_pStateValue->SetText("");
	m_pJoinLimitValue->SetText("");
	m_pWeaponLimitValue->SetText("");
	m_pObserverLimitValue->SetText("");
	m_pTeamBalanceValue->SetText("");
	
	m_pInfoWin->SetEnable(FALSE);
}

void CPhaseLobby::_ProcessCommon( REAL32 rDeltaSeconds)
{
	// 방생성 박스 갱신
	if(m_bEnableNicknameMsgBox)
	{
		_ProcessNickname(rDeltaSeconds);
	}
	else if (m_bEnableCreateRoomMsgBox)
	{
		_ProcessCreateRoom( rDeltaSeconds);
	}

	//Room List 갱신
	_ProcessRoomList( rDeltaSeconds);

	// 초대 메시지 검사
	if (m_bCheckInvite)
	{
		_ProcessCheckInvite(rDeltaSeconds);
	}

	// Chat 정보 갱신
	_ProcessChat( rDeltaSeconds);

	// 특수전
	if(m_pSpecialWarWin->isEnable())
	{
		_ProcessSpecialMode( rDeltaSeconds);

		OnAdjustSpecialWarShape();
	}

//이정우 - 주석 처리함 [ 키보드 ]
//#if defined( I3_DEBUG )
//	i3InputKeyboard* pKeyboard = g_pFramework->getKeyboard();
//	if( pKeyboard->GetPressState(I3I_KEY_LCTRL ) )
//	{
//		if( pKeyboard->GetPressState(I3I_KEY_F1 ) )
//		{
//			g_pFramework->getChatBox()->ResetChatBuffer();
//		}
//	}
//#endif

}	

void CPhaseLobby::_ProcessRoomList( REAL32 rDeltaSeconds)
{
	// 목록 톱 idx 기록
	UINT32 nOldTop = m_pList->GetCurrentTop();
	UINT32 nOldCount = m_pList->GetElementCount();


	// 기존 정보를 지운다
	m_pList->Clear();
	
	for(UINT32 i = 0; i < MAX_ROOM_PER_PAGE_EX; i++)
	{
		m_roomSlot[i]._pSlotBattleMarkFront->SetEnable(FALSE);
		m_roomSlot[i]._pSlotBattleMarkRear->SetEnable(FALSE);
		m_roomSlot[i]._pSlotKey->SetEnable(FALSE);
		m_roomSlot[i]._pSlotPing->SetEnable(FALSE);
		m_roomSlot[i]._pSlotNonRealIP->SetEnable(FALSE);		
	}


	// 새 정보를 기록한다
	for(INT32 i = 0; i < m_listRoom.GetCount(); i++)
	{		
		m_pList->AddElement("");
	}

	// 목록 톱 idx 복원
	UINT32 nNewCount = m_pList->GetElementCount();

	if (nOldCount < nNewCount)
	{
		UINT32 nNewTop = m_pList->GetCurrentTop();
		
		while(nOldTop < nNewTop)
		{
			m_pList->MovePrev();
			nNewTop = m_pList->GetCurrentTop();
		}
	}


	// 출력되는 영역에 있다면, 비밀방, 핑 표시
	for(UINT32 i = 0; i < MAX_ROOM_PER_PAGE_EX; i++)
	{
		BOOL bEnterRoom = TRUE;
		INT32 idxRoom = i + m_pList->GetCurrentTop();
		
		if (idxRoom < m_listRoom.GetCount())
		{
			ROOM_INFO_BASIC* pInfo = (ROOM_INFO_BASIC*)m_listRoom.GetItem(idxRoom);

			if (ROOM_STATE_EMPTY == pInfo->_State)
			{
				_AddListRoomEmpty(i);
			}
			else
			{			

				if( pInfo->_PersonNow >= pInfo->_PersonPlayerMax)		// 만땅
					bEnterRoom = FALSE;
				else if( (GET_ROOM_INFO_INTERENTER(pInfo->_InfoFlag) > 0) && (pInfo->_State != ROOM_STATE_READY))	//	난입 제한, 플레이중.
					bEnterRoom = FALSE;

				//	입장 할 수 여부에 따라 방 리스트의 색상을 결정
				if( bEnterRoom)
					_SetRoomListColor( i, GetColor( GCT_VALID_C));
				else
					_SetRoomListColor( i, GetColor( GCT_INVALID_C));
					
				_AddListRoomInfo(i, pInfo);
			}
		}
		else
		{
			_AddListRoomEmpty(i);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////
//	User info list
void CPhaseLobby::_BuildUserInfoListDummy(void)
{
#if defined (TEST_LOBBY_USER_LIST)

	if( g_pLobbyUserListDummy != NULL)
	{
		I3MEM_SAFE_FREE( g_pLobbyUserListDummy);
	}

	g_pLobbyUserListDummy = (LOBBY_USER_INFO_LIST*) i3MemAlloc( sizeof( LOBBY_USER_INFO_LIST) * g_nLobbyUserListDummyCount); 

	for(INT32 i=0; i<g_nLobbyUserListDummyCount; i++)
	{
		char temp[256] = "";

		LOBBY_USER_INFO_LIST * pDummy = g_pLobbyUserListDummy + i;

		pDummy->_Session = (UINT8) i+1;
		pDummy->_Rank = (UINT8) (rand() % 50);
		pDummy->_eSportID =  (UINT8) (rand() % 2);

		if( rand() % 100 < 50)
		{
			INT32 a1 = rand() % CClanMarkManager::GetMarkBackImageCount() + 1;
			INT32 b1 = rand() % CClanMarkManager::GetMarkShapeImageCount() + 1;
			INT32 a2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;
			INT32 b2 = rand() % CClanMarkManager::GetMarkColorCount() + 1;
			pDummy->_ClanMark = MAKE_CLANMARK_FLAG(a1, b1, a2, b2);
		}
		else
		{
			pDummy->_ClanMark = 0xFFFFFFFF;
		}		

		sprintf_s(temp, "USER_DUMMY_%d", i+1);

		i3String::Copy(pDummy->_Nick, temp);
	}
#endif
}

const LOBBY_USER_INFO_LIST * CPhaseLobby::_GetUserInfo(INT32 idx)
{
	const LOBBY_USER_INFO_LIST * pUser = NULL;
	
	if( idx < _GetUserInfoCount())
	{
#if defined (TEST_LOBBY_USER_LIST)		
		pUser = g_pLobbyUserListDummy + idx; 
#else
		pUser = g_pGameContext->getLobbyUserInfo(idx);						
#endif
	}

	return pUser;
}

INT32 CPhaseLobby::_GetUserInfoCount(void)
{
#if defined (TEST_LOBBY_USER_LIST)		
	return g_nLobbyUserListDummyCount;
#else
	return g_pGameContext->GetLobbyUserCount();		
#endif
}

void CPhaseLobby::_ProcessUserInfoList(REAL32 rDeltaSeconds)
{
	if( m_bRefrashUserInfoList)
	{		
		m_nUserInfoListStartIdx = m_pUserListScroll->GetCurrentTop();	//	중요 리스트가 변경되면 이것도 싱크를 맞춰야

		_ShowUserInfoList();	//	유저 목록 출력

		m_bRefrashUserInfoList = FALSE;
	}
}

void CPhaseLobby::_BuildUserInfoList(void)
{
	//	스크롤 리스트 갱신
	m_ViewUserInfoList.Clear();
	m_pUserListScroll->Clear();

	INT32 count = _GetUserInfoCount();

	for(INT32 i = 0; i< count; i++)
	{
		const LOBBY_USER_INFO_LIST * pUser1 = _GetUserInfo(i);

		I3ASSERT( pUser1 != NULL);
		
		if( pUser1->_Session < 0 )continue;
		if( pUser1->_Nick[0] == 0 )continue;

		//중복된 유저인지를 검사합니다.
		BOOL IsValidUser = TRUE;
		for(INT32 j = 0; j < i; j++)
		{
			const LOBBY_USER_INFO_LIST * pUser2 = _GetUserInfo(j);
			if( pUser1->_Session == pUser2->_Session)IsValidUser = FALSE;
		}

		//	정상적인 유저일 경우만 리스트에 등록
		if(IsValidUser)
		{
			m_ViewUserInfoList.Add( (void*)pUser1 );
			m_pUserListScroll->AddElement("");
		}
	}

	//	스크롤은 이미 화면을 넘어갔지만 리스트에는 스크롤될수 없는 적은 수의 유저만 있을 경우 강제로 스크롤을 리셋한다.
	if( m_nUserInfoListStartIdx >= m_pUserListScroll->GetElementCount())
	{
		m_pUserListScroll->SetCurValue(0.0f);
		m_nUserInfoListStartIdx = 0;
	}

	m_bRefrashUserInfoList = TRUE;
}

void CPhaseLobby::_ShowUserInfoList(void)
{
	//	목록 갱신
	for(INT32 i=0; i<MAX_LOBBY_USER_SLOT; i++) 
	{
		LOBBY_USER_INFO_LIST* pUser = NULL;

		if( m_ViewUserInfoList.GetCount() > m_nUserInfoListStartIdx + i)
		{
			pUser = (LOBBY_USER_INFO_LIST*) m_ViewUserInfoList.GetItem( m_nUserInfoListStartIdx + i);
		}

		_SetUserInfoListSlot(i, pUser);
	}
}

void CPhaseLobby::_SetUserInfoListSlot( INT32 slot, const LOBBY_USER_INFO_LIST* pUser)
{
	I3_BOUNDCHK( slot, MAX_LOBBY_USER_SLOT);

	if( pUser != NULL && pUser->_Nick[0] != 0)
	{
		m_pUserListSlot[ slot]._pRank->SetEnable(TRUE); 
		m_pUserListSlot[ slot]._pNick->SetEnable(TRUE);
		m_pUserListSlot[ slot]._pFilter->SetEnable(TRUE);

		//	클랜 마크
		if( IS_CLAN_MEMBER(pUser->_ClanMark))
		{
			m_pUserListSlot[ slot]._pClanMark->SetEnable(TRUE);

#if defined( USE_CLAN_MARK)
			SetClanMarkTexEx( m_pUserListSlot[ slot]._pClanMark, CLAN_MARK_SMALL, pUser->_ClanMark);
#endif
		}
		else
		{
			m_pUserListSlot[ slot]._pClanMark->SetEnable(FALSE);
		}

		//	계급
		if( g_pGameContext->IsBeginnerChannel1())	//	초보 서버
		{
		//	m_pUserListSlot[ slot]._pBeginner->SetEnable(TRUE);
			SetRankImage( m_pUserListSlot[ slot]._pRank, RANK_NEWBIE);
		}
		else
		{
 			m_pUserListSlot[ slot]._pBeginner->SetEnable(FALSE);
			SetRankImage( m_pUserListSlot[ slot]._pRank, pUser->_Rank);
		}

		//	유저 E-Sport 태그

		if (pUser->_eSportID == ESPORT_ID_LEVEL_ZERO)
		{
			m_pUserListSlot[ slot]._pESport->SetEnable( FALSE);
		}
		else
		{
			m_pUserListSlot[ slot]._pESport->SetEnable( TRUE);
			m_pUserListSlot[ slot]._pESport->SetShapeEnable(0, TRUE);

		}


		/*
		switch( pUser->_eSportID)
		{
		case ESPORT_ID:	// 유저의 E-Sport 태그
			{
				m_pUserListSlot[ slot]._pESport->SetEnable( TRUE);
				m_pUserListSlot[ slot]._pESport->SetShapeEnable(0, TRUE);
			}
			break;
		default:
			{
				m_pUserListSlot[ slot]._pESport->SetEnable( FALSE);
			}
			break;
		}
	*/
		//	닉네임
#ifdef USE_COLOR_NICK
		SetNameUsingColorTable( m_pUserListSlot[ slot]._pNick, (char*)pUser->_Nick, pUser->_NickColor );
#else
		m_pUserListSlot[ slot]._pNick->SetTextEllipsis( (char*) pUser->_Nick);
#endif
	}
	else
	{
		m_pUserListSlot[ slot]._pClanMark->SetEnable(FALSE);
		m_pUserListSlot[ slot]._pRank->SetEnable(FALSE);
		m_pUserListSlot[ slot]._pBeginner->SetEnable(FALSE);
		m_pUserListSlot[ slot]._pESport->SetEnable(FALSE);
		m_pUserListSlot[ slot]._pNick->SetEnable(FALSE);
		m_pUserListSlot[ slot]._pFilter->SetEnable(FALSE);
	}
}

void CPhaseLobby::_ResetUserInfoList(void)
{
	m_pUserListScroll->SetCurValue(0.0f);
	m_nUserInfoListStartIdx = 0;
	m_nUserInfoListSelectIdx = -1;
	m_bRefrashUserInfoList = FALSE;
	m_SelectUserInfo.Reset();

	for(INT32 i=0; i<MAX_LOBBY_USER_SLOT; i++)
	{
		_SetUserInfoListSlot(i, NULL);
	}

	m_ViewUserInfoList.Clear();
}

BOOL CPhaseLobby::_SetSelectedUserInfo(INT32 nUserListSlotIdx)
{
	if( nUserListSlotIdx >= 0 && nUserListSlotIdx < m_ViewUserInfoList.GetCount())
	{
		m_nUserInfoListSelectIdx = nUserListSlotIdx;

		LOBBY_USER_INFO_LIST* pUser = (LOBBY_USER_INFO_LIST*) m_ViewUserInfoList.GetItem( nUserListSlotIdx);
	
		i3mem::Copy( &m_SelectUserInfo, pUser, sizeof( LOBBY_USER_INFO_LIST));

		return TRUE;
	}
	else
	{
		m_nUserInfoListSelectIdx = -1;
		m_SelectUserInfo.Reset();

		return FALSE;
	}
}

void CPhaseLobby::OnUserListScroll(void)
{
	m_bRefrashUserInfoList = TRUE;
}

void CPhaseLobby::OnUserListSelect(INT32 nIdx)
{	
	if( _SetSelectedUserInfo( nIdx))
	{
		LOBBY_USER_INFO_LIST * pUser = &m_SelectUserInfo;

		ContextMenuReset();

		ContextMenuAddCommand(CMC_VIEW_DETAIL);		//	상세정보

		//	자신이 아닐 경우만
		if( ! g_pGameContext->IsMyNickName(pUser->_Nick))
		{
			ContextMenuAddCommand(CMC_WHISPER);			//	귓속말
			ContextMenuAddCommand(CMC_ADD_FRIEND);		//	친구추가

#if defined( USE_COMMUNITY_BLOCK)
			ContextMenuAddCommand(CMC_ADD_BLOCK);		//	차단
#endif

			if ( CanClanInvite() )
			{
				//	클랜 초대 권한이 있으며 상대가 비회원일 경우
				if( g_pFramework->GetClanContext()->IsPossibleClanInvite() && 
					pUser->_ClanMark == CLAN_MARK_FLAG_NONE )
				{	
					ContextMenuAddCommand(CMC_CLAN_INVITE);		//	클랜초대
				}
			}
		}		

		ContextMenuOpen(this, nIdx);		//	오른버튼 메뉴 연다

		PlaySound(GUI_SND_ITEM_SELECTING);
	}
}

void CPhaseLobby::OnUserListClick(INT32 nIdx)
{
	if( _SetSelectedUserInfo( nIdx))
	{
		PlaySound(GUI_SND_ITEM_SELECTING);
	}
}

void CPhaseLobby::OnUserListFocus(INT32 nIdx)
{
	m_pUserListFocus->SetEnable(FALSE);

	if( nIdx >= 0 && nIdx < m_ViewUserInfoList.GetCount())
	{		
		if(m_pUserListSlot[nIdx]._pFilter->isEnable())
		{
			REAL32 fPosY = m_pUserListSlot[nIdx]._pSlot->getPositionY() + m_pUserListSlot[nIdx]._pFilter->getPositionY();
			REAL32 fPosX = m_pUserListSlot[nIdx]._pSlot->getPositionX();

			m_pUserListFocus->setPositionY(fPosY);
			m_pUserListFocus->setPositionX(fPosX);
			m_pUserListFocus->SetEnable(TRUE);
		}
	}	
}

void CPhaseLobby::OnUserDetail(INT32 nIdx)
{
#if !defined (TEST_LOBBY_USER_LIST)

	if( _SetSelectedUserInfo(nIdx))
	{
		OnUserDetail( &m_SelectUserInfo);	//	상세정보 호출
	}
#endif
}

void CPhaseLobby::OnUserDetail(LOBBY_USER_INFO_LIST * pUser)
{	
	if( pUser == NULL) return;

#if !defined (TEST_LOBBY_USER_LIST)
	if( pUser->_Session < 0 || pUser->_Nick[0] == 0) 
		return;

	// 전적 정보 설정
	g_pCommunity->SetCommnunityInfoNick( pUser->_Nick);
	//g_pCommunity->SetCommnunityConnectState(node->_state);
	

	if (pUser->_Session == g_pGameContext->GetSessionIndex())
	{
		// 자신의 정보는 직접 설정한다.
		#ifdef USE_MODE_RECORD
		USER_INFO_BASIC3* pTargetUserInfoBasic3 = g_pCommunity->SetCommnuitUserInfoBasic3();
		USER_INFO_BASIC		MyUserInfoBasic;
		g_pGameContext->GetMyUserInfoBasic(MyUserInfoBasic);

		memcpy(&pTargetUserInfoBasic3->_strNick,	MyUserInfoBasic._nick, NET_NICK_NAME_SIZE);
		memcpy(&pTargetUserInfoBasic3->_clanName,	MyUserInfoBasic._clanName, NET_CLAN_NAME_SIZE);
		g_pGameContext->GetTitleInfo().GetEquipTitle(pTargetUserInfoBasic3->_EquipUserTitle);
		pTargetUserInfoBasic3->_ui32ClanIdx		= MyUserInfoBasic._clanidx;
		pTargetUserInfoBasic3->_ui32Rank		= MyUserInfoBasic._rank;
		pTargetUserInfoBasic3->_ui32Point		= MyUserInfoBasic._point;
		pTargetUserInfoBasic3->_ui32cash		= MyUserInfoBasic._cash;

		USER_INFO_ALL_RECORD* pTarget = g_pCommunity->SetCommnuitAllRecord();
		const USER_INFO_ALL_RECORD* pSource = g_pGameContext->GetMyUserInfoAllRecord();

		i3mem::Copy(pTarget, pSource, sizeof(USER_INFO_ALL_RECORD));		
		g_pCommunity->SetCommnunityInfoNick(pUser->_Nick);
		#else
		USER_INFO_RECORD* pTarget = g_pCommunity->SetCommnuitRecord();
		const USER_INFO_RECORD* pSource = g_pGameContext->GetMyUserInfoRecord();

		i3mem::Copy(pTarget, pSource, sizeof(USER_INFO_RECORD));		
		g_pCommunity->SetCommnunityInfoNick(pUser->_Nick);
		#endif
	}
	else
	{
		// 다른 유저의 정보는 요청한다.		
		g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_SESSION_N, &pUser->_Session);
		
	}
	//	미리 상세정보 팝업을 띄운다.
	#ifdef USE_MODE_RECORD
	if( ((CStageReady*)m_pParent)->IsEnablePopup(POPUP_USER_DETAIL_INFO) == FALSE)
		((CStageReady*)m_pParent)->TogglePopup( POPUP_USER_DETAIL_INFO, (void*) pUser);
	#else
	if( ((CStageReady*)m_pParent)->IsEnablePopup(POPUP_USER_INFO) == FALSE)
		((CStageReady*)m_pParent)->TogglePopup( POPUP_USER_INFO, (void*) pUser);
	#endif
#endif
}

void CPhaseLobby::_AddListRoomEmpty(UINT32 idxRoom)
{
	m_roomSlot[idxRoom]._pSlotIdx->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_roomSlot[idxRoom]._pSlotMode->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_roomSlot[idxRoom]._pSlotMap->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_roomSlot[idxRoom]._pSlotTitle->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_roomSlot[idxRoom]._pSlotState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_roomSlot[idxRoom]._pSlotPerson->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	_SetRoomListColor( idxRoom, GetColor( GCT_VALID_C));

	m_roomSlot[idxRoom]._pSlotIdx->SetText("");
	m_roomSlot[idxRoom]._pSlotMode->SetText("");
	m_roomSlot[idxRoom]._pSlotMap->SetText("");
	m_roomSlot[idxRoom]._pSlotTitle->SetText("");
	m_roomSlot[idxRoom]._pSlotState->SetText("");
	m_roomSlot[idxRoom]._pSlotPerson->SetText("");

	m_roomSlot[idxRoom]._pSlotPing->SetEnable(FALSE);
	m_roomSlot[idxRoom]._pSlotNonRealIP->SetEnable(FALSE);	
	m_roomSlot[idxRoom]._pSlotBattleMarkFront->SetEnable(FALSE);
	m_roomSlot[idxRoom]._pSlotBattleMarkRear->SetEnable(FALSE);
	m_roomSlot[idxRoom]._pSlotKey->SetEnable(FALSE);
}

void CPhaseLobby::_SetRoomListColor(INT32 idx, I3COLOR * color)
{
	m_roomSlot[idx]._pSlotIdx->SetTextColor(color);
	m_roomSlot[idx]._pSlotMode->SetTextColor(color);
	m_roomSlot[idx]._pSlotMap->SetTextColor(color);
	m_roomSlot[idx]._pSlotTitle->SetTextColor(color);
	m_roomSlot[idx]._pSlotState->SetTextColor(color);
	m_roomSlot[idx]._pSlotPerson->SetTextColor(color);
}

void CPhaseLobby::_AddListRoomInfo(UINT32 idxRoom,ROOM_INFO_BASIC* pInfo)
{
	// 방 번호
	char strNumber[MAX_STRING_COUNT];

	sprintf_s(strNumber, "%03d", pInfo->_Idx + 1);
	m_roomSlot[idxRoom]._pSlotIdx->SetText(strNumber);


	// 게임모드
	char strMode[MAX_STRING_COUNT];

	switch( pInfo->_StageMask )
	{
	case STAGE_MASK_TYPE_TRAINING:
		i3String::Copy( strMode, GAME_STRING("STR_TBL_GUI_LOBBY_TRANING_TEXT"), MAX_STRING_COUNT );//훈련전
		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(GetColor(GCT_TRAINING_MODE));
		break;
	case STAGE_MASK_TYPE_SNIPER:
		i3String::Copy( strMode, GAME_STRING("STR_TBL_GUI_LOBBY_SNIPERMODE_TITLE"), MAX_STRING_COUNT );//저격전
		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(GetColor(GCT_SNIPER_MODE));		
		break;
	case STAGE_MASK_TYPE_SHOTGUN:
		i3String::Copy( strMode, GAME_STRING("STR_TBL_GUI_LOBBY_SHOTGUNMODE_TITLE"), MAX_STRING_COUNT );//샷건전
		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(GetColor(GCT_SHOTGUN_MODE));
		break;
	case STAGE_MASK_TYPE_KNUCKLE:
		i3String::Copy( strMode, GAME_STRING("STR_TBL_GUI_LOBBY_KNUCKLEMODE_TITLE"), MAX_STRING_COUNT );//너클전
		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(GetColor(GCT_KNUCKLE_MODE));
		break;
	case STAGE_MASK_TYPE_CLANMATCH:
		// 클랜 매치 메이킹전일 경우에는 맵에 상관없이 매치메이킹 글자를 띄어준다.
		i3String::Copy( strMode, GAME_STRING("STR_TBL_GUI_LOBBY_CLAN_MATCH_MAKING_TEXT"), MAX_STRING_COUNT );//클랜 매치메이킹
		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(GetColor(GCT_MATCHMAKING));
		break;
	case STAGE_MASK_TYPE_HEADHUNTER:
		i3String::Copy( strMode, GAME_STRING("STR_TBL_GUI_LOBBY_HEADHUNTERMODE_TITLE"), MAX_STRING_COUNT );//너클전
		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(GetColor(GCT_HEAD_HUNTER_MODE));
		break;
	default:
		i3String::Copy( strMode, getStageTypeName( (STAGE_ID) pInfo->_StageID).c_str(), MAX_STRING_COUNT );
		I3COLOR Color;
		i3Color::Set(&Color, (I3COLOR*)GameGUI::GameModeColor( getStageType( pInfo->_StageID) ));

		m_roomSlot[idxRoom]._pSlotMode->SetTextColor(&Color);
		break;
	}

	m_pImageDummy4->SetEnable(FALSE);
	if( pInfo->_StageMask == STAGE_MASK_TYPE_HEADHUNTER )
	{
		m_pImageDummy4->SetEnable( TRUE );
	}

	m_roomSlot[idxRoom]._pSlotMode->SetText(strMode);

	// 맵정보
	{
 		char mapNameText[MAX_STRING_COUNT] = {0, };
#if defined (USE_RANDOMMAP_MODE)
		
		if (GET_ROOM_INFO_RANDOM_MODE( pInfo->_InfoFlag ))
		{
			i3String::Copy( mapNameText, GAME_STRING("STR_POPUP_RANDOMMAP"), MAX_STRING_COUNT );//
		}
		else
		{
			if ( TESTBIT(getNewMarkStageTable( getStageOrd(pInfo->_StageID)) , GOODS_MARK_NEW) )
			{
				i3String::Concat(mapNameText, GAME_STRING("STR_TBL_GUI_LOBBY_NEWMAP_PREFIX"));
			}
			char szTemp[256];

			getStageDisplayName(szTemp, (STAGE_ID) pInfo->_StageID);

#if defined( LOCALE_NORTHAMERICA)
		if( strcmp(szTemp, "Dino Breakdown") == 0 )
			i3String::Copy(szTemp, "Dino \nBreakdown", 256);
#endif

			i3String::Concat(mapNameText, szTemp);
		}


		m_roomSlot[idxRoom]._pSlotMap->SetText(mapNameText);
#else	
		// 신규맵 예외 처리 - 접두어 NEW 붙임
		if ( TESTBIT(getNewMarkStageTable( getStageOrd(pInfo->_StageID)), GOODS_MARK_NEW) )
		{
			i3String::Concat(mapNameText, GAME_STRING("STR_TBL_GUI_LOBBY_NEWMAP_PREFIX"));
		}
		
		char szTemp[256];

		getStageDisplayName(szTemp, (STAGE_ID) pInfo->_StageID);

#if defined( LOCALE_NORTHAMERICA)
		if( strcmp(szTemp, "Dino Breakdown") == 0 )
			i3String::Copy(szTemp, "Dino \nBreakdown");
#endif
		i3String::Concat(mapNameText, szTemp);


		m_roomSlot[idxRoom]._pSlotMap->SetText(mapNameText);
#endif
	}

	// 방제목
	m_roomSlot[idxRoom]._pSlotTitle->SetTextEllipsis(pInfo->_Title);	


	// 상태
	char strState[MAX_STRING_COUNT] = {0,};
	
	if (ROOM_STATE_READY == pInfo->_State)
	{
		i3String::Copy(strState, GAME_STRING("STBL_IDX_ROOM_STATE_WAIT"), MAX_STRING_COUNT );
	}
	else
	{
		i3String::Copy(strState, GAME_STRING("STBL_IDX_ROOM_STATE_PLAY"), MAX_STRING_COUNT );
	}

	m_roomSlot[idxRoom]._pSlotState->SetText(strState);


	// 사용자
	{
		char strPerson[MAX_STRING_COUNT];

		sprintf_s(strPerson, "%02d/%02d", pInfo->_PersonNow, pInfo->_PersonPlayerMax);
		m_roomSlot[idxRoom]._pSlotPerson->SetText(strPerson);
	}

	// 비밀방
	BOOL bSecretRoom = (BOOL)GET_ROOM_INFO_PRIVATE_MODE( pInfo->_InfoFlag );

	m_roomSlot[idxRoom]._pSlotKey->SetEnable(bSecretRoom);


	//	칼전/권총전 여부
	if(GET_ROOM_INFO_WEAPON_SNIPER(pInfo->_WeaponFlag) == 1) 
	{
		if( bSecretRoom)
		{
			SET_ROOM_BATTLE_MARK_SNIPER( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
		}
		else
		{
			SET_ROOM_BATTLE_MARK_SNIPER( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
		}
	}
	else if(GET_ROOM_INFO_WEAPON_SHOTGUN(pInfo->_WeaponFlag) == 1) 
	{
		if( bSecretRoom)
		{
			SET_ROOM_BATTLE_MARK_SHOTGUN( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
		}
		else
		{
			SET_ROOM_BATTLE_MARK_SHOTGUN( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
		}
	}
	else if( GET_ROOM_INFO_WEAPON_00(pInfo->_WeaponFlag) == 0)		//	주무기 lock
	{
		if( GET_ROOM_INFO_WEAPON_01(pInfo->_WeaponFlag) == 0)	//	보조무기 lock
		{			
			if(GET_ROOM_INFO_WEAPON_RPG7(pInfo->_WeaponFlag) == 1)		//	-------------	RPG7전
			{
				if( bSecretRoom)
				{
					SET_ROOM_BATTLE_MARK_RPG7( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
				}
				else
				{
					SET_ROOM_BATTLE_MARK_RPG7( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
				}
			}
			else if(GET_ROOM_INFO_WEAPON_02(pInfo->_WeaponFlag) == 1)		//	-------------	칼전
			{
				if( bSecretRoom)
				{
					SET_ROOM_BATTLE_MARK_KNIFE( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
				}
				else
				{
					SET_ROOM_BATTLE_MARK_KNIFE( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
				}
			}
			else if(GET_ROOM_INFO_WEAPON_KNUCKLE(pInfo->_WeaponFlag) == 1)		//	-------------	너클전
			{
				if( bSecretRoom)
				{
					SET_ROOM_BATTLE_MARK_KNUCKLE( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
				}
				else
				{
					SET_ROOM_BATTLE_MARK_KNUCKLE( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
				}
			}
		}
		else if(GET_ROOM_INFO_WEAPON_RPG7(pInfo->_WeaponFlag) == 1)		//	-------------	RPG7전
		{
			if( bSecretRoom)
			{
				SET_ROOM_BATTLE_MARK_RPG7( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
			}
			else
			{
				SET_ROOM_BATTLE_MARK_RPG7( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
			}
		}
		else															//	-------------	권총전
		{
			if( bSecretRoom)
			{
				SET_ROOM_BATTLE_MARK_HANDGUN( m_roomSlot[idxRoom]._pSlotBattleMarkFront);
			}
			else
			{
				SET_ROOM_BATTLE_MARK_HANDGUN( m_roomSlot[idxRoom]._pSlotBattleMarkRear);
			}
		}
	}	

	// 핑
	_ShowPingLevel(idxRoom, pInfo->_Ping);

	//	리얼 IP가 아니면 마크 표시
	_SetRealIP( idxRoom, GET_ROOM_INFO_REAL_IP(pInfo->_InfoFlag));

}

void CPhaseLobby::_ProcessCreateRoom( REAL32 rDeltaSeconds)
{
	// 확장된 방생성 팝업 업데이트
	{
		_ProcessMapInfo( rDeltaSeconds);
	}

	// 비밀번호가 입력되어있다면, 체크박스에 표시를 합니다. 반대의 경우 표시를 해제합니다.
	if( i3String::Length( m_pCreatePasswordValue->getIMETextBuf()) > 0)
	{
		m_pCreatePasswordCheck->setCheckBoxState( TRUE);
	}
	else
	{
		m_pCreatePasswordCheck->setCheckBoxState( FALSE);
	}

	// 확인버튼
	// IME 에디트 입력받는 부분은 받드시 상태를 체크해서 처리해야 됩니다. komet 09.06
	// IME를 처리하는 Windows API Proc은 메시지 큐잉 방식으로 처리하기 때문입니다.
	if (GUI_IME_CONFIRM(m_pCreatePasswordValue))
	{
		OnCreateRoomConfirmButtonClick();			
		m_pCreatePasswordValue->InputDone();
	}
	else if (GUI_IME_CONFIRM(m_pCreateTitleValue))
	{		
		OnCreateRoomConfirmButtonClick();
		m_pCreateTitleValue->InputDone();
	}
}

void CPhaseLobby::_ProcessRoomInfo( REAL32 rDeltaSeconds)
{
	// 정보 요청 중이면 대기한다
	if (m_bRequestRoomInfoAdd)
	{
		return;
	}

	if( m_SelectedElement == -1)
	{		
		_ClearRoomInfo();
		return;
	}

	m_pInfoWin->SetEnable(TRUE);


	char szTemp[MAX_STRING_COUNT] = {0,};	

	ROOM_INFO_BASIC* pInfo = NULL;
	INT32 idxRoom = m_SelectedElement;
	
	if(idxRoom < m_listRoom.GetCount())
	{
		pInfo = (ROOM_INFO_BASIC*)m_listRoom.GetItem(idxRoom);

		if (ROOM_STATE_EMPTY == pInfo->_State)
		{
			pInfo = NULL;
		}
	}
	
	if (NULL == pInfo)
	{
		_ClearRoomInfo();
		return;
	}
	
	STAGE_TYPE			stageType		= getStageType( pInfo->_StageID);


	UINT8 GameSetPrim	= g_pGameContext->getSelectedRoomInfoAdd()->_SubType & 0x0F;
	UINT8 GameSetSec	= g_pGameContext->getSelectedRoomInfoAdd()->_SubType & 0xF0;

	// Map Image
	{	
		char stageName[MAX_STRING_COUNT];

#if defined (USE_RANDOMMAP_MODE)
		BOOL Random;
		Random = GET_ROOM_INFO_RANDOM_MODE( pInfo->_InfoFlag);
		if( Random)
			g_pFramework->SetRandomMapShape(m_pImageDummy);
		else
			g_pFramework->SetMapShape(m_pImageDummy, (STAGE_ID) pInfo->_StageID);
#else	
		g_pFramework->SetMapShape(m_pImageDummy, (STAGE_ID) pInfo->_StageID);
#endif

		//맵 이름 및 New 마크에 대한 표시를 설정 한다.
		m_pImageDummy2->SetAllShapeEnable(FALSE);
		m_pImageDummy3->SetEnable(FALSE);
		m_pImageDummy4->SetEnable(FALSE);
		m_pImageXMas->SetEnable(FALSE);

		INT32 typeImageIdx = -1;
		
		switch(stageType)
		{
		case STAGE_TYPE_DEATHMATCH:		typeImageIdx = 2;	break;		//단체전
		case STAGE_TYPE_BOMB:			typeImageIdx = 0;	break;		//폭파 미션
		case STAGE_TYPE_DESTROY:		typeImageIdx = 3;	break;		//파괴 미션
		case STAGE_TYPE_DEFENCE:		typeImageIdx = 6;	break;		//방어미션
		case STAGE_TYPE_ANNIHILATION:	typeImageIdx = 1;	break;		//섬멸전
		case STAGE_TYPE_ESCAPE:	typeImageIdx = 5;	break;		//탈출미션
		case STAGE_TYPE_SUDDEN_DEATH:		typeImageIdx = 7;	break;		//서든데스
		}

		m_pImageDummy4->SetEnable(FALSE);
		if( pInfo->_StageMask == STAGE_MASK_TYPE_HEADHUNTER )
			m_pImageDummy4->SetEnable(TRUE);

		//챌린지(AI전)은 기존 모드(단체전 등등)로 판단후 _StageMask로 2차 판단한다. 
		if ( pInfo->_StageMask == STAGE_MASK_TYPE_TRAINING )
		{
			typeImageIdx = 4;
		}

		//맵 모드 이미지 설정
		if( typeImageIdx != -1 )
		{
			m_pImageDummy2->SetShapeEnable(typeImageIdx, TRUE);
		}

		//New 마크 표시
		if ( TESTBIT(getNewMarkStageTable( getStageOrd(pInfo->_StageID) ) , GOODS_MARK_NEW))
		{
			m_pImageDummy3->SetEnable(TRUE);
		}
		
		// X-Mas Mark
		if(TESTBIT(getNewMarkStageTable( getStageOrd(pInfo->_StageID) ) , GOODS_MARK_XMAS))
		{
			m_pImageXMas->SetEnable(TRUE);
		}

		char szTemp[256];

		getStageDisplayName(szTemp, (STAGE_ID) pInfo->_StageID);
		i3String::Copy(stageName, szTemp, MAX_STRING_COUNT );

#if defined (USE_RANDOMMAP_MODE)
		if(Random)
			m_pMapValue->SetText(GAME_STRING("STR_POPUP_RANDOMMAP"));
		else
			m_pMapValue->SetText(stageName);
#else
		m_pMapValue->SetText(stageName);
#endif
	}

	// 게임모드
	{
		char modeName[MAX_STRING_COUNT];

		switch( pInfo->_StageMask )
		{
		case STAGE_MASK_TYPE_TRAINING:
			i3String::Copy( modeName, GAME_STRING("STR_TBL_GUI_LOBBY_TRANING_TEXT"), MAX_STRING_COUNT );
			break;
		case STAGE_MASK_TYPE_SNIPER:
			i3String::Copy( modeName, GAME_STRING("STR_TBL_GUI_LOBBY_SNIPERMODE_TITLE"), MAX_STRING_COUNT );
			break;
		case STAGE_MASK_TYPE_KNUCKLE:
			i3String::Copy( modeName, GAME_STRING("STR_TBL_GUI_LOBBY_KNUCKLEMODE_TITLE"), MAX_STRING_COUNT );
			break;
		default:
			i3String::Copy( modeName, getStageTypeName( (STAGE_ID) pInfo->_StageID).c_str(), MAX_STRING_COUNT );
			break;
		}

		modeName[i3String::Length( modeName)] = 0;
		m_pModeValue->SetText(modeName);
	}

	//게임 설정
	switch( stageType)
	{
	case STAGE_TYPE_ESCAPE: 
		// STBL_IDX_MODE_TYPE1 : 공수교대
		i3String::Copy( szTemp, TextSet::Mode(STAGE_TYPE_ESCAPE), MAX_STRING_COUNT );
		break;
	case STAGE_TYPE_ANNIHILATION		: 
	case STAGE_TYPE_BOMB		: 
	case STAGE_TYPE_DESTROY		:
	case STAGE_TYPE_DEFENCE		:
	case STAGE_TYPE_SUDDEN_DEATH :
	case STAGE_TYPE_HEAD_KILLER :
		{
			i3String::Copy( szTemp, TextSet::Round(GameSetPrim), MAX_STRING_COUNT );
		}
		break; 		
	case STAGE_TYPE_DEATHMATCH	: 
	case STAGE_TYPE_HEAD_HUNTER	: 
	
	default						: 
		{
			i3String::Copy(szTemp, TextSet::KillCount(GameSetPrim), MAX_STRING_COUNT );
		}
		break;
	}

	//STBL_IDX_MODE_TYPE1 : 공수교대
	if( strcmp( szTemp, "" ) != 0 )
		i3String::Concat( szTemp, " /");
	// Time Count
	i3String::Concat( szTemp, TextSet::GameTime( GameSetSec) );

	m_pTypeValue->SetText( szTemp);	

	// 방장	
#ifdef USE_COLOR_NICK2
	SetNameUsingColorTable( m_pHostValue, g_pGameContext->getSelectedRoomInfoAdd()->getNick(), g_pGameContext->getSelectedRoomInfoAdd()->_NicklColor );
#else
	m_pHostValue->SetTextEllipsis( g_pGameContext->getSelectedRoomInfoAdd()->getNick());
#endif

	// 상태
	if (ROOM_STATE_READY == pInfo->_State)
	{
		m_pStateValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_WAIT_STATE_TEXT"));/*대기중*/
	}	
	else
	{
		// 게임모드
		switch( stageType)
		{
		case STAGE_TYPE_DEATHMATCH:// 단체전		
		case STAGE_TYPE_HEAD_HUNTER:
			sprintf_s(szTemp, "%02d:%02d", (INT32)(g_pGameContext->getSelectedRoomInfoAdd()->_NowIngTime / 60), (INT32)( g_pGameContext->getSelectedRoomInfoAdd()->_NowIngTime % 60)); 
			break;
		case STAGE_TYPE_ESCAPE			:// 보스미션전
		case STAGE_TYPE_ANNIHILATION	:// 섬멸전
		case STAGE_TYPE_SUDDEN_DEATH:// 서든 데쓰	
		case STAGE_TYPE_HEAD_KILLER:
		case STAGE_TYPE_DESTROY	: 
		case STAGE_TYPE_DEFENCE	: 
		case STAGE_TYPE_BOMB	: 
		default:			//미션 
			sprintf_s(szTemp, GAME_STRING("STR_TBL_GUI_LOBBY_CURRENT_ROUND_COUNT"), g_pGameContext->getSelectedRoomInfoAdd()->_NowRoundCount);	
			break;
		}

		m_pStateValue->SetText( szTemp );
	}

    
	// 난입 제한
	if (0 == g_pGameContext->getSelectedRoomInfoAdd()->_LockInterEnter)
	{
		m_pJoinLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*없음*/
	}
	else
	{
		m_pJoinLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"));/*제한*/
	}

	// 무기 제한
	if (pInfo->_WeaponFlag == LOCK_BASIC)
	{
		m_pWeaponLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*없음*/
	}
	else
	{
		m_pWeaponLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_TEXT"));/*제한*/
	}


	m_pObserverLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*없음*/
	m_pTeamBalanceValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*없음*/
	
	// 관전제한
	if( TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_CHASE) &&
		TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_FREE) &&
		TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY) &&
		TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		m_pObserverLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER1"));// 모두사용
	}
	else if(TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_CHASE) ||
			TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_FREE) ||
			TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY) ||
			TESTBIT(g_pGameContext->getSelectedRoomInfoAdd()->_LockObserver, LOCK_OBSERVER_ENEMY_HP) )
	{
		m_pObserverLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER2"));//사용
	}
	else
	{
		m_pObserverLimitValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_ROOMINFO_OBSERVER3"));//사용하지 않음
	}

    // 팀밸런스
	if (g_pGameContext->getSelectedRoomInfoAdd()->_LockTeamBalance == 0x01)
	{
		m_pTeamBalanceValue->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_PERSON"));/*인원*/
		
	}
	else if (g_pGameContext->getSelectedRoomInfoAdd()->_LockTeamBalance == 0x02)
	{
		m_pTeamBalanceValue->SetText(GAME_STRING("STR_TBL_GUI_READY_TEAM_BALANCE_SKILLED"));/*실력*/
	}
	else
	{
		m_pTeamBalanceValue->SetText(GAME_STRING("STR_TBL_GUI_LOBBY_NONE_TEXT"));/*없음*/
	}
}

void CPhaseLobby::_ShowPingLevel(INT32 idxSlot, INT32 nLevel)
{
	nLevel = MINMAX(1, nLevel, 5);
	
	m_roomSlot[idxSlot]._pSlotPing->SetAllShapeEnable(FALSE);
	m_roomSlot[idxSlot]._pSlotPing->SetShapeEnable(GUI_SHAPE_OFFSET + nLevel - 1, TRUE);
	m_roomSlot[idxSlot]._pSlotPing->SetEnable(TRUE);
}

void CPhaseLobby::_SetRealIP(INT32 idxSlot, BOOL bReal)
{
#if defined (USE_LOBBY_NON_REAL_IP_MARK)
	//	리얼 IP면 느낌표 마크가 표시 안된다.
	m_roomSlot[idxSlot]._pSlotNonRealIP->SetEnable( !bReal);
#else
	m_roomSlot[idxSlot]._pSlotNonRealIP->SetEnable( FALSE);
#endif
}

void CPhaseLobby::_Join(void)
{
	if (m_SelectedElement < m_listRoom.GetCount())
	{
		ROOM_INFO_BASIC* pInfo = (ROOM_INFO_BASIC*)m_listRoom.GetItem(m_SelectedElement);

		// 빈 슬롯 없음
		if (pInfo->_PersonNow >= pInfo->_PersonPlayerMax)
		{
			ERROR_POPUP(GAME_STRING("STBL_IDX_EP_LOBBY_SLOT_FULL"));
			return;
		}

		// 공개여부
		if (GET_ROOM_INFO_PRIVATE_MODE( pInfo->_InfoFlag ))
		{
			QueryPasswordParam	Param;
			Param.pMsg			= GAME_STRING("STBL_IDX_EP_LOBBY_JOIN_ROOM_Q");
			Param.LimitCount	= NET_ROOM_PW;
			Param.OnlyNumber	= TRUE;
			Param.ShowPassword	= TRUE;
			Param.Callback		= JoinRoomPasswordCallback;
			Param.pUserContext	= this;

			m_JoinTargetRoom	= *pInfo;

			((CStageReady*)m_pParent)->TogglePopup(POPUP_QUERY_PASSWORD, &Param);
		}
		else
		{
			// 입장
			m_bAllowRefresh = FALSE;
			i3GuiRoot::setGuiDisableInput(TRUE);
			g_pGameContext->JoinRoom(pInfo);
		}
	}
}


void CPhaseLobby::_BuildRoomListDummy(void)
{
#if defined (TEST_LOBBY_ROOM_LIST)
	static	BOOL	BuildedDummyRoomList = FALSE;
	static INT32 g_nRoomInfoDummyCount = 100;

	if( BuildedDummyRoomList == TRUE)
		return;
	BuildedDummyRoomList = TRUE;


	for(INT32 i=0; i<g_nRoomInfoDummyCount; i++)
	{
		ROOM_INFO_BASIC * pDummy = (ROOM_INFO_BASIC*) i3MemAlloc( sizeof( ROOM_INFO_BASIC));

		i3mem::FillZero( pDummy, sizeof( ROOM_INFO_BASIC));

		pDummy->_Idx = i;
		pDummy->_State = (UINT8) (i3Math::Randf() * (REAL32)(ROOM_STATE_BATTLE_END + 1));

		// EXPORT_AVALIABLE_CODE_VALUE
		STAGE_ORDINAL_ID ord = (i3Math::Rand() % (STAGE_ORD_MAX - 1));
		ord = (ord == 0 ? ord+1 : ord);

		pDummy->_StageID = getStageId( ord);

		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_StageMask |= STAGE_FLAG_TRAINING;
		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_StageMask |= STAGE_FLAG_SNIPER;
		if( (INT32) (i3Math::Randf() * 2.0))		pDummy->_StageMask |= STAGE_FLAG_SHOTGUN;
				
		pDummy->_PersonPlayerMax = (UINT8) (i3Math::Randf() * 16.0f);
		pDummy->_PersonPlayerMax = MINMAX( 2, pDummy->_PersonPlayerMax, 16);
		pDummy->_PersonNow = (UINT8) (i3Math::Randf() * (REAL32) pDummy->_PersonPlayerMax);
		pDummy->_PersonNow = MINMAX( 1, pDummy->_PersonNow, pDummy->_PersonPlayerMax);
		pDummy->_Ping = (UINT8) (i3Math::Randf() * 5.f) + 1;
		pDummy->_Ping = MINMAX( 1, pDummy->_Ping, 5);
		pDummy->_WeaponFlag = 
							((UINT8) (i3Math::Randf() * 2.0)) << 5 |
							((UINT8) (i3Math::Randf() * 2.0)) << 4 |
							((UINT8) (0x00000001)) << 3 |
							((UINT8) (0x00000001)) << 2 |
							((UINT8) (i3Math::Randf() * 2.0)) << 1 |
							((UINT8) (i3Math::Randf() * 2.0));

		sprintf_s( pDummy->_Title, "DUMMY_STAGE_%d", i);
		m_listRoom.Add(pDummy);
	}

#endif
}

void CPhaseLobby::_UpdateRoomList(void)
{
#if defined( TEST_LOBBY_ROOM_LIST)

	_BuildRoomListDummy();

#else
	// Empty All
	_EmptyRoomList();

	// Find View
	INT32 nRoomCount = g_pGameContext->getLobbyRoomCount();	

	// 현재 방의 갯수가 슬롯의 갯수에 못미칠경우 스크롤바를 초기화 합니다.
	if( nRoomCount < MAX_ROOM_PER_PAGE_EX) m_pList->SetCurValue( 0.0f);

	for(INT32 i = 0; i < nRoomCount; i++)
	{
		const ROOM_INFO_BASIC * pRoomInfo = g_pGameContext->getRoomInfo(i);
		if ( ! pRoomInfo)
		{
			I3ASSERT(0 && "Invalid room index.");
			continue;
		}

		// PersonNow가 0이면 깨진 방
		if (0 == pRoomInfo->_PersonNow)
		{
			continue;
		}

		BOOL bBeRoom = FALSE;
		ROOM_INFO_BASIC* pRoom = NULL;

		for(INT32 j = 0; j < m_listRoom.GetCount(); j++)
		{
			pRoom = (ROOM_INFO_BASIC*)m_listRoom.GetItem(j);
			
			if (pRoomInfo->_Idx == pRoom->_Idx)
			{
				if (ROOM_STATE_EMPTY != pRoom->_State)
				{
					bBeRoom = TRUE;
					break;
				}
			}
		}

		if (bBeRoom)
		{
			// Modify
			i3mem::Copy(pRoom, pRoomInfo, sizeof(ROOM_INFO_BASIC));
		}
		else
		{
			// AddTail
			ROOM_INFO_BASIC* pNewRoom = (ROOM_INFO_BASIC*)i3MemAlloc(sizeof(ROOM_INFO_BASIC));
			i3mem::Copy(pNewRoom, pRoomInfo, sizeof(ROOM_INFO_BASIC));
			m_listRoom.Add(pNewRoom);
		}
	}
#endif
}

void CPhaseLobby::_SortRoomList(void)
{
#if defined( TEST_LOBBY_ROOM_LIST)

	_BuildRoomListDummy();

#else
	// Clear Room List
	m_listRoom.SafeFreeAll();

    // Copy Room List
	INT32 nRoomCount = g_pGameContext->getLobbyRoomCount();
	
	for(INT32 i = 0; i < nRoomCount; i++)
	{
		const ROOM_INFO_BASIC * pRoomInfo = g_pGameContext->getRoomInfo(i);
		if ( ! pRoomInfo )
		{
			I3ASSERT(0 && "Invalid room index.");
			continue;
		}
		// PersonNow가 0이면 깨진 방
		if (0 == pRoomInfo->_PersonNow)
		{
			continue;
		}

		ROOM_INFO_BASIC* pNewRoom = (ROOM_INFO_BASIC*)i3MemAlloc(sizeof(ROOM_INFO_BASIC));

		i3mem::Copy(pNewRoom, pRoomInfo, sizeof(ROOM_INFO_BASIC));
		
		m_listRoom.Add(pNewRoom);
	}
#endif

	// Sort Room List
	m_listRoom.Sort(CompareRoom);
}

void CPhaseLobby::_EmptyRoomList(void)
{
	INT32 nPreRemoveRoomCount = 0;

	INT32 nRoomCount = g_pGameContext->getLobbyRoomCount();

	for(INT32 i = 0; i < m_listRoom.GetCount(); i++)
	{
		BOOL bBeRoom = FALSE;
		ROOM_INFO_BASIC* pRoom = (ROOM_INFO_BASIC*)m_listRoom.GetItem(i);

		if (ROOM_STATE_EMPTY != pRoom->_State)
		{
			for(INT32 j = 0; j < nRoomCount; j++)
			{
				const ROOM_INFO_BASIC* pRoomInfo = g_pGameContext->getRoomInfo(j);
				if ( ! pRoomInfo )
					continue;

				if (pRoom->_Idx == pRoomInfo->_Idx)
				{
					// PersonNow가 0이면 깨진 방
					if (0 != pRoomInfo->_PersonNow)
					{
						bBeRoom = TRUE;
					}
					break;
				}
			}
		}
		
		if (FALSE == bBeRoom)
		{
			// 화면에 보여지는 슬롯이하, 선택된 슬롯 이하는 없어져도 비우기만 한다. - 목록 땡겨지는 것 방지
			//if ((UINT32)i < m_pList->GetCurrentTop() + MAX_ROOM_PER_PAGE_EX || (-1 != m_SelectedElement && i < m_SelectedElement))

			// 보여지는 슬롯 이후에 존재하는 선택슬롯이 제거되면, 선택 슬롯 초기화 -> 뒷부분은 빈 방 제거되도록
			if (i < m_SelectedElement)
			{
				nPreRemoveRoomCount++;				
			}
			else if (i == m_SelectedElement)
			{
				m_SelectedElement = -1;
			}

			i3MemFree(m_listRoom.GetItem(i));
			m_listRoom.Delete(i);
			i--;
		}
	}


	// 선택 슬롯 이하부분 땡겨진 만큼 선택 슬롯 이동
	if (-1 != m_SelectedElement)
	{
		m_SelectedElement = MAX(-1, m_SelectedElement - nPreRemoveRoomCount);
	}
}

void CPhaseLobby::_ChangeSortOrder(INT32 SortType)
{
	// Find Order for SortType
	INT32 idxFind = -1;
	INT32 i = 0;
	for( i = 0; i < MAX_SORT_TYPE; i++)
	{
		if(SortType == sm_SortOrder[i]._nType)
		{
			idxFind = i;
			break;
		}
	}

	// Shift Other SortType
	if (-1 == idxFind)
	{
		i3mem::Copy(sm_SortOrder + 1, sm_SortOrder, sizeof(SORT_ORDER) * (MAX_SORT_TYPE - 1));
	}
	else if (0 < idxFind)
	{
		i3mem::Copy(sm_SortOrder + 1, sm_SortOrder, sizeof(SORT_ORDER) * i);
	}
	
	// Set & Toggle Asc/Desc
	sm_SortOrder[0]._nType = SortType;

	if (0 == idxFind)
	{
		sm_SortOrder[0]._bAscending = !sm_SortOrder[0]._bAscending;
	}
	else
	{
		sm_SortOrder[0]._bAscending = TRUE;
	}
}

void CPhaseLobby::EhWrongQuickJoin(void)
{
	OnCreateRoom();
}

void CPhaseLobby::EhSlotFull(void)
{
	// TTP # 292 - 슬롯없음이 암호틀림보다 우선되야한다고 합니다.

	ERROR_POPUP(GAME_STRING("STBL_IDX_EP_LOBBY_SLOT_FULL"));

	m_bCheckInvite = TRUE;
}

void CPhaseLobby::EhKickedByVote()
{
	ERROR_POPUP(GAME_STRING("STBL_IDX_EP_GAME_FAIL_START_C"));

	m_bCheckInvite = TRUE;
}

void CPhaseLobby::EhWrongPW(void)
{
	QueryPasswordParam	Param;
	Param.pMsg			= GAME_STRING("STBL_IDX_EP_LOBBY_WRONG_PW_Q");
	Param.LimitCount	= NET_ROOM_PW;
	Param.OnlyNumber	= TRUE;
	Param.ShowPassword	= TRUE;
	Param.Callback		= JoinRoomPasswordCallback;
	Param.pUserContext	= this;

	((CStageReady*)m_pParent)->TogglePopup(POPUP_QUERY_PASSWORD, &Param);
}

void CPhaseLobby::EhNotExist(void)
{
	ERROR_POPUP(GAME_STRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));

	m_SelectedElement = -1;

	m_bCheckInvite = TRUE;
}

void CPhaseLobby::EhLimitInterenter(void)
{
	STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_ENTER_ROOM"));/*난입이 제한된 방입니다.*/
	m_bCheckInvite = TRUE;
}

void CPhaseLobby::EhLimitLever(void)
{
	STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_LIMIT_GRADE"));/*계급이 제한된 방입니다.*/
	m_bCheckInvite = TRUE;
}

void CPhaseLobby::EhLimitTraining(void)
{
	STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_WARING_TRANING_ENTER"));/*훈련전은 방장만 입장할 수 있습니다.*/
	m_bCheckInvite = TRUE;
}

void CPhaseLobby::EhStageError(INT32 Event,INT32 Arg)
{	
	char bug[256];

	sprintf_s(bug, GAME_STRING("STR_TBL_GUI_LOBBY_DEBUG_POPUP_TEXT"), Event, Arg);/*디버그 팝업: Event(%d), Arg(0x%x)*/

#if defined(EVENT_ERROR_ROOM_JOIN_VOTE)
	if (EV_IS_ERROR(Arg, EVENT_ERROR_ROOM_JOIN_VOTE))
		sprintf_s(bug, GAME_STRING("STBL_IDX_EP_GAME_FAIL_START_C"));
#endif
	
	g_pFramework->MessageBox(bug, NULL, MBF_GAME_OK);
	m_SelectedElement = -1;
}

void CPhaseLobby::CbSameNick(void* pThis,INT32 nParam)
{
	((CPhaseLobby*)pThis)->_ShowNicknamePopup();
}

void CPhaseLobby::CbBeInvited(void* pThis,INT32 nParam)
{
	CPhaseLobby* pLobby = (CPhaseLobby*)pThis;

	pLobby->m_pInviteMessageBox = NULL;

	if (MBR_OK == nParam)
	{
		// 방만들기 창 닫기
		pLobby->OnCreateRoomCancelButtonClick();

		// 초대 수락
		for(INT32 i = 0; i < g_pGameContext->getLobbyRoomCount(); i++)
		{
			const ROOM_INFO_BASIC* pInfo = g_pGameContext->getRoomInfo(i);
			if ( ! pInfo )
				continue;

			// 방 인덱스로 찾는다
			if (pInfo->_Idx == pLobby->m_InviteInfo._idxRoom)
			{
				i3mem::FillZero(g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
				i3mem::Copy(g_pGameContext->getMyRoomPW(), pLobby->m_InviteInfo._pw, NET_ROOM_PW);

				g_pGameContext->JoinRoom(pInfo);
				return;
			}
		}

		// 찾을 수 없다면 에러팝업
		ERROR_POPUP(GAME_STRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));
		pLobby->m_bCheckInvite = TRUE;
	}
	else
	{
		// 초대 메시지 검사 다시 시작
		pLobby->m_bCheckInvite = TRUE;
	}
}

void CPhaseLobby::CbSuppliedConfirm(void* pThis,INT32 nParam)
{
	if( IsAvailableStageMode(STAGE_TYPE_TUTORIAL) )
	{
		((CPhaseLobby*)pThis)->ShowTutorialGuide();
	}
	else
	{
		// 초도보급 후, 이벤트 카드셋 팝업 출력
		((CPhaseLobby*)pThis)->ShowEventCardSetPopup();
	}
}

void CPhaseLobby::CbTutorialGuideOK(void* pThis,INT32 nParam)
{
	if(nParam == 0)
	{
		((CPhaseLobby*)pThis)->StartTutorialMode();
	}
}

void CPhaseLobby::CbEventCardSetConfirm(void* pThis,INT32 nParam)
{		
	// 이벤트 카드셋 팝업 출력 후, PC방 팝업 출력
	((CPhaseLobby*)pThis)->ShowPcCafePopup();
}

void CPhaseLobby::CbClanInvite(void* pThis, INT32 nParam)
{
	CPhaseLobby* pLobby = (CPhaseLobby*)pThis;

	if (MBR_OK == nParam)
	{
		pLobby->DoClanInvite();
	}
}

void CPhaseLobby::CbBlockUser(void* pThis, INT32 nParam)
{	
	CPhaseLobby* pLobby = (CPhaseLobby*)pThis;

	if (MBR_OK == nParam)
	{
		pLobby->DoBlockUser();
	}
}

INT32 CPhaseLobby::CompareRoom(void* pItem1,void* pItem2)
{
	ROOM_INFO_BASIC* pRoom[2];

	pRoom[0] = (ROOM_INFO_BASIC*)pItem1;
	pRoom[1] = (ROOM_INFO_BASIC*)pItem2;

	INT32 nResult = 0;

	INT32 i = 0;	//	여러 비교 기준중에서 최상단 한가지만 사용하도록 합시다. 멀티 비교할시 엔진 sort에서 문제 생김. komet

	switch(sm_SortOrder[i]._nType)
	{		
	case ROOM_SORT_NUMBER:
		nResult = pRoom[0]->_Idx - pRoom[1]->_Idx;													// idx순
		break;	
	case ROOM_SORT_MODE:
		{
			INT32 nRoomMask[2];
			
			for(INT32 i=0; i<2; i++)
			{
				switch( pRoom[i]->_StageMask )
				{
				case STAGE_MASK_TYPE_TRAINING:
					nRoomMask[i] = STAGE_MASK_TYPE_TRAINING;
					break;
				case STAGE_MASK_TYPE_SNIPER:
					nRoomMask[i] = STAGE_MASK_TYPE_SNIPER;
					break;
				case STAGE_MASK_TYPE_SHOTGUN:
					nRoomMask[i] = STAGE_MASK_TYPE_SHOTGUN;
					break;
				case STAGE_MASK_TYPE_KNUCKLE:
					nRoomMask[i] = STAGE_MASK_TYPE_KNUCKLE;
					break;
				case STAGE_MASK_TYPE_CLANMATCH:
					nRoomMask[i] = STAGE_MASK_TYPE_CLANMATCH;
					break;
				case STAGE_MASK_TYPE_HEADHUNTER:
					nRoomMask[i] = STAGE_MASK_TYPE_HEADHUNTER;
					break;
				default:
					nRoomMask[i] = 0;
					break;
				}
			}

			//	일반 모드
			if(nRoomMask[0] == 0 && nRoomMask[1] == 0)
			{
				nResult = getStageType( pRoom[0]->_StageID) - getStageType( pRoom[1]->_StageID);			// 단체전 < 개인전 < 미션전
			}
			//	일반 모드가 아니다.
			else if(nRoomMask[0] != 0 && nRoomMask[1] != 0)
			{
				//	모드 우선순위
				if( nRoomMask[0] > nRoomMask[1])
				{
					nResult = 1; 
				}
				else if( nRoomMask[0] < nRoomMask[1])
				{
					nResult = -1; 
				}
				else	//	같은 모드다.
				{
					nResult = 0; 
				}
			}
			else if(nRoomMask[0] != 0)
			{
				nResult = 1;
			}
			else if(nRoomMask[1] != 0)
			{
				nResult = -1;
			}
			
			break;
		}
	case ROOM_SORT_MAP:
		{
			char szRoomName[2][256];
			if ( GET_ROOM_INFO_RANDOM_MODE( pRoom[0]->_InfoFlag ) )
			{	
				strncpy(szRoomName[0], GAME_STRING("STR_POPUP_RANDOMMAP"), 256);
			}
			else
			{
				getStageDisplayName(szRoomName[0], (STAGE_ID) pRoom[0]->_StageID);
			}


			if( GET_ROOM_INFO_RANDOM_MODE( pRoom[1]->_InfoFlag ) )
			{
				strncpy(szRoomName[1], GAME_STRING("STR_POPUP_RANDOMMAP"), 256);
			}
			else
			{
				getStageDisplayName(szRoomName[1], (STAGE_ID) pRoom[1]->_StageID);
			}
			nResult = i3String::Compare(szRoomName[0], szRoomName[1]);			// 맵이름 순
		}
		break;
	//case ROOM_SORT_PRIVATE:
	//	{
	//		BOOL isPrivateRoom[2];

	//		isPrivateRoom[0] = GET_ROOM_INFO_PRIVATE_MODE( pRoom[0]->_InfoFlag );
	//		isPrivateRoom[1] = GET_ROOM_INFO_PRIVATE_MODE( pRoom[1]->_InfoFlag );

	//		// 일반방 < 비밀방
	//		if (isPrivateRoom[0] == isPrivateRoom[1])
	//		{
	//			nResult = 0;
	//		}
	//		else if (isPrivateRoom[0])
	//		{
	//			nResult = 1;
	//		}
	//		else
	//		{
	//			nResult = -1;
	//		}
	//	}
	//	break;
	case ROOM_SORT_STATE:
		nResult = pRoom[0]->_State - pRoom[1]->_State;							// 대기중 < 게임중
		break;
	case ROOM_SORT_PERSON:
		{
			INT32 nPerson[2];

			nPerson[0] = pRoom[0]->_PersonPlayerMax - pRoom[0]->_PersonNow;
			nPerson[1] = pRoom[1]->_PersonPlayerMax - pRoom[1]->_PersonNow;

			if (0 == nPerson[0]) nPerson[0] = SLOT_MAX_COUNT + 1;
			if (0 == nPerson[1]) nPerson[1] = SLOT_MAX_COUNT + 1;

			nResult = nPerson[0] - nPerson[1];									// (MAX - CUR), 단 0은 무조건 큼.
		}
		break;
	//case ROOM_SORT_PING:
	//	nResult = pRoom[0]->_Ping - pRoom[1]->_Ping;								// 핑레벨 낮은 순
	//	break;
	default:
		nResult = 0;
		break;
	}

	if( !sm_SortOrder[i]._bAscending )
	{
		nResult = -nResult;
	}

	return nResult;
}

void CPhaseLobby::ShowPcCafePopup(void)
{
	m_bPcCafeNotice = TRUE;


	USER_INFO_BASIC	MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	// 자기정보가 설정전이면 처리해선 안된다.
	if( i3String::Length(MyUserInfoBasic._nick) == 0)
	{
		return;
	}

	// PC방전용 아이템 지급 팝업을 출력한다.
	if (MyUserInfoBasic._pcCafe != PC_NOT_CAFE && MyUserInfoBasic._pcCafe != PC_INDO_TELCOM_ITEM)
	{
		char messageText[MAX_STRING_COUNT] =	UNKNOWN_TEXT;
		char expText[MAX_STRING_COUNT] =		UNKNOWN_TEXT;
		char pointText[MAX_STRING_COUNT] =		UNKNOWN_TEXT;
		char itemText[MAX_STRING_COUNT] =		UNKNOWN_TEXT;

		i3RegKey * pRootKey = Config::FindKey( "PCCafe");
		if( pRootKey != NULL)
		{
			i3RegKey * pFindKey = NULL;

			switch(MyUserInfoBasic._pcCafe)
			{
			case PC_NORMAL_CAFE:		pFindKey = Config::FindKey( "Normal", pRootKey);		break;
			case PC_PREMIUM_CAFE:		pFindKey = Config::FindKey( "Premium", pRootKey);		break;
			case PC_VACATION_EVENT_ITEM:		pFindKey = Config::FindKey( "EventItem", pRootKey);		break;
			}

			if( pFindKey != NULL)
			{
			//	FIND_REG_DATA( pFindKey, "BonusExp", expText);			//	PC방 보너스 경험치
			//	FIND_REG_DATA( pFindKey, "BonusPoint", pointText);		//	PC방 보너스 포인트
				FIND_REG_DATA( pFindKey, "BonusWeapon", itemText, MAX_STRING_COUNT );		//	PC방 보너스 무기
			}
		}

		//	PC방 보너스 팝업 내용
		i3String::Copy( messageText, TextSet::PCCafeMessage(MyUserInfoBasic._pcCafe), MAX_STRING_COUNT );


		m_pPcCafeMessage->SetText(messageText);
		sprintf_s( expText, "%d%%", g_pGameContext->GetPCCafeExp());
		m_pPcCafeExpValue->SetText(expText);
		sprintf_s( pointText, "%d%%", g_pGameContext->GetPCCafePoint());
		m_pPcCafePointValue->SetText(pointText);
		m_pPcCafeItemValue->SetText(itemText);

		m_pPcCafeWin->SetEnable(TRUE);
		_SetInputDisableGUI(TRUE);
	}
	else if (MyUserInfoBasic._pcCafe == PC_INDO_TELCOM_ITEM)
	{
		//인도네시아 텔컴 이벤트를 위해 만든것이라서 아이템아이디를 직접 넣습니다.	
		OpenItemWin(1105003003);
	//	OpenEventWin(702001024 );
	}
	else
	{
		ShowNoticePopup();
	}	
}

void CPhaseLobby::ShowTutorialGuide(void)
{
#if  !defined(LOCALE_MIDDLE_EAST)
	STRING_POPUP_Q(GAME_STRING("STR_TBL_GUI_BASE_NEW_ENTRY_TUTORIAL_MODE"), MAKE_CALLBACK(CbTutorialGuideOK), this);
#endif

}

void CPhaseLobby::ShowEventCardSetPopup(void)
{
	m_bEventCardSetNotice = TRUE;

	if (g_pGameContext->IsNewEventCardSet())
	{
		STRING_POPUP_C(GAME_STRING("STR_TBL_GUI_LOBBY_ETC_TEXT"), MAKE_CALLBACK(CbEventCardSetConfirm), this);
		g_pGameContext->RemoveGameFlag( GAMEFLAG_NEWEVENTCARDSET);
	}
	else if (g_pGameContext->GetReceivedEvent())
	{
		
		OpenEventWin(702001024);
		
	}
	else
	{
		ShowPcCafePopup();
	}
}

void CPhaseLobby::ShowNoticePopup(void)
{
	INT32 nListCount = g_pFramework->getNoticeListCount();
	char szTemp[MAX_PATH];

	const char *	pMyNickName = g_pGameContext->GetMyNickName();

	if( g_pFramework->getNoticeActivation())
	{
		// 초기 1회만 노출시키도록 체크합니다! (파일 비교 체크)
		{
			CFileFind finder;
			i3FileStream fileForCheck;

			BOOL bWorking = finder.FindFile("Script/Etc/NoticeCheck.log");	
			finder.Close();

			if( bWorking)
			{
				::DeleteFile("Script/Etc/NoticeCheck.log");
				fileForCheck.Close();

				if( fileForCheck.Create( "Script/Etc/NoticeCheck.log", STREAM_WRITE))
				{
					sprintf_s( szTemp, "%s_%s", g_pFramework->getNoticeVersion(), pMyNickName);
					fileForCheck.WriteLine( szTemp);
				}
			}
			else
			{
				if( fileForCheck.Create( "Script/Etc/NoticeCheck.log", STREAM_WRITE))
				{
					sprintf_s( szTemp, "%s_%s", g_pFramework->getNoticeVersion(), pMyNickName);
					fileForCheck.WriteLine( szTemp);
				}
			}				

			fileForCheck.Close();
		}

		g_pFramework->setNoticeActivation( FALSE);

		if( nListCount > 0)
		{
			INT32 i = 0;
			char szText[2048] = {0,};

			for( i=0; i<nListCount; i++)
			{
				i3String::Concat(szText, g_pFramework->getNoticeList(i));
			}

			STRING_NOTICE_POPUP(szText);
		}
	}
}


void CPhaseLobby::_SetInputDisableGUI(BOOL bDisable)
{
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(bDisable);
	m_pInfoWin->setInputDisable(bDisable);
	m_pRoomWin->setInputDisable(bDisable);
	m_pUserListWin->setInputDisable(bDisable);
}

void CPhaseLobby::_InitChat(void)
{
	m_pChatBody->InitChatBody( CHAT_BODY_STATE_LOBBY, MAX_CHAT_LINE_FOR_LOBBY);
}

void  CPhaseLobby::_ProcessChat( REAL32 rDeltaSeconds)
{
	m_pChatBody->OnUpdate(rDeltaSeconds, this);
}

void CPhaseLobby::OnCheckRandomMap()
{
	ROOM_INFO_BASIC		MyRoomInfo;
	g_pGameContext->getMyRoomInfo(&MyRoomInfo);
	MyRoomInfo._InfoFlag = SET_ROOM_INFO_RANDOM_MODE( MyRoomInfo._InfoFlag, (m_pLobbyCheckRandomMap->getCheckBoxState()) );
	g_pGameContext->setMyRoomInfo(&MyRoomInfo);

	// 랜덤맵 리스트 설정
	if( g_pGameContext->IsRandomMap() )	
	{
		CReadyPhaseBase::createRandomMap( getStageType(m_nStageID) );
	}
}


void CPhaseLobby::OnRandomMap(void)
{
	if(!m_pLobbyCheckRandomMap->getCheckBoxState())
		return;

	STAGE_TYPE	GameMode = getStageType(m_nStageID);
	((CStageReady*)m_pParent)->TogglePopup(POPUP_RANDOM_MAP, reinterpret_cast<void*>(GameMode));
}

void CPhaseLobby::EventMessage(void)
{
	INT32 i = 0;

	if(g_pGameContext->m_IsCertification == false)
	{
		// 중국인 경우에다 실명 인증이 안된 경우에만 들어옴
		g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_CHINA_NEED_CERTIFICATION_REALNAME"));
	}
	else
	{
		for( i=0; i<g_pFramework->getNoticeChatCount(); i++)
		{
			g_pFramework->getChatBox()->PutSystemChatting( g_pFramework->getNoticeChatList(i));
		}
	}
}

void CPhaseLobby::ResetCreateRoomAddInfo(void)
{
	// 확장된 방생성 팝업
	if( g_pGameContext->GetBattleChannel())
	{		
		m_nStageID = getFirstStageByType(STAGE_TYPE_BOMB);	// 폭파미션, 다운타운
		m_SubType = BATTLE_ROUND_TYPE_5 | BATTLE_TIME_MISSION_TYPE_3;
	}
	else
	{
		m_nStageID = getFirstStageByType(STAGE_TYPE_DEATHMATCH);
		m_SubType = BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
	}
	m_LockWeapon = LOCK_BASIC;//LOCK_PRIMARY | LOCK_SECONDARY | LOCK_MELEE | LOCK_THROW;
	m_LockInterEnter = 0;
	m_LockObserver = 0;
	m_LockTeamBalance = 1;
	
}

void CPhaseLobby::OnMapCombo(void)
{
	CReadyPhaseBase::InitEnableMapList(getStageType(m_nStageID), m_pMapList);

	ExclusionPopup(m_pMapList);
}

void CPhaseLobby::OnMapPopup(UINT32 idxSlot)
{
	STAGE_ID	stageId	= getStageId( (STAGE_ORDINAL_ID)m_EnableStageList[idxSlot].second );

	// 데스매치타입의 맵도 여러종류가 있다 
	// (섬멸전, 서든데스, 헤드헌터, 헤드킬러 - 나중에 추가될 수도 있음)
	// 현재 StageID에는 데스매치타입이 아닌 모드에 따른 타입이 저장되어 있으므로
	// 현재 StageID를 이용해 동일한 타입으로 바꿔줘야한다
	if ( STAGE_TYPE_DEATHMATCH == getStageType( stageId) )
	{
		m_nStageID	= (STAGE_ID)setStageType( stageId, getStageType( m_nStageID) );
	}
	else
	{
		m_nStageID	= stageId;
	}

	_UpdateMapInfo();
}

void CPhaseLobby::InitRandomMap()
{
	m_pLobbyCheckRandomMap->setCheckBoxState(g_pGameContext->IsRandomMap());

	// 랜덤맵 리스트 설정
	if( g_pGameContext->IsRandomMap() )	
	{
		CReadyPhaseBase::createRandomMap( getStageType(m_nStageID) );
	}
}

void CPhaseLobby::_ProcessMapInfo( REAL32 rDeltaSeconds)
{
	if( m_nStageID == 0)return; //getStageDisplayName( (STAGE_ID) pInfo->_StageID) 에러가 맵이동시 나올수 있습니다. 

	STAGE_TYPE stageType = getStageType( m_nStageID);
	UINT32 gameType	= m_SubType & 0x0F;
	UINT32 gameTime	= m_SubType & 0xF0;
	
	// 맵 이미지
	if(m_pLobbyCheckRandomMap->getCheckBoxState())
	{
		g_pFramework->SetRandomMapShape(m_pMapImage);//체크상태라면 ? 이미지 표현
		m_pLobbyRandomMapButton->setInputDisable(FALSE);
		m_pMapCombo->SetEnable(FALSE);
	}
	else
	{
		g_pFramework->SetMapShape( m_pMapImage, (STAGE_ID) m_nStageID);
		m_pLobbyRandomMapButton->setInputDisable(TRUE);
		m_pMapCombo->SetEnable(TRUE);
	}
	// 맵 이름, 모드
	{
		std::string strMapName;
		std::string strModeName;
		BOOL bAllowChangeMode = FALSE;
		
		GetStageMapName(m_pLobbyCheckRandomMap->getCheckBoxState(), (STAGE_ID)m_nStageID, strMapName);
		GetStageModeName(stageType, strModeName, bAllowChangeMode);

		m_pMap->SetText(strMapName.c_str());
		m_pGameModeValue->SetText(strModeName.c_str());

		// Mode Text
		m_pTypeImage->SetAllShapeEnable(FALSE);

		INT32 mapTypeIdx = GetMapImageShapeIndex(stageType);

		if( mapTypeIdx == -1 )
		{
			m_pTypeImage->SetVisible( FALSE );
		}
		else
		{
			m_pTypeImage->SetVisible( TRUE );
			m_pTypeImage->SetShapeEnable( mapTypeIdx, TRUE);
		}

		m_pExtraTypeImage->SetEnable(FALSE);
		if( g_pGameContext->IsPreStageOption( STAGE_OPTION_HEADHUNTER ) )
		{
			m_pExtraTypeImage->SetEnable(TRUE);
		}


		// NewMark
		BOOL bRandomMap = g_pGameContext->IsRandomMap();

		m_pMapNewMark->SetEnable(FALSE);
		if ( TESTBIT(getNewMarkStageTable( getStageOrd(m_nStageID)) , GOODS_MARK_NEW) && bRandomMap == FALSE)
		{
			m_pMapNewMark->SetEnable(TRUE);
		}

		// X-Mas Mark
		m_pMapXMasMark->SetEnable(FALSE);
		if ( TESTBIT(getNewMarkStageTable( getStageOrd(m_nStageID)) , GOODS_MARK_XMAS) && bRandomMap == FALSE)
		{
			m_pMapXMasMark->SetEnable(TRUE);
		}
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

		if( bAvariableMode && 
			!g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) && 
			!g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) )
		{
			m_pLimitRPG7Key->SetTextColor(GetColor(GCT_DEFAULT));
			m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		}
		else
		{
			m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID_C));
			m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		}
	}
	else
	{
		m_pLimitRPG7Key->SetEnable(FALSE);
		m_pLimitRPG7->SetEnable(FALSE);
	}

	if(g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) || 
		g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) ) 
	{
		m_pLimitRPG7Key->SetTextColor(GetColor(GCT_INVALID_C));
	}

	// 방어전에서는 밸런스 문제로 칼전, 권총전 불가능하도록 변경
	if( getStageType(m_nStageID) == STAGE_TYPE_ESCAPE ||
		getStageType(m_nStageID) == STAGE_TYPE_DEFENCE ||
		getStageType(m_nStageID) == STAGE_TYPE_DESTROY )
	{
		m_pLimitPrimary->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pLimitRPG7->setControlState(I3GUI_CONTROL_STATE_DISABLED);

		m_pLimitPrimary->setCheckBoxState( TRUE);
		m_pLimitPrimaryLock->SetEnable( TRUE);
	}
	else
	{
		if( !g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) ) 
		{
			if( !g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) ) 
			{
				m_pLimitPrimaryLock->SetEnable( FALSE);
			}
		}
	}
}

void CPhaseLobby::OnModePrevButtonClick(void)
{
	_RotateModeIndex(FALSE);
}

void CPhaseLobby::OnModeNextButtonClick(void)
{
	_RotateModeIndex(TRUE);
}

void CPhaseLobby::_RotateModeIndex(BOOL bNext,BOOL bRotateMap)
{
	// 변경된 모드에대한 스테이지를 표시합니다.
	m_nStageID = getNextStageType( (STAGE_ID)m_nStageID, bNext == TRUE);

	_UpdateMapInfo();

	// 랜덤맵 리스트 설정
	if( g_pGameContext->IsRandomMap() )
	{
		CReadyPhaseBase::createRandomMap( getStageType(m_nStageID) );
	}
}

void CPhaseLobby::OnVictoryPrevButtonClick(void)
{
	_RotateTypeIndex(FALSE);
}

void CPhaseLobby::OnVictoryNextButtonClick(void)
{
	_RotateTypeIndex(TRUE);
}

void CPhaseLobby::_RotateTypeIndex(BOOL bNext)
{
	{
		UINT8 idxMin;
		UINT8 idxMax;

		UINT8 stageType	= (UINT8)getStageType( m_nStageID);

		// 시작/끝 인덱스 설정
		switch( stageType)
		{
		case STAGE_TYPE_ANNIHILATION	: 
		case STAGE_TYPE_SUDDEN_DEATH	:
		case STAGE_TYPE_HEAD_KILLER	:
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
}

void CPhaseLobby::OnVictory2PrevButtonClick(void)
{
	_RotateTimeIndex(FALSE);
}

void CPhaseLobby::OnVictory2NextButtonClick(void)
{
	_RotateTimeIndex(TRUE);
}

void CPhaseLobby::_RotateTimeIndex(BOOL bNext)
{
	STAGE_TYPE stageType = getStageType( m_nStageID);

	m_SubType = CReadyPhaseBase::_RotateTimeIndex(stageType, m_SubType, bNext);
}

void CPhaseLobby::OnJoinLimitCheck(BOOL check)
{
	m_pLimitJoinYes->setCheckBoxState(check);
	m_pLimitJoinNo->setCheckBoxState(!check);
}
void CPhaseLobby::OnObserverLimitCheck(BOOL check)
{
//	m_pLimitObserverYes->setCheckBoxState(check);
	m_pLimitObserverNo->setCheckBoxState(!check);
}

void CPhaseLobby::OnTeamBalanceCheck(INT32 check)
{

	switch(check)
	{
	case 1:
		m_pGameTeamBalancePersonnel->setCheckBoxState(true);
		m_pGameTeamBalanceAbility->setCheckBoxState(false);
		m_pGameTeamBalanceNo->setCheckBoxState(false);
		break;
	case 2:
		m_pGameTeamBalancePersonnel->setCheckBoxState(false);
		m_pGameTeamBalanceAbility->setCheckBoxState(true);
		m_pGameTeamBalanceNo->setCheckBoxState(false);
		break;
	default:
		m_pGameTeamBalancePersonnel->setCheckBoxState(false);
		m_pGameTeamBalanceAbility->setCheckBoxState(false);
		m_pGameTeamBalanceNo->setCheckBoxState(true);
		break;
	}


}
void CPhaseLobby::_UpdateMapInfo(void)
{
	BOOL bCheckOffForRPG7 = (getStageSlot(m_nStageID) != STAGE_SLOT_MODE_4VS4);

	// RPG7 파괴미션에서는 불가
	if( getStageType(m_nStageID) != STAGE_TYPE_DESTROY && 
		getStageType(m_nStageID) != STAGE_TYPE_DEFENCE && 
		g_pConfigEx->GetGame().Enable_RPG7 && g_pConfigEx->GetGame().UnlimitedForRPG7)
	{
		bCheckOffForRPG7 = FALSE;
	}

	if( bCheckOffForRPG7)
	{
		//if(TESTBIT(m_LockWeapon ,	LOCK_RPG7))
		if( m_pLimitRPG7->getCheckBoxState())
		{
			if( m_pLimitRPG7->getCheckBoxState())
			{
				m_pLimitPrimary->setCheckBoxState(TRUE);
			}

			m_pLimitRPG7->setCheckBoxState(FALSE);			

//			UINT8 ui8Temp = ~LOCK_RPG7;
//			m_LockWeapon = m_LockWeapon & ui8Temp;
//			ui8Temp = LOCK_PRIMARY;
//			m_LockWeapon = m_LockWeapon | ui8Temp;
		}
	}

	// 변경된 모드에 따른 기본 설정 변경
	STAGE_TYPE type = getStageType( m_nStageID);

	switch( type)
	{
		case STAGE_TYPE_ANNIHILATION : 
		case STAGE_TYPE_BOMB :
		case STAGE_TYPE_ESCAPE :
		case STAGE_TYPE_SUDDEN_DEATH :
		case STAGE_TYPE_HEAD_KILLER :
			m_SubType	= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_5;
			break;

		case STAGE_TYPE_DESTROY :
			m_SubType	= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
			break;

		case STAGE_TYPE_DEFENCE :
			/*m_LockWeapon = */SETBIT( m_LockWeapon, LOCK_PRIMARY);		// 방어전은 주무기는 기본으로 설정
			m_SubType	= BATTLE_TIME_MISSION_TYPE_3 | BATTLE_ROUND_TYPE_3;
			break;
	
		default :
 			m_SubType	= BATTLE_TIME_TYPE_10 | BATTLE_KILL_TYPE_100;
			break;
	}	
	g_pGameContext->SetPrevStageType(type);
}

//static
void	CPhaseLobby::JoinRoomPasswordCallback(BOOL OK, const string & Password, void * pUserContext)
{
	if(i3String::Length(g_pGameContext->getMyRoomPW()) > 0)
		i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

	if ( OK )
	{
		//i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
		I3ASSERT(Password.size() <= sizeof(char) * NET_ROOM_PW);
		i3mem::Copy( g_pGameContext->getMyRoomPW(), Password.c_str(), Password.size());

		CPhaseLobby *	pLobby = (CPhaseLobby *)pUserContext;

		g_pGameContext->JoinRoom(&pLobby->GetJoinTargetRoom());
		i3GuiRoot::setGuiDisableInput(TRUE);
	}
}

void CPhaseLobby::FirstLoninPopup()
{
	if( g_pEnvSet->m_bWrongEnvSet)
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_LOBBY_NOTICE_DUPLICATED_KEY"));/*중복된 키 설정이 있어 키설정을 초기화 하였습니다*/
	}



	// 팝업 체인: 쿠폰팝업 -> PC방 팝업
	const COUPON_INFO* info = g_pGameContext->getCouponInfo();

	if (info != NULL && 0 < info->_count)
	{
		char couponMessage[MAX_STRING_COUNT];

		/*%d개의 쿠폰이 사용되었습니다.\n \n지급 내역\n \n　　　　　　　　　　　　%d 포인트*/
		sprintf_s(couponMessage, GAME_STRING("STR_TBL_GUI_LOBBY_CUPON_MESSAGE"), info->_count,info->_point);
		STRING_POPUP_C(couponMessage, MAKE_CALLBACK(CbSuppliedConfirm), this);
		g_pGameContext->clearCouponInfo();
		return;
	}

#if defined(USE_LINK_SHOP )
	//팝업 : 상점바로가기 : 로컬타임과 EnySet에 저장된 날짜를 비교합니다. 
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	g_pGameContext->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	char szTemp[128];
	if((i3String::Compare(g_pEnvSet->m_szDay,szTemp)!=0) && (InfoEquipWeapon._noprim == 0) )
	{
		if(g_pViewer->isFullScreen()&&i3String::Contain(g_pEnvSet->m_szDay,szTemp)!=-1 )
		{	//풀스크린모드시 m_szDay에 저장하는 값이 달라지므로 Contain으로 한번더 검사합니다.
		}
		else
		{
			//상점 바로가기 기능 
			(((CStageReady*)m_pParent)->TogglePopup(POPUP_LINK_SHOP, (void*) 1));
		}
		return;
	}
#endif
	
	if (!m_bEventCardSetNotice)
	{			
		ShowEventCardSetPopup();
		return;
	}

	if (!m_bEventNotice)
	{
		OpenEventWin(702001024);
		return;
	}

	if (!m_bPcCafeNotice)
	{
		ShowPcCafePopup();
		return;
	}

	ShowNoticePopup();
}

void CPhaseLobby::UpdateButton()
{
	//CStageReady * pReady = ((CStageReady*)m_pParent);
	// 특수전 비활성화
	/* 
	if(pReady->GetButtonEffect())
	{
		if(g_SpecialState)
		{
			m_pbtnSpecialWar->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pbtnSpecialWar->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
		if(g_RoomState)
		{
			m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
	}
	else
	{
		if(g_SpecialState)
		{
			m_pbtnSpecialWar->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pbtnSpecialWar->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
		if(g_RoomState)
		{
			m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pCreate->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
	}
	*/
}
//#ifdef USE_ABUSING_CHECK
void CPhaseLobby::OnAbusingConfirm()
{
	g_pGameContext->ResetAbuse();
	_SetInputDisableGUI(FALSE);
	m_pAbuseWindow->SetEnable(FALSE);
	((CStageReady*)m_pParent)->SetEffectButtonBegin();
	//어뷰징 완료 To 서버
	g_pGameContext->SetEvent(EVENT_ABUSING_SET);
}

void CPhaseLobby::_ProcessAbusing(REAL32 rTime) //어뷰징 프로세싱 하동익 20110402
{
	//if g_context gettime이 rTime이랑 같다면
	//버튼 활성화
	//아니면
	//버튼 텍스트 업데이트
	m_rAbusingTotalTime += rTime;
	UINT32 ForceTime = g_pGameContext->GetAbuseTime();
	UINT32 uiTemp = (UINT32)m_rAbusingTotalTime;

	if( uiTemp < ForceTime)
	{
		INT32 time = ForceTime - uiTemp;
		char temp[10],str[30];
		itoa(time,temp,10);
		sprintf_s(str,"%s",temp);//각 국가 번역 스트링으로
		INIT_CAPTION_BUTTON(m_pAbuseConfirm, str);/*확인*/
		//LinkControlEx(plist, "lbAbuseConfirm", m_pAbuseConfirm, GCI_LBP_A_BUTTON_ABUSING_CONFIRM);
		m_pAbuseConfirm->setInputDisable(TRUE);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 8);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 8);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 8);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONENABLE, GUI_SHAPE_OFFSET + 8);
	}
	else
	{
		INIT_CAPTION_BUTTON(m_pAbuseConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/
		m_pAbuseConfirm->setInputDisable(FALSE);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 9);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 9);
		m_pAbuseConfirm->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 8);
	
	}
}

//#endif