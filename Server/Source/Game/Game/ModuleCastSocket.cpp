#include "pch.h"
#include "ModuleCast.h"
#include "ModuleCastSocket.h"
#include "UserManager.h"
// #include "ModuleLogFile.h"
#include "RoomManager.h"
#include "Room.h"

#define CHECKVALID_BATTLE_ROOM CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom ); \
							if( pRoom == NULL) break; \
							if( !pRoom->m_bIsCreate ) break; \

#define CHECKVALID_BATTLE_ROOM2 CRoom* pRoom = _GetRoom( stRoomInfo._cChannel, stRoomInfo._sRoom ); \
							if( pRoom == NULL) break; \
							if( !pRoom->m_bIsCreate ) break; \

I3_CLASS_INSTANCE( CModuleCastSocket, i3NetworkClientSocket );


CModuleCastSocket::CModuleCastSocket(void)
{
	m_bConnect				= FALSE; 
	m_ui32CastSendTime		=  1000; 
}	

CModuleCastSocket::~CModuleCastSocket(void)
{
	OnDestroy();
}

BOOL CModuleCastSocket::OnCreate( UINT32 ui32IP, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16PortNo);
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

	return bRv;
}

void CModuleCastSocket::OnDestroy(void)
{
	DisConnect();
}

void CModuleCastSocket::DisConnect()
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::DisConnect();
}

void CModuleCastSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{
			//DWORD	dwThreadId = GetCurrentThreadId();
			g_pLog->WriteLog( L"[SUCC] Cast Server Connect Messagr Recv" );

			//처음 보내는 패킷을 보낸다. 			
			UINT8	ui8ServerType		= ASC_SERVER_GAME;			//Game Server Type

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData(&ui8ServerType,						sizeof(UINT8));			//GameServer
			SendPacketMessage(&SendPacket);
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				:
		m_bConnect = TRUE;
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		break;
	case PROTOCOL_BATTLE_SERVER_CREATE_ACK		:
		{
			P_BATTLE_ROOM_CREATE_ACK AckInfo; 
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_ROOM_CREATE_ACK)); 

			CHECKVALID_BATTLE_ROOM;

			pRoom->SetBattleGroupInfo( &AckInfo );
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_USER_ENTER_ACK	:		break;
	case PROTOCOL_BATTLE_SERVER_KILL_ACK				: 
		{
			P_BATTLE_ROOM_KILL_ACK	AckInfo; 
			DEATH_INFO_CHARA		aData[MAX_KILLUSER_COUNT];
			UINT8					ui8RoundNum;

			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_ROOM_KILL_ACK)); 
			if( AckInfo._killHeader._nCount > MAX_KILLUSER_COUNT)break;

			pPacket->ReadData(aData, (sizeof(DEATH_INFO_CHARA) * AckInfo._killHeader._nCount) ); 

			CHECKVALID_BATTLE_ROOM;

			pPacket->ReadData(&ui8RoundNum, sizeof(UINT8) ); 
			
			pRoom->DeathBattle( MODULE_CAST_THREAD_IDX, NULL, &AckInfo._killHeader, aData, ui8RoundNum);
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_M_BOMB_INSTALL_ACK		: 
		{
			P_BATTLE_M_BOMB_INSTALL_ACK AckInfo; 
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_BOMB_INSTALL_ACK)); 

			CHECKVALID_BATTLE_ROOM;

			pRoom->MissionBombInstall(MODULE_CAST_THREAD_IDX, NULL, AckInfo._iSlotIdx, AckInfo._BombPosition, AckInfo._BombVecPos);
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_M_BOMB_UNINSTALL_ACK	: 
		{
			P_BATTLE_M_BOMB_UNINSTALL_ACK AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_BOMB_UNINSTALL_ACK)); 

			CHECKVALID_BATTLE_ROOM;
			
			pRoom->MissionBombUnInstall(MODULE_CAST_THREAD_IDX, NULL, AckInfo._iSlotIdx);
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_M_DESTRUCTION_HP_ACK	: 
		{
			P_BATTLE_M_DESTRUCTION_HP_ACK AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_DESTRUCTION_HP_ACK)); 

			CHECKVALID_BATTLE_ROOM;
			
			pRoom->MissionObjectDestroy(MODULE_CAST_THREAD_IDX, NULL, &AckInfo._DestructionInfo);
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_M_DEFENCE_ACK			: 
		{
			P_BATTLE_M_DEFENCE_ACK AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_DEFENCE_ACK)); 

			CHECKVALID_BATTLE_ROOM;
			
			pRoom->MissionObjectDefence(NULL, &AckInfo._DefenceInfo); 
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_HACK_USER_ACK			: 
		{
			P_BATTLE_HACK_USER_ACK	AckInfo;
			P_BATTLE_HACK_INFO		aHackInfo[MAX_KILLUSER_COUNT];
			
			// read header
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_HACK_USER_ACK)); 
			if( AckInfo._uCount > MAX_KILLUSER_COUNT ) break;
			if( AckInfo._uCount == 0 ) break;
			// read data
			pPacket->ReadData(aHackInfo, sizeof(P_BATTLE_HACK_INFO)*AckInfo._uCount);
			
			CHECKVALID_BATTLE_ROOM;

			for (UINT32 i = 0; i < AckInfo._uCount; i++)
			{
				CUserSession* pSession = pRoom->GetSession(aHackInfo[i]._uSlotIdx);
				if (pSession == NULL)								continue;
				if (pSession->GetUID() != aHackInfo[i]._i64UID)	continue;
				if (pSession->m_ui32ATKillDelay > 0)				continue;

				//g_pLog->WriteLogSession(pSession, L"DEDI:%d", aHackInfo[i]._uReason);

				if (g_pContextMain->m_i32HackKickType > 0)
				{
					pRoom->SetKickBattle_Dedi(aHackInfo[i]._uSlotIdx);
				}
				else
				{

#if defined (I3_DEBUG)
					pRoom->SendNotifyHackUser(aHackInfo[i]._uSlotIdx, HACK_CHECK_TYPE_SERVER_D, MODULE_CAST_THREAD_IDX, aHackInfo[i]._uReason, pSession->m_ui8LocaleCode);

#else
					wchar_t wstrTemp[MAX_PATH];
					i3String::Format(wstrTemp, DEF_SIZE_HACK_NOTICE, L"[%d]DEDI:%d", AckInfo._RoomInfo._cBattle, aHackInfo[i]._uReason);
					pSession->DisconnectUserDelay(((UINT32)EVENT_ERROR_HACKING_USER) | aHackInfo[i]._uReason, KILL_REASON_HACK_SERVER_D, wstrTemp);
#endif
				}
			}
		}
		break;
	case PROTOCOL_BATTLE_SERVER_SLOT_EQUIPMENT_ACK		:
		{
			STRUCT_BATTLE_SLOT_EQUIPMENT_ACK	AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(STRUCT_BATTLE_SLOT_EQUIPMENT_ACK));

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if ( pRoom == NULL) break;
			if ( !pRoom->m_bIsCreate ) break;

			CUserSession* pSession = pRoom->GetSession( AckInfo.m_ui8SlotIdx );
			if ( pSession == NULL ) break;

			// 해당 유저가 있는 방의 전부에게 해당유저의 장비정보를 브로드캐스팅합니다. 2011.12.08 안국정
			pRoom->BattleSlotEquipment( pSession, AckInfo.m_ui8SlotIdx, AckInfo.m_ui16ChangeFlag, AckInfo.m_ui32EquipItem );
		}
		break;
	case PROTOCOL_BATTLE_SERVER_GM_PAUSE_ACK			:
		{
			BATTLE_ROOM_INFO	stRoomInfo;
			INT32				i32Result;

			pPacket->ReadData( &stRoomInfo,	sizeof( BATTLE_ROOM_INFO ) );
			pPacket->ReadData( &i32Result,	sizeof( INT32 ) );

			CHECKVALID_BATTLE_ROOM2;

			pRoom->GMPauseResult_C( i32Result );
		}
		break;
	case PROTOCOL_BATTLE_SERVER_KICK_USER_ACK			:
		{			
			BATTLE_ROOM_INFO	stRoomInfo;
			T_UID				i64UID;
			UINT32				ui32SlotIdx;

			pPacket->ReadData(&stRoomInfo,		sizeof(BATTLE_ROOM_INFO));
			pPacket->ReadData(&i64UID,			sizeof(T_UID)			);
			pPacket->ReadData(&ui32SlotIdx,		sizeof(UINT32)			);
			
			CHECKVALID_BATTLE_ROOM2;

			CUserSession* pSession = pRoom->GetSession( ui32SlotIdx );
			
			if( pSession == NULL ) break;
			if( pSession->GetUID() != i64UID ) break;

			g_pLog->WriteLog( L"[PROTOCOL_BATTLE_KICK_USER_BATTLE_ACK] Kick User_Dedi");
			pRoom->SetKickBattle_Dedi( ui32SlotIdx );
		}
		break;
	case PROTOCOL_BATTLE_SERVER_MISSION_ROUND_START_REQ:
		{
			BATTLE_ROOM_INFO	stRoomInfo;

			pPacket->ReadData(&stRoomInfo,			sizeof(BATTLE_ROOM_INFO));

			CHECKVALID_BATTLE_ROOM2;

			pRoom->SetRoundStart();
		}
		break;
	case PROTOCOL_BATTLE_SERVER_MISSION_ROUND_END_REQ:
		{
			BATTLE_ROOM_INFO	stRoomInfo;
			UINT8				ui8RoundEndType;
			UINT8				ui8RoundWinTeam;

			pPacket->ReadData(&stRoomInfo,			sizeof(BATTLE_ROOM_INFO));
			pPacket->ReadData(&ui8RoundEndType,		sizeof(UINT8)			);
			pPacket->ReadData(&ui8RoundWinTeam,		sizeof(UINT8)			);
			
			CHECKVALID_BATTLE_ROOM2;

			pRoom->SetRoundEnd( ui8RoundEndType, ui8RoundWinTeam );
		}
		break;
	case PROTOCOL_BATTLE_SERVER_M_TOUCHDOWN_REQ:
		{
			BATTLE_ROOM_INFO	stRoomInfo;
			UINT32				ui32SlotIdx;

			pPacket->ReadData(&stRoomInfo,			sizeof(BATTLE_ROOM_INFO));
			pPacket->ReadData(&ui32SlotIdx,			sizeof(UINT32)			);

			CHECKVALID_BATTLE_ROOM2;

			CUserSession* pSession = pRoom->GetSession( ui32SlotIdx );

			if( NULL == pSession ) break;

			pRoom->MissionTouchDown( pSession );
		}
		break;
	default:
		g_pLog->WriteLog( L"[CModuleCastSocket::PacketParsing]Unknown Message");
		break; 
	}

	m_ui32LastRecvTime = i3ThreadTimer::GetServerTime();
	return; 
}

void CModuleCastSocket::_ReloadPacket( i3NetworkPacket* pPacket )
{
	//Set ClanCreateInfo
	pPacket->ReadData( &g_pContextMain->m_ui8ClanCreateRank,	sizeof( UINT8 )); 
	pPacket->ReadData( &g_pContextMain->m_i32ClanCreatePoint,	sizeof( INT32 )); 

	pPacket->ReadData( &g_pContextMain->m_ui16BoostEventCount,	sizeof(UINT16) );
	pPacket->ReadData( g_pContextMain->m_stBoostEvent,			sizeof(BOOST_EVENT)*g_pContextMain->m_ui16BoostEventCount );
	pPacket->ReadData( &g_pContextMain->m_aRankUpItem,			sizeof(RANKUP_ITEM) * MAX_RANK_COUNT);
	pPacket->ReadData( &g_pContextMain->m_stTSEvent,			sizeof(TS_EVENT_INFO)*TS_EVENT_COUNT);

	UINT8	ui8GameServerCount;
	pPacket->ReadData( &ui8GameServerCount, sizeof(UINT8) );

	for( UINT8 i = 0 ; i < ui8GameServerCount ; i++ )
	{
		pPacket->ReadData( &g_pContextMain->m_ui16NowChannelCount[i],	sizeof(UINT16) );

		if(g_pContextMain->m_ui16NowChannelCount[i] > g_pContextMain->m_aui16InitChannelCount[i])
		{
			g_pContextMain->m_ui16NowChannelCount[i] = g_pContextMain->m_aui16InitChannelCount[i];
		}

	}
	
	pPacket->ReadData( &g_pContextMain->m_ui8TagCount,				sizeof(UINT8) );
	pPacket->ReadData( g_pContextMain->m_ui8TagPriority,			sizeof(UINT8) * g_pContextMain->m_ui8TagCount );

	g_pContextMain->m_ui32ChannelCountPer	= (UINT32)(g_pContextMain->m_ui16NowChannelCount[g_pContextMain->m_i32ServerIdx] * 95 * 0.01f);	// 한번 Full이 찬 상태에서 95%이하 전에는 초기화가 안됩니다.
	
}

CRoom * CModuleCastSocket::_GetRoom(UINT8 ui8Channel, UINT16 ui16Room)
{
	if( NULL == g_pRoomManager ) return NULL; 

	if(ui8Channel >= (INT32)g_pRoomManager->GetChannelCount())
	{
		g_pLog->WriteLog( L"GetRoom - Channel Error");
		return NULL; 
	}

	if(ui16Room >= g_pContextMain->m_ui32ChannelSessionCount )
	{
		g_pLog->WriteLog( L"GetRoom - Room Error");
		return NULL; 
	}

	CRoom* pRoom = g_pRoomManager->GetRoom( ui8Channel, ui16Room );
	if( NULL == pRoom ) return NULL;

	return pRoom; 
}