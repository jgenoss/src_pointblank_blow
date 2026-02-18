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

#define UDPPROTOCOL_HEADER_SIZE		sizeof(N_GAME_HEADER)

///////////////////////////////////////////////////////////////////////////////
//
// Function: CUdpBuffer
//
// Description:
//	- constructor
//		- 
// Arguments:
// Return:
//
CUdpBuffer::CUdpBuffer()
{
	i3mem::FillZero( m_pBuffer, UDP_SEND_PACKET_SIZE );
	m_ui32BufLen = UDP_SEND_PACKET_SIZE;
	
	i3mem::FillZero(&m_socketAddress, sizeof(m_socketAddress));	
	m_socketAddress.sin_family = AF_INET;
	m_ui32SocketAddressSize = sizeof( sockaddr_in );

	m_ui16ReadPosition	= UDPPROTOCOL_HEADER_SIZE;
	m_ui16WritePosition	= UDPPROTOCOL_HEADER_SIZE;

	m_pHead				= NULL;
	m_pRelay			= NULL;

	m_ui32RecvTime		= 0;
	m_cRef				= 0;

	ol.hEvent			= NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: :~CUdpBuffer
//
// Description:
//	- desctructor
//		- 
// Arguments:
// Return:
//
CUdpBuffer::~CUdpBuffer()
{
	// Do nothing
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: Init
//
// Description:
//	- 버퍼와 어드레스 리셋
//		- 
// Arguments:
// Return:
//
void CUdpBuffer::Init()
{
	memset( &ol, 0, sizeof(WSAOVERLAPPED) );
	m_ui32Operation = OP_READ;

	memset( m_pBuffer, 0, UDP_SEND_PACKET_SIZE );
	m_ui32BufLen = UDP_SEND_PACKET_SIZE;

	memset(&m_socketAddress, 0, sizeof(sockaddr_in));
	m_socketAddress.sin_family = AF_INET;
	m_ui32SocketAddressSize = sizeof( sockaddr_in );

	m_ui32IoOrder = 0;

	m_ui32RecvTime = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: InitReceive
//
// Description:
//	- 데이터 영역의 시작 (사용되지 않음)
//		- 
// Arguments:
// Return:
//
void CUdpBuffer::InitReceive(void)
{
	m_ui16ReadPosition		= UDPPROTOCOL_HEADER_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: InitSend
//
// Description:
//	- 데이터 영역 패키지 (사용되지 않음)
//		-
// Arguments:
// Return:
//
void CUdpBuffer::InitSend(void)
{
	N_GAME_HEADER* Head = (N_GAME_HEADER*)m_pBuffer;
	Head->_Size			= UDPPROTOCOL_HEADER_SIZE;
	m_ui16WritePosition	= UDPPROTOCOL_HEADER_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ReadData
//
// Description:
//	- 패킷에서 데이터의 일부만 얻는 함수 (사용되지 않음)
//		- 
// Arguments:
//	- pBuffer	: 출력 포인터
//	- sSize		: 데이터 길이
// Return:
//
void CUdpBuffer::ReadData( void* pBuffer, UINT16 ui16Size )
{
	if ( m_ui16ReadPosition + ui16Size < UDP_SEND_PACKET_SIZE )
	{
		i3mem::Copy( pBuffer, m_pBuffer + m_ui16ReadPosition, ui16Size );
		m_ui16ReadPosition += ui16Size;
	}
	else
	{
		i3mem::FillZero( pBuffer, ui16Size );
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: WriteData
//
// Description:
//	- 버퍼에 데이터를 기록하는 함수 (사용되지 않음)
//		- 
// Arguments:
//	- pBuffer	: 입력 버퍼
//	- sSize		: 데이터 길이
// Return:
//	- 
//
void CUdpBuffer::WriteData( void* pBuffer, UINT16 ui16Size )
{
	if ( m_ui16WritePosition + ui16Size < UDP_SEND_PACKET_SIZE )
	{
		i3mem::Copy( m_pBuffer + m_ui16WritePosition, pBuffer, ui16Size );
		m_ui16WritePosition += ui16Size;

		N_GAME_HEADER* Head = (N_GAME_HEADER*)m_pBuffer;
		Head->_Size		= m_ui16WritePosition;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ReadAppendData
//
// Description:
//	- 패킷으로부터 헤더, 릴레이 패킷을 때어낸다.
//		- 
// Arguments:
// Return:
//
void CUdpBuffer::ReadAppendData( )
{
	m_pHead		= (N_GAME_HEADER*)m_pBuffer;	
	m_pRelay	= (N_GAME_RELAY*)((m_pBuffer + m_pHead->_Size) - sizeof(N_GAME_RELAY));
	m_pHead->_Size -= sizeof(N_GAME_RELAY);
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: CopyBufferFrom
//
// Description:
//	- udp 버퍼의 내용을 복사한다. 
//		- 내부 버퍼에만 적용 (사용되지 않음)
//
// Arguments:
//	- pSource : source
//
// Return:
//
void CUdpBuffer::CopyBufferFrom( CUdpBuffer* pSource )
{
	i3mem::Copy( m_pBuffer, (char*)pSource->m_pBuffer, (UINT32)(pSource->GetBufferSize()) );

	m_ui16ReadPosition	= pSource->GetReadPosition();
	m_ui16WritePosition	= pSource->GetWritePosition();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: SetAddress
//
// Description:
//	- UDP client address를 설정한다.
//		- 
// Arguments:
//	- wIP	: ip address
//	- sPort	: port number
//
void CUdpBuffer::SetAddress( UINT32 ui32IP, UINT16 ui16Port )
{
	m_socketAddress.sin_family		= AF_INET;
	m_socketAddress.sin_addr.s_addr = ui32IP;
	m_socketAddress.sin_port		= ui16Port;
}

void CUdpBuffer::SetAddress(sockaddr_in *pAddress, INT32 i32AddressSize)
{
	if( i32AddressSize == sizeof(sockaddr_in) )
	{
		memcpy_s( &m_socketAddress, sizeof(m_socketAddress), pAddress, i32AddressSize);
	}
	else
	{
		DSERROR("SetAddress\n");
	}
}