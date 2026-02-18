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
#include "LogFile.h"

#include "TaskMgr.h"
#include "TaskProcessor.h"
#include "MonitorThread.h"
#include "TrackBuffer.h"


CTaskMgr* g_pTaskMgr = NULL;						// 전역 변수

// ---------------------------------------------------------------------------
// static variables 
// ---------------------------------------------------------------------------
HANDLE	CTaskMgr::m_phTaskThreads[ PX_MAX_TASK_THREAD_COUNT ];			// task processor threads
INT32	CTaskMgr::m_i32TaskThreadCount;
UINT32	CTaskMgr::m_ui32PollingTime;
UINT32	CTaskMgr::m_ui32EpsilonTime;
HANDLE	CTaskMgr::m_hTaskMonitorThread;		// monitor threads


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
	memset( m_phTaskThreads, 0, sizeof(m_phTaskThreads) );			// Task threads
	m_i32TaskThreadCount	= 0;									
	m_hTaskMonitorThread	= NULL;	
	m_ui32PollingTime		= PX_TASK_TIMEOUT;
	m_ui32EpsilonTime		= 0;
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
	// signaled 상태인지 검사
	if( WaitForSingleObject( g_hevtQuit, 16) != WAIT_OBJECT_0 )
	{
		DSMSG("[CTaskMgr::~CTaskMgr] g_hevtQuit is not signal state\n");
	}
	
	// 스레드가 종료하기를 기다린다
	WaitForMultipleObjects( m_i32TaskThreadCount, m_phTaskThreads, TRUE, 1000);
	
	// 리턴 코드를 검사하고 여전히 동작중이라면 강제 종료시킨다
	// 1. monitor threads
	DWORD exitCode = 0;
	GetExitCodeThread(m_hTaskMonitorThread, &exitCode);
	if( exitCode == STILL_ACTIVE )
	{
		TerminateThread(m_hTaskMonitorThread, (DWORD)-1);
	}
	
	// 2. task processor threads
	for( INT32 i = 0; i < m_i32TaskThreadCount; i++)
	{
		GetExitCodeThread(m_phTaskThreads[i], &exitCode);
		if( exitCode == STILL_ACTIVE )
		{
			TerminateThread(m_phTaskThreads[i], (DWORD)-1);
		}
		m_phTaskThreads[i] = NULL;
	}
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Create
//
// Description:
//	- launch child threads
//		- task threads
//      - monitor thread
//		- security threads
// Arguments:
//	- ui32TaskThreadCount		: # of task threads
//	- ui32GroupCount			: # of max group
//	- ui32PollingTime			: polling time (20ms? 현재는 64ms)
//	- ui32SecurityThreadCount	: # of security threads
// Return:
//	- succss / failure
//
BOOL CTaskMgr::Create (
						INT32	i32TaskThreadCount, 
						UINT32	ui32PollingTime,
						UINT32	ui32EpsilonTime
					  ) 
{
	I3ASSERT( i32TaskThreadCount > 0 );
	I3ASSERT( ui32PollingTime	 > 0);

	m_i32TaskThreadCount		= i32TaskThreadCount;
	m_ui32PollingTime			= ui32PollingTime;
	m_ui32EpsilonTime			= ui32EpsilonTime;
	
	// Task Monitor
	MONITOR_THREAD_PARAM* pMonitorParam = new MONITOR_THREAD_PARAM;
	if( pMonitorParam == NULL )
	{
		DSERROR("[CTaskMgr::Create] Memory Alloc Error\n");
		return FALSE;
	}

	pMonitorParam->m_hevtQuit			= g_hevtQuit;
	pMonitorParam->m_pfnCreateThread	= CTaskMgr::ReCreateThread;
	pMonitorParam->m_phThreads			= m_phTaskThreads;
	pMonitorParam->m_i32ThreadCount		= m_i32TaskThreadCount;
	pMonitorParam->m_CreateType			= THREAD_TYPE_TASK;
	
	m_hTaskMonitorThread = CreateThread( NULL, 0, CMonitorThread::MonitorThread, pMonitorParam, CREATE_SUSPENDED, NULL);
	if (m_hTaskMonitorThread == NULL)
    {
		DSERROR("[CTaskMgr::Create] CreateThread failed: TASK_MONITOR: ");
		DSSYSERR( GetLastError() );
        return FALSE;
    }
	
	// Task Processor
	for( INT32 i = 0 ; i < m_i32TaskThreadCount; i++ )
	{
		PX_TASK_PROCESSOR_PARAM* pTaskParam = new PX_TASK_PROCESSOR_PARAM;
		if( pTaskParam == NULL ) 
		{
			DSERROR("[CTaskMgr::Create] Memory Alloc Error\n");
			return FALSE;
		}
		
		pTaskParam->m_i32ThreadIdx			= i;
		pTaskParam->m_i32TaskThreadCount	= m_i32TaskThreadCount;
		pTaskParam->m_ui32PollingTime		= m_ui32PollingTime;
		pTaskParam->m_ui32EpsilonTime		= m_ui32EpsilonTime;
		
		m_phTaskThreads[ i ] = CreateThread( NULL, 0, CTaskProcessor::TaskProcessorThread, pTaskParam, 0, NULL ) ;
		if ( NULL == m_phTaskThreads[ i ] )
		{
			DSERROR( "[CTaskMgr::Create] CreateThread failed: TASK_PROCESSOR: ");
			DSSYSERR( GetLastError() );
			return FALSE;
		}
	}
	
	// Resume Monitors
	::ResumeThread( m_hTaskMonitorThread );

	return TRUE;
}

BOOL CTaskMgr::ReCreateThread(DS_THREAD_TYPE Type, INT32 i32ThreadIdx)
{
	switch ( Type )
	{
	case THREAD_TYPE_TASK:
		{
			if( i32ThreadIdx < 0 || i32ThreadIdx >= m_i32TaskThreadCount )
			{
				DSERROR("[CTaskMgr::ReCreateThread] ThreadIdx Error\n");
				return FALSE;
			}
			
			PX_TASK_PROCESSOR_PARAM* pParam = new PX_TASK_PROCESSOR_PARAM;
			if( pParam == NULL )
			{
				DSERROR("[CTaskMgr::ReCreateThread] Memory Alloc Error\n");
				return FALSE;
			}

			pParam->m_i32ThreadIdx				= i32ThreadIdx;
			pParam->m_i32TaskThreadCount		= m_i32TaskThreadCount;
			pParam->m_ui32PollingTime			= m_ui32PollingTime;
			pParam->m_ui32EpsilonTime			= m_ui32EpsilonTime;
	
			m_phTaskThreads[ i32ThreadIdx ]	= CreateThread( NULL, 0, CTaskProcessor::TaskProcessorThread, pParam, 0, NULL);
			if ( m_phTaskThreads[ i32ThreadIdx ] == NULL)
			{
				DSERROR( "[CTaskMgr::ReCreateThread] CreateThread failed: TASK_PROCESSOR: ");
				DSSYSERR( GetLastError() );
				return FALSE;
			}
		}
		break;
	default:
		{
			DSERROR("[CTaskMgr::ReCreateThread] Unknown Thread Type!\n");
			return FALSE;
		}
		break;
	}
    
	return TRUE;
}