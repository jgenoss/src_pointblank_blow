#include "pch.h"
#include "UIPopupQueryPassword.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "i3Base/string/ext/utf16_to_mb.h"


I3_CLASS_INSTANCE( UIPopupQueryPassword);//, UIPopupBase);

UIPopupQueryPassword::UIPopupQueryPassword()
{
	m_pED = nullptr;
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
	m_bSendEnterRoom = false;
}

UIPopupQueryPassword::~UIPopupQueryPassword()
{
}

/*virtual*/ void UIPopupQueryPassword::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	m_pED = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
	I3ASSERT( m_pED);
	m_pED->setAlphaNumericOnly( true );
}

void UIPopupQueryPassword::_SendEnterRoom(void)
{
	if(m_bSendEnterRoom)
	{
		if ( m_pCallback )
		{
		//	char szPW[ MAX_PATH] = {0,};
			i3::string	password;

			if( m_pED != nullptr && m_pED->getText() != nullptr)
			{
				i3::utf16_to_mb(m_pED->getPassword(), password);
				
			//	INT32 len = i3::generic_string_size( m_pED->getText());
			//	if( len > 0)
			//	{
			//		i3String::CopyWCHARtoASCII( szPW, sizeof(szPW), m_pED->getText(), len);
			//	}
			}

		//	string	password = szPW;
			if(password.empty() == false)
				m_pCallback(true, password, m_pUserContext);
		}

		m_bSendEnterRoom = false;
	}
}

/*virtual*/ void UIPopupQueryPassword::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_PasswordRoom.i3UIs");
}

/*virtual*/ bool UIPopupQueryPassword::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	const QPParam *	pParam	= static_cast<const QPParam *>(pArg1);

	m_pCallback		= pParam->Callback;
	m_pUserContext	= pParam->pUserContext;

	LuaState * L = _CallLuaFunction( "Initialize");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, pParam->wstrMsg.c_str(), pParam->wstrMsg.size());
		_EndLuaFunction( L, 1);
	}

	m_bSendEnterRoom = false;

	return true;
}

void UIPopupQueryPassword::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();
	g_pFramework->SetFocus( m_pED);
}

void UIPopupQueryPassword::OnExitEnd(void)
{
	_SendEnterRoom();

	UIPopupBase::OnExitEnd();
}

bool UIPopupQueryPassword::OnOKButton( void)
{
	if(i3::generic_string_size( m_pED->getText()) > 0)
	{
		m_bSendEnterRoom = true;
		return true;
	}

	GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_JOIN_ROOM_Q"));
	return false;
}

