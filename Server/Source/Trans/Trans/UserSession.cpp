#include "pch.h"
#include "UserSession.h"

I3_CLASS_INSTANCE( CUserSession, i3ElementBase );

CUserSession::CUserSession()
{
	m_ppRing		= NULL;
	m_i32ServerIdx	= -1;
}

CUserSession::~CUserSession()
{
	Destroy();
}

BOOL CUserSession::Create( i3RingBuffer** ppRing, INT32* pLogIdx, SV_STATE_LOG* pStateLog )
{
	m_ppRing	= ppRing;
	m_pLogIdx	= pLogIdx;
	m_pStateLog	= pStateLog;

	return TRUE;
}

void CUserSession::Destroy()
{
}

BOOL CUserSession::OnConnect( SOCKET hSocket, struct sockaddr_in* pAddr, INT32 i32ServerIdx )
{
	BOOL i32Rv = i3NetworkSession::OnConnect( hSocket, pAddr );

	m_i32ServerIdx = i32ServerIdx;

	return i32Rv;
}

BOOL CUserSession::OnConnectInit()
{
	if( FALSE == i3NetworkSession::OnConnectInit() )	return FALSE;

	// 여기서 최초 접속 메시지
	i3NetworkPacket	StartPacket( PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS );
	SendPacketMessage( &StartPacket );

	return TRUE;
}

//
//
//워크, 메인에서 호출 가능 Auth는 사용하지 않음 
BOOL CUserSession::OnDisconnect(BOOL bForceMainThread)
{	
	//m_pRing->ResetBuffer();
	m_i32ServerIdx = -1;

	return i3NetworkSession::OnDisconnect( bForceMainThread );
}

/*----------------------------------------------------------
Name : 
Desc : Work Thread에서만 호출 가능 
----------------------------------------------------------*/
INT32 CUserSession::PacketParsing( char* pPacketBuffer, INT32 i32Size )
{
	IOCP_RING_PUSH* pPush	= (IOCP_RING_PUSH*)m_ppRing[ m_WorkThreadIdx ]->PushPointer();
	if( NULL == pPush )
	{	// 더 이상 공간이 부족 합니다.
		g_pLog->WriteLog( L"[WARRING] UserSession Ring Full" );
		return 0;
	}
	
	INT32 i32PacketSize		= (INT16)(*(INT16*)pPacketBuffer)+PACKETHEADERSIZE;
	if( i32PacketSize > i32Size )			return 0;			// 아직 사이즈를 다 받지 못했습니다.
	if( i32PacketSize < 0 )					return 0;
	if( PACKETBUFFERSIZE < i32PacketSize )	return 0;			// 데이터가 이상합니다.

	pPush->m_i32ServerIdx	= m_i32ServerIdx;
	i3mem::Copy( pPush->m_pBuffer, pPacketBuffer, i32PacketSize );
	m_ppRing[ m_WorkThreadIdx ]->PushPointerIdx();

	m_pStateLog[ m_WorkThreadIdx ].m_i32RecvCount++;
	m_pStateLog[ m_WorkThreadIdx ].m_i32RecvSize += i32PacketSize;

	return i32PacketSize;
}

BOOL CUserSession::SendPacketMessage( i3NetworkPacket * pPacket )
{
	if( i3NetworkSession::SendPacketMessage( pPacket ) )
	{
		//m_i32SendErrorCount = 0;
	}
	//else
	//{
	//	m_i32SendErrorCount++; 
	//	if(m_i32SendErrorCount > 1)
	//	{
	//		// 서동권
	//		// 이 부분의 경우 굉장히 많이 호출되는 부분이고 여러 스레드에서 호출될 수 있어, 
	//		// 스레드 인댁스를 매개변수로 받는거보다 이부분만 예전 방식으로 처리하는게 효율적이어서 예정 방식으로 둠
	//		//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT , 0, KILL_REASON_SENDPACKET_ERROR, "SendPacket Error" );
	//		m_ui32ATIsKill		= g_pContextMain->GetServerTime();
	//		m_ui32KillReason	= KILL_REASON_SENDPACKET_ERROR;
	//	}
	//	return FALSE;
	//}

	return TRUE; 
}