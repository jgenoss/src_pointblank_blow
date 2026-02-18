#include "pch.h"
#include "ModuleControlSocket.h"
#include "Logfile.h"
#include "GroupMaker.h"
#include "ModuleControl.h"
#include "UdpServer.h"

I3_CLASS_INSTANCE( CModuleControlSocket, i3NetworkClientSocket );


CModuleControlSocket::CModuleControlSocket(void)
{
	m_bConnect			= FALSE; 
	m_iControlSendTime	=  1000; 
	m_ui32PortIdx		= 0;
}

CModuleControlSocket::~CModuleControlSocket(void)
{
	OnDestroy();
}

BOOL CModuleControlSocket::OnCreate(char * Address, UINT16 PortNo)
{
	BOOL Rv = i3NetworkClientSocket::OnCreate(Address, PortNo);
	m_iLastRecvTime	= g_pConfig->GetServerTime();
	
	return Rv; 
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
	i3NetworkPacket SendPacket;
	SendPacket.Clear();
	BOOL	bSendMessage = FALSE; 

	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{
			WRITE_LOG_CONTROL("CONTROL SERVER CONNECT MESSAGE RECV");
			pPacket->ReadData( &m_iControlSendTime,		sizeof(UINT32)); 

			INT32 i32ConnectType	= ASC_CONNECT_TYPE_SERVER;			//Connect Type
			UINT8 ui8ServerType		= ASC_SERVER_BATTLE;				//Server Type
			UINT8 ui8BattleType		= BATTLE_SERVER_TYPE_RELAY;	//

			//처음 보내는 패킷을 보낸다. 
			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_ASC_CONFIRMATION_REQ );
			SendPacket.WriteData(&i32ConnectType,			sizeof(INT32));
			SendPacket.WriteData(&ui8ServerType,			sizeof(UINT8));
			SendPacket.WriteData(&g_pConfig->m_UdpActiveIp,	sizeof(UINT32));	// Server IP
			SendPacket.WriteData(&ui8BattleType,			sizeof(UINT8));	
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				: 
		{
			m_bConnect = TRUE;

			if( SERVER_STATE_LOADING != g_i32StartServer )	break;

			// 컨트롤 서버의 빌드 버전을 받는다.
			pPacket->ReadData( &g_ui8ConBuildVerMonth,	sizeof(UINT8));
			pPacket->ReadData( &g_ui8ConBuildVerDay,	sizeof(UINT8));

			pPacket->ReadData(&g_pConfig->m_wGroupCount,	sizeof(UINT32));
			pPacket->ReadData(&g_pConfig->m_cUdpPortCount,	sizeof(UINT8));

			if( 0 < g_pConfig->m_cUdpPortCount )
			{
				g_pConfig->m_pUdpPort = (UINT16*)i3MemAlloc( sizeof(UINT16) * g_pConfig->m_cUdpPortCount);
				pPacket->ReadData(g_pConfig->m_pUdpPort,		sizeof(UINT16)*g_pConfig->m_cUdpPortCount);

				g_i32StartServer = SERVER_STATE_INIT;
			}
		}
		break; 
	case PROTOCOL_BASE_HEART_BIT_ACK			: 		break;
	case PROTOCOL_BATTLE_SERVER_CREATE_REQ		: 
		{
			if( SERVER_STATE_START != g_i32StartServer )break;

			P_BATTLE_ROOM_CREATE_REQ BattleRoomInfo; 
			P_BATTLE_ROOM_CREATE_ACK AckInfo; 
			INT32 nResult = EVENT_ERROR_SUCCESS;

			pPacket->ReadData(&BattleRoomInfo, sizeof(P_BATTLE_ROOM_CREATE_REQ));
			i3mem::Copy( &AckInfo._RoomInfo, &BattleRoomInfo._RoomInfo, sizeof(BATTLE_ROOM_INFO)); 

			// 그룹 ID 생성
			AckInfo._GroupIdx = g_pGroupMaker->MakeGroupId_C();
			if( AckInfo._GroupIdx == INVALID_RELAY_GROUP_ID )
			{
				nResult = EVENT_ERROR_FAIL; 
				WRITE_LOG_CONTROL("[PROTOCOL_RELAY_REGISTER_GROUP_REQ] MakeGroupId Error");
			}

			// 그룹 등록
			if(nResult == EVENT_ERROR_SUCCESS)
			{				
				AckInfo._Key = g_pGroupMaker->RegisterGroup_C( &AckInfo._RoomInfo, AckInfo._GroupIdx );
				if ( AckInfo._Key == RELAY_ENCRYPTKEY_DEFAULT )
				{
					nResult = EVENT_ERROR_FAIL; 
					WRITE_LOG_CONTROL("[PROTOCOL_RELAY_REGISTER_GROUP_REQ] RegisterGroup Error");
				}
			}

			AckInfo._Port = g_pUdpServer->GetRestPort_C();
			g_pModuleControl->SendRelayRegisterGroupRequest( nResult, &AckInfo );
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_DESTROY_REQ		: 
		{
			if( SERVER_STATE_START != g_i32StartServer )break;

			P_BATTLE_ROOM_DESTROY_REQ RecvPacket; 
			pPacket->ReadData(&RecvPacket,	sizeof(P_BATTLE_ROOM_DESTROY_REQ));

			if( FALSE == g_pGroupMaker->RevokeGroup_C( RecvPacket._GroupIdx ) )
			{
				WRITE_LOG_CONTROL("[PROTOCOL_BATTLE_SERVER_DESTROY_REQ] RevokeGroup Error");
				break; 
			}
		}
		break; 
	case PROTOCOL_BATTLE_SERVER_USER_LEAVE_REQ	:
		{
			if( SERVER_STATE_START != g_i32StartServer )break;

			P_BATTLE_USER_LEAVE_REQ RecvPacket; 
			pPacket->ReadData(&RecvPacket, sizeof(P_BATTLE_USER_LEAVE_REQ));
			
			if( FALSE == g_pGroupMaker->WithdrawGroup_C( RecvPacket._wGroupIdx, RecvPacket._cSlotIdx) )
			{
				WRITE_LOG_CONTROL("[PROTOCOL_BATTLE_SERVER_DESTROY_REQ] Error");
				break; 
			}
		}
		break; 
	default:
		WRITE_LOG_CONTROL("[CModuleControlSocket::PacketParsing]Unknown Message");
		break; 
	}

	if(bSendMessage)	SendPacketMessage( &SendPacket ); 
	m_iLastRecvTime			= g_pConfig->GetServerTime();
	m_iHeartBitCheckTime	= g_pConfig->GetServerTime();
	return; 
}
