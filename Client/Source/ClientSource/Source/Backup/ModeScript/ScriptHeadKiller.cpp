#include "pch.h"
#include "Script.h"
#include "ScriptHeadKiller.h"

#include "LuaAPIObject.h"
#include "LuaAPIUI.h"

CScriptHeadKiller::CScriptHeadKiller(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage) : 
CScript(pManager, hL, pStage)
{
}

bool CScriptHeadKiller::Init()
{
	LuaAPIObject::Init(m_hL);
	LuaAPIUI::Init(m_hL);

	return true;
}