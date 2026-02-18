#ifndef _LUAMANAGER_H_
#define _LUAMANAGER_H_

class IModeScript;
struct lua_State;
struct mode_script_arg;

#include "ScriptDef.h"
#include "tinst.h"

// 클라이언트에서 하나의 스크립트만 호출한다는 가정하에 구조를 잡았습니다.
// 혹시, 여러개의 모드스크립트를 생성시에 구조변경이 이루어져야 합니다.  루아에 관련된 설명은 배제하겠습니다. - 최형기

//
// 인터페이스정리했습니다. - 수빈.
//

class ModeScriptMgr
{
public:
	void				Init();
	void				Set( STAGE_TYPE eStage, i3Stage* pStage, bool bDataReset );
	void				Reset();
	IModeScript*		GetScript() { return m_pScript; }
	const IModeScript*	GetScript() const { return m_pScript; }

	void				set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, INT32 num);
	mode_script_arg&	call(const char* str_fun); // { return m_pScript->call(str_fun); }

private:
	ModeScriptMgr();
	virtual ~ModeScriptMgr();
	friend tinst_creator<ModeScriptMgr>;
	char m_szFile[STAGE_TYPE_MAX][MAX_STRING_COUNT];

	STAGE_TYPE		m_StageType;
	IModeScript*	m_pScript;
};

tinst<ModeScriptMgr>;

inline 
mode_script_arg&	script_call(const char* str_fun)
{
	return tinst<ModeScriptMgr>()->call(str_fun);
}

inline
void				set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, INT32 num)
{
	return tinst<ModeScriptMgr>()->set_modedata(p, data, num);
}

#endif