#include "pch.h"
#include "Script.h"
#include "ScriptAnnihilation.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptAnnihilation::CScriptAnnihilation(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptAnnihilation::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}