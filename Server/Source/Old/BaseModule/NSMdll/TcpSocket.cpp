#include "stdafx.h"
#include "Tcpsocket.h"
#include "NetworkBase.h"
#include "Memory.h"

NSM_cTcpsocket::NSM_cTcpsocket()
{
	m_socket				=  INVALID_SOCKET;
	m_recvEvent				= INVALID_HANDLE_VALUE;
	m_receivedPacketSize	= 0;

	m_pAgent				= NULL;
	m_pCallbackFunc			= NULL;
}

NSM_cTcpsocket::~NSM_cTcpsocket()
{
	OnDestroy();
}

BOOL NSM_cTcpsocket::OnConnect(UINT32 serverIp, UINT16 serverPort, CallbackRoutine CallBack, void* pAgent )
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket)return FALSE;

	INT32 i32optVal = 1024*1024*2; //2M
	INT32 i32optLen = sizeof(INT32);
	::setsockopt( m_socket, SOL_SOCKET, SO_SNDBUF, ( char* )&i32optVal, i32optLen );

	// 서버 접속
	struct sockaddr_in serverAddr;
	NSM_nsMemory::FillZero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = serverIp;
	serverAddr.sin_port = ::htons(serverPort);

	int result = ::connect(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	if (SOCKET_ERROR == result)
	{
		NSM_nsNetworkBase::GetError("[NSM_cTcpsocket::OnConnectTo]");
		OnDisConnect(); 
		return FALSE;
	}

	m_pAgent		= pAgent;
	m_pCallbackFunc = CallBack;

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;
	return TRUE; 
}

void NSM_cTcpsocket::OnDisConnect(void)
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
	return; 
}

//BOOL NSM_cTcpsocket::OnCreate(SOCKET Socket)
//{
//	m_socket = Socket; 
//	return TRUE; 
//}

BOOL NSM_cTcpsocket::OnCreate(char* serverIp, UINT16 serverPort, CallbackRoutine CallBack, void* pAgent )
{	
	UINT32 ip = ::inet_addr(serverIp);
	return OnConnect(ip, serverPort, CallBack, pAgent );
}

BOOL NSM_cTcpsocket::OnCreate(UINT32 serverIp, UINT16 serverPort, CallbackRoutine CallBack, void* pAgent )
{
	return OnConnect(serverIp, serverPort, CallBack, pAgent );
}

void NSM_cTcpsocket::OnDestroy(void)
{
	OnDisConnect(); 
	return; 
}

/*
패킷을 받는다.
*/
INT32 NSM_cTcpsocket::OnReceive(void)
{	
	INT32 receivedByte = ::recv(m_socket, &m_receiveBuffer[m_receivedPacketSize], (TCP_SOCKET_BUFFER_SIZE - m_receivedPacketSize), 0);

	if (SOCKET_ERROR == receivedByte)
	{
		OnDestroy();
		return TCP_DISCONNECT_TYPE_READ_ERROR;
	}
	
	if (0 == receivedByte)return receivedByte; 
	
	// 받은 데이터 크기 누적
	m_receivedPacketSize += receivedByte;

	// 버퍼상의 파싱 시작 위치 
	INT32	startIdx = 0;
	INT32	ReadSize; 
	BOOL	bRead	 = TRUE; 
	while(bRead)
	{
		if( m_pCallbackFunc )
		{
			ReadSize = (*m_pCallbackFunc)( m_pAgent, &m_receiveBuffer[startIdx],  m_receivedPacketSize - startIdx );
		}
		else
		{
			ReadSize = PacketParsing( &m_receiveBuffer[startIdx],  m_receivedPacketSize - startIdx); 
		}
		if(ReadSize == 0)bRead = FALSE; 

		startIdx = startIdx + ReadSize; 
		if(startIdx >= m_receivedPacketSize)
		{
			startIdx = m_receivedPacketSize; 
			bRead = FALSE; 
		}
	}

	//Data Rearrange
	m_receivedPacketSize = m_receivedPacketSize - startIdx;
	if( (0 < startIdx) && (0 < m_receivedPacketSize) )
	{
		memmove(m_receiveBuffer, m_receiveBuffer + startIdx, m_receivedPacketSize);
	}

	return receivedByte;
}

INT32 NSM_cTcpsocket::PacketParsing(char * pPacket, INT32 Packetsize)
{
	// Do nothing
	return Packetsize; 
}

INT32 NSM_cTcpsocket::SendPacketMessage(char * pPacket, INT32 size)
{
	// 소켓이 닫혔으면 보네지 않는다.
	if (INVALID_SOCKET == m_socket)	return 0;

	// 패킷을 다 보낼때까지 반복한다.
	INT32 sendIdx = 0;
	while(size > sendIdx)
	{
		int sendedByte = ::send(m_socket, pPacket + sendIdx, size - sendIdx, 0);

		// 전송 실패 
		if (SOCKET_ERROR == sendedByte)
		{
			int GetLastError = ::WSAGetLastError();

			if ( GetLastError != WSAEWOULDBLOCK )
			{
				return 0;
			}
			else
			{
				Sleep(10);
				continue;
			} 
		}

		// 연결이 끊어졌다.
		if (0 == sendedByte)return 0;

		// 전송 바이트만큼 인덱스 증가
		sendIdx += sendedByte;
	}

	return sendIdx;
}


INT32 NSM_cTcpsocket::SelectEvent(void)
{
	INT32 iRv = 0; 
	WSANETWORKEVENTS event;
	NSM_nsMemory::FillZero(&event, sizeof(event));

	// 처리할 이벤트를 확인한다.
	if (SOCKET_ERROR != ::WSAEventSelect(m_socket, m_recvEvent, FD_WRITE | FD_READ | FD_CLOSE))
	{
		if (SOCKET_ERROR != ::WSAEnumNetworkEvents(m_socket, m_recvEvent, &event))
		{
			if(event.lNetworkEvents & FD_READ)	
			{
				iRv = OnReceive();
			}

			if(event.lNetworkEvents & FD_CLOSE) 
			{
				OnDestroy();
				iRv = TCP_DISCONNECT_TYPE_OTHER;
			}
		}
	}

	return iRv; 
}

BOOL NSM_cTcpsocket::IsConnected(void) const
{
	if (INVALID_SOCKET == m_socket)	return FALSE;	
	return TRUE;
}

SOCKET NSM_cTcpsocket::GetSocket(void)
{
	return m_socket;
}

BOOL NSM_cTcpsocket::SetSocket(SOCKET Socket)
{
	// 리시브 이벤트 생성
	m_recvEvent = ::WSACreateEvent();

	// 소켓 생성
	m_socket = Socket;

	// 리시브 관련 멤버 초기화
	m_receivedPacketSize = 0;

	return TRUE;
}
