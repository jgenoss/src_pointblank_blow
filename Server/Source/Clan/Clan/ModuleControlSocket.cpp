#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"
#include "TaskProcessor.h"
//#include "Logfile.h"
//
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

BOOL CModuleControlSocket::OnCreate( UINT32 ui32ControlIP, UINT16 ui16PortNo, INT32 i32LogIdx )
{
	m_i32LogIdx = i32LogIdx;

	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32ControlIP, ui16PortNo );
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
			g_pLog->WriteLog( L"CONTROL SERVER CONNECT MESSAGE RECV");
			pPacket->ReadData( &m_ui32ControlSendTime, sizeof(UINT32)); 
			pPacket->ReadData( &g_pConfig->m_ControlVersion,	sizeof(SS_VERSION));

			//처음 보내는 패킷을 보낸다. 
			INT32 i32ConnectType	= ASC_CONNECT_TYPE_SERVER;	//Server
			UINT8 ui8ServerType		= ASC_SERVER_CLAN;			//Clan Server Type 

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData(&i32ConnectType,			sizeof(INT32));
			SendPacket.WriteData(&ui8ServerType,			sizeof(UINT8));
			SendPacket.WriteData(&g_pConfig->m_ClanVersion, sizeof(SS_VERSION));
			SendPacket.WriteData(&g_pConfig->m_ui16Port,	sizeof(UINT16));
			SendPacketMessage(&SendPacket); 
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				: 
		{
			m_bConnect = TRUE;

			switch ( g_eServerState )
			{
			case SERVER_STATE_START :	// Control 서버 Reconnect된 상태
				g_pModuleControl->ServerInit( EVENT_ERROR_SUCCESS );
				// break; // 주석이 맞음.
			default :
				m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
				m_ui32LastSendTime	= i3ThreadTimer::GetServerTime(); 
				return ; 
			case SERVER_STATE_LOADING :
				break;	
			}

			//// 컨트롤 서버의 빌드 버전을 받는다.
			//pPacket->ReadData( &g_ui8ConBuildVerMonth,	sizeof(UINT8));
			//pPacket->ReadData( &g_ui8ConBuildVerDay,	sizeof(UINT8));

			//// 컨트롤 서버의 빌드 버전을 받는다. (New)
			//pPacket->ReadData(&g_pConfig->m_ControlVersion, sizeof(SS_VERSION));

			pPacket->ReadData( &g_pConfig->m_ui32ClanMax,			sizeof(UINT32));
			pPacket->ReadData( &g_pConfig->m_ui8CreateMemberCount,	sizeof(UINT8));

			UINT32 ui32ServerCount = 0;
			pPacket->ReadData( &ui32ServerCount,					sizeof(UINT32));
			if( ui32ServerCount > 0 )
			{
				g_pConfig->SetGameServerCount( ui32ServerCount );
				g_pConfig->m_pui32ServerIp = (UINT32*)i3MemAlloc( sizeof(UINT32) * ui32ServerCount );

				pPacket->ReadData( g_pConfig->m_pui32ServerIp, sizeof(UINT32) * ui32ServerCount );
			}

			// Contents on/off state
			ContentsInfo aContentsList[CONTENTS_COUNT];
			pPacket->ReadData(aContentsList, sizeof(aContentsList));
			g_pConfig->InsertContents(aContentsList);

			// Clan match season
			for( INT32 i = 0; i < SEASON_TYPE_COUNT; ++i )
			{
				CLAN_MATCH_SEASON stClanMatchSeason;
				pPacket->ReadData(&stClanMatchSeason, sizeof(stClanMatchSeason));

				g_pConfig->InsertClanMatchSeason(static_cast<SEASON_TYPE>(i), &stClanMatchSeason);
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
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		break;
	case PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_REQ		:
		{
			INT32	i32WebSessionIdx = 0;
			UINT32	ui32CID = 0;
			INT32	i32Rv = EVENT_ERROR_FAIL;
			INT8	i8Count = 0;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &ui32CID,			sizeof(UINT32) );

			// 클랜이 존재하는지
			CLAN_DETAIL_INFO_SERVER* pstClan = g_pTaskProcessor->FindClan2( ui32CID );
			if( pstClan )
			{
				i32Rv = EVENT_ERROR_SUCCESS;
				BATTLE_CLAN_INFO* pBattleClanInfo = g_pTaskProcessor->m_pBattleTeam->FindClanTeam( ui32CID );
				if( pBattleClanInfo )
				{	
					i8Count = pBattleClanInfo->_stClanContext._ui8Totalcount;
				}
			}

			i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_ACK );
			SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );
			SendPacket.WriteData( &i32Rv,				sizeof(INT32) );
			SendPacket.WriteData( &i8Count,				sizeof(INT8)  );
			SendPacketMessage( &SendPacket );
		}
		break;

	default:
		g_pLog->WriteLog( L"[CModuleControlSocket::PacketParsing]Unknown Message");
		break; 
	}

	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

	return; 
}
