#include "pch.h"
#include "UdpBufferPool.h"
#include "UdpBuffer.h"
#include "TrackBuffer.h"
#include "ModulePhysics.h"
#include "HackCheckerMain.h"

CModulePhysics* g_pModulePhysics = NULL;
HANDLE CModulePhysics::m_hThreadPhysicsRecv = NULL;

static USHORT s_LocalPort =		0xFFFD;
SOCKET CModulePhysics::m_socket  = INVALID_SOCKET;

CModulePhysics::CModulePhysics()
{
	m_socket = INVALID_SOCKET;
}

CModulePhysics::~CModulePhysics()
{
	DWORD ui32ExitCode = 0;

	// check quit
	if( WaitForSingleObject( g_hevtQuit, 0 ) != WAIT_OBJECT_0 )
	{
		I3TRACE("CTaskMgr::~CTaskMgr(): g_hevtQuit is not signal state\n");
	}

	// check thread exit code
	if( m_hThreadPhysicsRecv != NULL )
	{
		WaitForSingleObject(m_hThreadPhysicsRecv, 16);
		GetExitCodeThread( m_hThreadPhysicsRecv, &ui32ExitCode );
		if( ui32ExitCode == STILL_ACTIVE )
		{
			TerminateThread( m_hThreadPhysicsRecv, (DWORD)-1);
		}
		m_hThreadPhysicsRecv = NULL;
	}
		
	// close socket
	if( m_socket != INVALID_SOCKET) closesocket( m_socket );
}

BOOL CModulePhysics::Create( INT32 i32BufferSize )
{
	m_hThreadPhysicsRecv = CreateThread( NULL, 4096 * 1024, CModulePhysics::ThreadPhysicsRecv, NULL, 0, NULL );
	if( m_hThreadPhysicsRecv == NULL ) return FALSE;
	
	m_socket = _CreateUdpSocket();
	if( m_socket == INVALID_SOCKET ) return FALSE;
	return TRUE;
}

SOCKET CModulePhysics::_CreateUdpSocket()
{
	SOCKET s = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( s == INVALID_SOCKET ) return INVALID_SOCKET;

	// relay에서는 100, 100을 사용. 200,200으로 바꿨는데도 문제 남아서, 변경.
	INT32 i32RecvBufSize = 1024 * 1024 * 100;
	INT32 i32SendBufSize = 2147483647; //1024 * 1024 * 300;
	INT32 i32Len = 0;
	setsockopt( s, SOL_SOCKET, SO_RCVBUF, (const char *) &i32RecvBufSize, sizeof(INT32));
	setsockopt( s, SOL_SOCKET, SO_SNDBUF, (const char *) &i32SendBufSize, sizeof(INT32));
	getsockopt( s, SOL_SOCKET, SO_RCVBUF, (char *) &i32RecvBufSize, &i32Len);
	getsockopt( s, SOL_SOCKET, SO_SNDBUF, (char *) &i32SendBufSize, &i32Len);

	//struct timeval tv_timeo = { 3, 500000 };  /* 3.5 second */
	struct timeval tv_timeo = { 0, 100000 };  /* 0.1 second */
	setsockopt( s, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv_timeo, sizeof( tv_timeo ) );
	
	// bind
	SOCKADDR_IN	localAddr;
	memset( &localAddr, 0,  sizeof(localAddr));

	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= htonl( INADDR_ANY );
	localAddr.sin_port			= htons( s_LocalPort-- );

	if( bind( s, (struct sockaddr *) &localAddr, sizeof( localAddr ) ) == SOCKET_ERROR )
	{
		closesocket(s);
		DSERROR( "UDP bind failed.\n" );
		return INVALID_SOCKET;
	}
	
	// set socket to non-blocking
	u_long iMode = 1;
	if ( ioctlsocket( s, FIONBIO, &iMode) == SOCKET_ERROR  )
	{
		closesocket(s);
		DSERROR( "UDP non-blocking failed.\n" );
		return INVALID_SOCKET;
	} 

	// turn off icmp
	DWORD    dwBytes = 0;
	DWORD    i32OptionValue = 0;
	if( WSAIoctl( s, SIO_UDP_CONNRESET, &i32OptionValue, sizeof(i32OptionValue), NULL, 0, &dwBytes, NULL, NULL ) == SOCKET_ERROR)
	{
		closesocket(s);
		DSERROR( "WSAIoctl: SIO_UDP_CONNRESET failed: %d\n", WSAGetLastError());
		return INVALID_SOCKET;
	}

	return s;
}


BOOL CModulePhysics::EnqueueToHackChecker( CUdpBuffer*	pRecvBuf )
{
	if( g_pHackChecker == NULL) return FALSE;

	// 해킹 판정일 경우에만 HackChecker에 알린다 
	PHYSICS_PACKET_HEADER* pHead	= (PHYSICS_PACKET_HEADER*)pRecvBuf->GetBuffer();
	UINT32				   nResult	= *(UINT32*)( pRecvBuf->GetBuffer() + sizeof(PHYSICS_PACKET_HEADER) );
	if( nResult == HACK_TYPE_NO ) return TRUE;

	DS_CHECK_PACKET_WRAPPER	* pCheckPacket = g_pHackChecker->Get_Check_Packet_Physics(); 
	if( pCheckPacket != NULL)
	{
		//header
		pCheckPacket->m_ui32DataType	= DS_HACK_DATA_PHYSICS;
		pCheckPacket->m_ui16GroupIdx	= (UINT16)pHead->m_ui16GroupIdx;	
		pCheckPacket->m_ui8SlotIdx		= (UINT8) pHead->m_ui8SlotIdx;	
		pCheckPacket->m_ui16Size		= sizeof(UINT32);
		//data
		*(UINT32*)pCheckPacket->m_pData = nResult;
		// check list
		pCheckPacket->m_ui32ChkList		= DS_HACK_CHECK_WALLSHOT; // wall shot 1개 밖에 없다.
		g_pHackChecker->Get_Check_PacketIdx_Physics(); 
		return TRUE;
	}
	else
	{
		DSERROR("g_pHackChecker->Get_Check_Packet_Physics():CMoudlePhysics\n");
		return FALSE;
	}
}

DWORD WINAPI CModulePhysics::ThreadPhysicsRecv( LPVOID lpParameter )
{
	WSABUF wsabuf;

	int rc = 0;
	DWORD dwBytesRecvd = 0;
	DWORD dwFlags = 0;
	
	// get udp buffer
	CUdpBuffer recvBuf; 
	
	while (1)
	{
		if( WaitForSingleObject( g_hevtQuit, 1 ) == WAIT_OBJECT_0 ) return 0;
		if( CModulePhysics::m_socket == INVALID_SOCKET) 
		{
			Sleep(1);
			continue;
		}
		
		wsabuf.buf = recvBuf.GetBuffer();
		wsabuf.len = UDP_SEND_PACKET_SIZE;
		dwBytesRecvd = 0;

		recvBuf.Init();
		rc = WSARecvFrom( CModulePhysics::m_socket, &wsabuf, 1, &dwBytesRecvd, &dwFlags, (sockaddr*)recvBuf.GetAddress(), recvBuf.GetAddressSize(), NULL, NULL);
		if( rc == 0 && dwBytesRecvd > 0 )
		{
			I3TRACE("RECV FROM PX %dBYTES\n", dwBytesRecvd);
			CModulePhysics::EnqueueToHackChecker( &recvBuf );
		}
		else
		{
			if( rc == SOCKET_ERROR )
			{
				INT32 i32Error = WSAGetLastError();
				if( i32Error != WSAEWOULDBLOCK )
				{
					DSERROR("[Module Physics] recvfrom (%d)\n", WSAGetLastError() );
				}
				else
				{
					Sleep(1);
				}
			}
		}
	}
}

BOOL CModulePhysics::SendToPhysicsServer( CUdpBuffer* pSendBuf )
{
	WSABUF wsabuf;
	wsabuf.buf = pSendBuf->GetBuffer();
	wsabuf.len = pSendBuf->GetBufferSize();
	DWORD dwBytesSent = 0;
	int rc = WSASendTo( m_socket, &wsabuf, 1, &dwBytesSent, 0, (const sockaddr*)pSendBuf->GetAddress(), *pSendBuf->GetAddressSize(), NULL, NULL );
	if( rc == SOCKET_ERROR )
	{
		INT32 i32Error = WSAGetLastError();
		if( i32Error != WSAEWOULDBLOCK )
		{
			DSERROR("[Module Physics] sendto (%d)\n",i32Error );
			return FALSE;
		}
	}
	else
	{
		I3TRACE("SENT OK %d Bytes\n", dwBytesSent );
	}
	return TRUE;
}