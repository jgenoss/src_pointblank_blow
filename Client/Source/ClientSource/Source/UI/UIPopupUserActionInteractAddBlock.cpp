#include "pch.h"
#include "UIPopupUserActionInteractAddBlock.h"

I3_CLASS_INSTANCE( UIPopupUserActionInteractAddBlock, UIPopupUserActionInteractBase);

UIPopupUserActionInteractAddBlock::UIPopupUserActionInteractAddBlock()
{
}

UIPopupUserActionInteractAddBlock::~UIPopupUserActionInteractAddBlock()
{
}

/*virtual*/ void UIPopupUserActionInteractAddBlock::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupUserActionInteractBase::_InitializeAtLoad( pScene);

	m_pReasonBox = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox");
}

/*virtual*/ void UIPopupUserActionInteractAddBlock::OnCreate( i3GameNode * pParent)
{
	UIPopupUserActionInteractBase::OnCreate( pParent);

	// Load Scene
	AddScene( "UIRe/PointBlankRe_PopUp_BlackList.i3UIe");
}

/*virtual*/ bool UIPopupUserActionInteractAddBlock::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupUserActionInteractBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	LuaState * L = _CallLuaFunction( "SetNick");
	i3Lua::PushA2UTF8String( L, m_szNick);
	_EndLuaFunction( L, 1);

	return true;
}

/*virtual*/ void UIPopupUserActionInteractAddBlock::OnOKButton( void)
{
	if( m_pReasonBox)
	{
		//m_pReasonBox->GetText();
	}
}
