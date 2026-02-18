#include "pch.h"
#include "UITabProfileBase_V10.h"

I3_CLASS_INSTANCE( UITabProfileBase_V10);

UITabProfileBase_V10::UITabProfileBase_V10()
{
	m_pScene = nullptr;
}

UITabProfileBase_V10::~UITabProfileBase_V10()
{
}

void UITabProfileBase_V10::OnCreate( i3GameNode * pParent)
{
	UITabBase::OnCreate(pParent);
	((UIBase*)pParent)->GetSlide()->AddSyncSlide( GetSlide() );
}