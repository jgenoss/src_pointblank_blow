#include "pch.h"
#include "ReceiverGSM.h"
#include "PacketLocker.h"
#include "Gateway.h"

I3_CLASS_INSTANCE(CReceiverGSM, i3Thread);

CReceiverGSM::CReceiverGSM()
{
	m_socket			= INVALID_SOCKET;	
	m_IsRunning			= FALSE;
	m_idx				= -1;
	m_pPacketLocker		= NULL;

	m_i32RecvSize		= 0;
	m_state				= STATE_WAITING;
}

CReceiverGSM::~CReceiverGSM()
{
	OnDestroy();
}

BOOL CReceiverGSM::Create(UINT32 idx, CPacketLocker* pPacketLocker)
{
	if( (TRANS_METHOD_NCSOFT_GP != g_pConfig->m_iTransMethod) &&
		(TRANS_METHOD_NCSOFT_GPSA != g_pConfig->m_iTransMethod) )	return TRUE; 

	I3ASSERT(NULL == m_pPacketLocker);
	I3ASSERT(-1 == m_idx);

	char strBuf[MAX_STRING_COUNT];
	sprintf(strBuf, "Receiver_Thread_%02d", idx);

	if (FALSE == i3Thread::Create(strBuf))	return FALSE;

	m_state			= STATE_WAITING;
	m_pPacketLocker = pPacketLocker;
	m_socket		= INVALID_SOCKET;
	m_idx			= idx;

	return TRUE;
}

void CReceiverGSM::OnDestroy()
{
	m_IsRunning = FALSE;
	i3Thread::WaitForFinish();

	if( INVALID_SOCKET != m_socket )
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

BOOL CReceiverGSM::SetSocket(SOCKET socket, BOOL bI3Packet)
{
	if( INVALID_SOCKET	!= m_socket )	return FALSE;
	if( STATE_WAITING	!= m_state )	return FALSE;

	m_socket	= socket;
	m_state		= STATE_READING;

	return TRUE;
}

UINT32 CReceiverGSM::OnRunning(void* pUserData)
{
	m_IsRunning = TRUE;
	while( m_IsRunning )
	{
		switch( m_state )
		{
		case STATE_READING			:
			_OnReading();
			break;
		case STATE_DISCONNECTING	: 
			_OnDisconnecting();
			break;
		case STATE_WAITING			: 
			::Sleep( 1000 );
			break;
		}
		Sleep(1);
	}

	return 0;
}
 
void CReceiverGSM::_OnReading(void)
{
#ifdef LINK_GSM
	INT32 rv = ::recv(m_socket, (char*)&(m_ui8RecvBuffer[m_i32RecvSize]), PACKETBUFFERSIZE - m_i32RecvSize, 0);

	if( SOCKET_ERROR == rv )
	{
		_RecvErrorHandling();
		return;
	}
	
	if( 0 == rv )
	{
		g_pModuleLogFile->Write_Log_Recv( m_idx, "CReceiverGSM::OnReading() - Disconnect Recv 0"  );
		m_state = STATE_DISCONNECTING;
		return;
	}

	m_i32RecvSize += rv;

	if( PACKETBUFFERSIZE < m_i32RecvSize )
	{	// 말이 안됩니다. 있을수 없는 일..
		g_pModuleLogFile->Write_Log_Recv( m_idx, "CReceiverGSM::OnReading() - Disconnect Recv Buffer Over"  );
		m_state = STATE_DISCONNECTING;
		return;
	}

	INT32	i32ReadSize	= 0;
	UINT16	ui16PacketSize;

	UINT8	pui8ResultData[ PACKETBUFFERSIZE ];
	int		nResultSize;
	INT32	i32IntSize = sizeof(int);

	while( ( m_i32RecvSize - i32ReadSize ) >= GSM_PACKETHEADERSIZE  )
	{
		ui16PacketSize = (UINT16)*((UINT16*)&m_ui8RecvBuffer[ i32ReadSize ]);
		if( ui16PacketSize > PACKETBUFFERSIZE )
		{	// 패킷이 깨진경우입니다. 
			g_pModuleLogFile->Write_Log_Recv(m_idx, "CReceiver::OnReading() - Disconnect Pack Break" );
			m_state = STATE_DISCONNECTING;
			return;
		}
		if( ui16PacketSize > ( m_i32RecvSize - i32ReadSize ) )	break;	// 아직 패킷을 다 받지 못했습니다.

		if( 0 == ::OnRecv( (unsigned char*)&m_ui8RecvBuffer[ (i32ReadSize+2) ], (int)(ui16PacketSize - 2), (int*)(&pui8ResultData[0]), &pui8ResultData[i32IntSize], &nResultSize) ) break;

		if( ui16PacketSize != (nResultSize+GSM_PACKETHEADERSIZE) )
		{
			g_pModuleLogFile->Write_Log_Recv(m_idx, "GSM Recv Fail" );
			break;	// 패킷 받기 실패
		}

		if( FALSE == m_pPacketLocker->Push( m_idx, pui8ResultData, (nResultSize+i32IntSize) ))
		{	// PacketLocket 가 풀입니다. 잠시 대기해야합니다.
			char szTemp[MAX_PATH];
			sprintf( szTemp, "Locker Overflow Idx:%d, Length:%d", m_idx, ui16PacketSize );
			g_pModuleLogFile->Write_Log_Recv( m_idx, szTemp  );
			break;
		}

		i32ReadSize += ui16PacketSize;
	}

	if( 0 == i32ReadSize )	return;

	m_i32RecvSize -= i32ReadSize;
	if( 0 != m_i32RecvSize )
	{
		memmove( m_ui8RecvBuffer, m_ui8RecvBuffer + i32ReadSize, m_i32RecvSize );
	}

	return; 
#endif
}

void CReceiverGSM::_OnDisconnecting(void)
{	
	::shutdown(m_socket, SD_BOTH);
	::closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	// 완성시키지 못한 잔여 packet을 지운다
	m_i32RecvSize = 0;
		
	m_state = STATE_WAITING;

	g_pModuleLogFile->Write_Log_Recv(m_idx, "Disconnected"  ); 

	return; 
}


//다른 쓰레드에서 호출됩니다. 
//로그 보낼때 주의 필요 
BOOL CReceiverGSM::SendGamePacket(i3NetworkPacket* pPacket)
{
	if (INVALID_SOCKET == m_socket)
	{
		char message[MAX_STRING_COUNT];
		sprintf(message, "[ERROR] CReceiverGSM::SendPacket() - Invalid Socket, packet(%d)", pPacket->GetProtocolID());
		//COMMON_LOG(message);
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
			//char message[MAX_STRING_COUNT];

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
				//sprintf(message, "CReceiverGSM::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message );
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
				//sprintf(message, "CReceiverGSM::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message );
			case WSAECONNABORTED:
				return FALSE;			
			case WSAEINPROGRESS:			
			case WSAEMSGSIZE:
			case WSAENOBUFS:
				//sprintf(message, "CReceiverGSM::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message );
			case WSAEWOULDBLOCK:	// 정상
				break;
			default:
				//sprintf(message, "CReceiverGSM::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message );
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

void CReceiverGSM::_RecvErrorHandling(void)
{
	char message[MAX_STRING_COUNT];
	INT32 nErrorCode = ::WSAGetLastError();

	switch(nErrorCode)
	{
	//정상
	case WSAEWOULDBLOCK		:	break;

	// 접속 끊어짐
	case WSANOTINITIALISED	:
	case WSAEFAULT			:
	case WSAEINVAL			:
	case WSAEISCONN			:
	case WSAENOTSOCK		:
	case WSAEOPNOTSUPP		:
	case WSAENETDOWN		:
	case WSAENETRESET		:
	case WSAESHUTDOWN		:
	case WSAETIMEDOUT		:
	case WSAECONNRESET		:
	case WSAEINTR			:
	case WSAECONNABORTED	:	
		sprintf(message, "CReceiverGSM::_RecvErrorHandling() - errorCode Dis(%d)", nErrorCode);
		g_pModuleLogFile->Write_Log_Recv(m_idx, message  );	
		m_state = STATE_DISCONNECTING;
		break;
	// 재시도
	case WSAEINPROGRESS:	
	case WSAEMSGSIZE:
		sprintf(message, "CReceiverGSM::_RecvErrorHandling() - errorCode Ret(%d)", nErrorCode);
		g_pModuleLogFile->Write_Log_Recv(m_idx, message  );
		break; 
	//알수 없음
	default:
		sprintf(message, "CReceiverGSM::_RecvErrorHandling() - errorCode Unk(%d)", nErrorCode);
		g_pModuleLogFile->Write_Log_Recv(m_idx, message  );	
		m_state = STATE_DISCONNECTING;
		break;
	}

	return; 
}

BOOL CReceiverGSM::IsWork(void)
{
	return INVALID_SOCKET != m_socket;
}

BOOL CReceiverGSM::IsDisconnecting(void)
{
	return STATE_DISCONNECTING == m_state;
}

BOOL CReceiverGSM::Disconnect(void)
{
	m_state = STATE_DISCONNECTING;

	return TRUE;
}
