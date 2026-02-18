#include "pch.h"
#include "UIHUDBase.h"

I3_CLASS_INSTANCE( UIHUDBase);

UIHUDBase::UIHUDBase()
{
}

UIHUDBase::~UIHUDBase()
{
}

/*virtual*/ void UIHUDBase::OnUpdate( REAL32 rDeltaSeconds)
{
	UIBase::OnUpdate( rDeltaSeconds);
}

static void recSetNoReleaseTextureFlag( i3UIControl * pCtrl, bool bRelease)
{
	if( bRelease)
		pCtrl->removeStyle( I3UI_STYLE_BLOCK_AUTO_TRASH_TEX);
	else
		pCtrl->addStyle( I3UI_STYLE_BLOCK_AUTO_TRASH_TEX);

	i3UIControl * pChild = static_cast<i3UIControl*>(pCtrl->getFirstChild());
	while( pChild != nullptr)
	{
		recSetNoReleaseTextureFlag( pChild, bRelease);

		pChild = static_cast<i3UIControl*>(pChild->getNext());
	}
}

void UIHUDBase::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);
/*
		I3ASSERT( pScene != nullptr);
		i3UIControl * pCtrl = static_cast<i3UIControl*>(pScene->getFirstChild());	
		recSetNoReleaseTextureFlag( pCtrl, false);
*/	
}

void UIHUDBase::_ClearAtUnload( i3UIScene * pScene)
{
//	UIBase::_InitializeAtLoad( pScene);
	UIBase::_ClearAtUnload(pScene);
/*
		I3ASSERT( pScene != nullptr);
		i3UIControl * pCtrl = static_cast<i3UIControl*>(pScene->getFirstChild());
		recSetNoReleaseTextureFlag( pCtrl, true);
*/
}
