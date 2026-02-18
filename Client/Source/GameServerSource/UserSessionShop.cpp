#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleContextNc.h"
#include "ModuleTrans.h"

BOOL	CUserSession::_PacketParsingShop( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
		//이 패킷 필요없다. 삭제 해야 한다. 백성민대리에게 물어보고.		1224
		//로비에 들어왔을때 주는 패킷으로 변경하면 된다. 
	case PROTOCOL_SHOP_GET_SAILLIST_REQ:
		{
			CHECKVALID_CHANNEL; 
			pPacket->ReadData(&i32Temp, sizeof(INT32));

			// 상점목록을 보낸다.
			_SendShopInfo(i32Temp); 

			// 서버 시간을 보낸다.
			bSendMessage= TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_SHOP_GET_SAILLIST_ACK);
			UTemp		= g_pContextMain->GetInvenTime();
			pSendPacket->WriteData(&UTemp, sizeof(INT32));			//Set Time
		}
		break;
	case PROTOCOL_FIELDSHOP_GOODSLIST_REQ:
		{
			CHECKVALID_CHANNEL; 

			_SendFieldShopInfo( 0 );
		}
		break;
	case PROTOCOL_FIELDSHOP_OPEN_REQ:
		{
			char strTemp[ LOG_FILE_STRING_COUNT ];
			CHECKVALID_CHANNEL; 

			// Read the version info from client packet
			UINT32	ui32ShopVersion = 0;
			UINT32	ui32FieldShopVersion = 0;
			pPacket->ReadData(&ui32ShopVersion,			sizeof(UINT32));
			pPacket->ReadData(&ui32FieldShopVersion,	sizeof(UINT32));

			// Get Shop Information
			CShop* pShop = g_pContextMain->GetShop();
			if( NULL == pShop )	break;

			// Compare Shop version
			if(ui32ShopVersion != pShop->GetVersion())
			{
				i3String::Format(strTemp, LOG_FILE_STRING_COUNT, "[SendShopInfo : %d != %d]", ui32ShopVersion, pShop->GetVersion() );	
				g_pModuleLogFile->Write_M_Log(1, this, strTemp);	
				_SendShopInfo(ui32ShopVersion);
			}

			i3NetworkPacket FieldShopOpenAckPacket( PROTOCOL_FIELDSHOP_OPEN_ACK );
			UINT8 ui8Result; // 0: Fail, 1:Ok

			if(ui32FieldShopVersion != g_pContextMain->GetFieldShopVersion())
			{
				// Different version, Send Fail Message to client
				ui8Result = 0; // Fail
				FieldShopOpenAckPacket.WriteData( &ui8Result,  sizeof (UINT8) );
				SendPacketMessage( &FieldShopOpenAckPacket );

				_SendFieldShopInfo(ui32FieldShopVersion);
			}
			else
			{
				// Same version, Send Ok Message to client
				ui8Result = 1; // OK
				FieldShopOpenAckPacket.WriteData( &ui8Result,  sizeof (UINT8) );
				SendPacketMessage( &FieldShopOpenAckPacket );
			}
		}
		break;
	case PROTOCOL_SHOP_ENTER_REQ			: 
		{
			CHECKVALID_CHANNEL; 

			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveShop(this, TRUE); 
			}

			//2. Inventory Check
			m_i32SubTask	= GAME_TASK_SUB_SHOP; 			

			//3. ReadData
			pPacket->ReadData(&UTemp, sizeof(UINT32));

            // 상점목록을 보낸다.
			_SendShopInfo(UTemp); 

			// 서버시간을 보낸다.
			bSendMessage= TRUE; 
			pSendPacket->SetProtocol( PROTOCOL_SHOP_ENTER_ACK );
			UTemp		= g_pContextMain->GetInvenTime();			
			pSendPacket->WriteData(&UTemp, sizeof(UINT32));			//Set Time
		}
		break;		
	case PROTOCOL_SHOP_LEAVE_REQ			:			//080303
		{
			CHECKVALID_CHANNEL;

			UINT32 ui32Check;
			UINT32 ui32EquipChange;			// 변경된 플래그
			UINT32 ui32EquipFlag = 0;		// 클라이언트에 보내질 플래그

			i32Temp		= EVENT_ERROR_SUCCESS; 
			pPacket->ReadData( &ui32EquipChange, sizeof(UINT32) );

			if( INVENTORY_FLAG_CHARACTER & ui32EquipChange )
			{	//캐릭터 변경
				USER_INFO_EQUIPMENT_CHARA RecvData; 
				pPacket->ReadData( &RecvData, sizeof(USER_INFO_EQUIPMENT_CHARA) );
				//2. Set Equipment 내부에서 검사를 합니다. 
				if( _CheckEquipmentChara(&RecvData, &ui32Check) )
				{
					i3mem::Copy( &m_UserInfoEquipmentChara, &RecvData, sizeof( USER_INFO_EQUIPMENT_CHARA ) );
					ChangeDataBaseInfo( SAVE_DATABASE_EQUIPMENT );
				}
				else
				{
					i32Temp = EVENT_ERROR_FAIL;
					ui32EquipFlag	|= INVENTORY_FLAG_CHARACTER;
				}
			}

			if( INVENTORY_FLAG_WEAPON & ui32EquipChange )
			{	//웨폰 변경
				USER_INFO_EQUIPMENT_WEAPON RecvData;
				pPacket->ReadData( &RecvData, sizeof(USER_INFO_EQUIPMENT_WEAPON) );
				//2. Set Equipment 내부에서 검사를 합니다.
				if( _CheckEquipmentWeapon(&RecvData, &ui32Check) )
				{
					i3mem::Copy( &m_UserInfoEquipmentWeapon, &RecvData, sizeof( USER_INFO_EQUIPMENT_WEAPON ) );
					ChangeDataBaseInfo( SAVE_DATABASE_EQUIPMENT );
				}
				else
				{
					i32Temp = EVENT_ERROR_FAIL;
					ui32EquipFlag	|= INVENTORY_FLAG_WEAPON;
				}
			}

			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveShop(this, FALSE); 
			}

			m_i32SubTask	= GAME_TASK_SUB_NONE;

			bSendMessage= TRUE;
			pSendPacket->SetProtocol(PROTOCOL_SHOP_LEAVE_ACK); 
			pSendPacket->WriteData(&i32Temp,			sizeof(UINT32));
			pSendPacket->WriteData(&ui32EquipFlag,	sizeof(UINT32));
		}
		break; 


	case PROTOCOL_RS_ENTER_REQ			: 
		{
			if( 1 != g_pModuleTrans->m_ui8RSEnable )	break;

			CHECKVALID_CHANNEL;

			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveRS(this, TRUE); 
			}

			m_i32SubTask	= GAME_TASK_SUB_RS; 

			bSendMessage= TRUE;

			// Trans 서버로 보낸다.
			if( FALSE == g_pModuleTrans->AuthSendRSEnterReq( m_WorkThreadIdx, this ) )
			{
				i32Temp = EVENT_ERROR_RS_FAIL_NETWORK;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_RS_ENTER_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
			}
		}
		break; 
	case PROTOCOL_RS_LEAVE_REQ			: 
		{
			if( 1 != g_pModuleTrans->m_ui8RSEnable )	break;

			CHECKVALID_CHANNEL;
			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveRS(this, FALSE); 
			}
			m_i32SubTask	= GAME_TASK_SUB_NONE;

			INT32	i32Result = EVENT_ERROR_SUCCESS;
		


			bSendMessage= TRUE;
			pSendPacket->SetProtocol(PROTOCOL_RS_LEAVE_ACK); 
			pSendPacket->WriteData(&i32Result, sizeof(INT32) );
			 
		}
		break; 


	case PROTOCOL_RS_ITEM_INFO_REQ	:
		{
			if( 1 != g_pModuleTrans->m_ui8RSEnable )	break;

			CHECKVALID_CHANNEL;

			// 클라이언트 팀 요청으로 막습니다. 2012.04.24 노준영.
			/*if( GAME_TASK_SUB_RS != GetSubTask() ) 
			{
				break;
			}*/

			// Trans 서버로 보낸다.
			//if( FALSE == g_pModuleTrans->AuthSendRSItemInfoReq( m_WorkThreadIdx, this ) )
			//{
			// Client -> Game -> Trans -> Game -> Client
			// 위 방식에서 효율성을 위해 방식을 바꿈
			// Trans -> Game 주기적으로 정보를 갱신해 줌
			// Client -> Game -> Client 개별 클라이언트의 요청을 Game 서버가 담당
			INT32 i32Rv = EVENT_ERROR_SUCCESS;
			bSendMessage= TRUE;
			pSendPacket->SetProtocol( PROTOCOL_RS_ITEM_INFO_ACK );
			pSendPacket->WriteData( &i32Rv,				sizeof(INT32));	
			pSendPacket->WriteData(&g_pModuleTrans->m_stRSItemInfo, sizeof(RS_ALL_ITEM_INFO));
			//}
		}
		break;

	case PROTOCOL_RS_ROULETTE_START_REQ	: 
		{
			if( 1 != g_pModuleTrans->m_ui8RSEnable )	break;

			CHECKVALID_CHANNEL;

			if( GAME_TASK_SUB_RS != GetSubTask() ) 
			{
				break;
			}

			RS_ROULETTE_INFO	pRsRInfo;		// 클라이언트에서 받는 용.

			i3mem::FillZero( &pRsRInfo, sizeof(RS_ROULETTE_INFO)   );		

			BOOL bIsHidden;
			pPacket->ReadData(&bIsHidden, sizeof(BOOL));
			pPacket->ReadData(&pRsRInfo, sizeof(RS_ROULETTE_INFO));


			// Trans 서버로 보낸다.
			if( FALSE == g_pModuleTrans->AuthSendRSStartReq( m_WorkThreadIdx, this, &pRsRInfo , bIsHidden ) )
			{
				// 일단 막는다.
				i32Temp = EVENT_ERROR_RS_FAIL_NETWORK;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_RS_ROULETTE_RESULT_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
			}
		}
		break; 

	case PROTOCOL_RS_JACKPOT_NOTIFY_REQ	:
		{
			if( 1 != g_pModuleTrans->m_ui8RSEnable )	break;

			CHECKVALID_CHANNEL;

			if( GAME_TASK_SUB_RS != GetSubTask() ) 
			{
				break;
			}

			// Trans 서버로 보낸다.
			if( FALSE == g_pModuleTrans->AuthSendRSJackPotNotifyReq( m_WorkThreadIdx, this, m_UserInfoBasic._nick ) )
			{
				RS_JACKPOT_INFO stRS;
				stRS.bIsJackPot = FALSE;
			 
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_RS_JACKPOT_NOTIFY_ACK );
				pSendPacket->WriteData(&stRS, sizeof(RS_JACKPOT_INFO));
			}
		}
		break;


	default:
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled 	= TRUE; 
			m_ui32KillReason 	= KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}