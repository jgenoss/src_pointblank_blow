#pragma once

#include "Script.h"

class i3Stage;

class CScriptEscape : public CScript
{
public:

	CScriptEscape(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
