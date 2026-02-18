#include "pch.h"
#include "Script.h"
#include "ScriptHeadHunter.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptHeadHunter::CScriptHeadHunter(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptHeadHunter::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}