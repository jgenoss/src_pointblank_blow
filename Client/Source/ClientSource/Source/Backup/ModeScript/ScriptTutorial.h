#pragma once

#include "Script.h"

class i3Stage;

class CScriptTutorial : public CScript
{
public:

	CScriptTutorial(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
