// IocpWorker.cpp
//
// YouJong Ha
//	Last update : 2010-10-13 (yyyy-mm-dd)
//	
// Description:
//	- recv 완료후 send queue check 추가 (2010-10-13)
//	- worker thread의 상위 레벨 (세부 구현은 iocpWorkerImpl.cpp에 구현할 것)
//
//
// Dependency: 
//		
//

#include "pch.h"
#include "ServerStatistics.h"
#include "UdpBuffer.h"
#include "PacketLocker.h" // send/recv queue
#include "iocpServer.h"
#include "iocpWorkerImpl.h"
#include "iocpWorker.h"

I3_CLASS_INSTANCE(CIocpWorker, i3ElementBase);

CIocpWorker::CIocpWorker()
{
	//m_nThreadIdx	= 0;
	
	//i3mem::FillZero( m_pui32PacketCount, sizeof( UINT32 )* PACKETCOUNT_IDX );
	//m_i32ErrorCount	= 0;	
	//m_ui32ErrorTime = 0;	
	//m_dwLastTick	= 0;	
}

//
CIocpWorker::~CIocpWorker()
{
}
//


///////////////////////////////////////////////////////////////////////////////
//
// Function: CompletionThread
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
DWORD WINAPI CIocpWorker::CompletionThread( LPVOID lpParam)
{
    I3ASSERT( g_hevtQuit );
	
	SOCKET_OBJ*	pSock		=	NULL;			// Per socket object for completed I/O
    CUdpBuffer*	pBuf		=	NULL;           // Per I/O object for completed I/O
    OVERLAPPED*	pOverlapped	=	NULL;			// Pointer to overlapped structure for completed I/O
    HANDLE      hCompletionPort;				// Completion port handle
    DWORD       dwBytesTransfered,				// Number of bytes transfered
                dwFlags;						// Flags for completed I/O
    INT32       i32ReturnCode, 
				i32Error;
	INT32		i32ThreadIdx;					// Thread Index(ID)

    
	InitIocpWorker( lpParam );					// one time initialization (현재는 하는 일 없음)
	
	IOCP_WORKER_PARAM* pParam = (IOCP_WORKER_PARAM*)lpParam;
	hCompletionPort	= pParam->m_hCompletionPort;	// thread
	i32ThreadIdx	= pParam->m_i32ThreadIndex;
	delete pParam;

    //alloc private log buffer
#ifdef NEW_LOG
	I3TRACE("CompletionThread=%d\n", GetCurrentThreadId() );
	if( NULL == g_pLogFile->AllocBuffer( GetCurrentThreadId() ) )
	{
		goto exit_loop;
	}
#endif 
	
	while ( 1 )
    {
		i32Error = NO_ERROR;
        i32ReturnCode = GetQueuedCompletionStatus( hCompletionPort, &dwBytesTransfered, (PULONG_PTR)&pSock, &pOverlapped, INFINITE );

        pBuf = CONTAINING_RECORD( pOverlapped, CUdpBuffer, ol);

        if ( i32ReturnCode == FALSE )
        {
            // check quit signal
			if ( WAIT_OBJECT_0 == WaitForSingleObject( g_hevtQuit, 0 ) )
			{
				I3TRACE("Iocp Worker Thread %d (0x%0x) Exit...\n", i32ThreadIdx, GetCurrentThreadId());
				return 0;
			}
			
			//If the call fails, call WSAGetOverlappedResult to translate the
            //error code into a Winsock error code.
			i32ReturnCode = WSAGetOverlappedResult( pSock->m_socket, &pBuf->ol, &dwBytesTransfered, FALSE, &dwFlags );
            if ( i32ReturnCode == FALSE)
            {
                i32Error = WSAGetLastError();
				// 종료가 아닌 경우에는 메모리를 돌려 주고 작업을 계속 해야 한다.
				// return error;
			}
        }
		
        // Handle the IO operation
        HandleIo( pSock, pBuf, dwBytesTransfered, i32Error);
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// Function: HandleIo
//
// Description:
//    소켓에 대한 IO를 처리한다. 
//		- recv	 완료: 완료된 recv를 다시 post.
//      - accept 완료: 새로운 AcceptEx를 post(삭제).
//		- send   완료: 버퍼 해제.
// Arguments:
//		- nThreadIndex : IO 쓰레드와 SendQueue를 1:1로 만들기 위한 식별자(0..nThread-1)
//		- pSock	: socket object
//		- pBuf	: buffer (CUdpBuffer를 사용)
//		- hCompPort : Accept 완료 후 클라이언트 소켓을 iocp에 등록하기 위한 목적(사용하지 않음)
//		- dwBytesTransfered : 네트워크로 수신/송신된 데이터 바이트
//		- dwError : WSAGetLastError() 리턴 값
//

// nthreadIndex는 삭제 예정.
void  CIocpWorker::HandleIo( SOCKET_OBJ *pSock, CUdpBuffer* pBuf, DWORD dwBytesTransferred, DWORD dwError)
{
	// ---------------
	// 에러 처리
	// ---------------
	switch( dwError )
	{
		case WSA_IO_INCOMPLETE:			// 비교적 자주 발생하고 재시도 하면 해결 되는 경우가 많다. udp에서는 무시하도록 구현.
		{
			dwBytesTransferred = 0;	// 밑에서 메모리 해제하도록 기대.
									// recv라면 반드시 recv를 재 post해야 한다. 그렇지 않으면 아무도 수신을 안함(소켓 잠김).
		}
		break;
	case WSA_OPERATION_ABORTED: // 995 (cancel io에 의해서 발생)
		{
			// read 작업일 경우이라도 새로운 read가 post 되기 때문에 버퍼 release
			if( pBuf != NULL ) g_pIocpServer->FreeBufferObj ( pBuf );
			return;
		}
		break;
	// critical error : 의도적으로 fall down
	case WSAEFAULT:					// 어딘가에서 heap memory가 오버라이트 됨
	case WSANOTINITIALISED:			// 윈속 초기화 안함.
	case WSAENETDOWN:				// 윈도우에서 network disconnection이 감지된 경우(케이블 빠졌다고 메시지 나오는 경우)
	case WSAENOTSOCK:				// 소켓 descriptor (handle) 
	case WSA_INVALID_HANDLE:		// 소켓 closed
	case WSA_INVALID_PARAMETER:		// iocp setup error
	default:
		{
			if( dwError != 0 )
			{
				if( pBuf != NULL )
				{
					DSERROR( "[CIocpWorker::HandleIo] WSAGetLastError() = %d, op=%d\n", dwError, pBuf->GetOperation() );
					if( pBuf->GetOperation() != OP_READ )
					{
						g_pIocpServer->FreeBufferObj ( pBuf );
						return;
					}
					
					dwBytesTransferred = 0; // post recv and release this buffer
				}
				else
				{
					DSERROR( "[CIocpWorker::HandleIo] WSAGetLastError() = %d\n", dwError );
				}
			}
		}
		break;
	}

	pSock->m_cs.Lock();
	
	if( pBuf )
	{
		switch( pBuf->GetOperation() )
		{
		case OP_READ	: 
			{
				//1. 로그용 데이터 처리 
				pSock->AddRecvPacket( g_pIocpServer->GetCounterIdx(), dwBytesTransferred );

				//2. 새로운 READ BUFFER 처리 
				CUdpBuffer * pNewRecvBuffer = g_pIocpServer->GetBufferObj1( pSock );		
				while( pNewRecvBuffer == NULL )
				{
					pNewRecvBuffer = g_pIocpServer->GetBufferObj1( pSock );		
					DSERROR("[CIocpWorker::HandleIo] GetBufferObj ERROR!...\n");
					Sleep(16); 
				}

				while( g_pIocpServer->PostRecv(pSock, pNewRecvBuffer) != NO_ERROR )
				{
					DSERROR("[CIocpWorker::HandleIo] PostRecv ERROR!...\n");
					Sleep(16); 
				}

				//3. 받은 BUFFER 처리 
				if( dwBytesTransferred > 0)
				{
					if( g_pStatistics ) g_pStatistics->IncrementReceivedBytes( dwBytesTransferred );

					IOCP_WORKER_THREAD_CONTEXT context;			// thread local variable
					pBuf->SetSocketIdx( pSock->GetSocketIdx() );
					
					BOOL bStop = FALSE;							// data 저장소에 insert되면 bStop이 true로 설정됨
					if( ReadContext( pBuf, dwBytesTransferred, &context, &bStop) )	// read context
					{	
						if(!bStop) OnProcess(&context, &bStop);

						if(!bStop)
						{	
							// bStop이 false인 경우 처리가 안된 경우이다(오류)
							// 단순히 수신 버퍼를 해제한다.
							g_pIocpServer->FreeBufferObj(pBuf);
						}
					}
					else
					{
						g_pIocpServer->FreeBufferObj(pBuf);
					}
				}
				else 
				{
						g_pIocpServer->FreeBufferObj(pBuf);
				}
			}			
			break; 
		case OP_WRITE	: 
			{
				pSock->AddSendPacket( g_pIocpServer->GetCounterIdx(), dwBytesTransferred );

				if(dwBytesTransferred > 0)
				{
					// Update the counters
					if( g_pStatistics ) g_pStatistics->IncrementSentBytes( dwBytesTransferred );
				}

				if ( pBuf ) 
				{
					g_pIocpServer->FreeBufferObj( pBuf );
				}
			}
			break;
		default			:	
			DSERROR("[CIocpWorker::HandleIo] default!...\n");
			if ( pBuf ) 
			{
				g_pIocpServer->FreeBufferObj( pBuf );
			}
			break; 
		}
	}
	pSock->m_cs.Unlock(); 

    return;
}
