#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleTrans.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingShop( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	UINT32		ui32UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_SHOP_GET_SAILLIST_REQ:
		{
			CHECKVALID_CHANNEL; 

			char strMD5[NET_MD5_KEY_SIZE+1];
			UINT8 ui8SendShopInfo = 0;	// 상점 데이터 전송 여부. 0 : 전송X , 1 : 전송O
		
			pPacket->ReadData(strMD5,	sizeof(char) * NET_MD5_KEY_SIZE ); 
			strMD5[NET_MD5_KEY_SIZE]	= 0;
 
			// 상점목록을 보낸다.
			if( TRUE == _SendShopInfo(strMD5) )  
			{
				ui8SendShopInfo = 1;
			}

			ui32UTemp = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			// 서버 시간을 보낸다.
			bSendMessage= TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_SHOP_GET_SAILLIST_ACK);
			pSendPacket->WriteData(&ui8SendShopInfo,	sizeof(UINT8));			 
			pSendPacket->WriteData(&ui32UTemp,			sizeof(INT32));			//Set Time
			
		}
		break;
		
	case PROTOCOL_SHOP_ENTER_REQ			:
		{
			CHECKVALID_LOBBY;

			INT32	i32Result;
			UINT8	ui8SendShopInfo = 0;		// 상점 데이터 전송 여부. 0 : 전송X , 1 : 전송O
			char	strMD5[NET_MD5_KEY_SIZE+1];

			pPacket->ReadData(strMD5, sizeof(char) * NET_MD5_KEY_SIZE );
			strMD5[NET_MD5_KEY_SIZE] = 0;
#ifdef USE_AAS_SYSTEM_FULL
			if ( g_pContextMain->CheckAASBlockON( AAS_BLOCK_ENTER_SHOP ) && CheckAAS() )
			{
				i32Result		= EVENT_ERROR_AAS_ENTER_SHOP;
			}
			else
#endif
			{
				i32Result		= EVENT_ERROR_SUCCESS;
				m_i32SubTask	= GAME_TASK_SUB_SHOP;
				CheckInventory_MAU(m_WorkThreadIdx);

				//1. Room에 있는지 체크 
				if( m_i32RoomIdx > SERVER_VALUE_UNKNOWN )
				{
					m_pRoom->MoveShop(this, TRUE);
				}

				//2. 상점목록을 보낸다.
				if( TRUE == _SendShopInfo(strMD5) )
				{
					ui8SendShopInfo = 1;
				}
			}

			// 계급 변화가 있는지 확인
			CheckRankUp( m_WorkThreadIdx );

			bSendMessage	= TRUE;
			ui32UTemp		= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			pSendPacket->SetProtocol( PROTOCOL_SHOP_ENTER_ACK );
#ifdef USE_AAS_SYSTEM_FULL
			pSendPacket->WriteData(&i32Result,			sizeof(INT32));
#endif
			pSendPacket->WriteData(&ui8SendShopInfo,	sizeof(UINT8));
			pSendPacket->WriteData(&ui32UTemp,			sizeof(UINT32));			//Set Time
		}
		break;
	case PROTOCOL_SHOP_LEAVE_REQ			:			//080303
		{
			CHECKVALID_LOBBY;

			// Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{				
				m_pRoom->MoveShop( this, FALSE );
			}
			
			m_i32SubTask	= GAME_TASK_SUB_NONE; 			

			PACKET_SHOP_LEAVE_ACK Packet;
			Packet.m_TResult	= EVENT_ERROR_SUCCESS;
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ			:						//1224
		{
			CHECKVALID_CHANNEL;			
			CHECKVALID_SHOP;

			UINT8	ui8GoodsCount;
			DB_GOODS aDBGoods[MAX_SHOP_GOODS_BUY_COUNT];
				
			pPacket->ReadData( &ui8GoodsCount,		sizeof(UINT8) );	// 장바구니 때문에 GoodsCount 를 먼저 받아 옵니다. 1이면 일반 구입 1 이상이면 장바구니 구입.

			// 값이 이상함.
			if( ui8GoodsCount < 1 || ui8GoodsCount > USE_BASKET_COUNT )	break;

			BUY_BASKET stBasket[ USE_BASKET_COUNT ];
			pPacket->ReadData( stBasket, sizeof( BUY_BASKET ) * ui8GoodsCount );

			i32Temp = _BuyGoods( ui8GoodsCount, stBasket );
			if( EV_FAILED( i32Temp ))
			{
				for( UINT8 i = 0 ; i < ui8GoodsCount; i++ ) 
				{
					aDBGoods[i].m_i32Rv		= i32Temp;
					aDBGoods[i].m_TGoodsID	= stBasket[i].m_ui32GoodsID;
				}
				g_pUserManager->OnGoodsBuy_A( m_SessionIdx, m_i64UID, 0, m_UserInfoBasic.m_ui32Cash, ui8GoodsCount, aDBGoods, TYPE_SHOP_GOODS_BUY );
			}
		}
		break;	
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ :
		{
			CHECKVALID_CHANNEL;
			T_ItemDBIdx TWareDBIdx;
			UINT8		ui8CharaSlotIdx = 0xFF;

			pPacket->ReadData(&TWareDBIdx,	sizeof(T_ItemDBIdx));

			i32Temp = EVENT_ERROR_SUCCESS;
			INVEN_BUFFER* pBuffer = GetItemByWareDBIdx( TWareDBIdx );
			if( NULL == pBuffer )									i32Temp  = EVENT_ERROR_FAIL;
			else
			{
				if( ITEM_ATTR_TYPE_BUY != pBuffer->_ui8ItemType )	i32Temp  = EVENT_ERROR_FAIL;
				if( 0 == pBuffer->_ui32ItemArg )					i32Temp  = EVENT_ERROR_FAIL;
				if( 0 == pBuffer->_ui32ItemID )						i32Temp  = EVENT_ERROR_FAIL;

				// 캐릭터 인증일 경우. 캐릭터슬롯인덱스도 같이 보내줍니다.
				if( ITEM_TYPE_CHARA == GET_ITEM_TYPE(pBuffer->_ui32ItemID) )
				{
					ITEM_INFO	ItemInfo;

					for( INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT ; i++ )
					{
						if( FALSE == m_MultiSlot.IsActiveChar( i ) )				continue;

						m_MultiSlot.GetCharEquipment( i, CHAR_EQUIPMENT_CHARA, &ItemInfo );

						if( TWareDBIdx != ItemInfo.m_TItemDBIdx )	continue;
						
						ui8CharaSlotIdx = (UINT8)i;
						break;
					}

					if( 0xFF == ui8CharaSlotIdx )
					{	// 캐릭터 검색 실패
						i32Temp = EVENT_ERROR_EVENT_NOT_EXIST_CHARA;
					}
				 
				}
			}

			if( EV_SUCCESSED( i32Temp ) ) 
			{
				i32Temp = _SetItemAbilityAuth( pBuffer );				
				if (EV_SUCCESSED(i32Temp))
				{
					INT32 ui32ItemType = GET_ITEM_TYPE(pBuffer->_ui32ItemID);
					UINT16 ui16InvenCount = this->m_Inventory.GetInventoryCount();
					switch (ui32ItemType)
					{
					case ITEM_TYPE_SET:
					case ITEM_TYPE_WRAP_COUNT:
						if (MAX_INVEN_COUNT - MAX_SHOP_GOODS_ITEM_COUNT < ui16InvenCount) i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL;
						break;
					default:
						if (MAX_INVEN_COUNT < ui16InvenCount) i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL;
						break;
					}
				}
				if( EV_SUCCESSED( i32Temp ) ) 
				{
					if( FALSE == g_pModuleTrans->AuthSendAuthItem(m_WorkThreadIdx, this, TWareDBIdx, 1, ui8CharaSlotIdx, pBuffer->_ui32ItemID) )	i32Temp = EVENT_ERROR_FAIL;
				}
			}
			
			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ:
		{
			CHECKVALID_CHANNEL;

			T_ItemDBIdx TWareDBIdx;
			UINT8 ui8Ability = 0;
			TTCHAR  pAbility[ 255 ];
			pPacket->ReadData(&TWareDBIdx,		sizeof(T_ItemDBIdx));
			pPacket->ReadData(&ui8Ability,		sizeof(UINT8));
			pPacket->ReadData(pAbility,			ui8Ability);			

			i32Temp = EVENT_ERROR_SUCCESS;
			INVEN_BUFFER* pBuffer = GetItemByWareDBIdx( TWareDBIdx );
			if( NULL == pBuffer )									i32Temp  = EVENT_ERROR_FAIL;
			else
			{
				if( ITEM_ATTR_TYPE_BUY != pBuffer->_ui8ItemType )	i32Temp  = EVENT_ERROR_FAIL;
				if( 0 == pBuffer->_ui32ItemArg )					i32Temp  = EVENT_ERROR_FAIL;
				if( 0 == pBuffer->_ui32ItemID )						i32Temp  = EVENT_ERROR_FAIL;
			}

			if( EV_SUCCESSED( i32Temp ) ) 
			{
				i32Temp = _SetItemAbilityAuth( pBuffer, (void*)pAbility ); 
			}

			if( EV_SUCCESSED( i32Temp ) ) 
			{                 
				 if( FALSE == g_pModuleTrans->AuthSendAuthItemData(m_WorkThreadIdx, this, TWareDBIdx, pBuffer->_ui32ItemID, ui8Ability, pAbility ) )	i32Temp = EVENT_ERROR_FAIL;
			}

			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_REQ:
		{
			CHECKVALID_CHANNEL;

			T_ItemDBIdx TWareDBIdx;
			UINT8 ui8Ability = 0;
			char  pAbility[ 255 ];
			pPacket->ReadData(&TWareDBIdx,		sizeof(T_ItemDBIdx));
			pPacket->ReadData(&ui8Ability,		sizeof(UINT8));
			pPacket->ReadData(pAbility,			ui8Ability);			

			i32Temp = EVENT_ERROR_SUCCESS;
			INVEN_BUFFER* pBuffer = GetItemByWareDBIdx( TWareDBIdx );
			if( NULL == pBuffer )									i32Temp  = EVENT_ERROR_FAIL;
			else
			{				
				if( 0 == pBuffer->_ui32ItemArg )					i32Temp  = EVENT_ERROR_FAIL;
			}

			if( EV_SUCCESSED( i32Temp ) ) 
			{
				i32Temp = _SetItemAbilityChange( pBuffer, (void*)pAbility ); 
			}
			
			bSendMessage	= TRUE;
			pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK); 
			pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			
		}
		break;
	case PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ:
		{
			T_ItemDBIdx TWareDBIdx;
			CHECKVALID_CHANNEL;
			pPacket->ReadData(&TWareDBIdx, sizeof(T_ItemDBIdx));

			i32Temp = EVENT_ERROR_SUCCESS;

			if( FALSE == g_pModuleTrans->AuthSendDeleteItem( m_WorkThreadIdx, this, TWareDBIdx ) )			i32Temp = EVENT_ERROR_FAIL;
			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ:	// 선물함 확인
		{
			CHECKVALID_DBINDEX;

			PACKET_AUTH_SHOP_GET_GIFTLIST_ACK	Packet;
			
			if( !g_pContextMain->IsContentsEnabled( CONTENTS_GIFT ) ) 
			{
				g_pUserManager->OnGetGift_A( this, EVENT_ERROR_DISABLE_CONTENTS, &Packet ); 
				break;
			}

			if( FALSE == g_pModuleTrans->AuthSendUserGift(m_WorkThreadIdx, this) ) 
			{
				g_pUserManager->OnGetGift_A( this, EVENT_ERROR_NETWORK, &Packet );
			}
		}
		break;
	
	case PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ:	// 선물 인증
		{	
			CHECKVALID_CHANNEL;

			PACKET_AUTH_SHOP_AUTH_GIFT_REQ	PacketRecv;
			S2MORecvPacket( &PacketRecv, pPacket->GetPacketBuffer() );
			
			if( !g_pContextMain->IsContentsEnabled( CONTENTS_GIFT ) ) 
			{
				g_pUserManager->OnGiftAuthFail_A( this, &PacketRecv, EVENT_ERROR_DISABLE_CONTENTS );
				break;
			}
			
			// 타입 체크
			switch( PacketRecv.m_ui8Type ) 
			{
			case TYPE_SHOP_GIFT_READ:
			case TYPE_SHOP_GIFT_AUTH:
			case TYPE_SHOP_GIFT_DELETE:
				break;
			default :
				g_pUserManager->OnGiftAuthFail_A( this, &PacketRecv, EVENT_ERROR_USER_HACK_PACKET );
				return FALSE;  
			}

			if( FALSE == g_pModuleTrans->AuthSendAuthGift(m_WorkThreadIdx, this, &PacketRecv ) )
			{
				g_pUserManager->OnGiftAuthFail_A( this, &PacketRecv, EVENT_ERROR_NETWORK );
				break;
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ:	// 선물 구매
		{
			CHECKVALID_CHANNEL;
			CHECKVALID_SHOP;

			PACKET_AUTH_SHOP_GOODS_GIFT_REQ PacketRecv; 
			S2MORecvPacket( &PacketRecv, pPacket->GetPacketBuffer() );

			if( GAME_TASK_SUB_GIFTSHOP != GetSubTask() ) 
			{
				g_pUserManager->OnGoodsGiftFail_A( this, &PacketRecv, EVENT_ERROR_GAME_WRONG_POSITION );
				break;
			}
				
			if( !g_pContextMain->IsContentsEnabled( CONTENTS_GIFTSHOP ) ) //여기선 선물샵 on/off 체크
			{
				g_pUserManager->OnGoodsGiftFail_A( this, &PacketRecv, EVENT_ERROR_DISABLE_CONTENTS );
				break;
			}

			if( 0 == PacketRecv.m_strMessage.GetCount() )
			{	// 기획의도입니다.
				PacketRecv.m_strMessage = _TT(" \n");
			}

			i32Temp = _BuyGoods( (UINT8)PacketRecv.m_aBuyGift.GetCount(), &PacketRecv.m_aBuyGift, SPEND_TYPE_BUY_GIFT, (wchar_t*)&PacketRecv.m_strReciverNick, (wchar_t*)&PacketRecv.m_strMessage );
			if( EV_FAILED( i32Temp ))  
			{
				g_pUserManager->OnGoodsGiftFail_A( this, &PacketRecv, i32Temp ); 
			}
		}
		break;	
	case PROTOCOL_AUTH_GET_POINT_CASH_REQ:		//081224
		{
			
			CHECKVALID_DBINDEX; 
			g_pModuleTrans->AuthSendGetRemainMoney(m_WorkThreadIdx, this);
		}
		break; 
		
	case PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_REQ:
		{	// 닉네임 검사.
			CHECKVALID_CHANNEL;

			T_RESULT TResult;
			TTCHAR strNickname[NET_NICK_NAME_SIZE];

			pPacket->ReadData( strNickname, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
			strNickname[ NET_NICK_NAME_SIZE-1 ] = 0;

			TResult = _CheckUseNickName( strNickname );

			if( EV_SUCCESSED( TResult ) )
			{
				if( FALSE == g_pModuleTrans->AuthSendCheckNickName( m_WorkThreadIdx, this, strNickname ) )
				{
					TResult = EVENT_ERROR_FAIL;
				}
			}

			if( EV_FAILED( TResult ) )
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK);
				pSendPacket->WriteData(&TResult,	sizeof(T_RESULT));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_EXTEND_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECKVALID_SHOP;

			PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ	ShopItemExtendReq;
			PACKET_H_AUTH_NEW_SHOP_ITEM_EXTEND_ACK	ShopItemExtendAckH;
			i3mem::FillZero(&ShopItemExtendAckH, sizeof(PACKET_H_AUTH_NEW_SHOP_ITEM_EXTEND_ACK));

			pPacket->ReadData( &ShopItemExtendReq,	sizeof(PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ) );
			INT32 i32Rv = _ExtendItem( &ShopItemExtendReq.m_ExtendItemData );

			if( EV_FAILED( i32Rv ))
			{
				DB_GOODS aDBGoods[MAX_SHOP_GOODS_BUY_COUNT];
				aDBGoods[0].m_i32Rv		= i32Rv;
				aDBGoods[0].m_TGoodsID	= ShopItemExtendReq.m_ExtendItemData.m_Basket.m_ui32GoodsID;
				g_pUserManager->OnGoodsBuy_A( m_SessionIdx, m_i64UID, 0, m_UserInfoBasic.m_ui32Cash, 1, aDBGoods, TYPE_SHOP_ITEM_EXTEND );
			}
		}
		break;
	case PROTOCOL_SHOP_REPAIR_REQ:
		{
			CHECKVALID_CHANNEL;
			UINT8 ui8CharaSlotIdx;
			UINT8 ui8RepairType	= S2MO_REPAIR_TYPE_ONE;

			T_ItemDBIdx				TItemDBIdx;
			T_ItemID				TItemID = 0;
			UINT32					ui32Point		= 0;
			UINT32					ui32Cash		= 0;
			UINT32					ui32AuthArg		= 0;
			UINT8					ui8CheckCount	= 0;
			UINT32 					ui32TotalPoint = 0;		
			UINT32 					ui32TotalCash = 0;		
			S2MO_REPAIR_INFO		stInfo;
			i3mem::FillZero( &stInfo, sizeof( S2MO_REPAIR_INFO ));
			S2MO_CHAR_EQUIP_INFO	CharEquip;
			ITEM_INFO				ItemInfo;
			S2MO_SHOP_REPAIR_RESULT	stFailInfo;// 실패용 구조체.
			
			pPacket->ReadData(&ui8RepairType,	sizeof(UINT8)); 
			pPacket->ReadData(&ui8CharaSlotIdx,	sizeof(UINT8)); 
			pPacket->ReadData(&TItemDBIdx,		sizeof(T_ItemDBIdx));
			
			INT32		i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;

			switch ( ui8RepairType )
			{
			case S2MO_REPAIR_TYPE_ONE :	// 단일 수리
				{
					i32Rv = _RepairItem( TItemDBIdx, &ui32Point, &ui32Cash, &ui32AuthArg, &TItemID );

					if( EV_SUCCESSED( i32Rv ) ) 
					{
						stInfo.m_ui8ItemCount = 1;
						if( ui32Point > 0 ) 
						{
							stInfo.m_stRepairItem[0].m_ui8BuyType = GOODS_BUY_POINT;
							stInfo.m_stRepairItem[0].m_ui32RepairPoint = ui32Point;
							ui32TotalPoint += ui32Point;
						}
						else if( ui32Cash > 0 ) 
						{
							stInfo.m_stRepairItem[0].m_ui8BuyType = GOODS_BUY_CASH;
							stInfo.m_stRepairItem[0].m_ui32RepairCash = ui32Cash;
							ui32TotalCash	+= ui32Cash;
						}
						else // 있을수 없는경우. 로그남기고 리턴합니다/
						{
							g_pLog->WriteLogSession( this,  L"1 Repair Point Cash Error/ UID : %I64d / ItemDBIdx : %d ",  m_i64UID , TItemDBIdx ); 
							return FALSE;	
						}

						stInfo.m_stRepairItem[0].m_TItemDBIdx = TItemDBIdx;
						stInfo.m_stRepairItem[0].m_ui32AuthArg = ui32AuthArg;
						stInfo.m_stRepairItem[0].m_TItemID = TItemID;
					}
				}
				break;

			default:	// 전체(캐릭터 장착중인 아이템) 수리
				{
					if(  !m_MultiSlot.IsActiveChar( (INT32)ui8CharaSlotIdx, MULTI_SLOT_CHECK_USING )  )
					{	// 슬롯 인덱스 체크.
						stFailInfo.m_i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;
						SendRepairResult(1, &stFailInfo);
						return FALSE;
					}

					switch( ui8RepairType ) 
					{ 
					case S2MO_REPAIR_TYPE_WEAPON :	ui8CheckCount = CHAR_EQUIPMENT_WEAPON_COUNT;		break;
					case S2MO_REPAIR_TYPE_ALL	:	ui8CheckCount = CHAR_EQUIPMENT_COUNT;				break;
					default : 
						{
							stFailInfo.m_i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;
							SendRepairResult(1, &stFailInfo);
							return FALSE;
						}
						break;
					}
					
					m_MultiSlot.GetCharEquipInfo((INT32)ui8CharaSlotIdx, &CharEquip );
	
					for( INT32 i = 0 ; i < ui8CheckCount ; i++ )
					{
						ui32Point = 0;
						ui32Cash = 0;
						ui32AuthArg = 0;
						CharEquip.GetCharEquipmentInfo(i,&ItemInfo );

						i32Rv = _RepairItem(ItemInfo.m_TItemDBIdx, &ui32Point, &ui32Cash, &ui32AuthArg, &TItemID);
						if( EV_SUCCESSED( i32Rv ) && TItemID > 0 )
						{	// 수리가 필요한 장비만 담습니다.
							if( ui32Point > 0 ) 
							{
								stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_ui8BuyType = GOODS_BUY_POINT;
								stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_ui32RepairPoint = ui32Point;
								ui32TotalPoint += ui32Point;
							}
							else if( ui32Cash > 0 )
							{	
								stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_ui8BuyType = GOODS_BUY_CASH;
								stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_ui32RepairCash = ui32Cash;
								ui32TotalCash += ui32Cash;
							}
							else // 있을수 없는경우. 로그남기고 리턴합니다/
							{
								g_pLog->WriteLogSession( this, L"WeaponAll Repair Point Cash Error/ UID : %I64d / ItemDBIdx : %d ", m_i64UID, ItemInfo.m_TItemDBIdx ); 
								return FALSE;	
							}
							stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_TItemDBIdx	= ItemInfo.m_TItemDBIdx;
							stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_ui32AuthArg	= ui32AuthArg;
							stInfo.m_stRepairItem[stInfo.m_ui8ItemCount].m_TItemID		= TItemID;
							stInfo.m_ui8ItemCount++;
						}
					}			
				}
				break;
			}

			// 수리할게 없다.
			if (stInfo.m_ui8ItemCount == 0 || stInfo.m_ui8ItemCount > CHAR_EQUIPMENT_COUNT )
			{
				stFailInfo.m_i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;
				SendRepairResult(1, &stFailInfo);
				break;
			}

			// 수리비 체크
			if(  ui32TotalPoint	> m_UserInfoBasic.m_ui32Point || ui32TotalCash > m_UserInfoBasic.m_ui32Cash ) 
			{
				stFailInfo.m_i32Rv = EVENT_ERROR_EVENT_AUTH_START_PRICE;
				SendRepairResult( 1, &stFailInfo ) ;
				break;
			}

			// 하나라도 성공이면 수리한다.
			if( g_pModuleTrans->AuthSendRepair( m_WorkThreadIdx, this, &stInfo  )  )
			{
				// 수리비 해킹 방지용.
				m_UserInfoBasic.m_ui32Point	-= ui32TotalPoint;
				ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
			}
			else
			{
				stFailInfo.m_i32Rv = EVENT_ERROR_SHOP_REPAIR_FAIL_NETWORK;
				SendRepairResult( 1, &stFailInfo ) ;
				break;
			} 
		}
		break;
	case PROTOCOL_AUTH_ENTER_GIFTSHOP_REQ			: 	 
		{
			CHECKVALID_LOBBY;

			bSendMessage = FALSE;

			char	strMD5[NET_MD5_KEY_SIZE + 1];
			pPacket->ReadData(strMD5, sizeof(char) * NET_MD5_KEY_SIZE);
			strMD5[NET_MD5_KEY_SIZE] = 0;

			PACKET_AUTH_ENTER_GIFTSHOP_ACK	PacketACk;			
			PacketACk.m_i32Result = EVENT_ERROR_SUCCESS;

			//선물샵 on/off 체크
			if (FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_GIFTSHOP))
			{
				PacketACk.m_i32Result = EVENT_ERROR_FAIL;
				S2MOSendEvent(&PacketACk);
				break;
			}				
			//랭크 체크
			if (m_UserInfoBasic.m_ui32Rank < g_pContextMain->m_ui8GiftShopRank)
			{
				PacketACk.m_i32Result = EVENT_ERROR_GIFT_SHOP_ENTER_RANK_FAIL;
				PacketACk.m_ui8GiftShopRank = g_pContextMain->m_ui8GiftShopRank;	
				S2MOSendEvent(&PacketACk);
				break;
			}	

			//0. AAS 체크
			if ( g_pContextMain->CheckAASBlockON( AAS_BLOCK_ENTER_SHOP ) && CheckAAS() )
			{
				PacketACk.m_i32Result = EVENT_ERROR_AAS_ENTER_SHOP;
				S2MOSendEvent( &PacketACk );
				break;
			}

			//1. Room에 있는지 체크 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveGiftShop(this, TRUE); 
			}
			
			m_i32SubTask	= GAME_TASK_SUB_GIFTSHOP;

			// 상점목록을 보낸다.
			if( TRUE == _SendShopInfo( strMD5) )
			{
				PacketACk.m_ui8SendShopInfo = 1;
			}

			PacketACk.m_ui32InvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();			
			S2MOSendEvent( &PacketACk );
		}
		break;
	case PROTOCOL_AUTH_LEAVE_GIFTSHOP_REQ			:
		{
			CHECKVALID_LOBBY;

			bSendMessage = FALSE;
			
			// Room에 있는지 체크  
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveGiftShop( this, FALSE );
			}
			
			m_i32SubTask	= GAME_TASK_SUB_NONE;

			PACKET_AUTH_LEAVE_GIFTSHOP_ACK	PacketACk;
			PacketACk.m_TResult = EVENT_ERROR_SUCCESS;
			S2MOSendEvent( &PacketACk );
		}
		break;
	case PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_REQ:
		{
			CHECKVALID_LOBBY;

			char strCoupon[GIFTCOUPON_LENGTH + 1];
			PACKET_AUTH_SHOP_USE_COUPON_REQ PacketRecv;
			PACKET_AUTH_SHOP_USE_COUPON_ACK	PacketACk;

			S2MORecvPacket( &PacketRecv, pPacket->GetPacketBuffer() );
			PacketRecv.m_strCouponCode.GetValue( strCoupon, GIFTCOUPON_LENGTH + 1 );

			if( !_CheckCouponNumber( strCoupon ) )
			{
				PacketACk.m_TResult = EVENT_ERROR_COUPON_WRONG_NUMBER;
				S2MOSendEvent( &PacketACk );
			}
			else if( !g_pModuleTrans->AuthSendUseCoupon( m_WorkThreadIdx, this, strCoupon ) )
			{
				PacketACk.m_TResult = EVENT_ERROR_NETWORK;
				S2MOSendEvent( &PacketACk );
			}
		}
		break;
	default:
			bisKilled = TRUE; 
			I3TRACE( "UnKnown Packet - Kill User"); 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	// 여기까지 2차 상용화
	}

	return bSendMessage;
}


