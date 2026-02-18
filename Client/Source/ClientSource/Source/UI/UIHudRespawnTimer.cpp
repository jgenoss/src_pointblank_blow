#include "pch.h"
#include "UIHudRespawnTimer.h"

#include "UIHudDefine.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIUtil.h"
#include "UI/UIMath.h"
#include "ui/Batmanager.h"
#include "LogRespawn.h"
#include "BattleHud.h"
#include "./hud/UIHUD_Domination_Manager.h"
#include "UIHudUtil.h"
#include "UIHudManager.h"
#include "MyRoomInfoContext.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "../StageBattle/UserContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

UIHudRespawnTimer::UIHudRespawnTimer(UIHudManager* p) : iHudBase(p)
{
}

UIHudRespawnTimer::~UIHudRespawnTimer()
{
}
 
/*********************************/
void UIHudRespawnTimer::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	AddFrameWnd(pScene, "i3UIFrameWnd0");	//text
	AddFrameWnd(pScene, "i3UIFrameWnd");	//timer

	m_pText = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText");
	m_pProgressBar = FindControl<i3UIProgressBar*>(pScene, "i3UIProgressBar");
	m_pRespawnTimeText = FindControl<i3UIStaticText*>(pScene, "i3UIStaticText0");

	m_working_respawn = false;
	m_block_send = true;
	m_complete_respawn = false;
}

namespace
{
	bool g_hold_respawn = false;
}

void	UIHudRespawnTimer::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if(m_enable_time)
	{
		REAL32 factor = uiMath::inverse_interpolate(m_curr_time, 0.f, m_targer_time);
		factor = uiMath::limited(factor, 0.f, 1.f);

		m_pProgressBar->setProgress(factor);

		if( BaTMaN::use_batman )
			rDeltaSeconds *= BaTMaN::magnification_respawn;

		//log_respawn::i()->AddRespawnTime(rDeltaSeconds);

		m_curr_time += rDeltaSeconds;
		if( m_curr_time >= m_targer_time) m_curr_time = m_targer_time;

		if( g_hold_respawn)
		{
			m_curr_time -= rDeltaSeconds;
			m_curr_time = MIN( m_curr_time, 0.f);
		}

#ifndef NC_BUILD
		if( GameUI::IsKeyboardPressed(I3I_KEY_LCTRL) && GameUI::IsKeyboardPressed(I3I_KEY_LALT) )
		{
			if(	GameUI::IsKeyboardStroked(I3I_KEY_LEFT) )
			{
				m_curr_time = 0.f;
			}
			else if( GameUI::IsKeyboardStroked(I3I_KEY_RIGHT) )
			{
				factor = 1.f;
			}
			else if( GameUI::IsKeyboardStroked(I3I_KEY_DOWN) )
			{
				g_hold_respawn = !g_hold_respawn;
			}
		}
#endif

		if( factor == 1.f)
		{
			m_complete_respawn = true;
			m_enable_time = false;
		}
	}

	if( m_working_respawn && m_complete_respawn == true && m_block_send == false )
	{
		iHudBase::SetVisible(false);
		GameEventSender::i()->SetEventOnce( EVENT_RESPAWN_BATTLE);
		
		m_block_send = true;
		m_working_respawn = false;
	}
}

/*********************************/
void UIHudRespawnTimer::init_text()
{
	i3_prof_func();

	REAL32 InvTime = 0.0f;
	
	if(s_pMyChara != nullptr)
		InvTime = s_pMyChara->getAbility( CHARA::eABILITY_INVINCIBLE_TIME);
	else
		InvTime = 4.0f;	// 디폴트 4초 무적

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) == false )
	{
		i3::rc_wstring wstr_notice;

		GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();		

		i3::sprintf(wstr_notice, GAME_RCSTRING("STR_TBL_BATTLEGUI_NOTICE_RESPAWN"),
			(INT32)InvTime, GameUI::GetKeyboardString(&pKeyMap[GAME_KEY_MAP_CHANGEWEAPON]));
	
		m_pText->SetText( wstr_notice );
	}

	m_pRespawnTimeText->SetText( hu::get_text_using_respawn_item() );
}

/*********************************/
void UIHudRespawnTimer::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	if( hu_ut::is_test_changing_hud() == false )
		iHudBase::SetEnable(false);
}

void UIHudRespawnTimer::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	if( hu_ut::is_test_changing_hud() == false )
		iHudBase::SetEnable(false);
}

namespace
{
	bool is_gm_observer_mode()
	{
#if defined( DEF_OBSERVER_MODE)
		if( BattleSlotContext::i()->isObserverMe())
#else
		if(UserInfoContext::i()->IsGM_Observer())
#endif
			return true;

		return false;
	}
}

void UIHudRespawnTimer::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	switch( arg1 )
	{
	case HUD::RT_START:
		{
			if( m_parent->is_block_msg_by_visible == true )
			{ GameEventSender::i()->SetEventOnce( EVENT_RESPAWN_BATTLE); return; } //바로 리스폰 한다.

			if( is_gm_observer_mode() )	return;

			INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
			bool bMyDeath = UserContext::i()->Death[mySlotIdx];
			bool show_mode = !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) && !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION);

			if(show_mode && bMyDeath)
			{
				m_working_respawn = true;
				m_complete_respawn = false;

				m_block_send = UIBattleFrame::i()->IsOpenPopup( cc::GetStyle());

				m_enable_time = true;
				m_curr_time = 0.f;
				m_targer_time = MyRoomInfoContext::i()->GetRespawnTime( mySlotIdx );

				init_text();

				iHudBase::SetEnable(true);
				if(hu_ut::is_tutorial_mode() == true)
					iHudBase::SetVisibleFrame(false, 0);	//Test Frame 튜토리얼 모드에서 invisible 처리합니다.
				
				if(CHud::i())
					CHud::i()->ResetKillMessage();
			}
		}		
		break;
	case HUD::RT_SEND_BLOCK:	m_block_send = true; break;
	case HUD::RT_SEND_UNBLOCK: m_block_send = false; break;
	case HUD::RT_HOLD: g_hold_respawn = true; break;

	case HUD::RT_RELEASE_HOLD:
		g_hold_respawn = false;
		m_curr_time = 0.f; //다시 처음 부터.
		break;

	case HUD::RT_IMMEDIATE:
		g_hold_respawn = false;
		m_curr_time = m_targer_time;
		break;

	case HUD::RT_RESET:
		iHudBase::SetVisible(false);
		m_block_send = true;
		m_working_respawn = false;
		break;
	}
}