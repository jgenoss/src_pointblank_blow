#include "pch.h"
#include "UIFloatUserList.h"

#include "UIMainFrame.h"
#include "UIPhaseReadyRoom.h"
#include "UIFloatChat.h"
#include "UIUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "../CommunityContext.h"
#include "../GameGUI.h"

#include "InviteContext.h"
#include "LobbyContext.h"
#include "BattleSlotContext.h"
#include "ChannelContext.h"
#include "UserInfoContext.h"

extern "C" {
	int UIFloatUserList_ClickTab( LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1);
		UIFloatUserList * pThis = static_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
		if( pThis != nullptr)
			pThis->SelectUserTab( tab);

		return 0;
	}

	int UIFloatUserList_OpenUserPopup( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 2);
		UIFloatUserList * pThis = static_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
		if( pThis != nullptr)
			pThis->OpenUserPopup( slot);
		return 0;
	}

	int UIFloatUserList_SelectWhisper( LuaState * L)
	{
		INT32 slot = i3Lua::GetIntegerArg( L, 2);
		UIFloatUserList * pThis = static_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
		if( pThis != nullptr)
			pThis->SelectWhisper( slot);
		return 0;
	}

	int UIFloatUserList_ClickAnotherWnd( LuaState * L)
	{
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(i3Lua::GetUserDataArg(L , 1));
		bool bUserList = i3Lua::GetBooleanArg(L , 2) == true ? true : false;

		UIFloatUserList * pThis = static_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
		if( pThis != nullptr)
			pThis->CloseCurrentFrame(pCtrl, bUserList);
		return 0;
	}

	int UIFloatUserList_SearchUser( LuaState * L)
	{
		UIFloatUserList * pThis = static_cast<UIFloatUserList *>(GameUI::GetFloating(UFW_USERLIST));
		if( pThis != nullptr)
			pThis->SearchUser();
		return 0;
	}
};

LuaFuncReg UIFloatUserList_Glue[] = {
	{"ClickTab",				UIFloatUserList_ClickTab},
	{"OpenUserPopup",			UIFloatUserList_OpenUserPopup},
	{"SelectWhisper",			UIFloatUserList_SelectWhisper},
	{"ClickAnotherWnd",			UIFloatUserList_ClickAnotherWnd},
	{"SearchUser",				UIFloatUserList_SearchUser},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatUserList);//, UIFloatingBase);

UIFloatUserList::UIFloatUserList()
{
	m_CurrentUserTab = USERLIST_TAB_NONE;
	m_bReqUserInfo = false;

	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;

#if defined( I3_DEBUG)
	SetName( "UserList");
#endif

	m_RSelectedIndex = -1;
	m_SelectedIndex = -1;

	m_pSearchEdit = nullptr;
	m_bUseSearchFrame = false;

	m_bReqUserInfo = false;

	m_bFriendAlarm = false;
	m_bAlarmFlag = false;
	m_rAlarmTerm = 0.0f;
	m_rUpdateUserListTime = 0;
	m_bUserListFrame = false;

	RegisterGameEventFunc( EVENT_INSERT_FRIEND,					&UIFloatUserList::ReceiveGameEvent_Insert_Friend);
	RegisterGameEventFunc( EVENT_CLAN_MEMBER_LIST_N,			&UIFloatUserList::ReceiveGameEvent_Clan_MemberList);
	RegisterGameEventFunc( EVENT_DELETE_BLOCK,					&UIFloatUserList::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_UPDATE_USER_LIST,				&UIFloatUserList::ReceiveGameEvent_Update_UserList);
	RegisterGameEventFunc( EVENT_GET_LOBBY_USER,				&UIFloatUserList::ReceiveGameEvent_GetLobbyUser);
	RegisterGameEventFunc( EVENT_GET_USERINFO_SESSION,			&UIFloatUserList::ReceiveGameEvent_Get_UserInfo_Session);
}

UIFloatUserList::~UIFloatUserList()
{
	_ClearUserList();
}


/*virtual*/ void UIFloatUserList::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);
	m_pSearchEdit = (i3UIEditBox*) pScene->FindChildByName("Edit_Search");

	m_pMainWnd[0] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd0");
	m_pMainWnd[1] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_UserList");
	m_pMainWnd[2] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_SearchList");
	m_pMainWnd[3] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_UserListTab");
}

/*virtual*/ void UIFloatUserList::_ClearAtUnload(i3UIScene * pScene)
{
	UIFloatingBase::_ClearAtUnload( pScene);

	m_pSearchEdit = nullptr;
	for(size_t i=0;i<UserListMaxFrm;++i)
	{
		m_pMainWnd[i] = 0;
	}
}

void UIFloatUserList::_ClearUserList( void)
{
	i3::cu::for_each_delete_clear(m_UserInfoList);
}

void UIFloatUserList::_SetUserSlot(void)
{
	if( m_CurrentUserTab != USERLIST_TAB_NONE)
	{
		size_t count = m_UserInfoList.size();

		LuaState *	L = _CallLuaFunction("ClearUserListBox");
		if( L != nullptr)
		{
			if( count < 10)
				count = 10;

			i3Lua::PushInteger(L, count);
			_EndLuaFunction( L, 1);
		}

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

void	UIFloatUserList::_SetActiveFrame(bool bUserlist)
{
	_SetUserListFrame(bUserlist);
	_SetSearchFrame(!bUserlist);

	LuaState * L = _CallLuaFunction("GetActiveFrame");
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L, bUserlist);
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L, 1));
		g_pFramework->SetFocus(pCtrl);
	}

	m_bReqUserInfo = false;
}

void UIFloatUserList::_SetUserListFrame(bool bView)
{
	LuaState * L = _CallLuaFunction("SetUserListFrame");
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L, bView == true ? true : false);
		_EndLuaFunction(L, 1);
	}


	// Key : User Guide System
	{
		if(bView == false )
			UserGuideMgr::i()->UnLoadGuideTargetInfo(this);
	}

	_RemoveOnMouseState();

	m_bUserListFrame = bView;

	if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM && m_bUserListFrame == true)
	{
		LobbyContext::i()->InitLobbyUserData();
		m_rUpdateUserListTime = 0;
		INT32 temp = USER_LIST_REQ_START;
		// hansoft 11844. 로비가 아닌 상태에서 유저 정보 획득. 매크로 마우스 광클에 대한 서버 디스커넥션 문제로 once 함수로 변경함.
		// 업데이트 시간 문제로 다시 Evnet로 변경합니다. 
		GameEventSender::i()->SetEvent(EVENT_GET_LOBBY_USER, &temp);	
		
	}
		

	//
	UINT32 ev = bView ? UIEVENT_SET_FOCUS_POPUP : UIEVENT_KILL_FOCUS_POPUP;
	GetMainFrame()->OnEvent( ev, this, 0, I3_EVT_CODE_ACTIVATE);
}

void UIFloatUserList::_SetSearchFrame(bool bView)
{
	LuaState * L = _CallLuaFunction("SetSearchFrame");
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L , bView);
		_EndLuaFunction(L, 1);
	}
	
	if(bView == true)
		m_pSearchEdit->SetText(L"");

	m_bUseSearchFrame = bView;
}

// 탭을 활성화 시킬지 여부 반환
bool UIFloatUserList::_CheckActiveTab(INT32 tab)
{
	LuaState * L = _CallLuaFunction("GetActiveFrame");
	
	if(L != nullptr)
	{
		i3Lua::PushBoolean(L, tab != USERLIST_TAB_SEARCH ? true : false);
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L, 1));

		// 현재 같은 프레임이 활성화 되있을 경우.
		if(pCtrl->isEnable())
		{
			// 이전 탭과 같은 탭을 선택했을 경우 해당 Frame을 Close 후 false 리턴
			if(tab == USERLIST_TAB_SEARCH)
			{
				_SetSearchFrame(false);
				m_CurrentUserTab = USERLIST_TAB_NONE;
				_ClearSelectTab();
				return false;
			}
			else
			{
				if(tab == m_CurrentUserTab)
				{
					_SetUserListFrame(false);
					m_CurrentUserTab = USERLIST_TAB_NONE;
					_ClearSelectTab();
					return false;
				}
			}
		}	
	}

	return true;
}

void UIFloatUserList::_SetUserInfo( INT32 idx, const USERINFO * pInfo)
{
	LuaState * L;
	i3::rc_wstring wstrRankToolTip;
	i3::rc_wstring wstrReasonToolTip;

	if( pInfo != nullptr && !pInfo->_wstrNick.empty() )
	{
		//	계급
		INT32 rank = (INT32) pInfo->_Rank;

		if( ChannelContext::i()->IsBeginnerChannel1())	//	초보 서버
			rank = RANK_NEWBIE;

		if( rank >= RANK_MAX)
			rank = RANK_MAX-1;

		wstrRankToolTip = GetCharaRank( rank);

		INT32 nConnectIdx = 0;

		switch(m_CurrentUserTab)
		{
		case USERLIST_TAB_CURRENT:	
			nConnectIdx = 0; 
			break;
		case USERLIST_TAB_FRIEND:
		case USERLIST_TAB_CLAN:	
			{
				switch(pInfo->_friendState)
				{
				case FRIEND_STATE_REQUEST:
				case FRIEND_STATE_HOLD:
					nConnectIdx = 2;
					break;
				case FRIEND_STATE_OFFLINE:
					nConnectIdx = 1;
					break;
				case FRIEND_STATE_ONLINE:
				case FRIEND_STATE_LOBBY:
				case FRIEND_STATE_ROOM:
				case FRIEND_STATE_CANT_INVITE:
					nConnectIdx = 0;
					break;
				}
			}			
			break;
		default:
			nConnectIdx = -1;
			break;

		}

		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, idx);
			i3Lua::PushInteger( L, nConnectIdx);
			i3Lua::PushInteger( L, rank);
			i3Lua::PushStringUTF16To8( L, wstrRankToolTip);
			i3Lua::PushStringUTF16To8( L, pInfo->_wstrNick);
			i3Lua::PushColor( L, (I3COLOR*) &pInfo->_NickColor);
			_EndLuaFunction( L, 9);
		}
	}
	else
	{
		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, idx);
			i3Lua::PushInteger( L, -1);
			i3Lua::PushInteger( L, -1);
			i3Lua::PushStringUTF16To8( L, L"");
			i3Lua::PushStringUTF16To8( L, L"");
			i3Lua::PushColor( L, GameGUI::GetColor( GCT_DEFAULT));
			_EndLuaFunction( L, 9);
		}
	}
}


/*virtual*/ void UIFloatUserList::OnCreate(i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_UserList.i3UIs", UIFloatUserList_Glue, true, false);
}

/*virtual*/ bool UIFloatUserList::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	m_slide.ForceEnd();	//slide 행동을 강제한다.

	if( m_pSearchEdit )
		m_pSearchEdit->SetLetterLimit( 16 );

	LuaState * L = _CallLuaFunction("InitFrameEnable");
	if(L != nullptr)
	{
		_EndLuaFunction(L , 0);
	}

	m_bUseSearchFrame = false;

	return true;
}

/*virtual*/ void UIFloatUserList::OnEntranceEnd(void)
{
	// 로그인 후 최초 1회에만, 알람을 설정합니다.
	if( CCommunityContext::i()->isFirstFriendAlarm() == false)
	{
		for(UINT32 idx = 0 ; idx < CCommunityContext::i()->GetFriendCount() ; ++idx)
		{
			// 친구 탭 내 요청받은 내역이 있으면, 설정
			if(FRIEND_STATE_HOLD == GET_FRIEND_STATE( CCommunityContext::i()->GetFriend(idx)->_state))
			{
				_StartFriendAlarm();
				break;
			}
		}

		CCommunityContext::i()->SetFirstFriendAlarm(true);
	}

	UIFloatingBase::OnEntranceEnd();
}

/*virtual*/ bool UIFloatUserList::OnExitStart(void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	m_slide.ForceEnd();	//slide 행동을 강제한다.

	return true;
}

/*virtual*/ void UIFloatUserList::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	if( m_CurrentUserTab == USERLIST_TAB_CLAN)
	{
		if( g_pFramework->GetClanContext() != nullptr)
			g_pFramework->GetClanContext()->UpdateClanMember( rDeltaSeconds);
	}

	if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM && m_bUserListFrame == true)
	{
		m_rUpdateUserListTime += rDeltaSeconds;
		if (m_rUpdateUserListTime > 2.f)
		{
			m_rUpdateUserListTime = 0;
			INT32 temp = USER_LIST_REQ_UPDATE;
			GameEventSender::i()->SetEvent(EVENT_GET_LOBBY_USER, &temp);
		}

	}
	_UpdateFriendAlarm(rDeltaSeconds);
}


void UIFloatUserList::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
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

/*virtual*/ bool UIFloatUserList::OnKey_Enter( void)
{
	if(m_bUseSearchFrame)
	{
		SearchUser();
		return true;
	}

	return false;
}

/*virtual*/ bool UIFloatUserList::OnKey_Escape( void)
{
	return !_CheckActiveTab(m_CurrentUserTab);
}

void UIFloatUserList::SelectUserTab( INT32 tab)
{
	// 서브페이즈가 활성화되있다면 처리 안합니다.
	switch(GetMainFrame()->GetCurrentSubPhaseType())
	{
	case 		USP_SHOP:
	case		USP_GIFTSHOP:
	case 		USP_INVENTORY:
		_ClearSelectTab();
		return;
		break;
	}

	if( tab > -1 && tab < USERLIST_TAB_MAX)
	{
		// 탭을 활성화할 것인지 체크
		if(_CheckActiveTab(tab) == true)
		{
			m_CurrentUserTab = (USERLIST_TAB)tab;
			_AttachToTopScreen();
			
			switch(tab)
			{
			case USERLIST_TAB_CURRENT:
			case USERLIST_TAB_FRIEND:
			case USERLIST_TAB_CLAN:
				{
					// 친구탭을 누를 경우 알람을 끕니다.
					if(tab == USERLIST_TAB_FRIEND)
						_EndFriendAlarm();

					_SetActiveFrame(true);			
					_UpdateUserList();

					// Key : User Guide System
					{
						if(tab == USERLIST_TAB_CURRENT)
							UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
						else
							UserGuideMgr::i()->UnLoadGuideTargetInfo(this);
					}

					_ListView_ScrollTop();
				}
				break;
			case USERLIST_TAB_SEARCH:
				{
					_SetActiveFrame(false);
				}
				break;
			}
		}		
	}
}

bool UIFloatUserList::OnRevive( i3RenderContext * pCtx)
{
	if( UIFloatingBase::OnRevive( pCtx) == false)
		return false;

	enum { Right_Dec = 10 };

	for(size_t i=0;i<UserListMaxFrm;++i)
	{
		VEC3D* pos = m_pMainWnd[i]->getPos(); VEC2D* size = m_pMainWnd[i]->getSize();
		m_pMainWnd[i]->setPos( (REAL32) (GameUI::AutoPosition::get_max_right() - size->x - Right_Dec), pos->y );
	}

	return true;
}


void UIFloatUserList::OpenUserPopup( INT32 slot)
{
	// 2번 Send 되는것을 막습니다.
	if(m_bReqUserInfo == true)
		return;

	if( _SetSelectedUser( slot))
	{
		USERINFO * pInfo = getUserInfo( slot);

		if(pInfo != nullptr)
		{
			if( !UserInfoContext::i()->IsMyNickName( pInfo->_wstrNick))
			{
				switch(m_CurrentUserTab)
				{
				case USERLIST_TAB_CURRENT:
					{
						if(pInfo->_Session > 0)
						{
							INT32  i32UserSessionIdx;
							i32UserSessionIdx = pInfo->_Session;
							
							GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_SESSION, &i32UserSessionIdx);
							GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_SUBTASK, &i32UserSessionIdx);
							
							// 유저리스트 가이드는 강제로 선택된 컨트롤을 체크합니다.
							_SendUserGuideCtrl( slot );
						}						
					}
					break;
				case USERLIST_TAB_FRIEND:
				case USERLIST_TAB_CLAN:
					{
						if(pInfo->_i64UID > 0)
						{
							INT64  i32UserSessionIdx;
							i32UserSessionIdx = pInfo->_i64UID;

							GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_UID, &i32UserSessionIdx);
						}
					}
					break;
				case USERLIST_TAB_SEARCH:
					{
						if(i3::generic_string_size(pInfo->_wstrNick) > 0)
						{
							GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_NICK, pInfo->_wstrNick.c_str() );
						}
					}
					break;
				}

				_SetUserListFrame(false);
				_SetSearchFrame(false);
				_ClearSelectTab();

				m_bReqUserInfo = true;
			}
		}
	}
}

void UIFloatUserList::SelectWhisper(INT32 slot)
{
	if(_SetSelectedUser(slot))
	{
		USERINFO * pInfo = getUserInfo( slot);
		if(pInfo != nullptr)
		{
			if( ! UserInfoContext::i()->IsMyNickName( pInfo->_wstrNick))
			{
				UIFloatChat * pChat = static_cast<UIFloatChat *>(GameUI::GetFloating(UFW_CHAT));
				if( pChat != nullptr)
					pChat->OnWhisper(pInfo->_wstrNick);
			}
		}
	}
}


bool UIFloatUserList::_SetSelectedUser( INT32 slot)
{
	if( slot > -1 && slot < (INT32)m_UserInfoList.size())
	{
		m_RSelectedIndex = slot;

//		LOBBY_USER_INFO_LIST* pUser = (LOBBY_USER_INFO_LIST*) m_UserInfoList.Items[ m_RSelectedIndex];
//		i3mem::Copy( &m_SelectedUserInfo, pUser, sizeof( m_SelectedUserInfo));

		return true;
	}
	else
	{
		m_RSelectedIndex = -1;
//		m_SelectedUserInfo.Reset();
		return false;
	}
}

void UIFloatUserList::__UpdateLobbyListByLobby( void)
{
	INT32 i, j;
	i3::wliteral_range wstrNick;

	for( i = 0; i < LobbyContext::i()->GetLobbyUserCount(); i++)
	{
		const LOBBY_USER_INFO_LIST * pUser1 = LobbyContext::i()->getLobbyUserInfo( i);

		I3ASSERT( pUser1 != nullptr);

		if( pUser1->_Session < 0 )continue;
		if( pUser1->_Nick[0] == 0 )continue;
		wstrNick = pUser1->_Nick;
		if( CCommunityContext::i()->CheckBlockUser(wstrNick)) continue;	//차단 유저인 경우 리스트에서 제외

		//중복된 유저인지를 검사합니다.
		bool IsValidUser = true;
		for( j = 0; j < i; j++)
		{
			const LOBBY_USER_INFO_LIST * pUser2 = LobbyContext::i()->getLobbyUserInfo( j);
			if( pUser1->_Session == pUser2->_Session)
			{
				IsValidUser = false;
				break;
			}
		}

		//	정상적인 유저일 경우만 리스트에 등록
		if(IsValidUser)
		{
			USERINFO * pInfo = new USERINFO;
			pInfo->Reset();
			pInfo->CopyFromLobbyUserByLobby( pUser1);
			m_UserInfoList.push_back( pInfo );
		}
	}
}

void UIFloatUserList::_UpdateLobbyList( void)
{
	__UpdateLobbyListByLobby();
}

void UIFloatUserList::_UpdateFriendList()
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

void UIFloatUserList::_UpdateClanMemberList( void)
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

void UIFloatUserList::_UpdateUserList( void)
{
	_ClearUserList();

	switch( m_CurrentUserTab)
	{
	case USERLIST_TAB_CURRENT :	_UpdateLobbyList();		break;
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

	// Set Slot
	_SetUserSlot();

	// highlight my slot
	_UpdateMyHighlight();
}

void UIFloatUserList::_StartFriendAlarm( void)
{
	m_bFriendAlarm = true;
	m_bAlarmFlag = true;
	m_rAlarmTerm = 0.0f;
}

void UIFloatUserList::_EndFriendAlarm( void)
{
	m_bFriendAlarm = false;
	m_bAlarmFlag = false;

	LuaState * L = _CallLuaFunction("GetFriendTab");
	if( L != nullptr)
	{
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 0));

		if(pCtrl != nullptr)
		{
			POINT pt = i3UI::getManager()->getCurrentMousePosition();

			I3UI_OVERLAP_INFO info;

			info.x = (REAL32)pt.x;
			info.y = (REAL32)pt.y;

			if(pCtrl->isOnMouse(&info))
				pCtrl->addState( I3UI_STATE_ONMOUSE);
			else
				pCtrl->removeState(I3UI_STATE_ONMOUSE);

			pCtrl->setModifyState(true);
		}
	}
}

void UIFloatUserList::_UpdateFriendAlarm(  REAL32 rDeltaSeconds)
{
	if(m_bFriendAlarm == false)
		return;

	m_rAlarmTerm += rDeltaSeconds;

	if(m_rAlarmTerm > 0.25f)
	{
		m_rAlarmTerm = 0.0f;
		m_bAlarmFlag = !m_bAlarmFlag;
	}

	LuaState * L = _CallLuaFunction("GetFriendTab");
	if( L != nullptr)
	{
		i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L , 0));

		if(pCtrl != nullptr)
		{
			if(m_bAlarmFlag)
				pCtrl->addState( I3UI_STATE_ONMOUSE);
			else
				pCtrl->removeState(I3UI_STATE_ONMOUSE);

			pCtrl->setModifyState(true);
		}
	}	
}

INT32 UIFloatUserList::_FindMyDataIndex( void)
{
	i3::rc_wstring my_nick = UserInfoContext::i()->GetMyNickName();

	for(size_t i = 0; i < m_UserInfoList.size(); i++)
	{
		USERINFO * pUser = getUserInfo( i);

		if( i3::generic_is_iequal( pUser->_wstrNick, my_nick) )
			return i;
	}

	return -1;
}

bool UIFloatUserList::_IsMyClanMember( INT64 uid)
{
	for(UINT32 i=0; i< CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)		return true;
	}

	return false;
}

void UIFloatUserList::_UpdateMyHighlight( void)
{
	//I3ASSERT( m_pUserListBox != nullptr);

	INT32 myIndex = _FindMyDataIndex();

	if( myIndex != m_SelectedIndex)
	{
		if( m_SelectedIndex != -1)
		{
			//m_pUserListBox->SelectData( m_SelectedIndex, false);
			m_SelectedIndex = -1;
		}

		if( myIndex != -1)
		{
			//m_pUserListBox->SelectData( myIndex, true);
			m_SelectedIndex = myIndex;
		}
	}
}

void UIFloatUserList::_RemoveOnMouseState( void)
{
	LuaState * L = _CallLuaFunction("GetUserListBox");

	if( L != nullptr)
	{
		i3UIListView_Box * pLVBox = reinterpret_cast<i3UIListView_Box *>(_ReturnLuaPointer(L , 0));

		I3ASSERT(pLVBox);

		i3UIListView_Item * pItem = nullptr;
		for(size_t i = 0 ; i < pLVBox->getItemList().size() ; ++i)
		{
			pItem = pLVBox->getItemList()[i];
			pItem->removeState(I3UI_STATE_ONMOUSE);
		}
	}
}

void UIFloatUserList::_ClearSelectTab(void)
{
	// 탭 활성화를 초기화시킵니다.
	LuaState * L = _CallLuaFunction("ClearSelectTab");
	if(L != nullptr)
		_EndLuaFunction(L , 0);
}

void UIFloatUserList::_SendUserGuideCtrl(INT32 idx)
{
	LuaState * L = _CallLuaFunction("GetUserListBox");
	if( L != nullptr)
	{
		i3UIListView_Box * pLVBox = reinterpret_cast<i3UIListView_Box *>(_ReturnLuaPointer(L , 0));
		I3ASSERT(pLVBox);
		i3UIListView_Item * pItem = pLVBox->getItemList()[idx];
		UserGuideMgr::i()->OnClick(pItem, false);
	}
}

void UIFloatUserList::_ListView_ScrollTop()
{
	LuaState * L = _CallLuaFunction("ScrollTop");
	if( L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
}

void UIFloatUserList::SetUserInfo( INT32 slot, LOBBY_USER_INFO_LIST * pInfo)
{
	//	if( m_CurrentUserTab == USERLIST_TAB_CURRENT)
	getUserInfo( slot)->CopyToLobbyUser( pInfo);
}


void UIFloatUserList::CloseCurrentFrame(i3UIControl* pCtrl, bool bUserList)
{
	I3ASSERT_RETURN(pCtrl);

	i3UIControl * pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());

	if(pCurCtrl == pCtrl->getParent())
		return;

	if(pCurCtrl == pCtrl)
		return;

	// 현재 Focus된 컨트롤이 UserListTab 일 경우에도 return
	LuaState * L = _CallLuaFunction("GetTabFrame");

	if(L != nullptr)
	{
		i3UIFrameWnd * pCheckFrame = reinterpret_cast<i3UIFrameWnd * >(_ReturnLuaPointer(L , 0));

		if(pCurCtrl == pCheckFrame)
			return;
	}

	if(bUserList == true)
		_SetUserListFrame(false);
	else
		_SetSearchFrame(false);

	_ClearSelectTab();
}

void UIFloatUserList::SearchUser( void)
{

		
	i3::wliteral_range	wTempRange = m_pSearchEdit->getTextRange();

	if(UserInfoContext::i()->IsMyNickName( wTempRange ) == false)
	{
		if(i3::generic_string_size(wTempRange) > 0)
		{
			GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_NICK, wTempRange.c_str() );

			_SetSearchFrame(false);
			_ClearSelectTab();
		}
	}	
}

void UIFloatUserList::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatUserList::ReceiveGameEvent_Insert_Friend( INT32 arg, const i3::user_data&)
{
	if (EV_SUCCESSED(arg))
	{
		INT32 realIdx = CCommunityContext::i()->GetViewIndexOfRealFriend(arg);
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(realIdx);			

		if (node)
		{
			if (FRIEND_STATE_HOLD == GET_FRIEND_STATE(node->_state))
			{
				_StartFriendAlarm();
			}
			else
			{
				// 자신이 신청한 경우
				i3::rc_wstring wstr_message;
				i3::sprintf(wstr_message, GAME_RCSTRING("STR_TBL_GUI_BASE_NOTICE_ADD_FRIEND_COMPLETE"), node->_wstr_nick);/*[%s]님이 친구목록에 추가되었습니다.*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_message);
			}
		}
		else
		{
			I3TRACE("[ERROR] EVENT_INSERT_FRIEND - Invalid friend idx(%d)\n", CCommunityContext::i()->GetRealIndexOfViewFriend(arg));
		}
	}
	else
	{
		i3::rc_wstring wstr_message;

		switch(TESTBIT(arg, EVENT_ERROR_MASK_FAIL))
		{
		case EVENT_ERROR_EVENT_FRIEND_INSERT_PARAMETER_USER:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//유저를 찾을 수 없습니다.
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_FULL:
			if (FRIEND_MAX_COUNT <= CCommunityContext::i()->GetFriendCount())
			{
				// 자신이 친구를 추가할 수 없다.
				wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_MORE_GET_FRIEND_STATE");//더이상 친구를 추가할 수 없습니다.
			}
			else
			{
				// 상대가 친구를 추가할 수 없다.
				wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_FAIL_REQUEST_FRIEND_BY_LIMIT");//상대방이 더 이상 친구를 추가할 수 없는 상태입니다.
			}
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_QUERY:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//유저를 찾을 수 없습니다.
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_ALREADY_FRIEND:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_ALREADY_REGIST_FRIEND_LIST");//이미 친구목록에 등록되어 있습니다.
			break;				
		case EVENT_ERROR_EVENT_FRIEND_INSERT_NOT_FIND_NICK:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//유저를 찾을 수 없습니다.
			break;
		case EVENT_ERROR_EVENT_FRIEND_INSERT_BLOCK_USER:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_ADD_FRIEND_FAIL_BLOCK");//친구 추가 할 수 없는 유저입니다.
			break;
		default:
			wstr_message = GAME_RCSTRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST");//유저를 찾을 수 없습니다.
			break;
		}

		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_message);
	}
}

void UIFloatUserList::ReceiveGameEvent_Clan_MemberList( INT32 arg, const i3::user_data&)
{
	if(SUCCEEDED(arg))
	{
		_UpdateClanMemberList();
	}
	else
	{
		//스트링 팝업 추가하자.
	}
}

void UIFloatUserList::ReceiveGameEvent_Update_UserList( INT32, const i3::user_data&)
{
	_UpdateUserList();
}

void UIFloatUserList::ReceiveGameEvent_GetLobbyUser( INT32, const i3::user_data&)
{
	_UpdateUserList();
	g_pFramework->GetUIMainframe()->SetLockInput(false);
}

void UIFloatUserList::ReceiveGameEvent_Get_UserInfo_Session( INT32 arg, const i3::user_data&)
{
	// 유저 정보가 있을때만 팝업을 엽니다.
	if(EV_SUCCESSED(arg))
	{
		switch( m_CurrentUserTab )
		{
		case USERLIST_TAB_CURRENT:
		case USERLIST_TAB_FRIEND:
		case USERLIST_TAB_CLAN:
			{
				if( 0 < m_UserInfoList.size() )
				{
					USERINFO * pInfo  = getUserInfo(m_RSelectedIndex);
					GetMainFrame()->OpenPopup(UPW_USERPROFILE, &pInfo->_wstrBlockReason);
				}
				else
				{
					GetMainFrame()->OpenPopup(UPW_USERPROFILE, nullptr );
				}
			}break;
		case USERLIST_TAB_SEARCH:
			{
				GetMainFrame()->OpenPopup(UPW_USERPROFILE, nullptr );
			}break;
		default :
			{
				// Room ready 
				if(GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
				{
					//UIPhaseReadyRoom * pPhase = i3::same_cast<UIPhaseReadyRoom*>(GetMainFrame()->GetCurrentPhase());
					UIPhaseReadyRoom * pPhase = i3::kind_cast<UIPhaseReadyRoom*>(GetMainFrame()->GetCurrentPhase());
					
					I3ASSERT(pPhase != nullptr);

					INT32 idx = pPhase->getSelectedSlot();
					i3::rc_wstring wstrUser = BattleSlotContext::i()->getNickForSlot(idx);

					const BLOCK_USER * pBlock = CCommunityContext::i()->GetBlockUser( wstrUser );

					if( nullptr != pBlock )
					{
						GetMainFrame()->OpenPopup(UPW_USERPROFILE, const_cast<i3::rc_wstring*>(&pBlock->_wstr_comment));
					}
					else
					{
						GetMainFrame()->OpenPopup(UPW_USERPROFILE, nullptr);
					}
				}
			}
		}
	}
	else
	{
		if(m_CurrentUserTab == USERLIST_TAB_SEARCH)
		{
			GameUI::MsgBox(MSG_TYPE_ERROR , GAME_RCSTRING("STR_TBL_USERSEARCH_ERROR_POPUP") );
		}
	}

	m_bReqUserInfo = false;
}
