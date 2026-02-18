#include "pch.h"
#include "UIHudMadnessNotice.h"

#include "UIHudDefine.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIUtil.h"
#include "UI/UIMath.h"
#include "ui/Batmanager.h"
#include "UIHudUtil.h"
#include "UIHudManager.h"
#include "BattleHud.h"
#include "MyRoomInfoContext.h"
#include "i3Base/profile/profile.h"

UIHudMadnessNotice::UIHudMadnessNotice(UIHudManager* p) : iHudBase(p)
{
}

UIHudMadnessNotice::~UIHudMadnessNotice()
{
}
 
/*********************************/
void UIHudMadnessNotice::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	AddFrameWnd(pScene, "Medness");

	m_pAlarmImage	=		FindControl<i3UIButtonImageSet*>(pScene, "BtnIS_Medness");
	m_pTextBG		=		FindControl<i3UIImageBox*>(pScene, "Img_Medness");
	m_pText			=		FindControl<i3UIStaticText*>(pScene, "St_Medness");
	
	i3::wstring strOut;
	i3::sprintf(strOut, GAME_RCSTRING("STR_TBL_GUI_INGAME_MADNESS_WARNING_LV"), 10);
	m_pText->SetText( strOut );

	m_curr_state	 = HUD::MN_FINISH;
	m_curr_time		 = 0.0f;
	m_pre_time		 = 0.0f;
	m_curr_spriteIdx = 0;
}

void UIHudMadnessNotice::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	switch(m_curr_state)
	{
	case HUD::MN_START:
		m_curr_state =  HUD::MN_UPDATING;
		break;
	case HUD::MN_UPDATING:
		{
			m_curr_time += rDeltaSeconds;

			if( m_curr_time <= HUD_MADNESSARAM_SPRITE_TIME )
			{			
				if( m_curr_time - m_pre_time > 0.1f )
				{
					m_curr_spriteIdx++;
					if( m_curr_spriteIdx > 2 )
						m_curr_spriteIdx = 0;

					m_pre_time  = m_curr_time;
					m_pAlarmImage->SetShapeIdx(m_curr_spriteIdx);
				}
			}
			else
			{
				m_curr_state = HUD::MN_STOP;
				m_curr_time = 0.0f;
				m_pre_time  = 0.0f;
				m_curr_spriteIdx = 0;
			}
		}
		break;
	case HUD::MN_STOP:
		m_curr_state = HUD::MN_FINISH;
		iHudBase::SetEnable(false);
		break;
	}
}

/*********************************/
void UIHudMadnessNotice::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	if( hu_ut::is_test_changing_hud() == false )
		iHudBase::SetEnable(false);
}

void UIHudMadnessNotice::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	//필요는 없음..
	if( hu_ut::is_test_changing_hud() == false )
		iHudBase::SetEnable(false);
}

void UIHudMadnessNotice::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	//START만 쓰면 됨.
	if( arg1 != HUD::MN_START )
		return;
	
	m_curr_state = (INT)arg1;
	iHudBase::SetEnable(true);
}