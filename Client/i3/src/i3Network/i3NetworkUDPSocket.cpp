#include "i3NetworkDef.h"
#include "i3NetworkUDPSocket.h"

I3_CLASS_INSTANCE( i3NetworkUDPSocket);

i3NetworkUDPSocket::i3NetworkUDPSocket()
{
	i3mem::FillZero( &m_RecvAddr, sizeof( sockaddr ) );
}

i3NetworkUDPSocket::~i3NetworkUDPSocket()
{
	Destroy();
}

bool i3NetworkUDPSocket::Create( INT32 nBufSize, UINT16 nBindPort )
{
	SOCKADDR_IN	localAddr;
	i3mem::FillZero( &localAddr, sizeof(localAddr) );
	
	m_hSocket	= ::socket( AF_INET, SOCK_DGRAM, 0 );

	if( m_hSocket == INVALID_SOCKET )
	{
		i3Net::GetError( "Socket create failed.\n" );
		return false;
	}
	
	// bind
	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= htonl( INADDR_ANY );
	localAddr.sin_port			= htons( nBindPort );

	if( ::bind( m_hSocket, (struct sockaddr *) &localAddr, sizeof( localAddr ) ) == SOCKET_ERROR )
	{
		i3Net::GetError( "UDP bind failed.\n" );
		return false;
	}

	// Buffer create
	m_nBufSize		= nBufSize;
	m_pszReciveBuffer = (char *) i3MemAlloc( m_nBufSize );
	i3mem::FillZero( m_pszReciveBuffer, sizeof( m_nBufSize ));

	return true;
}

bool i3NetworkUDPSocket::Destroy()
{
	INT32	result;

	if( m_hSocket != INVALID_SOCKET )
	{
		result = ::shutdown( m_hSocket, SD_BOTH );
		if( result < 0 )
		{
			i3Net::GetError( "UDP shutdown.\n" );
			return false;
		}
		result = ::closesocket( m_hSocket );
		if( result < 0 )
		{
			i3Net::GetError( "UDP close\n" );
			return false;
		}

		m_hSocket = INVALID_SOCKET;
	}

	I3MEM_SAFE_FREE_POINTER( m_pszReciveBuffer );

	return true;
}

INT32 i3NetworkUDPSocket::Send( char * pBuf, INT32 nSize, UINT32 nIP, UINT16 nPort )
{
	INT32	nRv;
	SOCKADDR_IN cast;
	cast.sin_family			= AF_INET;
	cast.sin_addr.s_addr	= nIP; 
	cast.sin_port			= ::htons( nPort );

	if( m_hSocket == INVALID_SOCKET )
	{
		I3TRACE( "UDP Socket invalid.\n" );
		return -1;
	}
	
	nRv = ::sendto( m_hSocket, pBuf, nSize, 0, (const sockaddr *)&cast, sizeof(sockaddr) );
	if( nRv < 0 )
	{
		i3Net::GetError( "UDPSend failed\n" );
	}

	return nRv;
}

INT32 i3NetworkUDPSocket::Recv( void )
{
	INT32		len	= sizeof( sockaddr );

	return ::recvfrom( m_hSocket, m_pszReciveBuffer, m_nBufSize, 0, &m_RecvAddr, &len );
}
