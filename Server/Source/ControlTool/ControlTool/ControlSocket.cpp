#include "pch.h"
#include "ControlSocket.h"
#include "ControlTool.h"
#include "ControlToolDoc.h"
#include "ControlToolView.h"

CControlSocket::CControlSocket()
{
	m_hSocket = INVALID_SOCKET;
	m_hRrecvEvent = INVALID_HANDLE_VALUE;

	m_i32ReceivedPacketSize = 0;
	::ZeroMemory( m_pReceiveBuffer, sizeof(m_pReceiveBuffer) );
}

CControlSocket::~CControlSocket()
{
	OnDestroy();
}

bool CControlSocket::OnCreate( UINT32 ui32IP, UINT16 ui16Port, void* pView )
{
	m_pView = pView;

	// 리시브 이벤트 생성
	m_hRrecvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_hSocket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if (INVALID_SOCKET == m_hSocket )
	{
		return false;
	}

	// 서버 접속
	struct sockaddr_in serverAddr;
	::ZeroMemory( &serverAddr, sizeof(serverAddr) );
	serverAddr.sin_family		= AF_INET;
	serverAddr.sin_addr.s_addr	= ui32IP;
	serverAddr.sin_port			= ::htons(ui16Port);

	INT32 result = ::connect( m_hSocket, (sockaddr*)&serverAddr, sizeof(serverAddr) );

	m_dwSendHeartBitTime = GetTickCount();

	if (SOCKET_ERROR == result)
	{
		return false;
	}

	// 리시브 관련 멤버 초기화
	m_i32ReceivedPacketSize = 0;

	return true;
}

void	CControlSocket::OnDestroy()
{
	if( INVALID_HANDLE_VALUE != m_hRrecvEvent )
	{
		::WSACloseEvent(m_hRrecvEvent);
		m_hRrecvEvent = INVALID_HANDLE_VALUE;
	}

	if( INVALID_SOCKET != m_hSocket )
	{
		::shutdown(m_hSocket, SD_BOTH);
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	m_i32ReceivedPacketSize = 0;
}

void	CControlSocket::OnRunning()
{
	if( IsConnected() )
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
			if( event.lNetworkEvents & FD_READ )	OnReceive();
			if( event.lNetworkEvents & FD_CLOSE )	OnDestroy();
		}
	}
}

void CControlSocket::OnReceive(void)
{
	// 패킷을 받는다.
	INT32 receivedByte = ::recv( m_hSocket, m_pReceiveBuffer + m_i32ReceivedPacketSize, (PACKETBUFFERSIZE-m_i32ReceivedPacketSize), 0 );

	if (SOCKET_ERROR == receivedByte)
	{
		INT32 error = ::WSAGetLastError();

		OnDestroy();
		return;		
	}
	
	if (0 == receivedByte)
	{
		return;	// ClosedSocket
	}

	
	// 받은 데이터 크기 누적
	m_i32ReceivedPacketSize += receivedByte;

	if( PACKETBUFFERSIZE < m_i32ReceivedPacketSize )
	{
		OnDestroy();
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


		// 파싱
		CPacket packet;
		
		packet.CopyToBuffer(m_pReceiveBuffer, packetSize, startIdx);
		PacketParsing(&packet);
		
		startIdx += packetSize;
	}

	// 파싱 후 남은 데이터를 재정렬한다.
	m_i32ReceivedPacketSize -= startIdx;

	if (0 < startIdx && 0 < m_i32ReceivedPacketSize)
	{
		memmove( m_pReceiveBuffer, m_pReceiveBuffer + startIdx, m_i32ReceivedPacketSize );
	}
}

INT32	CControlSocket::SendPacketMessage( CPacket* packet )
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_hSocket)	return 0;


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

BOOL	CControlSocket::IsConnected(void) const
{
	if (INVALID_SOCKET == m_hSocket)
	{
		return FALSE;
	}
	
	return TRUE;
}

SOCKET	CControlSocket::GetSocket(void)
{
	return m_hSocket;
}

void	CControlSocket::PacketParsing(CPacket* packet)
{
	((CControlToolView*)m_pView)->PacketParsing( packet );
}