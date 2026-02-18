#include "pch.h"
#include "GlobalVariables.h"
#include "PhaseClanBattleLobby.h"
#include "StageReady.h"

#define LIST_UPDATE_TIME	2.0f
static CLAN_BATTLE_TEAM_INFO * g_pTestMyTeamInfo = NULL;
static CLAN_BATTLE_MEMBER * g_pTestMyTeamMember = NULL;
static INT32 g_nTestMyTeamMemberCount = 0;
static CLAN_BATTLE_TEAM_INFO * g_pTestAllTeamInfo = NULL;
static INT32 g_nTestAllTeamInfoCount = 0;

I3_CLASS_INSTANCE( CStepClanBattleLobby, CStepBase);

CStepClanBattleLobby::CStepClanBattleLobby(void)
{
	m_pGui = NULL;

	m_pMyTeamWin = NULL;
	m_pMyTeamWinCaption = NULL;
	m_pMyTeamClanMarkImage = NULL;
	m_pMyTeamClanNameStatic = NULL;
	m_pMyTeamClanNameValue = NULL;
	m_pMyTeamClanRankStatic = NULL;
	m_pMyTeamClanRankValue = NULL;
	m_pMyTeamClanRankImage = NULL;
	m_pMyTeamNameValue = NULL;		
	m_pMyTeamChatText = NULL;
	m_pMyTeamChatScrollList = NULL;
	m_pMyTeamChatCombo = NULL;
	m_pMyTeamChatButton = NULL;
	m_pMyTeamChatPopup = NULL;
	m_pMyTeamChatEdit = NULL;
	m_pMyTeamChatLang = NULL;

	for(INT32 i=0; i<TEAM_MEMBER_SLOT_MAX; i++)
	{
		m_MyTeamMember[i]._pSlot = NULL;
		m_MyTeamMember[i]._pNick = NULL;
		m_MyTeamMember[i]._pRank = NULL;
		m_MyTeamMember[i]._pState = NULL;
	}

	m_pGuiTeamListWin = NULL;
	m_pTeamListCaption = NULL;
	m_pTeamBattleTypeValue = NULL;
	m_pTeamSearchStatic = NULL;
	m_pTeamSearchEdit = NULL;
	m_pTeamSearchButton = NULL;
	m_pTeamViewAll = NULL;
	m_pTeamQuckMatch = NULL;
	m_pTeamLeave = NULL;
	m_pTeamScrollList = NULL;

	m_pListHeadSlotNum = NULL;
	m_pListHeadClanRank = NULL;
	m_pListHeadClanName = NULL;
	m_pListHeadTeam = NULL;
	m_pListHeadState = NULL;

	for(INT32 i=0; i<BATTLE_TEAM_SLOT_MAX; i++)
	{
		m_BattleTeam[i]._pSlot = NULL;
		m_BattleTeam[i]._pClanMark = NULL;
		m_BattleTeam[i]._pClanName = NULL;
		m_BattleTeam[i]._pNumber = NULL;
		m_BattleTeam[i]._pRank = NULL;
		m_BattleTeam[i]._pTeamName = NULL;
		m_BattleTeam[i]._pState = NULL;
		m_BattleTeam[i]._pMatch = NULL;
	}

	m_idxTeamListStartSlot = 0;
	m_fUpdateTeamListTime= 0.f;
	m_nTeamListSection = 0;
	m_bUpdateTeamList = FALSE;			
}

CStepClanBattleLobby::~CStepClanBattleLobby(void)
{

}

void CStepClanBattleLobby::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_B_CLAN_BATTLE_LOBBY_QUICK_MATCH:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnQuickMatch();
		break;
	case GCI_B_CLAN_BATTLE_LOBBY_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_SCROLL:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnTeamListScroll();
		break;
	}
}

void CStepClanBattleLobby::OnGameEvent(INT32 event,INT32 arg)
{
	CStepBase::OnGameEvent(event, arg);

	switch(event)
	{
	case EVENT_MATCH_TEAM_INFO:	//	팀 맴버 정보 변경
		{
		}
		break;
	case EVENT_MATCH_TEAM_LEAVE:	//	팀 나가기
		{
			if( EV_SUCCESSED(arg))
			{
				//	다시 클랜전 페이지로
				((CStageReady*)m_pParent)->ChangeStep(STEP_CLAN_MEMBER);
			}
			else
			{
				STRING_POPUP("네트워크 문제로 나가기에 실패했습니다.\n 다시 시도해 주세요.");
			}
		}
		break;
	case EVENT_MATCH_TEAM_ALL_CONTEXT:		//	팀 리스트 컨텍스트 요청 결과
		{
			if( EV_SUCCESSED(arg))
			{
				m_fUpdateTeamListTime = 0.f;
				m_nTeamListSection = 0;
			}
			else
			{
				STRING_POPUP("네트워크 문제로 팀 리스트를 받을 수 없습니다.\n 다시 시도해 주세요.");
			}
		}
		break;
	case EVENT_MATCH_TEAM_ALL_LIST:			//	팀 리스트 정보 요청 결과
		{
			m_bUpdateTeamList = TRUE;
		}
		break;
	}
}

BOOL CStepClanBattleLobby::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char temp[MAX_STRING_COUNT] = "";

	LINK_STATIC_EX("ClanBattleLobby", m_pGui, -1);

	//	팀 정보 창	
	{
		LINK_STATIC_EX("clanBattleLobbyMyTeamWin", m_pMyTeamWin, GCI_UNKNOWN);

		LINK_EDIT_EX("clanBattleLobbyMyTeamWinCaption", m_pMyTeamWinCaption, GCI_UNKNOWN);

		LINK_STATIC_EX("clanBattleLobbyMyTeamClanMark", m_pMyTeamClanMarkImage, GCI_UNKNOWN);
		LINK_EDIT_EX("clanBattleLobbyMyTeamClanNameStatic", m_pMyTeamClanNameStatic, GCI_UNKNOWN);
		LINK_EDIT_EX("clanBattleLobbyMyTeamClanNameValue", m_pMyTeamClanNameValue, GCI_UNKNOWN);
		LINK_EDIT_EX("clanBattleLobbyMyTeamClanRankStatic", m_pMyTeamClanRankStatic, GCI_UNKNOWN);
		LINK_EDIT_EX("clanBattleLobbyMyTeamClanRankValue", m_pMyTeamClanRankValue, GCI_UNKNOWN);
		LINK_STATIC_EX("clanBattleLobbyMyTeamClanRankImage", m_pMyTeamClanRankImage, GCI_UNKNOWN);

		LINK_EDIT_EX("clanBattleLobbyMyTeamName", m_pMyTeamNameValue, GCI_UNKNOWN);

		for(INT32 i=0; i<TEAM_MEMBER_SLOT_MAX; i++)
		{
			sprintf(temp, "clanBattleLobbyMyTeamMemberSlot%d", i+1);
			LINK_STATIC_EX(temp, m_MyTeamMember[i]._pSlot, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyMyTeamMemberRank%d", i+1);
			LINK_STATIC_EX(temp, m_MyTeamMember[i]._pRank, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyMyTeamMemberNick%d", i+1);
			LINK_EDIT_EX(temp, m_MyTeamMember[i]._pNick, GCI_UNKNOWN);
		
			sprintf(temp, "clanBattleLobbyMyTeamMemberState%d", i+1);
			LINK_STATIC_EX(temp, m_MyTeamMember[i]._pState, GCI_UNKNOWN);
		}

		LINK_EDIT_EX("clanBattleLobbyMyTeamChatTextBox", m_pMyTeamChatText, GCI_UNKNOWN);
		LINK_LIST_EX("clanBattleLobbyMyTeamChatList", m_pMyTeamChatScrollList, GCI_UNKNOWN);

		LINK_EDIT_EX("clanBattleLobbyMyTeamChatCombo", m_pMyTeamChatCombo, GCI_UNKNOWN);
		LINK_POPUP_EX("clanBattleLobbyMyTeamChatPopup", m_pMyTeamChatPopup, GCI_UNKNOWN); 
		LINK_BUTTON_EX("clanBattleLobbyMyTeamChatButton", m_pMyTeamChatButton, GCI_UNKNOWN);
		
		LINK_EDIT_EX("clanBattleLobbyMyTeamChatBox", m_pMyTeamChatEdit, GCI_UNKNOWN);
		LINK_STATIC_EX("clanBattleLobbyMyTeamChatLang", m_pMyTeamChatLang, GCI_UNKNOWN);	
	}	

	//	팀 리스트 창
	{
		LINK_STATIC_EX("clanBattleLobbyTeamListWin", m_pGuiTeamListWin, GCI_UNKNOWN);

		LINK_EDIT_EX("clanBattleLobbyTeamListCaption", m_pTeamListCaption, GCI_UNKNOWN);

		LINK_EDIT_EX("clanBattleLobbyTeamListSearchStatic", m_pTeamSearchStatic, GCI_UNKNOWN);
		LINK_EDIT_EX("clanBattleLobbyTeamListSearchEdit", m_pTeamSearchEdit, GCI_UNKNOWN);
		LINK_BUTTON_EX("clanBattleLobbyTeamListSearchButton", m_pTeamSearchButton, GCI_B_CLAN_BATTLE_LOBBY_SEARCH);
		LINK_BUTTON_EX("clanBattleLobbyTeamListViewAll", m_pTeamViewAll, GCI_B_CLAN_BATTLE_LOBBY_VIEW_ALL);

		LINK_BUTTON_EX("clanBattleLobbyTeamListQuickButton", m_pTeamQuckMatch, GCI_B_CLAN_BATTLE_LOBBY_QUICK_MATCH);
		LINK_BUTTON_EX("clanBattleLobbyTeamListLeaveButton", m_pTeamLeave, GCI_B_CLAN_BATTLE_LOBBY_LEAVE);

		LINK_EDIT_EX("clanBattleLobbyTeamListTypeValue", m_pTeamBattleTypeValue, GCI_UNKNOWN);

		LINK_EDIT_EX("clanBattleLobbyTeamListHeadSlotNum", m_pListHeadSlotNum, GCI_S_CLAN_BATTLE_LOBBY_HEAD_NUM);
		LINK_EDIT_EX("clanBattleLobbyTeamListHeadClanRank", m_pListHeadClanRank, GCI_L_CLAN_BATTLE_LOBBY_HEAD_RANK);
		LINK_EDIT_EX("clanBattleLobbyTeamListHeadClanName", m_pListHeadClanName, GCI_L_CLAN_BATTLE_LOBBY_HEAD_NAME);
		LINK_EDIT_EX("clanBattleLobbyTeamListHeadTeam", m_pListHeadTeam, GCI_L_CLAN_BATTLE_LOBBY_HEAD_TEAM);
		LINK_EDIT_EX("clanBattleLobbyTeamListHeadState", m_pListHeadState, GCI_L_CLAN_BATTLE_LOBBY_HEAD_STATE);

		LINK_LIST_EX("clanBattleLobbyTeamListScrollList", m_pTeamScrollList, GCI_L_CLAN_BATTLE_LOBBY_SCROLL);

		for(INT32 i=0; i<BATTLE_TEAM_SLOT_MAX; i++)
		{
			sprintf(temp, "clanBattleLobbyTeamListSlot%d", i+1);
			LINK_STATIC_EX(temp, m_BattleTeam[i]._pSlot, GCI_UNKNOWN);
			
			sprintf(temp, "clanBattleLobbyTeamSlotNum%d", i+1);
			LINK_EDIT_EX(temp, m_BattleTeam[i]._pNumber, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyTeamClanName%d", i+1);
			LINK_EDIT_EX(temp, m_BattleTeam[i]._pClanName, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyTeamClanMark%d", i+1);
			LINK_STATIC_EX(temp, m_BattleTeam[i]._pClanMark, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyTeamName%d", i+1);
			LINK_EDIT_EX(temp, m_BattleTeam[i]._pTeamName, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyTeamClanRank%d", i+1);
			LINK_STATIC_EX(temp, m_BattleTeam[i]._pRank, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyTeamState%d", i+1);
			LINK_STATIC_EX(temp, m_BattleTeam[i]._pState, GCI_UNKNOWN);

			sprintf(temp, "clanBattleLobbyTeamMatchButton%d", i+1);
			LINK_BUTTON_EX(temp, m_BattleTeam[i]._pMatch, GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH1 + i);
		}
	}

	return FALSE;
}

void CStepClanBattleLobby::OnInitControl(void)
{
	m_pGui->SetEnable(FALSE);

	_InitMyTeam();
	_InitTeamList();
}

void CStepClanBattleLobby::OnPreGuiNotify(void)
{
}


void CStepClanBattleLobby::OnUpdate(REAL32 rDeltaSeconds)
{
	CStepBase::OnUpdate(rDeltaSeconds);

	_UpdateMyTeam(rDeltaSeconds);
	_UpdateTeamList(rDeltaSeconds);
}

void CStepClanBattleLobby::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			m_pTeamScrollList->MovePrev();
		}
	}
	else
	{
		for(INT32 i = 0; i > scrollby; i--)
		{
			m_pTeamScrollList->MoveNext();
		}
	}

	m_idxTeamListStartSlot = m_pTeamScrollList->GetCurrentTop();
	m_bUpdateTeamList = TRUE;
}

void CStepClanBattleLobby::SetInputDisable(BOOL bDisable)
{
	m_pGui->setInputDisable(bDisable);
}

BOOL CStepClanBattleLobby::GetInputDisable(void)
{
	return m_pGui->getInputDisable();
}

BOOL CStepClanBattleLobby::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
	}

	return CStepBase::OnEvent(event, pObj, param2, code);
}

void CStepClanBattleLobby::OnStart(void)
{
	CStepBase::OnStart();

	SetPushedStateButton(MBT_CLAN, TRUE);

	#if defined( TEST_CLAN_BATTLE)	
	INT32 nMemberCount = TEAM_MEMBER_SLOT_MAX;

	//	테스트용 네팀 정보
	if( g_pTestMyTeamInfo == NULL)
	{
		g_pTestMyTeamInfo = (CLAN_BATTLE_TEAM_INFO*) i3MemAlloc( sizeof(CLAN_BATTLE_TEAM_INFO));
		
		g_pTestMyTeamInfo->_Team._MasterIdx = 0;
		g_pTestMyTeamInfo->_Team._Naming = (UINT8) (rand() % 100 + 1);
		g_pTestMyTeamInfo->_Team._PerCount = (UINT8)nMemberCount;
		g_pTestMyTeamInfo->_Team._PerMax = (UINT8)nMemberCount;
		g_pTestMyTeamInfo->_Team._State = (UINT8)CLAN_ROOM_READY;
	}
	//	테스트용 네팀 맴버
	if( g_pTestMyTeamMember == NULL)
	{
		g_nTestMyTeamMemberCount = nMemberCount;
		g_pTestMyTeamMember = (CLAN_BATTLE_MEMBER*) i3MemAlloc( sizeof(CLAN_BATTLE_MEMBER) * nMemberCount);
		
		for(INT32 i=0; i<g_nTestMyTeamMemberCount; i++)
		{
			CLAN_BATTLE_MEMBER * pMember = g_pTestMyTeamMember + i;

			sprintf(pMember->_Name, "DUMMY - %d", i+1);
			pMember->_Rank = (UINT8) (rand() % 50);
			pMember->_State = (UINT8) (rand() % SLOT_STATE_READY + SLOT_STATE_SHOP);
			pMember->_DBIdx = i+1;
		}		
	}
	//	테스트용 상대팀 리스트
	if( g_pTestAllTeamInfo == NULL)
	{
		g_nTestAllTeamInfoCount = 100;
		g_pTestAllTeamInfo = (CLAN_BATTLE_TEAM_INFO*) i3MemAlloc(sizeof(CLAN_BATTLE_TEAM_INFO) * g_nTestAllTeamInfoCount);
		
		for(INT32 i=0; i<g_nTestAllTeamInfoCount; i++)
		{
			CLAN_BATTLE_TEAM_INFO * pTeam = g_pTestAllTeamInfo + i;

			pTeam->_Idx = i+1;

			sprintf(pTeam->_Clan._ClanName, "DUMMY CLAN (%d)", i+1);
			pTeam->_Clan._ClanIdx = i+1;
			pTeam->_Clan._ClanRank = (UINT8) (rand() % 50);
			pTeam->_Clan._ClanMark = MAKE_CLANMARK_FLAG( (rand() % CLAN_MARK_BACK_IMAGE_CNT + 1),
														 (rand() % CLAN_MARK_SHAPE_IMAGE_CNT + 1),
														 (rand() % CLAN_MARK_COLOR_MAX + 1),	
														 (rand() % CLAN_MARK_COLOR_MAX + 1));

			pTeam->_Team._Naming = (UINT8) (rand() % 100);
			pTeam->_Team._State = (UINT8) (rand() % CLAN_ROOM_PLAY + CLAN_ROOM_STAY);
			pTeam->_Team._PerMax = (UINT8) (rand() % 5 + 4);
			pTeam->_Team._PerCount = (UINT8) (rand() % pTeam->_Team._PerMax + 1);
			pTeam->_Team._MasterIdx = 0;
		}	
	}
	m_bUpdateTeamList = TRUE;
#else
	//	내가 속한 팀이 유효하지 않으므로 일반 로비로 나간다.
	if( !g_pFramework->GetClanContext()->InBattleTeam())
	{
		((CStageReady*)m_pParent)->MovePhase(PHASE_LOBBY);
		return;
	}
	else
	{
		//	팀 리스트 컨텍스트 요청
		g_pFramework->GetClanContext()->MatchTeamAllContext();
		g_pFramework->GetClanContext()->CreateBattleAllTeamList(MAX_BATTLE_ROOM_COUNT);
	}
	m_bUpdateTeamList = FALSE;
#endif

	m_pTeamScrollList->SetCurValue(0.0f);
	m_idxTeamListStartSlot = 0;
	m_fUpdateTeamListTime= 0.f;
	m_nTeamListSection = 0;	
}

void CStepClanBattleLobby::OnEnd(void)
{
	CStepBase::OnEnd();
	
	SetPushedStateButton(MBT_CLAN, TRUE);
	//SetPushedStateButton(MBT_CLAN, FALSE);

	g_pFramework->GetClanContext()->DeleteBattleAllTeamList();

#if defined( TEST_CLAN_BATTLE)
	I3MEM_SAFE_FREE( g_pTestMyTeamInfo);
	I3MEM_SAFE_FREE( g_pTestMyTeamMember);
	I3MEM_SAFE_FREE( g_pTestAllTeamInfo);
#endif
}

void CStepClanBattleLobby::OnInitRun(void)
{
	CStepBase::OnInitRun(); 
}

BOOL CStepClanBattleLobby::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CStepBase::OnSliding(rDeltaSeconds))
	{
		return FALSE;	// 변경사항이 없다.
	}
	
	// 등장/퇴장 시간에 따른 좌표 오프셋 얻어내기
	m_pGui->setPositionX(CStepBase::GetSlideX());	


	// 퇴장 완료 시 비활성화. 그외 활성화.
	BOOL enableWindow = TRUE;

	if (SS_OUT == CStepBase::GetSlideState())
	{
		enableWindow = FALSE;
	}
	
	m_pGui->SetEnable(enableWindow);

	return TRUE;
}

void CStepClanBattleLobby::OnTeamListScroll(void)
{
	m_idxTeamListStartSlot = m_pTeamScrollList->GetCurrentTop();	
	m_bUpdateTeamList = TRUE;
}

void CStepClanBattleLobby::OnQuickMatch(void)
{
}

void CStepClanBattleLobby::OnLeave(void)
{
	INT32 idx = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo()->_Idx;

	//	팀 나가기	
	g_pFramework->GetClanContext()->MatchTeamLeave(idx);
}

void CStepClanBattleLobby::_InitMyTeam(void)
{
	INIT_WIN_CAPTION(m_pMyTeamWinCaption, "팀정보");

	INIT_STATIC_CAPTION(m_pMyTeamClanNameStatic, "클랜명:");
	INIT_STATIC_CAPTION(m_pMyTeamClanNameValue, "INVALID");
	INIT_STATIC_CAPTION(m_pMyTeamClanRankStatic, "클랜 등급:");
	INIT_STATIC_CAPTION(m_pMyTeamClanRankValue, "INVALID");
	
	//	클랜 등급
	SetRankImage(m_pMyTeamClanRankImage, 0);
	m_pMyTeamClanRankValue->SetTextColor( GetColor(GCT_VALUE));

	//	팀 이름
	INIT_STATIC_CAPTION(m_pMyTeamNameValue, "INVALID");
	m_pMyTeamClanNameValue->SetTextColor( GetColor(GCT_VALUE));

	//	팀원 슬롯
	for(INT32 i=0; i<TEAM_MEMBER_SLOT_MAX; i++)
	{
		INIT_VALUE_CAPTION(m_MyTeamMember[i]._pNick, "");

		SetRankImage(m_MyTeamMember[i]._pRank, 0);

		m_MyTeamMember[i]._pState->SetAllShapeEnable(FALSE);

		m_MyTeamMember[i]._pSlot->SetEnable(FALSE);
	}

	//	채팅 화면
	m_pMyTeamChatText->ReCreateTextEx(DEFAULT_CHARA_SET, FONT_COUNT_2048, GAME_FONT_DEFAULT_SIZE, FALSE, NULL, FW_NORMAL, TRUE);	
	m_pMyTeamChatText->SetTextColor(GetColor(GCT_DEFAULT));	
	m_pMyTeamChatText->SetTextAutoWrap( TRUE);
	m_pMyTeamChatText->SetTextAlign( ALIGN_LEFT, ALIGN_BOTTOM);
	m_pMyTeamChatText->SetWordLineFeed( FALSE);
	
	REAL32 ry = g_pGUIRoot->getViewer()->GetViewHeight() * 0.13020833333333333333333333333333e-2f; // / I3GUI_RESOLUTION_HEIGHT;	
	m_pMyTeamChatText->SetTextSpace( 0, (INT32)(5 * ry));

    //	채팅 컴보
	INIT_STATIC_CAPTION(m_pMyTeamChatCombo, STR_TBL_CLAN(STBL_IDX_CLAN_CHAT_ALL));
	INIT_BUTTON_EX(m_pMyTeamChatButton);
	DEACTIVATE_BUTTON(m_pMyTeamChatButton);

	m_pMyTeamChatPopup->CreateTextEx(3, DEFAULT_CHARA_SET);
	m_pMyTeamChatPopup->SetSelColor(100, 100, 100, 100);
	m_pMyTeamChatPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMyTeamChatPopup->SetExclusiveEnableControl(TRUE);
	m_pMyTeamChatPopup->AddElement(STR_TBL_CLAN(STBL_IDX_CLAN_CHAT_ALL));	//	"전체"
	m_pMyTeamChatPopup->AddElement(STR_TBL_CLAN(STBL_IDX_CLAN_CHAT_TEAM));	//	"팀"
	m_pMyTeamChatPopup->AddElement(STR_TBL_CLAN(STBL_IDX_CLAN_CHAT_CLAN));	//	"클랜"
	m_pMyTeamChatPopup->SetEnable(FALSE);

	INIT_CAPTION(m_pMyTeamChatLang, STR_TBL_CLAN(STBL_IDX_CLAN_LANGUAGE_NATIVE));	//	"한"
	m_pMyTeamChatLang->SetCaptionColor(GetColor(GCT_FOCUS));

	//	채팅 스크롤바
	m_pMyTeamChatScrollList->CreateTextEx(10, DEFAULT_CHARA_SET, FONT_COUNT_256);
	m_pMyTeamChatScrollList->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMyTeamChatScrollList->SetSelColor(0, 0, 0, 0);
	m_pMyTeamChatScrollList->SetTextColor(GetColor(GCT_DEFAULT));
	
	for(UINT32 i = 0; i < 10; i++)
		m_pMyTeamChatScrollList->AddElement("");	
	
	//	채팅 입력창
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = ROOM_CHAT_SIZE;
	ime._StyleIME = 0;
	m_pMyTeamChatEdit->ReCreateTextEx(DEFAULT_CHARA_SET, FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime, FW_NORMAL, TRUE);
	m_pMyTeamChatEdit->setImeProcFlag(UPDATE_FLAG_CHANGE_LANGUAGE);
	m_pMyTeamChatEdit->SetTextAlign(ALIGN_LEFT, ALIGN_TOP);
	m_pMyTeamChatEdit->SetTextAutoWrap(FALSE);
	m_pMyTeamChatEdit->SetOffsetPos(0.0f, 2.0f);
	m_pMyTeamChatEdit->setCaretPosOffset(0.0f, 2.0f);	
	m_pMyTeamChatEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pMyTeamChatEdit->SetTextColor(GetColor(GCT_DEFAULT));
}

void CStepClanBattleLobby::_InitTeamList(void)
{
	INIT_WIN_CAPTION(m_pTeamListCaption, "클랜전 리스트");
	
	INIT_STATIC_CAPTION(m_pTeamSearchStatic, "클랜 찾기");

	//	검색 입력창
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pTeamSearchEdit->ReCreateTextEx(DEFAULT_CHARA_SET, FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime, FW_NORMAL, TRUE);
	m_pTeamSearchEdit->setImeProcFlag(UPDATE_FLAG_CHANGE_LANGUAGE);
	m_pTeamSearchEdit->SetTextAlign(ALIGN_LEFT, ALIGN_TOP);
	m_pTeamSearchEdit->SetTextAutoWrap(FALSE);
	m_pTeamSearchEdit->SetOffsetPos(0.0f, 2.0f);
	m_pTeamSearchEdit->setCaretPosOffset(0.0f, 2.0f);	
	m_pTeamSearchEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pTeamSearchEdit->SetTextColor(GetColor(GCT_DEFAULT));

	INIT_BUTTON_EX(m_pTeamSearchButton);
	INIT_CAPTION_BUTTON(m_pTeamViewAll, "전체보기");

	INIT_BUTTON_EX(m_pTeamQuckMatch);
	INIT_BUTTON_EX(m_pTeamLeave);

	m_pTeamBattleTypeValue->ReCreateTextEx(DEFAULT_CHARA_SET, FONT_COUNT_256, 12);
	m_pTeamBattleTypeValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTeamBattleTypeValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTeamBattleTypeValue->SetText("0 VS 0");

	INIT_WIN_CAPTION(m_pListHeadSlotNum, "번호");
	INIT_WIN_CAPTION(m_pListHeadClanRank, "등급");
	INIT_WIN_CAPTION(m_pListHeadClanName, "클랜명");
	INIT_WIN_CAPTION(m_pListHeadTeam, "팀");
	INIT_WIN_CAPTION(m_pListHeadState, "상태");

	m_pTeamScrollList->CreateTextEx(BATTLE_TEAM_SLOT_MAX, DEFAULT_CHARA_SET);
	m_pTeamScrollList->SetSelColor(0, 0, 0, 0); 

	for(INT32 i=0; i<BATTLE_TEAM_SLOT_MAX; i++)
	{
		INIT_WIN_CAPTION(m_BattleTeam[i]._pNumber, "000");

		m_BattleTeam[i]._pClanName->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_BattleTeam[i]._pClanName->SetTextColor(GetColor(GCT_VALUE));
		m_BattleTeam[i]._pClanName->SetTextEllipsis("INVALID");

		INIT_WIN_CAPTION(m_BattleTeam[i]._pTeamName, "INVALID");

		INIT_CAPTION_BUTTON(m_BattleTeam[i]._pMatch, "신청하기");

		m_BattleTeam[i]._pRank->SetAllShapeEnable(FALSE);

		m_BattleTeam[i]._pState->SetAllShapeEnable(FALSE);	
		m_BattleTeam[i]._pState->SetEnable(FALSE);

		m_BattleTeam[i]._pSlot->SetEnable(FALSE);
	}
}

void CStepClanBattleLobby::_UpdateMyTeam(REAL32 rDeltaSeconds)
{
	CLAN_BATTLE_TEAM_INFO * pMyTeam = NULL;
	CLAN_BATTLE_MEMBER * pMyMember = NULL;

	char szTemp[MAX_STRING_COUNT] = "";
	INT32 nTeamNaming = 0;
	INT32 nMemberCount = 0;
	
	//	클랜정보
	_UpdateClanInfo(rDeltaSeconds);

#if defined( TEST_CLAN_BATTLE)
	pMyTeam = g_pTestMyTeamInfo;
	pMyMember = g_pTestMyTeamMember;

	nTeamNaming = g_pTestMyTeamInfo->_Team._Naming;
	nMemberCount = g_nTestMyTeamMemberCount;
#else
	pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	pMyMember = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();

	nTeamNaming = pMyTeam->_Team._Naming;
	nMemberCount = pMyTeam->_Team._PerCount;
#endif

	//	맴버 정보
	{
		GetBattleTeamName( szTemp, nTeamNaming);
		m_pMyTeamNameValue->SetText( szTemp);

		for(INT32 i=0; i<TEAM_MEMBER_SLOT_MAX; i++)
		{
			if( i < nMemberCount)
				_SetMemberSlot(i, pMyMember + i);
			else
				_SetMemberSlot(i, NULL);
		}
	}
}

void CStepClanBattleLobby::_UpdateClanInfo(REAL32 rDeltaSeconds)
{
	CLAN_DETAIL_INFO * pClanInfo = g_pFramework->GetClanContext()->GetMyClanDetailInfo();

	//	클랜명
	m_pMyTeamClanNameValue->SetText(pClanInfo->_name);	

	//	클랜 등급
	SetClanRank(m_pMyTeamClanRankValue, m_pMyTeamClanRankImage, pClanInfo->_exp, pClanInfo->_currentPerson, pClanInfo->_expRank);

	//	클랜 마크
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(m_pMyTeamClanMarkImage, CLAN_MARK_MID, pClanInfo->_mark);
#endif
}

void CStepClanBattleLobby::_SetMemberSlot(INT32 idx, CLAN_BATTLE_MEMBER * pMember)
{
	BOUNDCHK( idx, TEAM_MEMBER_SLOT_MAX);

	MyTeamMemberSlot * pSlotInfo = &m_MyTeamMember[idx];
	if( pMember == NULL) {
		pSlotInfo->_pSlot->SetEnable(FALSE);
		return;
	}
	
	pSlotInfo->_pSlot->SetEnable(TRUE);
	pSlotInfo->_pState->SetAllShapeEnable(FALSE);

	pSlotInfo->_pNick->SetTextEllipsis( pMember->_Name);	//	닉네임
	SetRankImage(pSlotInfo->_pRank, pMember->_Rank);		//	계급

	//	맴버 현재 상태
	switch(pMember->_State)
	{
	case SLOT_STATE_INVENTORY:	pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);	break;
	case SLOT_STATE_INFO:		pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);	break;
	case SLOT_STATE_SHOP:		pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);	break;
	}
}

void CStepClanBattleLobby::_UpdateTeamList(RT_REAL32 rDeltaSeconds)
{
	CLAN_BATTLE_ALL_TEAM_CONTEXT* pAllTeamContext = NULL;
	CLAN_BATTLE_TEAM_INFO * pMyTeam = NULL;
	CLAN_BATTLE_MEMBER * pMyMember = NULL;
	CLAN_BATTLE_TEAM_INFO * pAllTeam = NULL;
	char szTemp[256] = "";
	INT32 nTeamCount = 0;

#if defined( TEST_CLAN_BATTLE)
	pAllTeamContext = NULL;
	pMyTeam = g_pTestMyTeamInfo;
	pMyMember = g_pTestMyTeamMember;
	pAllTeam = g_pTestAllTeamInfo;
	nTeamCount = g_nTestAllTeamInfoCount;
#else
	pAllTeamContext = g_pFramework->GetClanContext()->GetClanBattleAllTeamContext();
	pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	pMyMember = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();
	pAllTeam = g_pFramework->GetClanContext()->GetClanBattleAllTeamList();
	nTeamCount = pAllTeamContext->_totalcount;
	
	//	팀 리스트 업데이트
	if( nTeamCount > 0)
	{
		I3ASSERT( pAllTeamContext);

		m_fUpdateTeamListTime += rDeltaSeconds;

		//	팀 리스트를 받는다.
		if( m_fUpdateTeamListTime > LIST_UPDATE_TIME)
		{
			m_fUpdateTeamListTime = 0.f;

			g_pFramework->GetClanContext()->MatchTeamAllList(pMyTeam->_Team._PerMax, (UINT16) m_nTeamListSection);
			m_nTeamListSection++;

			if( m_nTeamListSection > pAllTeamContext->_sectionCount)
				m_nTeamListSection = 0;
		}
	}
#endif

	sprintf( szTemp, "%d VS %d", (INT32) pMyTeam->_Team._PerMax, (INT32) pMyTeam->_Team._PerMax);
	m_pTeamBattleTypeValue->SetText( szTemp);

	//	팀 리스트 셋팅
	if( m_bUpdateTeamList)
	{
		INT32 nSlot = 0;
		BOOL bMaster = FALSE;
		
#if defined( TEST_CLAN_BATTLE)
		bMaster = TRUE;
#else
		//	자신이 마스터인지 여부
		{
			INT32 idx = pMyTeam->_Team._MasterIdx;
			BOUNDCHK( idx, pMyTeam->_Team._PerCount);

			CLAN_BATTLE_MEMBER * pMasterInfo = pMyMember + idx;

			if( (INT32) pMasterInfo->_DBIdx == g_pFramework->GetClanContext()->GetMyDBIdx())
			{
				bMaster = TRUE;

				m_pTeamQuckMatch->SetEnable(TRUE);
			}
			else
			{
				m_pTeamQuckMatch->SetEnable(FALSE);
			}
		}
#endif

		m_pTeamScrollList->Clear();
		for(INT32 i=0; i<nTeamCount; i++)
		{
			CLAN_BATTLE_TEAM_INFO * pTeamInfo = pAllTeam + i;

			//	상태가 유효한 팀
			if( pTeamInfo->_Team._State != CLAN_ROOM_EMPTY)
			{
				m_pTeamScrollList->AddElement("");

				//	보여지는 슬롯 셋팅
				if( i >= m_idxTeamListStartSlot && nSlot < BATTLE_TEAM_SLOT_MAX)
				{
					_SetTeamListSlot(nSlot, pTeamInfo, bMaster);
					nSlot++;
				}
			}
		}

		//	빈 슬롯 처리
		if( nTeamCount < BATTLE_TEAM_SLOT_MAX) 
		{
			for(INT32 i = nSlot; i<BATTLE_TEAM_SLOT_MAX; i++)
				_SetTeamListSlot(i, NULL);
		}

		m_bUpdateTeamList = FALSE;
	}
}

void CStepClanBattleLobby::_SetTeamListSlot(INT32 idx, CLAN_BATTLE_TEAM_INFO * pTeamInfo, BOOL bMaster )
{
	BOUNDCHK( idx, BATTLE_TEAM_SLOT_MAX);

	char szTemp[256] = "";
	BattleTeamSlot * pSlotInfo = &m_BattleTeam[idx];

	if( pTeamInfo == NULL)
	{
		pSlotInfo->_pSlot->SetEnable(FALSE);
		return;
	}	

	pSlotInfo->_pSlot->SetEnable(TRUE);

	sprintf(szTemp, "%03d", pTeamInfo->_Idx);

	//	슬롯 번호
	pSlotInfo->_pNumber->SetText( szTemp);

	//	클랜 등급
	SetRankImage(pSlotInfo->_pRank, pTeamInfo->_Clan._ClanRank);

	//	클랜 마크
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(pSlotInfo->_pClanMark, CLAN_MARK_MID, pTeamInfo->_Clan._ClanMark);
#endif	

	//	클랜명
	pSlotInfo->_pClanName->SetTextEllipsis( pTeamInfo->_Clan._ClanName);

	//	팀명
	GetBattleTeamName(szTemp, pTeamInfo->_Team._Naming);
	pSlotInfo->_pTeamName->SetText( szTemp);
	
	pSlotInfo->_pState->SetEnable(FALSE);
	pSlotInfo->_pState->SetAllShapeEnable(FALSE);
	pSlotInfo->_pMatch->SetEnable(FALSE);

	switch( pTeamInfo->_Team._State)
	{
	//	신청 받을수 있는 상태
	case CLAN_ROOM_STAY:
		{
			pSlotInfo->_pMatch->SetEnable(TRUE);

			if( bMaster)
			{
				ACTIVATE_BUTTON(pSlotInfo->_pMatch);
			}
			else 
				DEACTIVATE_BUTTON(pSlotInfo->_pMatch);
		}
		break;
	//	매칭된 상태 (신청 받을수 없다)
	case CLAN_ROOM_READY:
	case CLAN_ROOM_PLAY:
		{
			pSlotInfo->_pState->SetEnable(TRUE);
			pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET+1, TRUE);
		}
		break;
	}
}

