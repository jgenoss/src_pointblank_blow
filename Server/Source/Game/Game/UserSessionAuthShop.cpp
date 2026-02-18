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

BOOL	CUserSession::_PacketParsingAuthShop( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ			:						//1224
		{
			CHECKVALID_CHANNEL;			
			CHECKVALID_SHOP;

			UINT8	ui8GoodsCount;
			// 장바구니 때문에 GoodsCount 를 먼저 받아 옵니다. 1이면 일반 구입 1 이상이면 장바구니 구입.
			pPacket->ReadData( &ui8GoodsCount,	sizeof(UINT8) );

			if( ui8GoodsCount < 1 )
			{
				i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED; // 상품 개수가 0 이거나 0 아래면 없는 상품으로 처리
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK);
				pSendPacket->WriteData(&i32Temp,			sizeof(INT32));
				break;
			}

			// 상품이 한개면 일반 구입으로
			if( ui8GoodsCount == 1 )
			{
				BUY_BASKET stBasket;
				pPacket->ReadData( &stBasket,	sizeof(BUY_BASKET) );

				i32Temp = _BuyGoods( stBasket.m_ui32GoodsID, stBasket.m_ui8BuyType, stBasket.m_ui32CouponID , stBasket.m_i64CouponIdx );
			}
			else // 상품이 2개 이상이면 장바구니 구입으로
			{
				// 8개를 넘어갈수는 없습니다.
				if( ui8GoodsCount > USE_BASKET_COUNT )	i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_BASCKET_FAILED;
				else
				{
					BUY_BASKET stBasket[ USE_BASKET_COUNT ];
					pPacket->ReadData( stBasket, sizeof( BUY_BASKET ) * ui8GoodsCount );

					i32Temp = _BuyGoodsBasket( stBasket, ui8GoodsCount );
				}
			}
			if( EV_FAILED( i32Temp ))
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK);
				pSendPacket->WriteData(&i32Temp,			sizeof(INT32));
			}
		}
		break;	
	case PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ:
		{
			CHECKVALID_CHANNEL;
			T_ItemDBIdx i64WareDBIdx;
			pPacket->ReadData(&i64WareDBIdx,	sizeof(T_ItemDBIdx));

			i32Temp = EVENT_ERROR_SUCCESS;
			INVEN_BUFFER* pBuffer = GetItemByWareDBIdx( i64WareDBIdx );
			if( NULL == pBuffer )									i32Temp  = EVENT_ERROR_FAIL;
			else
			{
				if( ITEM_ATTR_TYPE_BUY != pBuffer->_ui8ItemType )	i32Temp  = EVENT_ERROR_FAIL;
				if( 0 == pBuffer->_ui32ItemArg )					i32Temp  = EVENT_ERROR_FAIL;
				if( 0 == pBuffer->_ui32ItemID )						i32Temp  = EVENT_ERROR_FAIL;
			}

			if( EV_SUCCESSED( i32Temp ) ) 
			{
				i32Temp = _SetItemAbilityAuth( pBuffer );
				if( EV_SUCCESSED( i32Temp ) ) 
				{
					if( FALSE == g_pModuleTrans->AuthSendAuthItem(m_WorkThreadIdx, this, i64WareDBIdx, 1 ) )	i32Temp = EVENT_ERROR_FAIL;
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

			T_ItemDBIdx i64WareDBIdx;
			UINT8 ui8Ability = 0;
			char  pAbility[ 255 ];
			pPacket->ReadData(&i64WareDBIdx,	sizeof(T_ItemDBIdx));
			pPacket->ReadData(&ui8Ability,		sizeof(UINT8));
			pPacket->ReadData(pAbility,			ui8Ability);			

			i32Temp = EVENT_ERROR_SUCCESS;
			INVEN_BUFFER* pBuffer = GetItemByWareDBIdx( i64WareDBIdx );
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
                 //if( FALSE == g_pModuleContext->AuthSendAuthItem(m_WorkThreadIdx, this, ui32WareDBIdx, 1 ) )	Temp = EVENT_ERROR_FAIL;
				 if( FALSE == g_pModuleTrans->AuthSendAuthItemData(m_WorkThreadIdx, this, i64WareDBIdx, pBuffer->_ui32ItemID, ui8Ability, pAbility ) )	i32Temp = EVENT_ERROR_FAIL;
			}

			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ:
		{
			T_ItemDBIdx i64WareDBIdx;
			CHECKVALID_CHANNEL;
			pPacket->ReadData(&i64WareDBIdx, sizeof(T_ItemDBIdx));

			i32Temp = EVENT_ERROR_SUCCESS;

			if( FALSE == g_pModuleTrans->AuthSendDeleteItem( m_WorkThreadIdx, this, i64WareDBIdx ) )			i32Temp = EVENT_ERROR_FAIL;
			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_ITEM_REPAIR_REQ:
		{
			CHECKVALID_CHANNEL;

			UINT32	ui32Point	= 0;
			UINT32	ui32Cash	= 0;
			T_ItemDBIdx	i64WareDBIdx;

			pPacket->ReadData(&i64WareDBIdx,		sizeof(T_ItemDBIdx));

			INVEN_BUFFER* pInven = GetItemByWareDBIdx( i64WareDBIdx );

			i32Temp = EVENT_ERROR_SUCCESS;
			if( NULL == pInven )															i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;
			else
			{
				CShop* pShop = g_pContextMain->GetShop();
				SHOP_ITEM_BASE*	pItem		= pShop->GetItemAllData( );
				SHOP_ITEM_DATA*	pItemEx		= pShop->GetItemAllDataEx( );

				if( NULL == pItem || NULL == pItemEx )													i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS;
				else
				{
					if( ITEM_EXPIRE_LEAVE != pItem->_ui8AuthExpireType )					i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;
					else if( pItemEx->_ui32Durability <= pInven->_ui32ItemArg )				i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;
					else if( 0 == pItemEx->_ui32Durability )								i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_REPAIR;
					else
					{
						UINT32 ui32Arg = pItemEx->_ui32Durability - pInven->_ui32ItemArg;

						if( g_pContextMain->m_bIsNewRepair )
						{
							if( 0 < pItemEx->_ui32RepairPoint )
							{
								ui32Point	= (UINT32)((pItemEx->_ui32RepairPoint * ui32Arg)/pItemEx->_ui32Durability);
								ui32Point	+= (UINT32)(ui32Point * _GetRepairPremium( ((100.f/pItemEx->_ui32Durability)*pInven->_ui32ItemArg) ));
								if( 0 == ui32Point ) ui32Point = 1;
							}
							else if( 0 < pItemEx->_ui32RepairCash )
							{
								ui32Cash	= (UINT32)((pItemEx->_ui32RepairCash * ui32Arg)/pItemEx->_ui32Durability);
								ui32Cash	+= (UINT32)(ui32Cash * _GetRepairPremium( ((100.f/pItemEx->_ui32Durability)*pInven->_ui32ItemArg) ));
								if( 0 == ui32Cash ) ui32Cash = 1;
							}
						}
						else
						{
							ui32Point	= (ui32Arg * pItemEx->_ui32RepairPoint);
							ui32Cash	= (ui32Arg * pItemEx->_ui32RepairCash);
						}

						if( ui32Point		> m_UserInfoBasic._point )						i32Temp = EVENT_ERROR_EVENT_AUTH_START_PRICE;
						else if( ui32Cash	> m_UserInfoBasic._cash )						i32Temp = EVENT_ERROR_EVENT_AUTH_START_PRICE;
					}
				}
			}

			if( EV_SUCCESSED( i32Temp ) ) 
			{
				if( g_pModuleTrans->AuthSendRepairItem( m_WorkThreadIdx, this, i64WareDBIdx, ui32Point ) )
				{
					m_UserInfoBasic._point	-= ui32Point;
				}
				else
				{
					i32Temp = EVENT_ERROR_FAIL;
				}
			}

			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_REPAIR_ACK); 
				pSendPacket->WriteData(&i32Temp,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ:
		{
			BOOL bSuccess = TRUE;
			if( true == g_pContextMain->m_bGiftEnable ) 
			{
				// 선물함 확인
				 bSuccess = g_pModuleTrans->AuthSendUserGift(m_WorkThreadIdx, this);
			}
			else	
			{
				bSuccess = FALSE;	
			}
			if( FALSE == bSuccess )
			{
				i32Temp = EVENT_ERROR_FAIL;
				UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();
				UINT64 ui64Temp = 0;	// SIA에서 부터 내려온 사이즈를 맞추기 위해 임시 값을 보냅니다. 의미 없습니다.
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK); 
				pSendPacket->WriteData(&ui32InvenTime,	sizeof(UINT32));
				pSendPacket->WriteData(&ui64Temp,		sizeof(UINT64));
				pSendPacket->WriteData(&i32Temp,			sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ:
		{
			CHECKVALID_CHANNEL;

			if( true == g_pContextMain->m_bGiftEnable ) 
			{

				TGiftDxIdx	i32GiftWareIdx;

				pPacket->ReadData(&i32GiftWareIdx,		sizeof(TGiftDxIdx));

				// 선물받기
				if( FALSE == g_pModuleTrans->AuthSendAuthGift(m_WorkThreadIdx, this, i32GiftWareIdx ) )
				{
					i32Temp = EVENT_ERROR_FAIL;
					bSendMessage= TRUE;
					pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK); 
					pSendPacket->WriteData(&i32Temp,		sizeof(INT32));
				}
			}
			else	
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK); 
				pSendPacket->WriteData(&i32Temp,		sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECKVALID_SHOP;

			bSendMessage= TRUE;
			i32Temp = EVENT_ERROR_FAIL;
			pSendPacket->SetProtocol(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK);
			pSendPacket->WriteData(&i32Temp, sizeof(UINT32));

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

			char strNickname[NET_NICK_NAME_SIZE];

			pPacket->ReadData(strNickname, NET_NICK_NAME_SIZE);
			strNickname[NET_NICK_NAME_SIZE - 1] = 0;


			switch (m_ui8LocaleCode)
			{
			case LOCALE_CODE_TURKEY:		// 터키의 경우 닉네임 앞부분을 GM이나 MOD_로 생성할 수 없음 - 2012-09-07 김용수
				{
					if ( (strNickname[0] == 'G') && (strNickname[1] == 'M') )
					{
						i32Temp = EVENT_ERROR_FAIL;
						bSendMessage= TRUE;
						pSendPacket->SetProtocol( PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK );
						pSendPacket->WriteData(&i32Temp, sizeof(INT32));
						break;
					}
					if ( (strNickname[0] == 'M') && (strNickname[1] == 'O') && (strNickname[2] == 'D') && (strNickname[3] == '_') )
					{
						INT32 i32Rv = EVENT_ERROR_FAIL;
						bSendMessage = TRUE;
						pSendPacket->SetProtocol(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK);
						pSendPacket->WriteData(&i32Rv,	sizeof(INT32));
						break;
					}
				}
				break;

			// 클라팀 요청으로 일단 국가코드 없이 세팅되도록 수정합니다.
			// 2월빌드로 머지할 때는 지울 예정입니다.
			default:
				{
					if ( (strNickname[0] == 'G') && (strNickname[1] == 'M') )
					{
						i32Temp = EVENT_ERROR_FAIL;
						bSendMessage= TRUE;
						pSendPacket->SetProtocol( PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK );
						pSendPacket->WriteData(&i32Temp, sizeof(INT32));
						break;
					}
					if ( (strNickname[0] == 'M') && (strNickname[1] == 'O') && (strNickname[2] == 'D') && (strNickname[3] == '_') )
					{
						INT32 i32Rv = EVENT_ERROR_FAIL;
						bSendMessage = TRUE;
						pSendPacket->SetProtocol(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK);
						pSendPacket->WriteData(&i32Rv,	sizeof(INT32));
						break;
					}
				}
				break;
			}


			if( FALSE == g_pModuleTrans->AuthSendCheckNickName( m_WorkThreadIdx, this, strNickname ) )
			{
				INT32 i32Rv = EVENT_ERROR_FAIL;
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK);
				pSendPacket->WriteData(&i32Rv,	sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_AUTH_COUPON_AWARD_ITEM_REQ:
		{// 유저가 계속 입력하는 경우를 막을 필요 있음
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