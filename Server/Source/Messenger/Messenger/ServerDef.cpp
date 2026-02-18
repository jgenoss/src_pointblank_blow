#include "pch.h"
#include "Config.h"
#include "DataBase.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "TaskProcessor.h"
#include "ModuleControl.h"

#include "ConnectedUser.h"

//User PDH
#include <pdh.h>
#include <pdhmsg.h>

#include <Psapi.h>
#include <tlhelp32.h>
#include <windows.h>

//////////////////////////////////////////////////////////////////////////

UINT32			g_ui32ConnectServer		= 0;
char			g_pCCurrentPath[]		= {0,}; 
SERVER_STATE	g_eServerState			= SERVER_STATE_NONE;
UINT32			g_ui32StartTime			= 0;
INT32			g_i32MainLogIdx			= 0;
i3LogFileW*		g_pLog					= NULL;

void g_SetCurrentPath(char * pPath)
{
	//Set Char 
	i3String::Copy(g_pCCurrentPath, pPath, SERVER_STRING_COUNT); 
}

INT32 g_InitServer(void)
{
	char strPath[ SERVER_STRING_COUNT ];

	// Create Memory
	i3MemoryInit(0); 

	// Debug Info
	i3Error::Init();

	// Expection 생성
	i3String::Format( strPath, MAX_PATH, "%s\\%s", g_pCCurrentPath, "PBMess.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Timer Init
	if( false == i3ThreadTimer::Init() )											return SERVER_ERROR_TIMER;

	//Network Init
	if( FALSE == i3Net::Init() )													return SERVER_ERROR_NETWORK;

	// Create ContextFile 
	g_pConfig = CConfig::NewObject();
	if( NULL == g_pConfig )															return SERVER_ERROR_CONFIGINI;

	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_pCCurrentPath, "Config.ini" );
	INT32 i32LogResult = g_pConfig->LoadLogFile( strPath );
	if( SERVER_ERROR_NO != i32LogResult )											return i32LogResult;
	
	g_pLog = i3LogFileW::NewObject();
	if( NULL == g_pLog )															return SERVER_ERROR_LOGFILE;
	g_pLog->SetVersion( VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL );
	if( FALSE == g_pLog->Create( g_pConfig->m_wstrLogFilePath, L"Mess" ) )			return SERVER_ERROR_LOGFILE;
	
	g_i32MainLogIdx = g_pLog->AddLogThread( L"INIT", 0, GetCurrentThreadId() );
	g_pLog->WriteLog( L"--------------------- Mess Server Create ---------------------" );
	
	if( FALSE == g_pConfig->LoadConfigFile( strPath ) )
	{
		g_pLog->WriteLog( L"[FAIL] Load Config Info");
		return SERVER_ERROR_CONFIGINI;
	}
	g_pLog->WriteLog( L"[SUCC] Load Config Info");

	//Create Control 
	g_pModuleControl = CModuleControl::NewObject(); 
	if( FALSE == g_pModuleControl->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] CREATE MODULE CONTROL FAIL"  );
		return SERVER_ERROR_CONTROLSERVER; 
	}
	g_pLog->WriteLog( L"[SUCC] CREATE MODULE CONTROL"  );

	g_eServerState		= SERVER_STATE_LOADING;
	g_ui32StartTime		= i3ThreadTimer::GetServerTime();
	g_pLog->WriteLog( L"[SUCC] Init Complete" );

	//////////////////////////////////////////////////////////////////////////

	return EVENT_ERROR_SUCCESS;
}

INT32 g_StartServer(void)
{
	g_eServerState = SERVER_STATE_INITING;

	g_i32MainLogIdx = g_pLog->AddLogThread( L"MAIN", 0, GetCurrentThreadId() );


	// Create CPU Speed
	if( EVENT_ERROR_SUCCESS != g_pModuleControl->InitSystemLog() )
	{
		g_pLog->WriteLog( L"[FAIL] Start Control SystemLog" );
	}
	g_pLog->WriteLog( L"[SUCC] Start Control SystemLog" );

	g_pLog->WriteLog( L"[SUCC] CREATE GAME LOG FILE"  );
	
	{
		g_pDataBase = CDataBase::NewObject();
		if( FALSE == g_pDataBase->OnCreate() )
		{
			g_pLog->WriteLog( L"[FAIL] CREATE DATABASE"  );
			return SERVER_ERROR_DATABASE; 
		}
	}
	g_pLog->WriteLog( L"[SUCC] CREATE DATABASE"  );

	//Create Packet Locker
	g_pPacketLocker = CPacketLocker::NewObject();
	if(FALSE == g_pPacketLocker->Create(g_pConfig->m_ui32ServerCount, 200))
	{
		g_pLog->WriteLog( L"[FAIL] CREATE PACKET LOCKER"  );
		return SERVER_ERROR_PACKETLOCKER;
	}
	g_pLog->WriteLog( L"[SUCC] CREATE PACKET LOCKER"  );
	
	//Create GateWay
    g_pGateway = CGateway::NewObject();
	if(FALSE == g_pGateway->Create(g_pConfig->m_strIpAddress, g_pConfig->m_ui16Port, g_pPacketLocker))
	{
		g_pLog->WriteLog( L"[FAIL] CREATE GATEWAY"  );
		return SERVER_ERROR_GATEWAY;
	}
	g_pLog->WriteLog( L"[SUCC] CREATE GATEWAY"  );

	//Create Task
	g_pTaskProcessor = CTaskProcessor::NewObject();
	if(FALSE == g_pTaskProcessor->Create(PACKETBUFFERSIZE, g_pPacketLocker, g_pGateway))
	{
		g_pLog->WriteLog( L"[FAIL] CREATE TASK"  );
		return SERVER_ERROR_TASK;
	}
	g_pLog->WriteLog( L"[SUCC] CREATE TASK"  );
	
	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료. (New)
	if( FALSE == g_pConfig->m_Version.VersionCompare( &g_pConfig->m_ControlVersion ) )
	{
		g_pLog->WriteLog( L"[FAIL] Build version is different with Control Server - Control Server : %d.%d , Messenger Server : %d.%d",
			g_pConfig->m_ControlVersion._ui8VerGame, g_pConfig->m_ControlVersion._ui16VerSS,
			g_pConfig->m_Version._ui8VerGame, g_pConfig->m_Version._ui16VerSS);

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION;
	}
	
	g_pLog->WriteLog( L"--------------------------- Create finish ----------------------------"  );
	g_pLog->WriteLog( L"----------------------- Start Messenger Server -----------------------"  );
	
	g_eServerState = SERVER_STATE_START;

	return EVENT_ERROR_SUCCESS;
}

void g_UpdateServer()
{
	switch( g_eServerState )
	{
	case SERVER_STATE_INIT:
		{
			INT32 i32Result = g_StartServer();
			g_pModuleControl->ServerInit( i32Result );

			if( i32Result != EVENT_ERROR_SUCCESS )
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
			#ifdef DEF_CPU_LOG
			//4. CPU, Handle, Memory Size 로그
			// 프로세스가 사용하는 메모리정보 얻는 함수 ( 핸들로 찾는다.)
			//	[출처] 특정 프로세스가 사용하는 메모리 사용량 구하기|작성자 이듄기님
			PROCESS_MEMORY_COUNTERS pmc = {0};			
			GetProcessMemoryInfo(s_hHandleProcess, &pmc, sizeof(pmc));
			I3BCLOG(LOCATION, "CPU(%d),Handle(%d),Memory(%d)", g_pConfig->m_iCpuQuery, g_pConfig->m_iHandleQuery, (int)((float)pmc.WorkingSetSize));
			#endif
		}
		break;
	default:
		{
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 60 < i3ThreadTimer::GetServerTime() )
			{
				g_pLog->WriteLog( L"[ERROR] CONTROL SERVER Connect Fail"  );
				// 게임 종료
				g_DestroyServer(SERVER_ERROR_CONTROLSERVER);
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	}

	return; 
}

void g_DestroyServer(INT32 i32DestroyReason)
{
	if( g_pModuleControl )
	{
		UINT8	ui8Type = ASC_SERVER_MESSENGER;

		// Control 서버에 정상 종료 패킷 전송.
		i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		SendPacket.WriteData( &ui8Type, sizeof(UINT8));
		SendPacket.WriteData( &i32DestroyReason, sizeof(INT32));
		g_pModuleControl->SendPacketMessage( &SendPacket );

		Sleep(1000);
	}


	g_pLog->WriteLog( L"------------------------- End Messenger Server -------------------------"  );

	I3BCLOG(LOCATION, "[%02d%02d%02d][%02d:%02d:%02d]---------END--------------------",
		i3ThreadTimer::LocalTime().GetYear(),
		i3ThreadTimer::LocalTime().GetMonth(),
		i3ThreadTimer::LocalTime().GetDay(),
		i3ThreadTimer::LocalTime().GetHour(),
		i3ThreadTimer::LocalTime().GetMinute(),
		i3ThreadTimer::LocalTime().GetSecond() );

	if (g_pLog)	g_pLog->WriteLog( L"[SUCC] Destroy Control"  );
	I3_SAFE_RELEASE(g_pModuleControl);

	if (g_pLog)	g_pLog->WriteLog( L"[SUCC] Destroy TaskProcessor"  );
	if (g_pTaskProcessor != NULL)
	{
		g_pTaskProcessor->Terminate();
		I3_SAFE_RELEASE(g_pTaskProcessor);
	}

	if (g_pLog)	g_pLog->WriteLog( L"[SUCC] Destroy Gateway"  );
	if (g_pGateway != NULL)
	{
		g_pGateway->Terminate();
		I3_SAFE_RELEASE(g_pGateway);
	}

	if (g_pLog)	g_pLog->WriteLog( L"[SUCC] Destroy PacketLocker"  );
	I3_SAFE_RELEASE(g_pPacketLocker);

	if (g_pLog)	g_pLog->WriteLog( L"[SUCC]Destroy DataBase"  );
	I3_SAFE_RELEASE(g_pDataBase);

	if (g_pLog)	g_pLog->WriteLog( L"[SUCC] Destroy LogFile"  );
	I3_SAFE_RELEASE(g_pLog);


	I3_SAFE_RELEASE(g_pConfig);
	
	i3Net::Destroy();
	i3ThreadTimer::Destroy();

#if 0
	i3ElementBase::DumpObjects();
#endif

	return; 
}