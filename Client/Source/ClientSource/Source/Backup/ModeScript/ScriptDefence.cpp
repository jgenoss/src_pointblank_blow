#include "pch.h"
#include "Script.h"
#include "ScriptDefence.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptDefence::CScriptDefence(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptDefence::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}