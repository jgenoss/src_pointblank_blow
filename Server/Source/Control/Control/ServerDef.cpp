#include "pch.h"
#include "ServerDef.h"
#include "Control.h"
#include "ServerLogFile.h"
#include "UserManager.h"
#include "WebManager.h"
#include "LoadOptionDB.h"
#include "HackLogFile.h"
#include "ModuleKnock.h"
#include "Watch.h"

static UINT32		g_ui32ServerLogSaveTime		= 0;
char				g_strCurrentPath[]			= {0,}; 
INT32				g_i32MainLogIdx				= 0;
i3LogFileW*			g_pLog						= NULL;

INT32 g_InitServer(void)
{
	char strPath[ SERVER_STRING_COUNT ];

	//Create Memory
	i3MemoryInit(0);

	//Error Handle Init
	i3Error::Init();

	// Expection 생성
	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_strCurrentPath, "PBControls.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Create Thread Timer
	if( false == i3ThreadTimer::Init() )					return SERVER_ERROR_TIMER;

	//Create Network 
	if( FALSE == i3Net::Init() )							return SERVER_ERROR_NETWORK; 

	//Create Control //Config File 
	g_pControl = CControl::NewObject(); 
	i3String::Format( strPath, MAX_PATH, "%s\\Config.ini", g_strCurrentPath);

	INT32 i32LogResult = g_pControl->LoadLogInfo( strPath );
	if( SERVER_ERROR_LOGFILE == i32LogResult )				return SERVER_ERROR_LOGFILE;
	
	//Create LogFile
	g_pControl->SetServerStartTime();

	g_pLog = i3LogFileW::NewObject();
	if( NULL == g_pLog )															return SERVER_ERROR_LOGFILE;
	g_pLog->SetVersion( VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL );
	if( FALSE == g_pLog->Create( g_pControl->m_wstrLogFilePath, L"Control" ) )			return SERVER_ERROR_LOGFILE;
	
	g_i32MainLogIdx = g_pLog->AddLogThread( L"INIT", 0, GetCurrentThreadId() );
		
	if(i32LogResult == SERVER_ERROR_LOGPATH)
	{
		g_pLog->WriteLog( L"[WARNING] This Log file Path is Wong. Check the Logfilepath in Config.ini" );
	}
	g_pLog->WriteLog( L"[SUCC] Create LogFile" );

	if( FALSE == g_pControl->Create( strPath ) )
	{
		g_pLog->WriteLog( L"[FAIL] Create Control"); 
		return SERVER_ERROR_CONFIGINI;
	}

	g_pHackLogFile = CHackLogFile::NewObject();		
	if( NULL == g_pHackLogFile || FALSE == g_pHackLogFile->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Create Hack LogFILE"); 
		return SERVER_ERROR_LOGFILE; 
	}
	g_pLog->WriteLog( L"[SUCC] Create Hack LogFILE");

	//Create LoadOptionDB
	g_pLoadOptionDB = CLoadOptionDB::NewObject();
	if( NULL == g_pLoadOptionDB || FALSE == g_pLoadOptionDB->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Create LoadOptionDB");
		return SERVER_ERROR_LOAD_OPTION_DATABASE; 
	}
	g_pLog->WriteLog( L"[SUCC] Create LoadOptionDB");

	//Create Server Control Socket 
	g_pServerLogFile = CServerLogFile::NewObject();
	if (NULL == g_pServerLogFile || FALSE == g_pServerLogFile->OnCreate())
	{
		g_pLog->WriteLog( L"[FAIL] Create ServerLogFile");
		return SERVER_ERROR_SERVERLOG;
	}
	g_pLog->WriteLog( L"[SUCC] Create ServerLogFile");

	//Create Session Manager
	g_pSessionManager = CSessionManager::NewObject(); 
	if( NULL == g_pSessionManager || FALSE == g_pSessionManager->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Create SessionManager");
		return SERVER_ERROR_SESSION_MANAGER; 
	}
	g_pLog->WriteLog( L"[SUCC] Create SessionManager");

	//Create Web Session
	g_pWebSessionManager = CWebSessionManager::NewObject();
	if( NULL == g_pWebSessionManager || FALSE == g_pWebSessionManager->OnCreate() )
	{
		//Error 
		g_pLog->WriteLog( L"[FAIL] Create WebSessionManager");
		return SERVER_ERROR_WEB_MANAGER; 
	}
	g_pLog->WriteLog( L"[SUCC] Create WebSessionManager");

#ifdef USE_SERVER_WATCH_SAND_UDP
	// Create WatchUDP
	g_pWatch	= CWatch::NewObject();
	if( NULL == g_pWatch || FALSE == g_pWatch->Create() )
	{
		g_pLog->WriteLog( L"[FAIL] Create WatchUDP");
		return SERVER_ERROR_WATCH; 
	}
	g_pLog->WriteLog( L"[SUCC] Create WatchUDP");
#endif

	//Create Knock Server 
	if( g_pControl->m_bActiveKnockServer == TRUE) // Knock Server System을 사용 할 경우만
	{
		g_pModuleKnock = CModuleKnock::NewObject();
		if( NULL == g_pModuleKnock || FALSE == g_pModuleKnock->OnCreate() )
		{
			g_pLog->WriteLog( L"[FAIL] Create ModuleKnock");
			return SERVER_ERROR_KNOCK; 
		}
		g_pLog->WriteLog( L"[SUCC] Create ModuleKnock");
	}

	g_pLog->WriteLog( L"----------------------------------------");
	g_pLog->WriteLog( L"-----------Start Server-----------------");

	return EVENT_ERROR_SUCCESS;
}

void g_UpdateServer(void)
{
	//Time Update 
	g_pControl->UPdateMaxCCU();

	//Log Update 
	//g_pLogFile->OnUpdate(); 

	//Session Update 
	g_pSessionManager->OnUpdate( );

	//Web Session Update
	g_pWebSessionManager->OnUpdate( );

	//ServerLog Update
	if( (g_ui32ServerLogSaveTime + g_pControl->m_ui32UpdateTime) < i3ThreadTimer::GetServerTime() )
	{
	#ifdef USE_SERVER_WATCH_SAND_UDP
		g_pWatch->CheckServerIPUDPSocket( g_pControl->m_ui32IP );
	#endif
		g_pSessionManager->SendServerLog();
		g_ui32ServerLogSaveTime = i3ThreadTimer::GetServerTime();
	}

	return;
}

void g_DestroyServer(void)
{
	if(g_pLog) g_pLog->WriteLog( L"--Start Server Destory--" );

	I3_SAFE_RELEASE(g_pServerLogFile);
	if (g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy ServerLogFile");

	I3_SAFE_RELEASE( g_pLoadOptionDB );
	if(g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy LoadOptionDB" );

	I3_SAFE_RELEASE( g_pSessionManager );
	if(g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy SessionManager" );

	I3_SAFE_RELEASE( g_pWebSessionManager );
	if(g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy WebSessionManager" );

	I3_SAFE_RELEASE( g_pHackLogFile ); 
	if(g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy Hack Log File" );

#ifdef USE_SERVER_WATCH_SAND_UDP
	I3_SAFE_RELEASE( g_pWatch );
	if(g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy Watch" );	
#endif

	if(g_pLog) g_pLog->WriteLog( L"[SUCC] Destroy Control" );
	if(g_pLog) g_pLog->WriteLog( L"--------------------- Control Server End ---------------------" );

	Sleep(1000);
	
	I3_SAFE_RELEASE( g_pLog );
	I3_SAFE_RELEASE( g_pControl );

	i3Net::Destroy();
	i3mem::Destroy();

	i3ThreadTimer::Destroy();

	return; 
}

