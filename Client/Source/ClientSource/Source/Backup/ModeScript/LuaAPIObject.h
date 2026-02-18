#pragma once

#include "LuaAPIDef.h"

struct lua_State;

namespace LuaAPIObject
{
	void Init(lua_State* L);

	//
	// Event Handler Functions
	// Called : Client, Server
	//
	bool OnDamageObject( HPLAYER player, HOBJECT obj ); // Player가 해당 Object에 Damage를 준 경우 호출된다.
	LVOID OnDestroyObject( HOBJECT obj );				// 특정 Object의 HP가 0이 되어 파괴될 때 한번 호출된다. 파괴는 이미 결정된 사항이며, 번복되지 않는다.

	
	HOBJECT	getObject( const char* name );
	UINT32	getObjectHP( HOBJECT obj );
	const char*	getObjectName( HOBJECT obj );
}