#pragma once

#include <Windows.h>
#include <string>

#include "ModeScript.h"

#include "ScriptDef.h"

class ModeScriptMgr;
struct lua_State;

class CBattleScript : public IModeScript
{
public:
	CBattleScript(i3Stage* pStage);
	virtual ~CBattleScript();

public:

	void DestroyModeData();			// 파괴미션
	void DefenseModeData();			// 방어미션
	void DominationModeData(void);

};


