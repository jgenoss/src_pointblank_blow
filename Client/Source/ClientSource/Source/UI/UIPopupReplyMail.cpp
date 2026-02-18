#include "pch.h"
#include "UIMainFrame.h"
#include "UIPopupReplyMail.h"

#include "UIUtil.h"

////------------------------------------------------------------------------------//
////							Lua Glue Functions									//
////------------------------------------------------------------------------------//
//static UIPopupReplyMail* GetThis( void)
//{
//	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
//	I3ASSERT( pFrame);
//	return (UIPopupReplyMail*) pFrame->GetPopup( UPW_MAILREP);
//}
//
//
//extern "C" {
//	int UIPopupReplyMail_Send(LuaState * L)
//	{
//		GetThis()->OnSendNote();
//		return 0;
//	}
//
//	int UIPopupReplyMail_Cancel(LuaState * L)
//	{
//		GetThis()->GetMainFrame()->TogglePopup(UPW_MAILREP);
//		return 0;
//	}
//}
//
//LuaFuncReg  UIPopupReplyMail_Glue[] = {
//	{"Send_Mail",	UIPopupReplyMail_Send	},
//	{"Cancel",		UIPopupReplyMail_Cancel	},
//	{NULL,			NULL					}
//};
//
//I3_CLASS_INSTANCE( UIPopupReplyMail, UIPopupBase);
//
//UIPopupReplyMail::UIPopupReplyMail()
//{
//	m_UserName = NULL;
//}
//
//UIPopupReplyMail::~UIPopupReplyMail()
//{
//}
//
//void UIPopupReplyMail::OnSendNote()
//{
//	LuaState* L;
//	L = UILua::CallLuaFunction(GetScene(0), "GetReceiver");
//	const char *Who = UILua::ReturnLuaString(L, 0);
//
//	UILua::CallLuaFunction(GetScene(0), "GetMessage");
//	const char *szText = UILua::ReturnLuaString(L, 0);
//	
//	
//	if(i3String::Length(szText) <= 0)
//	{
//		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_INPUT_NOTE"));/*쪽지 내용을 입력해 주세요.*/
//	}
//	else
//	{
//		if ( i3String::CompareCase( g_pGameContext->GetMyNickName(), szText) == 0)
//		{
//			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_SEND_SELF"));/*자신에게 쪽지를 보낼 수 없습니다.*/
//		}
//		else
//		{
//			if( g_pFramework->IsFilter(szText) == FALSE)
//			{
//				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
//				//m_pNoteValue->SetFocus( TRUE);
//			}
//			else
//			{
//
//#ifdef USE_MESS_TEST
//				NOTE_BODY note;
//				i3String::Copy( note._sender, Who, NET_NICK_NAME_SIZE);
//				i3String::Copy( note._message, szText, NET_NOTE_SIZE);
//				note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
//				note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
//
//				g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
//
//#else
//				NOTE_BODY note;
//				i3String::Copy( note._sender, Who);
//				i3String::Copy( note._message, szText);
//				note._senderSize = (UINT8)(i3String::Length( note._sender) + 1);
//				note._messageSize = (UINT8)(i3String::Length( note._message) + 1);
//
//				g_pGameContext->SetEvent( EVENT_SEND_NOTE, &note);
//#endif
//				GetMainFrame()->ClosePopup(UPW_MAILREP);
//			}
//		}
//	}
//}
//
//void UIPopupReplyMail::_SetUserState(INT32 b_Index, const char *Nick)
//{
//	LuaState * L = _CallLuaFunction("SetData");
//	i3Lua::PushInteger(L, b_Index);
//	i3Lua::PushA2UTF8String(L, Nick);
//	i3Lua::PushA2UTF8String(L, "");
//	_EndLuaFunction(L, 3);
//}
//
///*virtual*/ void UIPopupReplyMail::ProcessGameEvent( GAME_EVENT event, INT32 Arg)
//{
//	base::ProcessGameEvent(event,Arg);
//}
//
///*virtual*/ void UIPopupReplyMail::OnLoadAllScenes()
//{
//	base::OnLoadAllScenes();
//	_SetUserState(0, m_UserName);
//}
//
///*virtual*/ void UIPopupReplyMail::OnCreate(i3GameNode * pParent)
//{
//	base::OnCreate( pParent);
//
//	AddScene( "UIRe/PBRe_PopUp_send_packet.i3UIe", UIPopupReplyMail_Glue);
//}
//
///*virtual*/ bool UIPopupReplyMail::OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */)
//{
//	if( base::OnEntranceStart(pArg1,pArg2) == false)
//		return false;
//
//	m_UserName = (const char *)pArg1;
//
//	return true;
//}
///*virtual*/ bool UIPopupReplyMail::OnCloseButton()
//{
//	base::OnCloseButton();
//	return true;
//}