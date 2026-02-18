#include "i3NetworkDef.h"
#include "i3NetworkCompletionHandler.h"
#include "i3NetworkIocpWorker.h"
#include "i3NetworkSessionManager.h"
#include "i3NetworkDB.h"

I3_CLASS_INSTANCE(i3NetworkCompletionHandler);

i3NetworkCompletionHandler::~i3NetworkCompletionHandler()
{
	OnDestroy(); 
}

bool i3NetworkCompletionHandler::OnCreate(INT32 WorkCount, i3NetworkSessionManager * pSessionManager)
{
	m_pSessionManager = pSessionManager; 

	//Create IOCP
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,0, 0, WorkCount); 
	if(m_hIOCP == 0)
	{
		I3TRACE("[i3NetworkCompletionHandler::OnCreate] CreateIoCompletionPort \n"); 
		return false;
	}

	//Create WorkIOCP
	m_WorkerCount = WorkCount; 
	m_pWorker	= (i3NetworkIocpWorker **)i3MemAlloc(sizeof(i3NetworkIocpWorker*) * m_WorkerCount);		
	char TempStr[64]; 
	for(INT32 i = 0; i < m_WorkerCount; i++)
	{
		m_pWorker[i] = i3NetworkIocpWorker::new_object();
		m_pWorker[i]->SetIocpHandle( m_hIOCP, m_pSessionManager, i, this); 

		sprintf(TempStr, "WorkIOCP%d",i); 
		m_pWorker[i]->Create(TempStr, 0, 4096, 0, PRIORITY_NORMAL); // 이부분은 추후 워크에 같이 넣는 방식으로
	}

	m_pWorkSession = (i3NetworkSession **)i3MemAlloc(sizeof(i3NetworkSession*) * m_WorkerCount); 
	i3mem::FillZero(m_pWorkSession, sizeof(i3NetworkSession*) * m_WorkerCount ); 

	return true;
}

bool i3NetworkCompletionHandler::OnDestroy(void)
{
	if( m_pWorker != nullptr)
	{
		for(INT32 i = 0; i < m_WorkerCount; i++)
		{
			m_pWorker[i]->Terminate(); // 이부분은 추후 워크에 같이 넣는 방식으로
			I3_SAFE_RELEASE( m_pWorker[i] );
		}

		I3MEM_SAFE_FREE( m_pWorker ); 	
	}
	I3MEM_SAFE_FREE( m_pWorkSession );

	return true;
}

void i3NetworkCompletionHandler::OnAddIOCP(HANDLE handle, DWORD keyValue)
{	
	::CreateIoCompletionPort( handle, m_hIOCP, keyValue, 0 );
	
	return; 
}

