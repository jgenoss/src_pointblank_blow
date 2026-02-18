#include "i3NetworkDef.h"
#include "i3NetworkServerSocket.h"

I3_CLASS_INSTANCE(i3NetworkServerSocket);

bool i3NetworkServerSocket::OnCreate( UINT32 IpNo, UINT16 portNo, int backlogCount)
{
	struct sockaddr_in sockAddr;

	::i3mem::FillZero( &sockAddr, sizeof( sockAddr ) );
	sockAddr.sin_family			= AF_INET;
	sockAddr.sin_addr.s_addr	= IpNo;
	sockAddr.sin_port			= ::htons( portNo );

	//Set Socket 
	m_ListenSock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );	
	if(m_ListenSock == INVALID_SOCKET) 
	{
		I3TRACE("[i3NetworkServerSocket::OnCreate] m_ListenSock is Invalid Socket \n");
		return false; 
	}

	//Sets a socket option
	INT32 reuse = 1;		
	INT32 err = ::setsockopt( m_ListenSock, SOL_SOCKET, SO_REUSEADDR, ( char* )&reuse, sizeof( reuse ) );

	if (err == SOCKET_ERROR)
	{
		I3PRINTLOG(I3LOG_WARN, "sockopt err1!!");
	}

	//Controls the I/O mode of a socket 비동기식으로 설정 
	unsigned long argp = 1;
	::ioctlsocket( m_ListenSock, FIONBIO, &argp );

	//Bind Socket 
	if( ::bind( m_ListenSock, ( struct sockaddr * )&sockAddr, sizeof( sockAddr ) ) == SOCKET_ERROR)
	{	
		I3TRACE("[i3NetworkServerSocket::OnCreate]Error Bind \n"); 		
		return false; 
	}

	//Listen Socket
	if(	::listen( m_ListenSock, backlogCount ) == SOCKET_ERROR ) 
	{
		I3TRACE("[i3NetworkServerSocket::OnCreate]Error Listen \n"); 
		return false; 
	}

	return true; 
}

void i3NetworkServerSocket::OnDestroy(void)
{
	if( m_ListenSock != INVALID_SOCKET )::closesocket( m_ListenSock );
	m_ListenSock = INVALID_SOCKET; 
	return;
}

#pragma warning(push)
#pragma warning(disable: 4127)  // warning C4127: conditional expression is constant

SOCKET	i3NetworkServerSocket::AcceptConnection( struct timeval * ptimeout, struct sockaddr_in * pAddr, INT32 * pSize )
{
	SOCKET AcceptSocket = INVALID_SOCKET;

	fd_set fds; 
	FD_ZERO(&fds); 
	FD_SET( m_ListenSock, &fds );

	if(::select( 0, &fds, NULL, NULL, ptimeout) == SOCKET_ERROR )
	{
		return AcceptSocket; 
	}

	if( FD_ISSET( m_ListenSock, &fds ) )
	{
		AcceptSocket = ::accept( m_ListenSock, (struct sockaddr *)pAddr, pSize);
	}

	return AcceptSocket; 
}

#pragma warning(pop)
