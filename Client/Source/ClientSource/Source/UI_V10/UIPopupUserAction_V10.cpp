#include "pch.h"
#include "UIPopupUserAction_V10.h"

#include "../UI/UIMainFrame_V10.h"
#include "UIPhaseReadyRoom_V10.h"
#include "UIPhaseLobby_V10.h"
#include "UIFloatChat_V10.h"
#include "UIFloatUserList_V10.h"
#include "../UI/UIUtil.h"
#include "../UI/UIFloatMercenaryList.h"

#include "../ClanDef.h"
#include "../CommunityContext.h"
#include "../UserInfoContext.h"
#include "../MyRoomInfoContext.h"
#include "../BattleSlotContext.h"
#include "../ViewOtherUserItemContext.h"
#include "../LobbyContext.h"
#include "../InviteContext.h"
#include "UI_V10/UIFloatCommunity.h"
#include "../ClanGameContext.h"

static UIPopupUserAction_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	return (UIPopupUserAction_V10*) pFrame->GetPopup( UPW_USERACTION);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupUserAction_V10_Click( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 1);
		GetThis()->OnCommand( slot);
		return 0;
	}
}

LuaFuncReg UIPopupUserAction_V10_Glue[] =
{
	{"ClickAction",		UIPopupUserAction_V10_Click},
	{nullptr,				nullptr}
};

//------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPopupUserAction_V10);

UIPopupUserAction_V10::UIPopupUserAction_V10()
{
	INT32 i;

	m_pObserver = nullptr;
	
	for( i = 0; i < UCM_MAX; i++)
	{
		m_commandText[i].clear();
	}

	m_pListBox = nullptr;

	m_UIStyle = UI_STYLE_NO_CLOSEBUTTON | UI_STYLE_REOPENTYPE;
}

UIPopupUserAction_V10::~UIPopupUserAction_V10()
{
}

/*virtual*/ void UIPopupUserAction_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pListBox = (i3UIListView_Box*) pScene->FindChildByName( "cmdList");
	I3ASSERT( m_pListBox);

	i3Vector::Copy( &m_vOriginSize, m_pListBox->getSize());
}

/*virtual*/ void UIPopupUserAction_V10::_ClearAtUnload( i3UIScene * pScene)
{
	m_pListBox = nullptr;

	UIPopupBase::_ClearAtUnload( pScene);
}

void UIPopupUserAction_V10::_ExecuteCommand( INT32 cmd)
{
	I3ASSERT( cmd < (INT32) m_commandCount);

	switch( m_commandMap[cmd])
	{
		case UCM_PROFILE		:	_CmdViewUserInfo();		break;
		case UCM_ADD_FRIEND		:	_CmdAddFriend();		break;
		case UCM_CLAN_INVITE	:	_CmdInviteClan();		break;
		case UCM_SEND_MAIL :		_CmdSendMail();		break;
		case UCM_ADD_BLOCK		:	_CmdAddBlock();		break;
		case UCM_REMOVE_BLOCK :		_CmdRemoveBlock();	break;
		case UCM_USER_REPORT :		_CmdReportUser();		break;
		//1.0 Lobby 강제
		case UCM_FIND_AWAY :		_CmdFindAway();		break;
		case UCM_SEND_PRESENT :		_CmdSendPresent();	break;
		case UCM_REMOVE_FRIEND :	GameUI::RemoveFriend( m_RequestInfo._CommunityIndex);			break;
		case UCM_KICK :				_CmdKickUser( m_RequestInfo._CommunityIndex);		break;
		// 1.0 Lobby 강제
		case UCM_INVITE :			_CmdInvite();		break;
		case UCM_WHISPER		:	_CmdWhispToUser();		break;
		case UCM_ACCEPT_FRIEND :	GameUI::AcceptFriend( m_RequestInfo._CommunityIndex);				break;
		case UCM_REFUSE_FRIEND :	GameUI::RefuseFriend( m_RequestInfo._CommunityIndex, MAKE_CALLBACK(CbDeleteFriend), this);	break;
		case UCM_TRANSFER_HOST :	GameEventSender::i()->SetEvent( EVENT_TRANSFER_HOST, &m_RequestInfo._CommunityIndex);	break;	// 방장 넘기기
		case UCM_VIEW_OTHER_USER_ITEM_INFO: _CmdViewOtherItem( m_RequestInfo._tUserInfo._Nick); break;
		case UCM_GMOPTION :			_CmdGMOption(); break;

		default :
			I3PRINTLOG(I3LOG_WARN, "Not yet made functions..!!! call swoongoo!!");
			break;
	}
}

void UIPopupUserAction_V10::_CmdViewUserInfo( void)
{	
#if !defined (TEST_LOBBY_USER_LIST)
	if( m_RequestInfo._tUserInfo._Nick[0] == 0) 
		return;

	// 전적 정보 설정
	CCommunityContext::i()->SetCommnunityInfoNick( m_RequestInfo._tUserInfo._Nick);

		// 다른 유저의 정보는 요청한다.
		UIPhaseBase * cStage = GetMainFrame()->GetCurrentPhase();
		CCommunityContext::i()->SetCommunityUserInfo( false);

		// 용병리스트의 경우, 이 이상 넘어가기 곤란하므로 미리 처리해준다.
		if(i3::same_of<UIFloatMercenaryList*>(m_pObserver))
		{
			INT32 idx = 1;
			if (GetMainFrame()->IsOpenedPopup(UPW_USERPROFILE) == false)
				GetMainFrame()->TogglePopup(UPW_USERPROFILE, &idx);
			return;
		}

		if(!i3::same_of<UIPhaseReadyRoom_V10*>(cStage))
		{
			if(m_RequestInfo._tUserInfo._Session < 0 )
				return;

			INT32  i32UserSessionIdx;
			i32UserSessionIdx = m_RequestInfo._tUserInfo._Session;
			
			GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_SESSION, &i32UserSessionIdx);
			GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_SUBTASK, &i32UserSessionIdx);

			m_RequestInfo._SlotIdx = 0;
		}

	//	미리 상세정보 팝업을 띄운다.
	INT32 idx = 1;
	if( GetMainFrame()->IsOpenedPopup( UPW_USERPROFILE) == false)
		GetMainFrame()->TogglePopup(  UPW_USERPROFILE, &idx);

#endif
}

void UIPopupUserAction_V10::_CmdAddFriend( void)
{
	if(m_RequestInfo._tUserInfo._Nick[0] == 0) 
		return;
	GameUI::AddFriend( m_RequestInfo._tUserInfo._Nick);
}

void UIPopupUserAction_V10::_CmdInviteClan( void)
{

	//if( m_RequestInfo._tUserInfo._Session < 0 || m_RequestInfo._tUserInfo._Nick[0] == 0) 
	//	return;
	if( m_RequestInfo._tUserInfo._Nick[0] == 0) 
		return;

	// 클랜 초대를 보내기 전에 사람을 찾습니다.
	GameEventSender::i()->SetEvent(EVENT_FIND_USER, m_RequestInfo._tUserInfo._Nick);

	GameUI::InviteClan( m_RequestInfo._tUserInfo._Nick, MAKE_CALLBACK(CbClanInvite), this);
}

void UIPopupUserAction_V10::_CmdSendMail( void)
{
	GetMainFrame()->TogglePopup( UPW_SENDMAIL, (void*) m_RequestInfo._tUserInfo._Nick);
}

void UIPopupUserAction_V10::_CmdReportUser( void)
{
}

void UIPopupUserAction_V10::_CmdWhispToUser( void)
{
	if(m_RequestInfo._tUserInfo._Nick[0] == 0)
		return;

	UIFloatChat_V10 * pChat;
	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_LOBBY || GetMainFrame()->GetCurrentPhaseType() == UIPHASE_CLANLOBBY)
		pChat = (UIFloatChat_V10*) GetMainFrame()->GetFloating( UFW_CHAT);
	else
		pChat = (UIFloatChat_V10*) GetMainFrame()->GetFloating( UFW_READYROOM_CHAT);

	if( pChat != nullptr)
	{
		pChat->OnWhisper( m_RequestInfo._tUserInfo._Nick);	
	}
}

void UIPopupUserAction_V10::_CmdSendPresent( void)
{
}

//차단 추가하기
void UIPopupUserAction_V10::_CmdAddBlock( void)
{
	GetMainFrame()->TogglePopup( UPW_ADDBLOCK, &m_RequestInfo._tUserInfo._Nick);
	
}

//차단 해제하기
void UIPopupUserAction_V10::_CmdRemoveBlock( void)
{
	//현재 선택된 유저의 정보를 가져온다.
	//INT32 _BlockCount = g_pCommunity->GetBlockUserCount();

	TTCHAR* Select_User = m_RequestInfo._tUserInfo._Nick;
	for(INT32 i=0; i < BLOCK_MAX_COUNT; i++)
	{
		const BLOCK_USER* t_Block = CCommunityContext::i()->GetBlockUser(i);
		if( i3::generic_compare(t_Block->_wstrNick, Select_User) == 0)
		{
			GameEventSender::i()->SetEvent(EVENT_DELETE_BLOCK, (const void *)&(t_Block->_blockInfo._Idx));
			return;
		}
	}
}

void UIPopupUserAction_V10::_CmdViewOtherItem( TTCHAR * username)
{
	INT32 ItemViewItemInfo = CASHITEM_ITEMID_VIEW_OTHER_USER_ITEM_INFO;
	
	if (CInvenList::i()->IsUsingCashItem(ItemViewItemInfo))
	{
		INT64 i64UID = CInvenList::i()->FindWareDBIdx_ByItemID(ItemViewItemInfo);
		UINT32 Size = sizeof(UINT8);

		UINT8 Type = 0;
		UINT32 Arg = 0;

		UIPhaseBase * pReadyRoom = GetMainFrame()->GetCurrentPhase();
		if( i3::same_of<UIPhaseReadyRoom_V10*>(pReadyRoom))
		{
			UIPhaseReadyRoom_V10* phase = static_cast<UIPhaseReadyRoom_V10*>(pReadyRoom);
			INT32 ReadyRoomSlotIndex = phase->getSelectedSlot();

			CInvenList::i()->GetItemTypeByItemID(INVEN::DB_CASHITEM, ItemViewItemInfo, &Type, &Arg);
		
			i3::rc_wstring wstr_selected_user_name = BattleSlotContext::i()->getNickForSlot(ReadyRoomSlotIndex);
			ViewOtherUserItemContext::i()->GetData()->m_wstrOtherUserName = wstr_selected_user_name;
			
			GameEventSender::i()->SetEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO, &i64UID, &Size, &ReadyRoomSlotIndex);
		}
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_GUI_VIEW_OTHER_USER_ITEM_INFO"), &GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE"), MAKE_CALLBACK(CbMoveShop), this);
	}
}

void UIPopupUserAction_V10::_CmdKickUser( INT32 idxSlot)
{
	// 훈련전 시 특정 슬롯이상은 동작 하지 않도록 합니다.
	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{
		if (8 <= idxSlot)
		{
			return;
		}
	}

	INT32 slotFlag = 0x00000000;
	const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(idxSlot);

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
	GameEventSender::i()->SetEvent(EVENT_CHANGE_SLOT_N, &slotFlag);
}


/*virtual*/ void UIPopupUserAction_V10::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/pb_ChattingWindow_UserList_PopUp.i3UIs", UIPopupUserAction_V10_Glue);
}

/*virtual*/ void UIPopupUserAction_V10::OnReset( void)
{
	m_commandCount = 0;
	m_pObserver = nullptr;
}



/*virtual*/ bool UIPopupUserAction_V10::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	// 팝업이 열리면 안되는 상황..
	i3EventReceiver* observer = static_cast<i3EventReceiver*>(pArg1);
	INT32 UserSlot = *static_cast<INT32*>(pArg2);

	if (0 == m_commandCount)	return false;
	if (nullptr == observer)		return false;

	m_pObserver = observer;

	//이것이 열려 있으면 다시 열도록...

	if( isEnable())
	{
		SetEnable( false);

		// 이전 메모리에 있는 Focus를 초기화..
		// 자기 자신이 다시 열리는 경우 메모리는 해제되었지만 Framework의 Focus를 처리가 안되고 있는 상황.
		g_pFramework->ResetFocus();
	}

	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( i3::same_of<UIFloatUserList_V10*>(m_pObserver))
	{
		UIFloatUserList_V10 * pUserList = (UIFloatUserList_V10*) m_pObserver;

		pUserList->SetUserInfo( UserSlot, &m_RequestInfo._tUserInfo);

		m_RequestInfo._CommunityIndex = UserSlot;
	}
	else if (i3::same_of<UIFloatMercenaryList*>(m_pObserver))
	{
		UIFloatMercenaryList* pMercList = static_cast<UIFloatMercenaryList*>(m_pObserver);

		if (pMercList == nullptr)
			return false;
		else
		{
			const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();
			const CLAN_MERCENARY_INFO *pMercInfo = ClanGameContext::i()->getClanMercenaryInfo(UserSlot);

			i3::generic_string_copy(m_RequestInfo._tUserInfo._Nick, pInfo->_UserInfo.m_nickName);
			i3::generic_string_copy(m_RequestInfo._tUserInfo._ClanName, pInfo->_UserInfo.m_strClanName);
			
			m_RequestInfo._tUserInfo._ClanMark = pInfo->_UserInfo.m_clanMark;			
			m_RequestInfo._tUserInfo._NickColor = pInfo->_UserInfo.m_nickColor;
			m_RequestInfo._tUserInfo._Rank = (UINT8)ClanGameContext::i()->GetMercIconIdx(pMercInfo);

			m_RequestInfo._CommunityIndex = UserSlot;
			m_RequestInfo._tUserInfo._Rank = (UINT8)ClanGameContext::i()->GetMercRank(UserSlot);
		}
	}
	else if( i3::same_of<UIPhaseReadyRoom_V10*>(m_pObserver))
	{
		UIPhaseReadyRoom_V10 * pPhaseReadyRoom = (UIPhaseReadyRoom_V10*) m_pObserver;

		const SLOT_INFO * pSlotInfo = BattleSlotContext::i()->getSlotInfo( UserSlot);
		
		m_RequestInfo._tUserInfo._ClanMark = pSlotInfo->_clanMark;
		m_RequestInfo._tUserInfo._eSportID = pSlotInfo->_eSportID;
		i3::generic_string_copy( m_RequestInfo._tUserInfo._Nick , BattleSlotContext::i()->getNickForSlot( UserSlot));
		
		m_RequestInfo._tUserInfo._NickColor = BattleSlotContext::i()->getNickColorForSlot( UserSlot);
		m_RequestInfo._tUserInfo._Rank = pSlotInfo->_Rank;

		m_RequestInfo._CommunityIndex = pPhaseReadyRoom->getSelectedSlot();
	}
	else if( i3::same_of<UIFloatCommunity*>(m_pObserver))
	{
		m_RequestInfo._CommunityIndex = UserSlot;

		UIFloatCommunity * pFloatCommunity = static_cast<UIFloatCommunity*>(m_pObserver);
		i3::generic_string_copy( m_RequestInfo._tUserInfo._Nick , pFloatCommunity->GetNickForUserIdx(UserSlot));
	}
	
	// data 설정전에 Size를 조절해줘야 Scrollbar가 안나온다.
	m_pListBox->setSize( &m_vOriginSize);

	LuaState * L = _CallLuaFunction( "ClearList");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, m_commandCount);
		_EndLuaFunction( L, 1);
	}

	for(INT32 i = 0; i < m_commandCount; i++)
	{
		L = _CallLuaFunction( "SetSlotInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, i);
			i3Lua::PushStringUTF16To8( L, m_commandText[ i]);
			_EndLuaFunction( L, 2);
		}
	}

	return true;
}

void UIPopupUserAction_V10::ResetCommand( void)
{
	m_commandCount = 0;
	m_pObserver = nullptr;
}

void UIPopupUserAction_V10::AddCommand( USER_CONTEXT_MENU cmd)
{
	I3ASSERT( m_commandCount < UCM_MAX);

	// 이것은 아직 기능이 없으므로 제외합니다. swoongoo
	// 위에서 튕겨냈으므로 아래코드에서도 주석 검. ( 16.07.29 수빈)
	if( cmd == UCM_USER_REPORT)
		return;

	switch( cmd)
	{
	case UCM_TRANSFER_HOST:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_GIVE_MASTER"), MAX_STRING_COUNT);		break;	// 방장넘기기
	case UCM_KICK:							i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_FORCE_GET_AWAY"), MAX_STRING_COUNT);	break;	// 강제퇴장
	case UCM_PROFILE:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_DETAIL_INFO"), MAX_STRING_COUNT);		break;	// 상세정보
	case UCM_WHISPER:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_WHISPER"), MAX_STRING_COUNT);			break;	// 귓속말
	case UCM_INVITE:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_INVITE"), MAX_STRING_COUNT);			break;	// 초대하기
	case UCM_FIND_AWAY:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_FIND_OUT"), MAX_STRING_COUNT);			break;	// 찾아가기
	case UCM_ADD_FRIEND:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_ADD_FRIEND"), MAX_STRING_COUNT);		break;	// 친구추가
	case UCM_ADD_BLOCK:						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_CUT_OFF"), MAX_STRING_COUNT);			break;	// 차단하기
	case UCM_REMOVE_BLOCK :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_REMOVE_BLOCK"), MAX_STRING_COUNT);		break;	// 차단삭제
	case UCM_CLAN_INVITE:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_CLAN_INVITE"), MAX_STRING_COUNT);		break;	// 클랜초대
//	case UCM_USER_REPORT :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_USER_REPORT"), MAX_STRING_COUNT);		break;	// 신고하기
	case UCM_SEND_MAIL :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_SEND_MAIL"), MAX_STRING_COUNT);			break;	// 쪽지 보내기
	case UCM_REMOVE_FRIEND:					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_REMOVE_FRIEND"), MAX_STRING_COUNT);		break;	// 친구 삭제
	case UCM_SEND_PRESENT :					i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_SEND_PRESENT"), MAX_STRING_COUNT);		break;	// 선물 보내기
	case UCM_ACCEPT_FRIEND :				i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_ACCEPT_FRIEND"), MAX_STRING_COUNT);		break;	// 친구 수락
	case UCM_REFUSE_FRIEND :				i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_POPUP_FRIEND_REFUSE"), MAX_STRING_COUNT);					break;	// 친구거절
	case UCM_VIEW_OTHER_USER_ITEM_INFO :	i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_TBL_GUI_ETC_CONTEXT_OTHERUSER_INFORMATION"), MAX_STRING_COUNT);				break;	// 상대방 아이템 보기
	case UCM_GMOPTION :						i3::safe_string_copy( m_commandText[m_commandCount], GAME_STRING("STR_GM_POPUP_INFO"), MAX_STRING_COUNT);				break;	// GM 옵션

	default:
		I3PRINTLOG(I3LOG_FATAL, "Not yet setting command!!");
		break;
	}

	m_commandMap[m_commandCount] = cmd;
	m_commandCount++;
}


void UIPopupUserAction_V10::OnCommand( INT32 cmd)
{
	GetMainFrame()->ClosePopup( UPW_USERACTION);

	if( cmd < (INT32) m_commandCount)
	{
		if (m_pObserver)
		{
			USERACTION_INFO info;

			info._Command = m_commandMap[cmd];

			m_pObserver->OnEvent( I3_EVT_USER_ACTION , this, (UINT32) &info, I3_EVT_CODE_ACTIVATE);
		}

		_ExecuteCommand( cmd);
	}

	
	PlaySound( UISND_ITEM_SELECTING);
}

void UIPopupUserAction_V10::DoClanInvite( void)
{
	//	클랜 초대를 보낸다.
	// 클랜 초대 통일합니다. 여러가지 섞어서 쓰니가 안되네요
	//if( i3::same_of<UIFloatUserList_V10*>(m_pObserver) || i3::same_of<UIFloatCommunity*>(m_pObserver))
	{
		g_pFramework->GetClanContext()->SendClanInvite();
	}
}

void UIPopupUserAction_V10::CbClanInvite( void* pThis, INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		((UIPopupUserAction_V10*)pThis)->DoClanInvite();
	}
}

void UIPopupUserAction_V10::CbDeleteFriend( void* pThis,INT32 nParam)
{
	if (nParam == MBR_OK)
	{
		UIPopupUserAction_V10* userlist = (UIPopupUserAction_V10*)pThis;

		if (0 > userlist->m_RequestInfo._CommunityIndex || false == CCommunityContext::i()->DeleteViewFriend( userlist->m_RequestInfo._CommunityIndex))
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_CHOOSE_FRINED_TO_DELETE"));/*삭제할 친구를 선택하셔야합니다.*/
		}
		else
		{
			userlist->m_RequestInfo._CommunityIndex = -1;
			CCommunityContext::i()->SetCommunityUserInfo(false);
		}
	}
}

void UIPopupUserAction_V10::CbMoveShop( void* pThis, INT32 nParam)
{
	if( nParam == MBR_OK)
	{
		/*UIPhaseReadyRoom_V10* pReadyRoom = (UIPhaseReadyRoom_V10*)pThis;
		pReadyRoom->GetMainFrame()->ChangeSubPhase( USP_SHOP);*/
	}
}

void UIPopupUserAction_V10::OnEntranceEnd( void )
{
	REAL32 MaxWidth = _ResizeListView();
	
	_ReposFrameWnd(MaxWidth);

	//_ReSizeAndPosIcon();
}

REAL32 UIPopupUserAction_V10::_ResizeListView()
{
	// 가장 긴 Text의 Width로 계산한다.
	REAL32 MaxWidth = 0.f;

	i3::vector<i3UIListView_Item*>& item_list = m_pListBox->getItemList();

	for(size_t i = 0; i < item_list.size(); i++)
	{
		i3UIListView_Item * pItem = item_list[ i];

		i3UIListView_Cell * pCell = pItem->getCell( 0);

		// Text의 길이 얻어오기
		if( pCell->getUIText() != nullptr)
		{
			if( MaxWidth < pCell->getUIText()->getExtentWidth() + 4)
				MaxWidth = (REAL32) pCell->getUIText()->getExtentWidth() + 4;
		}

		i3UIControl * pChild = pCell->getCellChild();
		if( i3::kind_of<i3UICaptionControl*>(pChild))
		{
			if( ((i3UICaptionControl*) pChild)->getUIText() )
			{
				if( MaxWidth < ((i3UICaptionControl*) pChild)->getUIText()->getExtentWidth() + 4)
				{
					MaxWidth = (REAL32) ((i3UICaptionControl*) pChild)->getUIText()->getExtentWidth() + 4;
				}
			}
		}
	}

	for(size_t i = 0; i < item_list.size(); i++)
	{
		i3UIListView_Item * pItem = item_list[ i];

		pItem->UpdateCellPosAndSize( MaxWidth, pItem->getHeight(), true);
		pItem->UpdateShapes();
	}

	return MaxWidth;
}

void UIPopupUserAction_V10::_ReposFrameWnd(REAL32 MaxWidth)
{
	REAL32 h = (m_pListBox->getBoundItem()->getHeight() * m_commandCount);

	// 위치를 마우스 위치로..
	POINT pos;
	REAL32 fScreenWidthRate = i3UI::getUIResolutionWidth();
	REAL32 fScreenHeightRate = i3UI::getUIResolutionHeight();

	I3ASSERT( i3UI::getManager() != nullptr);


	i3InputMouse* mouse = g_pFramework->getMouse();
	REAL32 x = mouse->GetX() / fScreenWidthRate;
	REAL32 y = mouse->GetY() / fScreenHeightRate;

	pos.x = (LONG) x;
	pos.y = (LONG) y;

	I3ASSERT( m_pListBox->getBoundItem() != nullptr);

	m_pFrameWnd->OnMapCoord( &pos);

	x = (REAL32) pos.x;
	y = (REAL32) pos.y;

	h += m_pListBox->getHeader()->getHeight();

	if( MaxWidth == 0.f)
		MaxWidth = getX(m_pListBox->getSize());

	m_pListBox->setSize( MaxWidth, h);

	// 화면을 나가면 안쪽으로 밀어주자..
	if( x + MaxWidth > i3UI::getManager()->getWidth())
		x = i3UI::getManager()->getWidth() - MaxWidth;

	if( y + h > i3UI::getManager()->getHeight())
		y = i3UI::getManager()->getHeight() - h;

	m_pFrameWnd->setPos( (REAL32)x, (REAL32)y);
	m_pFrameWnd->setSize( MaxWidth, h);
}

void UIPopupUserAction_V10::_ReSizeAndPosIcon()
{
	i3::vector<i3UIListView_Item*>& item_list = m_pListBox->getItemList();

	for(size_t i = 0; i < item_list.size(); i++)
	{
		i3UIListView_Item * pItem = item_list[ i];

		i3UIListView_Cell * pCell = pItem->getCell( 0);
			
		_SetIconSizeAndPos(pCell, pItem->getSepWidth());
	}
}

void UIPopupUserAction_V10::_SetIconSizeAndPos( i3UIListView_Cell * pCell , REAL32 sepWidth)
{
	i3UIButtonImageSet * pCtrl = static_cast<i3UIButtonImageSet*>(pCell->getCellChild());

	if(pCtrl != nullptr)
	{
		REAL32 width, height;
		width = pCtrl->getWidth();
		height = pCtrl->getHeight();

		REAL32 posX, posY;
		posX = (pCell->getWidth() / 2.0f) - (width / 2.0f) + sepWidth;
		posY = (pCell->getHeight() / 2.0f) - (height / 2.0f);

		pCtrl->setSize(width, height);
		pCtrl->setPos(posX, posY);
	}
}

void UIPopupUserAction_V10::_CmdGMOption( void )
{
	i3::stack_wstring wstrUserNickName;
	i3::sprintf(wstrUserNickName, L"%s", m_RequestInfo._tUserInfo._Nick);

	INT32 _UserIndex = LobbyContext::i()->getUserSessionIdxByUserNickName(wstrUserNickName);
	UINT8 _IndexType = 1;

	// Search를 이용한 검색은 로비 유저의 정보가 갱신되지 않기 때문에
	// 갱신된 InviteContext의 유저 목록에서 SessionIndex 검사를 한 번 더 해야함
/*	if ( _UserIndex != -1)
		_UserIndex = InviteContext::i()->GetUserSessionIdxByUserNickName(m_strUserNickName);*/

	// SessionIndex가 있으면 유저는 로비에 있음
	if ( _UserIndex != -1)
	{
		_IndexType = GMCHAT_INDEX_LOBBY;
		I3TRACE("유저가 로비에 있습니다.\n");
	}
	// SessionIndex가 -1이면 해당 유저는 로비에 없음
	else
	{
		// 유저가 있는 룸의 인덱스 번호
		const USER_SIMPLE_INFO * pInfo = CCommunityContext::i()->GetUserSimpleInfo();
		_UserIndex = pInfo->_UserInfo.m_RoomIdx + 1;
		_IndexType = GMCHAT_INDEX_ROOM;

		I3TRACE("유저가 방에 있습니다.\n");

		// 유저와 GM이 같은 방에 있는지 확인
		if( g_pFramework->GetUIMainframe()->GetCurrentPhaseType() != UIPHASE_LOBBY)
		{

			I3TRACE("GM이 방에 있습니다.\n");
			if ( _UserIndex == MyRoomInfoContext::i()->getMyRoomIndex())
			{
				_UserIndex = BattleSlotContext::i()->GetUserIndex(wstrUserNickName);
				_IndexType = GMCHAT_INDEX_ROOMSLOT;

				I3TRACE("GM과 유저가 같은 방에 있습니다.\n");
			}
			else
			{
				if (InviteContext::i()->GetUserSessionIdxByUserNickName(wstrUserNickName) != -1)
				{
					I3TRACE("유저가 방에 있습니다.\n");
					
					_IndexType = GMCHAT_INDEX_LOBBY;
					_UserIndex = InviteContext::i()->GetUserSessionIdxByUserNickName(wstrUserNickName);
				}
				else
				{
					I3TRACE("GM과 유저가 다른 방에 있습니다.\n");
				}
			}
		}
		else
		{
			I3TRACE("GM이 로비에 있습니다.\n");
		}
	}

	I3TRACE("IndexType : %d\n", _IndexType);
	I3TRACE("UserIndex : %d\n", _UserIndex);
	
	GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_NICK, wstrUserNickName.c_str() );
	GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_REQ, &_IndexType, &_UserIndex, wstrUserNickName.c_str() );
}

void UIPopupUserAction_V10::_CmdFindAway()
{
	if( i3::same_of<UIFloatCommunity*>(m_pObserver) == true)
	{
		UIFloatCommunity * pFloatCommunity = static_cast<UIFloatCommunity*>(m_pObserver);
		if(pFloatCommunity->GetCurrentTab() == USERLIST_TAB_FRIEND)
		{
			GameUI::FindAwayFriend(m_RequestInfo._CommunityIndex);
		}
		else if(pFloatCommunity->GetCurrentTab() == USERLIST_TAB_CLAN)
		{
			GameUI::FindAwayClan(m_RequestInfo._CommunityIndex);
		}
	}
	else
	{
		GameUI::FindAwayUser( USERLIST_TAB_CURRENT, m_RequestInfo._CommunityIndex);
	}
}

void UIPopupUserAction_V10::_CmdInvite()
{
	if( i3::same_of<UIFloatCommunity*>(m_pObserver) == true)
	{
		UIFloatCommunity * pFloatCommunity = static_cast<UIFloatCommunity*>(m_pObserver);
		if(pFloatCommunity->GetCurrentTab() == USERLIST_TAB_FRIEND)
		{
			GameUI::InviteFriend(USERLIST_TAB_FRIEND, m_RequestInfo._CommunityIndex);
		}
		else if(pFloatCommunity->GetCurrentTab() == USERLIST_TAB_CLAN)
		{
			if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_CLANLOBBY)
			{
				GameUI::InviteClanMember(m_RequestInfo._CommunityIndex);
			}
			else
			{
				GameUI::InviteFriend(USERLIST_TAB_CLAN, m_RequestInfo._CommunityIndex);
			}
		}
	}
	else
	{
		GameUI::InviteUser( USERLIST_TAB_CURRENT, m_RequestInfo._CommunityIndex);
	}

}

bool UIPopupUserAction_V10::IsEmptyCommand()
{
	return m_commandCount <= 0;
}