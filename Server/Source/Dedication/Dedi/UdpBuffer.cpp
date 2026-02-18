// UdpBuffer.cpp
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//	
#include "pch.h"
#include "UdpBuffer.h"

I3_CLASS_INSTANCE( CUdpBuffer, i3ElementBase );

#define UDPPROTOCOL_HEADER_SIZE		(sizeof(N_GAME_HEADER))

CUdpBuffer::CUdpBuffer()
{
	m_ui32Operation		= OP_READ;
	m_ui32BufLen		= 0;

	m_ui32SocketIdx		= (UINT32)-1;
	m_cRef				= 0;
}

CUdpBuffer::~CUdpBuffer(){}

void CUdpBuffer::Init()
{
	i3mem::FillZero( &ol, sizeof(WSAOVERLAPPED) );
	m_ui32Operation = OP_READ;

	i3mem::FillZero( m_pBuffer, UDP_SEND_PACKET_SIZE );
	m_ui32BufLen = UDP_SEND_PACKET_SIZE;
	m_relayInfo.Reset();

	i3mem::FillZero(&m_socketAddress, sizeof(sockaddr_in));
	m_socketAddress.sin_family = AF_INET;
	m_i32SocketAddressSize = sizeof( sockaddr_in );

	m_ui32SocketIdx		= (UINT32)-1;
}

void CUdpBuffer::CopyBufferFrom( CUdpBuffer* pSource, INT32 i32Size )
{
	m_ui32BufLen = i32Size;
	memcpy_s( m_pBuffer, sizeof( m_pBuffer ), pSource->m_pBuffer, i32Size);
}

void CUdpBuffer::SetAddress( UINT32 ui32IP, UINT16 ui16Port )
{
	m_socketAddress.sin_family		= AF_INET;
	m_socketAddress.sin_addr.s_addr = ui32IP;
	m_socketAddress.sin_port		= htons(ui16Port);
}

void CUdpBuffer::SetAddress(sockaddr_in *pAddress, INT32 i32AddressSize)
{
	I3ASSERT( i32AddressSize == sizeof(sockaddr_in));
	if ( i32AddressSize == sizeof(sockaddr_in) )
	{
		memcpy_s( &m_socketAddress, sizeof(m_socketAddress), pAddress, i32AddressSize);
	}
}