#include "pch.h"
#include "Script.h"
#include "ScriptTutorial.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptTutorial::CScriptTutorial(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptTutorial::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}