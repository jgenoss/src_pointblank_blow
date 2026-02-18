#include "pch.h"
#include "ModuleKnock.h"
#include "ModuleKnockSoket.h"

I3_CLASS_INSTANCE( CModuleKnock, i3Thread );

CModuleKnock * g_pModuleKnock		= NULL;

#define KNOCK_RECONNECT_TIME			1000 *	30	//MS
#define KNOCK_CONNECT_TIMEOUT			10
CModuleKnock::CModuleKnock(void)
{
	m_bRunning						= FALSE;
	g_pModuleKnock					= NULL;
}


CModuleKnock::~CModuleKnock(void)
{
	OnDestroy(); 
}

BOOL CModuleKnock::OnCreate()
{
	BOOL bRv = TRUE;
	//Create Socket 
	m_pKnockSocket = CModuleKnockSoket::NewObject();		
	bRv = m_pKnockSocket->OnCreate( g_pControl->m_strKnockServerAddress, g_pControl->m_ui16KnockServerPort, m_i32LogIdx ); 	
	if(bRv)
	{
		m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
	}
	else
	{
		m_pKnockSocket->OnDestroy();
		return FALSE;
	}

	//----------------------------------------------------
	m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
	m_pKnockSocket->m_ui32LastRecvTime = m_ui32LastSendTime + 1;

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleKnock", 0, 4096, NULL, PRIORITY_NORMAL) )	return FALSE;
	
	m_ui32SystemInfoSendTime = i3ThreadTimer::GetServerTime();

	return bRv; 
}


void CModuleKnock::OnDestroy(void)
{
	//----------------------------------------------------
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pKnockSocket );

	m_pdhPerfmon.Destroy();

	return; 
}

INT32 CModuleKnock::InitSystemLog()
{
	m_pdhPerfmon.Create( );

	return SERVER_ERROR_SYSTEMLOG;
}

//MODULE_KNOCK_THREAD_IDX
UINT32 CModuleKnock::OnRunning( void * pUserData)
{
	m_i32LogIdx = g_pLog->AddLogThread(L"KNOCK", 0, m_ThreadID);

	m_bRunning	= TRUE;

	while( m_bRunning )
	{
		//-------------------------------------------------------------------------------
		//1. Connect Check 
		/*if(!m_pKnockSocket->IsConnect())
		{
			m_pKnockSocket->OnDestroy(); 
			if( m_pKnockSocket->OnCreate( g_pControl->m_strKnockServerAddress, g_pControl->m_ui16KnockServerPort, m_i32LogIdx ) )
			{
				m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
				g_pLog->WriteLog( m_i32LogIdx, "Success ReConnect KnockServer 1" );
			}
			else
			{
				m_pKnockSocket->OnDestroy();

				g_pLog->WriteLog( m_i32LogIdx, "Fail ReConnect KnockServer 1" );
				::Sleep( KNOCK_RECONNECT_TIME );
			}

			m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
			m_pKnockSocket->m_ui32LastRecvTime = m_ui32LastSendTime + 1;

			::WaitForSingleObject( m_Handle, 1);
			continue; 
		}*/

		//-------------------------------------------------------------------------------
		//2. Send Message 
		if( m_pKnockSocket->IsActive() )
		{

			//2-5시스템 정보 패킷 보내기
			if( (m_ui32SystemInfoSendTime + m_pKnockSocket->m_ui32KnockSendTime) < i3ThreadTimer::GetServerTime() )
			{
				/*INT32 i32Ppwt = m_pdhPerfmon.CheckAllPerformWarning();
				if( i32Ppwt != PWT_NO ) g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_PERFOMANCE, L"Perfomance Limit Over Perform Warning Type : %d", i32Ppwt );*/
		
				char* strMessage;
				UINT32 ui32MessageSize;
				i3NetworkPacket SendPacket( PROTOCOL_KNOCK_SEND_WARNING_MESSAGE );

				for(INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
				{

					if( 0 == g_pControl->m_rbWarningMessage[i].GetBufferCount() )
					{
						strMessage = "";
					}
					else
					{
						strMessage = (char*)g_pControl->m_rbWarningMessage[i].Pop();
						g_pControl->m_rbWarningMessage[i].PopIdx();
					}

					ui32MessageSize = i3String::Length(strMessage);
					SendPacket.WriteData( &ui32MessageSize, sizeof(UINT32) );
					SendPacket.WriteData( strMessage,		(UINT16)ui32MessageSize );
				}

				SendPacket.WriteData( &g_pControl->m_i32CCUMaxCount_Day, sizeof(INT32) );
				SendPacket.WriteData( &g_pControl->m_i32CCUNowCount, sizeof(INT32) );

				m_pKnockSocket->SendPacketMessage(&SendPacket);
				m_ui32SystemInfoSendTime = i3ThreadTimer::GetServerTime();
			}
		} 
		else 
		{
			//Connect Req Fail
			if( m_ui32ConnectTime + KNOCK_CONNECT_TIMEOUT < i3ThreadTimer::GetServerTime())
			{
				m_pKnockSocket->OnDestroy(); 
				if( m_pKnockSocket->OnCreate( g_pControl->m_strKnockServerAddress, g_pControl->m_ui16KnockServerPort, m_i32LogIdx ) )
				{
					m_ui32ConnectTime = i3ThreadTimer::GetServerTime();
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"Success ReConnect KncokServer 3" );

				}
				else
				{
					m_pKnockSocket->OnDestroy();	
					g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"Fail ReConnect KnockServer3" );
					::Sleep(KNOCK_RECONNECT_TIME);
				}
			}
		}

		//-------------------------------------------------------------------------------
		//3. Recv Buffer
		m_pKnockSocket->SelectEvent();

		::WaitForSingleObject( m_Handle, 1 );
	}
	return 0; 
}

void CModuleKnock::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pKnockSocket->SendPacketMessage(SendPacket);
}

BOOL CModuleKnock::GetRunning()
{
	return m_bRunning;
}
