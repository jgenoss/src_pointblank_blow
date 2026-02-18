#include "pch.h"
#include "UIHudWinterMode.h"
#include "UIHudManager.h"

UIHudWinterMode::UIHudWinterMode(UIHudManager* p) : iHudBase(p)
{
}

UIHudWinterMode::~UIHudWinterMode()
{
}

/*********************************/
void UIHudWinterMode::InitializeAtLoad( i3UIScene * pScene)
{
	iHudBase::AddFrameWnd(pScene, "WinterMode");

	i3UIButtonImageSet* Icon  = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet1");
	i3UIButtonImageSet* Icon1 = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet2");
	i3UIButtonImageSet* Icon2 = FindControl<i3UIButtonImageSet*>(pScene, "i3UIButtonImageSet6");

	Icon->SetVisible(false);
	Icon1->SetVisible(false);
	Icon2->SetVisible(false);
}

void UIHudWinterMode::Update( REAL32 rDeltaSeconds)
{
}