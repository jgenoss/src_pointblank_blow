#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
// #include "ModuleLogFile.h"
#include "RoomBase.h"
#include "RoomManager.h"
#include "UserManager.h"
#include "Room.h"

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

CModuleControl * g_pModuleControl = NULL; 

//////////////////////////////////////////////////////////////
//CONTROL
#define CONTROL_MAX_SEND_IDX_COUNT				2000000000		
#define CONTROL_RECONNECT_TIME					10
#define CONTROL_SERVER_HEART_BIT				5
#define CONTROL_SERVER_SERVERINFO				5
#define CONTROL_SERVER_HEART_BIT_REQ_TIME		70
#ifdef I3_DEBUG
#define CONTROL_SEND_BUFFER_COUNT				20
#else
#define CONTROL_SEND_BUFFER_COUNT				100
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CONTROL_CHECKVALID_IDX		if( !m_pControlSocket->IsActive())				return FALSE; \
	CONNECT_SERVER_NET_BUFFER	* pNetBuffer = NULL; \
	switch( WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sControlMainMessage;			break; \
	case	MODULE_CONTROL_THREAD_IDX	: I3ASSERT_0;									break; \
	case	MODULE_CAST_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_TRANS_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_CLAN_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_MESSENGER_THREAD_IDX	: I3ASSERT_0;									break; \
	default								: pNetBuffer = &m_pControlUserMessage[WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CONTROL_MAX_SEND_IDX_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleControl WriteBuffer FULL" );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CONTROL_SEND_BUFFER_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleControl WriteBuffer Overflow" );\
	return FALSE; \
}\
	INT32 WriteIdx = pNetBuffer->_WritePos % CONTROL_SEND_BUFFER_COUNT;


CModuleControl::CModuleControl(void)
{
	m_bRunning						= FALSE;
	m_ui32ThreadCount				= 0;
  	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pControlSocket				= NULL;
	m_ui32ControlConnectTime		= 0; 
	m_ui32ControlSystemInfoSendTime	= 0;
	m_pControlUserMessage			= NULL;
	i3mem::FillZero( &m_sControlMainMessage, sizeof( CONNECT_SERVER_NET_BUFFER ) );
}

CModuleControl::~CModuleControl(void)
{
	OnDestroy();
}

BOOL CModuleControl::OnCreate( UINT32 ui32ThreadCount )
{
	m_ui32ThreadCount = ui32ThreadCount; //Set Buffer 

	// Buffer Create
	// Main
	m_sControlMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CONTROL_SEND_BUFFER_COUNT);
	for( UINT32 i = 0 ; i < CONTROL_SEND_BUFFER_COUNT ; i++ )
	{
		m_sControlMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}

	// UserSession
	m_pControlUserMessage = (CONNECT_SERVER_NET_BUFFER*) i3MemAlloc(sizeof(CONNECT_SERVER_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
	{
		m_pControlUserMessage[i]._ReadPos	= 0; 
		m_pControlUserMessage[i]._WritePos	= 0; 
		m_pControlUserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CONTROL_SEND_BUFFER_COUNT); 
		for( UINT32 j = 0 ; j < CONTROL_SEND_BUFFER_COUNT ; j++ )
		{
			m_pControlUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Socket
	m_pControlSocket = CModuleControlSocket::NewObject();
	if( FALSE == m_pControlSocket->OnCreate(g_pContextMain->m_ui32ControlServerIp, g_pContextMain->m_ui16ControlServerPort) )	return FALSE;

	m_ui32ControlConnectTime		= i3ThreadTimer::GetServerTime();
	m_ui32ControlSystemInfoSendTime	= i3ThreadTimer::GetServerTime();

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleContextA_Control", 0, 4096, NULL, PRIORITY_HIGH) )								return FALSE;
	
	return TRUE;
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

BOOL CModuleControl::InitSystemLog()
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
	g_pLog->AddLogThread( L"MDCTACTR", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

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
	if( m_pControlSocket->OnCreate(g_pContextMain->m_ui32ControlServerIp, g_pContextMain->m_ui16ControlServerPort) )
	{
		m_ui32ControlSystemInfoSendTime	= i3ThreadTimer::GetServerTime();

		return EVENT_ERROR_SUCCESS;
	}
	
	m_pControlSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleControl::_ControlUpdate(void) 
{
	INT32	i32SendCompleteCount = 0; 
	INT32	i32SendCount;
	INT32	i32ReadIdx;

	UINT32	i;

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
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"ReConnect ControlServer 1 Success" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"ReConnect ControlServer 1 Fail" );
		}
		return i32SendCompleteCount;
	}

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pControlSocket->IsActive() )
	{
		//2-1 Main Thread 
		i32SendCount = m_sControlMainMessage._WritePos - m_sControlMainMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sControlMainMessage._ReadPos % CONTROL_SEND_BUFFER_COUNT; 
			m_pControlSocket->SendPacketMessage( m_sControlMainMessage._ppNetMessage[ i32ReadIdx ] ); 
			m_sControlMainMessage._ReadPos++; 
			i32SendCompleteCount++;
		}
		
		//2-3 User Thread
		for( i = 0; i < m_ui32ThreadCount ; i++ )
		{
			CONNECT_SERVER_NET_BUFFER	* pUserMessage = &m_pControlUserMessage[ i ]; 
			i32SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_ReadPos % CONTROL_SEND_BUFFER_COUNT; 
				m_pControlSocket->SendPacketMessage( pUserMessage->_ppNetMessage[ i32ReadIdx ]); 
				pUserMessage->_ReadPos++; 
				i32SendCompleteCount++; 
			}
		}

		//2-4 Check Send 패킷
		if( (m_pControlSocket->m_ui32LastSendTime + CONTROL_SERVER_SERVERINFO) < i3ThreadTimer::GetServerTime() )
		{
			i3NetworkPacket SendPacket( PROTOCOL_BASE_GAME_SERVER_STATE_REQ ); 
			m_pControlSocket->SendPacketMessage(&SendPacket); 
			m_pControlSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
		}
			
		//2-5시스템 정보 패킷 보내기
		if( g_eServerState >= SERVER_STATE_START && (m_ui32ControlSystemInfoSendTime + m_pControlSocket->m_ui32ControlSendTime) < i3ThreadTimer::GetServerTime() )
		{
			INT32					i32CPUCount		= (UINT32)m_pdhPerfmon.GetValue( PST_CPU_CUR );
			INT32					i32HandleCount	= (INT32)m_pdhPerfmon.GetValue( PST_LOCALDISK_FREE_SPACE_PERSENT );
			
			INT32 i32Ppwt = m_pdhPerfmon.CheckAllPerformWarning();
				if( i32Ppwt != PWT_NO ) g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_PERFOMANCE, L"Perfomance Limit Over Perform Warning Type : %d", i32Ppwt );

			INT32 i32Arg1 = 0;
			INT32 i32Arg2 = 0;

			INT32 i32UserCount[GAME_CHANNEL_COUNT];

			if(g_pUserManager != NULL) i32Arg1 = g_pUserManager->GetSessionCount();	
			if( SERVER_TYPE_GATEWAY == g_pContextMain->m_i32Gateway )
			{
				if(g_pUserManager != NULL) i32Arg2 = g_pUserManager->GetActiveCount();	
			}
			else
			{
				if(g_pRoomManager != NULL) i32Arg2 = g_pRoomManager->GetUseRoomCount();
			}

			for(INT32 Loop = 0; Loop < GAME_CHANNEL_COUNT; Loop++)
			{
				if( g_pUserManager )
				{
					i32UserCount[Loop]	= g_pUserManager->GetChannelUserCount(Loop);
				}
				else
				{
					i32UserCount[Loop]	= 0;
				}
			}

			P_SERVER_STATE_REQ PacketData;
			i3mem::FillZero( &PacketData, sizeof(P_SERVER_STATE_REQ) );

			if ( SERVER_STATE_START == g_eServerState )
			{
				PacketData._ui32Active	= SERVER_ACTIVE_STATE_CONNECT;
			}
			else
			{
				PacketData._ui32Active	= SERVER_ACTIVE_STATE_LOADING;
			}
			PacketData._i32Cpu			= i32CPUCount;
			PacketData._i32Handle		= i32HandleCount;
			PacketData._i32Arg0			= i32Arg1;
			PacketData._i32Arg1			= i32Arg2;
			PacketData._i32Arg2			= g_pContextMain->m_i32PacketCount;
			PacketData._i32Arg3			= g_pContextMain->m_i32PacketSize;
	

			for( INT32 i32Warning = 0 ; i32Warning < WARNING_TYPE_COUNT; i32Warning++)
			{
				PacketData._ui32WarningInfo[ i32Warning ] = g_pLog->GetWarning().GetWarningInfo( i32Warning );
			}

			i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_REQ );
			SendPacket.WriteData( &PacketData,									sizeof(P_SERVER_STATE_REQ) );
			SendPacket.WriteData( i32UserCount,									sizeof(INT32) * GAME_CHANNEL_COUNT );
			SendPacket.WriteData( &g_pUserManager->m_ui32RecvHeartBit2Count,	sizeof(UINT32) );	// HeartBit2 

			m_pControlSocket->SendPacketMessage(&SendPacket);
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
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"ReConnect ControlServer 2 Success" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"ReConnect ControlServer 2 Fail" );
		}
	}

	return i32SendCompleteCount;
}

// ----------------------------------------------------------------------------------------
// Battle Server
// ----------------------------------------------------------------------------------------
BOOL CModuleControl::ServerInit( INT32 WorkIdx, INT32 i32Result )
{
	CONTROL_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_ASC_SERVER_LOADING_RESULT ); 
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &i32Result,	sizeof(INT32) );
	pNetBuffer->_WritePos++;

	return TRUE;
}

void CModuleControl::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pControlSocket->SendPacketMessage(SendPacket);
}