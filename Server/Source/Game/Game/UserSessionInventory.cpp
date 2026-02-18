#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
// #include "ModuleLogFile.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingInventory( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	UINT32		ui32UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_INVENTORY_ENTER_REQ		: //인벤토리 들어가기			확인 0331
		{
			CHECKVALID_LOBBY;

			INT32 i32Result;
#ifdef USE_AAS_SYSTEM_FULL
			if ( g_pContextMain->CheckAASBlockON( AAS_BLOCK_ENTER_INVEN ) && CheckAAS() )
			{
				i32Result		= EVENT_ERROR_AAS_ENTER_INVEN;
			}
			else
#endif
			{
				i32Result		= EVENT_ERROR_SUCCESS;
				m_i32SubTask	= GAME_TASK_SUB_INVENTORY;

				//1. Room에 있는지 체크 
				if( m_i32RoomIdx > SERVER_VALUE_UNKNOWN )
				{
					m_pRoom->MoveInventory(this, TRUE);
				}

				//2. 인벤토리 체크
				CheckInventory_MAU(m_WorkThreadIdx);
			}

			// 계급 변화가 있는지 확인 - 장성계급 아이템 지급
			CheckRankUp( m_WorkThreadIdx );

			bSendMessage	= TRUE;
			ui32UTemp		= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			pSendPacket->SetProtocol(PROTOCOL_INVENTORY_ENTER_ACK); 
#ifdef USE_AAS_SYSTEM_FULL
			pSendPacket->WriteData(&i32Result, sizeof(INT32));
#endif
			pSendPacket->WriteData(&ui32UTemp, sizeof(UINT32));
		}
		break;
	case PROTOCOL_INVENTORY_LEAVE_REQ		: //인벤토리에서 나가기			확인 0331			//각각의 함수에서 변경된 값이 정상인지 확인해야 한다.
		{
			CHECKVALID_LOBBY;

			// Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveInventory( this, FALSE );
			}
			
			m_i32SubTask	= GAME_TASK_SUB_NONE;

			PACKET_INVENTORY_LEAVE_ACK Packet;
			Packet.m_TResult	= EVENT_ERROR_SUCCESS;
			S2MOSendEvent( &Packet );
		}
		break; 
	default:
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 			
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}