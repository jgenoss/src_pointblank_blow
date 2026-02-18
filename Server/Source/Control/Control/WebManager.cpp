#include "pch.h"
#include "WebManager.h"
#include "WebSession.h"
#include "UserManager.h"

CWebSessionManager * g_pWebSessionManager = NULL;

I3_CLASS_INSTANCE( CWebSessionManager, i3ElementBase);

CWebSessionManager::CWebSessionManager()
{
}

CWebSessionManager::~CWebSessionManager()
{
	OnDestroy();
}

BOOL CWebSessionManager::OnCreate(void)
{
	//1. Make TCP Socket 
	m_timeout.tv_sec  = 0;
	m_timeout.tv_usec = 5;
	if(!m_AcceptSocket.OnCreate( g_pControl->m_ui32IP, g_pControl->m_ui16WebPort) )
	{
		return FALSE;
	}

	//2. Make Server Session
	CWebSession * pWebSession; 
	for(UINT32 i = 0; i < g_pControl->m_ui16WebSessionCount; i++)
	{
		pWebSession = CWebSession::NewObject();
		pWebSession->m_i32Idx = i;
		m_SessionList.Add(pWebSession);
	}

	return TRUE;
}

BOOL CWebSessionManager::OnDestroy(void)
{
	m_AcceptSocket.OnDestroy(); 

	CWebSession * pWebSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++) 
	{		
		pWebSession = (CWebSession*)m_SessionList.GetItem(i);
		I3_SAFE_RELEASE(pWebSession);
	}

	return TRUE;
}

void CWebSessionManager::OnUpdate(void)
{
	//1. connect 
	Accept();

	//2. Update Server
	Update();
}

void CWebSessionManager::Accept()
{
	INT32	i32AddrSize;
	SOCKET	SocketAccept;
	struct	sockaddr_in FromAddr;
	i32AddrSize = sizeof(sockaddr_in);
	SocketAccept = m_AcceptSocket.AcceptConnection( &m_timeout, &FromAddr, &i32AddrSize );

	if(SocketAccept != INVALID_SOCKET)
	{
		wchar_t wstrIP[ MAX_PATH ];
		i3Net::GetIPToSockW( &FromAddr, wstrIP );
		
		CWebSession * pNewSession = NULL;
		for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
		{
			pNewSession = (CWebSession*)m_SessionList.GetItem(i);
			if( !pNewSession->GetIsActive()) break;
			pNewSession = NULL;
		}

		if(pNewSession == NULL)
		{
			::shutdown(SocketAccept, SD_BOTH);
			::closesocket(SocketAccept);
		}
		else
		{
			pNewSession->OnCreate(SocketAccept);

			g_pLog->WriteLog( L"CWebSessionManager::Accept IP : %s", wstrIP );
		}
	}
}

void CWebSessionManager::Update()
{
	CWebSession * pSession;
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pSession = (CWebSession*)m_SessionList.GetItem(i);
		if( !pSession->GetIsActive())	continue;

		pSession->SelectEvent();

		// Timeout Check
		if( pSession->GetWorkTime() == 0	&&
			(pSession->GetRecvTime() + WEB_SESSION_RECV_TIME_OUT) < i3ThreadTimer::GetServerTime() )
		{
			g_pLog->WriteLog( L"[%d]Kill Web Session : RecvTimeOut", pSession->m_i32Idx );
			pSession->OnDestroy();
		}

		if( pSession->GetWorkTime() > 0		&&
			(pSession->GetWorkTime() + WEB_SESSION_WORK_TIME_OUT) < i3ThreadTimer::GetServerTime() )
		{
			g_pLog->WriteLog( L"[%d]Kill Web Session : WorkTimeOut", pSession->m_i32Idx );
			pSession->OnDestroy();
		}
	}
}

void CWebSessionManager::SendKickUserWebReq( CWebSession *pSession, INT32 i32WebSessionIdx, INT8 i8Count, INT64 *pi64UID )
{
	pSession->SetWorkTime( i3ThreadTimer::GetServerTime() );
	g_pSessionManager->SendKickUserWebReq( i32WebSessionIdx, i8Count, pi64UID );
}

void CWebSessionManager::RecvKickUserWebAck( INT32 i32WebSessionIdx, INT32 i32Rv )
{
	CWebSession * pSession = (CWebSession*)m_SessionList.GetItem(i32WebSessionIdx);

	i3NetworkPacket AckPacket( PROTOCOL_ASC_WEB_GM_KICK_ACK );
	AckPacket.WriteData( &i32Rv,	sizeof(INT32) );
	pSession->SendPacketMessage( &AckPacket );

	pSession->SetWorkTime( 0 );
}

void CWebSessionManager::SendUserInfoLocationWebReq(CWebSession *pSession, INT32 i32WebSessionIdx, INT64 i64UID)
{
	pSession->SetWorkTime( i3ThreadTimer::GetServerTime() );
	g_pSessionManager->SendUserInfoLocationWebReq( i32WebSessionIdx, i64UID );
}

void CWebSessionManager::RecvUserInfoLocationWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8ServerIdx, INT8 i8ChannelNum, INT32 i32RoomIdx )
{
	CWebSession * pSession = (CWebSession*)m_SessionList.GetItem(i32WebSessionIdx);

	i3NetworkPacket AckPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK );
	AckPacket.WriteData( &i32Rv,			sizeof(INT32) );
	AckPacket.WriteData( &i8ServerIdx,		sizeof(INT8)  );
	AckPacket.WriteData( &i8ChannelNum,		sizeof(INT8)  );
	AckPacket.WriteData( &i32RoomIdx,		sizeof(INT32) );
	pSession->SendPacketMessage( &AckPacket );

	pSession->SetWorkTime( 0 );

	char szLog[LOG_STRING_LEN];
	i3String::Format( szLog, LOG_STRING_LEN, "[%d] PROTOCOL_ASC_WEB_USER _INFO_LOCATION_ACK (Rv: %d, ServerIdx:%d, ChannelNum:%d, RoomIdx:%d)", pSession->m_i32Idx, i32Rv, i8ServerIdx, i8ChannelNum, i32RoomIdx);
	I3TRACE( szLog ); I3TRACE("\n");
}

void CWebSessionManager::SendClanInfoBattleWebReq(CWebSession *pSession, INT32 i32WebSessionIdx, UINT32 ui32CID)
{
	pSession->SetWorkTime( i3ThreadTimer::GetServerTime() );
	g_pSessionManager->SendClanInfoBattleWebReq( i32WebSessionIdx, ui32CID );
}

void CWebSessionManager::SendNoticeWebReq( UINT8 i8Type, TTCHAR* pstrMessage, UINT16 ui16Size, INT32 i32RGB )
{
	g_pSessionManager->SendNoticeMessage( i8Type, pstrMessage, ui16Size, i32RGB);
}

void CWebSessionManager::RecvClanInfoBattleWebAck(INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8Count)
{
	CWebSession * pSession = (CWebSession*)m_SessionList.GetItem(i32WebSessionIdx);

	i3NetworkPacket AckPacket( PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_ACK );
	AckPacket.WriteData( &i32Rv,	sizeof(INT32) );
	AckPacket.WriteData( &i8Count,	sizeof(INT8)  );
	pSession->SendPacketMessage( &AckPacket );

	pSession->SetWorkTime( 0 );

	char szLog[LOG_STRING_LEN];
	i3String::Format( szLog, LOG_STRING_LEN, "[%d] PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_ACK (Rv: %d, Count:%d)", pSession->m_i32Idx, i32Rv, i8Count);
	I3TRACE( szLog ); I3TRACE("\n");
}
