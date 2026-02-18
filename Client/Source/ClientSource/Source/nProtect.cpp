#include "pch.h"

#if defined (_NPROTECT_)

#include "StageBattle.h"
#include "nProtect.h"
#include "GlobalVariables.h"

static CNPGameLib * g_pNPGame = nullptr;

char g_szHackMsg[512];

static const char *	_getErrMsg( UINT32 code, bool bPopupFAQ)
{	
	const char * lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_UNKNOWN_ERROR");/*알 수 없는 에러*/

	// ‘6. 주요에러코드’를 참조하여 상황에 맞는 메시지를 출력해줍니다.
	switch( code)
	{
		case NPGAMEMON_ERROR_EXIST:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DUPLICATED_GAMEGAURD");/*게임가드가 실행 중 입니다. 잠시 후나 재부팅 후에 다시 실행해보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_GAME_EXIST:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DUPLICATED_RUN");/*게임이 중복 실행되었거나 게임가드가 이미 실행 중 입니다. 게임 종료 후 다시 실행해보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_INIT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_INIT_ERROR1");/*게임가드 초기화 에러입니다. 재부팅 후 다시 실행해보거나 충돌할 수 있는 다른 프로그램들을 종료한 후 실행해 보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
 
		case NPGAMEMON_ERROR_NFOUND_INI:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHECK_GAMEGAURD");/*게임가드 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치해보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_CRYPTOAPI:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DAMAGED_WINDOW_FILE");/*윈도우의 일부 시스템 파일이 손상되었습니다. 인터넷 익스플로러(IE)를 다시 설치해보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_EXECUTE:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_RUN_FAILED1");/*게임가드 실행에 실패했습니다. 게임가드 셋업 파일을 다시 설치해보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_WRONG_PROGRAM");/*불법 프로그램이 발견되었습니다. 불필요한 프로그램을 종료한 후 다시 실행해보시기 바랍니다.*/
			break;

		case NPGMUP_ERROR_ABORT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_UPDATED_ABORTED");/*게임가드 업데이트를 취소하셨습니다. 접속이 계속 되지 않을 경우 인터넷 및 개인 방화벽 설정을 조정해 보시기 바랍니다.*/
			break;

		case NPGMUP_ERROR_CONNECT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_CONNECT_FAIL2");/*게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 다시 접속하거나, 네트웍 상태를 점검한 후, 다시 시도해 주십시오.*/
			break;

			/*
		case HOOK_TIMEOUT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHECK_SYSTEM1");//바이러스나 스파이웨어로 인해 초기화에 실패하였습니다. 최신 백신을 받으신 후 컴퓨터 전체 검사를 해보신 후, 다시 시도해 주십시오.
			break;
			*/

		case NPGAMEMON_ERROR_GAMEGUARD:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_REINSTALL_GAMEGAURD");/*게임가드 초기화 에러 또는 구버젼의 게임가드 파일입니다. 게임가드 셋업파일을 다시 설치하고 게임을 실행해 보시기 바랍니다.*/
			break;

			/*
		case NPGMUP_ERROR_PARAM:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_NO_EXIST_INI_FILE");//ini 파일이 없거나 변조되었습니다. 게임가드 셋업 파일을 설치하면 해결할 수 있습니다.
			break;
			*/

			/*
		case NPGMUP_ERROR_INIT:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_NPGMUP_FAIL");//npgmup.des 초기화 에러입니다. 게임가드 폴더를 삭제후 다시 게임실행을 해봅니다.
			break;
			*/

		case NPGMUP_ERROR_DOWNCFG:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_CONNECT_FAIL1");/*게임가드 업데이트 서버 접속에 실패하였습니다. 잠시 후 재시도 해보거나, 개인 방화벽이 있다면 설정을 조정해 보시기 바랍니다.*/
			break;

		case NPGMUP_ERROR_AUTH:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_UPDATED_FAIL");/*게임가드 업데이트를 완료하지 못 했습니다. 바이러스 백신을 일시 중시 시킨 후 재시도 해보시거나, PC 관리 프로그램을 사용하시면 설정을 조정해 보시기 바랍니다.*/
			break;

		case NPGAMEMON_ERROR_NPSCAN:
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHECK_SYSTEM2");/*바이러스 및 해킹툴 검사 모듈 로딩에 실패 했습니다. 메모리 부족이거나 바이러스에 의한 감염일 수 있습니다.*/
			break;

		default:
			// 적절한 종료 메시지 출력
			lpszMsg = GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_RUN_FAILED2");/*게임가드 실행 중 에러가 발생하였습니다. 게임 폴더 안의 GameGuard 폴더에 있는 *.erl 파일들을 Game1@inca.co.kr로 첨부하여 메일 보내주시기 바랍니다.*/
			break;
	}

	 
	// 에러 코드를 넘겨주면 에러 발생시 자동으로 게임가드 FAQ 페이지를 띄워줌
	if( bPopupFAQ)
	{		
		//g_pNPGame->RunFAQ( code);		
	}

	I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect-err] : CodeNum[ %d ] - [ %s ]", code, lpszMsg );

	return lpszMsg;
}

static bool		_NP_CHK( UINT32 code)
{
	if( code != NPGAMEMON_SUCCESS)
	{		
		I3PRINTLOG(I3LOG_FATAL,  _getErrMsg( code, false));
		return false;
	}
	return true;
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	// 여기에서 직접 Message Box 등을 띄우지 않고, Queueing만 했다가
	// 종료하기 전에 출력해야 한다.
	// 이것은 Popup시키는 해당 위치를 추적해, GameGuard의 위치를 찾아내고
	// Hack할 가능성을 높여주기 때문에...
	CHud * pHUD = nullptr;
	if( g_pFramework->IsBattleStage() )
		pHUD = CHud::i();

	switch (dwMsg)
    {
		case NPGAMEMON_CHECK_CSAUTH2 : 
			{
				GG_AUTH_DATA AuthData;
				memcpy(&AuthData, (PVOID)dwArg, sizeof(GG_AUTH_DATA));
				GameEventSender::i()->SetEvent( EVENT_GAME_GUARD_AUTH2, (void*)dwArg);
			}
			return true; 
			break; 
		case NPGAMEMON_CHECK_CSAUTH3 : 
			{
				I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect] : 3" );	//CS 서버에서 인증 받음
				GameEventSender::i()->SetEvent( EVENT_GAME_GUARD_AUTH3, (void*)dwArg );
			}
			return true; 
			break; 
		case NPGAMEMON_COMM_ERROR :
		case NPGAMEMON_COMM_CLOSE :
			return false;

		case NPGAMEMON_INIT_ERROR:
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_GAMEGAURD_INIT_ERROR2"), dwArg);/*게임가드 초기화 에러 : %lu*/
			return false;

		case NPGAMEMON_SPEEDHACK:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_SPEED_HACK"));/*스피드핵이 감지되었습니다.*/			
			return false;

		case NPGAMEMON_GAMEHACK_KILLED:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s( g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_GAME_HACK"), g_pNPGame->GetInfo());/*게임핵이 발견되었습니다.\r\n%s*/
			return false;

		case NPGAMEMON_GAMEHACK_DETECT:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_DETECT_GAME_HACK"), g_pNPGame->GetInfo());/*게임핵이 발견되었습니다.\r\n%s*/
			return false;

		case NPGAMEMON_GAMEHACK_DOUBT:
			if( pHUD != nullptr) pHUD->SetActivateHackWarnning();
			sprintf_s(g_szHackMsg, 512, GAME_STRING("STR_TBL_GLOBALDEF_NPROTECT_CHANGED_GAME_FILE"));/*게임이나 게임가드가 변조되었습니다.*/
			return false;
	}

	return TRUE;		// <- Hacking이 발견된 경우에도 계속 Game을 진행하겠다는 의미.
	// return FALSE;	// <- 만약 Hacking이 Detect되었을 때 게임을 종료하려면 이걸로...
}

void nProtect::SetServerAuth2(GG_AUTH_DATA * pAuthData)
{
	g_pNPGame->Auth2(pAuthData); //에러처리를 하지말아야 한다. 잉카에서 받음 성백
}

void nProtect::SetServerAuth3(PBYTE pPacket, UINT32 size)
{
	DWORD ret = g_pNPGame->Auth3( pPacket, size, 0); //에러처리를 하지말아야 한다. 잉카에서 받음 성백
	if( ret == 1 )
	{
		I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect] : 2" );	//CS 서버로 인증 요청
	}
	else
	{
		//nProtect 문서에서 에러 코드를 확인
		I3PRINTLOG(I3LOG_DEFALUT,  "[nProtect-err] : 2 - [ %d ]", ret );	//CS 서버로 인증 요청 실패
	}
}

bool nProtect::Init( HWND hwndMain)
{
	UINT32 code; 

#ifdef LOCALE_THAILAND
	g_pNPGame = new CNPGameLib("PointBlankTH");
#elif defined(LOCALE_KOREA)
	g_pNPGame = new CNPGameLib("PointBlank");
#else
	I3ASSERT_0;
#endif

	code = g_pNPGame->Init();
	if( _NP_CHK( code) == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "_NP_CHK false code : %d", code);
		return false;
	}	

	g_pNPGame->SetHwnd( hwndMain);
	return true;
}

bool nProtect::Close(void)
{
/*
*게임가드 종료시 아래 적용 사항을 다시 한번 점검해 주시기 바랍니다.
1. new를 사용하여 동적으로 객체를 생성한 경우 동적으로 생성한 CNPGameLib 객체를 게임 종료 처리 부분에서 반드시 delete를 해주셔야 합니다.
*/
	delete g_pNPGame; 
	g_pNPGame = nullptr; 
	return true;
}

bool nProtect::CheckAlive(void)
{
	return g_pNPGame->Check() == NPGAMEMON_SUCCESS;
}

bool nProtect::SendUserID( char * pszID)
{
	UINT32 code = g_pNPGame->Send( pszID);

	return _NP_CHK( code);
}

bool nProtect::Set_D3DDeviceInfo(DWORD dwD3DVer, LPVOID *lppD3DDevicePtr)
{
	return (SetD3DDeviceInfo(dwD3DVer, lppD3DDevicePtr) == true) ? true : false;
}

#endif
