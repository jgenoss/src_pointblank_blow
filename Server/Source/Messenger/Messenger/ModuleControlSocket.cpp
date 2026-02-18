#include "pch.h"
#include "ModuleControl.h"
#include "ModuleControlSocket.h"

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

BOOL CModuleControlSocket::OnCreate(char * pAddress, UINT16 ui16PortNo, INT32 i32LogIdx)
{
	m_i32LogIdx = i32LogIdx;

	BOOL bRv = i3NetworkClientSocket::OnCreate(pAddress, ui16PortNo);
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
			pPacket->ReadData( &m_ui32ControlSendTime, sizeof(UINT32));
			pPacket->ReadData( &g_pConfig->m_ControlVersion,	sizeof(SS_VERSION));

			//처음 보내는 패킷을 보낸다. 
			{
				INT32 i32ConnectType	= ASC_CONNECT_TYPE_SERVER;	//Server
				UINT8 ui8ServerType		= ASC_SERVER_MESSENGER;		//Game Server Type 

				i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ );
				SendPacket.WriteData(&i32ConnectType,					sizeof(INT32));
				SendPacket.WriteData(&ui8ServerType,					sizeof(UINT8));
				SendPacket.WriteData(&g_pConfig->m_Version,				sizeof(SS_VERSION));
				SendPacket.WriteData(&g_pConfig->m_ui16Port,			sizeof(UINT16));
				
				// Send Control Server
				SendPacketMessage(&SendPacket);
			}
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
					return;

				case SERVER_STATE_LOADING :
					break;
					
				}

			UINT32 ui32ServerCount = 0;

			pPacket->ReadData( &ui32ServerCount, sizeof(UINT32));
			g_pConfig->SetGameServerCount( ui32ServerCount );
			g_pConfig->m_pui32ServerIp = (UINT32*)i3MemAlloc( sizeof(UINT32) * ui32ServerCount );
			pPacket->ReadData( g_pConfig->m_pui32ServerIp, sizeof(UINT32) * ui32ServerCount );

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
		m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
		break;
	default:
		g_pLog->WriteLog( L"[CModuleControlSocket::PacketParsing]Unknown Message"  );
		break; 
	}
	return; 
}
