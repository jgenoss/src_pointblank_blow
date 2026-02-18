#include "pch.h"
#include "UIHudDinoAmmoState.h"

#include "UIHudUtil.h"

#include "BattleHud.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHudDinoAmmoState::UIHudDinoAmmoState(UIHudManager* p) : UIHudState(p)
{
	m_is_dino_state_hud = 1;
}

UIHudDinoAmmoState::~UIHudDinoAmmoState()
{
}

/*********************************/
void UIHudDinoAmmoState::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "DinoAmmoState");

	hs_1_0::set_control(dino_control, pScene);
}

void	UIHudDinoAmmoState::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if ( s_pMyChara == nullptr)
		return;

	if ( !s_pMyChara->IsDino())
		return;

	//라운드 스타트 하기 전 까지 사람 임.
	REAL32 fPer = 0.f;
	hu::get_chara_hp( s_pMyChara, fPer);
	if(fPer > 0.0f && fPer < 1.0f)
		fPer = 1.0f;

	dino_control.player_hp.SetNumber( static_cast<INT32>(fPer));

	dino_control.dino_skill_guage->setProgress( hu_dino::att.ratio );
}

/*************************************/
/*virtual*/ void UIHudDinoAmmoState::event_respawn_battle_impl(INT32 arg)
{
	i3_prof_func();


	hs_1_0::show_dino_hud(dino_control);

	hc::btn_set_shape(dino_control.dino_skill_imgset[hs_1_0::SKILL1], hu_dino::att.idx1);
	dino_control.dino_skill_text[hs_1_0::SKILL1]->SetText( hu_dino::get_dino_attck_string(hu_dino::att.idx1) );

	hc::btn_set_shape(dino_control.dino_skill_imgset[hs_1_0::SKILL2], hu_dino::att.idx2);
	dino_control.dino_skill_text[hs_1_0::SKILL2]->SetText( hu_dino::get_dino_attck_string(hu_dino::att.idx2) );

	hc::btn_set_shape(dino_control.player_state_imgset, hu_dino::get_dino_shape_index());
}