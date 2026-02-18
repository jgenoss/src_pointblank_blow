#pragma once


#include "i3Framework/i3UI/i3UIConsoleCmd.h"
#include "Command/PBConsoleCmd.h"


class i3UICSCmdDispCtx;


struct PB_CCmdInfo //하나의 명령이 수행하는데 필요한 단위 데이터 정보
{
	i3::string CmdName;		//명령어 구분 명칭
	i3::vector<PBConsoleCmd*> vCmdElementList; //명령 수행단위 리스트

	PB_CCmdInfo(){}
	~PB_CCmdInfo()
	{
		i3::cu::for_each_delete(vCmdElementList);
	}
};

class PBConsoleManager
{
private:
	i3::vector<PB_CCmdInfo*> m_vCmdList; //총 명령어 목록
	i3::vector<PB_CCmdInfo*> m_vCmdRunList; //수행중인 명령어들

	i3UICSCmdDispCtx*		 m_dispCtx; // ExecuteCommand 실행동안만 유지됩니다.

public:
	PBConsoleManager(void);
	~PBConsoleManager(void);

	void	SetDispatchCtx(i3UICSCmdDispCtx* dispCtx) { m_dispCtx = dispCtx; }

	void	AddCommand( const char * pszName, PBConsoleCmd * pCmd);

	CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec);

	bool		ExecuteCommand( i3::string &CmdName, i3::vector<i3::string> &params, i3UICSCmdDispCtx* dispCtx);

	void		AddLog( const wchar_t* fmt, ...);
	void		AddLogStr( const wchar_t* str);

public:
	PB_CCmdInfo* getCCmdInfo(string CmdName);

public:
	static PBConsoleManager* m_pInstance;
	static void Release()
	{
		if(m_pInstance)
			delete m_pInstance;
		m_pInstance = nullptr;
	}
	static PBConsoleManager* getInstance()
	{
		return m_pInstance;
	}
	static PBConsoleManager* CreateInstance()
	{
		if(!m_pInstance)
			m_pInstance = new PBConsoleManager();
		return m_pInstance;
	}

};

