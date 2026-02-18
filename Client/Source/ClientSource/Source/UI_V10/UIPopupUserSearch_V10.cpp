#include "pch.h"
#include "UIPopupUserSearch_V10.h"

#include "UserInfoContext.h"
#include "CommunityContext.h"
#include "../UI/UIMainFrame_V10.h"

I3_CLASS_INSTANCE( UIPopupUserSearch_V10);

static UIPopupUserSearch_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

	return (UIPopupUserSearch_V10*) pFrame->GetPopup( UPW_USERSEARCH);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupUserSearch_V10_ClickOK( LuaState * L)
	{
		GetThis()->ClickOK( );
		return 0;
	}

	int UIPopupUserSearch_V10_ClickCancel( LuaState * L)
	{
		GetThis()->ClickCancel( );
		return 0;
	}
}

LuaFuncReg UIPopupUserSearch_V10_Glue[] =
{
	{"ClickOK",			UIPopupUserSearch_V10_ClickOK},
	{"ClickCancel",		UIPopupUserSearch_V10_ClickCancel},
	{nullptr,				nullptr}
};

UIPopupUserSearch_V10::UIPopupUserSearch_V10() : idxType_(0), textEditBox_(nullptr)
{
	RegisterGameEventFunc( EVENT_GET_USERINFO_SESSION,			&UIPopupUserSearch_V10::ReceiveGameEvent_UserInfo);
	RegisterGameEventFunc( EVENT_INSERT_FRIEND,					&UIPopupUserSearch_V10::ReceiveGameEvent_AddFriend);
}

UIPopupUserSearch_V10::~UIPopupUserSearch_V10()
{

}

void UIPopupUserSearch_V10::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Popup/PBRe_PopUp_UserSearch.i3UIs", UIPopupUserSearch_V10_Glue);
}

bool UIPopupUserSearch_V10::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	if( pArg1 != nullptr)
		idxType_ = *static_cast<INT32*>(pArg1);

	// 텍스트 세팅
	_SetStaticText();

	return true;
}

void UIPopupUserSearch_V10::ClickOK()
{
	switch(idxType_)
	{
	case USERSEARCH:
		{
			_UserSearch();
		} break;
	case FINDUSER:
		{	
			_AddUser();
		} break;
	default:
		break;
	}
}

void UIPopupUserSearch_V10::ClickCancel()
{
	// 팝업 종료
	GetMainFrame()->ClosePopup(UPW_USERSEARCH);
}

void UIPopupUserSearch_V10::_InitializeAtLoad( i3UIScene * pScene )
{
	UIPopupBase::_InitializeAtLoad(pScene);

	textEditBox_ = (i3UIEditBox *) pScene->FindChildByName("InputText");
}

void UIPopupUserSearch_V10::_ClearAtUnload( i3UIScene * pScene )
{
	UIPopupBase::_ClearAtUnload(pScene);

	textEditBox_ = nullptr;
}

void UIPopupUserSearch_V10::_SetStaticText()
{
	LuaState * L;
	switch(idxType_)
	{
	case USERSEARCH:
		{
			L = _CallLuaFunction("SetUserSearch");
			if(L != nullptr)
			{
				_EndLuaFunction(L, 0);
			}
		} break;
	case FINDUSER:
		{
			L = _CallLuaFunction("SetFindFriend");
			if(L != nullptr)
			{
				_EndLuaFunction(L, 0);
			}
		} break;
		
	default:
		break;
	}
}

void UIPopupUserSearch_V10::_UserSearch()
{
	if( textEditBox_ == nullptr) return;

	i3::wstring wstrbuf = textEditBox_->getText();

	if(UserInfoContext::i()->IsMyNickName( wstrbuf))
	{
		// 내 닉네임을 눌렀을 경우
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_INPUT_SELF"));
		return;
	}
	else
	{
		if (_CheckNickName(wstrbuf) == false) return;

		// 닉네임으로 유저를 찾는다.
		GameEventSender::i()->SetEvent(EVENT_GET_USERINFO_NICK, wstrbuf.c_str());

		// 닉네임 등록
		CCommunityContext::i()->SetCommnunityInfoNick(wstrbuf);
	}
}

void UIPopupUserSearch_V10::_AddUser()
{
	if( textEditBox_ == nullptr) return;

	i3::wstring wstrbuf = textEditBox_->getText();

	if (_CheckNickName(wstrbuf) == false) return;

	// 바로 친구를 추가합니다.
	GameUI::AddFriend(wstrbuf);
}

bool UIPopupUserSearch_V10::_CheckNickName(i3::wstring& nickname)
{
	// 닉네임 체크 버전별로 다를 수도 있음.
	// 1.5와 같이 쓰기 때문에 따로 처리합니다.
	if (nickname.empty() == true)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NICK_EMPTY")); /* 닉네임을 입력하셔야 합니다. */
		return false;
	}
	else if (nickname.size() < 2)
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_MESSAGE_LIMITED_NAME_LENGTH")); /* 닉네임은 2자 이상 16자 이하만 가능합니다. */
		return false;
	}

	return true;
}

void UIPopupUserSearch_V10::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData )
{
	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIPopupUserSearch_V10::RegisterGameEventFunc( GAME_EVENT evt, call_func f )
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupUserSearch_V10::ReceiveGameEvent_UserInfo( INT32 arg, const i3::user_data& )
{
	// UserInfo를 받았을 경우
	if(EV_SUCCESSED(arg))
	{
		// 이 팝업을 끕니다.
		ClickCancel();

		// UserInfo를 켭니다.
		// 여기서 친구와 아닌 사람을 분기 해야 합니다.
		INT32 idx = 0;
		if( GameUI::IsCommunityFriend(CCommunityContext::i()->GetCommnunityInfoNick()))
		{
			idx = 2;
		}
		else
		{
			idx = 3;
		}
	
		if( GetMainFrame()->IsOpenedPopup( UPW_USERPROFILE) == false)
			GetMainFrame()->TogglePopup(  UPW_USERPROFILE, &idx);
	}
	else
	{
		switch(idxType_)
		{
		case USERSEARCH:
			{
				// 존재하지 않는 사용자
				GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_NOT_EXIST_USER"));
			} break;
	
		}
		
	}
}

void UIPopupUserSearch_V10::ReceiveGameEvent_AddFriend( INT32 arg, const i3::user_data& )
{
	// 이벤트를 받으면 끕니다.
	ClickCancel();

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

/*virtual*/ bool UIPopupUserSearch_V10::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	if(i3UI::getGlobalIme()->isFocused())
	{
		i3InputKeyboard * pKeyboard = pInputMng->GetKeyboard();

		if(pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			return OnKey_Enter();
	}

	return UIPopupBase::OnKeyInput(pInputMng);
}

bool UIPopupUserSearch_V10::OnKey_Enter( void )
{
	ClickOK();
	return true;
}

void UIPopupUserSearch_V10::OnEntranceEnd( void )
{
	UIPopupBase::OnEntranceEnd();

	if ( textEditBox_ != nullptr )
	{
		g_pFramework->SetFocus(nullptr);
		g_pFramework->SetFocus(textEditBox_);
	}
}
