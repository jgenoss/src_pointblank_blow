
#include "pch.h"

#include "PBConsoleCommon.h"
#include "CheatKey.h"


// 임시 콘솔커맨드목록입니다. Cheatkey로 구현하기전에 임시적으로 콘솔명령작성시 사용합니다.
static bool ProcessTemporaryConsoleCommand(i3UICSCmdDispCtx* ctx, bool& rst);


static bool PBConsoleCmdDispatch(i3UICSCmdDispCtx* ctx)
{
	bool rstParse, rstExec = false;
	i3::string keyword;	
	i3::vector<i3::string> params;

	char szLabel[1024] = "";
	i3String::WideCharToMultiByte( GetCodePage(), 0, ctx->GetCmdText(), -1, szLabel, _countof(szLabel), nullptr, nullptr);

	i3::string mbcsStr(szLabel);

	bool temporaryResult = false;
	if(ProcessTemporaryConsoleCommand(ctx, temporaryResult))
		return temporaryResult;

	rstParse = Cheatkey::ParseCommand(mbcsStr.c_str(), keyword, params);
	
	if(!rstParse)
	{
		ctx->SetResult(EI3UICSRST_FAIL_EXEC_PARSE_ERR);
		return false;
	}

	i3::shared_ptr<Cheatkey::ICommand> cmd = Cheatkey::CreateCommand(keyword.c_str());

	if( cmd != nullptr)
		rstExec = (*cmd)(params);

	if(!rstExec)
	{
		if( PBConsoleManager::getInstance()->ExecuteCommand( keyword, params, ctx) == false)
		{
			ctx->SetResult(EI3UICSRST_FAIL_EXEC_INVALID_CALL);
			return false;
		}
	}

	ctx->SetResult(EI3UICSRST_OK);	
	return true;
}


// PBConsoleCommon.h파일에서  #define __USE_PBCONSOLE__ 를 제거하면 
// 이파일내에 함수코드들은 빌드후에 실행파일에서 모두 사라진다.
#ifdef __USE_PBCONSOLE__

void PBConsoleCmdGlue_Init() 
{
	i3UIConsoleClientContext::SetPreDispatcher(PBConsoleCmdDispatch);
}

void PBConsoleCmdGlue_Destroy() 
{
	i3UIConsoleClientContext::SetPreDispatcher(nullptr);
}

#endif




//
// 임시콘솔명령구현, CheatKey로 구현되면 삭제바람
//

#define PBCS_TMPCMD(x) (!strnicmp(szLabel, x, strlen(x)))

static bool ProcessTemporaryConsoleCommand(i3UICSCmdDispCtx* ctx, bool& rst)
{
	char szLabel[1024] = "";
	i3String::WideCharToMultiByte(GetCodePage(), 0, ctx->GetCmdText(), -1, szLabel, _countof(szLabel), nullptr, nullptr);
	i3::string mbcsStr(szLabel);

	if(PBCS_TMPCMD("/PhyConnect"))
	{
#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		i3PhysixContext * pCtx = g_pFramework->getPhysixContext();

		if(pCtx && pCtx->getSDK())
			pCtx->getSDK()->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
#endif
		return true;
	}

	return false; // CheatKey콘솔커맨드 수행
}
