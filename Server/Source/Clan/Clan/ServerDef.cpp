#include "pch.h"
#include "Config.h"
#include "DataBase.h"
//#include "LogFile.h"
#include "PacketLocker.h"
#include "Gateway.h"
#include "TaskProcessor.h"
#include "ModuleControl.h"
#include "MatchingManager.h"


UINT32			g_ConnectServer			= 0; 
char			g_pCCurrentPath[]		= {0,}; 
SERVER_STATE	g_eServerState			= SERVER_STATE_NONE;
UINT32			g_ui32StartTime			= 0;
INT32				g_i32MainLogIdx			= 0;
i3LogFileW*			g_pLog					= NULL;

void g_SetCurrentPath(char * pPath)
{
	//Set Char 
	i3String::Copy(g_pCCurrentPath, pPath, sizeof(g_pCCurrentPath)); 
}

INT32 g_InitServer(void)
{
	//_CrtSetBreakAlloc( 617 );
	 
	char strPath[ SERVER_STRING_COUNT ];

	// Create Memory
	i3MemoryInit(0); 

	// Debug Info
	i3Error::Init();

	// Expection 생성
	i3String::Format( strPath, MAX_PATH, "%s\\%s", g_pCCurrentPath, "PBClans.dmp" );
	i3Error::SetExceptionHandler( strPath );

	//Timer Init
	if( false == i3ThreadTimer::Init() )				return SERVER_ERROR_TIMER;

	//Network Init
	if( FALSE == i3Net::Init() )						return SERVER_ERROR_NETWORK;

	// Create ContextFile 
	g_pConfig = CConfig::NewObject();
	if( NULL == g_pConfig )															return SERVER_ERROR_CONFIGINI;

	//Load Log info from Config
	i3String::Format( strPath, SERVER_STRING_COUNT, "%s\\%s", g_pCCurrentPath, "Config.ini" );
	INT32 i32LogResult = g_pConfig->LoadLogFile( strPath );
	if( SERVER_ERROR_NO != i32LogResult )											return i32LogResult;
	
	g_pLog = i3LogFileW::NewObject();
	if( NULL == g_pLog )															return SERVER_ERROR_LOGFILE;
	g_pLog->SetVersion( VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL );
	if( FALSE == g_pLog->Create( g_pConfig->m_wstrLogFilePath, L"Clan" ) )			return SERVER_ERROR_LOGFILE;
	
	g_i32MainLogIdx = g_pLog->AddLogThread( L"INIT", 0, GetCurrentThreadId() );
	g_pLog->WriteLog( L"--------------------- Clan Server Create ---------------------" );
		
	// Load Config
	if( FALSE == g_pConfig->LoadConfigFile( strPath ) )
	{
		g_pLog->WriteLog( L"[ERRO] Load Config" );
		return SERVER_ERROR_CONFIGINI;
	}
	g_pLog->WriteLog( L"[SUCC] Load Config" );
	
	//Module Control
	g_pModuleControl = CModuleControl::NewObject(); 
	if( FALSE == g_pModuleControl->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] ModulControl Fail");
		return SERVER_ERROR_CONTROLSERVER; 
	}
	g_pLog->WriteLog( L"[SUCC] ModulControl Create");

	g_eServerState		= SERVER_STATE_LOADING;
	g_ui32StartTime		= i3ThreadTimer::GetServerTime();
	g_pLog->WriteLog( L"[SUCC] Init Complete" );

	return EVENT_ERROR_SUCCESS;
}

INT32 g_StartServer(void)
{
	//Memory Leak test
	//_crtBreakAlloc = 170428;

	g_eServerState = SERVER_STATE_INITING;

	g_i32MainLogIdx = g_pLog->AddLogThread( L"MAIN", 0, GetCurrentThreadId() );

	// Create CPU Speed
	if( EVENT_ERROR_SUCCESS != g_pModuleControl->InitSystemLog() )
	{
		g_pLog->WriteLog( L"[FAIL] Start Control SystemLog" );
	}
	g_pLog->WriteLog( L"[SUCC] Start Control SystemLog" );

	//Create DataBase
	g_pDataBase = CDataBase::NewObject(); 
	if( FALSE == g_pDataBase->OnCreate() )
	{
		g_pLog->WriteLog( L"[FAIL] Database Create Fail");
		return SERVER_ERROR_DATABASE; 
	}
	g_pLog->WriteLog( L"[SUCC] Database Create");

	// TaskPump
	UINT32 ui32receiverCount	= g_pConfig->m_ui32ServerCount;
	UINT32 ui32lockerCount		= ui32receiverCount + 1;	// RecursivePacket
	
	g_pPacketLocker = CPacketLocker::NewObject();
	g_pPacketLocker->Create(ui32lockerCount, 1000);
	
    g_pGateway = CGateway::NewObject();
	if ( FALSE == g_pGateway->Create( g_pConfig->m_ui32IPAddress, g_pConfig->m_ui16Port, g_pPacketLocker ) )
	{ 
		g_pLog->WriteLog( L"[FAIL] GateWay Create Fail");
		return SERVER_ERROR_GATEWAY; 
	}
	g_pLog->WriteLog( L"[SUCC] GateWay Create");

	g_pMatchingManager = CMatchingManager::NewObject();
	if( !g_pMatchingManager->Create(g_pConfig->GetServerCount()) )
	{
		g_pLog->WriteLog( L"[ERROR] Failed to create matching manager.");
		return SERVER_ERROR_MATCHING_MANAGER; 
	}

	g_pTaskProcessor = CTaskProcessor::NewObject();
	if ( FALSE == g_pTaskProcessor->Create(PACKETBUFFERSIZE, g_pPacketLocker, g_pGateway) )
	{ 
		g_pLog->WriteLog( L"[FAIL] TaskProcessor Create Fail");
		return SERVER_ERROR_TASKPROCESSOR; 
	}

	g_pLog->WriteLog( L"[SUCC] TaskProcessor Create");

	g_pGateway->Resume();

	g_pConfig->SetClanCount(g_pTaskProcessor->GetClanCount());
	g_pLog->WriteLog( L"[INIT] TaskPump on");	

	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료. (New)
	if ( (g_pConfig->m_ControlVersion._ui8VerGame != g_pConfig->m_ClanVersion._ui8VerGame) || (g_pConfig->m_ControlVersion._ui16VerSS != g_pConfig->m_ClanVersion._ui16VerSS) ) 
	{
		
		g_pLog->WriteLog( L"[INIT_LOG] Build version is different with Control Server - Control Server : %d.%d , Clan Server : %d.%d",
			g_pConfig->m_ControlVersion._ui8VerGame, g_pConfig->m_ControlVersion._ui16VerSS,
			g_pConfig->m_ClanVersion._ui8VerGame, g_pConfig->m_ClanVersion._ui16VerSS);

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION; 
	}

	

	g_pLog->WriteLog( L"---- Start Clan Server ----");

	// 서버 로딩 완료	

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
				g_DestroyServer( i32Result );
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
				g_pLog->WriteLog( L"[INIT_LOG]--ERROR-- CONTROL SERVER Connect Fail");
				// 게임 종료
				g_DestroyServer( SERVER_ERROR_CONTROLSERVER );
				i3mem::Destroy();
				exit(0);
			}
		}
		break;
	}

	return;
}

void	g_DestroyServer(INT32 i32DestroyReason)
{	
	if( g_pModuleControl )
	{
		UINT8 ui8Type = ASC_SERVER_CLAN;

		// Control 서버에 정상 종료 패킷 전송.
		i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		SendPacket.WriteData( &ui8Type, sizeof(UINT8));
		SendPacket.WriteData( &i32DestroyReason, sizeof(INT32));
		g_pModuleControl->SendPacketMessage( &SendPacket );
		Sleep(1000);
	}
	
	I3_SAFE_RELEASE( g_pModuleControl );
	if( g_pLog) g_pLog->WriteLog( L"[DES_LOG] Control");
	
	I3_SAFE_RELEASE(g_pTaskProcessor);
	if( g_pLog) g_pLog->WriteLog( L"[DES_LOG] Task");
	I3_SAFE_RELEASE(g_pGateway);
	if( g_pLog) g_pLog->WriteLog( L"[DES_LOG] Gateway");
	I3_SAFE_RELEASE(g_pPacketLocker);
	if( g_pLog) g_pLog->WriteLog( L"[DES_LOG] Packet");
	I3_SAFE_RELEASE(g_pMatchingManager);
	if( g_pLog) g_pLog->WriteLog( L"[DES_LOG] MatchingManager");

	I3_SAFE_RELEASE( g_pDataBase ); 
	if( g_pLog) g_pLog->WriteLog( L"[DES_LOG] Database");

	if( g_pLog) g_pLog->WriteLog( L"--------------------------------");
	I3_SAFE_RELEASE( g_pLog );

	I3_SAFE_RELEASE( g_pConfig ); 
	
	i3Net::Destroy();
	i3ThreadTimer::Destroy();

#if 0
	i3ElementBase::DumpObjects();
#endif

	return;
}


//void SendMessageToControl(UINT32 ui32ControlIP, UINT16 ui16ControlPort, INT32 i32DestroyReason)
//{
//	UINT8 ui8Type = ASC_SERVER_CLAN;
//
//	i3NetworkClientSocket tempSocket;
//	tempSocket.OnCreate(ui32ControlIP, ui16ControlPort) ;
//	
//	// Control 서버에 정상 종료 패킷 전송.
//	i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
//	SendPacket.WriteData( &ui8Type, sizeof(UINT8));
//	SendPacket.WriteData( &i32DestroyReason, sizeof(INT32));
//	tempSocket.SendPacketMessage( &SendPacket );
//	Sleep(1000);
//}