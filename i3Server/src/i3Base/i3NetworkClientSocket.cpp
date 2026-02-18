#include "i3CommonType.h"
#include "i3NetworkClientSocket.h"
#include "i3NetworkPacket.h"
#include "i3Inet.h"

I3_CLASS_INSTANCE(i3NetworkClientSocket, i3ElementBase); 

i3NetworkClientSocket::i3NetworkClientSocket()
{
	m_socket =  INVALID_SOCKET;
	m_recvEvent = INVALID_HANDLE_VALUE;

	m_receivedPacketSize = 0;
	i3mem::FillZero(m_receiveBuffer, sizeof(m_receiveBuffer));
}

i3NetworkClientSocket::~i3NetworkClientSocket()
{
	OnDestroy();
}

BOOL i3NetworkClientSocket::OnCreate( char* strIP,UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{	
	UINT32 ui32IP = ::inet_addr( strIP );

	return _Create( ui32IP, ui16Port, CallBack, pAgent );
}

BOOL i3NetworkClientSocket::OnCreate( UINT32 ui32IP,UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{
	return _Create( ui32IP, ui16Port, CallBack, pAgent );
}

void i3NetworkClientSocket::OnReceive(void)
{
	// 패킷을 받는다.
	INT32 receivedByte = ::recv(m_socket, GetRecvBuffer(), PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == receivedByte)
	{
		char message[MAX_PATH];
		INT32 error = ::WSAGetLastError();

		sprintf(message, "[i3NetworkClientSocket::OnReceive] lastError = %d", error);
		i3Net::GetError(message);

		DisConnect();
		return;		
	}

	if (0 == receivedByte)
	{
		return;	// ClosedSocket
	}


	// 받은 데이터 크기 누적
	m_receivedPacketSize += receivedByte;

	if (CLIENT_SOCKET_BUFFER_SIZE < m_receivedPacketSize)
	{
		I3TRACE("[WARNING] i3NetworkClientSocket::OnReceive() - Buffer overflow(%d bytes)!\n", m_receivedPacketSize);
		DisConnect();
		return;
	}


	// 버퍼상의 파싱 시작 위치 
	INT32 startIdx = 0;

	// 수신한 모든 패킷을 파싱한다.
	for(;;)
	{
		// 패킷 사이즈 정보가 없으면 나머지 데이터를 받을때까지 처리하지 않는다.
		if ( SOCKET_HEAD_DATASIZE_SIZE > m_receivedPacketSize - startIdx )
		{
			break;
		}

		// 패킷 사이즈의 버퍼 오버플로우 검사
		INT32 packetSize = (*((INT16*)(m_receiveBuffer + startIdx)) & 0x7FFF) + PACKETHEADERSIZE;			  

		if (0 >= packetSize || PACKETBUFFERSIZE < packetSize)
		{
			I3TRACE("[WARNING] i3NetworkClientSocket::OnReceive() - Break packet size(%d bytes)!\n", packetSize);
			DisConnect();
			return;
		}


		// 종료조건: 파싱할 패킷이 없으면 중지한다.
		if (m_receivedPacketSize - startIdx < packetSize)
		{
			break;
		}


		// 파싱
		i3NetworkPacket Packet;
		Packet.CopyToBuffer(m_receiveBuffer, packetSize, startIdx);

		if( m_pCallbackFunc )
		{
			(*m_pCallbackFunc)( m_pAgent, &Packet );
		}
		else
		{
			PacketParsing( &Packet );
		}

		//PacketParsing(m_receiveBuffer+startIdx, packetSize);
		
		startIdx += packetSize;
	}


	// 파싱 후 남은 데이터를 재정렬한다.
	m_receivedPacketSize -= startIdx;

	if (0 < startIdx && 0 < m_receivedPacketSize)
	{
		memmove(m_receiveBuffer, m_receiveBuffer + startIdx, m_receivedPacketSize);
	}
}

void i3NetworkClientSocket::DisConnect()
{
	if (INVALID_HANDLE_VALUE != m_recvEvent)
	{
		::WSACloseEvent(m_recvEvent);
		m_recvEvent = INVALID_HANDLE_VALUE;
	}

	if (INVALID_SOCKET != m_socket)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	m_receivedPacketSize = 0;
}

void i3NetworkClientSocket::OnDestroy(void)
{
	DisConnect();	
}

void i3NetworkClientSocket::PacketParsing(i3NetworkPacket* packet)
{
	// Do nothing
}

INT32 i3NetworkClientSocket::SendPacketMessage(i3NetworkPacket* packet)
{
	return SendPacketMessage(packet->GetPacketBuffer(), packet->GetPacketSize());
}

INT32 i3NetworkClientSocket::SendPacketMessage(const char* pBuffer, INT32 nSize)
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_socket)	return 0;

	// 패킷을 다 보낼때까지 반복한다.
	INT32 sendIdx = 0;
	while(nSize > sendIdx)
	{
		INT32 sendedByte = ::send(m_socket, pBuffer + sendIdx, nSize - sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)
		{
			char message[MAX_PATH];
			INT32 error = ::WSAGetLastError();

			sprintf(message, "[i3NetworkClientSocket::SendPacketMessage] lastError = %d", error);
			i3Net::GetError(message);
			
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

void i3NetworkClientSocket::SelectEvent(void)
{
	WSANETWORKEVENTS event;
	i3mem::FillZero(&event, sizeof(event));

	// 처리할 이벤트를 확인한다.
	if (SOCKET_ERROR != ::WSAEventSelect(m_socket, m_recvEvent, FD_WRITE | FD_READ | FD_CLOSE))
	{
		if (SOCKET_ERROR != ::WSAEnumNetworkEvents(m_socket, m_recvEvent, &event))
		{
			if (TESTBIT(event.lNetworkEvents, FD_READ)) OnReceive();
			if (TESTBIT(event.lNetworkEvents, FD_CLOSE)) DisConnect();

			return;
		}
	}

	// 오류 발생시 처리한다.
	// 오류 발생 원인이 동일하기때문에 출력을 구분하지 않습니다.
	char message[MAX_PATH];	
	INT32 error = ::WSAGetLastError();

	sprintf(message, "[i3NetworkClientSocket::SelectEvent] lastError = %d", error);
	i3Net::GetError(message);
}

BOOL i3NetworkClientSocket::IsConnected(void) const
{
	if (INVALID_SOCKET == m_socket)
	{
		return FALSE;
	}
	
	return TRUE;
}

SOCKET i3NetworkClientSocket::GetSocket(void)
{
	return m_socket;
}

char *	i3NetworkClientSocket::GetRecvBuffer()
{
	return m_receiveBuffer + m_receivedPacketSize;
}

BOOL i3NetworkClientSocket::SetSocket(SOCKET Socket)
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = Socket;

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;

	return TRUE;
}

BOOL i3NetworkClientSocket::_Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent )
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_socket)
	{
		I3TRACE("[i3NetworkClientSocket::OnConnectTo] Error Socket Is Invalid \n"); 
		return FALSE;
	}

	// 서버 접속
	struct sockaddr_in serverAddr;
	i3mem::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ui32IP;
	serverAddr.sin_port = ::htons(ui16Port);

	INT32 result = ::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	if (SOCKET_ERROR == result)
	{
		i3Net::GetError("[i3NetworkClientSocket::OnConnectTo]");
		return FALSE;
	}

	m_pAgent		= pAgent;
	m_pCallbackFunc = CallBack;

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;

	return TRUE;
}
