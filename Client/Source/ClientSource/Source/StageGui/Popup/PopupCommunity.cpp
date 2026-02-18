#include "pch.h"
#include "PopupCommunity.h"
#include "GlobalVariables.h"
#include "StageReady.h"
#include "ContextMenu.h"
#include "PhaseBase.h"
#include "StepBase.h"
#include "Shop.h"
#include "CommunityContext.h"
#include "UserLocationInfo.h"
#include "ServerInfo.h"
#include "Record.h"

#include <vector>
#include <algorithm>

#define USE_FRIEND_REFUSE

void PrintFriendState( UINT32 State, char * OutStr, UINT32 OutStrSize )
{
	switch(GET_FRIEND_STATE(State))
	{
	case FRIEND_STATE_REQUEST:	i3String::Copy(OutStr, GAME_STRING("STR_POPUP_SECRET"), OutStrSize );		/*비공개*/	break;
	case FRIEND_STATE_HOLD:		i3String::Copy(OutStr, GAME_STRING("STR_POPUP_SECRET"), OutStrSize );		/*비공개*/	break;
	case FRIEND_STATE_OFFLINE:	i3String::Copy(OutStr, GAME_STRING("STR_POPUP_OFFLINE_STATE"), OutStrSize );/*오프라인*/break;
	case FRIEND_STATE_ONLINE:	i3String::Copy(OutStr, GAME_STRING("STR_POPUP_READY_STATE"), OutStrSize );	/*대기중*/	break;
	case FRIEND_STATE_ROOM:
		{
			CUserLocationInfo	UserLocInfo(State);

			i3String::Format(OutStr, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_SERVER"),
				UserLocInfo.GetDisplayServerChannelNumber());/*%03d서버*/

			if (FRIEND_STATE_ROOM <= GET_FRIEND_STATE(State))
			{
				char room[MAX_STRING_COUNT];
				i3String::Format(room, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_ROOM"),
					UserLocInfo.GetDisplayRoomNumber());/* %d번 방*/
				i3String::Concat(OutStr, room);
			}
		}
		break;
	}
}

I3_CLASS_INSTANCE(CPopupCommunity, CPopupBase);

BOOL CPopupCommunity::m_bWarningNoteCount = FALSE;

CPopupCommunity::CPopupCommunity()
{
	m_pCommunityGui = NULL;
	m_pCommunityWin = NULL;
	m_pCommunityCaption = NULL;
	m_pContactChild = NULL;
	m_pNoteChild = NULL;
	m_pContactSect = NULL;
	m_pNoteSect = NULL;
	m_pContactTab = NULL;
	m_pNoteTab = NULL;
	m_pNoteKey = NULL;
	m_pNoteGageBg = NULL;
	m_pNoteGage = NULL;
	m_pLeave = NULL;

    m_pContactKey = NULL;
	m_pProfile = NULL;
	m_pFriend = NULL;
	m_pInvite = NULL;
	m_pContactClose = NULL;
	m_pFriendRefuse = NULL;
	m_pNicknameKey = NULL;
	m_pNicknameValue = NULL;
	m_pSeasonRecordKey = NULL;
	m_pSeasonRecord = NULL;
	m_pSeasonKillDeath = NULL;
	m_pSeasonHeadshot = NULL;
	m_pTotalRecordKey = NULL;
	m_pTotalRecord = NULL;
	m_pTotalKillDeath = NULL;
	m_pTotalHeadshot = NULL;
	m_pPositionKey = NULL;
	m_pPositionValue = NULL;

	i3mem::FillZero(m_pRecordTab, sizeof(m_pRecordTab));

	m_pUserDetailInfoPopup = NULL;
#ifdef USE_MODE_RECORD
	m_pCaption = NULL;

	// UserDetailInfoPopupFrame
	// User Info
	m_pUserDetailInfoRankMark = NULL;
	m_pUserDetailInfoNicknameValue = NULL;
	m_pUserDetailInfoRank = NULL;
	m_pUserDetailInfoClanName = NULL;	
	m_pUserDetailInfoTitle = NULL;	
	m_pUserDetailInfoPoint = NULL;
	m_pUserDetailInfoToll = NULL;

	m_pUserDetailInfoPositionKey = NULL;
	m_pUserDetailInfoPositionValue = NULL;

	// UserDetailInfoPopupFrame2
	// Total Record
	m_pUserDetailInfoTotalRecordKey = NULL;
	m_pUserDetailInfoTotalRecord = NULL;
	m_pUserDetailInfoTotalKillDeath = NULL;
	m_pUserDetailInfoTotalHeadshot = NULL;
	m_pUserDetailInfoTotalDis = NULL;
	m_pUserDetailInfoTotalUserRanking = NULL;

	// Season Record
	m_pUserDetailInfoSeasonRecordKey = NULL;
	m_pUserDetailInfoSeasonRecord = NULL;
	m_pUserDetailInfoSeasonKillDeath = NULL;
	m_pUserDetailInfoSeasonHeadshot = NULL;
	m_pUserDetailInfoSeasonDis = NULL;
	m_pUserDetailInfoSeasonUserRanking = NULL;

	// UserDetailInfoPopupFrame3		
	m_pModeRecordTab[0] = NULL;	// 통산, 시즌 tab
	m_pModeRecordTab[1] = NULL;	// 통산, 시즌 tab
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
	m_pUserDetailInfoProfile = NULL;
	m_pUserDetailInfoFriend = NULL;
	m_pUserDetailInfoInvite = NULL;
	m_pUserDetailInfoDetail = NULL;
	m_pUserDetailInfoContactClose = NULL;
	m_pUserDetailInfoFriendRefuse = NULL;

	m_bUserDetailInfoFlag = FALSE;	
	m_SelectUid = 0;

	//UserDetailInfoPopupMessageBox
	m_pUserDetailInfoMsg = NULL;
	m_pUserDetailInfoMsgCaption = NULL;
	m_pUserDetailInfoMsgMessage = NULL;	
	m_pUserDetailInfoMsgConfirm = NULL;
	m_pUserDetailInfoMsgCancel = NULL;

#endif
	//////////////////////////////////////////////////////////////////////////
	m_pFriendTab = NULL;
	m_pBlockTab = NULL;
	m_pClanTab = NULL;
	m_pFind = NULL;
	m_pAdd = NULL;
//	m_pDelete = NULL;
	m_pContactListName = NULL;
	m_pContactListState = NULL;
	m_pContactSelect = NULL;
	m_pContactFocus = NULL;
	m_pContactList = NULL;

	i3mem::FillZero(m_contactSlot, sizeof(m_contactSlot));

	m_pNoteListTab = NULL;
	m_pNoteDelete = NULL;
	m_pNoteWrite = NULL;
	m_pNoteSelect = NULL;
	m_pNoteFocus = NULL;
	m_pNoteListContent = NULL;
	m_pNoteListWho = NULL;
	m_pNoteListDate = NULL;
	
	i3mem::FillZero(m_noteSlot, sizeof(m_noteSlot));

	m_pNoteList = NULL;
	m_pNoteWhoKey = NULL;
	m_pNoteWhoValue = NULL;
	m_pNoteValue = NULL;
	m_pNoteSend = NULL;
	m_pNoteClose = NULL;
	m_pNoteAccept = NULL;
	m_pNoteReject = NULL;

	m_bReceiverUIDFlag = FALSE;
	m_iListIdx = 0;

	//	Gift note
	
	//
	m_pNicknameWin = NULL;
	m_pNicknameCaption = NULL;
	m_pNicknameMessage = NULL;
	m_pNicknameInput = NULL;
	m_pNicknameConfirm = NULL;
	m_pNicknameCancel = NULL;


	m_SelectedContact = -1;
	m_SelectedNote = -1;
	m_SelectedNoteIndex = -1;
	m_lastDeleteFriend = -1;

	m_nickPopupType = NPT_ADD_FRIEND;

	m_fNoteEmptyRate = 0.0f;
	for(UINT32 i = 0; i < MAX_NOTE_COUNT; i++)
		m_bNoteCheck[i] = FALSE;
	m_bAlarmCommunity = FALSE;
	m_bAlarmFlag = FALSE;
	m_rAlarmTerm = 0.0f;

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	m_PackageGoodsID		= 0;
	m_CurrentUserInfoUID	= 0;
	m_CurrentUserInfoType	= COMMUNITY_TAB_INVALID;
	
	m_SelectUid = 0;
	m_acceptFriendIdx = 0;

	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;
	m_currentTab = COMMUNITY_TAB_INVALID;

	m_nModeTypeComboIdx = 0;
	m_nNoteType = NOTE_TYPE_SEND;

	memset( m_nTypeMessage, 0, sizeof(m_nTypeMessage ) );
	memset( m_pModeRecordTab, 0, sizeof(m_pModeRecordTab ) );
	memset( m_pNoteGiftValue, 0, sizeof(m_pNoteGiftValue ) );
}

CPopupCommunity::~CPopupCommunity()
{
	/* Do nothing */
}

void CPopupCommunity::OnPreGuiNotify(void)
{
	CPopupBase::OnPreGuiNotify();

	// 커뮤니티
	{
		m_pContactFocus->SetEnable(FALSE);

		INT32 idxSlot = m_SelectedContact - m_pContactList->GetCurrentTop();

		if (0 <= idxSlot && idxSlot < MAX_SLOT_FOR_CONTACT_LIST)
		{
			m_pContactSelect->setPositionY(30.f + (idxSlot * 30.f));
			m_pContactSelect->SetEnable(TRUE);
		}
		else
		{
			m_pContactSelect->SetEnable(FALSE);
		}
	}

	// 쪽지
	{
		m_pNoteFocus->SetEnable(FALSE);
	
		INT32 idxSlot = m_SelectedNote - m_pNoteList->GetCurrentTop();

		if (0 <= idxSlot && idxSlot < MAX_NOTE_SLOT)
		{
			m_pNoteSelect->setPositionY(29.0f + (idxSlot * 24.0f));
			m_pNoteSelect->SetEnable(TRUE);
		}
		else
		{
			m_pNoteSelect->SetEnable(FALSE);
		}
	}
}

void CPopupCommunity::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_CMP_B_CONTACT_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnContactTab();
		break;
	case GCI_CMP_B_NOTE_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteTab();
		break;
	case GCI_CMP_B_LEAVE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnLeave();
		break;
	case GCI_CMP_B_PROFILE:
	case GCI_CMP_B_DETAIL_PROFILE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
			if(NULL == node) break;

			if(node)
			{
				switch(GET_FRIEND_STATE(node->_state))
				{
				//	등록 대기 친구면 수락한다.
				case FRIEND_STATE_HOLD:			OnProfile();		break;
				//	이미 등록된 친구라면 삭제한다.
				case FRIEND_STATE_REQUEST:
				case FRIEND_STATE_OFFLINE:
				case FRIEND_STATE_ONLINE:
				case FRIEND_STATE_ROOM:			OnDelete();			break;
				default:	break;
				}
			}
		}
		break;
	case GCI_CMP_B_FRIEND:
	case GCI_CMP_B_DETAIL_FRIEND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnVisit();
		break;
	case GCI_CMP_B_INVITE:
	case GCI_CMP_B_DETAIL_INVITE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnInviteRoom();
		break;
	#ifdef USE_MODE_RECORD	
	case GCI_CMP_B_DETAIL:		
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	
		{
			if(500 == SUBTRACTE_POINT)	OnUserDetailInfoPupupMsgConfirm();
			else						OnUserDetailInfo();
			
		}
		break;
	case GCI_CMP_B_DETAIL_CONFIRM:		
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfoPupupMsgConfirm();
		break;
	case GCI_CMP_B_DETAIL_CANCEL:		
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnUserDetailInfoPupupMsgCancel();
		break;
	case GCI_CMP_B_MODE_TOTAL_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnModeTab(FALSE);
		break;
	case GCI_CMP_B_MODE_SEASON_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnModeTab(TRUE);
		break;
	case GCI_CMP_B_MODE_TYPE:
		if(I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnTypeComboButton();
		break;		
	case GCI_CMP_P_MODE_TYPE_POPUP:
		if (I3GUI_POPUPLIST_NOTIFY_CLICKED == pNotify->m_nEvent)	OnTypeComboPopup(pNotify->m_nPriParam);
		break;
	#endif
	case GCI_CMP_B_CONTACT_CLOSE:
	case GCI_CMP_B_DETAIL_CONTACT_CLOSE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnContactClose();
		break;
	case GCI_CMP_B_FRIEND_REFUSE:
	case GCI_CMP_B_DETAIL_FRIEND_REFUSE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRefuseFriend();
		break;
    case GCI_CMP_B_FRIEND_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnFriendTab();
		break;
	case GCI_CMP_B_BLOCK_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnBlockTab();
		break;
	case GCI_CMP_B_CLAN_TAB:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnClanTab();
		break;
	case GCI_CMP_B_RECORD_TAB_1:
	case GCI_CMP_B_RECORD_TAB_2:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnRecordTab(pNotify->m_nID - GCI_CMP_B_RECORD_TAB_1);
		break;
	case GCI_CMP_B_FIND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnFind();
		break;
	case GCI_CMP_B_ADD:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnAdd();
		break;
	case GCI_CMP_B_DELETE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnDelete();
		break;
	case GCI_CMP_L_CONTACT_LIST:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) OnContactDrag();
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnContactFocus(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) OnContactSelect(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_RCLICK == pNotify->m_nEvent) OnContactContext(pNotify->m_nPriParam);
		break;
	case GCI_CMP_B_NOTE_DELETE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) STRING_POPUP_Q( GAME_STRING("STR_POPUP_Q_MESSAGE_DELETE_NOTE"), MAKE_CALLBACK(CbDeleteNote), this);/*선택한 쪽지를 삭제 하시겠습니까?*/
		break;
	case GCI_CMP_B_NOTE_WRITE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnWriteNote();
		break;
	case GCI_CMP_L_NOTE:
		if (I3GUI_LIST_NOTIFY_DRAG == pNotify->m_nEvent) g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
		if (I3GUI_LIST_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnNoteFocus(pNotify->m_nPriParam);
		if (I3GUI_LIST_NOTIFY_CLICKED == pNotify->m_nEvent) g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
		break;
	case GCI_CMP_B_SEND:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnSendNote();
		break;
	case GCI_CMP_B_ACCEPT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteAccept(TRUE);
		break;
	case GCI_CMP_B_REJECT:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteAccept(FALSE);
		break;
	case GCI_CMP_B_CLOSE:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNoteClose();
		break;
	case GCI_CMP_B_NICK_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNickConfirm();
		break;
	case GCI_CMP_B_NICK_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent) OnNickCancel();
		break;
	case GCI_CMP_C_NOTE_CHECK_1:	case GCI_CMP_C_NOTE_CHECK_2:	case GCI_CMP_C_NOTE_CHECK_3:
	case GCI_CMP_C_NOTE_CHECK_4:	case GCI_CMP_C_NOTE_CHECK_5:	case GCI_CMP_C_NOTE_CHECK_6:
	case GCI_CMP_C_NOTE_CHECK_7:	case GCI_CMP_C_NOTE_CHECK_8:	case GCI_CMP_C_NOTE_CHECK_9:
	case GCI_CMP_C_NOTE_CHECK_10:
		if (I3GUI_CHECK_NOTIFY_CLICKED == pNotify->m_nEvent) SetNoteCheck(pNotify->m_nID - GCI_CMP_C_NOTE_CHECK_1);
		if (I3GUI_CHECK_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnNoteFocus(pNotify->m_nID - GCI_CMP_C_NOTE_CHECK_1);
		break;

	case GCI_CMP_E_NOTE_CONTENT_1:	case GCI_CMP_E_NOTE_CONTENT_2:	case GCI_CMP_E_NOTE_CONTENT_3:
	case GCI_CMP_E_NOTE_CONTENT_4:	case GCI_CMP_E_NOTE_CONTENT_5:	case GCI_CMP_E_NOTE_CONTENT_6:
	case GCI_CMP_E_NOTE_CONTENT_7:	case GCI_CMP_E_NOTE_CONTENT_8:	case GCI_CMP_E_NOTE_CONTENT_9:
	case GCI_CMP_E_NOTE_CONTENT_10:
		if (I3GUI_STATIC_NOTIFY_CLICKED == pNotify->m_nEvent) SetSelectedNote(pNotify->m_nID - GCI_CMP_E_NOTE_CONTENT_1);
		if (I3GUI_STATIC_NOTIFY_ONMOUSE == pNotify->m_nEvent) OnNoteFocus(pNotify->m_nID - GCI_CMP_E_NOTE_CONTENT_1);
		break;
	}
}

BOOL CPopupCommunity::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	char strName[MAX_STRING_COUNT];

	LinkControlEx(plist, "Community", m_pCommunityGui, -1);
	LinkControlEx(plist, "cmCommunityWin", m_pCommunityWin, -1);
	LinkControlEx(plist, "cmCommunityCaption", m_pCommunityCaption, -1);
	LinkControlEx(plist, "cmContactChild", m_pContactChild, -1);
	LinkControlEx(plist, "cmNoteChild", m_pNoteChild, -1);
	LinkControlEx(plist, "cmContactSect", m_pContactSect, -1);
	LinkControlEx(plist, "cmNoteSect", m_pNoteSect, -1);
	LinkControlEx(plist, "cmContactTab", m_pContactTab, GCI_CMP_B_CONTACT_TAB);
	LinkControlEx(plist, "cmNoteTab", m_pNoteTab, GCI_CMP_B_NOTE_TAB);
	LinkControlEx(plist, "cmNoteKey", m_pNoteKey, -1);
	LinkControlEx(plist, "cmNoteGageBg", m_pNoteGageBg, -1);
	LinkControlEx(plist, "cmNoteGage", m_pNoteGage, -1);
	LinkControlEx(plist, "cmLeave", m_pLeave, GCI_CMP_B_LEAVE);

    LinkControlEx(plist, "cmContactKey", m_pContactKey, -1);
	LinkControlEx(plist, "cmProfile", m_pProfile, GCI_CMP_B_PROFILE);
	LinkControlEx(plist, "cmFriend", m_pFriend, GCI_CMP_B_FRIEND);
	LinkControlEx(plist, "cmInvite", m_pInvite, GCI_CMP_B_INVITE);
	LinkControlEx(plist, "cmContactClose", m_pContactClose, GCI_CMP_B_CONTACT_CLOSE);
	LinkControlEx(plist, "cmFriendRefuse", m_pFriendRefuse, GCI_CMP_B_FRIEND_REFUSE);
	LinkControlEx(plist, "cmNicknameKey", m_pNicknameKey, -1);
	LinkControlEx(plist, "cmNicknameValue", m_pNicknameValue, -1);
	LinkControlEx(plist, "cmSeasonRecordKey", m_pSeasonRecordKey, -1);
	LinkControlEx(plist, "cmSeasonRecord", m_pSeasonRecord, -1);
	LinkControlEx(plist, "cmSeasonKillDeath", m_pSeasonKillDeath, -1);
	LinkControlEx(plist, "cmSeasonHeadshot", m_pSeasonHeadshot, -1);
	LinkControlEx(plist, "cmTotalRecordKey", m_pTotalRecordKey, -1);
	LinkControlEx(plist, "cmTotalRecord", m_pTotalRecord, -1);
	LinkControlEx(plist, "cmTotalKillDeath", m_pTotalKillDeath, -1);
	LinkControlEx(plist, "cmTotalHeadshot", m_pTotalHeadshot, -1);
	LinkControlEx(plist, "cmPositionKey", m_pPositionKey, -1);
	LinkControlEx(plist, "cmPositionValue", m_pPositionValue, -1);

	for(INT32 i=0; i<2; i++)
	{
		sprintf_s(strName, "cmRecordTab%d", i+1);
		LinkControlEx(plist, strName, m_pRecordTab[i], GCI_CMP_B_RECORD_TAB_1+i);
	}

	LinkControlEx(plist, "cmFriendTab", m_pFriendTab, GCI_CMP_B_FRIEND_TAB);
	LinkControlEx(plist, "cmBlockTab", m_pBlockTab, GCI_CMP_B_BLOCK_TAB);
	LinkControlEx(plist, "cmClanTab", m_pClanTab, GCI_CMP_B_CLAN_TAB);
	LinkControlEx(plist, "cmFind", m_pFind, GCI_CMP_B_FIND);
	LinkControlEx(plist, "cmAdd", m_pAdd, GCI_CMP_B_ADD);
//	LinkControlEx(plist, "cmDelete", m_pDelete, GCI_CMP_B_DELETE);
	LinkControlEx(plist, "cmContactListName", m_pContactListName, -1);
	LinkControlEx(plist, "cmContactListState", m_pContactListState, -1);
	
	for(UINT32 i = 0; i < MAX_SLOT_FOR_CONTACT_LIST; i++)
	{
		sprintf_s(strName, "cmContactSlotMark%d", i + 1);
		LinkControlEx(plist, strName, m_contactSlot[i]._pMark, -1);

		sprintf_s(strName, "cmContactSlotNick%d", i + 1);
		LinkControlEx(plist, strName, m_contactSlot[i]._pNick, -1);

		sprintf_s(strName, "cmContactSlotState%d", i + 1);
		LinkControlEx(plist, strName, m_contactSlot[i]._pState, -1);
	}

    LinkControlEx(plist, "cmContactList", m_pContactList, GCI_CMP_L_CONTACT_LIST);
	LinkControlEx(plist, "cmContactSelect", m_pContactSelect, -1);
	LinkControlEx(plist, "cmContactFocus", m_pContactFocus, -1);

	LinkControlEx(plist, "cmNoteListTab", m_pNoteListTab, -1);
	LinkControlEx(plist, "cmNoteDelete", m_pNoteDelete, GCI_CMP_B_NOTE_DELETE);
	LinkControlEx(plist, "cmNoteWrite", m_pNoteWrite, GCI_CMP_B_NOTE_WRITE);
	LinkControlEx(plist, "cmNoteSelect", m_pNoteSelect, -1);
	LinkControlEx(plist, "cmNoteFocus", m_pNoteFocus, -1);
	LinkControlEx(plist, "cmNoteListContent", m_pNoteListContent, -1);
	LinkControlEx(plist, "cmNoteListWho", m_pNoteListWho, -1);
	LinkControlEx(plist, "cmNoteListDate", m_pNoteListDate, -1);
	
	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "cmNoteCheck%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pCheck, GCI_CMP_C_NOTE_CHECK_1+i);

		sprintf_s(strName, "cmNoteContent%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pContent, -1);

		sprintf_s(strName, "cmNoteWho%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pWho, -1);

		sprintf_s(strName, "cmNoteDate%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pDate, -1);

		sprintf_s(strName, "cmNoteSel%d", i + 1);
		LinkControlEx(plist, strName, m_noteSlot[i]._pSelect, GCI_CMP_E_NOTE_CONTENT_1+i);
	}
	
	LinkControlEx(plist,  "cmNoteGiftItemFrame", m_pNoteGiftItemFrame, -1);
	LinkControlEx(plist,  "cmNoteGiftWeaponImage", m_pNoteGiftWeaponImage, -1);
	LinkControlEx(plist,  "cmNoteGiftCharacterImage", m_pNoteGiftCharacterImage, -1);
	LinkControlEx(plist,  "cmNoteGiftCashItemImage", m_pNoteGiftCashItemImage, -1);
	LinkControlEx(plist,  "cmNoteGiftSeparate", m_pNoteGiftSeparate, -1);

	for(UINT32 i = 0; i < 2; i++)
	{
		char strName[MAX_STRING_COUNT];

		sprintf_s(strName, "cmNoteGiftValue%d", i + 1);
		LinkControlEx(plist, strName, m_pNoteGiftValue[i], -1);
	}

	LinkControlEx(plist, "cmNoteList", m_pNoteList, GCI_CMP_L_NOTE);
	LinkControlEx(plist, "cmNoteWhoKey", m_pNoteWhoKey, -1);
	LinkControlEx(plist, "cmNoteWhoValue", m_pNoteWhoValue, -1);
	LinkControlEx(plist, "cmNoteValue", m_pNoteValue, -1);
	LinkControlEx(plist, "cmNoteSend", m_pNoteSend, GCI_CMP_B_SEND);
	LinkControlEx(plist, "cmNoteClose", m_pNoteClose, GCI_CMP_B_CLOSE);
	LinkControlEx(plist, "cmNoteAccept", m_pNoteAccept, GCI_CMP_B_ACCEPT);
	LinkControlEx(plist, "cmNoteReject", m_pNoteReject, GCI_CMP_B_REJECT);

	LinkControlEx(plist, "cmNicknameWin", m_pNicknameWin, -1);
	LinkControlEx(plist, "cmNicknameCaption", m_pNicknameCaption, -1);
	LinkControlEx(plist, "cmNicknameMessage", m_pNicknameMessage, -1);
	LinkControlEx(plist, "cmNicknameInput", m_pNicknameInput, -1);
	LinkControlEx(plist, "cmNicknameConfirm", m_pNicknameConfirm, GCI_CMP_B_NICK_CONFIRM);
	LinkControlEx(plist, "cmNicknameCancel", m_pNicknameCancel, GCI_CMP_B_NICK_CANCEL);

	// UserDetailInfo
	LinkControlEx(plist,  "cmUserDetailInfoPopup", m_pUserDetailInfoPopup, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupCaption", m_pCaption, -1);

	// UserDetailInfoPopupFrame
	// User Info	
	LinkControlEx(plist,  "cmUserDetailInfoPopupNicknameValue", m_pUserDetailInfoNicknameValue, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupClanName", m_pUserDetailInfoClanName, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTitle", m_pUserDetailInfoTitle, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupRankValue", m_pUserDetailInfoRank, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupRank", m_pUserDetailInfoRankMark, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupPoint", m_pUserDetailInfoPoint, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupToll", m_pUserDetailInfoToll, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupPositionKey", m_pUserDetailInfoPositionKey, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupPositionValue", m_pUserDetailInfoPositionValue, -1);

	// UserDetailInfoPopupFrame2
	// Season Record
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonRecordKey", m_pUserDetailInfoSeasonRecordKey, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonRecord", m_pUserDetailInfoSeasonRecord, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonKillDeath", m_pUserDetailInfoSeasonKillDeath, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonHeadshot", m_pUserDetailInfoSeasonHeadshot, -1);	
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonDis", m_pUserDetailInfoSeasonDis, -1);	
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonUserRanking", m_pUserDetailInfoSeasonUserRanking, -1);	

	// Total Record
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalRecordKey", m_pUserDetailInfoTotalRecordKey, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalRecord", m_pUserDetailInfoTotalRecord, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalKillDeath", m_pUserDetailInfoTotalKillDeath, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalHeadshot", m_pUserDetailInfoTotalHeadshot, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalDis", m_pUserDetailInfoTotalDis, -1);	
	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalUserRanking", m_pUserDetailInfoTotalUserRanking, -1);	

	// UserDetailInfoPopupFrame3
	// Mode Record
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeRecord", m_pModeRecordValue, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeKillDeath", m_pModeKillDeath, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeHeadshot", m_pModeHeadshot, -1);
	LinkControlEx(plist,  "cmUserDetailInfoPopupModeAccuracyRate", m_pModeHitPercent, -1);

	LinkControlEx(plist,  "cmUserDetailInfoPopupTotalTab", m_pModeRecordTab[0], GCI_CMP_B_MODE_TOTAL_TAB);
	LinkControlEx(plist,  "cmUserDetailInfoPopupSeasonTab", m_pModeRecordTab[1], GCI_CMP_B_MODE_SEASON_TAB);

	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypeComboCaption", m_pModeTypeStatic, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypeCombo", m_pModeTypeCombo, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypeButton", m_pModeTypeButton, GCI_CMP_B_MODE_TYPE);
	LinkControlEx(plist, "cmUserDetailInfoPopupModeTypePopup", m_pModeTypePopup, GCI_CMP_P_MODE_TYPE_POPUP);

	// Button
	LinkControlEx(plist,  "cmUserDetailInfoPopupProfile", m_pUserDetailInfoProfile, GCI_CMP_B_DETAIL_PROFILE);
	LinkControlEx(plist,  "cmUserDetailInfoPopupFriend", m_pUserDetailInfoFriend, GCI_CMP_B_DETAIL_FRIEND);
	LinkControlEx(plist,  "cmUserDetailInfoPopupInvite", m_pUserDetailInfoInvite, GCI_CMP_B_DETAIL_INVITE);
	LinkControlEx(plist,  "cmUserDetailInfoPopupDetail", m_pUserDetailInfoDetail, GCI_CMP_B_DETAIL);
	LinkControlEx(plist,  "cmUserDetailInfoPopupClose", m_pUserDetailInfoContactClose, GCI_CMP_B_DETAIL_CONTACT_CLOSE);
	LinkControlEx(plist, "cmUserDetailInfoPopupFriendRefuse", m_pUserDetailInfoFriendRefuse, GCI_CMP_B_DETAIL_FRIEND_REFUSE);

	// UserDetailInfoPopupMessageBox
	LinkControlEx(plist, "cmUserDetailInfoPopupMsg", m_pUserDetailInfoMsg, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgCaption", m_pUserDetailInfoMsgCaption, -1);
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgMessage", m_pUserDetailInfoMsgMessage, -1);	
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgConfirm", m_pUserDetailInfoMsgConfirm, GCI_CMP_B_DETAIL_CONFIRM);
	LinkControlEx(plist, "cmUserDetailInfoPopupMsgCancel", m_pUserDetailInfoMsgCancel, GCI_CMP_B_DETAIL_CANCEL);
	

	// 쪽지용 리스트
	/*m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= STR_TBL_GUI_MESSAGER_CLAN_INVITE;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= STR_TBL_GUI_MESSAGER_CLAN_SECESSION;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= STR_TBL_GUI_MESSAGER_CLAN_MASTER;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= STR_TBL_GUI_MESSAGER_CLAN_STAFF;
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= STR_TBL_GUI_MESSAGER_CLAN_REGULAR;*/
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_ACCEPT ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_INVITE_DENIAL ]	= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_INVITE_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_ACCEPT ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_ACCEPT");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_JOIN_DENIAL ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_JOIN_DENIAL");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_SECESSION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_SECESSION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_DEPORTATION ]		= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_DEPORTATION");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_MASTER ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_MASTER");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_STAFF ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_STAFF");
	m_nTypeMessage[ NOTE_MESSAGE_CLAN_REGULAR ]			= GET_GAME_STRING_HANDLE("STR_TBL_GUI_MESSAGER_CLAN_REGULAR");
	return FALSE;
}

void CPopupCommunity::OnInitControl(void)
{
	_InitCommon();
	_InitContactSect();
	_InitContactChild();
	_InitUserDetailInfo();
	_InitNoteSect();
	_InitNoteChild();

	CPopupBase::OnInitControl();
}

void CPopupCommunity::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate(rDeltaSeconds);

	_UpdateContactList();
	_UpdateContactChild();
	_UpdateNote();
	

	// 노트알람~!
	_UpdateAlarmNote(rDeltaSeconds);
	
	if( GetInputDisable() == FALSE)
	{
		if (GUI_IME_CONFIRM(m_pNicknameInput))
		{
			if (m_pNicknameInput->getIsFocus())
			{
				OnNickConfirm();

				m_pNicknameInput->InputDone();
				m_pNicknameInput->SetFocus(FALSE);
				i3GuiRoot::setFocusControl(NULL);
			}
			else
			{
				m_pNicknameInput->InputDone();
			}
		}
	}
}

void CPopupCommunity::OnExitKey(void)
{
	OnClosePopup();
}

#ifdef USE_MODE_RECORD
void CPopupCommunity::OnModeTab(BOOL bModeFlag)
{
	if(m_bRecordTabFlag == bModeFlag) return;

	m_bRecordTabFlag = bModeFlag;	

	if(FALSE == m_bRecordTabFlag)
	{
		m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
		m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}
	else
	{		
		m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
		m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	}	
}

void CPopupCommunity::OnTypeComboButton(void)
{
	ExclusionPopup(m_pModeTypePopup);
}

void CPopupCommunity::OnTypeComboPopup(INT32 idx)
{
	m_nModeTypeComboIdx = idx; 
	char * pszSelect = (char*) m_pModeTypePopup->GetText(idx);
	m_pModeTypeCombo->SetText( pszSelect);
}
#endif

void CPopupCommunity::SetInputDisable(BOOL bDisable)
{
	m_pCommunityGui->setInputDisable(bDisable);
}

BOOL CPopupCommunity::GetInputDisable(void)
{
	return m_pCommunityGui->getInputDisable();
}

void CPopupCommunity::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	#ifdef USE_MODE_RECORD
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);	
	m_bRecordTabFlag = FALSE;	
	m_bUserDetailInfoFlag = FALSE;
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	#endif
	
	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_currentTab = COMMUNITY_FRIEND_TAB;

	SetCurrentUserInfoType(COMMUNITY_FRIEND_TAB);

	_UpdateContactChild();
	_UpdateContactList();

	m_SelectedContact = -1;
	m_SelectedNote = -1;
	m_pContactList->SetCurValue(0.0f);
	m_pNoteList->SetCurValue(0.0f);

//	m_pDelete->setControlDisable(TRUE);
	g_pCommunity->SetCommunityUserInfo(FALSE);

	m_pCommunityGui->SetEnable(TRUE);

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	//	현재 커뮤니티를 열때마다 선물 받은게 있나 체크하도록 요청합니다.
	//g_pFramework->GetShopContext()->RequestGiftList();

	SetPushedStateButton(MBT_COMMUNITY, TRUE);
}

void CPopupCommunity::OnClosePopup(void * pArg1, void * pArg2)
{
	#ifdef USE_MODE_RECORD
	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;	
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	// 상세 정보 보기 확인창을 닫는다.		
	m_pUserDetailInfoMsg->SetEnable(FALSE);
	ACTIVATE_BUTTON(m_pUserDetailInfoDetail);	
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	#endif

	CPopupBase::OnClosePopup();

	m_pCommunityGui->SetEnable(FALSE);

	m_pCommunityWin->setInputDisable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	SetPushedStateButton(MBT_COMMUNITY, FALSE);

	// Note관련
	ResetNote();
}

BOOL CPopupCommunity::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (pObj && pObj->IsExactTypeOf(CContextMenu::GetClassMeta()))
		{
			// 현재 선택되어있는 친구
			switch(event)
			{
			case CMC_WHISPER		:		OnWhisper();	break;
			case CMC_INVITE			:		OnInviteRoom();	break;
			case CMC_VISIT			:		OnVisit();		break;
			case CMC_ADD_FRIEND		:		OnAddFriend(); break;

			case CMC_CLAN_INVITE	:
				if ( ! CanClanInvite() )
					return FALSE;
				OnClanInvite();
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
				if (m_pNicknameWin->isEnable())
				{
					OnNickCancel();
					return TRUE;
				}

				#ifdef USE_MODE_RECORD
				if (m_pUserDetailInfoMsg->isEnable())
				{
					OnUserDetailInfoPupupMsgCancel();
					return TRUE;
				}

				if (m_pUserDetailInfoPopup->isEnable())
				{
					OnContactClose();
					return TRUE;
				}
				#else
				if (m_pContactChild->isEnable())
				{
					OnContactClose();
					return TRUE;
				}
				#endif

				if (m_pCommunityWin->isEnable())
				{
					OnLeave();
					return TRUE;
				}
			}

			if( InputKeyboardEnter())
			{
				if (m_pNicknameWin->isEnable())
				{
					BOOL nickFocus = m_pNicknameInput->getIsFocus();

					if (FALSE == nickFocus)
					{
						// ENTER키는 focus상태의 IME control에서 처리하기때문에,
						// focus상태의 IME control이 없을 경우에만 처리한다.

						OnNickConfirm();
						return TRUE;
					}
				}
			}
		}
	}

	I3ASSERT(pObj != NULL);
	return CPopupBase::OnEvent(event, pObj, param2, code);
}

void CPopupCommunity::OnGameEvent( INT32 event, INT32 arg)
{
}

void CPopupCommunity::OnWheelUpdate(INT32 scrollby)
{
	if (m_pContactSect->isEnable())
	{
		if (0 < scrollby)
		{
			for(INT32 i = 0; i < scrollby; i++)
			{
				m_pContactList->MovePrev();
			}
		}
		else
		{
			for(INT32 i = 0; i > scrollby; i--)
			{
				m_pContactList->MoveNext();
			}
		}

		_UpdateContactList();
	}
	else if (m_pNoteSect->isEnable())
	{
		if (0 < scrollby)
		{
			for(INT32 i = 0; i < scrollby; i++)
			{
				m_pNoteList->MovePrev();
			}
		}
		else
		{
			for(INT32 i = 0; i > scrollby; i--)
			{
				m_pNoteList->MoveNext();
			}
		}

		g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
		_UpdateNote();
	}
}

void CPopupCommunity::OnContactTab(void)
{
	m_pContactTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pNoteTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_pNoteSect->SetEnable(FALSE);
	m_pNoteGageBg->SetEnable( FALSE);
	m_pContactSect->SetEnable(TRUE);
	m_pNoteChild->SetEnable(FALSE);

	// 쪽지에서 사용되던 변수들을 초기화합니다.
	{
		m_SelectedNote = -1;
		m_SelectedNoteIndex = -1;
	}
}

void CPopupCommunity::OnNoteTab(void)
{
	if ( ! g_pConfigEx->GetCommunity().Enable_Note )
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));/*차후 구현 예정입니다.*/
		return;
	}

	m_pContactTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pNoteTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pNoteSect->SetEnable(TRUE);
	m_pNoteGageBg->SetEnable( TRUE);
	m_pContactSect->SetEnable(FALSE);

	g_pCommunity->SetCommunityUserInfo(FALSE);
	
	ReceiveAllNote();
	//
	EndAlarmNote();
}

void CPopupCommunity::OnLeave(void)
{
	OnClosePopup();
}

void CPopupCommunity::OnProfile(void)
{
	// dummy로 친구수락으로 일단 대체

	if (-1 != m_SelectedContact)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

		if(node && FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
		{
			m_acceptFriendIdx = m_SelectedContact;

			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_POPUP_Q_MESSAGE_ACCEPT_TO_FRINED"), node->_nick);/*[%s]님의 친구요청을 수락하시겠습니까?*/
			STRING_POPUP_Q(message, MAKE_CALLBACK(CbAcceptFriend), this);
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_READY_FRIEND"));/*등록대기 중인 친구를 선택하셔야합니다.*/
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_ACCEPT"));/*수락하실 친구를 선택하셔야합니다.*/
	}
}

//////////////////////////////////////////////////////////////////////////
#ifdef USE_MODE_RECORD
void CPopupCommunity::OnUserDetailInfo(void)
{
	m_pUserDetailInfoMsg->SetEnable(TRUE);
}

void CPopupCommunity::OnUserDetailInfoPupupMsgCancel(void)
{
	// 상세 정보 보기 확인창을 닫는다.	
	m_pUserDetailInfoMsg->SetEnable(FALSE);
}

void CPopupCommunity::OnUserDetailInfoPupupMsgConfirm(void)
{
	// 상세 정보 보기 확인창을 닫는다.	
	m_pUserDetailInfoMsg->SetEnable(FALSE);

	//
	m_bUserDetailInfoFlag = FALSE;
	USER_INFO_BASIC		MyUserInfoBasic;
	g_pGameContext->GetMyUserInfoBasic(&MyUserInfoBasic);

	if (SUBTRACTE_POINT > MyUserInfoBasic._point)
	{
		STRING_POPUP(GAME_STRING("STBL_IDX_CLAN_NO_POINT"));		//	"포인트가 부족합니다."
		return;
	}

	switch(m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:	// 친구
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
			if(NULL == node) break;

			// 자기 자신이면 자신의 정보 표시, 아니면 게임서버에 정보 요청
			if(i3String::Compare( MyUserInfoBasic._nick, node->_nick) == 0)
			{
				m_bUserDetailInfoFlag = TRUE;		
			}
			else
			{
				m_SelectUid = node->_uid;
				g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_ALL_DB_N, &m_SelectUid);
			}				
		}
		break;
	case COMMUNITY_CLAN_TAB:	// 클랜
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
			I3ASSERT(node != NULL);
			if (!node)	return;	

			// 자기 자신이면 자신의 정보 표시, 아니면 게임서버에 정보 요청
			if(i3String::Compare( MyUserInfoBasic._nick, node->_nick) == 0)
			{
				m_bUserDetailInfoFlag = TRUE;		
			}
			else
			{
				m_SelectUid = node->_uid;
				g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_ALL_DB_N, &m_SelectUid);
			}	
		}		
		break;
	default:
		{
			// Error
			return;
		}		
		break;
	}	

	DEACTIVATE_BUTTON( m_pUserDetailInfoDetail);
}
#endif

void CPopupCommunity::OnAddFriend(void)
{
	char nick[MAX_STRING_COUNT];
	// 클랜원을 친구로 추가
	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

	memcpy(nick, node->_nick, sizeof(char) * NET_NICK_NAME_SIZE);
	_AddFriend(nick);
}

void CPopupCommunity::OnVisit(void)
{
	//TODO: 찾아가기
	// 클랜 매치 중이면 찾아가기 실패
	if( g_pFramework->GetClanContext()->InBattleTeam() )
		return;

	const CServerInfo * pMyServerInfo = g_pServerInfo->GetCurGameServer();
	CHECK_ASSERT(pMyServerInfo != NULL);


	CUserLocationInfo	UserLocInfo;
	if (!GetCurSelUserLoccationInfo(UserLocInfo))
		return;

	const CServerInfo * pOtherServerInfo = g_pServerInfo->GetServerInfo( UserLocInfo.GetServerNumber() );
	CHECK_ASSERT(pOtherServerInfo != NULL);

	// 양쪽 모두 대회서버이면 이동이 가능하도록 한다.
	if ( pMyServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP ||
		pOtherServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP )
	{
		if ( pMyServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_VISIT_FROM_IN_CHAMPIONSHIP_USER"));
			return;
		}
		if ( pOtherServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_VISIT_TO_IN_CHAMPIONSHIP_USER"));
			return;
		}
	}


	switch( m_currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		OnVisitFriend();
		break;
	case COMMUNITY_CLAN_TAB:
		OnVisitClan();
		break;
	}
}

void CPopupCommunity::OnWhisper(void)
{
	char * pszNick = NULL;

	if( m_SelectedFriendNode._nick[0] != 0)
		pszNick = &m_SelectedFriendNode._nick[0];

	if( m_SelectedClanMemberNode._nick[0] != 0)
		pszNick = &m_SelectedClanMemberNode._nick[0];

	CStepBase * pStep =  ((CStageReady*) m_pParent)->GetCurrentStep();
	CPhaseBase * pPhase = ((CStageReady*) m_pParent)->GetCurrentPhase();
	CGameChatBody * pChatBody = NULL;

	//	GUI Phase 상위에 GUI Step이 떠 있으면 GUI Step을 채팅 보내기를 우선으로 한다.
	if( ((CStageReady*) m_pParent)->GetCurrentStepType() != STEP_NULL)
	{
		pChatBody = pStep->GetChatBody();

		if( pChatBody != NULL)
			pChatBody->OnWhisper(pszNick);
	}

	//	GUI Phase 상위에 떠 있는 GUI Step이 없으면 GUI Phase의 채팅으로 보낸다.
	if ( pPhase != NULL)
	{
		pChatBody = pPhase->GetChatBody();

		if( pChatBody != NULL)
			pChatBody->OnWhisper(pszNick);
	}
}

void CPopupCommunity::OnClanInvite(void)
{
	// 친구를 클랜원으로 초대
	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
	if(NULL == node) return;

	g_pFramework->GetClanContext()->SendClanInviteUID(node->_uid, 0);

	// 에러 코드
	//const CLAN_MEMBER_NODE* node = g_pCommunity->GetClanMember(m_SelectedContact);
	//g_pFramework->GetClanContext()->SendClanInviteUID( node->_uid, 0 );
}

void CPopupCommunity::OnInviteRoom(void)
{
	const CServerInfo * pMyServerInfo = g_pServerInfo->GetCurGameServer();
	CHECK_ASSERT(pMyServerInfo != NULL);


	CUserLocationInfo	UserLocInfo;
	if (!GetCurSelUserLoccationInfo(UserLocInfo))
		return;

	const CServerInfo * pOtherServerInfo = g_pServerInfo->GetServerInfo( UserLocInfo.GetServerNumber() );
	CHECK_ASSERT(pOtherServerInfo != NULL);

	if ( pMyServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP ||
		pOtherServerInfo->GetType() != S_CHANNEL_TYPE_CHAMPIONSHIP )
	{
		if ( pMyServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_INVITE_FROM_IN_CHAMPIONSHIP_USER"));
			return;
		}
		if ( pOtherServerInfo->GetType() == S_CHANNEL_TYPE_CHAMPIONSHIP )
		{
			SYSTEM_POPUP2(GAME_STRING("STR_TBL_GUI_CAN_NOT_INVITE_TO_IN_CHAMPIONSHIP_USER"));
			return;
		}
	}


	// 현재 매치 중인지 아닌지에 대한 체크
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		OnClanRoomInvite();
	}
	else
	{
		OnFriendInvite( m_currentTab );
	}
}


void CPopupCommunity::OnVisitFriend(void)
{
	//TODO: 찾아가기
	if (-1 != m_SelectedContact)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
		
		if (node && FRIEND_STATE_ROOM == GET_FRIEND_STATE(node->_state))
		{
			// 현재 레뒤룸에 있고, 서버, 체널 룸이 같으면 이미 방에 참여중입니다.
			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() && SameRoom)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
			else
			{
				g_pCommunity->SetVisitViewFriend(m_SelectedContact);
				((CStageReady*)m_pParent)->OnInviteMove();
			}
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*찾아가시려면 친구가 방에 있어야합니다.*/
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PLEASE_MESSAGE_FRIEND_TO_VISIT"));/*찾아갈 친구를 선택하셔야합니다.*/
	}
}

void CPopupCommunity::OnVisitClan(void)
{
	if (-1 == m_SelectedContact)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PLEASE_MESSAGE_FRIEND_TO_VISIT"));/*찾아갈 친구를 선택하셔야합니다.*/
		return;
	}

	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
	if( node == NULL )
		return;

	if( GET_FRIEND_MATCH( node->_state ) > 0 )
	{
		if ( FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state))
		{
			g_pCommunity->SetVisitViewClan(m_SelectedContact);
			((CStageReady*)m_pParent)->OnInviteMoveClan();
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CANNOT_VISIT_MATCHING"));/*매치 중에는 찾아가기를 하실 수 없습니다.*/
		}
	}
	else
	{
		if (node && FRIEND_STATE_ROOM == GET_FRIEND_STATE(node->_state))
		{
			// 현재 레뒤룸에 있고, 서버, 체널 룸이 같으면 이미 방에 참여중입니다.
			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() && SameRoom)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
			else
			{
				g_pCommunity->SetVisitViewClan(m_SelectedContact);
				((CStageReady*)m_pParent)->OnInviteMoveClan();
			}
		}
		else
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_FRIEND_IN_THE_ROOM"));/*찾아가시려면 친구가 방에 있어야합니다.*/
		}
	}


}

void CPopupCommunity::OnFriendInvite( COMMUNITY_TAB currentTab )
{
	if ( PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType() )
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_IN_ROOM"));/*방에서만 초대할 수 있습니다.*/
		return;
	}

	if (0 > m_SelectedContact)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*초대할 친구를 선택하셔야 합니다.*/
		return;
	}
	
	switch( currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

			if (NULL == node)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*초대할 친구를 선택하셔야 합니다.*/
				return;
			}

			if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
				return;
			}

			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) || SameRoom == FALSE)
			{
				g_pCommunity->InviteViewFriend(m_SelectedContact);
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

			if (NULL == node)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRIEND_TO_INVITE"));/*초대할 친구를 선택하셔야 합니다.*/
				return;
			}

			if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
				return;
			}

			BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
			if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) || SameRoom == FALSE)
			{
				g_pCommunity->InviteViewClan(m_SelectedContact);
			}
			else
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_ALREADY_JOIN_ROOM"));/*이미 방에 참여중입니다.*/
			}
		}
		break;
	}

}

void CPopupCommunity::OnClanRoomInvite(void)
{
  	if ( ( PHASE_LOBBY != ((CStageReady*)m_pParent)->GetCurrentPhaseType() ))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_CLAN_IN_LOBBY"));/*클랜 팀 로비에서만 초대할 수 있습니다.*/
		return;
	}

	if (0 > m_SelectedContact)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_CLAN_MEMBER"));/*초대할 클랜원을 선택하셔야 합니다.*/
		return;
	}

	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

	if (NULL == node)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_CLAN_MEMBER"));/*초대할 클랜원을 선택하셔야 합니다.*/
		return;
	}

	if (FRIEND_STATE_ONLINE > GET_FRIEND_STATE(node->_state))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_ONLINE_FRIEND"));/*접속 중인 친구만 초대할 수 있습니다.*/
		return;
	}

	if (FRIEND_STATE_ROOM != GET_FRIEND_STATE(node->_state) ||
		g_pServerInfo->GetCurGameServerIndex() != GET_FRIEND_SERVER(node->_state) ||
		g_pServerInfo->GetCurChannelNumber() != GET_FRIEND_CHANNEL(node->_state) ||
		g_pGameContext->getClanBattleMyTeamInfo()->_Team._Naming != GET_FRIEND_MATCH( node->_state )-1)
	{
		if (g_pCommunity->InviteViewClan(m_SelectedContact))
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_SEND_INVITE_MESSAGE"), node->_nick);/*[%s]님에게 초대 메시지를 전송하였습니다.*/
			STRING_POPUP(message);
		}
	}
	else
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_ALREADY_JOIN_TEAM"));/*이미 팀에 참여중입니다.*/
	}
}

void CPopupCommunity::OnContactClose(void)
{
	#ifdef USE_MODE_RECORD
	m_bRecordTabFlag = FALSE;
	m_bUserDetailInfoFlag = FALSE;	
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	// 상세 정보 보기 확인창을 닫는다.		
	m_pUserDetailInfoMsg->SetEnable(FALSE);	
	g_pCommunity->SetUserDetailInfoFlag(FALSE);
	ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	#endif

	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::OnFriendTab(void)
{
	OnContactClose();

  	m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_SEASON_HISTORY"));/*시즌전적*/
	m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_TOTAL_HISTORY"));/*통산전적*/
	m_pAdd->SetCaption( GAME_STRING("STR_POPUP_ADD_FRIEND"));/*친구추가*/

	m_pAdd->SetEnable( TRUE);
	m_pFind->SetEnable( TRUE);

	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
  	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_SelectedContact = -1;
	
	//	default
	#ifdef USE_MODE_RECORD
	m_pUserDetailInfoFriend->SetEnable(TRUE);
	m_pUserDetailInfoInvite->SetEnable(TRUE);
	m_pUserDetailInfoProfile->SetEnable(TRUE);
	m_pUserDetailInfoFriendRefuse->SetEnable(TRUE);
	#else
	m_pFriend->SetEnable(TRUE);
	m_pInvite->SetEnable(TRUE);
	m_pProfile->SetEnable(TRUE);
	#ifdef USE_FRIEND_REFUSE
	m_pFriendRefuse->SetEnable(TRUE);	
	#endif
	#endif

	m_currentTab = COMMUNITY_FRIEND_TAB;

	ACTIVATE_BUTTON( m_pFind );
	ACTIVATE_BUTTON( m_pAdd );
//	ACTIVATE_BUTTON( m_pDelete );

	for(INT32 i=0; i<2; i++)
		m_pRecordTab[i]->SetEnable(FALSE);

	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::OnBlockTab(void)
{
	OnContactClose();

#if defined( USE_COMMUNITY_BLOCK)
	m_pAdd->SetCaption( GAME_STRING("STR_POPUP_ADD_BLOCK"));/*차단추가*/

	m_pAdd->SetEnable( TRUE);
	m_pFind->SetEnable( TRUE);

	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	m_SelectedContact = -1;

	m_currentTab = COMMUNITY_BLOCK_TAB;

	ACTIVATE_BUTTON( m_pFind );
	ACTIVATE_BUTTON( m_pAdd );
//	ACTIVATE_BUTTON( m_pDelete );

	g_pCommunity->SetCommunityUserInfo(FALSE);
#else
	STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));/*차후 구현 예정입니다.*/
#endif
}

void CPopupCommunity::OnClanTab(void)
{
	if ( ! g_pConfigEx->GetClan().Enable )
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));		//차후 구현 예정입니다.
		return;
	}

	OnContactClose();

	m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_SEASON_HISTORY"));	//클랜 시즌전적
	m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_TOTAL_HISTORY"));	//클랜 통산전적

	m_pAdd->SetEnable( FALSE);
	m_pFind->SetEnable( FALSE);

    m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pBlockTab->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	m_pClanTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);
	
	m_pProfile->SetEnable(FALSE);
	m_pFriendRefuse->SetEnable(FALSE);

	#ifdef USE_MODE_RECORD
	m_pUserDetailInfoProfile->SetEnable( FALSE );
	m_pUserDetailInfoFriendRefuse->SetEnable( FALSE );
	#endif

	m_SelectedContact = -1;

	m_currentTab = COMMUNITY_CLAN_TAB;

	DEACTIVATE_BUTTON( m_pFind );
	DEACTIVATE_BUTTON( m_pAdd );
//	DEACTIVATE_BUTTON( m_pDelete );

	ACTIVATE_BUTTON( m_pRecordTab[0]);
	ACTIVATE_BUTTON( m_pRecordTab[1]);
	
	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::OnRecordTab(INT32 idx)
{
	const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
	I3ASSERT( node != NULL);

	switch(idx)
	{
	case 0:		//	클랜전적
		{
			// 현재 보고 있는 유저정보 요청은 스킵.
			if ( g_pCommunity->BeCommunityUserInfo() && GetCurrentUserInfoUID() == node->_uid && GetCurrentUserInfoType() == COMMUNITY_CLAN_TAB)
				break;

			m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
			m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);

			m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_SEASON_HISTORY"));/*클랜 시즌전적*/
			m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_CLAN_TOTAL_HISTORY"));/*클랜 통산전적*/

			g_pCommunity->SetCommnunityInfoNick(node->_nick);
			//g_pCommunity->SetCommnunityConnectState(((UINT32)node->_state));

			SetCurrentUserInfoUID(node->_uid);
			SetCurrentUserInfoType(COMMUNITY_CLAN_TAB);

			g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_DB_N, &node->_uid);
			// TODO : 클랜전적 기획 완료 후 작업 예정.
			//g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_CLAN_N, &node->_uid);
		}
		break;
	case 1:		//	개인전적
		{
			if ( g_pCommunity->BeCommunityUserInfo() && GetCurrentUserInfoUID() == node->_uid && GetCurrentUserInfoType() == COMMUNITY_FRIEND_TAB)
				break;

			m_pRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
			m_pRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_DISABLED);

			m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_PRIVATE_SEASON_HISTORY"));/*개인 시즌전적*/
			m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_PRIVATE_TOTAL_HISTORY"));/*개인 통산전적*/

			g_pCommunity->SetCommnunityInfoNick(node->_nick);
			//g_pCommunity->SetCommnunityConnectState(node->_state);

			SetCurrentUserInfoUID(node->_uid);
			SetCurrentUserInfoType(COMMUNITY_FRIEND_TAB);
			g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_DB_N, &node->_uid);
		}
		break;
	}
}

void CPopupCommunity::OnFind(void)
{
#if 0
	STRING_POPUP(GAME_STRING("STR_POPUP_PREPARE_PLAN"));/*차후 구현 예정입니다.*/
#else
	m_pCommunityWin->setInputDisable(TRUE);

	m_pNicknameCaption->SetText(GAME_STRING("STR_POPUP_FIND_USER"));/*유저찾기*/
	m_pNicknameMessage->SetText(GAME_STRING("STR_POPUP_MESSAGE_FINDING_NAME"));/*찾을 사용자의 닉네임을\n입력하십시오.*/

	m_pNicknameWin->SetEnable(TRUE);
	m_pNicknameInput->SetText("");
	m_pNicknameInput->SetIMEText("");
	m_pNicknameInput->SetFocus(TRUE);

	m_nickPopupType = NPT_FIND_USER;
#endif
}

void CPopupCommunity::OnAdd(void)
{
	m_pCommunityWin->setInputDisable(TRUE);

	switch(m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			m_pNicknameCaption->SetText(GAME_STRING("STR_POPUP_ADD_FRIEND"));/*친구추가*/
			m_pNicknameMessage->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_FRIEND_NAME"));/*친구목록에 추가할 사용자의 닉네임을\n입력하십시오.*/

			m_pNicknameWin->SetEnable(TRUE);
			m_pNicknameInput->SetText("");
			m_pNicknameInput->SetIMEText("");
			m_pNicknameInput->SetFocus(TRUE);

			m_nickPopupType = NPT_ADD_FRIEND;
		}
		break;
	case COMMUNITY_BLOCK_TAB:
		{
			m_pNicknameCaption->SetText(GAME_STRING("STR_POPUP_ADD_BLOCK"));/*목록추가*/
			m_pNicknameMessage->SetText(GAME_STRING("STR_POPUP_MESSAGE_INPUT_BLOCK_NAME"));/*차단 목록에 추가할 사용자의 닉네임을 입력 하십시오.*/

			m_pNicknameWin->SetEnable(TRUE);
			m_pNicknameInput->SetText("");
			m_pNicknameInput->SetIMEText("");
			m_pNicknameInput->SetFocus(TRUE);

			m_nickPopupType = NPT_ADD_BLOCK;
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			//
		}
		break;
	}

}

void CPopupCommunity::OnDelete(void)
{
	m_lastDeleteFriend = m_SelectedContact;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

	if (NULL == node)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*삭제할 친구를 선택하셔야합니다.*/
	}
	else
	{
		char message[MAX_STRING_COUNT];

		sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_DELETE_FRIEND"), node->_nick);/*[%s]님을 친구목록에서 삭제합니다.*/
		STRING_POPUP_Q(message, MAKE_CALLBACK(CbDeleteFriend), this);
	}
}

void CPopupCommunity::OnRefuseFriend(void)
{
	m_lastDeleteFriend = m_SelectedContact;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);

	if (NULL == node)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_REFUSE"));/*친구요청을 거절할 친구를 선택하셔야합니다.*/
	}
	else
	{
		char message[MAX_STRING_COUNT];

		sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_REFUSE_FRIEND"), node->_nick);/*[%s]님의 친구요청을 거절하시겠습니까?*/
		STRING_POPUP_Q(message, MAKE_CALLBACK(CbDeleteFriend), this);
	}
}

void CPopupCommunity::OnContactDrag(void)
{
	_UpdateContactList();
}

void CPopupCommunity::OnContactFocus(UINT32 idxItem)
{
	I3_BOUNDCHK( idxItem, MAX_SLOT_FOR_CONTACT_LIST);
	m_pContactFocus->setPositionY(30.f + (idxItem * 30.f));
	m_pContactFocus->SetEnable(TRUE);
}

void CPopupCommunity::OnContactSelect(UINT32 idxItem)
{
	m_SelectedContact = idxItem;

	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

//	m_pDelete->setControlDisable(FALSE);
	#ifdef USE_MODE_RECORD
	if(TRUE == m_bUserDetailInfoFlag)
	{
		DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	}
	else
	{
		ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	}
	#endif
	
	switch( m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			FRIEND_NODE* node = (FRIEND_NODE*) g_pCommunity->GetViewFriend(idxItem);			

			if (node)
			{
				if ( g_pCommunity->BeCommunityUserInfo() && GetCurrentUserInfoUID() == node->_uid && GetCurrentUserInfoType() == COMMUNITY_FRIEND_TAB)
					break;

				#ifdef USE_MODE_RECORD
				m_bRecordTabFlag = FALSE;
				m_bUserDetailInfoFlag = FALSE;	
				m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
				m_pModeRecordTab[1]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
				// 상세 정보 보기 확인창을 닫는다.		
				m_pUserDetailInfoMsg->SetEnable(FALSE);	
				g_pCommunity->SetUserDetailInfoFlag(FALSE);
				ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
				#endif

				node->CopyTo( &m_SelectedFriendNode);

				g_pCommunity->SetCommnunityInfoNick(node->_nick);
				g_pCommunity->SetCommnunityConnectState(node->_state);

				SetCurrentUserInfoUID(node->_uid);
				SetCurrentUserInfoType(COMMUNITY_FRIEND_TAB);
				g_pGameContext->SetEvent(EVENT_COMMUNITY_RECORD_DB_N, &node->_uid);
			}
			else
			{
				OnContactClose();				
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			CLAN_MEMBER_NODE* node = (CLAN_MEMBER_NODE*) g_pCommunity->GetViewClanMember(idxItem);
	
			if (node)
			{
				node->CopyTo( &m_SelectedClanMemberNode);

				OnRecordTab(0);			//	default to clan score
			}
		}
		break;
	}

	PlaySound(GUI_SND_ITEM_SELECTING);
}

void CPopupCommunity::OnContactAccept(UINT32 idxItem)
{
#if 0
	if (idxItem >= g_pCommunity->GetFriendCount())	return;

	m_SelectedContact = idxItem;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend(idxItem);
	if(NULL == node) return;

	if (FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
	{
		m_acceptFriendIdx = idxItem;

		char message[MAX_STRING_COUNT];

		sprintf_s(message, GAME_STRING("STR_POPUP_Q_MESSAGE_ACCEPT_TO_FRINED"), node->_nick);/*[%s]님의 친구요청을 수락하시겠습니까?*/
		STRING_POPUP_Q(message, CbAcceptFriend, this);
	}
	else
	{
		OnContactSelect(idxItem);
	}
#endif
}

BOOL CPopupCommunity::_IsMyFriend( INT64 uid)
{
	for(UINT32 i=0; i<g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* pNode = g_pCommunity->GetViewFriend(i);
		if(NULL == pNode) continue;

		if( pNode->_uid == uid)
			return TRUE;
	}

	return FALSE;
}

BOOL CPopupCommunity::_IsMyClanMember( INT64 uid)
{
	for(UINT32 i=0; i<g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = g_pCommunity->GetViewClanMember(i);

		if( pNode->_uid == uid)		return TRUE;
	}

	return FALSE;
}

BOOL CPopupCommunity::_CheckCanWhisper(FRIEND_STATE State)
{
	//	접속되어 있으며
	if( State != FRIEND_STATE_OFFLINE)
	{
		CStepBase * pStep =  ((CStageReady*) m_pParent)->GetCurrentStep();
		CPhaseBase * pPhase = ((CStageReady*) m_pParent)->GetCurrentPhase();
		CGameChatBody * pChatBody = NULL;

		//	GUI Phase 상위에 GUI Step이 떠 있으면 GUI Step을 채팅 보내기를 우선으로 한다.
		if( ((CStageReady*) m_pParent)->GetCurrentStepType() != STEP_NULL)
		{
			pChatBody = pStep->GetChatBody();

			if( pChatBody != NULL)		return TRUE;
		}

		//	GUI Phase 상위에 떠 있는 GUI Step이 없으면 GUI Phase의 채팅으로 보낸다.
		if ( pPhase != NULL)
		{
			pChatBody = pPhase->GetChatBody();

			if( pChatBody != NULL)		return TRUE;
		}
	}

	return FALSE;
}

void CPopupCommunity::OnContactContext(UINT32 idxItem)
{
	m_SelectedFriendNode.Reset();
	m_SelectedClanMemberNode.Reset();

	m_SelectedContact = idxItem;

	ContextMenuReset();

	switch( m_currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		{
			if( g_pCommunity->GetFriendCount() <= idxItem)
			{
				m_SelectedContact = -1;
				return;
			}

			FRIEND_NODE* node = (FRIEND_NODE*) g_pCommunity->GetViewFriend(idxItem);
			if(NULL == node) break;

			node->CopyTo( &m_SelectedFriendNode);

			if( _CheckCanWhisper((FRIEND_STATE)GET_FRIEND_STATE(node->_state)))
				ContextMenuAddCommand(CMC_WHISPER);

			switch(GET_FRIEND_STATE(node->_state))
			{
			case FRIEND_STATE_ONLINE:

				if( TRUE == g_pFramework->GetClanContext()->InBattleTeam() )
					break;

				if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType())
					break;

				ContextMenuAddCommand(CMC_INVITE);
				break;

			case FRIEND_STATE_ROOM:
				if( TRUE == g_pFramework->GetClanContext()->InBattleTeam() )
					break;

				if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType())
				{
					ContextMenuAddCommand(CMC_VISIT);
					break;
				}

				BOOL	SameRoom = g_pGameContext->IsSameRoom(node->_state);
				// 같은 방이 아니라면 초대및 찾아가기
				if (SameRoom == FALSE)
				{
					ContextMenuAddCommand(CMC_INVITE);
					ContextMenuAddCommand(CMC_VISIT);
				}
				break;
			}

			// 클랜 초대권한 있으면 클랜초대
			if ( CanClanInvite() )
			{
				if ((g_pFramework->GetClanContext()->IsPossibleClanInvite()) &&
					(!_IsMyClanMember( node->_uid)))
				{
					ContextMenuAddCommand(CMC_CLAN_INVITE);
				}
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			if( g_pCommunity->GetClanMemberCount() <= idxItem)
			{
				m_SelectedContact = -1;
				return;
			}

			CLAN_MEMBER_NODE* node = (CLAN_MEMBER_NODE*) g_pCommunity->GetViewClanMember(idxItem);

			node->CopyTo( &m_SelectedClanMemberNode);

			// 클랜 상태 64 를 친구와 같이 32로 재 정의 합니다.
			UINT32 ui32State = FRIEND_STATE_64_TO_32(node->_state);
			if( _CheckCanWhisper( (FRIEND_STATE)GET_FRIEND_STATE(ui32State) ))
				ContextMenuAddCommand(CMC_WHISPER);

			// 클랜원이 친구가 아닌 경우
			// 친구 추가 가능
			if ( !_IsMyFriend( node->_uid))
			{
				ContextMenuAddCommand(CMC_ADD_FRIEND);
			}

			switch(GET_FRIEND_STATE(node->_state))
			{
				//친구가 마루에 있을떄
			case FRIEND_STATE_ONLINE:
				{
					if( GET_FRIEND_MATCH( node->_state ) > 0 )			// 대상이 배틀 팀 상태일때
					{
						if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() ) // 내가 팀에 없으면
						{	// 찾아가기 됨
							ContextMenuAddCommand(CMC_VISIT);
						}
					}
					else												// 대상이 배틀 팀 상태 아닐때
					{
						// 초대하기 됨
						if( PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType() )
						{	// 찾아가기 됨
							ContextMenuAddCommand(CMC_INVITE);
						}
						else
						{
							if( g_pFramework->GetClanContext()->InBattleTeam() )
							{
								ContextMenuAddCommand(CMC_INVITE);
							}
						}
					}
				}
				break;
				//	친구가 방에 있을때
			case FRIEND_STATE_ROOM:
				{
					if( GET_FRIEND_MATCH( node->_state ) == 0 )			// 대상이 배틀 팀 상태가 아닐때
					{
						if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
						{
							//	자신이 방에 들어가 있으면 친구초대 활성
							if(PHASE_READY_ROOM == ((CStageReady*)m_pParent)->GetCurrentPhaseType())
								ContextMenuAddCommand(CMC_INVITE);

							ContextMenuAddCommand(CMC_VISIT);		//	친구 찾아가기 활성
						}
						else
						{
							ContextMenuAddCommand(CMC_INVITE);
						}
					}
				}
				break;
			}
		}
		break;
	}

#if defined(CHANGE_ADDCOMMAND_METHOD)
	INT32 OldCmdMap[64] = {};
	INT32 NewCmdMap[64] = {};
	for ( INT32 i = 0 ; i < 64 ; ++ i )
	{
		OldCmdMap[i] = NewCmdMap[i] = 0xff;
	}

	((CStageReady*)m_pParent)->CopyContextMenuList(OldCmdMap);

	ContextMenuReset();

	{
		UINT64	State = 0;
		INT64	UID = 0;
		switch(m_currentTab)
		{
		case COMMUNITY_FRIEND_TAB:
			{
				FRIEND_NODE* node = (FRIEND_NODE*) g_pCommunity->GetViewFriend(idxItem);
				if(NULL == node) break;

				node->CopyTo( &m_SelectedFriendNode);
				State	= node->_state;
				UID		= node->_uid;
			}
			break;
		case COMMUNITY_CLAN_TAB:
			{
				CLAN_MEMBER_NODE* node = (CLAN_MEMBER_NODE*) g_pCommunity->GetViewClanMember(idxItem);

				node->CopyTo( &m_SelectedClanMemberNode);
				State	= node->_state;
				UID		= node->_uid;
			}
			break;
		}

		CUserLocationInfo	LocInfo(State);

		AddPopupCommand(CMC_ADD_FRIEND, LocInfo, UID);
		AddPopupCommand(CMC_CLAN_INVITE, LocInfo, UID);
		AddPopupCommand(CMC_INVITE, LocInfo, UID);
		AddPopupCommand(CMC_VISIT, LocInfo, UID);
		AddPopupCommand(CMC_WHISPER, LocInfo, UID);
	}


	((CStageReady*)m_pParent)->CopyContextMenuList(NewCmdMap);

	std::vector<INT32>	vNewCmpMap(&NewCmdMap[0], &NewCmdMap[64]);
	std::vector<INT32>	vOldCmpMap(&OldCmdMap[0], &OldCmdMap[64]);

	std::sort(vNewCmpMap.begin(), vNewCmpMap.end());
	std::sort(vOldCmpMap.begin(), vOldCmpMap.end());

	if ( vNewCmpMap != vOldCmpMap )
	{
		string	strNewCmd, strOldCmd;
		for ( size_t i = 0 ; i < vNewCmpMap.size() ; ++i )
		{
			if (vNewCmpMap[i] == 0xff )
				continue;
			char str[64] = {};
			i3String::Format(str, 64, "%d, ", vNewCmpMap[i]);
			strNewCmd += str;
		}

		strNewCmd += "\n\n";

		for ( size_t i = 0 ; i < vOldCmpMap.size() ; ++i )
		{
			if (vOldCmpMap[i] == 0xff )
				continue;
			char str[64] = {};
			i3String::Format(str, 64, "%d, ", vOldCmpMap[i]);
			strOldCmd += str;
		}

		I3FATAL("[Dev Msg]수정된 팝업 메뉴 잘못되었습니다. 무시하고 진행해도 됩니다.\n%s\n%s", strNewCmd.c_str(), strOldCmd.c_str());
	}

#endif

	ContextMenuOpen(this, idxItem);
}

void CPopupCommunity::OnDeleteNote(void)
{
	INT32 i = 0;
	UINT8 DeleteCount = 0;
	INT32 DeleteArray[MAX_NOTE_COUNT];
	
	NOTE_RECEIVE* pNote = NULL;

	for( i=g_pGameContext->GetNoteReceiveCount() - 1; i>=0; i--)
	{
		if( m_bNoteCheck[i])
		{
			// if it Checked...
			pNote = g_pGameContext->GetReceiveNote( i);

			//	선물쪽지는 따로 관리되므로 여기서 삭제되면 안됩니다. komet
			if( pNote->header._type != NT_GIFT)
			{
				DeleteArray[DeleteCount] = pNote->header._id;
				DeleteCount++;
			}
		}
	}

	// 체크박스로 선택되어진것들뿐만 아니라, 현재 페이지가 열려있는 쪽지에 대해서도 삭제처리를 진행합니다.
	if( m_SelectedNoteIndex != -1)
	{
		UINT32 TopIdx = m_pNoteList->GetCurrentTop();
		INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (m_SelectedNoteIndex+1)-TopIdx;
		if( m_bNoteCheck[nListIdx] == FALSE)
		{
			// if it Checked...
			pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

			//	선물쪽지는 따로 관리되므로 여기서 삭제되면 안됩니다. komet
			if( pNote->header._type != NT_GIFT)
			{
				DeleteArray[DeleteCount] = pNote->header._id;
				DeleteCount++;
			}
		}
	}

	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		m_noteSlot[i]._pCheck->setCheckBoxState( FALSE);
	}

	for(UINT32 i = 0; i < MAX_NOTE_COUNT; i++) m_bNoteCheck[i] = FALSE;

	if( DeleteCount>0)
	{
		g_pGameContext->SetEvent( EVENT_DELETE_NOTE, &DeleteCount, DeleteArray);

		OnNoteClose();
	}
}

void CPopupCommunity::OnWriteNote(void)
{
	SetNoteType( NOTE_TYPE_SEND);
	m_pNoteWhoValue->SetIMEText( "");
	m_pNoteValue->SetIMEText( "");

	m_pNoteAccept->SetEnable(FALSE);
	m_pNoteReject->SetEnable(FALSE);
	m_pNoteSend->SetEnable(TRUE);
	m_pNoteValue->SetEnable(TRUE);
}

void CPopupCommunity::OnNoteFocus(UINT32 idxItem)
{
	if( g_pGameContext->GetNoteReceiveCount() >= (INT32)(idxItem+1))
	{
		m_pNoteFocus->setPositionY(29.0f + (idxItem * 24.0f));
		m_pNoteFocus->SetEnable(TRUE);
	}
}

void CPopupCommunity::OnNoteSelect(UINT32 idxItem)
{
	// 이코드는 사용되지 않습니다.
	{
		UINT8 ReadCount = 0;
		UINT32 TopIdx = m_pNoteList->GetCurrentTop();

		//
		PlaySound( GUI_SND_ITEM_SELECTING);

		ReceiveAllNote();

		INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (idxItem+1) - TopIdx;

		SetNoteType( NOTE_TYPE_REPLY);

		NOTE_RECEIVE* pNote = NULL;
		pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

		// 읽음상태로 변경합니다.
		pNote->header._state = NS_READED;

		//
		m_pNoteWhoValue->SetIMEText(pNote->body._sender);
	
		//
		m_pNoteValue->SetIMEText(pNote->body._message);

		m_noteSlot[idxItem]._pContent->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxItem]._pWho->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxItem]._pDate->SetTextColor(GetColor(GCT_NOTIFY));

		ReadCount++;
		g_pGameContext->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
	}

}

void CPopupCommunity::OnSendNote(void)
{
	switch( m_nNoteType)
	{
	case NOTE_TYPE_SEND:
		{
			if(i3String::Length( m_pNoteWhoValue->GetText()) <= 0)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_RECEIVER"));/*받는 사람을 입력해 주세요.*/
			}
			else if(i3String::Length( m_pNoteValue->GetText()) <= 0)
			{
				STRING_POPUP(GAME_STRING("STR_POPUP_INPUT_NOTE"));/*쪽지 내용을 입력해 주세요.*/
			}
			else
			{
				if ( i3String::CompareCase( g_pGameContext->GetMyNickName(), m_pNoteWhoValue->GetText()) == 0)
				{
					STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_SEND_SELF"));/*자신에게 쪽지를 보낼 수 없습니다.*/
					m_pNoteWhoValue->SetIMEText( "");
					m_pNoteWhoValue->SetFocus( TRUE);
				}
				else
				{
					if( g_pFramework->IsFilter(m_pNoteValue->GetText()) == FALSE && m_nNoteType == NOTE_TYPE_SEND)
					{
						STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
						m_pNoteValue->SetFocus( TRUE);
					}
					else
					{
						m_pNoteChild->SetEnable(FALSE);

						// 쪽지 보내기
				#ifdef USE_MESS_TEST
						if (FALSE == m_bReceiverUIDFlag)
						{
							NOTE_BODY note;
							i3String::Copy( note._sender, m_pNoteWhoValue->GetText(), NET_NICK_NAME_SIZE );
							i3String::Copy( note._message, m_pNoteValue->GetText(), NET_NOTE_SIZE );
							note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
							note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
	
							g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
						}
						else
						{
							NOTE_BODY_BY_UID note;
							char Sender[NET_NICK_NAME_SIZE];

							m_bReceiverUIDFlag = FALSE;

							i3String::Copy( Sender, m_pNoteWhoValue->GetText(), NET_NICK_NAME_SIZE );

							// 쪽지 목록으로 확인
							note._receiverUid = _FindNoteByUid(Sender);
							if (0 >= note._receiverUid)
							{
								break;
							}
							i3String::Copy( note._message, m_pNoteValue->GetText(), NET_NOTE_SIZE );
	
							note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

							g_pGameContext->SetEvent( EVENT_SEND_NOTE_BY_UID, &note);
						}
				#else
						NOTE_BODY note;
						i3String::Copy( note._sender, m_pNoteWhoValue->GetText());
						i3String::Copy( note._message, m_pNoteValue->GetText());
						note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
						note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

						g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
				#endif
					}
				}
			}
		}
		break;
	case NOTE_TYPE_REPLY:
	case NOTE_TYPE_GIFT:
		{
			m_bReceiverUIDFlag = TRUE;
			// 답장하기
			SetNoteType( NOTE_TYPE_SEND);

			m_pNoteValue->SetEnable(TRUE);
			m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pNoteValue->SetIMEText( "");
			m_pNoteValue->SetFocus( TRUE);
		}
		break;
	}
}

void CPopupCommunity::OnNoteClose(void)
{
	m_SelectedNote = -1;
	m_SelectedNoteIndex = -1;
	m_pContactList->SetCurValue(0.0f);
	m_pNoteList->SetCurValue(0.0f);
	ReceiveAllNote();

	m_pNoteChild->SetEnable(FALSE);
}

void CPopupCommunity::OnNoteAccept(BOOL bVal)
{
	UINT32 TopIdx = m_pNoteList->GetCurrentTop();
	INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (m_SelectedNoteIndex+1)-TopIdx;

	if( 0 > nListIdx || nListIdx > g_pGameContext->GetNoteReceiveCount() - 1)
	{
		I3ASSERT_0;
		return;
	}
	
	// if it Checked...
	NOTE_RECEIVE * pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

	//	클랜 가입 요청일 겨우
	if(pNote->header._type == NT_CLAN_ASK)
	{
		UINT32 wClanDBIdx = (INT32) pNote->header._appendingData;

		g_pFramework->GetClanContext()->SendClanInviteAccept(wClanDBIdx, (UINT8) bVal);

		// 읽음상태로 변경합니다.
		pNote->header._state = NS_READED;

		// 읽은 상태의 쪽지는 남은기간을 7일로 설정합니다.
		pNote->header._remainDay = 7;

		INT32 ReadCount = 1;
		g_pGameContext->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
	}

	OnNoteClose();
}

void CPopupCommunity::OnNickConfirm(void)
{
	// 닉네임 입력창을 종료한다.
	m_pCommunityWin->setInputDisable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);

	
	// 입력된 닉네임을 읽어온다.
	char nick[MAX_STRING_COUNT];

	i3String::Copy( nick, m_pNicknameInput->getIMETextBuf(), MAX_STRING_COUNT );

	if (0 == i3String::Length(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_NAME"));/*닉네임을 입력하셔야 합니다.*/
		return;
	}

	if (FALSE == CStringFilter::IsValidNickname(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_LIMITED_NAME_LENGTH"));/*닉네임은 2자 이상 16자 이하만 가능합니다.*/
		return;
	}


	// 해당 기능을 처리한다
	switch(m_nickPopupType)
	{
	case NPT_ADD_FRIEND:
		_AddFriend(nick);
		break;
	case NPT_FIND_USER:
		_FindUser(nick);
		break;
	case NPT_ADD_BLOCK:
		_AddBlock(nick);
		break;
	default:
		I3ASSERT_0;
		break;
	}
}

void CPopupCommunity::OnNickCancel(void)
{
	m_pCommunityWin->setInputDisable(FALSE);
	m_pNicknameWin->SetEnable(FALSE);
}

void CPopupCommunity::CbAcceptFriend(void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		CPopupCommunity* community = (CPopupCommunity*)pThis;
		g_pCommunity->AcceptViewFriend(community->m_acceptFriendIdx);
	}
}

void CPopupCommunity::CbDeleteFriend(void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		CPopupCommunity* community = (CPopupCommunity*)pThis;

		if (0 > community->m_lastDeleteFriend || FALSE == g_pCommunity->DeleteViewFriend(community->m_lastDeleteFriend))
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*삭제할 친구를 선택하셔야합니다.*/
		}
		else
		{
			community->m_SelectedContact = -1;
			g_pCommunity->SetCommunityUserInfo(FALSE);
		}
	}
}

void CPopupCommunity::CbDeleteNote( void * pThis, INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		CPopupCommunity* community = (CPopupCommunity*)pThis;
		community->OnDeleteNote();
	}
}
void CPopupCommunity::ReceiveAllNote(INT32 offset)
{
	INT32 curValue = 0;
	INT32 i = 0;
	NOTE_RECEIVE* pNote = NULL;
	char szTemp[256];

	for( i=g_pGameContext->GetNoteReceiveCount() - 1 - offset; curValue<MAX_NOTE_SLOT; i--, curValue++)
	{
		if( i >= 0)
		{
			pNote = g_pGameContext->GetReceiveNote( i);

			m_noteSlot[curValue]._pCheck->SetEnable(TRUE);

			if( 0 < pNote->body._messageSize )
			{
				char * pDumpMessage = pNote->body._message;
				char Subject[NET_NOTE_SIZE] = {0,};
				INT32 SubjectIdx = 0;
				while(*pDumpMessage)
				{
					// 10은 NewLine문자로써, 화면에 보여주는 메세지에서 자릅니다.
					if( *pDumpMessage == 10) break;

					pDumpMessage++;
					SubjectIdx++;
				}

				i3mem::Copy( Subject, pNote->body._message, sizeof(char)*SubjectIdx);
	
                m_noteSlot[curValue]._pContent->SetTextEllipsis(Subject);
			}
			else
			{
				UINT8* pcSize		= (UINT8*)pNote->body._message;
				UINT8* pcMessazeType = (UINT8*)&pNote->body._message[1];

				char szMessage[ MAX_PATH ];
				szMessage[ 0 ] = '\0';

				switch( *pcMessazeType )
				{
					case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:	// "축하합니다!\n클랜 가입이 승인되었습니다.\n즐거운 클랜 활동을 즐기세요."
					case NOTE_MESSAGE_CLAN_MASTER:			// "클랜 마스터 권한이 위임되었습니다.\n클랜 마스터가 되신 것을 축하 합니다."
					case NOTE_MESSAGE_CLAN_STAFF:			// "축하합니다.\n클랜 마스터가 스태프로 임명하였습니다."
					case NOTE_MESSAGE_CLAN_REGULAR:			// "클랜 마스터가 일반회원으로 임명하였습니다."
						i3String::Copy( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), MAX_PATH );
						break;
					case NOTE_MESSAGE_CLAN_INVITE:			// "[%s] 클랜가입 권유\n클랜으로부터 가입을 초대받았습니다.\n가입에 동의하시겠습니까?"
					case NOTE_MESSAGE_CLAN_INVITE_DENIAL:	// "[%s]\n클랜으로부터 가입 신청이\n거부 되었습니다."
					case NOTE_MESSAGE_CLAN_DEPORTATION:		// "[%s]\n클랜에서 제명되었습니다."
						sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), pNote->body._sender );
						break;
					case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:		// "[%s] 님께서\n클랜 가입 초대를 수락하였습니다."
					case NOTE_MESSAGE_CLAN_JOIN_DENIAL:		// "[%s] 님께서\n클랜 가입 신청을 거부하였습니다."
					case NOTE_MESSAGE_CLAN_SECESSION:		// "[%s] 님께서\n클랜에서 탈퇴하였습니다."
						{
							char szName[ NET_NICK_NAME_SIZE ];
							i3String::NCopy( szName, &pNote->body._message[2], *pcSize - 1 );
                            sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), szName );
						}
						break;
				}

				m_noteSlot[curValue]._pContent->SetTextEllipsis( szMessage );
			}
			m_noteSlot[curValue]._pWho->SetTextEllipsis(pNote->body._sender);

			if( (pNote->header._id & NT_GIFT_FLAG) &&
				(pNote->header._remainDay == 0))	//	유저가 삭제하지 못하는 쪽지 (자동으로 나중에 삭제됨)
			{
				i3String::Copy( szTemp, "--", 256 );
			}
			else
			{
				sprintf_s( szTemp, GAME_STRING("STR_POPUP_WHAT_DAY"), pNote->header._remainDay);/*%d일*/
			}

			m_noteSlot[curValue]._pDate->SetText(szTemp);

			switch(pNote->header._state)
			{
			case NS_READED:
				m_noteSlot[curValue]._pContent->SetTextColor(GetColor(GCT_DEFAULT));
				m_noteSlot[curValue]._pWho->SetTextColor(GetColor(GCT_DEFAULT));
				m_noteSlot[curValue]._pDate->SetTextColor(GetColor(GCT_DEFAULT));
				break;
			case NS_UNREAD:
				m_noteSlot[curValue]._pContent->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pWho->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pDate->SetTextColor(GetColor(GCT_VALUE));
				break;
			case NS_NEW:
				m_noteSlot[curValue]._pContent->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pWho->SetTextColor(GetColor(GCT_VALUE));
				m_noteSlot[curValue]._pDate->SetTextColor(GetColor(GCT_VALUE));
				break;
			}

			//	선물 쪽지는 따로 관리되므로 유저가 직접 삭제할 수 없다.
			if( pNote->header._type == NT_GIFT)
			{
				m_noteSlot[curValue]._pCheck->setCheckBoxState(FALSE);
				m_noteSlot[curValue]._pCheck->SetEnable(FALSE);
			}
			else
			{
				m_noteSlot[curValue]._pCheck->setCheckBoxState(m_bNoteCheck[i]);
				m_noteSlot[curValue]._pCheck->SetEnable(TRUE);
			}
		}
		else
		{
			m_noteSlot[curValue]._pCheck->SetEnable(FALSE);
			m_noteSlot[curValue]._pContent->SetTextEllipsis("");
			m_noteSlot[curValue]._pWho->SetTextEllipsis("");
			m_noteSlot[curValue]._pDate->SetText("");
		}
	}

	INT32 idxSlot = m_SelectedNote - m_pNoteList->GetCurrentTop();
	if( idxSlot >= 0 && idxSlot < MAX_NOTE_SLOT)
	{
		m_noteSlot[idxSlot]._pContent->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxSlot]._pWho->SetTextColor(GetColor(GCT_NOTIFY));
		m_noteSlot[idxSlot]._pDate->SetTextColor(GetColor(GCT_NOTIFY));
	}
}

void CPopupCommunity::SetNoteCheck( INT32 idx)
{
	UINT32 TopIdx = m_pNoteList->GetCurrentTop();
	INT32 nListIdx = (g_pGameContext->GetNoteReceiveCount()-1) - idx - TopIdx;

	if( m_bNoteCheck[nListIdx])
	{
		m_bNoteCheck[nListIdx] = FALSE;
	}
	else
	{
		m_bNoteCheck[nListIdx] = TRUE;
	}
}

void CPopupCommunity::SetNoteType( NOTE_TYPE type)
{
	m_nNoteType = type;
	m_pNoteChild->SetEnable(TRUE);

	switch( m_nNoteType)
	{
	case NOTE_TYPE_SEND:
		m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_RECEIVER"));/*받는사람*/
		m_pNoteSend->SetCaption( GAME_STRING("STR_POPUP_SEND"));/*보내기*/
		m_pNoteWhoValue->setInputDisable( FALSE);
		m_pNoteValue->setInputDisable( FALSE);
		m_pNoteWhoValue->SetFocus( TRUE);
		m_pNoteValue->SetEnable( TRUE);

		m_pNoteGiftItemFrame->SetEnable(FALSE);
		m_pNoteGiftWeaponImage->SetEnable(FALSE);
		m_pNoteGiftCharacterImage->SetEnable(FALSE);
		m_pNoteGiftCashItemImage->SetEnable(FALSE);
		m_pNoteGiftValue[0]->SetEnable(FALSE);
		m_pNoteGiftValue[1]->SetEnable(FALSE);
		m_pNoteGiftSeparate->SetEnable(FALSE);
		break;
	case NOTE_TYPE_REPLY:
		m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_SENDER_2"));/*보낸사람*/
		m_pNoteSend->SetCaption( GAME_STRING("STR_POPUP_REPLY"));/*답장하기*/
		m_pNoteWhoValue->setInputDisable( TRUE);
		m_pNoteValue->setInputDisable( TRUE);
		m_pNoteValue->SetEnable( TRUE);

		m_pNoteGiftItemFrame->SetEnable(FALSE);
		m_pNoteGiftWeaponImage->SetEnable(FALSE);
		m_pNoteGiftCharacterImage->SetEnable(FALSE);
		m_pNoteGiftCashItemImage->SetEnable(FALSE);
		m_pNoteGiftValue[0]->SetEnable(FALSE);
		m_pNoteGiftValue[1]->SetEnable(FALSE);
		m_pNoteGiftSeparate->SetEnable(FALSE);
		break;
	case NOTE_TYPE_GIFT:
		m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_SENDER_2"));/*보낸사람*/
		m_pNoteSend->SetCaption( GAME_STRING("STR_POPUP_REPLY"));/*답장하기*/
		m_pNoteWhoValue->setInputDisable( TRUE);
		m_pNoteValue->setInputDisable( TRUE);
		m_pNoteValue->SetEnable( FALSE);

		m_pNoteGiftItemFrame->SetEnable(TRUE);
		m_pNoteGiftWeaponImage->SetEnable(FALSE);
		m_pNoteGiftCharacterImage->SetEnable(FALSE);
		m_pNoteGiftCashItemImage->SetEnable(FALSE);

		FLUSH_CONTROL(m_pNoteGiftItemFrame);
		FLUSH_CONTROL(m_pNoteGiftWeaponImage);
		FLUSH_CONTROL(m_pNoteGiftCharacterImage);
		FLUSH_CONTROL(m_pNoteGiftCashItemImage);

		m_pNoteGiftValue[0]->SetEnable(TRUE);
		m_pNoteGiftValue[1]->SetEnable(TRUE);
		m_pNoteGiftSeparate->SetEnable(TRUE);
		break;
	}
}

BOOL CPopupCommunity::_SetNormalNote( NOTE_RECEIVE* pNote)
{
	SetNoteType( NOTE_TYPE_REPLY);

	m_pNoteValue->SetEnable(TRUE);
	m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);

	if( 0 < pNote->body._messageSize )
		m_pNoteValue->SetIMEText(pNote->body._message);
	else
	{
		UINT8* pcSize		= (UINT8*)pNote->body._message;
		UINT8* pcMessazeType = (UINT8*)&pNote->body._message[1];

		char szMessage[ MAX_PATH ];
		szMessage[ 0 ] = '\0';

		switch( *pcMessazeType )
		{
			case NOTE_MESSAGE_CLAN_INVITE_ACCEPT:	// "축하합니다!\n클랜 가입이 승인되었습니다.\n즐거운 클랜 활동을 즐기세요."
			case NOTE_MESSAGE_CLAN_MASTER:			// "클랜 마스터 권한이 위임되었습니다.\n클랜 마스터가 되신 것을 축하 합니다."
			case NOTE_MESSAGE_CLAN_STAFF:			// "축하합니다.\n클랜 마스터가 스태프로 임명하였습니다."
			case NOTE_MESSAGE_CLAN_REGULAR:			// "클랜 마스터가 일반회원으로 임명하였습니다."
				i3String::Copy( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), MAX_PATH );
				break;
			case NOTE_MESSAGE_CLAN_INVITE:			// "[%s] 클랜가입 권유\n클랜으로부터 가입을 초대받았습니다.\n가입에 동의하시겠습니까?"
			case NOTE_MESSAGE_CLAN_INVITE_DENIAL:	// "[%s]\n클랜으로부터 가입 신청이\n거부 되었습니다."
			case NOTE_MESSAGE_CLAN_DEPORTATION:		// "[%s]\n클랜에서 제명되었습니다."
				sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), pNote->body._sender );
				break;
			case NOTE_MESSAGE_CLAN_JOIN_ACCEPT:		// "[%s] 님께서\n클랜 가입 초대를 수락하였습니다."
			case NOTE_MESSAGE_CLAN_JOIN_DENIAL:		// "[%s] 님께서\n클랜 가입 신청을 거부하였습니다."
			case NOTE_MESSAGE_CLAN_SECESSION:		// "[%s] 님께서\n클랜에서 탈퇴하였습니다."
				{
					char szName[ NET_NICK_NAME_SIZE ];
					i3String::NCopy( szName, &pNote->body._message[2], *pcSize - 1 );
                    sprintf_s( szMessage, GAME_STRING_BY_HANDLE(m_nTypeMessage[*pcMessazeType]), szName );
				}
				break;
		}

		m_pNoteValue->SetIMEText( szMessage );
	}

	return TRUE;
}

BOOL CPopupCommunity::_SetGiftNote( NOTE_RECEIVE* pNote)
{
	char szWeaponName[ MAX_STRING_COUNT];

	//	선물 쪽지만 표시
	if(pNote->header._type != NT_GIFT)
		return FALSE;

	INT32 iType = AUTH_UNKNWON;
	char message[1024] = "";
	i3String::Copy( message, GAME_STRING("STR_POPUP_UNKNOWN_GIFT"), 1024 );	/*알 수 없는 상품입니다.*/

	//	선물하기 Note에서 pNote->body._user 포인터는 GIFT_BUFFER 구조체 포인터임.
	GIFT_BUFFER * pInfo = (GIFT_BUFFER*) pNote->body._user;

	INT32 authTerm = 0, authCount = 1;
	BOOL IsPackageGoods = FALSE;


	// GIFT_BUFFER의 _GoodsID가 32비트로 바뀌었는지 확인하여야 합니다.
	//I3ASSERT_0;

	UINT32 GoodsID = pInfo->_ui32GoodsID;
	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(pInfo->_ui32GoodsID);
	if ( pGoodsList )
	{
		if (1 < pGoodsList->_ui8ItemCount)
		{
			IsPackageGoods = TRUE;
		}
		else
		{//authArg
			iType = pGoodsList->_ppItemData[0]->_ui8AuthType;

			if ( AUTH_COUNT == iType )
			{
				authCount = pGoodsList->_pui32ItemArg[0];
			}
			else if ( AUTH_SEC == iType )
			{
				authTerm = pGoodsList->_pui32ItemArg[0];
			}
		}
	}


	SetNoteType( NOTE_TYPE_GIFT);

	if (IsPackageGoods)
	{

		BOOL isPeriod = FALSE;
		INT32 periodItemIdx = 0;
		INT32 notPointItemIdx = 0;

		INT32 ItemCount = pGoodsList->_ui8ItemCount;
		for(INT32 j = 0; j < ItemCount; j++)
		{
			if (AUTH_SEC == pGoodsList->_ppItemData[j]->_ui8AuthType)
			{
				isPeriod = TRUE;
				periodItemIdx = j;
				break;
			}
			else if (ITEM_TYPE_POINT != GET_ITEM_FLAG_TYPE(pGoodsList->_ppItemData[j]->_ui32ItemID))
			{
				notPointItemIdx = j;
			}
		}


		if (isPeriod)
		{
	
			UINT32 seconds = pGoodsList->_pui32ItemArg[periodItemIdx];
			UINT32 day = ConvertSecToUseDay(seconds);

			ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
			I3ASSERT( pInfo != NULL);

			sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), day);/*이름: %s\n사용기간: %d일*/
		}
		else
		{
			INT32 ItemID = pGoodsList->_ppItemData[periodItemIdx]->_ui32ItemID;
			INT32 itemUsage = (ITEM_TYPE) GET_ITEM_FLAG_TYPE(ItemID);

			//	유료 아이템 Goods에는 pGoods->_AuthArg에 모두 갯수가 있기 때문에 예외 처리가 필요 komet
			switch( (ITEM_TYPE) itemUsage)
			{
				//	지속성
			case ITEM_TYPE_MAINTENANCE:
				{
					//	getInfo는 keyItem으로 가져와야 정확한 info를 얻을 수 있다. (그렇지않으면 NULL일 수 있다.)
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
					I3ASSERT( pInfo != NULL);

					UINT32 Day = pGoodsList->_pui32ItemArg[periodItemIdx];
					sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), Day);/*이름: %s\n사용기간: %d일*/
				}
				break;
				//	소모성
			case ITEM_TYPE_EXPENDABLE:
				{
					UINT32 count = pGoodsList->_pui32ItemArg[periodItemIdx];

					//	getInfo는 keyItem으로 가져와야 정확한 info를 얻을 수 있다. (그렇지않으면 NULL일 수 있다.)
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
					I3ASSERT( pInfo != NULL);

					sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), pInfo->GetScriptName(), count);/*이름: %s\n사용개수: %d개*/;
				}
				break;
			default:
				sprintf_s( message, UNKNOWN_TEXT);
				break;
			}
		}


		g_pFramework->SetPackageGoodsShape(m_pNoteGiftCashItemImage, GoodsID);

		REAL32 posX = m_pNoteGiftCashItemImage->getPositionX();
		REAL32 posY = m_pNoteGiftCashItemImage->getPositionY();

		posX += 24.0f;
		posY -= 14.0f;

		m_pNoteGiftCashItemImage->setPosition(posX, posY);

		m_PackageGoodsID = GoodsID;

	}
	else
	{
		I3ASSERT(pGoodsList != NULL);
		UINT32 ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;
		INT32 authDay = (INT32) ConvertSecToUseDay((UINT32)authTerm);	//	인증기간 (일)로 변환

		//	상품 종류에 따라 분류
		ITEM_TYPE itemClass = (ITEM_TYPE)GET_ITEM_FLAG_TYPE(ItemID);
		switch(itemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			{
				m_pNoteGiftWeaponImage->SetEnable(TRUE);
				FLUSH_CONTROL(m_pNoteGiftWeaponImage);

				WEAPON_CLASS_TYPE type = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(ItemID);
				INT32 number = GET_ITEM_FLAG_NUMBER(ItemID);

				GUI_WEAPON_VALID( type, number);

				CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( type, number);

				I3ASSERT( iType != AUTH_UNKNWON );
				switch( iType )
				{
				case AUTH_COUNT:
					{
						pInfo->GetWeaponInfoName( szWeaponName);
						sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), szWeaponName, authCount);/*이름: %s\n사용개수: %d개*/
					}
					break;
				case AUTH_SEC:
					{
						pInfo->GetWeaponInfoName( szWeaponName);
						sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), szWeaponName, authDay);/*이름: %s\n사용기간: %d일*/
					}
					break;
				}

				g_pFramework->SetWeaponSelectMark(m_pNoteGiftWeaponImage, (UINT8)type , static_cast<UINT8>(number));
			}
			break;
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
			{
				m_pNoteGiftCharacterImage->SetEnable(TRUE);
				FLUSH_CONTROL(m_pNoteGiftCharacterImage);

				g_pFramework->SetCharacterShape(m_pNoteGiftCharacterImage, ItemID);

				char name[MAX_STRING_COUNT] = "";
				g_pCharaInfoDataBase->GetCharacterName(ItemID, name, sizeof(name));

				I3ASSERT( iType != AUTH_UNKNWON );
				switch( iType )
				{
				case AUTH_COUNT:
					{
						sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), name, authCount);/*이름: %s\n사용개수: %d개*/
					}
					break;
				case AUTH_SEC:
					{
						sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), name, authDay);/*이름: %s\n사용기간: %d일*/
					}
					break;
				}
			}
			break;
			//	인증되지 않은 상품일 경우 1일, 7일, 30일 짜리 기간제도 갯수제로 취급되기 때문에 정확한 기간제, 갯수제를
			//	판단하려면 ITEM_TYPE를 사용하지 말고 CASHIITEM_USAGE을 사용해야 한다.
			//	상점에서는 유료 아이템의 경우 기간제도 갯수제로 판매되기 때문. komet
		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_EXPENDABLE:
			{
				INT32 itemUsage = GET_ITEM_FLAG_TYPE(ItemID);

				m_pNoteGiftCashItemImage->setPosition(59.0f, 11.0f);
				REAL32 widthRate = i3GuiRoot::getGuiResolutionWidth();
				REAL32 heightRate = i3GuiRoot::getGuiResolutionHeight();
				m_pNoteGiftCashItemImage->SetControlSize((INT32)(90 * widthRate), (INT32)(65 * heightRate));

				m_PackageGoodsID = 0;
				m_pNoteGiftCashItemImage->SetEnable(TRUE);
				FLUSH_CONTROL(m_pNoteGiftCashItemImage);

				g_pFramework->SetCashItemShape(m_pNoteGiftCashItemImage, ItemID);

				//	getInfo는 keyItem으로 가져와야 정확한 info를 얻을 수 있다. (그렇지않으면 NULL일 수 있다.)
				char Name[MAX_STRING_COUNT] = {};
				g_pShopItemInfoDataBase->GetItemName(ItemID, Name, MAX_STRING_COUNT);

				I3ASSERT( iType != AUTH_UNKNWON );
				switch( iType )
				{
				case AUTH_SEC:
					{
						sprintf_s( message, GAME_STRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), Name, authDay);/*이름: %s\n사용기간: %d일*/
					}
					break;
				case AUTH_COUNT:
					{
						char szName[512] = "";

						//	서버에서는 갯수제로 넘어오지만 실제론 기간제 아이템일 경우 아이템 이름 뒤에 기간을 붙인다. komet
						//	(예: 경험치 130% 7일권 -> 서버: 1개)
						//	캐시 아이템 내에서 분류는 ITEM_TYPE를 사용하지 말고 CASHIITEM_USAGE을 사용해야 한다. komet
						if( itemUsage == ITEM_TYPE_MAINTENANCE)
						{
							INT32 nTerm = pGoodsList->_pui32ItemArg[0];
							if( nTerm > 0)
							{
								UINT32 Day = ConvertSecToUseDay(nTerm);
								sprintf_s(szName, GAME_STRING("STR_INVEN_CASH_ITEM_TERM"), Name, Day);/*%s %d일권*/
							}
							else
							{
								sprintf_s(szName, "%s", Name);
							}
						}
						else
						{
							sprintf_s(szName, "%s", Name);
						}

						sprintf_s( message, GAME_STRING("STR_POPUP_EXPENDABLES_GIFT_NAME"), szName, authCount);/*이름: %s\n사용개수: %d개*/
					}
					break;
				}

			}
		case ITEM_TYPE_TEMP:
			{
			}
			break;
		}
	}

	//	메시지
	m_pNoteGiftValue[0]->SetText( message);
	m_pNoteGiftValue[1]->SetText( pNote->body._message);

	return TRUE;

}

void CPopupCommunity::SetSelectedNote( INT32 idx)
{
	if( g_pGameContext->GetNoteReceiveCount() <= idx) return;

	m_SelectedNoteIndex = idx;

	UINT8 ReadCount = 0;
	UINT32 TopIdx = m_pNoteList->GetCurrentTop();

	UINT32 idxSlot = idx - m_pNoteList->GetCurrentTop();
	m_pNoteSelect->setPositionY(29.0f + (idxSlot * 24.0f));
	m_pNoteSelect->SetEnable(TRUE);

	//
	PlaySound( GUI_SND_ITEM_SELECTING);

	ReceiveAllNote(TopIdx);

	INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (idx+1)-TopIdx;

	m_SelectedNote = g_pGameContext->GetNoteReceiveCount() - (nListIdx+1);

	NOTE_RECEIVE* pNote = NULL;
	pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

	m_pNoteWhoValue->SetIMEText(pNote->body._sender);

	// 쪽지 메시지
	m_iListIdx = nListIdx;
	m_pNoteAccept->SetEnable(FALSE);
	m_pNoteReject->SetEnable(FALSE);
	m_pNoteSend->SetEnable(FALSE);

	m_pNoteGiftItemFrame->SetEnable(FALSE);
	m_pNoteGiftWeaponImage->SetEnable(FALSE);
	m_pNoteGiftCharacterImage->SetEnable(FALSE);
	m_pNoteGiftCashItemImage->SetEnable(FALSE);
	m_pNoteGiftSeparate->SetEnable(FALSE);

	m_pNoteGiftValue[0]->SetEnable(FALSE);
	m_pNoteGiftValue[1]->SetEnable(FALSE);
	m_pNoteValue->SetEnable(FALSE);

	//	GUI 처리
	{
		switch( pNote->header._type)
		{
		case NT_NORMAL:
			{
				m_pNoteSend->SetEnable(TRUE);
				_SetNormalNote(pNote);
			}
			break;
		case NT_GIFT:
			{
				m_pNoteSend->SetEnable(TRUE);
				_SetGiftNote(pNote);
			}
			break;
		case NT_WEB:
		case NT_NOTICE:
			{
				_SetNormalNote(pNote);
			}
			break;
		case NT_CLAN:
			{
				_SetNormalNote(pNote);

				m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_CLAN_SENDER"));/*보낸클랜*/
			}
			break;
		case NT_CLAN_ASK:
			{
				m_pNoteAccept->SetEnable(TRUE);
				m_pNoteReject->SetEnable(TRUE);

				if( pNote->header._state != NS_READED )
				{
					ACTIVATE_BUTTON( m_pNoteAccept );
					ACTIVATE_BUTTON( m_pNoteReject );
				}
				else
				{
					DEACTIVATE_BUTTON( m_pNoteAccept );
					DEACTIVATE_BUTTON( m_pNoteReject );
				}

				_SetNormalNote(pNote);
			}
			break;
		default:
			{
				m_pNoteValue->SetEnable(TRUE);
				m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
				m_pNoteValue->SetIMEText(GAME_STRING("STR_POPUP_UNKNOWN_NOTE"));/*알 수 없는 쪽지입니다.*/

				SetNoteType( NOTE_TYPE_REPLY);
			}
			break;
		}
	}

	//	서버로 이벤트 처리
	{
		switch( pNote->header._type)
		{
		case NT_GIFT:
			{
				if( pNote->header._state != NS_READED)
				{
					//	선물쪽지에 선물 정보가 있다.
					GIFT_BUFFER * pInfo = (GIFT_BUFFER*) pNote->body._user;
					I3ASSERT( pInfo->_ui32GoodsID != 0);
					//I3ASSERT( pInfo->_giftId != 0); // 사용하지 않음.

					// 읽음상태로 변경합니다.
					pNote->header._state = NS_READED;
	
					//	선물의 상품은 쪽지를 읽음과 동시에 인벤에 저장되도록 함.
					//g_pGameContext->SetEvent( EVENT_CHECK_GIFT_GOODS, &pInfo->_i64GiftWareDBIdx, &pInfo->_ui32GoodsID );
					g_pGameContext->SetEvent( EVENT_CHECK_GIFT_GOODS, &pInfo->_i32GiftWareDBIdx );	// GoodsID는 보내지 않습니다.
				}
			}
			break;
		case NT_CLAN_ASK:
			//	가입 신청 수락 메시지는 읽을때 변경하지 않는다. 나중에 답변할때 보냄.
			break;
		case NT_NORMAL:
		case NT_NOTICE:
		case NT_WEB:
		case NT_CLAN:
			{
				if(pNote->header._state != NS_READED)
				{
				// 읽음상태로 변경합니다.
					pNote->header._state = NS_READED;

					// 읽은 상태의 쪽지는 남은기간을 7일로 설정합니다.
					pNote->header._remainDay = 7;

					ReadCount++;
					g_pGameContext->SetEvent( EVENT_CHECK_READED_NOTE, &ReadCount, &pNote->header._id);
				}
			}
			break;
		}
	}

	//
	g_pGameContext->AddGameFlag( GAMEFLAG_NOTEREFRESH);
	_UpdateNote();
}

void CPopupCommunity::ResetNote(void)
{
	m_pNoteChild->SetEnable(FALSE);

	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		m_noteSlot[i]._pCheck->setCheckBoxState( FALSE);
	}

	for(UINT32 i = 0; i < MAX_NOTE_COUNT; i++) m_bNoteCheck[i] = FALSE;

	OnContactTab();
}

void CPopupCommunity::_InitCommon(void)
{
	m_pCommunityGui->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pCommunityCaption, GAME_STRING("STR_POPUP_COMMUNITY"));/*커뮤니티*/

	INIT_TAB_EX(m_pContactTab);
	INIT_TAB_EX(m_pNoteTab);
	INIT_CAPTION_BUTTON(m_pLeave, GAME_STRING("STR_POPUP_LEAVE"));/*나가기*/

	m_pContactTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);


	m_pNicknameWin->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pNicknameCaption, GAME_STRING("STR_POPUP_ADD_FRIEND"));/*친구추가*/
	INIT_CAPTION_BUTTON(m_pNicknameConfirm, GAME_STRING("STR_POPUP_OK"));/*확인*/
	INIT_CAPTION_BUTTON(m_pNicknameCancel, GAME_STRING("STR_POPUP_CANCEL"));/*취소*/

	m_pNicknameMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameMessage->SetTextColor(GetColor(GCT_DEFAULT));

	CREATE_IME_ELEMENT ime;

	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;

	m_pNicknameInput->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNicknameInput->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameInput->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNicknameInput->SetText("");


	// dummy
	m_pNoteGageBg->SetEnable(FALSE);
}

void CPopupCommunity::_InitContactSect(void)
{
	// 아직 기능이 비완성된 차단탭을 비활성화 한후
	// 그자리로 클랜탭을 이동합니다.
	{
		m_pBlockTab->SetEnable( FALSE);
		m_pClanTab->setPosition( m_pBlockTab->getPositionX(), m_pBlockTab->getPositionY());
	}

	INIT_CAPTION_TAB(m_pFriendTab, GAME_STRING("STR_POPUP_FRIEND"));/*친구*/
	INIT_CAPTION_TAB(m_pBlockTab, GAME_STRING("STR_POPUP_BLOCK"));/*차단*/
    INIT_CAPTION_TAB(m_pClanTab, GAME_STRING("STR_POPUP_CLAN"));/*클랜*/
	m_pFriendTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_currentTab = COMMUNITY_FRIEND_TAB;

	INIT_CAPTION_BUTTON(m_pFind, GAME_STRING("STR_POPUP_FIND_USER"));
	INIT_CAPTION_BUTTON(m_pAdd, GAME_STRING("STR_POPUP_ADD_FRIEND"));
//	INIT_BUTTON_EX(m_pDelete);

	m_pContactListName->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pContactListName->SetTextColor(GetColor(GCT_DEFAULT));
	m_pContactListName->SetText(GAME_STRING("STR_POPUP_NAME"));/*이름*/

	m_pContactListState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pContactListState->SetTextColor(GetColor(GCT_DEFAULT));
	m_pContactListState->SetText(GAME_STRING("STR_POPUP_STATE"));/*상태*/

	for(UINT32 i = 0; i < MAX_SLOT_FOR_CONTACT_LIST; i++)
	{
		GameGUI::SetRankStatic(m_contactSlot[i]._pMark);
		m_contactSlot[i]._pMark->SetEnable(FALSE);
		m_contactSlot[i]._pNick->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_contactSlot[i]._pNick->SetOffsetPos(3.0f, 0.0f);
		m_contactSlot[i]._pNick->SetTextColor(GetColor(GCT_DEFAULT));
		m_contactSlot[i]._pState->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_contactSlot[i]._pState->SetTextColor(GetColor(GCT_DEFAULT));
	}

	m_pContactList->CreateTextEx(MAX_SLOT_FOR_CONTACT_LIST,  GetDefaultFontName());
	m_pContactList->SetTextColor(GetColor(GCT_DEFAULT));
	m_pContactList->SetSelColor(0, 0, 0, 0);

	m_pContactSelect->SetEnable(FALSE);
	m_pContactFocus->SetEnable(FALSE);

//	m_pDelete->setControlDisable(TRUE);
	g_pCommunity->SetCommunityUserInfo(FALSE);
}

void CPopupCommunity::_InitContactChild(void)
{
	m_pContactChild->SetEnable(FALSE);

	g_pCommunity->SetCommunityUserInfo(FALSE);

	INIT_CAPTION(m_pContactKey, GAME_STRING("STR_POPUP_DETAIL_INFO"));/*상세정보*/

#if 0
	INIT_CAPTION_BUTTON(m_pProfile, GAME_STRING("STR_POPUP_LOOK_PRIVATE_ROOM"));/*관물대 보기*/
	INIT_CAPTION_BUTTON(m_pFriend, GAME_STRING("STR_POPUP_ADD_FRIEND"));/*친구추가*/
#else
	// dummy
	INIT_CAPTION_BUTTON(m_pProfile, GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));/*친구수락*/
	INIT_CAPTION_BUTTON(m_pFriend, GAME_STRING("STR_POPUP_VISIT"));/*찾아가기*/
#endif
	INIT_CAPTION_BUTTON(m_pInvite, GAME_STRING("STR_POPUP_INVITE"));/*초대하기*/
	INIT_CAPTION_BUTTON(m_pContactClose, GAME_STRING("STR_POPUP_CLOSE"));/*닫기*/
	INIT_CAPTION_BUTTON(m_pFriendRefuse, GAME_STRING("STR_POPUP_FRIEND_REFUSE"));/*친구거절*/

	DEACTIVATE_BUTTON(m_pProfile);
	DEACTIVATE_BUTTON(m_pFriend);
	DEACTIVATE_BUTTON(m_pInvite);
	DEACTIVATE_BUTTON(m_pContactClose);
	DEACTIVATE_BUTTON(m_pFriendRefuse);

	#ifndef USE_MODE_RECORD
	ACTIVATE_BUTTON(m_pProfile);
	ACTIVATE_BUTTON(m_pFriend);
	ACTIVATE_BUTTON(m_pInvite);
	ACTIVATE_BUTTON(m_pContactClose);
	ACTIVATE_BUTTON(m_pFriendRefuse);
	#endif

	m_pFriendRefuse->SetEnable(FALSE);

	//	전적 탭
	INIT_CAPTION_TAB(m_pRecordTab[0], GAME_STRING("STR_POPUP_CLAN"));/*클랜*/
	INIT_CAPTION_TAB(m_pRecordTab[1], GAME_STRING("STR_POPUP_PRIVATE"));/*개인*/

	for(INT32 i=0; i<2; i++)
		m_pRecordTab[i]->SetEnable(FALSE);

	m_pNicknameKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pSeasonRecordKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pTotalRecordKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pPositionKey->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_BOLD);
	m_pNicknameKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonRecordKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalRecordKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPositionKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNicknameKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pSeasonRecordKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pTotalRecordKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pPositionKey->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNicknameKey->SetText(GAME_STRING("STR_POPUP_NICKNAME"));/*닉네임*/
	m_pSeasonRecordKey->SetText(GAME_STRING("STR_POPUP_SEASON_HISTORY"));/*시즌전적*/
	m_pTotalRecordKey->SetText(GAME_STRING("STR_POPUP_TOTAL_HISTORY"));/*통산전적*/
	m_pPositionKey->SetText(GAME_STRING("STR_POPUP_CUR_POSITION"));/*현재위치*/

	m_pNicknameValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonRecord->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonKillDeath->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pSeasonHeadshot->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalRecord->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalKillDeath->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTotalHeadshot->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pPositionValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
}

void CPopupCommunity::_InitUserDetailInfo(void)
{
	m_pUserDetailInfoPopup->SetEnable(FALSE);
	g_pCommunity->SetCommunityUserInfo(FALSE);

	INIT_WIN_CAPTION( m_pCaption, GAME_STRING("STR_POPUP_DETAIL_INFO"));	//"상세정보"

	// UserDetailInfoPopupFrame
	// User Info
	GameGUI::SetRankStatic(m_pUserDetailInfoRankMark);
	m_pUserDetailInfoRankMark->SetEnable(FALSE);	
	INIT_VALUE_CAPTION( m_pUserDetailInfoNicknameValue, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoRank, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoClanName, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTitle, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoPoint, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoToll, "");

	INIT_STATIC_CAPTION( m_pUserDetailInfoPositionKey, GAME_STRING("STR_POPUP_CUR_POSITION"));	//	"현재위치"
	INIT_VALUE_CAPTION( m_pUserDetailInfoPositionValue, "");

	// UserDetailInfoPopupFrame2
	// Total Record	
	INIT_STATIC_CAPTION( m_pUserDetailInfoTotalRecordKey, GAME_STRING("STR_POPUP_TOTAL_HISTORY"));	//	"통산전적"
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalRecord, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalKillDeath, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalHeadshot, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalDis, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoTotalUserRanking, "");

	// Season Record
	INIT_STATIC_CAPTION( m_pUserDetailInfoSeasonRecordKey, GAME_STRING("STR_POPUP_SEASON_HISTORY"));	//	"시즌전적"
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonRecord, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonKillDeath, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonHeadshot, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonDis, "");
	INIT_VALUE_CAPTION( m_pUserDetailInfoSeasonUserRanking, "");

	// UserDetailInfoPopupFrame3	
	INIT_CAPTION_TAB(m_pModeRecordTab[0], GAME_STRING("STR_POPUP_TOTAL_HISTORY"));
	INIT_CAPTION_TAB(m_pModeRecordTab[1], GAME_STRING("STR_POPUP_SEASON_HISTORY"));
	m_pModeRecordTab[0]->setControlState(I3GUI_CONTROL_STATE_DISABLED);
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
#if 0
	INIT_CAPTION_BUTTON(m_pUserDetailInfoProfile,		GAME_STRING("STR_POPUP_LOOK_PRIVATE_ROOM"));/*관물대 보기*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoFriend,		GAME_STRING("STR_POPUP_ADD_FRIEND"));		/*친구추가*/
#else
	// dummy
	INIT_CAPTION_BUTTON(m_pUserDetailInfoProfile,		GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));							/*친구수락*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoFriend,		GAME_STRING("STR_POPUP_VISIT"));									/*찾아가기*/
#endif
	INIT_CAPTION_BUTTON(m_pUserDetailInfoInvite,		GAME_STRING("STR_POPUP_INVITE"));									/*초대하기*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoDetail,		GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_UNKNOWN_VIEW"));		/*미확인 정보보기*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoContactClose,	GAME_STRING("STR_POPUP_CLOSE"));									/*닫기*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoFriendRefuse,	GAME_STRING("STR_POPUP_FRIEND_REFUSE"));							/*친구거절*/

	DEACTIVATE_BUTTON(m_pUserDetailInfoProfile);
	DEACTIVATE_BUTTON(m_pUserDetailInfoFriend);
	DEACTIVATE_BUTTON(m_pUserDetailInfoInvite);
	DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	DEACTIVATE_BUTTON(m_pUserDetailInfoContactClose);
	DEACTIVATE_BUTTON(m_pUserDetailInfoFriendRefuse);

	#ifdef USE_MODE_RECORD
	ACTIVATE_BUTTON(m_pUserDetailInfoProfile);
	ACTIVATE_BUTTON(m_pUserDetailInfoFriend);
	ACTIVATE_BUTTON(m_pUserDetailInfoInvite);
	ACTIVATE_BUTTON(m_pUserDetailInfoDetail);
	ACTIVATE_BUTTON(m_pUserDetailInfoContactClose);
	ACTIVATE_BUTTON(m_pUserDetailInfoFriendRefuse);
	#endif

	m_pUserDetailInfoPositionKey->SetEnable( FALSE);
	m_pUserDetailInfoPositionValue->SetEnable( FALSE);

	m_bUserDetailInfoFlag = FALSE;	

	// UserDetailInfoPopupMessageBox
	INIT_WIN_CAPTION(m_pUserDetailInfoMsgCaption, GAME_STRING("STR_TBL_PROFILE_NOTICE_TEXT"));			//알림 /*상세정보보기 정보이용료 결재창*/	
	char Msg[MAX_STRING_COUNT];
	Msg[0] = NULL;
	#ifdef USE_MODE_RECORD
	sprintf_s(Msg, GAME_STRING("STR_POPUP_OUTHER_USER_DETAIL_INFO_BILLING"), SUBTRACTE_POINT);	
	#endif
	INIT_VALUE_CAPTION( m_pUserDetailInfoMsgMessage,Msg);		/*상대방의 상세정보를 보기위해 SUBTRACTE_POINT 포인트가 차감됩니다. 확인하시겠습니까?*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoMsgConfirm, GAME_STRING("STR_POPUP_OK"));/*확인*/
	INIT_CAPTION_BUTTON(m_pUserDetailInfoMsgCancel, GAME_STRING("STR_POPUP_CANCEL"));/*취소*/
	m_pUserDetailInfoMsgMessage->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pUserDetailInfoMsgMessage->SetTextColor(GetColor(GCT_DEFAULT));

	m_pUserDetailInfoMsg->SetEnable(FALSE);
}

void CPopupCommunity::_InitNoteSect(void)
{
	m_pNoteSect->SetEnable(FALSE);

	INIT_CAPTION_TAB(m_pNoteListTab, GAME_STRING("STR_POPUP_NOTE"));/*쪽지*/
	INIT_BUTTON_EX(m_pNoteDelete);
	INIT_CAPTION_BUTTON(m_pNoteWrite, GAME_STRING("STR_POPUP_WRITE_NOTE"));/*쪽지쓰기*/
	m_pNoteListTab->setControlState(I3GUI_CONTROL_STATE_DISABLED);

	m_pNoteListContent->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteListWho->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteListDate->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteListContent->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteListWho->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteListDate->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteListContent->SetText(GAME_STRING("STR_POPUP_CONTENT"));/*내용*/
	m_pNoteListWho->SetText(GAME_STRING("STR_POPUP_SENDER_1"));/*보낸이*/
	m_pNoteListDate->SetText(GAME_STRING("STR_POPUP_REMAIN_DAY"));/*남은날짜*/

	for(UINT32 i = 0; i < MAX_NOTE_SLOT; i++)
	{
		m_noteSlot[i]._pCheck->setCheckBoxState(FALSE);
		m_noteSlot[i]._pCheck->setInputDisable(FALSE);
		m_noteSlot[i]._pCheck->SetEnable( FALSE);
		m_noteSlot[i]._pContent->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_noteSlot[i]._pWho->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_noteSlot[i]._pDate->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_noteSlot[i]._pContent->SetTextColor(GetColor(GCT_VALUE));
		m_noteSlot[i]._pWho->SetTextColor(GetColor(GCT_VALUE));
		m_noteSlot[i]._pDate->SetTextColor(GetColor(GCT_VALUE));
	}

	m_pNoteList->CreateTextEx(MAX_NOTE_SLOT,  GetDefaultFontName());
	m_pNoteList->SetTextColor(GetColor(GCT_DEFAULT));
	m_pNoteList->SetSelColor(0, 0, 0, 0);

	m_pNoteSelect->SetEnable(FALSE);
	m_pNoteFocus->SetEnable(FALSE);
}

void CPopupCommunity::_InitNoteChild(void)
{
	m_pNoteChild->SetEnable(FALSE);

	m_pNoteWhoKey->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteWhoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pNoteWhoKey->SetTextColor(GetColor(GCT_STATIC));
	m_pNoteWhoValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteWhoKey->SetText(GAME_STRING("STR_POPUP_RECEIVER"));/*받는사람*/

	INIT_CAPTION_BUTTON(m_pNoteSend, GAME_STRING("STR_POPUP_REPLY"));/*답장하기*/
	INIT_CAPTION_BUTTON(m_pNoteAccept, GAME_STRING("STR_POPUP_ACCEPT"));/*수락*/
	INIT_CAPTION_BUTTON(m_pNoteReject, GAME_STRING("STR_POPUP_REJECT"));/*거절*/
	INIT_CAPTION_BUTTON(m_pNoteClose, GAME_STRING("STR_POPUP_CLOSE"));/*닫기*/

	//	무기, 캐시 아이템 쉐이프의 크기가  한번 플러쉬 되도록
	m_pNoteGiftItemFrame->SetEnable(TRUE);
	m_pNoteGiftWeaponImage->SetEnable(TRUE);
	m_pNoteGiftCharacterImage->SetEnable(TRUE);
	m_pNoteGiftCashItemImage->SetEnable(TRUE);
	FLUSH_CONTROL(m_pNoteGiftItemFrame);
	FLUSH_CONTROL(m_pNoteGiftWeaponImage);
	FLUSH_CONTROL(m_pNoteGiftCharacterImage);
	FLUSH_CONTROL(m_pNoteGiftCashItemImage);
	m_pNoteGiftItemFrame->SetEnable(TRUE);
	m_pNoteGiftWeaponImage->SetEnable(TRUE);
	m_pNoteGiftCharacterImage->SetEnable(TRUE);
	m_pNoteGiftCashItemImage->SetEnable(TRUE);

	//
	CREATE_IME_ELEMENT ime;
	ime._hWnd = g_hWnd;
	ime._hInst = g_hInstance;

	// 받는 사람
	ime._nTextLimitCount = NICKNAME_CHAR_COUNT;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	m_pNoteWhoValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNoteWhoValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_pNoteWhoValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteWhoValue->SetTextAutoWrap( FALSE);
	m_pNoteWhoValue->SetIMEMode( IME_MODE_NATIVE);
	
	// 보내는 내용
	ime._nTextLimitCount = 120;
	ime._StyleIME = IME_STYLE_LIMIT_CHARACTER;
	ime._StyleIME |= IME_STYLE_MULTILINE;
	m_pNoteValue->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE, TRUE, &ime);
	m_pNoteValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pNoteValue->SetTextColor(GetColor(GCT_VALUE));
	m_pNoteValue->SetIMEMode( IME_MODE_NATIVE);

	m_pNoteGiftItemFrame->SetEnable(FALSE);
	m_pNoteGiftWeaponImage->SetEnable(FALSE);
	m_pNoteGiftCharacterImage->SetEnable(FALSE);
	m_pNoteGiftCashItemImage->SetEnable(FALSE);
	m_pNoteGiftSeparate->SetEnable(FALSE);

	for(INT32 i=0; i<2; i++)
	{
		INIT_STATIC_CAPTION(m_pNoteGiftValue[i], "");

		m_pNoteGiftValue[i]->SetEnable(FALSE);
	}

	// Note 게이지
	INIT_CAPTION(m_pNoteGageBg, "");
}

void CPopupCommunity::_UpdateContactChild(void)
{
	if (0 < m_PackageGoodsID && m_pNoteGiftCashItemImage->isEnable())
	{
		g_pFramework->SetPackageGoodsShape(m_pNoteGiftCashItemImage, m_PackageGoodsID);

		REAL32 posX = m_pNoteGiftCashItemImage->getPositionX();
		REAL32 posY = m_pNoteGiftCashItemImage->getPositionY();

		posX += 24.0f;
		posY -= 14.0f;

		m_pNoteGiftCashItemImage->setPosition(posX, posY);
		m_pNoteGiftCashItemImage->UpdateRenderObjects();
	}

	switch( m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			// 업데이트할 정보가 있는지 확인한다.
			if (g_pCommunity->BeCommunityUserInfo())
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(TRUE);
				#else
				m_pContactChild->SetEnable(TRUE);
				#endif
			}
			else
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(FALSE);
				#else
				m_pContactChild->SetEnable(FALSE);
				#endif
				return;
			}

			#ifdef USE_MODE_RECORD			
			// 접속 위치/상태표시
			char position[MAX_STRING_COUNT] = {0, };
			UINT32 connectState = g_pCommunity->GetCommnunityConnectState();
			PrintFriendState(connectState, position, MAX_STRING_COUNT);
			m_pUserDetailInfoPositionValue->SetText(position);

			if(TRUE == g_pCommunity->GetUserDetailInfoFlag())
			{
				m_bUserDetailInfoFlag = TRUE;
				g_pCommunity->SetUserDetailInfoFlag(FALSE);
				DEACTIVATE_BUTTON( m_pUserDetailInfoDetail);
			}
			const USER_INFO_ALL_RECORD* info = g_pCommunity->GetCommunityAllRecord();
			_SetRecord( info);
			#else
			// 접속 위치/상태표시
			char position[MAX_STRING_COUNT] = {0, };
			UINT32 connectState = g_pCommunity->GetCommnunityConnectState();
			PrintFriendState(connectState, position, MAX_STRING_COUNT);
			m_pPositionValue->SetText(position);

			// 닉네임
			m_pNicknameValue->SetTextEllipsis(g_pCommunity->GetCommnunityInfoNick());

			// 전승패
			char record[MAX_STRING_COUNT];
			char killDeath[MAX_STRING_COUNT];
			char headshot[MAX_STRING_COUNT];

			const USER_INFO_RECORD* info = g_pCommunity->GetCommunityRecord();

			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_match, info->_win, info->_lose);/*%d전 %d승 %d패*/
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_killcount, info->_death, g_pGameContext->getKillRate(info->_killcount, info->_death));/*KILL/DEATH: %d/%d*/

			INT32 headshotPercent = TotalHeadShotPercent(info);

			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);/*HEADSHOT: %d%%*/

			m_pTotalRecord->SetText(record);
			m_pTotalKillDeath->SetText(killDeath);
			m_pTotalHeadshot->SetText(headshot);


			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_smatch, info->_swin, info->_slose);/*%d전 %d승 %d패*/
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_skillcount, info->_sdeath, g_pGameContext->getKillRate(info->_skillcount, info->_sdeath));/*KILL/DEATH: %d/%d*/

			INT32 sheadshotPercent = SeasonHeadShotPercent(info);
			
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), sheadshotPercent);/*HEADSHOT: %d%%*/

			m_pSeasonRecord->SetText(record);
			m_pSeasonKillDeath->SetText(killDeath);
			m_pSeasonHeadshot->SetText(headshot);
			#endif

			// 버튼 설정
			m_pProfile->SetEnable(TRUE);

			//	default
			DEACTIVATE_BUTTON(m_pFriend);
			DEACTIVATE_BUTTON(m_pInvite);


			// 친구 목록에 있는지 확인한다.
			BOOL beFriendList = FALSE;
			for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
			{
				const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
				if(NULL == node) continue;

				if (0 == i3String::Compare(g_pCommunity->GetCommnunityInfoNick(), node->_nick))
				{
					beFriendList = TRUE;
					break;
				}
			}

			// 친구 목록에 없다면, 버튼처리 하지 않는다.
			if (!beFriendList)
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoProfile->SetEnable(TRUE);
				m_pUserDetailInfoFriendRefuse->SetEnable(TRUE);
				DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
				#else
				m_pProfile->SetEnable(FALSE);
				m_pFriendRefuse->SetEnable(FALSE);
				#endif
				return;
			}


			//	친구 요청상태이면 '친구수락' 버튼으로, 정식 친구면 '친구삭제' 버튼으로,
			switch(GET_FRIEND_STATE(connectState))
			{
			//	친구가 등록 대기
			case FRIEND_STATE_HOLD:
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));/*친구수락*/					
					m_pUserDetailInfoFriendRefuse->SetEnable(TRUE);
					DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_ACCEPT_FRIEND"));/*친구수락*/
					#ifdef USE_FRIEND_REFUSE
					m_pFriendRefuse->SetEnable(TRUE);
					#endif
					#endif
				}
				break;
			//	친구가 수락 요청 또는 오프라인
			case FRIEND_STATE_REQUEST:
			case FRIEND_STATE_OFFLINE:
				{
					#ifdef USE_MODE_RECORD					
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*친구삭제*/										
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*친구삭제*/
					m_pFriendRefuse->SetEnable(FALSE);
					#endif
				}
				break;
			//	친구가 대기중
			case FRIEND_STATE_ONLINE:
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*친구삭제*/
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*친구삭제*/
					m_pFriendRefuse->SetEnable(FALSE);
					#endif

					CStageReady * pStageReady = ((CStageReady*)m_pParent);
					I3ASSERT( pStageReady != NULL);
	
					if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
					{	// 내가 배틀 중이 아니라면
						if(PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType())
						{	//	자신이 방에 들어가 있으면 친구초대 활성
							#ifdef USE_MODE_RECORD
							ACTIVATE_BUTTON(m_pUserDetailInfoInvite);
							#else
							ACTIVATE_BUTTON(m_pInvite);
							#endif
						}
					}
				}
				break;
			//	친구가 방에 있을때
			case FRIEND_STATE_ROOM:
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*친구삭제*/
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					#else
					m_pProfile->SetCaption(GAME_STRING("STR_POPUP_DELETE_FRIEND"));/*친구삭제*/
					m_pFriendRefuse->SetEnable(FALSE);
					#endif

					CStageReady * pStageReady = ((CStageReady*)m_pParent);
					I3ASSERT( pStageReady != NULL);

					if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
					{	// 내가 배틀 중이 아니라면
						//	자신이 방에 들어가 있으면 친구초대 활성
						if(PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType())
						{
							BOOL	SameRoom = g_pGameContext->IsSameRoom(connectState);
							if (SameRoom == FALSE)
							{
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoInvite);
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);		//	친구 찾아가기 활성
								#else
								ACTIVATE_BUTTON(m_pInvite);
								ACTIVATE_BUTTON(m_pFriend);		//	친구 찾아가기 활성
								#endif
							}
						}
						else
						{
							#ifdef USE_MODE_RECORD
							ACTIVATE_BUTTON(m_pUserDetailInfoFriend);		//	친구 찾아가기 활성
							#else
							ACTIVATE_BUTTON(m_pFriend);		//	친구 찾아가기 활성
							#endif
						}
					}
				}
				break;
			default:	
				{
					#ifdef USE_MODE_RECORD
					m_pUserDetailInfoProfile->SetEnable(FALSE);
					m_pUserDetailInfoFriendRefuse->SetEnable(FALSE);
					DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					#else
					m_pProfile->SetEnable(FALSE);
					m_pFriendRefuse->SetEnable(FALSE);
					#endif
				}
				break;				
			}
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			if (g_pCommunity->BeCommunityUserInfo())
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(TRUE);
				#else
				m_pContactChild->SetEnable(TRUE);
				#endif
			}
			else
			{
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(FALSE);
				#else
				m_pContactChild->SetEnable(FALSE);
				#endif
				return;
			}


			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);

 			if (node)
			{
				char position[MAX_STRING_COUNT] = {0, };

				CStageReady * pStageReady = ((CStageReady*)m_pParent);
				I3ASSERT( pStageReady != NULL);

				CUserLocationInfo	UserLocInfo(node->_state);
				INT32 nMatch = UserLocInfo.GetMatch();

				if( nMatch > 0 )
				{
					char state2[MAX_STRING_COUNT] = {0, };

					switch(GET_FRIEND_STATE(node->_state))
					{
					case FRIEND_STATE_OFFLINE:
						{
							i3String::Copy(position, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*오프라인*/
							#ifdef USE_MODE_RECORD
							DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
							#endif
						}
						break;
					case FRIEND_STATE_ONLINE:
						{
							GetBattleTeamName( state2, MAX_STRING_COUNT, nMatch-1, TRUE );

							sprintf_s(position, GAME_STRING("STR_POPUP_WHAT_READY"),
									UserLocInfo.GetDisplayServerChannelNumber(), state2);/*%03d-%s 대기중*/
						}
						break;
					case FRIEND_STATE_ROOM:
						{
							GetBattleTeamName( state2, MAX_STRING_COUNT, nMatch-1, TRUE );

							sprintf_s(position, GAME_STRING("STR_POPUP_WHAT_PLAY"),
									UserLocInfo.GetDisplayServerChannelNumber(), state2);/*%03d-%s 게임중*/
							break;
						}
					}
				}
				else
				{
					UINT32	State = FRIEND_STATE_64_TO_32(node->_state);
					PrintFriendState(State, position, MAX_STRING_COUNT);

					#ifdef USE_MODE_RECORD
					if(FRIEND_STATE_OFFLINE == GET_FRIEND_STATE(State))
					{
						DEACTIVATE_BUTTON(m_pUserDetailInfoDetail);
					}
					#endif
				}

				#ifdef USE_MODE_RECORD
				if(TRUE == g_pCommunity->GetUserDetailInfoFlag())
				{
					m_bUserDetailInfoFlag = TRUE;
					g_pCommunity->SetUserDetailInfoFlag(FALSE);
				}

				m_pUserDetailInfoNicknameValue->SetTextEllipsis((char*)node->_nick);
				m_pUserDetailInfoPositionValue->SetText(position);

				const USER_INFO_ALL_RECORD* info = g_pCommunity->GetCommunityAllRecord();
				_SetRecord(info);
				#else
				char record[MAX_STRING_COUNT];
				char killDeath[MAX_STRING_COUNT];
				char headshot[MAX_STRING_COUNT];

				m_pNicknameValue->SetTextEllipsis(node->_nick);
				m_pPositionValue->SetText(position);
	
				const USER_INFO_RECORD* info = g_pCommunity->GetCommunityRecord();

				sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_match, info->_win, info->_lose);/*%d전 %d승 %d패*/
				sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_killcount, info->_death, g_pGameContext->getKillRate(info->_killcount, info->_death));/*KILL/DEATH: %d/%d*/

				INT32 headshotPercent = TotalHeadShotPercent(info);

				sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);/*HEADSHOT: %d%%*/

				m_pTotalRecord->SetText(record);
				m_pTotalKillDeath->SetText(killDeath);
				m_pTotalHeadshot->SetText(headshot);

				sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), info->_smatch, info->_swin, info->_slose);/*%d전 %d승 %d패*/
				sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), info->_skillcount, info->_sdeath, g_pGameContext->getKillRate(info->_skillcount, info->_sdeath));/*KILL/DEATH: %d/%d*/
	
				INT32 sheadshotPercent = SeasonHeadShotPercent(info);
	
				sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), sheadshotPercent);/*HEADSHOT: %d%%*/

				m_pSeasonRecord->SetText(record);
				m_pSeasonKillDeath->SetText(killDeath);
				m_pSeasonHeadshot->SetText(headshot);
				#endif

				#ifdef USE_MODE_RECORD
				DEACTIVATE_BUTTON(m_pUserDetailInfoFriend);
				DEACTIVATE_BUTTON(m_pUserDetailInfoInvite);
				#else
				DEACTIVATE_BUTTON(m_pFriend);
				DEACTIVATE_BUTTON(m_pInvite);
				#endif

				//	친구 요청상태이면 '친구수락' 버튼으로, 정식 친구면 '친구삭제' 버튼으로,
				switch(GET_FRIEND_STATE(node->_state))
				{
					//친구가 마루에 있을떄
				case FRIEND_STATE_ONLINE:
					{
						if( GET_FRIEND_MATCH( node->_state ) > 0 )			// 대상이 배틀 팀 상태일때
						{
							if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() ) // 내가 팀에 없으면
							{	// 찾아가기 됨
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);								
								#else
								ACTIVATE_BUTTON(m_pFriend);
								#endif
							}
						}
						else												// 대상이 배틀 팀 상태 아닐때
						{
							// 초대하기 됨
							if( PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType() )
							{	// 찾아가기 됨
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoInvite);								
								#else
								ACTIVATE_BUTTON(m_pInvite);
								#endif
							}
							else
							{
								if( g_pFramework->GetClanContext()->InBattleTeam() )
								{
									#ifdef USE_MODE_RECORD
									ACTIVATE_BUTTON(m_pUserDetailInfoInvite);								
									#else
									ACTIVATE_BUTTON(m_pInvite);
									#endif
								}
							}
						}
					}
					break;
					//	친구가 방에 있을때
				case FRIEND_STATE_ROOM:
					{
						if( GET_FRIEND_MATCH( node->_state ) == 0 )			// 대상이 배틀 팀 상태가 아닐때
						{
							if( FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
							{
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);								
								#else
								ACTIVATE_BUTTON(m_pFriend);			//	친구 찾아가기 활성
								#endif

								CStageReady * pStageReady = ((CStageReady*)m_pParent);
								I3ASSERT( pStageReady != NULL);

								//	자신이 방에 들어가 있으면 친구초대 활성
								if(PHASE_READY_ROOM == pStageReady->GetCurrentPhaseType())
								{
									#ifdef USE_MODE_RECORD
									ACTIVATE_BUTTON(m_pUserDetailInfoInvite);								
									#else
									ACTIVATE_BUTTON(m_pInvite);			//	친구 찾아가기 활성
									#endif
								}
							}
							else
							{
								#ifdef USE_MODE_RECORD
								ACTIVATE_BUTTON(m_pUserDetailInfoFriend);								
								#else
								ACTIVATE_BUTTON(m_pInvite);			//	친구 찾아가기 활성
								#endif
							}
						}
					}
					break;
				}
			}
		}
		break;
	}
}

void CPopupCommunity::_UpdateContactList(void)
{
	UINT32 topIdx = m_pContactList->GetCurrentTop();

	for(UINT32 i = 0; i < MAX_SLOT_FOR_CONTACT_LIST; i++)
	{
		m_contactSlot[i]._pNick->SetText("");
		m_contactSlot[i]._pState->SetText("");
		m_contactSlot[i]._pMark->SetEnable(FALSE);
	}

	m_pContactList->Clear();

	//////////////////////////////////////////////////////////////
	//	친구 목록
	switch( m_currentTab)
	{
	case COMMUNITY_FRIEND_TAB:
		{
			UINT32 count = g_pCommunity->GetFriendCount();

			for(UINT32 i = 0; i < count; i++)
			{
				m_pContactList->AddElement("");
			}

			for(UINT32 i = 0; i + topIdx < count && i < MAX_SLOT_FOR_CONTACT_LIST; i++)
			{
				const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i + topIdx);
				if(NULL == node)	continue;

				m_contactSlot[i]._pMark->SetEnable(TRUE);
				m_contactSlot[i]._pMark->SetShapeUVIndex(node->_rank);
				m_contactSlot[i]._pNick->SetTextEllipsis((char*)node->_nick);

				char state[MAX_STRING_COUNT] = {0, };

				switch(GET_FRIEND_STATE(node->_state))
				{
				case 1:		i3String::Copy( state, GAME_STRING("STR_POPUP_NEED_TO_ACCEPT"), MAX_STRING_COUNT );/*수락요청*/		break;
				case 2:		i3String::Copy( state, GAME_STRING("STR_POPUP_READY_TO_ACCEPT"), MAX_STRING_COUNT );/*등록대기*/	break;
				case 3:		i3String::Copy( state, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*오프라인*/		break;
				case 4:		i3String::Copy( state, GAME_STRING("STR_POPUP_READY_STATE"), MAX_STRING_COUNT );/*대기중*/			break;
				case 5:
					{
						CUserLocationInfo	UserLocInfo(node->_state);

						i3String::Format(state, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_POSITION"),
							UserLocInfo.GetDisplayServerChannelNumber(), UserLocInfo.GetDisplayRoomNumber());/*%d-%d번방*/
						break;
					}
				default:	i3String::Copy( state, "", MAX_STRING_COUNT );	break;
				}

				m_contactSlot[i]._pState->SetText(state);


				// 색
				I3COLOR* color;

				if (3 < GET_FRIEND_STATE(node->_state))
				{
					color = GetColor(GCT_FOCUS);
				}
				else
				{
					color = GetColor(GCT_DEFAULT);
				}

				m_contactSlot[i]._pNick->SetTextColor(color);
				m_contactSlot[i]._pState->SetTextColor(color);

				#ifdef USE_MODE_RECORD
				// 열려있는 유저 정보 상세보기 창의 갱신을 위해
				if(m_CurrentUserInfoUID == node->_uid)
				{
					g_pCommunity->SetCommnunityConnectState(node->_state);
					g_pCommunity->SetCommnuitUserInfoBasic3()->_ui32Rank = node->_rank;
				}				
				#endif				
			}

			if (m_pContactList->GetElementCount() <= m_SelectedContact)
			{
				m_SelectedContact = -1;
//				m_pDelete->setControlDisable(TRUE);
				g_pCommunity->SetCommunityUserInfo(FALSE);
			}
		}
		break;
	//////////////////////////////////////////////////////////////
	//	차단 목록
	case COMMUNITY_BLOCK_TAB:
		{
		}
		break;
	//////////////////////////////////////////////////////////////
	//	클랜원 목록
	case COMMUNITY_CLAN_TAB:
		{
			UINT32 count = g_pCommunity->GetClanMemberCount();

			for(UINT32 i = 0; i < count; i++)
			{
				m_pContactList->AddElement("");
			}

 			for(UINT32 i = 0; i + topIdx < count && i < MAX_SLOT_FOR_CONTACT_LIST; i++)
			{
				const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i + topIdx);

				m_contactSlot[i]._pMark->SetEnable(TRUE);
				m_contactSlot[i]._pMark->SetShapeUVIndex(node->_rank);
				m_contactSlot[i]._pNick->SetTextEllipsis(node->_nick);

				char strState[MAX_STRING_COUNT] = {0, };
	
				CUserLocationInfo	UserLocInfo(node->_state);

				INT32 nMatch = UserLocInfo.GetMatch();

				if( nMatch > 0 )
				{
					char strState2[MAX_STRING_COUNT] = {0, };

					switch(UserLocInfo.GetState())
					{
					case FRIEND_STATE_OFFLINE:	i3String::Copy(strState, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*오프라인*/	break;
					case FRIEND_STATE_ONLINE:
						{
							GetBattleTeamName( strState2, MAX_STRING_COUNT, nMatch-1, FALSE );

							i3String::Format(strState, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_READY_MATCHING"),
								UserLocInfo.GetDisplayServerChannelNumber(), strState2);/*%03d-%s\n클랜매치 대기중*/
						}
						break;
					case FRIEND_STATE_ROOM:
						{
							GetBattleTeamName( strState2, MAX_STRING_COUNT, nMatch-1, FALSE );

							i3String::Format(strState, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_PLAY_MATCHING"),
								UserLocInfo.GetDisplayServerChannelNumber(), strState2);/*%03d-%s\n클랜매치 게임중*/
							break;
						}
					default:	i3String::Copy( strState, "", MAX_STRING_COUNT );			break;
					}
				}
				else
				{
					switch(UserLocInfo.GetState())
					{
					case FRIEND_STATE_OFFLINE:	i3String::Copy( strState, GAME_STRING("STR_POPUP_OFFLINE_STATE"), MAX_STRING_COUNT );/*오프라인*/	break;
					case FRIEND_STATE_ONLINE:	i3String::Copy( strState, GAME_STRING("STR_POPUP_ONLINE_STATE"), MAX_STRING_COUNT );/*온라인*/		break;
					case FRIEND_STATE_ROOM:
						{
							i3String::Format(strState, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_WHAT_POSITION"),
								UserLocInfo.GetDisplayServerChannelNumber(), UserLocInfo.GetDisplayRoomNumber());/*%d-%d번방*/
							break;
						}
					default:	i3String::Copy( strState, "", MAX_STRING_COUNT );			break;
					}
				}

				m_contactSlot[i]._pState->SetText(strState);

				// 색
				I3COLOR* color;

				if (FRIEND_STATE_OFFLINE < GET_FRIEND_STATE(node->_state))
				{
					color = GetColor(GCT_FOCUS);
				}
				else
				{
					color = GetColor(GCT_DEFAULT);
				}

				m_contactSlot[i]._pNick->SetTextColor(color);
				m_contactSlot[i]._pState->SetTextColor(color);

				#ifdef USE_MODE_RECORD
				// 열려있는 유저 정보 상세보기 창의 갱신을 위해
				if(m_CurrentUserInfoUID == node->_uid)
				{
					g_pCommunity->SetCommnunityConnectState((UINT32)node->_state);
					g_pCommunity->SetCommnuitUserInfoBasic3()->_ui32Rank = node->_rank;
				}				
				#endif
			}

			if (m_pContactList->GetElementCount() <= m_SelectedContact)
			{
				m_SelectedContact = -1;
				#ifdef USE_MODE_RECORD
				m_pUserDetailInfoPopup->SetEnable(FALSE);
				#else
				m_pContactChild->SetEnable(FALSE);
				#endif
				g_pCommunity->SetCommunityUserInfo(FALSE);
			}
		}
		break;
	}
}

void CPopupCommunity::_AddFriend(char* nick)
{
	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_NO_MORE_FRIEND"));/*더 이상 친구를 추가할 수 없습니다.*/
		return;
	}

	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			char message[MAX_STRING_COUNT];

			sprintf_s(message, GAME_STRING("STR_POPUP_MESSAGE_ALREADY_FRIEND"), nick);/*[%s]님은 이미 친구목록에 등록되어 있습니다.*/
			STRING_POPUP(message);
			return;
		}
	}

	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INVITE_SELF"));/*자신은 초대할 수 없습니다.*/
		return;
	}

	g_pCommunity->InsertFriend(nick);
}

void CPopupCommunity::_AddBlock(char* nick)
{
}

void CPopupCommunity::_FindUser(char* nick)
{
	// 자신은 막는다.
	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*자신의 닉네임을 입력하셨습니다.*/
		return;
	}

	// 친구 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// 목록선택
			OnContactSelect(i);
			return;
		}
	}

	// 클랜 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID 검출
			OnContactSelect(i);
			return;
		}
	}

	// 같은 서버의 유저 목록에 있는지 확인한다.
	// TODO :

	// 목록에 없다면 서버에 요청
	g_pCommunity->SetCommunityUserInfo(FALSE);
	g_pCommunity->SetCommnunityInfoNick(nick);
	g_pCommunity->FindUser(nick);
}

#ifdef USE_MODE_RECORD
void CPopupCommunity::_SetRecord( const USER_INFO_ALL_RECORD * pRecord)
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

		m_pUserDetailInfoRankMark->SetEnable(TRUE);
		m_pUserDetailInfoNicknameValue->SetEnable(TRUE);
		m_pUserDetailInfoRank->SetEnable(TRUE);
		m_pUserDetailInfoClanName->SetEnable(TRUE);	
		m_pUserDetailInfoTitle->SetEnable(TRUE);	
		m_pUserDetailInfoPoint->SetEnable(TRUE);
		m_pUserDetailInfoToll->SetEnable(TRUE);
		//m_pPositionKey->SetEnable(TRUE);
		//m_pPositionValue->SetEnable(TRUE);
		// Total Record
		m_pUserDetailInfoTotalRecord->SetEnable(TRUE);
		m_pUserDetailInfoTotalKillDeath->SetEnable(TRUE);
		m_pUserDetailInfoTotalHeadshot->SetEnable(TRUE);
		m_pUserDetailInfoTotalDis->SetEnable(TRUE);
		m_pUserDetailInfoTotalUserRanking->SetEnable(TRUE);
		// Season Record
		m_pUserDetailInfoSeasonRecord->SetEnable(TRUE);
		m_pUserDetailInfoSeasonKillDeath->SetEnable(TRUE);
		m_pUserDetailInfoSeasonHeadshot->SetEnable(TRUE);
		m_pUserDetailInfoSeasonDis->SetEnable(TRUE);
		m_pUserDetailInfoSeasonUserRanking->SetEnable(TRUE);	
		// Mode Record
		m_pModeRecordValue->SetEnable(TRUE);
		m_pModeKillDeath->SetEnable(TRUE);
		m_pModeHeadshot->SetEnable(TRUE);
		//m_pModeHitPercent->SetEnable(TRUE);

		// User Info		
		UINT32 connectState = GET_FRIEND_STATE(g_pCommunity->GetCommnunityConnectState());
		UINT32 ui32Rank = g_pCommunity->GetCommunityUserInfoBasic3()->_ui32Rank;
		m_pUserDetailInfoNicknameValue->SetTextEllipsis((char*)g_pCommunity->GetCommnunityInfoNick());		

		sprintf_s(rank, "%s : %s", GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_RANK"), GetCharaRank(ui32Rank));
		m_pUserDetailInfoRank->SetText(rank);
		m_pUserDetailInfoRankMark->SetShapeUVIndex(ui32Rank);

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

			m_pUserDetailInfoClanName->SetText(clan_name);
			m_pUserDetailInfoTitle->SetText(title);
			m_pUserDetailInfoPoint->SetText(point);
			m_pUserDetailInfoToll->SetText(cash);
		}
		else
		{
			// User Info
			m_pUserDetailInfoClanName->SetText(clan_name);
			m_pUserDetailInfoTitle->SetText(title);
			m_pUserDetailInfoPoint->SetText(point);
			m_pUserDetailInfoToll->SetText(cash);
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

			m_pUserDetailInfoSeasonRecord->SetText(all_record);
			m_pUserDetailInfoSeasonKillDeath->SetText(killDeath);
			m_pUserDetailInfoSeasonHeadshot->SetText(headshot);
			m_pUserDetailInfoSeasonDis->SetText(dis_count);
			m_pUserDetailInfoSeasonUserRanking->SetText(user_ranking);			
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

			m_pUserDetailInfoTotalRecord->SetText(all_record);
			m_pUserDetailInfoTotalKillDeath->SetText(killDeath);
			m_pUserDetailInfoTotalHeadshot->SetText(headshot);
			m_pUserDetailInfoTotalDis->SetText(dis_count);
			m_pUserDetailInfoTotalUserRanking->SetText(user_ranking);
		}    

		// Mode Record
		if (TRUE == m_bUserDetailInfoFlag)
		{
			headshotPercent = 0;
			hitPercent = 0;

			if (FALSE == m_bRecordTabFlag)
			{	
				//	"승패 : %d전 %d승 %d패"
				sprintf_s(mode_match, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._dmatch, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._win, pRecord->_detail_record._total_mode_record[m_nModeTypeComboIdx]._lose);
				sprintf_s(all_record, "%s%s", str_record, mode_match);
				//	"KILL/DEATH: %d/%d"
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
				//	"승패 : %d전 %d승 %d패"
				sprintf_s(mode_match, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._dmatch, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._win, pRecord->_detail_record._season_mode_record[m_nModeTypeComboIdx]._lose);
				sprintf_s(all_record, "%s%s", str_record, mode_match);
				//	"KILL/DEATH: %d/%d"
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
		m_pUserDetailInfoRankMark->SetEnable(FALSE);
		m_pUserDetailInfoNicknameValue->SetEnable(FALSE);
		m_pUserDetailInfoRank->SetEnable(FALSE);
		m_pUserDetailInfoClanName->SetEnable(FALSE);	
		m_pUserDetailInfoTitle->SetEnable(FALSE);	
		m_pUserDetailInfoPoint->SetEnable(FALSE);
		m_pUserDetailInfoToll->SetEnable(FALSE);
		//m_pPositionKey->SetEnable(FALSE);
		//m_pPositionValue->SetEnable(FALSE);
		// Total Record
		m_pUserDetailInfoTotalRecord->SetEnable(FALSE);
		m_pUserDetailInfoTotalKillDeath->SetEnable(FALSE);
		m_pUserDetailInfoTotalHeadshot->SetEnable(FALSE);
		m_pUserDetailInfoTotalDis->SetEnable(FALSE);
		m_pUserDetailInfoTotalUserRanking->SetEnable(FALSE);
		// Season Record
		m_pUserDetailInfoSeasonRecord->SetEnable(FALSE);
		m_pUserDetailInfoSeasonKillDeath->SetEnable(FALSE);
		m_pUserDetailInfoSeasonHeadshot->SetEnable(FALSE);
		m_pUserDetailInfoSeasonDis->SetEnable(FALSE);
		m_pUserDetailInfoSeasonUserRanking->SetEnable(FALSE);
		// Mode Record
		m_pModeRecordValue->SetEnable(FALSE);
		m_pModeKillDeath->SetEnable(FALSE);
		m_pModeHeadshot->SetEnable(FALSE);
		//m_pModeHitPercent->SetEnable(FALSE);
	}	
}
#endif

#ifdef USE_MESS_TEST
INT64 CPopupCommunity::_FindUserByUid(char* nick)
{
	// 자신은 막는다.
	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*자신의 닉네임을 입력하셨습니다.*/
		return 0;
	}

	// 친구 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
		if(NULL == node) continue;

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID 검출
			return node->_uid;
		}
	}

	// 클랜 목록에 있는지 확인한다.
	for(UINT32 i = 0; i < g_pCommunity->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(i);

		if (0 == i3String::Compare(nick, node->_nick))
		{
			// UID 검출
			return node->_uid;
		}
	}

	// 같은 서버의 유저 목록에 있는지 확인한다.
	// TODO :
	// 목록에 없다면 서버에 요청
	//g_pCommunity->SetCommunityUserInfo(FALSE);
	//g_pCommunity->SetCommnunityInfoNick(nick);
	//g_pCommunity->FindUser(nick);

	return 0;
}
#endif

void CPopupCommunity::_UpdateNote(void)
{
	char szTemp[256];
	BOOL m_bIsSelectNote = FALSE;

	UINT32 count = g_pGameContext->GetNoteReceiveCount();
	m_pNoteList->Clear();
	for(UINT32 i = 0; i < count; i++)	m_pNoteList->AddElement("");
	UINT32 topIdx = m_pNoteList->GetCurrentTop();

	if( g_pGameContext->IsNoteNew())
	{
		//m_SelectedNote = -1;
		//m_SelectedNoteIndex = -1;
		// 쪽지 리스트 인덱스 초기화 안함
		ReceiveAllNote(topIdx);
		g_pGameContext->RemoveGameFlag( GAMEFLAG_NOTENEW);
	}

	if( g_pGameContext->IsNoteRefresh() )
	{
		ReceiveAllNote(topIdx);
		g_pGameContext->RemoveGameFlag( GAMEFLAG_NOTEREFRESH);
	}

	// 쪽지 지우기 활성화/비활성화
	{
		for( INT32 i=0 ; i<g_pGameContext->GetNoteReceiveCount(); i++)
		{
			if( m_bNoteCheck[i])
			{
				m_bIsSelectNote = TRUE;
				break;
			}
		}

		// 체크박스로 선택되어진것들뿐만 아니라, 현재 페이지가 열려있는 쪽지에 대해서도 활성화/비활성화합니다.
		if( m_SelectedNoteIndex != -1) m_bIsSelectNote = TRUE;

		if( !m_bIsSelectNote)
		{
			m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_DISABLED);
		}
		else
		{
			if( m_SelectedNoteIndex != -1)
			{
				UINT32 TopIdx = m_pNoteList->GetCurrentTop();
				INT32 nListIdx = g_pGameContext->GetNoteReceiveCount() - (m_SelectedNoteIndex+1)-TopIdx;
				NOTE_RECEIVE * pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( nListIdx);

				if( pNote->header._type == NT_GIFT)
				{	//	선물 쪽지는 따로 관리되므로 유저가 직접 삭제할 수 없다.
					m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_DISABLED);
				}
				else
				{
					if( m_pNoteDelete->getControlState() == I3GUI_CONTROL_STATE_DISABLED)
					{
						m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_NORMAL);
					}
				}
			}
			else
			{
				if( m_pNoteDelete->getControlState() == I3GUI_CONTROL_STATE_DISABLED)
				{
					m_pNoteDelete->setControlState( I3GUI_CONTROL_STATE_NORMAL);
				}
			}
		}
	}
	
	// 현재 note 사용량
	m_fNoteEmptyRate = (REAL32)g_pGameContext->GetNoteReceiveCount();
	if( m_fNoteEmptyRate > 100.0f) m_fNoteEmptyRate = 100.0f;
	
	if( !m_bWarningNoteCount && m_pNoteSect->isEnable())
	{
		m_bWarningNoteCount = TRUE;

		if( m_fNoteEmptyRate >= 100.0f)
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));/*쪽지함의 용량이 부족하여 더 이상 쪽지를 받으실 수 없습니다.\n불필요한 쪽지를 삭제해 주십시오.*/
		}
		else if( m_fNoteEmptyRate >= 80.0f)
		{
			STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));/*쪽지함의 용량이 얼마 남지 않았습니다.\n쪽지함이 꽉 차게 되면 쪽지를 받으실 수 없으니\n불필요한 쪽지는 삭제해 주십시오.*/
		}
	}

	// 해상도에 따른 Offset값.
	REAL32 rateX = i3GuiRoot::getViewer()->GetViewWidth() * 0.9765625e-3f;

	i3Sprite2D* pSprite = m_pNoteGage->getSprite();
	m_pNoteGage->setSize((INT32)((m_fNoteEmptyRate / 100.0f) * (147.0f*rateX)), (INT32)(m_pNoteGage->getHeight()));
	pSprite->SetRect(GUI_SHAPE_OFFSET, m_pNoteGage->getTotalPosX(), m_pNoteGage->getTotalPosY(), (m_fNoteEmptyRate / 100.0f) * (147.0f*rateX), (REAL32)m_pNoteGage->getHeight());

	sprintf_s( szTemp, "%d%%", (INT32)m_fNoteEmptyRate);
	m_pNoteGageBg->SetCaption( szTemp);
	
	if( m_nNoteType == NOTE_TYPE_SEND)
	{
		if(i3GuiRoot::getFocusControl() == m_pNoteWhoValue)
		{
			if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_TAB ) ||
				g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_ENTER ) ||
				g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_NUMPADENTER ) )
			{
				m_pNoteValue->SetEnable( TRUE);
				m_pNoteValue->SetFocus( TRUE);
			}
		}
	}
}

void CPopupCommunity::StartAlarmNote(void)
{
	m_bAlarmCommunity = TRUE;

	// 쪽지 경고창 플래그 활성화
	m_bWarningNoteCount = FALSE;
}

void CPopupCommunity::EndAlarmNote(void)
{
	m_bAlarmCommunity = FALSE;

	// 원상복귀
	m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
}

void CPopupCommunity::_UpdateAlarmNote(REAL32 rDeltaSeconds)
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
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
			}
			else
			{
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
				m_pNoteTab->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
			}
		}
	}
}

#ifdef USE_MESS_TEST
INT64 CPopupCommunity::_FindNoteByUid(char* nick)
{
	// 자신은 막는다.
	if (g_pGameContext->IsMyNickName(nick))
	{
		STRING_POPUP(GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*자신의 닉네임을 입력하셨습니다.*/
		return 0;
	}

	NOTE_RECEIVE* pNote = NULL;
	pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( m_iListIdx);

	if (pNote && 0 != i3String::Compare(pNote->body._sender, nick))
	{
		NOTE_BODY note;
		i3String::Copy( note._sender, m_pNoteWhoValue->GetText(), NET_NICK_NAME_SIZE );
		i3String::Copy( note._message, m_pNoteValue->GetText(), NET_NOTE_SIZE );
		note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
		note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
		g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
		return 0;
	}

	I3ASSERT(pNote != NULL);
	return pNote->header._suid;
}

#if defined(CHANGE_ADDCOMMAND_METHOD)
void CPopupCommunity::AddPopupCommand( ContextMenuCommand Cmd, const CUserLocationInfo & UserLocInfo, INT64 UID )
{
	//
	// 함수 맨 밑에서 메뉴가 추가됩니다.
	//

	switch(Cmd)
	{
	case CMC_TRANSFER_HOST:
		break;
	case CMC_KICK:
		break;
	case CMC_PROFILE:
		break;
	case CMC_WHISPER:
		{
			if( ! _CheckCanWhisper(UserLocInfo.GetState()))
				return;
		}
		break;
	case CMC_INVITE:
		{
			if ( UserLocInfo.IsMatch() )
				return;

			switch(m_currentTab)
			{
			case COMMUNITY_FRIEND_TAB:
				{
					if( TRUE == g_pFramework->GetClanContext()->InBattleTeam() )
						return;

					if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType())
						return;

					if (g_pGameContext->IsSameRoom(UserLocInfo))
					{
						return;
					}
				}
				break;
			case COMMUNITY_CLAN_TAB:
				{
					if (PHASE_READY_ROOM != ((CStageReady*)m_pParent)->GetCurrentPhaseType() &&
						FALSE == g_pFramework->GetClanContext()->InBattleTeam() )
					{
						return;
					}
				}
				break;
			}

		}
		break;
	case CMC_VISIT:
		{
			if (UserLocInfo.GetState() != FRIEND_STATE_ROOM)
			{
				if ( UserLocInfo.IsMatch() == FALSE )
					return;
				if ( TRUE ==  g_pFramework->GetClanContext()->InBattleTeam())
					return;
			}

			if (g_pGameContext->IsSameRoom(UserLocInfo))
			{
				return;
			}
		}
		break;
	case CMC_ADD_FRIEND:
		{
			if ( _IsMyFriend(UID) )
				return;
		}
		break;
	case CMC_ADD_BLOCK:
		break;
	case CMC_CLAN_INVITE:
		{
			if ( m_currentTab == COMMUNITY_CLAN_TAB )
				return;

			if ( ! CanClanInvite() )
				return;

			if ( !g_pFramework->GetClanContext()->IsPossibleClanInvite() )
				return;

			if (_IsMyClanMember( UID))
				return;
		}
		break;
	case CMC_VIEW_DETAIL:
		break;
	case CMC_VIEW_OTHER_USER_ITEM_INFO:
		break;
	default:
		break;
	}

	ContextMenuAddCommand(Cmd);
}


#endif //CHANGE_ADDCOMMAND_METHOD


BOOL	CPopupCommunity::GetCurSelUserLoccationInfo(CUserLocationInfo & UserLocInfo)
{
	if( g_pFramework->GetClanContext()->InBattleTeam() )
	{
		const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
		if (!node)
			return FALSE;
		
		UserLocInfo = CUserLocationInfo(node->_state);
		return TRUE;
	}


	switch( m_currentTab )
	{
	case COMMUNITY_FRIEND_TAB:
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(m_SelectedContact);
			if (!node)
				return FALSE;

			UserLocInfo = CUserLocationInfo(node->_state);
		}
		break;
	case COMMUNITY_CLAN_TAB:
		{
			const CLAN_MEMBER_NODE* node = g_pCommunity->GetViewClanMember(m_SelectedContact);
			if (!node)
				return FALSE;

			UserLocInfo = CUserLocationInfo(node->_state);
		}
		break;
	}


	return TRUE;
}

#endif



