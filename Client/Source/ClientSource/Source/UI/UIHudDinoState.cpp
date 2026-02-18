#include "pch.h"
#include "UIHudDinoState.h"

#include "UIHudUtil.h"

#include "BattleHud.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHudDinoState::UIHudDinoState(UIHudManager* p) : UIHudState(p)
{
	m_is_dino_state_hud = 1;
}

UIHudDinoState::~UIHudDinoState()
{
}

void UIHudDinoState::InitializeAtLoad(i3UIScene * scene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(scene, "DinoState");

	m_attack1Img	= FindControl<i3UIButtonImageSet*>(scene, "Attack1");
	m_attack2Img	= FindControl<i3UIButtonImageSet*>(scene, "Attack2");
	m_skillGuage	= FindControl<i3UIProgressBar*>(scene, "SkillGuage");
	m_dinoName		= FindControl<i3UIStaticText*>(scene, "DinoName");
	m_dinoName->SetTextAlign(I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE);
}

void	UIHudDinoState::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	m_skillGuage->setProgress(hu_dino::att.ratio);
}

/*************************************/
/*virtual*/ void UIHudDinoState::event_respawn_battle_impl(INT32 arg)
{
	i3_prof_func();

	m_attack1Img->SetShapeIdx(static_cast<INT32>(hu_dino::att.idx1));
	m_attack2Img->SetShapeIdx(static_cast<INT32>(hu_dino::att.idx2));
	m_dinoName->SetText(hu_dino::att.wstr_name);
}