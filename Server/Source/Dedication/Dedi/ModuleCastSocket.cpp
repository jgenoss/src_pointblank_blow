#include "pch.h"
#include "ModuleCast.h"
#include "ModuleCastSocket.h"
#include "GroupMaker.h"
#include "DediRoom.h"
#include "ModuleControl.h"
#include "ModuleCast.h"
#include "iocpServer.h"
#include "MapManager.h"

extern char* g_pszHackName[];

I3_CLASS_INSTANCE( CModuleCastSocket, i3NetworkClientSocket );


CModuleCastSocket::CModuleCastSocket(void)
{
	m_bConnect				= FALSE;
	m_ui32CastSendTime	= 1000;
	m_i32PortIdx			= 0;
	m_ui32LastRecvTime		= 0;
	m_ui32LastSendTime		= 0;
}

CModuleCastSocket::~CModuleCastSocket(void)
{

}

BOOL CModuleCastSocket::OnCreate( UINT32 ui32IP, UINT16 ui16Port )
{
	DWORD dwOld = GetTickCount();
	BOOL bRv = i3NetworkClientSocket::OnCreate( ui32IP, ui16Port );
	DSERROR("Connection Return in %d ms\n", GetTickCount() - dwOld );
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();

	if( bRv == TRUE )
	{
		// 인도네시아 라이브(2011-12-02) 
		SOCKET s = GetSocket();
		// relay에서는 100, 100을 사용. 200,200으로 바꿨는데도 문제 남아서, 변경.
		INT32 i32RecvBufSize = 1024 * 1024 * 100;
		INT32 i32SendBufSize = 2147483647; //1024 * 1024 * 300;
		setsockopt( s, SOL_SOCKET, SO_RCVBUF, (const char *) &i32RecvBufSize, sizeof(INT32));
		setsockopt( s, SOL_SOCKET, SO_SNDBUF, (const char *) &i32SendBufSize, sizeof(INT32));
	}

	return bRv;
}

void CModuleCastSocket::OnDestroy(void)
{
	m_bConnect = FALSE; 
	i3NetworkClientSocket::OnDestroy();

	DSERROR("[CModuleCastSocket::OnDestroy] Disconnected From Cast Server!\n");
}

void CModuleCastSocket::PacketParsing(i3NetworkPacket  * pPacket)
{
	//AT Mode 
	if( g_pRoomManager->GetAtMode() )
	{
		m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
		return;
	}

	i3NetworkPacket SendPacket;
	SendPacket.Clear();
	
	BOOL		bSendMessage = FALSE; 
	INT32		i32Result = EVENT_ERROR_FAIL;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch(ProtocolID)
	{
	case PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS	://연결되었음
		{
			UINT8 ui8ServerType	= ASC_SERVER_BATTLE;			 	//Server Type

			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_ASC_CONFIRMATION_REQ );
			SendPacket.WriteData(&ui8ServerType,				sizeof(UINT8));
		}
		break;
	case PROTOCOL_ASC_CONFIRMATION_ACK				: 
		{
			m_bConnect = TRUE;

			pPacket->ReadData( g_pConfig->m_aMapLoad, sizeof(MAP_STAGE_DATA) * STAGE_UID_MAX );

			g_pConfig->m_bMapDataReceived = TRUE;

			switch ( g_eServerState )
			{
			case SERVER_STATE_START :	// Cast 서버 Reconnect된 상태
				g_pModuleCast->ServerInit( MODULE_CAST_THREAD_IDX, EVENT_ERROR_SUCCESS );
				// break; // 주석이 맞음.
			default :
				m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime(); 
				return ;

			case SERVER_STATE_LOADING :
				break;
					
			}
		}
		break; 
	case PROTOCOL_ASC_BATTLE_ROOM_COUNT_ACK: break;

	case PROTOCOL_BATTLE_SERVER_CREATE_REQ:	// 방 등록 요청
		{    
			if( SERVER_STATE_START != g_eServerState )break; 

			P_BATTLE_ROOM_CREATE_REQ ReqInfo; 
			P_BATTLE_ROOM_CREATE_ACK AckInfo;
			
			AckInfo.m_i32Result = EVENT_ERROR_SUCCESS;
			pPacket->ReadData( &ReqInfo, sizeof(P_BATTLE_ROOM_CREATE_REQ));
			memcpy_s( &AckInfo._RoomInfo, sizeof(AckInfo._RoomInfo), &ReqInfo._RoomInfo, sizeof(ReqInfo._RoomInfo)); 

			
			char * pExtraPacketData = (char*)pPacket->GetFieldBuffer() ; 
			UINT32 iExtraPacketSize = pPacket->GetDataFieldSize() - sizeof(P_BATTLE_ROOM_CREATE_REQ); 
			I3TRACE("[PROTOCOL_BATTLE_SERVER_CREATE_REQ]iExtraPacketSize = %d\n", iExtraPacketSize);
			
			DSMSG("packet size of PROTOCOL_BATTLE_SERVER_CREATE_REQ = %d\n", pPacket->GetPacketSize() );

			pExtraPacketData = &pExtraPacketData[sizeof(P_BATTLE_ROOM_CREATE_REQ)]; 
			

			AckInfo._GroupIdx = g_pRoomManager->MakeGroupId_C();
			if( AckInfo._GroupIdx == INVALID_RELAY_GROUP_ID )
			{
				AckInfo.m_i32Result = EVENT_ERROR_FAIL;
				DSERROR("[PROTOCOL_BATTLE_SERVER_CREATE_REQ] MakeGroupId Error\n");
			}

			// 그룹 등록
			if( AckInfo.m_i32Result == EVENT_ERROR_SUCCESS)
			{	
				// 방 정보를 채운다
				INT32 i32SocketIdx = 0; 
				
				AckInfo._Ip			= g_pConfig->m_ui32UdpActiveIp;
				AckInfo._Port		= GetServerPort_C( &i32SocketIdx );
				AckInfo._BattleType	= BATTLE_SERVER_TYPE_DEDICATION;
				// AckInfo._Port	= g_pIocpServer->GetRestPort_C(&i32SocketIdx); // 소켓 인덱스와 port를 방에 할당
				AckInfo._Key		= g_pRoomManager->RegisterGroup_C( &AckInfo, &ReqInfo, pExtraPacketData, (UINT16)iExtraPacketSize, i32SocketIdx );
				if ( AckInfo._Key == RELAY_ENCRYPTKEY_DEFAULT )
				{
					i32Result = EVENT_ERROR_FAIL;
					DSERROR("[PROTOCOL_BATTLE_SERVER_CREATE_REQ] RegisterGroup Error");
				}
			}

			// 결과 전달
			g_pModuleCast->Send_PROTOCOL_DSERVER_REGISTER_ROOM_ACK( MODULE_CAST_THREAD_IDX, &AckInfo );
		} 
		break; 			
	case PROTOCOL_BATTLE_SERVER_DESTROY_REQ:		// 방 해제 요청
		{    			
			P_BATTLE_ROOM_DESTROY_REQ RecvPacket; 
			pPacket->ReadData(&RecvPacket,	sizeof(P_BATTLE_ROOM_DESTROY_REQ));

			if( g_pRoomManager->RevokeGroup_C( RecvPacket._GroupIdx ) )
			{
				i32Result = EVENT_ERROR_SUCCESS;
				DSMSG( "[REVOKE_ROOM OK] GroupIdx:%d\n", RecvPacket._GroupIdx );
			}
			else
			{
				DSWARNING("[REVOKE_ROOM FAIL] GroupIdx:%d\n", RecvPacket._GroupIdx );
			}

		} 
		break; 

	case PROTOCOL_BATTLE_SERVER_RESPAWN_REQ:
		{
			 P_BATTLE_RESPAWN_REQ RecvPacket;

			 pPacket->ReadData( &RecvPacket, sizeof(P_BATTLE_RESPAWN_REQ));

			 I3TRACE("PROTOCOL_BATTLE_RESPAWN_2LEVEL_REQ: group:%d slot:%d\n", RecvPacket._GroupIdx, RecvPacket._cSlotIdx );

			// 해당 그룹의 슬롯의 주무기 교체 코드 작성해 주시면됩니다..1215
			if( g_pRoomManager->RespawnReq_C( &RecvPacket ) == FALSE )
			{
				DSWARNING("[RESPAWN ERROR] group:%d slot:%d\n",  RecvPacket._GroupIdx, RecvPacket._cSlotIdx );
			}
		}
		break;

	case PROTOCOL_BATTLE_SERVER_GM_PAUSE_REQ:
		{
			BATTLE_ROOM_INFO	RoomInfo;
			UINT32				ui32GroupIdx;
			bool				bPause;
			
			pPacket->ReadData( &RoomInfo,		sizeof( BATTLE_ROOM_INFO ) );
			pPacket->ReadData( &ui32GroupIdx,	sizeof( UINT32 ) );
			pPacket->ReadData( &bPause,			sizeof( bool ) );

			CDediRoom* pRoom = g_pRoomManager->GetGroup( ui32GroupIdx );
			if ( pRoom )
			{
				if ( bPause )
				{
					i32Result = pRoom->GMPause_C();
					g_pModuleCast->Send_PROTOCOL_BATTLE_GM_PAUSE_ACK( MODULE_CAST_THREAD_IDX, &RoomInfo, i32Result );
				}
				else
				{
					pRoom->GMResume_C();
				}
			}
		}
		break;

	case PROTOCOL_BATTLE_SERVER_ROUND_START_REQ	: 
		{
			P_BATTLE_ROUND_REQ Packet; 
			pPacket->ReadData(&Packet,	sizeof(P_BATTLE_ROUND_REQ));

			g_pRoomManager->SetRoundInfo_C( &Packet );

		}
		break; 

	case PROTOCOL_BATTLE_SERVER_QA_AUTO_COMMAND_REQ:
		{
			P_BATTLE_QA_AUTO_REQ QAInfo;
			pPacket->ReadData( &QAInfo, sizeof( P_BATTLE_QA_AUTO_REQ ) );

			g_pRoomManager->SetQACommand_C( &QAInfo );
		}
		break;

	case PROTOCOL_BATTLE_SERVER_M_DINO_INFO_REQ	: 
		{// repawn 과 따로 만들어야 하나? 통합하는 방법을 찾아 봅시다. 
			P_BATTLE_M_DINO_INFO_REQ Packet; 
			pPacket->ReadData(&Packet,	sizeof(P_BATTLE_M_DINO_INFO_REQ));

			g_pRoomManager->SetDinoInfo_C ( &Packet );
		}
		break;
	case PROTOCOL_BATTLE_SERVER_SLOT_EQUIPMENT_REQ:
		{//야점 상점에서만 받습니다. 
			STRUCT_BATTLE_SLOT_EQUIPMENT_REQ ReqInfo;
			STRUCT_BATTLE_SLOT_EQUIPMENT_ACK AckInfo;

			pPacket->ReadData( &ReqInfo, sizeof(STRUCT_BATTLE_SLOT_EQUIPMENT_REQ) );

			// 장비정보를 셋팅합니다.
			CDediRoom* pRoom = g_pRoomManager->GetGroup( ReqInfo.m_ui32GroupIdx );
			if( pRoom )
			{
				if( ! pRoom->SetUserEquip_C(&ReqInfo) )
				{
					DSERROR("SetUserEquip_C Fail(G:%d S:%d)\n", ReqInfo.m_ui32GroupIdx, ReqInfo.m_ui8SlotIdx );
				}
				else
				{
					DSMSG("SetUserEquip_C OK(G:%d S:%d)\n", ReqInfo.m_ui32GroupIdx, ReqInfo.m_ui8SlotIdx );
				}
				i3mem::Copy( &AckInfo._RoomInfo, &ReqInfo._RoomInfo, sizeof(BATTLE_ROOM_INFO) );
				AckInfo.m_ui32GroupIdx		= ReqInfo.m_ui32GroupIdx;
				AckInfo.m_ui8SlotIdx		= ReqInfo.m_ui8SlotIdx;
				AckInfo.m_ui16ChangeFlag	= ReqInfo.m_Struct.m_ui16ChangeFlag;
				AckInfo.m_ui32EquipItem		= ReqInfo.m_Struct.m_EquipItem;
				g_pModuleCast->Send_PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK( MODULE_CAST_THREAD_IDX, &AckInfo );
			}
		}
		break;

	case PROTOCOL_BATTLE_SERVER_USER_ENTER_REQ:		// 유저 입장
		{    
			P_BATTLE_USER_ENTER_REQ ReqInfo; 
			pPacket->ReadData(&ReqInfo, sizeof(P_BATTLE_USER_ENTER_REQ));
			
			DSMSG("[USER_ENTER_REQ] RoomIdx:%d, GroupIdx:%d, Slot:%d, Relay:%d, Game:%d, Channel:%d\n", 
						ReqInfo._RoomInfo._sRoom,
						ReqInfo._wGroupIdx,
						ReqInfo._cSlotIdx, 
						ReqInfo._RoomInfo._cBattle,
						ReqInfo._RoomInfo._cGame,
						ReqInfo._RoomInfo._cChannel 
					    );
			
			if ( g_pRoomManager->EnterGroup_C( &ReqInfo ) == EVENT_ERROR_FAIL )
			{
				DSWARNING("[USER_ENTER_FAIL] RoomIdx:%d, GroupIdx:%d, Slot:%d, Relay:%d, Game:%d, Channel:%d\n",
								ReqInfo._RoomInfo._sRoom,
								ReqInfo._wGroupIdx,
								ReqInfo._cSlotIdx, 
								ReqInfo._RoomInfo._cBattle,
								ReqInfo._RoomInfo._cGame,
								ReqInfo._RoomInfo._cChannel
								);
			}
			
			bSendMessage = FALSE;
		} 
		break; 				
	case PROTOCOL_BATTLE_SERVER_USER_LEAVE_REQ:		// 유저 퇴장
		{    
			P_BATTLE_USER_LEAVE_REQ ReqInfo;
			pPacket->ReadData( &ReqInfo, sizeof( P_BATTLE_USER_LEAVE_REQ) );

			DSMSG("[USER_LEAVE] GROUP:%d, SLOT:%d\n", ReqInfo._wGroupIdx, ReqInfo._cSlotIdx );
			
			i32Result = g_pRoomManager->LeaveGroup_C(&ReqInfo);
			if( i32Result != EVENT_ERROR_SUCCESS)
			{
				DSWARNING("[USER_LEAVE FAIL] GROUP:%d, SLOT:%d\n", ReqInfo._wGroupIdx, ReqInfo._cSlotIdx );
			}
		} 
		break; 
	default: 
		DSERROR("[CModuleCastSocket::PacketParsing] Unknown Message(%d)\n", ProtocolID ); break; 
	}
	
	if( bSendMessage )
	{
		if( 0 >= SendPacketMessage( &SendPacket ))
		{
			DSERROR("SendPacketMessage:%d\n", SendPacket.GetProtocolID() );
		}
		
		m_ui32LastSendTime	= i3ThreadTimer::GetServerTime();
	}
	
	m_ui32LastRecvTime	= i3ThreadTimer::GetServerTime();
	
	return;
}


UINT16	CModuleCastSocket::GetServerPort_C( INT32* pi32OutPortIdx )
{
	I3ASSERT ( pi32OutPortIdx != NULL );
	I3ASSERT ( g_pConfig->m_ui32UdpPortCount != 0 );
	
	INT32	i32PortIdx = m_i32PortIdx++;
	m_i32PortIdx %= g_pConfig->m_ui32UdpPortCount; 
	*pi32OutPortIdx = i32PortIdx;
	
	return g_pConfig->m_pui16UdpPort[ i32PortIdx ];
}