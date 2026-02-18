#include "pch.h"
#include "BattleHud_Helicopter.h"

#include "BattleHud.h"
#include "UI/UIUtil.h"
#include "ConfigRenderBattle.h"
#include "ChannelContext.h"

I3_CLASS_INSTANCE( CHud_Helicopter);

namespace inner { CHud_Helicopter* this_ptr = 0; }
CHud_Helicopter* CHud_Helicopter::instance() { return inner::this_ptr; }

CHud_Helicopter::CHud_Helicopter(void)
{
	inner::this_ptr = this;

	m_pHud = nullptr;
}

CHud_Helicopter::~CHud_Helicopter(void)
{
	inner::this_ptr = 0;
}

void CHud_Helicopter::Create(CHud * pHud)
{
	m_pHud = pHud;
}

void CHud_Helicopter::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate( rDeltaSeconds );
}

void CHud_Helicopter::SetRidingMark(EUH60_CONTROL_MARK eMark)
{
//	char message[MAX_STRING_COUNT] = {0, };
	i3::stack_wstring	wstrMessage;

	GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();

	if(eMark == ECM_FLY_AND_CONTROL_M197)
	{
		i3::sprintf(wstrMessage, L"{col:240, 210, 110, 255}[%s]{/col}%s", 
		GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_SPECIALACTION]),
		GAME_RCSTRING("STR_TBL_INGAME_HUD_HELI_AND_M197"));/*키를 누르면 비행 및 M197 사용이 가능합니다.*/				  
		m_pHud->SetMissionMessageString(wstrMessage);
	}
	else if(eMark == ECM_CONTROL_M197)
	{
		i3::sprintf(wstrMessage, L"{col:240, 210, 110, 255}[%s]{/col}%s", 
				GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_SPECIALACTION]),
				GAME_RCSTRING("STR_TBL_INGAME_HUD_ACTIVE_M197"));/*키를 누르면 M197 사용이 가능합니다.*/				  
		m_pHud->SetMissionMessageString(wstrMessage);
	}
	else if(eMark == ECM_UNCONTROL_M197)
	{
		i3::sprintf(wstrMessage, L"{col:240, 210, 110, 255}[%s]{/col}%s", 
				GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_SPECIALACTION]),
				GAME_RCSTRING("STR_TBL_INGAME_HUD_DEACTIVE_M197"));/*키를 누르면 M197 사용을 해제 합니다.*/				  
		m_pHud->SetMissionMessageString(wstrMessage);
	}
	else if(eMark == ECM_NONE)
	{
		m_pHud->SetEnableMissionMessage( false);
	}
	else
	{
		I3ASSERT_0;
	}
}

void CHud_Helicopter::UpdateWarningNotice()
{
	i3::stack_wstring wstrTemp;
	i3::sprintf(wstrTemp, L"\"%s\"", GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_WARNING_HELI"));
	m_pHud->SetMissionNoticeString(wstrTemp, 3.f, 0, 45);
}
