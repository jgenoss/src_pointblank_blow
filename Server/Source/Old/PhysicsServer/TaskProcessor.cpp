// TaskProcessor.cpp
//
// YouJong Ha
//	Last update : 2010-10-06 (yyyy:mm:dd)
//	
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
#include "UdpBufferPool.h"
#include "TrackBuffer.h"
#include "PacketLocker.h"
#include "TaskProcessorImpl.h"
#include "TaskProcessor.h"
#include "MapMgr.h"
#include "MapData.h"

///////////////////////////////////////////////////////////////////////////////
//
// Function: CTaskProcessor
//
// Description:
//	- constructor
//		- m_hevtSendTimer는 항상 non signaled 상태이다. 
//		  wait함수의 timeout 리턴을 이용하여 타이머 역할을 수행.
// Arguments:
// Return:
//
CTaskProcessor::CTaskProcessor()
{
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ~CTaskProcessor
//
// Description:
//	- desttructor
//		- 
// Arguments:
// Return:
//
CTaskProcessor::~CTaskProcessor()
{
}

//////////////////////////////////////////////////////////////////////////////
//
// Function: OnUpdate
//
// Description:
//	- 쓰레드 함수에서 주기적으로 호출하는 콜백 함수
//		
// Arguments:
//	- pParam	: TASK_PROCESSOR_PARAM 구조체 포인터
//	- dwDelta	: 이전 호출로부터의 시간 간격
// Return:
//	- 
//
void	CTaskProcessor::OnUpdate(void* pParam, DWORD dwDeltaMiliSec)
{
	PX_TASK_PROCESSOR_PARAM * p = (PX_TASK_PROCESSOR_PARAM*)pParam;
	
	// map과 task는 1:1로 매치되어 있다.
	CMapData* pMap = g_pMapMgr->GetMap( STAGEORD( p->m_i32ThreadIdx + 1 ) );
	if( pMap == NULL ) 
	{
		//yjha DSERROR("[Task::OnUpdate] pMap == NULL\n");
		return;
	}
	
	// 2. process packet
	INT32 nCount = g_pDataArchive->GetCount( p->m_i32ThreadIdx );
	for( INT32 i = 0; i < nCount; i++ )
	{
		CUdpBuffer* pRecvBuf = NULL;
		g_pDataArchive->Pop( p->m_i32ThreadIdx,  (i3ElementBase**)&pRecvBuf );
		if ( pRecvBuf != NULL )
		{
			ProcessRecvPacket( p->m_i32ThreadIdx, pRecvBuf );
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuf );
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: TaskProcessorThread
//
// Description:
//	- 쓰레드 함수
//		- 
//      - 
//		- 
// Arguments:
//	- lpParameter:	TASK_PROCESSOR_PARAM 구조체 포인터
// Return:
//	- 
//
DWORD WINAPI CTaskProcessor::TaskProcessorThread(LPVOID lpParameter)
{
	I3ASSERT( g_hevtQuit != NULL );
	I3ASSERT( g_hevtQuit != INVALID_HANDLE_VALUE );
	
	PX_TASK_PROCESSOR_PARAM  threadParam;
	PX_TASK_PROCESSOR_PARAM* param = (PX_TASK_PROCESSOR_PARAM*)lpParameter;
	memcpy_s( &threadParam, sizeof(threadParam), param, sizeof(threadParam));
	delete param;
	
	HANDLE	hQuitEvent	= g_hevtQuit;
	DWORD	wo			= 0;									

	UINT32  dwPollingTime = threadParam.m_ui32PollingTime;
	UINT32	dwEpsilonTime = threadParam.m_ui32EpsilonTime;
	DWORD	dwUpdateTic, dwLastUpdateTic, dwSleepTime; 
	
	dwUpdateTic = dwLastUpdateTic = 0; 
	dwLastUpdateTic = GetTickCount(); 

	DWORD dwNowTime; 
	while( 1 )
	{
		dwNowTime		= GetTickCount(); 
		dwUpdateTic		= dwNowTime - dwLastUpdateTic; 
		dwLastUpdateTic = dwNowTime; 

		if(dwUpdateTic > dwPollingTime) 
		{
			dwSleepTime = 0; 
			//DSMSG("Update TimeOut %d \n", dwUpdateTic); 
		}
		else 
		{
			dwSleepTime = dwPollingTime - dwUpdateTic; 
		}

		if( dwSleepTime > dwEpsilonTime )
		{
			if( dwSleepTime == dwPollingTime )
			{
				dwSleepTime -= dwEpsilonTime;	
			}
			
			wo = WaitForSingleObject( hQuitEvent, dwSleepTime);
			if( wo == WAIT_OBJECT_0 ) goto exit_loop;
			
		}
		
		OnUpdate(&threadParam, dwUpdateTic);
	}

exit_loop:
	
	I3TRACE("Task Processor (0x%x) Exited...\n", GetCurrentThreadId());
	 
	return 0;
}