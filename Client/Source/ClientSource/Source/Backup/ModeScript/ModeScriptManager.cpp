#include "pch.h"
#include "ModeScriptManager.h"

#include "ScriptDef.h"
#include "DomiScript.h"

void	ModeScriptMgr::Set( STAGE_TYPE eStage, i3Stage* pStage, bool bDataReset )
{
	if( m_StageType == eStage )
	{
		if( m_pScript != NULL ) m_pScript->Reset( pStage, bDataReset );
		return;
	}

	LUA_RETURN;

	ModeScriptMgr::Reset(); 

	// 현재 제압이 아니라면 스크립트는 쓰지 않도록 조절...문제가 생기면 생긴곳에서 모드분기처리합니다..
	if (eStage == STAGE_TYPE_DOMINATION)
	{
		// 생성 & 초기화..
		m_pScript = new DomiScript(pStage);
		// 초기화
		if(!m_pScript->Open(m_szFile[eStage]))
		{
			I3ASSERT(!"m_pScript->Init() 초기화 실패");
			ModeScriptMgr::Reset();
		}
	}

	m_StageType	= eStage;
}

void	ModeScriptMgr::Reset()
{
	delete m_pScript;		m_pScript = NULL;
}

void ModeScriptMgr::Init()
{
	LUA_RETURN;

	// 파일 열기
	TiXmlDocument doc("Script\\ModeTable.xml");

	bool rt = doc.LoadFile();
	I3ASSERT(rt);

	// 공지 요소 찾기
	TiXmlElement* pRoot = doc.FirstChildElement("MODE_TABLE");
	I3ASSERT(pRoot);


	LPCSTR szNode[STAGE_TYPE_MAX] = {
		"None",
		"DeathMatch",
		"Bomb",
		"Destroy",
		"Annihilation",
		"Defence",
		"SuddenDeath",
		"Escape",
		"HeadHunter",
		"HeadKiller",
		"Tutorial",
		"Domination",
		"CrossCount"
	};

	TiXmlElement* pNode;
	for (int i = STAGE_TYPE_NA ; i < STAGE_TYPE_MAX ; ++i)
	{
		pNode = pRoot->FirstChildElement(szNode[i]);
		assert(pNode);
		i3String::NCopy(m_szFile[i], pNode->Attribute("File"), MAX_STRING_COUNT - 1);
	}
}

ModeScriptMgr::ModeScriptMgr() : m_pScript(NULL), m_StageType( STAGE_TYPE_NA )
{
}

ModeScriptMgr::~ModeScriptMgr()
{
	Reset();
}

mode_script_arg&	ModeScriptMgr::call(const char* str_fun) 
{ 
	return m_pScript->call(str_fun); 
}
void			ModeScriptMgr::set_modedata(PROTOCOL p, P_BATTLE_SERVER_SYNC_MODEDATA_ACK* data, INT32 num)
{
	I3ASSERT( m_pScript != NULL);

	if( m_pScript != NULL)
		m_pScript->set_modedata(p, data, num);
}
