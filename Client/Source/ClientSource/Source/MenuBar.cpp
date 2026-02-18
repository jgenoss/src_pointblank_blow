#include "pch.h"
#include "MenuBar.h"
#include "StageReady.h"
#include "GlobalVariables.h"
#include "MessageBox.h"

I3_CLASS_INSTANCE(CMenuBar, CGuiNotifyReceiver);

char CMenuBar::m_strAnnounceBuffer[MAX_STRING_COUNT];
INT32 CMenuBar::m_idxAnnounceLetter = -1;
char CMenuBar::m_strAnnounceBuffer2[MAX_STRING_COUNT];
BOOL CMenuBar::m_bFCMFlag = FALSE;
BOOL CMenuBar::m_bFlowFlag = FALSE;

CMenuBar::CMenuBar()
{
	m_pMenuBar = NULL;
	m_pInfoBar = NULL;
	m_pClan = NULL;
	m_pProfile = NULL;
	m_pInventory = NULL;
	m_pShop = NULL;
	m_pCommunity = NULL;
	m_pOption = NULL;
	m_pExit = NULL;
	m_pNickname = NULL;
	m_pExp = NULL;
	m_pPinoKey = NULL;
	m_pPinoValue = NULL;
	m_pNcCoinKey = NULL;
	m_pNcCoinValue = NULL;
	m_pRank = NULL;
	m_pExpBar = NULL;
	m_pExpGage = NULL;
	m_pESport = NULL;
	m_pExpText = NULL;
	m_pAnnounce = NULL;
	m_pCalculateText = NULL;

	m_bAlarmCommunity = FALSE;
	m_bAlarmFlag = FALSE;
	m_rAlarmTerm = 0.0f;

	m_pMessageBox = NULL;
	m_bRoomMsterKickAlarmFlag = FALSE;
	m_nCurrentCountDown = 0;
	m_ExitMsgBoxOffset.x = m_ExitMsgBoxOffset.y = 0;
	m_bNickSpace = false;

	m_bFlowTime = 0.0f;

	m_FFlow = FALSE;
	m_flowState = FS_INIT;
	m_nAnnounceWidth = 0;
	m_nCurBlankLength = 0;
	m_nMaxBlankLength = 0;
	m_nMoveOffset = 0;
	m_rCurrentOffset = 0;
}

CMenuBar::~CMenuBar()
{
	if(m_pMessageBox)
	{
		g_pFramework->CloseMessageBox(m_pMessageBox);
		m_pMessageBox = NULL;
	}

	_DestroyFlowAnnounce();
}

void CMenuBar::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch( pNotify->m_nID)
	{
	case GCI_MB_B_CLAN:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_CLAN);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnClan();
			}
		}
		break;
	case GCI_MB_B_PROFILE:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_PROFILE);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnProfile();
			}
		}
		break;
	case GCI_MB_B_INVENTORY:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_INVENTORY);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnInventory();
			}
		}
		break;
	case GCI_MB_B_SHOP:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_SHOP);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnShop();
			}
		}
		break;
	case GCI_MB_B_COMMUNITY:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_COMMUNITY);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnCommunity();
			}
		}
		break;
	case GCI_MB_B_OPTION:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_OPTION);	
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) 
		{
			if( !m_bNickSpace )
			{
				OnOption();
			}
		}
		break;
	case GCI_MB_B_EXIT:
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_ONMOUSE) SetTooltip(TIP_EXIT);
		if (pNotify->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED)
		{
			if( !m_bNickSpace )
			{
				OnExit();
			}
		}
		break;
	}
}

BOOL CMenuBar::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	// 메뉴바
	LinkControlEx(plist, "mbMenuBarBody", m_pMenuBar, -1);
	LinkControlEx(plist, "mbInfoBar", m_pInfoBar, -1);

	// 메뉴바 버튼
	LinkControlEx(plist, "mbClan", m_pClan, GCI_MB_B_CLAN);
	LinkControlEx(plist, "mbProfile", m_pProfile, GCI_MB_B_PROFILE);
	LinkControlEx(plist, "mbInventory", m_pInventory, GCI_MB_B_INVENTORY);
	LinkControlEx(plist, "mbShop", m_pShop, GCI_MB_B_SHOP);
	LinkControlEx(plist, "mbCommunity", m_pCommunity, GCI_MB_B_COMMUNITY);
	LinkControlEx(plist, "mbOption", m_pOption, GCI_MB_B_OPTION);
	LinkControlEx(plist, "mbExit", m_pExit, GCI_MB_B_EXIT);

	// 인포바 텍스트 정보
    LinkControlEx(plist, "mbNickname", m_pNickname, -1);
	LinkControlEx(plist, "mbExp", m_pExp, -1);
	LinkControlEx(plist, "mbPinoKey", m_pPinoKey, -1);
	LinkControlEx(plist, "mbPinoValue", m_pPinoValue, -1);
	LinkControlEx(plist, "mbNcCoinKey", m_pNcCoinKey, -1);
	LinkControlEx(plist, "mbNcCoinValue", m_pNcCoinValue, -1);

	// 스테틱 마크, 경험치 게이지
	LinkControlEx(plist, "mbRankMark", m_pRank, -1);
	LinkControlEx(plist, "mbExpBar", m_pExpBar, -1);
	LinkControlEx(plist, "mbExpGage", m_pExpGage, -1);
	LinkControlEx(plist, "mbESport", m_pESport, -1);	

	LinkControlEx(plist, "mbExpText", m_pExpText, -1);
	
	// 상단 공지
	LinkControlEx(plist, "mbAnnounce", m_pAnnounce, -1);	
	
	return FALSE;
}

void CMenuBar::OnInitControl(void)
{
	CGuiNotifyReceiver::OnInitControl();

	// 버튼 초기화
	INIT_BUTTON_EX(m_pClan);
	INIT_BUTTON_EX(m_pProfile);
	INIT_BUTTON_EX(m_pInventory);
	INIT_BUTTON_EX(m_pShop);
	INIT_BUTTON_EX(m_pCommunity);
	INIT_BUTTON_EX(m_pOption);
	INIT_BUTTON_EX(m_pExit);


	// 에디트 박스 초기화
	const UINT32 MaxEdit = 7;
	i3GuiEditBox* table[MaxEdit] = {m_pNickname, m_pExp, m_pPinoKey, 
									m_pPinoValue, m_pNcCoinKey, 
									m_pNcCoinValue, m_pExpText};

	for(UINT32 i = 0; i < MaxEdit; i++)
	{
		table[i]->SetTextColor(GetColor(GCT_INVERSE));
	}

	m_pNickname->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pExp->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPinoKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPinoValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNcCoinKey->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNcCoinValue->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);

	m_pAnnounce->SetTextColor(GetColor(GCT_NOTIFY));
	m_pAnnounce->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	

	// 문자열 정렬을 위해 '톨'뒤에 전각 띄어쓰기 2자 추가했습니다.
	m_pPinoKey->SetText(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_POINT"));/*포인트    :*/


#if defined( LOCALE_KOREA)
	char str[256] = "";
	i3String::Format( str, 256, "%s\n%s", GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_MONEY"), GAME_STRING("STR_TBL_GUI_HAPPY_COIN"));
	m_pNcCoinKey->SetText(str);/*N-Coin  해피코인　　    :*/
#else
	m_pNcCoinKey->SetText(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_MONEY"));/*톨　　    :*/
#endif

	m_pExpText->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	// 스테틱 초기화
	GameGUI::SetRankStatic(m_pRank);
	
	m_pExpGage->SetAllShapeEnable(FALSE);

	m_pESport->SetEnable( FALSE);
	m_pESport->SetAllShapeEnable(FALSE);


	m_pClan->SetEnable(FALSE);
	m_pInfoBar->SetEnable(FALSE);
	m_pProfile->SetEnable(FALSE);
	m_pInventory->SetEnable(FALSE);
	m_pShop->SetEnable(FALSE);
	m_pCommunity->SetEnable(FALSE);
	m_pOption->SetEnable(FALSE);


	_InitFlowAnnounce();
}

void CMenuBar::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CGuiNotifyReceiver::OnUpdate(rDeltaSeconds);

	_UpdateUserInfo();

	_UpdateFlowAnnounce(rDeltaSeconds);	

	_UpdateAlarm(rDeltaSeconds);

	_UpdateRoomMasterKickAlarm();


	UpdateButton();
}

void CMenuBar::OnClan(void)
{
	if ( ! g_pConfigEx->GetClan().Enable )
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));/*차후에 지원될 예정입니다.*/
		return;
	}

	StepType stepType = STEP_NULL;
	BOOL bEnter = FALSE;

	if(g_pFramework->GetClanContext()->IsMember())
	{
		stepType = STEP_CLAN_MEMBER;			//	클랜 가입자 

		bEnter = ((CStageReady*)m_pParent)->IsEnterClanMember();
	}
	else
	{
		stepType = STEP_CLAN_NON_MEMBER;		//	클랜 미가입자

		bEnter = ((CStageReady*)m_pParent)->IsEnterClanNonMember();
	}

	if (bEnter)
	{
		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);

#if defined TEST_CLAN_MEMBER
		((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_MEMBER);	
#else
		((CStageReady*)m_pParent)->ChangeStep(stepType);	
#endif
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(stepType);
		}
	}

}

void CMenuBar::OnProfile(void)
{
	g_pGameContext->DeActiveMissionButtonEffect();
#if defined (ENABLE_MISSION_CARD)
	if (((CStageReady*)m_pParent)->IsEnterInfo())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
			else
				pParent->ChangeStep(STEP_PROFILE);
		}
		else
		{
			pParent->ChangeStep(STEP_PROFILE);
		}

		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_PROFILE);
		}
	}
#else
	STRING_POPUP(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));/*차후에 지원될 예정입니다.*/
#endif
}

void CMenuBar::OnInventory(void)
{
	if (((CStageReady*)m_pParent)->IsEnterInventory())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
			else
				pParent->ChangeStep(STEP_INVENTORY);
		}
		else
		{
			pParent->ChangeStep(STEP_INVENTORY);
		}

		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);			
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_INVENTORY);			
		}
	}
}

void CMenuBar::OnShop(void)
{
	if ( ! g_pConfigEx->GetShop().Enable )
	{
		STRING_POPUP(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_UNDER_CONSTRUCTION"));/*차후에 지원될 예정입니다.*/
		return;
	}

	if (((CStageReady*)m_pParent)->IsEnterShop())
	{
		CStageReady * pParent = ((CStageReady*)m_pParent);
		I3ASSERT( pParent->IsTypeOf( CStageReady::GetClassMeta()));

		if( g_pFramework->GetClanContext()->InBattleTeam())
		{
			if( g_pFramework->GetClanContext()->GetInClanBattleLobby())
				pParent->ChangeStep(STEP_CLAN_BATTLE_LOBBY);		//	클랜전 로비 예외 처리
			else
				pParent->ChangeStep(STEP_SHOP);
		}
		else
		{
			pParent->ChangeStep(STEP_SHOP);
		}

		((CStageReady*)m_pParent)->TogglePopup(POPUP_NULL);		
	}
	else
	{
		if (FALSE == i3GuiRoot::getGuiDisableInput())
		{
			((CStageReady*)m_pParent)->ExclusedEnter(STEP_SHOP);
			g_pFramework->GetShopContext()->EnterServer();	
		}
	}
}

void CMenuBar::OnCommunity(void)
{
	EndAlarmCommunity();
	((CStageReady*)m_pParent)->TogglePopup(POPUP_COMMUNITY);
}

void CMenuBar::OnOption(void)
{
	((CStageReady*)m_pParent)->TogglePopup(POPUP_OPTION);
	//((CStageReady*)m_pParent)->TogglePopup(POPUP_COLOR_SET);
	
}

void CMenuBar::OnExit(void)
{
	CMessageBox * pMessageBox = ERROR_POPUP_Q(GAME_STRING("STBL_IDX_EP_GAME_EXIT_NORMAL_QE"), EXIT_CALLBACK(CbExitNormal), this);
	POINT2D ptCenter = g_pFramework->GetCenterPosition(pMessageBox->GetWidth(), pMessageBox->GetHeight());
	ptCenter.x += m_ExitMsgBoxOffset.x;
	ptCenter.y += m_ExitMsgBoxOffset.y;
	pMessageBox->SetPosition(ptCenter.x, ptCenter.y);
}

void CMenuBar::SetInputDisable(BOOL bEnable)
{
	m_pMenuBar->setInputDisable(bEnable);
}

void CMenuBar::SetClanInputDisable(BOOL bEnable)
{
	m_pClan->setInputDisable(bEnable);
}

void CMenuBar::SetServerMode(void)
{
	m_pClan->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pProfile->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pInventory->setControlState(I3GUI_CONTROL_STATE_DISABLED);		
	m_pShop->setControlState(I3GUI_CONTROL_STATE_DISABLED);		
	m_pCommunity->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pOption->setControlState(I3GUI_CONTROL_STATE_DISABLED);
}

void CMenuBar::SetReadyMode(void)
{
	m_pInfoBar->SetEnable(TRUE);
	m_pClan->SetEnable(TRUE);
	m_pProfile->SetEnable(TRUE);
	m_pInventory->SetEnable(TRUE);
	m_pShop->SetEnable(TRUE);
	m_pCommunity->SetEnable(TRUE);
	m_pOption->SetEnable(TRUE);

	m_pClan->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pProfile->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pInventory->setControlState(I3GUI_CONTROL_STATE_NORMAL);	
	m_pOption->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_pShop->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	
	m_pCommunity->setControlState(I3GUI_CONTROL_STATE_NORMAL);
}

void CMenuBar::StartAlarmCommunity(void)
{
	m_bAlarmCommunity = TRUE;
}

void CMenuBar::EndAlarmCommunity(void)
{
	m_bAlarmCommunity = FALSE;

	// 원상복귀
	m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
}

void CMenuBar::SetPushedStateButton(INT32 menuButtonType,BOOL bPushed)
{
	// 상태를 바꿀 버튼을 받는다.
	i3GuiButton* button;

	switch(menuButtonType)
	{
	case MBT_CLAN:			button = m_pClan;		break;
	case MBT_PROFILE:		button = m_pProfile;	break;
	case MBT_INVENTORY:		button = m_pInventory;	break;
	case MBT_SHOP:			button = m_pShop;		break;
	case MBT_COMMUNITY:		button = m_pCommunity;	break;
	case MBT_OPTION:		button = m_pOption;		break;
	default:
		return;
	}

	// 버튼의 상태를 바꾼다.
	if (bPushed)
	{
		button->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_CLICKED);		
		button->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_CLICKED);
	}
	else
	{
		button->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_NORMAL);
		button->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_ONMOUSE);
	}
}

void CMenuBar::SetExitMsgBoxOffset(INT32 x, INT32 y)
{
	m_ExitMsgBoxOffset.x	= x;
	m_ExitMsgBoxOffset.y	= y;
}

void CMenuBar::SetFlowAnnounce(const char* strAnnounce)
{
	if (MAX_STRING_COUNT > i3String::Length(strAnnounce))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if (0 != i3String::Compare(m_strAnnounceBuffer, strAnnounce))
		{
			i3String::Copy(m_strAnnounceBuffer, strAnnounce, MAX_STRING_COUNT );		
			m_idxAnnounceLetter = -1;			
		}
	}
}

void CMenuBar::SetFlowAnnounce2(const char* strAnnounce)
{
	if (MAX_STRING_COUNT > i3String::Length(strAnnounce))
	{
		// 기존 공지와 다를 경우만 바꾼다
		if (0 != i3String::Compare(m_strAnnounceBuffer2, strAnnounce))
		{
			i3String::Copy(m_strAnnounceBuffer2, strAnnounce, MAX_STRING_COUNT );		
			m_idxAnnounceLetter = -1;			
			m_bFCMFlag = TRUE;
			m_bFlowFlag = TRUE;
		}
	}
}
//하동익 : Abusing 완료 후 갱신 : 하동익
void CMenuBar::_UpdateUserInfo(void)
{
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	
	// 닉네임
	if( MyUserInfoBasic._nickcolor == 0)
	{
		m_pNickname->SetTextColor(GetColor(GCT_INVERSE));	//	컬러 닉네임이 아닐 경우
		m_pNickname->SetTextEllipsis(MyUserInfoBasic._nick);
	}
	else
	{	//	컬러 닉네임을 사용하고 있을 경우
		SetNameUsingColorTable(m_pNickname, MyUserInfoBasic._nick, MyUserInfoBasic._nickcolor);
	}



	/*
	switch( MyUserInfoBasic._eSportID)
	{
	case ESPORT_ID_LEVEL_ZERO:	// 유저의 E-Sport 태그
		{
			m_pESport->SetEnable( TRUE);
			m_pESport->SetShapeEnable(0, TRUE);
		}
		break;
	default:
		{
			m_pESport->SetEnable( FALSE);
		}
		break;
	}
	*/
	if (MyUserInfoBasic._eSportID == ESPORT_ID_LEVEL_ZERO)
	{
		m_pESport->SetEnable( FALSE);
	}
	else
	{
		m_pESport->SetEnable( TRUE);
		m_pESport->SetShapeEnable(0, TRUE);

	}

	{	
		char strBuf[256];
		char strBuf1[256];
		_ShowRankMark( MyUserInfoBasic._rank );

		UINT32 wRank = g_pGameContext->GetRealRank();

		INT32 nCurrentExp = MyUserInfoBasic._exp - _GetExpMax(wRank);
		nCurrentExp = MAX(nCurrentExp, 0);	// 음수값은 0
		INT32 nExpRate = 100;


		// 상대평가 계급이 목표일 경우(45:대령 진 이상) 목표 경험치 제거
		if(51 == wRank)
		{
			i3String::Copy( strBuf, GAME_STRING("STR_TBL_GUI_ETC_CUMULATIVE_EXP"), 256 );
			sprintf_s( strBuf1, "%d    ", MyUserInfoBasic._exp);
			i3String::Concat( strBuf,strBuf1 );
		
		}
		else if (45 <= wRank)
		{
			sprintf_s( strBuf, "%d (100%%)   ", MyUserInfoBasic._exp);
		}
		else
		{
			INT32 nMaxExp = _GetExpMax(wRank + 1) - _GetExpMax(wRank);		
			if( nMaxExp != 0) nExpRate = (nCurrentExp * 100) / nMaxExp;

			nExpRate = MINMAX(0, nExpRate, 100);	// 100%가 넘을 경우 100%으로 고정 출력
			sprintf_s( strBuf, "%d/%d (%d%%)  ", nCurrentExp, nMaxExp, nExpRate);
		}
		m_pExp->SetText(strBuf);


		char gageText[MAX_STRING_COUNT] = {0, };
		BOOL visibleGage = TRUE;

        // 상대평가 계급 (45:대령 진 이상)
		
		if(51 == wRank)
		{
			i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_HERO"), MAX_STRING_COUNT );//영웅

			visibleGage = FALSE;
		}
		else if (50 == wRank)
		{
			i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED5"), MAX_STRING_COUNT );//상위 1% 랭킹의 원수 계급 유지 중
			visibleGage = FALSE;
		}
		else if (45 <= wRank)
		{
			if (_GetExpMax(50) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED3"), MAX_STRING_COUNT );//원수 진급은 상위 1% 랭킹 필요
				visibleGage = FALSE;
			}
			else if (_GetExpMax(49) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED6"), MAX_STRING_COUNT );//대장 진급은 상위 2% 랭킹 필요
				visibleGage = FALSE;
			}
			else if (_GetExpMax(48) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED1"), MAX_STRING_COUNT );//중장 진급은 상위 3% 랭킹 필요
				visibleGage = FALSE;
			}
			else if (_GetExpMax(47) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED4"), MAX_STRING_COUNT );//소장 진급은 상위 4% 랭킹 필요
				visibleGage = FALSE;
			}
			else if (_GetExpMax(46) <= (signed)MyUserInfoBasic._exp)
			{
				i3String::Copy(gageText, GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_RANKUP_NEEDED2"), MAX_STRING_COUNT );//준장 진급은 상위 5% 랭킹 필요
				visibleGage = FALSE;
			}
		}

	
		m_pExpText->SetText(gageText);
		m_pExpBar->SetEnable(visibleGage);

		if (visibleGage)
		{
			_ShowDotRate(nExpRate);
		}

		sprintf_s( strBuf, "%d", MyUserInfoBasic._point);
		m_pPinoValue->SetText(strBuf);

#if defined (LOCALE_KOREA)		
		//N-Coin & 해피코인는 한국 NC만 사용
		sprintf_s( strBuf, "%d\n %d", MyUserInfoBasic._NCCoin, MyUserInfoBasic._HappyCoin);
#else
		sprintf_s( strBuf, "%d", MyUserInfoBasic._cash);

#endif
		m_pNcCoinValue->SetText( strBuf);
	}
}

void CMenuBar::_ShowRankMark(INT32 nRank)
{
	m_pRank->SetShapeUVIndex(nRank);
}

INT32 CMenuBar::_GetExpMax(INT32 nRank)
{
	I3ASSERT(nRank >= 0);
	I3ASSERT((nRank >= 0 && nRank < RANK_NEWBIE/*임시*/) || (nRank == RANK_GAMEMASTER) || (nRank == RANK_NEWBIE));

	if (nRank >= 0 && nRank < RANK_51+1)
	{
		return g_ExpValue[nRank];
	}
	else
	{
		return 0;
	}
}

void CMenuBar::_ShowDotRate(INT32 nExpRate)
{
	INT32 nDotNumber;

	if( nExpRate >= 100)	nDotNumber = 20;
	else if( nExpRate <= 0)		nDotNumber = 0;
	else nDotNumber = nExpRate / 5;

	m_pExpGage->SetAllShapeEnable(FALSE);
	
	for(INT32 i = 0; i < nDotNumber; i++)
	{
		m_pExpGage->SetShapeEnable(GUI_SHAPE_OFFSET + i, TRUE);
	}
}

void CMenuBar::_InitFlowAnnounce(void)
{
	I3ASSERT(NULL == m_pCalculateText);
	
	I3COLOR color;
	i3Color::Set(&color, (UINT8) 0, 0, 0, 0);

	m_pCalculateText = i3TextNodeDX2::NewObject();
#if USE_VERTEX_BANK
	m_pCalculateText->Create( GetDefaultFontName(), 256, 10, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, TRUE);
#else
	m_pCalculateText->Create( GetDefaultFontName(), 256, 10, FW_NORMAL, 0, 0, 0, 0, FONT_SHADOW_QUALITY_LOW, 1.0f, FALSE);
#endif
	m_pCalculateText->setPos(0, 0);
	m_pCalculateText->setSize(100, 25);
	m_pCalculateText->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);	
	m_pCalculateText->SetColor(&color);
	g_pFramework->getMessagBoxLayer()->AddChild(m_pCalculateText);

	REAL32 rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;
	m_nAnnounceWidth = (INT32)(m_pAnnounce->getWidth() * rateX);
	m_nAnnounceWidth = MAX(0, m_nAnnounceWidth - 60);

	// dummmy
	//SetFlowAnnounce(GAME_STRING("STR_TBL_GUI_ETC_MENUBAR_ADVERTISEMENT"));/*[공지] 타격전율! 포인트 블랭크, 동접 100만 돌파 기념 감사 이벤트 - 매스킬을 노려라! 한녀석, 두녀석 날아갈때마다 경품도 함께 터진다! 미니쿠페, 하와이 패키지, AK47(세금별도), 불카누스2, 제페토 핸드폰고리 한웅쿰 등 믿기힘든 경품들이 준비되어있다!*/
	SetFlowAnnounce( g_pGameContext->getNormalNotice() );
	
	m_idxAnnounceLetter = 0;
	m_flowState = FS_INIT;
}

void CMenuBar::_DestroyFlowAnnounce(void)
{
	if (m_pCalculateText)
	{
		g_pFramework->getMessagBoxLayer()->RemoveChild(m_pCalculateText);
		I3_SAFE_RELEASE(m_pCalculateText);
	}
}

void CMenuBar::_UpdateFlowAnnounce(REAL32 rDeltaSeconds)
{	
	// 2가지 버전을 시험중입니다. - praptor
	//  1. i3GuiEditBox자체를 이동시키는 방법
	//  2. SetTextOffset을 이용해 한글자씩 팝/푸시하면서 당기는 방법
	//
	// 따라서 참조하지 않거나, 의미가 조금 다른 데이터멤버가 있습니다. 삭제하지 말아주세요.
	// 현재 양쪽다 공지 흐름에 '움찔거리는' 느낌이 있습니다.
	//
	//  3. 2개의 i3GuiEditBox를 이용해 상호 Filp시키며 이동시키는 방법을 고려중입니다.	

	switch(m_flowState)
	{
	case FS_INIT:
		if(m_bFlowTime > 120.0f)
		{
			m_bFCMFlag = FALSE;
			m_bFlowFlag = FALSE;
			m_bFlowTime = 0.0f;
			i3String::Copy(m_strAnnounceBuffer2, m_strAnnounceBuffer, MAX_STRING_COUNT );
		}
		if(TRUE  == m_bFCMFlag)	
		{
			//m_bFCMFlag = FALSE;
			m_pAnnounce->SetText(m_strAnnounceBuffer2);			
		}
		else
		{
			m_pAnnounce->SetText(m_strAnnounceBuffer);
		}

		m_nMoveOffset = m_pAnnounce->getTextNodeDX()->getTextWidth();
		m_rCurrentOffset = (REAL32)i3GuiRoot::getViewer()->GetViewWidth();
		m_pAnnounce->setPositionX(m_rCurrentOffset);
		m_flowState = FS_FLOW;
		break;
	case FS_FLOW:
		{
			m_rCurrentOffset -= 60.0f * rDeltaSeconds;
			m_pAnnounce->setPositionX(m_rCurrentOffset);
			if(m_bFlowFlag)
			{
				m_bFlowTime += rDeltaSeconds;
			}

			// 마이너스 m_nMoveOffset보다 작아지면
			if (-m_nMoveOffset > m_rCurrentOffset || -1 == m_idxAnnounceLetter)
			{
				m_idxAnnounceLetter = 0;
				m_flowState = FS_INIT;
			}
		}
		break;
	case FS_READ_LETTER:
		// Empty
		break;
	default:
		break;
	}
}

void CMenuBar::_UpdateAlarm(REAL32 rDeltaSeconds)
{
	if (m_bAlarmCommunity)
	{
		m_rAlarmTerm += rDeltaSeconds;

		if (0.25f < m_rAlarmTerm)
		{
			m_rAlarmTerm = 0.0f;
			m_bAlarmFlag = !m_bAlarmFlag;

			if (m_bAlarmFlag)
			{
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
			}
			else
			{
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
				m_pCommunity->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
			}
		}
	}
}

// 채팅창이 없는 씬에 있는 경우에는 경고창을 표시
void CMenuBar::_UpdateRoomMasterKickAlarm(void)
{	
#ifdef USE_ROOM_MASTER_KICK
	// 방장일 경우에만 참이 됨.
	if(TRUE == ((CStageReady*)m_pParent)->Get_bRoomMsterAwayCountDown())
	{		
		// 방장이 룸에서 다른 창으로 가는지 확인하기 위해 사용
		if(STEP_NULL == ((CStageReady*)m_pParent)->GetCurrentStepType())
		{
			if(TRUE == m_bRoomMsterKickAlarmFlag)	m_bRoomMsterKickAlarmFlag = FALSE;
			return;
		}
		// 방장 강퇴 경고창의 확인 버튼을 누른 경우인지 검사
		if(FALSE == g_pFramework->IsMessageBoxEnabled() &&	TRUE == m_bRoomMsterKickAlarmFlag)	return;
		// 방장 강퇴 경고창이 처음으로 팝업될 경우 팝업 Flag를 활성화한다.
		if(FALSE == m_bRoomMsterKickAlarmFlag)	m_bRoomMsterKickAlarmFlag = TRUE;
		// 방장 강퇴 경고창에서 강퇴 시간을 알림
		if ((INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f) <= ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime())// && (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_TIME + AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION) > m_RoomMasterAwayCheckerTime)
		{			
			if(m_nCurrentCountDown == ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime())	return;
			m_nCurrentCountDown = ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime();

			char countdownText[MAX_STRING_COUNT];
			if(m_pMessageBox)
			{
				g_pFramework->CloseMessageBox(m_pMessageBox);
				m_pMessageBox = NULL;
			}
			i3String::Format(countdownText, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_READYROOM_NOTICE_PRE_ROOM_MSTER_KICK_ALARM"), ((CStageReady*)m_pParent)->GetRoomMasterAwayCheckerTime() - (INT32)(AWAY_INPUT_ROOM_MASTER_GAMESTART_CAUTION - 1.0f));/*{col:255,120,0,255}방장 강퇴 %d초 전{/col}*/
			m_pMessageBox = STRING_POPUP(countdownText);
		}
	}
	else
	{
		// 방장 강퇴가 취소되었을 경우 경고창 팝업 Flag를 초기화한다.
		if(TRUE == m_bRoomMsterKickAlarmFlag)	m_bRoomMsterKickAlarmFlag = FALSE;
		// 방장 강퇴 카운트 시간을 초기화한다.
		if(0 != m_nCurrentCountDown)	m_nCurrentCountDown = 0;
		// 방장 강퇴 경고창이 떠 있는 경우 창을 닫는다.
		if(m_pMessageBox)
		{
			g_pFramework->CloseMessageBox(m_pMessageBox);
			m_pMessageBox = NULL;
		}		
	}
#endif
}

i3GuiButton* CMenuBar::GetEffectButton()
{
	return m_pShop;
}

void CMenuBar::UpdateButton()
{
	CStageReady * pReady = ((CStageReady*)m_pParent);
	bool bMouseOnProfile = false;

	if(m_pShop->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)
	{
		g_ShopState = FALSE;
		m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET+1);
	}
	else if(m_pProfile->getControlState() == I3GUI_CONTROL_STATE_ONMOUSE)
	{
		bMouseOnProfile = true;
		m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET+1);
	}
	else
	{
		if(g_FirstShop)
			g_ShopState = TRUE;
		else
			g_ShopState = FALSE;
	}
	
	if(pReady->GetButtonEffect())
	{
		if(g_ShopState)
		{
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
		if(g_pGameContext->IsShowMissionButtonEffect() && bMouseOnProfile == false)
		{
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
		}
	}
	else
	{
		if(g_ShopState)
		{
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pShop->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
		if(g_pGameContext->IsShowMissionButtonEffect() && bMouseOnProfile == false)
		{
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
			m_pProfile->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
		}
	}
}

void CMenuBar::EnterNickSpace()	
{
	m_bNickSpace = true;	
}

void CMenuBar::OutroNickSpace()	
{	
	m_bNickSpace = false;	
}