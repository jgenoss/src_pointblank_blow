#include "pch.h"
#include "UIFloatUserList_V10.h"
#include "UIFloatChat_V10.h"
#include "UIPopupUserAction_V10.h"

#include "../UI/UIMainFrame_V10.h"
#include "../UI/UIPhaseReadyRoom.h"
#include "../UI/UIUtil.h"

#include "../CommunityContext.h"
#include "../GameGUI.h"

#include "../ChannelContext.h"
#include "../UserInfoContext.h"
#include "../LobbyContext.h"

#include "../UI/UIPhaseClanLobby.h"
#include "../UI_V10/UIPhaseLobby_V10.h"

static UIFloatUserList_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		return (UIFloatUserList_V10*) pFrame->GetFloating( UFW_USERLIST);
	}

	return nullptr;
}

static void s_ClanMarkProcessor( i3UIListView_Cell * pCell, void * pData)
{
	GameUI::SetClanTextureForListView( pCell, pData);
}


extern "C" {
	
	int UIFloatUserList_V10_OpenUserPopup( LuaState * L)
	{
		//INT32 tab = i3Lua::GetIntegerArg( L, 1);
		INT32 slot = i3Lua::GetIntegerArg( L, 2);
		GetThis()->OpenUserPopup( slot);
		return 0;
	}

	int UIFloatUserList_V10_OpenUserProfile(LuaState * L)
	{
		//INT32 tab = i3Lua::GetIntegerArg( L, 1);
		INT32 slot = i3Lua::GetIntegerArg(L, 2);
		GetThis()->OpenUserProfile(slot);
		return 0;
	}
};

LuaFuncReg UIFloatUserList_V10_Glue[] = {
	{"OpenUserPopup",			UIFloatUserList_V10_OpenUserPopup },
	{"OpenUserProfile",			UIFloatUserList_V10_OpenUserProfile }, 
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatUserList_V10);

UIFloatUserList_V10::UIFloatUserList_V10()
{
	m_bReqUserInfo = false;

	m_AlignedPos = UIFLOATING_POS_LEFTTOP;

#if defined( I3_DEBUG)
	SetName( "UserList");
#endif

	i3mem::FillZero( m_pClanImg, sizeof( m_pClanImg));

	m_pUserListBox = nullptr;

	m_RSelectedIndex = -1;
	m_SelectedIndex = -1;

	m_pMainFmw = nullptr;
}

UIFloatUserList_V10::~UIFloatUserList_V10()
{
	_ClearUserList();
}

UIFloatChat_V10 * UIFloatUserList_V10::GetChat( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	if( pFrame != nullptr)
	{
		return (UIFloatChat_V10*) pFrame->GetFloating( UFW_CHAT);
	}

	return nullptr;
}

void UIFloatUserList_V10::_ClearUserList( void)
{
	//i3::cu::for_each_SafeFree_clear(m_UserInfoList);
	i3::cu::for_each_delete_clear(m_UserInfoList);
}

void UIFloatUserList_V10::_SetUserInfo( INT32 idx, const USERINFO * pInfo)
{
	LuaState * L;
	i3::rc_wstring wszRankToolTip;

	if( pInfo != nullptr && pInfo->_Nick[0] != 0)
	{
		//	계급
		INT32 rank = (INT32) pInfo->_Rank;

		if( ChannelContext::i()->IsBeginnerChannel1())	//	초보 서버
			rank = RANK_NEWBIE;

		if( rank >= RANK_MAX)
			rank = RANK_MAX-1;

		wszRankToolTip = GetCharaRank( rank);

		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, idx);
			GameUI::SetClanMarkToLua( L, pInfo->_ClanMark);
			i3Lua::PushStringUTF16To8( L, pInfo->_Clan);
			i3Lua::PushInteger( L, rank);
			i3Lua::PushStringUTF16To8( L, wszRankToolTip);
			i3Lua::PushStringUTF16To8( L, pInfo->_Nick);
			i3Lua::PushStringUTF16To8( L, pInfo->_BlockReason);
			//i3Lua::PushStringUTF16To8( L, pInfo->_szPosition);
			i3Lua::PushColor( L, (I3COLOR*) &pInfo->_NickColor);
			//i3Lua::PushColor( L, (I3COLOR*) &pInfo->_PosColor);
			_EndLuaFunction( L, 15);
		}
	}
	else
	{
		L = _CallLuaFunction( "SetUserInfo");
		if( L != nullptr)
		{
			i3Lua::PushInteger( L, idx);
			GameUI::SetClanMarkToLua( L, 0xFFFFFFFF);
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushInteger( L, -1);
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushA2UTF8String( L, "");
			//i3Lua::PushA2UTF8String( L, "");
			i3Lua::PushColor( L, GameGUI::GetColor( GCT_DEFAULT));
			//i3Lua::PushColor( L, GameGUI::GetColor( GCT_DEFAULT));
			_EndLuaFunction( L, 15);
		}
	}
}

/*virtual*/ void UIFloatUserList_V10::OnUpdate( REAL32 rDeltaSeconds)
{
	UIFloatingBase::OnUpdate( rDeltaSeconds);

	if( g_pFramework->GetClanContext() != nullptr)
			g_pFramework->GetClanContext()->UpdateClanMember( rDeltaSeconds);

	// 초대 팝업 위치
	//UIPHASE ePhase = GetMainFrame()->GetCurrentPhaseType();

	//if( (ePhase == UIPHASE_LOBBY || ePhase == UIPHASE_READY_ROOM)  && 
	//	GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE && g_pCommunity->GetInviteCount() > 0)
	//{
	//	GetMainFrame()->GetCurrentPhase()->InvitedMessage();
	//}
}

/*virtual*/ void UIFloatUserList_V10::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	switch( evt)
	{
	case EVENT_DELETE_BLOCK :
	case EVENT_UPDATE_USER_LIST :
		_UpdateUserList();
		break;
	case EVENT_INSERT_FRIEND:
		_AddFriend(arg);
		_UpdateUserList();
		break;
	}
}

bool UIFloatUserList_V10::OnRevive(i3RenderContext * pCtx)
{
	_SetPhaseAtPos();

	return UIFloatingBase::OnRevive(pCtx);
}

void UIFloatUserList_V10::OpenUserPopup( INT32 slot)
{
	UINT32 i;

	if( _SetSelectedUser( slot))
	{
		USERINFO * pInfo = getUserInfo( slot);

		GetMainFrame()->ResetUserActionPopup();

		if( pInfo->_Session > -1 &&  GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
			GetMainFrame()->AddUserActionCommand( UCM_PROFILE);		//	상세보기

		//	자신이 아닐 경우만
		if( ! UserInfoContext::i()->IsMyNickName( m_SelectedUserInfo._Nick))
		{
			//@eunil. Russia 요청사항 채팅창이 떠있어야 합니다. 1.0은왜 레디룸 챗 따로 있어가지고...
			if( (GetMainFrame()->IsEnableFloatWin(UFW_CHAT) || GetMainFrame()->IsEnableFloatWin(UFW_READYROOM_CHAT) )
				&& pInfo->_friendState != FRIEND_STATE_OFFLINE)
			{	// 온라인 상태라면
				GetMainFrame()->AddUserActionCommand( UCM_WHISPER);			//	귓속말
			}

			bool bFriend = false;
			bool bClan = false;
			for( i = 0; i < CCommunityContext::i()->GetFriendCount(); i++)
			{
				const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend( i);
				if(nullptr == node)	continue;
				if( i3::generic_compare( node->_wstr_nick, m_SelectedUserInfo._Nick) == 0)
				{
					// 이미 친구이다.
					bFriend = true;
					bClan = _IsMyClanMember(node->_uid);
				}
			}

			if( bFriend == false)
				GetMainFrame()->AddUserActionCommand( UCM_ADD_FRIEND);		//	친구추가

			if( LocaleValue::Enable("EnableBlock") ) // 차단하기 버튼 활성화 여부
			{
				GetMainFrame()->AddUserActionCommand( UCM_ADD_BLOCK);		//	차단하기
			}

			//{	// 현재 접속자만 초대가 가능하도록 되어 있다..
			if( CanClanInvite() )
			{
				if( g_pFramework->GetClanContext()->IsPossibleClanInvite() && 
					((m_SelectedUserInfo._ClanMark == CLAN_MARK_FLAG_NONE) || bClan == false))
				{	
					GetMainFrame()->AddUserActionCommand( UCM_CLAN_INVITE);		// 클랜초대(CurrentTab)
				}
			}

			// GM 옵션창
			if ( UserInfoContext::i()->GetMyRank() == RANK_GAMEMASTER)
			{
				GetMainFrame()->AddUserActionCommand( UCM_GMOPTION);		// GM Option 창 GM일 경우만
			}
		}

		// 친구 검색으로 Profile을 열 경우에만 해당됩니다.
		CCommunityContext::i()->SetFindUser(false);

		GetMainFrame()->OpenUserActionPopup( this, slot);

		PlaySound( UISND_ITEM_SELECTING);
	}
}

void UIFloatUserList_V10::OpenUserProfile(INT32 slot)
{
	if (_SetSelectedUser(slot))
	{
		USERINFO * pInfo = getUserInfo(slot);

		CCommunityContext::i()->SetCommnunityInfoNick(pInfo->_Nick);
		CCommunityContext::i()->SetCommunityUserInfo(false);

		if (pInfo->_Session < 0)
			return;

		INT32  i32UserSessionIdx;
		i32UserSessionIdx = pInfo->_Session;

		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_SESSION, &i32UserSessionIdx);
		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_SUBTASK, &i32UserSessionIdx);

		INT32 idx = 1;
		GetMainFrame()->OpenPopup(UPW_USERPROFILE, &idx);
	}
}

INT32 UIFloatUserList_V10::GetSlotIdxbyName(i3::rc_wstring _name)
{
	for (INT32 i = 0; i < (INT32)m_UserInfoList.size(); ++i)
	{
		i3::rc_wstring wstrName; 
		i3::sprintf(wstrName, m_UserInfoList[i]->_Nick);
		if (i3::generic_is_iequal(wstrName, _name))
		{
			return i;
		}
	}
	return -1;
}


bool UIFloatUserList_V10::_SetSelectedUser( INT32 slot)
{
	if( slot > -1 && slot < (INT32)m_UserInfoList.size())
	{
		m_RSelectedIndex = slot;

		USERINFO* pUser = m_UserInfoList[ m_RSelectedIndex];
		i3mem::Copy( &m_SelectedUserInfo, pUser, sizeof( m_SelectedUserInfo));

		return true;
	}
	else
	{
		m_RSelectedIndex = -1;
		m_SelectedUserInfo.Reset();

		return false;
	}
}

void UIFloatUserList_V10::_UpdateCurrentList( void)
{
	INT32 i, j;

	for( i = 0; i < LobbyContext::i()->GetLobbyUserCount(); i++)
	{
		const LOBBY_USER_INFO_LIST * pUser1 = LobbyContext::i()->getLobbyUserInfo( i);

		I3ASSERT( pUser1 != nullptr);
		
		if( pUser1->_Session < 0 )continue;
		if( pUser1->_Nick[0] == 0 )continue;
		if( CCommunityContext::i()->CheckBlockUser(pUser1->_Nick)) continue;	//차단 유저인 경우 리스트에서 제외

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
			pInfo->CopyFromLobbyUser( pUser1);
			m_UserInfoList.push_back( pInfo );
		}
	}
}

void UIFloatUserList_V10::_UpdateUserList( void)
{
	INT32 i;
	LuaState * L;

	_ClearUserList();

	// 1.0에는 탭이 없기 때문에 Current만 업데이트 합니다.
	_UpdateCurrentList();
	
	INT32 count = (INT32)m_UserInfoList.size();

	// Set Slot
	L = _CallLuaFunction( "ClearUserList");
	if( L != nullptr)
	{
		if( count < 8)
			count = 8;

		i3Lua::PushInteger( L, count);
		i3Lua::PushUserData( L, (I3UI_LISTVIEW_CELL_CALLBACK) s_ClanMarkProcessor);
		_EndLuaFunction( L, 2);
	}

	for( i = 0; i < count; i++)
	{
		if( i < (INT32)m_UserInfoList.size())
		{
			USERINFO * pUser = getUserInfo( i);

			_SetUserInfo(i, pUser);
		}
		else
		{
			_SetUserInfo( i, nullptr);
		}
	}

	// highlight my slot
	_UpdateMyHighlight();
}


INT32 UIFloatUserList_V10::_FindMyDataIndex( void)
{
	for(size_t i = 0; i < m_UserInfoList.size(); i++)
	{
		USERINFO * pUser = getUserInfo( i);

		if( i3::generic_compare( pUser->_Nick, UserInfoContext::i()->GetMyNickName()) == 0)
			return i;
	}

	return -1;
}

bool UIFloatUserList_V10::_IsMyClanMember( INT64 uid)
{
	for(UINT32 i=0; i<CCommunityContext::i()->GetClanMemberCount(); i++)
	{
		const CLAN_MEMBER_NODE* pNode = CCommunityContext::i()->GetViewClanMember(i);

		if( pNode->_uid == uid)		return true;
	}

	return false;
}

void UIFloatUserList_V10::_UpdateMyHighlight( void)
{
	I3ASSERT( m_pUserListBox != nullptr);

	INT32 myIndex = _FindMyDataIndex();

	if( myIndex != m_SelectedIndex)
	{
		if( m_SelectedIndex != -1)
		{
			m_pUserListBox->SelectData( m_SelectedIndex, false);
			m_SelectedIndex = -1;
		}

		if( myIndex != -1)
		{
			m_pUserListBox->SelectData( myIndex, true);
			m_SelectedIndex = myIndex;
		}
	}
	
}

void UIFloatUserList_V10::SetUserInfo( INT32 slot, LOBBY_USER_INFO_LIST * pInfo)
{
//	if( m_CurrentUserTab == USERLIST_TAB_CURRENT)
		getUserInfo( slot)->CopyToLobbyUser( pInfo);
}

void UIFloatUserList_V10::_AddFriend( INT32 arg )
{
	if (EV_SUCCESSED(arg))
	{
		INT32 realIdx = CCommunityContext::i()->GetViewIndexOfRealFriend(arg);
		const FRIEND_NODE* node = CCommunityContext::i()->GetViewFriend(realIdx);			

		if (node)
		{
			if (FRIEND_STATE_HOLD != GET_FRIEND_STATE(node->_state))
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

void UIFloatUserList_V10::OnLoadAllScenes()
{
	m_slide.AddFrameWnd(GetFirstFrameWnd(), UISlide::R_TO_L);
}

void UIFloatUserList_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIFloatingBase::_InitializeAtLoad(pScene);

	I3ASSERT( pScene != nullptr);

	m_pUserListBox = (i3UIListView_Box*)pScene->FindChildByName( "ListView_UserList");
	I3ASSERT( m_pUserListBox);
	
	m_pMainFmw = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_UserList");
	I3ASSERT(m_pMainFmw);

	m_slide.AddFrameWnd(m_pMainFmw, UISlide::R_TO_L);

	m_bFixedWindow = true;
}

void UIFloatUserList_V10::_ClearAtUnload( i3UIScene * pScene )
{
	UIFloatingBase::_ClearAtUnload(pScene);

	m_pUserListBox = nullptr;
}

void UIFloatUserList_V10::OnCreate( i3GameNode * pParent )
{
	UIFloatingBase::OnCreate(pParent);
	
	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_UserList.i3UIs", UIFloatUserList_V10_Glue);
}

bool UIFloatUserList_V10::OnEntranceStart( void * pArg1 /* = NULL */, void * pArg2 /* = NULL */ )
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//m_slide.ForceEnd();	//slide 행동을 강제한다.

	// 위치를 새로 잡고 고정한다.
	_UpdateUserList();
	_SetPhaseAtPos();

	//강제로 위치를 맞춰주기 때문에 슬라이드 최종 위치도 수정해준다.
	m_slide.SetOriginalPos( m_pMainFmw->getPos()->x - g_pViewer->GetViewWidth(), m_pMainFmw->getPos()->y);

	return true;
}

bool UIFloatUserList_V10::OnExitStart( void )
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	//m_slide.ForceEnd();	//slide 행동을 강제한다.

	return true;
}

void UIFloatUserList_V10::_SetPhaseAtPos()
{
	if (!isEnable())
		return;

	VEC3D* pos  = m_pMainFmw->getPos();
	VEC2D* size = m_pMainFmw->getSize();

	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		const VEC3D* pBenchmark_pos = nullptr;
		UIPhaseClanLobby* p_clanlobby = ((UIPhaseClanLobby*)GetMainFrame()->GetPhase(UIPHASE_CLANLOBBY));
		UIPhaseLobby_V10* p_lobby = ((UIPhaseLobby_V10*)GetMainFrame()->GetPhase(UIPHASE_LOBBY));

		if (p_clanlobby && p_clanlobby->isEnable())
			pBenchmark_pos = p_clanlobby->GetMainFrameWndPos();
		else
		if (p_lobby && p_lobby->isEnable())
			pBenchmark_pos = p_lobby->GetMainFrameWndPos();

		if (pBenchmark_pos != nullptr)
			pos->x = pBenchmark_pos->x - size->x;

		m_pMainFmw->setPos(pos);
	}
}
