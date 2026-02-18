#pragma once

#include "Script.h"

class i3Stage;

class CScriptSuddenDeath : public CScript
{
public:

	CScriptSuddenDeath(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
