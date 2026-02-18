#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
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

BOOL CModuleControlSocket::OnCreate( UINT32 ui32IP, UINT16 ui16PortNo)
{
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16PortNo);

	INT32 i32optVal = 2 * MByte; //2M
	INT32 i32optLen = sizeof(INT32);
	if (SOCKET_ERROR == setsockopt(GetSocket(), SOL_SOCKET, SO_SNDBUF, (char*)&i32optVal, i32optLen))	return FALSE;
	if (SOCKET_ERROR == setsockopt(GetSocket(), SOL_SOCKET, SO_RCVBUF, (char*)&i32optVal, i32optLen))	return FALSE;


	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

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
			g_pLog->WriteLog( L"[SUCC] Control Server Connect Messagr Recv");
			pPacket->ReadData( &m_ui32ControlSendTime, sizeof(UINT32)); 
			pPacket->ReadData( &g_pContextMain->m_ControlVersion,	sizeof(SS_VERSION));

			//처음 보내는 패킷을 보낸다. 
			INT32	i32ConnectType		= ASC_CONNECT_TYPE_SERVER;	//Server
			UINT8	ui8ServerType		= ASC_SERVER_GAME;			//Game Server Type
			//UINT16	ui16MaxUserCount	= (UINT16)(GAME_CHANNEL_COUNT * g_pContextMain->m_ui32ChannelSessionCount);

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData(&i32ConnectType,						sizeof(INT32));			//Server
			SendPacket.WriteData(&ui8ServerType,						sizeof(UINT8));			//GameServer
			SendPacket.WriteData(&g_pContextMain->m_Version,			sizeof(SS_VERSION));
			SendPacket.WriteData(g_pContextMain->m_pui32ProxyAddressIp,	sizeof(UINT32));
			SendPacket.WriteData(g_pContextMain->m_pui16AddressPort,	sizeof(UINT16));
			//SendPacket.WriteData(&ui16MaxUserCount,					sizeof(UINT16));

			SendPacketMessage(&SendPacket);
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				:
		{
			m_bConnect = TRUE;

			switch ( g_eServerState )
			{
			case SERVER_STATE_START :	// Control 서버 Reconnect된 상태
				{
					INT32 i32Result = EVENT_ERROR_SUCCESS;
					i3NetworkPacket ResultPacket( PROTOCOL_ASC_SERVER_LOADING_RESULT ); 
					ResultPacket.WriteData(&i32Result,			    			sizeof(INT32));			//Server
					SendPacketMessage(&ResultPacket);
				}
				// break; // 주석이 맞음.
			default :
				m_ui32LastRecvTime = i3ThreadTimer::GetServerTime();
				return;

			case SERVER_STATE_LOADING :
				break;

			}
			pPacket->ReadData( &g_pContextMain->m_eServerVersion,				sizeof(PB_VER));

			//Get Basic Info
			//Get CommonServer Info 
			pPacket->ReadData( &g_pContextMain->m_i32ServerIdx,				sizeof(INT32));

			pPacket->ReadData( &g_pContextMain->m_ui32TransServerIp,		sizeof(UINT32) );
			pPacket->ReadData( &g_pContextMain->m_ui16TransServerPort,		sizeof(UINT16) );
			pPacket->ReadData( &g_pContextMain->m_ui32ClanServerIp,			sizeof(UINT32) );
			pPacket->ReadData( &g_pContextMain->m_ui16ClanServerPort,		sizeof(UINT16) );
			pPacket->ReadData( &g_pContextMain->m_ui32MessengerServerIp,	sizeof(UINT32) );
			pPacket->ReadData( &g_pContextMain->m_ui16MessengerServerPort,	sizeof(UINT16) );
			pPacket->ReadData( &g_pContextMain->m_ui32LogDServerIp,			sizeof(UINT32) );
			pPacket->ReadData( &g_pContextMain->m_ui16LogDServerPort,		sizeof(UINT16) );
			pPacket->ReadData( &g_pContextMain->m_ui32CastServerIp,			sizeof(UINT32) );
			pPacket->ReadData( &g_pContextMain->m_ui16CastServerPort,		sizeof(UINT16) );
		
			pPacket->ReadData( &g_pContextMain->m_ui32GameServerCount,		sizeof(UINT32) ); 
			pPacket->ReadData( g_pContextMain->m_aGameServerInfo,			sizeof(GAME_SERVER_INFO) * g_pContextMain->m_ui32GameServerCount );

			//Get GameServer Info 
			g_pContextMain->m_pChannelType = (UINT8*)i3MemAlloc(sizeof(UINT8) * GAME_CHANNEL_COUNT);
			i3mem::FillZero(g_pContextMain->m_pChannelType, (sizeof(UINT8) * GAME_CHANNEL_COUNT) ); 

			// Channel & Have Channel Setting
			for( UINT32 i = 0; i < g_pContextMain->m_ui32GameServerCount ; i++ )
			{
				if( i+1 != (UINT32)g_pContextMain->m_i32ServerIdx ) continue;
				i3mem::Copy( g_pContextMain->m_pChannelType, g_pContextMain->m_aGameServerInfo[i+1].m_aui8ChannelType, sizeof(UINT8) * GAME_CHANNEL_COUNT  );
			}


			pPacket->ReadData( &g_pContextMain->m_ui32QuestEventDate,			sizeof(UINT32));
			pPacket->ReadData( &g_pContextMain->m_vlVoteLimit,					sizeof(VoteLimit));
			pPacket->ReadData( &g_pContextMain->m_chlChatHackLimit,				sizeof(ChatHackLimit));

			UINT16 aui16ChannelCount[MAX_GAME_SERVER_COUNT], ui16ExtraCount;
			pPacket->ReadData( aui16ChannelCount,								sizeof(UINT16) * g_pContextMain->m_ui32GameServerCount );
			pPacket->ReadData( &ui16ExtraCount,									sizeof(UINT16));
			g_pContextMain->SetUserCount( aui16ChannelCount, ui16ExtraCount );



			pPacket->ReadData( &g_pContextMain->m_i32ClanMax,					sizeof(INT32));
			if( 0 < g_pContextMain->m_i32ClanMax )
			{
				for( INT32 i = 0 ; i < CLAN_LIST_COUNT ; i++ )
				{
					g_pContextMain->m_ClanList[i] = (CLAN_LIST_BASIC_INFO*)i3MemAlloc(sizeof(CLAN_LIST_BASIC_INFO)*g_pContextMain->m_i32ClanMax);
					i3mem::FillZero( g_pContextMain->m_ClanList[i],				sizeof(CLAN_LIST_BASIC_INFO)*g_pContextMain->m_i32ClanMax );

				}
			}
			
			pPacket->ReadData( &g_pContextMain->m_ui8LocalCode,					sizeof(INT8));
			g_pContextMain->SetGeneralItem();

			//Set MD5 Key
			pPacket->ReadData( g_pContextMain->m_strMD5Key[0], NET_MD5_KEY_SIZE); 
			pPacket->ReadData( g_pContextMain->m_strMD5Key[1], NET_MD5_KEY_SIZE);

			pPacket->ReadData( &g_pContextMain->m_DailyGift,					sizeof(DAILY_GIFT) );	// 판수보상

			// Set Info Mission CardSet
			pPacket->ReadData( &g_pContextMain->m_ui8ActiveQuestCount,			sizeof(UINT8));
			if( 0 < g_pContextMain->m_ui8ActiveQuestCount )
			{
				g_pContextMain->m_pui32ActiveQuest = (UINT32*)i3MemAlloc(sizeof(UINT32) * g_pContextMain->m_ui8ActiveQuestCount);
				pPacket->ReadData( g_pContextMain->m_pui32ActiveQuest,			sizeof(UINT32)*g_pContextMain->m_ui8ActiveQuestCount);
			}
			
			// 신고하기 - 스크린샷 기능 사용 - 서동권
			UINT8 ui8ReportFtpAddressSize;
			pPacket->ReadData( &ui8ReportFtpAddressSize,						sizeof(UINT8) );
			pPacket->ReadData( g_pContextMain->m_strReportFtpAddress,			ui8ReportFtpAddressSize );
			g_pContextMain->m_strReportFtpAddress[ ui8ReportFtpAddressSize ] = '\0';
			pPacket->ReadData( &g_pContextMain->m_ui16ReportFtpPort,			sizeof(UINT16) );

			UINT8 ui8DefaultItemCount;
			DEFAULT_ITEM aDefaultItem[ DEFAULT_ITEM_COUNT ];

			pPacket->ReadData( &ui8DefaultItemCount,							sizeof(UINT8));
			pPacket->ReadData( aDefaultItem,									sizeof(DEFAULT_ITEM)*ui8DefaultItemCount );
			g_pContextMain->SetDefaultItem( ui8DefaultItemCount, aDefaultItem );
			
			// v1.5 Medal 
			pPacket->ReadData( &g_pContextMain->m_MedalSystemInfo,				sizeof(MEDAL_SYSTEM_INFO) );

			// 중독방지 시스템 - 안국정
			pPacket->ReadData( &g_pContextMain->m_AASOption,					sizeof(AAS_OPTION_DATA) );

			// Contents on/off state
			ContentsInfo aContentsList[CONTENTS_COUNT];
			pPacket->ReadData( aContentsList,									sizeof(aContentsList) );
			g_pContextMain->InsertContents(aContentsList);

			pPacket->ReadData( g_pContextMain->GetGiftBuyRanking(),				sizeof(GIFT_BUY_RANKING) );

			pPacket->ReadData( &g_pContextMain->m_ui8MultiWeaponCount,			sizeof(UINT8) );

 			// Clan match season
 			for( INT32 i = 0; i < SEASON_TYPE_COUNT; ++i )
 			{
 				CLAN_MATCH_SEASON stClanMatchSeason;
 				pPacket->ReadData( &stClanMatchSeason,							sizeof(stClanMatchSeason) );
 
 				g_pContextMain->InsertClanMatchSeason(static_cast<SEASON_TYPE>(i), &stClanMatchSeason);
 			}

			UINT8 ui8Count;
			pPacket->ReadData(&ui8Count, sizeof(UINT8));
			pPacket->ReadData(&g_pContextMain->m_pHackCheckRatio, sizeof(UINT8)*ui8Count);

			pPacket->ReadData(&g_pContextMain->m_ui8GiftShopRank, sizeof(UINT8));
			g_eServerState = SERVER_STATE_INIT;

			_ReloadPacket( pPacket );

		}
		break; 
	case PROTOCOL_ASC_DATABASE_CONFIG_ACK:
		{
			// Database config
			INT32 i32DBCount = 0;
			pPacket->ReadData(&i32DBCount, sizeof(i32DBCount));

			DATABASE_CONFIG	 stDatabaseConfig[MAX_DATABASE_COUNT];
			pPacket->ReadData(stDatabaseConfig, sizeof(DATABASE_CONFIG) * i32DBCount);
			for( INT32 i = 0; i < i32DBCount; ++i )
				g_pContextMain->InsertDatabaseConfig(&stDatabaseConfig[i]);
		}
		break;
	case PROTOCOL_ASC_BILLING_CONFIG_ACK:
		{
			UINT8 ui8BillingUrlCount = 0;

			pPacket->ReadData( &ui8BillingUrlCount, sizeof(UINT8) );

			for( UINT8 i = 0; i < ui8BillingUrlCount; ++i )
			{
				UINT8 ui8LinkMethod;
				UINT8 ui8BillingURLSize;

				pPacket->ReadData( &ui8LinkMethod, sizeof(UINT8) );
				pPacket->ReadData( &ui8BillingURLSize, sizeof(UINT8));
				pPacket->ReadData( g_pContextMain->m_astrBillingURL[ui8LinkMethod], ui8BillingURLSize );

			}
		}
		break;
	case PROTOCOL_ASC_CONFIG_RELOAD_ACK:
		{
			_ReloadPacket( pPacket );
			_ReLoadURLListPacket(pPacket);
		}
		break;
	case PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF:
		{
			ChangedHackCheckRatio sHackCheckRatio;

			pPacket->ReadData(&sHackCheckRatio, sizeof(ChangedHackCheckRatio));

			g_pContextMain->m_pHackCheckRatio.m_aui8HackCheckRatio[sHackCheckRatio.m_ui8HackCheckType] = sHackCheckRatio.m_ui8ChangedCheckRatio;
		}
		break;
		//PROTOCOL_BASE_SERVER_STATE_REQ, 
	case PROTOCOL_BASE_GAME_SERVER_STATE_ACK				: 
		{
			//Get GameServer Info 
			pPacket->ReadData( &g_pContextMain->m_ui32GameServerCount,	sizeof(UINT32) ); 
			pPacket->ReadData( g_pContextMain->m_aGameServerInfo,		sizeof(GAME_SERVER_INFO) * g_pContextMain->m_ui32GameServerCount );
			pPacket->ReadData( g_pContextMain->m_ui16ChannelUserCount,	sizeof(UINT16) * g_pContextMain->m_ui32GameServerCount*GAME_CHANNEL_COUNT );
		}
		break; 
	case PROTOCOL_ASC_SET_MD5_KEY_ACK				: 
		{
			pPacket->ReadData( g_pContextMain->m_strMD5Key[0], NET_MD5_KEY_SIZE); 
			pPacket->ReadData( g_pContextMain->m_strMD5Key[1], NET_MD5_KEY_SIZE); 
		}
		break; 
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_ACK	: 
		{
			g_pLog->WriteLog( L"CONTROL SERVER KICK USER MESSAGE RECV");
			g_pUserManager->KickAllSession(); 
		}
		break; 
	case PROTOCOL_BASE_NOTICE_ACK:
	{
		UINT16 ui16Len = 0;
		pPacket->ReadData(&ui16Len, sizeof(UINT16));
		g_pContextMain->m_i32SlidingSize = ui16Len;
		pPacket->ReadData(g_pContextMain->m_strSlidingNotice, sizeof(TTCHAR) * ui16Len);
		pPacket->ReadData(&g_pContextMain->m_i32SlidingRGB, sizeof(INT32));

		pPacket->ReadData(&ui16Len, sizeof(UINT16));
		g_pContextMain->m_i32ChatSize = ui16Len;
		pPacket->ReadData(g_pContextMain->m_strChatNotice, sizeof(TTCHAR) * ui16Len);
		pPacket->ReadData(&g_pContextMain->m_i32ChatRGB, sizeof(INT32));

	}
	break;
	case PROTOCOL_BASE_URL_LIST_ACK:
	{
		UINT16		ui16Len = 0;
		URL_LIST	*pulURLList = nullptr;

		g_pContextMain->m_ui8URLListCount = 0;
		i3mem::FillZero(g_pContextMain->m_ulURLList, sizeof(URL_LIST)*URL_MAX_COUNT);

		pPacket->ReadData(&g_pContextMain->m_ui8URLVersion, sizeof(UINT8));
		pPacket->ReadData(&g_pContextMain->m_ui8URLListCount, sizeof(UINT8));

		for (INT32 i = 0; i < g_pContextMain->m_ui8URLListCount; i++)
		{
			pulURLList = &g_pContextMain->m_ulURLList[i];

			pPacket->ReadData(&ui16Len, sizeof(UINT16));
			pPacket->ReadData(&pulURLList->m_i32Type, sizeof(INT32));
			pPacket->ReadData(&pulURLList->m_i32Data, sizeof(INT32));
			pPacket->ReadData(pulURLList->m_strURL, sizeof(wchar_t) *ui16Len);
		}
	}
	break;
	case PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_ACK		: 
		{
			UINT32	ui32Type;
			UINT32	ui32strLen;

			pPacket->ReadData(&ui32Type, sizeof(UINT32));
			pPacket->ReadData(&ui32strLen, sizeof(UINT32));

			switch (ui32Type)
			{
			case NOTICE_TYPE_NORMAL:
				if (ui32strLen > NOTICE_STRING_MAX_COUNT - 1)ui32strLen = NOTICE_STRING_MAX_COUNT - 1;
				pPacket->ReadData(g_pContextMain->m_strNormalNotice, sizeof(TTCHAR)*ui32strLen);
				g_pContextMain->m_strNormalNotice[ui32strLen] = '\0';
				if (NULL != g_pUserManager)
				{
					g_pUserManager->OnSendNoticeMessage(MODULE_CONTROL_THREAD_IDX, NOTICE_TYPE_NORMAL, g_pContextMain->m_strNormalNotice);
				}
				// Log 기록
				g_pLog->WriteLog(L"Admin Announce Normal Notice Message %s", g_pContextMain->m_strNormalNotice);
				break;
			case NOTICE_TYPE_EMERGENCY:
				if (ui32strLen > NOTICE_STRING_MAX_COUNT - 1)ui32strLen = NOTICE_STRING_MAX_COUNT - 1;
				pPacket->ReadData(g_pContextMain->m_strEmergencyNotice, sizeof(TTCHAR)*ui32strLen);
				g_pContextMain->m_strEmergencyNotice[ui32strLen] = '\0';
				if (NULL != g_pUserManager)
				{
					g_pUserManager->OnSendNoticeMessage(MODULE_CONTROL_THREAD_IDX, NOTICE_TYPE_EMERGENCY, g_pContextMain->m_strEmergencyNotice);
				}
				// Log 기록
				g_pLog->WriteLog(L"Admin Announce Emergency Notice Message %s", g_pContextMain->m_strEmergencyNotice);
				break;
			case NOTICE_TYPE_SLIDING:
				if (ui32strLen > NOTICE_STRING_COUNT - 1)ui32strLen = NOTICE_STRING_COUNT - 1;
				pPacket->ReadData(g_pContextMain->m_strSlidingNotice, sizeof(TTCHAR)*ui32strLen);
				pPacket->ReadData(&g_pContextMain->m_i32SlidingRGB, sizeof(INT32));
				g_pContextMain->m_strSlidingNotice[ui32strLen] = '\0';
				if (NULL != g_pUserManager)
				{
					g_pUserManager->OnSendNoticeMessage(MODULE_CONTROL_THREAD_IDX, NOTICE_TYPE_SLIDING, g_pContextMain->m_strSlidingNotice, g_pContextMain->m_i32SlidingRGB);
				}
				// Log 기록
				g_pLog->WriteLog(L"Admin Announce Sliding Notice Message %s", g_pContextMain->m_strSlidingNotice);
				break;
			case NOTICE_TYPE_CHAT:
				if (ui32strLen > NOTICE_STRING_COUNT - 1)ui32strLen = NOTICE_STRING_COUNT - 1;
				pPacket->ReadData(g_pContextMain->m_strChatNotice, sizeof(TTCHAR)*ui32strLen);
				pPacket->ReadData(&g_pContextMain->m_i32ChatRGB, sizeof(INT32));
				g_pContextMain->m_strChatNotice[ui32strLen] = '\0';
				if (NULL != g_pUserManager)
				{
					g_pUserManager->OnSendNoticeMessage(MODULE_CONTROL_THREAD_IDX, NOTICE_TYPE_CHAT, g_pContextMain->m_strChatNotice, g_pContextMain->m_i32ChatRGB);
				}
				// Log 기록
				g_pLog->WriteLog(L"Admin Announce Chat Notice Message %s", g_pContextMain->m_strChatNotice);
				break;
			}			
		}
		break;
	case PROTOCOL_BASE_MAP_RULELIST_ACK:
		{
			pPacket->ReadData( &g_pContextMain->m_ui8RuleTotalCount, sizeof(UINT8) );
			pPacket->ReadData( g_pContextMain->m_aRuleData, g_pContextMain->m_ui8RuleTotalCount * sizeof(MAP_RULE_DATA) );
		}
		break;
	case PROTOCOL_BASE_MAP_MATCHINGLIST_ACK:
		{
			UINT16 ui16CurItemCount;
			UINT16 ui16StartIdx;

			pPacket->ReadData(&g_pContextMain->m_ui16StageMatchingCount, sizeof(UINT16));
			pPacket->ReadData(&ui16CurItemCount, sizeof(UINT16));
			pPacket->ReadData(&ui16StartIdx, sizeof(UINT16));
			pPacket->ReadData(&g_pContextMain->m_aStageMatching[ui16StartIdx], (UINT16)(sizeof(MAP_STAGE_MATCHING)*ui16CurItemCount));
		}
		break;
	case PROTOCOL_BASE_MAP_RANDOM_LIST_ACK:
		{
			pPacket->ReadData(&g_pContextMain->m_ui16RandomMapCount, sizeof(UINT16));
			pPacket->ReadData(&g_pContextMain->m_aRandomMapData, (sizeof(MAP_RANDOM_STAGE_DATA)*g_pContextMain->m_ui16RandomMapCount));
		}
		break;
	case PROTOCOL_BASE_MAP_RANDOM_INDEX_ACK:
		{
			pPacket->ReadData(&g_pContextMain->m_aRandomMapIdxData, (sizeof(MAP_RANDOM_IDX_DATA)*RULE_DATA_MAX));
		}
		break;
	default:
		g_pLog->WriteLog( L"[CModuleControlSocket::PacketParsing]Unknown Message");
		break; 
	}

	m_ui32LastRecvTime = i3ThreadTimer::GetServerTime();
	return; 
}

void CModuleControlSocket::_ReloadPacket( i3NetworkPacket* pPacket )
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

void CModuleControlSocket::_ReLoadURLListPacket(i3NetworkPacket* pPacket)
{
	//Update URL List 
	UINT16		ui16Len = 0;
	URL_LIST	*pulURLList;

	g_pContextMain->m_ui8URLListCount = 0;
	i3mem::FillZero(g_pContextMain->m_ulURLList, sizeof(URL_LIST)*URL_MAX_COUNT);

	pPacket->ReadData(&g_pContextMain->m_ui8URLVersion, sizeof(UINT8));
	pPacket->ReadData(&g_pContextMain->m_ui8URLListCount, sizeof(UINT8));

	for (INT32 i = 0; i < g_pContextMain->m_ui8URLListCount; i++)
	{
		pulURLList = &g_pContextMain->m_ulURLList[i];

		pPacket->ReadData(&ui16Len, sizeof(UINT16));
		pPacket->ReadData(&pulURLList->m_i32Type, sizeof(INT32));
		pPacket->ReadData(&pulURLList->m_i32Data, sizeof(INT32));
		pPacket->ReadData(pulURLList->m_strURL, sizeof(wchar_t) *ui16Len);
	}
}

CRoom * CModuleControlSocket::_GetRoom(UINT8 ui8Channel, UINT16 ui16Room)
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