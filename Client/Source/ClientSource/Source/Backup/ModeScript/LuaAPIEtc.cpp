#include "pch.h"
#include "LuaAPIEtc.h"

#include "ScriptDef.h"

namespace LuaAPIEtc
{
	void Init(lua_State* L)
	{
		lua_tinker::def(L, "OnTimeOut", OnTimeOut);

		lua_tinker::def(L, "addKillScore", addKillScore);
		lua_tinker::def(L, "getKillScore", getKillScore);
		lua_tinker::def(L, "killRoundTimer", killRoundTimer);
		lua_tinker::def(L, "killTimer", killTimer);
		lua_tinker::def(L, "setRespawnDelayTime", setRespawnDelayTime);
		lua_tinker::def(L, "setRoundTimer", setRoundTimer);
		lua_tinker::def(L, "setTimer", setTimer);
	}

	// Return : ¾øÀ½
	// To do
	LVOID OnTimeOut( TIMER timerIdx )
	{
		return LNULL;
	}

	LVOID addKillScore( TEAM team, INT32 score )
	{
		return NULL;
	}
	INT32 getKillScore( TEAM team )
	{
		return 0;
	}
	LVOID killRoundTimer( bool ShowUI )
	{
		return NULL;
	}
	LVOID killTimer( INT32 idTimer, bool showUI )
	{
		return NULL;
	}
	LVOID setRespawnDelayTime( INT32 secs )
	{
		return NULL;
	}
	LVOID setRoundTimer( INT32 secs )
	{
		return NULL;
	}
	LVOID setTimer( INT32 idTimer, INT32 secs )
	{
		return NULL;
	}

}

