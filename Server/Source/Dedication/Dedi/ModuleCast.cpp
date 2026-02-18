#include "pch.h"
#include "ModuleCast.h"
#include "ModuleCastSocket.h"
#include "GroupMaker.h"
#include "iocpServer.h"
#include "UdpBufferPool.h"
//#include "UdpBuffer.h"

I3_CLASS_INSTANCE( CModuleCast, i3Thread );

CModuleCast *	g_pModuleCast		= NULL; 

//////////////////////////////////////////////////////////////
//CAST
#define CAST_MAX_SEND_IDX_COUNT				2000000000		
#define CAST_MAX_ZERO_COUNT					1900000000
#define CAST_RECONNECT_TIME					10
#define CAST_SERVER_HEART_BIT				60
#define CAST_SERVER_SERVERINFO				5
#define CAST_SERVER_HEART_BIT_REQ_TIME		70
#define CAST_SEND_BUFFER_COUNT				500

#define CAST_CHECKVALID_IDX		if( !m_pCastSocket->IsActive())								return FALSE; \
	CAST_NET_BUFFER	* pNetBuffer = NULL; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sCastMainMessage;					break; \
	case	MODULE_CAST_THREAD_IDX		: pNetBuffer = &m_sCastMessage;						break; \
	default								: pNetBuffer = &m_pCastUserMessage[i32WorkIdx];		break; \
}\
	if(pNetBuffer->_i32WritePos > CAST_MAX_ZERO_COUNT)\
{\
	if( pNetBuffer->_i32WritePos == pNetBuffer->_i32ReadPos )\
{\
	pNetBuffer->_i32WritePos = 0;\
	pNetBuffer->_i32ReadPos	= 0;\
}\
}\
	if( pNetBuffer->_i32ReadPos > CAST_MAX_SEND_IDX_COUNT )\
{\
	DSERROR("ModuleCast WriteBuffer FULL\n" );\
	return FALSE; \
}\
	if( (pNetBuffer->_i32WritePos - pNetBuffer->_i32ReadPos) >= (CAST_SEND_BUFFER_COUNT -1) )\
{\
	DSERROR("ModuleCast WriteBuffer Overflow\n");\
	return FALSE; \
}\
INT32 i32WriteIdx = pNetBuffer->_i32WritePos % CAST_SEND_BUFFER_COUNT; \


CModuleCast::CModuleCast(void)
{
	m_bRunning						= FALSE;
	m_ui32ThreadCount				= 0;
  	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Cast 
	m_pCastSocket				= NULL;
	m_ui32CastConnectTime		= 0;
	m_pCastUserMessage		= NULL;
	i3mem::FillZero( &m_sCastMainMessage,	sizeof( CAST_NET_BUFFER ) );
	i3mem::FillZero( &m_sCastMessage,		sizeof( CAST_NET_BUFFER ) );

	m_ui32UpdateCheckStartIdx		= 0;
	m_ui32UpdateCheck				= 0;

	for( INT32 i = 0; i < WARNING_TYPE_COUNT; i++)
	{
		for( INT32 j = 0; j < WARNING_LEVEL_COUNT; j++ )
		{
			m_ui32WriteTime[i][j] = 0;
			m_ui32ReadTime[i][j] = 0;
		}
	}

}

CModuleCast::~CModuleCast(void)
{
	OnDestroy(); 
}

BOOL CModuleCast::OnCreate( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ThreadCount )
{
	m_ui32IP	= ui32IP;
	m_ui16Port	= ui16Port;

	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	// Buffer Create
	// Main
	m_sCastMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CAST_SEND_BUFFER_COUNT);
	for( UINT32 i = 0 ; i < CAST_SEND_BUFFER_COUNT ; i++ )
	{
		m_sCastMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}
	m_sCastMessage._ppNetMessage		= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CAST_SEND_BUFFER_COUNT);
	for( UINT32 i = 0; i < CAST_SEND_BUFFER_COUNT ; i++ )
	{
		m_sCastMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}

	// UserSession
	m_pCastUserMessage = (CAST_NET_BUFFER*) i3MemAlloc(sizeof(CAST_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
	{
		m_pCastUserMessage[i]._i32ReadPos	= 0;
		m_pCastUserMessage[i]._i32WritePos	= 0;
		m_pCastUserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CAST_SEND_BUFFER_COUNT); 
		for( UINT32 j = 0 ; j < CAST_SEND_BUFFER_COUNT ; j++ )
		{
			m_pCastUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Socket
	m_pCastSocket = CModuleCastSocket::NewObject();
	if( FALSE == m_pCastSocket->OnCreate( m_ui32IP, m_ui16Port ) )	return SERVER_ERROR_CAST;

	m_ui32CastConnectTime			= i3ThreadTimer::GetServerTime();

	m_ui32UpdateCheck				= i3ThreadTimer::GetServerTime();
	m_ui32UpdateCheckStartIdx		= 0;

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleContextA_Cast", 0, 4096, NULL, PRIORITY_HIGH) )	return SERVER_ERROR_CAST;
	
#ifdef NEW_LOG
	//Create Private Log Buffer
	I3TRACE("CModuleCast=%d\n", m_ThreadID );
	if( NULL == g_pLogFile->AllocBuffer( m_ThreadID ) ) return SERVER_ERROR_CAST;
#endif

	return EVENT_ERROR_SUCCESS;
}

void CModuleCast::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pCastSocket );

	if( m_sCastMainMessage._ppNetMessage )
	{
		for(UINT32 i = 0; i < CAST_SEND_BUFFER_COUNT; i++)
		{
			I3_SAFE_RELEASE( m_sCastMainMessage._ppNetMessage[i] ); 
		}

		I3MEM_SAFE_FREE(m_sCastMainMessage._ppNetMessage);
	}

	if( m_sCastMessage._ppNetMessage )
	{
		for(UINT32 i = 0; i < CAST_SEND_BUFFER_COUNT; i++)
		{
			I3_SAFE_RELEASE( m_sCastMessage._ppNetMessage[i] );
		}

		I3MEM_SAFE_FREE(m_sCastMessage._ppNetMessage);
	}

	if( m_pCastUserMessage )
	{
		for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
		{
			if( m_pCastUserMessage[i]._ppNetMessage )
			{
				for( UINT32 j = 0 ; j < CAST_SEND_BUFFER_COUNT ; j++)
				{
					I3_SAFE_RELEASE(m_pCastUserMessage[i]._ppNetMessage[j] );
				}
				I3MEM_SAFE_FREE(m_pCastUserMessage[i]._ppNetMessage); 
			}
		}
		I3MEM_SAFE_FREE( m_pCastUserMessage );
	}

	m_pdhPerfmon.Destroy();

	return; 
}

INT32 CModuleCast::InitSystemLog()
{
	if( m_pdhPerfmon.Create( ) == TRUE )
	{
		return EVENT_ERROR_SUCCESS;
	}
	else
	{
		return SERVER_ERROR_SYSTEMLOG;
	}
}

UINT32 CModuleCast::OnRunning( void * pUserData)
{
	m_bRunning		= TRUE;

	while( m_bRunning )
	{
		if( 0 < _CastUpdate() )	continue;

		// 한일이 없으면 1ms 휴식.
		::WaitForSingleObject( m_Handle, 1);
	}

	return 0; 
}

INT32 CModuleCast::_CastConnect()
{
	if( m_ui32CastConnectTime + CAST_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32CastConnectTime = i3ThreadTimer::GetServerTime();

	m_pCastSocket->OnDestroy(); 
	if( m_pCastSocket->OnCreate( m_ui32IP, m_ui16Port ) )
	{
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pCastSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleCast::_CastUpdate(void) 
{
	INT32	iSendCompleteCount = 0; 
	INT32	i32SendCount	= 0;
	INT32	i32ReadIdx		= 0;
	INT32	i32SendBytes	= 0;

	UINT32	i;

	//-------------------------------------------------------------------------------
	//1. Connect Check 
	if( FALSE == m_pCastSocket->IsConnect())
	{
		INT32 i32Rv = _CastConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			DSWARNING("ReConnect CastServer 1 Success\n");
			SetWarningInfo( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			DSERROR("ReConnect CastServer 1 Fail\n");
			SetWarningInfo( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK );
		}
		return iSendCompleteCount;
	}

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pCastSocket->IsActive() )
	{
		////2-1 Main Thread 
		i32SendCount = m_sCastMainMessage._i32WritePos - m_sCastMainMessage._i32ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sCastMainMessage._i32ReadPos % CAST_SEND_BUFFER_COUNT; 
			
			i32SendBytes = m_pCastSocket->SendPacketMessage( m_sCastMainMessage._ppNetMessage[ i32ReadIdx ] ); 
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleCast::_ControlUpdate:SYSTEM_PACKET] SendPacketMessage Error(%d), protcol=%d\n", GetLastError(), m_sCastMainMessage._ppNetMessage[ i32ReadIdx ]->GetProtocolID() );
			}
			// 무한루프로 빠지지 않도록 실패시에도 카운터 증가 시킴			
			m_sCastMainMessage._i32ReadPos++;
			iSendCompleteCount++;
		}

		////2-1-2 cast Thread
		i32SendCount = m_sCastMessage._i32WritePos - m_sCastMessage._i32ReadPos;
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sCastMessage._i32ReadPos % CAST_SEND_BUFFER_COUNT;

			i32SendBytes = m_pCastSocket->SendPacketMessage( m_sCastMessage._ppNetMessage[ i32ReadIdx ] );
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleCast::_ControlUpdate:SYSTEM_PACKET] SendPacketMessage Error(%d), protcol=%d\n", GetLastError(), m_sCastMessage._ppNetMessage[ i32ReadIdx ]->GetProtocolID() );
			}
			// 무한루프로 빠지지 않도록 실패시에도 카운터 증가 시킴			
			m_sCastMessage._i32ReadPos++;
			iSendCompleteCount++;
		}
		
		//2-2 User Thread
		for( i = 0; i < m_ui32ThreadCount ; i++ )
		{
			CAST_NET_BUFFER	* pUserMessage = &m_pCastUserMessage[ i ]; 
			i32SendCount = pUserMessage->_i32WritePos - pUserMessage->_i32ReadPos;
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_i32ReadPos % CAST_SEND_BUFFER_COUNT; 
				i32SendBytes = m_pCastSocket->SendPacketMessage( pUserMessage->_ppNetMessage[ i32ReadIdx ]); 
				if( i32SendBytes <= 0)
				{
					DSERROR( "[CModuleCast::_ControlUpdate:TASK_PACKET] SendPacketMessage Error(%d), protocol=%d\n", GetLastError(), pUserMessage->_ppNetMessage[ i32ReadIdx ]->GetProtocolID() );
				}
				// 무한루프로 빠지지 않도록 실패시에도 카운터 증가 시킴
				pUserMessage->_i32ReadPos++;
				iSendCompleteCount++; 
			}
		}

		// 2-3 HEART BIT 테스트
		if( ( m_pCastSocket->m_ui32LastSendTime + CAST_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket( PROTOCOL_ASC_BATTLE_ROOM_COUNT_REQ );
			INT32 i32RoomCount = 0;
			if( SERVER_STATE_START == g_eServerState )
			{
				i32RoomCount = (INT32)g_pRoomManager->GetGroupCount();
			}
			SendPacket.WriteData( &i32RoomCount, sizeof(INT32) );

			i32SendBytes = m_pCastSocket->SendPacketMessage(&SendPacket);
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleCast::_ControlUpdate:HEART_BIT] SendPacketMessage Error(%d)\n", GetLastError() );
			}
			m_pCastSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pCastSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State
	if( (m_pCastSocket->m_ui32LastRecvTime + CAST_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
	{
		INT32 i32Rv = _CastConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			DSWARNING("ReConnect CastServer 2 Success\n" );
			SetWarningInfo( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			DSERROR( "ReConnect CastServer 2 Fail\n");
			SetWarningInfo( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK );
		}
	}

	// 5. Check Group Count
	if( SERVER_STATE_START == g_eServerState )
	{
		if( ( m_ui32UpdateCheck + UPDATE_CHECK_TIME ) < i3ThreadTimer::GetServerTime() )
		{	// UPDATE_CHECK_TIME 시간만에 한번씩 체크한다.
			m_ui32UpdateCheck = i3ThreadTimer::GetServerTime();

			g_pRoomManager->OnUpdate_C( m_ui32UpdateCheckStartIdx );

			m_ui32UpdateCheckStartIdx += UPDATE_CHECK_JUMP_COUNT;
			m_ui32UpdateCheckStartIdx %= g_pConfig->m_i32RoomCount;
		}
	}

	return iSendCompleteCount;
}

BOOL CModuleCast::Send_PROTOCOL_DSERVER_REGISTER_ROOM_ACK( INT32 i32WorkIdx, P_BATTLE_ROOM_CREATE_ACK * pRoomInfo)
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_CREATE_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pRoomInfo,	sizeof(P_BATTLE_ROOM_CREATE_ACK));

	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_KILL_ACK ( INT32 i32WorkIdx, P_BATTLE_ROOM_KILL_ACK* pHeader, DEATH_INFO_CHARA* pCharacters, UINT8 ui8RoundNum )
{
	CAST_CHECKVALID_IDX;
	
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_KILL_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pHeader,			sizeof( P_BATTLE_ROOM_KILL_ACK ));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pCharacters,		sizeof( DEATH_INFO_CHARA ) * pHeader->_killHeader._nCount );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8RoundNum,	sizeof( UINT8 ) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL	CModuleCast::Send_PROTOCOL_BATTLE_M_BOMB_INSTALL_ACK(INT32 i32WorkIdx,  P_BATTLE_M_BOMB_INSTALL_ACK*	pBombInstall	)
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_M_BOMB_INSTALL_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData  ( pBombInstall, sizeof(P_BATTLE_M_BOMB_INSTALL_ACK));
	pNetBuffer->_i32WritePos++;
	
	return TRUE;
}

BOOL	CModuleCast::Send_PROTOCOL_BATTLE_M_BOMB_UNINSTALL_ACK( INT32 i32WorkIdx, P_BATTLE_M_BOMB_UNINSTALL_ACK*	pBombUninstall  )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_M_BOMB_UNINSTALL_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData  ( pBombUninstall, sizeof(P_BATTLE_M_BOMB_UNINSTALL_ACK));
	pNetBuffer->_i32WritePos++;
	
	return TRUE;
}

BOOL	CModuleCast::Send_PROTOCOL_BATTLE_M_DESTRUCTION_HP_ACK( INT32 i32WorkIdx, P_BATTLE_M_DESTRUCTION_HP_ACK*	pDestructionHP	)
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_M_DESTRUCTION_HP_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData  ( pDestructionHP, sizeof (P_BATTLE_M_DESTRUCTION_HP_ACK));
	pNetBuffer->_i32WritePos++;
	
	return TRUE;
}

BOOL	CModuleCast::Send_PROTOCOL_BATTLE_M_DEFENCE_ACK ( INT32 i32WorkIdx, P_BATTLE_M_DEFENCE_ACK* pDefence )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_M_DEFENCE_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData  ( pDefence, sizeof(P_BATTLE_M_DEFENCE_ACK));
	pNetBuffer->_i32WritePos++;
	
	return TRUE;
}

BOOL	CModuleCast::Send_PROTOCOL_BATTLE_HACK_USER_ACK( INT32 i32WorkIdx, P_BATTLE_HACK_USER_ACK* pHeader, P_BATTLE_HACK_INFO* pUsers ) 
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_HACK_USER_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData  ( pHeader, (UINT16)(sizeof(P_BATTLE_HACK_USER_ACK)) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData	( pUsers,  (UINT16)(sizeof(P_BATTLE_HACK_INFO) * pHeader->_uCount) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK( INT32 i32WorkIdx, STRUCT_BATTLE_SLOT_EQUIPMENT_ACK* pHeader )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_SLOT_EQUIPMENT_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pHeader, sizeof(STRUCT_BATTLE_SLOT_EQUIPMENT_ACK) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_KICK_USER_BATTLE_ACK( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32SlotIdx, INT64 i64UID )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_KICK_USER_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRoomInfo,		sizeof(BATTLE_ROOM_INFO) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i64UID,			sizeof(INT64)			 );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32SlotIdx,	sizeof(UINT32)			 );
	pNetBuffer->_i32WritePos++;

	return TRUE;	
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_GM_PAUSE_ACK( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, INT32 i32Result )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_GM_PAUSE_ACK );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRoomInfo,		sizeof( BATTLE_ROOM_INFO ) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i32Result,	sizeof( INT32 ) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

void CModuleCast::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pCastSocket->SendPacketMessage(SendPacket);
}

BOOL CModuleCast::ServerInit( INT32 i32WorkIdx, INT32 i32Result )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_ASC_SERVER_LOADING_RESULT );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i32Result,	sizeof(INT32) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_RoundStart( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_MISSION_ROUND_START_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRoomInfo,			sizeof(BATTLE_ROOM_INFO) );
	pNetBuffer->_i32WritePos++;
	
	return TRUE;
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_RoundEnd( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT8 ui8RoundEndType, UINT8 ui8RoundWindTeam )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_MISSION_ROUND_END_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRoomInfo,			sizeof(BATTLE_ROOM_INFO) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8RoundEndType,	sizeof(UINT8) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui8RoundWindTeam,	sizeof(UINT8) );
	pNetBuffer->_i32WritePos++;
	
	return TRUE;
}

BOOL CModuleCast::Send_PROTOCOL_BATTLE_TouchDown( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32SlotIdx )
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_M_TOUCHDOWN_REQ );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( pRoomInfo,			sizeof(BATTLE_ROOM_INFO) );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32SlotIdx,		sizeof( UINT32 ) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

UINT32	CModuleCast::GetWarningInfo( INT32 i32WarningType )
{
	if( 0 > i32WarningType )						return 0;
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return 0;

	UINT32 ui32WarningInfo = 0;

	for (INT32 i = 0; i < WARNING_LEVEL_COUNT; i++)
	{
		if ( m_ui32WriteTime[i32WarningType][i] > m_ui32ReadTime[i32WarningType][i] )
		{
			ui32WarningInfo = ui32WarningInfo | GetFlagWarningLevel( i + 1);
			m_ui32ReadTime[i32WarningType][i] = i3ThreadTimer::GetServerTime();
		}
	}

	return ui32WarningInfo;
}

BOOL	CModuleCast::SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel )
{
	if( 0 > i32WarningType )						return FALSE;
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return FALSE;
	if( WARNING_LEVEL_NO > i32WarningLevel )		return FALSE;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return FALSE;

	UINT32 ui32ServerTime = i3ThreadTimer::GetServerTime();
	
	m_ui32WriteTime[i32WarningType][i32WarningLevel - 1] = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.
	
	return TRUE;
}

UINT32	CModuleCast::GetFlagWarningLevel( INT32 i32WarningLevel )
{
	if( WARNING_LEVEL_NO >= i32WarningLevel )		return 0;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return 0;

	return (0x1 << (i32WarningLevel - 1));
}