#pragma once

#include "LuaAPIDef.h"

extern CSampleClass g_kSample; 

namespace LuaAPIStage
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	LVOID OnEndRound();		// Round가 종료될 때 한번 호출된다. 이 함수는 Round제가 아닌, 단체전의 경우에도 한번 호출된다.
	LVOID OnEndStage();		// Stage가 종료될 때 한번 호출된다.
	LVOID OnPreStartRound();// Round가 시작되기 전 한번 호출된다. 이 함수는 Round제가 아닌, 단체전의 경우에도 한번 호출된다.
	LVOID OnStartRound();	// Round를 시작할 때 한번 호출된다. 이 함수는 Round제가 아닌, 단체전의 경우에도 한번 호출된다.
	LVOID OnStartStage();	// Stage가 시작될 때 한번 호출된다.

};
