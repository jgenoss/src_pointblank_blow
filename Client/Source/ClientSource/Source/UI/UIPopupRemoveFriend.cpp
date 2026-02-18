#include "pch.h"
#include "UIPopupRemoveFriend.h"

#include "../CommunityContext.h"

I3_CLASS_INSTANCE( UIPopupRemoveFriend);//, UIPopupInteractBase);

UIPopupRemoveFriend::UIPopupRemoveFriend()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupRemoveFriend::~UIPopupRemoveFriend()
{
}

/*virtual*/ void UIPopupRemoveFriend::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PointBlankRe_PopUp_DeleteFriend.i3UIs");
}

/*virtual*/ bool UIPopupRemoveFriend::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;


	i3::stack_wstring wstrTemp;
	i3::sprintf( wstrTemp, GAME_RCSTRING("STR_UI_DELETE_FRIEND_DESC"), m_wstrNick);

	LuaState * L = _CallLuaFunction( "SetUserInfo");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, m_wstrNick.c_str(), m_wstrNick.size());
		i3Lua::PushStringUTF16To8( L, wstrTemp.c_str(), wstrTemp.size() );
		_EndLuaFunction( L, 2);
	}

	return true;
}

bool UIPopupRemoveFriend::OnOKButton( void)
{
	CCommunityContext::i()->DeleteViewFriend( m_SelectedIndex);
	return true;
}
