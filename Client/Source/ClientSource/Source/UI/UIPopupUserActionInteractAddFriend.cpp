#include "pch.h"
#include "UIPopupUserActionInteractAddFriend.h"

#include "UIMainFrame.h"

#include "../CommunityContext.h"

I3_CLASS_INSTANCE( UIPopupUserActionInteractAddFriend, UIPopupUserActionInteractBase);

UIPopupUserActionInteractAddFriend::UIPopupUserActionInteractAddFriend()
{
}

UIPopupUserActionInteractAddFriend::~UIPopupUserActionInteractAddFriend()
{
}

/*virtual*/ void UIPopupUserActionInteractAddFriend::OnCreate( i3GameNode * pParent)
{
	UIPopupUserActionInteractBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_PopUp_AddFriend.i3UIe");
}

/*virtual*/ bool UIPopupUserActionInteractAddFriend::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupUserActionInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction( "SetNick");
	i3Lua::PushA2UTF8String( L, m_szNick);
	_EndLuaFunction( L, 1);

	return true;
}

void UIPopupUserActionInteractAddFriend::OnOKButton( void)
{
	g_pCommunity->InsertFriend( m_szNick);
}
