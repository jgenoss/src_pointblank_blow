#include "pch.h"
#include "UIPopupAddBlockList.h"
#include "UIMainFrame.h"
#include "UIUtil.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

I3_CLASS_INSTANCE( UIPopupAddBlockList);//, UIPopupInteractBase);

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupAddBlockList_Confirm(LuaState * L)
	{
		UIPopupAddBlockList * pThis = static_cast<UIPopupAddBlockList*>(GameUI::GetPopup( UPW_ADDBLOCK));
		if( pThis != nullptr)
			pThis->OnOKButton();
		return 0;
	}
	int UIPopupAddBlockList_Cancel(LuaState * L)
	{
		UIPopupAddBlockList * pThis = static_cast<UIPopupAddBlockList*>(GameUI::GetPopup( UPW_ADDBLOCK));
		if( pThis != nullptr)
			pThis->OnCancel();
		return 0;
	}
}

LuaFuncReg UIPopupAddBlockList_Glue[] = {
	{"AddBlock_Confirm",	UIPopupAddBlockList_Confirm},
	{"AddBlock_Cancel",		UIPopupAddBlockList_Cancel },
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UIPopupAddBlockList::UIPopupAddBlockList()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupAddBlockList::~UIPopupAddBlockList()
{
}

/*virtual*/ void UIPopupAddBlockList::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupInteractBase::_InitializeAtLoad( pScene);
}

/*virtual*/ void UIPopupAddBlockList::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PointBlankRe_PopUp_BlackList.i3UIs",UIPopupAddBlockList_Glue);
}

/*virtual*/ bool UIPopupAddBlockList::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	LuaState * L = _CallLuaFunction( "SetNick");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, m_wstrNick.c_str(), m_wstrNick.size());
		_EndLuaFunction( L, 1);
	}
	return true;
}

/*virtual*/ void UIPopupAddBlockList::ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupInteractBase::ProcessGameEvent(event,Arg, UserData);
	if( event == EVENT_INSERT_BLOCK)
	{
		if(EV_SUCCESSED(Arg))
		{
			i3::rc_wstring wstrMessage;

			i3::sprintf(wstrMessage, GAME_RCSTRING("STR_UI_BLOCK_SUCCESS"), CCommunityContext::i()->GetLastAdded()->_wstrNick);

			GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
			GetMainFrame()->TogglePopup(UPW_ADDBLOCK);
		}
	}
}

/*virtual*/ bool UIPopupAddBlockList::OnOKButton( void)
{
	bool rv = UIPopupInteractBase::OnOKButton();

	LuaState* L = UILua::CallLuaFunction(GetScene(0),	"GetComment");
	
	const char* szComment = UILua::ReturnLuaString(L, 0);
	
	i3::utf8_to_utf16(szComment, m_wstrComment);			
//	m_Comment = UILua::ReturnLuaString(L, 0);
	
		
	if( i3::generic_string_size(m_wstrComment) > 0)
	{
		GameUI::AddBlockList( m_wstrNick, MAKE_CALLBACK(CbBlockUser), this);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_PLEASE_INPUT"));
	}

	return rv;
}

void UIPopupAddBlockList::OnCancel(void)
{
	GetMainFrame()->TogglePopup(UPW_ADDBLOCK);
}

void UIPopupAddBlockList::CbBlockUser( void* pThis, INT32 nParam)
{
	if (MBR_OK == nParam)
	{
		((UIPopupAddBlockList*)pThis)->DoBlockUser();
	}
}

void UIPopupAddBlockList::DoBlockUser( void)
{
	m_BlockUser._wstrNick = m_wstrNick; 

	m_BlockUser._wstr_comment = m_wstrComment; // BLOCK_COMMENT_MAX;

	GameEventSender::i()->SetEvent(EVENT_INSERT_BLOCK, (void *)&m_BlockUser);
}