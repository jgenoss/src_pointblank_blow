// UdpBuffer.h
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	CUdpBuffer class 선언
//	IOCP 통신에 필요한 OVERLAPPED 구조체를 포함한다.
//
// Dependency: 
//		
//	

#ifndef _UdpBuffer_h
#define _UdpBuffer_h

#define OP_ACCEPT       0                   // AcceptEx
#define OP_READ         1                   // WSARecv/WSARecvFrom
#define OP_WRITE        2                   // WSASend/WSASendTo

// I/O 버퍼 클래스. 소켓 I/O를 처리하기 위한 정보 뿐만아니라 WSAOVERLAPPED 구조체를 포함한다.
class CUdpBuffer: public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpBuffer);

public:
	// iocp 관련 변수 선언
	WSAOVERLAPPED		ol;
	UINT32				m_ui32Operation;		// operation code (OP_ACCEPT|OP_READ|OP_WRITE)
	UINT32              m_ui32IoOrder;			// Order in which this I/O was posted
	
	operator OVERLAPPED*() { return &ol; }		// OVERLAPPED cast 연산자
	
	
protected:
	// 내부 버퍼 선언
	char			m_pBuffer[ UDP_SEND_PACKET_SIZE ];	// 내부 버퍼
	UINT32			m_ui32BufLen;						// 버퍼 크기
														//	- read = UDP_SEND_PACKET_SIZE
														//	- send = bytes to send
	// client address
	sockaddr_in		m_socketAddress;					// ip/port
	INT32			m_ui32SocketAddressSize;			// sizeof(sockaddr_in)

	// recv time (for cheating check)
	UINT32			m_ui32RecvTime;

	// 패킷 파서를 위한 상태 변수들
	UINT16			m_ui16ReadPosition;					// 데이터 읽기 포지션 (초기 값은 헤더를 제외한 패킷의 시작 위치)
	UINT16			m_ui16WritePosition;				// 데이터 쓰기 포지션 (초기 값은 헤더를 제외한 패킷의 시작 위치)
public:
	N_GAME_HEADER*	m_pHead;							// 수신된 udp data 헤더
	N_GAME_RELAY*	m_pRelay;							// 릴레이 정보 (패킷의 마지막에 포함되어 있다)

	// 메모리 풀 관련 변수
	INT32			m_i32PoolIndex;						// udp buffer pool에서 사용하는 인덱스
	UINT8			m_cRef;								// release buffer 확인하기 위함
public:
	// 서버 소켓 
	// - iocpServer에 static 배열로 정의되어 있으며 udp buffer에서는 배열 인덱스를 유지한다.
	INT32			m_i32SocketIdx;						// local port에 binding 된 소켓

	// constructor/ destructor
	CUdpBuffer();
	~CUdpBuffer();				

	// initialization functions
	void			Init(void);							// 내부 버퍼 변수들 초기화
	void 			InitReceive(void);					
	void 			InitSend(void);						

	// protocol 
	void			ReadData ( void* pBuffer, UINT16 ui16Size );	
	void			WriteData( void* pBuffer, UINT16 ui16Size );	

	void			ReadAppendData();	
	void			CopyBufferFrom( CUdpBuffer* pSource );		// not used

	void			SetUdpProtocolId(UINT8 cUdpProtocolId )	{	m_pBuffer[0] = cUdpProtocolId;			} // not used
	UINT8			GetUdpProtocolId(void)					{	return m_pBuffer[0];					} 

	// buffer accessor
	UINT16			GetBufferSize(void)						{	return ((UINT16)m_ui32BufLen);			} 
	void			SetBufferSize(UINT32 uiSize)			{	m_ui32BufLen = uiSize;					}
	char*			GetBuffer()								{	return m_pBuffer;						}
	
	// client ip/port
	void			SetAddress(UINT32 ui32IP, UINT16 ui16Port);	
	void			SetAddress( sockaddr_in* pAddress,  INT32 i32AddressSize );
	sockaddr_in*	GetAddress()							{	return &m_socketAddress;						}
	INT32*			GetAddressSize()						{	return &m_ui32SocketAddressSize;				}
	UINT32			GetIp(void)								{	return m_socketAddress.sin_addr.s_addr;			}
	UINT16			GetPort(void)							{	return m_socketAddress.sin_port;				}
	char*			GetIpString()							{ 	return inet_ntoa( m_socketAddress.sin_addr );	}

	// packet
	UINT16			GetReadPosition()						{	return m_ui16ReadPosition;				}
	UINT16			GetWritePosition()						{	return m_ui16WritePosition;				}

	// 메모리풀 인덱스
	void			SetIdx(INT32 i32Idx)					{	m_i32PoolIndex = i32Idx;  				}
	INT32			GetIdx()								{	return m_i32PoolIndex; 					}
	
	// 그룹 관련 변수 접근
	UINT32			GetGroupIdx()							{	return m_pRelay->wGroupIdx;				}
	UINT32			GetEncryptKey()							{	return m_pRelay->wEncryptKey;			}
	UINT8			GetTargetIdx()							{	return m_pRelay->cTargetIdx;			}
	UINT8			GetSlotIdx()							{	return m_pHead->_Index;					}

	// cheating check
	UINT32			GetRecvTime()							{	return m_ui32RecvTime;					}
	void			SetRecvTime(UINT32 ui32RecvTime)		{	m_ui32RecvTime = ui32RecvTime;			}
};

#endif