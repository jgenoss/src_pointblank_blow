#include "pch.h"
#include "UIPopupAddFriend.h"

#include "UIMainFrame.h"

#include "../CommunityContext.h"

I3_CLASS_INSTANCE( UIPopupAddFriend, UIPopupInteractBase);

UIPopupAddFriend::UIPopupAddFriend()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupAddFriend::~UIPopupAddFriend()
{
}

/*virtual*/ void UIPopupAddFriend::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_PopUp_AddFriend.i3UIe");
}

/*virtual*/ bool UIPopupAddFriend::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	char szMessage[MAX_STRING_COUNT];
	sprintf( szMessage, GAME_STRING("STR_UI_POPUP_ADD_FRIEND_DETAIL"), m_szNick);

	LuaState * L = _CallLuaFunction( "SetNick");
	if( L != NULL)
	{
		i3Lua::PushA2UTF8String( L, m_szNick);
		i3Lua::PushA2UTF8String( L, szMessage);
		_EndLuaFunction( L, 2);
	}
	return true;
}

bool UIPopupAddFriend::OnOKButton( void)
{
	g_pCommunity->InsertFriend( m_szNick);

	return true;
}
