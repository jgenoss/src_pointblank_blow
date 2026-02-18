#include "pch.h"
#include "ModuleNet.h"

#define NETWORK_BUFFER_COUNT		100

CModuleNet * g_pModuleNet = NULL; 

static UINT32 _DefThreadProc( void * pPointer )
{
	pPointer;
	g_pModuleNet->OnWorking();
	return 0;
}

CModuleNet::CModuleNet()
{
	m_ui32ServerCount	= 0;
	m_hThread			= NULL;
	m_bIsRunning		= FALSE;
	m_ppNetServer		= NULL;
	m_ppRecvBuffer		= NULL;
	m_ppSendBuffer		= NULL;
}

CModuleNet::~CModuleNet()
{
	OnDestroy();
}

BOOL CModuleNet::OnCreate(UINT32 * pServerList, INT32 iServerCount)
{	
	m_ui32ConnectCount	= 0;
	m_i32RecvServerIdx	= 0; 
	m_ui32ServerCount	= iServerCount;
	
	//Create Buffer 
	m_ppRecvBuffer	= new i3RingBuffer*[ m_ui32ServerCount ];
	m_ppSendBuffer	= new i3RingBuffer*[ m_ui32ServerCount ];
	m_ppNetServer	= new CModuleNet_Server*[ m_ui32ServerCount ];

	for( INT32 i = 0 ; i < m_ui32ServerCount ; i++ )
	{
		m_ppSendBuffer[i]	= i3RingBuffer::NewObject();
		m_ppSendBuffer[i]->Create( sizeof(NET_PACKET_BUFFER), NETWORK_BUFFER_COUNT );

		m_ppRecvBuffer[i]	= i3RingBuffer::NewObject();
		m_ppRecvBuffer[i]->Create( sizeof(NET_PACKET_BUFFER), NETWORK_BUFFER_COUNT );
	
		m_ppNetServer[i]	= CModuleNet_Server::NewObject();
		m_ppNetServer[i]->Create( pServerList[i], m_ppRecvBuffer[i], m_ppSendBuffer[i] ); 
	}

	//1. Create Listen Socket 
	if( FALSE == m_sServerSocket.OnCreate( g_pConfig->m_ui32Ip, g_pConfig->m_ui16Port ) )	return FALSE;

	//2. Make Thread 
	m_hThread  = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD *)&m_ui32ThreadID);
	if( NULL == m_hThread )	return FALSE; 	
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NETWORK) )return FALSE; 

	return TRUE; 
}

BOOL CModuleNet::OnDestroy(void)
{
	m_bIsRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	for(INT32 i = 0; i < m_ui32ServerCount; i++)
	{
		I3_SAFE_RELEASE( m_ppNetServer[i] );
		I3_SAFE_RELEASE( m_ppRecvBuffer[i] ); 
		I3_SAFE_RELEASE( m_ppSendBuffer[i] ); 
	}

	I3_SAFE_DELETE_ARRAY( m_ppNetServer );
	I3_SAFE_DELETE_ARRAY( m_ppRecvBuffer );
	I3_SAFE_DELETE_ARRAY( m_ppSendBuffer );

	m_sServerSocket.OnDestroy();

	return TRUE; 
}

void CModuleNet::OnWorking()
{
	g_pLog->AddLogThread( L"NETW", 0, m_ui32ThreadID );

	SOCKET hAcceptSocket; 
	struct sockaddr_in FromAddr;
	struct timeval timeout;
	INT32 i32AddrSize;
	INT32 i;

	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		//////////////////////////////////////////////////////////////////
		//1. Accept Socket 
		timeout.tv_sec  = 0;
		timeout.tv_usec = 1;
		i32AddrSize = sizeof(sockaddr_in);
		hAcceptSocket = m_sServerSocket.AcceptConnection( &timeout, &FromAddr, &i32AddrSize ); 
		if( hAcceptSocket != INVALID_SOCKET )
		{
			UINT32 ui32ConnectIp = FromAddr.sin_addr.s_addr; 
			for(i = 0; i < m_ui32ServerCount; i++)
			{
				if( m_ppNetServer[i]->GetIP() == ui32ConnectIp )break; 
			}

			if(i < m_ui32ServerCount)
			{
				m_ui32ConnectCount++; 
				if( m_ppNetServer[i]->IsConnected() )m_ppNetServer[i]->OnDestroy(); 
				m_ppNetServer[i]->SetServer( hAcceptSocket, m_ui32ConnectCount);
				INT32 i32optVal = 1024*1024*2; //2M
				INT32 i32optLen = sizeof(INT32);
				setsockopt(hAcceptSocket, SOL_SOCKET, SO_SNDBUF, (char*)&i32optVal, i32optLen );
				{
					// 처음 접속기념 패킷을 보낸다.
					_SendConnect( i );
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Server Connect Id = %d Working Idx = %d", i, m_ppNetServer[i]->GetWorkingIdx());
				}
			}
			else
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"[Error]UnKnown Server Connect Request"); 
			}
		}

		//////////////////////////////////////////////////////////////////
		//2. Read Write 
		for( i = 0 ; i < m_ui32ServerCount ; i++ )
		{
			//Idx 가 0 이면 연결이 되지 않은 사람입니다. 
			if( m_ppNetServer[i]->GetWorkingIdx() == 0 )continue;

			//Recive  
			m_ppNetServer[i]->SelectEvent();

			//Send 
			if( !m_ppNetServer[i]->Send() )
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Server Send Error Id = %d Working Idx = %d", i, m_ppNetServer[i]->GetWorkingIdx());
			}

			//Check Disconnect 
			if( !m_ppNetServer[i]->IsConnected() )
			{			
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Server Disconnect Id = %d Working Idx = %d", i, m_ppNetServer[i]->GetWorkingIdx());
				m_ppNetServer[i]->SetServer(INVALID_SOCKET, 0);
			}

			//
			//Calculation Recv Send Count For Log 
			m_ppNetServer[i]->Dump(); 
		}

	}

	return; 
}

INT32 CModuleNet::GetRecvCount(void)
{
	INT32 i32count = 0; 

	for( INT32 i = 0 ; i < m_ui32ServerCount ; i++ )
	{
		i32count = i32count + m_ppRecvBuffer[i]->GetBufferCount();
	}

	return i32count;
}

NET_PACKET_BUFFER * CModuleNet::RecvPop(UINT32 * pui32Idx)
{
	void * pBuffer; 
	NET_PACKET_BUFFER * pRv = NULL; 
	for( INT32 i = 0 ; i < m_ui32ServerCount ; i++ )
	{
		m_i32RecvServerIdx++; 
		if( m_i32RecvServerIdx >= m_ui32ServerCount )m_i32RecvServerIdx = 0; 
		pBuffer = m_ppRecvBuffer[m_i32RecvServerIdx]->Pop();
		if(pBuffer != NULL) 
		{
			*pui32Idx = m_ppNetServer[m_i32RecvServerIdx]->GetWorkingIdx(); 
			pRv = (NET_PACKET_BUFFER *)pBuffer; 
			break; 
		}
	}return pRv;
}

void CModuleNet::RecvPopIdx(void)
{
	m_ppRecvBuffer[m_i32RecvServerIdx]->PopIdx(); 
}

INT32 CModuleNet::SendPush( UINT32 ui32Idx, void* pBuffer )
{
	for( INT32 i = 0 ; i < m_ui32ServerCount ; i++ )
	{
		if( m_ppNetServer[i]->GetWorkingIdx() == ui32Idx )
		{
			return m_ppSendBuffer[ i ]->Push( pBuffer );
		}
	}
	return RING_BUFFER_ERROR_BUFFER_FULL; 
}

void CModuleNet::_SendConnect( UINT32 ui32Idx )
{
	NET_PACKET_BUFFER pBuffer( PROTOCOL_MASK_CONNECT );
	m_ppNetServer[ui32Idx]->SendPacketMessage((char*)&pBuffer, pBuffer._ui16TotalSize);
}

void CModuleNet::SendPush( void* pBuffer )
{
	for( INT32 i = 0 ; i < m_ui32ServerCount ; i++ )
	{
		if( m_ppNetServer[i]->IsConnected() )
		{
			m_ppSendBuffer[ i ]->Push( pBuffer );
		}
	}
}