#include "pch.h"
#include "PBConsoleManager.h"
#include "i3Framework/i3UI/i3UIConsoleContext.h"
#include "../UserInfoContext.h"
#include "../../Console/PBConsoleClientCtx.h"

//AllUseCmd

//InGameCmd
#include "Command/InGameCmd/PBCmd_InfiniteGunBullet.h"
#include "Command/InGameCmd/PBCmd_SetGunBullet.h"
#include "Command/IngameCmd/PBCmd_PenetrateWall.h"
#include "Command/IngameCmd/PBCmd_ViewBone.h"
#include "Command/IngameCmd/PBCmd_HUD.h"

//OutGameCmd
#include "./Command/OutGameCmd/PBCmd_UseItem.h"

#include "i3Base/string/compare/generic_compare.h"
//
// PBConsoleManager
//

PBConsoleManager* PBConsoleManager :: m_pInstance = nullptr;
PBConsoleManager::PBConsoleManager(void)
{
	m_dispCtx = nullptr;

	// 명령목록 추가 -----------------------------------------------------------
	// Item 사용인증 커맨드  
	AddCommand( "/RNDBOX", new PBCmd_UseItem);
	
	// 무한탄창 커맨드
	AddCommand( "/RAMBO", new PBCmd_InfiniteGunBullet);
	AddCommand( "/GRENADE", new PBCmd_InfiniteGrenade);

	// 탄창 총알수 셋팅 커맨드
	AddCommand( "/GUNBULLET", new PBCmd_SetGunBullet);

	AddCommand( "/WALLPASS", new PBCmd_PenetrateWall);

	AddCommand( "/VIEWBONE", new PBCmd_ViewBone);

	AddCommand( "/POS", new PBCmd_ViewPosition);
}

PBConsoleManager::~PBConsoleManager(void)
{
	i3::cu::for_each_delete(m_vCmdList);
}

void PBConsoleManager::AddCommand( const char * pszName, PBConsoleCmd * pCmd)
{
	PB_CCmdInfo* pCommand = new PB_CCmdInfo();
	I3ASSERT( pszName != nullptr);
	pCommand->CmdName = pszName;
	pCommand->vCmdElementList.push_back(pCmd); 
	m_vCmdList.push_back(pCommand);
}

CMD_RESULT_FLAG PBConsoleManager :: RunCommand(REAL32 DeltaSec)
{
#if !defined(BUILD_RELEASE_QA_VERSION) && !defined(DEBUG)
	if( !UserInfoContext::i()->CheckSpecialAbility(SA_CHEAT) )
	{
		return CMD_RESULT_FAIL;
	}
#endif

	for(std::size_t i = 0;i < m_vCmdRunList.size(); ++i)
	{
		bool CommandEndFlag = true;
		for(std::size_t j = 0;j < m_vCmdRunList[i]->vCmdElementList.size(); ++j)
		{
			int LoopCount = m_vCmdRunList[i]->vCmdElementList[j]->getLoopCount();
			if(LoopCount > 0)
			{
				CommandEndFlag = false;
				CMD_RESULT_FLAG ResultFlag = m_vCmdRunList[i]->vCmdElementList[j]->RunCommand(DeltaSec);
				if(ResultFlag == CMD_RESULT_OK) // 완료시 삭제
				{
					m_vCmdRunList[i]->vCmdElementList[j]->setLoopCount(LoopCount-1);
				}
				else if(ResultFlag == CMD_RESULT_FAIL) // 실패 예외처리
				{
					CommandEndFlag = true;
					break;
				}
				//CMD_RESULT_LOOP 의 경우는 계속 반복해야할 명령으로 인식
				
			}
		}
		if(CommandEndFlag)
		{
			m_vCmdRunList.erase((m_vCmdRunList.begin()+i));
			--i;
		}
		
		
	}

	return CMD_RESULT_OK;
}

bool PBConsoleManager::ExecuteCommand(i3::string &CmdName, i3::vector<i3::string> &params, i3UICSCmdDispCtx* dispCtx)
{
	I3ASSERT(dispCtx);

	struct DispatchCtxSetter
	{
		DispatchCtxSetter(PBConsoleManager* man, i3UICSCmdDispCtx* dCtx)
		{
			m_man = man; m_man->SetDispatchCtx(dCtx);
		}
		~DispatchCtxSetter() { m_man->SetDispatchCtx(nullptr); }
		PBConsoleManager* m_man;

	} dispatchCtxSetter(this, dispCtx);

	for( UINT32 i = 0; i < m_vCmdList.size(); ++i)
	{
		PB_CCmdInfo * pInfo = m_vCmdList.at(i);

		if( i3::generic_compare( pInfo->CmdName, CmdName) == 0)
		{
			for( UINT32 j = 0; j < pInfo->vCmdElementList.size(); j++)
			{
				PBConsoleCmd * pCmd = pInfo->vCmdElementList.at(j);
				CMD_RESULT_FLAG ResultFlag = pCmd->ExecuteCommand( params);
				if( ResultFlag == CMD_RESULT_OK )
				{
					//명령 입력 성공 수행에 리스트 추가
					m_vCmdRunList.push_back( pInfo);
					pCmd->setLoopCount( 1);
					return true;
				}
			}
		}
	}

	return false;
}

void PBConsoleManager::AddLog( const wchar_t* fmt, ...)
{
	wchar_t buf[1024] = { 0 };

	va_list arg; va_start(arg, fmt);

	vswprintf_s(buf, 1024, fmt, arg); va_end(arg);

	AddLogStr( buf);
}

void PBConsoleManager::AddLogStr( const wchar_t * str)
{
	i3UIManager::CLog(str);
}

PB_CCmdInfo* PBConsoleManager::getCCmdInfo(string CmdName)
{
	for(std::size_t i = 0;i < m_vCmdList.size();++i)
	{
		if(!strcmp(m_vCmdList[i]->CmdName.c_str(), CmdName.c_str()))
			return m_vCmdList[i];
	}
	return nullptr;
}
