#pragma once

#include "Script.h"

class i3Stage;

class CScriptHeadHunter : public CScript
{
public:

	CScriptHeadHunter(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
