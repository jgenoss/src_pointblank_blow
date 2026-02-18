#include "i3NetworkDef.h"
#include "i3NetworkAcceptor.h"
#include "i3NetworkServerSocket.h"
#include "i3NetworkSessionManager.h"
#include "i3NetworkCompletionHandler.h"

I3_CLASS_INSTANCE(i3NetworkAcceptor);

i3NetworkAcceptor::~i3NetworkAcceptor(void)
{	
	m_IsRunning			= false; 
	i3Thread::WaitForFinish();
	
	for(UINT8 i = 0; i < m_SocketCount; i++)I3_SAFE_RELEASE(m_ppServerSock[i]);
	I3MEM_SAFE_FREE(m_pTimeOut);
	I3MEM_SAFE_FREE(m_ppServerSock);
} 

bool i3NetworkAcceptor::OnCreate(UINT8 SocketCount, UINT32 * pIpAddress, UINT16 * pPortNo, UINT8 * pTimeOut, i3NetworkSessionManager * pSessionManager, i3NetworkCompletionHandler * pIOCP)
{
	//Set Value 
	m_pSessionManager = pSessionManager; 
	m_pIOCP = pIOCP; 
	m_SocketCount = SocketCount; 

	//Create Server Socket 
	m_ppServerSock	= (i3NetworkServerSocket**)i3MemAlloc(sizeof(i3NetworkServerSocket*) * m_SocketCount); 
	m_pTimeOut		= (UINT8*)i3MemAlloc(sizeof(UINT8) * m_SocketCount);
	for(UINT8 i = 0; i < m_SocketCount; i++)
	{
		m_ppServerSock[i] = i3NetworkServerSocket::new_object(); 
		if (!m_ppServerSock[i]->OnCreate(pIpAddress[i], pPortNo[i])) return false;
		m_pTimeOut[i] = pTimeOut[i]; 
	}

	//Create Thread 
	i3Thread::Create("i3Acceptor", 0, 4096, nullptr, PRIORITY_NORMAL);
	
	return true;
}


UINT32 i3NetworkAcceptor::OnRunning( void * pUserData )
{	
	struct	timeval timeout; 
	SOCKET	SocketAccept;
	DWORD	SessionPointer;

	m_IsRunning		= true; 
	
	struct sockaddr_in FromAddr; 
	INT32  AddrSize; 
	while(m_IsRunning)
	{
		for(UINT8 i = 0; i < m_SocketCount; i++)
		{
			timeout.tv_sec  = 0;
			timeout.tv_usec = m_pTimeOut[i];

			AddrSize = sizeof(sockaddr_in); 
			SocketAccept = m_ppServerSock[i]->AcceptConnection( &timeout, &FromAddr, &AddrSize );
			
			//정상적인 소켓 입니다. 처리를 합니다. 
			if(SocketAccept != INVALID_SOCKET) 
			{
				SessionPointer = m_pSessionManager->ConnectSession( SocketAccept, &FromAddr);
				
				if (SessionPointer != 0)
				{
					m_pIOCP->OnAddIOCP((HANDLE)SocketAccept, SessionPointer);
				}
				else
				{
					// 허용 세션 초과 시, 연결을 끊는다.
					::shutdown(SocketAccept, SD_BOTH);
					::closesocket(SocketAccept);
				}
			}
		}
	}

	return 0; 
}
