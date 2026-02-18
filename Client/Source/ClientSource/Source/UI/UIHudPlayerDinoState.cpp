#include "pch.h"
#include "UIHudPlayerDinoState.h"

#include "UIHudUtil.h"

#include "BattleHud.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

UIHudPlayerDinoState::UIHudPlayerDinoState(UIHudManager* p) : UIHudState(p)
{
	m_is_dino_state_hud = 1;
}

UIHudPlayerDinoState::~UIHudPlayerDinoState()
{
}

/*********************************/
void UIHudPlayerDinoState::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "PlayerDinoState");

	m_HP.AddNumberControl(pScene, "i3UIButtonImageSet5");
	m_HP.AddNumberControl(pScene, "i3UIButtonImageSet4");
	m_HP.AddNumberControl(pScene, "i3UIButtonImageSet3");

	m_pText[KILL] = FindControl<i3UIStaticText*>(pScene, "KillCount");
	m_pText[DEATH] = FindControl<i3UIStaticText*>(pScene, "DeathCount");
	m_pText[ASSIST] = FindControl<i3UIStaticText*>(pScene, "AssistCount");

	m_ProgressBar.SetControl(pScene, "i3UIProgressBar");
	m_ProgressBar.AddColorSpot(30, 255, 255, 255);
	m_ProgressBar.AddColorSpot(0, 255, 0, 0);
}

void	UIHudPlayerDinoState::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	//라운드 스타트 하기 전 까지 사람 임.
	REAL32 fPer = 0.f;
	hu::get_chara_hp( s_pMyChara, fPer);
	if(fPer > 0.0f && fPer < 1.0f)
		fPer = 1.0f;
	m_HP.SetNumber( static_cast<INT32>(fPer));

	m_ProgressBar.SetNumber(static_cast<INT32>(fPer), 0, 100);

	INT32 nKill = 0, nDeath = 0, nAssist = 0;
	hu::get_kill_death_assist(nKill, nDeath, nAssist);

	i3::stack_wstring strKill;
	i3::stack_wstring strDeath;
	i3::stack_wstring strAssist;
	i3::sprintf(strKill, L"%d Kill", nKill);
	i3::sprintf(strDeath, L"%d Death", nDeath);
	i3::sprintf(strAssist, L"%d Assist", nAssist);

	m_pText[KILL]->SetText(strKill);
	m_pText[DEATH]->SetText(strDeath);
	m_pText[ASSIST]->SetText(strAssist);
}