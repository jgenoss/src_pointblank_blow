#include "pch.h"
#include "Receiver.h"
#include "PacketLocker.h"
//#include "LogFile.h"
#include "Gateway.h"


I3_CLASS_INSTANCE(CReceiver, i3Thread);

CReceiver::CReceiver()
{
	m_socket			= INVALID_SOCKET;	
	m_bIsRunning		= FALSE;
	m_i32idx			= -1;
	m_pPacketLocker		= NULL;
}

CReceiver::~CReceiver()
{
	OnDestroy();
}

BOOL CReceiver::Create( UINT32 ui32idx, CPacketLocker* pPacketLocker )
{
	I3ASSERT(NULL == m_pPacketLocker);
	I3ASSERT(-1 == m_i32idx);
		
	char strBuf[MAX_STRING_COUNT];
	i3String::Format(strBuf, MAX_STRING_COUNT, "Receiver_%02d", ui32idx);
		
	m_state = STATE_WAITING;
	m_pPacketLocker = pPacketLocker;
	m_socket = INVALID_SOCKET;
	m_i32idx = ui32idx;

	if (FALSE == i3Thread::Create(strBuf))	return FALSE;
	
	return TRUE;
}

void CReceiver::OnDestroy()
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	if( INVALID_SOCKET != m_socket )
	{
		::shutdown(m_socket, SD_BOTH);
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

BOOL CReceiver::SetSocket(SOCKET socket,BOOL bI3Packet)
{	
	if( INVALID_SOCKET	!= m_socket )	return FALSE;
	if( STATE_WAITING	!= m_state )	return FALSE;

	m_socket	= socket;
	m_state		= STATE_READING;

	return TRUE;
}

UINT32 CReceiver::OnRunning(void* pUserData)
{	
	wchar_t wstrReceiverName[MAX_STRING_COUNT];
	i3String::Format(wstrReceiverName, DB_STRING_LENGTH, L"RECEIVER%02d", m_i32idx);

	m_i32LogIdx = g_pLog->AddLogThread(wstrReceiverName, 0, m_ThreadID);

	m_bIsRunning = TRUE;
	while( m_bIsRunning )
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

void CReceiver::_OnReading(void)
{
	INT32 i32Rv = ::recv(m_socket, (char*)&(m_ui8RecvBuffer[m_i32RecvSize]), PACKETBUFFERSIZE - m_i32RecvSize, 0);

	if( SOCKET_ERROR == i32Rv )
	{
		_RecvErrorHandling();
		return;
	}
	
	if( 0 == i32Rv )
	{
		g_pLog->WriteLog( L"CReceiver::OnReading() - Disconnect Recv 0" );
		m_state = STATE_DISCONNECTING;
		return;
	}

	m_i32RecvSize += i32Rv;

	if( PACKETBUFFERSIZE < m_i32RecvSize )
	{	// 말이 안됩니다. 있을수 없는 일..
		g_pLog->WriteLog( L"CReceiver::OnReading() - Disconnect Recv Buffer Over" );
		m_state = STATE_DISCONNECTING;
		return;
	}

	INT32 i32ReadSize	= 0;
	UINT16 ui16PacketSize;
	while( ( m_i32RecvSize - i32ReadSize ) >= PACKETHEADERSIZE  )
	{
		ui16PacketSize = (UINT16)*((UINT16*)&m_ui8RecvBuffer[ i32ReadSize ])+PACKETHEADERSIZE;
		if( ui16PacketSize > PACKETBUFFERSIZE )
		{	// 패킷이 깨진경우입니다. 
			g_pLog->WriteLog( L"CReceiver::OnReading() - Disconnect Pack Break");
			m_state = STATE_DISCONNECTING;
			return;
			//char szTemp[MAX_PATH];
			//sprintf( szTemp, "OnPacketProcessing Push Over " );
			// m_pPacketLocker
			//g_pConfig->Write_Log_Recv(m_idx, szTemp );
		}
		if( ui16PacketSize > ( m_i32RecvSize - i32ReadSize ) )	break;	// 아직 패킷을 다 받지 못했습니다.

		if( FALSE == m_pPacketLocker->Push( m_i32idx, &(m_ui8RecvBuffer[ i32ReadSize ]), ui16PacketSize ))
		{	// PacketLocket 가 풀입니다. 잠시 대기해야합니다.
			g_pLog->WriteLog( L"Locker Overflow Idx:%d, Length:%d", m_i32idx, ui16PacketSize );
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
}

void CReceiver::_OnDisconnecting(void)
{	
	::shutdown(m_socket, SD_BOTH);
	::closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	// 완성시키지 못한 잔여 packet을 지운다
	m_i32RecvSize = 0;
		
	m_state = STATE_WAITING;

	g_pLog->WriteLog( L"Disconnected"); 

	return; 
}

BOOL CReceiver::SendPacket(i3NetworkPacket* pPacket)
{
	if (INVALID_SOCKET == m_socket)
	{	

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
			INT32 i32ErrorCode = ::WSAGetLastError();
			
			switch(i32ErrorCode)
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
			case WSAECONNABORTED:
			case WSAEINTR:
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
			ui32Send += ui32Rv;
		}
	}	

	return TRUE; 
}

void CReceiver::_RecvErrorHandling(void)
{
	char strMessage[MAX_STRING_COUNT];
	INT32 i32ErrorCode = ::WSAGetLastError();

	switch(i32ErrorCode)
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
		i3String::Format(strMessage, MAX_STRING_COUNT, "CReceiver::_RecvErrorHandling() - errorCode Dis(%d)", i32ErrorCode);
		//g_pLogFile->Write_Log_Recv(m_idx, message);	
		m_state = STATE_DISCONNECTING;
		break;
	// 재시도
	case WSAEINPROGRESS:
	case WSAEMSGSIZE:
		i3String::Format(strMessage, MAX_STRING_COUNT, "CReceiver::_RecvErrorHandling() - errorCode Ret(%d)", i32ErrorCode);
		//g_pLogFile->Write_Log_Recv(m_idx, message);
		break; 
	//알수 없음
	default:
		i3String::Format(strMessage, MAX_STRING_COUNT, "CReceiver::_RecvErrorHandling() - errorCode Unk(%d)", i32ErrorCode);
		//g_pLogFile->Write_Log_Recv(m_idx, message);
		m_state = STATE_DISCONNECTING;
		break;
	}

	return;
}

BOOL CReceiver::IsWork(void)
{
	return INVALID_SOCKET != m_socket;
}

BOOL CReceiver::IsDisconnecting(void)
{
	return STATE_DISCONNECTING == m_state;
}

BOOL CReceiver::Disconnect(void)
{
	m_state = STATE_DISCONNECTING;

	return TRUE;
}

RECEIVER_TYPE CReceiver::GetReceiverType(UINT32 ui32idx)
{
	if (ui32idx < g_pConfig->m_ui32ServerCount)
	{
		return GAME_SERVER;
	}

	return UNKOWN_SERVER;
}
