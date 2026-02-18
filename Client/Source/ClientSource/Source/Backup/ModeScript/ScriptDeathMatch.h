#pragma once

#include "Script.h"

class i3Stage;

class CScriptDeathMatch : public CScript
{
public:

	CScriptDeathMatch(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
