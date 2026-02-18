#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
//
//#include "LogFile.h"

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

#define CONTROL_SEND_BUFFER		100
CModuleControl * g_pModuleControl		= NULL; 

#define CONTROL_RECONNECT_TIME				10	//MS
#define CONTROL_SERVER_HEART_BIT			60
#define CONTROL_CONNECT_TIMEOUT				10
#define CONTROL_SERVER_HEART_BIT_REQ_TIME	70			//10초 동안 하트비트를 받지 못하면 연결을 다시한다. 
//------------------------------------------------------------------------------------------
CModuleControl::CModuleControl(void)
{
	m_bRunning					= FALSE;
	m_pControlSocket			= NULL;
	m_ui32SystemInfoSendTime	= 0; 
}

CModuleControl::~CModuleControl(void)
{
	OnDestroy(); 
}

BOOL CModuleControl::OnCreate()
{
	//Create Socket
	m_pControlSocket = CModuleControlSocket::NewObject();
	if( FALSE == m_pControlSocket->OnCreate( g_pConfig->m_ui32ControlIP, g_pConfig->m_ui16ControlServerPort ))	return FALSE;

	m_ui32ControlConnectTime	= i3ThreadTimer::GetServerTime();
	m_ui32SystemInfoSendTime	= i3ThreadTimer::GetServerTime();

	//Create Thread
	if( FALSE == i3Thread::Create( "ModuleControl", 0, 4096, NULL, PRIORITY_NORMAL ) )							return FALSE;
	
	return TRUE; 
}

void CModuleControl::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pControlSocket );

	m_pdhPerfmon.Destroy();
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

INT32 CModuleControl::_ControlConnect()
{
	if( m_ui32ControlConnectTime + CONTROL_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32ControlConnectTime = i3ThreadTimer::GetServerTime();

	m_pControlSocket->OnDestroy(); 
	if( m_pControlSocket->OnCreate( g_pConfig->m_ui32ControlIP, g_pConfig->m_ui16ControlServerPort ) )
	{
		m_ui32SystemInfoSendTime	= i3ThreadTimer::GetServerTime();

		return EVENT_ERROR_SUCCESS;
	}
	
	m_pControlSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

UINT32 CModuleControl::OnRunning( void * pUserData)
{
	g_pLog->AddLogThread( L"CONT", 0, m_ThreadID );

	BOOL	bIsSend;
	m_bRunning = TRUE;
	while( m_bRunning )
	{
		bIsSend = FALSE;

		//-------------------------------------------------------------------------------
		//1. Connect Check 
		if(!m_pControlSocket->IsConnect())
		{
			// Control 서버에 정상 종료 패킷 전송.
			i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
			m_pControlSocket->SendPacketMessage(&SendPacket);

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
			::WaitForSingleObject( m_Handle, 1);
			continue;
		}

		//-------------------------------------------------------------------------------
		//2. Send Message 
		if( m_pControlSocket->IsActive() )
		{
			//2-4 Check Send 패킷
			if( (m_pControlSocket->m_ui32LastSendTime + CONTROL_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
			{
				i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
				m_pControlSocket->SendPacketMessage(&SendPacket);
				m_pControlSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
			}
			
			//2-5시스템 정보 패킷 보내기
			if( (m_ui32SystemInfoSendTime + m_pControlSocket->m_ui32ControlSendTime) < i3ThreadTimer::GetServerTime() )
			{
				INT32					i32CPUCount		= (UINT32)m_pdhPerfmon.GetValue( PST_CPU_CUR );
				INT32					i32HandleCount	= (INT32)m_pdhPerfmon.GetValue( PST_LOCALDISK_FREE_SPACE_PERSENT );

				INT32 i32Ppwt = m_pdhPerfmon.CheckAllPerformWarning();
				if( i32Ppwt != PWT_NO ) g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_PERFOMANCE, L"Perfomance Limit Over Perform Warning Type : %d", i32Ppwt );

				P_SERVER_STATE_REQ PacketData;
				i3mem::FillZero( &PacketData, sizeof(P_SERVER_STATE_REQ) );

				if ( SERVER_STATE_START == g_eServerState )
				{
					PacketData._ui32Active	= SERVER_ACTIVE_STATE_CONNECT;
					PacketData._i32Arg0		= g_pConfig->GetActiveGameServerCount();
				}
				else
				{
					PacketData._ui32Active	= SERVER_ACTIVE_STATE_LOADING;
				}

				PacketData._i32Cpu			= i32CPUCount;
				PacketData._i32Handle		= i32HandleCount;

				for( INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)	
				{
					PacketData._ui32WarningInfo[i] = g_pLog->GetWarning().GetWarningInfo(i);
				}

				i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_REQ );
				SendPacket.WriteData( &PacketData, sizeof(P_SERVER_STATE_REQ) ); 

				m_pControlSocket->SendPacketMessage(&SendPacket);
				m_ui32SystemInfoSendTime = i3ThreadTimer::GetServerTime();
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
			m_pControlSocket->SendPacketMessage(&SendPacket);

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

		::WaitForSingleObject( m_Handle, 1);
	}
	return 0; 
}

void CModuleControl::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pControlSocket->SendPacketMessage(SendPacket);
}

BOOL CModuleControl::ServerInit( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_ASC_SERVER_LOADING_RESULT );
	SendPacket.WriteData( &i32Result,	sizeof(INT32) );
	m_pControlSocket->SendPacketMessage( &SendPacket );

	return TRUE;
}