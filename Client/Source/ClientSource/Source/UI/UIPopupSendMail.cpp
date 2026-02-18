#include "pch.h"
#include "UIPopupSendMail.h"
//
//#include "UIMainFrame.h"
//#include "UIUtil.h"
//
//static UIPopupSendMail * GetThis( LuaState * L)
//{
//	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
//	if( pFrame != NULL)
//	{
//		UIPOPUPWIN id = pFrame->FindPopup( L);
//		I3ASSERT( id != UPW_NULL);
//
//		return (UIPopupSendMail*) pFrame->GetPopup( id);
//	}
//
//	return NULL;
//}
//
////--------------------------------------------------------------------------//
////								Lua Glue function							//
////--------------------------------------------------------------------------//
//extern "C" {
//	int UIPopupSendMail_Search( LuaState * L)
//	{
//		GetThis( L)->CheckUser();
//		return 0;
//	}
//}
//
//LuaFuncReg UIPopupSendMail_Glue[] = {
//	{ "Search",					UIPopupSendMail_Search},
//	{NULL,						NULL}
//};
//
//I3_CLASS_INSTANCE( UIPopupSendMail, UIPopupInteractBase);
//
//UIPopupSendMail::UIPopupSendMail()
//{	
//}
//
//UIPopupSendMail::~UIPopupSendMail()
//{
//}
//
///*virtual*/ void UIPopupSendMail::_InitializeAtLoad( i3UIScene * pScene)
//{
//	UIPopupInteractBase::_InitializeAtLoad( pScene);
//
//	m_pWho = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox0");
//	I3ASSERT( m_pWho);
//
//	m_pNote = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox3");
//	//아이디 검색 기능미구현으로주석처리합니다. -상옥
//	m_pSearch = (i3UIButtonComposed3 *) pScene->FindChildByName("i3UIButtonComposed31");
//	m_pSearch->SetEnable(false);
//
//	I3ASSERT( m_pNote);
//}
//
//#ifdef USE_MESS_TEST
//INT64 UIPopupSendMail::_FindNoteByUid( const char* nick)
//{
//	INT32 length;
//
//	// 자신은 막는다.
//	if (g_pGameContext->IsMyNickName( nick))
//	{
//		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_INPUT_SELF"));/*자신의 닉네임을 입력하셨습니다.*/
//		return 0;
//	}
//
//	NOTE_RECEIVE* pNote = NULL;
//	pNote = (NOTE_RECEIVE*)g_pGameContext->GetReceiveNote( m_iListIdx);
//
//	if( pNote == NULL )
//	{
//		I3NOTICE( "pNote == NULL \n");
//		return 0;
//	}
//
//	if ( 0 != i3String::Compare(pNote->body._sender, nick))
//	{
//		NOTE_BODY note;
//		length = i3::generic_string_size( m_pWho->getText());
//		i3String::CopyWCHARtoASCII( note._sender, sizeof(note._sender), m_pWho->getText(), length);
//		
//		length = i3::generic_string_size( m_pNote->getText());
//		i3String::CopyWCHARtoASCII( note._message, sizeof( note._message), m_pNote->getText(), length);
//		note._senderSize = (UINT8)(i3::generic_string_size( note._sender) + 1);
//		note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);
//		GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);
//		return 0;
//	}
//
//	return pNote->header._suid;
//}
//#endif
//
///*virtual*/ void UIPopupSendMail::OnCreate( i3GameNode * pParent)
//{
//	UIPopupInteractBase::OnCreate( pParent);
//
//	AddScene( "UIRe/PointBlankRe_PopUp_MessageSend.i3UIs", UIPopupSendMail_Glue);
//}
//
///*virtual*/ bool UIPopupSendMail::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
//{
//	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
//		return false;
//
//	LuaState * L = _CallLuaFunction( "SetUser");
//	if( L != NULL)
//	{
//		i3Lua::PushA2UTF8String( L, m_szNick);
//		_EndLuaFunction( L, 1);
//	}
//
//	if( pArg2 != NULL)
//	{
//		m_iListIdx = *(INT32*)pArg2;
//	}
//
//	return true;
//}
//
///*virtual*/ bool UIPopupSendMail::OnOKButton( void)
//{
//	INT32 length;
//	char Sender[NET_NICK_NAME_SIZE];
//	char Message[ NET_NOTE_SIZE];
//
//	if(i3::generic_string_size( m_pWho->getText()) <= 0)
//	{
//		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_MESSAGE_INPUT_RECEIVER"));/*받는 사람을 입력해 주세요.*/
//		return false;
//	}
//	else if(i3::generic_string_size( m_pNote->getText()) <= 0)
//	{
//		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_INPUT_NOTE"));/*쪽지 내용을 입력해 주세요.*/
//		return false;
//	}
//	else
//	{
//		i3mem::FillZero( Sender, sizeof( Sender));
//		i3mem::FillZero( Message, sizeof( Message));
//
//		length = i3::generic_string_size( m_pWho->getText());
//		i3String::CopyWCHARtoASCII( Sender, NET_NICK_NAME_SIZE, m_pWho->getText(), length);
//
//		length = i3::generic_string_size( m_pNote->getText());
//		i3String::CopyWCHARtoASCII( Message, sizeof(Message), m_pNote->getText(), length);
//		
//		if ( i3String::CompareCase( g_pGameContext->GetMyNickName(), Sender) == 0)
//		{
//			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_SEND_SELF"));/*자신에게 쪽지를 보낼 수 없습니다.*/
//			m_pWho->SetText( L"");
//			g_pFramework->SetFocus( m_pWho);
//			return false;
//		}
//		else
//		{
//			if( g_pFramework->IsFilter( Message) == FALSE)
//			{
//				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
//				g_pFramework->SetFocus( m_pNote);
//				return false;
//			}
//			else
//			{
//				// 쪽지 보내기
//
//		#ifdef USE_MESS_TEST
//				if( m_bReceiverUIDFlag == false)
//				{
//					NOTE_BODY note;
//					
//					i3::safe_string_copy( note._sender, Sender, NET_NICK_NAME_SIZE );
//					
//					i3::safe_string_copy( note._message, Message, NET_NOTE_SIZE );
//					
//					note._senderSize = (UINT8)(i3::generic_string_size( note._sender) + 1);
//					note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);
//	
//					GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);
//				}
//				else
//				{
//					NOTE_BODY_BY_UID note;
//
//					m_bReceiverUIDFlag = FALSE;
//
//					// 쪽지 목록으로 확인
//					note._receiverUid = _FindNoteByUid(Sender);
//					if( 0 >= note._receiverUid)
//					{
//						return true;
//					}
//
//					i3::safe_string_copy( note._message, Message, NET_NOTE_SIZE );
//	
//					note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);
//
//					GameEventSender::i()->SetEvent( EVENT_SEND_NOTE_BY_UID, &note);
//				}
//		#else
//				NOTE_BODY note;
//				i3::safe_string_copy( note._sender, Sender);
//				i3::safe_string_copy( note._message, Message);
//				note._senderSize = (UINT8)(i3::generic_string_size( note._sender) + 1);
//				note._messageSize = (UINT8)(i3::generic_string_size( note._message) + 1);
//
//				GameEventSender::i()->SetEvent( EVENT_SEND_NOTE, &note);
//		#endif
//			}
//		}
//	}
//
//	return true;
//}
//
//void UIPopupSendMail::CheckUser( void)
//{
//	char szTemp[ MAX_STRING_COUNT] = {0,};
//
//	INT32 lenW = i3::generic_string_size( m_pWho->getText());
//	i3String::CopyWCHARtoASCII( szTemp, MAX_STRING_COUNT, m_pWho->getText(), lenW);
//
//	if( GameUI::FindUserUidByNick( szTemp) <= 0)
//	{
//		/* 유저를 찾을 수 없습니다 */
//		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_TBL_GUI_BASE_NO_USER_IN_USERLIST"));
//	}
//}
//
///*virtual*/bool UIPopupSendMail::OnKey_Enter(void)
//{
//	if(OnOKButton())
//		GetMainFrame()->ClosePopup( UPW_SENDMAIL);
//	
//	return true;
//}	