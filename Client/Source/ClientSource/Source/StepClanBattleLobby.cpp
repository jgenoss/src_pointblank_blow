#include "pch.h"
#include "GlobalVariables.h"
#include "StepClanBattleLobby.h"
#include "StageReady.h"

#define LIST_UPDATE_TIME	2.0f

#if defined( TEST_CLAN_BATTLE)
static CLAN_BATTLE_TEAM_INFO * g_pTestMyTeamInfo = NULL;
static CLAN_BATTLE_MEMBER * g_pTestMyTeamMember = NULL;
static INT32 g_nTestMyTeamMemberCount = 0;
static CLAN_BATTLE_TEAM_INFO * g_pTestAllTeamInfo = NULL;
static INT32 g_nTestAllTeamInfoCount = 0;
#endif

CLAN_SORT_TYPE CStepClanBattleLobby::m_sTeamSortType = CLAN_SORT_NONE;

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
	m_pMyTeamClanPointStatic = NULL;
	m_pMyTeamClanPointValue = NULL;
	m_pMyTeamNameValue = NULL;		
	
	m_pMyTeamTypeStatic = NULL;
	m_pMyTeamTypeCombo = NULL;
	m_pMyTeamTypeComboButton = NULL;
	m_pMyTeamTypeComboPopup = NULL;

	i3mem::FillZero(m_MyTeamMember, sizeof(m_MyTeamMember));


	m_pGuiTeamListWin = NULL;
	m_pTeamListCaption = NULL;
	m_pTeamSearchStatic = NULL;
	m_pTeamSearchEdit = NULL;
	m_pTeamSearchButton = NULL;
	m_pTeamViewAll = NULL;
	m_pTeamQuickMatch = NULL;
	m_pTeamLeave = NULL;
	m_pTeamScrollList = NULL;

	m_pBattleTeamFocus = NULL;
	m_pBattleTeamSelect = NULL;

	m_pListHeadSlotNum = NULL;
	m_pListHeadClanRank = NULL;
	m_pListHeadClanName = NULL;
	m_pListHeadTeam = NULL;
	m_pListHeadType = NULL;
	m_pListHeadState = NULL;

	i3mem::FillZero(m_BattleTeam, sizeof(m_BattleTeam));

	m_pSelectClanInfoStatic = NULL;

	m_pSelectClanMarkImage = NULL;
	m_pSelectClanMasterRankImage = NULL;
	m_pSelectClanTeamLeaderRankImage = NULL;
	m_pSelectClanRankImage = NULL;
	m_pSelectClanExpBar = NULL;
	m_pSelectClanAddressBullet = NULL;

	m_pSelectClanNameStatic = NULL;
	m_pSelectClanNameValue = NULL;
	m_pSelectClanStaffStatic = NULL;
	m_pSelectClanStaffValue = NULL;
	m_pSelectClanMasterStatic = NULL;
	m_pSelectClanMasterValue = NULL;
	m_pSelectClanTeamLeaderStatic = NULL;
	m_pSelectClanTeamLeaderValue = NULL;
	m_pSelectClanRankStatic = NULL;
	m_pSelectClanRankValue = NULL;
	m_pSelectClanExpStatic = NULL;
	m_pSelectClanExpValue = NULL;
	m_pSelectClanOrderStatic = NULL;
	m_pSelectClanOrderValue = NULL;
	m_pSelectClanAddressStatic = NULL;
	m_pSelectClanAddressValue = NULL;		

	m_pBattleTeamInfo = NULL;
	m_nBattleTeamInfoCount = 0;

	m_szSearchKeyword[0] = 0;

	m_idxBattleTeamMatchType = 0;
	m_idxTeamListStartSlot = 0;
	m_nMatchTeamSlot = -1;
	m_nSelectTeamSlot = -1;
	m_fUpdateTeamListTime= 0.f;
	m_nTeamListSection = 0;
	m_bUpdateTeamList = FALSE;	

	m_sTeamSortType = CLAN_SORT_NONE;

	m_pChatBody = CGameChatBody::NewObject();
	m_pClanContext = NULL;
	m_pTeamBattleTypeValue = NULL;
}

CStepClanBattleLobby::~CStepClanBattleLobby(void)
{
	I3_SAFE_RELEASE( m_pChatBody);
	
	I3MEM_SAFE_FREE(m_pBattleTeamInfo);
	m_BattleTeamList.Clear();

#if defined( TEST_CLAN_BATTLE)
	I3MEM_SAFE_FREE( g_pTestMyTeamInfo);
	I3MEM_SAFE_FREE( g_pTestMyTeamMember);
	I3MEM_SAFE_FREE( g_pTestAllTeamInfo);
#endif
}

void CStepClanBattleLobby::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	//	클랜 리스트 소트 헤더 초기화
	{
		m_pListHeadSlotNum->SetTextColor( GetColor(GCT_DEFAULT));
		m_pListHeadClanRank->SetTextColor( GetColor(GCT_DEFAULT));
		m_pListHeadClanName->SetTextColor( GetColor(GCT_DEFAULT));
		m_pListHeadTeam->SetTextColor( GetColor(GCT_DEFAULT)); 
		m_pListHeadType->SetTextColor( GetColor(GCT_DEFAULT)); 
		m_pListHeadState->SetTextColor( GetColor(GCT_DEFAULT));
	}

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
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH1:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH2:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH3:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH4:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH5:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH6:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH7:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	
			OnTeamMatch( pNotify->m_nID - GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH1);
		break;	
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT1:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT2:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT3:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT4:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT5:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT6:
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT7:
		if( I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent)	
			OnTeamSelect( pNotify->m_nID - GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT1);
		else if( I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent)	
			OnTeamFocus( pNotify->m_nID - GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT1);
		break;
	case GCI_B_CLAN_BATTLE_LOBBY_SEARCH:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSearch();
		break;
	case GCI_E_CLAN_BATTLE_LOBBY_TEAM_TYPE_COMBO:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)		OnTeamTypeButton();
		break;
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_TYPE_COMBO:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTeamTypeButton();
		break;
	case GCI_P_CLAN_BATTLE_LOBBY_TEAM_TYPE_COMBO:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnTeamTypePopup(pNotify->m_nPriParam);
		break;
	case GCI_B_CLAN_BATTLE_LOBBY_VIEW_ALL:
		if( I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnViewAll();
		break;
	case GCI_S_CLAN_BATTLE_LOBBY_HEAD_NUM:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSortListByNumber();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_HEAD_RANK:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSortListByRank();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_HEAD_NAME:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSortListByName();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_HEAD_TEAM:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSortListByTeam();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_HEAD_TYPE:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSortListByType();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_HEAD_STATE:
		if( I3GUI_EDIT_NOTIFY_CLICKED == pNotify->m_nEvent)	OnSortListByState();
		break;
	case GCI_L_CLAN_BATTLE_LOBBY_TEAM_CHAT_LIST:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent)			m_pChatBody->OnDragChatList();
		break;
	case GCI_B_CLAN_BATTLE_LOBBY_TEAM_CHAT_COMBO:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnChatCombo();
		break;
	case GCI_P_CLAN_BATTLE_LOBBY_TEAM_CHAT:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnChatPopup(pNotify->m_nPriParam);
		break;
	case GCI_P_CLAN_BATTLE_LOBBY_CHAT_CHANNEL_TYPE1:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(0);
		break;
	case GCI_P_CLAN_BATTLE_LOBBY_CHAT_CHANNEL_TYPE2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(1);
		break;
	case GCI_P_CLAN_BATTLE_LOBBY_CHAT_CHANNEL_TYPE3:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(2);
		break;
	case GCI_P_CLAN_BATTLE_LOBBY_CHAT_CHANNEL_TYPE4:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	m_pChatBody->OnChatChannel(3);
		break;
	}
}

void CStepClanBattleLobby::OnGameEvent(INT32 event,INT32 arg)
{
	CStepBase::OnGameEvent(event, arg);

	switch(event)
	{
	case EVENT_MATCH_TEAM_INFO_N:	//	팀 맴버 정보 변경
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
				switch(arg)
				{
				case EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_EMPTY_FAIL:
				case EVENT_ERROR_CLAN_BATTLE_TEAM_LEAVE_NOUSER_FAIL:
				default:
					//	"네트워크 문제로 나가기에 실패했습니다.\n 다시 시도해 주세요."
					STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_FAIL_OUT_TEAM"));
					break;
				}
			}
		}
		break;
	case EVENT_MATCH_TEAM_ALL_LIST_N:			//	팀 리스트 정보 요청 결과
		{
			if( EV_SUCCESSED(arg))
			{
				//	리스트 구성
				_BuildBattleTeamList();

				m_bUpdateTeamList = TRUE;
			}
		}
		break;	
	case EVENT_MATCH_TEAM_DETAIL_INFO:			//	상대 클랜 정보 요청
		{
			/*if( EV_SUCCESSED(arg))
			{
				CLAN_DETAIL_INFO * pInfo = g_pGameContext->getClanDetailInfo();
				_SetSelectClanInfo( pInfo);
			}
			else
			{
				_SetSelectClanInfo(NULL);
			}*/
		}
		break;	
	}
}

BOOL CStepClanBattleLobby::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char temp[MAX_STRING_COUNT] = "";
	
	LinkControlEx(plist, "ClanBattleLobby", m_pGui, -1);

	//	팀 정보 창	
	{
		LinkControlEx(plist, "clanBattleLobbyMyTeamWin", m_pMyTeamWin, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyMyTeamWinCaption", m_pMyTeamWinCaption, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyMyTeamClanMark", m_pMyTeamClanMarkImage, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanNameStatic", m_pMyTeamClanNameStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanNameValue", m_pMyTeamClanNameValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanRankStatic", m_pMyTeamClanRankStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanRankValue", m_pMyTeamClanRankValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanRankImage", m_pMyTeamClanRankImage, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanPointStatic", m_pMyTeamClanPointStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamClanPointValue", m_pMyTeamClanPointValue, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyMyTeamName", m_pMyTeamNameValue, GCI_UNKNOWN);

		for(INT32 i=0; i<TEAM_MEMBER_SLOT_MAX; i++)
		{
			sprintf_s(temp, "clanBattleLobbyMyTeamMemberSlot%d", i+1);
			LinkControlEx(plist, temp, m_MyTeamMember[i]._pSlot, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyMyTeamMemberRank%d", i+1);
			LinkControlEx(plist, temp, m_MyTeamMember[i]._pRank, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyMyTeamMemberNick%d", i+1);
			LinkControlEx(plist, temp, m_MyTeamMember[i]._pNick, GCI_UNKNOWN);
		
			sprintf_s(temp, "clanBattleLobbyMyTeamMemberState%d", i+1);
			LinkControlEx(plist, temp, m_MyTeamMember[i]._pState, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyMyTeamMemberLeader%d", i+1);
			LinkControlEx(plist, temp, m_MyTeamMember[i]._pLeader, GCI_UNKNOWN);
		}

		LinkControlEx(plist, "clanBattleLobbyMyTeamChatTextBox", m_pChatBody->m_pTextBox, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamChatList", m_pChatBody->m_pChatList, GCI_L_CLAN_BATTLE_LOBBY_TEAM_CHAT_LIST);

		LinkControlEx(plist, "clanBattleLobbyMyTeamChatCombo", m_pChatBody->m_pChatCombo, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamChatPopup", m_pChatBody->m_pChatPopup, GCI_P_CLAN_BATTLE_LOBBY_TEAM_CHAT); 
		LinkControlEx(plist, "clanBattleLobbyMyTeamChatButton", m_pChatBody->m_pChatButton, GCI_B_CLAN_BATTLE_LOBBY_TEAM_CHAT_COMBO);
		
		LinkControlEx(plist, "clanBattleLobbyMyTeamChatBox", m_pChatBody->m_pChatBox, GCI_UNKNOWN);	// ChatEdit
		LinkControlEx(plist, "clanBattleLobbyMyTeamChatLang", m_pChatBody->m_pChatLang, GCI_UNKNOWN);	

		for(INT32 i=0; i<CHAT_CHANNEL_BUTTON_COUNT; i++)
		{
			sprintf_s(temp, "clanBattleLobbyMyTeamChatTab%d", i + 1);

			LinkControlEx(plist, temp, m_pChatBody->m_pChatChannelButton[i], GCI_P_CLAN_BATTLE_LOBBY_CHAT_CHANNEL_TYPE1+i);
		}

		LinkControlEx(plist, "clanBattleLobbyMyTeamTypeStatic", m_pMyTeamTypeStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyMyTeamTypeCombo", m_pMyTeamTypeCombo, GCI_E_CLAN_BATTLE_LOBBY_TEAM_TYPE_COMBO);
		LinkControlEx(plist, "clanBattleLobbyMyTeamTypeComboButton", m_pMyTeamTypeComboButton, GCI_B_CLAN_BATTLE_LOBBY_TEAM_TYPE_COMBO);
		LinkControlEx(plist, "clanBattleLobbyMyTeamTypeComboPopup", m_pMyTeamTypeComboPopup, GCI_P_CLAN_BATTLE_LOBBY_TEAM_TYPE_COMBO);
		
	}	

	//	팀 리스트 창
	{
		LinkControlEx(plist, "clanBattleLobbyTeamListWin", m_pGuiTeamListWin, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyTeamListCaption", m_pTeamListCaption, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyTeamListSearchStatic", m_pTeamSearchStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyTeamListSearchEdit", m_pTeamSearchEdit, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyTeamListSearchButton", m_pTeamSearchButton, GCI_B_CLAN_BATTLE_LOBBY_SEARCH);
		LinkControlEx(plist, "clanBattleLobbyTeamListViewAll", m_pTeamViewAll, GCI_B_CLAN_BATTLE_LOBBY_VIEW_ALL);

		LinkControlEx(plist, "clanBattleLobbyTeamListQuickButton", m_pTeamQuickMatch, GCI_B_CLAN_BATTLE_LOBBY_QUICK_MATCH);
		LinkControlEx(plist, "clanBattleLobbyTeamListLeaveButton", m_pTeamLeave, GCI_B_CLAN_BATTLE_LOBBY_LEAVE);

		LinkControlEx(plist, "clanBattleLobbyTeamListHeadSlotNum", m_pListHeadSlotNum, GCI_S_CLAN_BATTLE_LOBBY_HEAD_NUM);
		LinkControlEx(plist, "clanBattleLobbyTeamListHeadClanRank", m_pListHeadClanRank, GCI_L_CLAN_BATTLE_LOBBY_HEAD_RANK);
		LinkControlEx(plist, "clanBattleLobbyTeamListHeadClanName", m_pListHeadClanName, GCI_L_CLAN_BATTLE_LOBBY_HEAD_NAME);
		LinkControlEx(plist, "clanBattleLobbyTeamListHeadTeam", m_pListHeadTeam, GCI_L_CLAN_BATTLE_LOBBY_HEAD_TEAM);
		LinkControlEx(plist, "clanBattleLobbyTeamListHeadType", m_pListHeadType, GCI_L_CLAN_BATTLE_LOBBY_HEAD_TYPE);		
		LinkControlEx(plist, "clanBattleLobbyTeamListHeadState", m_pListHeadState, GCI_L_CLAN_BATTLE_LOBBY_HEAD_STATE);

		LinkControlEx(plist, "clanBattleLobbyTeamListFocus", m_pBattleTeamFocus, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyTeamListSelect", m_pBattleTeamSelect, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyTeamListScrollList", m_pTeamScrollList, GCI_L_CLAN_BATTLE_LOBBY_SCROLL);

		for(INT32 i=0; i<BATTLE_TEAM_SLOT_MAX; i++)
		{
			sprintf_s(temp, "clanBattleLobbyTeamListSlot%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pSlot, GCI_UNKNOWN);
			
			sprintf_s(temp, "clanBattleLobbyTeamSlotNum%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pNumber, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamClanName%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pClanName, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamClanMark%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pClanMark, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamName%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pTeamName, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamType%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pTeamType, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamClanRank%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pRank, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamState%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pState, GCI_UNKNOWN);

			sprintf_s(temp, "clanBattleLobbyTeamMatchButton%d", i+1);	// Match Join Button
			LinkControlEx(plist, temp, m_BattleTeam[i]._pMatch, GCI_B_CLAN_BATTLE_LOBBY_TEAM_MATCH1 + i);

			sprintf_s(temp, "clanBattleLobbyTeamFilter%d", i+1);
			LinkControlEx(plist, temp, m_BattleTeam[i]._pFilter, GCI_B_CLAN_BATTLE_LOBBY_TEAM_SLOT1 + i);			
		}

		LinkControlEx(plist, "clanBattleLobbyTeamListInfoStatic", m_pSelectClanInfoStatic, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyClanInfoMark", m_pSelectClanMarkImage, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoMasterRankImage", m_pSelectClanMasterRankImage, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoTeamLeaderRankImage", m_pSelectClanTeamLeaderRankImage, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoRankImage", m_pSelectClanRankImage, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoExpBar", m_pSelectClanExpBar, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoAddressBullet", m_pSelectClanAddressBullet, GCI_UNKNOWN);

		LinkControlEx(plist, "clanBattleLobbyClanInfoNameStatic", m_pSelectClanNameStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoNameValue", m_pSelectClanNameValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoStaffStatic", m_pSelectClanStaffStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoStaffValue", m_pSelectClanStaffValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoRankStatic", m_pSelectClanRankStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoMasterStatic", m_pSelectClanMasterStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoMasterValue", m_pSelectClanMasterValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoTeamLeaderStatic", m_pSelectClanTeamLeaderStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoTeamLeaderValue", m_pSelectClanTeamLeaderValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoRankValue", m_pSelectClanRankValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoExpStatic", m_pSelectClanExpStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoExpValue", m_pSelectClanExpValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoOrderStatic", m_pSelectClanOrderStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoOrderValue", m_pSelectClanOrderValue, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoAddressStatic", m_pSelectClanAddressStatic, GCI_UNKNOWN);
		LinkControlEx(plist, "clanBattleLobbyClanInfoAddressValue", m_pSelectClanAddressValue, GCI_UNKNOWN);	
	}

	return FALSE;
}

void CStepClanBattleLobby::OnInitControl(void)
{
	CStepBase::OnInitControl();

	m_pGui->SetEnable(FALSE);

	//	채팅 초기화
	m_pChatBody->InitChatBody( CHAT_BODY_STATE_CLAN_MATCH, MAX_CHAT_LINE_FOR_TEAM_LOBBY);

	_InitMyTeam();
	_InitTeamList();
}

void CStepClanBattleLobby::OnPreGuiNotify(void)
{
	m_pBattleTeamFocus->SetEnable(FALSE);
}

void CStepClanBattleLobby::OnUpdate(REAL32 rDeltaSeconds)
{
	CStepBase::OnUpdate(rDeltaSeconds);

	_UpdateMyTeam(rDeltaSeconds);
	_UpdateTeamList(rDeltaSeconds);
	_UpdateSelectClan( rDeltaSeconds);

	//	신청받은 팀 응답 처리
	g_pFramework->GetClanContext()->UpdateClanBattleTeam();

	I3ASSERT( m_pChatBody != NULL);
	m_pChatBody->OnUpdate(rDeltaSeconds, this);
}

void CStepClanBattleLobby::OnWheelUpdate(INT32 scrollby)
{
	if (0 < scrollby)
	{
		for(INT32 i = 0; i < scrollby; i++)
		{
			if( m_pTeamScrollList->getIsFocus())
			{
				m_pTeamScrollList->MovePrev();
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
			if( m_pTeamScrollList->getIsFocus())
			{
				m_pTeamScrollList->MoveNext();
			}
			else if( m_pChatBody->m_pChatList->getIsFocus())
			{
				m_pChatBody->m_pChatList->MoveNext();
			}
		}
	}

	m_idxTeamListStartSlot = m_pTeamScrollList->GetCurrentTop();
	m_bUpdateTeamList = TRUE;
	
	//	선택되어 있는 팀 슬롯
	if((m_nSelectTeamSlot == -1) || 
		(m_nSelectTeamSlot - m_idxTeamListStartSlot < 0) || (m_nSelectTeamSlot - m_idxTeamListStartSlot >= BATTLE_TEAM_SLOT_MAX))
	{
		m_pBattleTeamSelect->SetEnable(FALSE);
	}
	else
	{
		m_pBattleTeamSelect->SetEnable(TRUE);
		m_pBattleTeamSelect->setPositionY( m_nSelectTeamSlot * 27.0f + 1.0f);
	}

	if( m_pChatBody->m_pChatList->getIsFocus())
	{
		g_pFramework->getChatBox()->CalcBackupBufferByRatio(m_pChatBody->m_pChatList->getCurValue()*0.01f, MAX_CHAT_LINE_FOR_TEAM_LOBBY);	
	}
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
		if( event == I3_EVT_INPUT)
		{
			i3InputDeviceManager * pMng = (i3InputDeviceManager*) pObj;
			i3InputKeyboard * pKeyboard = pMng->GetKeyboard();
			
			//	클랜 검색일때
			if( m_pTeamSearchEdit->getIsFocus())
			{
				if(pKeyboard->GetStrokeState(I3I_KEY_ENTER))
				{
					OnSearch();
					return TRUE;
				}
			}

			//	채팅 처리
			if( m_pGui->isEnable() &&
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

	return CStepBase::OnEvent(event, pObj, param2, code);
}

void CStepClanBattleLobby::OnStart(void)
{
	CStepBase::OnStart();

	SetPushedStateButton(MBT_CLAN, TRUE);
	
	m_pTeamScrollList->SetCurValue(0.0f);
	m_idxTeamListStartSlot = 0;
	m_idxBattleTeamMatchType = 0;
	m_nMatchTeamSlot = -1;
	m_nSelectTeamSlot = -1;
	m_fUpdateTeamListTime= 0.f;
	m_nTeamListSection = 0;	

	m_pTeamSearchEdit->SetText("");

	m_sTeamSortType = CLAN_SORT_NUMBER_ASC;
	m_BattleTeamList.Clear();
	m_BattleTeamList.SetCompareProc(cbSortTeamListByNumber);
	I3MEM_SAFE_FREE(m_pBattleTeamInfo);
	m_nBattleTeamInfoCount =0;
	m_szSearchKeyword[0] = 0;

#if defined( TEST_CLAN_BATTLE)	
	INT32 nMemberCount = TEAM_MEMBER_SLOT_MAX;

	//	테스트용 네팀 정보
	if( g_pTestMyTeamInfo == NULL)
	{
		g_pTestMyTeamInfo = (CLAN_BATTLE_TEAM_INFO*) i3MemAlloc( sizeof(CLAN_BATTLE_TEAM_INFO));
		i3mem::FillZero( g_pTestMyTeamInfo, sizeof(CLAN_BATTLE_TEAM_INFO));
		
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
		i3mem::FillZero( g_pTestMyTeamMember, sizeof(CLAN_BATTLE_MEMBER) * nMemberCount);
		
		for(INT32 i=0; i<g_nTestMyTeamMemberCount; i++)
		{
			CLAN_BATTLE_MEMBER * pMember = g_pTestMyTeamMember + i;

			sprintf_s(pMember->_Name, "DUMMY - %d", i+1);
			pMember->_Rank = (UINT8) (rand() % 50);
			pMember->_State = (UINT8) (rand() % SLOT_STATE_READY + SLOT_STATE_SHOP);
			pMember->_i64UID = i+1;
		}		
	}
	//	테스트용 상대팀 리스트
	if( g_pTestAllTeamInfo == NULL)
	{
		g_nTestAllTeamInfoCount = 100;
		g_pTestAllTeamInfo = (CLAN_BATTLE_TEAM_INFO*) i3MemAlloc(sizeof(CLAN_BATTLE_TEAM_INFO) * g_nTestAllTeamInfoCount);
		i3mem::FillZero( g_pTestAllTeamInfo, sizeof(CLAN_BATTLE_TEAM_INFO) * g_nTestAllTeamInfoCount);
		
		for(INT32 i=0; i<g_nTestAllTeamInfoCount; i++)
		{
			CLAN_BATTLE_TEAM_INFO * pTeam = g_pTestAllTeamInfo + i;

			pTeam->_Idx = (UINT16)(i+1);

			sprintf_s(pTeam->_Clan._ClanName, "DUMMY CLAN (%d)", i+1);
			pTeam->_Clan._ClanIdx = i+1;
			pTeam->_Clan._ClanRank = (UINT8) (rand() % 50);
			pTeam->_Clan._ClanMark = MAKE_CLANMARK_FLAG( (rand() % CClanMarkManager::GetMarkBackImageCount() + 1),
														 (rand() % CClanMarkManager::GetMarkShapeImageCount() + 1),
														 (rand() % CClanMarkManager::GetMarkColorCount() + 1),	
														 (rand() % CClanMarkManager::GetMarkColorCount() + 1));

			pTeam->_Team._Naming = (UINT8) (rand() % 100);
			pTeam->_Team._State = (UINT8) (rand() % CLAN_ROOM_PLAY + CLAN_ROOM_STAY);
			pTeam->_Team._PerMax = (UINT8) (rand() % 5 + 4);
			pTeam->_Team._PerCount = (UINT8) (rand() % pTeam->_Team._PerMax + 1);
			pTeam->_Team._MasterIdx = 0;
		}	
	}

	_BuildBattleTeamList(g_pTestAllTeamInfo, g_nTestAllTeamInfoCount);
#else
	//	팀 리스트 컨텍스트 요청
	g_pFramework->GetClanContext()->CreateBattleAllTeamList(VIEW_ROOM_COUNT);
	g_pFramework->GetClanContext()->MatchTeamAllContext();

	m_bUpdateTeamList = FALSE;
#endif

	_SetSelectClanInfo(NULL);

	((CStageReady*)m_pParent)->SetMenuClanInputDisable(TRUE);

	m_pChatBody->SetEntranceStart(this);

	g_pFramework->getChatBox()->setWhisperEnable( TRUE);
	g_pFramework->getChatBox()->ResetHistoryBufferIndex();

	g_pFramework->GetClanContext()->SetInClanBattleLobby(TRUE);
}

void CStepClanBattleLobby::OnEnd(void)
{
	CStepBase::OnEnd();
	
	SetPushedStateButton(MBT_CLAN, TRUE);
	//SetPushedStateButton(MBT_CLAN, FALSE);

	g_pFramework->GetClanContext()->DeleteBattleAllTeamList();

	m_BattleTeamList.Clear();
	I3MEM_SAFE_FREE(m_pBattleTeamInfo);
	m_nBattleTeamInfoCount =0;
	m_szSearchKeyword[0] = 0;

#if defined( TEST_CLAN_BATTLE)
	I3MEM_SAFE_FREE( g_pTestMyTeamInfo);
	I3MEM_SAFE_FREE( g_pTestMyTeamMember);
	I3MEM_SAFE_FREE( g_pTestAllTeamInfo);
#endif

	m_pChatBody->OnExitEnd();

	g_pFramework->getChatBox()->setWhisperEnable( FALSE);
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

void CStepClanBattleLobby::OnExitKey(void)
{
	OnLeave();
}

void CStepClanBattleLobby::OnTeamListScroll(void)
{
	m_idxTeamListStartSlot = m_pTeamScrollList->GetCurrentTop();	
	m_bUpdateTeamList = TRUE;
}

void CStepClanBattleLobby::OnQuickMatch(void)
{
	STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_PREPARE_PLAN"));

/*#if defined( TEST_CLAN_BATTLE)
	CreateBattleRoom();
#else
	g_pFramework->GetClanContext()->RequestTeamQuickMatch();
#endif*/
}

void CStepClanBattleLobby::OnTeamMatch(INT32 slot)
{
	I3_BOUNDCHK( slot, BATTLE_TEAM_SLOT_MAX);

#if defined( TEST_CLAN_BATTLE)
	CreateBattleRoom();
#else

	//	자신이 마스터인지 여부
	{
		const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
		CLAN_BATTLE_MEMBER * pMyMember = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();

		INT32 idx = pMyTeam->_Team._MasterIdx;
 		I3_BOUNDCHK( idx, pMyTeam->_Team._PerMax);
		
		CLAN_BATTLE_MEMBER * pMasterInfo = pMyMember + idx;

		//	리더만이 매치가 가능하다.
		if(pMasterInfo->_i64UID != g_pFramework->GetClanContext()->GetMyDBIdx())
		{
			//	"리더만 신청할 수 있습니다."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_ONLY_LEADER"));
			return;
		}
	}

#if !defined( CLAN_UNLIMITED_MATCH)
	//	팀 인원이 모두 차야 신청할 수 있다.
	{
		const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
		if( pMyTeam->_Team._PerCount < pMyTeam->_Team._PerMax)
		{
			//	"게임을 시작하기 위한 인원이 충분하지 않습니다.\n 팀 구성 인원을 확인해 주세요."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_NEED_TO_MEMBER"));
			return;
		}
	}
#endif

	if(m_BattleTeamList.GetCount() > 0)
	{
		m_nMatchTeamSlot = m_idxTeamListStartSlot + slot;

		I3_BOUNDCHK( m_nMatchTeamSlot, m_BattleTeamList.GetCount());
		CLAN_BATTLE_TEAM_INFO* pTeamInfo = (CLAN_BATTLE_TEAM_INFO*) m_BattleTeamList.GetItem(m_nMatchTeamSlot);

		g_pFramework->GetClanContext()->RequestTeamMatch( pTeamInfo);
	}
#endif
}

void CStepClanBattleLobby::OnLeave(void)
{
	//	"게임이 시작된 이후에는 다시 팀에 참여할 수 없습니다.\n 그래도 나가시겠습니까?"
	STRING_POPUP_Q( GAME_STRING("STBL_IDX_CLAN_BATTLE_LEAVE_Q"), MAKE_CALLBACK(cbQueryLeave), this);
}

void CStepClanBattleLobby::OnSearch(void)
{
	const char * pszKeyword = m_pTeamSearchEdit->getIMETextBuf();
	
	if( i3String::Compare( m_szSearchKeyword, pszKeyword) != 0)	//	이전과 다른 검색어다
	{
		_BuildBattleTeamList(pszKeyword);

		m_idxTeamListStartSlot = 0;
		m_pTeamScrollList->SetCurValue(0.f);
	}
}

void CStepClanBattleLobby::OnViewAll(void)
{
	_BuildBattleTeamList("");

	m_idxTeamListStartSlot = 0;
	m_pTeamScrollList->SetCurValue(0.f);
}

void CStepClanBattleLobby::OnTeamTypeButton(void)
{
	const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	INT32 idxMaster = pMyTeam->_Team._MasterIdx;
	I3_BOUNDCHK( idxMaster, pMyTeam->_Team._PerMax);

	CLAN_BATTLE_MEMBER * pMemberInfo = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();
	CLAN_BATTLE_MEMBER * pMasterInfo = pMemberInfo + idxMaster;

	//	리더만 가능하다.
	if(pMasterInfo->_i64UID == g_pFramework->GetClanContext()->GetMyDBIdx())
	{
		ExclusionPopup(m_pMyTeamTypeComboPopup);
	}
	else
	{
		//	"팀 리더만 변경할 수 있습니다."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_MODIFY_ONLY_LEADER"));
	}	
}

void CStepClanBattleLobby::OnTeamTypePopup(INT32 idx)
{
	const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	INT32 idxMaster = pMyTeam->_Team._MasterIdx;
	I3_BOUNDCHK( idxMaster, pMyTeam->_Team._PerMax);
	I3_BOUNDCHK( idx, CLAN_MATCH_MAX);

	CLAN_BATTLE_MEMBER * pMemberInfo = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();
	CLAN_BATTLE_MEMBER * pMasterInfo = pMemberInfo + idxMaster;

	//	리더만 가능하다.
	if(pMasterInfo->_i64UID == g_pFramework->GetClanContext()->GetMyDBIdx())
	{		
		INT32 nMaxMember = GetClanBattleMatchMember((CLAN_MATCH_TYPE) idx);

		if( nMaxMember < pMyTeam->_Team._PerCount)
		{
			//	"팀 구성 인원을 조절할 수 없습니다. \n 원하는 구성원에 맞게 팀 인원을 조정한 후\n 다시 시도해 주십시오."
			STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_CANNOT_ORGANIZE_TEAM"));
		}
		else
		{
			m_idxBattleTeamMatchType = idx; 

			char * pszMatchType = (char*)GetClanBattleMatchType( (CLAN_MATCH_TYPE) m_idxBattleTeamMatchType);
			m_pMyTeamTypeCombo->SetText(pszMatchType);

			g_pFramework->GetClanContext()->MatchTeamChangePerMax( (UINT8) nMaxMember);
		}
	}
	else
	{
		//	"팀 리더만 변경할 수 있습니다."
		STRING_POPUP( GAME_STRING("STBL_IDX_CLAN_BATTLE_MODIFY_ONLY_LEADER"));
	}	
}

void CStepClanBattleLobby::OnTeamFocus(INT32 idx)
{
	//	포커스된 클랜팀 슬롯
	m_pBattleTeamFocus->SetEnable(TRUE);
	m_pBattleTeamFocus->setPositionY( idx * 27.0f + 1.0f);
}

void CStepClanBattleLobby::OnTeamSelect(INT32 idx)
{	
#ifndef TEST_CLAN_BATTLE 
	//	선택된 클랜팀 슬롯
	CLAN_BATTLE_TEAM_INFO * pTeamInfo = NULL;
	CLAN_BATTLE_ALL_TEAM_CONTEXT * pAllTeamContext = g_pFramework->GetClanContext()->GetClanBattleAllTeamContext();
	I3ASSERT( pAllTeamContext != NULL);

	if(m_BattleTeamList.GetCount() > 0)
	{
		I3_BOUNDCHK( idx + m_idxTeamListStartSlot, pAllTeamContext->_totalcount);

		m_nSelectTeamSlot = idx + m_idxTeamListStartSlot;

		I3_BOUNDCHK( m_nSelectTeamSlot, m_BattleTeamList.GetCount());

		m_pBattleTeamSelect->SetEnable(TRUE);
		m_pBattleTeamSelect->setPositionY( m_nSelectTeamSlot * 27.0f + 1.0f);

		pTeamInfo = (CLAN_BATTLE_TEAM_INFO*) m_BattleTeamList.GetItem(m_nSelectTeamSlot);

		//	서버에 선택된 클랜 정보 요청
		g_pFramework->GetClanContext()->MatchTeamDetailInfo( pTeamInfo->_Idx);
	}
#endif
}

void CStepClanBattleLobby::OnChatCombo(void)
{
	ExclusionPopup(m_pChatBody->m_pChatPopup);
}

void CStepClanBattleLobby::OnChatPopup(INT32 idx)
{
	m_pChatBody->OnChatPopup(idx);
}

void CStepClanBattleLobby::OnSortListByNumber(void)
{
	if( m_sTeamSortType == CLAN_SORT_NUMBER_ASC)
		m_sTeamSortType = CLAN_SORT_NUMBER_DESC;
	else
		m_sTeamSortType = CLAN_SORT_NUMBER_ASC;

	m_BattleTeamList.SetCompareProc(cbSortTeamListByNumber);

	//	리스트 재구성
	_BuildBattleTeamList();

	m_pListHeadSlotNum->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanBattleLobby::OnSortListByRank(void)
{
	if( m_sTeamSortType == CLAN_SORT_RANK_ASC)
		m_sTeamSortType = CLAN_SORT_RANK_DESC;
	else
		m_sTeamSortType = CLAN_SORT_RANK_ASC;

	m_BattleTeamList.SetCompareProc(cbSortTeamListByRank);

	//	리스트 재구성
	_BuildBattleTeamList();

	m_pListHeadClanRank->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanBattleLobby::OnSortListByName(void)
{
	if( m_sTeamSortType == CLAN_SORT_NAME_ASC)
		m_sTeamSortType = CLAN_SORT_NAME_DESC;
	else
		m_sTeamSortType = CLAN_SORT_NAME_ASC;

	m_BattleTeamList.SetCompareProc(cbSortTeamListByName);

	//	리스트 재구성
	_BuildBattleTeamList();

	m_pListHeadClanName->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanBattleLobby::OnSortListByTeam(void)
{ 
	if( m_sTeamSortType == CLAN_SORT_TEAM_ASC)
		m_sTeamSortType = CLAN_SORT_TEAM_DESC;
	else
		m_sTeamSortType = CLAN_SORT_TEAM_ASC;

	m_BattleTeamList.SetCompareProc(cbSortTeamListByTeam);

	//	리스트 재구성
	_BuildBattleTeamList();

	m_pListHeadTeam->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanBattleLobby::OnSortListByType(void)
{ 
	if( m_sTeamSortType == CLAN_SORT_TYPE_ASC)
		m_sTeamSortType = CLAN_SORT_TYPE_DESC;
	else
		m_sTeamSortType = CLAN_SORT_TYPE_ASC;

	m_BattleTeamList.SetCompareProc(cbSortTeamListByType);

	//	리스트 재구성
	_BuildBattleTeamList();

	m_pListHeadType->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CStepClanBattleLobby::OnSortListByState(void)
{
	if( m_sTeamSortType == CLAN_SORT_STATE_ASC)
		m_sTeamSortType = CLAN_SORT_STATE_DESC;
	else
		m_sTeamSortType = CLAN_SORT_STATE_ASC;

	m_BattleTeamList.SetCompareProc(cbSortTeamListByState);

	//	리스트 재구성
	_BuildBattleTeamList();

	m_pListHeadState->SetTextColor( GetColor(GCT_INVERSE));
	PlaySound(GUI_SND_ITEM_SELECTING);
}

INT32 CStepClanBattleLobby::cbSortTeamListByNumber(void * item1, void * item2)
{
	CLAN_BATTLE_TEAM_INFO * pInfo1 = (CLAN_BATTLE_TEAM_INFO*) item1;
	CLAN_BATTLE_TEAM_INFO * pInfo2 = (CLAN_BATTLE_TEAM_INFO*) item2;

	//	번호순
	if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_NUMBER_ASC)
	{
		return COMPARE_ASC(pInfo1->_Idx, pInfo2->_Idx);
	}
	else if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_NUMBER_DESC)
	{
		return COMPARE_DESC(pInfo1->_Idx, pInfo2->_Idx);
	}

	return 0;
}

INT32 CStepClanBattleLobby::cbSortTeamListByRank(void * item1, void * item2)
{
	CLAN_BATTLE_TEAM_INFO * pInfo1 = (CLAN_BATTLE_TEAM_INFO*) item1;
	CLAN_BATTLE_TEAM_INFO * pInfo2 = (CLAN_BATTLE_TEAM_INFO*) item2;

	//	랭크순
	if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_RANK_ASC)
	{
		return COMPARE_ASC(pInfo1->_Clan._ClanRank, pInfo2->_Clan._ClanRank);
	}
	else if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_RANK_DESC)
	{
		return COMPARE_DESC(pInfo1->_Clan._ClanRank, pInfo2->_Clan._ClanRank);
	}

	return 0;
}

INT32 CStepClanBattleLobby::cbSortTeamListByName(void * item1, void * item2)
{
	CLAN_BATTLE_TEAM_INFO * pInfo1 = (CLAN_BATTLE_TEAM_INFO*) item1;
	CLAN_BATTLE_TEAM_INFO * pInfo2 = (CLAN_BATTLE_TEAM_INFO*) item2;

	//	이름순
	if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_NAME_ASC)
	{
		return CompareString(pInfo1->_Clan._ClanName, pInfo2->_Clan._ClanName, TRUE);
	}
	else if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_NAME_DESC)
	{
		return CompareString(pInfo1->_Clan._ClanName, pInfo2->_Clan._ClanName, FALSE);
	}

	return 0;
}

INT32 CStepClanBattleLobby::cbSortTeamListByTeam(void * item1, void * item2)
{
	CLAN_BATTLE_TEAM_INFO * pInfo1 = (CLAN_BATTLE_TEAM_INFO*) item1;
	CLAN_BATTLE_TEAM_INFO * pInfo2 = (CLAN_BATTLE_TEAM_INFO*) item2;

	//	팀명 순
	if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_TEAM_ASC)
	{
		return COMPARE_ASC(pInfo1->_Team._Naming, pInfo2->_Team._Naming);
	}
	else if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_TEAM_DESC)
	{
		return COMPARE_DESC(pInfo1->_Team._Naming, pInfo2->_Team._Naming);
	}

	return 0;
}

INT32 CStepClanBattleLobby::cbSortTeamListByType(void * item1, void * item2)
{
	CLAN_BATTLE_TEAM_INFO * pInfo1 = (CLAN_BATTLE_TEAM_INFO*) item1;
	CLAN_BATTLE_TEAM_INFO * pInfo2 = (CLAN_BATTLE_TEAM_INFO*) item2;

	//	팀 구성 순
	if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_TYPE_ASC)
	{
		return COMPARE_ASC(pInfo1->_Team._PerMax, pInfo2->_Team._PerMax);
	}
	else if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_TYPE_DESC)
	{
		return COMPARE_DESC(pInfo1->_Team._PerMax, pInfo2->_Team._PerMax);
	}

	return 0;
}

INT32 CStepClanBattleLobby::cbSortTeamListByState(void * item1, void * item2)
{
	CLAN_BATTLE_TEAM_INFO * pInfo1 = (CLAN_BATTLE_TEAM_INFO*) item1;
	CLAN_BATTLE_TEAM_INFO * pInfo2 = (CLAN_BATTLE_TEAM_INFO*) item2;

	//	팀상태 순
	if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_STATE_ASC)
	{
		return COMPARE_ASC(pInfo1->_Team._State, pInfo2->_Team._State);
	}
	else if( CStepClanBattleLobby::m_sTeamSortType == CLAN_SORT_STATE_DESC)
	{
		return COMPARE_DESC(pInfo1->_Team._State, pInfo2->_Team._State);
	}

	return 0;
}

void CStepClanBattleLobby::CreateBattleRoom(void)
{
	ROOM_INFO_BASIC MyRoomInfo;
	MyRoomInfo.Reset();

	// 1.방이름
	{
		char szRoomName[MAX_STRING_COUNT] = "";

		//STRING_TABLE_IDX idx = (STRING_TABLE_IDX)(STBL_IDX_ROOMNAME_1 + (i3Math::Rand() % FIXED_ROOMNAME_COUNT));
		//char * pszRoomName = GAME_STRING("idx");
#if defined( TEST_CLAN_BATTLE)
		i3String::Copy(szRoomName, "TEST");
#else
		const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
		CLAN_BATTLE_TEAM_INFO * pReqTeam = g_pFramework->GetClanContext()->GetClanBattleReqTeamInfo();

		sprintf_s(szRoomName, "%s VS %s", pMyTeam->_Clan._ClanName, pReqTeam->_Clan._ClanName);
#endif

		i3String::NCopy(MyRoomInfo._Title, szRoomName, NET_ROOM_NAME_SIZE-1);

		//// 무기 제한	
		MyRoomInfo._WeaponFlag = 0x0F;
	}

	// 2.게임모드
	MyRoomInfo._StageID = getFirstStageByType( STAGE_TYPE_DEATHMATCH);
	g_pGameContext->setMyRoomInfo(&MyRoomInfo);
	
	// 3.비밀방 여부
	i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

	//	레디룸 생성
	g_pGameContext->SetEvent( EVENT_CREATE_ROOM);

	((CStageReady*)m_pParent)->ChangeStep(STEP_NULL);
	//((CStageReady*)m_pParent)->MovePhase(PHASE_READY_ROOM);

	//	옵션 기본 변경
	{
		ROOM_INFO_ADD infoAdd; 
		i3mem::FillZero( &infoAdd, sizeof( ROOM_INFO_ADD));
		
		// 난입제한
		infoAdd._LockInterEnter = 1;

		// 관전제한
		infoAdd._LockObserver = 0;

		// 팀밸런스
		infoAdd._LockTeamBalance = 0;

		// 비교후 변경된것이 있으면 보낸다.
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_OPTION_N, &infoAdd);
	}
}

void CStepClanBattleLobby::_InitMyTeam(void)
{
	INIT_WIN_CAPTION(m_pMyTeamWinCaption, GAME_STRING("STBL_IDX_CLAN_BATTLE_TEAM_INFO"));	//	"팀정보"

	INIT_STATIC_CAPTION(m_pMyTeamClanNameStatic, GAME_STRING("STBL_IDX_CLAN_NAME"));		//	"클랜명:"
	INIT_STATIC_CAPTION(m_pMyTeamClanNameValue, INVALID_TEXT);
	INIT_STATIC_CAPTION(m_pMyTeamClanRankStatic, GAME_STRING("STBL_IDX_CLAN_RANK"));		//	"클랜 등급:"
	INIT_STATIC_CAPTION(m_pMyTeamClanRankValue, INVALID_TEXT);
	INIT_STATIC_CAPTION(m_pMyTeamClanPointStatic, GAME_STRING("STBL_IDX_CLAN_POINT"));	//	"클랜 포인트:"
	INIT_STATIC_CAPTION(m_pMyTeamClanPointValue, INVALID_TEXT);

	I3COLOR color;
	i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
	m_pMyTeamClanPointValue->SetTextColor(&color);
	
	//	클랜 등급
	SetRankImage(m_pMyTeamClanRankImage, 0);
	m_pMyTeamClanRankValue->SetTextColor( GetColor(GCT_VALUE));

	//	클랜 포인트
	SetClanPoint(m_pMyTeamClanPointValue, 0.f);

	//	팀 이름
	INIT_STATIC_CAPTION(m_pMyTeamNameValue, INVALID_TEXT);
	m_pMyTeamClanNameValue->SetTextColor( GetColor(GCT_VALUE));

	//	팀원 슬롯
	for(INT32 i=0; i<TEAM_MEMBER_SLOT_MAX; i++)
	{
		INIT_VALUE_CAPTION(m_MyTeamMember[i]._pNick, "");

		SetRankImage(m_MyTeamMember[i]._pRank, 0);

		m_MyTeamMember[i]._pState->SetAllShapeEnable(FALSE);
		m_MyTeamMember[i]._pLeader->SetEnable(FALSE);

		m_MyTeamMember[i]._pSlot->SetEnable(FALSE);
	}

	//	팀 구성
	INIT_NORMAL_CAPTION(m_pMyTeamTypeStatic, GAME_STRING("STBL_IDX_CLAN_BATTLE_TEAM_ORGANIZATION"));	//	"팀 구성"
	m_pMyTeamTypeStatic->SetTextAlign( TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);

	char * pszMatchType = (char*)GetClanBattleMatchType( (CLAN_MATCH_TYPE) m_idxBattleTeamMatchType);
	INIT_NORMAL_CAPTION(m_pMyTeamTypeCombo, pszMatchType);	//	팀 구성 콤보
	INIT_COMBO_EX(m_pMyTeamTypeComboButton);	
	m_pMyTeamTypeComboPopup->CreateTextEx(5,  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_POPUPLIST_SIZE );
	m_pMyTeamTypeComboPopup->SetSelColor(100, 100, 100, 100);
	m_pMyTeamTypeComboPopup->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pMyTeamTypeComboPopup->SetTextColor(GetColor(GCT_DEFAULT));
	m_pMyTeamTypeComboPopup->SetExclusiveEnableControl(TRUE);

	for(INT32 i=0; i<CLAN_MATCH_MAX; i++)
	{
		char * pszMatchType = (char*)GetClanBattleMatchType( (CLAN_MATCH_TYPE) i);
		m_pMyTeamTypeComboPopup->AddElement( pszMatchType);
	}

	m_pMyTeamTypeComboPopup->SetSelectedElement(m_idxBattleTeamMatchType, FALSE);
	m_pMyTeamTypeComboPopup->SetEnable(FALSE);
	
}

void CStepClanBattleLobby::_InitTeamList(void)
{
	INIT_WIN_CAPTION(m_pTeamListCaption, GAME_STRING("STBL_IDX_CLAN_BATTLE_LIST"));	//	"클랜전 리스트"
	
	INIT_STATIC_CAPTION(m_pTeamSearchStatic, GAME_STRING("STBL_IDX_CLAN_BATTLE_SEARCHING"));	//	"클랜찾기"

	//	검색 입력창
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NET_CLAN_NAME_SIZE - 1;
	ime._StyleIME = 0;
	m_pTeamSearchEdit->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime, FW_NORMAL, TRUE);
	m_pTeamSearchEdit->SetTextAlign(ALIGN_LEFT, ALIGN_TOP);
	m_pTeamSearchEdit->SetTextAutoWrap(FALSE);
	m_pTeamSearchEdit->SetOffsetPos(0.0f, 2.0f);
	m_pTeamSearchEdit->setCaretPosOffset(0.0f, 2.0f);	
	m_pTeamSearchEdit->SetIMEMode( IME_MODE_NATIVE);
	m_pTeamSearchEdit->SetTextColor(GetColor(GCT_DEFAULT));

	INIT_BUTTON_EX(m_pTeamSearchButton);
	INIT_CAPTION_BUTTON(m_pTeamViewAll, GAME_STRING("STBL_IDX_CLAN_VIEW_ALL"));	//	"전체보기"
	m_pTeamViewAll->SetCaptionTextSpace( 0, -4);
	m_pTeamViewAll->SetCaptionTextOffset( 2.0f, -2.0f);

	INIT_BUTTON_EX(m_pTeamQuickMatch);
	INIT_BUTTON_EX(m_pTeamLeave);

	m_pTeamQuickMatch->SetEnable(FALSE);

	INIT_NORMAL_CAPTION(m_pListHeadSlotNum, GAME_STRING("STBL_IDX_CLAN_BATTLE_HEADER_NUMBER"));		//	"번호"
	INIT_NORMAL_CAPTION(m_pListHeadClanRank, GAME_STRING("STBL_IDX_CLAN_BATTLE_HEADER_RANK"));		//	"등급"
	INIT_NORMAL_CAPTION(m_pListHeadClanName, GAME_STRING("STBL_IDX_CLAN_BATTLE_HEADER_NAME"));		//	"클랜명"
	INIT_NORMAL_CAPTION(m_pListHeadTeam, GAME_STRING("STBL_IDX_CLAN_BATTLE_HEADER_TEAM"));			//	"팀"
	INIT_NORMAL_CAPTION(m_pListHeadType, GAME_STRING("STBL_IDX_CLAN_BATTLE_HEADER_ORGANIZATION"));	//	"구성"
	INIT_NORMAL_CAPTION(m_pListHeadState, GAME_STRING("STBL_IDX_CLAN_BATTLE_HEADER_STATE"));		//	"상태"

	m_pListHeadType->SetTextSpace( 0, -2);

	m_pTeamScrollList->CreateTextEx(BATTLE_TEAM_SLOT_MAX,  GetDefaultFontName());
	m_pTeamScrollList->SetSelColor(0, 0, 0, 0); 

	for(INT32 i=0; i<BATTLE_TEAM_SLOT_MAX; i++)
	{
		INIT_WIN_CAPTION(m_BattleTeam[i]._pNumber, "000");

		m_BattleTeam[i]._pClanName->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_BattleTeam[i]._pClanName->SetTextColor(GetColor(GCT_VALUE));
		m_BattleTeam[i]._pClanName->SetTextSpace( 0, -2);
		m_BattleTeam[i]._pClanName->SetTextEllipsis(INVALID_TEXT); 

		INIT_WIN_CAPTION(m_BattleTeam[i]._pTeamName, INVALID_TEXT); 
		INIT_WIN_CAPTION(m_BattleTeam[i]._pTeamType, INVALID_TEXT);
		m_BattleTeam[i]._pTeamName->SetTextSpace( 0, -2);
		m_BattleTeam[i]._pTeamType->SetTextSpace( 0, -2);

		INIT_CAPTION_BUTTON(m_BattleTeam[i]._pMatch, GAME_STRING("STBL_IDX_CLAN_BATTLE_PROPOSE"));		//	"신청하기"
		m_BattleTeam[i]._pMatch->SetCaptionTextSpace( 0, -3);

		m_BattleTeam[i]._pRank->SetAllShapeEnable(FALSE);

		m_BattleTeam[i]._pState->SetAllShapeEnable(FALSE);	
		m_BattleTeam[i]._pState->SetEnable(FALSE);

		m_BattleTeam[i]._pSlot->SetEnable(FALSE);
	}

	m_pBattleTeamFocus->SetEnable(FALSE);
	m_pBattleTeamFocus->setInputDisable(TRUE);
	m_pBattleTeamSelect->SetEnable(FALSE);
	m_pBattleTeamSelect->setInputDisable(TRUE);

	//	선택 클랜 정보
	INIT_STATIC_CAPTION(m_pSelectClanInfoStatic, GAME_STRING("STBL_IDX_CLAN_INFORMATION"));	//	"클랜 정보"

	INIT_STATIC_CAPTION(m_pSelectClanNameStatic, GAME_STRING("STBL_IDX_CLAN_NAME"));		//	"클랜명 :"
	INIT_STATIC_CAPTION(m_pSelectClanStaffStatic, GAME_STRING("STBL_IDX_CLAN_PERSON"));	//	"클랜 인원수 :"
	INIT_STATIC_CAPTION(m_pSelectClanMasterStatic, GAME_STRING("STBL_IDX_CLAN_MASTER"));	//	"클랜 마스터 :"
	INIT_STATIC_CAPTION(m_pSelectClanTeamLeaderStatic, GAME_STRING("STBL_IDX_CLAN_BATTLE_TEAM_LEADER"));	//	"팀 리더 :"	
	INIT_STATIC_CAPTION(m_pSelectClanRankStatic, GAME_STRING("STBL_IDX_CLAN_RANK"));		//	"클랜 등급 :"
	INIT_STATIC_CAPTION(m_pSelectClanExpStatic, GAME_STRING("STBL_IDX_CLAN_EXP"));		//	"클랜 경험치 :"
	INIT_STATIC_CAPTION(m_pSelectClanOrderStatic, GAME_STRING("STBL_IDX_CLAN_ORDER"));	//	"클랜 순위 :"
	INIT_STATIC_CAPTION(m_pSelectClanAddressStatic, GAME_STRING("STBL_IDX_CLAN_AZIT"));	//	"클랜 아지트 :"
	
	INIT_VALUE_CAPTION(m_pSelectClanNameValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanStaffValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanMasterValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanTeamLeaderValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanRankValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanExpValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanOrderValue, "");
	INIT_VALUE_CAPTION(m_pSelectClanAddressValue, "");

	m_pSelectClanAddressValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

	I3COLOR color;
	i3Color::Set(&color, (UINT8) 255, 125, 0, 255);
	m_pSelectClanAddressValue->SetTextColor(&color);

#ifndef CLAN_ADDRESS_ENABLE
	m_pSelectClanAddressBullet->SetEnable(FALSE);
	m_pSelectClanAddressStatic->SetEnable(FALSE);
	m_pSelectClanAddressValue->SetEnable(FALSE);
#endif

	_SetSelectClanInfo(NULL);
}

void CStepClanBattleLobby::_UpdateMyTeam(REAL32 rDeltaSeconds)
{
	const CLAN_BATTLE_TEAM_INFO * pMyTeam = NULL;
	CLAN_BATTLE_MEMBER * pMyMember = NULL;

	char szTemp[MAX_STRING_COUNT] = "";
	INT32 nTeamNaming = 0;
	INT32 max = 0;
	
	//	클랜정보
	_UpdateClanInfo(rDeltaSeconds);

#if defined( TEST_CLAN_BATTLE)
	pMyTeam = g_pTestMyTeamInfo;
	pMyMember = g_pTestMyTeamMember;
#else
	pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	pMyMember = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();
#endif
	
	if( pMyTeam != NULL)
	{
		const char * pszMatchType = INVALID_TEXT;
		nTeamNaming = pMyTeam->_Team._Naming;
		max = pMyTeam->_Team._PerMax;

		//	팀 구성 셋팅
		m_idxBattleTeamMatchType = (INT32) ConvertClanBattleMatchType(max);

		if( -1 < m_idxBattleTeamMatchType)
			pszMatchType = GetClanBattleMatchType( (CLAN_MATCH_TYPE) m_idxBattleTeamMatchType);
	
		m_pMyTeamTypeCombo->SetText(pszMatchType);
	}

	//	맴버 정보
	{
		GetBattleTeamName( szTemp, MAX_STRING_COUNT, nTeamNaming);
		m_pMyTeamNameValue->SetText( szTemp);

		for(INT32 i=0; i<max; i++)
		{
			CLAN_BATTLE_MEMBER * pMember = pMyMember + i;

			if( pMember->_State != CLAN_ROOM_EMPTY )
				_SetMemberSlot(i, pMyMember + i);
			else
				_SetMemberSlot(i, NULL);
		}
	}	
}

void CStepClanBattleLobby::_UpdateClanInfo(REAL32 rDeltaSeconds)
{
	CLAN_DETAIL_INFO * pClanInfo = g_pFramework->GetClanContext()->GetMyClanDetailInfo();
	const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();

	//	클랜명
	SetNameUsingColorTable( m_pMyTeamClanNameValue, pMyTeam->_Clan._ClanName, pMyTeam->_Clan._ClanColor );

	//	클랜 등급
	SetClanRank(m_pMyTeamClanRankValue, m_pMyTeamClanRankImage, pClanInfo->_exp, pClanInfo->_currentPerson, pMyTeam->_Clan._ClanRank);

	//	클랜 포인트
	SetClanPoint(m_pMyTeamClanPointValue, pMyTeam->_Clan._ClanPoint);

	//	클랜 마크
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(m_pMyTeamClanMarkImage, CLAN_MARK_MEDIUM, pMyTeam->_Clan._ClanMark);
#endif
}

void CStepClanBattleLobby::_UpdateSelectClan(REAL32 rDeltaSeconds)
{
	if( m_nSelectTeamSlot != -1)
	{
		CLAN_DETAIL_INFO * pInfo = g_pGameContext->getClanDetailInfo();
		_SetSelectClanInfo( pInfo);
	}
	else 
	{
		_SetSelectClanInfo( NULL);
	}
}

void CStepClanBattleLobby::_SetMemberSlot(INT32 idx, CLAN_BATTLE_MEMBER * pMember)
{
	I3_BOUNDCHK( idx, TEAM_MEMBER_SLOT_MAX);

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
	if(pMember->_State >= CLAN_ROOM_READY)
	{
		pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
	}
	/*{
	case SLOT_STATE_INVENTORY:	pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);	break;
	case SLOT_STATE_INFO:		pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);	break;
	case SLOT_STATE_SHOP:		pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);	break;
	}*/

	

	//	리더 표시
	{
		const CLAN_BATTLE_TEAM_INFO * pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();

		//	리더 확인
		if( idx == pMyTeam->_Team._MasterIdx)
			pSlotInfo->_pLeader->SetEnable(TRUE);
		else
			pSlotInfo->_pLeader->SetEnable(FALSE);
	}
}

void CStepClanBattleLobby::_UpdateTeamList(RT_REAL32 rDeltaSeconds)
{
	CLAN_BATTLE_ALL_TEAM_CONTEXT* pAllTeamContext = NULL;
	const CLAN_BATTLE_TEAM_INFO * pMyTeam = NULL;
	CLAN_BATTLE_MEMBER * pMyMember = NULL;
	INT32 nMax = 0;

#if defined( TEST_CLAN_BATTLE)
	pAllTeamContext = NULL;
	pMyTeam = g_pTestMyTeamInfo;
	pMyMember = g_pTestMyTeamMember;
	pAllTeam = g_pTestAllTeamInfo;
	nMax = g_nTestMyTeamMemberCount;
#else
	pAllTeamContext = g_pFramework->GetClanContext()->GetClanBattleAllTeamContext();
	pMyTeam = g_pFramework->GetClanContext()->GetClanBattleMyTeamInfo();
	pMyMember = g_pFramework->GetClanContext()->GetClanBattleMyTeamMember();
	nMax = (INT32) pMyTeam->_Team._PerMax;
	
	//	팀 리스트 업데이트
	if( pAllTeamContext != NULL)
	{
		m_fUpdateTeamListTime += rDeltaSeconds;

		//	팀 리스트를 받는다.
		if( m_fUpdateTeamListTime > LIST_UPDATE_TIME)
		{
			m_fUpdateTeamListTime = 0.f;

			if( m_nTeamListSection > pAllTeamContext->_sectionCount)
				m_nTeamListSection = 0;

			g_pFramework->GetClanContext()->MatchTeamAllList( (UINT16) m_nTeamListSection);
			m_nTeamListSection++;
		}
	}
#endif

	//	팀 리스트 셋팅
	if( m_bUpdateTeamList)
	{
		BOOL bMaster = FALSE;
		
#if defined( TEST_CLAN_BATTLE)
		bMaster = TRUE;
		m_pTeamQuickMatch->SetEnable(FALSE);
#else
		//	자신이 마스터인지 여부
		{
			INT32 idx = pMyTeam->_Team._MasterIdx;
 			I3_BOUNDCHK( idx, pMyTeam->_Team._PerMax);

			CLAN_BATTLE_MEMBER * pMasterInfo = pMyMember + idx;

			//	리더만이 퀵매치가 가능하다.
			if(pMasterInfo->_i64UID == g_pFramework->GetClanContext()->GetMyDBIdx())
			{
				bMaster = TRUE;
				//m_pTeamQuickMatch->SetEnable(TRUE);
				m_pTeamQuickMatch->SetEnable(FALSE);		//	일단 기획실 요청으로 막습니다. komet
			}
			else
			{
				bMaster = FALSE;
				m_pTeamQuickMatch->SetEnable(FALSE);

			}

		}
#endif

		for(INT32 i=0; i<BATTLE_TEAM_SLOT_MAX; i++)
		{
			INT32 idx = m_idxTeamListStartSlot + i;

			//	보여지는 슬롯 셋팅
			if( idx < m_BattleTeamList.GetCount())
			{
				CLAN_BATTLE_TEAM_INFO * pTeamInfo = (CLAN_BATTLE_TEAM_INFO*) m_BattleTeamList.GetItem(idx);

				_SetTeamListSlot(i, idx, pTeamInfo, bMaster);
			}
			//	빈 슬롯 처리
			else
			{
				_SetTeamListSlot(i, idx, NULL);
			}
		}

		m_bUpdateTeamList = FALSE;
	}
}

void CStepClanBattleLobby::_SetTeamListSlot(INT32 idx, INT32 nIdx, CLAN_BATTLE_TEAM_INFO * pTeamInfo, BOOL bMaster )
{
	I3_BOUNDCHK( idx, BATTLE_TEAM_SLOT_MAX);

	char szTemp[256] = "";
	BattleTeamSlot * pSlotInfo = &m_BattleTeam[idx];

	if( pTeamInfo == NULL)
	{
		pSlotInfo->_pSlot->SetEnable(FALSE);
		return;
	}	

	pSlotInfo->_pSlot->SetEnable(TRUE);

	sprintf_s(szTemp, "%03d", nIdx+1);

	//	슬롯 번호
	pSlotInfo->_pNumber->SetText( szTemp);

	//	클랜 등급
	SetClanRankImage(pSlotInfo->_pRank, pTeamInfo->_Clan._ClanRank);

	//	클랜 마크
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(pSlotInfo->_pClanMark, CLAN_MARK_MEDIUM, pTeamInfo->_Clan._ClanMark);
#endif	

	//	클랜명
	SetNameUsingColorTable( pSlotInfo->_pClanName, pTeamInfo->_Clan._ClanName, pTeamInfo->_Clan._ClanColor );

	//	팀명
	GetBattleTeamName(szTemp, 256, pTeamInfo->_Team._Naming, FALSE);
	pSlotInfo->_pTeamName->SetText( szTemp);

	//	팀구성
	sprintf_s(szTemp, "%d / %d", pTeamInfo->_Team._PerCount, pTeamInfo->_Team._PerMax);
	pSlotInfo->_pTeamType->SetText( szTemp);

	if(pTeamInfo->_Team._PerCount == pTeamInfo->_Team._PerMax)
		pSlotInfo->_pTeamType->SetTextColor( GetColor( GCT_VALUE));
	else
		pSlotInfo->_pTeamType->SetTextColor( GetColor( GCT_DEFAULT));
	
	pSlotInfo->_pState->SetEnable(FALSE);
	pSlotInfo->_pState->SetAllShapeEnable(FALSE);
	
	switch( pTeamInfo->_Team._State)
	{
	//	신청 받을수 있는 상태
	case CLAN_ROOM_STAY:
		{
			pSlotInfo->_pMatch->SetEnable(TRUE);

			if( bMaster)
			{
				//	팀 인원이 모두 만족되야 신청가능합니다...CLAN_UNLIMITED_MATCH는 팀 인원 관계없이 신청할 수 있도록 합니다. koemt
#if defined( CLAN_UNLIMITED_MATCH)	
				ACTIVATE_BUTTON(pSlotInfo->_pMatch);

#else
				// 상대팀의 인원이 정상이고, 내팀 인원이 정상이고, 상대팀, 내팀 제한인원이 같아야 신청가능 
				if(pTeamInfo->_Team._PerCount == pTeamInfo->_Team._PerMax && 
					g_pGameContext->getClanBattleMyTeamInfo()->_Team._PerCount == g_pGameContext->getClanBattleMyTeamInfo()->_Team._PerMax &&
					pTeamInfo->_Team._PerMax == g_pGameContext->getClanBattleMyTeamInfo()->_Team._PerMax )
				{
					ACTIVATE_BUTTON(pSlotInfo->_pMatch);
				}
				else
				{
					DEACTIVATE_BUTTON(pSlotInfo->_pMatch);
				}
#endif
			}
			else 
			{
				DEACTIVATE_BUTTON(pSlotInfo->_pMatch);
			}

			pSlotInfo->_pMatch->OnUpdate(0.f);
		}
		break;
	//	매칭된 상태 (신청 받을수 없다)
	case CLAN_ROOM_READY:
	case CLAN_ROOM_PLAY:
		{
			pSlotInfo->_pMatch->SetEnable(FALSE);
			pSlotInfo->_pState->SetEnable(TRUE);
			pSlotInfo->_pState->SetShapeEnable(GUI_SHAPE_OFFSET+1, TRUE);
		}
		break;
	}
}

void CStepClanBattleLobby::_CreateTeamListBuffer(INT32 count)
{
	//	버퍼 생성
	if( m_nBattleTeamInfoCount < count)
	{
		I3MEM_SAFE_FREE(m_pBattleTeamInfo);

		m_nBattleTeamInfoCount = count;

		if( m_pBattleTeamInfo == NULL && m_nBattleTeamInfoCount > 0)
		{
			m_pBattleTeamInfo = (CLAN_BATTLE_TEAM_INFO*) i3MemAlloc( sizeof(CLAN_BATTLE_TEAM_INFO) * m_nBattleTeamInfoCount);
			I3ASSERT( m_pBattleTeamInfo != NULL);

			i3mem::FillZero( m_pBattleTeamInfo, sizeof(CLAN_BATTLE_TEAM_INFO) * m_nBattleTeamInfoCount);
		}
	}
}

void CStepClanBattleLobby::_BuildBattleTeamList(void)
{
	_BuildBattleTeamList(m_szSearchKeyword);
}

void CStepClanBattleLobby::_BuildBattleTeamList(const char * pszKeyword)
{	
	INT32 count = 0;

#if defined( TEST_CLAN_BATTLE)
	CLAN_BATTLE_TEAM_INFO * pAllTeam = g_pTestAllTeamInfo;
	count = g_nTestAllTeamInfoCount;
#else
	CLAN_BATTLE_ALL_TEAM_CONTEXT * pAllTeamContext = g_pFramework->GetClanContext()->GetClanBattleAllTeamContext();
	CLAN_BATTLE_TEAM_INFO * pAllTeam = g_pFramework->GetClanContext()->GetClanBattleAllTeamList();
	count = pAllTeamContext->_totalcount;
#endif

	I3ASSERT(pAllTeamContext != NULL);

	//	하나도 없다. 클리어~
	if(pAllTeamContext->_totalcount == 0 || pAllTeam == NULL)
	{
		I3MEM_SAFE_FREE(m_pBattleTeamInfo);
		m_nBattleTeamInfoCount = 0;

		m_pTeamScrollList->Clear();
		m_BattleTeamList.Clear();

		m_szSearchKeyword[0] = 0;

		return;
	}

	i3String::Copy( m_szSearchKeyword, pszKeyword, MAX_STRING_COUNT );

	//	검색어가 없다.
	if( pszKeyword == NULL || i3String::Length(pszKeyword) == 0)
		_BuildBattleTeamList(pAllTeam, count);
	else
	{
		i3List searchList;

		//	해당 키워드를 가진 항목이 있는지 비교해서 찾는다.
		searchList.Clear();
		for(INT32 i=0; i<count; i++)
		{
			CLAN_BATTLE_TEAM_INFO * pSrc = pAllTeam + i;
			I3_BOUNDCHK( pSrc->_Team._PerMax, 9);	

			if( ContainString(pSrc->_Clan._ClanName, pszKeyword) != -1)
			{
				searchList.Add( (void*) pSrc);
			}
		}

		_BuildBattleTeamList(&searchList);
	}
}

void CStepClanBattleLobby::_BuildBattleTeamList(CLAN_BATTLE_TEAM_INFO * pTeamInfo, INT32 count)
{
	_CreateTeamListBuffer(count);

	//	정보 복사
	m_pTeamScrollList->Clear();
	m_BattleTeamList.Clear();
	for(INT32 i=0; i<count; i++)
	{
		I3_BOUNDCHK( i, m_nBattleTeamInfoCount); 

		CLAN_BATTLE_TEAM_INFO * pSrc = pTeamInfo + i;		
		I3ASSERT( pSrc != NULL);

		//	상태가 유효한 팀
		if( pSrc->_Team._State != CLAN_ROOM_EMPTY)
		{
			CLAN_BATTLE_TEAM_INFO * pDst = m_pBattleTeamInfo + i;
			I3ASSERT( pDst != NULL);
			I3_BOUNDCHK( pDst->_Team._PerMax, 9);	

			m_pTeamScrollList->AddElement("");

			i3mem::Copy( pDst, pSrc, sizeof(CLAN_BATTLE_TEAM_INFO));

			m_BattleTeamList.Add( (void*) pDst);
		}
	}	

	m_bUpdateTeamList = TRUE;
}

void CStepClanBattleLobby::_BuildBattleTeamList(i3List * pList) 
{
	INT32 count = pList->GetCount();

	_CreateTeamListBuffer(count);

	//	정보 복사
	m_pTeamScrollList->Clear();
	m_BattleTeamList.Clear();
	for(INT32 i=0; i<count; i++)
	{
		I3_BOUNDCHK( i, m_nBattleTeamInfoCount);

		CLAN_BATTLE_TEAM_INFO * pSrc = (CLAN_BATTLE_TEAM_INFO*) pList->GetItem(i);	
		I3_BOUNDCHK( pSrc->_Team._PerMax, 9);	

		//	상태가 유효한 팀
		if( pSrc->_Team._State != CLAN_ROOM_EMPTY)
		{
			CLAN_BATTLE_TEAM_INFO * pDst = m_pBattleTeamInfo + i;
			I3_BOUNDCHK( pDst->_Team._PerMax, 9);	

			m_pTeamScrollList->AddElement("");

			i3mem::Copy( pDst, pSrc, sizeof(CLAN_BATTLE_TEAM_INFO));

			m_BattleTeamList.Add( (void*) pDst);
		}
	}	

	m_bUpdateTeamList = TRUE;
}

void CStepClanBattleLobby::_SetSelectClanName(const char * pszName)
{
	m_pSelectClanNameValue->SetText(pszName);
}

void CStepClanBattleLobby::_SetSelectClanStaff(INT32 nNum, INT32 nTotal)
{
	SetClanStaff(m_pSelectClanStaffValue, nNum, nTotal);
}

void CStepClanBattleLobby::_SetSelectClanMaster(const char * pszName, UINT8 nRank)
{
	SetClanMaster(m_pSelectClanMasterValue, m_pSelectClanMasterRankImage, pszName, nRank);
}

void CStepClanBattleLobby::_SetSelectClanTeamLeader(const char * pszName, UINT8 nRank)
{
	SetClanMaster(m_pSelectClanTeamLeaderValue, m_pSelectClanTeamLeaderRankImage, pszName, nRank);
}

void CStepClanBattleLobby::_SetSelectClanRank(UINT32 nClanExp, INT32 nClanStaffNum, UINT8 nClanRank)
{
	SetClanRank(m_pSelectClanRankValue, m_pSelectClanRankImage, nClanExp, nClanStaffNum, nClanRank);
}

void CStepClanBattleLobby::_SetSelectClanOrder(INT32 nCurOrder, INT32 nPrevOrder)
{
	SetClanOrder(m_pSelectClanOrderValue, nCurOrder, nPrevOrder);
}

void CStepClanBattleLobby::_SetSelectClanAddress(const char * pszAddress)
{
#ifdef CLAN_ADDRESS_ENABLE
	m_pSelectClanAddressValue->SetText(pszAddress);
#else
	m_pSelectClanAddressStatic->SetEnable(FALSE);
	m_pSelectClanAddressValue->SetEnable(FALSE);
#endif
}

void CStepClanBattleLobby::_SetSelectClanExp(UINT32 nCurExp, UINT32 nMaxExp)
{
	REAL32 rx = ((CStageReady*)m_pParent)->GetGuiRoot()->getGuiResolutionWidth();

	SetClanExpText(m_pSelectClanExpValue, nCurExp, nMaxExp);
	SetClanExpGauge(m_pSelectClanExpBar, rx, nCurExp, nMaxExp);
}

void CStepClanBattleLobby::_SetSelectClanMark(UINT32 nFlag)
{
#if defined( USE_CLAN_MARK)
	::SetClanMarkTexEx(m_pSelectClanMarkImage, CLAN_MARK_MEDIUM, nFlag);
#endif
}

void CStepClanBattleLobby::_SetSelectClanInfo(CLAN_DETAIL_INFO * pSelect)
{
	CLAN_DETAIL_INFO * pInfo = g_pGameContext->getClanDetailInfo();
	if( pSelect != NULL && pInfo != NULL)
	{
		UINT32 wExp = pInfo->_exp;

		if( m_BattleTeamList.GetCount() > m_nSelectTeamSlot)
		{
			CLAN_BATTLE_TEAM_INFO * pSelectTeamInfo = (CLAN_BATTLE_TEAM_INFO*) m_BattleTeamList.GetItem(m_nSelectTeamSlot);
			_SetSelectClanTeamLeader( pSelectTeamInfo->_Member._Name, pSelectTeamInfo->_Member._Rank);
		}
		else
		{
			_SetSelectClanTeamLeader( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"), 0);	// "알수없음"
		}
		
		_SetSelectClanMark(pInfo->_mark);
		SetNameUsingColorTable( m_pSelectClanNameValue, pInfo->_name, pInfo->_color );
		_SetSelectClanStaff( pInfo->_currentPerson, pInfo->_maxPerson);
		_SetSelectClanMaster( pInfo->_masterNick, pInfo->_masterRank);
		_SetSelectClanRank( pInfo->_exp, pInfo->_currentPerson, pInfo->_expRank);

#ifdef CLAN_ADDRESS_ENABLE
		char temp[MAX_STRING_COUNT] = "";
		i3String::Copy( temp, g_pConfigEx->GetClan().AzitAddressBase, MAX_STRING_COUNT );
		i3String::Concat(temp, pInfo->_azitURL);
		_SetSelectClanAddress(temp);
#else
		m_pSelectClanAddressStatic->SetEnable(FALSE);
		m_pSelectClanAddressValue->SetEnable(FALSE);
#endif

		//_SetSelectClanOrder( pInfo->_clanRank, pInfo->_prevRank);		
		_SetSelectClanOrder( 0, 0);		
		
		if( wExp > DF_CLAN_RANK_L5 ) wExp = DF_CLAN_RANK_L5;
		UINT32 BeforeExp = GetClanBeforeRankExp( wExp );
		_SetSelectClanExp(wExp - BeforeExp, GetClanNextRankExp(wExp) - BeforeExp);	
	}
	else
	{		
		_SetSelectClanMark(0);
		_SetSelectClanName( GAME_STRING("STBL_IDX_CLAN_UNDECIDED"));	// "선택되지 않음"
		_SetSelectClanStaff( 0, 0);
		_SetSelectClanTeamLeader( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"), 0);	// "알수없음"
		_SetSelectClanMaster( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"), 0);	// "알수없음"
		_SetSelectClanRank( 0, 0, 0);

#ifdef CLAN_ADDRESS_ENABLE
		_SetSelectClanAddress( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "알수없음"		
#else
		m_pSelectClanAddressStatic->SetEnable(FALSE);
		m_pSelectClanAddressValue->SetEnable(FALSE);
#endif
		_SetSelectClanExp(0, 0);
		m_pSelectClanOrderValue->SetText( GAME_STRING("STBL_IDX_CLAN_UNKNOWN"));	// "알수없음"
	}
}

void CStepClanBattleLobby::cbQueryLeave(void * pThis, INT32 nParam)
{
	//	클랜전에서 나가기 처리
	if( nParam == MB_OK)
	{
		CStepClanBattleLobby * pLobby = (CStepClanBattleLobby*) pThis;

#if defined( TEST_CLAN_BATTLE)
		((CStageReady*) pLobby->getParent())->ChangeStep(STEP_CLAN_MEMBER);	
#else

		//	팀 나가기	
		g_pFramework->GetClanContext()->MatchTeamLeave();		//	클랜전 팀에서 나감
#endif
		
		((CStageReady*) pLobby->getParent())->SetMenuClanInputDisable(FALSE);
	}
}
