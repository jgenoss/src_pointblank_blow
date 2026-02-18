#include "pch.h"
#include "Receiver.h"
#include "PacketLocker.h"
#include "LogFile.h"
#include "Gateway.h"


I3_CLASS_INSTANCE(CReceiver, i3Thread);

CReceiver::CReceiver()
{
	m_socket = INVALID_SOCKET;	
	m_IsRunning = FALSE;
	m_idx = -1;
	m_pPacketLocker = NULL;
	m_bI3Packet = TRUE;
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

BOOL CReceiver::Create(UINT32 idx,CPacketLocker* pPacketLocker)
{
	I3ASSERT(NULL == m_pPacketLocker);
	I3ASSERT(-1 == m_idx);

	char strBuf[MAX_STRING_COUNT];
	sprintf(strBuf, "Receiver_%02d", idx);

	if (FALSE == i3Thread::Create(strBuf))
	{
		return FALSE;
	}

	m_Combiner.Create(PACKETBUFFERSIZE * 3);
	m_socket = INVALID_SOCKET;
	m_idx = idx;	

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
	m_IsRunning = TRUE;
	m_state = WAITING;

	while(m_IsRunning)
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
	UINT8 packet[PACKETBUFFERSIZE];

	UINT32 nLength = m_Combiner.PopPacket(packet, m_bI3Packet);

	while( nLength )
	{
		if (FALSE == m_pPacketLocker->Push(m_idx, packet, nLength))
		{
			// Idx Overflow
		}
		else
		{
			I3TRACE("[PacketLocker] Push idx = %d\n", m_idx);
		}

		nLength = m_Combiner.PopPacket(packet, m_bI3Packet);
	}

	m_state = READING;
}
 
void CReceiver::OnReading(void)
{
	UINT8 packet[PACKETBUFFERSIZE];

	INT32 rv = ::recv(m_socket, (char*)packet, PACKETBUFFERSIZE, 0);

	if (SOCKET_ERROR == rv)
	{
		_RecvErrorHandling();
		return;
	}
	
	if (0 == rv)
	{
		//WRITE_LOG(m_idx, "CReceiver::OnReading() - disconnect");
		m_state = DISCONNECTING;
		return;
	}

	if (m_Combiner.PushStream(packet, rv))
	{


#if 0
		// 10초당 패킷 읽기량 구하기
		if (m_idx < (INT32)g_pConfig->m_ServerCount)
		{
			static UINT32 time = g_pConfig->GetServerTime();
			static UINT32 size = 0;

			size += rv;

			if (g_pConfig->GetServerTime() > time + 10)
			{
				char message[MAX_STRING_COUNT];

				sprintf(message, "[NOTICE] Reading PacketSize = %d", size);
				//WRITE_LOG(message);

				time = g_pConfig->GetServerTime();
				size = 0;
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

		char strBuf[MAX_STRING_COUNT];
		sprintf(strBuf, "Breaked Pakcet - Receiver %d", m_idx);
		//WRITE_LOG(m_idx, strBuf);
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

    
	char message[MAX_STRING_COUNT] = {0, };

	switch(GetReceiverType(m_idx))
	{
	case GAME_SERVER:
		UNSETBIT(g_ConnectServer, 0x01 << m_idx);
		g_pConfig->SubConnectedGameServer();
		sprintf(message, "Disconnected GameServer - %d", m_idx);
		break;
	default:
		i3String::Copy(message, "Disconnected UnkownServer", MAX_STRING_COUNT);
		break;
	}

	//WRITE_LOG(m_idx, message);
}

BOOL CReceiver::SendPacket(i3NetworkPacket* pPacket)
{
	if (INVALID_SOCKET == m_socket)
	{
		char message[MAX_STRING_COUNT];
		sprintf(message, "[ERROR] CReceiver::SendPacket() - Invalid Socket, packet(%d)", pPacket->GetProtocolID());
		//WRITE_LOG(message);

		return FALSE;
	}

	UINT32 nSend = 0;
	UINT32 nLength = pPacket->GetPacketSize();
	UINT8* pData = (UINT8*)pPacket->GetPacketBuffer();
	
	UINT32 rv;
	while (nSend < nLength)
	{
		rv = ::send(m_socket, (const char*)pData + nSend, nLength - nSend, 0);

		if (SOCKET_ERROR == rv)
		{
			char message[MAX_STRING_COUNT];

			INT32 nErrorCode = ::WSAGetLastError();
			
			switch(nErrorCode)
			{
			case WSANOTINITIALISED:
			case WSAEFAULT:
			case WSAEINVAL:
			case WSAENOTSOCK:
			case WSAEOPNOTSUPP:
			case WSAEACCES:
				// 일어날 수 없음
				sprintf(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
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
				sprintf(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
			case WSAECONNABORTED:
				return FALSE;			
			case WSAEINPROGRESS:			
			case WSAEMSGSIZE:
			case WSAENOBUFS:
				sprintf(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
			case WSAEWOULDBLOCK:	// 정상
				break;
			default:
				sprintf(message, "CReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
				return FALSE;
			}
		}
		else
		{
			nSend += rv;
		}
	}	

#if 0
	if (PROTOCOL_AUTH_SHOP_GET_SAILLIST_ACK == pPacket->GetProtocolID() ||
		PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS == pPacket->GetProtocolID())
	{
		char message[MAX_STRING_COUNT];
		sprintf(message, "GET_SAILLIST_ACK - sendByte(%d)", nSend);
		//WRITE_LOG(m_idx, message);
	}
#endif

	return TRUE; 
}

BOOL CReceiver::SendIBGamePacket(UINT8* pPakcet,UINT32 nSize)
{
	if (INVALID_SOCKET == m_socket)
	{
		return FALSE;
	}

	UINT32 nSend = 0;
	UINT32 nLength = nSize;
	UINT8* pData = pPakcet;
	
	UINT32 rv;
	while (nSend < nLength)
	{
		rv = ::send(m_socket, (const char*)pData + nSend, nLength - nSend, 0);

		if (SOCKET_ERROR == rv)
		{
			INT32 nErrorCode = ::WSAGetLastError();
			
			switch(nErrorCode)
			{
			case WSANOTINITIALISED:
			case WSAEFAULT:
			case WSAEINVAL:
			case WSAENOTSOCK:
			case WSAEOPNOTSUPP:
			case WSAEACCES:
				// 일어날 수 없음
				return FALSE;
			case WSAENETDOWN:
			case WSAENETRESET:
			case WSAESHUTDOWN:
			case WSAETIMEDOUT:
			case WSAECONNRESET:
			case WSAENOTCONN:
			case WSAEHOSTUNREACH:
			case WSAEINTR:
			case WSAECONNABORTED:
				// 연결 끊김
				return FALSE;
			case WSAEINPROGRESS:
			case WSAEWOULDBLOCK:	// 정상
			case WSAEMSGSIZE:
			case WSAENOBUFS:
				break;
			default:
				return FALSE;
			}
		}
		else
		{
			nSend += rv;
		}
	}	

	return TRUE; 
}

void CReceiver::_RecvErrorHandling(void)
{
	char message[MAX_STRING_COUNT];
	INT32 nErrorCode = ::WSAGetLastError();

	switch(nErrorCode)
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
		sprintf(message, "CReceiver::_RecvErrorHandling() - errorCode(%d)", nErrorCode);
		//WRITE_LOG(m_idx, message);
	case WSAECONNABORTED:	
		m_state = DISCONNECTING;
		break;
	case WSAEINPROGRESS:	
	case WSAEMSGSIZE:
		// 재시도
		sprintf(message, "CReceiver::_RecvErrorHandling() - errorCode(%d)", nErrorCode);
		//WRITE_LOG(m_idx, message);
	case WSAEWOULDBLOCK:	// 정상
		break;
	default:
		sprintf(message, "CReceiver::_RecvErrorHandling() - errorCode(%d)", nErrorCode);
		//WRITE_LOG(m_idx, message);	
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

RECEIVER_TYPE CReceiver::GetReceiverType(UINT32 idx)
{
	if (idx < g_pConfig->m_ServerCount)
	{
		return GAME_SERVER;
	}

	return UNKOWN_SERVER;
}
