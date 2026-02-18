#include "pch.h"
#include "ServerDef.h"
#include "Config.h"
#include "ModuleNet.h"
#include "ModuleTask.h"
#include "ModuleControl.h"

//User PDH
#include <pdh.h>
#include <pdhmsg.h>


SERVER_STATE		g_eServerState			= SERVER_STATE_NONE;
UINT32				g_ui32StartTime			= 0;

//Program Path
char				g_strCCurrentPath[]		= {0,};
i3LogFileW*			g_pLog					= NULL;

// Steam API
FnSteam_BDecryptTicket		g_pSteam_BDecryptTicket = NULL;
FnSteam_BIsTicketForApp		g_pSteam_BIsTicketForApp = NULL;
FnSteam_GetTicketSteamID	g_pSteam_GetTicketSteamID = NULL;

void g_SetCurrentPath(char * pPath)
{
	//Set Char 
	i3String::Copy( g_strCCurrentPath, pPath, PATH_STRING_COUNT );
}

int g_InitServer(void)
{
	//Set Error Dump File 
	char strPath[ SERVER_STRING_COUNT ];

	//Create Memory
	i3MemoryInit(0);

	//Error Handle Init
	i3Error::Init();

	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_strCCurrentPath, "SIAss.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Timer Init
	if( false == i3ThreadTimer::Init() )													return SERVER_ERROR_TIMER;
		
	//Network Init
	if( FALSE == i3Net::Init() )						return SERVER_ERROR_NETWORK;

	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_strCCurrentPath, "SIAConfig.ini" );
	
	g_pConfig = new CConfig;
	if( NULL == g_pConfig )																	return SERVER_ERROR_LOGFILE;
 	INT32 i32LogResult = g_pConfig->LoadLogInfo( strPath );
	if( SERVER_ERROR_LOGFILE == i32LogResult )												return SERVER_ERROR_LOGFILE;
	
	//2. Create Log Module
	g_pLog = i3LogFileW::NewObject();
	if (NULL == g_pLog)	return SERVER_ERROR_LOGFILE;
	g_pLog->SetVersion(VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL);
	if (FALSE == g_pLog->Create(g_pConfig->m_wstrLogFilePath, L"SIA"))	return SERVER_ERROR_LOGFILE;
	g_pLog->AddLogThread(L"INIT", 0, GetCurrentThreadId());

	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"============================================");
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] CREATE LOG FILE");

	// Load Config
	if( FALSE == g_pConfig->LoadConfigFile( strPath ) )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[ERRO] Load Config" );
		return SERVER_ERROR_CONFIGINI;
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] Load Config" );

	// 통계 오브젝트
	g_pStatistics = new CServerStatistics;
	if (g_pStatistics == NULL)
	{
		g_pLog->WriteLog(L"[ERRO] Init g_pStatistics Fail");
		return SERVER_ERROR_STATISTICS;
	}

	STAT_THREADCOUNT	stStatThread;
	i3mem::FillZero(&stStatThread, sizeof(STAT_THREADCOUNT));
	stStatThread.m_i32Count[STAT_TYPE_LINK_LOGIN] = g_pConfig->m_i32LoginThreadCount;
	stStatThread.m_i32Count[STAT_TYPE_LINK_BILLING] = g_pConfig->m_i32BillingThreadCount;
	stStatThread.m_i32Count[STAT_TYPE_LINK_PC_CAFE] = g_pConfig->m_i32PCCafeThreadCount;

	if (FALSE == g_pStatistics->Create(&stStatThread))
	{
		g_pLog->WriteLog(L"[ERRO] Create g_pStatistics Fail");
		return SERVER_ERROR_STATISTICS;
	}
	
#ifdef USE_STEAM_API
	for (UINT8 i = 0; i < g_pConfig->m_ui8LoginMethodCount; ++i)
	{
		if (LINK_METHOD_NORTHA_STEAM == g_pConfig->m_aeLoginMethod[i])
		{
			g_pSteamAPI = new cSteamAPI;
			if (NULL == g_pSteamAPI || !g_pSteamAPI->OnCreate(g_strCCurrentPath))
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] CREATE SteamAPI");
				return SERVER_ERROR_INIT;
			}
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] CREATE SteamAPI");
			break;
		}
		else if (LINK_METHOD_RUSSIA == g_pConfig->m_aeLoginMethod[i])
		{
			g_pIntegrationAPI = new cIntegrationAPI;
			if (NULL == g_pIntegrationAPI || !g_pIntegrationAPI->OnCreate(g_strCCurrentPath))
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] CREATE IntegrationAPI");
				return SERVER_ERROR_INIT;
			}
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] CREATE IntegrationAPI");

			// Initialize Russia API.
			char strConfigFileLocation[SERVER_STRING_COUNT];
			i3String::Format(strConfigFileLocation, SERVER_STRING_COUNT, "%s\\innovaBilling.cfg", g_strCCurrentPath);

			INT32 i32retValue = g_pInte_IInitializeA(strConfigFileLocation, &g_pConfig->m_ServiceIdForRussia);

			if (i32retValue != 0)
			{
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"------------------------------------------");
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"Innova System API Initialize Failed. [Error Code= %d]", i32retValue);
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"------------------------------------------");

				return SERVER_ERROR_RUSSA_INI;

			}
			else
			{
				// 초기화 성공
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"------------------------------------------");
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"--Innova System API Initialize Success");
				g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"------------------------------------------");
			}
			break;
		}
	}
#endif

	g_pModuleControl = CModuleControl::NewObject();
	if (FALSE == g_pModuleControl->Create(g_pConfig->m_ui32ControlIP, g_pConfig->m_ui16ControlPort))
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] CREATE CONTROL");
		return SERVER_ERROR_CONTROLSERVER; 
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] CREATE CONTROL");


	g_eServerState = SERVER_STATE_LOADING;
	g_ui32StartTime = i3ThreadTimer::GetServerTime();

	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"============================================" );
	
	return EVENT_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_pLog->AddLogThread( L"MAIN", 0, GetCurrentThreadId() );

	g_eServerState = SERVER_STATE_INITING;
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"============================================");
	
	// Create CPU Speed
	if( FALSE == g_pModuleControl->InitSystemLog() )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] Start Control SystemLog" );
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] Start Control SystemLog" );

	// NetWork Module
	g_pModuleNet		= new CModuleNet; 
	if( !g_pModuleNet->OnCreate( g_pConfig->m_pServerIP, g_pConfig->m_i32ServerCount ) )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] CREATE NETWORK");
		return SERVER_ERROR_MODULE_NETWORK; 
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] CREATE NETWORK");

	// Create Task Module
	g_pModuleTask = new CModuleTask; 
	INT32 i32ECode = g_pModuleTask->OnCreate_M();
	if(EVENT_SUCCESS != i32ECode)
	{
		switch(i32ECode)
		{
		case SERVER_ERROR_TASK:
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[g_InitServer()][EVENT_FAIL_CREATE_TASK Fail]");
			break;
		case SERVER_ERROR_LINK:
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[g_InitServer()][EVENT_FAIL_CREATE_LINK Fail]");
			break;
		default:
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[g_InitServer()][Unknown Ecode(%x)", i32ECode);
			break;
		}
		return i32ECode;
	}
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[SUCC] CREATE TASK ");

	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료.
	if( FALSE == g_pConfig->m_Version.VersionCompare( &g_pConfig->m_ControlVersion ) )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] Build version is different with Control Server - Control Server : %d.%d , SIA Server : %d.%d",
			g_pConfig->m_ControlVersion._ui8VerGame, g_pConfig->m_ControlVersion._ui16VerSS,
			g_pConfig->m_Version._ui8VerGame, g_pConfig->m_Version._ui16VerSS );

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION; 
	}
	
	// 서버 로딩 완료
	g_eServerState = SERVER_STATE_START;
	
	g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"===================== Start S.I.A Server =======================");

	return EVENT_SUCCESS;
}

void g_UpdateServer(void)
{
	switch( g_eServerState )
	{
	case SERVER_STATE_INIT:
		{
			INT32	i32Rv = g_StartServer();
			g_pModuleControl->ServerInit( 0 );

			if( i32Rv != EVENT_SUCCESS)
			{
				//에러가 난 경우
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[FAIL] g_ServerStart()");
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
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[INITIA] CONTROL SERVER Connect Fail");
				
				// 게임 종료
				g_DestroyServer(SERVER_ERROR_CONTROLSERVER);
				exit(0);
			}
		}
		break;
	}

	return; 
}

void g_DestroyServer(INT32 i32DestroyReason)
{
	//Control 서버로, 정상 종료 패킷만 보내던것을 에러가 갈때도 보내도록 변경.
	if( g_pModuleControl )
	{
		UINT8 ui8Type = ASC_SERVER_SIA;

		NET_I3_PACKET_BUFFER SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		SendPacket.WriteData( &ui8Type, sizeof(UINT8));
		SendPacket.WriteData( &i32DestroyReason, sizeof(INT32));
		g_pModuleControl->SendPacketMessage( (char* )&SendPacket, SendPacket._ui16TotalSize );

		Sleep(1000);
	}


	if(g_pLog)g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"============================================");
	I3_SAFE_DELETE( g_pModuleTask ); 
	if(g_pLog)g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"DESTROY TASK");
	I3_SAFE_DELETE( g_pModuleNet );	
	if(g_pLog)g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"DESTROY NETWORK");
	I3_SAFE_RELEASE( g_pModuleControl );
	if(g_pLog)g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"DESTROY Control");
	
#ifdef USE_STEAM_API
	I3_SAFE_DELETE( g_pSteamAPI );
	if (g_pLog)g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"DESTROY SteamAPI");
#endif
	for (UINT8 i = 0; i < g_pConfig->m_ui8LoginMethodCount; ++i)
	{
		if (LINK_METHOD_RUSSIA == g_pConfig->m_aeLoginMethod[i])
		{
			if(g_pIntegrationAPI->GetDll())
			{
				g_pInte_IUninitialize();
				if (g_pLog) g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"--Innova System API Uninitialize");
			}
			
		}
	}
	I3_SAFE_DELETE( g_pIntegrationAPI );
	if(g_pLog) g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"--DESTROY IntegrationAPI" );

	I3_SAFE_DELETE(g_pStatistics);
	if (g_pLog) g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"--DESTROY Statistics");


	if(g_pLog)g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"============================================"); 

	Sleep(100); 
	I3_SAFE_RELEASE( g_pLog );

	I3_SAFE_DELETE( g_pConfig );

	i3ThreadTimer::Destroy();

	return; 
}


char* g_Int32ToStringIP( UINT32 ui32IP )
{
	in_addr InAddr;
	InAddr.S_un.S_addr = ui32IP;
	return inet_ntoa( InAddr );
}