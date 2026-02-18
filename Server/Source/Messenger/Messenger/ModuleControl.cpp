#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "UserFinder.h"
#include "TaskProcessor.h"

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

CModuleControl * g_pModuleControl		= NULL; 

#define CONTROL_SEND_BUFFER					100
#define CONTROL_RECONNECT_TIME				1000 *	30	//MS
#define CONTROL_SERVER_HEART_BIT			60
#define CONTROL_CONNECT_TIMEOUT				10
#define CONTROL_SERVER_HEART_BIT_REQ_TIME	10			//10초 동안 하트비트를 받지 못하면 연결을 다시한다. 
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

CModuleControl::CModuleControl(void)
{
	m_bRunning				= FALSE;
	m_pControlSocket		= NULL;
}

CModuleControl::~CModuleControl(void)
{
	OnDestroy();
}

BOOL CModuleControl::OnCreate()
{
	if(!g_pConfig->m_bControlActive)return TRUE; 
		
	BOOL bRv = TRUE; 
	//Create Socket 
	m_pControlSocket = CModuleControlSocket::NewObject(); 
	bRv = m_pControlSocket->OnCreate(g_pConfig->m_strControlServerIp, g_pConfig->m_ui16ControlServerPort, m_i32LogIdx);
	if(bRv)
	{
		m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
	}
	else
	{
		m_pControlSocket->OnDestroy();
	}

	//----------------------------------------------------
	m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
	m_pControlSocket->m_ui32LastRecvTime = m_ui32LastSendTime + 1;


	//Create Thread
	bRv = i3Thread::Create("ModuleControl", 0, 4096, NULL, PRIORITY_NORMAL);
	if(!bRv)return FALSE;
	
	return bRv;
}

void CModuleControl::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	//----------------------------------------------------
	I3_SAFE_RELEASE( m_pControlSocket );

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
	m_i32LogIdx = g_pLog->AddLogThread(L"MDCT", 0, m_ThreadID);

	BOOL	bIsSend;
	m_bRunning	= TRUE;

	while( m_bRunning )
	{
		bIsSend = FALSE;
		
		//-------------------------------------------------------------------------------
		//1. Connect Check 
		if(!m_pControlSocket->IsConnect())
		{

			// Control 서버에 정상 종료 패킷 전송.
			i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
			m_pControlSocket->SendPacketMessage( &SendPacket );

			Sleep(1000);

			m_pControlSocket->OnDestroy(); 
			if( m_pControlSocket->OnCreate(g_pConfig->m_strControlServerIp, g_pConfig->m_ui16ControlServerPort, m_i32LogIdx) )
			{
				m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
				I3TRACE("Success ReConnect ControlServer Check 1\n");
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect ControlServer 1" );
			}
			else
			{
				m_pControlSocket->OnDestroy(); 
				I3TRACE("Fail ReConnect ControlServer Check 1\n");
				g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect ControlServer 1" );
				::Sleep( CONTROL_RECONNECT_TIME );
			}

			m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
			m_pControlSocket->m_ui32LastRecvTime = m_ui32LastSendTime + 1;

			::WaitForSingleObject( m_Handle, 1);
			continue; 
		}

		//-------------------------------------------------------------------------------
		//2. Send Message 
		if( m_pControlSocket->IsActive() )
		{
			//2-4 Check Send 패킷
			if( (m_ui32LastSendTime + CONTROL_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
			{
				i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
				m_pControlSocket->SendPacketMessage(&SendPacket);
				m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
			}
			
			//2-5시스템 정보 패킷 보내기
			if( (m_ui32SystemInfoSendTime + m_pControlSocket->m_ui32ControlSendTime) < i3ThreadTimer::GetServerTime() )
			{
				INT32					i32CPUCount		= (UINT32)m_pdhPerfmon.GetValue( PST_CPU_CUR );
				INT32					i32HandleCount	= (INT32)m_pdhPerfmon.GetValue( PST_LOCALDISK_FREE_SPACE_PERSENT );
		
				INT32 i32Ppwt = m_pdhPerfmon.CheckAllPerformWarning();
				if( i32Ppwt != PWT_NO ) g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_PERFOMANCE, L"Perfomance Limit Over Perform Warning Type : %d", i32Ppwt );

				P_SERVER_STATE_REQ	PacketData;
				i3mem::FillZero( &PacketData, sizeof(P_SERVER_STATE_REQ) );

				if ( SERVER_STATE_START == g_eServerState )
				{
					PacketData._ui32Active	= SERVER_ACTIVE_STATE_CONNECT;
					PacketData._i32Arg0		= (INT32)g_pConfig->GetConnectedGameServerCount();
					PacketData._i32Arg1		= g_pTaskProcessor->GetUserCount();
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
		else 
		{
			//Connect Req Fail
			if( m_ui32ConnectTime + CONTROL_CONNECT_TIMEOUT < i3ThreadTimer::GetServerTime())
			{
				// Control 서버에 정상 종료 패킷 전송.
				i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
				m_pControlSocket->SendPacketMessage( &SendPacket );

				Sleep(1000);

				m_pControlSocket->OnDestroy(); 
				if( m_pControlSocket->OnCreate(g_pConfig->m_strControlServerIp, g_pConfig->m_ui16ControlServerPort, m_i32LogIdx ) )
				{
					m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect ControlServer 3" );
				}
				else
				{
					m_pControlSocket->OnDestroy(); 
					g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect ControlServer3" );
					::Sleep(CONTROL_RECONNECT_TIME);
				}
			}
		}

		//-------------------------------------------------------------------------------
		//3. Recv Buffer
		m_pControlSocket->SelectEvent();

		//-------------------------------------------------------------------------------
		//4. Check Connect State
		if( m_pControlSocket->IsActive() && (m_ui32LastSendTime > m_pControlSocket->m_ui32LastRecvTime) )
		{//ACK를 못받을때 들어온다. 
			if( (m_ui32LastSendTime + CONTROL_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
			{
				// Control 서버에 정상 종료 패킷 전송.
				i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
				m_pControlSocket->SendPacketMessage( &SendPacket );

				Sleep(1000);

				m_pControlSocket->OnDestroy(); 
				if( m_pControlSocket->OnCreate(g_pConfig->m_strControlServerIp, g_pConfig->m_ui16ControlServerPort , m_i32LogIdx) )
				{
					m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
					I3TRACE("Success ReConnect ControlServer 2\n");
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect ControlServer 2" );
				}
				else
				{
					m_pControlSocket->OnDestroy(); 
					I3TRACE("Fail ReConnect ControlServer2\n");
					g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect ControlServer2" );
					::Sleep(CONTROL_RECONNECT_TIME);
				}

				m_ui32LastSendTime						= i3ThreadTimer::GetServerTime();
				m_pControlSocket->m_ui32LastRecvTime	= m_ui32LastSendTime + 1;
			}
		}

		::WaitForSingleObject( m_Handle, 1);
	}
	return 0;
}

void CModuleControl::SendPacketMessage( i3NetworkPacket *pSendPacket )
{
	m_pControlSocket->SendPacketMessage(pSendPacket);
}

BOOL CModuleControl::ServerInit( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_ASC_SERVER_LOADING_RESULT );
	SendPacket.WriteData( &i32Result,	sizeof(INT32) );
	m_pControlSocket->SendPacketMessage( &SendPacket );

	return TRUE;
}