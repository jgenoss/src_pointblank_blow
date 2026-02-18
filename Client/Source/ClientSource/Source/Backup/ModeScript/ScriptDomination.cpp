#include "pch.h"
#include "Script.h"
#include "ScriptDomination.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptDomination::CScriptDomination(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptDomination::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}