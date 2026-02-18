#include "pch.h"
#include "HackShield.h"
#include "StageBattle.h"
#include "GlobalVariables.h"

#if defined (_HACK_SHIELD_)

// HSHIELD_CALL_UPDATER : V3 핵쉴드의 스마트 업데이트 사용 (핵쉴드 엔진 모듈 업데이트함. 업데이트 서버가 구축되어야 함.)



// 개발시 경로 (dll이 다름)
#if defined(I3_DEBUG)

#define		HSHIELD_PATH			"\\HShield_Dev"

//	>>> 디버깅시 주의! 2009.04.30
// EhSvc.dll와 HShield.dat 파일은 디버깅용과 엔드 유저용이 따로 존재하며 스마트 업데이트를 이용하여
// 자동 업데이트를 하면 업데이트 서버의 엔드 유저용이 다운받게되어 초기화(_AhnHS_Initialize)시 
// 디버깅 툴 감지되어 에러값을 리턴하게 되므로 디버깅시에는 스마트 업데이트를 사용하지 말것.
//#define		HSHIELD_CALL_UPDATER

#else

#define		HSHIELD_PATH			"\\HShield"
#define		HSHIELD_CALL_UPDATER

#endif

static TCHAR	g_szHShieldPath[MAX_PATH] = {0,};		//	핵쉴드 파일이 있는 절대 경로

void CbExitGame(void* pThis,INT32 nParam)
{
	// 강제종료..
	g_pFramework->GotoNextStage( "Exit");
}

//	핵쉴드 콜백 처리
int __stdcall _CallbackProc(long lCode, long lParamSize, void* pParam)
{
	char szFullMsg[1024] = "";
	char szMsg[1024] = "";

	switch(lCode)
	{
		case AHNHS_ACTAPC_STATUS_HACKSHIELD_RUNNING:
		{
			//	핵쉴드 정상 작동함..
		}
		break;
		//Engine Callback
		case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			//	"다음 위치에서 해킹툴이 발견되어 프로그램을 종료시켰습니다.\n(%s)\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_GAME_HACK), (char*)pParam);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			
			break;
		}
		//Speed 관련
		case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			if( g_pFramework->GetCurrentStage()->IsExactTypeOf( CStageBattle::GetClassMeta()) )
			{
				CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
				if( pStage->getHud() != NULL) pStage->getHud()->SetActivateHackWarnning();
			}

			//	"스피드핵으로 의심되는 동작이 감지되었습니다. (Code = 0x%X)\r\n".
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_SPEEDHACK), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);*/

			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}
		//디버깅 방지 
		case AHNHS_ACTAPC_DETECT_KDTRACE:	
		case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			//	"프로그램에 대하여 디버깅 시도가 발생하였습니다. (Code = 0x%X)\n프로그램을 종료합니다.\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_KDTRACE), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}
		// 오토 매크로
		case AHNHS_ACTAPC_DETECT_AUTOMACRO:
		{
			//	"매크로 기능으로 의심되는 동작이 감지되었습니다. (Code = 0x%X)\n프로그램을 종료합니다.\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_AUTOMACRO), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}

		// 코드 패치 감지
		case AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS:
		{
			//	"다음 위치에서 메모리 접근이 감지되어 프로그램을 종료시켰습니다.\n(%s)\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_MEM_ACCESS), (char*)pParam);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}

		//그외 해킹 방지 기능 이상 
		case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
		case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
		case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
		case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		case AHNHS_ACTAPC_DETECT_LMP_FAILED:
		case AHNHS_ACTAPC_DETECT_ENGINEFAILED:
		case AHNHS_ACTAPC_DETECT_CODEMISMATCH:
		case AHNHS_ACTAPC_DETECT_PROTECTSCREENFAILED:
		case AHNHS_ACTAPC_DETECT_ABNORMAL_HACKSHIELD_STATUS:
		case AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP:
		case AHNHS_ACTAPC_DETECT_ANTIFREESERVER:
		{
			//	해킹 방어 기능에 이상이 발생하였습니다. (Code = 0x%X)\n프로그램을 종료합니다.\r\n"
			/*sprintf( szMsg, STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_OTHER), lCode);
			sprintf( szFullMsg, "[HackShield:0x%X]\n %s \n", lCode, szMsg);
			__log( szFullMsg);

			SYSTEM_POPUP_C2( szFullMsg, CbExitGame, (void*)g_pFramework);*/
			SYSTEM_POPUP_C2( STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM), CbExitGame, (void*)g_pFramework);
			break;
		}
	}

	return 1;
}

static char g_szLastMsg[1024] = UNKNOWN_TEXT;

static const char * _getErrMsg( INT32 code)
{	
	const char * lpszMsg = UNKNOWN_TEXT;

	switch( code)
	{
	case 0:
		return NULL;
		break;
	case HS_ERR_INVALID_PARAM:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INVALID_PARAM);	// "파라미터가 잘못되었습니다.";
		break;
	case HS_ERR_COMPATIBILITY_MODE_RUNNING:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_COMPATIBILITY_MODE_RUNNING);	//	"호환성 모드로 프로그램이 실행중입니다.\n프로그램을 종료합니다.";
		break;	
	case HS_ERR_INVALID_LICENSE:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INVALID_LICENSE);	//"라이선스 키가 잘못 입력되었습니다.";
		break;	
	case HS_ERR_INVALID_FILES:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INVALID_FILES);	//"잘못된 파일 설치되었습니다.\n프로그램을 재설치하시기 바랍니다.";
		break;	
	case HS_ERR_INIT_DRV_FAILED:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_INIT_DRV_FAILED);	//"드라이버 시작이 실패하였습니다.";
		break;	
	case HS_ERR_DEBUGGER_DETECT:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_DETECT_DEBUGGER);	//"컴퓨터에서 디버거 실행이 감지되었습니다.\n디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다.";
		break;	
	case HS_ERR_NOT_INITIALIZED:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_NOT_INITIALIZED);	//"초기화 되지 않았습니다.";
		break;	
	case HS_ERR_SERVICE_STILL_RUNNING:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_SERVICE_STILL_RUNNING);	//"이미 실행중입니다.";
		break;	
	case HS_ERR_SERVICE_NOT_RUNNING:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_SERVICE_NOT_RUNNING);	//"시작되지 않았습니다.";
		break;
	case HSERROR_ENVFILE_NOTREAD:
	case HSERROR_ENVFILE_NOTWRITE:
	case HSERROR_NETWORK_CONNECT_FAIL:
	case HSERROR_HSUPDATE_TIMEOUT:
	case HSERROR_MISMATCH_ENVFILE:
	case HSERROR_HOSTFILE_MODIFICATION:
	case HSERROR_LIB_NOTEDIT_REG:
	case HSERROR_NOTFINDFILE:
	case HSERROR_PROTECT_LISTLOAD_FAIL:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_UPDATE_FAILED);	//"업데이트에 실패했습니다.";
		break;	
	case HS_ERR_DRV_FILE_CREATE_FAILED:
	case HS_ERR_REG_DRV_FILE_FAILED:
	case HS_ERR_START_DRV_FAILED:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_PROBLEM);	//"해킹 방지 기능에 문제가 발생하였습니다.";
		break;
	case HS_ERR_NEED_UPDATE:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_NEED_MODULE);	//"모듈의 업데이트가 필요합니다.";
		break;
	case HS_ERR_NEED_ADMIN_RIGHTS:
		lpszMsg = STR_TBL_GLOBALDEF( STR_TBL_GLOBALDEF_HSHIELD_HAVE_ADMIN);		//"ADMIN 권한이 필요합니다.";
		break;
	}

	sprintf( g_szLastMsg, "[HackShield:0x%X]\n %s \n", code, lpszMsg);

	return g_szLastMsg;
}

enum HS_NOTIFY_TYPE
{
	HS_NOTIFY_NONE = 0,
	HS_NOTIFY_MSG_BOX,
	HS_NOTIFY_I3FATAL,
	HS_NOTIFY_I3WARN,
	HS_NOTIFY_I3TRACE,	
};


static BOOL __HS_CHK(INT32 code, HS_NOTIFY_TYPE popup, const char * file, INT32 line)
{
	if( code != 0)
	{		
		const char * pszMsg = _getErrMsg( code);
		char msg[1024] = {};
		i3String::Format(msg, 1024, _T("file : %s / line : %d\n%s"), file, line, pszMsg);

		switch( popup)
		{
		case HS_NOTIFY_MSG_BOX:		MessageBox( NULL, msg, "Error", MB_OK);		break;
		case HS_NOTIFY_I3FATAL:		I3FATAL( msg);								break;
		case HS_NOTIFY_I3WARN:		I3WARN( msg);								break;
		case HS_NOTIFY_I3TRACE:		I3TRACE( msg);								break;
		}

		__log( msg);
		
		return FALSE;
	}

	return TRUE;
}

#define		_HS_CHK(code, popup)		__HS_CHK(code, popup, __FILE__, __LINE__)

//	스마트 업데이터를 사용할 경우 Init함수보다 선행되어야 한다.
//	업데이트 서버와 연결되어 있어야 함
//
//	>>> 디버깅시 주의! 2009.04.30
// EhSvc.dll와 HShield.dat 파일은 디버깅용과 엔드 유저용이 따로 존재하며 스마트 업데이트를 이용하여
// 자동 업데이트를 하면 업데이트 서버의 엔드 유저용이 다운받게되어 초기화(_AhnHS_Initialize)시 
// 디버깅 툴 감지되어 에러값을 리턴하게 되므로 디버깅시에는 스마트 업데이트를 사용하지 말것.
BOOL HackShield::RunUpdater(void)
{
	AHNHS_EXT_ERRORINFO	extErrorInfo ={};	// 모니터링 기능을 사용하지 않으면 0으로 초기화해서 넘기면 됩니다.

	DWORD dwRet = _AhnHS_HSUpdateEx( g_szHShieldPath,		// 핵쉴드 폴더 경로
								 1000 * 600,				// 업데이트 전체 타임아웃
								 PB_HSHIELD_GAME_CODE,		// 게임 코드
								 AHNHSUPDATE_CHKOPT_HOSTFILE | AHNHSUPDATE_CHKOPT_GAMECODE,
								 extErrorInfo,
								 1000 * 20);				// 서버 연결 타임아웃

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] update (0x%X).\r\n", (INT32)dwRet);
	__log( temp);*/

	if( !_HS_CHK( (INT32) dwRet, HS_NOTIFY_MSG_BOX))		return FALSE;

	return TRUE;

}

//
BOOL HackShield::Init()
{
	INT32 nRet = 0;
	TCHAR szFullFilePath[MAX_PATH<<2] = {};
	DWORD dwOption = 0;


	//	핵쉴드를 초기화하는 EhSvc.dll와 HShield.dat 파일의 경우 배포용 파일과 개발용 파일이이 다르므로 경로도 다르다.


	::GetCurrentDirectory( MAX_PATH, szFullFilePath);
	i3String::Concat(szFullFilePath, HSHIELD_PATH);

	i3String::Copy(g_szHShieldPath, szFullFilePath);	// HShield-Path 세팅



	// 핵쉴드 스마트 업데이트를 호출한다. (단, 미리 업데이트 서버와 연결 및 설정이되어 있어야 함)
	//	EhSvc.dll이 로드되기 전에 핵쉴드 모듈 업데이트가 완료되어야 한다.
#if defined(HSHIELD_CALL_UPDATER)

	if( ! HackShield::RunUpdater())
		return FALSE;
#else
	__log( "[HackShield] no update.\r\n");
#endif

	// 핵쉴드 폴더의 EhSvc.dll 위치를 저정합니다.
	i3String::Concat( szFullFilePath, _T("\\EhSvc.dll"));		// EHSVC.DLL =Path 세팅

	/*
	AHNHS_CHKOPT_ALL 옵션에 아래 옵션 기능 포함

	AHNHS_CHKOPT_SPEEDHACK					: 스피드 핵 차단
	AHNHS_CHKOPT_READWRITEPROCESSMEMORY		: 핵쉴드 DLL을 사용하는 프로세스 메모리를 외부에서 접근 차단
	AHNHS_CHKOPT_KDTRACER					: 커널 모드 디버거 실행을 감지하여 게임에 알려줌.
	AHNHS_CHKOPT_OPENPROCESS				: 핵쉴드 DLL을 사용하는 프로세스에 대한 정보를 얻는 OpenProcess API 함수 호출을 차단.
	AHNHS_CHKOPT_AUTOMOUSE					: 오토 마우스 종류의 프로그램에 의해 영향 받지 않도록 차단
	AHNHS_CHKOPT_MESSAGEHOOK				: 메시지 후킹 차단
	AHNHS_CHKOPT_PROCESSSCAN				: 주기적으로 프로세스 목록 검사하여 해킹 툴 실행 여부 검사
	AHNHS_CHKOPT_HSMODULE_CHANGE			: 
	AHNHS_USE_LOG_FILE						: 핵쉴드와 관련된 로그를 기록 EhSvc.dll이 위치한 hshield.log로 파일 남김. 암호화되어 있어 다르게 확인 가능.
	*/

	// Initialize 함수 호출 옵션 플래그 정의
	dwOption =	AHNHS_CHKOPT_ALL;

	//	게임 자체에서 메시지 후킹을 사용하기 때문에 메시지 후킹을 막으면 게임을 플레이할 수 없다.
	dwOption &= ~AHNHS_CHKOPT_MESSAGEHOOK;

#if defined (I3_DEBUG)	
	//	코드 디버깅시에는 빼야 정상 디버깅이 가능함.
	dwOption &= ~AHNHS_CHKOPT_READWRITEPROCESSMEMORY;
	dwOption &= ~AHNHS_CHKOPT_KDTRACER;
	dwOption &= ~AHNHS_CHKOPT_OPENPROCESS;
#else	
	dwOption |= AHNHS_DISPLAY_HACKSHIELD_LOGO;		// 초기화시 핵쉴드 이미지 로고를 표시해줌.
	dwOption |= AHNHS_DISPLAY_HACKSHIELD_TRAYICON;	// 트레이아이콘에 핵쉴드를 보여줌.
	dwOption |= AHNHS_CHKOPT_STANDALONE;			// 동일한 게임 코드를 사용하는 게임에 대하여 중복 실행이 불가능하도록 핵쉴드에서 오류 코드를 리턴합니다	
	dwOption |= AHNHS_CHKOPT_PROTECT_D3DX;			// DirectX 모듈의 VTable을 후킹 하는 해킹 툴을 차단하는 기능. Windows NT 이상의 OS에서만 동작
	dwOption |= AHNHS_CHKOPT_LOCAL_MEMORY_PROTECTION;	//	지정한 보호 대상 파일의 메모리 영역을 보호하는 옵션입니다.
	dwOption |= AHNHS_CHKOPT_SELF_DESTRUCTION;		//	핵쉴드에서 해킹 행위를 감지하여 CallBack 함수 호출 한 이후에도 게임 프로세스가 종료 되지 않은 경우, 일정 시간 이후 (1분 후) 핵쉴드 자체적으로 프로세스를 종료시키는 기능입니다.
	dwOption |= AHNHS_CHKOPT_DETECT_VIRTUAL_MACHINE; // 가상 머신 프로그램감지
	//dwOption |= AHNHS_CHKOPT_UPDATED_FILE_CHECK; 
#endif

	//	Initialize 함수 호출하여 핵쉴드를 초기화합니다.
	nRet = _AhnHS_Initialize( szFullFilePath, 
								_CallbackProc,		// 콜백 함수
								PB_HSHIELD_GAME_CODE,	// 고유 게임 코드
								PB_HSHIELD_LICENSE_KEY,	// 라이선스 키
								dwOption,
								AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL);

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] initialize (0x%X).\r\n", nRet);
	__log( temp);*/
	
	//	초기화 실패시 에러 처리
	if( !_HS_CHK( nRet, HS_NOTIFY_MSG_BOX))		return FALSE;
				
	return TRUE;
}

BOOL HackShield::StartService(BOOL bErrorHandle)
{
	INT32 nRet = _AhnHS_StartService();

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] start (0x%X).\r\n", nRet);
	__log( temp);*/

	if( bErrorHandle)
	{
		//	초기화 실패시 에러 처리
		if( !_HS_CHK( nRet, HS_NOTIFY_I3FATAL))		return FALSE;
	}
	else
	{
		if(nRet != 0)	return FALSE;
	}

	return TRUE;	
}

BOOL HackShield::StopService(BOOL bErrorHandle)
{
	INT32 nRet = _AhnHS_StopService();

	/*char temp[256] = "";
	sprintf( temp, "[HackShield] stop (0x%X).\r\n", nRet);
	__log( temp);*/

	if( bErrorHandle)
	{
		//	초기화 실패시 에러 처리
		if( !_HS_CHK( nRet, HS_NOTIFY_I3FATAL))		return FALSE;
	}
	else
	{
		if(nRet != 0)	return FALSE;
	}

	return TRUE;
}

BOOL HackShield::Release(BOOL bErrorHandle)
{
	INT32 nRet = _AhnHS_Uninitialize();
	
	/*char temp[256] = "";
	sprintf( temp, "[HackShield] release (0x%X).\r\n", nRet);
	__log( temp);*/

	if( bErrorHandle)	
	{
		//	초기화 실패시 에러 처리
		if( !_HS_CHK( nRet, HS_NOTIFY_I3FATAL))		return FALSE;
	}
	else
	{
		if(nRet != 0)	return FALSE;
	}
				
	return TRUE;
}

static INT32 s_ResponseRet = 0;

BOOL HackShield::MakeResponse( AHNHS_TRANS_BUFFER * pRequest, AHNHS_TRANS_BUFFER * pResponse)
{
	if( pRequest && pResponse)
	{
		UINT32 nRet = _AhnHS_MakeResponse( pRequest->byBuffer, pRequest->nLength, pResponse);

		if( nRet != ERROR_SUCCESS)
		{
			switch( nRet)
			{
				// 구버전 체크는 TRUE 반환 합니다.
			case HS_ERR_ANTICPXCNT_BAD_HSHIELD_MODULE:
				//I3NOTICE( "[HackShield] Detect invalid response (0x%X)", nRet);
			case HS_ERR_ANTICPXCNT_BAD_CLIENT_FILE:
#ifdef I3_DEBUG
			case HS_ERR_ANTICPXCNT_DEBUGGER_DETECTED:		// 디버깅 상황 감지
#endif
				s_ResponseRet = nRet;
				return TRUE;
			default:
			//case HS_ERR_ANTICPXCNT_INVALID_PARAMETER:		// 파라미터 값이 올바르지 않다.
			//case HS_ERR_ANTICPXCNT_INVALID_ADDRESS:			// 잘못된 메모리 주소
			//case HS_ERR_ANTICPXCNT_NOT_ENOUGH_MEMORY:		// 메모리 부족
			//case HS_ERR_ANTICPXCNT_CRC_TABLE_INIT_FAILED:	// 초기화에 실패
			//case HS_ERR_ANTICPXCNT_BAD_LENGTH:				// 메시지 사이즈가 잘못
			//case HS_ERR_ANTICPXCNT_INSUFFICIENT_BUFFER:		// 전달된 버퍼의 사이즈가 잘못
			//case HS_ERR_ANTICPXCNT_NOT_SUPPORTED:			// 현재 버전에서 지원 않음.
			//case HS_ERR_ANTICPXCNT_FILE_NOT_FOUND:			// 클라이언트 파일을 찾을 수 없다.
			//case HS_ERR_ANTICPXCNT_INVALID_MESSAGE_SIZE:	// 입력 받은 메시지의 크기가 잘못
			//case HS_ERR_ANTICPXCNT_BAD_FORMAT:				// 올바른 포멧이 아님.			
			//case HS_ERR_ANTICPXCNT_BAD_HSHIELD_MODULE:		// 핵쉴드 모듈 경로가 잘못되었거나 모듈이 잘못
			//case HS_ERR_ANTICPXCNT_BAD_CLIENT_FILE:			// 클라이언트 모듈이 잘못
			//case HS_ERR_ANTICPXCNT_BAD_REQUEST:				// 서버로부터 받은 요청 메시지가 잘못
			//case HS_ERR_ANTICPXCNT_HSHIELD_CORE_ENGINE_NOT_WORKING:	// 핵쉴드 코어 엔진이 정상 작동 않음.
			//case HS_ERR_ANTICPXCNT_UNKNOWN:
				{
					if( s_ResponseRet != nRet)
					{
						//I3NOTICE( "[HackShield] Detect invalid response (0x%X)", nRet);

						s_ResponseRet = nRet;
					}
				}
				return FALSE;
			}
				
		}
		else
		{
			s_ResponseRet = nRet;
			return TRUE;
		}
	}

	return FALSE;
}

static LPTOP_LEVEL_EXCEPTION_FILTER s_lpPreviousFilter = NULL;

LONG CALLBACK ProcessUnhandledException(LPEXCEPTION_POINTERS params)
{
	//	핵쉴드 종료 (예외 종료시에도 처리해줘야 함.)
	HackShield::StopService(FALSE);
	HackShield::Release(FALSE);

	/*char szMsg[MAX_STRING_COUNT] = "";
	char * pszExceptType = "";
	switch(params->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		pszExceptType="Access violation. You're probably doing something nasty with a null pointer.";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		pszExceptType="Array bounds exceeded. You're reading beyond the end of an array.";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		pszExceptType="Float divide by zero. You're dividing a float or double by zero.";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		pszExceptType="Integer divide by zero. You're dividing an int or long by zero.";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		pszExceptType="Illegal instruction. Either your .EXE was corrupted or somebody's\n"
			"been fooling around with bad assembly code.";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		pszExceptType="Noncontinuable exception. You hit \"ignore\" after a fatal error.";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		pszExceptType="Stack overflow. You either have a problem with recursive functions or you're\n"
			"declaring WAY too many local variables.";
		break;
	default:
		pszExceptType="Unknown exception. Try searching for the code under MSDN.";
		break;
	}

	i3String::Format( szMsg, sizeof( szMsg),
				">>> Unhandled exception: 0x%08X (%s)\r\nAddress: 0x%08X\r\n",
				params->ExceptionRecord->ExceptionCode,
				pszExceptType,
				params->ExceptionRecord->ExceptionAddress);

	__log( szMsg);*/

	//	이전 핸들러 복구
	SetUnhandledExceptionFilter( s_lpPreviousFilter);

#ifdef I3_DEBUG
    return EXCEPTION_CONTINUE_SEARCH;    
#else
    return EXCEPTION_EXECUTE_HANDLER;
#endif
}

void HackShield::SetUnhandledException(void)
{
	//	핸들러 셋팅
	s_lpPreviousFilter = SetUnhandledExceptionFilter( ProcessUnhandledException);
}

#endif