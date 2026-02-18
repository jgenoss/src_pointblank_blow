#include "pch.h"
#include "TaskProcessor.h"
#include "ModuleDBShop.h"
#include "UserFinder.h"
#include "ModuleSIA.h"
#include "ModuleLog.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// (구) SIA Task
// Task 쓰레드 
bool CTaskProcessor::_UpdateShop()
{
	// 1 프레임당 버퍼 별로 한개씩만 처리합니다.

	bool bIsWorked = false;
	BOOL bWriteZlog = FALSE;

	char strErrorLog[MAX_PATH];

	// Shop Item
	if( 0 < g_pModuleDBShop->GetOutShopItemCount_T() )
	{		 
		DB_SHOP_ITEM* pDBShopItem = g_pModuleDBShop->GetOutShopItemData_T();
		//인서트 타입으로 선물하기 일경우 
		//sender에게 메시지 내용도 보내서 받는애한테 쪽지보내는 형식으로 변경.
		USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBShopItem->m_TUID);
		if( NULL != pNode ) 
		{
			INT64 i64CharaSync = 0;
			switch( pDBShopItem->m_stDBItem.m_ui8InsertType ) 
			{
			case ITEM_INSERT_REQUEST_MEDAL_REWARD:
				{	// 메달 보상 처리
					i3NetworkPacket	SendPacket( PROTOCOL_GET_MEDAL_REWARD_ACK );	
					SendPacket.WriteData(&pNode->_i32Sessionidx, sizeof(INT32));
					SendPacket.WriteData(&pNode->_i64UID, sizeof(T_UID));
					SendPacket.WriteData(&pDBShopItem->m_i32Rv, sizeof(INT32));
					SendPacket.WriteData(&pDBShopItem->m_stDBItem.m_ui8ItemCount, sizeof(UINT8));

					if (EV_SUCCESSED(pDBShopItem->m_i32Rv))
					{
						for (UINT8 i = 0; i < pDBShopItem->m_stDBItem.m_ui8ItemCount; ++i)
						{	// 유저 인벤토리 Update
							INVEN_BUFFER InvenBuffer;
							i3mem::Copy(&InvenBuffer, &pDBShopItem->m_stDBItem.m_aItem[i], sizeof(INVEN_BUFFER));
							UserInsertItem(pNode, &InvenBuffer, i64CharaSync);
						}
						SendPacket.WriteData(pDBShopItem->m_stDBItem.m_aItem, sizeof(INVEN_BUFFER_DB) * pDBShopItem->m_stDBItem.m_ui8ItemCount);
					}

					SendPacket.WriteData(pDBShopItem->m_stMedalReward.m_aDetail, sizeof(MEDAL_REWARD_DETAIL_INFO)* pDBShopItem->m_stDBItem.m_ui8ItemCount);
					SendGamePacket( pNode->_i32Serveridx, &SendPacket );
				}
				break;
			default :
				{
					i3NetworkPacket	SendPacket( PROTOCOL_AUTH_SHOP_INSERT_ITEM_ACK );
					SendPacket.WriteData(&pNode->_i32Sessionidx, sizeof(INT32));
					SendPacket.WriteData(&pNode->_i64UID, sizeof(T_UID));
					SendPacket.WriteData(&pDBShopItem->m_stDBItem.m_ui8InsertType, sizeof(UINT8));
					SendPacket.WriteData(&pDBShopItem->m_i32Rv, sizeof(INT32));

					if (EV_SUCCESSED(pDBShopItem->m_i32Rv))
					{
						for (UINT8 i = 0; i < pDBShopItem->m_stDBItem.m_ui8ItemCount; ++i)
						{	// 유저 인벤토리 Update
							INVEN_BUFFER InvenBuffer;
							i3mem::Copy(&InvenBuffer, &pDBShopItem->m_stDBItem.m_aItem[i], sizeof(INVEN_BUFFER));
							UserInsertItem(pNode, &InvenBuffer, i64CharaSync);
						}
						SendPacket.WriteData(&pDBShopItem->m_stDBItem.m_ui8ItemCount, sizeof(UINT8));
						SendPacket.WriteData(pDBShopItem->m_stDBItem.m_aItem, sizeof(INVEN_BUFFER_DB) * pDBShopItem->m_stDBItem.m_ui8ItemCount);
						SendPacket.WriteData(&pDBShopItem->m_ui32CapsuleItemID, sizeof(UINT32));
					}
					SendGamePacket( pNode->_i32Serveridx, &SendPacket );
				}
				break;
			}
		}
		//선물 데이터 내용 추가
		if (!g_pConfig->IsContentsEnabled(CONTENTS_GIFT) )
		{
			if (TRUE == IsUseGiftBoxContents(pDBShopItem->m_stDBItem.m_ui8InsertType))
			{
				i3NetworkPacket	SendPacket(PROTOCOL_AUTH_GIFT_INSERT_ACK);
				SendPacket.WriteData(&pDBShopItem->m_i32Rv, sizeof(INT32));
				SendPacket.WriteData(&pDBShopItem->m_TUID, sizeof(T_UID));
				SendPacket.WriteData(&pDBShopItem->m_stDBItem.m_ui8InsertType, sizeof(UINT8));

				UINT8 ui8NickSize = i3String::Length(pDBShopItem->m_strSenderNick);
				UINT8 ui8MessageSize = i3String::Length(pDBShopItem->m_strMessage);

				SendPacket.WriteData(&ui8NickSize, sizeof(UINT8));
				SendPacket.WriteData(&ui8MessageSize, sizeof(UINT8));
				SendPacket.WriteData(pDBShopItem->m_strSenderNick, sizeof(TTCHAR) * ui8NickSize);
				SendPacket.WriteData(pDBShopItem->m_strMessage, sizeof(TTCHAR) * ui8MessageSize);
				if (NULL != pNode)
				{
					SendGamePacket(pNode->_i32Serveridx, &SendPacket);
				}
				else
				{
					for (INT32 i = 1; i < g_pConfig->GetGameServerCount(); i++) //연결된 game 서버를 찾아서 보낸다.
					{
						if (m_pSessionManager->IsActive(i))
						{
							SendGamePacket(i, &SendPacket);
							break;
						}
					}
				}
			}
		}

		g_pModuleDBShop->GetOutShopItemIdx_T();

		bIsWorked = true; 
		m_ui32WorkCount_DB_Item++;
	}


	// ShopBuy
	if( 0 < g_pModuleDBShop->GetOutShopBuyCount_T() )
	{		 
		DB_SHOP_GOODS* pDBShopBuy = g_pModuleDBShop->GetOutShopBuyData_T();

		USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBShopBuy->m_TUID);
		if( NULL == pNode ) goto SHOP_BUY_FAIL;	// 유저 없음.

		switch( pDBShopBuy->m_ui8JobAction )
		{
		default:	
			break;
		case TYPE_SHOP_CHARA_BUY : 
			{
				DB_GOODS *pDBGoods = &pDBShopBuy->m_stDBGoods[0];

				if( EV_FAILED( pDBGoods->m_i32Rv ) )
				{
					_SendFailCreateChara(pNode, pDBGoods->m_i32Rv );

					if( TYPE_SHOP_GOODS_BUY_LINK == pDBShopBuy->m_ui8DBAction  )
					{	// 보상 필요. 로그 남깁니다.
						i3String::Format( strErrorLog, MAX_PATH, "BUY CHARA,ERR, SHOP UID = %I64d,  GoodsID : %d, SlotIdx : %d, DBIDx : %d / Need Compensation ",
							pDBShopBuy->m_TUID, pDBShopBuy->m_stChara.m_TGoodsID, pDBShopBuy->m_stChara.m_ui8CharSlot, pDBShopBuy->m_stChara.m_Inven._TItemWareDBIdx ); 
						WRITE_SERVICE_LOG_BUY_TASK( strErrorLog ); 
					}
					goto SHOP_BUY_FAIL;
				}
				
				T_GoodsID TGoodsID = pDBGoods->m_TGoodsID; 
				pDBShopBuy->m_stChara.m_TUID		= pDBShopBuy->m_TUID;
				pDBShopBuy->m_stChara.m_TGoodsID	= TGoodsID;
				i3mem::Copy( &pDBShopBuy->m_stChara.m_Inven, &pDBGoods->m_stDBItem.m_aItem[0], sizeof( INVEN_BUFFER_DB ) );

				switch( pDBShopBuy->m_ui8DBAction ) // ModuleDBShop 에서의 작업별로 처리.
				{
				case TYPE_SHOP_GOODS_INSERT :	// 포인트 구매
				case TYPE_SHOP_GOODS_BUY_LINK :	// 캐쉬 구매 LINK 결제후 - 인벤토리 DB에 UPDATE 완료후.
					{	
						if( pDBShopBuy->m_stChara.m_bAddCharaSlot )
						{
							// Chara DB에 INSERT 합니다. 
							pDBShopBuy->m_stChara.m_ui8CharSlot = (UINT8)pNode->m_MultiSlot.GetInsertSlot();
							INT32 i32Result = _CreateChar( &pDBShopBuy->m_stChara );
							if( EV_FAILED( i32Result ) )
							{
								_SendFailCreateChara(pNode, i32Result );

								if( TYPE_SHOP_GOODS_BUY_LINK == pDBShopBuy->m_ui8DBAction  )
								{	// 보상 필요. 로그 남깁니다.
									i3String::Format( strErrorLog, MAX_PATH, "BUY CHARA,ERR, CHARA UID = %I64d,  GoodsID : %d, SlotIdx : %d, DBIDx : %d / Need Compensation ",
										pDBShopBuy->m_TUID, pDBShopBuy->m_stChara.m_TGoodsID, pDBShopBuy->m_stChara.m_ui8CharSlot, pDBShopBuy->m_stChara.m_Inven._TItemWareDBIdx ); 
									WRITE_SERVICE_LOG_BUY_TASK( strErrorLog ); 
								}
								goto SHOP_BUY_FAIL;
							}	
						}
						else
						{	// 기간/갯수 연장시 결과를 바로 Game서버로 내려줍니다. 
							pDBShopBuy->m_stChara.m_i32Result = pDBGoods->m_i32Rv;

							S2MO_CHAR_SKILL_INFO stSkill;
							_SendCreateChara( pNode, &pDBShopBuy->m_stChara, &stSkill);
						}

						// 인벤토리 업데이트 
						INT64 i64CharaSync = -1;
						UserInsertItem( pNode, &pDBShopBuy->m_stChara.m_Inven, i64CharaSync);

						i3String::Format( strErrorLog, MAX_PATH, "BUY CHARA,ACK, CHARA UID = %I64d,  GoodsID : %d, SlotIdx : %d, DBIDx : %d , BuyType : %d",
						pDBShopBuy->m_TUID, pDBShopBuy->m_stChara.m_TGoodsID, pDBShopBuy->m_stChara.m_ui8CharSlot, pDBShopBuy->m_stChara.m_Inven._TItemWareDBIdx, pDBShopBuy->m_stChara.m_eType ); 
						WRITE_SERVICE_LOG_BUY_TASK( strErrorLog ); 
					}
					break;
				
				case TYPE_SHOP_GOODS_BUY :
					{ 
						PACKET_BUY_GOODS_REQ BuyReq; 
						SHOP_GOODS_DATA* pGoods = g_pShop->GetGoodsData( TGoodsID); 
						BuyReq.m_stLinkBuy[0].m_TGoodsID	= TGoodsID;
						BuyReq.m_stLinkBuy[0].m_ui32Price	= pGoods->GetCash();
						i3String::Copy( BuyReq.m_stLinkBuy[0].m_strGoodsName, pGoods->_strName,  MAX_ITEM_NAME  );							  
						i3mem::Copy( BuyReq.m_strBillingID,		pNode->m_strBillingID,		BILLING_ID_SIZE );
						i3String::Copy( BuyReq.m_strUserNick,	pNode->_sBasic.m_strNick,	NET_NICK_NAME_SIZE );
						BuyReq.m_ui8GoodsCount		= 1;
						BuyReq.m_TUID				= pNode->_i64UID;
						BuyReq.m_ui8Action			= TYPE_SHOP_CHARA_BUY;
						BuyReq.m_ui32ClientIP		= pNode->_IpAddress;
						BuyReq.m_ui32Rank			= pNode->_sBasic.m_ui32Rank;
						BuyReq.m_ui64UserBillingIdx = pNode->_ui64UserBillingIdx;
						BuyReq.m_ui8LinkMethod		= pNode->m_ui8LinkMethod;						
						
						if( FALSE == g_pModuleSIA->BuyGoods_T( &BuyReq, &pDBShopBuy->m_stChara ) )
						{	// 로그 남깁니다.

							pDBShopBuy->m_stDBGoods[0].m_i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_NETWORK;

							g_pLog->WriteLog( L"SIA Send Fail - Shop Buy Chara  /  UID : %I64d / GoodsID : %d ", pNode->_i64UID, TGoodsID );
							_SendFailCreateChara(pNode, pDBShopBuy->m_stDBGoods[0].m_i32Rv );

							// 인벤토리 DB에서 STAND BY 로우를 DELETE 합니다.
							pDBShopBuy->m_ui8DBAction = TYPE_SHOP_GOODS_BUY_LINK;
							if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InShopBuy( pDBShopBuy ) )
							{	// Error. 가장 첫번째 GoodsID만 기록.
								g_pLog->WriteLog( L"BUFFER OVER - Shop Buy Chara StandBy Delete /  UID : %I64d / GoodsID : %d ", pDBShopBuy->m_TUID, pDBShopBuy->m_stDBGoods[0].m_TGoodsID ); 
							}
						}
					}
					break;
				case TYPE_SHOP_ITEM_EXTEND :
					{	// 기간연장
						// 추후
						// 포인트 , 쪼개서 처리

						// 캐쉬
					}
					break;

				}

				goto SHOP_BUY_FAIL;

			}
			break;
		}


		switch( pDBShopBuy->m_ui8DBAction ) // ModuleDBShop 에서의 작업별로 처리.
		{
		case TYPE_SHOP_GOODS_INSERT :
		case TYPE_SHOP_ITEM_EXTEND :
			{	
				_SendDataShopBuy( pNode, pDBShopBuy ); 
			}
			break;

		case TYPE_SHOP_GOODS_BUY :
			{	// DB에서 하나라도 실패했다면 실패 처리합니다.
				for( UINT8 a = 0; a < pDBShopBuy->m_ui8GoodsCount ; a++ )
				{
					if( EV_FAILED(pDBShopBuy->m_stDBGoods[a].m_i32Rv ) )
					{
						g_pLog->WriteLog(L"SHOP BUY INSERT STAND BY / Fail UID : %d, Method : %d,  ", pNode->_i64UID, pNode->m_ui8LinkMethod);
						for( UINT8 b = 0; b < pDBShopBuy->m_ui8GoodsCount ; b++ ) pDBShopBuy->m_stDBGoods[b].m_i32Rv = EVENT_ERROR_DATABASE;
				
						_SendDataShopBuy( pNode, pDBShopBuy ); 
						goto SHOP_BUY_FAIL;
					}
				}
				
				PACKET_BUY_GOODS_REQ BuyReq;
				for( UINT8 i = 0 ; i < pDBShopBuy->m_ui8GoodsCount ; i++ ) 
				{
					BuyReq.m_stLinkBuy[i].m_TGoodsID= pDBShopBuy->m_stDBGoods[i].m_TGoodsID;
					SHOP_GOODS_DATA* pGoods = g_pShop->GetGoodsData( pDBShopBuy->m_stDBGoods[i].m_TGoodsID );
					BuyReq.m_stLinkBuy[i].m_ui32Price = pGoods->GetCash();
					i3String::Copy( BuyReq.m_stLinkBuy[i].m_strGoodsName, pGoods->_strName,  MAX_ITEM_NAME  );							 
				}

				i3mem::Copy( BuyReq.m_strBillingID,		pNode->m_strBillingID,		BILLING_ID_SIZE );
				i3String::Copy( BuyReq.m_strUserNick,	pNode->_sBasic.m_strNick,	NET_NICK_NAME_SIZE );
				BuyReq.m_TUID				= pNode->_i64UID;
				BuyReq.m_ui8Action			= pDBShopBuy->m_ui8JobAction;
				BuyReq.m_ui32ClientIP		= pNode->_IpAddress;
				BuyReq.m_ui32Rank			= pNode->_sBasic.m_ui32Rank;
				BuyReq.m_ui64UserBillingIdx = pNode->_ui64UserBillingIdx;
				BuyReq.m_ui8LinkMethod		= pNode->m_ui8LinkMethod;
				BuyReq.m_ui8GoodsCount		= pDBShopBuy->m_ui8GoodsCount;
				
				if( FALSE == g_pModuleSIA->BuyGoods_T( &BuyReq, pDBShopBuy ) )
				{	// 로그 남깁니다.
					g_pLog->WriteLog( L"SIA Send Fail - Shop Buy  /  UID : %I64d / GoodsCount : %d / GoodsID : %d ", pDBShopBuy->m_TUID, pDBShopBuy->m_ui8GoodsCount, pDBShopBuy->m_stDBGoods[0].m_TGoodsID );
					for( UINT8 f = 0 ; f < pDBShopBuy->m_ui8GoodsCount ; f++ ) pDBShopBuy->m_stDBGoods[f].m_i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_NETWORK;
					
					// 인벤토리 DB에서 STAND BY 로우를 DELETE 합니다.
					pDBShopBuy->m_ui8DBAction = TYPE_SHOP_GOODS_BUY_LINK;
					if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InShopBuy( pDBShopBuy ) )
					{	// Error. 가장 첫번째 GoodsID만 기록.
						g_pLog->WriteLog( L"BUFFER OVER - DBBuffer StandBy Delete /  UID : %I64d / GoodsCount : %d / GoodsID : %d ", pDBShopBuy->m_TUID, pDBShopBuy->m_ui8GoodsCount, pDBShopBuy->m_stDBGoods[0].m_TGoodsID ); 
					}
				}	
			}	// case 
			break;

		case TYPE_SHOP_GOODS_BUY_LINK:	// DB - LINK - DB 후 최종단계.
			{
				_SendDataShopBuy( pNode, pDBShopBuy, (TYPE_CURRENT_USER_CASH)pDBShopBuy->m_ui32CurrentUserCashType, pDBShopBuy->m_ui32CurrentUserCash );				
			}
			break;
		}	// switch

SHOP_BUY_FAIL:
		g_pModuleDBShop->GetOutShopBuyIdx_T();

		bIsWorked = true; 
		m_ui32WorkCount_DB_Buy++;
	}

	//DataBase_Shop	->Network or Work
	if( 0 < g_pModuleDBShop->GetOutShopCount_T() )
	{
		DB_SHOP_OUT* pDBShopOut = g_pModuleDBShop->GetOutShopData_T();

		// Game서버로 보내는 루틴은 유저 노드가 없으면 break;
		// DB Shop 스레드로 다시 보내는 루틴은 유저 노드 검사안함. 
		switch( pDBShopOut->_ui8Type )
		{
		case TYPE_SHOP_ITEM_DELETE:
			{
				if( pDBShopOut->_i8SlotIdx > -1 )
				{	// 캐릭터 삭제
					if( EV_SUCCESSED(pDBShopOut->_i32Rv) ) 
					{ 
						DeleteCharaDB(pDBShopOut->_i64UID, (INT32)pDBShopOut->_i8SlotIdx, pDBShopOut->_TItemWareDBIdx);
					}
					break;
				}

				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBShopOut->_i64UID);
				if (NULL == pNode) break;

				i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK);
				SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof(INT32) );
				SendPacket.WriteData( &pNode->_i64UID,					sizeof(T_UID) );
				SendPacket.WriteData( &pDBShopOut->_i32Rv,				sizeof(INT32) );
				if( EV_SUCCESSED(pDBShopOut->_i32Rv) ) 
				{
					SendPacket.WriteData( &pDBShopOut->_TItemWareDBIdx,		sizeof(T_ItemDBIdx) );

					UserDeleteItem( pNode, pDBShopOut->_TItemWareDBIdx );
				}
				SendGamePacket( pNode->_i32Serveridx, &SendPacket );
			}
			break;

		case TYPE_SHOP_ITEM_AUTH:
			{
				SHOP_ITEM_DATA* pItemData = g_pShop->GetItemByID( pDBShopOut->_ui32TempItem );	
				if( NULL == pItemData )
				{	// 해당 Item 이 없다.
					g_pLog->WriteLog( L"ItemAuth - Item Find Fail / UID : %I64d, ItemId : %d", pDBShopOut->_i64UID, pDBShopOut->_ui32TempItem );
					break;
				}
			
				if( EV_SUCCESSED(pDBShopOut->_i32Rv) )
				{
					if( 0 != pItemData->_ui32TermItemID )
					{		// 기간제로 바뀌어야 하는 아이템일 경우 다시 DB 에 Insert 를 보낸다.
						ST_ITEM DBItem;
						DBItem.m_TItemID = pItemData->_ui32TermItemID;
						DBItem.m_TItemArg = pItemData->_ui32TermAuthSec;
						if( EV_FAILED( _ItemInsertDB(pDBShopOut->_i64UID, ITEM_INSERT_REQUEST_AUTH, 1, &DBItem ) ) )
						{	
							g_pLog->WriteLog( L"Item Auth Req Fail / UID:%I64d / ItemID:%d / ItemArg:%d",  pDBShopOut->_i64UID, DBItem.m_TItemID, DBItem.m_TItemArg );
							break;
						}
					}

					// 캐릭터 인증일 경우. 추가 처리.
					if( ITEM_TYPE_CHARA == GET_ITEM_TYPE(pDBShopOut->_ui32ItemID) )
					{
						RING_CREATE_CHAR	CharIn;
						CharIn.m_eType			= CHARA_RING_TYPE_USE;
						CharIn.m_TUID			= pDBShopOut->_i64UID;
						CharIn.m_ui8CharSlot	= pDBShopOut->_i8SlotIdx;
						
						_CreateChar( &CharIn );
					}
				}

				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBShopOut->_i64UID);
				if( NULL == pNode ) break;

				// 아니라면 ACK
				i3NetworkPacket SendPacket(PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK);
				SendPacket.WriteData( &pNode->_i32Sessionidx,					sizeof(INT32) );
				SendPacket.WriteData( &pNode->_i64UID,							sizeof(T_UID) );
				SendPacket.WriteData( &pDBShopOut->_i32Rv,						sizeof(INT32) );
				if( EV_SUCCESSED(pDBShopOut->_i32Rv) ) 
				{
					INVEN_BUFFER InvenItem;
					InvenItem._TItemWareDBIdx	= pDBShopOut->_TItemWareDBIdx;
					InvenItem._ui32ItemArg		= pDBShopOut->_ui32AuthSec;
					InvenItem._ui32ItemID		= pDBShopOut->_ui32ItemID;
					InvenItem._ui8ItemType		= pDBShopOut->_ui8ItemState;

					UINT32 ui32ItemID = InvenItem._ui32ItemID;
					if( 0 == InvenItem._ui32ItemID )
					{	// 아이템이 만료되어 삭제
						ui32ItemID = UserDeleteItem( pNode, InvenItem._TItemWareDBIdx );

						// 캐릭터 만료일 경우. 추가 처리.
						if( ITEM_TYPE_CHARA == GET_ITEM_TYPE(ui32ItemID) )
						{
							DeleteCharaDB(pDBShopOut->_i64UID, (INT32)pDBShopOut->_i8SlotIdx, pDBShopOut->_TItemWareDBIdx);
						}
					}
					else
					{
						INT64 i64CharaSync = -1;
						UserInsertItem( pNode, &InvenItem, i64CharaSync);
					}
					_ItemAbility( pNode, ui32ItemID );
					// 캡슐이면 여기서 처리해준다.
					SetCapsule( pNode, ui32ItemID );
					SetPointItem( pNode, ui32ItemID, GET_POINT_ROUTE_ITEM );

					SendPacket.WriteData( &InvenItem._TItemWareDBIdx,		sizeof(T_ItemDBIdx) );
					SendPacket.WriteData( &InvenItem._ui32ItemID,			sizeof(UINT32) );
					SendPacket.WriteData( &InvenItem._ui8ItemType,			sizeof(UINT8) );
					SendPacket.WriteData( &InvenItem._ui32ItemArg,			sizeof(UINT32) );
				}
				SendGamePacket( pNode->_i32Serveridx, &SendPacket );
			}
			break;
		case TYPE_SHOP_ITEM_LOOP_AUTH :
			{
				TLoopCnt TCount				= pDBShopOut->_32Arg1.TLoopCount;
				SHOP_ITEM_DATA* pItemData	= g_pShop->GetItemByID(pDBShopOut->_ui32TempItem);
				if ( nullptr == pItemData )
				{	// 해당 Item 이 없다.
					g_pLog->WriteLog(L"ItemAuthLoop - Item Find Fail / UID : %I64d, ItemId : %d", pDBShopOut->_i64UID, pDBShopOut->_ui32TempItem);
					break;
				}

				if ( EV_SUCCESSED( pDBShopOut->_i32Rv ) )
				{
					if (0 != pItemData->_ui32TermItemID)
					{		// 기간제로 바뀌어야 하는 아이템일 경우 다시 DB 에 Insert 를 보낸다.
						ST_ITEM DBItem;
						DBItem.m_TItemID	= pItemData->_ui32TermItemID;
						DBItem.m_TItemArg	= pItemData->_ui32TermAuthSec;
						if (EV_FAILED(_ItemInsertDB(pDBShopOut->_i64UID, ITEM_INSERT_REQUEST_AUTH, 1, &DBItem)))
						{
							g_pLog->WriteLog(L"Item Loop Auth Req Fail / UID:%I64d / ItemID:%d / ItemArg:%d", pDBShopOut->_i64UID, DBItem.m_TItemID, DBItem.m_TItemArg);
							break;
						}
					}
				}

				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBShopOut->_i64UID);
				if ( nullptr == pNode ) break;

				// 아니라면 ACK
				i3NetworkPacket SendPacket(PROTOCOL_CHEAT_ITEM_AUTH_ACK);
				SendPacket.WriteData(&pNode->_i32Sessionidx,	sizeof(INT32));
				SendPacket.WriteData(&pNode->_i64UID,			sizeof(T_UID));
				SendPacket.WriteData(&pDBShopOut->_i32Rv,		sizeof(INT32));
				if ( EV_SUCCESSED( pDBShopOut->_i32Rv ) )
				{
					INVEN_BUFFER InvenItem;
					InvenItem._TItemWareDBIdx	 = pDBShopOut->_TItemWareDBIdx;
					InvenItem._ui32ItemArg		 = pDBShopOut->_ui32AuthSec;
					InvenItem._ui32ItemID		 = pDBShopOut->_ui32ItemID;
					InvenItem._ui8ItemType		 = pDBShopOut->_ui8ItemState;

					UINT32 ui32ItemID = InvenItem._ui32ItemID;
					if ( 0 == InvenItem._ui32ItemID )
					{	// 아이템이 만료되어 삭제
						ui32ItemID = UserDeleteItem(pNode, InvenItem._TItemWareDBIdx);
					}
					else
					{
						INT64 i64CharaSync = -1;
						UserInsertItem(pNode, &InvenItem, i64CharaSync);
					}
					// _ItemAbility(pNode, ui32ItemID);
				
					// 루프 처리. ItemAbility는 사용 가능성이 없으므로 지금은 제외합니다
					SetCapsuleCount(pNode,	 ui32ItemID, TCount);
					SetPointItemCount(pNode, ui32ItemID, GET_POINT_ROUTE_ITEM, TCount);

					SendPacket.WriteData(&InvenItem._TItemWareDBIdx,	sizeof(T_ItemDBIdx));
					SendPacket.WriteData(&InvenItem._ui32ItemID,		sizeof(UINT32));
					SendPacket.WriteData(&InvenItem._ui8ItemType,		sizeof(UINT8));
					SendPacket.WriteData(&InvenItem._ui32ItemArg,		sizeof(UINT32));
				} // successed
				SendGamePacket( pNode->_i32Serveridx, &SendPacket );
			} // case TYPE_SHOP_ITEM_LOOP_AUTH

		break;
		} // DB Shop Out Type

		g_pModuleDBShop->GetOutShopIdx_T();

		bIsWorked = true; 
		m_ui32WorkCount_DB_Shop++;
	}

	if( 0 < g_pModuleDBShop->GetOutInventoryCount_T() )
	{
		DB_INVENTORY_OUT* pDBInvenOut = g_pModuleDBShop->GetOutInventoryData_T();
		USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBInvenOut->_i64UID);
		if( NULL != pNode ) 
		{
			if( EV_SUCCESSED( pDBInvenOut->_i32Rv ) )
			{
				INT64 i64CharaSync = -1;
				for( TItemCnt i = 0 ; i < pDBInvenOut->_ui16Count; i++ )
				{
					UserInsertItem( pNode, &pDBInvenOut->_Inven[i], i64CharaSync);
				}

				pNode->m_bGiftAutoAuth = pDBInvenOut->_bGiftAutoAuth;
				pNode->_ui16DeleteCharaCount = pDBInvenOut->_ui16DeleteCharaCount;
				i3mem::Copy(pNode->_DeleteCharaDBIdx, pDBInvenOut->_DeleteCharaDBIdx, sizeof(T_ItemDBIdx)*pDBInvenOut->_ui16DeleteCharaCount);
			}
		}

		g_pModuleDBShop->GetOutInventoryIdx_T();

		bIsWorked = true;
		m_ui32WorkCount_DB_Inven++;
	}

	//DataBase_Gift->Network or Work
	if( 0 < g_pModuleDBShop->GetOutGiftCount_T() )
	{
		SEND_GIFT* pGift = g_pModuleDBShop->GetOutGiftData_T();

		switch( pGift->m_eGiftType ) 
		{
		case GIFT_TYPE_USER :
			{
				// 유저 선물 구매는 결제 방식에 따라 나눕니다.
				SEND_GIFT	PointGift;
				SEND_GIFT	CashGift;
				i3mem::Copy( &PointGift,	pGift, sizeof( SEND_GIFT ) );
				i3mem::Copy( &CashGift,		pGift, sizeof( SEND_GIFT ) ); 
				PointGift.m_ui8GiftCount	= 0;
				CashGift.m_ui8GiftCount		= 0; 

				for( UINT8 i = 0; i < pGift->m_ui8GiftCount ; ++i ) 
				{
					switch( pGift->m_aGift[i].m_ui8BuyType ) 
					{	// DB 결과값 UPDATE.
					case GOODS_BUY_POINT : 
						i3mem::Copy( &PointGift.m_aGift[PointGift.m_ui8GiftCount], &pGift->m_aGift[i], sizeof( S2MO_RECV_GIFT ) ); 
						i3mem::Copy(&PointGift.m_InvenItem[PointGift.m_ui8GiftCount], &pGift->m_InvenItem[i], sizeof(INVEN_BUFFER_DB));
						++PointGift.m_ui8GiftCount ;
						break;
					case GOODS_BUY_CASH : 
						i3mem::Copy( &CashGift.m_aGift[CashGift.m_ui8GiftCount], &pGift->m_aGift[i], sizeof( S2MO_RECV_GIFT ) ); 
						i3mem::Copy(&CashGift.m_InvenItem[CashGift.m_ui8GiftCount], &pGift->m_InvenItem[i], sizeof(INVEN_BUFFER_DB));
						++CashGift.m_ui8GiftCount;
						break;
					}
				}

				if( PointGift.m_ui8GiftCount > 0 )
				{	// ZLog 기록
					WriteGiftLog( &PointGift );
				}

				USER_NODE* pSenderNode = g_pUserFinder->GetUserNodeByUID(pGift->m_TSenderUID);
				if( NULL == pSenderNode ) 
				{
					g_pLog->WriteLog( L"NOT FIND USER - Task Shop Gift / Sender UID : %I64d / Receiver UID : %I64d ", 
					pGift->m_TSenderUID, pGift->m_TReceiverUID );
					break;
				}

				if( PointGift.m_ui8GiftCount > 0 )
				{	// 포인트 결제 선물은 보낸 사람, 받는 사람에게 각각 Send. 
 
					_SendGiftSender( pSenderNode, &PointGift ); 

					if (g_pConfig->IsContentsEnabled(CONTENTS_GIFT))
						_SendGiftRecv(PointGift.m_TReceiverUID, &PointGift);
					else
						_SendGiftRecv_Inven(PointGift.m_TReceiverUID, &PointGift);
				
				}

				if( CashGift.m_ui8GiftCount > 0 )
				{
					// SIA 서버로 보냅니다.
					PACKET_BUY_GOODS_REQ BuyReq;
					for( UINT8 i = 0 ; i < CashGift.m_ui8GiftCount ; ++i ) 
					{
						BuyReq.m_stLinkBuy[i].m_TGoodsID = CashGift.m_aGift[i].m_ui32GoodsID;
						SHOP_GOODS_DATA* pGoods = g_pShop->GetGoodsData( BuyReq.m_stLinkBuy[i].m_TGoodsID );
						BuyReq.m_stLinkBuy[i].m_ui32Price = pGoods->GetCash(PRICE_TYPE_GIFT_REAL);
						i3String::Copy( BuyReq.m_stLinkBuy[i].m_strGoodsName, pGoods->_strName,  MAX_ITEM_NAME  );							 
					}

					i3mem::Copy( BuyReq.m_strBillingID,		pSenderNode->m_strBillingID,		BILLING_ID_SIZE );
					i3String::Copy( BuyReq.m_strUserNick,	pSenderNode->_sBasic.m_strNick,	NET_NICK_NAME_SIZE );
					BuyReq.m_TUID				= pSenderNode->_i64UID;
					BuyReq.m_ui8Action			= TYPE_SHOP_GIFT_BUY;
					BuyReq.m_ui32ClientIP		= pSenderNode->_IpAddress;
					BuyReq.m_ui32Rank			= pSenderNode->_sBasic.m_ui32Rank;
					BuyReq.m_ui64UserBillingIdx = pSenderNode->_ui64UserBillingIdx;
					BuyReq.m_ui8LinkMethod		= pSenderNode->m_ui8LinkMethod;
					BuyReq.m_ui8GoodsCount		= CashGift.m_ui8GiftCount;


					if( FALSE == g_pModuleSIA->BuyGoods_T( &BuyReq ,pGift ) )
					{	// 로그 남깁니다.
						g_pLog->WriteLog( L"SIA Send Fail - Gift Buy  /  UID : %I64d / GiftCount : %d / GoodsID : %d ", pGift->m_TSenderUID, CashGift.m_ui8GiftCount,  CashGift.m_aGift[0].m_ui32GoodsID );
						for( UINT8 i = 0 ; i < CashGift.m_ui8GiftCount ; ++i ) CashGift.m_aGift[i].m_i32Rv = EVENT_ERROR_EVENT_CHECK_GIFT_NETWORK;
						_SendGiftSender( pSenderNode, &CashGift );
					}
				
				}

			}

			break;
		default : 
			{
				// EVENT 보상, GM 선물은 받은 유저에게 Send.
				_SendGiftRecv( pGift->m_TReceiverUID, pGift );

				// ZLog 기록
				WriteGiftLog( pGift );
			}
			break;
		}

		g_pModuleDBShop->GetOutGiftIdx_T();

		bIsWorked = true; 
		m_ui32WorkCount_DB_Gift++;
	}

	//DataBase_GiftList->Network or Work
	if( 0 < g_pModuleDBShop->GetOutGiftListCount_T() )
	{
		DB_GIFT_OUT* pOut = g_pModuleDBShop->GetOutGiftListData_T();

		switch( pOut->m_ui8Type )
		{
		case TYPE_SHOP_GIFT_LIST:
			{
				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pOut->m_TUID);
				if( NULL == pNode ) break;

				i3NetworkPacket	SendPacket( PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK );
				SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
				SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
				SendPacket.WriteData( &pOut->_i32Rv,		sizeof( INT32 ) );
				if( EV_SUCCESSED( pOut->_i32Rv ) )
				{
					UINT8	ui8TotalGiftCount		= (UINT8)pOut->m_ui8GiftCount;	// 총 선물 갯수
					ui8TotalGiftCount = MIN( ui8TotalGiftCount, MAX_SHOP_USER_GIFT_COUNT ); 

					UINT32	ui8SendGiftCount	= 0;	// 한 패킷에 보내는 선물 갯수
					UINT32	ui8StartIdx			= 0;	// 선물 배열 인덱스.( 시작 )

					SendPacket.WriteData( &ui8TotalGiftCount,		sizeof( UINT8 ) );
					SendPacket.WriteData( &ui8SendGiftCount,		sizeof( UINT8 ) );	// 보내는 개수
					SendPacket.WriteData( &ui8StartIdx,				sizeof( UINT8 ) );	// 보낼 인덱스

					UINT32 ui32PacketSize  = 0;
					for( ui8StartIdx = 0 ; ui8StartIdx < ui8TotalGiftCount ; ++ui8StartIdx )
					{
						pOut->m_Gift[ui8StartIdx].m_ui8SenderNickSize	= (UINT8)(i3String::Length(pOut->m_Gift[ui8StartIdx].m_strSenderNick ) + 1 ); 
						pOut->m_Gift[ui8StartIdx].m_ui8MessageSize		= (UINT8)(i3String::Length(pOut->m_Gift[ui8StartIdx].m_strMessage ) + 1 );

						ui32PacketSize += sizeof(S2MO_GIFT);

						// MTU 사이즈에 맞춥니다
						if( PACKET_BUFFERSIZE_MTU <= ui32PacketSize  )
						{
							// ui8SendGiftCount 가 write된 위치로 jump 해서 다시 write합니다.
							SendPacket.WriteData( sizeof(INT32) + sizeof(T_UID) + sizeof(INT32) + sizeof( UINT8 ), &ui8SendGiftCount, sizeof( UINT8 ) );

							SendGamePacket( pNode->_i32Serveridx, &SendPacket );

							ui8SendGiftCount = 0;
							ui32PacketSize = 0;

							SendPacket.Clear();
							SendPacket.SetProtocol( PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK ) ;
							SendPacket.WriteData( &pNode->_i32Sessionidx,		sizeof( INT32 ) );
							SendPacket.WriteData( &pNode->_i64UID,				sizeof( T_UID ) );
							SendPacket.WriteData( &pOut->_i32Rv,				sizeof( T_RESULT ) );
							SendPacket.WriteData( &ui8TotalGiftCount,			sizeof( UINT8 ) );
							SendPacket.WriteData( &ui8SendGiftCount,			sizeof( UINT8 ) );
							SendPacket.WriteData( &ui8StartIdx,					sizeof( UINT8 ) );
						}
						SendPacket.WriteData( &pOut->m_Gift[ui8StartIdx].m_TDBIdx,			sizeof(T_GiftDBIdx));
						SendPacket.WriteData( &pOut->m_Gift[ui8StartIdx].m_TGoodsID,		sizeof(T_GoodsID));
						SendPacket.WriteData( &pOut->m_Gift[ui8StartIdx].m_ui8Deliver,		sizeof(UINT8));
						SendPacket.WriteData( &pOut->m_Gift[ui8StartIdx].m_ui32Expire,		sizeof(UINT32));
						SendPacket.WriteData( &pOut->m_Gift[ui8StartIdx].m_ui8SenderNickSize,	sizeof(UINT8));
						SendPacket.WriteData( pOut->m_Gift[ui8StartIdx].m_strSenderNick,	sizeof(TTCHAR) * pOut->m_Gift[ui8StartIdx].m_ui8SenderNickSize);
						SendPacket.WriteData( &pOut->m_Gift[ui8StartIdx].m_ui8MessageSize,	sizeof(UINT8));
						SendPacket.WriteData( pOut->m_Gift[ui8StartIdx].m_strMessage,		sizeof(TTCHAR) * pOut->m_Gift[ui8StartIdx].m_ui8MessageSize);
						ui8SendGiftCount++;
					}

					SendPacket.WriteData( sizeof(UINT32) + sizeof(T_UID) + sizeof(UINT32) + sizeof( UINT8 ), &ui8SendGiftCount, sizeof( UINT8 ) );
					SendGamePacket( pNode->_i32Serveridx, &SendPacket );
				}
				else	SendGamePacket( pNode->_i32Serveridx, &SendPacket );

			}
			break;

		case TYPE_SHOP_GIFT_BUY :
			{
				// DB 결과값 UPDATE.
				for( UINT8 i = 0; i < pOut->m_ui8GiftCount; ++i ) 
				{
					pOut->m_SendGift.m_aGift[i].m_i32Rv = pOut->m_aAuthGift[i].m_i32Rv;
				}

				pOut->m_SendGift.m_eRequestType = ITEM_INSERT_REQUEST_USER_GIFT;	

				USER_NODE* pSenderNode = g_pUserFinder->GetUserNodeByUID(pOut->m_SendGift.m_TSenderUID);
				if( NULL != pSenderNode ) 
				{	// 선물 보낸 사람에게 Send
					_SendGiftSender( pSenderNode, &pOut->m_SendGift, (TYPE_CURRENT_USER_CASH)pOut->m_SendGift.m_ui32CurrentUserCashType, pOut->m_SendGift.m_ui32CurrentUserCash ); 
				}

				// 선물 받는 사람에게 Send

				//_SendGiftRecv( pOut->m_TUID , &pOut->m_SendGift );

				if (g_pConfig->IsContentsEnabled(CONTENTS_GIFT))
					_SendGiftRecv(pOut->m_TUID, &pOut->m_SendGift);
				else
					_SendGiftRecv_Inven(pOut->m_TUID, &pOut->m_SendGift);

				bWriteZlog = TRUE;		// ZLOG 기록함

			}
			break;

		case TYPE_SHOP_GIFT_AUTH :
		case TYPE_SHOP_GIFT_DELETE :
			{
				// Zlog 기록을 위한 추가 작업.
				bWriteZlog = TRUE;		// ZLOG 기록함
				pOut->m_SendGift.m_eRequestType = ITEM_INSERT_REQUEST_UNKNOWN;	// 선물 지급 출처는 알 수 없음.
				pOut->m_SendGift.m_ui8GiftCount = pOut->m_ui8GiftCount;
				pOut->m_SendGift.m_TReceiverUID = pOut->m_TUID;
				for( UINT8 i = 0 ; i < pOut->m_ui8GiftCount ; ++i )
				{
					pOut->m_SendGift.m_aGift[i].m_i32Rv = pOut->m_aAuthGift[i].m_i32Rv;
					pOut->m_SendGift.m_aGift[i].m_ui32GoodsID = pOut->m_aAuthGift[i].m_TGoodsID;
				}
			}
			// break;		// 없는게 맞음
		case TYPE_SHOP_GIFT_READ :
			{
				USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pOut->m_TUID);
				if( NULL == pNode ) break;

				_SendGiftAuth( pNode, pOut ); 
			}
			break;
		}

		// ZLog 기록
		if( TRUE == bWriteZlog ) 
		{
			WriteGiftLog( &pOut->m_SendGift, (ENUM_TYPE_SHOP)pOut->m_ui8Type );
		}

		g_pModuleDBShop->GetOutGiftListIdx_T();

		bIsWorked = true; 
		m_ui32WorkCount_DB_Gift++;
	}
	
	// Repair
	if( 0 < g_pModuleDBShop->GetOutRepairCount_T() )
	{
		DB_REPAIR* pDB = g_pModuleDBShop->GetOutRepairData_T();

		USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDB->m_TUID);
		if( NULL == pNode ) goto SHOP_REPAIR_FAIL;	// 유저 없음.

		switch( pDB->m_eType ) // DB 작업별로 처리.
		{
		case DB_REPAIR_TYPE_BUY :
			{
				S2MO_REPAIR_INFO stRepair[GOODS_BUY_CASH];
				i3mem::FillZero( stRepair, sizeof( S2MO_REPAIR_INFO ) * GOODS_BUY_CASH );
				
				// 결제 타입별로 버퍼에 담는다.
				for( UINT8 i = 0 ; i < pDB->m_stRepairInfo.m_ui8ItemCount ; i++ )
				{
					INT32 Idx = pDB->m_stRepairInfo.m_stRepairItem[i].m_ui8BuyType - 1;	// enum은 1부터시작 하므로 맞춥니다.
					i3mem::Copy( &stRepair[Idx].m_stRepairItem[stRepair[Idx].m_ui8ItemCount], &pDB->m_stRepairInfo.m_stRepairItem[i], sizeof(S2MO_REPAIR_ITEM)  );
					stRepair[Idx].m_ui8ItemCount++;
				}

				// 결제 타입별로 처리.
				for( INT32 k = 0 ; k < GOODS_BUY_CASH ; k++ )
				{
					if( 0 == stRepair[k].m_ui8ItemCount ) continue;	// 요청이 없었다.

					switch( k + 1 )		// enum은 1부터 시작하므로.
					{
					case GOODS_BUY_POINT :
						{	// Game서버로 보냅니다.
							_SendDataRepair( pNode->_i64UID, &stRepair[k] );
						}
						break;
					case GOODS_BUY_CASH :
						{	//수리요청간 유지할 데이터를 함께  SIA서버로 보냅니다.  
							
							PACKET_BUY_GOODS_REQ BuyReq;
							BuyReq.m_ui8GoodsCount = stRepair[k].m_ui8ItemCount;

							for( UINT8 t = 0 ; t < stRepair[k].m_ui8ItemCount ; t++ ) 
							{
								BuyReq.m_stLinkBuy[t].m_TGoodsID= stRepair[k].m_stRepairItem[t].m_TItemID;
								BuyReq.m_stLinkBuy[t].m_ui32Price = stRepair[k].m_stRepairItem[t].m_ui32RepairCash;

								SHOP_ITEM_DATA* pItem = g_pShop->GetItemData( stRepair[k].m_stRepairItem[t].m_TItemID );
								i3String::Copy( BuyReq.m_stLinkBuy[t].m_strGoodsName, pItem->_strName,  MAX_ITEM_NAME  );							 
							}

							i3mem::Copy( BuyReq.m_strBillingID,		pNode->m_strBillingID,		BILLING_ID_SIZE );
							i3String::Copy( BuyReq.m_strUserNick,	pNode->_sBasic.m_strNick,	NET_NICK_NAME_SIZE );
							BuyReq.m_TUID				= pNode->_i64UID;
							BuyReq.m_ui8Action			= TYPE_SHOP_REPAIR;
							BuyReq.m_ui32ClientIP		= pNode->_IpAddress;
							BuyReq.m_ui32Rank			= pNode->_sBasic.m_ui32Rank;
							BuyReq.m_ui64UserBillingIdx = pNode->_ui64UserBillingIdx;
							BuyReq.m_ui8LinkMethod		= pNode->m_ui8LinkMethod;

							if( FALSE == g_pModuleSIA->BuyGoods_T( &BuyReq, &stRepair[k] ) )
							{	// 로그 남깁니다.
								g_pLog->WriteLog( L"SIA Send Fail - Repair  /  UID : %I64d / ItemCount : %d / ItemID : %d / ItemArg : %d / Cash : %d ", 
									pNode->_i64UID,  stRepair[k].m_ui8ItemCount, stRepair[k].m_stRepairItem[0].m_TItemID, stRepair[k].m_stRepairItem[0].m_ui32AuthArg, stRepair[k].m_stRepairItem[0].m_ui32RepairCash );
								for( UINT8 b = 0 ; b < stRepair[k].m_ui8ItemCount ; b++ ) stRepair[k].m_stRepairItem[b].m_i32Rv = EVENT_ERROR_EVENT_BUY_GOODS_NETWORK;
								_SendDataRepair( pNode->_i64UID, &stRepair[k] );
							}
						}
						break;
					}	// switch
				}	// for
			}	// case 
			break;

		case DB_REPAIR_TYPE_LINK:	// DB - LINK - DB 후 최종단계.
			{
				_SendDataRepair( pNode->_i64UID, &pDB->m_stRepairInfo );
			}
			break;
		}	// switch

SHOP_REPAIR_FAIL:
		g_pModuleDBShop->GetOutRepairIdx_T();

		bIsWorked = true;
		m_ui32WorkCount_DB_Repair++;
	}

	// GiftCoupon
	if( 0 < g_pModuleDBShop->GetOutCouponCount_T() )
	{
		DB_COUPON* pDBCoupon = g_pModuleDBShop->GetOutCouponData_T();

		USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pDBCoupon->m_TUID);
		if( NULL == pNode ) goto SHOP_COUPON_FAIL;

		switch( pDBCoupon->m_eType )
		{
		case DB_COUPON_TYPE_USE:
			{
				PACKET_USE_GIFTCOUPON_REQ CouponReq;
		
				i3String::Copy( CouponReq.m_strBillingID, pNode->m_strBillingID, BILLING_ID_SIZE );
				i3String::Copy( CouponReq.m_strCouponCode, pDBCoupon->m_strCoupon, GIFTCOUPON_LENGTH + 1);
				CouponReq.m_TUID				= pNode->_i64UID;
				CouponReq.m_ui64UserBillingIdx	= pNode->_ui64UserBillingIdx;
				CouponReq.m_ui8PcCafe			= pNode->_ui8RoleFlag;

				i3String::Copy( CouponReq.m_strID, pNode->_strID, NET_ID_SIZE );
				i3String::Copy( CouponReq.m_strUserNick, pNode->_sBasic.m_strNick, NET_NICK_NAME_SIZE );
				CouponReq.m_ui32BirthDay	= pNode->_ui32BirthDay;
				CouponReq.m_ui32IP			= pNode->_IpAddress;
				CouponReq.m_ui32CreateDate	= pNode->_ui32CreateDate;
				CouponReq.m_ui8LinkMethod	= pNode->m_ui8LinkMethod;
				
				if( FALSE == g_pModuleSIA->UseGiftCoupon_T( &CouponReq ) )
				{
					g_pLog->WriteLog( L"SIA Send Fail - Use GiftCoupon  /  UID : %I64d /  CouponCode : %s ", pNode->_i64UID, pDBCoupon->m_strCoupon );
				}
			}
			break;
		case DB_COUPON_TYPE_LINK:
			{
				i3NetworkPacket	SendPacket( PROTOCOL_AUTH_USE_GIFTCOUPON_ACK );
				SendPacket.WriteData( &pNode->_i32Sessionidx,			sizeof( INT32 ) );
				SendPacket.WriteData( &pNode->_i64UID,					sizeof( T_UID ) );
				SendPacket.WriteData( &pDBCoupon->m_i32Ret,				sizeof( INT32 ) );
				SendPacket.WriteData( &pDBCoupon->m_strCoupon,			GIFTCOUPON_LENGTH + 1 );
				if ( EV_SUCCESSED( pDBCoupon->m_i32Ret ) )
				{
					SendPacket.WriteData( &pDBCoupon->m_i8GoodsCount,		sizeof( INT8 ) );
					SendPacket.WriteData( &pDBCoupon->m_ai32GoodsID,		sizeof( INT32 )*pDBCoupon->m_i8GoodsCount );
				}
				SendGamePacket( pNode->_i32Serveridx, &SendPacket );
			}
			break;
		}

SHOP_COUPON_FAIL:
		g_pModuleDBShop->GetOutCouponIdx_T();

		bIsWorked = true;
		m_ui32WorkCount_DB_Coupon++;
	}

	// Gacha
	if( 0 < g_pModuleDBShop->GetOutGachaCount_T() )
	{
		DB_GACHA* pGacha = g_pModuleDBShop->GetOutGachaData_T();
		USER_NODE* pNode = g_pUserFinder->GetUserNodeByUID(pGacha->m_TUID);
		if( NULL != pNode ) 
		{
			if( EV_FAILED( pGacha->m_i32Rv ) )
			{
				pGacha->m_i32ResultCount = 0; 
				SendGachaPurchaseAck(pNode, GACHA_ERROR_INTERNAL, pGacha );
			}
			else
			{
				switch( pGacha->m_ui8Action )
				{
				case TYPE_GACHA_PURCHASE:	// Link( SIA ) 결제전
					{
						PACKET_BUY_GOODS_REQ BuyReq;
						BuyReq.m_ui8GoodsCount = 1;
						BuyReq.m_stLinkBuy[0].m_TGoodsID = pGacha->m_ui32GachaGoodsID;
						BuyReq.m_stLinkBuy[0].m_ui32Price = pGacha->m_ui32GachaPrice;

						SHOP_GOODS_LIST* pGoodsList = g_pShop->GetGoodsByID( pGacha->m_ui32GachaGoodsID );
						
						i3String::Copy( BuyReq.m_stLinkBuy[0].m_strGoodsName, pGoodsList->_pGoodsData->_strName,  MAX_ITEM_NAME  );							 

						i3mem::Copy( BuyReq.m_strBillingID,		pNode->m_strBillingID,		BILLING_ID_SIZE );
						i3String::Copy( BuyReq.m_strUserNick,	pNode->_sBasic.m_strNick,	NET_NICK_NAME_SIZE );
						BuyReq.m_TUID				= pNode->_i64UID;
						BuyReq.m_ui8Action			= TYPE_GACHA_PURCHASE;
						BuyReq.m_ui32ClientIP		= pNode->_IpAddress;
						BuyReq.m_ui32Rank			= pNode->_sBasic.m_ui32Rank;
						BuyReq.m_ui64UserBillingIdx = pNode->_ui64UserBillingIdx;
						BuyReq.m_ui8LinkMethod		= pNode->m_ui8LinkMethod;

						if( FALSE == g_pModuleSIA->BuyGoods_T( &BuyReq, pGacha ) )
						{	// 로그 남깁니다.
							g_pLog->WriteLog( L"SIA Send Fail - Gacha / UID : %I64d / GoodsID : %d ",  pNode->_i64UID, pGacha->m_ui32GachaGoodsID );
							pGacha->m_i32ResultCount = 0;
							SendGachaPurchaseAck(pNode, GACHA_ERROR_INTERNAL, pGacha );
						} 
					}
					break;
				case TYPE_GACHA_PROVIDE:
					SendGachaPurchaseAck(pNode, GACHA_ERROR_SUCCESS, pGacha );
					break;
				case TYPE_GACHA_TICKET_DELETE: // Link( SIA ) 결제후
					{
						pGacha->m_ui8Action = TYPE_GACHA_PROVIDE;		 
						ProvideGachaItem( pNode, pGacha ); // 당첨 상품 선정

						if( RINGBUFFER_SUCCESS != g_pModuleDBShop->InGacha( pGacha ) )
						{	 
							g_pLog->WriteLog( L"BUFFER OVER - DB Gacha 2 / Action : %d / UID : %I64d / GoodsID : %d ", 
								pGacha->m_ui8Action, pNode->_i64UID, pGacha->m_ui32GachaGoodsID );
							pGacha->m_i32ResultCount = 0;
							SendGachaPurchaseAck(pNode, GACHA_ERROR_INTERNAL, pGacha );
						}
					}
					break;
				default:
					break;
				}
			}
		}

		g_pModuleDBShop->GetOutGachaIdx_T();

		bIsWorked = true;
		m_ui32WorkCount_DB_Gacha++;
	}

	return bIsWorked;
}
