// UdpBuffer.h
//
// YouJong Ha
//	Last update : 2012-02-24 (yyyy:mm:dd)
//	
// Description:
//	CUdpBuffer class 선언
//	IOCP 통신에 필요한 OVERLAPPED 구조체를 포함한다.
//
#ifndef _UdpBuffer_h
#define _UdpBuffer_h

#define OP_ACCEPT       0                   // AcceptEx
#define OP_READ         1                   // WSARecv/WSARecvFrom
#define OP_WRITE        2                   // WSASend/WSASendTo

#define MEMORY_MARKER		0xbadbad00UL

class CUdpBuffer: public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpBuffer);

public:
	WSAOVERLAPPED	ol;	// IO 작업 완료후 this 포인터를 계산하기 위해 public으로 선언

	// initialization
	void			Init(void);
	
	// protocol
	void			CopyBufferFrom( CUdpBuffer* pSource, INT32 i32Size );		
	UINT8			GetUdpProtocolId(void) const			{	return m_pBuffer[0];				} 
	
	// IO state
	LPWSAOVERLAPPED	GetOverlapped()							{	return &ol;							}				
	void			ResetOverlapped()						{	memset(&ol, 0, sizeof(ol));			}
	void			SetOperation(UINT32 ui32Operation )		{	m_ui32Operation = ui32Operation;	}
	UINT32			GetOperation() const					{	return m_ui32Operation;				}
	
	// buffer
	UINT32			GetBufferSize(void) const				{	return m_ui32BufLen;				} 
	void			SetBufferSize(UINT32 ui32Size)			{	
																I3ASSERT( ui32Size <= UDP_SEND_PACKET_SIZE );
																if( ui32Size > UDP_SEND_PACKET_SIZE ) m_ui32BufLen = UDP_SEND_PACKET_SIZE;
																else m_ui32BufLen = ui32Size;
															}
	char*			GetBuffer()								{	return m_pBuffer;						}
	BOOL			CheckMarker() const						{	return (m_ui32Marker == MEMORY_MARKER);	}
	
	// server socket 
	BOOL			SetSocketIdx( UINT32 ui32Idx )			{	if( ui32Idx < MAX_UDP_SOCKET_COUNT ) { 
																		m_ui32SocketIdx = ui32Idx;
																		return TRUE;
																}
																I3ASSERT(0);
																return FALSE;
															}
	UINT32			GetSocketIdx() const					{	return m_ui32SocketIdx;	}
	
	// client ip/port
	void			SetAddress(UINT32 ui32IP, UINT16 ui16Port);	
	void			SetAddress( sockaddr_in* pAddress,  INT32 i32AddressSize );
	sockaddr_in*	GetAddress()							{	return &m_socketAddress;						}
	INT32*			GetAddressSize()						{	return &m_i32SocketAddressSize;					}
	UINT32			GetIp(void)		const					{	return m_socketAddress.sin_addr.s_addr;			}
	UINT16			GetPort(void)	const					{	return ntohs(m_socketAddress.sin_port);			}
	char*			GetIpString()	const					{ 	return inet_ntoa( m_socketAddress.sin_addr );	}
	
	// group info.
	UINT32			GetGroupIdx()	const					{	return m_relayInfo.wGroupIdx; 				}
	UINT32			GetEncryptKey()	const					{	return m_relayInfo.wEncryptKey;				}
	UINT8			GetTargetIdx()	const					{	return m_relayInfo.cTargetIdx;				}
	void			SetGroupIdx  ( INT32  i32GroupIdx )		{	m_relayInfo.wGroupIdx	= i32GroupIdx;		}
	void			SetEncryptKey( UINT32 ui32Key )			{	m_relayInfo.wEncryptKey = ui32Key;			}
	void			SetTargetIdx ( UINT8  ui8Idx	 )		{	m_relayInfo.cTargetIdx  = ui8Idx;			}
	UINT8			GetSlotIdx()	const					{	return ((N_GAME_HEADER*)m_pBuffer)->_Index; }
	
	// buffer reference counter : debug
	void			InitRef()								{	m_cRef = 0;	m_ui32Marker = MEMORY_MARKER;	}
	UINT8			AddRef()								{	return ++m_cRef;		}
	UINT8			Release()								{	return --m_cRef;		}
	
	// constructor / destructor
	CUdpBuffer();
	~CUdpBuffer();

protected:
	UINT32			m_ui32Operation;					// operation code (OP_ACCEPT|OP_READ|OP_WRITE)
	
	char			m_pBuffer[ UDP_SEND_PACKET_SIZE ];	// 내부 버퍼
	UINT32			m_ui32Marker;						// memory marker
	N_GAME_RELAY	m_relayInfo;						// 릴레이 정보 (패킷의 마지막에 포함되어 있다)
	UINT32			m_ui32BufLen;						// 버퍼 크기
														//	- read = UDP_SEND_PACKET_SIZE
														//	- send = bytes to send
	// client address
	sockaddr_in		m_socketAddress;					// ip/port
	INT32			m_i32SocketAddressSize;				// sizeof(sockaddr_in)
	// 서버 소켓
	// - iocpServer에 static 배열로 정의되어 있으며 udp buffer에서는 배열 인덱스를 유지한다.
	UINT32			m_ui32SocketIdx;					// local port에 binding 된 소켓
	UINT8			m_cRef;								// get buffer / release buffer 확인용
};

#endif