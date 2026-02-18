#include "pch.h"
#include "UIPopupAcceptFriend.h"

#include "../CommunityContext.h"

I3_CLASS_INSTANCE( UIPopupAcceptFriend, UIPopupInteractBase);

UIPopupAcceptFriend::UIPopupAcceptFriend()
{
}

UIPopupAcceptFriend::~UIPopupAcceptFriend()
{
}

/*virtual*/ void UIPopupAcceptFriend::OnCreate( i3GameNode * pParent)
{
	UIPopupInteractBase::OnCreate( pParent);

	AddScene( "UIRe/PointBlankRe_PopUp_InviteAccept.i3UIe");
}

/*virtual*/ bool UIPopupAcceptFriend::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if( m_SelectedIndex == -1)
		return false;

	const FRIEND_NODE* node = g_pCommunity->GetViewFriend( m_SelectedIndex);

	if( node == NULL)
	{
		return false;
	}
	else
	{
		LuaState * L = _CallLuaFunction( "SetUserInfo");
		if( L != NULL)
		{
			i3Lua::PushA2UTF8String( L, node->_nick);
			_EndLuaFunction( L, 1);
		}
	}

	return true;
}

/*virtual*/ bool UIPopupAcceptFriend::OnOKButton( void)
{
	g_pCommunity->AcceptViewFriend( m_SelectedIndex);
	return true;
}
