#include "pch.h"
#include "UdpService.h"
#include "UdpBuffer.h"
#include "UdpBufferPool.h"
#include "TrackBuffer.h"
#include "PacketLocker.h"

CUdpService*	g_pUdpService = NULL;

SOCKET			CUdpService::m_socket;	// server socket
HANDLE			CUdpService::m_hEvent;	// socket event
HANDLE			CUdpService::m_hThread;	// io thread

CUdpService::CUdpService(void)
{
	m_socket	= INVALID_SOCKET;
	m_hEvent	= NULL;
	m_hThread	= NULL;
}

CUdpService::~CUdpService(void)
{
	Stop();
}

BOOL CUdpService::Create()
{
	// create udp socket
	m_socket	= socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if ( m_socket == INVALID_SOCKET )
	{
		DSERROR("Create Socket Error! \n");
		return FALSE;
	}
	
	// relay에서는 100, 100을 사용. 200,200으로 바꿨는데도 문제 남아서, 변경.
	INT32 i32RecvBufSize = 1024 * 1024 * 100;
	INT32 i32SendBufSize = 2147483647; //1024 * 1024 * 300;
	INT32 i32Len = 0;
	setsockopt( m_socket, SOL_SOCKET, SO_RCVBUF, (const char *) &i32RecvBufSize, sizeof(INT32));
	setsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, (const char *) &i32SendBufSize, sizeof(INT32));
	getsockopt( m_socket, SOL_SOCKET, SO_RCVBUF, (char *) &i32RecvBufSize, &i32Len);
	getsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, (char *) &i32SendBufSize, &i32Len);

	//struct timeval tv_timeo = { 3, 500000 };  /* 3.5 second */
	struct timeval tv_timeo = { 0, 100000 };	/* 0.1 second */
	setsockopt( m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv_timeo, sizeof( tv_timeo ) );
	
	
	// bind
	SOCKADDR_IN	localAddr;
	memset(&localAddr, 0, sizeof(localAddr));

	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= g_pConfig->m_ui32UdpIp;
	localAddr.sin_port			= htons( g_pConfig->m_ui16UdpPort );

	if( ::bind(m_socket, (struct sockaddr *) &localAddr, sizeof( localAddr ) ) == SOCKET_ERROR )
	{
		DSERROR( "UDP bind failed.\n" );
		return FALSE;
	}
	// Set our socket to non-blocking mode using ioctlsocket() function
	u_long iMode = 1;
	if ( ioctlsocket( m_socket, FIONBIO, &iMode) == SOCKET_ERROR  )
	{
		DSERROR( "UDP non-blocking failed.\n" );
		return FALSE;
	}

	// create event
	m_hEvent = WSACreateEvent();
	if( WSAEventSelect( m_socket, m_hEvent, FD_READ | FD_WRITE ) == SOCKET_ERROR)
    {
        DSERROR( "WSAEventSelect failed: %d\n", WSAGetLastError());
        return FALSE;
    }

	THREAD_OBJ* pThreadObj = new THREAD_OBJ(m_socket, m_hEvent);
	m_hThread = CreateThread( NULL, 4096 * 1024, CUdpService::ChildThread, (LPVOID)pThreadObj, 0, 0 );
	return ( m_hThread != INVALID_HANDLE_VALUE );
}

void CUdpService::Stop()
{
	DWORD exitCode = 0;
	
	// signaled 상태인지 검사
	if( WaitForSingleObject( g_hevtQuit, 0) != WAIT_OBJECT_0 )
	{
		I3TRACE("CIocpServer::~CIocpServer(): g_hevtQuit is not signal state\n");
	}
	
	if ( m_hThread )
	{
		WaitForSingleObject( m_hThread, 500);
		GetExitCodeThread ( m_hThread, &exitCode );
		if (exitCode == STILL_ACTIVE )
		{
			::TerminateThread( m_hThread, 0 );
			m_hThread = NULL;
		}
	}
	
	if( m_socket != INVALID_SOCKET )
	{
		closesocket( m_socket );
		m_socket = INVALID_SOCKET;
	}

	if( m_hEvent != NULL )
	{
		CloseHandle( m_hEvent);
		m_hEvent = NULL;
	}
}

DWORD WINAPI CUdpService::ChildThread( LPVOID lpParameter)
{
	THREAD_OBJ threadObj;
	threadObj.m_hEvent = ((THREAD_OBJ*)lpParameter)->m_hEvent;
	threadObj.m_socket = ((THREAD_OBJ*)lpParameter)->m_socket;
	delete (THREAD_OBJ*)lpParameter;
	
	HANDLE Handles[2] = { g_hevtQuit, threadObj.m_hEvent };

	int rc = 0;
	
	while ( 1 )
	{
		 rc = WaitForMultipleObjects(
                2,
                Handles,
                FALSE,
                1
                );
        if (rc == WAIT_FAILED )
        {
            DSERROR("ChildThread: WaitForMultipleObjects failed: %d\n", GetLastError());
            break;
        }
        else
        {
            if( WAIT_OBJECT_0 == rc )
			{
				DSERROR("ChildThread: Quit Event Detected\n" ); // quit - 로그용
				break;
			}
			
            rc = WaitForSingleObject( Handles[1], 0 ); // socket event
            if ( WAIT_FAILED == rc )
            {
				DSERROR("ChildThread: WaitForSingleObject failed: %d\n", GetLastError());
                break;    
            }
			
			if ( WAIT_OBJECT_0 == rc )
			{ 
				if ( HandleIo( &threadObj ) == SOCKET_ERROR)
				{
					// do nothing
				}
			}
        }
	}
	return 0;

}

int CUdpService::HandleIo( THREAD_OBJ* pThreadObj )
{
	WSANETWORKEVENTS nevents;

    int rc = WSAEnumNetworkEvents(
			pThreadObj->m_socket,
			pThreadObj->m_hEvent,
			&nevents
            );
    if (rc == SOCKET_ERROR)
    {
        DSERROR("HandleIo: WSAEnumNetworkEvents failed: %d\n", WSAGetLastError());
        return SOCKET_ERROR;
    }

    if (nevents.lNetworkEvents & FD_READ)
    {
        // Check for read error
        if (nevents.iErrorCode[FD_READ_BIT] == 0)
        {
            rc = ReceivePendingData( pThreadObj->m_socket );
            if (rc == -1)
            {
                g_pStatistics->IncrementErrors();
				return SOCKET_ERROR;
            }
            rc = SendPendingData( pThreadObj->m_socket );
            if (rc == -1)
            {
                return SOCKET_ERROR;
            }
        }
        else
        {
            DSERROR( "HandleIo: FD_READ error %d\n", 
                    nevents.iErrorCode[FD_READ_BIT]);
            return SOCKET_ERROR;
        }
    }
    if ( nevents.lNetworkEvents & FD_WRITE)
    {
        // Check for write error
		if (nevents.iErrorCode[FD_WRITE_BIT] == 0)
        {
            rc = SendPendingData( pThreadObj->m_socket );
            if (rc == -1)
            {
                return SOCKET_ERROR;
            }
        }
        else
        {
            DSERROR( "HandleIo: FD_WRITE error %d\n",
                    nevents.iErrorCode[FD_WRITE_BIT]);
            return SOCKET_ERROR;
        }
    }
    return NO_ERROR;
}


int CUdpService::ReceivePendingData( SOCKET s )
{
	int         rc,
                ret;

	CUdpBuffer* pRecvBuf = NULL;
	g_pUdpBufferPool->GetBuffer(&pRecvBuf);
	if ( pRecvBuf == NULL )
	{
		DSERROR( "ReceivePendingData: GET_BUFFER\n");
		return SOCKET_ERROR;
	}
    
    rc = recvfrom(
            s,
			pRecvBuf->GetBuffer(),
			UDP_SEND_PACKET_SIZE,
            0,
			(SOCKADDR *)pRecvBuf->GetAddress(),
			pRecvBuf->GetAddressSize()
            );
    
    if (rc == SOCKET_ERROR)
    {
        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            // Socket connection has failed, close the socket
            DSERROR("recv(from) failed: %d\n", WSAGetLastError());
            ret = SOCKET_ERROR;
        }
        else
        {
            ret = WSAEWOULDBLOCK;
        }
    }
    else if (rc == 0)
    {
        ret = SOCKET_ERROR;
    }
    else
    {
		DSMSG("[UDPS] RECV %d Bytes\n", rc);
        // Read data, updated the counters
		pRecvBuf->SetBufferSize( rc );
		ret = 0;

		if( g_pConfig->IsValidBattleServer( pRecvBuf->GetIp() ) )
		{
			PHYSICS_PACKET_HEADER* pHead = (PHYSICS_PACKET_HEADER*)pRecvBuf->GetBuffer();
			if ( pHead->m_ui8StageOrd != STAGE_ORD_NA && pHead->m_ui8StageOrd < STAGE_ORD_MAX ) 
			{
				if( g_pDataArchive->Push( pHead->m_ui8StageOrd - 1, pRecvBuf ) == FALSE )
				{
					DSERROR( "ERR g_pRecvQueue->Push\n" );
					ret = SOCKET_ERROR;
				}
				// trace
				/*{
					in_addr in;
					in.S_un.S_addr= uiIP;
					I3TRACE("Dedicated ip: %u, %s\n", uiIP, inet_ntoa(in) );
				}*/
			}
			else
			{
				ret = SOCKET_ERROR;
			}
		}
		else
		{
			DSERROR( "ERR unknown ip address: %s\n", pRecvBuf->GetIpString() );
			ret = SOCKET_ERROR;
		}
    }
    
	if( ret != 0 && pRecvBuf != NULL )
	{
		g_pUdpBufferPool->ReleaseBuffer( pRecvBuf );
	}
	
	return ret;
}

int	CUdpService::SendPacket( CUdpBuffer* pBuf )	// 일반 데이터용
{
	int rc = ::sendto( m_socket, pBuf->GetBuffer(), pBuf->GetBufferSize(), 0, (const sockaddr*) pBuf->GetAddress(), *pBuf->GetAddressSize() );
	return rc;
}

int CUdpService::SendPendingData( SOCKET s )
{
    CUdpBuffer* pSendBuf = NULL;

    int         ret = 0,
                rc = 0;

    // Attempt to dequeue all the buffer objects on the socket
	g_pSendQueue->Pop((i3ElementBase**)&pSendBuf );
	while ( pSendBuf != NULL )
    {
        rc = sendto(
                s,
				pSendBuf->GetBuffer(),
				pSendBuf->GetBufferSize(),
                0,
				(SOCKADDR *)pSendBuf->GetAddress(),
				*pSendBuf->GetAddressSize()
                );
        if ( rc == SOCKET_ERROR)
        {
            if ( WSAGetLastError() == WSAEWOULDBLOCK)
            {
				DSERROR( "WSAEWOULDBLOCK\n" );
                ret = WSAEWOULDBLOCK;
            }
            else
            {
                ret = SOCKET_ERROR;
				g_pSendQueue->Push( pSendBuf );
            }
            break;
        }
        else
        {
			ret = 0;
			g_pUdpBufferPool->ReleaseBuffer( pSendBuf );
			DSMSG("[UDPS] SENT %d Bytes OK\n", rc);
        }

		g_pSendQueue->Pop( (i3ElementBase**)&pSendBuf );
    }
 
	if ( ret != 0  && pSendBuf != NULL )
	{
		g_pUdpBufferPool->ReleaseBuffer( pSendBuf );
	}

    return ret;
}