#include "pch.h"
#include "UITabProfileBase_V11.h"

I3_CLASS_INSTANCE( UITabProfileBase_V11);

UITabProfileBase_V11::UITabProfileBase_V11()
{
	m_pScene = nullptr;
}

UITabProfileBase_V11::~UITabProfileBase_V11()
{
}

void UITabProfileBase_V11::OnCreate( i3GameNode * pParent)
{
	UITabBase::OnCreate(pParent);
	((UIBase*)pParent)->GetSlide()->AddSyncSlide( GetSlide() );
}


/*
void	UITabProfileBase_V11::OnOpen()
{
	// 구코드에서는 스크롤리스트의 0값셋팅 존재함...여기서는 우선 생략..
	SetEnable(true);
	//i3UI::getManager()->ShowSceneTop(GetScene(0));

}
*/