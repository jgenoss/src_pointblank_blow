// TaskMgr.cpp
//
// YouJong Ha
//	Last update : 
//		2010-10-11 task thread create 함수 수정
//		2010-10-01 (yyyy:mm:dd)
// Description:
//	
//
//
// Dependency: 
//		
//	
#include "pch.h"
#include "TaskMgr.h"
#include "TaskProcessor.h"
#include "TrackBuffer.h"
#include "PacketLocker.h" // send/recv queue

CTaskMgr* g_pTaskMgr = NULL;						// 전역 변수

// ---------------------------------------------------------------------------
// static variables 
// ---------------------------------------------------------------------------
HANDLE	CTaskMgr::m_ahTaskThreads[ DS_MAX_TASK_THREAD_COUNT];			// task processor threads
INT32	CTaskMgr::m_i32TaskThreadCount;
INT32	CTaskMgr::m_i32GroupMax;
UINT32	CTaskMgr::m_ui32PollingTime;

///////////////////////////////////////////////////////////////////////////////
//
// Function: CTaskMgr
//
// Description:
//	- constructor
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
CTaskMgr::CTaskMgr(void)
{
	i3mem::FillZero( m_ahTaskThreads, sizeof(m_ahTaskThreads) );	// Task threads
	m_i32TaskThreadCount = 0;										// # of thread objects
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//

CTaskMgr::~CTaskMgr(void)
{
	DWORD ui32ExitCode = 0;

	// 종료 이벤트가 signaled 상태인지 검사
	if( WaitForSingleObject( g_hevtQuit, 0 ) != WAIT_OBJECT_0 )
	{
		I3TRACE("CTaskMgr::~CTaskMgr(): g_hevtQuit is not signal state\n");
	}
	
	// 스레드가 종료하기를 기다린다
	WaitForMultipleObjects( m_i32TaskThreadCount, m_ahTaskThreads, TRUE, 1000);
	
	// 리턴 코드를 검사하고 여전히 동작중이라면 강제 종료시킨다
	// 1. task processor threads
	for( INT32 i = 0; i < m_i32TaskThreadCount; i++)
	{
		GetExitCodeThread( m_ahTaskThreads[i], &ui32ExitCode);
		if( ui32ExitCode == STILL_ACTIVE )
		{
			TerminateThread( m_ahTaskThreads[i], (DWORD)-1);
		}
		m_ahTaskThreads[i] = NULL;
	}
}

BOOL CTaskMgr::Create (
						INT32	i32TaskThreadCount, 
						INT32	i32GroupCount, 
						UINT32	ui32PollingTime
					  ) 
{
	// copy value to member	
	m_i32TaskThreadCount		= i32TaskThreadCount;
	m_i32GroupMax				= i32GroupCount;
	m_ui32PollingTime			= ui32PollingTime;
	
	// Task Processor
	for( INT32 i = 0 ; i < m_i32TaskThreadCount; i++ )
	{
		 TASK_PROCESSOR_PARAM * pTaskParam = new TASK_PROCESSOR_PARAM;
		
		pTaskParam->m_i32ThreadIdx				= i;
		pTaskParam->m_i32TaskThreadCount		= m_i32TaskThreadCount;
		pTaskParam->m_i32GroupMax				= m_i32GroupMax;
		pTaskParam->m_ui32PollingTime			= m_ui32PollingTime;
		
		m_ahTaskThreads[ i ] = CreateThread( NULL, 0, CTaskProcessor::TaskProcessorThread, pTaskParam, 0, NULL ) ;
		if ( NULL == m_ahTaskThreads[ i ] )
		{
			DSERROR( "[CTaskMgr::Create] CreateThread failed: TASK_PROCESSOR: %d\n", i);
			return FALSE;
		}
	}

	return TRUE;
}
