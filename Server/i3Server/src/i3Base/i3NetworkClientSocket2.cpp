#include "i3CommonType.h"
#include "i3NetworkClientSocket2.h"
#include "i3NetworkPacket.h"
#include "i3Inet.h"

I3_CLASS_INSTANCE(i3NetworkClientSocket2, i3ElementBase); 

i3NetworkClientSocket2::i3NetworkClientSocket2()
{
	m_socket =  INVALID_SOCKET;
	m_recvEvent = INVALID_HANDLE_VALUE;

	m_receivedPacketSize = 0;
	i3mem::FillZero(m_receiveBuffer, sizeof(m_receiveBuffer));
}

i3NetworkClientSocket2::~i3NetworkClientSocket2()
{
	OnDestroy();
}

BOOL i3NetworkClientSocket2::OnCreate( char* strIP,UINT16 ui16Port, CallbackRoutine2 CallBack, void* pAgent )
{	
	UINT32 ui32IP = ::inet_addr( strIP );

	return _Create( ui32IP, ui16Port, CallBack, pAgent );
}

BOOL i3NetworkClientSocket2::OnCreate( UINT32 ui32IP,UINT16 ui16Port, CallbackRoutine2 CallBack, void* pAgent )
{
	return _Create( ui32IP, ui16Port, CallBack, pAgent );
}

INT32 i3NetworkClientSocket2::OnReceive(void)
{
	// 패킷을 받는다.
	INT32 receivedByte = ::recv(m_socket, GetRecvBuffer(), PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == receivedByte)
	{
		char message[MAX_PATH];
		INT32 error = ::WSAGetLastError();

		sprintf(message, "[i3NetworkClientSocket2::OnReceive] lastError = %d", error);
		i3Net::GetError(message);

		DisConnect();
		return TCP_DISCONNECT_TYPE_READ_ERROR;		
	}
	
	if (0 == receivedByte)
	{
		return receivedByte;	// ClosedSocket
	}

	
	// 받은 데이터 크기 누적
	m_receivedPacketSize += receivedByte;

	if (CLIENT_SOCKET_BUFFER_SIZE < m_receivedPacketSize)
	{
		I3TRACE("[WARNING] i3NetworkClientSocket2::OnReceive() - Buffer overflow(%d bytes)!\n", m_receivedPacketSize);
		DisConnect();
		return TCP_DISCONNECT_TYPE_READ_ERROR;
	}


	// 버퍼상의 파싱 시작 위치 
	INT32 i32StartIdx = 0;

	// 수신한 모든 패킷을 파싱한다.
	for(;;)
	{
		// 패킷 사이즈의 버퍼 오버플로우 검사
		INT32 packetSize;
		if( m_pCallbackFunc )
		{
			packetSize = (*m_pCallbackFunc)( m_pAgent, m_receiveBuffer+i32StartIdx, m_receivedPacketSize-i32StartIdx );
		}
		else
		{
			packetSize = PacketParsing( m_receiveBuffer+i32StartIdx, m_receivedPacketSize-i32StartIdx );
		}

		if (0 > packetSize || PACKETBUFFERSIZE < packetSize)
		{
			I3TRACE("[WARNING] i3NetworkClientSocket2::OnReceive() - Break packet size(%d bytes)!\n", packetSize);
			DisConnect();
			return TCP_DISCONNECT_TYPE_READ_ERROR;
		}
		if( 0 == packetSize ) break;

		i32StartIdx += packetSize;
		if( i32StartIdx >= m_receivedPacketSize )
		{
			break;
		}
	}


	// 파싱 후 남은 데이터를 재정렬한다.
	m_receivedPacketSize -= i32StartIdx;
	if (0 < i32StartIdx && 0 < m_receivedPacketSize)
	{
		memmove(m_receiveBuffer, m_receiveBuffer + i32StartIdx, m_receivedPacketSize);
	}

	return receivedByte;
}

void i3NetworkClientSocket2::DisConnect()
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

void i3NetworkClientSocket2::OnDestroy(void)
{
	DisConnect();
}

INT32 i3NetworkClientSocket2::PacketParsing(char* pBuffer, INT32 nSize)
{
	// Do nothing
	return 0;
}

INT32 i3NetworkClientSocket2::SendPacketMessage(i3NetworkPacket* packet)
{
	return SendPacketMessage(packet->GetPacketBuffer(), packet->GetPacketSize());
}

INT32 i3NetworkClientSocket2::SendPacketMessage(const char* pBuffer, INT32 nSize)
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

			sprintf(message, "[i3NetworkClientSocket2::SendPacketMessage] lastError = %d", error);
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

INT32 i3NetworkClientSocket2::SelectEvent(void)
{
	INT32 i32Rv = 0; 
	WSANETWORKEVENTS event;
	i3mem::FillZero(&event, sizeof(event));

	// 처리할 이벤트를 확인한다.
	if (SOCKET_ERROR != ::WSAEventSelect(m_socket, m_recvEvent, FD_WRITE | FD_READ | FD_CLOSE))
	{
		if (SOCKET_ERROR != ::WSAEnumNetworkEvents(m_socket, m_recvEvent, &event))
		{
			if (TESTBIT(event.lNetworkEvents, FD_READ))
			{
				i32Rv = OnReceive();
			}
			if (TESTBIT(event.lNetworkEvents, FD_CLOSE))
			{
				DisConnect();
				i32Rv = TCP_DISCONNECT_TYPE_OTHER;
			}

			return i32Rv;
		}
	}

	// 오류 발생시 처리한다.
	// 오류 발생 원인이 동일하기때문에 출력을 구분하지 않습니다.
	char message[MAX_PATH];	
	INT32 error = ::WSAGetLastError();
	sprintf(message, "[i3NetworkClientSocket2::SelectEvent] lastError = %d", error);
	i3Net::GetError(message);

	return i32Rv;
}

BOOL i3NetworkClientSocket2::IsConnected(void) const
{
	if (INVALID_SOCKET == m_socket)
	{
		return FALSE;
	}
	
	return TRUE;
}

SOCKET i3NetworkClientSocket2::GetSocket(void)
{
	return m_socket;
}

char *	i3NetworkClientSocket2::GetRecvBuffer()
{
	return m_receiveBuffer + m_receivedPacketSize;
}

BOOL i3NetworkClientSocket2::SetSocket(SOCKET Socket)
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = Socket;

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;

	return TRUE;
}

BOOL i3NetworkClientSocket2::_Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine2 CallBack, void* pAgent )
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_socket)
	{
		I3TRACE("[i3NetworkClientSocket2::OnConnectTo] Error Socket Is Invalid \n"); 
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
		i3Net::GetError("[i3NetworkClientSocket2::OnConnectTo]");
		return FALSE;
	}

	m_pAgent		= pAgent;
	m_pCallbackFunc = CallBack;

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;

	return TRUE;
}
