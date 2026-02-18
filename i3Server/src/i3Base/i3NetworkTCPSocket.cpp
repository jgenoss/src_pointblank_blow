#include "i3CommonType.h"
#include "i3NetworkPacket.h"
#include "i3NetworkTCPSocket.h"
#include "i3Memory.h"
#include "i3Inet.h"

I3_CLASS_INSTANCE( i3NetworkTCPSocket, i3ElementBase );

i3NetworkTCPSocket::i3NetworkTCPSocket()
{
	m_Socket				= INVALID_SOCKET; 
	m_hEvent				= INVALID_HANDLE_VALUE; 
	m_ReceivedPacketSize	= 0; 

	m_pAgent				= NULL;
	m_pCallbackFunc			= NULL; 

	m_nSendDataLen			= 0;    // length of data to send
	m_nBytesSent			= 0;      // bytes sent so far

	i3mem::FillZero( &m_LocalAddr, sizeof( I3SOCKETADDR ) );
}

i3NetworkTCPSocket::~i3NetworkTCPSocket()
{
	OnDestroy();
}

BOOL i3NetworkTCPSocket::OnCreate( char * pszAddress, UINT16 nPort, CallbackRoutine CallBack, void* pAgent )
{
	m_hEvent		= ::WSACreateEvent();
	m_Socket		= ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if( m_Socket == INVALID_SOCKET )
	{		
		I3TRACE("[i3NetworkClientSocket::OnConnectTo] Error Socket Is Invalid \n"); 
		return FALSE; 
	}

	struct sockaddr_in serv_addr;
	i3mem::FillZero( &serv_addr, sizeof(serv_addr)); 
	serv_addr.sin_family		= AF_INET; 
	serv_addr.sin_addr.s_addr	= ::inet_addr(pszAddress); 
	serv_addr.sin_port			= htons( nPort ); 

	if( ::connect( m_Socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) != 0)
	{		
		i3Net::GetError("[i3NetworkClientSocket::OnConnectTo]"); 
		return FALSE; 
	}

	m_pAgent		= pAgent;
	m_pCallbackFunc = CallBack; 
	
	return TRUE;
}


BOOL i3NetworkTCPSocket::OnDestroy()
{
	if(  m_hEvent != INVALID_HANDLE_VALUE )
	{
		::WSACloseEvent( m_hEvent );
		m_hEvent = INVALID_HANDLE_VALUE;
	}

	if( m_Socket != INVALID_SOCKET )
	{
		::shutdown( m_Socket, SD_BOTH); 
		::closesocket( m_Socket );
		m_Socket = INVALID_SOCKET; 
	}

	return TRUE;
}

void i3NetworkTCPSocket::SelectEvent()
{
	WSANETWORKEVENTS nEvent; 
	i3mem::FillZero( &nEvent, sizeof( nEvent ) ); 
	::WSAEventSelect( m_Socket, m_hEvent, FD_WRITE | FD_READ | FD_CLOSE ); 
	::WSAEnumNetworkEvents( m_Socket, m_hEvent, &nEvent ); 

	if( (nEvent.lNetworkEvents & FD_READ )	== FD_READ)		OnReceive();
	if( (nEvent.lNetworkEvents & FD_WRITE ) == FD_WRITE)	OnSend();
	if( (nEvent.lNetworkEvents & FD_CLOSE ) == FD_CLOSE)	OnDestroy();

	return;
}

BOOL i3NetworkTCPSocket::OnSend()
{
	OVERLAPPED Overlapped;

	BOOL	Rv = TRUE;

	if( m_nBytesSent < m_nSendDataLen )
	{
		DWORD nBytes;

		i3mem::FillZero( &Overlapped, sizeof( OVERLAPPED ) );
		Rv = ::WriteFile( (HANDLE) m_Socket, m_SendBuffer + m_nBytesSent, m_nSendDataLen - m_nBytesSent, &nBytes, &Overlapped );
		if( !Rv )
		{
			if( !(::GetLastError() == ERROR_IO_PENDING) ) 
			{	
				i3Net::GetError("[i3NetworkClientSocket::OnSend]");
				m_nBytesSent = 0;
				m_nSendDataLen = 0;
				return Rv;
			}
		}
		else
			m_nBytesSent += nBytes;
	}

	if (m_nBytesSent == m_nSendDataLen)
		m_nBytesSent = m_nSendDataLen = 0;

	return Rv;
}


void i3NetworkTCPSocket::OnReceive()
{
	i3NetworkPacket RecvPacket; 
	DWORD			nBytes; 

	OVERLAPPED		Overlapped;
	i3mem::FillZero( &Overlapped, sizeof( OVERLAPPED ) );
	if( ::ReadFile( (HANDLE)m_Socket, m_ReceiveBuffer, PACKETBUFFERSIZE, &nBytes, &Overlapped ) )
	{
		m_ReceivedPacketSize += nBytes; 

		if(m_ReceivedPacketSize > PACKETBUFFERSIZE) 
		{
			I3TRACE("[i3NetworkClientSocket::OnReceive]Error Packet Size %d \n", m_ReceivedPacketSize); 
			return; 
		}

		INT32 StartPos = 0; 
		while( m_ReceivedPacketSize > 0 )
		{
			RecvPacket.CopyToBuffer( m_ReceiveBuffer, m_ReceivedPacketSize, StartPos );

			if( m_pCallbackFunc == NULL )
			{
				PacketParsing( &RecvPacket );
			}
			else
			{
				( *m_pCallbackFunc )( &RecvPacket );
			}

			m_ReceivedPacketSize	-= RecvPacket.GetPacketSize(); 
			StartPos				+= RecvPacket.GetPacketSize();
		}
	}
	else
	{
		if( GetLastError() != ERROR_IO_PENDING )
		{
			i3Net::GetError("[i3NetworkClientSocket::OnReceive]");
		} 
	}
	return; 
}

void i3NetworkTCPSocket::PacketParsing(i3NetworkPacket * pPacket)
{
	return; 
}

INT32 i3NetworkTCPSocket::SendMessage(i3NetworkPacket * pPacket)
{
	INT32		nBuflen = pPacket->GetPacketSize();
		 
	if (m_nSendDataLen != 0 || nBuflen > PACKETBUFFERSIZE)
	{	
		I3TRACE( "i3NetworkTCPSocket::SendMessage() can't accept more data\n" );
		return -1;
	}
	
	memcpy( m_SendBuffer, pPacket->GetPacketBuffer(), nBuflen );
	m_nSendDataLen = nBuflen;
	m_nBytesSent = 0;
	if( !OnSend() )
		return -1;
	
	return nBuflen; 
}

void i3NetworkTCPSocket::GetName( SOCKADDR_IN * plocaladdr )
{
	sockaddr	addr;
	INT32	len = sizeof( sockaddr );

	::getsockname( m_Socket, &addr, &len );

	i3mem::Copy( &m_LocalAddr, &addr, sizeof( I3SOCKETADDR ) );

	if( plocaladdr != NULL )
	{
		i3mem::Copy( plocaladdr, &m_LocalAddr, sizeof( SOCKADDR_IN ) );
	}
}
