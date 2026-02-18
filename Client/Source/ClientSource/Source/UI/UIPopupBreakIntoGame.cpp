#include "pch.h"

#include "UIPopupBreakIntoGame.h"

#include "UIMainFrame.h"
#include "UIPhaseReadyRoom.h"
#include "BreakIntoGame.h"


I3_CLASS_INSTANCE( UIPopupBreakIntoGame);//, UITabBase);

namespace inner
{
	enum	{ OK, CANCEL};
	UIPopupBreakIntoGame* g_this = 0;
	UIPhaseBase* pReadyRoom =0;
}

extern "C" 
{
	int break_into_game_click( LuaState * L)
	{
		INT32 value = i3Lua::GetIntegerArg( L, 1);
		inner::g_this->clicked( value );
		return 0;
	}
}

LuaFuncReg break_into_game_glue[] =
{
	{"Click",		break_into_game_click},
	{nullptr,				nullptr}
};

/************************//************************/
void UIPopupBreakIntoGame::clicked(int idx) const
{
	switch( idx )
	{
	case inner::OK:
		if( bing::is_break_into_game() == true ) //게임 난입 시도할 순간에 게임 방 상태가 변경될 수 있기 때문에.
		{
			inner::pReadyRoom->JoinGame();
		}
		break;
	case inner::CANCEL:
		break;
	}

	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	pFrame->ClosePopup(UPW_BREAK_INTO_GAME);
}

/************************//************************/
UIPopupBreakIntoGame::UIPopupBreakIntoGame()
{
	inner::g_this = this;
}

UIPopupBreakIntoGame::~UIPopupBreakIntoGame()
{
}


/************************//************************/
/*virtual*/ void UIPopupBreakIntoGame::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Breakinto.i3UIs", break_into_game_glue);
}

bool UIPopupBreakIntoGame::OnEntranceStart( void * pArg1, void * pArg2)
{
	bool resPopup = UIPopupBase::OnEntranceStart( pArg1, pArg2 );
	I3ASSERT(resPopup);

	inner::pReadyRoom = static_cast<UIPhaseBase*>(pArg1);

	GetSlide()->ForceEnd();

	return true;
}

void UIPopupBreakIntoGame::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	INT32 red_score, blue_score;
	i3::vector< i3::wstring > list;
	INT32	ScoreType = bing::get_room_info_string(list, red_score, blue_score);

	switch(ScoreType)
	{
	case 1:
		{
			LuaState * L = _CallLuaFunction( "SetKillCountMode");
			i3Lua::PushStringUTF16To8( L, list[0].c_str(), list[0].size() );
			i3Lua::PushStringUTF16To8( L, list[1].c_str(), list[1].size() );
			i3Lua::PushInteger( L, red_score );
			i3Lua::PushInteger( L, blue_score );
			UILua::EndLuaFunction(L , 4);
		}
		break;
	case 2:
		{
			LuaState * L = _CallLuaFunction( "SetRoundCountMode");
			i3Lua::PushStringUTF16To8( L, list[0].c_str(), list[0].size() );
			i3Lua::PushInteger( L, red_score );
			i3Lua::PushInteger( L, blue_score );
			UILua::EndLuaFunction(L , 3);
		}
		break;
	case 3:
		{
			LuaState * L = _CallLuaFunction( "SetDifficultMode");
			i3Lua::PushStringUTF16To8( L, list[0].c_str(), list[0].size() );
			i3Lua::PushStringUTF16To8( L, list[1].c_str(), list[1].size() );
			i3Lua::PushStringUTF16To8( L, list[2].c_str(), list[2].size() );
			UILua::EndLuaFunction(L , 3);
		}
		break;
	};
}

bool UIPopupBreakIntoGame::OnKey_Enter( void)
{
	clicked(inner::OK);

	return false;
}