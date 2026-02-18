#include "pch.h"
#include "UIPopupSendMail_V11.h"

#include "../UserInfoContext.h"

#include "../UI/UIMainFrame_V11.h"
#include "../UI/UIUtil.h"

static UIPopupSendMail_V11 * GetThis( LuaState * L)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if( pFrame != nullptr)
	{
		UIPOPUPWIN id = pFrame->FindPopup( L);
		I3ASSERT( id != UPW_NULL);

		return (UIPopupSendMail_V11*) pFrame->GetPopup( id);
	}

	return nullptr;
}

//--------------------------------------------------------------------------//
//								Lua Glue function							//
//--------------------------------------------------------------------------//
extern "C" {
	int UIPopupSendMail_V11_Search( LuaState * L)
	{
		GetThis( L)->CheckUser();
		return 0;
	}
}

LuaFuncReg UIPopupSendMail_V11_Glue[] = {
	{ "Search",					UIPopupSendMail_V11_Search},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIPopupSendMail_V11);

UIPopupSendMail_V11::UIPopupSendMail_V11()
{
}

UIPopupSendMail_V11::~UIPopupSendMail_V11()
{
}

/*virtual*/ void UIPopupSendMail_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupInteractBase::_InitializeAtLoad( pScene);

	m_pWho = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox0");
	I3ASSERT( m_pWho);

	m_pNote = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox3");
	//아이디 검색 기능미구현으로주석처리합니다. -상옥
	m_pSearch = (i3UIButtonComposed3 *) pScene->FindChildByName("i3UIButtonComposed31");
	m_pSearch->EnableCtrl(false);

	I3ASSERT( m_pNote);
}

#ifdef USE_MESS_TEST
INT64 UIPopupSendMail_V11::_FindNoteByUid( const char* nick)
{
	INT32 length;

	// 자신은 막는다.
	if (g_pGameContext->IsMyNickName( nick))
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*자신의 닉네임을 입력하셨습니다.*/
		return 0;
	}

	NOTE_RECEIVE* pNote = nullptr;
	pNote = (NOTE_RECEIVE*)MailBoxContext::i()->GetReceiveNote( m_iListIdx);

	if( pNote == nullptr )
	{
		I3NOTICE( "pNote == nullptr \n");
		return 0;
	}

	if ( 0 != i3String::Compare(pNote->body._sender, nick))
	{
		NOTE_BODY note;
		length = i3String::LengthW( m_pWho->getText());
		i3String::CopyWCHARtoASCII( note._sender, sizeof(note._sender), m_pWho->getText(), length);

		length = i3String::LengthW( m_pNote->getText());
		i3String::CopyWCHARtoASCII( note._message, sizeof( note._message), m_pNote->getText(), length);
		note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
		note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
		g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
		return 0;
	}

	return pNote->header._suid;
}
#endif

/*virtual*/ void UIPopupSendMail_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PointBlankRe_PopUp_MessageSend.i3UIs", UIPopupSendMail_V11_Glue);
}

/*virtual*/ bool UIPopupSendMail_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction( "SetUser");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, m_wstrNick);
		_EndLuaFunction( L, 1);
	}

	if( pArg2 != nullptr)
	{
		m_iListIdx = *static_cast<INT32*>(pArg2);
	}

	return true;
}

/*virtual*/ bool UIPopupSendMail_V11::OnOKButton( void)
{
	i3::stack_wstring Sender;
	i3::stack_wstring Message;

	if(i3::generic_string_size( m_pWho->getText()) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_INPUT_RECEIVER"));/*받는 사람을 입력해 주세요.*/
		return false;
	}
	else if(i3::generic_string_size( m_pNote->getText()) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_INPUT_NOTE"));/*쪽지 내용을 입력해 주세요.*/
		return false;
	}
	else
	{
		Sender = m_pWho->getText();
		Message = m_pNote->getText();

		if ( i3::generic_compare( UserInfoContext::i()->GetMyNickName(), Sender) == 0)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_SEND_SELF"));/*자신에게 쪽지를 보낼 수 없습니다.*/
			m_pWho->SetText( L"");
			g_pFramework->SetFocus( m_pWho);
			return false;
		}
		else
		{
			if( g_pFramework->ChangeFilterStringOK( Message) == false)
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
				g_pFramework->SetFocus( m_pNote);
				return false;
			}
			else
			{
				// 쪽지 보내기

#ifdef USE_MESS_TEST
				if( m_bReceiverUIDFlag == false)
				{
					NOTE_BODY note;

					i3String::Copy( note._sender, Sender, NET_NICK_NAME_SIZE );

					i3String::Copy( note._message, Message, NET_NOTE_SIZE );

					note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
					note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

					g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
				}
				else
				{
					NOTE_BODY_BY_UID note;

					m_bReceiverUIDFlag = false;

					// 쪽지 목록으로 확인
					note._receiverUid = _FindNoteByUid(Sender);
					if( 0 >= note._receiverUid)
					{
						return true;
					}

					i3String::Copy( note._message, Message, NET_NOTE_SIZE );

					note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

					g_pGameContext->SetEvent( EVENT_SEND_NOTE_BY_UID, &note);
				}
#else
				NOTE_BODY note;
				i3::generic_string_copy( note._sender, Sender);
				i3::generic_string_copy( note._message, Message);
				note._senderSize = (UINT8)(i3::generic_string_size( note._sender) + 1);
				note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);

				GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);
#endif
			}
		}
	}

	return true;
}

void UIPopupSendMail_V11::CheckUser( void)
{
	if( GameUI::FindUserUidByNick( m_pWho->getText()) <= 0)
	{
		/* 유저를 찾을 수 없습니다 */
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"));
	}
}

/*virtual*/bool UIPopupSendMail_V11::OnKey_Enter(void)
{
	if (g_pFramework->getFocus() == m_pWho)
	{
		if (m_pNote->getTextRange().empty())			// 비어있으면 내용쪽으로 포커싱..
		{
			g_pFramework->SetFocus(m_pNote);
		}
		else											// 내용이 있으면 닫는쪽으로 돌린다...
		{
			if(OnOKButton())
				GetMainFrame()->ClosePopup( UPW_SENDMAIL);
		}
	}
	else
	if (g_pFramework->getFocus() == m_pNote)			// 쪽지 작성중엔 엔터키는 그냥 라인피드로만 처리..
	{
		// 닫는쪽 엔터키가 적용되서는 안된다..일단 이대로 둡니다..
	}
	else
	if(OnOKButton())
		GetMainFrame()->ClosePopup( UPW_SENDMAIL);

	return true;
}

/*virtual*/ bool UIPopupSendMail_V11::OnKeyInput( i3InputDeviceManager * pInputMng )
{
	i3InputKeyboard*	pKeyboard	= pInputMng->GetKeyboard();
	I3ASSERT( pKeyboard != nullptr);

	// 쪽지보내기 - 보낼사람에 포커스일 때 tab키 누르면 메세지작성으로 포커스
	if( pKeyboard->GetStrokeState(I3I_KEY_TAB) )
	{
		if( m_pWho != nullptr && m_pWho->isFocused()
			&& m_pNote != nullptr )
		{
			g_pFramework->SetFocus( m_pNote );
			return true;
		}
	}

	return UIPopupInteractBase::OnKeyInput( pInputMng );
}