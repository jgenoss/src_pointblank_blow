#include "pch.h"
#include "UIPopupSendClanMail_V11.h"
#include "../UI/UIMainFrame_V11.h"
#include "../UI/UIUtil.h"

I3_CLASS_INSTANCE( UIPopupSendClanMail_V11);


static UIPopupSendClanMail_V11 * GetThis()
{
	UIPopupSendClanMail_V11* pPopup = static_cast<UIPopupSendClanMail_V11*>(GameUI::GetPopup(UPW_SEND_CLANMAIL));
	if( pPopup != nullptr)
	{
		return pPopup;
	}

	return nullptr;
}

//--------------------------------------------------------------------------//
//								Lua Glue function							//
//--------------------------------------------------------------------------//
extern "C" {
	int UIPopupSendClanMail_V11_SendMsg( LuaState * L)
	{
		GetThis()->SendMsg();
		return 0;
	}

	int UIPopupSendClanMail_V11_CloseMsg(LuaState* L)
	{
		GetThis()->CloseMsg();
		return 0;
	}
}

LuaFuncReg UIPopupSendClanMail_V11_Glue[] = {
	{ "SendMsg",					UIPopupSendClanMail_V11_SendMsg},
	{"MsgClose", 					UIPopupSendClanMail_V11_CloseMsg},
	{nullptr,						nullptr}
};



UIPopupSendClanMail_V11::UIPopupSendClanMail_V11()
{
	 m_pNote = nullptr;
}

UIPopupSendClanMail_V11::~UIPopupSendClanMail_V11()
{
}

/*virtual*/ void UIPopupSendClanMail_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_Popup_ClanMessage.i3UIs", UIPopupSendClanMail_V11_Glue);
}

/*virtual*/ void UIPopupSendClanMail_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupInteractBase::_InitializeAtLoad( pScene);
	m_pNote = (i3UIEditBox*) pScene->FindChildByName( "Context");
	I3ASSERT( m_pNote);
}

/*virtual*/bool UIPopupSendClanMail_V11::OnKey_Enter(void)
{
	if(SendMsg())
		GetMainFrame()->ClosePopup( UPW_SEND_CLANMAIL);

	return true;
}


bool UIPopupSendClanMail_V11::SendMsg( void)
{
	i3::stack_wstring Message;

	if(i3::generic_string_size( m_pNote->getText()) <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_POPUP_INPUT_NOTE"));/*쪽지 내용을 입력해 주세요.*/
		return false;
	}
	else
	{
		Message = m_pNote->getText();

		if( g_pFramework->ChangeFilterStringOK( Message) == false)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_STRING("STR_POPUP_MESSAGE_INVALID_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다.*/
			g_pFramework->SetFocus( m_pNote);
			return false;
		}
		else
		{
			// 쪽지보내기
			g_pFramework->GetClanContext()->SendClanNote( (UINT8)CLAN_NOTE_ALL, Message.c_str());
		}

	}

	return true;

}

void UIPopupSendClanMail_V11::CloseMsg( void)
{
	GetMainFrame()->ClosePopup(UPW_SEND_CLANMAIL);
}