#include "pch.h"
#include "ModuleReceiver.h"
#include "ModuleGateway.h"
#include "RingBuffer.h"

I3_CLASS_INSTANCE(CModuleReceiver, i3Thread);

CModuleReceiver::CModuleReceiver()
{
	m_hSocket			= INVALID_SOCKET;	
	m_IsRunning			= FALSE;
	m_i32dx				= -1;
	m_pRingReceiver		= NULL;
	m_i32RecvSize		= 0;
}

CModuleReceiver::~CModuleReceiver()
{
	OnDestroy();
}

BOOL CModuleReceiver::Create( UINT32 i32Idx )
{
	// Log 생성
	m_i32LogThreadIdx	= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )		return FALSE;

	m_hSocket		= INVALID_SOCKET;
	m_i32dx			= i32Idx;
	m_eState		= STATE_WAITING;

	char strBuf[ MAX_STRING_COUNT ];
	i3String::Format( strBuf, MAX_STRING_COUNT, "Receiver_%02d", m_i32dx );
	if (FALSE == i3Thread::Create( strBuf ))			return FALSE;

	return TRUE;
}

void CModuleReceiver::OnDestroy()
{
	m_IsRunning = FALSE;
	i3Thread::WaitForFinish();

	if( INVALID_SOCKET != m_hSocket )
	{
		::shutdown(m_hSocket, SD_BOTH);
		::closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

BOOL CModuleReceiver::SetSocket( SOCKET hSocket )
{
	if( INVALID_SOCKET	!= m_hSocket )	return FALSE;
	if( STATE_WAITING	!= m_eState )	return FALSE;

	m_hSocket	= hSocket;
	m_eState	= STATE_READING;

	return TRUE;
}

BOOL CModuleReceiver::SetRingBuffer( CRingBuffer* pRingReceiver )
{
	m_pRingReceiver	= pRingReceiver;
	return TRUE;
}

UINT32 CModuleReceiver::OnRunning(void* pUserData)
{
	m_IsRunning = TRUE;
	while( m_IsRunning )
	{
		switch( m_eState )
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
 
void CModuleReceiver::_OnReading(void)
{
	INT32 rv = ::recv( m_hSocket, (char*)&(m_ui8RecvBuffer[m_i32RecvSize]), PACKETBUFFERSIZE - m_i32RecvSize, 0);

	if( SOCKET_ERROR == rv )
	{
		_RecvErrorHandling();
		return;
	}
	
	if( 0 == rv )
	{
		//WRITE_LOG_RECV( m_idx, "CModuleReceiver::OnReading() - Disconnect Recv 0" );
		m_eState = STATE_DISCONNECTING;
		return;
	}

	m_i32RecvSize += rv;

	if( PACKETBUFFERSIZE < m_i32RecvSize )
	{	// 말이 안됩니다. 있을수 없는 일..
		//WRITE_LOG_RECV( m_idx, "CModuleReceiver::OnReading() - Disconnect Recv Buffer Over" );
		m_eState = STATE_DISCONNECTING;
		return;
	}

	INT32 i32ReadSize	= 0;
	UINT16 ui16PacketSize;
	while( ( m_i32RecvSize - i32ReadSize ) >= PACKETHEADERSIZE  )
	{
		ui16PacketSize = (UINT16)*((UINT16*)&m_ui8RecvBuffer[ i32ReadSize ])+PACKETHEADERSIZE;
		if( ui16PacketSize > PACKETBUFFERSIZE )
		{	// 패킷이 깨진경우입니다. 
			//WRITE_LOG_RECV(m_idx, "CModuleReceiver::OnReading() - Disconnect Pack Break");
			m_eState = STATE_DISCONNECTING;
			return;
		}
		if( ui16PacketSize > ( m_i32RecvSize - i32ReadSize ) )	break;	// 아직 패킷을 다 받지 못했습니다.

		void* pRingBuffer = m_pRingReceiver->PushPointer();
		if( NULL == pRingBuffer )
		{	// PacketLocket 가 풀입니다. 잠시 대기해야합니다.
			char szTemp[MAX_PATH];
			sprintf( szTemp, "Locker Overflow Idx:%d, Length:%d", m_i32dx, ui16PacketSize );
			//WRITE_LOG_RECV( m_idx, szTemp );
			break;
		}

		i3mem::Copy( pRingBuffer, &(m_ui8RecvBuffer[ i32ReadSize ]), ui16PacketSize );

		i32ReadSize += ui16PacketSize;
		m_pRingReceiver->PushPointerIdx();
	}

	if( 0 == i32ReadSize )	return;

	m_i32RecvSize -= i32ReadSize;
	if( 0 != m_i32RecvSize )
	{
		memmove( m_ui8RecvBuffer, m_ui8RecvBuffer + i32ReadSize, m_i32RecvSize );
	}

	return; 
}

void CModuleReceiver::_OnDisconnecting(void)
{	
	::shutdown(m_hSocket, SD_BOTH);
	::closesocket(m_hSocket);
	m_hSocket = INVALID_SOCKET;

	// 완성시키지 못한 잔여 packet을 지운다
	m_i32RecvSize = 0;
		
	m_eState = STATE_WAITING;

	//WRITE_LOG_RECV(m_idx, "Disconnected"); 

	return; 
}


//다른 쓰레드에서 호출됩니다. 
//로그 보낼때 주의 필요 
BOOL CModuleReceiver::SendPacket(i3NetworkPacket* pPacket)
{
	if (INVALID_SOCKET == m_hSocket)
	{
		char message[MAX_STRING_COUNT];
		sprintf(message, "[ERROR] CModuleReceiver::SendPacket() - Invalid Socket, packet(%d)", pPacket->GetProtocolID());
		//COMMON_LOG(message);
		return FALSE;
	}

	UINT32 nSend = 0;
	UINT32 nLength = pPacket->GetPacketSize();
	UINT8* pData = (UINT8*)pPacket->GetPacketBuffer();
	
	UINT32 rv;
	while (nSend < nLength)
	{
		rv = ::send(m_hSocket, (const char*)pData + nSend, nLength - nSend, 0);

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
				//sprintf(message, "CModuleReceiver::SendPacket() - errorCode(%d)", nErrorCode);
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
				//sprintf(message, "CModuleReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
			case WSAECONNABORTED:
				return FALSE;			
			case WSAEINPROGRESS:			
			case WSAEMSGSIZE:
			case WSAENOBUFS:
				//sprintf(message, "CModuleReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
			case WSAEWOULDBLOCK:	// 정상
				break;
			default:
				//sprintf(message, "CModuleReceiver::SendPacket() - errorCode(%d)", nErrorCode);
				//WRITE_LOG(m_idx, message);
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

void CModuleReceiver::_RecvErrorHandling(void)
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
		sprintf(message, "CModuleReceiver::_RecvErrorHandling() - errorCode Dis(%d)", nErrorCode);
		//WRITE_LOG_RECV(m_idx, message);	
		m_eState = STATE_DISCONNECTING;
		break;
	// 재시도
	case WSAEINPROGRESS:	
	case WSAEMSGSIZE:
		sprintf(message, "CModuleReceiver::_RecvErrorHandling() - errorCode Ret(%d)", nErrorCode);
		//WRITE_LOG_RECV(m_idx, message);
		break; 
	//알수 없음
	default:
		sprintf(message, "CModuleReceiver::_RecvErrorHandling() - errorCode Unk(%d)", nErrorCode);
		//WRITE_LOG_RECV(m_idx, message);	
		m_eState = STATE_DISCONNECTING;
		break;
	}

	return; 
}

BOOL CModuleReceiver::IsWork(void)
{
	return INVALID_SOCKET != m_hSocket;
}

BOOL CModuleReceiver::IsDisconnecting(void)
{
	return STATE_DISCONNECTING == m_eState;
}

BOOL CModuleReceiver::Disconnect(void)
{
	m_eState = STATE_DISCONNECTING;

	return TRUE;
}
