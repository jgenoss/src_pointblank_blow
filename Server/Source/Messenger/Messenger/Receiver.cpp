#include "pch.h"
#include "Receiver.h"
#include "PacketLocker.h"
#include "Gateway.h"

I3_CLASS_INSTANCE(CReceiver, i3Thread);

CReceiver::CReceiver()
{
	m_socket			= INVALID_SOCKET;
	m_bIsRunning		= FALSE;
	m_i32idx			= -1;
	m_pPacketLocker		= NULL;
	m_bI3Packet			= TRUE;
	m_bRequestDisconnect = FALSE;
}

CReceiver::~CReceiver()
{
	if (INVALID_SOCKET != m_socket)
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

BOOL CReceiver::Create(UINT32 ui32idx,CPacketLocker* pPacketLocker)
{
	I3ASSERT(NULL == m_pPacketLocker);
	I3ASSERT(-1 == m_i32idx);


	char strBuf[MAX_STRING_COUNT];
	i3String::Format(strBuf, MAX_STRING_COUNT, "Receiver_%02d", ui32idx);

	if (FALSE == i3Thread::Create(strBuf))
	{
		return FALSE;
	}
	
	m_Combiner.Create(PACKETBUFFERSIZE * 3);
	m_socket = INVALID_SOCKET;
	m_i32idx = ui32idx;	

	m_pPacketLocker = pPacketLocker;

	return TRUE;
}

BOOL CReceiver::SetSocket(SOCKET socket,BOOL bI3Packet)
{	
	if (INVALID_SOCKET != m_socket)
	{
		return FALSE;
	}

	m_socket = socket;
	m_bI3Packet = bI3Packet;

	return TRUE;
}

UINT32 CReceiver::OnRunning(void* pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"RCVR", 0, m_ThreadID);

	m_bIsRunning = TRUE;
	m_state = WAITING;

	while(m_bIsRunning)
	{
		if (m_bRequestDisconnect)
		{
			m_state = DISCONNECTING;
			m_bRequestDisconnect = FALSE;
		}

		switch(m_state)
		{
		case WAITING:
			OnWaiting();
			break;
		case DISCONNECTING:
			OnDisconnecting();
			break;
		case PACKET_PROCESSING:
			OnPacketProcessing();
			break;
		case READING:
			OnReading();

			// 데이터를 기다리고 있다면 잠시 쉰다.
			if (READING == m_state)
			{
				::Sleep(1);
			}
			break;
		}
	}

	return 0;
}

void CReceiver::OnWaiting(void)
{
	if (INVALID_SOCKET == m_socket)
	{
		::Sleep(1);
		return;
	}

	m_state = PACKET_PROCESSING;
}

void CReceiver::OnPacketProcessing(void)
{
	UINT8 aui8packet[PACKETBUFFERSIZE];

	UINT32 ui32Length = m_Combiner.PopPacket(aui8packet, m_bI3Packet);

	while( ui32Length )
	{
		if (FALSE == m_pPacketLocker->Push(m_i32idx, aui8packet, ui32Length))
		{
			// Idx Overflow
		}
		else
		{
			//I3TRACE("[PacketLocker] Push idx = %d\n", m_idx);
		}

		ui32Length = m_Combiner.PopPacket(aui8packet, m_bI3Packet);
	}

	m_state = READING;
}
 
void CReceiver::OnReading(void)
{
	UINT8 aui8packet[PACKETBUFFERSIZE];

	INT32 i32Rv = ::recv(m_socket, (char*)aui8packet, PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == i32Rv)
	{
		_RecvErrorHandling();
		return;
	}
	
	if (0 == i32Rv)
	{
		g_pLog->WriteLog( L"CReceiver::OnReading() - disconnect"  );
		m_state = DISCONNECTING;
		return;
	}

	if (m_Combiner.PushStream(aui8packet, i32Rv))
	{


#if 0
		// 10초당 패킷 읽기량 구하기
		if (m_i32idx < (INT32)g_pConfig->m_ui32ServerCount)
		{
			static UINT32 ui32time = g_pConfig->GetServerTime();
			static UINT32 ui32size = 0;

			ui32size += i32Rv;

			if (g_pConfig->GetServerTime() > ui32time + 10)
			{
				char strMessage[MAX_STRING_COUNT];

				i3String::Format(strMessage, MAX_STRING_COUNT, "[NOTICE] Reading PacketSize = %d", ui32size);
				COMMON_LOG(strMessage);

				ui32time = g_pConfig->GetServerTime();
				ui32size = 0;
			}
		}

#endif

		m_state = PACKET_PROCESSING;
	}
	else
	{
		// Breaked Packet
		m_Combiner.Clear();
		m_state = DISCONNECTING;

		g_pLog->WriteLog( L"Breaked Pakcet - Receiver %d", m_i32idx  );
	}
}

void CReceiver::OnDisconnecting(void)
{	
	::shutdown(m_socket, SD_BOTH);
	::closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	// 완성시키지 못한 잔여 packet을 지운다
	m_Combiner.Clear();

	m_state = WAITING;

    
	//char message[MAX_STRING_COUNT] = {0, };
	//i3String::Format(message, "Disconnected GameServer - %d", m_idx);
	//WRITE_LOG(m_idx, message);
	//이거 쓰레드 확인해야 합니다. 
}

BOOL CReceiver::SendPacket(i3NetworkPacket* pPacket)
{
	char strMessage[MAX_STRING_COUNT];
	INT32 i32ErrorCode = 0;

	if (INVALID_SOCKET == m_socket)
	{
//		char message[MAX_STRING_COUNT];
//		i3String::Format(message, "[ERROR] CReceiver::SendPacket() - Invalid Socket, packet(%d)", pPacket->GetProtocolID());
//		COMMON_LOG(message);

		return FALSE;
	}

	UINT32 ui32Send = 0;
	UINT32 ui32Length = pPacket->GetPacketSize();
	UINT8* pui8Data = (UINT8*)pPacket->GetPacketBuffer();

	UINT32 ui32Rv;
	while (ui32Send < ui32Length)
	{
		ui32Rv = ::send(m_socket, (const char*)pui8Data + ui32Send, ui32Length - ui32Send, 0);

		if (SOCKET_ERROR == ui32Rv)
		{
			i3mem::FillZero( strMessage, sizeof (char) * MAX_STRING_COUNT );

			i32ErrorCode = ::WSAGetLastError();
			
			switch(i32ErrorCode)
			{
			case WSANOTINITIALISED:
			case WSAEFAULT:
			case WSAEINVAL:
			case WSAENOTSOCK:
			case WSAEOPNOTSUPP:
			case WSAEACCES:
				// 일어날 수 없음
				//i3String::Format(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
				//쓰레드 확인하고 보내야함.
				return FALSE;
			case WSAENETDOWN:
			case WSAENETRESET:
			case WSAESHUTDOWN:
			case WSAETIMEDOUT:
			case WSAECONNRESET:
			case WSAENOTCONN:
			case WSAEHOSTUNREACH:
			case WSAEINTR:
				// 연결 끊김
				//i3String::Format(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
				//쓰레드 확인하고 보내야함.
			case WSAECONNABORTED:
				return FALSE;
			case WSAEINPROGRESS:
			case WSAEMSGSIZE:
			case WSAENOBUFS:
				//i3String::Format(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
				//쓰레드 확인하고 보내야함.
			case WSAEWOULDBLOCK:	// 정상
				break;
			default:
				//i3String::Format(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
				//쓰레드 확인하고 보내야함.
				return FALSE;
			}
		}
		else
		{
			ui32Send += ui32Rv;
		}
	}	

#if 0
	if (PROTOCOL_AUTH_SHOP_GET_SAILLIST_ACK == pPacket->GetProtocolID() ||
		PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS == pPacket->GetProtocolID())
	{
		char message[MAX_STRING_COUNT];
		i3String::Format(message, "GET_SAILLIST_ACK - sendByte(%d)", nSend);
		WRITE_LOG(m_idx, message);
	}
#endif

	return TRUE; 
}

void CReceiver::_RecvErrorHandling(void)
{
	char strMessage[MAX_STRING_COUNT];
	INT32 i32ErrorCode = ::WSAGetLastError();

	i3mem::FillZero( strMessage, sizeof (char) * MAX_STRING_COUNT );

	switch(i32ErrorCode)
	{
	case WSANOTINITIALISED:
	case WSAEFAULT:
	case WSAEINVAL:
	case WSAEISCONN:
	case WSAENOTSOCK:
	case WSAEOPNOTSUPP:
		// 발생할 수 없음
		// fall through
	case WSAENETDOWN:
	case WSAENETRESET:
	case WSAESHUTDOWN:
	case WSAETIMEDOUT:
	case WSAECONNRESET:
	case WSAEINTR:
		// 접속 끊어짐
		//i3String::Format(message, "CReceiver::_RecvErrorHandling() - errorCode(%d)", nErrorCode);
		//WRITE_LOG(m_idx, message);
		//쓰레드 확인하고 보내야함.
	case WSAECONNABORTED:
		m_state = DISCONNECTING;
		break;
	case WSAEINPROGRESS:
	case WSAEMSGSIZE:
		// 재시도
		//i3String::Format(message, "CReceiver::_RecvErrorHandling() - errorCode(%d)", nErrorCode);
		//WRITE_LOG(m_idx, message);
		//쓰레드 확인하고 보내야함.
	case WSAEWOULDBLOCK:	// 정상
		break;
	default:
		//i3String::Format(message, "CReceiver::_RecvErrorHandling() - errorCode(%d)", nErrorCode);
		//WRITE_LOG(m_idx, message);
		//쓰레드 확인하고 보내야함.
		break;
	}
}

BOOL CReceiver::IsWork(void)
{
	return INVALID_SOCKET != m_socket;
}

BOOL CReceiver::IsDisconnecting(void)
{
	return DISCONNECTING == m_state;
}

BOOL CReceiver::Disconnect(void)
{
	for(UINT32 i = 0; i < 1000; i++)
	{
		if (WAITING == m_state)
		{
			return TRUE;
		}

		::Sleep(1);
	}

	m_bRequestDisconnect = TRUE;

	return FALSE;
}

void CReceiver::RequestDisconnect(void)
{
	m_bRequestDisconnect = TRUE;
}
