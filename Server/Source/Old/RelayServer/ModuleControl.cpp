#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "GroupMaker.h"
#include "LogFile.h"
#include "UdpServer.h"

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

CModuleControl * g_pModuleControl		= NULL; 

#define CONTROL_SEND_BUFFER_COUNT			100
#define CONTROL_RECONNECT_TIME				1000 *	30	//MS
#define CONTROL_SERVER_HEART_BIT			60
#define CONTROL_CONNECT_TIMEOUT				10
#define CONTROL_SERVER_HEART_BIT_REQ_TIME	120			//70초 동안 하트비트를 받지 못하면 연결을 다시한다. 
#define MODULE_CONTEXT_MAX_SEND_IDX_COUNT	2000000000	
#define RELAY_SEND_BUFFER_COUNT					100
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

#define RELAY_CHECKVALID_IDX		if( !m_pControlSocket->IsActive())return FALSE; \
	CONTROL_NET_BUFFER	* pNetBuffer = &m_SendBuffer; \
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > MODULE_CONTEXT_MAX_SEND_IDX_COUNT )\
{\
	WRITE_LOG_CONTROL("Relay Buffer FULL");\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= RELAY_SEND_BUFFER_COUNT )\
{\
	WRITE_LOG_CONTROL("Relay Buffer Overflow");\
	return FALSE; \
}\
	INT32 WriteIdx = pNetBuffer->_WritePos % RELAY_SEND_BUFFER_COUNT;


CModuleControl::CModuleControl(void)
{
	m_hKillEvent		= ::CreateEvent(NULL, false, false, NULL);		
}

CModuleControl::~CModuleControl(void)
{
	for(INT32 i = 0; i < CONTROL_SEND_BUFFER_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_SendBuffer._ppNetMessage[i] ); 
	}

	I3MEM_SAFE_FREE(m_SendBuffer._ppNetMessage); 

	::SetEvent( m_hKillEvent ); 
	::Sleep(1000);
	::CloseHandle( m_hKillEvent ); 
	OnDestroy(); 
}

BOOL CModuleControl::OnCreate()
{
	if(!g_pConfig->m_bControlActive)	return TRUE; 

	m_SendBuffer._ReadPos		= 0;
	m_SendBuffer._WritePos		= 0;
    m_SendBuffer._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CONTROL_SEND_BUFFER_COUNT);

	for(INT32 i = 0; i < CONTROL_SEND_BUFFER_COUNT; i++)
	{
		m_SendBuffer._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}

	BOOL Rv = TRUE; 
	//Create Socket 
	m_pControlSocket = CModuleControlSocket::NewObject(); 
	Rv = m_pControlSocket->OnCreate(g_pConfig->m_ControlServerIp, g_pConfig->m_ControlServerPort); 	
	if( FALSE == Rv)
	{
		m_pControlSocket->OnDestroy();
		WRITE_LOG_MAIN("Connect Fail ControlServer");
		return FALSE;
	}
	m_iConnectTime			= g_pConfig->GetServerTime();
	m_wUpdateCheck			= g_pConfig->GetServerTime();
	m_wUpdateCheckStartIdx	= 0;

	//Create Thread
	Rv = i3Thread::Create("ModuleControl", 0, 4096, NULL, PRIORITY_NORMAL);
	if(!Rv)	return FALSE; 
	WRITE_LOG_MAIN("[CModuleControl::OnCreate] CONNECT SUCCESS"); 

	return Rv; 
}

void CModuleControl::OnDestroy(void)
{
	if(!g_pConfig->m_bControlActive)return; 

	//----------------------------------------------------
	I3_SAFE_RELEASE( m_pControlSocket );

	return; 
}

INT32 CModuleControl::_ServerConnect()
{
	if( m_iConnectTime + CONTROL_CONNECT_TIMEOUT > g_pConfig->GetServerTime() )	return 1;
	m_iConnectTime = g_pConfig->GetServerTime();

	m_pControlSocket->OnDestroy(); 
	if( m_pControlSocket->OnCreate(g_pConfig->m_ControlServerIp, g_pConfig->m_ControlServerPort))
	{
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pControlSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

UINT32 CModuleControl::OnRunning( void * pUserData)
{
	INT32	iSendCompleteCount = 0; 
	BOOL	binfinityLoop = TRUE;
	INT32	i;
	INT32	SendCount;
	INT32	ReadIdx;

	while( binfinityLoop )
	{
		if( 0 == iSendCompleteCount && ::WaitForSingleObject( m_hKillEvent, 1) == WAIT_OBJECT_0 ) break;
		iSendCompleteCount = 0;

		//-------------------------------------------------------------------------------
		//1. Connect Check 
		if( FALSE == m_pControlSocket->IsConnect() )
		{
			INT32 i32Rv = _ServerConnect();
			if( EVENT_ERROR_SUCCESS == i32Rv )
			{
				WRITE_LOG_CONTROL("Success ReConnect ControlServer 1");
			}
			else if( EVENT_ERROR_FAIL == i32Rv )
			{
				WRITE_LOG_CONTROL("Fail ReConnect ControlServer 1");
			}
			continue; 
		}

		//-------------------------------------------------------------------------------
		//2. Send Message 
		if( m_pControlSocket->IsActive() )
		{
			//Main Thread 
			SendCount = m_SendBuffer._WritePos - m_SendBuffer._ReadPos; 
			for( i = 0; i < SendCount; i++)
			{
				ReadIdx = m_SendBuffer._ReadPos % CONTROL_SEND_BUFFER_COUNT; 
				m_pControlSocket->SendPacketMessage( m_SendBuffer._ppNetMessage[ReadIdx] ); 
				m_SendBuffer._ReadPos++; 
				iSendCompleteCount++;
			}

			//Send HeartBit Packet 
			if( ( m_pControlSocket->m_iHeartBitCheckTime + CONTROL_SERVER_HEART_BIT) < g_pConfig->GetServerTime() )
			{
				i3NetworkPacket SendPacket(PROTOCOL_BASE_HEART_BIT_REQ); 
				m_pControlSocket->SendPacketMessage(&SendPacket);
				m_pControlSocket->m_iHeartBitCheckTime = g_pConfig->GetServerTime();
			}

			//시스템 정보 패킷 보내기
			if( (m_iSystemInfoSendTime + m_pControlSocket->m_iControlSendTime) < g_pConfig->GetServerTime() )
			{
				INT32 i32RoomCount		= (INT32)g_pGroupMaker->GetGroupCount();
				INT32 i32PacketCount	= 0;
				if( SERVER_STATE_START == g_i32StartServer )
				{
					i32PacketCount = (INT32)g_pUdpServer->GetPacketCount_C();
				}

				P_SERVER_STATE_REQ PacketData;
				PacketData._iCpu	= g_pConfig->m_iCpuQuery; 
				PacketData._iHandle = g_pConfig->m_iHandleQuery; 
				PacketData._iArg0	= i32RoomCount; 
				PacketData._iArg1	= i32PacketCount; 
				PacketData._iArg2	= 0; 
				PacketData._iArg3	= 0; 
				PacketData._iArg4	= BATTLE_SERVER_TYPE_RELAY; 
				PacketData._ui32WarningInfo = 0; 

				i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_REQ );
				SendPacket.WriteData( &PacketData, sizeof(P_SERVER_STATE_REQ)); 
				
				m_pControlSocket->SendPacketMessage(&SendPacket);
				m_iSystemInfoSendTime = g_pConfig->GetServerTime(); 
			}
		}

		//-------------------------------------------------------------------------------
		//3. Recv Buffer
		m_pControlSocket->SelectEvent(); 		

		//-------------------------------------------------------------------------------
		//4. Check Connect State
		if( (m_pControlSocket->m_iLastRecvTime + CONTROL_SERVER_HEART_BIT_REQ_TIME) < g_pConfig->GetServerTime() )
		{
			INT32 i32Rv = _ServerConnect();
			if( EVENT_ERROR_SUCCESS == i32Rv )
			{
				WRITE_LOG_CONTROL("Success ReConnect ControlServer 21");
			}
			else if( EVENT_ERROR_FAIL == i32Rv )
			{
				WRITE_LOG_CONTROL("Fail ReConnect ControlServer 2");
			}
			continue; 
		}

		// Check Group Count
		if( SERVER_STATE_START == g_i32StartServer )
		{
			if( ( m_wUpdateCheck + UPDATE_CHECK_TIME ) < g_pConfig->GetServerTime() )
			{	// UPDATE_CHECK_TIME 시간만에 한번씩 체크한다.
				m_wUpdateCheck = g_pConfig->GetServerTime();

				g_pGroupMaker->OnUpdate_C( m_wUpdateCheckStartIdx );

				m_wUpdateCheckStartIdx	+= UPDATE_CHECK_JUMP_COUNT;
				m_wUpdateCheckStartIdx	%= g_pConfig->m_wGroupCount;
			}

			g_pUdpServer->SetPacketCount_C();
		}
	}
	return 0; 
}

BOOL CModuleControl::SendRelayRegisterGroupRequest( INT32 nResult, P_BATTLE_ROOM_CREATE_ACK * pAckInfo )
{
	RELAY_CHECKVALID_IDX

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear(); 
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_CREATE_ACK );
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &nResult,	sizeof(INT32) );
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( pAckInfo,	sizeof(P_BATTLE_ROOM_CREATE_ACK) );
	pNetBuffer->_WritePos++;

	return TRUE;
}
