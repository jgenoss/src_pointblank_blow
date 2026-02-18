#include "pch.h"

#include "UIPopupGMChatBox.h"
#include "UIMainFrame.h"
#include "GameMaster.h"
#include "UserInfoContext.h"
#include "UIUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "GlobalInfoDef.h"
#include "GMChatBuffer.h"
#include "../GMAuthorityContext.h"

I3_CLASS_INSTANCE( UIPopupGMChatBox);//, UIPopupBase);

extern "C" {

}

LuaFuncReg UIPopupGMChatBox_Glue[] = {
	{nullptr,							nullptr}
};


UIPopupGMChatBox::UIPopupGMChatBox() : m_pChatLogBox(nullptr),
m_pChatEdit(nullptr)
{
	m_pChatBuffer = new CGMChatBuffer();
	MEMDUMP_NEW( m_pChatBuffer, sizeof(CGMChatBuffer) );

	RegisterGameEventFunc( EVENT_GM_AUTHORITY_CHAT_RECEIVE, &UIPopupGMChatBox::ReceiveGameEvent_Chat_Message_Receive);
}

UIPopupGMChatBox::~UIPopupGMChatBox()
{
	I3_SAFE_DELETE( m_pChatBuffer);
}

void UIPopupGMChatBox::_SendChatMessage(void)
{
//	i3::string strImeBuf;
//	i3::utf16_to_mb(m_pChatEdit->getIMETextBuf(), strImeBuf);

	const i3::wstring& wstrImeBuf = m_pChatEdit->getIMETextBufString();

	if(wstrImeBuf.length() > 0)
	{
		i3::wstring wstr_chatMsg = L"[";

		i3::generic_string_cat( wstr_chatMsg, UserInfoContext::i()->GetMyNickName() );
		i3::generic_string_cat( wstr_chatMsg, L"] ");
		i3::generic_string_cat( wstr_chatMsg, wstrImeBuf);

		T_UID _UID = 0;
		I3ASSERT( wstr_chatMsg.length() < 65536);
	
	
		// 채팅 버퍼에 기록
		m_pChatBuffer->PutChatting(wstr_chatMsg);

		UINT16 _Size = (UINT16)wstr_chatMsg.size();
		// 채팅 메세지 전송
		GameEventSender::i()->SetEvent(EVENT_GM_AUTHORITY_CHAT_SEND, &_UID, &_Size, wstr_chatMsg.c_str() );

		// EditBox 초기화
		m_pChatEdit->SetText( L"");
		m_pChatEdit->GetIME()->SetInputText( L"", 0);

		_UpdateChatLogBox();
	}
}

void UIPopupGMChatBox::_UpdateChatLogBox(void)
{
	if(m_pChatLogBox == nullptr)
		return;

	i3::wstring wstrChatLog; // [ MAX_GM_CHATBOX_BUFF];

	m_pChatBuffer->CopyChatBuffer(wstrChatLog);
	m_pChatLogBox->SetText(wstrChatLog);
}

/*virtual*/ void UIPopupGMChatBox::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pChatLogBox = i3::same_cast<i3UIEditBox*>(pScene->FindChildByName( "Edit_ChatLog"));
	I3ASSERT( m_pChatLogBox != nullptr);

	m_pChatEdit = i3::same_cast<i3UIEditBox*>(pScene->FindChildByName( "Edit_Chat"));
	I3ASSERT( m_pChatEdit != nullptr);
}

/*virtual*/ void UIPopupGMChatBox::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	m_pChatLogBox = nullptr;
	m_pChatEdit = nullptr;
	m_pChatBuffer->ResetBuffer();
}

/*virtual*/ void UIPopupGMChatBox::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	//// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_GMChatBox.i3UIs", UIPopupGMChatBox_Glue);
}

/*virtual*/ bool UIPopupGMChatBox::OnEntranceStart(void * pArg1 , void * pArg2 )
{
	if(UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	GetSlide()->ForceEnd();
	return true;
}

/*virtual*/ bool UIPopupGMChatBox::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	bool bRv = UIPopupBase::OnEvent( event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_CLEAR					:
		case EDIT_STATE_CHANGE_COMPOSITION_CHAR :
		case EDIT_STATE_CHANGE_INPUT_CHAR		:	break;
		case EDIT_STATE_RETURN					:	_SendChatMessage();	break;
		default:	break;
		}
	}

	return bRv;
}

/*virtual*/ bool UIPopupGMChatBox::OnKey_Enter(void)
{
	if( !m_pChatEdit->isFocused())
	{
		g_pFramework->SetFocus( m_pChatEdit);
		return true;
	}

	return false;
}

/*virtual*/ void UIPopupGMChatBox::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIPopupGMChatBox::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupGMChatBox::ReceiveGameEvent_Chat_Message_Receive( INT32 arg, const i3::user_data&)
{
	if ( EV_SUCCESSED(arg))
	{
//		char _RecvMssage[MAX_CHATTING_COUNT] = "";
//		i3::safe_string_copy(_RecvMssage, GMAuthorityContext::i()->GetChatMessage(), GMAuthorityContext::i()->GetMessageLenth());
		
		m_pChatBuffer->PutChatting(GMAuthorityContext::i()->GetChatMessage());

		_UpdateChatLogBox();
	}
}
