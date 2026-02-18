// iocpServer.h
//
// YouJong Ha
//	Last update : 2010-10-06 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//
#ifndef _iocpServer_H
#define _iocpServer_H


#include "S2CriticalSection.h"

#define DEFAULT_BUFFER_SIZE         4096	// default buffer size
#define DEFAULT_OVERLAPPED_COUNT    5		// Number of overlapped recv per socket
#define MAX_IO_THREAD_COUNT			32		// Maximum number of completion threads allowed

#define PACKETCOUNT_IDX			2
#define PACKETCOUNT_TIME		1

class CUdpBuffer;				// unit memory
class CUdpBufferPool;			// memory allocator

// -------------------------------------------------------------------------------------
// iocp worker parameter
// -------------------------------------------------------------------------------------
struct IOCP_WORKER_PARAM
{
	HANDLE		m_hevtQuit;								// 종료 시그널
	HANDLE		m_hCompletionPort;						// IOCP 핸들
	INT32		m_i32ThreadIndex;						// I/O thread index = send buffer index (0..n1)
														// <> Task processor index (0..n2)
														// 자신에게 할당받은 send buffer를 사용하기 위함
	IOCP_WORKER_PARAM()
	{
		m_hevtQuit			= NULL;
		m_hCompletionPort	= NULL;
		m_i32ThreadIndex	= -1;
	}
};

struct PACKET_PROFILE
{
	INT32					m_i32RecvPacketCount;					// 초당 패킷수 Recv
	INT32					m_i32RecvPacketSize;					// 초당 패킷크기 Recv
	INT32					m_i32SendPacketCount;					// 초당 패킷수 Send
	INT32					m_i32SendPacketSize;					// 초당 패킷크기 Send
};

// -------------------------------------------------------------------------------------
// socket object
// : GetQueuedCompletionStatus이 리턴할 때마다 소켓 관련 정보들에 접근을 용이하게 한다.
// -------------------------------------------------------------------------------------

struct SOCKET_OBJ
{
    SOCKET					m_socket;									// Socket handle
	UINT32					m_ui32Index;								// m_pSocketObj[]의 index(local binding socket)
	UINT16					m_ui16Port;									// Socket Port
    
	PACKET_PROFILE			m_PacketProfile[ PACKETCOUNT_IDX ];
	PACKET_PROFILE			m_PacketProfileTotal;

	S2CriticalSection   m_cs;									
	
	// ---------------------------------------------------------------------
	//	inline utility functions
	// ---------------------------------------------------------------------

	void					SetSocketIdx( UINT32 ui32Idx ) 	{
								I3ASSERT( ui32Idx < MAX_UDP_SOCKET_COUNT );
								if( ui32Idx < MAX_UDP_SOCKET_COUNT ) m_ui32Index = ui32Idx;
								else ::DSERROR("socket index error\n");
							}
	
	UINT32					GetSocketIdx() const {	return m_ui32Index;		}

	void					ResetPacketProfile( UINT32 ui32Idx )				{	i3mem::FillZero( &m_PacketProfile[ ui32Idx ], sizeof(PACKET_PROFILE) );	}
	PACKET_PROFILE*			GetPacketProfile( UINT32 ui32Idx ) 					{	return &m_PacketProfile[ ui32Idx ];										}
	void					AddRecvPacket( INT32 i32Idx, UINT32 ui32Size )		
	{	
		m_PacketProfile[ i32Idx ].m_i32RecvPacketCount++;
		m_PacketProfile[ i32Idx ].m_i32RecvPacketSize	+= ui32Size;

		m_PacketProfileTotal.m_i32RecvPacketCount++;
		m_PacketProfileTotal.m_i32RecvPacketSize		+= ui32Size;
		//I3TRACE( "Recv Pack Size: %d\n", m_PacketProfileTotal.m_i32RecvPacketSize );
	}
	void					AddSendPacket( INT32 i32Idx, UINT32 ui32Size )		
	{	
		m_PacketProfile[ i32Idx ].m_i32SendPacketCount++;
		m_PacketProfile[ i32Idx ].m_i32SendPacketSize	+= ui32Size;

		m_PacketProfileTotal.m_i32SendPacketCount++;
		m_PacketProfileTotal.m_i32SendPacketSize		+= ui32Size;
		//I3TRACE( "Send Pack Size: %d\n", m_PacketProfileTotal.m_i32SendPacketSize );
	}
	
	void					SetSocket( SOCKET s )
	{
					struct	sockaddr_storage Addr;
					INT32	i32AddrLen = sizeof (Addr);

					m_cs.Lock(); 
						m_socket = s;
						if( getsockname( m_socket, (SOCKADDR*)&Addr, & i32AddrLen) == SOCKET_ERROR ) 
						{
							DSERROR( "[SOCKET_OBJ] getsockname error:%d", WSAGetLastError() ); 
							m_cs.Unlock(); 
							return;  
						} 
						m_ui16Port = ntohs( SS_PORT(&Addr));
					m_cs.Unlock(); 
				}

	UINT16		GetPort() const													{  return m_ui16Port; }

	// ---------------------------------------------------------------------
	//	constructor
	// ---------------------------------------------------------------------
	SOCKET_OBJ()
	{
		m_socket	= NULL; 
		m_ui32Index	= (UINT32)-1;	
		m_ui16Port	= 0;
				
		i3mem::FillZero( &m_PacketProfile,		sizeof(PACKET_PROFILE)*PACKETCOUNT_IDX );
		i3mem::FillZero( &m_PacketProfileTotal, sizeof(PACKET_PROFILE) );
	}
};


///////////////////////////////////////////////////////////////////////////////
//
// Class: CIocpServer
//
// Description:
//	- server class
class CIocpServer 
{
public:
		// ---------------------------------------------------------------------
		//	creation 함수 (소켓, IOCP, 쓰레드 관련 작업을 실행한다)
		// ---------------------------------------------------------------------
		BOOL 		Create			( UINT32 ui32SocketCount, UINT32 ui32IP, UINT16* pui16Port, UINT32 ui32IoThreadCount );

		
		UINT32		GetIoThreadCount()	const		{	return m_ui32IoThreadCount;	};				
		HANDLE		GetIoThread	( INT32 i32Idx )	{	
															I3ASSERT( i32Idx >= 0 && i32Idx < DS_MAX_IOCP_THREAD_COUNT);
															return m_phIoThreads[ i32Idx ];	
													};			
		INT32		GetCounterIdx()	const			{	return m_i32NewCountIdx;	};			// 패킷 카운트 저장 버퍼 인덱스

		// ---------------------------------------------------------------------
		//	call back : control module 
		// ---------------------------------------------------------------------
		void		SetPacketCount_C();															// 시간당 패킷 카운트 리셋
		void		GetPacketProfile_C( INT32& i32RecvPacketSize, INT32& i32SendPacketSize );	// 시간당 총 패킷 얻기
//		UINT16		GetRestPort_C	( INT32* pi32SocketIdx );									// 가장 트래픽이 적은 포트를 얻는다

		// ---------------------------------------------------------------------
		//	IO : 리턴 값 : SOCKET_ERROR / NO_ERROR 
		// ---------------------------------------------------------------------
		INT32		PostSend( SOCKET_OBJ* pSock, CUdpBuffer* pBuf );					// post send i/o
		INT32		PostRecv( SOCKET_OBJ* pSock, CUdpBuffer* pBuf );					// post read i/o
		INT32		CancelAndPostRecv ( SOCKET_OBJ* pSock );
	
		// ---------------------------------------------------------------------
		//	socket object
		// ---------------------------------------------------------------------
		SOCKET_OBJ* GetSocketObj ( INT32 i32Idx )	{ 
														if ( i32Idx >= 0 && i32Idx < DS_DEFAULT_PORT_COUNT )
															return m_ppSocketObjs[ i32Idx ]; 
														return NULL;
													}										// get socket object
		void		FreeSocketObj( SOCKET_OBJ*	pSock	);									// free socket object
	
		// ---------------------------------------------------------------------
		//	buffer object
		// ---------------------------------------------------------------------
		CUdpBuffer* GetBufferObj1 ( SOCKET_OBJ*	pSock	);									// get buffer obj
		void		FreeBufferObj( CUdpBuffer*	pBuf	);									// free buffer obj
		
		// ---------------------------------------------------------------------
		//	constructor / destructor
		// ---------------------------------------------------------------------
		CIocpServer( CUdpBufferPool* pMemoryAllocator);
		~CIocpServer();

private:
		// ---------------------------------------------------------------------
		//	memory allocator
		// ---------------------------------------------------------------------
		static CUdpBufferPool*	m_pMemoryAllocator;									// memory allocator

		// ---------------------------------------------------------------------
		//	thread shared variables
		// ---------------------------------------------------------------------
		static HANDLE			m_hCompletionPort;									// io completion port
		
		static HANDLE			m_phIoThreads[ DS_MAX_IOCP_THREAD_COUNT ];			// worker threads
		static UINT32			m_ui32IoThreadCount;								// # of thread objects
		
		static SOCKET_OBJ*		m_ppSocketObjs[ DS_DEFAULT_PORT_COUNT ];			// socket objects
		static UINT32			m_ui32SocketCount;									// # of socket objects
		
		// ---------------------------------------------------------------------
		//	statistics
		// ---------------------------------------------------------------------
		INT32					m_i32OldCountIdx;									// 패킷 수를 구하기 위한 인덱스(0/1)
		INT32					m_i32NewCountIdx;									// ...
		UINT32					m_ui32Time;											// server time

		S2CriticalSection		m_cs;
};

extern CIocpServer* g_pIocpServer;

#endif // _iocpServer_H