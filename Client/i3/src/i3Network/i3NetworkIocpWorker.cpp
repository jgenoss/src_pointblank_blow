#include "i3NetworkDef.h"
#include "i3NetworkIocpWorker.h"
#include "i3NetworkSession.h"
#include "i3NetworkSessionManager.h"
#include "i3NetworkCompletionHandler.h"

I3_CLASS_INSTANCE(i3NetworkIocpWorker);


i3NetworkIocpWorker::~i3NetworkIocpWorker()
{
	m_bIsRun = false;
	i3Thread::WaitForFinish();
}

UINT32	i3NetworkIocpWorker::OnRunning( void * pUserData)	
{	
	m_bIsRun = true;
	DWORD TransferSize, IoKey;
	LPOVERLAPPED OverLapped;
	i3NetworkSession * pSession; 

	OVERLAPPED2 * poverlapped;

	while(m_bIsRun)
	{
		//Check IOPort 
		IoKey = 0;
		OverLapped = 0;
		BOOL Rv = ::GetQueuedCompletionStatus(m_hIOCP, &TransferSize, &IoKey, &OverLapped, 1); 
		
		if( Rv && (IoKey != 0) && ( TransferSize != 0 ) && (OverLapped != 0) )
		{
			poverlapped = ( OVERLAPPED2* )OverLapped;
			if( SESSION_ASYNCFLAG_RECEIVE == poverlapped->flags )
			{
				//처리 
				pSession = ( i3NetworkSession* )IoKey;
				if(pSession->GetIsActive())
				{
					if( m_pCompletionHandler->AddWorkingSession(pSession, m_Idx) )
					{
						pSession->m_WorkThreadIdx = m_Idx;
						pSession->Dispatch(TransferSize); 
						pSession->m_WorkThreadIdx = -1;

						m_pCompletionHandler->RemoveWorkingSession(m_Idx);

						// Read Event 를 걸어줍니다.
						pSession->WaitPacketReceive(m_Idx);
					}
				}
			}
			else
			{
				// Send 일때는 아무런 일도 하지 않습니다. 
			}
		}
		else
		{
			if( IoKey != 0 && GetLastError() != ERROR_OPERATION_ABORTED )
			{//이녀석은 삭제처리 합니다. 
				pSession = ( i3NetworkSession* )IoKey;
				if( m_pCompletionHandler->AddWorkingSession(pSession, m_Idx) )
				{	
					pSession->m_WorkThreadIdx = m_Idx;
					m_pSessionManager->DisConnectSession(pSession, false);
					pSession->m_WorkThreadIdx = -1;
					m_pCompletionHandler->RemoveWorkingSession(m_Idx); 
				}
				else
				{
					I3TRACE("[i3NetworkIocpWorker::OnRunning]AddWorkingSession DisConnect Error \n"); 
				}
			}			
		}
		::WaitForSingleObject( m_Handle, 1);
	}
	return 0; 
}

void	i3NetworkIocpWorker::AfterRunning( void * pUserData)
{
	return; 
}
