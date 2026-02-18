#include "pch.h"
#include "WatchUDPSocket.h"

I3_CLASS_INSTANCE( CWatchUDPSocket, i3ElementBase );

CWatchUDPSocket::CWatchUDPSocket()
{
	m_hSocket			= INVALID_SOCKET;
	i3mem::FillZero( &m_RecvAddr, sizeof( sockaddr ) );
	i3mem::FillZero( m_pszReciveBuffer, sizeof(m_pszReciveBuffer));
	m_ui32SendCount		= 0;
	m_ui32SendBytes		= 0;
	m_ui32RecvCount		= 0;
	m_ui32RecvBytes		= 0;
}

CWatchUDPSocket::~CWatchUDPSocket()
{
	Destroy();
}

BOOL CWatchUDPSocket::Create( UINT16 ui16BindPort )
{
	SOCKADDR_IN	localAddr;

	m_hSocket	= ::socket( AF_INET, SOCK_DGRAM, 0 );

	if( m_hSocket == INVALID_SOCKET )
	{
		i3Net::GetError( "Socket create failed.\n" );
		return FALSE;
	}

	// bind
	i3mem::FillZero( &localAddr, sizeof(SOCKADDR_IN) );
	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= htonl( INADDR_ANY );
	localAddr.sin_port			= htons( ui16BindPort );

	if( ::bind( m_hSocket, (struct sockaddr *) &localAddr, sizeof( localAddr ) ) == SOCKET_ERROR )
	{
		i3Net::GetError( "UDP bind failed.\n" );
		return FALSE;
	}

	return TRUE;
}

void CWatchUDPSocket::CloseSocket()
{
	INT32	i32result;
	if( m_hSocket != INVALID_SOCKET )
	{
		i32result = ::shutdown( m_hSocket, SD_BOTH );
		if( i32result < 0 )
		{
			i3Net::GetError( "UDP shutdown.\n" );
			return;
		}

		i32result = ::closesocket( m_hSocket );
		if( i32result < 0 )
		{
			i3Net::GetError( "UDP close\n" );
			return;
		}

		m_hSocket = INVALID_SOCKET;
	}
}

BOOL CWatchUDPSocket::Destroy()
{
	CloseSocket();
	return TRUE;
}

INT32 CWatchUDPSocket::Send( char * pBuf, INT32 i32BufferSize, INT32 i32Size, UINT32 ui32IP, UINT16 ui16Port )
{
	INT32	i32Rv;
	SOCKADDR_IN cast;
	cast.sin_family			= AF_INET;
	cast.sin_addr.s_addr	= ui32IP;
	cast.sin_port			= ::htons( ui16Port );

	if( g_Encryption(pBuf, (INT16)i32Size, sizeof(pBuf)) < 0 )
	{
		I3NOTICE( " Encryption invalid.\n " );
		return -1;
	}

	if( m_hSocket == INVALID_SOCKET )
	{
		I3NOTICE( "UDP Socket invalid.\n" );
		return -1;
	}

	i32Rv = ::sendto( m_hSocket, pBuf, i32Size, 0, (const sockaddr *)&cast, sizeof(sockaddr) );

	if( i32Rv == SOCKET_ERROR )
	{
		i3Net::GetError( "UDPSocket Send()");
		I3NOTICE( "UDPSocket Send() IP %s Port :%d", inet_ntoa( cast.sin_addr ), cast.sin_port);
	}
	else if( i32Rv != i32Size )
	{	
		char strTrace[ 256] = {0,};
		i3String::Format( strTrace, 256, "UDP Send size not equal. %d, %d\n", i32Size, i32Rv);
		I3NOTICE( strTrace);
	}

	m_ui32SendCount++;
	m_ui32SendBytes += i32Size;

	return i32Rv;
}

INT32 CWatchUDPSocket::NomalSend( char * pBuf, INT32 i32BufferSize, INT32 i32Size, UINT32 ui32IP, UINT16 ui16Port )
{
	INT32	i32Rv;
	SOCKADDR_IN cast;
	cast.sin_family			= AF_INET;
	cast.sin_addr.s_addr	= ui32IP;
	cast.sin_port			= ::htons( ui16Port );

	if( g_WatchEncryption(pBuf, (INT16)i32Size, (INT16)i32BufferSize) < 0 )
	{
		I3NOTICE( " Encryption invalid.\n " );
		return -1;
	}

	if( m_hSocket == INVALID_SOCKET )
	{
		I3NOTICE( "UDP Socket invalid.\n" );
		return -1;
	}

	i32Rv = ::sendto( m_hSocket, pBuf, i32Size, 0, (const sockaddr *)&cast, sizeof(sockaddr) );

	if( i32Rv == SOCKET_ERROR )
	{
		i3Net::GetError( "UDPSocket Send()");
		I3NOTICE( "UDPSocket Send() IP %s Port :%d", inet_ntoa( cast.sin_addr ), cast.sin_port);
	}
	else if( i32Rv != i32Size )
	{	
		char strTrace[ 256] = {0,};
		i3String::Format( strTrace, 256, "UDP Send size not equal. %d, %d\n", i32Size, i32Rv);
		I3NOTICE( strTrace);
	}

	m_ui32SendCount++;
	m_ui32SendBytes += i32Size;

	return i32Rv;
}

INT32 CWatchUDPSocket::Recv( void )
{
	INT32	i32len	= sizeof( sockaddr );
	INT32	i32RecvSize;

	i32RecvSize = ::recvfrom( m_hSocket, m_pszReciveBuffer, UDP_RECV_PACKET_SIZE, 0, &m_RecvAddr, &i32len );

	if( g_Decryption(m_pszReciveBuffer, (INT16)i32RecvSize, sizeof(m_pszReciveBuffer)) < 0 )
	{
		I3NOTICE( " Decryption invalid.\n " );
		return -1;
	}

	m_ui32RecvCount++;
	m_ui32RecvBytes += i32RecvSize;

	return i32RecvSize;
}
