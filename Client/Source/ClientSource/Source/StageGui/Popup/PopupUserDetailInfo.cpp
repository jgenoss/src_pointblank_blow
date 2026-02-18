#include "pch.h"
#include "GlobalVariables.h"
#include "PopupUserDetailInfo.h"
#include "CommunityContext.h"

I3_CLASS_INSTANCE( CPopupUserDetailInfo, CPopupBase);

CPopupUserDetailInfo::CPopupUserDetailInfo(void)
{
	INT32 Loop = 0;

	m_pRoot = NULL;

	m_pCaption = NULL;

	// UserDetailInfoPopupFrame
	// User Info
	m_pRankMark = NULL;
	m_pNicknameValue = NULL;
	m_pRank = NULL;
	m_pClanName = NULL;	
	m_pTitle = NULL;	
	m_pPoint = NULL;
	m_pToll = NULL;

	m_pPositionKey = NULL;
	m_pPositionValue = NULL;

	// UserDetailInfoPopupFrame2
	// Total Record
	m_pTotalRecordKey = NULL;
	m_pTotalRecord = NULL;
	m_pTotalKillDeath = NULL;
	m_pTotalHeadshot = NULL;
	m_pTotalDis = NULL;
	m_pTotalUserRanking = NULL;

	// Season Record
	m_pSeasonRecordKey = NULL;
	m_pSeasonRecord = NULL;
	m_pSeasonKillDeath = NULL;
	m_pSeasonHeadshot = NULL;
	m_pSeasonDis = NULL;
	m_pSeasonUserRanking = NULL;

	// UserDetailInfoPopupFrame3	
	for(Loop = 0; Loop < 2; Loop++) {	m_pRecordTab[Loop] = NULL; }	// 통산, 시즌 tab
	m_bRecordTabFlag = FALSE;

	m_pModeTypeStatic = NULL;
	m_pModeTypeCombo = NULL;
	m_pModeTypeButton = NULL;
	m_pModeTypePopup = NULL;

	m_pModeRecordValue = NULL;
	m_pModeKillDeath = NULL;
	m_pModeHeadshot = NULL;
	m_pModeHitPercent = NULL;

	m_nModeTypeComboIdx = 0;

	// [Button]
	// Button
	for(Loop = 0; Loop < 2; Loop++) { m_pButton[Loop] = NULL; }

	m_CurrentUser.Reset();
	m_bDone = FALSE;

	m_bUserDetailInfoFlag = FALSE;	

	//UserDetailInfoPopupMessageBox
	m_pUserDetailInfoPopupMsg = NULL;
	m_pUserDetailInfoPopupMsgCaption = NULL;
	m_pUserDetailInfoPopupMsgMessage = NULL;	
	m_pUserDetailInfoPopupMsgConfirm = NULL;
	m_pUserDetailInfoPopupMsgCancel = NULL;

	m_pClose = NULL;
}

CPopupUserDetailInfo::~CPopupUserDetailInfo(void)
{
}

BOOL CPopupUserDetailInfo::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "UserDetailInfoPopup", m_pRoot, -1);

	LinkControlEx(plist,  "UserDetailInfoPopupCaption", m_pCaption, -1);
		
	// UserDetailInfoPopupFrame
	// User Info	
	LinkControlEx(plist,  "UserDetailInfoPopupNicknameValue", m_pNicknameValue, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupClanName", m_pClanName, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupTitle", m_pTitle, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupRankValue", m_pRank, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupRank", m_pRankMark, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupPoint", m_pPoint, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupToll", m_pToll, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupPositionKey", m_pPositionKey, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupPositionValue", m_pPositionValue, -1);
	
	// UserDetailInfoPopupFrame2
	// Season Record
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonRecordKey", m_pSeasonRecordKey, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonRecord", m_pSeasonRecord, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonKillDeath", m_pSeasonKillDeath, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonHeadshot", m_pSeasonHeadshot, -1);	
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonDis", m_pSeasonDis, -1);	
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonUserRanking", m_pSeasonUserRanking, -1);	

	// Total Record
	LinkControlEx(plist,  "UserDetailInfoPopupTotalRecordKey", m_pTotalRecordKey, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupTotalRecord", m_pTotalRecord, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupTotalKillDeath", m_pTotalKillDeath, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupTotalHeadshot", m_pTotalHeadshot, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupTotalDis", m_pTotalDis, -1);	
	LinkControlEx(plist,  "UserDetailInfoPopupTotalUserRanking", m_pTotalUserRanking, -1);	

	// UserDetailInfoPopupFrame3
	// Mode Record
	LinkControlEx(plist,  "UserDetailInfoPopupModeRecord", m_pModeRecordValue, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupModeKillDeath", m_pModeKillDeath, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupModeHeadshot", m_pModeHeadshot, -1);
	LinkControlEx(plist,  "UserDetailInfoPopupModeAccuracyRate", m_pModeHitPercent, -1);

	LinkControlEx(plist,  "UserDetailInfoPopupTotalTab", m_pRecordTab[0], GCI_USER_INFO_B_MODE_TOTAL_TAB);
	LinkControlEx(plist,  "UserDetailInfoPopupSeasonTab", m_pRecordTab[1], GCI_USER_INFO_B_MODE_SEASON_TAB);

	LinkControlEx(plist, "UserDetailInfoPopupModeTypeComboCaption", m_pModeTypeStatic, -1);
	LinkControlEx(plist, "UserDetailInfoPopupModeTypeCombo", m_pModeTypeCombo, -1);
	LinkControlEx(plist, "UserDetailInfoPopupModeTypeButton", m_pModeTypeButton, GCI_USER_INFO_B_MODE_TYPE);
	LinkControlEx(plist, "UserDetailInfoPopupModeTypePopup", m_pModeTypePopup, GCI_USER_INFO_P_MODE_TYPE_POPUP);

	// Button
	LinkControlEx(plist,  "UserDetailInfoPopupInvite", m_pButton[0], GCI_USER_INFO_B_ADD_FRIEND);
	LinkControlEx(plist,  "UserDetailInfoPopupDetail", m_pButton[1], GCI_USER_INFO_B_USER_DETAIL_INFO);
	LinkControlEx(plist,  "UserDetailInfoPopupClose", m_pClose, GCI_USER_INFO_B_CLOSE);

	// UserDetailInfoPopupMessageBox
	LinkControlEx(plist, "UserDetailInfoPopupMsg", m_pUserDetailInfoPopupMsg, -1);
	LinkControlEx(plist, "UserDetailInfoPopupMsgCaption", m_pUserDetailInfoPopupMsgCaption, -1);
	LinkControlEx(plist, "UserDetailInfoPopupMsgMessage", m_pUserDetailInfoPopupMsgMessage, -1);	
	LinkControlEx(plist, "UserDetailInfoPopupMsgConfirm", m_pUserDetailInfoPopupMsgConfirm, GCI_USER_INFO_B_USER_DETAIL_INFO_CONFIRM);
	LinkControlEx(plist, "UserDetailInfoPopupMsgCancel", m_pUserDetailInfoPopupMsgCancel, GCI_USER_INFO_B_USER_DETAIL_INFO_CANCEL);

	return FALSE;
}

void CPopupUserDetailInfo::OnInitControl(void)
{
	m_pRoot->SetEnable(FALSE);

	INIT_WIN_CAPTION( m_pCaption, GAME_STRING("STR_POPUP_DETAIL_INFO"));	//"상세정보"

	// UserDetailInfoPopupFrame
	// User Info
	GameGUI::SetRankStatic(m_pRankMark);
	m_pRankMark->SetEnable(FALSE);	
	INIT_VALUE_CAPTION( m_pNicknameValue, "");
	INIT_VALUE_CAPTION( m_pRank, "");
	INIT_VALUE_CAPTION( m_pClanName, "");
	INIT_VALUE_CAPTION( m_pTitle, "");
	INIT_VALUE_CAPTION( m_pPoint, "");
	INIT_VALUE_CAPTION( m_pToll, "");

	INIT_STATIC_CAPTION( m_pPositionKey, GAME_STRING("STR_POPUP_CUR_POSITION"));	//	"현재위치"
	INIT_VALUE_CAPTION( m_pPositionValue, "");
	
	// UserDetailInfoPopupFrame2
	// Total Record	
	INIT_STATIC_CAPTION( m_pTotalRecordKey, GAME_STRING("STR_POPUP_TOTAL_HISTORY"));	//	"통산전적"
	INIT_VALUE_CAPTION( m_pTotalRecord, "");
	INIT_VALUE_CAPTION( m_pTotalKillDeath, "");
	INIT_VALUE_CAPTION( m_pTotalHeadshot, "");
	INIT_VALUE_CAPTION( m_pTotalDis, "");
	INIT_VALUE_CAPTION( m_pTotalUserRanking, "");

	// Season Record
	INIT_STATIC_CAPTION( m_pSeasonRecordKey, GAME_STRING("STR_POPUP_SEASON_HISTORY"));	//	"시즌전적"
	INIT_VALUE_CAPTION( m_pSeasonRecord, "");
	INIT_VALUE_CAPTION( m_pSeasonKillDeath, "");
	INIT_VALUE_CAPTION( m_pSeasonHeadshot, "");
	INIT_VALUE_CAPTION( m_pSeasonDis, "");
	INIT_VALUE_CAPTION( m_pSeasonUserRanking, "");

	// UserDetailInfoPopupFrame3	
	INIT_CAPTION_TAB(m_pRecordTab[0], GAME_STRING("STR_POPUP_TOTAL_HISTORY"));
	INIT_CAPTION_TAB(m_pRecordTab[1], GAME_STRING("STR_POPUP_SEASON_HISTORY"));
	m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_bRecordTabFlag = FALSE;

	INIT_STATIC_CAPTION(m_pModeTypeStatic, "");
	INIT_WIN_CAPTION(m_pModeTypeCombo, GAME_STRING("STBL_IDX_CLAN_ALL_MEMBER"));/*전체*/
	INIT_COMBO_EX(m_pModeTypeButton);

	//	팀 구성 팝업
	m_pModeTypePopup->CreateTextEx(STAGE_TYPE_ANNIHILATION,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pModeTypePopup->SetSelColor(100, 100, 100, 100);
	m_pModeTypePopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pModeTypePopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pModeTypePopup->SetExclusiveEnableControl(TRUE);

	for(int i = 0; i < MODE_RECORD_MAX; i++)
	{
		switch(i)
		{
		case MODE_DEATHMATCH:
			{
				m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_TEAM_DEATH_MATCH"));/*단체전*/	//	break;
			}
			break;
		case MODE_ANNIHILATION:
			{
				m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_ANNIHILATION"));/*섬멸전*/	//	break;
			}
			break;
		case MODE_BOMB:
			{
				m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_EXPLOSION"));/*폭파미션*/	//break;
			}
			break;
		case MODE_ETC:
			{
				//m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_DESTROY"));/*파괴미션*/	break;
				//m_pModeTypePopup->AddElement(GAME_STRING("STR_HEADER_MODE_BOSS"));/*보스미션*/	break;
				m_pModeTypePopup->AddElement("etc");/*기타*///	break;
			}
			break;
		}
	}

	m_nModeTypeComboIdx = 0;
	m_pModeTypePopup->SetSelectedElement(m_nModeTypeComboIdx, FALSE);
	m_pModeTypePopup->SetEnable(FALSE);

	INIT_VALUE_CAPTION( m_pModeRecordValue,"");
	INIT_VALUE_CAPTION( m_pModeKillDeath,  "");
	INIT_VALUE_CAPTION( m_pModeHeadshot,   "");
	INIT_VALUE_CAPTION( m_pModeHitPercent, "");

	// [Button]
	// Button
	INIT_CAPTION_BUTTON(m_pButton[0], GAME_STRING("STR_POPUP_ADD_FRIEND"));								//  "친구추가"	
	INIT_CAPTION_BUTTON(m_pButton[1], GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN_VIEW"));	//	"미확인 정보보기"
	INIT_CAPTION_BUTTON(m_pClose, GAME_STRING("STR_POPUP_CLOSE"));										//	"닫기"

	m_pPositionKey->SetEnable( FALSE);
	m_pPositionValue->SetEnable( FALSE);

	m_bUserDetailInfoFlag = FALSE;	

	// UserDetailInfoPopupMessageBox
	INIT_WIN_CAPTION(m_pUserDetailInfoPopupMsgCaption, GAME_STRING("STR_TBL_PROFILE_NOTICE_TEXT"));			//알림 /*상세정보보기 정보이용료 결재창*/	
	char Msg[MAX_STRING_COUNT];
	//Msg[0] = NULL;
	#ifdef USE_MODE_RECORD
	sprintf_s(Msg, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_BILLING"), SUBTRACTE_POINT);	
	#endif
	INIT_VALUE_CAPTION( m_pUserDetailInfoPopupMsgMessage,Msg);		/*상대방의 상세정보를 보기위해 SUBTRACTE_POINT 포인트가 차감됩니다. 확인하시겠습니까?*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoPopupMsgConfirm, GAME_STRING("STR_POPUP_OK"));/*확인*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoPopupMsgCancel, GAME_STRING("STR_POPUP_CANCEL"));/*취소*/
	m_pUserDetailInfoPopupMsgMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pUserDetailInfoPopupMsgMessage->SetTextColor(GetColor(GCT_DEFAULT));

	m_pUserDetailInfoPopupMsg->SetEnable(FALSE);
}

void CPopupUserDetailInfo::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	/*switch( event )
	{
		
	}*/
}

void CPopupUserDetailInfo::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{    
	switch(pNotify->m_nID) 
	{		
		case GCI_USER_INFO_B_USER_DETAIL_INFO:	
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfo();
			break;
		case GCI_USER_INFO_B_USER_DETAIL_INFO_CONFIRM:	
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfoPupupMsgConfirm();
			break;
		case GCI_USER_INFO_B_USER_DETAIL_INFO_CANCEL:	
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfoPupupMsgCancel();
			break;
		case GCI_USER_INFO_B_ADD_FRIEND:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnAddFriend();
			break;
		case GCI_USER_INFO_B_CLOSE:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnExitKey();
			break;
		case GCI_USER_INFO_B_MODE_TOTAL_TAB:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnModeTab(FALSE);
			break;
		case GCI_USER_INFO_B_MODE_SEASON_TAB:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnModeTab(TRUE);
			break;
		case GCI_USER_INFO_B_MODE_TYPE:
			if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnTypeComboButton();
			break;		
		case GCI_USER_INFO_P_MODE_TYPE_POPUP:
			if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTypeComboPopup(pNotify->m_nPriParam);
			break;
	}
}

void CPopupUserDetailInfo::OnOpenPopup(void * pArg1, void * pArg2)
{
	m_bRecordTabFlag = FALSE;

	CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

	if( pArg1)
	{
		i3mem::Copy( &m_CurrentUser, (LOBBY_USER_INFO_LIST*) pArg1, sizeof( LOBBY_USER_INFO_LIST));
	}

	#ifdef USE_MODE_RECORD
	ACTIVATE_BUTTON( m_pButton[1]);
	_SetRecord(NULL);
	#endif

	m_pRoot->SetEnable(TRUE);

	m_nModeTypeComboIdx = 0;
	char * pszSelect = (char*) m_pModeTypePopup->GetText(0);
	m_pModeTypeCombo->SetText( pszSelect);

	//	친구 버튼
	{
		BOOL bFriend = FALSE;

		for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
			//if(NULL == node) continue;

			if (0 == i3String::Compare(m_CurrentUser._Nick, node->_nick))
			{
				bFriend = TRUE;
				break;
			}
		}

		if (g_pGameContext->IsMyNickName(m_CurrentUser._Nick))
		{
			bFriend = TRUE;
			DEACTIVATE_BUTTON( m_pButton[1]);			
			m_bUserDetailInfoFlag = TRUE;
			#ifdef USE_MODE_RECORD
			g_pCommunity->SetUserDetailInfoFlag(TRUE);
			g_pCommunity->SetUserDetailInfoResult(TRUE);
			#endif
		}

		if( bFriend)
		{
			DEACTIVATE_BUTTON( m_pButton[0]);
		}
		else
		{
			ACTIVATE_BUTTON( m_pButton[0]);
		}
	}

	m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);	

	#ifdef USE_MODE_RECORD
	// 열려있는 유저 정보 상세보기 창의 갱신을 위해
	g_pCommunity->SetCommnuitUserInfoBasic3()->_ui32Rank = m_CurrentUser._Rank;	
	#endif
}

void CPopupUserDetailInfo::OnClosePopup(void * pArg1, void * pArg2)
{		
	m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;
	#ifdef USE_MODE_RECORD
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	// 상세 정보 보기 확인창을 닫는다.	
	m_pUserDetailInfoPopupMsg->SetEnable(FALSE);
	#endif
	ACTIVATE_BUTTON( m_pButton[1]);

	CPopupBase::OnClosePopup();

	m_pRoot->SetEnable(FALSE);

	m_CurrentUser.Reset();
	m_bDone = FALSE;	
}

#ifdef USE_MODE_RECORD
void CPopupUserDetailInfo::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	//if( !m_bDone)
	{
		if( i3String::Compare( g_pCommunity->GetCommnunityInfoNick(), m_CurrentUser._Nick) == 0)
		{
			const USER_INFO_ALL_RECORD* pRecord = g_pCommunity->GetCommunityAllRecord();			
			if(TRUE == g_pCommunity->GetUserDetailInfoFlag())
			{
				m_bUserDetailInfoFlag = TRUE;
				g_pCommunity->SetUserDetailInfoFlag(FALSE);
			}
			_SetRecord( pRecord);			
			m_bDone = TRUE;
		}

		if(FALSE == g_pCommunity->GetUserDetailInfoResult())
		{
			ACTIVATE_BUTTON( m_pButton[1]);
		}
	}
}
#else
void CPopupUserDetailInfo::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	//if( !m_bDone)
	{
		if( i3String::Compare( g_pCommunity->GetCommnunityInfoNick(), m_CurrentUser._Nick) == 0)
		{
			#ifdef USE_MODE_RECORD
			const USER_INFO_RECORD* pRecord = g_pCommunity->GetCommunityRecord();
			DEACTIVATE_BUTTON( m_pButton[1]);			
			_SetRecord( pRecord);
			#endif

			m_bDone = TRUE;
		}
	}
}
#endif

void CPopupUserDetailInfo::OnPreGuiNotify(void)
{
	
}

void CPopupUserDetailInfo::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupUserDetailInfo::OnModeTab(BOOL bModeFlag)
{
	if(m_bRecordTabFlag == bModeFlag) return;

	m_bRecordTabFlag = bModeFlag;

	if (FALSE == m_bRecordTabFlag)
	{		
		m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}
	else
	{
		m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	}

	m_bRecordTabFlag = bModeFlag;
}

void CPopupUserDetailInfo::OnTypeComboButton(void)
{
	ExclusionPopup(m_pModeTypePopup);
}

void CPopupUserDetailInfo::OnTypeComboPopup(INT32 idx)
{
	m_nModeTypeComboIdx = idx; 
	char * pszSelect = (char*) m_pModeTypePopup->GetText(idx);
	m_pModeTypeCombo->SetText( pszSelect);
}


//////////////////////////////////////////////////////////////////////////
void CPopupUserDetailInfo::OnUserDetailInfo(void)
{
	m_pUserDetailInfoPopupMsg->SetEnable(TRUE);
}

void CPopupUserDetailInfo::OnUserDetailInfoPupupMsgCancel(void)
{
	// 상세 정보 보기 확인창을 닫는다.	
	m_pUserDetailInfoPopupMsg->SetEnable(FALSE);
}

void CPopupUserDetailInfo::OnUserDetailInfoPupupMsgConfirm(void)
{
	// 상세 정보 보기 확인창을 닫는다.	
	m_pUserDetailInfoPopupMsg->SetEnable(FALSE);

	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);
	
	if (SUBTRACTE_POINT > MyUserInfoBasic._point)
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_NO_POINT"));		//	"포인트가 부족합니다."
		return;
	}

	if( m_CurrentUser._Session <= 0)	return;	

	DEACTIVATE_BUTTON( m_pButton[1]);

	// 자기 자신이면 자신의 정보 표시, 아니면 게임서버에 정보 요청
	if( i3String::Compare( MyUserInfoBasic._nick, m_CurrentUser._Nick) == 0)
	{
		m_bUserDetailInfoFlag = TRUE;		
	}
	else
	{
		g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_ALL_N, &m_CurrentUser._Session);
	}						
}

void CPopupUserDetailInfo::OnAddFriend(void)
{
	char nick[MAX_STRING_COUNT];
	
	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_NO_MORE_FRIEND"));		//	"더이상 친구를 추가할 수 없습니다."
		return;
	}

	if( m_CurrentUser._Session <= 0)		
		return;

	i3String::Copy( nick, m_CurrentUser._Nick, MAX_STRING_COUNT );

	g_pCommunity->InsertFriend(nick);
}

#ifdef USE_MODE_RECORD
void CPopupUserDetailInfo::_SetRecord( const USER_INFO_ALL_RECORD * pRecord)
{
	if( pRecord != NULL)
	{
		INT32 headshotPercent = 0;
		INT32 hitPercent = 0;

		// User Info
		char rank[MAX_STRING_COUNT];
		char clan_name[MAX_STRING_COUNT];
		char title[MAX_STRING_COUNT];
		char point[MAX_STRING_COUNT];
		char cash[MAX_STRING_COUNT];

		// Record
		char str_record[MAX_STRING_COUNT];
		char record[MAX_STRING_COUNT] = "";
		char all_record[MAX_STRING_COUNT];
		char killDeath[MAX_STRING_COUNT];
		char headshot[MAX_STRING_COUNT];
		char dis_count[MAX_STRING_COUNT];
		char str_user_ranking[MAX_STRING_COUNT];
		char user_ranking[MAX_STRING_COUNT];

		// Mode Record
		char mode_match[MAX_STRING_COUNT] = "";
		char mode_killDeath[MAX_STRING_COUNT];
		char mode_headshot[MAX_STRING_COUNT];
		char mode_hit_count[MAX_STRING_COUNT];

		sprintf_s(clan_name, "%s %s", GAME_STRING("STBL_IDX_CLAN_NAME"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(title, "%s :  -  ,  -  ,  -  ", GAME_STRING("STR_TBL_DESIGNATION_TEXT"));
		sprintf_s(point, "%s : %s", GAME_STRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_POINT_TEXT"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(cash, "%s : %s", GAME_STRING("STR_SHOP_CASH"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));

		sprintf_s(str_record, "%s%s : ", GAME_STRING("STBL_IDX_BATTLE_WIN_COUNT"),GAME_STRING("STBL_IDX_BATTLE_LOSE_COUNT"));
		//sprintf_s(killDeath, "%s : ", GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE2"));
		//sprintf_s(headshot, "%s : ", GAME_STRING("STR_TBL_PROFILE_HEADSHOT_TEXT"));
		//sprintf_s(dis_count, "%s", GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"));
		sprintf_s(str_user_ranking, "UserRanking : ");

		sprintf_s(mode_match, "%s", GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(mode_killDeath, "%s : %s", GAME_STRING("STR_TBL_PROFILE_KILL_DEATH_RATE2"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(mode_headshot, "%s : %s", GAME_STRING("STR_TBL_PROFILE_HEADSHOT_TEXT"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
		sprintf_s(mode_hit_count, "%s : %s", GAME_STRING("STR_POPUP_HIT_RATE"), GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));

		m_pRankMark->SetEnable(TRUE);
		m_pNicknameValue->SetEnable(TRUE);
		m_pRank->SetEnable(TRUE);
		m_pClanName->SetEnable(TRUE);	
		m_pTitle->SetEnable(TRUE);	
		m_pPoint->SetEnable(TRUE);
		m_pToll->SetEnable(TRUE);
		//m_pPositionKey->SetEnable(TRUE);
		//m_pPositionValue->SetEnable(TRUE);
		// Total Record
		m_pTotalRecord->SetEnable(TRUE);
		m_pTotalKillDeath->SetEnable(TRUE);
		m_pTotalHeadshot->SetEnable(TRUE);
		m_pTotalDis->SetEnable(TRUE);
		m_pTotalUserRanking->SetEnable(TRUE);
		// Season Record
		m_pSeasonRecord->SetEnable(TRUE);
		m_pSeasonKillDeath->SetEnable(TRUE);
		m_pSeasonHeadshot->SetEnable(TRUE);
		m_pSeasonDis->SetEnable(TRUE);
		m_pSeasonUserRanking->SetEnable(TRUE);	
		// Mode Record
		m_pModeRecordValue->SetEnable(TRUE);
		m_pModeKillDeath->SetEnable(TRUE);
		m_pModeHeadshot->SetEnable(TRUE);
		//m_pModeHitPercent->SetEnable(TRUE);

		// User Info
		UINT32 ui32Rank = g_pCommunity->GetCommunityUserInfoBasic3()->_ui32Rank;
		m_pNicknameValue->SetTextEllipsis((char*)g_pCommunity->GetCommnunityInfoNick());		

		sprintf_s(rank, "%s : %s", GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_RANK"), GetCharaRank(ui32Rank));
		
		m_pRank->SetText(rank);
		m_pRankMark->SetShapeUVIndex(ui32Rank);

		if (TRUE == m_bUserDetailInfoFlag)
		{
			// User Info
			//	클랜명
			sprintf_s(clan_name, "%s%s", GAME_STRING("STBL_IDX_CLAN_NAME"), g_pCommunity->GetCommunityUserInfoBasic3()->_clanName);
			// 호칭
			INT32	Loop = 0;
			char	EquipUserTitle[MAX_EQUIP_USER_TITLE][256];
			for(Loop = 0; Loop < MAX_EQUIP_USER_TITLE; Loop++)
			{
				EquipUserTitle[Loop][0] = ' ';
				EquipUserTitle[Loop][1] = ' ';
				EquipUserTitle[Loop][2] = '-';
				EquipUserTitle[Loop][3] = ' ';
				EquipUserTitle[Loop][4] = ' ';
				EquipUserTitle[Loop][5] = '\0';	
			}

			for(Loop = 0; Loop < MAX_EQUIP_USER_TITLE; Loop++)
			{
				if(0 < g_pCommunity->GetCommunityUserInfoBasic3()->_EquipUserTitle[Loop])
				{
					GetUserTitleName(g_pCommunity->GetCommunityUserInfoBasic3()->_EquipUserTitle[Loop], EquipUserTitle[Loop]);
				}
			}

			sprintf_s(title, "%s : %s, %s, %s", GAME_STRING("STR_TBL_DESIGNATION_TEXT"), EquipUserTitle[0], EquipUserTitle[1], EquipUserTitle[2]);
			// 포인트
			sprintf_s(point, "%s : %d", GAME_STRING("STR_TBL_PROFILE_CHALLENGE_PRIZE_POINT_TEXT"), g_pCommunity->GetCommunityUserInfoBasic3()->_ui32Point);
			// 톨
			sprintf_s(cash, "%s : %d", GAME_STRING("STR_SHOP_CASH"), g_pCommunity->GetCommunityUserInfoBasic3()->_ui32cash);

			m_pClanName->SetText(clan_name);
			m_pTitle->SetText(title);
			m_pPoint->SetText(point);
			m_pToll->SetText(cash);
		}
		else
		{
			// User Info
			m_pClanName->SetText(clan_name);
			m_pTitle->SetText(title);
			m_pPoint->SetText(point);
			m_pToll->SetText(cash);
		}

		// 시즌 기록
		{
			headshotPercent = 0;

			//	"승패 : %d전 %d승 %d패"
			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_basic_record._season_record._match, pRecord->_basic_record._season_record._win, pRecord->_basic_record._season_record._lose);
			sprintf_s(all_record, "%s%s", str_record, record);			

			//	"KILL/DEATH: %d/%d"
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_basic_record._season_record._killcount, pRecord->_basic_record._season_record._death, g_pGameContext->getKillRate(pRecord->_basic_record._season_record._killcount, pRecord->_basic_record._season_record._death));

			if (0 == pRecord->_basic_record._season_record._dkillcount)
			{
				headshotPercent = 0;
			}
			else
			{
				headshotPercent = (INT32)(pRecord->_basic_record._season_record._headshot * 100.0f / pRecord->_basic_record._season_record._dkillcount);
			}

			//	"HEADSHOT: %d%%"
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);
			sprintf_s(dis_count, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_basic_record._season_record._dis);
			if (TRUE == m_bUserDetailInfoFlag)
			{
				if(0 >= pRecord->_basic_record._season_record._user_ranking)
				{
					sprintf_s(user_ranking, "%s  -  ", str_user_ranking);
				}
				else
				{
					sprintf_s(user_ranking, "%s%d", str_user_ranking, pRecord->_basic_record._season_record._user_ranking);
				}
			}
			else
			{
				sprintf_s(user_ranking, "%s%s", str_user_ranking, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
			}

			m_pSeasonRecord->SetText(all_record);
			m_pSeasonKillDeath->SetText(killDeath);
			m_pSeasonHeadshot->SetText(headshot);
			m_pSeasonDis->SetText(dis_count);
			m_pSeasonUserRanking->SetText(user_ranking);			
		}

		// 토탈 기록
		{
			headshotPercent = 0;			

			//	"승패 : %d전 %d승 %d패"
			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_basic_record._total_record._match, pRecord->_basic_record._total_record._win, pRecord->_basic_record._total_record._lose);
			sprintf_s(all_record, "%s%s", str_record, record);

			//	"KILL/DEATH: %d/%d"
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_basic_record._total_record._killcount, pRecord->_basic_record._total_record._death, g_pGameContext->getKillRate(pRecord->_basic_record._total_record._killcount, pRecord->_basic_record._total_record._death));

			if (0 == pRecord->_basic_record._total_record._dkillcount)
			{
				headshotPercent = 0;
			}
			else
			{
				headshotPercent = (INT32)(pRecord->_basic_record._total_record._headshot * 100.0f / pRecord->_basic_record._total_record._dkillcount);
			}

			//	"HEADSHOT: %d%%"
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);
			sprintf_s(dis_count, GAME_STRING("STR_TBL_GUI_READY_BATTLE_LEAVE_RATE"), pRecord->_basic_record._total_record._dis);			
			if (TRUE == m_bUserDetailInfoFlag)
			{
				if(0 >= pRecord->_basic_record._season_record._user_ranking)
				{
					sprintf_s(user_ranking, "%s  -  ", str_user_ranking);
				}
				else
				{
					sprintf_s(user_ranking, "%s%d", str_user_ranking, pRecord->_basic_record._total_record._user_ranking);
				}
			}
			else
			{
				sprintf_s(user_ranking, "%s%s", str_user_ranking, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN"));
			}

			m_pTotalRecord->SetText(all_record);
			m_pTotalKillDeath->SetText(killDeath);
			m_pTotalHeadshot->SetText(headshot);
			m_pTotalDis->SetText(dis_count);
			m_pTotalUserRanking->SetText(user_ranking);
		}    

		// Mode Record
		if (TRUE == m_bUserDetailInfoFlag)
		{
			headshotPercent = 0;
			hitPercent = 0;

			if (FALSE == m_bRecordTabFlag)
			{				
				sprintf_s(mode_match, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dmatch, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._win, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._lose);
				sprintf_s(all_record, "%s%s", str_record, mode_match);
				sprintf_s(mode_killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dkillcount, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._death, g_pGameContext->getKillRate(pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._killcount, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._death));

				if (0 == pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dkillcount)
				{
					headshotPercent = 0;
				}
				else
				{
					headshotPercent = (INT32)(pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._headshot * 100.0f / pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dkillcount);
				}
				sprintf_s(mode_headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);

				if (0 == pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._shoot_count)
				{
					hitPercent = 0;
				}
				else
				{
					hitPercent = (INT32)(pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._hit_count * 100.0f / pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._shoot_count);
				}				
				sprintf_s(mode_hit_count, "%s : %d", GAME_STRING("STR_POPUP_HIT_RATE"), hitPercent);
			}
			else
			{// 시즌 전적
				sprintf_s(mode_match, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dmatch, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._win, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._lose);
				sprintf_s(all_record, "%s%s", str_record, mode_match);
				sprintf_s(mode_killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dkillcount, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._death, g_pGameContext->getKillRate(pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._killcount, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._death));

				if (0 == pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dkillcount)
				{
					headshotPercent = 0;
				}
				else
				{
					headshotPercent = (INT32)(pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._headshot * 100.0f / pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dkillcount);
				}
				sprintf_s(mode_headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);

				if (0 == pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._shoot_count)
				{
					hitPercent = 0;
				}
				else
				{
					hitPercent = (INT32)(pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._hit_count * 100.0f / pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._shoot_count);
				}
				sprintf_s(mode_hit_count, "%s : %d", GAME_STRING("STR_POPUP_HIT_RATE"), hitPercent);
			}

			m_pModeRecordValue->SetText(all_record);
			m_pModeKillDeath->SetText(mode_killDeath);
			m_pModeHeadshot->SetText(mode_headshot);
			//m_pModeHitPercent->SetText(mode_hit_count);
		}		
		else
		{
			sprintf_s(all_record, "%s%s", str_record, mode_match);
			m_pModeRecordValue->SetText(all_record);
			m_pModeKillDeath->SetText(mode_killDeath);
			m_pModeHeadshot->SetText(mode_headshot);
			//m_pModeHitPercent->SetText(mode_hit_count);
		}
	}
	else
	{
		m_pRankMark->SetEnable(FALSE);
		m_pNicknameValue->SetEnable(FALSE);
		m_pRank->SetEnable(FALSE);
		m_pClanName->SetEnable(FALSE);	
		m_pTitle->SetEnable(FALSE);	
		m_pPoint->SetEnable(FALSE);
		m_pToll->SetEnable(FALSE);
		//m_pPositionKey->SetEnable(FALSE);
		//m_pPositionValue->SetEnable(FALSE);
		// Total Record
		m_pTotalRecord->SetEnable(FALSE);
		m_pTotalKillDeath->SetEnable(FALSE);
		m_pTotalHeadshot->SetEnable(FALSE);
		m_pTotalDis->SetEnable(FALSE);
		m_pTotalUserRanking->SetEnable(FALSE);
		// Season Record
		m_pSeasonRecord->SetEnable(FALSE);
		m_pSeasonKillDeath->SetEnable(FALSE);
		m_pSeasonHeadshot->SetEnable(FALSE);
		m_pSeasonDis->SetEnable(FALSE);
		m_pSeasonUserRanking->SetEnable(FALSE);
		// Mode Record
		m_pModeRecordValue->SetEnable(FALSE);
		m_pModeKillDeath->SetEnable(FALSE);
		m_pModeHeadshot->SetEnable(FALSE);
		//m_pModeHitPercent->SetEnable(FALSE);
	}	
}
#endif