#include "pch.h"
#include "UIHudChallengeBoard.h"

#include "UIHudUtil.h"
#include "UIHudDefine.h"

#include "../StageBattle/AIModeContext.h"
#include "UserInfoContext.h"

UIHudChallengeBoard::UIHudChallengeBoard(UIHudManager* p) : iHudBase(p)
{
	pf_update_level[HUD::HS_1_5] = &UIHudChallengeBoard::update_level_1_5;
	pf_update_level[HUD::HS_1_0] = &UIHudChallengeBoard::update_level_1_0;
}

UIHudChallengeBoard::~UIHudChallengeBoard()
{
}

/*********************************/
void UIHudChallengeBoard::InitializeAtLoad( i3UIScene * pScene)
{
	iHudBase::AddFrameWnd(pScene, "ChallengeBoard1");
	iHudBase::AddFrameWnd(pScene, "ChallengeBoard2");

	m_Number[PT].AddNumberControl(pScene, "cmd_Img_B1_Cnt0");
	m_Number[PT].AddNumberControl(pScene, "cmd_Img_B1_Cnt1");
	m_Number[PT].AddNumberControl(pScene, "cmd_Img_B1_Cnt2");
	m_Number[PT].AddNumberControl(pScene, "cmd_Img_B1_Cnt3");
	m_Number[PT].AddNumberControl(pScene, "cmd_Img_B1_Cnt4");
	
	m_Number[DIF].AddNumberControl(pScene, "cmd_Img_B2_Cnt0");
	m_Number[DIF].AddNumberControl(pScene, "cmd_Img_B2_Cnt1");

	if(g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		m_Number[PT].EnableDynamicCenter();
		m_Number[DIF].EnableDynamicCenter();
	}

	if( g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
		m_pLevelText = FindControl<i3UIStaticText*>(pScene, "cmd_ST_B2_Level");
	else
		m_pLevel_ImgSet = FindControl<i3UIButtonImageSet*>(pScene, "cmd_ImgSet_B2_Level");

	i3UIStaticText* pText = FindControl<i3UIStaticText*>(pScene, "cmd_ST_B1_Name");
	pText->SetText( GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_SCORE_GET_POINT") );

	pText = FindControl<i3UIStaticText*>(pScene, "cmd_ST_B2_Name");
	pText->SetText( GAME_RCSTRING("STR_TBL_GUI_READY_DIFFICULTY") );
}

void	UIHudChallengeBoard::Update( REAL32 rDeltaSeconds)
{
	(this->*pf_update_level[g_pEnvSet->m_nGameInterface])(rDeltaSeconds);
}

void UIHudChallengeBoard::update_level_1_5(REAL32 rDeltaSeconds)
{
	m_Number[PT].SetNumber( hu::get_challenge_point() );

	INT32 diff = AIModeContext::i()->CurrentLevel;
	m_Number[DIF].SetNumber( diff );
	m_pLevelText->SetText( hu::get_challenge_difficulty_text(diff) );
}
void UIHudChallengeBoard::update_level_1_0(REAL32 rDeltaSeconds)
{
	m_Number[PT].SetNumber( hu::get_challenge_point() );

	INT32 diff = AIModeContext::i()->CurrentLevel;
	m_Number[DIF].SetNumber( diff );
	m_pLevel_ImgSet->SetShapeIdx( hu::get_challenge_difficulty_idx(diff) );
}

/*********************************/
void UIHudChallengeBoard::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	if (hu_ut::is_stage_mask_training() || hu_ut::is_stage_mask_diehard())
		iHudBase::SetEnable(true);
	else
		iHudBase::SetEnable(false);

	iHudBase::SetVisible(false);
}

void UIHudChallengeBoard::event_first_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	if( UserInfoContext::i()->IsGM_Observer() == true )
		iHudBase::SetEnable(false);
	else
		iHudBase::SetVisible(true);
}

void UIHudChallengeBoard::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable( !hu_ut::is_domination_mode() );
}