#include "pch.h"
#include "MonitorThread.h"

//	IOCP Server와 TaskManager에서 공통으로 사용

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

BOOL    (*pfnCreateThread)(DS_THREAD_TYPE Type, INT32 i32ThreadIndex);

DWORD WINAPI CMonitorThread::MonitorThread(LPVOID lpParameter)
{
	I3ASSERT( lpParameter != NULL);

	MONITOR_THREAD_PARAM* param = (MONITOR_THREAD_PARAM*)lpParameter;
	HANDLE handles[ MAXIMUM_WAIT_OBJECTS ] = { NULL, };
	
	DWORD	dwWaitObj = WAIT_TIMEOUT; 
	UINT32	ui32WaitObjectCount  = param->m_ui32ThreadCount + 1; // evtQuit의 수 포함
	DS_THREAD_TYPE	Type = param->m_Type;
	
	pfnCreateThread = param->m_pfnCreateThread;
	if( pfnCreateThread == NULL )
	{
		DSERROR("[CMonitorThread::MonitorThread] 재시작 함수 포인터가 NULL입니다.\n");
		delete param;
		return (DWORD)-1;
	}
	
	// size check
	if( ui32WaitObjectCount >= MAXIMUM_WAIT_OBJECTS )
	{
		DSERROR("[CMonitorThread::MonitorThread] 핸들 수가 너무 많습니다");
		delete param;
		return (DWORD)-1;
	}

	// copy handles (종료 이벤트와 스레드 핸들을 배열 하나로 처리)
	handles[0] = param->m_hevtQuit;
	for ( UINT32 i = 1; i < ui32WaitObjectCount; i++ )
	{
		handles[i] = param->m_phThreads[ i-1 ];
	}
	
	delete param;
	
	while ( 1 )
	{
		dwWaitObj = WaitForMultipleObjects( ui32WaitObjectCount, handles, FALSE, 100);
		
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