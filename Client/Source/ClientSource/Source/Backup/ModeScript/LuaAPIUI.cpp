#include "pch.h"

#include "ScriptDef.h"
#include "LuaAPIUI.h"

namespace LuaAPIUI
{
	void Init(lua_State* L)
	{
		lua_tinker::def(L, "hideKillScoreUI", hideKillScoreUI);
		lua_tinker::def(L, "hideTeamScoreUI", hideTeamScoreUI);
		lua_tinker::def(L, "mapHighlight", mapHighlight);
		lua_tinker::def(L, "msgChat", msgChat);
		lua_tinker::def(L, "msgHelp", msgHelp);
		lua_tinker::def(L, "msgPopup", msgPopup);
		lua_tinker::def(L, "showKillScoreUI", showKillScoreUI);
		lua_tinker::def(L, "showTeamScoreUI", showTeamScoreUI);
	}

	LVOID	hideKillScoreUI()
	{
		return LNULL;
	}
	LVOID	hideTeamScoreUI()
	{
		return LNULL;
	}
	LVOID	mapHighlight( PVEC3D pos )
	{
		return LNULL;
	}
	LVOID	msgChat( const char* msg )
	{
		return LNULL;
	}
	LVOID	msgHelp( const char* msg )
	{
		return LNULL;
	}
	LVOID	msgPopup( const char* msg )
	{
		return LNULL;
	}
	LVOID	showKillScoreUI()
	{
		return LNULL;
	}
	LVOID	showTeamScoreUI()
	{
		return LNULL;
	}

}