#include "pch.h"
#include "WebManager.h"
#include "WebSession.h"

I3_CLASS_INSTANCE(CWebSession, i3NetworkClientSocket);

CWebSession::CWebSession()
{
	m_ui32RecvTime	= 0;
	m_ui32WorkTime	= 0;
}

CWebSession::~CWebSession()
{
	OnDestroy();
}

void CWebSession::OnCreate(SOCKET Socket)
{
	i3NetworkClientSocket::SetSocket( Socket );

	m_ui32RecvTime = i3ThreadTimer::GetServerTime();
	m_ui32WorkTime = 0;

	g_pLog->WriteLog( L"Session Connect.");
	return;
}

void CWebSession::OnDestroy(void)
{
	i3NetworkClientSocket::OnDestroy();
}

/*----------------------------------------------------------
Name : 
Desc : Work Thread에서만 호출 가능 
----------------------------------------------------------*/
void CWebSession::PacketParsing(i3NetworkPacket * pPacket)
{
	char strLog[LOG_STRING_LEN];
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();
	switch( ProtocolID )
	{
	case PROTOCOL_ASC_WEB_GM_KICK_REQ	:
		{
			INT8	i8Count;
			INT64	ai64UID[WEB_GM_KICK_MAX_COUNT];
			pPacket->ReadData( &i8Count,	sizeof(INT8) );
			pPacket->ReadData( ai64UID,		sizeof(INT64)*i8Count );

			g_pWebSessionManager->SendKickUserWebReq( this, m_i32Idx, i8Count, ai64UID );

			i3String::Format( strLog, LOG_STRING_LEN, "[%d] PROTOCOL_ASC_WEB_GM_KICK_REQ (UID: %I64d)", m_i32Idx, ai64UID);
			I3TRACE( strLog ); I3TRACE("\n");
		}
		break;
	case PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ	:
		{
			INT64 i64UID;
			pPacket->ReadData( &i64UID,		sizeof(INT64) );

			g_pWebSessionManager->SendUserInfoLocationWebReq( this, m_i32Idx, i64UID );

			i3String::Format( strLog, LOG_STRING_LEN, "[%d] PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ (UID: %I64d)", m_i32Idx, i64UID);
			I3TRACE( strLog ); I3TRACE("\n");
		}
		break;
	case PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_REQ	:
		{
			UINT32 ui32CID;
			pPacket->ReadData( &ui32CID,		sizeof(UINT32) );

			g_pWebSessionManager->SendClanInfoBattleWebReq( this, m_i32Idx, ui32CID );

			i3String::Format( strLog, LOG_STRING_LEN, "[%d] PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_REQ (CID: %d)", m_i32Idx, ui32CID);
			I3TRACE( strLog ); I3TRACE("\n");
		}
		break;
	case PROTOCOL_ASC_WEB_NOTICE_REQ			:
		{
			INT8	i8Type; 
			UINT16	ui16Size; 
			TTCHAR	strNoticeMessage[LOG_STRING_LEN];
			INT32	i32RGB;
			
			pPacket->ReadData( &i8Type,				sizeof(INT8) );
			pPacket->ReadData( &ui16Size,			sizeof(UINT16) );		if( ui16Size > (LOG_STRING_LEN-2) ) ui16Size = LOG_STRING_LEN-2;
			pPacket->ReadData( strNoticeMessage,	sizeof(TTCHAR) * ui16Size );
			pPacket->ReadData(&i32RGB,				sizeof(INT32));
			strNoticeMessage[ ui16Size ] = '\0';

			// 실제 Notice를 보내기 전에 로그를 먼저 기록합니다.
			g_pLog->WriteLog(L"WebSession ASC Web Notice REQ Type : %d, Notice Message %s, RGB %d", i8Type, strNoticeMessage, i32RGB);

			// 게임서버가 '\0' 값을 직접 넣기 때문에 순수 문자열 내용만 보냅니다.
			g_pWebSessionManager->SendNoticeWebReq(i8Type, strNoticeMessage, ui16Size, i32RGB);
		}
		break;
	default		:
		{
			i3String::Format( strLog, LOG_STRING_LEN, "Unknown Packet - %d", ProtocolID );
			I3TRACE( strLog );
		}
	}

	SetRecvTime( i3ThreadTimer::GetServerTime() );

	return;
}
