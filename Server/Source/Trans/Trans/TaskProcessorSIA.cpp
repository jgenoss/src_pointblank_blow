#include "pch.h"
#include "TaskProcessor.h"
#include "UserFinder.h"
#include "ModuleDBShop.h"
#include "ModuleLog.h"
#include "ModuleSIA.h"

bool CTaskProcessor::_UpdateSIA()
{
	// 1 프레임당 한개씩만 처리합니다.
	if( 0 == m_pRingSIAPop->GetBufferCount() )	return false;

	TASK_INVEN_BUFFER* pInven = (TASK_INVEN_BUFFER*)m_pRingSIAPop->Pop();
	if( NULL == pInven )						return false;

	char strErrorLog[MAX_PATH];

	switch( pInven->m_ui8Type )
	{
	case TASK_INVEN_TYPE_LOGIN:
		{
			switch( pInven->m_i32Result )
			{
			case EVENT_ERROR_SUCCESS					: //성공
				{
					UID_IN	*	pInUID = m_pDBUID->PushUIDPT(); 
					if(pInUID != NULL)
					{
						pInUID->_ui8Type				= TYPE_UID_LOAD;
						pInUID->m_ui8PBVer				= pInven->m_ui8PBVer;
						pInUID->_ui32ServerIdx			= pInven->m_i32ServerIdx;
						pInUID->_ui32SessionIdx			= pInven->m_i32SessionIdx;
						pInUID->_i64UID					= pInven->m_i64UID;
						pInUID->_i32AuthKey				= pInven->m_ui32AuthKey;
						pInUID->_birthday				= pInven->m_ui32Birthday;
						pInUID->_ui8Gender				= 1;
						pInUID->_pcCafe					= pInven->m_ui8PCCafe;
						pInUID->_ui32PCCafeGUID			= pInven->m_ui32PCCafeGUID;
						pInUID->_ui32PCCafeBillingUID	= pInven->m_ui32PCCafeBillingUID;
						pInUID->_ncCoin					= pInven->m_ui32Cash;
						pInUID->_IpAddress				= pInven->m_ui32IP;
						pInUID->_UniqueKey				= pInven->m_ui64UniqueKey;
						pInUID->m_ui32LoginTime			= pInven->m_ui32LoginTime;
						pInUID->m_ui8LinkThreadIdx		= pInven->m_ui8LinkThreadIdx;
						pInUID->_ui64UserBillingIdx		= pInven->m_ui64BillingIdx;
						pInUID->m_i8UIDCreateType		= pInven->m_i8UIDCreateType;
						pInUID->m_i8LogOutSendType		= pInven->m_i8LogOutSendType;
						pInUID->m_ui8LocaleCode			= pInven->m_ui8LocaleCode;
						pInUID->m_ui8Country			= pInven->m_ui8Country;
						pInUID->m_ui8LinkMethod			= pInven->m_ui8LinkMethod;

						i3String::Copy( pInUID->_strID,			pInven->m_strID,			NET_ID_SIZE );
						i3mem::Copy(	pInUID->m_strBillingID,	pInven->m_strBillingID,		BILLING_ID_SIZE );
						i3mem::Copy(	pInUID->m_strExtUID,	pInven->m_strExtUID,		EXT_UID_SIZE );
						i3mem::Copy(	pInUID->m_strWebLoginKey, pInven->m_strWebLoginKey, WEB_LOGINKEY_SIZE);
						pInUID->_strID[ NET_ID_SIZE-1 ]				= '\0';
						pInUID->m_strBillingID[ BILLING_ID_SIZE-1 ]	= '\0';
						pInUID->m_strExtUID[ EXT_UID_SIZE - 1 ]		= '\0';
						pInUID->m_strWebLoginKey[WEB_LOGINKEY_SIZE - 1] = '\0';

						i3mem::Copy( pInUID->m_aui8ShutDownTime, pInven->m_aui8ShutDownTime, sizeof(UINT8) * SHUTDOWN_TIME_SIZE );

						m_pDBUID->PushUIDPTIdx(); 
					}
					else 
					{
						_SendLogInPacket( EVENT_ERROR_EVENT_LOG_IN_DB_BUFFER_FULL, pInven->m_i32ServerIdx, pInven->m_i32SessionIdx, pInven->m_ui32AuthKey, 0, NULL ); 
					}
				}
				break; 
			default:
				{
					_SendLogInPacket( pInven->m_i32Result, pInven->m_i32ServerIdx, pInven->m_i32SessionIdx, pInven->m_ui32AuthKey, 0, NULL );
				}
				break;
			}
		}
		break;
		// SIA 서버에서 결제된 Cash Buy, Extend, Repair, Gift 의 결과가 다 이쪽으로 옵니다.
	case TASK_INVEN_TYPE_BUY_GOODS:	
		{
			//요청한 작업 종류에 따라 나누어 처리합니다.
			switch ( pInven->m_ui8Action ) 
			{
			case TYPE_SHOP_GOODS_BUY:
			case TYPE_SHOP_ITEM_EXTEND:
				{
					DB_SHOP_GOODS	stDBShopBuy;
					stDBShopBuy.m_ui8JobAction		= pInven->m_ui8Action;
					stDBShopBuy.m_ui8DBAction		= TYPE_SHOP_GOODS_BUY_LINK;
					stDBShopBuy.m_TUID				= pInven->m_i64UID;
					stDBShopBuy.m_ui8GoodsCount		= pInven->m_ui8GoodsCount;
					stDBShopBuy.m_ui32CurrentUserCash	= pInven->m_ui32CurrentUserCash;
					stDBShopBuy.m_ui32CurrentUserCashType = pInven->m_ui32CurrentUserCashType;
					stDBShopBuy.m_TItemDBIdx		= pInven->m_stShopBuffer.m_TItemDBIdx;

					for( UINT8 i = 0; i < pInven->m_ui8GoodsCount ; i++ ) 
					{
						DB_GOODS *pGoods = &stDBShopBuy.m_stDBGoods[i];
						pGoods->m_i32Rv		= pInven->m_i32Rv[i];
						pGoods->m_TGoodsID	= pInven->m_aGoodsID[i];
						pGoods->m_ui8BuyType	= GOODS_BUY_CASH;

						pGoods->m_stDBItem.m_ui8InsertType = ITEM_INSERT_REQUEST_BUY;

						pGoods->m_stDBItem.m_ui8ItemCount = pInven->m_stShopBuffer.m_stDBGoods[i].m_ui8ItemCount;

						SHOP_GOODS_LIST*	pShopGoods = g_pShop->GetGoodsByID( pInven->m_aGoodsID[i] );	
						if( !pShopGoods ) continue;
					 
						for( UINT8 k = 0; k < stDBShopBuy.m_stDBGoods[i].m_stDBItem.m_ui8ItemCount ; k++ ) 
						{
							INVEN_BUFFER_DB* pItem = &pGoods->m_stDBItem.m_aItem[k];
							pItem->_ui32ItemID		= pShopGoods->_ppItemData[k]->_ui32ItemID;
							pItem->_ui32AddItemArg	= pShopGoods->_pui32ItemArg[k];
							pItem->_ui8ItemType	= ITEM_ATTR_TYPE_STAND_BY;
							pItem->_TItemWareDBIdx = pInven->m_stShopBuffer.m_stDBGoods[i].m_aItemDBIdx[k];
						}
					}

					if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InShopBuy( &stDBShopBuy ) )
					{	
						for( UINT8 k = 0 ; k < pInven->m_ui8GoodsCount ; ++k )
						{
							if( EV_SUCCESSED( pInven->m_i32Rv[k] ) )
							{	// 링크 결과값이 성공입니다. 보상 필요.
								i3String::Format( strErrorLog, MAX_PATH, "BUYGOODS,ERR, UID = %I64d, Idx = %d, GoodsID = %d / Need Compensation ",
								pInven->m_i64UID, k, stDBShopBuy.m_stDBGoods[k].m_TGoodsID );
							}
							else
							{	// 링크 결과값이 실패입니다. 
								i3String::Format( strErrorLog, MAX_PATH, "BUYGOODS,ERR, UID = %I64d, Idx = %d, GoodsID = %d, Link Rv = %x ",
								pInven->m_i64UID, k, stDBShopBuy.m_stDBGoods[k].m_TGoodsID,  pInven->m_i32Rv[k] );
							}
							WRITE_SERVICE_LOG_BUY_TASK( strErrorLog );
						}

						g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Shop Buy" );
													
						USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pInven->m_i64UID);
						if( NULL != pNode )
						{
							for( UINT8 f = 0 ; f < pInven->m_ui8GoodsCount ; ++f ) stDBShopBuy.m_stDBGoods[f].m_i32Rv = EVENT_ERROR_EVENT_BUFFER_FULL;
							_SendDataShopBuy( pNode, &stDBShopBuy );
						}
					}
				}
				break;
			case TYPE_SHOP_REPAIR:
				{
					DB_REPAIR DBIn;
					DBIn.m_TUID							= pInven->m_i64UID;
					DBIn.m_eType						= DB_REPAIR_TYPE_LINK;
					DBIn.m_stRepairInfo.m_ui8ItemCount	= pInven->m_stRepairBuffer.m_ui8ItemCount;
					i3mem::Copy( DBIn.m_stRepairInfo.m_stRepairItem, pInven->m_stRepairBuffer.m_stRepairItem, sizeof( S2MO_REPAIR_ITEM ) * DBIn.m_stRepairInfo.m_ui8ItemCount );

					// 상품별로 LINK(SIA) 결과값을 업데이트 시킵니다.
					for( UINT8 i = 0; i < pInven->m_ui8GoodsCount ; i++ ) DBIn.m_stRepairInfo.m_stRepairItem[i].m_i32Rv = pInven->m_i32Rv[i];

					if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InRepair( &DBIn ) )
					{	
						for( UINT8 k = 0; k < pInven->m_ui8GoodsCount ; ++k )
						{
							S2MO_REPAIR_ITEM* Repair = (S2MO_REPAIR_ITEM*)&pInven->m_stRepairBuffer.m_stRepairItem[k];

							if( EV_SUCCESSED( pInven->m_i32Rv[k] ) )
							{	 // 링크 결과값이 성공입니다. 보상 필요. 
								i3String::Format( strErrorLog, MAX_PATH, "REPAIR,ERR, UID = %I64d, Idx : %d, ItemID : %d, ItemArg : %d, RepairCash : %d / Need Compensation ",
								pInven->m_i64UID, k, Repair->m_TItemID, Repair->m_ui32AuthArg, Repair->m_ui32RepairCash ); 
							}
							else
							{	 // 링크 결과값이 실패입니다.
								i3String::Format( strErrorLog, MAX_PATH, "REPAIR,ERR, UID = %I64d, Idx : %d, ItemID : %d, ItemArg : %d, RepairCash : %d, Link Rv : %x",
								pInven->m_i64UID, k, Repair->m_TItemID, Repair->m_ui32AuthArg, Repair->m_ui32RepairCash, pInven->m_i32Rv[k] ); 
							}
							WRITE_SERVICE_LOG_REPAIR_TASK( strErrorLog ); 
						}

						g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Shop Repair" ); 

						USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pInven->m_i64UID);
						if( NULL != pNode )
						{
							S2MO_REPAIR_INFO stRepair;
							stRepair.m_ui8ItemCount = pInven->m_stRepairBuffer.m_ui8ItemCount; 

							// sizeof( S2MO_REPAIR_ITEM ) 와 sizeof( REPAIR_ITEM ) 다르면 문제 생깁니다.
							i3mem::Copy( stRepair.m_stRepairItem, pInven->m_stRepairBuffer.m_stRepairItem, sizeof( S2MO_REPAIR_ITEM ) * pInven->m_stRepairBuffer.m_ui8ItemCount );

							for( UINT8 f = 0 ; f < pInven->m_stRepairBuffer.m_ui8ItemCount ; ++f ) 
							{
								stRepair.m_stRepairItem[f].m_i32Rv = EVENT_ERROR_EVENT_BUFFER_FULL;								
							}
							_SendDataRepair( pNode->_i64UID, &stRepair );  
						}
					}
				}
				break;
			case TYPE_SHOP_CHARA_BUY:
				{
					SHOP_GOODS_LIST *pGoods = g_pShop->GetGoodsByID( pInven->m_stCharaBuffer.m_TGoodsID );	
					UINT32 ui32GoodsPrice	= pGoods->_pGoodsData->GetCash();		

					if( EV_SUCCESSED( pInven->m_i32Rv[0] ))	
					{// 캐쉬 판매 로그 기록. 	
						pGoods->_pGoodsData->_ui32SaleCount++;  
					}

					DB_SHOP_GOODS	stDBShopBuy;
					stDBShopBuy.m_ui8JobAction				= pInven->m_ui8Action;
					stDBShopBuy.m_ui8DBAction				= TYPE_SHOP_GOODS_BUY_LINK;
					stDBShopBuy.m_TUID						= pInven->m_i64UID;
					stDBShopBuy.m_ui8GoodsCount				= 1;
					stDBShopBuy.m_ui32CurrentUserCash		= pInven->m_ui32CurrentUserCash;
					stDBShopBuy.m_ui32CurrentUserCashType	= pInven->m_ui32CurrentUserCashType;
					
					stDBShopBuy.m_stChara.m_eType			= CHARA_RING_TYPE_BUY_CASH;
					stDBShopBuy.m_stChara.m_TUID			= pInven->m_i64UID;
					stDBShopBuy.m_stChara.m_ui8CharSlot		= pInven->m_stCharaBuffer.m_ui8CharSlot;
					stDBShopBuy.m_stChara.m_ui8Class		= pInven->m_stCharaBuffer.m_ui8Class;
					stDBShopBuy.m_stChara.m_TGoodsID		= pInven->m_stCharaBuffer.m_TGoodsID;
					stDBShopBuy.m_stChara.m_Inven._TItemWareDBIdx		= pInven->m_stCharaBuffer.m_TItemDBIdx;
					i3String::Copy( stDBShopBuy.m_stChara.m_CharBaseInfo.m_strCharName, pInven->m_stCharaBuffer.m_strCharName, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
					
					DB_GOODS* pDBGoods		= &stDBShopBuy.m_stDBGoods[0];
					pDBGoods->m_i32Rv		= pInven->m_i32Rv[0];
					pDBGoods->m_TGoodsID	= pInven->m_stCharaBuffer.m_TGoodsID;																																																																																																																																																																			
					pDBGoods->m_stDBItem.m_ui8InsertType = ITEM_INSERT_REQUEST_BUY;
					pDBGoods->m_stDBItem.m_ui8ItemCount	= 1;
					pDBGoods->m_ui8BuyType	= GOODS_BUY_CASH;

					INVEN_BUFFER_DB* pItem = &pDBGoods->m_stDBItem.m_aItem[0];
					pItem->_ui32ItemID		= pInven->m_stCharaBuffer.m_TGoodsID / 100; 
					pItem->_ui8ItemType		= ITEM_ATTR_TYPE_STAND_BY;
					pItem->_TItemWareDBIdx = pInven->m_stCharaBuffer.m_TItemDBIdx;
					pItem->_ui32AddItemArg	= pGoods->_pui32ItemArg[0];

					if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InShopBuy( &stDBShopBuy ) )
					{	
						if( EV_SUCCESSED( pInven->m_i32Rv[0] ) )
						{	// 링크 결과값이 성공입니다. 보상 필요.
							i3String::Format( strErrorLog, MAX_PATH, "BUY CHARA,ERR, UID = %I64d,  GoodsID : %d, SlotIdx : %d, DBIDx : %d / Need Compensation ",
								pInven->m_i64UID, pInven->m_stCharaBuffer.m_TGoodsID, pInven->m_stCharaBuffer.m_ui8CharSlot, pInven->m_stCharaBuffer.m_TItemDBIdx ); 
						}
						else
						{	// 링크 결과값이 실패입니다. 
							i3String::Format( strErrorLog, MAX_PATH, "BUY CHARA,ERR, UID = %I64d,  GoodsID : %d, SlotIdx : %d, Link Rv : %x, DBIDx : %d",
								pInven->m_i64UID, pInven->m_stCharaBuffer.m_TGoodsID, pInven->m_stCharaBuffer.m_ui8CharSlot, pInven->m_i32Rv[0], pInven->m_stCharaBuffer.m_TItemDBIdx ); 
						}
						WRITE_SERVICE_LOG_BUY_TASK( strErrorLog );

						g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Chara Buy" );

						// 유저에게 실패패킷 전송
						USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pInven->m_i64UID);
						if( NULL != pNode )
						{
							_SendFailCreateChara(pNode, EVENT_ERROR_EVENT_BUFFER_FULL );
						}
						break;
					}

					// 성공이면 Cash Update
					USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pInven->m_i64UID);
					_UpdateCash( pNode, ui32GoodsPrice, pInven->m_ui32CurrentUserCash, (TYPE_CURRENT_USER_CASH)pInven->m_ui32CurrentUserCashType );
				}
				break;

			case TYPE_GACHA_PURCHASE:
				{
					USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pInven->m_i64UID); 
					if( !pNode ) break;

					DB_GACHA stDBGacha;
					i3mem::FillZero( &stDBGacha, sizeof( DB_GACHA ) );

					BUFFER_BUY_GACHA* pGacha =  &pInven->m_stGachaBuffer;
					i3mem::Copy( &stDBGacha, pGacha, sizeof( GACHA_PURCHACE_BASE ) );
					stDBGacha.m_TUID = pInven->m_i64UID;
					stDBGacha.m_ui8BuyType = GOODS_BUY_CASH;
					stDBGacha.m_ui8Action = TYPE_GACHA_TICKET_DELETE;
					stDBGacha.m_TTicketDBIdx = pGacha->m_TTicketDBIdx;  
					stDBGacha.m_ui32GachaGoodsID = pInven->m_aGoodsID[0];

					SHOP_GOODS_LIST* pGoods = g_pShop->GetGoodsByID( pInven->m_aGoodsID[0] );
					stDBGacha.m_ui32GachaPrice = pGoods->_pGoodsData->GetCash();

					if( EV_FAILED(pInven->m_i32Rv[0]) )
					{
						stDBGacha.m_i32ResultCount = 0;
						SendGachaPurchaseAck(pNode, GACHA_ERROR_PAYMENT, &stDBGacha );
						break;
					}

					// 유저 캐쉬 UPDATE 
					_UpdateCash( pNode, stDBGacha.m_ui32GachaPrice, pInven->m_ui32CurrentUserCash, (TYPE_CURRENT_USER_CASH)pInven->m_ui32CurrentUserCashType ); 						

					// DB에 INSERT된 Gacha Ticket을 Delete 합니다. 
					if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InGacha( &stDBGacha ) )
					{	// 실패할 경우 보상 필요합니다.
						g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Gacha Ticket Delete Need Compensation / UID : %I64d / DBIdx : %d / GoodsID : %d ",
							pInven->m_i64UID, pGacha->m_TTicketDBIdx, pInven->m_aGoodsID[0]);

						i3String::Format( strErrorLog, MAX_PATH, "BUFFER OVER - DBBuffer Gacha Ticket Delete Need Compensation / UID : %I64d / DBIdx : %d / GoodsID : %d ",
							pInven->m_i64UID, pGacha->m_TTicketDBIdx, pInven->m_aGoodsID[0]);
						WRITE_SERVICE_LOG_BUY_TASK( strErrorLog );

						stDBGacha.m_i32ResultCount = 0;
						SendGachaPurchaseAck(pNode, GACHA_ERROR_INTERNAL, &stDBGacha );
					}
				}
				break;
			case TYPE_SHOP_GIFT_BUY:
				{	
					DB_GIFT_IN	GiftIn;
					GiftIn.m_ui8Type		= TYPE_SHOP_GIFT_BUY;
					GiftIn.m_TUID			= pInven->m_stGiftBuffer.m_TReceiverUID;	// DB는 선물 받는 유저 UID로.
					GiftIn.m_ui8GiftCount	= pInven->m_ui8GoodsCount;
					
					GiftIn.m_SendGift.m_ui8GiftCount			= pInven->m_ui8GoodsCount;
					GiftIn.m_SendGift.m_ui32CurrentUserCash		= pInven->m_ui32CurrentUserCash;
					GiftIn.m_SendGift.m_ui32CurrentUserCashType	= pInven->m_ui32CurrentUserCashType;
					GiftIn.m_SendGift.m_eGiftType = GIFT_TYPE_USER;

					GiftIn.m_SendGift.m_TReceiverUID = pInven->m_stGiftBuffer.m_TReceiverUID;
					GiftIn.m_SendGift.m_TSenderUID = pInven->m_i64UID;

					USER_NODE* pSenderNode = g_pUserFinder->GetUserNodeByUID(pInven->m_i64UID);
					if( pSenderNode ) i3String::Copy( GiftIn.m_SendGift.m_strSenderNick, pSenderNode->_sBasic.m_strNick, NET_NICK_NAME_SIZE );
					else i3String::Copy( GiftIn.m_SendGift.m_strSenderNick, _TT("NONE USER"), NET_NICK_NAME_SIZE );	// 임시로
					i3String::Copy( GiftIn.m_SendGift.m_strMessage, pInven->m_stGiftBuffer.m_strMessage, pInven->m_stGiftBuffer.m_ui8MessageSize + 1 );
					GiftIn.m_SendGift.m_strMessage[pInven->m_stGiftBuffer.m_ui8MessageSize] = 0;					
					
					for( UINT8 i = 0; i < pInven->m_ui8GoodsCount ; ++i )
					{ 
						GiftIn.m_SendGift.m_aGift[i].m_ui8BuyType = GOODS_BUY_CASH;
						GiftIn.m_SendGift.m_aGift[i].m_ui32GoodsID = pInven->m_aGoodsID[i];
						GiftIn.m_SendGift.m_aGift[i].m_TGiftDBIdx = pInven->m_stGiftBuffer.m_aGiftDBIdx[i]; 
						
						GiftIn.m_aAuthGift[i].m_i32Rv =  pInven->m_i32Rv[i]; 
						GiftIn.m_aAuthGift[i].m_TDBIdx = pInven->m_stGiftBuffer.m_aGiftDBIdx[i]; 
					}

					if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InGift( &GiftIn ) )
					{	// 링크 결과 값( pInven->m_i32Rv[0] )이 성공이라면 이경우는 아이템 보상이 필요합니다. 
						for( UINT8 k = 0; k < pInven->m_ui8GoodsCount ; ++k )
						{
							if( EV_SUCCESSED( pInven->m_i32Rv[k] ) )
							{	 // 링크 결과값이 성공입니다. 보상 필요.  
								i3String::Format( strErrorLog, MAX_PATH, "BUY GIFT,ERR, S_UID = %I64d, R_UID = %I64d, GoodsID = %d, DBIdx = %d, Cash = %d / Need Compensation",
								pInven->m_i64UID, pInven->m_stGiftBuffer.m_TReceiverUID, pInven->m_aGoodsID[k], pInven->m_stGiftBuffer.m_aGiftDBIdx[k], pInven->m_ui32CurrentUserCash );
							}
							else
							{	 // 링크 결과값이 성공입니다. 보상 필요.  
								i3String::Format( strErrorLog, MAX_PATH, "BUY GIFT,ERR, S_UID = %I64d, R_UID = %I64d, GoodsID = %d, DBIdx = %d, Cash = %d, Link Rv = %x",
								pInven->m_i64UID, pInven->m_stGiftBuffer.m_TReceiverUID, pInven->m_aGoodsID[k], pInven->m_stGiftBuffer.m_aGiftDBIdx[k], pInven->m_ui32CurrentUserCash, pInven->m_i32Rv[k] );
							}

							WRITE_SERVICE_LOG_GIFT_TASK( strErrorLog );
						}  
						g_pLog->WriteLog( L"BUFFER OVER - DBBuffer Gift Buy " ); 						

						if( pSenderNode ) 
						{	// 선물 보낸 사람에게 Send
							_SendGiftSender( pSenderNode, &GiftIn.m_SendGift, (TYPE_CURRENT_USER_CASH)pInven->m_ui32CurrentUserCashType, pInven->m_ui32CurrentUserCash ); 
						} 
					}					
				}
				break;	
			}
		}
		break;

	case TASK_INVEN_TYPE_GET_CASH:
		{
			USER_NODE* pUserNode =  g_pUserFinder->GetUserNodeByUID( pInven->m_i64UID );
			if( NULL == pUserNode )					
			{
				g_pLog->WriteLog( L"Task SIA Fail / Non User / UID : %I64d ",  pInven->m_i64UID );

				break;
			}
			pUserNode->_ui32Cash = pInven->m_ui32Cash;

			_AlarmCoin( pUserNode->_i32Serveridx, pUserNode->_i32Sessionidx, pUserNode->_i64UID, pInven->m_i32Result, 0, pUserNode->_ui32Cash );
		}
		break;

	case TASK_INVEN_TYPE_USE_GIFTCOUPON:
		{
			USER_NODE* pUserNode = g_pUserFinder->GetUserNodeByUID( pInven->m_i64UID );
			if( NULL == pUserNode )
			{
				g_pLog->WriteLog( L"Task SIA Fail / Non User / UID : %I64d / Use GiftCoupon",  pInven->m_i64UID );

				break;
			}

			DB_COUPON DBIn;
			DBIn.m_TUID			= pUserNode->_i64UID;
			DBIn.m_eType		= DB_COUPON_TYPE_LINK;
			DBIn.m_i32Ret		= pInven->m_i32Result;
			i3mem::Copy( &DBIn.m_strCoupon, pInven->m_strCouponCode, GIFTCOUPON_LENGTH + 1 );

			if ( EV_SUCCESSED( DBIn.m_i32Ret ) )
			{
				DBIn.m_i8GoodsCount	= pInven->m_ui8GoodsCount;
				i3mem::Copy( DBIn.m_ai32GoodsID, pInven->m_i32Rv, sizeof(INT32)*DBIn.m_i8GoodsCount );

				pUserNode->_ui32Cash += pInven->m_ui32Cash;
				_AlarmCoin( pUserNode->_i32Serveridx, pUserNode->_i32Sessionidx, pUserNode->_i64UID, pInven->m_i32Result, 0, pUserNode->_ui32Cash );
			}

			if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InCoupon( &DBIn ) )
			{
				DBIn.m_strCoupon[GIFTCOUPON_LENGTH] = '\0';
				g_pLog->WriteLog( L"SIA Send Fail - Use GiftCoupon / UID : %I64d / CouponCode : %s", pInven->m_i64UID, DBIn.m_strCoupon );
			}
		}
		break;

	case TASK_INVEN_TYPE_PCCAFE:
		{
			switch( pInven->m_ui8Action )
			{
			case TYPE_PCCAFE_SUBTRACTION:
				{
					USER_NODE* pUserNode = g_pUserFinder->GetUserNodeByUID( pInven->m_Subtraction.m_TUID );
					if( NULL == pUserNode )
					{
						// 유저가 게임중이 아닌데 웹젠서버에서 차감정보가 전달되었다. 웹젠서버에 로그아웃 요청을 해야한다.
						ST_PACKET_LOGOUT_REQ		 LogoutData;
						LogoutData.m_ui32PCCafeBillingUID = pInven->m_Subtraction.m_i32BillingGUID;

						if( FALSE == g_pModuleSIA->SendLogout( &LogoutData ) )
						{
							g_pLog->WriteLog( L"[ERROR]SEND LOGOUT FAIL, BillingUID:%u", LogoutData.m_ui32PCCafeBillingUID );
						}
					}
				}
				break;
			case TYPE_PCCAFE_CONNECT:
				{
					UINT32 ui32MaxUserCount = g_pUserFinder->GetMaxUserCount();

					for( UINT32 i = 0; i < ui32MaxUserCount; i++ )
					{
						USER_NODE* pNode = g_pUserFinder->GetUserNodeByIdx( i );
						
						if( NULL == pNode )												continue;
						if( USER_STATE_INIT == pNode->_ui32State )						continue;
						if( pNode->m_PCCafeInfo.GetPCCafeGUID() <= 0 )					continue;
						if( PCCAFE_WORK_STATE_NONE != pNode->m_PCCafeInfo.GetState() )	continue;
						if( PC_NOT_CAFE != pNode->_ui8RoleFlag )						continue;

						pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_LOGIN );
					}
				}
				break;
			case TYPE_PCCAFE_DISCONNECT:
				{
					UINT32 ui32MaxUserCount = g_pUserFinder->GetMaxUserCount();

					for( UINT32 i = 0; i < ui32MaxUserCount; i++ )
					{
						USER_NODE* pNode = g_pUserFinder->GetUserNodeByIdx( i );
						
						if( NULL == pNode )												continue;
						if( USER_STATE_INIT == pNode->_ui32State )						continue;
						if( pNode->m_PCCafeInfo.GetPCCafeGUID() <= 0 )					continue;
						if( PCCAFE_WORK_STATE_NONE != pNode->m_PCCafeInfo.GetState() )	continue;
						if( PC_NOT_CAFE == pNode->_ui8RoleFlag )						continue;

						pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_REMOVE_RIGHT );
					}
				}
				break;
			case TYPE_PCCAFE_LOGIN:
				{
					USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( pInven->m_Login.m_TUID );
					if( NULL == pNode )
					{
						g_pLog->WriteLog( L"Task SIA Fail / Non User / UID : %I64d / MASK_PCCAFE_ACK_LOGIN", pInven->m_Login.m_TUID );
						break;
					}

					// 1. 유저노드 정보에 PC방 혜택을 재부여한다.
					pNode->_ui8RoleFlag = pInven->m_Login.m_ui8PCCafe;

					// 2. 3분마다 정량 재충전 확인 상태를 종료한다.
					pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_NONE );

					// 3. 새로 발급된 카페UID와 빌링UID를 적용한다.
					pNode->m_PCCafeInfo.SetPCCafeGUID( pInven->m_Login.m_ui32PCCafeGUID );
					pNode->m_PCCafeInfo.SetBillingUID( pInven->m_Login.m_ui32BillingUID );

					// 4. 게임서버와 클라이언트에게 노티스한다.
					SendPCCafeStatusAck( pNode );
				}
				break;
			case TYPE_PCCAFE_EMPTY_AMOUNT:
				{
					// 정량이 소진되었으므로 유저의 pc방혜택을 제거한다.
					USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( pInven->m_EmptyAmount.m_TUID );
					if( NULL == pNode )
					{
						g_pLog->WriteLog( L"Task SIA Fail / Non User / UID : %I64d / MASK_PCCAFE_ACK_EMPTY_AMOUNT", pInven->m_EmptyAmount.m_TUID );
						break;
					}

					// 1. 유저노드 정보에 PC방 혜택을 제거한다.
					pNode->_ui8RoleFlag = PC_NOT_CAFE;

					// 2. 3분마다 웹젠서버에 정량이 재충전되었는지 확인하는 패킷을 보내도록 상태 변경한다.
					pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_CHARGE_CHECK );
					
					// 3. 게임서버와 클라이언트에게 노티스한다.
					SendPCCafeStatusAck( pNode );
				}
				break;
			case TYPE_PCCAFE_CHECK_AMOUNT:
				{
					// 소진되었던 정량이 충전되었으므로 유저의 pc방혜택을 원복한다.
					USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID( pInven->m_CheckAmount.m_TUID );
					if( NULL == pNode )
					{
						g_pLog->WriteLog( L"Task SIA Fail / Non User / UID : %I64d / MASK_PCCAFE_ACK_CHECK_AMOUNT", pInven->m_CheckAmount.m_TUID );
						break;
					}

					// 1. 유저노드 정보에 PC방 혜택을 재부여한다.
					pNode->_ui8RoleFlag = pInven->m_CheckAmount.m_ui8PCCafe;

					// 2. 3분마다 정량 재충전 확인 상태를 종료한다.
					pNode->m_PCCafeInfo.SetState( PCCAFE_WORK_STATE_NONE );
					
					// 3. 새로 발급된 카페UID와 빌링UID를 적용한다.
					pNode->m_PCCafeInfo.SetPCCafeGUID( pInven->m_CheckAmount.m_ui32PCCafeGUID );
					pNode->m_PCCafeInfo.SetBillingUID( pInven->m_CheckAmount.m_ui32BillingUID );

					// 4. 게임서버와 클라이언트에게 노티스한다.
					SendPCCafeStatusAck( pNode );
				}
				break;
			}
		}
		break;
	}

	m_pRingSIAPop->PopIdx();

	return true;
}
