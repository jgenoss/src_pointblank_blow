#include "pch.h"
#include "StepProfile.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "WeaponBase.h"
#include "QuestTextDictionary.h"

#include "StepProfile_PrivateInfoBG.h"
#include "StepProfile_CompensateWin.h"
#include "StepProfile_Designation.h"
#include "StepProfile_CardCompleteWin.h"
#include "StepProfile_Challenge.h"


I3_CLASS_INSTANCE(CStepProfile, CStepBase);

CStepProfile::CStepProfile()
{
	m_pProfileGui = NULL;
	m_pProfileWin = NULL;
	m_pProfileCaption = NULL;
	m_pLeave = NULL;

	m_pChallengeTab = NULL;
	m_pDesignationTab = NULL;

	m_pChallenge = NULL;
	m_pChallengeWin = NULL;

	// PrivateInfo window
	m_pPrivateInfoBG = NULL;

	// Designation
	m_pDesignation = NULL;

	// 튜토리얼 보상창
	m_pCompensate	= NULL;	

	m_currentProfile = PT_START;
	m_bIsMyInfoView = TRUE;

}

CStepProfile::~CStepProfile()
{	
	I3_SAFE_RELEASE( m_pPrivateInfoBG);
	I3_SAFE_RELEASE( m_pCompensate);
	I3_SAFE_RELEASE( m_pDesignation);
	I3_SAFE_RELEASE( m_pChallengeWin);
}

void CStepProfile::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_PRS_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	case GCI_PRS_B_CHALLENGE_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnChallengeTab();
		break;
	case GCI_PRS_B_DESIGNATION_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDesignationTab();
		break;
	}

	m_pPrivateInfoBG->GuiNotify( pNotify);
	m_bIsMyInfoView = m_pPrivateInfoBG->isMyInfoView();
#if defined( ENABLE_NEW_CHALLENGE)
	m_pCompensate->GuiNotify( pNotify);
#endif


	m_pDesignation->setMyInfoView( m_bIsMyInfoView);
	m_pDesignation->GuiNotify( pNotify);
	m_pChallengeWin->setMyInfoView( m_bIsMyInfoView );
	m_pChallengeWin->GuiNotify( pNotify);
}

BOOL CStepProfile::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{	
	LinkControlEx(plist, "Profile", m_pProfileGui, -1);
	LinkControlEx(plist, "prProfileWin", m_pProfileWin, -1);
	LinkControlEx(plist, "prProfileCaption", m_pProfileCaption, -1);
	LinkControlEx(plist, "prLeave", m_pLeave, GCI_PRS_B_LEAVE);	

	LinkControlEx(plist, "prChallengeTab", m_pChallengeTab, GCI_PRS_B_CHALLENGE_TAB);
	LinkControlEx(plist, "prDesignationTab", m_pDesignationTab, GCI_PRS_B_DESIGNATION_TAB);	

	// PrivateInfo window
	m_pPrivateInfoBG = CStepProfile_PrivateInfoBG::NewObject();
	m_pPrivateInfoBG->LinkControl( this, plist, pControl);

	// Designation tap
	m_pDesignation = CStepProfile_Designation::NewObject();
	m_pDesignation->LinkControl( this, plist, pControl);


	// 튜토리얼 보상창
#if defined( ENABLE_NEW_CHALLENGE)
	m_pCompensate = CStepProfile_CompensateWin::NewObject();
	m_pCompensate->LinkControl( this, plist, pControl);
#endif
	
	LinkControlEx(plist, "prChallenge", m_pChallenge, -1);

	m_pChallengeWin = CStepProfile_Challenge::NewObject();
	m_pChallengeWin->LinkControl( this, plist, pControl);

	return FALSE;
}

void CStepProfile::OnInitControl(void)
{
	// 컨트롤 초기화
	_InitCommon();
	_InitChallenge();

#ifndef DESIGZATION_ENABLE
	m_pDesignationTab->SetEnable( FALSE);
#endif
	m_pDesignation->Init();

#if defined( ENABLE_NEW_CHALLENGE)
	m_pCompensate->Init();
#endif

	m_pChallengeWin->Init();

#if defined(ENABLE_MISSION_CARD)
	_EnableExclusionProfile(PT_CHALLENGE);
#endif

	CStepBase::OnInitControl();
}

void CStepProfile::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CStepBase::OnUpdate(rDeltaSeconds);

	// 퀘스트 매니저가 생성될때까지 업데이트하지 않는다.
	CQuestManager * pQM = g_pGameContext->getQuestManager();
	if( pQM == NULL )//|| (pQM->isQuestActivated() == false))
	{
		return;
	}

	if( m_pChallengeWin->isEnable() )
		m_pChallengeWin->Update( rDeltaSeconds);


#if defined( DESIGZATION_ENABLE)
	// Designation(호칭) Update.
	m_pPrivateInfoBG->Update( rDeltaSeconds);

	if( m_pDesignation->isEnable())
	{
		m_pDesignation->Update( rDeltaSeconds);
	}

#endif
}

void CStepProfile::OnStart(void)
{
	CStepBase::OnStart();

	// Default View
	_EnableExclusionProfile(PT_CHALLENGE);
	OnChallengeTab();

	SetPushedStateButton(MBT_PROFILE, TRUE);

	m_pChallengeWin->Reset();
}

void CStepProfile::OnEnd(void)
{
	CStepBase::OnEnd();

	m_pChallengeWin->OnEnd();

	if( g_pGameContext->getQuestManager() != NULL)
	{
		g_pGameContext->getQuestManager()->NoticeChangeActiveIndex();
	}

	SetPushedStateButton(MBT_PROFILE, FALSE);
}

void CStepProfile::OnInitRun(void)
{
	CStepBase::OnInitRun();

	MoveCamera(CL_RECORD);

	m_pPrivateInfoBG->Init();
}

void CStepProfile::OnPreGuiNotify(void)
{
}

void CStepProfile::OnExitKey(void)
{
	OnLeave();
}

void CStepProfile::SetInputDisable(BOOL bDisable)
{
	m_pProfileGui->setInputDisable(bDisable);
}

BOOL CStepProfile::GetInputDisable(void)
{
	return m_pProfileGui->getInputDisable();
}

void CStepProfile::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	m_pDesignation->OnGameEvent( event, arg);
	m_pPrivateInfoBG->OnGameEvent( event, arg);
	m_pChallengeWin->OnGameEvent( event, arg);
}

BOOL CStepProfile::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				if( m_pChallengeWin->OnPopupCancel() )
					return TRUE;
			}

			if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				if( m_pChallengeWin->OnPopupConfirm() )
					return TRUE;
			}
		}
	}

	return CStepBase::OnEvent(event, pObj, param2, code);
}

BOOL CStepProfile::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CStepBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;
	}

	// 시간에 따른 기준위치 지정
	m_pProfileGui->setPositionY(CStepBase::GetSlideY());


	// 퇴장 시, 비활성화. 그외 활성화
	BOOL enableWindow = TRUE;

	if (SS_OUT == CStepBase::GetSlideState())
	{
		enableWindow = FALSE;
	}

	m_pProfileGui->SetEnable(enableWindow);

	return TRUE;
}

void CStepProfile::EnterInfo(void)
{
	/* Do nothing */
}

void CStepProfile::LeaveInfo(void)
{
	g_pGameContext->SetEventOnce(EVENT_LEAVE_INFO);
}

void CStepProfile::OnLeave(void)
{	
	CStageReady * pParent = ((CStageReady*)m_pParent);
	I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

	if( g_pFramework->GetClanContext()->InBattleTeam())
	{
		if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
			pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
		else
			pParent->ChangeStep(STEP_NULL);
	}
	else
	{
		pParent->ChangeStep(STEP_NULL);
	}
}


void CStepProfile::OnChallengeTab(void)
{
	m_pDesignationTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pDesignation->SetEnable(FALSE);


	m_pChallengeTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);	
	m_pChallengeWin->SetEnable(TRUE);


	// Designation(호칭)관련
	m_pDesignation->Reset();

#if !defined( ENABLE_VIEW_USER)
	m_pPrivateInfoBG->SetEnable( FALSE);
#endif
}


void CStepProfile::OnDesignationTab(void)
{
	m_pChallengeTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pChallengeWin->SetEnable(FALSE);

	m_pDesignationTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pDesignation->SetEnable(TRUE);

	// Designation(호칭)관련
	m_pDesignation->Reset();

#if !defined( ENABLE_VIEW_USER)
	m_pPrivateInfoBG->SetEnable( TRUE);
#endif
}


void CStepProfile::_InitCommon(void)
{
	// 기본
	m_pProfileGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pProfileCaption, "");

	INIT_BUTTON_EX(m_pLeave);
}

void CStepProfile::_InitChallenge(void)
{
	m_pChallengeWin->SetEnable(FALSE);

	// 임무 공통
	INIT_TAB_EX(m_pChallengeTab);
	INIT_TAB_EX(m_pDesignationTab);
	m_pChallengeTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

}

void CStepProfile::_EnableExclusionProfile(ProfileType changeProfile)
{
	switch(m_currentProfile)
	{
	case PT_CHALLENGE:
		m_pChallengeWin->SetEnable( FALSE);
		break;
	}

	m_currentProfile = changeProfile;


	char strCaption[MAX_STRING_COUNT] = {};

	switch(m_currentProfile)
	{
	case PT_CHALLENGE:
		m_pChallengeWin->SetEnable(TRUE);
		i3String::Copy(strCaption, GAME_STRING("STR_TBL_PROFILE_CHALLENGE_CARD_TEXT1"), MAX_STRING_COUNT);/*임무카드*/
		break;
	}

	m_pProfileCaption->SetText(strCaption);
}


void CStepProfile::EnableInputProfileWin( void)
{
	m_pProfileWin->setInputDisable(FALSE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(FALSE);
}

void CStepProfile::DisableInputProfileWin( void)
{
	m_pProfileWin->setInputDisable(TRUE);
	((CStageReady*)m_pParent)->SetMenuBarInputDisable(TRUE);
}



