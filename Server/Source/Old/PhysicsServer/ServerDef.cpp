#include "pch.h"	// precompiled header
#include "ServerDef.h"	

#include <pdh.h>	// Performance Data Helper
#include <pdhmsg.h>

// 유틸리티
#include "ServerStatistics.h"
#include "LogFile.h"

// IO 작업을 위한 헤더파일
#include "UdpBufferPool.h"
#include "TrackBuffer.h"
#include "PacketLocker.h"
#include "UdpService.h"
#include "TaskMgr.h"
#include "MapMgr.h"		// 충돌, 오브젝트, 애니메이션 리소스 관리
#include "NxGlobal.h"

// 전역 변수 선언
HANDLE g_hevtQuit = NULL;

TCHAR g_pCCurrentPath[SERVER_STRING_COUNT] = { '\0',};				// path
static BOOL	g_bPath = FALSE;									// path setting
INT32	g_i32StartServer		= SERVER_STATE_NONE;			// server state: {NONE|LOADING|INIT|INITING|START}
static UINT32	g_ui32StartTime			= 0;					// start time

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
	// 통계 오브젝트
	g_pStatistics = new CServerStatistics;
	if( g_pStatistics == NULL )
	{						 
		return SERVER_ERROR_STATISTICS;
	}

	// PhysX 초기화
	if( FALSE == InitNx() )
	{
		return SERVER_ERROR_PHYSICS;
	}
		
	// i3 엔진 초기화
	i3MemoryInit(0);
	i3Error::Init();

	char szPath[ MAX_PATH ] = { '\0', };
	// Expection 생성
	{
		sprintf_s(szPath, MAX_PATH, "%s%s", g_pCCurrentPath, "\\PBPxs.dmp" );
		i3Error::SetExceptionHandler( szPath );
	}

	// i3 네트워크 초기화
	if (!i3Net::Init())
	{
		return SERVER_ERROR_NETWORK;
	}
		
	// Config.ini 파일 처리
	sprintf_s(szPath, MAX_PATH, "%s\\Config.ini", g_pCCurrentPath);
	g_pConfig = CConfig::NewObject();
	I3ASSERT(g_pConfig != NULL);
	if( FALSE == g_pConfig->LoadFromFile(szPath))
	{
		return SERVER_ERROR_CONFIG;
	}

	g_pConfig->RenewTime();	// 서버 시작 시간 설정 
	
	// 로그파일 초기화
	g_pLogFile = CLogFile::NewObject(); 
	I3ASSERT( g_pLogFile != NULL );
	if( FALSE == g_pLogFile->OnCreate())
	{
		return SERVER_ERROR_LOGFILE; 
	}

	/////////////////////////////////////////////
	// Data structure setup
	
	// data queue
	g_pDataArchive = CPacketLocker::NewObject();
	I3ASSERT(g_pDataArchive != NULL);
	if( FALSE == g_pDataArchive->Create( STAGE_ORD_MAX - 1, 10000)) // max stage
	{
		return SERVER_ERROR_UDPMODULE;	
	}
	
	// send queue
	g_pSendQueue = CTrackBuffer::NewObject();
	if( NULL  == g_pSendQueue || 
		FALSE == g_pSendQueue->Create( 3000 ))
	{
		return SERVER_ERROR_MEMORY;	
	}
	
	//// recv queue
	//g_pRecvQueue = CTrackBuffer::NewObject();
	//if( NULL == g_pRecvQueue || 
	//	FALSE == g_pRecvQueue->Create( 3000 ) ) 
	//{
	//	return SERVER_ERROR_MEMORY;	
	//}

	// memory pool
	g_pUdpBufferPool = CUdpBufferPool::NewObject();
	if( NULL == g_pUdpBufferPool  || 
		FALSE == g_pUdpBufferPool->Create( 10000 ) )
	{
		return SERVER_ERROR_MEMORY;	
	}
	
	g_i32StartServer	= SERVER_STATE_INIT; // 데디케이션에서는 컨트롤 서버 접속 성공시 셋팅된다
	g_ui32StartTime		= g_pConfig->GetServerTime();

	return EVENT_ERROR_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: g_StartServer
//
// Description:
//	- iocp server와 task manager를 setup 한다
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- SERVER_ERROR_xxxx 에러 코드 (성공: EVENT_ERROR_SUCCESS)
//
INT32 g_StartServer(void)
{
	g_i32StartServer = SERVER_STATE_INITING;

	// 서버 종료를 위한 이벤트 객체 생성
	g_hevtQuit = CreateEvent(NULL, TRUE, FALSE, NULL);

	// map manager setup
	g_pMapMgr = new CMapMgr;
	I3ASSERT( g_pMapMgr != NULL);
	if( FALSE == g_pMapMgr->Init() )
	{
		return SERVER_ERROR_RESOURCE;
	}

	// Task Processor Manager
	g_pTaskMgr = new CTaskMgr;
	I3ASSERT(g_pTaskMgr != NULL );
	if( FALSE == g_pTaskMgr->Create(	g_pConfig->m_i32TaskThreadCount, 
										g_pConfig->m_ui32UpdateTime,
										g_pConfig->m_ui32EpsilonTime ) )
	{
		return SERVER_ERROR_TASKMANAGER;
	}

	// udp server start
	g_pUdpService = new CUdpService;
	I3ASSERT(g_pUdpService != NULL);
	if( FALSE == g_pUdpService->Create()  )
	{
		return SERVER_ERROR_UDPSERVICE;
	}
	
	g_i32StartServer = SERVER_STATE_START;

	DSERROR("Server Started Successfully...\n");

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
void g_UpdateServer()
{
	g_pConfig->RenewTime(); 

	switch( g_i32StartServer )
	{
	case SERVER_STATE_INIT:
		g_StartServer();
		break;
	case SERVER_STATE_START:
#ifdef _DEBUG		
		//g_pStatistics->PrintStaticsticsInfo();
#endif
		g_pLogFile->OnUpdate();
		g_pMapMgr->OnUpdate_Main();
		break;
	default:
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

void g_DestroyServer(void)
{	
	SetEvent(g_hevtQuit);				// signal to quit
	Sleep(500);
	
	SAFE_DELETE(g_pUdpService);
	DSMSG("[MAIN] --Destroy Udp Service\n");
	
	SAFE_DELETE(g_pTaskMgr);
	DSMSG("[MAIN] --Destroy Udp Task Manager\n");	
	
	DSMSG("[MAIN] --Destroy SendQueue\n");
	I3_SAFE_RELEASE(g_pSendQueue);

	DSMSG("[MAIN] --Destroy DataArchive\n");
	I3_SAFE_RELEASE(g_pDataArchive);
	
	DSMSG("[MAIN] --Destroy UdpBufferPool\n");
	I3_SAFE_RELEASE(g_pUdpBufferPool);

	DSMSG("[MAIN] --Destroy MapManager\n");
	SAFE_DELETE( g_pMapMgr );
	
	DSMSG("[MAIN] --Destroy LogFile\n");
	I3_SAFE_RELEASE(g_pLogFile);
	I3_SAFE_RELEASE(g_pConfig);

	SAFE_DELETE( g_pStatistics );

	i3Net::Destroy(); 
	i3mem::Destroy();

	// PhysX 해제
	ReleaseNx();

	// 서버 종료를 위한 이벤트 객체 삭제
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
	strcpy_s( g_pCCurrentPath, sizeof(g_pCCurrentPath), pPath ); 
	g_bPath = TRUE; 
}

