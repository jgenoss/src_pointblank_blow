#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"

CModuleControl * g_pModuleControl		= NULL;

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

CModuleControl::CModuleControl(void)
{
	m_bRunning			= FALSE;
	m_pControlSocket	= NULL;

	m_i32LogThreadIdx	= LOG_THREAD_INVALID;
}

CModuleControl::~CModuleControl(void)
{
	OnDestroy(); 
}

BOOL CModuleControl::OnCreate( UINT32 ui32IP, UINT16 ui16Port )
{
	m_ui32IP	= ui32IP;
	m_ui16Port	= ui16Port;

	m_i32LogThreadIdx		= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )											return FALSE;

	m_pControlSocket = CModuleControlSocket::NewObject();
	if( NULL == m_pControlSocket )															return FALSE;
	if( FALSE == m_pControlSocket->OnCreate( m_ui32IP, m_ui16Port, m_i32LogThreadIdx ) )	return FALSE;

	m_ui32ConnectTime		= g_pSystem->GetServerTime();
	m_ui32SystemSendTime	= g_pSystem->GetServerTime();

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleControl", 0, 4096, NULL, PRIORITY_NORMAL) )		return FALSE;

	WriteLog( g_i32MainLogThreadIdx, "[INIT] ControlServer Create Success" );

	return TRUE; 
}

INT32 CModuleControl::_ControlConnect()
{
	if( m_ui32ConnectTime + CONTROL_RECONNECT_TIME > g_pSystem->GetServerTime() )	return 1;
	m_ui32ConnectTime = g_pSystem->GetServerTime();

	m_pControlSocket->OnDestroy(); 
	if( m_pControlSocket->OnCreate( m_ui32IP, m_ui16Port, m_i32LogThreadIdx ) )
	{
		m_ui32SystemSendTime	= g_pSystem->GetServerTime();
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pControlSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

void CModuleControl::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pControlSocket );
}

UINT32 CModuleControl::OnRunning( void * pUserData)
{
	m_bRunning	= TRUE;
	while( m_bRunning )
	{
		//-------------------------------------------------------------------------------
		//1. Connect Check
		if( FALSE == m_pControlSocket->IsConnect() )
		{
			INT32 i32Rv = _ControlConnect();
			if( EVENT_ERROR_SUCCESS == i32Rv )
			{
				WriteLog( m_i32LogThreadIdx, "[CONT] ReConnect ControlServer 1 Success" );
			}
			else if( EVENT_ERROR_FAIL == i32Rv )
			{
				WriteLog( m_i32LogThreadIdx, "[CONT] ReConnect ControlServer 1 Fail" );
			}
			continue;
		}

		//-------------------------------------------------------------------------------
		//2. Send Message
		if( m_pControlSocket->IsActive() )
		{
			//2-1 Check Send 패킷
			if( (m_pControlSocket->GetLastSendTime() + CONTROL_SERVER_HEART_BIT) < g_pSystem->GetServerTime() )
			{
				i3NetworkPacket SendPacket( PROTOCOL_BASE_HEART_BIT_REQ );
				m_pControlSocket->SendPacketMessage( &SendPacket );
				m_pControlSocket->SetLastSendTime( g_pSystem->GetServerTime() );
			}
			
			//2-2 시스템 정보 패킷 보내기
			if( (m_ui32SystemSendTime + m_pControlSocket->GetSystemDelay()) < g_pSystem->GetServerTime() )
			{
				g_pSystem->OnUpdate();

				P_SERVER_STATE_REQ stState;
				stState._iCpu				= g_pSystem->GetCpuQuery();
				stState._iHandle			= g_pSystem->GetHandleQuery();
				stState._ui32Active			= 1;
				stState._ui32WarningInfo	= 0;
				stState._iArg0				= 0;
				stState._iArg1				= 0;
				stState._iArg2				= 0;
				stState._iArg3				= 0;
				stState._iArg4				= 0;

				i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_REQ );
				SendPacket.WriteData( &stState,			sizeof(P_SERVER_STATE_REQ) );
				m_pControlSocket->SendPacketMessage( &SendPacket );

				m_ui32SystemSendTime = g_pSystem->GetServerTime();
			}
		}

		//-------------------------------------------------------------------------------
		//3. Recv Buffer
		m_pControlSocket->SelectEvent(); 		

		//-------------------------------------------------------------------------------
		//4. Check Connect State
		if( (m_pControlSocket->GetLastRecvTime() + CONTROL_SERVER_HEART_BIT_REQ_TIME) < g_pSystem->GetServerTime() )
		{
			INT32 i32Rv = _ControlConnect();
			if( EVENT_ERROR_SUCCESS == i32Rv )
			{
				WriteLog( m_i32LogThreadIdx, "[CONT] ReConnect ControlServer 2 Success" );
			}
			else if( EVENT_ERROR_FAIL == i32Rv )
			{
				WriteLog( m_i32LogThreadIdx, "[CONT] ReConnect ControlServer 2 Fail" );
			}
		}
		
		::WaitForSingleObject( m_Handle, 1000 );
	}
	return 0;
}
