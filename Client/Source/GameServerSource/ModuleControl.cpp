#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "RoomBase.h"
#include "RoomManager.h"
#include "UserManager.h"
#include "Room.h"

I3_CLASS_INSTANCE( CModuleControl, i3Thread );

CModuleControl * g_pModuleControl = NULL; 
UINT8			g_ui8Active;

//////////////////////////////////////////////////////////////
//CONTROL
#define CONTROL_MAX_SEND_IDX_COUNT				2000000000		
#define CONTROL_RECONNECT_TIME					10
#define CONTROL_SERVER_HEART_BIT				5
#define CONTROL_SERVER_SERVERINFO				5
#define CONTROL_SERVER_HEART_BIT_REQ_TIME		70
#define CONTROL_SEND_BUFFER_COUNT				100
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CONTROL_CHECKVALID_IDX		if( !m_pControlSocket->IsActive())				return FALSE; \
	SERVER_NET_BUFFER	* pNetBuffer = NULL; \
	switch( i32WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sControlMainMessage;			break; \
	case	MODULE_CONTROL_THREAD_IDX	: I3ASSERT_0;									break; \
	case	MODULE_TRANS_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_CLAN_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_MESSENGER_THREAD_IDX	: I3ASSERT_0;									break; \
	default								: pNetBuffer = &m_pControlUserMessage[i32WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CONTROL_MAX_SEND_IDX_COUNT )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleControl WriteBuffer FULL", WARNING_NETWORK );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CONTROL_SEND_BUFFER_COUNT )\
{\
	g_pModuleLogFile->Write_M_Log(i32WorkIdx, NULL,"ModuleControl WriteBuffer Overflow", WARNING_NETWORK );\
	return FALSE; \
}\
	INT32 i32WriteIdx = pNetBuffer->_WritePos % CONTROL_SEND_BUFFER_COUNT;


CModuleControl::CModuleControl(void)
{
	m_bRunning						= FALSE;
	m_ui32ThreadCount				= 0;
  	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pControlSocket				= NULL;
	m_ui32ControlConnectTime		= 0; 
	m_ui32ControlSystemInfoSendTime	= 0;
	m_pControlUserMessage			= NULL;
	i3mem::FillZero( &m_sControlMainMessage, sizeof( SERVER_NET_BUFFER ) );

	m_stCPU.m_hQuery				= NULL;
	m_stHandle.m_hQuery				= NULL;
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
	m_pControlUserMessage = (SERVER_NET_BUFFER*) i3MemAlloc(sizeof(SERVER_NET_BUFFER) * m_ui32ThreadCount); 
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
	if( FALSE == m_pControlSocket->OnCreate(g_pContextMain->m_strControlServerIp, g_pContextMain->m_ui16ControlServerPort) )	return FALSE;

	m_ui32ControlConnectTime			= g_pContextMain->GetServerTime();
	m_ui32ControlSystemInfoSendTime	= g_pContextMain->GetServerTime();

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

	return; 
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
	if( m_ui32ControlConnectTime + CONTROL_RECONNECT_TIME > g_pContextMain->GetServerTime() )	return 1;
	m_ui32ControlConnectTime = g_pContextMain->GetServerTime();

	m_pControlSocket->OnDestroy(); 
	if( m_pControlSocket->OnCreate(g_pContextMain->m_strControlServerIp, g_pContextMain->m_ui16ControlServerPort) )
	{
		m_ui32ControlSystemInfoSendTime	= g_pContextMain->GetServerTime();

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
			g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL,"ReConnect ControlServer 1 Success", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL,"ReConnect ControlServer 1 Fail", WARNING_NETWORK );
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
			SERVER_NET_BUFFER	* pUserMessage = &m_pControlUserMessage[ i ]; 
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
		if( (m_pControlSocket->m_ui32LastSendTime + CONTROL_SERVER_SERVERINFO) < g_pContextMain->GetServerTime() )
		{
			i3NetworkPacket SendPacket( PROTOCOL_BASE_GAME_SERVER_STATE_REQ ); 
			m_pControlSocket->SendPacketMessage(&SendPacket); 
			m_pControlSocket->m_ui32LastSendTime = g_pContextMain->GetServerTime();
		}
			
		//2-5시스템 정보 패킷 보내기
		if( g_eServerState >= SERVER_STATE_START && (m_ui32ControlSystemInfoSendTime + m_pControlSocket->m_ui32ControlSendTime) < g_pContextMain->GetServerTime() )
		{
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
			PacketData._ui32Active = g_ui8Active;
			PacketData._iCpu	= i32CPUCount;
			PacketData._iHandle = i32HandleCount;
			PacketData._iArg0	= i32Arg1;
			PacketData._iArg1	= i32Arg2;
			PacketData._iArg2	= 0;
			PacketData._iArg3	= 0;
			PacketData._iArg4	= 0;
			PacketData._ui32WarningInfo = g_pContextMain->GetServerStateInfo();

			i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_REQ );
			SendPacket.WriteData( &PacketData,									sizeof(P_SERVER_STATE_REQ) );
			SendPacket.WriteData( i32UserCount,									sizeof(INT32) * GAME_CHANNEL_COUNT );
			SendPacket.WriteData( &g_pUserManager->m_ui32RecvHeartBit2Count,	sizeof(UINT32) );	// HeartBit2 

			m_pControlSocket->SendPacketMessage(&SendPacket);
			m_ui32ControlSystemInfoSendTime = g_pContextMain->GetServerTime();
		}
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pControlSocket->SelectEvent(); 		

	//-------------------------------------------------------------------------------
	//4. Check Connect State
	if( (m_pControlSocket->m_ui32LastRecvTime + CONTROL_SERVER_HEART_BIT_REQ_TIME) < g_pContextMain->GetServerTime() )
	{
		// Control 서버에 정상 종료 패킷 전송.
		i3NetworkPacket SendPacket( PROTOCOL_ASC_NORMAL_DISCONNECT_REQ );
		m_pControlSocket->SendPacketMessage( &SendPacket );

		Sleep(1000);

		INT32 i32Rv = _ControlConnect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL,"ReConnect ControlServer 2 Success", WARNING_NETWORK );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL,"ReConnect ControlServer 2 Fail", WARNING_NETWORK );
		}
	}

	return i32SendCompleteCount;
}

// ----------------------------------------------------------------------------------------
// Battle Server
// ----------------------------------------------------------------------------------------
BOOL CModuleControl::BattleCreate(INT32 i32WorkIdx, TYPE_ROOM_INFO * pRoomInfo, UINT32 ui32StartUser)
{
	CONTROL_CHECKVALID_IDX;

	UINT32	ui32Temp; 

	P_BATTLE_ROOM_CREATE_REQ ReqPacket; 
	i3mem::FillZero( &ReqPacket, sizeof( P_BATTLE_ROOM_CREATE_REQ ) );
	ReqPacket._RoomInfo._cChannel	= (UINT8)pRoomInfo->_ui32ChannelNum;
	ReqPacket._RoomInfo._sRoom		= (UINT16)pRoomInfo->_InfoBasic._Idx;

	ReqPacket._iStage				= pRoomInfo->_InfoBasic._StageID;
	ReqPacket._StageMask			= pRoomInfo->_InfoBasic._StageMask; 
	ReqPacket._iStartUser			= ui32StartUser;
	ReqPacket._ui8AvailableWeapon	= pRoomInfo->_InfoBasic._WeaponFlag;
	
	//Create Packet 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_CREATE_REQ); 
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ReqPacket,	sizeof(P_BATTLE_ROOM_CREATE_REQ));

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( (ui32StartUser & (1UL << i))) 
		{
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pRoomInfo->_SlotInfo[i]._Item, sizeof(UINT32) ); 

			ui32Temp = pRoomInfo->_pSession[i]->m_UseCharaList.GetCharaID( (TEAM_TYPE)(i % 2) ); 
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32Temp, sizeof(UINT32) ); 

			ui32Temp = pRoomInfo->_pSession[i]->m_UseCharaList.GetHairNum();
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32Temp, sizeof(UINT32) ); 

			ui32Temp = pRoomInfo->_pSession[i]->m_UseCharaList.GetItemNum();
			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &ui32Temp, sizeof(UINT32) ); 

			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pRoomInfo->_pSession[i]->m_aui8EquipUserTitle, sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE ); 

			pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData( &pRoomInfo->_pSession[i]->m_i64UID, sizeof(INT64) );

			
		}
	}
	
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleControl::BattleDestroy(INT32 i32WorkIdx, BATTLE_ROOM_INFO * pRoomInfo, UINT32 ui32GroupIdx )
{
	CONTROL_CHECKVALID_IDX;

	P_BATTLE_ROOM_DESTROY_REQ Packet;
	i3mem::Copy( &Packet._RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO) ); 
	Packet._GroupIdx = ui32GroupIdx; 

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_DESTROY_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&Packet,	sizeof(P_BATTLE_ROOM_DESTROY_REQ));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleControl::BattleUserEnter(INT32 i32WorkIdx, BATTLE_ROOM_INFO * pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 ui32Item, CUserSession* pSession)
{	
	CONTROL_CHECKVALID_IDX;

	P_BATTLE_USER_ENTER_REQ ReqPacket;
	ReqPacket._wGroupIdx	= ui32GroupIdx; 
	ReqPacket._cSlotIdx		= ui8SlotIdx;
	ReqPacket._iItem		= ui32Item;
	
	ReqPacket._ui32Character= pSession->m_UseCharaList.GetCharaID( (TEAM_TYPE)(ui8SlotIdx % 2) );
	ReqPacket._ui32Helmet	= pSession->m_UseCharaList.GetHairNum();
	ReqPacket._ui32Beret	= pSession->m_UseCharaList.GetItemNum();
	
	i3mem::Copy( ReqPacket._aui8Designation, pSession->m_aui8EquipUserTitle, sizeof( UINT8 )*MAX_EQUIP_USER_TITLE );
	i3mem::Copy( &ReqPacket._RoomInfo , pRoomInfo, sizeof(BATTLE_ROOM_INFO)); 
	ReqPacket._i64UID		= pSession->m_i64UID;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_USER_ENTER_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ReqPacket, sizeof(P_BATTLE_USER_ENTER_REQ));

	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleControl::BattleUserLeave(INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, CUserSession* pSession)
{
	CONTROL_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_USER_LEAVE_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(pRoomInfo,		sizeof(BATTLE_ROOM_INFO));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui32GroupIdx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&ui8SlotIdx,		sizeof(UINT8));

	pNetBuffer->_WritePos++;

	return TRUE; 

}

BOOL CModuleControl::BattleRoundStart(INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, TYPE_ROOM_INFO * pTypeRoomInfo, bool bTeamChange)
{
	CONTROL_CHECKVALID_IDX;

	//Set Data 
	P_BATTLE_ROUND_REQ Packet; 
	i3mem::FillZero( &Packet, sizeof( P_BATTLE_ROUND_REQ ) );
	i3mem::Copy( &Packet._RoomInfo , pRoomInfo, sizeof(BATTLE_ROOM_INFO) ); 
	i3mem::Copy( &Packet._iDinoInfo, pTypeRoomInfo->_aui32DInoCharInfo, (sizeof(UINT32) * SLOT_MAX_COUNT) ); 
	// 공룡인 아닌 유저들은 인간 캐릭터 정보를 넣어줌
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( SLOT_STATE_READY > pTypeRoomInfo->_SlotInfo[i]._State ) continue;
		
		// 원래코드
		//if( Packet._iDinoInfo[i] > 0 )							continue;
		//Packet._iDinoInfo[i]	= pTypeRoomInfo->_pSession[i]->m_UseCharaList.GetCharaID( (TEAM_TYPE)(i % 2) ); 

		//수정 코드(yjha)
		if( Packet._iDinoInfo[i] == 0 )
		{
			TEAM_TYPE Team = (TEAM_TYPE)(i % 2);
#ifdef USE_BOMB_AUTO_TEAM_CHANGE
			if ( bTeamChange )
			{
				Team = (TEAM_TYPE)!Team;
			}
#endif
			Packet._iDinoInfo[i] = pTypeRoomInfo->_pSession[i]->m_UseCharaList.GetCharaID( Team );
		}

		Packet._aui32Item[i]	= pTypeRoomInfo->_SlotInfo[i]._Item;
		Packet._aui32Helmet[i]	= pTypeRoomInfo->_pSession[i]->m_UseCharaList.GetHairNum();
		Packet._aui32Beret[i]	= pTypeRoomInfo->_pSession[i]->m_UseCharaList.GetItemNum();
		i3mem::Copy( Packet._aui8Designation[i], pTypeRoomInfo->_pSession[i]->m_aui8EquipUserTitle, sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE );
	}

	Packet._GroupIdx	= ui32GroupIdx; 
	Packet._iRound		= pTypeRoomInfo->_InfoAdd._NowRoundCount; 
#ifdef USE_BOMB_AUTO_TEAM_CHANGE
	Packet._bTeamChange	= bTeamChange;
#endif

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_ROUND_START_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&Packet,	sizeof(P_BATTLE_ROUND_REQ));

	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleControl::BattleChangeDinoInfo(INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT16 ui16Slot, TYPE_ROOM_INFO* pInfo )
{
	CONTROL_CHECKVALID_IDX;

	//Set Data 
	P_BATTLE_M_DINO_INFO_REQ Packet; 
	i3mem::Copy( &Packet._RoomInfo , pRoomInfo, sizeof(BATTLE_ROOM_INFO) ); 
	Packet._GroupIdx	= ui32GroupIdx; 
	Packet._iSlotIdx	= ui16Slot; 
	Packet._iDinoInfo	= pInfo->_aui32DInoCharInfo[ui16Slot];
	Packet._ui32Item	= pInfo->_SlotInfo[ui16Slot]._Item;
	Packet._ui32Helmet	= pInfo->_pSession[ui16Slot]->m_UseCharaList.GetHairNum();
	Packet._ui32Beret	= pInfo->_pSession[ui16Slot]->m_UseCharaList.GetItemNum();
	i3mem::Copy( Packet._aui8Designation, pInfo->_pSession[ui16Slot]->m_aui8EquipUserTitle, sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE );

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_M_DINO_INFO_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&Packet,	sizeof(P_BATTLE_M_DINO_INFO_REQ));

	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

// 야전상점에서 무기구매시 데디케이션 서버에 통보하기 위해 먼저 컨트롤 서버에 통보한다.
BOOL CModuleControl::BattleWeaponInfo(	INT32 i32WorkIdx,	BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, GC_PROTOCOL_SLOT_EQUIPMENT* pStruct )
{
	CONTROL_CHECKVALID_IDX;

	STRUCT_BATTLE_SLOT_EQUIPMENT_REQ Packet;
	i3mem::Copy( &Packet.m_RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO) );
	Packet.m_ui32GroupIdx	= ui32GroupIdx;
	Packet.m_ui8SlotIdx		= ui8SlotIdx;
	i3mem::Copy( &Packet.m_Struct, pStruct, sizeof(GC_PROTOCOL_SLOT_EQUIPMENT) );

	pNetBuffer->_ppNetMessage[i32WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[i32WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SLOT_EQUIPMENT_2LEVEL_REQ);
	pNetBuffer->_ppNetMessage[i32WriteIdx]->WriteData(&Packet, sizeof(STRUCT_BATTLE_SLOT_EQUIPMENT_REQ) );

	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleControl::BattleRespawn( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount )
{
	CONTROL_CHECKVALID_IDX;

	i3NetworkPacket* pPacket = pNetBuffer->_ppNetMessage[i32WriteIdx];

	P_BATTLE_RESPAWN_REQ Packet;
	i3mem::Copy( &Packet._RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO) );
	Packet._GroupIdx = ui32GroupIdx;
	Packet._cSlotIdx = ui8SlotIdx;
	i3mem::Copy( &Packet._pUseWeaponList, pui32WeaponList, sizeof(UINT32) * WEAPON_USAGE_COUNT);
	Packet._i32TotalRespawnIdx = i32RespawnIdx;
	Packet._i32RespawnCount = i32RespawnCount;

	pPacket->Clear();
	pPacket->SetProtocol(PROTOCOL_BATTLE_RESPAWN_2LEVEL_REQ);
	pPacket->WriteData(&Packet, sizeof(P_BATTLE_RESPAWN_REQ));

	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleControl::BattleGMPause( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 wGroupIdx, bool bPause )
{
	CONTROL_CHECKVALID_IDX;

	i3NetworkPacket* pPacket = pNetBuffer->_ppNetMessage[i32WriteIdx];

	pPacket->Clear();
	pPacket->SetProtocol( PROTOCOL_BATTLE_SERVER_GM_PAUSE_REQ );
	pPacket->WriteData( pRoomInfo,	sizeof( BATTLE_ROOM_INFO ) );
	pPacket->WriteData( &wGroupIdx,	sizeof( UINT32 ) );
	pPacket->WriteData( &bPause,	sizeof( bool ) );

	pNetBuffer->_WritePos++;

	return TRUE;
}

void CModuleControl::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pControlSocket->SendPacketMessage(SendPacket);
}