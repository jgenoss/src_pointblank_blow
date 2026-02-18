#include "pch.h"
#include "ModuleGateway.h"
#include "ModuleReceiver.h"

I3_CLASS_INSTANCE( CModuleGateway, i3Thread );

CModuleGateway::CModuleGateway()
{
	m_bRunning				= FALSE;
	m_pServerSocket			= NULL;
	
	m_i32ReceiverIPCount	= 0;
	m_pui32ReciveIP			= NULL;

	m_i32ReceiverCount		= 0;
	m_ppReceiver			= NULL;

	m_ppUseRingBuffer		= NULL;
	m_ppRingReceiver		= NULL;

	m_i32LogThreadIdx	= LOG_THREAD_INVALID;
}

CModuleGateway::~CModuleGateway()
{
	OnDestroy();
}

BOOL CModuleGateway::OnCreate( UINT32 ui32IP, UINT16 ui16Port, INT32 i32ServerCount, UINT32* pui32ServerIP, CRingBuffer** ppRingReceiver )
{
	// 서버 소켓 생성
	m_pServerSocket = i3NetworkServerSocket::NewObject();
	if( NULL == m_pServerSocket )													return FALSE;
	if( FALSE == m_pServerSocket->OnCreate( ui32IP, ui16Port ) )					return FALSE;

	// 리시버 생성
	m_i32ReceiverIPCount	= i32ServerCount;
	m_pui32ReciveIP			= (UINT32*)i3MemAlloc( sizeof(UINT32)*m_i32ReceiverIPCount );
	i3mem::Copy( m_pui32ReciveIP, pui32ServerIP, sizeof(UINT32)*m_i32ReceiverIPCount );

	// Ring Buffer 생성
	m_ppRingReceiver		= ppRingReceiver;
	// UseRingBuffer
	m_ppUseRingBuffer		= (CModuleReceiver**)i3MemAlloc(sizeof(CModuleReceiver*)*m_i32ReceiverIPCount);
	i3mem::FillZero( m_ppUseRingBuffer, sizeof(CModuleReceiver*)*m_i32ReceiverIPCount );

	// 재접속 용 Receviver를 하나 더 생성합니다.
	m_i32ReceiverCount		= m_i32ReceiverIPCount+1;
	m_ppReceiver = (CModuleReceiver**)i3MemAlloc( sizeof(CModuleReceiver*) * m_i32ReceiverCount );
	for( INT32 i = 0 ; i < m_i32ReceiverCount ; i++ )
	{
		m_ppReceiver[i] = CModuleReceiver::NewObject();
		if( NULL == m_ppReceiver[i] )												return FALSE;
		if( FALSE == m_ppReceiver[i]->Create( i ) )									return FALSE;
	}

	// Log 생성
	m_i32LogThreadIdx	= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )									return FALSE;

	// Thread 생성
	if (FALSE == i3Thread::Create( "Gateway" ))										return FALSE;

	return TRUE;
}

void CModuleGateway::OnDestroy()
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pServerSocket );

	if( m_ppReceiver )
	{
		for( INT32 i = 0 ; i < m_i32ReceiverCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppReceiver[i] );
		}
		I3MEM_SAFE_FREE( m_ppReceiver );
	}
	I3MEM_SAFE_FREE( m_pui32ReciveIP );
	I3MEM_SAFE_FREE( m_ppUseRingBuffer );
}

UINT32 CModuleGateway::OnRunning( void* pUserData )
{
	m_bRunning = TRUE;
	
	while( m_bRunning )
	{
		for( INT32 i = 0 ; i < m_i32ReceiverIPCount ; i++ )
		{
			if( NULL == m_ppUseRingBuffer[i] )		continue;
			if( m_ppUseRingBuffer[i]->IsWork() )	continue;

			m_ppUseRingBuffer[i] = NULL;
		} 

		_AcceptSocket();
		::WaitForSingleObject( m_Handle, 1 );
	}
	return 0;
}

INT32 CModuleGateway::_GetAcceptIP( UINT32 ui32IP )
{
	for( INT32 i = 0; i < m_i32ReceiverIPCount ; i++ )
	{
		if( ui32IP != m_pui32ReciveIP[i] ) continue;
		
		return i;
	}
	return -1;
}

INT32 CModuleGateway::_GetWaitSocket()
{
	for( INT32 i = 0 ; i < m_i32ReceiverCount ; i++ )
	{
		if( m_ppReceiver[i]->IsWork() ) continue;
		return i;

	}
	return -1;
}

INT32 CModuleGateway::_GetWaitRingBuffer()
{
	for( INT32 i = 0 ; i < m_i32ReceiverIPCount ; i++ )
	{
		if( m_ppUseRingBuffer[i] ) continue;

		return i;
	}
	return -1;
}

void CModuleGateway::_AcceptSocket()
{
	struct timeval timeout = { 0, ACCEPTOR_RECV_TIME};
	struct sockaddr_in FromAddr; 
	INT32  AddrSize = sizeof(sockaddr_in);

	// 게임서버 연결 확인
	SOCKET hSocketAccept = m_pServerSocket->AcceptConnection( &timeout, &FromAddr, &AddrSize);
	if( INVALID_SOCKET == hSocketAccept )		return; 

	// 허용된 IP인지 체크
	if( -1 == _GetAcceptIP( FromAddr.sin_addr.s_addr ) ) 
	{
		WriteLog( m_i32LogThreadIdx, "[Acce] Unknow Server (IP:%d)", FromAddr.sin_addr.s_addr );
		::shutdown( hSocketAccept, SD_BOTH );
		::closesocket( hSocketAccept );
		return;
	}
	// 쉬고 있는 Receiver 를 찾는다.
	INT32 i32ReceiverIdx = _GetWaitSocket();
	if( -1 == i32ReceiverIdx )
	{
		WriteLog( m_i32LogThreadIdx, "[Acce] Not Found Waiting Socket (IP:%d)", FromAddr.sin_addr.s_addr );
		::shutdown( hSocketAccept, SD_BOTH );
		::closesocket( hSocketAccept );
		return;
	}
	INT32 i32RingIdx = _GetWaitRingBuffer();
	// 쉬고 있는 RingBuffer를 찾는다.
	if( -1 == i32RingIdx )
	{
		WriteLog( m_i32LogThreadIdx, "[Acce] Not Found Waiting RingBuffer (IP:%d)", FromAddr.sin_addr.s_addr );
		::shutdown( hSocketAccept, SD_BOTH );
		::closesocket( hSocketAccept );
		return;
	}

	CModuleReceiver* pReceiver = m_ppReceiver[i32ReceiverIdx];

	if( FALSE == pReceiver->SetSocket( hSocketAccept ) )
	{
		WriteLog( m_i32LogThreadIdx, "[Acce] Use Socket (IP:%d)", FromAddr.sin_addr.s_addr );
		::shutdown( hSocketAccept, SD_BOTH );
		::closesocket( hSocketAccept );
		return;
	}

	m_ppUseRingBuffer[i32RingIdx] = pReceiver;
	pReceiver->SetSocket( hSocketAccept );
	pReceiver->SetRingBuffer( m_ppRingReceiver[i32RingIdx] );

	// 연결 통보
	i3NetworkPacket	SendPacket(PROTOCOL_BILLING_CONNECT_REQ);
	pReceiver->SendPacket( &SendPacket );

	WriteLog( m_i32LogThreadIdx, "[Acce] Server Connect Receiver:%d,RingIdx:%d", i32ReceiverIdx, i32RingIdx );
}

BOOL CModuleGateway::SendPacket( INT32 i32Idx, i3NetworkPacket* pPacket )
{
	if( i32Idx > m_i32ReceiverCount ) return FALSE;

	return m_ppUseRingBuffer[i32Idx]->SendPacket( pPacket );
}

/*
// 게임서버가 연결 중인지 확인한다
// param idx	게임서버 배열 idx
// return		연결여부
BOOL CModuleGateway::IsConnectedGameServer( UINT32 idx )
{
	if (NULL == m_ppReceiver)	return FALSE;
	return m_ppReceiver[idx]->IsWork();
}
*/