#include "i3CommonType.h"
#include "i3NetworkCompletionHandler.h"
#include "i3NetworkIocpWorker.h"
#include "i3NetworkSessionManager.h"
#include "i3NetworkDB.h"

I3_CLASS_INSTANCE(i3NetworkCompletionHandler, i3ElementBase); 

i3NetworkCompletionHandler::i3NetworkCompletionHandler()
{
	m_hIOCP				= 0;
	m_pWorker			= NULL;
	m_WorkerCount		= 0;
	m_pSessionManager	= NULL;
	m_pWorkSession		= NULL; 
}

i3NetworkCompletionHandler::~i3NetworkCompletionHandler()
{
	OnDestroy(); 
}

BOOL i3NetworkCompletionHandler::OnCreate(INT32 WorkCount, i3NetworkSessionManager * pSessionManager)
{
	m_pSessionManager = pSessionManager; 

	//Create IOCP
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,0, 0, WorkCount); 
	if(m_hIOCP == 0)
	{
		I3TRACE("[i3NetworkCompletionHandler::OnCreate] CreateIoCompletionPort \n"); 
		return FALSE; 
	}

	//Create WorkIOCP
	m_WorkerCount = WorkCount; 
	m_pWorker	= (i3NetworkIocpWorker **)i3MemAlloc(sizeof(i3NetworkIocpWorker*) * m_WorkerCount);		
	UINT32	* pui32WorkerThreadIdList = (UINT32*)i3MemAlloc(sizeof(UINT32)*m_WorkerCount );
	char TempStr[64]; 
	for(INT32 i = 0; i < m_WorkerCount; i++)
	{
		m_pWorker[i] = i3NetworkIocpWorker::NewObject();
		m_pWorker[i]->SetIocpHandle( m_hIOCP, m_pSessionManager, i, this); 

		sprintf(TempStr, "WorkIOCP%d",i); 
		m_pWorker[i]->Create(TempStr, 0, 4096, 0, PRIORITY_NORMAL); // 이부분은 추후 워크에 같이 넣는 방식으로
		pui32WorkerThreadIdList[i] = m_pWorker[i]->GetThreadID();
	}
	m_pSessionManager->SetWorkerThreadIdList( pui32WorkerThreadIdList );

	m_pWorkSession = (i3NetworkSession **)i3MemAlloc(sizeof(i3NetworkSession*) * m_WorkerCount); 
	i3mem::FillZero(m_pWorkSession, sizeof(i3NetworkSession*) * m_WorkerCount ); 

	m_pui64PacketCount	= (UINT64*)i3MemAlloc(sizeof(UINT64)*m_WorkerCount );
	if( NULL == m_pui64PacketCount )				return FALSE;
	m_pui64PacketSize	= (UINT64*)i3MemAlloc(sizeof(UINT64)*m_WorkerCount );
	if( NULL == m_pui64PacketSize )					return FALSE;

	m_pui64SendPacketCount = (UINT64*)i3MemAlloc(sizeof(UINT64)*m_WorkerCount);
	if (NULL == m_pui64SendPacketCount)				return FALSE;
	m_pui64SendPacketSize = (UINT64*)i3MemAlloc(sizeof(UINT64)*m_WorkerCount);
	if (NULL == m_pui64SendPacketSize)					return FALSE;

	return TRUE; 
}

BOOL i3NetworkCompletionHandler::OnDestroy(void)
{
	I3MEM_SAFE_FREE( m_pui64PacketCount );
	I3MEM_SAFE_FREE( m_pui64PacketSize );
	I3MEM_SAFE_FREE(m_pui64SendPacketCount);
	I3MEM_SAFE_FREE(m_pui64SendPacketSize);

	if( m_pWorker != NULL)
	{
		for(INT32 i = 0; i < m_WorkerCount; i++)
		{
			m_pWorker[i]->Terminate(); // 이부분은 추후 워크에 같이 넣는 방식으로
			I3_SAFE_RELEASE( m_pWorker[i] );
		}

		I3MEM_SAFE_FREE( m_pWorker ); 	
	}
	I3MEM_SAFE_FREE( m_pWorkSession );

	return TRUE; 
}

void i3NetworkCompletionHandler::OnAddIOCP(HANDLE handle, ULONG_PTR keyValue)
{	
	::CreateIoCompletionPort( handle, m_hIOCP, keyValue, 0 );
	
	return; 
}

void i3NetworkCompletionHandler::GetPacketInfo(UINT64* pi64Count, UINT64* pi64Size, UINT64* pi64SendCount, UINT64* pi64SendSize)
{
	*pi64Count = 0;
	*pi64Size = 0;
	*pi64SendCount = 0;
	*pi64SendSize = 0;
	for (INT32 i = 0; i < m_WorkerCount; i++)
	{
		*pi64Count += m_pui64PacketCount[i];
		*pi64Size += m_pui64PacketSize[i];
		*pi64SendCount += m_pui64SendPacketCount[i];
		*pi64SendSize += m_pui64SendPacketSize[i];
	}
}

void i3NetworkCompletionHandler::GetPacketInfoThread(INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size, UINT64* pi64SendCount, UINT64* pi64SendSize)
{
	if (0 > i32ThreadIdx)				return;
	if (m_WorkerCount <= i32ThreadIdx)	return;

	*pi64Count += m_pui64PacketCount[i32ThreadIdx];
	*pi64Size += m_pui64PacketSize[i32ThreadIdx];
	*pi64SendCount += m_pui64SendPacketCount[i32ThreadIdx];
	*pi64SendSize += m_pui64SendPacketSize[i32ThreadIdx];
}
