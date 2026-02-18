#include "pch.h"	// precompiled header
#include "ServerDef.h"	

#include <pdh.h>	// Performance Data Helper
#include <pdhmsg.h>

// 유틸리티
#include "ServerStatistics.h"

// IO 작업을 위한 헤더파일
#include "iocpServer.h"
#include "UdpBufferPool.h"
#include "PacketLocker.h"
#include "PositionQueue.h"
#include "TaskMgr.h"

// GROUP 관련 헤더파일
#include "MapManager.h"	// 충돌, 오브젝트, 애니메이션 리소스 관리
#include "GroupMaker.h"	
#include "DediRoom.h"
#include "DediMember.h"

// 다른 서버와의 통신
#include "ModuleControl.h"		// Control Server
#include "ModuleCast.h"
#include <ctime>
#include <time.h>

#include "MapManager.h"

// xml parsing for hack checking
#include "XmlWrapper.h"
#include "WeaponTable.h"
#include "CharacterTable.h"
#include "EquipmentTable.h"
#include "SkillTable.h"
#include "StageTable.h"

#include "InterlockedList.h"

#include "ModuleControlSocket.h"
#include "NationalCodeDef.h"

void g_InitMapBounds();

// 전역 변수 선언
HANDLE g_hevtQuit = NULL;

TCHAR			g_pCCurrentPath[SERVER_STRING_COUNT] = {0,};			// path
SERVER_STATE	g_eServerState			= SERVER_STATE_NONE;			// server state: {NONE|LOADING|INIT|INITING|START}
UINT32			g_ui32StartTime			= 0;							// start time

// 핵 이름
char* g_pszHackName[] = {
	"",
	"Gravity Hack",
	"Damage Hack",
	"Range Hack",
	"Speed Hack",
	"Wall Shot",
	"Grenade Hack",
	"Magazine Hack",
	"Ghost Hack",
	"Recall Hack",
	"Weapon Hack",
	"Training Mode Hack",
	"Fire Speed Hack",
	"Invincibility Hack",
	"Respawn Time Hack",
	"Respawn Position Hack",
	"C4 Install Hack",
	"Client Time Hack",
	"Head Shot Hack",
	"Item Check",
	"Weapon Data Hack",
	"Weapon Not Found",
	"UDP Version",
	"GM Pause Move Hack",
	"GM Pause Shoot Hack",
	"Usurpation Check",
	"GrenadePos Check",
	"RPG Cheat",
	"Space Cheat"
};

void SendMessageToControl( UINT32 ui32IP, UINT16 ui16ControlPort, INT32 i32DestroyReason);

///////////////////////////////////////////////////////////////////////////////
//
// Function: g_InitServer
//
// Description:
//	- 서버 초기화 작업
//		- i3 엔진/에러/네트워크 객체 초기화
//      - exception 생성
//		- config.ini 처리
//		- 서버 객체 생성
//			. log
//			. send queue
//			. data queue
//			. memory pool
//			. group setup
// Arguments:
//	- 
//		- 
// Return:
//	- SERVER_ERROR_xxxx 에러 코드 (성공: EVENT_ERROR_SUCCESS)
//
INT32 g_InitServer(void)
{
	//_CrtSetBreakAlloc( 20292 );
	
	srand( (unsigned)time( NULL ) );

	// i3 엔진 초기화
	i3MemoryInit(0);

	// Error Expection 생성
	i3Error::Init();
	char szPath[ MAX_PATH ] = { '\0', };
	{
		i3String::Format( szPath, MAX_PATH, "%s%s", g_pCCurrentPath, "\\PBDedis.dmp" );
		i3Error::SetExceptionHandler( szPath );
	}

	//Create Thread Timer
	if (!i3ThreadTimer::Init())
	{
		return SERVER_ERROR_TIMER;
	}

	// i3 네트워크 초기화
	if (!i3Net::Init())
	{
		return SERVER_ERROR_NETWORK;
	}
		
	//Config.ini 파일 처리
	i3String::Format(szPath, MAX_PATH, "%s\\Config.ini", g_pCCurrentPath);
	g_pConfig = CConfig::NewObject();
	I3ASSERT(g_pConfig != NULL);

	if( FALSE == g_pConfig->LoadConfigFile(szPath))
	{
		return SERVER_ERROR_CONFIGINI;
	}
	

	// 통계 오브젝트
	g_pStatistics = new CServerStatistics;
	if( g_pStatistics == NULL )
	{
		return SERVER_ERROR_STATISTICS;
	}

	g_pConfig->SetServerStartTime();

	// 로그파일 초기화
	g_pLogFile = new CLogFile; //CLogFile::NewObject(); 
	I3ASSERT( g_pLogFile != NULL );
	if( FALSE == g_pLogFile->OnCreate(LOG))
	{
		return SERVER_ERROR_LOGFILE; 
	}
	
	// 로그파일 초기화
	g_pHackLogFile = new CLogFile; //CLogFile::NewObject(); 
	I3ASSERT( g_pHackLogFile != NULL );
	if( FALSE == g_pHackLogFile->OnCreate(HACKLOG))
	{
		return SERVER_ERROR_LOGFILE; 
	}
	
	
	if ( sizeof(g_pszHackName)/sizeof(char*) != HACK_TYPE_MAX )
	{
		return SERVER_ERROR_NOT_ENOUGH_HACK_NAME;
	}

	

	// PhysX 초기화
	if( g_pConfig->m_bPhysicsActive )
	{
		if( ! InitNx() )
		{
			return SERVER_ERROR_PHYSICS;
		}
	}

	// 룸매니저
	// 함수 밑단으로 내리면 안됩니다.14.03.14확인
	// 밑단으로 내릴경우 컨트롤서버 패킷이 먼저 들어와 크래쉬 발생
	g_pRoomManager = CGroupMaker::NewObject();
	I3ASSERT(g_pRoomManager != NULL);

	// 메모리 검사
	g_CheckSystemMemory();

	/////////////////////////////////////////////
	// Data structure setup
	
	// data queue
	g_pDataArchive = CPacketLocker::NewObject();
	I3ASSERT(g_pDataArchive != NULL);
	if( FALSE == g_pDataArchive->Create(g_pConfig->m_i32RoomCount, MAX_TRACK_BUFFER_COUNT)) // max room can be reached to 4000
	{
		return SERVER_ERROR_UDPMODULE;	
	}

	// memory pool
	g_pUdpBufferPool = CUdpBufferPool::NewObject();
	I3ASSERT(g_pUdpBufferPool != NULL);
	if( FALSE == g_pUdpBufferPool->Create(MAX_UDP_BUFFER_COUNT))
	{
		return SERVER_ERROR_UDPMODULE;	
	}

	//cheating : stage boundary setup
	g_InitMapBounds(); 
	
	// Create ControlServer Connect
	g_pModuleControl = CModuleControl::NewObject(); 

	if( EVENT_ERROR_SUCCESS != g_pModuleControl->OnCreate( g_pConfig->m_ui32ControlServerIP, g_pConfig->m_ui16ControlServerPort, g_pConfig->m_i32TaskThreadCount ) )
	{
		return SERVER_ERROR_CONTROLSERVER; 
	}

	g_eServerState		= SERVER_STATE_LOADING;
	g_ui32StartTime		= i3ThreadTimer::GetServerTime();

	return EVENT_ERROR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: g_StartServer
//
// Description:
//	- iocp server와 task manager를 setup 한다
//	- 컨트롤 서버로부터 설정 정보를 받은 후에 실행된다	- 
//	
// Return:
//	- SERVER_ERROR_xxxx 에러 코드 (성공: EVENT_ERROR_SUCCESS)
//

INT32 g_StartServer(void)
{
	g_eServerState = SERVER_STATE_INITING;

	// 서버 종료를 위한 이벤트 객체 생성
	g_hevtQuit = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Create CPU Speed
	if( EVENT_ERROR_SUCCESS != g_pModuleControl->InitSystemLog() )
	{
		DSMUST( "[ERRO] Start Control SystemLog\n" );
	}
	DSMUST( "[SUCC] Start Control SystemLog\n" );

	g_pModuleCast = CModuleCast::NewObject();
	if( (NULL == g_pModuleCast ) ||
		( EVENT_ERROR_SUCCESS != g_pModuleCast->OnCreate( g_pConfig->m_ui32CastServerIP, g_pConfig->m_ui16CastServerPort, g_pConfig->m_i32TaskThreadCount ) ) )
	{
		DSMUST( "[ERRO] Start Cast Module\n" );
		return SERVER_ERROR_CAST;
	}
	DSMUST( "[SUCC] Start Cast Module\n" );

	// Map manager setup [ Object & Collision & Respawn ]
	g_pMapManager = new CMapManager;
	I3ASSERT( g_pMapManager != NULL);
	if( FALSE == g_pMapManager->Init() )
	{
		return SERVER_ERROR_RESOURCE;
	}
	
	// Room setup

	if( FALSE == g_pRoomManager->Create())
	{
		return SERVER_ERROR_GROUPMAKER;
	}

	// Set the national code to load
	// National Code
	// get national code from config.ini (default setting is philiphine..because of test)
	UINT8 ui8NationalCode = g_pConfig->m_ui8NationalCode;  
	BOOL bLoading = FALSE;

	if( ui8NationalCode >= NationalCode_Max )
	{
		::DSMUST("[ERROR_LOG] Check the [HackCheck] section in config.ini file!\n");
		return SERVER_ERROR_XMLFILENATIONALCODE;
	}
	
	//----------------------------------------------------------
	// 1. Load Weapon - Weapon.i3RegXML
	//----------------------------------------------------------

	XmlDocumentWrapper xmlDoc1, xmlDoc2, xmlDoc3, xmlDoc4, xmlDoc5, xmlDoc6;
	
	TCHAR strWeaponXMLPath[MAX_PATH];
	i3String::Format(strWeaponXMLPath, sizeof(strWeaponXMLPath), "%s\\XML\\Weapon.i3RegXML", g_pCCurrentPath); 

	// load the xml file (weapon.xml)
	bLoading = xmlDoc1.Load(strWeaponXMLPath);
	if(!bLoading) 
	{
		::DSMUST("[ERROR_LOG] Weapon.i3RegXML file (in XML folder) loading fail! Check dedi server directory!\n");
		return SERVER_ERROR_XMLFILELOADING;
	}
	
	// Set National Code
	WeaponTable::GetInstance().SetNationalCode(ui8NationalCode); 
	
	// Parse xml file data
	WeaponTable::GetInstance().ParseWeaponTable(xmlDoc1.AsNode());
	WeaponTable::GetInstance().ParseWeaponTable_New();
	
	//----------------------------------------------------------
	// 2. Load Character - Character.i3RegXML
	//----------------------------------------------------------
	
	TCHAR strCharacterXMLPath[MAX_PATH];
	i3String::Format(strCharacterXMLPath, sizeof(strCharacterXMLPath), "%s\\XML\\Character.i3RegXML", g_pCCurrentPath); 

	// load the xml file(character.xml)
	bLoading = xmlDoc2.Load(strCharacterXMLPath);
	if(!bLoading)
	{
		::DSMUST("[ERROR_LOG] Character.i3RegXML file (in XML folder) loading fail! Check dedi server directory!\n");
		return SERVER_ERROR_XMLFILELOADING;
	}

	// Set National Code
	CharacterTable::GetInstance().SetNationalCode(ui8NationalCode);
	
	// Parse xml file data
	CharacterTable::GetInstance().ParseCharacterTable(xmlDoc2.AsNode());
	
	//----------------------------------------------------------
	// 3. Load Equipment - Equipment.i3RegXML
	//----------------------------------------------------------
	
	TCHAR strEquipmentXMLPath[MAX_PATH];
	i3String::Format( strEquipmentXMLPath, sizeof( strEquipmentXMLPath), "%s\\XML\\Equipment.i3RegXML", g_pCCurrentPath);
	
	bLoading = xmlDoc3.Load( strEquipmentXMLPath );
	if( !bLoading )
	{
		::DSMUST("[ERROR_LOG] Equipment.i3RegXML file (in XML folder) loading fail! Check dedi server directory!\n");
		return SERVER_ERROR_XMLFILELOADING;
	}
	
	// set national code
	EquipmentTable::GetInstance().SetNationalCode( ui8NationalCode );
	
	// parse xml file data
	EquipmentTable::GetInstance().ParseEquipmentTable(xmlDoc3.AsNode());

	//----------------------------------------------------------
	// 4. Load Skill - Skill.i3RegXML
	//----------------------------------------------------------
	TCHAR strSkillXMLPath[MAX_PATH];
	i3String::Format( strSkillXMLPath, sizeof( strSkillXMLPath), "%s\\XML\\Skill.i3RegXML", g_pCCurrentPath);

	bLoading = xmlDoc5.Load( strSkillXMLPath );
	if( !bLoading )
	{
		::DSMUST("[ERROR_LOG] Skill.i3RegXML file (in XML folder) loading fail! Check dedi server directory!\n");
		return SERVER_ERROR_XMLFILELOADING;
	}

	// set national code
	SkillTable::GetInstance().SetNationalCode( ui8NationalCode );

	// parse xml file data
	SkillTable::GetInstance().ParseSkillInfo(xmlDoc5.AsNode());
	//----------------------------------------------------------

	//----------------------------------------------------------
	// 6. Load Stage - Stage.i3RegXML
	//----------------------------------------------------------
	TCHAR strStageXMLPath[MAX_PATH];
	i3String::Format( strStageXMLPath, sizeof( strStageXMLPath), "%s\\XML\\Stage.i3RegXML", g_pCCurrentPath);

	bLoading = xmlDoc6.Load( strStageXMLPath );
	if( !bLoading )
	{
		::DSMUST("[ERROR_LOG] Stage.i3RegXML file (in XML folder) loading fail! Check dedi server directory!\n");
		return SERVER_ERROR_XMLFILELOADING;
	}

	// set national code
	StageTable::GetInstance().SetNationalCode( ui8NationalCode );

	// parse xml file data
	StageTable::GetInstance().ParseStageInfo(xmlDoc6.AsNode());
	//----------------------------------------------------------
	
	
	// Task Processor Manager
	g_pTaskMgr = new CTaskMgr;
	if( NULL == g_pTaskMgr )
	{
		return SERVER_ERROR_TASKMANAGER;
	}

	if( FALSE == g_pTaskMgr->Create(	g_pConfig->m_i32TaskThreadCount, 
										g_pConfig->m_i32RoomCount,
										g_pConfig->m_dwUpdateTime 
									)	)
	{
		DSMUST("[g_StartServer()] TaskMgr Create failed\n");
		return SERVER_ERROR_TASKMANAGER;
	}

	DSMUST("[g_StartServer()] TaskMgr Create Success\n");

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// iocp server setup
	g_pIocpServer = new CIocpServer( g_pUdpBufferPool );
	if( NULL == g_pIocpServer )
	{
		return SERVER_ERROR_IOCPSERVER;
	}
	
	DSMUST("[g_StartServer()] IocpServer new Success\n");

	if( FALSE == g_pIocpServer->Create( g_pConfig->m_ui32UdpPortCount, g_pConfig->m_ui32UdpIp, g_pConfig->m_pui16UdpPort, g_pConfig->m_ui32IoThreadCount) )
	{
		DSMUST("[g_StartServer()] IocpServer Create failed\n");
		return SERVER_ERROR_IOCPSERVER;
	}

	DSMUST("[g_StartServer()] IocpServer Create Success\n");

	// 컨트롤 서버와 빌드 버전 비교후 다르다면 서버 종료. (New)
	if( FALSE == g_pConfig->m_Version.VersionCompare( &g_pConfig->m_ControlVersion ) )
	{
		char strTemp[MAX_PATH];
		i3String::Format( strTemp, MAX_PATH, "Build version is different with Control Server - Control Server : %d.%d , Dedication Server : %d.%d",
			g_pConfig->m_ControlVersion._ui8VerGame, g_pConfig->m_ControlVersion._ui16VerSS,
			g_pConfig->m_Version._ui8VerGame, g_pConfig->m_Version._ui16VerSS);
		DSMUST(strTemp);

		return SERVER_ERROR_DIFFERENT_BUILD_VERSION; 
	}

	// 서버 로딩 완료
	g_eServerState = SERVER_STATE_START;
	
	DSMUST("---------------- START DEDI SERVER ---------------------\n");

	return EVENT_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: g_UpdateServer
//
// Description:
//	- 메인 쓰레드가 SERVER_MAIN_UPDATE_TIME 마다 주기적으로 호출한다
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
UINT32 g_ui32LastPrintTime = 0;
extern UINT32	g_aui32ProcessCount[64];

void g_UpdateServer()
{
	switch( g_eServerState )
	{
	case SERVER_STATE_INIT:
		{
			INT32 i32Result = g_StartServer();
			g_pModuleControl->ServerInit( MAIN_THREAD_IDX, i32Result );
			g_pModuleCast->ServerInit( MAIN_THREAD_IDX, i32Result );

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
			g_pIocpServer->SetPacketCount_C();

			g_pLogFile->OnUpdate();

			g_pHackLogFile->OnUpdate();

			//g_pRoomManager->OnUpdate_Main();

			g_pMapManager->OnUpdate_Main();

			//g_pStatistics->PrintStatistics();
		}
		break;
	default:
		{
		#ifndef	_DEBUG
			// Control 서버와 연결 안됨
			if( g_ui32StartTime + 360 < i3ThreadTimer::GetServerTime() )
			{
				DSMUST("[MAIN] [INIT_LOG]--ERROR-- CONTROL SERVER Connect Fail\n");
				// 게임 종료
				g_DestroyServer(SERVER_ERROR_CONTROLSERVER);
				i3mem::Destroy();
				exit(0);		
			}
		#endif
		}
		break;
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: g_DestroyServer
//
// Description:
//	- 서버 종료
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//

void g_DestroyServer(INT32 i32DestroyReason)
{
	// Control 서버에 정상 종료 패킷 전송.
	if (g_pModuleControl)
	{
		UINT8 ui8Type = ASC_SERVER_BATTLE;

		i3NetworkPacket SendPacket(PROTOCOL_ASC_NORMAL_DISCONNECT_REQ);
		SendPacket.WriteData(&ui8Type, sizeof(UINT8));
		SendPacket.WriteData(&i32DestroyReason, sizeof(INT32));
		g_pModuleControl->SendPacketMessage(&SendPacket);
		Sleep(1000);
	}

	SetEvent(g_hevtQuit);				// signal to quit
	Sleep(500);

	SAFE_DELETE(g_pIocpServer);
	DSMUST("[MAIN] --Destroy UdpModule\n");

	SAFE_DELETE(g_pTaskMgr);
	DSMUST("[MAIN] --Destroy Udp Task Manager\n");

	I3_SAFE_RELEASE(g_pModuleControl);
	DSMUST("[MAIN] --Destroy Control\n");

	SAFE_DELETE(g_pMapManager);
	DSMUST("[MAIN] --Destroy MapManager\n");

	I3_SAFE_RELEASE(g_pDataArchive);
	DSMUST("[MAIN] --Destroy DataArchive\n");

	SAFE_DELETE(g_pPhysicsResult);
	DSMUST("[MAIN] --Destroy PhysicsResult\n");

	I3_SAFE_RELEASE(g_pUdpBufferPool);
	DSMUST("[MAIN] --Destroy UdpBufferPool\n");

	I3_SAFE_RELEASE(g_pRoomManager);
	DSMUST("[MAIN] --Destroy GroupMaker\n");

	I3_SAFE_RELEASE(g_pModuleCast);
	DSMUST("[MAIN] --Destroy Cast\n");

	DSMUST("[MAIN] --Destroy LogFile\n");
	SAFE_DELETE(g_pLogFile);

	DSMUST("[MAIN] --Destroy LogFile\n");
	SAFE_DELETE(g_pHackLogFile);


	// PhysX 해제
	if (g_pConfig)
	{

		if (g_pConfig->m_bPhysicsActive)
		{
			ReleaseNx();
		}
	}

	//I3_SAFE_RELEASE(g_pConfig);
	SAFE_DELETE(g_pConfig);

	i3Net::Destroy(); 
	i3mem::Destroy();

	i3ThreadTimer::Destroy();

	SAFE_DELETE( g_pStatistics );
	
	CloseHandle(g_hevtQuit);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: g_SetCurrentPath
//
// Description:
//	- 프로그램 시작시 작업 경로를 설정(dlg: InitDialog, console, service: main)
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
void g_SetCurrentPath(LPCTSTR pPath)
{
	strcpy_s( g_pCCurrentPath, sizeof(g_pCCurrentPath), pPath);
}

//Log Setup후에 호출해야 한다.
#define DIV 1024
#define WIDTH 7
BOOL g_CheckSystemMemory()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// 32bit window check
	if( si.lpMaximumApplicationAddress < (LPVOID)0x80000000ULL ) //2GB
	{
		DSMUST("WARNING: Maximum Application Address is below 2G: %I64d kb\n", (ULONGLONG)si.lpMaximumApplicationAddress );
		//return FALSE;
	}
	
	// log system memory usage 
	 MEMORYSTATUSEX statex;

	 statex.dwLength = sizeof (statex);

	 GlobalMemoryStatusEx (&statex);

	 if( statex.ullAvailPhys < 0x80000000ULL ) //2GB
	 {
		  DSMUST ("There is  %*ld percent of memory in use.\n",
				  WIDTH, statex.dwMemoryLoad);
		  DSMUST ("There are %*I64d total Kbytes of physical memory.\n",
				  WIDTH, statex.ullTotalPhys/DIV);
		  DSMUST ("There are %*I64d free Kbytes of physical memory.\n",
				  WIDTH, statex.ullAvailPhys/DIV);
		  DSMUST ("There are %*I64d total Kbytes of paging file.\n",
				  WIDTH, statex.ullTotalPageFile/DIV);
		  DSMUST ("There are %*I64d free Kbytes of paging file.\n",
				  WIDTH, statex.ullAvailPageFile/DIV);
		  DSMUST ("There are %*I64d total Kbytes of virtual memory.\n",
				  WIDTH, statex.ullTotalVirtual/DIV);
		  DSMUST ("There are %*I64d free Kbytes of virtual memory.\n",
				  WIDTH, statex.ullAvailVirtual/DIV);

		  // Show the amount of extended memory available.
		  DSMUST ("There are %*I64d free Kbytes of extended memory.\n",
				  WIDTH, statex.ullAvailExtendedVirtual/DIV);
		  // return FALSE;
	 }

	return TRUE;
}


void SendMessageToControl( UINT32 ui32IP, UINT16 ui16ControlPort, INT32 i32DestroyReason)
{
	UINT8 ui8Type = ASC_SERVER_BATTLE;

	CModuleControlSocket tempSocket;
	tempSocket.OnCreate( ui32IP, ui16ControlPort);

	i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
	SendPacket.WriteData( &ui8Type, sizeof(UINT8));
	SendPacket.WriteData( &i32DestroyReason, sizeof(INT32));
	tempSocket.SendPacketMessage( &SendPacket );
	Sleep(1000);
}