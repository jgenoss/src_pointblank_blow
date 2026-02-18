#include "pch.h"
#include "MonitorThread.h"

CMonitorThread::CMonitorThread(void)
{
}

CMonitorThread::~CMonitorThread(void)
{
}

// procedure:
// 1. exit check
// 2. thread handles check
// 3. if (!exit && thread signaled) restart threads
DWORD WINAPI CMonitorThread::MonitorThread(LPVOID lpParameter)
{
	MONITOR_THREAD_PARAM* param = (MONITOR_THREAD_PARAM*)lpParameter;
	HANDLE handles[MAXIMUM_WAIT_OBJECTS] = { NULL, };
	BOOL    (*pfnCreateThread)(DS_THREAD_TYPE Type, INT32 i32ThreadIndex) = NULL;
	DS_THREAD_TYPE	Type = THREAD_TYPE_UNKNOWN;
	
	// size check
	INT32 i32WaitObjectCount  = param->m_i32ThreadCount + 1; // evtQuit의 수 포함
	if( i32WaitObjectCount >= MAXIMUM_WAIT_OBJECTS )
	{
		delete param;
		DSERROR("[CMonitorThread::MonitorThread] 핸들 수가 너무 많습니다\n");
		return (DWORD)-1;
	}

	// copy handles (종료 이벤트와 스레드 핸들을 배열 하나로 처리)
	handles[ 0 ] = param->m_hevtQuit;
	for ( INT32 i32Idx = 1; i32Idx < i32WaitObjectCount; i32Idx++)
	{
		handles[ i32Idx ] = param->m_phThreads[ i32Idx - 1 ];
	}
	
	// restart function
	I3ASSERT(param->m_pfnCreateThread);
	pfnCreateThread = param->m_pfnCreateThread;
	Type			= param->m_CreateType;

	if( pfnCreateThread == NULL )
	{
		delete param;
		DSERROR("[CMonitorThread::MonitorThread] 재시작 함수 포인터가 NULL입니다\n");
		return (DWORD)-1;
	}
	
	delete param;

	DWORD dwWaitObj = WAIT_TIMEOUT; 
	while ( 1 )
	{
		dwWaitObj = WaitForMultipleObjects( i32WaitObjectCount, handles, FALSE, 100);
		
		switch (dwWaitObj)
		{
		case WAIT_OBJECT_0: // quit
			I3TRACE("Monitor Thread Exit...(0x%x)\n", GetCurrentThreadId());
			return 0;
		case WAIT_TIMEOUT:  // continue watching
			continue;
			break;
		default:
			{
				// check index
				DWORD dwWaitObjectIndex = dwWaitObj - WAIT_OBJECT_0;
				if ( dwWaitObjectIndex > 0 && dwWaitObjectIndex < MAXIMUM_WAIT_OBJECTS)
				{
					// check exit code
					DWORD dwExit = 0;
					::GetExitCodeThread( handles[dwWaitObjectIndex], &dwExit );
					
					// still alive (may be hang)
					if( dwExit == STILL_ACTIVE )
					{
						TerminateThread( handles[ dwWaitObjectIndex], (DWORD)-1);
					}
					else
					{
						CloseHandle( handles[ dwWaitObjectIndex] );
					}

					// launch new thead
					DSWARNING("[Monitor] Retry Launch Thread (index = %d)\n", dwWaitObjectIndex - 1 );
					pfnCreateThread( Type, dwWaitObjectIndex - 1);
				} // if valid index
			} // default
		};
	}
	
	return 0;
}