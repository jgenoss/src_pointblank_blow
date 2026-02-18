#pragma once

#include "Script.h"

class i3Stage;

class CScriptHeadKiller : public CScript
{
public:

	CScriptHeadKiller(CLuaScriptManager* pManager, lua_State* hL, i3Stage* pStage);

	virtual bool Init();

};
