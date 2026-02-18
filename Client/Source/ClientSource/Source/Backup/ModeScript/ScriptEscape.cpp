#include "pch.h"
#include "Script.h"
#include "ScriptEscape.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptEscape::CScriptEscape(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptEscape::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}