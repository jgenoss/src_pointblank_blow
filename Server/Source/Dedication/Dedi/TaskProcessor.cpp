#include "pch.h"
#include "iocpServer.h"
#include "PacketLocker.h"
#include "GroupMaker.h"
#include "DediRoom.h"
#include "DediMember.h"
#include "TaskMgr.h"
#include "UdpBufferPool.h"
#include "TaskProcessor.h"
#include "ServerStatistics.h"

UINT32	g_aui32ProcessCount[64];	// OnUpdate 1회 호출당 처리되는 room의 수 
									// 각 스레드의 동작 상태를 보기 위함

CTaskProcessor::CTaskProcessor()
{
	
}

CTaskProcessor::~CTaskProcessor()
{
}

// Description:
//	- 쓰레드 함수에서 주기적으로 호출하는 콜백 함수
//		
// Arguments:
//	- pParam	: TASK_PROCESSOR_PARAM 구조체 포인터
//	- dwDelta	: 이전 호출로부터의 시간 간격
void	CTaskProcessor::OnUpdate(void* pParam, DWORD dwDeltaMiliSec)
{
	TASK_PROCESSOR_PARAM * p = (TASK_PROCESSOR_PARAM*)pParam;

	if( NULL == g_pConfig) return;
	
	UINT32	i32RoomsPerIteration	= p->m_i32GroupMax  / p->m_i32TaskThreadCount;	// 이 스레드가 검사해야 하는 그룹의 수
	if ( (p->m_i32GroupMax  % p->m_i32TaskThreadCount) != 0 ) i32RoomsPerIteration++;

	INT32	i32NextRoomIdx			= p->m_i32ThreadIdx  % p->m_i32TaskThreadCount;	// 다음 검사 대상
	UINT32	ui32Step				= p->m_i32TaskThreadCount;						// 다음 스텝 (그룹)

	CDediRoom * pRoom = NULL; 
	// ----------------
	// room scheduling
	// ----------------
	INT32 i32DeleteCount = 0;
	INT32 i32UseRoomCount = 0;
	for( UINT32 i = 0; i < i32RoomsPerIteration; i++)
	{
		// room 하나에 대해서만 처리하는 함수
		pRoom = g_pRoomManager->GetGroup( i32NextRoomIdx );
		
		if( !pRoom )
		{//이걸로 해결 안될듯. 
			DSERROR("CRITICAL_ERROR\n"); // 여기에 들어오면 안됩니다
			i32NextRoomIdx = (i32NextRoomIdx + ui32Step) % p->m_i32GroupMax;
			continue;
		}
	
		//이해가 안됨. 김성백
		if( RGS_USE != pRoom->GetGroupState() )
		{
			// next index
			if(i32DeleteCount < 2)
			{
				pRoom->DeleteData( i32NextRoomIdx ); 
				i32DeleteCount++;
			}
			i32NextRoomIdx = (i32NextRoomIdx + ui32Step) % p->m_i32GroupMax;
			continue;
		}
		pRoom->UpdateBattleTime();
		if ( pRoom->IsTrainingRoom() )
		{
			ProcessRoom_Relay( p->m_i32ThreadIdx, pRoom, i32NextRoomIdx, dwDeltaMiliSec );
		}
		else
		{
			ProcessRoom( p->m_i32ThreadIdx, pRoom, i32NextRoomIdx );
		}

		i32UseRoomCount++;

		// next index
		i32NextRoomIdx = (i32NextRoomIdx + ui32Step) % p->m_i32GroupMax;
	}

	// 개발용
	g_aui32ProcessCount[ p->m_i32ThreadIdx ]  = i32UseRoomCount;

	return; 
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
	// 스레드 파라미터로 전달된 구조체로 인해서 메모리 릭이 생기는 것을 
	// 방지하기 위해서 로컬 구조체로 복사하고 바로 삭제한다.
	TASK_PROCESSOR_PARAM  threadParam;
	TASK_PROCESSOR_PARAM* param = (TASK_PROCESSOR_PARAM*)lpParameter;
	memcpy(&threadParam, param, sizeof(threadParam));
	delete param;

#ifdef NEW_LOG
	//alloc private log buffer
	I3TRACE("TaskProcessorThread=%d\n", GetCurrentThreadId() );
	if( NULL == g_pLogFile->AllocBuffer( GetCurrentThreadId() ) )
	{
		goto exit_loop;
	}
#endif 	
	HANDLE handles[1];
	handles[0] = g_hevtQuit;

	DWORD	wo= 0;											// wait 함수 리턴 값

	UINT32 dwPollingTime = 50; // threadParam.m_ui32PollingTime;		
								// 초당 20회(50ms) 보내도록 수정->15회(67ms)까지 줄일 예정.
	DWORD	dwUpdateTic, dwLastUpdateTic, dwSleepTime; 
	dwUpdateTic = dwLastUpdateTic = 0; 

	dwLastUpdateTic = GetTickCount(); 

	DWORD dwNowTime; 
	while( 1 )
	{
		dwNowTime		= GetTickCount(); 
		dwUpdateTic		= dwNowTime - dwLastUpdateTic; 

		if(dwUpdateTic > dwPollingTime) 
		{
			dwSleepTime = 0; 
			//DSERROR("Update TimeOut %d \n", dwUpdateTic); 
		}
		else 
		{
			dwSleepTime = dwPollingTime - dwUpdateTic; 
		}

		wo = WaitForMultipleObjects(1, handles, FALSE, dwSleepTime);
		
		if( wo == WAIT_OBJECT_0 ) goto exit_loop;

		if( dwUpdateTic > 0 )
		{
			OnUpdate(&threadParam, dwUpdateTic);
			dwLastUpdateTic = GetTickCount(); 
		}
	}

exit_loop:
	
	I3TRACE("Task Processor (0x%x) Exited...\n", GetCurrentThreadId());
	 
	return 0;
}
