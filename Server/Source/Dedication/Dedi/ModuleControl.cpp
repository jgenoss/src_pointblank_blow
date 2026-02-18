#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "GroupMaker.h"
#include "iocpServer.h"
#include "UdpBufferPool.h"
//#include "UdpBuffer.h"

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

CModuleControl * g_pModuleControl		= NULL; 

//////////////////////////////////////////////////////////////
//CONTROL
#define CONTROL_MAX_SEND_IDX_COUNT				2000000000		
#define CONTROL_MAX_ZERO_COUNT					1900000000
#define CONTROL_RECONNECT_TIME					10
#define CONTROL_SERVER_HEART_BIT				5
#define CONTROL_SERVER_SERVERINFO				5
#define CONTROL_SERVER_HEART_BIT_REQ_TIME		70
#define CONTROL_SEND_BUFFER_COUNT				500

#define CONTROL_CHECKVALID_IDX		if( !m_pControlSocket->IsActive())				return FALSE; \
	CONTROL_NET_BUFFER	* pNetBuffer = NULL; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sControlMainMessage;				break; \
	case	MODULE_CONTROL_THREAD_IDX	: pNetBuffer = &m_sControlMessage;					break; \
		default							: pNetBuffer = &m_pControlUserMessage[i32WorkIdx];	break; \
}\
	if(pNetBuffer->_i32WritePos > CONTROL_MAX_ZERO_COUNT)\
{\
	if( pNetBuffer->_i32WritePos == pNetBuffer->_i32ReadPos )\
{\
	pNetBuffer->_i32WritePos = 0;\
	pNetBuffer->_i32ReadPos	= 0;\
}\
}\
	if( pNetBuffer->_i32ReadPos > CONTROL_MAX_SEND_IDX_COUNT )\
{\
	DSERROR("ModuleControl WriteBuffer FULL\n" );\
	return FALSE; \
}\
	if( (pNetBuffer->_i32WritePos - pNetBuffer->_i32ReadPos) >= (CONTROL_SEND_BUFFER_COUNT -1) )\
{\
	DSERROR("ModuleControl WriteBuffer Overflow\n");\
	return FALSE; \
}\
INT32 i32WriteIdx = pNetBuffer->_i32WritePos % CONTROL_SEND_BUFFER_COUNT; \


CModuleControl::CModuleControl(void)
{
	m_bRunning						= FALSE;
	m_ui32ThreadCount				= 0;
  	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control 
	m_pControlSocket				= NULL;
	m_ui32ControlConnectTime		= 0;
	m_ui32ControlSystemInfoSendTime	= 0;
	m_pControlUserMessage			= NULL;
	i3mem::FillZero( &m_sControlMainMessage,	sizeof( CONTROL_NET_BUFFER ) );
	i3mem::FillZero( &m_sControlMessage,		sizeof( CONTROL_NET_BUFFER ) );

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

CModuleControl::~CModuleControl(void)
{
	OnDestroy(); 
}

BOOL CModuleControl::OnCreate( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ThreadCount )
{
	m_ui32IP	= ui32IP;
	m_ui16Port	= ui16Port;

	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	// Buffer Create
	// Main
	m_sControlMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CONTROL_SEND_BUFFER_COUNT);
	for( UINT32 i = 0 ; i < CONTROL_SEND_BUFFER_COUNT ; i++ )
	{
		m_sControlMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}
	m_sControlMessage._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CONTROL_SEND_BUFFER_COUNT);
	for( UINT32 i = 0; i < CONTROL_SEND_BUFFER_COUNT ; i++ )
	{
		m_sControlMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}

	// UserSession
	m_pControlUserMessage = (CONTROL_NET_BUFFER*) i3MemAlloc(sizeof(CONTROL_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
	{
		m_pControlUserMessage[i]._i32ReadPos	= 0;
		m_pControlUserMessage[i]._i32WritePos	= 0;
		m_pControlUserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CONTROL_SEND_BUFFER_COUNT); 
		for( UINT32 j = 0 ; j < CONTROL_SEND_BUFFER_COUNT ; j++ )
		{
			m_pControlUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Socket
	m_pControlSocket = CModuleControlSocket::NewObject();
	if( FALSE == m_pControlSocket->OnCreate( m_ui32IP, m_ui16Port ) )		return SERVER_ERROR_CONTROL;

	m_ui32ControlConnectTime		= i3ThreadTimer::GetServerTime();
	m_ui32ControlSystemInfoSendTime	= i3ThreadTimer::GetServerTime();

	m_ui32UpdateCheck				= i3ThreadTimer::GetServerTime();
	m_ui32UpdateCheckStartIdx		= 0;

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleContextA_Control", 0, 4096, NULL, PRIORITY_HIGH) )	return SERVER_ERROR_CONTROL;
	
#ifdef NEW_LOG
	//Create Private Log Buffer
	I3TRACE("CModuleControl=%d\n", m_ThreadID );
	if( NULL == g_pLogFile->AllocBuffer( m_ThreadID ) ) return SERVER_ERROR_CONTROL;
#endif

	return EVENT_ERROR_SUCCESS;
}

void CModuleControl::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pControlSocket );

	if( m_sControlMainMessage._ppNetMessage )
	{
		for(UINT32 i = 0; i < CONTROL_SEND_BUFFER_COUNT; i++)
		{
			I3_SAFE_RELEASE( m_sControlMainMessage._ppNetMessage[i] ); 
		}

		I3MEM_SAFE_FREE(m_sControlMainMessage._ppNetMessage);
	}

	if( m_sControlMessage._ppNetMessage )
	{
		for(UINT32 i = 0; i < CONTROL_SEND_BUFFER_COUNT; i++)
		{
			I3_SAFE_RELEASE( m_sControlMessage._ppNetMessage[i] );
		}

		I3MEM_SAFE_FREE(m_sControlMessage._ppNetMessage);
	}

	if( m_pControlUserMessage )
	{
		for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
		{
			if( m_pControlUserMessage[i]._ppNetMessage )
			{
				for( UINT32 j = 0 ; j < CONTROL_SEND_BUFFER_COUNT ; j++)
				{
					I3_SAFE_RELEASE(m_pControlUserMessage[i]._ppNetMessage[j] );
				}
				I3MEM_SAFE_FREE(m_pControlUserMessage[i]._ppNetMessage); 
			}
		}
		I3MEM_SAFE_FREE( m_pControlUserMessage );
	}

	m_pdhPerfmon.Destroy();

	return; 
}

INT32 CModuleControl::InitSystemLog()
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

UINT32 CModuleControl::OnRunning( void * pUserData)
{
	m_bRunning		= TRUE;

	while( m_bRunning )
	{
		if( 0 < _ControlUpdate() )	continue;

		// 한일이 없으면 1ms 휴식.
		::WaitForSingleObject( m_Handle, 1);
	}

	return 0; 
}

INT32 CModuleControl::_ControlConnect()
{
	if( m_ui32ControlConnectTime + CONTROL_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32ControlConnectTime = i3ThreadTimer::GetServerTime();

	m_pControlSocket->OnDestroy(); 
	if( m_pControlSocket->OnCreate( m_ui32IP, m_ui16Port ) )
	{
		m_ui32ControlSystemInfoSendTime	= i3ThreadTimer::GetServerTime();

		return EVENT_ERROR_SUCCESS;
	}
	
	m_pControlSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleControl::_ControlUpdate(void) 
{
	INT32	iSendCompleteCount = 0; 
	INT32	i32SendCount	= 0;
	INT32	i32ReadIdx		= 0;
	INT32	i32SendBytes	= 0;

	//-------------------------------------------------------------------------------
	//1. Connect Check 
	if( FALSE == m_pControlSocket->IsConnect())
	{
		// Control 서버에 정상 종료 패킷 전송.
		i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		m_pControlSocket->SendPacketMessage( &SendPacket );
		Sleep(1000);

		INT32 i32Rv = _ControlConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			DSWARNING("ReConnect ControlServer 1 Success\n");
			SetWarningInfo( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			DSERROR("ReConnect ControlServer 1 Fail\n");
			SetWarningInfo( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK );
		}
		return iSendCompleteCount;
	}

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pControlSocket->IsActive() )
	{
		////2-1 Main Thread 
		i32SendCount = m_sControlMainMessage._i32WritePos - m_sControlMainMessage._i32ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sControlMainMessage._i32ReadPos % CONTROL_SEND_BUFFER_COUNT; 
			
			i32SendBytes = m_pControlSocket->SendPacketMessage( m_sControlMainMessage._ppNetMessage[ i32ReadIdx ] ); 
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleControl::_ControlUpdate:SYSTEM_PACKET] SendPacketMessage Error(%d), protcol=%d\n", GetLastError(), m_sControlMainMessage._ppNetMessage[ i32ReadIdx ]->GetProtocolID() );
			}
			// 무한루프로 빠지지 않도록 실패시에도 카운터 증가 시킴			
			m_sControlMainMessage._i32ReadPos++;
			iSendCompleteCount++;
		}

		////2-1-2 Control Thread
		i32SendCount = m_sControlMessage._i32WritePos - m_sControlMessage._i32ReadPos;
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sControlMessage._i32ReadPos % CONTROL_SEND_BUFFER_COUNT;

			i32SendBytes = m_pControlSocket->SendPacketMessage( m_sControlMessage._ppNetMessage[ i32ReadIdx ] );
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleControl::_ControlUpdate:SYSTEM_PACKET] SendPacketMessage Error(%d), protcol=%d\n", GetLastError(), m_sControlMessage._ppNetMessage[ i32ReadIdx ]->GetProtocolID() );
			}
			// 무한루프로 빠지지 않도록 실패시에도 카운터 증가 시킴			
			m_sControlMessage._i32ReadPos++;
			iSendCompleteCount++;
		}
		
		//2-2 User Thread
		for( INT32 i = 0; i < m_ui32ThreadCount ; i++ )
		{
			CONTROL_NET_BUFFER	* pUserMessage = &m_pControlUserMessage[ i ]; 
			i32SendCount = pUserMessage->_i32WritePos - pUserMessage->_i32ReadPos;
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_i32ReadPos % CONTROL_SEND_BUFFER_COUNT; 
				i32SendBytes = m_pControlSocket->SendPacketMessage( pUserMessage->_ppNetMessage[ i32ReadIdx ]); 
				if( i32SendBytes <= 0)
				{
					DSERROR( "[CModuleControl::_ControlUpdate:TASK_PACKET] SendPacketMessage Error(%d), protocol=%d\n", GetLastError(), pUserMessage->_ppNetMessage[ i32ReadIdx ]->GetProtocolID() );
				}
				// 무한루프로 빠지지 않도록 실패시에도 카운터 증가 시킴
				pUserMessage->_i32ReadPos++;
				iSendCompleteCount++; 
			}
		}

		// 2-3 HEART BIT 테스트
		if( ( m_pControlSocket->m_ui32LastSendTime + CONTROL_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket( PROTOCOL_BASE_HEART_BIT_REQ ); 
			i32SendBytes = m_pControlSocket->SendPacketMessage(&SendPacket);
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleControl::_ControlUpdate:HEART_BIT] SendPacketMessage Error(%d)\n", GetLastError() );
			}
			m_pControlSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
		}
			
		//2-4시스템 정보 패킷 보내기
		if( (m_ui32ControlSystemInfoSendTime + m_pControlSocket->m_ui32ControlSendTime) < i3ThreadTimer::GetServerTime() )
		{
			INT32					i32CPUCount		= (UINT32)m_pdhPerfmon.GetValue( PST_CPU_CUR);
			INT32					i32HandleCount	= (INT32)m_pdhPerfmon.GetValue( PST_LOCALDISK_FREE_SPACE_PERSENT );

			INT32 i32Ppwt = m_pdhPerfmon.CheckAllPerformWarning();
				if( i32Ppwt != PWT_NO )
				{
					DSWARNING("Perfomance Limit Over Perform Warning Type : %d \n", i32Ppwt );
					SetWarningInfo( WARNING_TYPE_NORMAL, WARNING_LEVEL_PERFOMANCE );
				}

				P_SERVER_STATE_REQ PacketData;
			if( SERVER_STATE_START == g_eServerState )
			{
				PacketData._ui32Active	= SERVER_ACTIVE_STATE_CONNECT;
				PacketData._i32Arg0		= (INT32)g_pRoomManager->GetGroupCount();
				g_pIocpServer->GetPacketProfile_C( PacketData._i32Arg1, PacketData._i32Arg2 );
			}
			else
			{
				PacketData._ui32Active	= SERVER_ACTIVE_STATE_LOADING;
				PacketData._i32Arg0		= 0;
				PacketData._i32Arg1		= 0;
				PacketData._i32Arg2		= 0;
			}

			PacketData._i32Cpu			= i32CPUCount;
			PacketData._i32Handle		= i32HandleCount;
			PacketData._i32Arg3			= g_pUdpBufferPool->GetEmptyBufferCount();
			PacketData._i32Arg4			= 0;//g_pConfig->isRelayMode();

			for( INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
			{
				PacketData._ui32WarningInfo[i] = GetWarningInfo( i );
			}

			i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_REQ );
			SendPacket.WriteData( &PacketData,	sizeof(P_SERVER_STATE_REQ) );
			i32SendBytes = m_pControlSocket->SendPacketMessage(&SendPacket);
			if( i32SendBytes <= 0 )
			{
				DSERROR( "[CModuleControl::_ControlUpdate:SYS_STATE] SendPacketMessage Error(%d)\n", GetLastError());
			}
			m_ui32ControlSystemInfoSendTime = i3ThreadTimer::GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pControlSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State
	if( (m_pControlSocket->m_ui32LastRecvTime + CONTROL_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
	{
		// Control 서버에 정상 종료 패킷 전송.
		i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		m_pControlSocket->SendPacketMessage( &SendPacket );

		Sleep(1000);


		INT32 i32Rv = _ControlConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			DSWARNING("ReConnect ControlServer 2 Success\n" );
			SetWarningInfo( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			DSERROR( "ReConnect ControlServer 2 Fail\n");
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

void CModuleControl::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pControlSocket->SendPacketMessage(SendPacket);
}

BOOL CModuleControl::ServerInit( INT32 i32WorkIdx, INT32 i32Result )
{
	CONTROL_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol( PROTOCOL_ASC_SERVER_LOADING_RESULT );
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &i32Result,	sizeof(INT32) );
	pNetBuffer->_i32WritePos++;

	return TRUE;
}

UINT32	CModuleControl::GetWarningInfo( INT32 i32WarningType )
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

BOOL	CModuleControl::SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel )
{
	if( 0 > i32WarningType )						return FALSE;
	if( WARNING_TYPE_COUNT - 1 < i32WarningType )	return FALSE;
	if( WARNING_LEVEL_NO > i32WarningLevel )		return FALSE;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return FALSE;

	UINT32 ui32ServerTime = i3ThreadTimer::GetServerTime();
	
	m_ui32WriteTime[i32WarningType][i32WarningLevel - 1] = ui32ServerTime;	// 해당 워닝 레벨에 현재 서버 시간 기록.
	
	return TRUE;
}

UINT32	CModuleControl::GetFlagWarningLevel( INT32 i32WarningLevel )
{
	if( WARNING_LEVEL_NO >= i32WarningLevel )		return 0;
	if( WARNING_LEVEL_COUNT - 1 < i32WarningLevel )	return 0;

	return (0x1 << (i32WarningLevel - 1));
}