#include "pch.h"
#include "UserSession.h"
#include "UserManager.h"

I3_CLASS_INSTANCE(CSession, i3NetworkClientSocket); 

CSession::CSession()
{
	m_i32ConnectType	= ASC_CONNECT_TYPE_UNKNOWN;
	m_i32ServerType		= -1;
	m_i32ServerIdx		= -1;
	m_ui32LastRecvST	= 0;
	m_ui32LastMD5		= 0;
	m_bIsNormalDisconnect = TRUE;
}

CSession::~CSession()
{
	OnDestroy();
}

void CSession::OnCreate(SOCKET Socket, UINT32 ui32ConIp, char * pstrIpString )
{
	i3NetworkClientSocket::SetSocket( Socket );

	INT32 i32optVal = 2 * MByte; //2M
	INT32 i32optLen = sizeof(INT32);
	if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_SNDBUF, (char*)&i32optVal, i32optLen))	return;
	if (SOCKET_ERROR == setsockopt(Socket, SOL_SOCKET, SO_RCVBUF, (char*)&i32optVal, i32optLen))	return;

	m_ui32ConIp = ui32ConIp;
	i3String::Copy( m_strIpString, pstrIpString, SERVER_IP_SIZE );
	i3Net::GetIPToLongW( m_ui32ConIp, m_wstrIpString );

	//Set Type 
	m_i32ConnectType	= ASC_CONNECT_TYPE_UNKNOWN;

	//Set Auth
	m_i8Auth_Service	= 0;

	//Connect Ok
	//TOTAL_VERSION	ControlServerVer;

	// 여기서 업데이트 서버인지 체크를 할 수 없다......
	i3NetworkPacket	StartPacket( PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS );
	StartPacket.WriteData( &g_pControl->m_ui32UpdateTime, sizeof(UINT32)); 
	StartPacket.WriteData( &g_pControl->m_ControlVersion, sizeof(SS_VERSION));

	SendPacketMessage(&StartPacket);

	m_i32ServerIdx		= -1;
	m_ui32IsKillST		= 0;

	//Set Nick Name
	m_strId[0]			= 0;
	m_strPass[0]		= 0;
	m_ui32LastRecvST	= i3ThreadTimer::GetServerTime();
	g_pLog->WriteLog( L"Session Connect.");

	m_bIsNormalDisconnect = FALSE;

	return;
}

void CSession::OnDestroy(void)
{
	DisConnect();
}

void	CSession::DisConnect()
{
	// Log관련
	if(m_i32ServerIdx != -1)
	{	
		g_pLog->WriteLog( L"Disconnect [ServerType : %d][SeverIdx : %d]", m_i32ServerType, m_i32ServerIdx);

		g_pControl->DisConnectServer( m_i32ServerType, m_i32ServerIdx, this );
		m_i32ServerIdx = -1;
	}

	i3NetworkClientSocket::DisConnect();
}

BOOL	CSession::_CheckAdminAuth()
{
	if( ASC_CONNECT_TYPE_ADMIN	!= m_i32ConnectType )	return FALSE;
	if( SERVICE_AUTH_ADMIN		!= m_i8Auth_Service )	return FALSE;
	return TRUE;
}

void CSession::_KillUser()
{
	m_ui32IsKillST = i3ThreadTimer::GetServerTime();
}

void CSession::_PacketConfirmation( i3NetworkPacket* pPacket )
{
	UINT8	ui8Type;
	
	pPacket->ReadData(&ui8Type,			sizeof(UINT8)); //IsType
	m_i32ServerIdx = g_pControl->GetServerIdx( m_ui32ConIp, ui8Type );

	if( -1 == m_i32ServerIdx )
	{
		_KillUser();
		g_pLog->WriteLog( L"[MAIN] Confirmaion Fail[Type=%d]", ui8Type );
		return;
	}

	m_i32ConnectType	= ASC_CONNECT_TYPE_SERVER;
	m_i32ServerType		= (INT32)ui8Type;
	g_pLog->WriteLog( L"[MAIN] Confirmation Success[ServerType = %d,ServerIdx=%d]", ui8Type, m_i32ServerIdx );
	
	CLAN_MATCH_SEASON aClanMatchSeason[SEASON_TYPE_COUNT];
	for( INT32 i = 0; i < SEASON_TYPE_COUNT; ++i )
	{
		g_pControl->MakePacketClanMatchSeason( static_cast<SEASON_TYPE>(i), &aClanMatchSeason[i] );  
	} 

	i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_ACK );

	switch( ui8Type ) 
	{
	case ASC_SERVER_SIA			:
		{
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ServerVersion,	sizeof(SS_VERSION));
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ui16Port,		sizeof(UINT16));

			ContentsInfo aContentsList[CONTENTS_COUNT];
			g_pControl->GetContentsList(aContentsList);
			SendPacket.WriteData( aContentsList, sizeof(aContentsList) );
		}
		break;
	case ASC_SERVER_GATEWAY		:
		{
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ServerVersion,	sizeof(SS_VERSION));

			// 다른 서버일때 게임서버의 정보를 보내준다.
			SendPacket.WriteData( &g_pControl->m_ui32GameServerCount, sizeof( UINT32 ));
			for(UINT32 i = 0; i < g_pControl->m_ui32GameServerCount; i++)
				SendPacket.WriteData( &g_pControl->m_aGameServerList[i]._ui32Ip, sizeof( UINT32 ) );
		}
		break;
	case ASC_SERVER_MESSENGER	:
		{
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ServerVersion,	sizeof(SS_VERSION));
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ui16Port,		sizeof(UINT16));

			// 다른 서버일때 게임서버의 정보를 보내준다.
			SendPacket.WriteData( &g_pControl->m_ui32GameServerCount, sizeof( UINT32 ));
			for(UINT32 i = 0; i < g_pControl->m_ui32GameServerCount; i++)
				SendPacket.WriteData( &g_pControl->m_aGameServerList[i]._ui32Ip, sizeof( UINT32 ) );
		}
		break;
	case ASC_SERVER_TRANS		:
		{
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ServerVersion, sizeof(SS_VERSION));
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ui16Port,		sizeof(UINT16));


			SendPacket.WriteData( &g_pControl->m_eServerVersion,					sizeof(PB_VER) );

			SERVER_INFO_COMMON* pCommonServer;
			// Send ZLog
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_SIA );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,							sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,						sizeof(UINT16) );
			// Send ZLog
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_ZLOG );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,							sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,						sizeof(UINT16) );

			// Trans 서버는 NewUserPoint, 게임서버 정보를 보내준다. 
			SendPacket.WriteData( &g_pControl->m_ui32GameServerCount,				sizeof( UINT32 ));
			for(UINT32 i = 0; i < g_pControl->m_ui32GameServerCount; i++)
			{
				SendPacket.WriteData( &g_pControl->m_aGameServerList[i]._ui32Ip,	sizeof( UINT32 ) );
			}

			SendPacket.WriteData( &g_pControl->m_aAttendanceInfo,					sizeof( ATTENDANCE_INFO ) * ATTENDANCE_MAX_EVENT_COUNT );
			SendPacket.WriteData( &g_pControl->m_MedalSystemInfo,					sizeof( MEDAL_SYSTEM_INFO ) );
			SendPacket.WriteData( &g_pControl->m_AASOption,							sizeof(AAS_OPTION_DATA) );

			ContentsInfo aContentsList[CONTENTS_COUNT];
			g_pControl->GetContentsList(aContentsList);
			SendPacket.WriteData( aContentsList,									sizeof(aContentsList) );
			SendPacket.WriteData( aClanMatchSeason,									sizeof(CLAN_MATCH_SEASON)*SEASON_TYPE_COUNT);

			// Reload Packet은 별도로 보냅니다. 
			i3NetworkPacket ReloadPacket( PROTOCOL_ASC_CONFIG_RELOAD_ACK ); 
			g_pSessionManager->PacketMakeTransReConfig( &ReloadPacket );
			SendPacketMessage(&ReloadPacket); 
		}
		break;
	case ASC_SERVER_CLAN:
		{
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ServerVersion,	sizeof(SS_VERSION));
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ui16Port,		sizeof(UINT16));

			// Clan 서버일때 ClanMaxCount, 게임서버의 정보를 보내준다.
			SendPacket.WriteData( &g_pControl->m_i32ClanMaxCount,				sizeof(INT32));
			SendPacket.WriteData( &g_pControl->m_ui8ClanCreateMemberCount,		sizeof(UINT8) );
			SendPacket.WriteData( &g_pControl->m_ui32GameServerCount,			sizeof(UINT32));
			for(UINT32 i = 0; i < g_pControl->m_ui32GameServerCount; i++)
				SendPacket.WriteData( &g_pControl->m_aGameServerList[i]._ui32Ip,sizeof(UINT32) );

			ContentsInfo aContentsList[CONTENTS_COUNT];
			g_pControl->GetContentsList(aContentsList);
			SendPacket.WriteData(aContentsList, sizeof(aContentsList));

			SendPacket.WriteData( aClanMatchSeason,				sizeof(CLAN_MATCH_SEASON)*SEASON_TYPE_COUNT);
		}
		break;
	case ASC_SERVER_UPDATER:
#ifdef OLD_UPDATESERVER
		// 기존 업데이트서버는 버전체크를 하지 않으므로 컨트롤서버의 버전으로 대체
		g_pControl->m_aUpdateServerList[m_i32ServerIdx]._ServerVersion = g_pControl->m_ControlVersion;
#else
		// 버전 읽음
		pPacket->ReadData(&g_pControl->m_aUpdateServerList[m_i32ServerIdx]._ServerVersion, sizeof(SS_VERSION));
#endif
		break;
	case ASC_SERVER_BATTLE:
		{
			SERVER_INFO_BATTLE * pBattleServer = &g_pControl->m_aBattleServerList[ m_i32ServerIdx ];

			// 버전 읽음
			pPacket->ReadData( &pBattleServer->_ServerVersion, sizeof(SS_VERSION));

			
			pBattleServer->bNotAllowRoomCreation = false; // because of rebooting
			pBattleServer->_pSession = this;

			// initialize hack check ratio
			//memset(&g_pControl->m_pHackCheckRatioList[m_ServerIdx], 0, sizeof(ServerHackCheckRatio));
			g_pControl->m_pDediHackCheckRatioList[m_i32ServerIdx].SetDefaultValue();
			g_pControl->m_pui8NotAllowRoomCreationList[m_i32ServerIdx] = 0;

			//Read Info  
			pPacket->ReadData(&pBattleServer->_OutIP, sizeof(UINT32));
			pPacket->ReadData(&pBattleServer->_ui8Type, sizeof(UINT8));

			// Send Cast
			SERVER_INFO_COMMON* pCommonServer;
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_CAST, pBattleServer->m_ui32Group );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,		sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,	sizeof(UINT16) );

			//Write Info
			SendPacket.WriteData( &g_pControl->m_ui32BattleRoomCount,	sizeof(UINT32) );
			SendPacket.WriteData( &g_pControl->m_ui8BattlePortCount,	sizeof(UINT8) );
			SendPacket.WriteData( &g_pControl->m_aui16BattlePortList,	sizeof(UINT16)*g_pControl->m_ui8BattlePortCount );
			SendPacket.WriteData( &g_pControl->m_eServerVersion,		sizeof(PB_VER) );
			SendPacket.WriteData( &g_pControl->m_ui8MultiWeaponCount,	sizeof(UINT8) );			
			if( pBattleServer->_ui8Type == BATTLE_SERVER_TYPE_DEDICATION )
			{
				// 데디에 핵체크 기본값을 보낸다
				UINT8 aui8Ratio[MAX_PATH];
				UINT8 ui8Count = g_pControl->m_pDediHackCheckRatioList[m_i32ServerIdx].CopyToPacket(aui8Ratio, sizeof(aui8Ratio));
				I3ASSERT( ui8Count == HACK_TYPE_MAX );
				SendPacket.WriteData( &ui8Count, sizeof(UINT8));
				SendPacket.WriteData( aui8Ratio, ui8Count );

				// 컨트롤 툴 동기화 (패킷을 너무 많이 보낸다 수정필요)
				// 컨트롤 툴 초기값은 0이기 때문에 값이 있는 것만 보낸다.
				for(INT32 i = 1; i < ui8Count; i++)
				{
					if( aui8Ratio[i] ) 
					{
						i3NetworkPacket ConrolToolPacket(PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF);
						ChangedHackCheckRatio sChangedRatio;
										
						sChangedRatio.m_ui8HackCheckType = (UINT8)i;

						sChangedRatio.m_ui8ServerIdx			= (UINT8)m_i32ServerIdx;
						sChangedRatio.m_ui8ChangedCheckRatio	= aui8Ratio[i];

						ConrolToolPacket.WriteData(&sChangedRatio, sizeof(sChangedRatio));
						g_pSessionManager->SendToAllControlTools(ConrolToolPacket.GetPacketBuffer(), ConrolToolPacket.GetPacketSize());
						//I3TRACE("Send PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF\n");
					} // if
				} // for
			}
		}
		break;
	case ASC_SERVER_GAME:
		{
			/////////////////////////////////////////////////////////////////////////////////////////
			// 현재 Session 을 저장한다.
			SERVER_INFO_GAME* pGameServer = &g_pControl->m_aGameServerList[ m_i32ServerIdx ];
			pGameServer->_pSession = this;

			// 버전 읽음
			pPacket->ReadData( &pGameServer->_ServerVersion,			sizeof(SS_VERSION));
			pPacket->ReadData(&pGameServer->_OutIP,						sizeof(UINT32));
			pPacket->ReadData(&pGameServer->_OutPort,					sizeof(UINT16));

			/////////////////////////////////////////////////////////////////////////////////////////
			// Write
			SendPacket.WriteData( &g_pControl->m_eServerVersion,		sizeof(PB_VER) );
			SendPacket.WriteData( &m_i32ServerIdx,						sizeof(INT32) );
			
			//Write CommonServer Info 
			SERVER_INFO_COMMON* pCommonServer;
			// Send Trans
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_TRANS );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,		sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,	sizeof(UINT16) );
			// Send Clan
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_CLAN );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,		sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,	sizeof(UINT16) );
			// Send Messanger
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_MESSENGER );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,		sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,	sizeof(UINT16) );
			// Send ZLog
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_ZLOG );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,		sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,	sizeof(UINT16) );
			// Send Cast
			pCommonServer = g_pControl->GetServerInfo( ASC_SERVER_CAST, pGameServer->m_ui32Group );
			SendPacket.WriteData( &pCommonServer->_ui32Ip,		sizeof(UINT32) );
			SendPacket.WriteData( &pCommonServer->_ui16Port,	sizeof(UINT16) );

			/////////////////////////////////////////////////////////////////////////////////////////
			//Write GameServer Info
			SendPacket.WriteData( &g_pControl->m_ui32GameServerCount,	sizeof( UINT32 ));
			GAME_SERVER_INFO pServerInfo[ MAX_SERVER_COUNT ];
			for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
			{
				pServerInfo[i]._IsActive			= g_pControl->m_aGameServerList[i]._i32IsActive;
				pServerInfo[i]._Arg0				= g_pControl->m_aGameServerList[i]._i32Arg0;
				pServerInfo[i]._AllType				= g_pControl->m_aGameServerList[i]._AllType;
				pServerInfo[i]._OutIP				= g_pControl->m_aGameServerList[i]._OutIP;
				pServerInfo[i]._OutPort				= g_pControl->m_aGameServerList[i]._OutPort;
				pServerInfo[i]._ui16MaxUserCount	= g_pControl->m_aGameServerList[i]._ui16MaxUserCount;
				i3mem::Copy( pServerInfo[i].m_aui8ChannelType, g_pControl->m_aGameServerList[i]._EachType, sizeof(UINT8) * GAME_CHANNEL_COUNT );
			}
			SendPacket.WriteData( pServerInfo,									(UINT16)(sizeof(GAME_SERVER_INFO)*g_pControl->m_ui32GameServerCount) );
			SendPacket.WriteData( &g_pControl->m_ui32QuestEvent,				sizeof(UINT32));

			SendPacket.WriteData( &g_pControl->m_vlVoteLimit,					sizeof(VoteLimit));
			SendPacket.WriteData( &g_pControl->m_chlChatHackLimit,				sizeof(ChatHackLimit));

			// Server Info
			for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
			{
				SendPacket.WriteData( &g_pControl->m_aGameServerList[ i ]._ui16InitUserCount,		sizeof(UINT16));
			}

			SendPacket.WriteData( &pGameServer->_ui16ExtraCount,				sizeof(UINT16));
						
			SendPacket.WriteData( &g_pControl->m_i32ClanMaxCount,				sizeof(INT32 ));							
							
			SendPacket.WriteData( &g_pControl->m_ui8LocaleCode,					sizeof(UINT8));
			SendPacket.WriteData( g_pControl->m_strKey1,						NET_MD5_KEY_SIZE);
			SendPacket.WriteData( g_pControl->m_strKey2,						NET_MD5_KEY_SIZE);
			SendPacket.WriteData( &g_pControl->m_DailyGift,						sizeof(DAILY_GIFT) );
			SendPacket.WriteData( &g_pControl->m_ui8ActiveQuestCount,			sizeof(UINT8));
			SendPacket.WriteData( g_pControl->m_pui32ActiveQuest,				sizeof(UINT32)*g_pControl->m_ui8ActiveQuestCount);
							
			// 신고하기 - 스크린샷 기능 - 서동권
			UINT8 ui8ReportFtpAddressSize = (UINT8)i3String::Length( g_pControl->m_strReportFtpAddress );
			SendPacket.WriteData( &ui8ReportFtpAddressSize,						sizeof(UINT8) );
			SendPacket.WriteData( g_pControl->m_strReportFtpAddress,			ui8ReportFtpAddressSize );
			SendPacket.WriteData( &g_pControl->m_ui16ReportFtpPort,				sizeof(UINT16) );

			SendPacket.WriteData( &g_pControl->m_ui8DefaultItemCount,			sizeof(UINT8));
			SendPacket.WriteData( g_pControl->m_TDetaultItem,					sizeof(DEFAULT_ITEM)*g_pControl->m_ui8DefaultItemCount);
			
			SendPacket.WriteData( &g_pControl->m_MedalSystemInfo, sizeof( MEDAL_SYSTEM_INFO ) );

			// 중독방지 시스템 - 안국정
			SendPacket.WriteData( &g_pControl->m_AASOption,						sizeof(AAS_OPTION_DATA) );

			ContentsInfo aContentsList[CONTENTS_COUNT];
			g_pControl->GetContentsList(aContentsList);

			SendPacket.WriteData( aContentsList, sizeof(aContentsList) );

			SendPacket.WriteData( g_pControl->GetGiftBuyRanking(), sizeof(GIFT_BUY_RANKING) );
			SendPacket.WriteData( &g_pControl->m_ui8MultiWeaponCount, sizeof(UINT8) );
			SendPacket.WriteData( aClanMatchSeason,				sizeof(CLAN_MATCH_SEASON)*SEASON_TYPE_COUNT);
							
			UINT8 aui8Ratio[GS_HACK_TYPE_MAX];
			UINT8 ui8Count = g_pControl->m_pGameHackCheckRatioList[m_i32ServerIdx].CopyToPacket(aui8Ratio, GS_HACK_TYPE_MAX);
			SendPacket.WriteData(&ui8Count, sizeof(UINT8));
			SendPacket.WriteData(aui8Ratio, ui8Count);
			SendPacket.WriteData(&g_pControl->m_ui8GiftShopRank,  sizeof(UINT8));

			// Reload Packet은 별도로 보냅니다.
			// (이우진) 게임서버인덱스 문제로 다시 합쳐서 보냅니다.
			g_pSessionManager->PacketMakeGameReConfig( &SendPacket );

			UINT16 ui16Len;

			//공지내용은 별도의 패킷으로 보낸다.
			i3NetworkPacket PacketNotice(PROTOCOL_BASE_NOTICE_ACK);
			ui16Len = (UINT16)(i3String::Length(g_pControl->m_strSlidingNotice));
			g_pControl->m_strSlidingNotice[ui16Len] = '\0';
			PacketNotice.WriteData(&ui16Len, sizeof(UINT16));
			PacketNotice.WriteData(g_pControl->m_strSlidingNotice, sizeof(TTCHAR) * ui16Len);
			PacketNotice.WriteData(&g_pControl->m_i32SlidingRGB, sizeof(INT32));

			ui16Len = (UINT16)(i3String::Length(g_pControl->m_strChatNotice));
			g_pControl->m_strChatNotice[ui16Len] = '\0';
			PacketNotice.WriteData(&ui16Len, sizeof(UINT16));
			PacketNotice.WriteData(g_pControl->m_strChatNotice, sizeof(TTCHAR) *ui16Len);
			PacketNotice.WriteData(&g_pControl->m_i32ChatRGB, sizeof(INT32));
			SendPacketMessage(&PacketNotice);

			_SendMapDataPacket();
			_SendRandomMapDataPacket();
			_SendBannerURLPacket();
		}
		break;
	case ASC_SERVER_ZLOG		:
		{
			// 버전 읽음
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ServerVersion, sizeof(SS_VERSION));
			pPacket->ReadData(&g_pControl->m_aCommonServerList[m_i32ServerIdx]._ui16Port,		sizeof(UINT16));

			UINT32 iCommonCount = 1 + g_pControl->m_ui32GameServerCount;	// Trans + Game
			SendPacket.WriteData( &iCommonCount,								sizeof(UINT32) );

			SERVER_INFO_COMMON* pTrans = g_pControl->GetServerInfo( ASC_SERVER_TRANS );
			SendPacket.WriteData( &pTrans->_ui32Ip,								sizeof(UINT32) );
			for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
			{
				SendPacket.WriteData( &g_pControl->m_aGameServerList[i]._ui32Ip,sizeof(UINT32));
			}
		}
		break;
	case ASC_SERVER_CAST:
		{
			SERVER_INFO_CAST* pCastServer = &g_pControl->m_CastServerList[ m_i32ServerIdx ];

			// 버전 읽음
			pPacket->ReadData( &pCastServer->_ServerVersion,				sizeof(SS_VERSION));
			pPacket->ReadData( &pCastServer->_ui16Port,						sizeof(UINT16));

			SERVER_INFO_COMMON* pServer;
			SendPacket.WriteData( &pCastServer->m_ui32GroupGameCount,		sizeof(UINT32));
			for( UINT32 i = 0 ; i < pCastServer->m_ui32GroupGameCount ; i++ )
			{
				pServer = g_pControl->GetServerInfo( ASC_SERVER_GAME, pCastServer->m_ui32GroupGameIdx[ i ] );
				SendPacket.WriteData( &pServer->_ui32Ip,					sizeof(UINT32) );
			}
			SendPacket.WriteData( &pCastServer->m_ui32GroupBattleCount,		sizeof(UINT32));
			for( UINT32 i = 0 ; i < pCastServer->m_ui32GroupBattleCount ; i++ )
			{
				pServer = g_pControl->GetServerInfo( ASC_SERVER_BATTLE, pCastServer->m_ui32GroupBattleIdx[ i ] );
				SendPacket.WriteData( &pServer->_ui32Ip,					sizeof(UINT32) );
			}

			SendPacket.WriteData( &g_pControl->m_ui8StageTotalCount, sizeof(UINT8) );
			SendPacket.WriteData( &g_pControl->m_aStageData, sizeof(MAP_STAGE_DATA) * g_pControl->m_ui8StageTotalCount );
		}
		break;
	default:
		{
			_KillUser();
		}
		break;
	}

	ASC_SERVER_TYPE eServer = static_cast<ASC_SERVER_TYPE>(ui8Type); 

	SERVER_INFO_COMMON* pServer = g_pControl->GetServerInfo( eServer, m_i32ServerIdx );
	pServer->_i32IsActive = ASC_ICON_LOADING;

	// Database Config 정보는 별도 패킷으로 보냅니다.
	if( eServer != ASC_SERVER_BATTLE && eServer != ASC_SERVER_UPDATER )
	{
		DATABASE_CONFIG stDatabaseConfig[MAX_DATABASE_COUNT];
		g_pControl->MakePacketDBConfig(eServer, stDatabaseConfig);

		INT32 i32DBConfigCnt = g_pControl->GetDatabaseConfigCount(eServer);
 
		i3NetworkPacket PacketDatabaseConfig(PROTOCOL_ASC_DATABASE_CONFIG_ACK);
		PacketDatabaseConfig.WriteData( &i32DBConfigCnt, sizeof(INT32) );
		PacketDatabaseConfig.WriteData( stDatabaseConfig, (UINT16)( sizeof(DATABASE_CONFIG) * i32DBConfigCnt ));
		SendPacketMessage(&PacketDatabaseConfig); 
	}  

	
	if (eServer == ASC_SERVER_GAME)
	{
		// Game서버인 경우 BillingURL 정보를 별도로 보냅니다.
		i3NetworkPacket BillingUrlPacket(PROTOCOL_ASC_BILLING_CONFIG_ACK);
				
		BillingUrlPacket.WriteData( &g_pControl->m_ui8BillingUrlCount,			sizeof(UINT8) );

		for( UINT8 ui8LinkMethod = 0; ui8LinkMethod < LINK_METHOD_MAX; ++ui8LinkMethod)
		{
			UINT8 ui8BillingURLSize = static_cast<UINT8>( i3String::Length(g_pControl->m_astrBillingURL[ui8LinkMethod]) );

			if (0 == ui8BillingURLSize) continue;

			BillingUrlPacket.WriteData( &ui8LinkMethod,								sizeof(UINT8) );
			BillingUrlPacket.WriteData( &ui8BillingURLSize,							sizeof(UINT8) );
			BillingUrlPacket.WriteData( g_pControl->m_astrBillingURL[ui8LinkMethod],	ui8BillingURLSize );
			
		}		
		
		SendPacketMessage(&BillingUrlPacket);


	}
	

	SendPacketMessage( &SendPacket );
}

void CSession::_SendMapDataPacket()
{
	//RuleList는 별도 패킷으로 보낸다.
	i3NetworkPacket RuleListPacket(PROTOCOL_BASE_MAP_RULELIST_ACK);
	RuleListPacket.WriteData(&g_pControl->m_ui8RuleTotalCount, sizeof(UINT8));
	RuleListPacket.WriteData(g_pControl->m_aRuleData, g_pControl->m_ui8RuleTotalCount * sizeof(MAP_RULE_DATA));
	SendPacketMessage(&RuleListPacket);

	
	//RuleStageMatching 데이터는 별도 패킷으로 보낸다.
	INT32	i32PacketSize		= PACKETBUFFERSIZE - PACKETHEADERSIZE - sizeof(UINT16) - sizeof(UINT16) - sizeof(UINT16);
	UINT16	ui16MaxCount		= (UINT16)(i32PacketSize / sizeof(MAP_STAGE_MATCHING));
	UINT16	ui16RemainCount		= g_pControl->m_ui16StageMatchingCount;
	UINT16	ui16CurItemCount	= 0;
	UINT16	ui16StartIdx		= 0;
	bool	bSendContinue		= true;

	while (bSendContinue)
	{
		ui16StartIdx += ui16CurItemCount;
		if ( ui16RemainCount > ui16MaxCount ) { ui16CurItemCount = ui16MaxCount; }
		else { ui16CurItemCount = ui16RemainCount; bSendContinue = false; }
		ui16RemainCount -= ui16CurItemCount;

		i3NetworkPacket StageMatchingPacket(PROTOCOL_BASE_MAP_MATCHINGLIST_ACK);
		StageMatchingPacket.WriteData(&g_pControl->m_ui16StageMatchingCount, sizeof(UINT16));
		StageMatchingPacket.WriteData(&ui16CurItemCount, sizeof(UINT16));
		StageMatchingPacket.WriteData(&ui16StartIdx, sizeof(UINT16));
		StageMatchingPacket.WriteData(&g_pControl->m_aStageMatching[ui16StartIdx], (UINT16)(sizeof(MAP_STAGE_MATCHING)*ui16CurItemCount));
		SendPacketMessage(&StageMatchingPacket);
	}
		
}

void CSession::_SendRandomMapDataPacket()
{
	if (0 < g_pControl->m_ui16RandomMapCount)
	{
		//RandomMap 정보가 있으면 담아서 보낸다.
		i3NetworkPacket RandomMpaDataPacket(PROTOCOL_BASE_MAP_RANDOM_LIST_ACK);
		RandomMpaDataPacket.WriteData(&g_pControl->m_ui16RandomMapCount, sizeof(UINT16));
		RandomMpaDataPacket.WriteData(g_pControl->m_aRandomMapData, g_pControl->m_ui16RandomMapCount * sizeof(MAP_RANDOM_STAGE_DATA));
		SendPacketMessage(&RandomMpaDataPacket);

		//RandomMap Idx 정보
		i3NetworkPacket RandomMapIdxPacket(PROTOCOL_BASE_MAP_RANDOM_INDEX_ACK);
		RandomMapIdxPacket.WriteData(g_pControl->m_aRandomMapIdxData, RULE_DATA_MAX * sizeof(MAP_RANDOM_IDX_DATA));
		SendPacketMessage(&RandomMapIdxPacket);
	}
}

void CSession::_SendBannerURLPacket()
{
	i3NetworkPacket SendBannerURLPacket(PROTOCOL_BASE_URL_LIST_ACK);
	UINT16		ui16Len = 0;
	URL_LIST	*pulURLList = nullptr;

	SendBannerURLPacket.WriteData(&g_pControl->m_ui8URLVersion, sizeof(UINT8));
	SendBannerURLPacket.WriteData(&g_pControl->m_ui8URLListCount, sizeof(UINT8));

	for (INT32 i = 0; i < g_pControl->m_ui8URLListCount; i++)
	{
		pulURLList = &g_pControl->m_ulURLList[i];
		ui16Len = (UINT16)(i3String::Length(pulURLList->m_strURL));
		SendBannerURLPacket.WriteData(&ui16Len, sizeof(UINT16));
		SendBannerURLPacket.WriteData(&pulURLList->m_i32Type, sizeof(INT32));
		SendBannerURLPacket.WriteData(&pulURLList->m_i32Data, sizeof(INT32));
		SendBannerURLPacket.WriteData(pulURLList->m_strURL, sizeof(wchar_t) *ui16Len);
	}

	SendPacketMessage(&SendBannerURLPacket);
}