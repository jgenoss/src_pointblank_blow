#pragma once

#include "Script.h"

class i3Stage;

class CScriptBomb : public CScript
{
public:

	CScriptBomb(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
