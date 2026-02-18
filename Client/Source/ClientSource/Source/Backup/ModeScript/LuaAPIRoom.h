#pragma once

#include "LuaAPIDef.h"

namespace LuaAPIRoom
{
	// 방이 방장(Room Master)에 의해 만들어져, Stage를 시작하고, 
	// 종료하는 과정에서 발생하는 여러 가지 시점에서 호출되는 Event Handler 함수들이다.
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID OnDetachPlayer( HPLAYER Player );	// Player가 배틀에서 나갈때마다 한번씩 호출되며, 나갈때마다 남은 플레이어로 배틀 또는 라운드를 지속 가능한지 판단한다.
	LVOID OnJoinPlayer( HPLAYER player );	// Player가 배틀에 들어올때마다 한번씩 호출되며, 유저가 들어올때 배틀 시작 가능한지를 판단하여 배틀을 시작한다.
	LVOID OnStageConfig();					// Stage에 대한 설정을 한다. 이는 일반적으로 방이 생성되어 Stage를 선택할 때 Stage가 변경될 때마다 호출된다.
};
