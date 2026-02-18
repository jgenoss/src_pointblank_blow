#include "pch.h"
#include "ModuleControlSocket.h"

I3_CLASS_INSTANCE( CModuleControlSocket, i3NetworkClientSocket );

CModuleControlSocket::CModuleControlSocket(void)
{	
	m_bConnect			= FALSE; 
	m_ui32SystemDelay	=  1000; 
}

CModuleControlSocket::~CModuleControlSocket(void)
{
	OnDestroy();
}

BOOL CModuleControlSocket::OnCreate( UINT32 ui32IP, UINT16 ui16Port, INT32 i32LogThreadIdx )
{
	if( FALSE == i3NetworkClientSocket::OnCreate( ui32IP, ui16Port ) ) return FALSE;

	m_i32LogThreadIdx	= i32LogThreadIdx;
	m_ui32LastRecvTime	= g_pSystem->GetServerTime(); 	
	m_ui32LastSendTime	= g_pSystem->GetServerTime();

	return TRUE; 
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

	switch( ProtocolID )
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{
			pPacket->ReadData( &m_ui32SystemDelay, sizeof(UINT32));

			////처음 보내는 패킷을 보낸다. 
			INT32 i32ConnectType	= ASC_CONNECT_TYPE_SERVER;	//Server
			UINT8 ui8ServerType		= ASC_SERVER_BILLING;		//Game Server Type 

			i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIRMATION_REQ ); 
			SendPacket.WriteData( &i32ConnectType,	sizeof(INT32) ); 
			SendPacket.WriteData( &ui8ServerType,	sizeof(UINT8) ); 
			SendPacketMessage(&SendPacket); 

			WriteLog( m_i32LogThreadIdx, "[CONT] First Packet Recv OK");
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				: 
		{
			m_bConnect = TRUE;

			if( SERVER_STATE_LOADING != g_i32StartServer )	break;

			g_i32StartServer = SERVER_STATE_INIT;
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_ACK				: 
		
		break;
	default:
		WriteLog( m_i32LogThreadIdx, "[CONT] Unknown Message : %d", ProtocolID );
		break; 
	}

	m_ui32LastRecvTime	= g_pSystem->GetServerTime(); 

	return; 
}
