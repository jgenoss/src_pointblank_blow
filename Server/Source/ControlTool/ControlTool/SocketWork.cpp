
#include "pch.h"
#include "SocketWork.h"
#include "ControlToolDoc.h"
#include "ControlToolView.h"

CSocketWork::CSocketWork()
{
	Init();
}


CSocketWork::~CSocketWork()
{
	OnDestroy();
}


void CSocketWork::Init()
{
	m_hThread		= INVALID_HANDLE_VALUE;
	m_ThreadID		= 0;

	m_hSocket		= INVALID_SOCKET;
	m_hRrecvEvent	= INVALID_HANDLE_VALUE;

	m_i32ReceivedPacketSize = 0;
	::ZeroMemory( m_pReceiveBuffer, sizeof(m_pReceiveBuffer) );

	m_bThreadOper	= FALSE;
	m_bSetHeartBit	= FALSE;
}



BOOL CSocketWork::OnCreate(UINT32 ui32IP, UINT16 ui16Port, BOOL UseHeartbit)
{
	if(!CreateSocket())
		return FALSE;

	// 서버 접속
	::ZeroMemory( &m_serverAddr, sizeof(m_serverAddr) );
	m_serverAddr.sin_family		= AF_INET;
	m_serverAddr.sin_addr.s_addr	= ui32IP;
	m_serverAddr.sin_port			= ::htons(ui16Port);

	//연결
	if(!DoConnect())
		return FALSE;

	// 리시브 이벤트 생성
	m_hRrecvEvent = ::WSACreateEvent();

	if(UseHeartbit)
	{
		m_dwSendHeartBitTime = GetTickCount();
		m_bSetHeartBit = TRUE;
	}
	else
	{
		m_dwSendHeartBitTime = 0;
		m_bSetHeartBit = FALSE;
	}
		
	
	//쓰레드 생성
	m_hThread = (HANDLE)CreateThread(0, 0, SocketThread, (LPVOID)this, 0, &m_ThreadID);
	if ( INVALID_HANDLE_VALUE == m_hThread )
	{
		return FALSE;
	}

	// 리시브 관련 멤버 초기화
	m_i32ReceivedPacketSize = 0;

	return TRUE;
}





void	CSocketWork::OnDestroy()
{
	if( INVALID_HANDLE_VALUE != m_hThread )
	{
		m_bThreadOper = FALSE;

		WaitForSingleObject(m_hThread, INFINITE);
		
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}

	SocketClose();

	if( INVALID_HANDLE_VALUE != m_hRrecvEvent )
	{
		::WSACloseEvent(m_hRrecvEvent);
		m_hRrecvEvent = INVALID_HANDLE_VALUE;
	}

	m_i32ReceivedPacketSize = 0;
}







void	CSocketWork::SocketClose(void)
{
	if( INVALID_SOCKET != m_hSocket )
	{
		::shutdown(m_hSocket, SD_BOTH);
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}







DWORD WINAPI CSocketWork::SocketThread(LPVOID lpParam)
{
	CSocketWork* SockThread = (CSocketWork*) lpParam;
	
	SockThread->OnRunning();

	ExitThread(SockThread->m_ThreadID);
	
	return SockThread->m_ThreadID;
}








void	CSocketWork::OnRunning()
{
	m_bThreadOper =TRUE;

	while(m_bThreadOper)
	{
		if( IsConnected() && m_bSetHeartBit)
		{
			if( m_dwSendHeartBitTime + 60000 < GetTickCount() )
			{	
				CPacket SendPacket( PROTOCOL_BASE_HEART_BIT_REQ ); 	
				SendPacketMessage( &SendPacket );

				m_dwSendHeartBitTime = GetTickCount();
			}
		}

		WSANETWORKEVENTS event;
		::ZeroMemory( &event, sizeof(event) );

		// 처리할 이벤트를 확인한다.
		if( SOCKET_ERROR != ::WSAEventSelect (m_hSocket, m_hRrecvEvent, FD_WRITE | FD_READ | FD_CLOSE) )
		{
			if( SOCKET_ERROR != ::WSAEnumNetworkEvents(m_hSocket, m_hRrecvEvent, &event) )
			{
				if( event.lNetworkEvents & FD_READ )			OnReceive();
				if( event.lNetworkEvents & FD_CLOSE )			
				{
					m_bThreadOper = FALSE;
					SocketClose();
				}
			}
		}

		Sleep( 1 );
	}
}






void CSocketWork::OnReceive(void)
{
	// 패킷을 받는다.
	INT32 receivedByte = ::recv( m_hSocket, m_pReceiveBuffer + m_i32ReceivedPacketSize, (PACKETBUFFERSIZE-m_i32ReceivedPacketSize), 0 );

	if (SOCKET_ERROR == receivedByte)
	{
		INT32 error = ::WSAGetLastError();

		//연결되지 않은 소켓이거나 소켓의 연결이 종료된 경우 재생성&연결
		if( WSAENOTCONN == error || WSAESHUTDOWN == error )
		{
			CreateSocket();
			DoConnect();
		}
		
		return;		
	}
	
	if (0 == receivedByte)
	{
		return;
	}

	
	// 받은 데이터 크기 누적
	m_i32ReceivedPacketSize += receivedByte;

	if( PACKETBUFFERSIZE < m_i32ReceivedPacketSize )
	{
		//OnDestroy();
		return;
	}

	// 버퍼상의 파싱 시작 위치 
	INT32 startIdx = 0;

	// 수신한 모든 패킷을 파싱한다.
	for(;;)
	{
		// 패킷 사이즈 정보가 없으면 나머지 데이터를 받을때까지 처리하지 않는다.
		if ( SOCKET_HEAD_DATASIZE_SIZE > m_i32ReceivedPacketSize - startIdx )
		{
			break;
		}

		// 패킷 사이즈의 버퍼 오버플로우 검사
		INT32 packetSize = (*((INT16*)(m_pReceiveBuffer + startIdx)) & 0x7FFF) + PACKETHEADERSIZE;			  

		if (0 >= packetSize || PACKETBUFFERSIZE < packetSize)
		{
			OnDestroy();
			return;
		}


		// 종료조건: 파싱할 패킷이 없으면 중지한다.
		if (m_i32ReceivedPacketSize - startIdx < packetSize)
		{
			break;
		}


		// 수신데이터 Que로 전달 셋팅
		PacketSetting(m_pReceiveBuffer, packetSize, startIdx);
		
		startIdx += packetSize;
	}

	// 파싱 후 남은 데이터를 재정렬한다.
	m_i32ReceivedPacketSize -= startIdx;

	if (0 < startIdx && 0 < m_i32ReceivedPacketSize)
	{
		memmove( m_pReceiveBuffer, m_pReceiveBuffer + startIdx, m_i32ReceivedPacketSize );
	}
}






INT32	CSocketWork::SendPacketMessage( CPacket* packet )
{
	// 패킷을 다 보낼때까지 반복한다.
	INT32 sendIdx = 0;
	while(packet->GetPacketSize() > sendIdx)
	{
		INT32 sendedByte = ::send(m_hSocket, packet->GetPacketBuffer() + sendIdx, packet->GetPacketSize() - sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)
		{
			return -1;
		}

		// 연결이 끊어졌다.
		if (0 == sendedByte)
		{
			return 0;
		}

		// 전송 바이트만큼 인덱스 증가
		sendIdx += sendedByte;
	}

	return sendIdx;
}





BOOL	CSocketWork::IsConnected(void) const
{
	if (INVALID_SOCKET == m_hSocket)
	{
		return FALSE;
	}
	
	return TRUE;
}


SOCKET	CSocketWork::GetSocket(void)
{
	return m_hSocket;
}


void	CSocketWork::PacketSetting(const char * buffer, INT32 i32Size, INT32 i32index)
{
	m_PacketBuffer.PushData(buffer, i32Size, i32index);
}


CPacket* CSocketWork::GetPacket()
{
	if(m_PacketBuffer.IsEmpty())
		return m_PacketBuffer.PopData();
	else
		return NULL;
}






BOOL	CSocketWork::CreateSocket(void)
{
	if(m_hSocket != INVALID_SOCKET)
		SocketClose();

	// 소켓 생성
	m_hSocket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if (INVALID_SOCKET == m_hSocket )
	{
		return FALSE;
	}

	return TRUE;
}





BOOL	CSocketWork::DoConnect(void)
{
	INT32 result = ::connect( m_hSocket, (sockaddr*)&m_serverAddr, sizeof(m_serverAddr) );

	if (SOCKET_ERROR == result)
	{
		closesocket(m_hSocket);
		return FALSE;
	}

	return TRUE;
}
