#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "Config.h"
#include "TaskProcessor.h"
#include "GeneralRankup.h"

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

BOOL CModuleControlSocket::OnCreate( UINT32 ui32IP, UINT16 ui16Port )
{
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16Port);
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
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
			g_pLog->WriteLog( L"[SUCC] Control Server Connect" );

			pPacket->ReadData( &m_ui32ControlSendTime,			sizeof(UINT32)); 
			pPacket->ReadData( &g_pConfig->m_ControlVersion,	sizeof(SS_VERSION));

			//처음 보내는 패킷을 보낸다. 
			INT32 i32ConnectType	= ASC_CONNECT_TYPE_SERVER;	//Server
			UINT8 ui8ServerType		= ASC_SERVER_TRANS;			//Game Server Type 

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData(&i32ConnectType,				sizeof(INT32)); 
			SendPacket.WriteData(&ui8ServerType,				sizeof(UINT8));
			SendPacket.WriteData(&g_pConfig->m_Version,			sizeof(SS_VERSION));
			SendPacket.WriteData(&g_pConfig->m_ui16Port,		sizeof(UINT16));
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
					g_pModuleControl->ServerInit( EVENT_ERROR_SUCCESS );
				}
				// break; // 주석이 맞음.
			default :
				return; 
			case SERVER_STATE_LOADING :
				break;

			}

			pPacket->ReadData( &g_pConfig->m_eServerVersion,		sizeof(PB_VER) );

			pPacket->ReadData( &g_pConfig->m_ui32SIAServerIP,		sizeof(UINT32) );
			pPacket->ReadData( &g_pConfig->m_ui16SIAServerPort,		sizeof(UINT16) );
			pPacket->ReadData( &g_pConfig->m_ui32ZLogServerIP,		sizeof(UINT32) );
			pPacket->ReadData( &g_pConfig->m_ui16ZLogServerPort,	sizeof(UINT16) );

			UINT32	 ui32ServerCount;
			UINT32 * pui32ServerList;
			pPacket->ReadData( &ui32ServerCount,				sizeof(UINT32) );
			pui32ServerList = (UINT32*)i3MemAlloc( sizeof(UINT32)*ui32ServerCount );
			pPacket->ReadData( pui32ServerList,					sizeof(UINT32)*ui32ServerCount );
			g_pConfig->SetServerInfo( ui32ServerCount, pui32ServerList );
			I3MEM_SAFE_FREE( pui32ServerList );

			pPacket->ReadData( &g_pConfig->m_aAttendanceInfo,	sizeof(ATTENDANCE_INFO)*ATTENDANCE_MAX_EVENT_COUNT );
			pPacket->ReadData( &g_pConfig->m_MedalSystemInfo,	sizeof(MEDAL_SYSTEM_INFO) );
			pPacket->ReadData( &g_pConfig->m_AASOption,			sizeof(AAS_OPTION_DATA) );

			// Content on/off state
			ContentsInfo aContentsList[CONTENTS_COUNT];
			pPacket->ReadData( aContentsList,					sizeof(aContentsList) );
			g_pConfig->InsertContents(aContentsList);

			// Clan match season
			for( INT32 i = 0; i < SEASON_TYPE_COUNT; ++i )
			{
				CLAN_MATCH_SEASON stClanMatchSeason;
				pPacket->ReadData( &stClanMatchSeason,			sizeof(stClanMatchSeason) );
				g_pConfig->InsertClanMatchSeason( static_cast<SEASON_TYPE>(i), &stClanMatchSeason);
			}

			if( g_pConfig->IsContentsEnabled(CONTENTS_GACHA) )
			{
				i3NetworkPacket stGachaNotice(PROTOCOL_SS_GACHA_NOTICE_REQ);
				SendPacketMessage(&stGachaNotice);
			}

			g_eServerState = SERVER_STATE_INIT;
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
				g_pConfig->InsertDatabaseConfig(&stDatabaseConfig[i]);
		}
		break;
	case PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_ACK		: 
		{
			g_pTaskProcessor->AllUserKick();
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		m_ui32LastRecvTime = i3ThreadTimer::GetServerTime();
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_ACK	:
		{
		}
		break;
	case PROTOCOL_ASC_CONFIG_RELOAD_ACK :
		{
			_ReloadPacket( pPacket );
		}
		break;
	case PROTOCOL_ASC_WEB_GM_KICK_REQ :
		{
			INT32	i32WebSessionIdx = -1;
			INT8	i8Count = 0;
			T_UID	i64UID;
			INT32	i32Rv = EVENT_ERROR_SUCCESS;

			pPacket->ReadData( &i32WebSessionIdx,		sizeof(INT32) );
			pPacket->ReadData( &i8Count,				sizeof(INT8) );

			for( INT8 i = 0; i < i8Count; i++ )
			{
				pPacket->ReadData( &i64UID,				sizeof(T_UID) );

				ASC_WEB_REQ_INFO *pReqInfo;

				if( g_pTaskProcessor->GetASCWebReqInfoCount() < TASK_RING_BUFFER_COUNT )
				{
					pReqInfo = g_pTaskProcessor->GetASCWebReqInfoInsertPtr();
					
					pReqInfo->reqType			= ASC_WEB_GM_KICK;
					pReqInfo->i32WebSessionIdx	= i32WebSessionIdx;
					pReqInfo->i64UID			= i64UID;

					g_pTaskProcessor->GetASCWebReqInfoInsertIdx();
				}
				else
				{
					i32Rv = EVENT_ERROR_FAIL;
				}

			}

			i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_GM_KICK_ACK );
			SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );
			SendPacket.WriteData( &i32Rv,				sizeof(INT32) );
			SendPacketMessage( &SendPacket );
		}
		break;
	case PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ :
		{
			INT32	i32WebSessionIdx = -1;
			T_UID	i64UID = 0;
			INT32	i32Rv = EVENT_ERROR_FAIL;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i64UID,				sizeof(T_UID) );

			ASC_WEB_REQ_INFO *pReqInfo;

			if( g_pTaskProcessor->GetASCWebReqInfoCount() < TASK_RING_BUFFER_COUNT )
			{
				pReqInfo = g_pTaskProcessor->GetASCWebReqInfoInsertPtr();
				
				pReqInfo->reqType			= ASC_WEB_USER_INFO_LOCATION;
				pReqInfo->i32WebSessionIdx	= i32WebSessionIdx;
				pReqInfo->i64UID			= i64UID;

				g_pTaskProcessor->GetASCWebReqInfoInsertIdx();
			}
			else
			{
				i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK );
				SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );
				SendPacket.WriteData( &i32Rv,				sizeof(INT32) );
				SendPacketMessage( &SendPacket );
			}
		}
		break;


	case PROTOCOL_SS_GACHA_NOTICE_ACK:
		{

			PACKET_GACHA_NOTICE stNotice[GACHA_NOTICE_COUNT];

			UINT8 ui8NoticeCount;
			pPacket->ReadData(&ui8NoticeCount, sizeof(UINT8));

			ui8NoticeCount = MIN( ui8NoticeCount, GACHA_NOTICE_COUNT );

			pPacket->ReadData(stNotice, sizeof(PACKET_GACHA_NOTICE) * ui8NoticeCount );

			CGachaShop* pGachaShop = g_pConfig->GetGachaShop();

			for( UINT8 i = 0 ; i < ui8NoticeCount ; ++i )
			{
				pGachaShop->InsertNotice(stNotice[i].m_szNotice);
			}

			g_pLog->WriteLog( L"[CModuleControlSocket::PacketParsing] Gacha notice inserted.");

			GACHA_GROUP eGachaGroup = GACHA_GROUP_POOR;
			i3NetworkPacket stGachaReq(PROTOCOL_SS_GACHA_ITEM_INFO_REQ);
			stGachaReq.WriteData( &eGachaGroup,	sizeof(GACHA_GROUP) );
			SendPacketMessage(&stGachaReq);
		}
		break;

	
	case PROTOCOL_SS_GACHA_ITEM_INFO_ACK:
		{
			SS_GACHA_GROUP_INFO stGachaGroup;

			CGachaShop* pGachaShop = g_pConfig->GetGachaShop(); 

			bool	bActive = false;

			pPacket->ReadData(&stGachaGroup.m_eGachaGroup,			sizeof(GACHA_GROUP));
			pPacket->ReadData(&bActive,								sizeof(bool)); 
			
			if( bActive )
			{
				//pPacket->ReadData(&stGachaGroup.m_ui32PaymentGoodsID,	sizeof(UINT32));
				//pPacket->ReadData(&stGachaGroup.m_i32RequireTicket,		sizeof(INT32));
				pPacket->ReadData(&stGachaGroup.m_ui32LuckyGoodsID,		sizeof(UINT32));
				pPacket->ReadData(&stGachaGroup.m_i32LuckyProbability,	sizeof(INT32));
				//pPacket->ReadData(&stGachaGroup.m_i32GloryCount,		sizeof(INT32));

				pPacket->ReadData(&stGachaGroup.m_i32WinItemCount,		sizeof(INT32));
				pPacket->ReadData(stGachaGroup.m_arWinItems,			sizeof(SS_GACHA_ITEM_INFO)*stGachaGroup.m_i32WinItemCount);

				pPacket->ReadData(&stGachaGroup.m_i32RandomItemCnt,		sizeof(INT32));
				pPacket->ReadData(stGachaGroup.m_arRandomGoodsIDs,		sizeof(UINT32)*stGachaGroup.m_i32RandomItemCnt);

				
				pGachaShop->SetGachaGroupInfo(&stGachaGroup);
				pGachaShop->Initialize(stGachaGroup.m_eGachaGroup); 
			}

			g_pLog->WriteLog( L"[Gacha] GachaGroup Setting Success / GroupIdx : %d / Active : %d", stGachaGroup.m_eGachaGroup, bActive );

			INT32 i32GroupIdx = (INT32)stGachaGroup.m_eGachaGroup;
			++i32GroupIdx;
			GACHA_GROUP eGachaGroup = (GACHA_GROUP)i32GroupIdx;

			if( eGachaGroup >= GACHA_GROUP_COUNT ) 
			{
				if( !pGachaShop->SaveGachaShopFile() )
				{
					g_pLog->WriteLog( L"[Gacha] Fail Create GachaShopFile");
				}
				else
				{
					g_pLog->WriteLog( L"[Gacha] Success Create GachaShopFile");
				}
				break;
			
			}

			i3NetworkPacket stGachaReq(PROTOCOL_SS_GACHA_ITEM_INFO_REQ);
			stGachaReq.WriteData( &eGachaGroup,	sizeof(GACHA_GROUP) );
			SendPacketMessage(&stGachaReq);
			
		}
		break;

	default:
		g_pLog->WriteLog( L"[CModuleControlSocket::PacketParsing]Unknown Message : %d ", ProtocolID  );
		break; 
	}
	return; 
}

void CModuleControlSocket::_ReloadPacket( i3NetworkPacket* pPacket )
{
	pPacket->ReadData( &g_pConfig->m_ui32NewUserPoint,			sizeof(UINT32));

	pPacket->ReadData( g_pConfig->m_i32GeneralRank,				sizeof(INT32)*GENERAL_RANK_COUNT );
	pPacket->ReadData( &g_pConfig->m_i32GeneralRankStartTime,	sizeof(INT32));
	
	pPacket->ReadData( &g_pConfig->m_ui16EventItemNewCount,		sizeof(UINT16) );
	pPacket->ReadData( g_pConfig->m_aEventItemNew,				sizeof(NEW_USER_EVENT)*g_pConfig->m_ui16EventItemNewCount );

	pPacket->ReadData( &g_pConfig->m_ui16EventItemLoginCount,	sizeof(UINT16) );
	pPacket->ReadData( g_pConfig->m_aEventItemLogin,			sizeof(LOGIN_USER_EVENT)*g_pConfig->m_ui16EventItemLoginCount );

	pPacket->ReadData( &g_pConfig->m_ui8IPBlockIPCount,			sizeof(UINT8) );
	pPacket->ReadData( g_pConfig->m_aIPBlockIPOpen,				(UINT16)(sizeof(IPBLOCK_IP)*g_pConfig->m_ui8IPBlockIPCount) );

	pPacket->ReadData( &g_pConfig->m_ui8IPBlockNationCount,		sizeof(UINT8) );
	pPacket->ReadData( g_pConfig->m_aui8IPBlockNationOpen,		(UINT16)(sizeof(UINT8)*g_pConfig->m_ui8IPBlockNationCount) );	

	if (g_pTaskProcessor)
	{
		g_pTaskProcessor->SetIPBlockOpenList(g_pConfig->IsContentsEnabled(CONTENTS_IP_BLOCK), g_pConfig->m_ui8IPBlockIPCount, g_pConfig->m_aIPBlockIPOpen, g_pConfig->m_ui8IPBlockNationCount, g_pConfig->m_aui8IPBlockNationOpen);
		g_pTaskProcessor->SetGeneralRankUpStartHour(g_pConfig->m_i32GeneralRankStartTime);
	}

	g_pConfig->SetUserEvent();
}