#include "pch.h"
#include "UIHudLast.h"

#include "UIHudDefine.h"
#include "UIHUDIngameChat.h"
#include "UIHudUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

namespace
{
	HUD::MISSION_MODE g_setting_mission_mode = HUD::MISSION_NONE;
}

UIHudLast::UIHudLast(UIHudManager* p) : iHudBase(p)
{
}

UIHudLast::~UIHudLast()
{
}

/*********************************/
void UIHudLast::ClearAtUnload( i3UIScene * pScene)
{
	i3_prof_func();

	hu_dino::reset_dino_attribute();
}

/*********************************/
void UIHudLast::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	HUD::MISSION_MODE mode = static_cast< HUD::MISSION_MODE >( arg1 );
	if( mode == HUD::MISSION_OBSERVER )
	{
		if( g_setting_mission_mode == mode ) return;
		g_setting_mission_mode = mode;
	}
}

/*********************************/
void UIHudLast::event_first_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	UIHUDIngameChat* pChat = UIHUDIngameChat::instance();
	if(pChat)
		pChat->PrintStartingMessage();
}

void UIHudLast::event_m_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	g_setting_mission_mode = HUD::MISSION_NONE;
	
	UIHUDIngameChat* pChat = UIHUDIngameChat::instance();
	if(pChat)
		pChat->re_position();
}
