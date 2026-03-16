#include "i3CommonType.h"
#include "i3NetworkIocpWorker.h"
#include "i3NetworkSession.h"
#include "i3NetworkSessionManager.h"
#include "i3NetworkCompletionHandler.h"

I3_CLASS_INSTANCE(i3NetworkIocpWorker, i3Thread); 

i3NetworkIocpWorker::i3NetworkIocpWorker()
{
	m_bIsRun			= FALSE; 
	m_Idx				= -1; 
	m_pSessionManager	= NULL;
}

i3NetworkIocpWorker::~i3NetworkIocpWorker()
{
	m_bIsRun	= FALSE; 
	i3Thread::WaitForFinish();
}

void i3NetworkIocpWorker::BeforeRunning( void * pUserData)
{	
	return; 
}

UINT32	i3NetworkIocpWorker::OnRunning( void * pUserData)	
{	
	m_bIsRun = TRUE;
	ULONG_PTR IoKey;
	DWORD TransferSize;
	LPOVERLAPPED OverLapped;
	BOOL Rv; 
	i3NetworkSession * pSession; 

	OVERLAPPED2 * poverlapped;

	while(m_bIsRun)
	{
		//Check IOPort 
		IoKey = 0;
		OverLapped = 0;
		Rv = ::GetQueuedCompletionStatus(m_hIOCP, &TransferSize, &IoKey, &OverLapped, 1); 
		
		if( Rv && (IoKey != 0) && ( TransferSize != 0 ) && (OverLapped != 0) )
		{
			poverlapped = ( OVERLAPPED2* )OverLapped;
			if( SESSION_ASYNCFLAG_RECEIVE == poverlapped->flags )
			{
				//처리 
				pSession = ( i3NetworkSession* )IoKey;
				if(pSession->GetIsActive())
				{
					if( m_pCompletionHandler->AddWorkingSession( pSession, m_Idx ) )
					{
						pSession->m_WorkThreadIdx = m_Idx;
						pSession->Dispatch(TransferSize); 
						pSession->m_WorkThreadIdx = -1;

						m_pCompletionHandler->AddPacketInfo( m_Idx, TransferSize );

						m_pCompletionHandler->RemoveWorkingSession(m_Idx);

						// Read Event 를 걸어줍니다.
						pSession->WaitPacketReceive(m_Idx);
					}
				}
			}
			else if (SESSION_ASYNCFLAG_SEND == poverlapped->flags)
			{
				// Send 일때는 아무런 일도 하지 않습니다. 
				m_pCompletionHandler->AddSendPacketInfo(m_Idx, TransferSize);
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
					m_pSessionManager->DisConnectSession(pSession, FALSE);
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
