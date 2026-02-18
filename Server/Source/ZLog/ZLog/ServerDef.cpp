#include "pch.h"
#include "ServerDef.h"
#include "ModuleConfig.h"
#include "ModuleTask.h"
#include "ModuleControl.h"

//User PDH

SERVER_STATE		g_eServerState			= SERVER_STATE_NONE;
UINT32				g_ui32StartTime			= 0;
char				g_strCCurrentPath[]		= {0,};
i3LogFileW*			g_pLog					= NULL;

//-------------------------------------------------------------------------
//Time
static	INT32		s_i32ShopUpdateTime = 0;

void g_SetCurrentPath(char * pstrPath)
{
	//Set Char 
	i3String::Copy(g_strCCurrentPath, pstrPath, PATH_STRING_COUNT ); 
}

int g_InitServer(void)
{	
	//Set Error Dump File 
	char strPath[MAX_PATH];

	//Create Memory
	i3MemoryInit(0);

	//Error Handle Init
	i3Error::Init();

	i3String::Format( strPath, MAX_PATH, "%s\\%s", g_strCCurrentPath, "ZLog.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Timer Init
	if( false == i3ThreadTimer::Init() )													return SERVER_ERROR_TIMER;
		
	//Network Init
	if( FALSE == i3Net::Init() )															return SERVER_ERROR_NETWORK;

	i3String::Format( strPath, MAX_PATH, "%s\\%s", g_strCCurrentPath, "ZLogConfig.ini" );
	
	g_pConfig = new CModuleConfig;
	if( NULL == g_pConfig )																	return SERVER_ERROR_LOGFILE;
 	INT32 i32LogResult = g_pConfig->LoadLogInfo( strPath );
	if( SERVER_ERROR_NO != i32LogResult )													return SERVER_ERROR_LOGFILE;

	// Create Log Module
	g_pLog = i3LogFileW::NewObject();
	if (NULL == g_pLog) return SERVER_ERROR_LOGFILE;
	g_pLog->SetVersion(VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL);
	if (FALSE == g_pLog->Create(g_pConfig->m_wstrLogFilePath, L"ZLog"))	return SERVER_ERROR_LOGFILE;
	g_pLog->AddLogThread(L"INIT", 0, GetCurrentThreadId());
	
	// Load Config
	if( FALSE == g_pConfig->LoadConfigFile( strPath ) )
	{
		g_pLog->WriteLog(L"[ERRO] Load Config" );
		return SERVER_ERROR_CONFIGINI;
	}
	g_pLog->WriteLog(L"[SUCC] Load Config" );

	// Create Control Module
	g_pModuleControl = cModuleControl::NewObject();
	if( NULL == g_pModuleControl )															return SERVER_ERROR_SYSTEMLOG;
	g_pModuleControl->InitSystemLog();
	g_pLog->WriteLog(L"[SUCC] CREATE SYSTEM QUERY ");
	if( FALSE == g_pModuleControl->Create( g_pConfig->m_ui32ControlIP, g_pConfig->m_ui16ControlPort ) )
	{
		g_pLog->WriteLog(L"[FAIL] CREATE CONTROL MODULE ");
		return SERVER_ERROR_CONTROLSERVER;
	}
	g_pLog->WriteLog(L"[SUCC] CREATE CONTROL");

	g_eServerState		= SERVER_STATE_LOADING;
	g_ui32StartTime		= i3ThreadTimer::GetServerTime();

	g_pLog->WriteLog(L"============================================" );

	return EVENT_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_pLog->AddLogThread( L"MAIN", 0, GetCurrentThreadId() );

	g_eServerState = SERVER_STATE_INITING;

	g_pLog->WriteLog(L"===================================");
	
	// Create Task Module
	g_pModuleTask = new cModuleTask;
	INT32 i32Result = g_pModuleTask->Create();
	if( SERVER_ERROR_NO != i32Result )
	{
		g_pLog->WriteLog(L"[FAIL] CREATE TASK RESULT : %d", i32Result);
		return i32Result; 
	}
	g_pLog->WriteLog(L"[SUCC] CREATE TASK ");

	g_pLog->WriteLog(L"[INITIA] INITIALIZE Complete");
	g_pLog->WriteLog(L"===================================");

	// 컨트롤서버로부터 받는 버전 패킷에 문제가 있어서 임시로 주석처리
	//// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료.
	//if ( (g_ControlVersion._ui8VerGame != g_Version._ui8VerGame) || (g_ControlVersion._ui16VerBuild != g_Version._ui16VerBuild) || (g_ControlVersion._ui16VerSS != g_Version._ui16VerSS) )
	//{
	//	char strTemp[MAX_PATH];
	//	i3String::Format( strTemp, MAX_PATH, "Build version is different with Control Server - Control Server : %d.%d.%d , SIA Server : %d.%d.%d",
	//		g_ControlVersion._ui8VerGame, g_ControlVersion._ui16VerBuild, g_ControlVersion._ui16VerSS,
	//		g_Version._ui8VerGame, g_Version._ui16VerBuild, g_Version._ui16VerSS);
	//	g_pLog->WriteLog( strTemp );

	//	return EVENT_FAIL_CREATE_CONTROL; 
	//}

	g_pLog->WriteLog(L"================= Start Zlog Server ==================");

	// 서버 로딩 완료
	g_eServerState = SERVER_STATE_START;

	return EVENT_SUCCESS;
}

void g_UpdateServer(void)
{	
	switch( g_eServerState )
	{
	case SERVER_STATE_INIT:
		{
			INT32	i32Rv = g_StartServer();

			NET_I3_PACKET_BUFFER	SendErrorPacket(PROTOCOL_ASC_SERVER_LOADING_RESULT);
			SendErrorPacket.WriteData(&i32Rv, sizeof(INT32));
			g_pModuleControl->SendPacketMessage((char*)&SendErrorPacket, SendErrorPacket._ui16TotalSize);

			if( i32Rv != EVENT_SUCCESS )
			{
				// 에러가 났다면 서버 종료
				g_pLog->WriteLog(L"[INITIA] FAILED g_StartServer()");
				g_DestroyServer(i32Rv);
				exit(0);
			}
		}
		break;
	case SERVER_STATE_START:
		{
		}
		break;
	default:
		{
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 60 < i3ThreadTimer::GetServerTime() ) // 60초를 연결시간 제한으로 두고 60초 이상동안 연결이 안되면 종료됨
			{
				g_pLog->WriteLog(L"[INITIA] CONTROL SERVER Connect Fail");
				
				// 게임 종료
				g_DestroyServer(SERVER_ERROR_CONTROLSERVER);
				exit(0);
			}
		}
		break;
	}

	::Sleep(1);

	return; 
}


void g_DestroyServer(INT32 i32ErrorReason)
{
	// Control 서버에 정상 종료 패킷 전송.
	if( g_pModuleControl )
	{
		UINT8 ui8Type = ASC_SERVER_ZLOG;

		NET_I3_PACKET_BUFFER SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		SendPacket.WriteData( &ui8Type, sizeof(UINT8));
		SendPacket.WriteData( &i32ErrorReason, sizeof(INT32));
		g_pModuleControl->SendPacketMessage( (char* )&SendPacket, SendPacket._ui16TotalSize );
		Sleep(1000);
	}

	if( g_pLog )	g_pLog->WriteLog( L"===================================" );
	I3_SAFE_DELETE( g_pModuleTask );
	if( g_pLog )	g_pLog->WriteLog(L"[DESTROY] TASK");
	I3_SAFE_DELETE( g_pConfig );
	if( g_pLog )	g_pLog->WriteLog(L"[DESTROY] CONFIG");
	I3_SAFE_RELEASE( g_pModuleControl );
	if( g_pLog )	g_pLog->WriteLog(L"[DESTROY] Control");
	if( g_pLog )	g_pLog->WriteLog(L"===================================");

	Sleep( 10 );
	I3_SAFE_RELEASE( g_pLog );
	I3_SAFE_DELETE( g_pConfig );

	i3ThreadTimer::Destroy();
	return; 
}

//파일명이 yyyymmdd.... 인 파일명에서 yyyy_mm_dd인 문자열을 얻어냅니다.
bool GetDateFromFileName(wchar_t* wstrFileName, wchar_t* pwbuffer, UINT8 ui8size )
{
	wchar_t  wstrDate[11];
	INT32	i32PathLength	= -1;
	INT32	i32strPos		= 0;
	INT32	i32FullLength	= i3String::Length(wstrFileName);

	//전체파일명중 마지막 '\'가 있는위치 확인
	while(wstrFileName[i32strPos] != 0 )
	{
		if( wstrFileName[i32strPos] == '\\' ) i32PathLength = i32strPos;

		++i32strPos;
	}

	++i32PathLength;

	//파일명 아닌 경우 실패처리
	if( i32FullLength - i32PathLength < 8 )	return FALSE;

	//파일명 앞 8글자가 모두 숫자가 아닌경우 실패 처리
	for( INT32 i = i32PathLength, j = 0 ; i < i32PathLength + 8 ; ++i, ++j)
	{
		INT32 i32CharValue = wstrFileName[i] - '0';

		if( (i32CharValue < 0) || (i32CharValue > 9) )	return FALSE;
		
		wstrDate[j] = wstrFileName[i];

		if( j == 3 || j == 6 )	//연원일 구분을 위해 _를 넣습니다.
		{
			++j;
			wstrDate[j] = '_';
		}
	}

	wstrDate[10] = '\0';

	i3String::Copy(pwbuffer, wstrDate, ui8size);

	return TRUE;
}

//파일명 확장자를 변경하고 해당 파일을 rename 시킵니다.
bool ChangeFileExtension( char* strFileName, char* strExtension, INT32 i32Size )
{
	char strNewFileName[MAX_PATH];
	i3String::Copy( strNewFileName, strFileName, MAX_PATH );

	INT32 i32LastDotidx = -1;
	INT32 i32FileLength;

	i32FileLength = i3String::Length( strFileName );

	//파일명에서 마지막으로 '.'이 찍힌 위치를 찾습니다.
	for( INT32 i = 0 ; i < i32FileLength ; ++i )
	{
		if( strFileName[i] == '.' )	i32LastDotidx = i;
	}
	
	if( -1 < i32LastDotidx )
		strNewFileName[ i32LastDotidx + 1 ] = '\0';

	i3String::Concat( strNewFileName, strExtension );

	rename( strFileName, strNewFileName );
	i3String::Copy( strFileName, strNewFileName, i32Size );

	return TRUE;
}

HANDLE MakeFile( char* strFileName )
{		
	HANDLE	hFile;

	// File 생성	
	hFile = ::CreateFile( strFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == hFile )
	{
		wchar_t wstrFileName[MAX_PATH];
		MultiByteToWideChar(CP_ACP, NULL, strFileName, -1, wstrFileName, MAX_PATH);

		g_pLog->WriteLog( L"[ERR] Create File Fail : File:%s", wstrFileName);

		::CloseHandle( hFile );
		hFile = INVALID_HANDLE_VALUE;
		return false;
	}

	// 한번도 DB 에 쓰지 않았다는 Mark를 합니다.
	INT32 i32Size = 0;
	char strBuffer[ 16 ];
	strcpy_s( strBuffer, 16, "0000000012\r\n" );
	::WriteFile( hFile, strBuffer, 12, (LPDWORD)&i32Size, NULL );

	// 이미 쓰던 로그 파일일수도 있으니 맨 마지막 포인트로 이동합니다.
	//SetFilePointer( hFile, 0, NULL, FILE_END );

	return hFile;
}