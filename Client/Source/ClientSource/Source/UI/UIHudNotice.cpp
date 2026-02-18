#include "pch.h"
#include "UIHudNotice.h"

#include "UIHudUtil.h"
#include "UIHudManager.h"

#include "BattleHud.h"
#include "UIHUDIngameChat.h"
#include "ShutDownContext.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "../StageBattle/AIModeContext.h"

UIHudNotice::UIHudNotice(UIHudManager* p) : iHudBase(p)
{
	m_TextShow = false;
	m_need_update = false;

	m_AlarmState = EMPTY_ALARM;
}

UIHudNotice::~UIHudNotice()
{
}

/*********************************/
void UIHudNotice::re_position()
{
	i3_prof_func();

	if( g_pEnvSet->m_nGameInterface == HUD::HS_1_0 ) return;

	enum { OffsetY = 10, };

	iHudBase* phud = 0;
	if( hu_ut::is_dino(s_pMyChara) == false )		phud = HUD::instance()->GetHud(HUD::PLAYER_STATE);
	else		phud = HUD::instance()->GetHud(HUD::PLAYER_DINO_STATE);

	if( phud->IsEnable() == false ) return;

	REAL32 y = phud->GetFrameWnd()->getPos()->y - OffsetY - GetFrameWnd()->getSize()->y;

	VEC3D* pos = GetFrameWnd()->getPos();
	GetFrameWnd()->setPos( pos->x, y );

	UIHUDIngameChat::instance()->re_position();
}

/*********************************/
void UIHudNotice::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "ChallengeNotice");

	m_TextBox.SetControl(pScene, "cmd_ST_Nt");
}

void	UIHudNotice::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if( m_TextShow && !m_TextBox.OnUpdate(rDeltaSeconds) )
	{
		m_TextShow = false;
		iHudBase::SetVisible(false);
		
		UIHUDIngameChat* pChat = UIHUDIngameChat::instance();
		if(pChat)
			pChat->re_position();

		ShutDownContext::i()->SetAlarmState(m_AlarmState);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLOSE_PLAYTIME_ALARM);
	}
}

void UIHudNotice::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	if( hu_ut::is_stage_mask_training() == false && hu_ut::is_tutorial_mode() == false) return;

	m_AlarmState = ShutDownContext::i()->GetAlarmState();
	ShutDownContext::i()->SetAlarmState(PRINT_NOTICE);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CLOSE_PLAYTIME_ALARM);

	enum { LetterPerSecond = 30, ShowTimeAfterFinish = 2, };

	i3::rc_wstring wstr_text;

	HUD::NoticeMessageType type = static_cast<HUD::NoticeMessageType>(arg1);
	switch( type )
	{
	case HUD::NMT_Challenge:	wstr_text = hu::get_challenge_msg( (HUD::ChallengeMessage)arg2 );	break;
	case HUD::NMT_Tutorial:		wstr_text = hu::get_tutorial_msg(arg2);								break;
	}

	m_TextBox.SetText( wstr_text );
	m_TextBox.Start(LetterPerSecond, true, ShowTimeAfterFinish);

	m_TextShow = true;

	iHudBase::SetVisible(true);

	re_position();
}

/*********************************/
void UIHudNotice::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetEnable(false);
}

void UIHudNotice::event_first_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetEnable(false);
	if( hu_ut::is_stage_mask_training() == true )
	{
		iHudBase::SetEnable(true);
		SetParameter(HUD::NMT_Challenge, HUD::CHM_BATTLE_START);
	}
	else if( hu_ut::is_tutorial_mode() == true )
	{
		iHudBase::SetEnable(true);
		SetParameter(HUD::NMT_Tutorial, 1);
	}
}

void UIHudNotice::event_death_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	INT32 trainingTeamKill = AIModeContext::i()->KillCount;

	bool apply =false;
	HUD::ChallengeMessage msg = HUD::CHM_NONE;

	switch(trainingTeamKill)
	{
	case 20:	apply = true; msg = HUD::CHM_20_KILL;	break;
	case 40:	apply = true; msg = HUD::CHM_40_KILL;	break;
	case 60:	apply = true; msg = HUD::CHM_60_KILL;	break;
	case 80:	apply = true; msg = HUD::CHM_80_KILL;	break;
	case 100:	apply = true; msg = HUD::CHM_100_KILL;	break;

	case 120:	apply = true; msg = HUD::CHM_120_KILL;	break;
	case 140:	apply = true; msg = HUD::CHM_140_KILL;	break;
	case 160:	apply = true; msg = HUD::CHM_160_KILL;	break;
	case 180:	apply = true; msg = HUD::CHM_180_KILL;	break;
	case 200:	apply = true; msg = HUD::CHM_200_KILL;	break;
	}

	if(apply)
	{
		if (10 != AIModeContext::i()->CurrentLevel)
		{
			SetParameter(HUD::NMT_Challenge, msg);
		}

		if( iHudBase::IsVisible() )
		{
			if( CHud::i())
				CHud::i()->AddTrainingKills(trainingTeamKill);
		}
	}
}

void UIHudNotice::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable( !hu_ut::is_domination_mode() );
}