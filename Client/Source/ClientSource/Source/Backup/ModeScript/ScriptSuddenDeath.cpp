#include "pch.h"
#include "Script.h"
#include "ScriptSuddenDeath.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptSuddenDeath::CScriptSuddenDeath(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptSuddenDeath::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}