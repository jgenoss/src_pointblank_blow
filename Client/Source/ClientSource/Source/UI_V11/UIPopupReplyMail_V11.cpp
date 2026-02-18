#include "pch.h"
#include "UI/UIMainFrame.h"
#include "UIPopupReplyMail_V11.h"
#include "UserInfoContext.h"

#include "../UI/UIUtil.h"
#include "../UI/UIDefine.h"

//------------------------------------------------------------------------------//
//							Lua Glue Functions									//
//------------------------------------------------------------------------------//
static UIPopupReplyMail_V11* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupReplyMail_V11*) pFrame->GetPopup( UPW_MAILREP);
}


extern "C" {
	int UIPopupReplyMail_V11_Send(LuaState * L)
	{
		GetThis()->OnSendNote();
		return 0;
	}

	int UIPopupReplyMail_V11_Cancel(LuaState * L)
	{
		GetThis()->GetMainFrame()->TogglePopup(UPW_MAILREP);
		return 0;
	}
}

LuaFuncReg  UIPopupReplyMail_V11_Glue[] = {
	{"Send_Mail",	UIPopupReplyMail_V11_Send	},
	{"Cancel",		UIPopupReplyMail_V11_Cancel	},
	{nullptr,			nullptr					}
};

I3_CLASS_INSTANCE( UIPopupReplyMail_V11);

UIPopupReplyMail_V11::UIPopupReplyMail_V11()
{
}

UIPopupReplyMail_V11::~UIPopupReplyMail_V11()
{
}

void UIPopupReplyMail_V11::OnSendNote()
{
	LuaState* L;
	L = UILua::CallLuaFunction(GetScene(0), "GetReceiver");
	i3::rc_wstring Who;
	i3::utf8_to_utf16( UILua::ReturnLuaString(L, 0), Who );

	UILua::CallLuaFunction(GetScene(0), "GetMessage");
	i3::rc_wstring Text;
	i3::utf8_to_utf16( UILua::ReturnLuaString(L, 0), Text );
	
	
	if(i3::generic_string_size(Text) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_INPUT_NOTE"));/*쪽지 내용을 입력해 주세요.*/
	}
	else
	{
		if ( i3::generic_is_equal( UserInfoContext::i()->GetMyNickName(), Who) )
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_SEND_SELF"));/*자신에게 쪽지를 보낼 수 없습니다.*/
		}
		else
		{
			if( g_pFramework->IsFilterOK(Text) == false)
			{
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
			}
			else
			{

#ifdef USE_MESS_TEST
				NOTE_BODY note;
				i3String::Copy( note._sender, Who, NET_NICK_NAME_SIZE);
				i3String::Copy( note._message, szText, NET_NOTE_SIZE);
				note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
				note._messageSize = (UINT8)(i3String::Length( note._message) + 1);

				GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);

#else
				NOTE_BODY note;

				i3::generic_string_copy(note._sender, Who); 
				i3::generic_string_copy(note._message,  Text); 
				note._senderSize = (UINT8)(i3::generic_string_size(note._sender) + 1);
				note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);

				GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);
#endif
				GetMainFrame()->ClosePopup(UPW_MAILREP);
			}
		}
	}
}

void UIPopupReplyMail_V11::_SetUserState( i3::rc_wstring Nick)
{
	LuaState * L = _CallLuaFunction("SetData");
	i3Lua::PushStringUTF16To8(L, Nick);
	i3Lua::PushStringUTF16To8(L, L"");
	_EndLuaFunction(L, 2);
}

/*virtual*/ void UIPopupReplyMail_V11::ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData)
{
	base::ProcessGameEvent( evt, arg, UserData);
}

/*virtual*/ void UIPopupReplyMail_V11::OnLoadAllScenes()
{
	base::OnLoadAllScenes();
	
}

/*virtual*/ void UIPopupReplyMail_V11::OnCreate(i3GameNode * pParent)
{
	base::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_SendMessage.i3UIs", UIPopupReplyMail_V11_Glue);
}

/*virtual*/ bool UIPopupReplyMail_V11::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( base::OnEntranceStart(pArg1,pArg2) == false)
		return false;

	m_UserName = *static_cast<i3::rc_wstring*>(pArg1);
	_SetUserState(m_UserName);

	return true;
}
/*virtual*/ bool UIPopupReplyMail_V11::OnCloseButton()
{
	base::OnCloseButton();
	return true;
}