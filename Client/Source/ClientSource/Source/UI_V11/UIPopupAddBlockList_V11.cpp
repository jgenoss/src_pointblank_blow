#include "pch.h"
#include "UIPopupAddBlockList_V11.h"
#include "../UI/UIMainFrame_V11.h"
#include "../UI/UIUtil.h"

I3_CLASS_INSTANCE( UIPopupAddBlockList_V11);

static UIPopupAddBlockList_V11 * GetThis(void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupAddBlockList_V11*) pFrame->GetPopup( UPW_ADDBLOCK);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupAddBlockList_V11_Confirm(LuaState * L)
	{
//		GetThis()->SetComment(i3Lua::GetStringArg(L, 1, nullptr, 0, ""));
		GetThis()->OnOKButton();
		return 0;
	}
	int UIPopupAddBlockList_V11_Cancel(LuaState * L)
	{
		GetThis()->OnCancel();
		return 0;
	}
}

LuaFuncReg UIPopupAddBlockList_V11_Glue[] = {
	{"AddBlock_Confirm",	UIPopupAddBlockList_V11_Confirm},
	{"AddBlock_Cancel",		UIPopupAddBlockList_V11_Cancel },
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UIPopupAddBlockList_V11::UIPopupAddBlockList_V11() : m_pCommentBox(nullptr)
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupAddBlockList_V11::~UIPopupAddBlockList_V11()
{
}

/*virtual*/ void UIPopupAddBlockList_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	Base::_InitializeAtLoad( pScene);

	m_pCommentBox = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
	I3ASSERT( m_pCommentBox != nullptr);
}

/*virtual*/ void UIPopupAddBlockList_V11::OnCreate( i3GameNode * pParent)
{
	Base::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PointBlankRe_PopUp_BlackList.i3UIs",UIPopupAddBlockList_V11_Glue);
}

/*virtual*/ bool UIPopupAddBlockList_V11::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( Base::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	LuaState * L = _CallLuaFunction( "SetNick");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, m_wstrNick);
		_EndLuaFunction( L, 1);
	}
	return true;
}

/*virtual*/ void UIPopupAddBlockList_V11::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	Base::ProcessGameEvent(event, Arg, UserData);
	switch(event)
	{
	case EVENT_INSERT_BLOCK:
		if(EV_SUCCESSED(Arg))
		{
			i3::stack_wstring _Message;

			i3::sprintf(_Message, GAME_STRING("STR_UI_BLOCK_SUCCESS"), CCommunityContext::i()->GetLastAdded()->_wstrNick);
			GameUI::MsgBox( MSG_TYPE_GAME_OK, _Message);
			GetMainFrame()->TogglePopup(UPW_ADDBLOCK);
		}
	}
}

/*virtual*/ bool UIPopupAddBlockList_V11::OnOKButton( void)
{
	bool rv = Base::OnOKButton();

	
	if( i3::generic_string_size(m_pCommentBox->getText()) > 0)
	{
		GameUI::AddBlockList( m_wstrNick, MAKE_CALLBACK(CbBlockUser), this);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STBL_IDX_CLAN_PLEASE_INPUT"));
	}

	return rv;
}

void UIPopupAddBlockList_V11::OnCancel(void)
{
	GetMainFrame()->TogglePopup(UPW_ADDBLOCK);
}

void UIPopupAddBlockList_V11::CbBlockUser( void* pThis, INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		((UIPopupAddBlockList_V11*)pThis)->DoBlockUser();
	}
}

void UIPopupAddBlockList_V11::DoBlockUser( void)
{
	i3::safe_string_copy( m_BlockUser._wstrNick, m_wstrNick, NET_NICK_NAME_SIZE );
	i3::safe_string_copy( m_BlockUser._wstr_comment, m_pCommentBox->getText(), BLOCK_COMMENT_MAX );
	/*m_BlockUser._nicksize = (UINT8)(i3String::Length(m_szNick) + 1);
	m_BlockUser._commentsize = (UINT8)(i3String::Length(m_Comment) + 1);*/

	GameEventSender::i()->SetEvent(EVENT_INSERT_BLOCK, (void *)&m_BlockUser);
}