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

BOOL CModuleControlSocket::OnCreate( UINT32 ui32ControlIP, UINT16 ui16PortNo )
{	
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
			g_pLog->WriteLog(  L"CONTROL SERVER CONNECT MESSAGE RECV");
			pPacket->ReadData( &m_ui32ControlSendTime, sizeof(UINT32)); 
			pPacket->ReadData( &g_pConfig->m_ControlVersion,	sizeof(SS_VERSION));

			//처음 보내는 패킷을 보낸다. 
			INT32 i32ConnectType	= ASC_CONNECT_TYPE_SERVER;	//Server
			UINT8 ui8ServerType		= ASC_SERVER_CAST;			//Clan Server Type 

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData(&i32ConnectType,			sizeof(INT32));
			SendPacket.WriteData(&ui8ServerType,			sizeof(UINT8));
			SendPacket.WriteData(&g_pConfig->m_Version,		sizeof(SS_VERSION));
			SendPacket.WriteData(&g_pConfig->m_ui16Port,	sizeof(UINT16));
			SendPacketMessage(&SendPacket); 
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				: 
		{
			g_pLog->WriteLog(L"CONTROL SERVER CONFIRMATION RECV");

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

			INT32 i32ServerCount;
			UINT32 ui32IP;
			pPacket->ReadData( &i32ServerCount,					sizeof(INT32));
			g_pConfig->SetGameServerCount( i32ServerCount );
			for( INT32 i = 0 ; i < i32ServerCount ; i++ )
			{
				pPacket->ReadData( &ui32IP,						sizeof(UINT32));
				g_pConfig->SetGameServerIP( i, ui32IP );
			}
			pPacket->ReadData( &i32ServerCount,					sizeof(INT32));
			g_pConfig->SetBattleServerCount( i32ServerCount );
			for( INT32 i = 0 ; i < i32ServerCount ; i++ )
			{
				pPacket->ReadData( &ui32IP,						sizeof(UINT32));
				g_pConfig->SetBattleServerIP( i, ui32IP );
			}

			UINT8			ui8StageTotalCount;
			MAP_STAGE_DATA	aStageData[STAGE_TOTAL_MAX];

			pPacket->ReadData( &ui8StageTotalCount, sizeof(UINT8) );
			pPacket->ReadData( aStageData, sizeof(MAP_STAGE_DATA) * ui8StageTotalCount );

			g_pConfig->m_MapLoadManager.Create( i32ServerCount, ui8StageTotalCount, aStageData );
			g_pConfig->m_MapLoadManager.MakeMapLoadList();
				
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
	case PROTOCOL_ASC_NOTALLOW_ROOMCREATION_NTF:
		{
			UINT8	ui8DediIndex;

			pPacket->ReadData( &ui8DediIndex, sizeof(UINT8) );

			g_pConfig->SetAllowRoomCreate( ui8DediIndex, FALSE );

			g_pLog->WriteLog(L"CONTROL SERVER NOTALLOW_ROOMCREATION RECV / Dedi Idx : %d ", ui8DediIndex);
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		break;

	default:
		g_pLog->WriteLog(  L"[CModuleControlSocket::PacketParsing]Unknown Message");
		break; 
	}

	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

	return; 
}
