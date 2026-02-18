#pragma once

#include "Script.h"

class i3Stage;

class CScriptDestroy : public CScript
{
public:

	CScriptDestroy(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
