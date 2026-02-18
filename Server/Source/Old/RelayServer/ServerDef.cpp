#include "pch.h"
#include "ServerDef.h"
#include "Config.h"
#include "LogFile.h"
#include "Gateway.h"
#include "PacketLocker.h"
#include "TaskProcessor.h"
#include "UdpServer.h"
#include "GroupMaker.h"
#include "ModuleControl.h"
#include "ModuleLog.h"

#include <pdh.h>
#include <pdhmsg.h>

UINT32	g_ConnectServer = 0;
char	g_pCCurrentPath[] = {0,};
BOOL	g_bPath = FALSE;
INT32	g_i32StartServer		= SERVER_STATE_NONE;
UINT32	g_ui32StartTime			= 0;
BOOL	g_PdhStatusError		= FALSE;

UINT8	g_ui8ConBuildVerMonth	= 0;
UINT8	g_ui8ConBuildVerDay		= 0;

static UINT32	s_LastLogUpdateTime_State = 0; 
static HQUERY	s_hCpuQuery = NULL;
static HCOUNTER	s_hCpuCounter;
static HQUERY	s_hHandleQuery = NULL;
static HCOUNTER	s_hHandleCounter;


INT32 g_InitServer(void)
{
	// i3 엔진 초기화
	i3MemoryInit(0);
	i3Error::Init();

	// Expection 생성
	char szPath[ MAX_PATH ];
	sprintf( szPath, "%s%s", g_pCCurrentPath, "\\PBRelays.dmp" );
	i3Error::SetExceptionHandler( szPath );

	// Config.ini 설정파일 초기화
	char szFileName[SERVER_STRING_COUNT];
	sprintf(szFileName, "%s\\Config.ini", g_pCCurrentPath);

	g_pConfig = CConfig::NewObject();
	I3ASSERT(g_pConfig != NULL);
	if( FALSE == g_pConfig->LoadFromFile(szFileName))
	{
		return SERVER_ERROR_CONFIG;
	}

	g_pConfig->RenewTime();

	// 로그파일 초기화
	g_pLogFile = CLogFile::NewObject(); 
	if( FALSE == g_pLogFile->OnCreate())
	{
		return SERVER_ERROR_LOGFILE; 
	}

	if (!i3Net::Init())
	{
		WRITE_LOG_MAIN("[INIT] i3Net::Init() Fail");
		return SERVER_ERROR_NETWORK;
	}

	
	// GroupMaker
	//g_pGroupMaker = CGroupMaker::NewObject();
	
	//if (!g_pGroupMaker->Create())
	//{
	//	return SERVER_ERROR_GROUPMAKER;
	//}

	// UDP Module
	//g_pUdpServer = CUdpServer::NewObject();
	//if( FALSE== g_pUdpServer->Create( g_pConfig->m_cUdpPortCount, g_pConfig->m_UdpIp, g_pConfig->m_pUdpPort ) )
	//{
	//	return SERVER_ERROR_UDPMODULE;
	//}

	// GroupMaker
	g_pGroupMaker = CGroupMaker::NewObject();

	//ModuleLogD
	g_pModuleLog = CModuleLog::NewObject(); 
	if( !g_pModuleLog->OnCreate( ) )
	{
		WRITE_LOG_MAIN("[INIT] Error Create Module LogD");
		return SERVER_ERROR_LOG; 
	}
	WRITE_LOG_MAIN("[INIT] Create Module LogD");

	//Create System Query
	{
		char Temp[SERVER_STRING_COUNT]; 

		PDH_STATUS		pdhStatus;		
#ifdef UNICODE
		WCHAR16 WTemp[SERVER_STRING_COUNT]; 
#endif
		//Create CPU Speed
		pdhStatus = PdhOpenQuery(NULL, NULL, &s_hCpuQuery);
		if (ERROR_SUCCESS != pdhStatus) g_PdhStatusError = TRUE;//return SERVER_ERROR_SYSTEMLOG; 		
		strcpy(Temp, "\\Processor(_Total)\\% Processor Time");
#ifdef UNICODE		
		i3String::CopyASCIItoWCHAR(WTemp, Temp); 
		pdhStatus = PdhAddCounter(s_hCpuQuery, WTemp, 0, &s_hCpuCounter);		
#else 
		pdhStatus = PdhAddCounter(s_hCpuQuery, Temp, 0, &s_hCpuCounter);	
#endif
		if (ERROR_SUCCESS != pdhStatus)	g_PdhStatusError = TRUE;//return SERVER_ERROR_SYSTEMLOG; 
		pdhStatus = PdhCollectQueryData(s_hCpuQuery);
		if (ERROR_SUCCESS != pdhStatus)	g_PdhStatusError = TRUE;//return SERVER_ERROR_SYSTEMLOG; 

		//Create Handle Count 
		pdhStatus = PdhOpenQuery(NULL, NULL, &s_hHandleQuery);
		if (ERROR_SUCCESS != pdhStatus) g_PdhStatusError = TRUE;//return SERVER_ERROR_SYSTEMLOG; 		
		strcpy(Temp, "\\Process(_Total)\\Handle Count");
#ifdef UNICODE		
		i3String::CopyASCIItoWCHAR(WTemp, Temp); 
		pdhStatus = PdhAddCounter(s_hHandleQuery, WTemp, 0, &s_hHandleCounter);		
#else 
		pdhStatus = PdhAddCounter(s_hHandleQuery, Temp, 0, &s_hHandleCounter);	
#endif
		if (ERROR_SUCCESS != pdhStatus)	g_PdhStatusError = TRUE;//return SERVER_ERROR_SYSTEMLOG; 
		pdhStatus = PdhCollectQueryData(s_hHandleQuery);
		if (ERROR_SUCCESS != pdhStatus)	g_PdhStatusError = TRUE;//return SERVER_ERROR_SYSTEMLOG;         
	}
	if(TRUE == g_PdhStatusError)
	{
		g_pConfig->m_iCpuQuery = 0;
		g_pConfig->m_iHandleQuery = 0;				
		WRITE_LOG_MAIN("[Virtual Server] SERVER_ERROR_SYSTEMLOG!!!");
	}
	WRITE_LOG_MAIN("[INIT] Create System Query");

	//Create Control 
	g_pModuleControl = CModuleControl::NewObject(); 
	if( !g_pModuleControl->OnCreate() )
	{
		WRITE_LOG_MAIN("[INIT] Error Create Control");
		return SERVER_ERROR_CONTROLSERVER; 
	}	
	WRITE_LOG_MAIN("[INIT] Create Control");

	g_i32StartServer	= SERVER_STATE_LOADING;
	g_ui32StartTime		= g_pConfig->GetServerTime();

	return EVENT_ERROR_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_i32StartServer = SERVER_STATE_INITING;
	
	if( FALSE == g_pLogFile->OnServerCreate( g_pConfig->m_cUdpPortCount ) )
	{
		WRITE_LOG_MAIN("[START] Error Create Log");
		return SERVER_ERROR_LOG;
	}
	WRITE_LOG_MAIN("[START] Create Log");

	if (!g_pGroupMaker->Create())
	{
		WRITE_LOG_MAIN("[START] Error Create GroupMaker");
		return SERVER_ERROR_GROUPMAKER;
	}
	WRITE_LOG_MAIN("[START] Create GroupMaker");

	// UDP Module
	g_pUdpServer = CUdpServer::NewObject();
	if( FALSE== g_pUdpServer->Create( g_pConfig->m_cUdpPortCount, g_pConfig->m_UdpIp, g_pConfig->m_pUdpPort ) )
	{
		WRITE_LOG_MAIN("[START] Error Create UdpServer");
		return SERVER_ERROR_UDPMODULE;
	}

	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료.
	if( GAME_VER_MONTH != g_ui8ConBuildVerMonth || GAME_VER_DAY != g_ui8ConBuildVerDay )	 
	{
		char strTemp[MAX_PATH];
		sprintf_s( strTemp, MAX_PATH, "Build version is different with Control Server - Control Server : %d / %d , Relay Server : %d / %d ", g_ui8ConBuildVerMonth, g_ui8ConBuildVerDay, GAME_VER_MONTH, GAME_VER_DAY); 
		WRITE_LOG_MAIN( strTemp );

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION; 
	}

	WRITE_LOG_MAIN("[START] Create UdpServer");
	
	g_i32StartServer = SERVER_STATE_START;

	return EVENT_ERROR_SUCCESS;
}

void g_UpdateServer()
{
	g_pConfig->RenewTime(); 

	switch( g_i32StartServer )
	{
	case SERVER_STATE_INIT:
		if( EV_FAILED( g_StartServer() ) )
		{
			g_DestroyServer();
			i3mem::Destroy();
			exit(0);
		}
		break;
	case SERVER_STATE_START:
		{
			g_pLogFile->OnUpdate();
			
			if( (g_pConfig->GetServerTime() - s_LastLogUpdateTime_State ) > LOG_UPDATE_TIME_STATE)
			{
				PDH_STATUS				pdhStatus;		
				DWORD					ctrType;
				PDH_FMT_COUNTERVALUE	fmtValue;

				s_LastLogUpdateTime_State = g_pConfig->GetServerTime(); 

				if(FALSE == g_PdhStatusError)
				{
					//1. CPU 점유율 			
					pdhStatus = PdhCollectQueryData (s_hCpuQuery);		
					pdhStatus = PdhGetFormattedCounterValue (s_hCpuCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue);
					g_pConfig->m_iCpuQuery = (INT32)fmtValue.doubleValue; 

					//2. Handle Count 
					pdhStatus = PdhCollectQueryData (s_hHandleQuery);
					pdhStatus = PdhGetFormattedCounterValue (s_hHandleCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue);
					g_pConfig->m_iHandleQuery = (INT32)fmtValue.doubleValue; 
				}

				//3. Send Message 
				g_pModuleLog->SendLogMessage( LOG_MESSAGE_SERVER_STATE );					//서버상태

			}	
		}
		break;
	default:
		{
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 60 < g_pConfig->GetServerTime() )
			{
				WRITE_LOG_MAIN("[INIT_LOG]--ERROR-- CONTROL SERVER Connect Fail");
				// 게임 종료
				g_DestroyServer();
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	}
	return;
}

void g_DestroyServer(void)
{	
	I3_SAFE_RELEASE(g_pModuleLog);
	WRITE_LOG_MAIN("--Destroy LogD");
	
	I3_SAFE_RELEASE(g_pModuleControl);
	WRITE_LOG_MAIN("--Destroy Control");
	
	I3_SAFE_RELEASE(g_pUdpServer);
	WRITE_LOG_MAIN("--Destroy UdpModule");

	WRITE_LOG_MAIN("--Destroy TaskProcessor");	
	if (g_pTaskProcessor != NULL)
	{
		g_pTaskProcessor->Terminate();
		I3_SAFE_RELEASE(g_pTaskProcessor);
	}

	WRITE_LOG_MAIN("--Destroy Gateway");
	if (g_pGateway != NULL)
	{
		g_pGateway->Terminate();
		I3_SAFE_RELEASE(g_pGateway);
	}

	WRITE_LOG_MAIN("--Destroy PacketLocker");
	I3_SAFE_RELEASE(g_pPacketLocker);

	WRITE_LOG_MAIN("--Destroy GroupMaker");
	I3_SAFE_RELEASE(g_pGroupMaker);

	WRITE_LOG_MAIN("--Destroy LogFile");
	I3_SAFE_RELEASE(g_pLogFile);
	I3_SAFE_RELEASE(g_pConfig);

	i3Net::Destroy(); 
	i3mem::Destroy();
}

void g_SetCurrentPath(char * pPath)
{
	//Set Char 
	i3String::Copy(g_pCCurrentPath, pPath, sizeof(g_pCCurrentPath)); 

	g_bPath = TRUE; 
	return; 
}