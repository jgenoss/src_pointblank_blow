#include "pch.h"
#include "UIHudTargetObject.h"

#include "UIHudUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHudTargetObject::UIHudTargetObject(UIHudManager* p) : iHudBase(p)
{
}

UIHudTargetObject::~UIHudTargetObject()
{
}

/*********************************/
void UIHudTargetObject::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "TargetObject");
	m_TargetObject[GENERATOR] = FindControl<i3UIButtonImageSet*>(pScene, "to_btn_img_set0");
}


void UIHudTargetObject::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetEnable(false);
}

void UIHudTargetObject::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable( !hu_ut::is_domination_mode() );
}