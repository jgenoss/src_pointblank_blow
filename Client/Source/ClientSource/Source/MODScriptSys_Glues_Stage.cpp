#include "pch.h"
#include "MODScriptSys_Glues.h"

extern "C"
{

int MODScript::finishRound( lua_State * pState)
{
	return 0;
}

int MODScript::getConfigRoundCount( lua_State * pState)
{
	return 0;
}

int MODScript::getConfigTime( lua_State * pState)
{
	return 0;
}

int MODScript::setMODName( lua_State * pState)
{
	return 0;
}

int MODScript::setPlayerCount( lua_State * pState)
{
	return 0;
}

int MODScript::setRankCondition( lua_State * pState)
{
	return 0;
}

int MODScript::setRoundCount( lua_State * pState)
{
	return 0;
}

}; // extern "C"