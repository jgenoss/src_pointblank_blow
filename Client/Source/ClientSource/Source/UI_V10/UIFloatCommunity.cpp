#include "pch.h"

#include "UIFloatCommunity.h"

#include "UI/UIMainFrame.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIPopupReceiveMail.h"
#include "GameGUI.h"
#include "GameContextUtil.h"
#include "ChannelContext.h"
#include "MailBoxContext.h"
#include "UserInfoContext.h"
#include "UI_V10/UITopMenu_V10.h"

#include "Shop.h"
#include "UI/UIUtil.h"
#include "UI/UIShopFunction.h"
#include "UI/UIItemInfoUtil.h"

extern "C" 
{
	int UIPopupCommunity_ClickTab( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->SelectTab( tab);

		return 0;
	}

	int UIPopupCommunity_ClickSubTab( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->SelectSubTab( tab);

		return 0;
	}
	
	int UIPopupCommunity_Click( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->Click( tab);

		return 0;
	}

	int UIPopupCommunity_ReadMail( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L, 1, -1 );	
		INT32 i32Cell = i3Lua::GetIntegerArg(L, 2, -1);

		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if(nullptr != pThis && i32Cell != 0)
			pThis->ReadMail(i32Slot);

		return 0;
	}

	int UIPopupCommunity_ClickUser( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->ListClick( LIST_LCLICK, idx);

		return 0;
	}

	int UIPopupCommunity_ClickOpenUserPopup( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->ListClick( LIST_RCLICK, idx);

		return 0;
	}

	int UIPopupCommunity_ClickFindUser( LuaState * L)
	{
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->FindUserButton();

		return 0;
	}

	int UIPopupCommunity_ClickAddFriend( LuaState * L)
	{
		UIFloatCommunity * pThis = static_cast<UIFloatCommunity *>(GameUI::GetFloating(UFW_COMMUNITY));
		if( pThis != nullptr)
			pThis->AddFriendButton();

		return 0;
	}
}

LuaFuncReg Community_glue[] =
{
	{"ClickTab",		UIPopupCommunity_ClickTab},
	{"ClickSubTab",		UIPopupCommunity_ClickSubTab},
	{"ClickButton",		UIPopupCommunity_Click},
	{"ReadMail",		UIPopupCommunity_ReadMail},
	{"ClickUser",		UIPopupCommunity_ClickUser},
	{"OpenUserPopup",	UIPopupCommunity_ClickOpenUserPopup},
	{"ClickFindUser",	UIPopupCommunity_ClickFindUser},
	{"ClickAddFriend",	UIPopupCommunity_ClickAddFriend},
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE( UIFloatCommunity);//, UITabBase);

UIFloatCommunity::UIFloatCommunity() : m_i32SelectIdx(-1)
{
	m_MainTab = MAINTAB_USERLIST;
	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;
	RegisterGameEventFunc( EVENT_UPDATE_USER_LIST,			&UIFloatCommunity::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_INSERT_FRIEND,				&UIFloatCommunity::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_CLAN_MEMBER_LIST_N,		&UIFloatCommunity::ReceiveGameEvent_Update_UserList);

	RegisterGameEventFunc( EVENT_DELETE_NOTE,				&UIFloatCommunity::ReceiveGameEvent_Update_DeleteNote);
	RegisterGameEventFunc( EVENT_AUTH_GIFT_GOODS,			&UIFloatCommunity::ReceiveGameEvent_Update_GiftGoods);
	RegisterGameEventFunc( EVENT_RECEIVE_NOTE,				&UIFloatCommunity::ReceiveGameEvent_Update_DeleteNote);
}

UIFloatCommunity::~UIFloatCommunity()
{
	m_CurrentUserTab = USERLIST_TAB_NONE;

	_ClearUserList();
}


/************************//************************/
/*virtual*/ void UIFloatCommunity::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_MessageBox_Ver10.i3UIs", Community_glue);
}

/*virtual*/ void UIFloatCommunity::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	m_pItemListBox = (i3UIListView_Box*) pScene->FindChildByName( "i3UIListView_Box");
	I3ASSERT( m_pItemListBox != nullptr );

	m_pItemListNote = (i3UIListView_Box*) pScene->FindChildByName( "ListView_Note");
	I3ASSERT( m_pItemListNote != nullptr );

	m_pMsgTab = (i3UITab*) pScene->FindChildByName( "i3UITab0");
	I3ASSERT( m_pMsgTab != nullptr );

	m_pUserTab = (i3UITab*) pScene->FindChildByName( "i3UITab");
	I3ASSERT( m_pUserTab != nullptr );

	

	m_pScrollRoomList = m_pItemListBox->getScrollBar();
	I3ASSERT( m_pScrollRoomList != nullptr );
}

/*virtual*/ void UIFloatCommunity::_ClearAtUnload( i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);
}

/*virtual*/ bool UIFloatCommunity::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//GetFirstFrameWnd()->EnableCtrl(false);

	LuaState * L = _CallLuaFunction("SetTab");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, 0);
		_EndLuaFunction(L , 1);
	}

	SelectUserTab(USERLIST_TAB_FRIEND);

	m_pScrollRoomList->setScrollPos(0);

	__BuildNoteDataList();
	m_eSortType = MAIL_SORT_INDEX;
	m_i32SelectIdx = -1;
	return true;
}

/*virtual*/ void UIFloatCommunity::OnEntranceEnd(void)
{
	UIFloatingBase::OnEntranceEnd();
	INT32 note_count = MailBoxContext::i()->GetNoteReceiveCount();

	SelectTab(0);

	if( note_count >= MAX_NORMAL_NOTE_COUNT)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_FULL_NOTE"));//쪽지함의 용량이 부족하여 더 이상 쪽지를 받으실 수 없습니다.\n불필요한 쪽지를 삭제해 주십시오.
	}
	else if( note_count >= ( MAX_NORMAL_NOTE_COUNT - 20 ) )
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_DELETE_NEEDLESS_NOTE"));//쪽지함의 용량이 얼마 남지 않았습니다.\n쪽지함이 꽉 차게 되면 쪽지를 받으실 수 없으니\n불필요한 쪽지는 삭제해 주십시오.
	}

	_UpdateNoteList();
}

void UIFloatCommunity::OnLoadAllScenes()
{
	m_bOnLoad = true;
}

bool UIFloatCommunity::OnExitStart()
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	return true;
}

void	UIFloatCommunity::OnUnloadAllScenes()
{
	m_bOnLoad = false;
}

/*virtual*/ void UIFloatCommunity::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate(rDeltaSeconds);

	if(m_bOnLoad)
	{
		if( m_CurrentUserTab == USERLIST_TAB_CLAN)
		{
			if( g_pFramework->GetClanContext() != nullptr)
				g_pFramework->GetClanContext()->UpdateClanMember( rDeltaSeconds);
		}
		UpdateAlarmNote(rDeltaSeconds);
	}
}

bool UIFloatCommunity::OnKey_Escape()
{
	return UIFloatingBase::OnKey_Escape();
}

bool UIFloatCommunity::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	return UIFloatingBase::OnKeyInput(pInputMng);
}

void UIFloatCommunity::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIFloatingBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIFloatCommunity::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatCommunity::ReceiveGameEvent_Update_UserList( INT32, const i3::user_data&)
{
	_UpdateUserList();
}
void UIFloatCommunity::ReceiveGameEvent_Update_DeleteNote( INT32, const i3::user_data&)
{
	__BuildNoteDataList();
	_UpdateNoteList();
	//__UpdateMailList( false );
}
void UIFloatCommunity::ReceiveGameEvent_Update_GiftGoods( INT32, const i3::user_data&)
{
	if( !MailBoxContext::i()->IsWaitResult() )
	{
		switch( MailBoxContext::i()->GetGiftReqType() )
		{
		case TYPE_SHOP_GIFT_READ :
			{
				_UpdateNoteList();
				//__UpdateMailList( true );
			}
			break;
		case TYPE_SHOP_GIFT_AUTH :
			{
				_UpdateNoteList();
				//__UpdateMailList( true );
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));
			}
			break;
		case TYPE_SHOP_GIFT_DELETE :
			{
				__BuildNoteDataList();
				_UpdateNoteList();
				//__UpdateMailList( false );

				INT32 i32Count =  MailBoxContext::i()->GetGiftReqSuccessCount();
				i3::rc_wstring wstrMsg = i3::format_string( GAME_RCSTRING("STR_GIFT_DELETEALL"), i32Count);

				GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg );
			}
			break;
		}		

		i3UI::setDisableUIInput(false);
		MailBoxContext::i()->ResetGiftReqList();
	}
}

void UIFloatCommunity::_ClearUserList( void)
{
	i3::cu::for_each_delete_clear(m_UserInfoList);
}

void UIFloatCommunity::_UpdateFriendList()
{
	UINT32 i;

	for( i = 0; i  < CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
		if(nullptr == node)	continue;

		USERINFO * pInfo = new USERINFO;
		pInfo->Reset();
		pInfo->CopyFromFriend( node);

		// 색
		if( FRIEND_STATE_OFFLINE == pInfo->_friendState && FRIEND_STATE_CANT_INVITE != pInfo->_friendState )
		{	// offline
			i3Color::Copy( &pInfo->_NickColor, GameGUI::GetColor(GCT_DEFAULT));
		}

		m_UserInfoList.push_back( pInfo);
	}
}

void UIFloatCommunity::_UpdateClanMemberList( void)
{
	UINT32 i;

	I3ASSERT( CCommunityContext::i() != nullptr);

	for( i = 0; i < CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* node = CCommunityContext::i()->GetViewClanMember( i);

		if( node)
		{
			USERINFO * pInfo = new USERINFO;
			pInfo->Reset();
			pInfo->CopyFromClanUser( node);

			m_UserInfoList.push_back( pInfo );
		}
	}
}

void UIFloatCommunity::_UpdateUserList( void)
{
	_ClearUserList();

	switch( m_CurrentUserTab)
	{
	case USERLIST_TAB_FRIEND :	_UpdateFriendList();		break;
	case USERLIST_TAB_CLAN :
		if( m_bReqClanMemberList == false)
		{
			//GameEventSender::i()->SetEvent( EVENT_CLAN_COMMUNITY_LIST);
			m_bReqClanMemberList = true;
		}
		else
			_UpdateClanMemberList();
		break;
	}

	LuaState * L = _CallLuaFunction("InitUserList");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, m_UserInfoList.size());
		_EndLuaFunction(L , 1);
	}
	// Set Slot
	_SetUserSlot();

}

void UIFloatCommunity::_UpdateNoteList( void)
{
	for(INT32 idx = 0; idx < GetNoteCount() ; ++idx)
	{
		//NOTE_RECEIVE * pNote = m_NoteList.at(idx).pNote;

		//_SetMailData(idx, pNote, bMailStateOnly);
		//_SetMailTextColor(idx, pNote);
	}

	LuaState * L = _CallLuaFunction("InitNoteList");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, m_NoteList.size());
		_EndLuaFunction(L , 1);
	}
	// Set Slot
	_SetNoteSlot();

}

void UIFloatCommunity::UpdateAlarmNote(REAL32 rDeltaSeconds)
{
	if( m_bAlarmCommunity)
	{
		m_rAlarmTerm += rDeltaSeconds;

		if (0.25f < m_rAlarmTerm)
		{
			m_rAlarmTerm = 0.0f;
			m_bAlarmFlag = !m_bAlarmFlag;
		}
		if(m_bAlarmFlag)
			m_pMsgTab->OnTabSelected();
		else
			m_pMsgTab->RemoveTabSelect();
	}
}

void UIFloatCommunity::CheckUnReadNote()
{
	if(m_MainTab == MAINTAB_USERLIST)
	{
		const NOTE_RECEIVE * pNote = nullptr;

		INT32 nCount = MailBoxContext::i()->GetNoteReceiveCount();
		for(INT32 i = 0 ; i < nCount ; ++i)
		{
			pNote = MailBoxContext::i()->GetReceiveNote(i);
			if(pNote->header._state == NS_UNREAD)
			{
				StartHighlightNote();
				return;
			}
		}
	}
	EndHighlightNote();
}

void UIFloatCommunity::StartHighlightNote( void)
{
	m_bAlarmCommunity = true;
	m_bAlarmFlag = true;
	m_rAlarmTerm = 0.0f;
}

void UIFloatCommunity::EndHighlightNote( void)
{
	m_bAlarmCommunity = false;
	m_bAlarmFlag = false;

	if(m_MainTab == MAINTAB_USERLIST)
		m_pMsgTab->OnTabSelected();
	else
		m_pMsgTab->RemoveTabSelect();

}

void UIFloatCommunity::_SetNoteSlot(void)
{
	for(INT32 idx = 0; idx < GetNoteCount() ; ++idx)
	{
		NOTE_RECEIVE * pNote = m_NoteList.at(idx).pNote;

		COLOR col;
		i3::wstring ReamindDay;
		i3::sprintf(ReamindDay, GAME_STRING("STR_POPUP_WHAT_DAY"), pNote->header._remainDay);

		if(pNote->header._state == NS_UNREAD)
			i3Color::Set(&col, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);
		else
			i3Color::Set(&col, (UINT8)136, (UINT8)136, (UINT8)139, (UINT8)255);

		m_pItemListNote->SetItemCellTextColor(idx, 0, &col, false);
		m_pItemListNote->SetItemCellTextColor(idx, 1, &col, false);
		m_pItemListNote->SetItemCellTextColor(idx, 2, &col, false);
		m_pItemListNote->SetItemCellTextColor(idx, 3, &col, false);

		m_pItemListNote->SetItemCell(idx, 0, 0);	// 메시지
		m_pItemListNote->SetItemCell(idx, 1, pNote->body._message);	// 메시지
		m_pItemListNote->SetItemCell(idx, 2, pNote->body._sender);		// 유저상태
		m_pItemListNote->SetItemCell(idx, 3, ReamindDay.c_str());	// 남은날짜

	}
}
void UIFloatCommunity::_SetUserSlot(void)
{
	if( m_CurrentUserTab != USERLIST_TAB_NONE)
	{
		size_t count = m_UserInfoList.size();

		for( size_t i = 0; i < count; i++)
		{
			if( i < m_UserInfoList.size())
			{
				USERINFO * pUser = getUserInfo( i);

				_SetUserInfo(i, pUser);
			}
			else
			{
				_SetUserInfo( i, nullptr);
			}
		}
	}
}

void UIFloatCommunity::_SetUserInfo( INT32 idx, const USERINFO * pInfo)
{
	//// 무언가 이유가 있어서 nConnectIdx = 0놓고 변수변경없이 뒤에 분기문 처리를 한거 같은데..
	//// 꼰대리티가 이렇게 하는거 싫어합니다.   제가 봐도 이렇게 하는거 별루구요....
	//// 일단은 불필요한 코드는 삭제없이 모두 주석처리합니다.. ( 16.07.29.수빈)
	
	//INT32 nConnectIdx = 0;

	//LuaState * L;
	//i3::rc_wstring wstrState;

	if( pInfo != nullptr && !pInfo->_wstrNick.empty() )
	{
		//	계급
		INT32 rank = (INT32) pInfo->_Rank;

		if( ChannelContext::i()->IsBeginnerChannel1())	//	초보 서버
			rank = RANK_NEWBIE;

		if( rank >= RANK_MAX)
			rank = RANK_MAX-1;

		m_pItemListBox->SetItemCell(idx, 0, pInfo->_Rank);			// 계급
		m_pItemListBox->SetItemCell(idx, 1, pInfo->_wstrNick.c_str());		// 닉네임
		m_pItemListBox->SetItemCell(idx, 2, pInfo->_szPosition.c_str());		// 유저상태

		COLOR col;
//		if(nConnectIdx == 0)
//		{
			i3Color::Set(&col, pInfo->_NickColor.r, pInfo->_NickColor.g, pInfo->_NickColor.b, pInfo->_NickColor.a);
			m_pItemListBox->SetItemCellTextColor(idx, 1, &col, false);
			i3Color::Set(&col, GameGUI::GetColor( GCT_VALID_C));
			m_pItemListBox->SetItemCellTextColor(idx, 2, &col, false);
//		}
//		else
//		{
//			i3Color::Set(&col, GameGUI::GetColor( GCT_INVALID_C));
//			m_pItemListBox->SetItemCellTextColor(idx, 1, &col, false);
//			m_pItemListBox->SetItemCellTextColor(idx, 2, &col, false);
//		}

	}			
	else
	{
		m_pItemListBox->SetItemCell(idx, 0, -1);			// 계급
		m_pItemListBox->SetItemCell(idx, 1, "");		// 닉네임
		m_pItemListBox->SetItemCell(idx, 2, "");		// 유저상태
	}
	

}

void UIFloatCommunity::SelectUserTab( INT32 tab)
{
	if( tab > -1 && tab < USERLIST_TAB_MAX)
	{
		m_CurrentUserTab = (USERLIST_TAB)tab;

		_UpdateUserList();
	}		
}


/************************//************************/
void UIFloatCommunity::SelectTab(int idx)
{
	LuaState * L = _CallLuaFunction("SetTab");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, idx);
		_EndLuaFunction(L , 1);
	}

	m_MainTab = (MAINTAB)idx;
	m_CurrentUserTab = USERLIST_TAB_FRIEND;

	if( m_MainTab == MAINTAB_USERLIST)
	{
		_UpdateUserList();
		StartHighlightNote();
		CheckUnReadNote();
		m_pUserTab->OnTabSelected();
		m_pMsgTab->RemoveTabSelect();
	}
	else
	{
		EndHighlightNote();
		m_pUserTab->RemoveTabSelect();
		m_pMsgTab->OnTabSelected();
		_UpdateNoteList();
	}
}

void UIFloatCommunity::SelectSubTab(int idx)
{
	m_CurrentUserTab = (USERLIST_TAB)idx;
	_UpdateUserList();
}

void UIFloatCommunity::Click(int idx)
{
	switch( idx )
	{
	case BUTTON_EXIT: 
		{
			//GetFirstFrameWnd()->EnableCtrl(false);
			UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
			pUIFrame->GetTopMenu_V10()->SetEnableCommunity(false);
		}
		break;
	case BUTTON_SEND:
		{
			SendMail();
		}
		break;
	case BUTTON_DELETE:
		{
			DeleteMail();
		}
		break;
	default:
		break;
	}

}

void UIFloatCommunity::ListClick( int idx , int listidx)
{
	switch( idx)
	{
	case LIST_LCLICK:
		_OpenUserInfo(listidx);
		break;
	case LIST_RCLICK:
		_OpenUserAction(listidx);
		break;
	default:
		break;
	}
}


void UIFloatCommunity::SwitchClick()
{
	//GetFirstFrameWnd()->EnableCtrl(!GetFirstFrameWnd()->isEnable());
}

/************************//************************/

/*=============================================================================*/

void UIFloatCommunity::__BuildNoteDataList(void)
{
	m_NoteList.clear();

	_Note note;
	
	INT32 i32NoteCnt = MailBoxContext::i()->GetNoteReceiveCount();

	for( int idx = 0; idx < i32NoteCnt; ++idx )
	{
		note.idx	= idx;
		note.pNote	= const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetReceiveNote(idx));

		m_NoteList.push_back( note );
	}

	std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_idx );

	CheckUnReadNote();
}

/*virtual*/ INT32 UIFloatCommunity::_GetMailType(NOTE_RECEIVE & Note)
{
	INT32 i32MailType = UIPopupReceiveMail::READ;

	if(Note.header._type == NT_CLAN_ASK)
	{
		if(Note.header._state == NS_UNREAD)
			i32MailType = UIPopupReceiveMail::CLAN_READ_ASK;
		else
			i32MailType = UIPopupReceiveMail::CLAN_READ;
	}
	else if(Note.header._type == NT_GIFT)
	{
		i32MailType = UIPopupReceiveMail::GIFT;
	}
	else
	{
		if(Note.header._type == NT_CLAN)
			i32MailType = UIPopupReceiveMail::CLAN_READ;
		else
			i32MailType = UIPopupReceiveMail::READ;

		if(Note.header._state != NS_READED)
		{
			Note.header._state = NS_READED;	// 읽음상태로 변경합니다.
			Note.header._remainDay = 7;		// 읽은 상태의 쪽지는 남은기간을 7일로 설정합니다.
		}
	}

	return i32MailType;
}


void UIFloatCommunity::SendMail(void)
{
	UIPopupReceiveMail::OPEN_DATA openData;

	openData.mailType	= UIPopupReceiveMail::SEND;	
	openData.wstrNickName.clear();	

	if( true == _IsSafeNoteIdx( m_i32SelectIdx) )
	{
		_Note& note = m_NoteList[ m_i32SelectIdx ];
		openData.wstrNickName = note.pNote->body._sender;
		openData.noteIdx = note.pNote->header._id;
	}

	GetMainFrame()->OpenPopup( UPW_MAILREV, &openData );
}

void UIFloatCommunity::ReadMail(INT32 i32Idx)
{
	if( false == _IsSafeNoteIdx( i32Idx ) )
		return;

	_Note& note = m_NoteList[ i32Idx ];

	UIPopupReceiveMail::OPEN_DATA openData;

	openData.mailType = static_cast<UIPopupReceiveMail::MAILTYPE>( _GetMailType( *(note.pNote)));

	if(openData.mailType == UIPopupReceiveMail::GIFT)
		_GetAttachedItemInfo(note.pNote, openData.wstrAttachedItem);

	openData.noteIdx		= note.pNote->header._id;
	openData.noteAppendData = (int)note.pNote->header._appendingData;
	openData.wstrNickName	=note.pNote->body._sender;
	openData.wstrMessage		= note.pNote->body._message;

	int nSelCnt = 0;
	int nSelList[ 100 ];
	const NOTE_RECEIVE* pNote = note.pNote;
	nSelList[ nSelCnt++ ] = pNote->header._id;

	GameEventSender::i()->SetEvent( EVENT_CHECK_READED_NOTE, &nSelCnt, nSelList );

	_UpdateNoteList();
	GetMainFrame()->OpenPopup( UPW_MAILREV, &openData );
}

void UIFloatCommunity::DeleteMail(void)
{	
	int idx = 0;
	int nSelCnt = 0;
	int nSelList[ 100 ];

	UINT8		ui8Type			= TYPE_SHOP_GIFT_DELETE;
	UINT8		ui8GiftCount	= 0;
	T_GiftDBIdx aGiftIdx[ MAX_SHOP_GOODS_BUY_COUNT ];
	i3mem::FillZero( aGiftIdx, sizeof(aGiftIdx));

	for each( const _Note & note in m_NoteList )
	{
		const NOTE_RECEIVE* pNote = note.pNote;

		if( false == m_pItemListNote->isCheckedData( idx++ , 0 ) )
			continue;

		if( nullptr == pNote )
			continue;

		//@eunil. 1.0은 선물함 개념이 없으므로 바로 인벤으로 들어간다.
		//임시 크래시 방어코드
		if(pNote->header._type == NT_GIFT)
		{
			const GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);
			if (pGift) {
				aGiftIdx[ui8GiftCount++] = pGift->_i32GiftWareDBIdx;
			}
			else {
				nSelList[nSelCnt++] = pNote->header._id;
			}
		}
		else
		{
			nSelList[ nSelCnt++ ] = pNote->header._id;
		}
	}

	//_SendEvent(ACTION_DELETE, nSelList, nSelCnt );
	if(nSelCnt > 0)
		GameEventSender::i()->SetEvent( EVENT_DELETE_NOTE, &nSelCnt, nSelList );

	if( ui8GiftCount > 0)
		GameEventSender::i()->SetEvent( EVENT_AUTH_GIFT_GOODS, &ui8Type, &ui8GiftCount, aGiftIdx );
}

/*virtual*/ void UIFloatCommunity::_GetAttachedItemInfo(const NOTE_RECEIVE* pNote, i3::rc_wstring& wstrItemInfo)
{
	wstrItemInfo = GAME_RCSTRING("STR_POPUP_UNKNOWN_GIFT");

	//	선물하기 Note에서 pNote->body._user 포인터는 GIFT_BUFFER 구조체 포인터임.
	GIFT_BUFFER * pGift = (GIFT_BUFFER*)pNote->body._user;

	if (pGift == nullptr)
	{
		// GD팀 요청사항으로 상품 정보가 없을 시(서버에 선물샵이 닫혀 있을 경우) UI 1.0에만 있는 상품 정보 미출력 시킨다. - sanghun.han
		wstrItemInfo = L"";

		return;
	}

	SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData( pGift->_ui32GoodsID );

	if( pGoodsList == nullptr )
		return;


	if( pGoodsList->_ui8ItemCount > 1 )
	{
		bool isPeriod = false;
		INT32 i32PeriodIdx = 0;

		for(INT32 j = 0; j < pGoodsList->_ui8ItemCount; j++)
		{
			if (ITEM_TYPE_TIME == pGoodsList->_ppItemData[j]->_ui8AuthType)
			{
				isPeriod = true;
				i32PeriodIdx = j;
				break;
			}
		}

		//	getInfo는 keyItem으로 가져와야 정확한 info를 얻을 수 있다. (그렇지않으면 nullptr일 수 있다.)
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo( pGift->_ui32GoodsID );
		I3ASSERT( pInfo != nullptr);
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[i32PeriodIdx];

		if( isPeriod )
		{
			UINT32 day = ConvertSecToUseDay(ui32ItemArg);
			// 이름: %s\n사용기간: %d일
			wstrItemInfo = i3::format_string( GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), day );
		}
		else
		{
			T_ItemID ItemID = pGoodsList->_ppItemData[i32PeriodIdx]->_ui32ItemID;

			i3::rc_wstring wstrTemp;

			//	유료 아이템 Goods에는 pGoods->_AuthArg에 모두 갯수가 있기 때문에 예외 처리가 필요 komet
			switch( (ITEM_TYPE) GET_ITEM_TYPE(ItemID) )
			{	
				//	지속성
			case	ITEM_TYPE_MAINTENANCE	: wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"); break;		
				//	소모성
			case	ITEM_TYPE_WRAP_PERIOD	:
			case	ITEM_TYPE_WRAP_COUNT	: wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME"); break;
			default							: return;
			}

			wstrItemInfo = i3::format_string(wstrTemp, pInfo->GetScriptName(), ui32ItemArg);							
		}
	}
	else
	{
		UINT8 ui8AuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[0] ; 
		T_ItemID ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;

		if( GET_ITEM_TYPE(ItemID) == ITEM_TYPE_SALECOUPON )
		{
#if defined (USE_SALE_COUPON)
			SALE_COUPON * pCoupon = CShop::i()->GetCouponData(ItemID);

			i3::rc_wstring wstrCouponName;				// TODO : 서버 협업 요망..(2014.07.16.수빈)
			i3::mb_to_utf16(pCoupon->m_strCouponName, wstrCouponName);
			wstrItemInfo = wstrCouponName;
#endif		
		}
		else
		{
			i3::rc_wstring wstrTemp;
			i3::rc_wstring wstrItemName;		
			usf::GetShopItemName(wstrItemName, ItemID);

			I3ASSERT( ui8AuthType != 0 );
			switch( ui8AuthType )
			{
			case ITEM_TYPE_COUNT :
				{
					if( item_def::get_category(ItemID) == item_def::CTG_ITEM )
					{
						if (GET_ITEM_TYPE(ItemID) != ITEM_TYPE_WRAP_COUNT)
						{
							UINT32 ui32ItemTerm = GET_ITEM_SUBTYPE(ItemID);
							if (ui32ItemTerm > 0)
								wstrItemName = i3::format_string(GAME_RCSTRING("STR_INVEN_CASH_ITEM_TERM"), wstrItemName, ui32ItemTerm);
						}
					}

					wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME");
				}
				break;
			case ITEM_TYPE_TIME :
				{
					wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME");
					ui32ItemArg = (INT32) ConvertSecToUseDay(ui32ItemArg);	//	인증기간 (일)로 변환
				}
				break;
			}

			wstrItemInfo = i3::format_string(wstrTemp, wstrItemName, ui32ItemArg);
		}	
	}
}

void UIFloatCommunity::_OpenUserAction( INT32 idx )
{
	const USERINFO * pInfo = getUserInfo(idx);

	if( pInfo != nullptr)
	{
		GetMainFrame()->ResetUserActionPopup();

		switch( m_CurrentUserTab)
		{
		case USERLIST_TAB_FRIEND :
			_FriendUserAction(pInfo);
			break;
		case USERLIST_TAB_CLAN :
			_ClanUserAction(pInfo);
			break;
		default : 
			break;
		}

		GetMainFrame()->OpenUserActionPopup( this, idx);
	}
}

void UIFloatCommunity::_FriendUserAction( const USERINFO * pInfo )
{
	switch(pInfo->_friendState)
	{
	case FRIEND_STATE_LOBBY:
	case FRIEND_STATE_ONLINE:
		{
			// 클랜전이 아닐경우
			if( true == g_pFramework->GetClanContext()->InBattleTeam())
				break;

			// 내가 레디룸에 있고 상대가 온라인이면
			if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
				GetMainFrame()->AddUserActionCommand( UCM_INVITE);
		} break;
	case FRIEND_STATE_ROOM:
		{
			// 클랜전이 아닐경우
			if( true == g_pFramework->GetClanContext()->InBattleTeam())
				break;

			// 상대가 룸에 있고 내가 레디룸이 아니면 찾아가기
			if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_READY_ROOM)
			{
				GetMainFrame()->AddUserActionCommand( UCM_FIND_AWAY);
				break;
			}

			// 상대가 룸에 있고 나랑 같은 방이 아닐 경우
			if( gcu::IsSameRoom(pInfo->_friendState) == false)
			{
				GetMainFrame()->AddUserActionCommand( UCM_INVITE);
				GetMainFrame()->AddUserActionCommand( UCM_FIND_AWAY);
			}
			
		} break;
	}

	// 클랜 초대는 내 클랜 아닐경우 전부 초대 됨.
	// 이게 V1인데 이렇게 밖에 못하는 경우인가보다. 왜?!
	if( CanClanInvite() )
	{
		if( g_pFramework->GetClanContext()->IsPossibleClanInvite() && 
			(_IsMyClanMember(pInfo->_i64UID) == false))
		{	
			GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);
		}
	}

	// 귓속말
	//@eunil. Russia 요청사항 채팅창이 떠있어야 합니다.
	if ( (GetMainFrame()->IsEnableFloatWin(UFW_CHAT) || GetMainFrame()->IsEnableFloatWin(UFW_READYROOM_CHAT)) && pInfo->_friendState != FRIEND_STATE_OFFLINE)
		GetMainFrame()->AddUserActionCommand( UCM_WHISPER);
}

void UIFloatCommunity::_ClanUserAction( const USERINFO * pInfo )
{
	const CLAN_MEMBER_NODE * pNode = _GetClanMemberNode(pInfo->_i64UID);

	// 귓속말
	//@eunil. Russia 요청사항 채팅창이 떠있어야 합니다.
	if ( (GetMainFrame()->IsEnableFloatWin(UFW_CHAT) || GetMainFrame()->IsEnableFloatWin(UFW_READYROOM_CHAT)) && pNode->_state != FRIEND_STATE_OFFLINE)
		GetMainFrame()->AddUserActionCommand( UCM_WHISPER);

	// 친구 추가
	if( _IsMyFriend(pInfo->_i64UID) == false)
		GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);

	switch(pInfo->_friendState)
	{
	case FRIEND_STATE_ONLINE:
	case FRIEND_STATE_LOBBY:
		{
			if( GET_FRIEND_MATCH(pNode->_state) != MATCH_TEAM_INVALID_IDX) // 클랜 매치 상태 일 때
			{
				if( false == g_pFramework->GetClanContext()->InBattleTeam()) // 내가 팀에 없을 경우
				{
					GetMainFrame()->AddUserActionCommand(UCM_FIND_AWAY); // 찾아가기
				}
			}
			else // 상대가 클랜 매치 아닐 경우
			{
				if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM) // 내가 레디룸에 있을 경우
				{
					// 초대하기
					GetMainFrame()->AddUserActionCommand( UCM_INVITE);
				}
				else
				{
					// 클랜전 일 경우 
					if( true == g_pFramework->GetClanContext()->InBattleTeam())
					{
						// 초대하기
						GetMainFrame()->AddUserActionCommand( UCM_INVITE);	
					}
				}
			}
		} break;
	case FRIEND_STATE_ROOM:
		{
			if( GET_FRIEND_MATCH(pNode->_state) == MATCH_TEAM_INVALID_IDX) // 클랜 매치 상태가 아니지만 레디룸에 있을 경우
			{
				if( false == g_pFramework->GetClanContext()->InBattleTeam()) // 내가 클랜 매치 팀에 없을 경우
				{
					if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM) // 내가 일반 방에 있을 경우
					{
						// 초대하기
						GetMainFrame()->AddUserActionCommand( UCM_INVITE);	
					}

					GetMainFrame()->AddUserActionCommand(UCM_FIND_AWAY); // 찾아가기
				}
				else // 내가 클랜 매치 일 경우
				{
					// 초대하기
					GetMainFrame()->AddUserActionCommand( UCM_INVITE);	
				}
			}
		} break;
	}
}

void UIFloatCommunity::_OpenUserInfo( INT32 idx )
{
	const USERINFO * pInfo = getUserInfo(idx);

	if(pInfo != nullptr)
	{
		// 닉네임 등록
		CCommunityContext::i()->SetCommnunityInfoNick(pInfo->_wstrNick);

		if(pInfo->_i64UID < 0)
			return;

		INT64 UserUID = 0;
		UserUID = pInfo->_i64UID;

		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_UID, &UserUID);

		INT32 idx2 = 2;
		if (m_CurrentUserTab == USERLIST_TAB_CLAN) idx2 = 4;

		if( GetMainFrame()->IsOpenedPopup( UPW_USERPROFILE) == false)
			GetMainFrame()->TogglePopup(  UPW_USERPROFILE, &idx2);
	}
}

bool UIFloatCommunity::_IsMyClanMember( INT64 uid )
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)		return true;
	}

	return false;
}

bool UIFloatCommunity::_IsMyFriend( INT64 uid )
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetFriendCount(); i++)
	{
		const FRIEND_NODE* pNode = CCommunityContext::i()->GetViewFriend(i);
		if(nullptr == pNode) continue;

		if( pNode->_uid == uid)
			return true;
	}

	return false;
}

const CLAN_MEMBER_NODE * UIFloatCommunity::_GetClanMemberNode( INT64 uid )
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)
		{
			return pNode;
		}
	}

	return nullptr;
}

i3::rc_wstring UIFloatCommunity::GetNickForUserIdx( INT32 idx )
{
	USERINFO * pInfo = getUserInfo(idx);

	if( pInfo != nullptr)
	{
		return pInfo->_wstrNick;
	}

	return i3::rc_wstring();
}

void UIFloatCommunity::FindUserButton()
{
	INT32 idx = 1;
	GetMainFrame()->OpenPopup(UPW_USERSEARCH, &idx);
}

void UIFloatCommunity::AddFriendButton()
{
	INT32 idx = 2;
	GetMainFrame()->OpenPopup(UPW_USERSEARCH, &idx);
}
