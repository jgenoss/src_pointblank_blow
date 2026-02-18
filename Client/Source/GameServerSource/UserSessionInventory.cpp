#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingInventory( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{	
	case PROTOCOL_INVENTORY_ENTER_REQ		: //인벤토리 들어가기			확인 0331
		{			
			CHECKVALID_CHANNEL; 
			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveInventory(this, TRUE); 
			}

			//. Inventory Check
			m_i32SubTask	= GAME_TASK_SUB_INVENTORY; 
			CheckInventory_MAU(); 

			bSendMessage= TRUE; 
			UTemp		= g_pContextMain->GetInvenTime(); 
			pSendPacket->SetProtocol(PROTOCOL_INVENTORY_ENTER_ACK); 
			pSendPacket->WriteData(&UTemp, sizeof(UINT32));
		}
		break;
	case PROTOCOL_INVENTORY_LEAVE_REQ		: //인벤토리에서 나가기			확인 0331			//각각의 함수에서 변경된 값이 정상인지 확인해야 한다.
		{
			CHECKVALID_CHANNEL;

			i32Temp = EVENT_ERROR_SUCCESS; 
			UINT32 ui32Check;
			UINT32 ui32EquipChange;			// 변경된 플래그
			UINT32 ui32EquipFlag = 0;		// 클라이언트에 보내질 플래그

			pPacket->ReadData( &ui32EquipChange, sizeof(UINT32) );			// 변경된 플래그

			if( INVENTORY_FLAG_CHARACTER & ui32EquipChange )
			{	//캐릭터 변경
				USER_INFO_EQUIPMENT_CHARA RecvData;
				pPacket->ReadData( &RecvData, sizeof(USER_INFO_EQUIPMENT_CHARA) );

				//2. Set Equipment 내부에서 검사를 합니다. 
				if( _CheckEquipmentChara( &RecvData, &ui32Check ) )
				{	// 모두 올바를 경우 정보를 바꿔준다.
					i3mem::Copy( &m_UserInfoEquipmentChara, &RecvData, sizeof( USER_INFO_EQUIPMENT_CHARA ) );
					ChangeDataBaseInfo( SAVE_DATABASE_EQUIPMENT );
				}
				else
				{	// 해당 무기를 가지고 있지 않다.
					i32Temp			= EVENT_ERROR_FAIL;
					ui32EquipFlag	|= INVENTORY_FLAG_CHARACTER;
				}
			}

			if( INVENTORY_FLAG_WEAPON & ui32EquipChange )
			{	//웨폰 변경
				USER_INFO_EQUIPMENT_WEAPON RecvData; 				
				pPacket->ReadData( &RecvData, sizeof(USER_INFO_EQUIPMENT_WEAPON) );

				//2. Set Equipment 내부에서 검사를 합니다. 
				if( _CheckEquipmentWeapon( &RecvData, &ui32Check ) )
				{
					i3mem::Copy( &m_UserInfoEquipmentWeapon, &RecvData, sizeof( USER_INFO_EQUIPMENT_WEAPON ) );
					ChangeDataBaseInfo( SAVE_DATABASE_EQUIPMENT );
				}
				else
				{
					i32Temp			= EVENT_ERROR_FAIL;
					ui32EquipFlag	|= INVENTORY_FLAG_WEAPON;
				}
			}

			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{				
				m_pRoom->MoveInventory( this, FALSE );
			}
			
			m_i32SubTask	= GAME_TASK_SUB_NONE; 			

			bSendMessage= TRUE;
			pSendPacket->SetProtocol(PROTOCOL_INVENTORY_LEAVE_ACK);
			pSendPacket->WriteData(&i32Temp,			sizeof(INT32));
			pSendPacket->WriteData(&ui32EquipFlag,	sizeof(UINT32));
		}
		break; 
	default:
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User");
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}