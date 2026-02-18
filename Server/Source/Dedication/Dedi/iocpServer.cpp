#include "pch.h"
#include "resolve.h"
#include "iocpWorker.h"
#include "iocpServer.h"
#include "udpbuffer.h"
#include "udpbufferPool.h"

BOOL g_bWriteSendError = FALSE;	// send error는 한번 찍히면 너무 많이 찍히기 때문에 한번만 찍게 하기 위함

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------
CIocpServer* g_pIocpServer = NULL; // 전역 변수 생성

// ----------------------------------------------------------------------------
// static variables
// ----------------------------------------------------------------------------
HANDLE				CIocpServer::m_hCompletionPort;									// io completion port
HANDLE				CIocpServer::m_phIoThreads[ DS_MAX_IOCP_THREAD_COUNT ];			// worker threads
UINT32				CIocpServer::m_ui32IoThreadCount;								// # of thread objects
SOCKET_OBJ*			CIocpServer::m_ppSocketObjs[ DS_DEFAULT_PORT_COUNT ];			// socket objects
UINT32				CIocpServer::m_ui32SocketCount;									// # of socket objects
CUdpBufferPool*		CIocpServer::m_pMemoryAllocator;

CIocpServer::CIocpServer( CUdpBufferPool* pAllocator )
{
	m_hCompletionPort	= NULL;									// io completion port

	i3mem::FillZero( m_phIoThreads, sizeof(m_phIoThreads));		// worker threads
	m_ui32IoThreadCount = 0;									// # of thread objects

	i3mem::FillZero( m_ppSocketObjs, sizeof( m_ppSocketObjs ));	// socket objects
	m_ui32SocketCount	= 0;									// # of socket objects
	
	m_i32OldCountIdx	= 0;									// 패킷 수를 구하기 위한 인덱스(0/1)
	m_i32NewCountIdx	= 0;									// ...
	m_ui32Time			= 0;									// server time

	m_pMemoryAllocator  = pAllocator;
}

CIocpServer::~CIocpServer()
{
	I3ASSERT( g_hevtQuit);
	
	DWORD ui32ExitCode = 0;

	// IOCP 핸들 닫기
	if ( m_hCompletionPort != NULL )
	{
		CloseHandle( m_hCompletionPort );
		m_hCompletionPort = NULL;
	}
	
	// 종료 이벤트가 signaled 상태인지 검사
	if( WaitForSingleObject( g_hevtQuit, 16) != WAIT_OBJECT_0 )
	{
		I3TRACE("[CIocpServer::~CIocpServer] g_hevtQuit is not signal state\n");
	}
	
	// worker 스레드가 종료하기를 기다린다
	::WaitForMultipleObjects( m_ui32IoThreadCount, m_phIoThreads, TRUE, 1000);
	
	// 리턴 코드 검사를 하고 여전히 동작중이라면 강제 종료시킨다
	
	// iocp worker threads
	for( UINT32 i = 0; i < m_ui32IoThreadCount; i++ )
	{
		GetExitCodeThread( m_phIoThreads[i], &ui32ExitCode);
		if( ui32ExitCode == STILL_ACTIVE)
		{
			TerminateThread( m_phIoThreads[i], (DWORD)-1 );
		}
		m_phIoThreads[i] = NULL;
	}
	
	// 소켓 오브젝트 삭제
	for( UINT32 i = 0; i < m_ui32SocketCount; i++ )
	{
		FreeSocketObj( m_ppSocketObjs[i] );
		SAFE_DELETE( m_ppSocketObjs[i] );
	}
}

BOOL	CIocpServer::Create( UINT32	 ui32SocketCount,  UINT32  ui32IP, UINT16* pui16Port, UINT32	ui32IoThreadCount)
{
	I3ASSERT( ui32IoThreadCount > 0 && ui32IoThreadCount <=  MAX_IO_THREAD_COUNT);
	I3ASSERT( ui32IP );
	I3ASSERT( pui16Port );
	I3ASSERT( ui32SocketCount <= DS_DEFAULT_PORT_COUNT); 
	I3ASSERT( g_hevtQuit );
	
	struct addrinfo *res = NULL,
					*ptr = NULL;
	SOCKET_OBJ*		pSock =NULL;
	HANDLE			hCompletionPort;
	INT32			rc, i32EndPointCount = 0;

	// 소켓 오브젝트를 생성한다.
	m_ui32SocketCount =  ui32SocketCount;
	for(UINT32 i = 0; i < m_ui32SocketCount; i++)
	{
		m_ppSocketObjs[i] = new SOCKET_OBJ;
		if ( m_ppSocketObjs[i] == NULL )
		{
			DSERROR("[CIocpServer::Create] new SOCKET_OBJ failed: %d\n", GetLastError());
			return FALSE;
		}
	}
	
	// Completion Port를 생성한다.
    m_hCompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)NULL, 0);
    if( m_hCompletionPort == NULL)
    {
        DSERROR("[CIocpServer::Create] CreateIoCompletionPort failed: %d\n", GetLastError());
        return FALSE;
    }
	
	// Launch threads
	{
		// Iocp Worker Threads 
		m_ui32IoThreadCount = ui32IoThreadCount;
		for( UINT32 i = 0; i < m_ui32IoThreadCount; i++)
		{
			IOCP_WORKER_PARAM* pWorkerParam = new IOCP_WORKER_PARAM;
			pWorkerParam->m_hCompletionPort	= m_hCompletionPort;
			pWorkerParam->m_hevtQuit		= g_hevtQuit;
			pWorkerParam->m_i32ThreadIndex	= i;
		
			m_phIoThreads[i] = CreateThread(NULL, 0, CIocpWorker::CompletionThread, (LPVOID)pWorkerParam, 0, NULL);
			if (m_phIoThreads[i] == NULL)
			{
				DSERROR("[CIocpServer::Create] CreateThread failed: %d (IOCP WORKER)\n", GetLastError());
				return FALSE;
			}
		}
	}
	
	// 소켓을 생성하고, iocp에 연결한다.
	for( INT32 i32PortIdx = 0; i32PortIdx < (INT32)m_ui32SocketCount; i32PortIdx++)
	{
		// get string ip address
		sockaddr_in addr;
		i3mem::FillZero(&addr, sizeof(sockaddr_in));
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= ui32IP;
		
		char* szSvrIp = inet_ntoa(addr.sin_addr);
		char  szSvrPort[6] = { '\0',  };
		i3String::Format( szSvrPort, sizeof( szSvrPort ), "%d", pui16Port[ i32PortIdx ]);

		// query local addresses
		res = ResolveAddress( szSvrIp, szSvrPort, AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		if (res == NULL)
		{
			DSERROR("[CIocpServer::Create] ResolveAddress failed to return any addresses!\n");
			return FALSE;
		}
		
		// For each local address returned, create a receiving socket
		ptr = res;
		while ( ptr )
		{
			PrintAddress(ptr->ai_addr, (int) ptr->ai_addrlen); 
			
			// create the socket
			pSock = m_ppSocketObjs[ i32PortIdx ]; 
			pSock->SetSocketIdx( i32PortIdx ) ;

			SOCKET s = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if ( s == INVALID_SOCKET)
			{
				DSERROR( "[CIocpServer::Create] socket failed: %d\n", WSAGetLastError());
				freeaddrinfo(res);
				return FALSE;
			}

			// relay에서는 100, 100을 사용. 200,200으로 바꿨는데도 문제 남아서, 변경.
			INT32 i32RecvBufSize = 1024 * 1024 * 100;    
			INT32 i32SendBufSize = 1024 * 1024 * 100 * 20; //20명 이라고 가정.  
			INT32 i32Len = 0;
			::setsockopt( s, SOL_SOCKET, SO_RCVBUF, (const char *) &i32RecvBufSize, sizeof(INT32));
			::setsockopt( s, SOL_SOCKET, SO_SNDBUF, (const char *) &i32SendBufSize, sizeof(INT32));
			::getsockopt( s, SOL_SOCKET, SO_RCVBUF, (char *) &i32RecvBufSize, &i32Len);
			::getsockopt( s, SOL_SOCKET, SO_SNDBUF, (char *) &i32SendBufSize, &i32Len);
			DSERROR("i32RecvBufSize = %d\n", i32RecvBufSize); 
			DSERROR("i32SendBufSize = %d\n", i32SendBufSize);
			
			struct timeval tv_timeo = { 0, 500000 };  /* 0.5 second */
			setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv_timeo, sizeof( tv_timeo ) );
				
			// Associate the socket and its SOCKET_OBJ to the completion port
			hCompletionPort = CreateIoCompletionPort( (HANDLE)s, m_hCompletionPort, (ULONG_PTR)pSock, 0);
			if ( hCompletionPort == NULL)
			{
				DSERROR( "[CIocpServer::Create] CreateIoCompletionPort failed: %d\n", GetLastError());
				freeaddrinfo(res);
				closesocket(s);
				return FALSE;
			}

			// Bind the socket to a local address and port
			rc = bind( s, ptr->ai_addr, (int) ptr->ai_addrlen );
			if ( rc == SOCKET_ERROR )
			{
				DSERROR( "[CIocpServer::Create] Bind failed: %d\n", WSAGetLastError());
				freeaddrinfo(res);
				closesocket(s);
				return FALSE;
			}
			
			// Set socket and port : 바인드 이후에 호출해야 한다.
			pSock->SetSocket(s);
			
			// Turn off UDP errors resulting from ICMP messages (port/host unreachable, etc)
			DWORD      dwBytes = 0;
			INT32      i32OptionValue = 0;
				
			rc = WSAIoctl( pSock->m_socket, SIO_UDP_CONNRESET, &i32OptionValue, sizeof(i32OptionValue), NULL, 0, &dwBytes, NULL, NULL );
			if (rc == SOCKET_ERROR)
			{
				DSERROR( "[CIocpServer::Create] WSAIoctl: SIO_UDP_CONNRESET failed: %d\n", WSAGetLastError());
				freeaddrinfo(res);
				closesocket(s);
				return FALSE;
			}

			// For UDP, simply post some receives
			CUdpBuffer *pBuf = NULL;
			for( INT32 i = 0; i < DEFAULT_OVERLAPPED_COUNT; i++ )
			{
				pBuf = GetBufferObj1( pSock );
				if( pBuf ) PostRecv( pSock, pBuf ); // 오류가 있을 경우 로그 남음
			}

			i32EndPointCount++;
			ptr = ptr->ai_next;
		}
		
		// free the addrinfo structure for the 'bind' address
		freeaddrinfo(res);
	}

	return TRUE;
}

void	CIocpServer::SetPacketCount_C()
{
	I3ASSERT( g_pConfig );
	
	if( m_ui32Time + PACKETCOUNT_TIME > i3ThreadTimer::GetServerTime() )	return;

	INT32 i32CountIdx = m_i32OldCountIdx = m_i32NewCountIdx;
	i32CountIdx++;
	i32CountIdx %= PACKETCOUNT_IDX;
	
	for( UINT32 i = 0 ; i < m_ui32SocketCount ; i++ )
	{
		m_ppSocketObjs[i]->ResetPacketProfile( i32CountIdx );
	}

	m_i32NewCountIdx = i32CountIdx;

	m_ui32Time = i3ThreadTimer::GetServerTime();
}

void CIocpServer::GetPacketProfile_C( INT32& i32RecvPacketSize, INT32& i32SendPacketSize )
{
	i32RecvPacketSize	= 0;
	i32SendPacketSize	= 0;

	PACKET_PROFILE* pPacketProfile;
	for( UINT32 i = 0 ; i < m_ui32SocketCount ; i++ )
	{
		pPacketProfile		= m_ppSocketObjs[ i ]->GetPacketProfile( m_i32OldCountIdx );

		i32RecvPacketSize	+= pPacketProfile->m_i32RecvPacketSize;
		i32SendPacketSize	+= pPacketProfile->m_i32SendPacketSize;
	}
}

INT32 CIocpServer::PostRecv(SOCKET_OBJ *pSock, CUdpBuffer* pRecvBuf)
{
	WSABUF	wbuf;
	DWORD	dwBytes = 0, dwFlags = 0;
	INT32	rc = 0;

	pRecvBuf->Init();
	wbuf.buf = pRecvBuf->GetBuffer();
	wbuf.len = pRecvBuf->GetBufferSize();

	dwFlags = 0;

	// Assign the IO order to this receive. This must be performned within
	//    the critical section. The m_ui32Operation of assigning the IO count and posting
	//    the receive cannot be interupted.
	pSock->m_cs.Lock();
	{
		rc = WSARecvFrom( pSock->m_socket, &wbuf, 1, &dwBytes, &dwFlags, (SOCKADDR *)pRecvBuf->GetAddress(), pRecvBuf->GetAddressSize(), pRecvBuf->GetOverlapped(), NULL );
	}
	pSock->m_cs.Unlock();

	if ( rc == SOCKET_ERROR )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			DSERROR( "[CIocpServer::PostRecv] WSARecv* failed: %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}
	}

	return NO_ERROR;
}

INT32 CIocpServer::CancelAndPostRecv( SOCKET_OBJ* pSock )
{
	CUdpBuffer* pBuf = NULL;
	
	CancelIo( (HANDLE)pSock->m_socket );
	
	for( INT32 i = 0; i < DEFAULT_OVERLAPPED_COUNT; i++ )
	{
		pBuf = GetBufferObj1( pSock );
		if( pBuf ) PostRecv( pSock, pBuf ); // 오류가 있을 경우 로그 남음
	}

	return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: PostSend
//
// Description:
//	- 소켓에 overlapped send를 포스트 한다.
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
int CIocpServer::PostSend( SOCKET_OBJ *pSock, CUdpBuffer* pSendBuf )
{
	WSABUF  wbuf;
	DWORD   dwBytes = 0;
	INT32	rc		= NO_ERROR;

	pSendBuf->ResetOverlapped();
	pSendBuf->SetOperation( OP_WRITE );
	
	// encrypt
	dwBytes = g_Encryption( pSendBuf->GetBuffer(), (INT16)pSendBuf->GetBufferSize(), UDP_SEND_PACKET_SIZE );
	if ( dwBytes > UDP_SEND_PACKET_SIZE )
	{
		DSERROR("Packet Encrypt Error\n");
		return SOCKET_ERROR;
	}
	
	// set wsabuf	
	wbuf.buf = pSendBuf->GetBuffer();	
	wbuf.len = dwBytes;
	if( wbuf.len == 0 || wbuf.len > UDP_SEND_PACKET_SIZE )
	{
		DSERROR("[CIocpServer::PostSend] size=%d\n", wbuf.len );
		return SOCKET_ERROR;
	}
	
	// check address
	if( pSendBuf->GetAddress()->sin_addr.s_addr == 0 ||	pSendBuf->GetAddress()->sin_port == 0 )
	{
		// 10049
		return SOCKET_ERROR;
	}
	
	pSock->m_cs.Lock();
	{
		rc = WSASendTo( pSock->m_socket, &wbuf, 1, &dwBytes, 0, (SOCKADDR *)pSendBuf->GetAddress(), *pSendBuf->GetAddressSize(), pSendBuf->GetOverlapped(), NULL );
	}
	pSock->m_cs.Unlock();
		
	if (rc == SOCKET_ERROR)
	{
		INT32 i32ErrorCode = WSAGetLastError();

		if ( i32ErrorCode != WSA_IO_PENDING)
		{
			if( i32ErrorCode == WSAENOBUFS ) // 10055 : critical error
			{
				if( g_bWriteSendError == FALSE ) // 한번 발생하면 너무 많이 찍혀서 플래그 처리 로그 오픈시 다시 false로 리셋
				{
					g_bWriteSendError = TRUE; 
					// cancel io and post recv
					DSERROR( "PostSend: Critical WSASend* failed: %d\n", i32ErrorCode );
					// 2011-12-02: 이 에러가 찍힌 후, 즉 리셋후에도 소켓이 동작하지 않아서 막습니다.
					//CancelAndPostRecv( pSock ); 
				}
			}
			else
			{
				// 다른 에러일 경우에는 send 버퍼를 해제하고 무시한다
				DSERROR( "PostSend: ignore WSASend* failed: %d\n", i32ErrorCode );
			}
			
			return SOCKET_ERROR;
		}
	}
	
	return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: GetBufferObj
//
// Description:
//	- udp buffer를 할당한다.
//		- 내부적으로 메모리 풀에 요청한다
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
CUdpBuffer* CIocpServer::GetBufferObj1(SOCKET_OBJ* pSock )
{
	I3ASSERT( m_pMemoryAllocator );
	
	CUdpBuffer* pBuf = NULL;
	if( !m_pMemoryAllocator->GetBuffer( &pBuf ) )return NULL;
	
	pBuf->Init();
	pBuf->SetSocketIdx( pSock->GetSocketIdx() );

	return pBuf;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: FreeBufferObj
//
// Description:
//	- 버퍼 오브젝트를 해제한다
//		- 메모리 큐로 반환한다.
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
void CIocpServer::FreeBufferObj( CUdpBuffer* pBuf )
{
	I3ASSERT( m_pMemoryAllocator );
	m_pMemoryAllocator->ReleaseBuffer( pBuf );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: FreeSocketObj
//
// Description:
//	- 진행중인 작업이 없으면 소켓 오브젝트를 해제한다.
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//
void CIocpServer::FreeSocketObj(SOCKET_OBJ *pSock )
{
	// Close the socket if it hasn't already been closed
	if ( pSock->m_socket != INVALID_SOCKET )
	{
		closesocket( pSock->m_socket );
		pSock->m_socket = INVALID_SOCKET;
	}
}
