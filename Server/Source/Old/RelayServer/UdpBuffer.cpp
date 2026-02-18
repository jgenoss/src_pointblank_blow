#include "pch.h"
#include "UdpBuffer.h"

I3_CLASS_INSTANCE( CUdpBuffer, i3ElementBase );

#define UDPPROTOCOL_HEADER_SIZE		sizeof(N_GAME_HEADER)

CUdpBuffer::CUdpBuffer()
{
	i3mem::FillZero( m_pBuffer, UDP_SEND_PACKET_SIZE );

	i3mem::FillZero(&m_SocketAddress, sizeof(sockaddr_in));	
	m_nSocketAddressSize = sizeof( sockaddr_in );

	m_SocketAddress.sin_family = AF_INET;

	m_sReadPosition		= UDPPROTOCOL_HEADER_SIZE;
	m_sWritePosition	= UDPPROTOCOL_HEADER_SIZE;

	m_pHead				= NULL;
	m_pRelay			= NULL;
}

CUdpBuffer::~CUdpBuffer()
{
	// Do nothing
}

void CUdpBuffer::InitReceive(void)
{
	m_sReadPosition		= UDPPROTOCOL_HEADER_SIZE;
//	m_sWritePosition	= UDPPROTOCOL_HEADER_SIZE;	
}

void CUdpBuffer::InitSend(void)
{
	N_GAME_HEADER* Head = (N_GAME_HEADER*)m_pBuffer;
	Head->_Size			= UDPPROTOCOL_HEADER_SIZE;

//	m_sReadPosition		= UDPPROTOCOL_HEADER_SIZE;
	m_sWritePosition	= UDPPROTOCOL_HEADER_SIZE;
}

void CUdpBuffer::ReadData( void* pBuffer, UINT16 sSize )
{
	if ( m_sReadPosition + sSize < UDP_SEND_PACKET_SIZE )
	{
		i3mem::Copy( pBuffer, m_pBuffer + m_sReadPosition, sSize );
		m_sReadPosition = m_sReadPosition + sSize;
	}
	else
	{
		i3mem::FillZero( pBuffer, sSize );
	}
}

void CUdpBuffer::WriteData( void* pBuffer, UINT16 sSize )
{
	if ( m_sWritePosition + sSize < UDP_SEND_PACKET_SIZE )
	{
		i3mem::Copy( m_pBuffer + m_sWritePosition, pBuffer, sSize );
		m_sWritePosition = m_sWritePosition + sSize;

		N_GAME_HEADER* Head = (N_GAME_HEADER*)m_pBuffer;
		Head->_Size		= m_sWritePosition;
	}
}

// -----------------------------------------------------------
// Name : ReadAppendData
// Desc : 패킷으로부터 헤더, 릴레이 패킷을 때어낸다.
// -----------------------------------------------------------
void CUdpBuffer::ReadAppendData( )
{
	m_pHead		= (N_GAME_HEADER*)m_pBuffer;	
	m_pRelay	= (N_GAME_RELAY*)((m_pBuffer + m_pHead->_Size) - sizeof(N_GAME_RELAY));
	m_pHead->_Size -= sizeof(N_GAME_RELAY);
}

void CUdpBuffer::CopyBufferFrom( CUdpBuffer* pSource )
{
	i3mem::Copy( m_pBuffer, (char*)pSource->m_pBuffer, (UINT32)(pSource->GetBufferSize()) );

	m_sReadPosition		= pSource->GetReadPosition();
	m_sWritePosition	= pSource->GetWritePosition();
}

void CUdpBuffer::SetAddress( UINT32 wIP, UINT16 sPort )
{
	m_SocketAddress.sin_addr.s_addr = wIP;
	m_SocketAddress.sin_port		= sPort;
}