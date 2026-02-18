#include "pch.h"
#include "Config.h"
#include "ModuleControl.h"
#include "TaskProcessor.h"

char	g_pCCurrentPath[]		= {0,}; 
INT32	g_i32MainLogThreadIdx	= LOG_THREAD_INVALID;
INT32	g_i32StartServer		= SERVER_STATE_NONE;
UINT32	g_ui32StartTime			= 0;

void g_SetCurrentPath( char * pPath )
{
	//Set Char 
	i3String::Copy( g_pCCurrentPath, pPath, sizeof(g_pCCurrentPath) ); 
	return;
}

INT32 g_InitServer(void)
{
	//////////////////////////////////////////////////////////////
	// Debug Info
	i3Error::Init();
	//////////////////////////////////////////////////////////////
	// Expection 생성
	char szPath[ MAX_PATH ];
	sprintf( szPath, "%s%s", g_pCCurrentPath, "\\Billing.i3b" );
	i3Error::SetExceptionHandler( szPath );

	//////////////////////////////////////////////////////////////
	// Network Init
	if( FALSE == i3Net::Init() )														return SERVER_ERROR_NETWORK;

	//////////////////////////////////////////////////////////////
	// System File
	g_pSystem = CSystem::NewObject();
	if( NULL == g_pSystem )																return SERVER_ERROR_MEMORY;
	if( FALSE == g_pSystem->OnCreate())													return SERVER_ERROR_SYSTEM; 
	g_pSystem->RenewTime();

	//////////////////////////////////////////////////////////////
	// Config File 
	char strTemp[SERVER_STRING_COUNT];
	char strTemp2[SERVER_STRING_COUNT];
	i3String::Format( strTemp, SERVER_STRING_COUNT, "%s\\Config.ini", g_pCCurrentPath );
	i3String::Format( strTemp2, SERVER_STRING_COUNT, "%s\\ConfigReLoad.ini", g_pCCurrentPath );

	g_pConfig = CConfig::NewObject();
	if( NULL == g_pConfig )																return SERVER_ERROR_MEMORY;
	if( FALSE == g_pConfig->OnCreate( strTemp, strTemp2 ) )								return SERVER_ERROR_CONFIG; 
	g_pSystem->RenewTime();

	//////////////////////////////////////////////////////////////
	// Log File
	g_pLog = CModuleLogFile::NewObject();
	if( NULL == g_pLog )																return SERVER_ERROR_MEMORY;
	if( FALSE == g_pLog->OnCreate( g_pConfig->m_strLogFilePath ) )						return SERVER_ERROR_LOGFILE;

	g_i32MainLogThreadIdx = g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == g_i32MainLogThreadIdx )									return SERVER_ERROR_LOGFILE;

	WriteLog( g_i32MainLogThreadIdx, "---------[INIT] Start Init----------------" );
	WriteLog( g_i32MainLogThreadIdx, "[INIT] CreateMoudle LogFile Success" );

	//////////////////////////////////////////////////////////////
	// Config ThreadLoad
	if( FALSE == g_pConfig->OnCreateThread() )											return SERVER_ERROR_CONFIG; 

	//////////////////////////////////////////////////////////////
	// Control Server
	WriteLog( g_i32MainLogThreadIdx, "[INIT] CreateMoudle Control" );
	g_pModuleControl = CModuleControl::NewObject(); 
	if( NULL == g_pModuleControl )														return SERVER_ERROR_MEMORY;
	if( FALSE == g_pModuleControl->OnCreate( g_pConfig->m_ui32ControlServerIP, g_pConfig->m_ui16ControlServerPort ) )
	{
		WriteLog( g_i32MainLogThreadIdx, "[INIT] CreateMoudle Control Fail" );
		return SERVER_ERROR_CONTROLSERVER; 
	}
	WriteLog( g_i32MainLogThreadIdx, "[INIT] CreateMoudle Control Success" );

	WriteLog( g_i32MainLogThreadIdx, "---------[INIT] End Init----------------" );

	g_i32StartServer	= SERVER_STATE_LOADING;
	g_ui32StartTime		= g_pSystem->GetServerTime();

	return EVENT_ERROR_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_i32StartServer = SERVER_STATE_INITING;

	WriteLog( g_i32MainLogThreadIdx, "---------[LOAD] Start Load----------------" );

	//////////////////////////////////////////////////////////////
	 // Task
	WriteLog( g_i32MainLogThreadIdx, "[LOAD] CreateMoudle TaskProcess" );
	g_pTaskProcessor = CTaskProcessor::NewObject();
	if( FALSE == g_pTaskProcessor->OnCreate() )
	{
		WriteLog( g_i32MainLogThreadIdx, "[LOAD] CreateMoudle TaskProcess Fail");
		return SERVER_ERROR_TASK; 
	}
	WriteLog( g_i32MainLogThreadIdx, "[LOAD] CreateMoudle TaskProcess Success");

	WriteLog( g_i32MainLogThreadIdx, "---------[LOAD] End Load----------------" );
	WriteLog( g_i32MainLogThreadIdx, "---------[STAR] Start Server---------------");

	//Create DataBase
	//pUdlFilePath "File Name=C:\\BcDataBase.udl;" <- 이런 형식으로 보내줘야 합니다. 	
	//g_pDataBase = CDataBase::NewObject();
	//if( !g_pDataBase->OnCreate() )
	//{
	//	WriteLog( g_i32MainLogThreadIdx, "[LOAD] Create Database Fail");
	//	return SERVER_ERROR_DATABASE; 
	//}	
	//WriteLog( g_i32MainLogThreadIdx, "[LOAD] Create Database Success");

	////Create DataBase Worker
	//g_pModuleDBWorker = CModuleDBWorker::NewObject(); 
	//if( !g_pModuleDBWorker->OnCreate() )
	//{
	//	WriteLog( g_i32MainLogThreadIdx, "[LOAD] Create Database Work Fail");
	//	return FALSE; 
	//}
	//WriteLog( g_i32MainLogThreadIdx, "[LOAD] Create Database Work Success");

	//g_pPacketLocker = CPacketLocker::NewObject();
	//g_pPacketLocker->Create(i32ServerCount, 1000);
	//WriteLog( g_i32MainLogThreadIdx, "[LOAD] Create PacketLocker Success");

  	//// ModuleLogD
	//g_pModuleLogD = CModuleLog::NewObject(); 
	//if( FALSE == g_pModuleLogD->OnCreate( ) )	
	//{
	//	WriteLog( g_i32MainLogThreadIdx, "[LOAD] CreateModule LogD Fail");
	//	return SERVER_ERROR_LOGD; 
	//}
	//WriteLog( g_i32MainLogThreadIdx, "[LOAD] CreateModule LogD Success");


	g_i32StartServer = SERVER_STATE_START;

	return EVENT_ERROR_SUCCESS;
}

INT32 g_UpdateServer(void)
{
	g_pSystem->RenewTime(); 

	switch( g_i32StartServer )
	{
	case SERVER_STATE_INIT:
		{
			return g_StartServer();
		}
		break;
	case SERVER_STATE_START:
		{
		}
		break;
	default:
		{
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 60 < g_pSystem->GetServerTime() )
			{
				WriteLog( g_i32MainLogThreadIdx, "[INIT] Time Over.. CONTROL SERVER Connect Fail");
				// 게임 종료
				g_DestroyServer();
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	}
	
	return EVENT_ERROR_SUCCESS; 
}

void g_DestroyServer(void)
{
	WriteLog( g_i32MainLogThreadIdx, "---------[Dest] Start End----------------" );
	
	I3_SAFE_RELEASE( g_pTaskProcessor );
	WriteLog( g_i32MainLogThreadIdx, "[Dest] Task Release");

	I3_SAFE_RELEASE( g_pModuleControl );
	WriteLog( g_i32MainLogThreadIdx, "[Dest] Control Release");

	I3_SAFE_RELEASE( g_pConfig );
	WriteLog( g_i32MainLogThreadIdx, "[Dest] Config Release");
	
	I3_SAFE_RELEASE( g_pLog );
	WriteLog( g_i32MainLogThreadIdx, "[Dest] Log Release");

	I3_SAFE_RELEASE( g_pSystem );

	WriteLog( g_i32MainLogThreadIdx, "---------[Dest] End End----------------" );

	i3Net::Destroy(); 	

#if 0
	i3ElementBase::DumpObjects();
#endif

}
