#include "pch.h"
#include "ModuleCast.h"
#include "ModuleCastSocket.h"
// #include "ModuleLogFile.h"
#include "RoomBase.h"
#include "RoomManager.h"
#include "UserManager.h"
#include "Room.h"

I3_CLASS_INSTANCE( CModuleCast, i3Thread );

CModuleCast *	g_pModuleCast = NULL; 

//////////////////////////////////////////////////////////////
//CAST
#define CAST_MAX_SEND_IDX_COUNT				2000000000		
#define CAST_RECONNECT_TIME					10
#define CAST_SERVER_HEART_BIT				60
#define CAST_SERVER_SERVERINFO				5
#define CAST_SERVER_HEART_BIT_REQ_TIME		70
#ifdef I3_DEBUG
#define CAST_SEND_BUFFER_COUNT				30
#else
#define CAST_SEND_BUFFER_COUNT				100
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define CAST_CHECKVALID_IDX		if( !m_pSocket->IsActive())				return FALSE; \
	CONNECT_SERVER_NET_BUFFER	* pNetBuffer = NULL; \
	switch( WorkIdx )\
{\
	case	MAIN_THREAD_IDX				: pNetBuffer = &m_sMainMessage;			break; \
	case	MODULE_CONTROL_THREAD_IDX	: I3ASSERT_0;									break; \
	case	MODULE_CAST_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_TRANS_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_CLAN_THREAD_IDX		: I3ASSERT_0;									break; \
	case	MODULE_MESSENGER_THREAD_IDX	: I3ASSERT_0;									break; \
	default								: pNetBuffer = &m_pUserMessage[WorkIdx];	break; \
}\
	if( pNetBuffer->_WritePos == pNetBuffer->_ReadPos )\
{\
	pNetBuffer->_WritePos	= 0;\
	pNetBuffer->_ReadPos	= 0;\
}\
	if( pNetBuffer->_ReadPos > CAST_MAX_SEND_IDX_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleCast WriteBuffer FULL" );\
	return FALSE; \
}\
	if( (pNetBuffer->_WritePos - pNetBuffer->_ReadPos) >= CAST_SEND_BUFFER_COUNT )\
{\
	g_pLog->WriteLog( L"ModuleCast WriteBuffer Overflow" );\
	return FALSE; \
}\
	INT32 WriteIdx = pNetBuffer->_WritePos % CAST_SEND_BUFFER_COUNT;


CModuleCast::CModuleCast(void)
{
	m_bRunning				= FALSE;
	m_ui32ThreadCount		= 0;
  	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pSocket				= NULL;
	m_ui32ConnectTime		= 0; 
	m_pUserMessage			= NULL;
		
	i3mem::FillZero( &m_sMainMessage, sizeof( CONNECT_SERVER_NET_BUFFER ) );
}

CModuleCast::~CModuleCast(void)
{
	OnDestroy();
}

BOOL CModuleCast::OnCreate( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ThreadCount )
{
	m_ui32IP			= ui32IP;
	m_ui16Port			= ui16Port;
	m_ui32ThreadCount	= ui32ThreadCount; //Set Buffer 

	// Buffer Create
	// Main
	m_sMainMessage._ppNetMessage	= (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CAST_SEND_BUFFER_COUNT);
	for( UINT32 i = 0 ; i < CAST_SEND_BUFFER_COUNT ; i++ )
	{
		m_sMainMessage._ppNetMessage[i] = i3NetworkPacket::NewObject();
	}

	// UserSession
	m_pUserMessage = (CONNECT_SERVER_NET_BUFFER*) i3MemAlloc(sizeof(CONNECT_SERVER_NET_BUFFER) * m_ui32ThreadCount); 
	for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
	{
		m_pUserMessage[i]._ReadPos	= 0; 
		m_pUserMessage[i]._WritePos	= 0; 
		m_pUserMessage[i]._ppNetMessage = (i3NetworkPacket **)i3MemAlloc( sizeof(i3NetworkPacket *) * CAST_SEND_BUFFER_COUNT); 
		for( UINT32 j = 0 ; j < CAST_SEND_BUFFER_COUNT ; j++ )
		{
			m_pUserMessage[i]._ppNetMessage[j] = i3NetworkPacket::NewObject(); 
		}
	}

	//Socket
	m_pSocket = CModuleCastSocket::NewObject();
	if( FALSE == m_pSocket->OnCreate( m_ui32IP, m_ui16Port ) )	return FALSE;

	m_ui32ConnectTime		= i3ThreadTimer::GetServerTime();

	//Create Thread
	if( FALSE == i3Thread::Create("ModuleContextCast", 0, 4096, NULL, PRIORITY_HIGH) )								return FALSE;

	return TRUE;
}

void CModuleCast::OnDestroy(void)
{
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pSocket );

	if( m_sMainMessage._ppNetMessage )
	{
		for(UINT32 i = 0; i < CAST_SEND_BUFFER_COUNT; i++)
		{
			I3_SAFE_RELEASE( m_sMainMessage._ppNetMessage[i] ); 
		}

		I3MEM_SAFE_FREE(m_sMainMessage._ppNetMessage);
	}

	if( m_pUserMessage )
	{
		for( UINT32 i = 0 ; i < m_ui32ThreadCount ; i++ )
		{
			if( m_pUserMessage[i]._ppNetMessage )
			{
				for( UINT32 j = 0 ; j < CAST_SEND_BUFFER_COUNT ; j++)
				{
					I3_SAFE_RELEASE(m_pUserMessage[i]._ppNetMessage[j] ); 			
				}
				I3MEM_SAFE_FREE(m_pUserMessage[i]._ppNetMessage); 
			}
		}
		I3MEM_SAFE_FREE( m_pUserMessage );
	}

	return; 
}

UINT32 CModuleCast::OnRunning( void * pUserData)
{	
	g_pLog->AddLogThread( L"Cast", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	m_bRunning		= TRUE;	

	while( m_bRunning )
	{
		if( 0 < _Update() )	continue;

		// 한일이 없으면 1ms 휴식.
		::WaitForSingleObject( m_Handle, 1);
	}

	return 0; 
}

INT32 CModuleCast::_Connect()
{
	if( m_ui32ConnectTime + CAST_RECONNECT_TIME > i3ThreadTimer::GetServerTime() )	return 1;
	m_ui32ConnectTime = i3ThreadTimer::GetServerTime();

	m_pSocket->OnDestroy(); 
	if( m_pSocket->OnCreate( m_ui32IP, m_ui16Port ) )
	{
		return EVENT_ERROR_SUCCESS;
	}
	
	m_pSocket->OnDestroy();

	return EVENT_ERROR_FAIL;
}

INT32 CModuleCast::_Update(void) 
{
	INT32	i32SendCompleteCount = 0; 
	INT32	i32SendCount;
	INT32	i32ReadIdx;

	UINT32	i;

	//-------------------------------------------------------------------------------
	//1. Connect Check 
	if( FALSE == m_pSocket->IsConnect())
	{
		Sleep(1000);

		INT32 i32Rv = _Connect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"ReConnect CastServer 1 Success" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"ReConnect CastServer 1 Fail" );
		}
		return i32SendCompleteCount;
	}

	//-------------------------------------------------------------------------------
	//2. Send Message 
	if( m_pSocket->IsActive() )
	{
		//2-1 Main Thread 
		i32SendCount = m_sMainMessage._WritePos - m_sMainMessage._ReadPos; 
		if( 0 < i32SendCount )
		{
			i32ReadIdx = m_sMainMessage._ReadPos % CAST_SEND_BUFFER_COUNT; 
			m_pSocket->SendPacketMessage( m_sMainMessage._ppNetMessage[ i32ReadIdx ] ); 
			m_sMainMessage._ReadPos++; 
			i32SendCompleteCount++;
		}
		
		//2-3 User Thread
		for( i = 0; i < m_ui32ThreadCount ; i++ )
		{
			CONNECT_SERVER_NET_BUFFER	* pUserMessage = &m_pUserMessage[ i ]; 
			i32SendCount = pUserMessage->_WritePos - pUserMessage->_ReadPos; 
			if( 0 < i32SendCount )
			{
				i32ReadIdx = pUserMessage->_ReadPos % CAST_SEND_BUFFER_COUNT; 
				m_pSocket->SendPacketMessage( pUserMessage->_ppNetMessage[ i32ReadIdx ]); 
				pUserMessage->_ReadPos++; 
				i32SendCompleteCount++; 
			}
		}
	}

	//2-4 Check Send 패킷
	if( (m_pSocket->m_ui32LastSendTime + CAST_SERVER_HEART_BIT) < i3ThreadTimer::GetServerTime() )
	{
		i3NetworkPacket SendPacket( PROTOCOL_BASE_HEART_BIT_REQ ); 
		m_pSocket->SendPacketMessage(&SendPacket); 
		m_pSocket->m_ui32LastSendTime = i3ThreadTimer::GetServerTime();
	}

	//-------------------------------------------------------------------------------
	//3. Recv Buffer
	m_pSocket->SelectEvent();

	//-------------------------------------------------------------------------------
	//4. Check Connect State
	if( (m_pSocket->m_ui32LastRecvTime + CAST_SERVER_HEART_BIT_REQ_TIME) < i3ThreadTimer::GetServerTime() )
	{
		Sleep(1000);

		INT32 i32Rv = _Connect();
		if( EVENT_ERROR_SUCCESS == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_NETWORK, L"ReConnect CastServer 2 Success" );
		}
		else if( EVENT_ERROR_FAIL == i32Rv )
		{
			g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_NETWORK, L"ReConnect CastServer 2 Fail" );
		}
	}

	return i32SendCompleteCount;
}

// ----------------------------------------------------------------------------------------
// Battle Server
// ----------------------------------------------------------------------------------------
BOOL CModuleCast::BattleCreate(INT32 WorkIdx, TYPE_ROOM_INFO * pRoomInfo, UINT32 ui32StartUser)
{
	CAST_CHECKVALID_IDX;

	P_BATTLE_ROOM_CREATE_REQ ReqPacket; 
	i3mem::FillZero( &ReqPacket, sizeof( P_BATTLE_ROOM_CREATE_REQ ) );
	ReqPacket._RoomInfo._cChannel	= (UINT8)pRoomInfo->_ui32ChannelNum;
	ReqPacket._RoomInfo._sRoom		= (UINT16)pRoomInfo->_InfoBasic._Idx;

	ReqPacket._iStage				= pRoomInfo->_InfoBasic._StageID;	
	ReqPacket._iStartUser			= ui32StartUser;
	ReqPacket._ui8AvailableWeapon	= pRoomInfo->_InfoBasic._WeaponFlag;
	ReqPacket._SubType				= pRoomInfo->_InfoAdd._SubType;
	ReqPacket._ui8InfoFlag			= pRoomInfo->_InfoBasic._InfoFlag;
	ReqPacket._bCanSoloPlay			= (ROOM_READY_ONE_TEAM == pRoomInfo->m_eReadyOption && 1 == pRoomInfo->m_i8PersonAtLeast);
	ReqPacket.m_ui8AICount			= pRoomInfo->m_i8enemyAiCount;
	ReqPacket._ui32BitHackOffUser	= pRoomInfo->_ui32HackOffUser;
	ReqPacket.m_ui8MaxPerson		= g_pContextMain->GetMaxPerson(pRoomInfo->_InfoBasic._StageID);
	
	//Create Packet 
	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol( PROTOCOL_BATTLE_SERVER_CREATE_REQ );
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &ReqPacket,	sizeof(P_BATTLE_ROOM_CREATE_REQ) );

	for( INT32 i = 0; i < SLOT_MAX_COUNT ; i++ )
	{
		if( (ui32StartUser & (1UL << i))) 
		{
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pRoomInfo->_aSlotInfo[i]._Item, sizeof(UINT32) ); 
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pRoomInfo->_pSession[i]->m_ui8PBVer, sizeof(UINT8) );
			
			for( INT32 j = 0 ; j < CHAR_EQUIPMENT_PARTS_COUNT ; j++ )
			{
				T_ItemID TEquipID = pRoomInfo->_pSession[i]->m_aTBattleEquipParts[ j ].m_TItemID;
				pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &TEquipID, sizeof(T_ItemID) );
			}

			S2MO_CHAR_SKILL_INFO CharSkillInfo;
			pRoomInfo->_pSession[i]->m_pDesign->GetMainCharSkillInfo(&pRoomInfo->_pSession[i]->m_MultiSlot, &CharSkillInfo, i % TEAM_COUNT);  //m_MultiSlot.GetMainCharSkillInfo(&CharSkillInfo);

			CHARA_SLOT_SKILL stCharSkills;
			stCharSkills.m_PrimarySkill		= static_cast<CHARA_CLASS>(CharSkillInfo.m_ui8Class);
			stCharSkills.m_CommonSkill		= static_cast<COMMON_SKILL_TYPE>(CharSkillInfo.m_ui8CommonSkill);
			stCharSkills.m_MainSkill		= static_cast<MAIN_SKILL_TYPE>(CharSkillInfo.m_ui8MainSkill);
			stCharSkills.m_AssistSkill		= static_cast<ASSIST_SKILL_TYPE>(CharSkillInfo.m_ui8AssistSkill);
			stCharSkills.m_ui8CommonSkillLv	= CharSkillInfo.m_ui8CommonSkillLv;
			stCharSkills.m_ui8MainSkillLv	= CharSkillInfo.m_ui8MainSkillLv;
			stCharSkills.m_ui8AssistSkillLv	= CharSkillInfo.m_ui8AssistSkillLv;

			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&stCharSkills, sizeof(stCharSkills));
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pRoomInfo->_pSession[i]->m_i64UID, sizeof(T_UID) );
			pNetBuffer->_ppNetMessage[WriteIdx]->WriteData( &pRoomInfo->_pSession[i]->m_aui8EquipUserTitle, sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE ); 			
			
		}
	}
	
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleCast::BattleDestroy(INT32 WorkIdx, BATTLE_ROOM_INFO * pRoomInfo, UINT32 ui32GroupIdx )
{
	CAST_CHECKVALID_IDX;

	P_BATTLE_ROOM_DESTROY_REQ Packet;
	i3mem::Copy( &Packet._RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO) ); 
	Packet._GroupIdx = ui32GroupIdx; 

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_DESTROY_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Packet,	sizeof(P_BATTLE_ROOM_DESTROY_REQ));
	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleCast::BattleUserEnter(INT32 WorkIdx, BATTLE_ROOM_INFO * pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 ui32Item, CUserSession* pSession)
{	
	CAST_CHECKVALID_IDX;

	P_BATTLE_USER_ENTER_REQ ReqPacket;
	ReqPacket._wGroupIdx	= ui32GroupIdx; 
	ReqPacket._cSlotIdx		= ui8SlotIdx;
	ReqPacket._iItem		= ui32Item;
	ReqPacket._bHackOff		= (pSession->m_ui8QAAutoFlag & QA_AUTO_HACK_DETECT_OFF) ? true : false;
	
	
	// vv3 - check만
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
	{
		ReqPacket.m_TPartsID[i] = pSession->m_aTBattleEquipParts[i].m_TItemID;
	}

	S2MO_CHAR_SKILL_INFO CharSkillInfo;
	pSession->m_pDesign->GetMainCharSkillInfo(&pSession->m_MultiSlot, &CharSkillInfo, ui8SlotIdx % TEAM_COUNT);	//m_MultiSlot.GetCharSkillInfo(pSession->m_MultiSlot.GetTeamSlot(), &CharSkillInfo);

	ReqPacket.m_CharSkills.m_PrimarySkill		= static_cast<CHARA_CLASS>(CharSkillInfo.m_ui8Class);
	ReqPacket.m_CharSkills.m_CommonSkill		= static_cast<COMMON_SKILL_TYPE>(CharSkillInfo.m_ui8CommonSkill);
	ReqPacket.m_CharSkills.m_MainSkill			= static_cast<MAIN_SKILL_TYPE>(CharSkillInfo.m_ui8MainSkill);
	ReqPacket.m_CharSkills.m_AssistSkill		= static_cast<ASSIST_SKILL_TYPE>(CharSkillInfo.m_ui8AssistSkill);
	ReqPacket.m_CharSkills.m_ui8CommonSkillLv	= CharSkillInfo.m_ui8CommonSkillLv;
	ReqPacket.m_CharSkills.m_ui8MainSkillLv		= CharSkillInfo.m_ui8MainSkillLv;
	ReqPacket.m_CharSkills.m_ui8AssistSkillLv	= CharSkillInfo.m_ui8AssistSkillLv;

	i3mem::Copy( ReqPacket._aui8Designation, pSession->m_aui8EquipUserTitle, sizeof( UINT8 )*MAX_EQUIP_USER_TITLE );
	i3mem::Copy( &ReqPacket._RoomInfo , pRoomInfo, sizeof(BATTLE_ROOM_INFO)); 
	ReqPacket._i64UID							= pSession->m_i64UID;
	ReqPacket.m_ui8PBVer						= pSession->m_ui8PBVer;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_USER_ENTER_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ReqPacket, sizeof(P_BATTLE_USER_ENTER_REQ));

	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleCast::BattleUserLeave(INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, CUserSession* pSession)
{
	CAST_CHECKVALID_IDX;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_USER_LEAVE_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(pRoomInfo,		sizeof(BATTLE_ROOM_INFO));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui32GroupIdx,	sizeof(UINT32));
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&ui8SlotIdx,		sizeof(UINT8));

	pNetBuffer->_WritePos++;

	return TRUE; 

}

BOOL CModuleCast::BattleRoundStart(INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, TYPE_ROOM_INFO * pTypeRoomInfo, UINT8 ui8AtkDefChange)
{
	CAST_CHECKVALID_IDX;

	//Set Data 
	P_BATTLE_ROUND_REQ Packet; 
	i3mem::FillZero( &Packet, sizeof( P_BATTLE_ROUND_REQ ) );
	i3mem::Copy( &Packet._RoomInfo , pRoomInfo, sizeof(BATTLE_ROOM_INFO) ); 
	//i3mem::Copy( &Packet._iDinoInfo, pTypeRoomInfo->_ui32DInoCharInfo, (sizeof(UINT32) * SLOT_MAX_COUNT) ); 
	// 공룡인 아닌 유저들은 인간 캐릭터 정보를 넣어줌
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( SLOT_STATE_READY > pTypeRoomInfo->_aSlotInfo[i]._State ) continue;
		
		Packet._aui32Item[i]	= pTypeRoomInfo->_aSlotInfo[i]._Item;

		CUserSession	* pTargetSession = pTypeRoomInfo->_pSession[i];
		// vv3 - check
		if( pTypeRoomInfo->_aui32DInoCharInfo[i] == 0 )
		{
			ITEM_INFO CharItemInfo; 
			INT32 i32Team = (ui8AtkDefChange > 0) ? 1 : 0;
			pTargetSession->m_pDesign->GetMainCharEquipment(&pTargetSession->m_MultiSlot, CHAR_EQUIPMENT_CHARA, &CharItemInfo, ( i + i32Team ) % TEAM_COUNT); //m_MultiSlot.GetMainCharEquipment( CHAR_EQUIPMENT_CHARA, &CharItemInfo ); 
			Packet.m_TPartsID[i][ CHAR_EQUIPMENT_PARTS_CHARA ] = CharItemInfo.m_TItemID;
		}
		else
		{
			Packet.m_TPartsID[i][ CHAR_EQUIPMENT_PARTS_CHARA ] = pTypeRoomInfo->_aui32DInoCharInfo[i];
		}

		for( INT32 k = CHAR_EQUIPMENT_PARTS_HEAD ; k < CHAR_EQUIPMENT_PARTS_COUNT ; ++k )
		{
			Packet.m_TPartsID[i][k] = pTargetSession->m_aTBattleEquipParts[ k ].m_TItemID;
		}

		i3mem::Copy( Packet._aui8Designation[i], pTargetSession->m_aui8EquipUserTitle, sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE );
	}

	Packet._GroupIdx	= ui32GroupIdx; 
	Packet._iRound		= pTypeRoomInfo->_InfoAdd._NowRoundCount;

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_ROUND_START_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Packet,	sizeof(P_BATTLE_ROUND_REQ));

	pNetBuffer->_WritePos++;

	return TRUE; 
}

BOOL CModuleCast::BattleChangeDinoInfo(INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT16 ui16Slot, TYPE_ROOM_INFO* pInfo )
{
	CAST_CHECKVALID_IDX;

	//Set Data 
	P_BATTLE_M_DINO_INFO_REQ Packet; 
	i3mem::Copy( &Packet._RoomInfo , pRoomInfo, sizeof(BATTLE_ROOM_INFO) ); 
	Packet._GroupIdx	= ui32GroupIdx; 
	Packet._iSlotIdx	= ui16Slot; 
	Packet._ui32Item	= pInfo->_aSlotInfo[ui16Slot]._Item;

	// vv3 - check
	Packet.m_TPartsID[ CHAR_EQUIPMENT_PARTS_CHARA ]		= pInfo->_aui32DInoCharInfo[ui16Slot];

	for( INT32 i = CHAR_EQUIPMENT_PARTS_HEAD ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
	{
		Packet.m_TPartsID[i] = pInfo->_pSession[ui16Slot]->m_aTBattleEquipParts[ i ].m_TItemID;
	}
	i3mem::Copy( Packet._aui8Designation, pInfo->_pSession[ui16Slot]->m_aui8EquipUserTitle, sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE );

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_M_DINO_INFO_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Packet,	sizeof(P_BATTLE_M_DINO_INFO_REQ));

	pNetBuffer->_WritePos++;
	
	return TRUE; 
}

// 야전상점에서 무기구매시 데디케이션 서버에 통보하기 위해 먼저 컨트롤 서버에 통보한다.
BOOL CModuleCast::BattleWeaponInfo(	INT32 WorkIdx,	BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, GC_PROTOCOL_SLOT_EQUIPMENT* pStruct )
{
	CAST_CHECKVALID_IDX;

	STRUCT_BATTLE_SLOT_EQUIPMENT_REQ Packet;
	i3mem::Copy( &Packet._RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO) );
	Packet.m_ui32GroupIdx	= ui32GroupIdx;
	Packet.m_ui8SlotIdx		= ui8SlotIdx;
	i3mem::Copy( &Packet.m_Struct, pStruct, sizeof(GC_PROTOCOL_SLOT_EQUIPMENT) );

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_SLOT_EQUIPMENT_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Packet, sizeof(STRUCT_BATTLE_SLOT_EQUIPMENT_REQ) );

	pNetBuffer->_WritePos++;

	return TRUE;
}


BOOL CModuleCast::BattleRespawn( INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount )
{
	CAST_CHECKVALID_IDX;

	i3NetworkPacket* pPacket = pNetBuffer->_ppNetMessage[WriteIdx];

	P_BATTLE_RESPAWN_REQ Packet;
	i3mem::Copy( &Packet._RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO) );
	Packet._GroupIdx = ui32GroupIdx;
	Packet._cSlotIdx = ui8SlotIdx;
	i3mem::Copy( &Packet._pUseWeaponList, pui32WeaponList, sizeof(UINT32) * CHAR_EQUIPMENT_WEAPON_COUNT);
	Packet._i32TotalRespawnIdx = i32RespawnIdx;
	Packet._i32RespawnCount = i32RespawnCount;

	pPacket->Clear();
	pPacket->SetProtocol(PROTOCOL_BATTLE_SERVER_RESPAWN_REQ);
	pPacket->WriteData(&Packet, sizeof(P_BATTLE_RESPAWN_REQ));

	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleCast::BattleGMPause( INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, bool bPause )
{
	CAST_CHECKVALID_IDX;

	i3NetworkPacket* pPacket = pNetBuffer->_ppNetMessage[WriteIdx];

	pPacket->Clear();
	pPacket->SetProtocol( PROTOCOL_BATTLE_SERVER_GM_PAUSE_REQ );
	pPacket->WriteData( pRoomInfo,		sizeof( BATTLE_ROOM_INFO ) );
	pPacket->WriteData( &ui32GroupIdx,	sizeof( UINT32 ) );
	pPacket->WriteData( &bPause,		sizeof( bool ) );
	
	pNetBuffer->_WritePos++;

	return TRUE;
}

BOOL CModuleCast::BattleQAAuto(INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT8 ui8CommandID, INT32* pi32Arg )
{
	CAST_CHECKVALID_IDX;

	P_BATTLE_QA_AUTO_REQ	Data;
	i3mem::Copy( &Data._RoomInfo, pRoomInfo, sizeof(BATTLE_ROOM_INFO));
	Data._ui8SlotIdx = ui8SlotIdx;
	Data._ui32GroupIdx = ui32GroupIdx;
	Data._ui8CommandId = ui8CommandID;
	i3mem::Copy( Data._i32Arg, pi32Arg, sizeof(INT32)*MAX_COUNT_QA_AUTO_ARG );

	pNetBuffer->_ppNetMessage[WriteIdx]->Clear();
	pNetBuffer->_ppNetMessage[WriteIdx]->SetProtocol(PROTOCOL_BATTLE_SERVER_QA_AUTO_COMMAND_REQ);
	pNetBuffer->_ppNetMessage[WriteIdx]->WriteData(&Data, sizeof(P_BATTLE_QA_AUTO_REQ));

	pNetBuffer->_WritePos++;

	return TRUE;
}

void CModuleCast::SendPacketMessage( i3NetworkPacket *SendPacket )
{
	m_pSocket->SendPacketMessage(SendPacket);
}