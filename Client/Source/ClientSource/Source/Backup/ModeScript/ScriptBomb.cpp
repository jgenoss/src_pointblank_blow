#include "pch.h"
#include "Script.h"
#include "ScriptBomb.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptBomb::CScriptBomb(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptBomb::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}