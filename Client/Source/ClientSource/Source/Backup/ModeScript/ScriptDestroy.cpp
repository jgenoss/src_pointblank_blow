#include "pch.h"
#include "Script.h"
#include "ScriptDestroy.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptDestroy::CScriptDestroy(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptDestroy::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}