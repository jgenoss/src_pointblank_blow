#include "pch.h"
#include "UIHudPlayerState.h"

#include "UIHudUtil.h"
#include "UIUtil.h"

#include "BattleHud.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHudPlayerState::UIHudPlayerState(UIHudManager* p) : UIHudState(p)
{
	m_is_dino_state_hud = 0;
}

UIHudPlayerState::~UIHudPlayerState()
{
}

/*********************************/
void UIHudPlayerState::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "PlayerState");

	m_HP[CURR].AddNumberControl(pScene, "ps_Img_Cnt0");
	m_HP[CURR].AddNumberControl(pScene, "ps_Img_Cnt1");
	m_HP[CURR].AddNumberControl(pScene, "ps_Img_Cnt2");

	i3UIButtonImageSet* btn = FindControl<i3UIButtonImageSet*>(pScene, "ps_Img_Cnt6");
	btn->SetShapeIdx(10);

	m_HP[MAXIMUM].AddNumberControl(pScene, "ps_Img_Cnt3");
	m_HP[MAXIMUM].AddNumberControl(pScene, "ps_Img_Cnt4");
	m_HP[MAXIMUM].AddNumberControl(pScene, "ps_Img_Cnt5");

	m_pText[KILL] = FindControl<i3UIStaticText*>(pScene, "ps_ST_Kill");
	m_pText[DEATH] = FindControl<i3UIStaticText*>(pScene, "ps_ST_Death");
	m_pText[ASSIST] = FindControl<i3UIStaticText*>(pScene, "ps_ST_Assist");

	m_ProgressBar.SetControl(pScene, "ps_ProgressBar");

	m_ProgressBar.AddMaxColorSpot(142, 183,  64); //연녹색
	m_ProgressBar.AddColorSpot(80, 202, 176,  54); //노란
	m_ProgressBar.AddColorSpot(60, 202, 134,  54); //살구
	m_ProgressBar.AddColorSpot(40, 211,  92,  34); //갈색
	m_ProgressBar.AddColorSpot(20, 181,  35,   0); //빨간
}

void	UIHudPlayerState::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	INT32 hp =0, max_hp = 0;
	hu::get_chara_hp(s_pMyChara, hp, max_hp);

	m_HP[CURR].SetNumber(hp);
	m_HP[MAXIMUM].SetNumber(max_hp);

	m_ProgressBar.SetNumber(hp, 0, max_hp);

	/*INT32 kill =0, death = 0;
	hu::get_kill_death(kill, death);*/

	if (hu_ut::is_domination_mode() == true)
	{
		m_pText[KILL]->SetText( L"");
		m_pText[DEATH]->SetText(L"");
		m_pText[ASSIST]->SetText(L"");
	}
	else
	{
		i3::wstring strKill, strDeath, strAssist;
		hu::get_KDA_to_string( strKill, strDeath, strAssist);
		m_pText[KILL]->SetText( strKill);
		m_pText[DEATH]->SetText( strDeath);
		m_pText[ASSIST]->SetText( strAssist);
	}
}