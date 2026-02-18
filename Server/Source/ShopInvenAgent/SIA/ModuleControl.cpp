#include "pch.h"
#include "ModuleControl.h"
#include "Config.h"

I3_CLASS_INSTANCE( CModuleControl, i3NetworkClientSocket2 );

CModuleControl * g_pModuleControl = NULL; 

#define CONTROL_SERVER_HEART_BIT		60	
#define CONTROL_SERVER_CONNECT_TIME		120
#define CONTROL_SERVER_INFO_TIME		5


static UINT32 _DefThreadProc( void * pTemp )
{
	pTemp;
	g_pModuleControl->OnWorking( );	
	return 0;
}

CModuleControl::CModuleControl()
{
	m_ui32ServerIP					= 0;
	m_ui16ServerPort				= 0;
	m_ui32LastConnectTime			= 0;
	m_ui32LastSendTime				= 0;
	m_ui32LastInfoTime				= 0;
	m_hThread						= NULL;
	m_bConnect						= FALSE;
	m_bIsRunning					= FALSE;

	m_stCPU.m_hQuery				= NULL;
	m_stHandle.m_hQuery				= NULL;
}

CModuleControl::~CModuleControl()
{
	Destroy();
}

BOOL CModuleControl::Create( UINT32 ui32ServerIp, UINT16 ui16Port )
{
	m_ui32ServerIP			= ui32ServerIp;
	m_ui16ServerPort		= ui16Port;
	m_bConnect				= FALSE;

	m_ui32LastConnectTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime		= i3ThreadTimer::GetServerTime();
	m_ui32LastInfoTime		= 0;
	
	m_i32PerformRevisionTime	= i3ThreadTimer::GetServerTime();

	// TCP Connect
	if( FALSE == i3NetworkClientSocket2::OnCreate(m_ui32ServerIP, m_ui16ServerPort ) )		return FALSE;

	// Therad Create
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , NULL, 0, (DWORD *)&m_ui32ThreadID);
	if( NULL == m_hThread )	return FALSE;
	//Set Thread Priority
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_CONTROL) )			return FALSE;

	return TRUE;
}

void CModuleControl::Destroy()
{
	m_bIsRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	if( m_stCPU.m_hQuery )
	{
		PdhCloseQuery ( m_stCPU.m_hQuery );
		m_stCPU.m_hQuery	= NULL;
	}
	if( m_stHandle.m_hQuery )
	{
		PdhCloseQuery ( m_stHandle.m_hQuery );
		m_stHandle.m_hQuery = NULL;
	}

	i3NetworkClientSocket2::OnDestroy();
}

BOOL CModuleControl::InitSystemLog()
{
	PDH_STATUS		pdhStatus;

	//Create CPU Speed
	pdhStatus = PdhOpenQuery( NULL, NULL, &m_stCPU.m_hQuery );
	if( ERROR_SUCCESS != pdhStatus )		goto e_Fail;

	pdhStatus = PdhAddCounter( m_stCPU.m_hQuery, "\\Processor(_Total)\\% Processor Time", 0, &m_stCPU.m_hCounter );	
	if( ERROR_SUCCESS != pdhStatus )		goto e_Fail;

	pdhStatus = PdhCollectQueryData( m_stCPU.m_hQuery );
	if( ERROR_SUCCESS != pdhStatus )		goto e_Fail;

	//Create Handle Count 
	pdhStatus = PdhOpenQuery( NULL, NULL, &m_stHandle.m_hQuery );
	if( ERROR_SUCCESS != pdhStatus )		goto e_Fail;

	pdhStatus = PdhAddCounter( m_stHandle.m_hQuery, "\\Process(_Total)\\Handle Count", 0, &m_stHandle.m_hCounter );	
	if( ERROR_SUCCESS != pdhStatus )		goto e_Fail;

	pdhStatus = PdhCollectQueryData( m_stHandle.m_hQuery );
	if( ERROR_SUCCESS != pdhStatus )		goto e_Fail;

	return TRUE;

e_Fail:
	
	if( m_stCPU.m_hQuery )
	{
		PdhCloseQuery ( m_stCPU.m_hQuery );
		m_stCPU.m_hQuery	= NULL;
	}
	if( m_stHandle.m_hQuery )
	{
		PdhCloseQuery ( m_stHandle.m_hQuery );
		m_stHandle.m_hQuery = NULL;
	}

	return FALSE;
}

INT32 CModuleControl::PacketParsing(char * pPacket, INT32 iPacketSize)
{
	//예외 처리 1: [정상]패킷 사이즈가 헤더보다 작으면 패킷을 기다린다
	if( iPacketSize < I3_PACKET_HEADER_SIZE )		return 0;

	NET_I3_PACKET_BUFFER* pBuffer = (NET_I3_PACKET_BUFFER*)pPacket;
	// I3 패킷은 실제 데이타 사이즈만 전달한다. Headr Size 추가.
	pBuffer->_ui16TotalSize = pBuffer->_ui16DataSize + I3_PACKET_HEADER_SIZE;

	// 
	if( iPacketSize < pBuffer->_ui16TotalSize )		return 0;

	//예외 처리 3: [비정상] 패킷 사이즈가 너무 크다.
	if( pBuffer->_ui16TotalSize > sizeof(NET_I3_PACKET_BUFFER))
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"I3 Packet size Error TotalSize = %d TotalDatasize = %d ProtoclID = %d", pBuffer->_ui16TotalSize, pBuffer->_ui16DataSize, pBuffer->_ui16Protocol);
		m_bConnect = FALSE;
		OnDestroy();
		return pBuffer->_ui16TotalSize;
	}

	INT32	_i32CurrentReadPosition = 0;

	switch( pBuffer->_ui16Protocol )
	{
		case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS:
			//처음 보내는 패킷을 보낸다. 
			{
				
				INT32	i32ConnectType	= 1;	//ASC_CONNECT_TYPE_SERVER;	//Server
				UINT8	ui8ServerType	= 9;	//ASC_SERVER_SIA//Game Server Type 
				UINT32	ui32UpdateTime;

				pBuffer->ReadData( &ui32UpdateTime,					sizeof(UINT32),		&_i32CurrentReadPosition );
				pBuffer->ReadData( &g_pConfig->m_ControlVersion,	sizeof(SS_VERSION), &_i32CurrentReadPosition );

				NET_I3_PACKET_BUFFER SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ );
				SendPacket.WriteData( &i32ConnectType,				sizeof(INT32) );
				SendPacket.WriteData( &ui8ServerType,				sizeof(UINT8) );
				SendPacket.WriteData( &g_pConfig->m_Version,		sizeof(SS_VERSION) );
				SendPacket.WriteData( &g_pConfig->m_ui16Port,		sizeof(UINT16) );
				SendPacketMessage( (char* )&SendPacket,				SendPacket._ui16TotalSize );
			}
			break;
		case PROTOCOL_ASC_CONFIRMATION_ACK:
			{
				m_bConnect = TRUE;

				switch ( g_eServerState )
				{
				case SERVER_STATE_START :	// Control 서버 Reconnect된 상태
					g_pModuleControl->ServerInit( EVENT_ERROR_SUCCESS );
					// break; // 주석이 맞음.
				default :
					m_ui32LastConnectTime	= i3ThreadTimer::GetServerTime();
					m_ui32LastSendTime		= i3ThreadTimer::GetServerTime(); 
					return pBuffer->_ui16TotalSize;

				case SERVER_STATE_LOADING :
					break;
					
				}


				if( SERVER_STATE_START == g_eServerState )
				{	// Control 서버 Reconnect
					g_pModuleControl->ServerInit( EVENT_ERROR_SUCCESS );
					break;
				}
				if( SERVER_STATE_LOADING != g_eServerState )	break;

				// Content on/off state
				ContentsInfo aContentsList[CONTENTS_COUNT];
				pBuffer->ReadData( aContentsList, sizeof(aContentsList), &_i32CurrentReadPosition );
				g_pConfig->InsertContents( aContentsList );

				g_eServerState = SERVER_STATE_INIT;
			}
			break;
		case PROTOCOL_ASC_DATABASE_CONFIG_ACK:
			{
				INT32 i32DBCount = 0;
				pBuffer->ReadData(&i32DBCount, sizeof(i32DBCount), &_i32CurrentReadPosition);

				DATABASE_CONFIG	 stDatabaseConfig[MAX_DATABASE_COUNT];
				pBuffer->ReadData(stDatabaseConfig, UINT16(sizeof(DATABASE_CONFIG) * i32DBCount), &_i32CurrentReadPosition);

				for( INT32 i = 0; i < i32DBCount; ++i )
					g_pConfig->InsertDatabaseConfig(&stDatabaseConfig[i]);
			}
			break;
		case PROTOCOL_BASE_HEART_BIT_ACK:
			{
			}
			break;
		default:
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"I3 Unknow Packet TotalSize = %d TotalDatasize = %d ProtoclID = %d", pBuffer->_ui16TotalSize, pBuffer->_ui16DataSize, pBuffer->_ui16Protocol);
			}
			break;
	}

	m_ui32LastConnectTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime		= i3ThreadTimer::GetServerTime();

	return pBuffer->_ui16TotalSize;
}

void	CModuleControl::OnWorking()
{
	g_pLog->AddLogThread( L"CONT", 0, m_ui32ThreadID );

	m_bIsRunning = TRUE;
	while( m_bIsRunning ) 
	{
		if(!IsConnected() )
		{
			m_bConnect = FALSE;
			i3NetworkClientSocket2::OnDestroy();
			Sleep(1000);
			if( i3NetworkClientSocket2::OnCreate( m_ui32ServerIP, m_ui16ServerPort ) )
			{
				m_ui32LastConnectTime	= i3ThreadTimer::GetServerTime();
				m_ui32LastSendTime		= i3ThreadTimer::GetServerTime();
				m_ui32LastInfoTime		= 0;
			}
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NETWORK, L"Reconnect Control Server[Socket]" );
		}

		SelectEvent();

		if( (m_ui32LastConnectTime + CONTROL_SERVER_CONNECT_TIME) < i3ThreadTimer::GetServerTime() )
		{	// 재접속 요청

			m_bConnect = FALSE;
			i3NetworkClientSocket2::OnDestroy();
			if( i3NetworkClientSocket2::OnCreate( m_ui32ServerIP, m_ui16ServerPort ) )
			{
				m_ui32LastConnectTime	= i3ThreadTimer::GetServerTime();
				m_ui32LastSendTime		= i3ThreadTimer::GetServerTime();
				m_ui32LastInfoTime		= 0;
			}
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NETWORK, L"Reconnect Control Server[Packet]" );
		}

		if( m_bConnect )
		{
			if( (m_ui32LastSendTime + CONTROL_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
			{	// Hart Bit
				m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
				NET_I3_PACKET_BUFFER SendPacket( PROTOCOL_BASE_HEART_BIT_REQ );
				SendPacketMessage( (char* )&SendPacket, SendPacket._ui16TotalSize );
			}


			if( (m_ui32LastInfoTime + CONTROL_SERVER_INFO_TIME) < i3ThreadTimer::GetServerTime() )
			{	// Server Info
				PDH_STATUS				pdhStatus;
				DWORD					ctrType;
				PDH_FMT_COUNTERVALUE	fmtValue;

				INT32					i32CPUCount		= 0;
				INT32					i32HandleCount	= 0;
				if( m_stCPU.m_hQuery )
				{	// CPU 점유율
					pdhStatus = PdhCollectQueryData( m_stCPU.m_hQuery );
					pdhStatus = PdhGetFormattedCounterValue( m_stCPU.m_hCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue );
					i32CPUCount		= (UINT32)fmtValue.doubleValue;
				}
				if( m_stHandle.m_hQuery )
				{	// Handle Count 
					pdhStatus = PdhCollectQueryData( m_stHandle.m_hQuery );
					pdhStatus = PdhGetFormattedCounterValue ( m_stHandle.m_hCounter, PDH_FMT_DOUBLE, &ctrType, &fmtValue);
					i32HandleCount	= (INT32)fmtValue.doubleValue; 
				}

				if( i32CPUCount <= 95 ) m_i32PerformRevisionTime			= i3ThreadTimer::GetServerTime();
				if( (m_i32PerformRevisionTime + 30) < i3ThreadTimer::GetServerTime() ) g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_PERFOMANCE, L"[LOG]Perfomance Limit Over CPU" );

				P_SERVER_STATE_REQ ServerInfoPacket;
				i3mem::FillZero( &ServerInfoPacket, sizeof(P_SERVER_STATE_REQ) );

				if ( SERVER_STATE_START == g_eServerState )
				{
					ServerInfoPacket._ui32Active	= SERVER_ACTIVE_STATE_CONNECT;
				}
				else
				{
					ServerInfoPacket._ui32Active	= SERVER_ACTIVE_STATE_LOADING;
				}
				ServerInfoPacket._i32Cpu			= i32CPUCount;
				ServerInfoPacket._i32Handle			= i32HandleCount;

				//for( INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
				//{	
				//	ServerInfoPacket._ui32WarningInfo[i] = g_pLog->GetWarningInfo(i);
				//}

				NET_I3_PACKET_BUFFER SendPacket( PROTOCOL_ASC_STATE_REQ );	
				SendPacket.WriteData( &ServerInfoPacket, sizeof(P_SERVER_STATE_REQ) );
				SendPacketMessage( (char* )&SendPacket, SendPacket._ui16TotalSize );

				m_ui32LastInfoTime = i3ThreadTimer::GetServerTime();
			}
		}

		::WaitForSingleObject( m_hThread, 1 );
	}
}

BOOL CModuleControl::ServerInit( INT32 i32Result )
{
	NET_I3_PACKET_BUFFER Packet( PROTOCOL_ASC_SERVER_LOADING_RESULT );
	Packet.WriteData( &i32Result,	sizeof(INT32) );
	SendPacketMessage( (char*)&Packet, Packet._ui16TotalSize );

	return TRUE;
}