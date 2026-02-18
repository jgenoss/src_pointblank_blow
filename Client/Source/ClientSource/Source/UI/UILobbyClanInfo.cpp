#include "pch.h"
#include "UILobbyClanInfo.h"

#include "UIMainFrame.h"
#include "ClanGameContext.h"
#include "UIUtil.h"

extern "C" {

}

LuaFuncReg UILobbyClanInfo_Glue[] = {
	{ nullptr,						nullptr}
};


I3_CLASS_INSTANCE( UILobbyClanInfo);//, UIFloatingBase);

UILobbyClanInfo::UILobbyClanInfo()
{
	m_AlignedPos = UIFLOATING_POS_LEFTTOP;
	m_bFixedWindow = true;
}

UILobbyClanInfo::~UILobbyClanInfo()
{

}
/*virtual*/ void UILobbyClanInfo::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Clan/PBRe_Clan_Channel_Info.i3UIs", UILobbyClanInfo_Glue);
}

void	UILobbyClanInfo::OnLoadAllScenes()
{
	UIFloatingBase::OnLoadAllScenes();
	SetLuaClanInfo();

	i3UIFrameWnd* pWnd = static_cast<i3UIFrameWnd*>( GetScene( "PBRe_Clan_Channel_Info")->FindChildByName("FrameWnd_MapInfo") );
	if (pWnd != nullptr)
	{
		m_slide.AddFrameWnd(pWnd, UISlide::R_TO_L);
	}
}

/*virtual*/ void UILobbyClanInfo::ProcessGameEvent(GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData)
{
	UIFloatingBase::ProcessGameEvent( Event, Arg, UserData);

	switch(Event)
	{
	case EVENT_CLAN_WAR_ALL_MATCHING_INFO :
		{
			//로비 일때만
			if( GetMainFrame()->GetCurrentSubPhaseType() != USP_NONE )
				return;

			if( GetMainFrame()->GetCurrentPhaseType() != UIPHASE_CLANLOBBY )
				return;
			
			SetLuaClanInfo();
		}
		break;
	}
}

void UILobbyClanInfo::SetLuaClanInfo()
{
	const CLAN_ALL_MATCHING_INFO * pInfo = ClanGameContext::i()->getClanAllMatingInfo();

	i3UIScene * pScene = GetScene( "PBRe_Clan_Channel_Info");
	
	LuaState* L = UILua::CallLuaFunction( pScene, "SetData");
	if(L != nullptr)
	{
		// 클랜매치 된 팀
		i3::rc_wstring wstr_complete_team = L"";
		i3::sprintf(wstr_complete_team, L": %d", pInfo->getMatchCompleteTeamCount() );
	
		// 매치 대기중인 팀
		i3::rc_wstring wstr_waiting_team = L"";
		i3::sprintf(wstr_waiting_team, L": %d", pInfo->getMatchWaitTeamCount() );
		
		i3Lua::PushStringUTF16To8(L, wstr_complete_team);	
		i3Lua::PushStringUTF16To8(L, wstr_waiting_team);	
		i3Lua::PushStringUTF16To8(L, L"");	
		UILua::EndLuaFunction(L , 3);
	}	
}