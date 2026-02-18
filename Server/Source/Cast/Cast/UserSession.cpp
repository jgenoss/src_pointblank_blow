#include "pch.h"
#include "UserSession.h"
#include "UserManager.h"

I3_CLASS_INSTANCE( CUserSession, i3ElementBase );

CUserSession::CUserSession()
{
	m_ppRing		= NULL;
	m_pManager		= NULL;

	m_eServerType	= ASC_SERVER_NONE;
	m_i32ServerIdx	= -1;
}

CUserSession::~CUserSession()
{
	Destroy();
}

BOOL CUserSession::Create( i3RingBuffer** ppRing, SV_STATE_LOG* pStateLog, CUserSessionManager* pManager )
{
	m_ppRing		= ppRing;
	m_pStateLog		= pStateLog;
	m_pManager		= pManager;

	return TRUE;
}

void CUserSession::Destroy()
{
}

BOOL CUserSession::OnConnect( SOCKET hSocket, struct sockaddr_in* pAddr )
{
	BOOL i32Rv = i3NetworkSession::OnConnect( hSocket, pAddr );
		
	m_i32ServerIdx = -1;

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
	// 배틀 서버일 경우 더이상 방 생성을 못하게 막습니다.
	if( ASC_SERVER_BATTLE == m_eServerType )
	{
		g_pConfig->SetAllowRoomCreate( m_i32ServerIdx, FALSE );
	}

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
	INT32 i32PacketSize		= (INT16)(*(INT16*)pPacketBuffer)+PACKETHEADERSIZE;
	if( i32PacketSize > i32Size )			return 0;			// 아직 사이즈를 다 받지 못했습니다.
	if (i32PacketSize	< 0)				return 0;			// 데이터가 이상합니다.
	if( PACKETBUFFERSIZE < i32PacketSize )	return 0;			// 데이터가 이상합니다.

	// 기본 패킷 일 경우 여기서 받아 내가 누군지 확인.
	// Manger 에 등록 및 확인. 하지만 여기서 크리 안쓰는 방법은? 클라에서 자신의 Idx 를 받는거. 받아서 어따 등록하지?
	i3NetworkPacket Packet;
	Packet.CopyToBuffer( pPacketBuffer, i32PacketSize );
		
	switch( Packet.GetProtocolID() )
	{
	case PROTOCOL_ASC_CONFIRMATION_REQ:
		{
			UINT8 ui8ServerType;
			Packet.ReadData( &ui8ServerType,	sizeof(UINT8) );

			ASC_SERVER_TYPE eServerType = (ASC_SERVER_TYPE)ui8ServerType;

			// 붙을 수 없는 IP 입니다.
			INT32 i32Idx	= g_pConfig->GetServerIdx( eServerType, m_ConIp );
			if( -1 == i32Idx )
			{
				g_pLog->WriteLog(L"[ERROR] GetServerIdx Fail / Type : %d, IP : %d ", eServerType, m_ConIp);
				OnDisconnect( FALSE );
				return 0;
			}
			if( FALSE == m_pManager->SetServerIdx( eServerType, i32Idx, m_SessionIdx ) )
			{
				g_pLog->WriteLog(L"[ERROR] SetServerIdx Fail / Type : %d / IP : %d / Idx : %d / Sidx : %d", eServerType, m_ConIp, i32Idx, m_SessionIdx);
				OnDisconnect( FALSE );
				return 0;
			}

			m_eServerType	= eServerType;
			m_i32ServerIdx	= i32Idx;

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_ACK );
			
			if (ASC_SERVER_BATTLE == m_eServerType)
			{
				MAP_STAGE_DATA	aMapLoad[STAGE_UID_MAX];
				g_pConfig->m_MapLoadManager.CopyToPacket(m_i32ServerIdx, aMapLoad, sizeof(aMapLoad));
				SendPacket.WriteData( aMapLoad, sizeof(MAP_STAGE_DATA) * STAGE_UID_MAX );
			}
			
			SendPacketMessage( &SendPacket);
		}
		break;
	case PROTOCOL_ASC_SERVER_LOADING_RESULT:
		{
			g_pConfig->SetAllowRoomCreate( m_i32ServerIdx, TRUE );
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_REQ:
		{
			i3NetworkPacket SendPacket( PROTOCOL_BASE_HEART_BIT_ACK );
			SendPacketMessage( &SendPacket);
		}
		break;
	case PROTOCOL_ASC_BATTLE_ROOM_COUNT_REQ:
		{			
			i3NetworkPacket SendPacket(PROTOCOL_ASC_BATTLE_ROOM_COUNT_ACK);
			SendPacketMessage( &SendPacket);
		}
		//break;		<-- 방개수 일치화를 위해 Task Thread 로 넘깁니다. 주석 맞음
	case PROTOCOL_BATTLE_SERVER_CREATE_REQ:
	case PROTOCOL_BATTLE_SERVER_DESTROY_REQ:
		{
			IOCP_RING_PUSH* pPush	= (IOCP_RING_PUSH*)m_ppRing[ m_WorkThreadIdx ]->PushPointer();
			if( NULL == pPush )
			{	// 더 이상 공간이 부족 합니다.
				g_pLog->WriteLog( L"[WARRING] UserSession Ring Full" );
				return 0;
			}

			pPush->m_i32ServerIdx	= m_i32ServerIdx;
			i3mem::Copy( pPush->m_pBuffer, pPacketBuffer, i32PacketSize );
			m_ppRing[ m_WorkThreadIdx ]->PushPointerIdx();
		}
		break;
	case PROTOCOL_BATTLE_SERVER_CREATE_ACK:
		{
			P_BATTLE_ROOM_CREATE_ACK* pPacket = (P_BATTLE_ROOM_CREATE_ACK*)Packet.GetFieldBuffer();

			m_pManager->SendPacketMessageGame( pPacket->_RoomInfo._cGame, &Packet );
		}
		break;
	default:
		{
			BATTLE_ROOM_INFO RoomInfo;

			Packet.ReadData( &RoomInfo,		sizeof(BATTLE_ROOM_INFO) );

			switch( m_eServerType )
			{
			case ASC_SERVER_GAME:		m_pManager->SendPacketMessageBattle( RoomInfo._cBattle, &Packet );	break;
			case ASC_SERVER_BATTLE:		m_pManager->SendPacketMessageGame( RoomInfo._cGame, &Packet );		break;
			}
		}
		break;
	}

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

BOOL CUserSession::CheckSameServer( ASC_SERVER_TYPE eServerType, INT32 i32Idx )
{
	if( eServerType != m_eServerType )	return FALSE;
	if( i32Idx != m_i32ServerIdx )		return FALSE;

	return TRUE;
}