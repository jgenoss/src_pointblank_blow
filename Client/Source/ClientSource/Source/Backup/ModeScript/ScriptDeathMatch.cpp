#include "pch.h"
#include "Script.h"
#include "ScriptDeathMatch.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptDeathMatch::CScriptDeathMatch(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptDeathMatch::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}