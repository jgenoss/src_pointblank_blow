#include "pch.h"
#include "ModuleControlSocket.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "Room.h"

I3_CLASS_INSTANCE( CModuleControlSocket, i3NetworkClientSocket );


CModuleControlSocket::CModuleControlSocket(void)
{
	m_bConnect				= FALSE; 
	m_ui32ControlSendTime	=  1000; 
}

CModuleControlSocket::~CModuleControlSocket(void)
{
	OnDestroy();
}

BOOL CModuleControlSocket::OnCreate(char * pAddress, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate(pAddress, ui16PortNo);
	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();
	m_ui32LastSendTime	= g_pContextMain->GetServerTime();

	return bRv; 
}

void CModuleControlSocket::OnDestroy(void)
{
	DisConnect();
}

void CModuleControlSocket::DisConnect()
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::DisConnect();
}

void CModuleControlSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL, "[SUCC] Control Server Connect Messagr Recv");
			pPacket->ReadData( &m_ui32ControlSendTime, sizeof(UINT32)); 
			pPacket->ReadData( &g_pContextMain->m_ControlVersion,	sizeof(SS_VERSION));

			//처음 보내는 패킷을 보낸다. 
			INT32	i32ConnectType		= ASC_CONNECT_TYPE_SERVER;	//Server
			UINT8	ui8ServerType		= ASC_SERVER_GAME;			//Game Server Type
			UINT16	ui16MaxUserCount	= (UINT16)(GAME_CHANNEL_COUNT * g_pContextMain->m_ui32ChannelSessionCount);

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData(&i32ConnectType,						sizeof(INT32));			//Server
			SendPacket.WriteData(&ui8ServerType,						sizeof(UINT8));			//GameServer
			SendPacket.WriteData(&g_pContextMain->m_GameVersion,		sizeof(SS_VERSION));
			SendPacket.WriteData(g_pContextMain->m_pui32ProxyAddressIp,	sizeof(UINT32));
			SendPacket.WriteData(g_pContextMain->m_pui16AddressPort,	sizeof(UINT16));
			SendPacket.WriteData(&ui16MaxUserCount,						sizeof(UINT16));

			SendPacketMessage(&SendPacket);
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				:
		{
			m_bConnect = TRUE;

			if( SERVER_STATE_LOADING != g_eServerState )	break;

			UINT32	ui32CommonServerCount;
			UINT8	ui8Type;
			UINT32	ui32IP;

			//Get Basic Info
			pPacket->ReadData( &g_pContextMain->m_ui8UseServerSelect,		sizeof(UINT8) );

			g_pContextMain->m_pui8ChannelType = (UINT8*)i3MemAlloc(sizeof(UINT8) * GAME_CHANNEL_COUNT);
			i3mem::FillZero(g_pContextMain->m_pui8ChannelType, (sizeof(UINT8) * GAME_CHANNEL_COUNT) ); 
			i3mem::FillZero(g_pContextMain->m_aui8HaveChannel, (sizeof(UINT8) * CHANNEL_TYPE_MAX) );
			
			pPacket->ReadData( g_pContextMain->m_pui8ChannelType, (sizeof(UINT8) * GAME_CHANNEL_COUNT) );

			for( UINT8 i = 0; i < GAME_CHANNEL_COUNT; ++i )
			{
				g_pContextMain->m_aui8HaveChannel[ g_pContextMain->m_pui8ChannelType[i] ] = 1;
			}

			//Get CommonServer Info 
			pPacket->ReadData( &ui32CommonServerCount,						sizeof(UINT32) ); 
			for(UINT32 i = 0; i < ui32CommonServerCount; i++)
			{
				pPacket->ReadData( &ui8Type,								sizeof(UINT8) ); 
				pPacket->ReadData( &ui32IP,									sizeof(UINT32) ); 
			}

			//Get GameServer Info 
			pPacket->ReadData( &g_pContextMain->m_ui32GameServerCount,		sizeof(UINT32) ); 
			pPacket->ReadData( g_pContextMain->m_aGameServerInfo,			sizeof(GAME_SERVER_INFO) * g_pContextMain->m_ui32GameServerCount );
			pPacket->ReadData( &g_pContextMain->m_ui32QuestEventDate,		sizeof(UINT32));

			// Default Stage
			pPacket->ReadData( &g_pContextMain->m_ui8StageTotalCount,		sizeof(UINT8));
			pPacket->ReadData( &g_pContextMain->m_ui8StageBitCount,			sizeof(UINT8));	
			pPacket->ReadData( g_pContextMain->m_aui32StageBitActive,		sizeof(UINT32)*g_pContextMain->m_ui8StageBitCount);
			pPacket->ReadData( g_pContextMain->m_aTStageActiveOption,		sizeof(V_STAGEOPTION)*g_pContextMain->m_ui8StageTotalCount );
			pPacket->ReadData( g_pContextMain->m_ai32StageType,				sizeof(INT32)*g_pContextMain->m_ui8StageTotalCount );
			pPacket->ReadData( g_pContextMain->m_aui8StageMark,				sizeof (UINT8)*g_pContextMain->m_ui8StageTotalCount );
			pPacket->ReadData( g_pContextMain->m_aStageDefaultActive,		sizeof(STAGE_ORDINAL_ID)*STAGE_TYPE_MAX);

			for( INT32 i = 0 ; i < g_pContextMain->m_ui8StageTotalCount ; i++ )
			{
				UINT8 ui8Size;
				pPacket->ReadData( &ui8Size,								sizeof(UINT8));
				pPacket->ReadData( g_pContextMain->m_astrStageName[i],		ui8Size );
				g_pContextMain->m_astrStageName[i][ STAGE_CODE_NAME_COUNT-1 ] = '\0';
			}

			g_pContextMain->SetMapSetting();

			pPacket->ReadData( &g_pContextMain->m_i32ClanMax,				sizeof(INT32));
			if( 0 < g_pContextMain->m_i32ClanMax )
			{
				for( INT32 i = 0 ; i < CLAN_LIST_COUNT ; i++ )
				{
					g_pContextMain->m_apClanList[i] = (CLAN_BASIC_INFO*)i3MemAlloc(sizeof(CLAN_BASIC_INFO)*g_pContextMain->m_i32ClanMax);
					i3mem::FillZero( g_pContextMain->m_apClanList[i],			sizeof(CLAN_BASIC_INFO)*g_pContextMain->m_i32ClanMax );
				}
			}
			pPacket->ReadData( &g_pContextMain->m_i32ServerIdx,				sizeof(INT32));
			pPacket->ReadData( &g_pContextMain->m_i8PrimaryWeapon,			sizeof(INT8));
			//Set MD5 Key
			pPacket->ReadData( g_pContextMain->m_astrMD5Key[0], NET_MD5_KEY_SIZE); 
			pPacket->ReadData( g_pContextMain->m_astrMD5Key[1], NET_MD5_KEY_SIZE);

			// Set Info Mission CardSet
			pPacket->ReadData( &g_pContextMain->m_ui8ActiveQuestCount,		sizeof(UINT8));
			if( 0 < g_pContextMain->m_ui8ActiveQuestCount )
			{
				g_pContextMain->m_pui32ActiveQuest = (UINT32*)i3MemAlloc(sizeof(UINT32) * g_pContextMain->m_ui8ActiveQuestCount);
				pPacket->ReadData( g_pContextMain->m_pui32ActiveQuest,		sizeof(UINT32)*g_pContextMain->m_ui8ActiveQuestCount);
			}
			pPacket->ReadData(g_pContextMain->m_aRankUpItem, sizeof(g_pContextMain->m_aRankUpItem) );
			// TS 이벤트 받기	
			pPacket->ReadData(g_pContextMain->m_aTSEvent, sizeof(g_pContextMain->m_aTSEvent) );

			if(g_pContextMain->m_aTSEvent[0]._ui32StartDate != 0)
			{
				g_pContextMain->m_i32TSInitHour = (g_pContextMain->m_aTSEvent[0]._ui32StartDate % 10000) / 100;
				g_pContextMain->m_i32TSInitMin  = g_pContextMain->m_aTSEvent[0]._ui32StartDate % 100;
			}

			pPacket->ReadData(&g_pContextMain->m_bIsNewRepair, sizeof(bool));

			// 신고하기 - 스크린샷 기능 사용 - 서동권
			pPacket->ReadData( g_pContextMain->m_strReportFtpAddress,		MAX_STRING_COUNT );
			pPacket->ReadData( &g_pContextMain->m_ui32ReportFtpPort,		sizeof(UINT32) );
			pPacket->ReadData( &g_pContextMain->m_bGiftEnable,				sizeof(bool));

			UINT8 ui8DefaultItemCount;
			DEFAULT_ITEM aDefaultItem[ DEFAULT_ITEM_COUNT ];

			pPacket->ReadData( &ui8DefaultItemCount,						sizeof(UINT8));
			pPacket->ReadData( aDefaultItem,								sizeof(DEFAULT_ITEM)*ui8DefaultItemCount );
			g_pContextMain->SetDefaultItem( ui8DefaultItemCount, aDefaultItem );

			// 이태리 빌링연동 Cash 충전 URL - 임준현
			pPacket->ReadData( g_pContextMain->m_strBillingURL, MAX_STRING_COUNT );
			g_pContextMain->m_ui16BillingURLLen = (UINT16)i3String::Length(g_pContextMain->m_strBillingURL);

			//문자열 길이가 0보다 큰경우(빈 문자열이 아닌 경우)
			if( 0 < g_pContextMain->m_ui16BillingURLLen)
				g_pContextMain->m_ui16BillingURLLen++;		//'\0' 전송을 위해 전체길이를 1 증가

			// 국기표기 기능 사용여부 - 안국정
			pPacket->ReadData( &g_pContextMain->m_bNationalFlag,			sizeof(bool));

			// ReConfig
			_ReloadPacket( pPacket );

			g_eServerState = SERVER_STATE_INIT;
		}
		break; 
	case PROTOCOL_ASC_CONFIG_RELOAD_ACK:
		{
			_ReloadPacket( pPacket );
		}
		break;
		//PROTOCOL_BASE_SERVER_STATE_REQ, 
	case PROTOCOL_BASE_GAME_SERVER_STATE_ACK				: 
		{
			//Get GameServer Info 
			pPacket->ReadData( &g_pContextMain->m_ui32GameServerCount,	sizeof(UINT32) ); 
			pPacket->ReadData( g_pContextMain->m_aGameServerInfo,	sizeof(GAME_SERVER_INFO) * g_pContextMain->m_ui32GameServerCount );
		}
		break; 
	case PROTOCOL_ASC_SET_MD5_KEY_ACK				: 
		{
			pPacket->ReadData( g_pContextMain->m_astrMD5Key[0], NET_MD5_KEY_SIZE); 
			pPacket->ReadData( g_pContextMain->m_astrMD5Key[1], NET_MD5_KEY_SIZE); 
		}
		break; 
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_ACK	: 
		{
			g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX, NULL, "CONTROL SERVER KICK USER MESSAGE RECV");
			g_pUserManager->KickAllSession(); 
		}
		break; 
	case PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_ACK		: 
		{
			UINT32	ui32Type; 
			UINT32	ui32strLen; 
			pPacket->ReadData(&ui32Type, sizeof(UINT32));
			switch( ui32Type )
			{
			case NOTICE_TYPE_NORMAL		: 
				pPacket->ReadData(&ui32strLen, sizeof(UINT32)); 
				if(ui32strLen > NOTICE_STRING_MAX_COUNT - 1)ui32strLen = NOTICE_STRING_MAX_COUNT - 1; 
				pPacket->ReadData(g_pContextMain->m_strNormalNotice, ui32strLen);
				g_pContextMain->m_strNormalNotice[ui32strLen] = 0; 
				if ( NULL != g_pUserManager )
				{
					g_pUserManager->OnSendNoticeMessage( MODULE_CONTROL_THREAD_IDX, NOTICE_TYPE_NORMAL, g_pContextMain->m_strNormalNotice, ui32strLen);
				}
				break; 
			case NOTICE_TYPE_EMERGENCY	: 
				pPacket->ReadData(&ui32strLen, sizeof(UINT32)); 
				if(ui32strLen > NOTICE_STRING_MAX_COUNT - 1)ui32strLen = NOTICE_STRING_MAX_COUNT - 1; 
				pPacket->ReadData(g_pContextMain->m_strEmergencyNotice, ui32strLen);
				g_pContextMain->m_strEmergencyNotice[ui32strLen] = 0; 
				if ( NULL != g_pUserManager )
				{
					g_pUserManager->OnSendNoticeMessage( MODULE_CONTROL_THREAD_IDX, NOTICE_TYPE_EMERGENCY, g_pContextMain->m_strEmergencyNotice, ui32strLen);
				}
				break;
			}			
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_CREATE_ACK		:
		{
			INT32 i32Result;
			P_BATTLE_ROOM_CREATE_ACK AckInfo; 
			pPacket->ReadData(&i32Result,	sizeof(INT32));	
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_ROOM_CREATE_ACK)); 

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL)return; 

			pRoom->SetBattleGroupInfo( i32Result, &AckInfo );
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_USER_ENTER_ACK	:		break;
	case PROTOCOL_BATTLE_KILL_ACK				: 
		{
			P_BATTLE_ROOM_KILL_ACK	AckInfo; 
			DEATH_INFO_CHARA		aData[MAX_KILLUSER_COUNT];
			UINT8					ui8RoundNum;

			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_ROOM_KILL_ACK)); 
			if( AckInfo._killHeader._nCount > MAX_KILLUSER_COUNT)break;

			pPacket->ReadData(aData, (sizeof(DEATH_INFO_CHARA) * AckInfo._killHeader._nCount) ); 

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL)break; 

			pPacket->ReadData(&ui8RoundNum, sizeof(UINT8) ); 
			
			pRoom->DeathBattle( MODULE_CONTROL_THREAD_IDX, NULL, &AckInfo._killHeader, aData, ui8RoundNum);
		}
		break; 
	case PROTOCOL_BATTLE_M_BOMB_INSTALL_ACK		: 
		{
			P_BATTLE_M_BOMB_INSTALL_ACK AckInfo; 
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_BOMB_INSTALL_ACK)); 

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL)break; 

			pRoom->MissionBombInstall(NULL, AckInfo._iSlotIdx, AckInfo._BombPosition, AckInfo._BombVecPos);
		}
		break; 
	case PROTOCOL_BATTLE_M_BOMB_UNINSTALL_ACK	: 
		{
			P_BATTLE_M_BOMB_UNINSTALL_ACK AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_BOMB_UNINSTALL_ACK)); 

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL)break; 
			
			pRoom->MissionBombUnInstall(NULL, AckInfo._iSlotIdx);	
		}
		break; 
	case PROTOCOL_BATTLE_M_DESTRUCTION_HP_ACK	: 
		{
			P_BATTLE_M_DESTRUCTION_HP_ACK AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_DESTRUCTION_HP_ACK)); 

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL)break; 
			
			pRoom->MissionObjectDestroy(NULL, &AckInfo._DestructionInfo); 
		}
		break; 
	case PROTOCOL_BATTLE_M_DEFENCE_ACK			: 
		{
			P_BATTLE_M_DEFENCE_ACK AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_M_DEFENCE_ACK)); 

			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL)break; 
			
			pRoom->MissionObjectDefence(NULL, &AckInfo._DefenceInfo); 
		}
		break; 
	case PROTOCOL_BATTLE_HACK_USER_ACK			: 
		{
			P_BATTLE_HACK_USER_ACK	AckInfo;
			P_BATTLE_HACK_INFO		aHackInfo[MAX_KILLUSER_COUNT];
			char szTemp[ MAX_PATH ] = { '\0', };	
			
			// read header
			pPacket->ReadData(&AckInfo,		sizeof(P_BATTLE_HACK_USER_ACK)); 
			if( AckInfo._uCount > MAX_KILLUSER_COUNT ) break;
			if( AckInfo._uCount == 0 ) break;
			// read data
			pPacket->ReadData(aHackInfo, sizeof(P_BATTLE_HACK_INFO)*AckInfo._uCount);
			
			CRoom* pRoom = _GetRoom( AckInfo._RoomInfo._cChannel, AckInfo._RoomInfo._sRoom );
			if( pRoom == NULL) break; 

			for ( UINT32 i = 0; i < AckInfo._uCount; i++ )
			{
				CUserSession* pSession = pRoom->GetSession( aHackInfo[i]._uSlotIdx );
				if( pSession == NULL) break;
				if( pSession->m_ui32ATSendKill > 0 ) continue;
				
#if defined (I3_DEBUG)
				pRoom->SendNotifyHackUser( aHackInfo[i]._uSlotIdx, HACK_CHECK_TYPE_SERVER_D, MODULE_CONTROL_THREAD_IDX, aHackInfo[i]._uReason, pSession->m_ui8LocaleCode );
				i3String::Format( szTemp, MAX_PATH, "DEDI:%d", aHackInfo[i]._uReason );
				g_pModuleLogFile->Write_M_Log( MODULE_CONTROL_THREAD_IDX, pSession, szTemp );
#else
				UINT8 ui8RandTime = rand() % HACK_USER_DISCONNECT_TIME;
				pSession->m_ui32ATSendKill		= g_pContextMain->GetServerTime() + ui8RandTime;
				pSession->m_ui32KillReason			= KILL_REASON_HACK_SERVER_D;
				pSession->m_ui32HackKillReason		= aHackInfo[i]._uReason;
				i3String::Format( pSession->m_strHackNotice, DEF_SIZE_HACK_NOTICE, "DEDI:%d, RandTime:%d", aHackInfo[i]._uReason, ui8RandTime );
#endif
			}
		}
		break;
	case PROTOCOL_BATTLE_SLOT_EQUIPMENT_2LEVEL_ACK		:
		{
			STRUCT_BATTLE_SLOT_EQUIPMENT_ACK	AckInfo;
			pPacket->ReadData(&AckInfo,		sizeof(STRUCT_BATTLE_SLOT_EQUIPMENT_ACK));

			CRoom* pRoom = _GetRoom( AckInfo.m_RoomInfo._cChannel, AckInfo.m_RoomInfo._sRoom );
			if ( pRoom == NULL) break;

			CUserSession* pSession = pRoom->GetSession( AckInfo.m_ui8SlotIdx );
			if ( pSession == NULL ) break;

			// 해당 유저가 있는 방의 전부에게 해당유저의 장비정보를 브로드캐스팅합니다. 2011.12.08 안국정
			pRoom->BattleSlotEquipment( pSession, AckInfo.m_ui8SlotIdx, AckInfo.m_ui16ChangeFlag, AckInfo.m_ui32EquipItem );
		}
		break;
#ifdef USE_GM_PAUSE
	case PROTOCOL_BATTLE_SERVER_GM_PAUSE_ACK			:
		{
			BATTLE_ROOM_INFO	stRoomInfo;
			INT32				i32Result;

			pPacket->ReadData( &stRoomInfo,	sizeof( BATTLE_ROOM_INFO ) );
			pPacket->ReadData( &i32Result,	sizeof( INT32 ) );

			CRoom* pRoom = _GetRoom( stRoomInfo._cChannel, stRoomInfo._sRoom );

			if ( pRoom == NULL) break;

			pRoom->GMPauseResult_C( i32Result );
		}
		break;
#endif
	case PROTOCOL_BATTLE_M_TOUCHDOWN_REQ:
		{
			BATTLE_ROOM_INFO	stRoomInfo;
			UINT32				ui32SlotIdx;

			pPacket->ReadData(&stRoomInfo,			sizeof(BATTLE_ROOM_INFO));
			pPacket->ReadData(&ui32SlotIdx,			sizeof(UINT32)			);

			CRoom* pRoom = _GetRoom( stRoomInfo._cChannel, stRoomInfo._sRoom );
			
			if ( NULL == pRoom ) break;

			CUserSession* pSession = pRoom->GetSession( ui32SlotIdx );

			if( NULL == pSession ) break;

			pRoom->MissionTouchDown( pSession );
		}
		break;
	default:
		g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX, NULL, "[CModuleControlSocket::PacketParsing]Unknown Message");
		break; 
	}

	m_ui32LastRecvTime	= g_pContextMain->GetServerTime();
	return; 
}

void CModuleControlSocket::_ReloadPacket( i3NetworkPacket* pPacket )
{
	//Set ClanCreateInfo
	pPacket->ReadData( &g_pContextMain->m_ui8ClanCreateRank,	sizeof( UINT8 )); 
	pPacket->ReadData( &g_pContextMain->m_i32ClanCreatePoint,	sizeof( INT32 )); 

	//Event Bonus - 2011.05.11 - 서동권 이벤트 보너스 갯수를 추가하여 그 수만큼만 보내고 받는다.
	pPacket->ReadData( &g_pContextMain->m_ui16EventBonusCount,	sizeof(UINT16) );
	pPacket->ReadData( g_pContextMain->m_stEventBouns,			sizeof(TYPE_EVENT_BONUS)*g_pContextMain->m_ui16EventBonusCount );
	pPacket->ReadData( &g_pContextMain->m_aRankUpItem,			sizeof(g_pContextMain->m_aRankUpItem) );
	pPacket->ReadData( &g_pContextMain->m_aTSEvent,				sizeof(g_pContextMain->m_aTSEvent) );

}

CRoom * CModuleControlSocket::_GetRoom(UINT8 ui8Channel, UINT16 ui16Room)
{
	if( NULL == g_pRoomManager ) return NULL; 

	if(ui8Channel >= (INT32)g_pRoomManager->GetChannelCount())
	{
		g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL, "GetRoom - Channel Error");
		return NULL; 
	}

	if(ui16Room >= VIEW_ROOM_COUNT )
	{
		g_pModuleLogFile->Write_M_Log(MODULE_CONTROL_THREAD_IDX,NULL, "GetRoom - Room Error");
		return NULL; 
	}

	CRoom* pRoom = g_pRoomManager->GetRoom( ui8Channel, ui16Room );

	return pRoom; 
}