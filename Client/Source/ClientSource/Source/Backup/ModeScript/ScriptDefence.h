#pragma once

#include "Script.h"

class i3Stage;

class CScriptDefence : public CScript
{
public:

	CScriptDefence(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
