#include "pch.h"
#include "Config.h"

#include "UserFinder.h"
#include "TaskProcessor.h"
#include "ModuleLog.h"
#include "ModuleControl.h"
#include "ModuleSIA.h"
#include "ModuleThaiPc.h"
#include "ModuleDBShop.h"
#include "ModuleZlog.h"

UINT32				g_ui32ConnectServer		= 0;
char				g_pCCurrentPath[]		= {0,}; 
SERVER_STATE		g_eServerState			= SERVER_STATE_NONE;
UINT32				g_ui32StartTime			= 0;
INT32				g_i32MainLogIdx			= 0;
i3LogFileW*			g_pLog					= NULL;

void g_SetCurrentPath( char* strPath )
{
	//Set Char 
	i3String::Copy( g_pCCurrentPath,	strPath,  sizeof(g_pCCurrentPath) );
}

INT32 g_InitServer(void)
{
	//_CrtSetBreakAlloc( 644 );

	char strPath[ SERVER_STRING_COUNT ];

	// Create Memory
	i3MemoryInit(0); 

	// Error Handle Init
	i3Error::Init();

	// Expection 생성
	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_pCCurrentPath, "PBTrans.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Timer Init
	if( false == i3ThreadTimer::Init() )											return SERVER_ERROR_TIMER;

	//Network Init
	if( FALSE == i3Net::Init() )													return SERVER_ERROR_NETWORK;

	// Create ContextFile 
	g_pConfig = CConfig::NewObject();
	if( NULL == g_pConfig )															return SERVER_ERROR_CONFIGINI;
	if( FALSE == g_pConfig->Create() )												return SERVER_ERROR_CONFIGINI;

	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_pCCurrentPath, "Config.ini" );
	INT32 i32LogResult = g_pConfig->LoadLogFile( strPath );
	if( SERVER_ERROR_NO != i32LogResult )											return i32LogResult;

	g_pLog = i3LogFileW::NewObject();
	if( NULL == g_pLog )															return SERVER_ERROR_LOGFILE;
	g_pLog->SetVersion( VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL );
	if( FALSE == g_pLog->Create( g_pConfig->m_wstrLogFilePath, L"Trans" ) )			return SERVER_ERROR_LOGFILE;
	
	g_i32MainLogIdx = g_pLog->AddLogThread( L"INIT", 0, GetCurrentThreadId() );
	g_pLog->WriteLog( L"--------------------- Trans Server Create ---------------------" );

	// Load Config
	if( FALSE == g_pConfig->LoadConfigFile( strPath ) )
	{
		g_pLog->WriteLog( L"[ERRO] Load Config" );
		return SERVER_ERROR_CONFIGINI;
	}
	g_pLog->WriteLog( L"[SUCC] Load Config" );

	//Create Control 
	g_pModuleControl = CModuleControl::NewObject(); 
	if( FALSE == g_pModuleControl->OnCreate() )
	{
		g_pLog->WriteLog( L"[ERRO] Create Control" );
		return SERVER_ERROR_CONTROLSERVER; 
	}
	g_pLog->WriteLog( L"[SUCC] Create Control" );

	// Init End 
	g_eServerState		= SERVER_STATE_LOADING;
	g_ui32StartTime		= i3ThreadTimer::GetServerTime();
	g_pLog->WriteLog( L"[SUCC] Init Complete" );

	return EVENT_ERROR_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_eServerState = SERVER_STATE_INITING;

	g_pLog->AddLogThread( L"MAIN", 0, GetCurrentThreadId() );

	g_pLog->WriteLog( L"---------[LOAD] Start Load----------------"  );

	// Create CPU Speed
	if( EVENT_ERROR_SUCCESS != g_pModuleControl->InitSystemLog() )
	{
		g_pLog->WriteLog( L"[ERRO] Start Control SystemLog" );
	}
	g_pLog->WriteLog( L"[SUCC] Start Control SystemLog" );

	// Create ServiceLogFile
	g_pModuleServiceLogFile = CModuleServiceLogFile::NewObject();
	if( NULL == g_pModuleServiceLogFile || 
		FALSE == g_pModuleServiceLogFile->OnCreate() )
	{
		g_pLog->WriteLog( L"[ERRO] Create ServiceLogFile" );
		return SERVER_ERROR_LOGFILE; 
	}
	g_pLog->WriteLog( L"[SUCC] Create ServiceLogFile" );

	// Read User List 
	g_pUserFinder = CUserFinder::NewObject();
	if( NULL == g_pUserFinder ||
		FALSE == g_pUserFinder->Create() )
	{
		g_pLog->WriteLog( L"[ERRO] Create User" );
		return SERVER_ERROR_INIT; 
	}
	g_pLog->WriteLog( L"[SUCC] Create User"  );

	// 통계 오브젝트
	g_pStatistics = new CServerStatistics;
	if (g_pStatistics == NULL)
	{
		g_pLog->WriteLog(L"[LOAD] Init g_pStatistics Fail");
		return SERVER_ERROR_STATISTICS;
	}

	STAT_THREADCOUNT	stStatThread;
	i3mem::FillZero(&stStatThread, sizeof(STAT_THREADCOUNT));
	stStatThread.m_i32Count[STAT_TYPE_DB_LOGIN] = DATABASE_THREAD_COUNT;
	stStatThread.m_i32Count[STAT_TYPE_DB_LOGOUT] = DATABASE_THREAD_COUNT;
	stStatThread.m_i32Count[STAT_TYPE_DB_SHOP] = DATABASE_THREAD_COUNT;

	if (FALSE == g_pStatistics->Create(&stStatThread))
	{
		g_pLog->WriteLog(L"[LOAD] Create g_pStatistics Fail");
		return SERVER_ERROR_STATISTICS;
	}

	// Shop
	g_pShop = CShop::NewObject();
	if( NULL == g_pShop )
	{
		g_pLog->WriteLog( L"[ERRO] Create Shop" );
	}
	g_pLog->WriteLog( L"[SUCC] Create Shop" );

	// Create Module DataBase Shop 
	g_pModuleDBShop	= CModuleDBShop::NewObject();
	INT32 i32Code = g_pModuleDBShop->OnCreate();
	if( 1 != i32Code )	
	{
		g_pLog->WriteLog( L"[LOAD] Create Module DataBase Shop Fail, Code:%d", i32Code  );
		return SERVER_ERROR_DATABASE_SHOP; 
	}
	g_pLog->WriteLog( L"[LOAD] Create Module DataBase Shop Success"  );

    //일하는 프로세스 
	g_pTaskProcessor = CTaskProcessor::NewObject();
	INT32 i32Rv = g_pTaskProcessor->Create( MAXPACKETBUFFERSIZE );
	if( SERVER_ERROR_NO != i32Rv )
	{
		g_pLog->WriteLog( L"[FAIL] Create Task, Code:%d", i32Rv  );
		return i32Rv; 
	}
	g_pLog->WriteLog( L"[SUCC] Create Task"  );

	// SIA Server
	g_pModuleSIA = new CModuleSIA;
	if( FALSE == g_pModuleSIA->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] CreateModule SIA"  );
		return SERVER_ERROR_SIA;
	}
	g_pLog->WriteLog( L"[SUCC] CreateModule SIA"  );

	// ModuleLogD
	g_pModuleLogD = CModuleLog::NewObject(); 
	if( FALSE == g_pModuleLogD->OnCreate( ) )	
	{
		g_pLog->WriteLog( L"[FAIL] CreateModule LogD"  );
		return SERVER_ERROR_LOGD; 
	}
	g_pLog->WriteLog( L"[SUCC] CreateModule LogD"  );

	// Create Zlog Transfer Module
	g_pModuleZlog = CModuleZLog::NewObject();
	if( FALSE == g_pModuleZlog->Create( g_pConfig->m_ui32ZLogServerIP, g_pConfig->m_ui16ZLogServerPort ) )
	{
		g_pLog->WriteLog( L"[FAIL] Create ZLog Module");
		return SERVER_ERROR_ZLOG_INIT_FAIL; 
	}
	g_pLog->WriteLog( L"[SUCC] Create ZLog Module");
	
	//ThaiPc Module 
	g_pModulePcList = CModulePcList::NewObject();
	if( (NULL == g_pModulePcList) || (FALSE == g_pModulePcList->OnCreate()) )
	{
		g_pLog->WriteLog( L"[FAIL] CreateModule PCList"  );
		return SERVER_ERROR_PCROOM; 
	}
	g_pLog->WriteLog( L"[SUCC] CreateModule PCList"  );

	g_pLog->WriteLog( L"---------[LOAD] End Load----------------"  );

	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료. (New)
	if( FALSE == g_pConfig->m_Version.VersionCompare( &g_pConfig->m_ControlVersion ) )
	{
		g_pLog->WriteLog( L"Build version is different with Control Server - Control Server : %d.%d , Trans Server : %d.%d",
			g_pConfig->m_ControlVersion._ui8VerGame, g_pConfig->m_ControlVersion._ui16VerSS,
			g_pConfig->m_Version._ui8VerGame, g_pConfig->m_Version._ui16VerSS );

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION; 
	}

	g_eServerState = SERVER_STATE_START;

	g_pLog->WriteLog( L"----------------------------------------" );
	g_pLog->WriteLog( L"-----------Start Server-----------------" );

	return EVENT_ERROR_SUCCESS;
}

BOOL g_UpdateServer(void)
{
	switch( g_eServerState )
	{
	case SERVER_STATE_INIT:
		{
			INT32 i32Result = g_StartServer();
			g_pModuleControl->ServerInit( i32Result );

			if( EVENT_ERROR_SUCCESS !=  i32Result )
			{
				Sleep( 1000 );					// Control Server 전송을 위해 1초간 쉬었다 종료 합니다.
				g_DestroyServer(i32Result);
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	case SERVER_STATE_START:
		{
			g_pConfig->OnUpdate(); 
		}
		break;
	default:
		{
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 60 < i3ThreadTimer::GetServerTime() )
			{
				g_pLog->WriteLog( L"[INIT_LOG]--ERROR-- CONTROL SERVER Connect Fail" );
				// 게임 종료
				if( g_pModuleControl )
				{
					g_pModuleControl->ServerInit(SERVER_ERROR_TIME_OUT );
				}
				g_DestroyServer(SERVER_ERROR_CONTROLSERVER);
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	}

	return TRUE; 
}

void g_DestroyServer(INT32 DestroyReason)
{
	if( g_pModuleControl )
	{
		i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		g_pModuleControl->SendPacketMessage( &SendPacket );
		Sleep(1000);
	}

	if(g_pLog) g_pLog->WriteLog( L"------------End Server------------------"  );
	if(g_pLog) g_pLog->WriteLog( L"----------------------------------------"  );
		
	if(g_pLog) g_pLog->WriteLog( L"---------[Dest] Start Destory----------------"  );
	
	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy PCList" );
	I3_SAFE_RELEASE(g_pModulePcList);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy SIA");
	I3_SAFE_DELETE( g_pModuleSIA );
	
	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy Control");
	I3_SAFE_RELEASE(g_pModuleControl);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy TaskProcessor" );
	I3_SAFE_RELEASE(g_pTaskProcessor);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy Shop"  );
	I3_SAFE_RELEASE(g_pShop);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy UserFinder" );
	I3_SAFE_RELEASE(g_pUserFinder);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy DBShop" );
	I3_SAFE_RELEASE(g_pModuleDBShop);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy LogD"  );
	I3_SAFE_RELEASE(g_pModuleLogD);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy ZLog Transfer Module"  );
	I3_SAFE_RELEASE(g_pModuleZlog);

	if (g_pLog) g_pLog->WriteLog(L"[Dest] Destroy Statistics");
	NEW_SAFE_DELETE(g_pStatistics);

	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy LogFile"  );
	if(g_pLog) g_pLog->WriteLog( L"[Dest] Destroy ServiceLogFile"  );
	if(g_pLog) g_pLog->WriteLog( L"---------[Dest] End Destory----------------"  );
	Sleep(10);
	I3_SAFE_RELEASE(g_pLog);
	I3_SAFE_RELEASE(g_pModuleServiceLogFile);

	I3_SAFE_RELEASE(g_pConfig);	
	i3Net::Destroy();
	i3ThreadTimer::Destroy();
#if 0
	i3ElementBase::DumpObjects();
#endif

	i3mem::Destroy();

	return; 
}

// 상점 목록
CShop* g_pShop = NULL;
//CShopSerializer* g_pShopSerializer = NULL;
