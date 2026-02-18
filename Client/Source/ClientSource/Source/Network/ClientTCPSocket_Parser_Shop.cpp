#include "pch.h"
#include "ClientTCPSocket.h"
#include "Shop.h"
#include "IGShopContext.h"
#include "IGShopItemController.h"
#include "UserInfoContext.h"
#include "MailBoxContext.h"
#include "ShopContext.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"

#include "UI/PurchaseMgr.h"
#include "UI_V11/UITopMenu_V11.h"
#include "UI/UIPhaseSubBase.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "i3Base/string/compare/generic_compare.h"

#include "Designation/DesigUtil.h"


namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK : return "PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK";
		case PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK : return "PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK";
		case PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK : return "PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK";
		case PROTOCOL_SHOP_REPAIR_ACK : return "PROTOCOL_SHOP_REPAIR_ACK";
		case PROTOCOL_AUTH_SHOP_CAPSULE_ACK : return "PROTOCOL_AUTH_SHOP_CAPSULE_ACK";
		case PROTOCOL_AUTH_SHOP_RECV_GIFT_ACK : return "PROTOCOL_AUTH_SHOP_RECV_GIFT_ACK";
		case PROTOCOL_AUTH_SHOP_JACKPOT_ACK : return "PROTOCOL_AUTH_SHOP_JACKPOT_ACK";
		case PROTOCOL_AUTH_SHOP_ITEMLIST_ACK : return "PROTOCOL_AUTH_SHOP_ITEMLIST_ACK";
		case PROTOCOL_AUTH_SHOP_GOODSLIST_ACK : return "PROTOCOL_AUTH_SHOP_GOODSLIST_ACK";
		case PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK : return "PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK";
		case PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK : return "PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK";
		case PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK : return "PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK";
		case PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK : return "PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK";
		case PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK : return "PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK";
		case PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK : return "PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK";
		case PROTOCOL_AUTH_GET_POINT_CASH_ACK : return "PROTOCOL_AUTH_GET_POINT_CASH_ACK";
		case PROTOCOL_SHOP_PLUS_POINT_ACK : return "PROTOCOL_SHOP_PLUS_POINT_ACK";
		case PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK : return "PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK";
		case PROTOCOL_SHOP_ENTER_ACK : return "PROTOCOL_SHOP_ENTER_ACK";
		case PROTOCOL_SHOP_LEAVE_ACK : return "PROTOCOL_SHOP_LEAVE_ACK";
		case PROTOCOL_SHOP_GET_SAILLIST_ACK : return "PROTOCOL_SHOP_GET_SAILLIST_ACK";
		case PROTOCOL_AUTH_ENTER_GIFTSHOP_ACK : return "PROTOCOL_AUTH_ENTER_GIFTSHOP_ACK";
		case PROTOCOL_AUTH_LEAVE_GIFTSHOP_ACK : return "PROTOCOL_AUTH_LEAVE_GIFTSHOP_ACK";
		case PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK : return "PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK";
		case PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK: return "PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK"; 
		default:
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_SHOP_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_SHOP_UNKNOWN";
		}
	}
}


void ClientTCPSocket::_PacketParsingShop(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Shop.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Shop.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Shop_Function( void)
{
	RegisterParser( PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_BuyGoods);
	RegisterParser( PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_AuthItem);
	RegisterParser( PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_DeleteItem);
	RegisterParser( PROTOCOL_SHOP_REPAIR_ACK,				&ClientTCPSocket::__Parse_Auth_Lobby_RepairEquipment);
	RegisterParser( PROTOCOL_AUTH_SHOP_CAPSULE_ACK,			&ClientTCPSocket::__Parse_Auth_Shop_Capsule);
	RegisterParser( PROTOCOL_AUTH_SHOP_JACKPOT_ACK,			&ClientTCPSocket::__Parse_Auth_Shop_Jackpot);
	RegisterParser( PROTOCOL_AUTH_SHOP_ITEMLIST_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_ItemList);
	RegisterParser( PROTOCOL_AUTH_SHOP_GOODSLIST_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_GoodsList);
	RegisterParser( PROTOCOL_AUTH_SHOP_REPAIRLIST_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_RepairList);
	RegisterParser( PROTOCOL_AUTH_SHOP_MATCHINGLIST_ACK,	&ClientTCPSocket::__Parse_Auth_Shop_MatchingList);
	RegisterParser( PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_ACK,	&ClientTCPSocket::__Parse_Auth_UseItem_CheckNick);
	RegisterParser( PROTOCOL_AUTH_GET_POINT_CASH_ACK,		&ClientTCPSocket::__Parse_Auth_Get_PointAndCash);
	RegisterParser( PROTOCOL_SHOP_PLUS_POINT_ACK,			&ClientTCPSocket::__Parse_Auth_PlusPoint);
	RegisterParser( PROTOCOL_AUTH_SHOP_SALECOUPONLIST_ACK,	&ClientTCPSocket::__Parse_Auth_SaleCouponList);
	RegisterParser( PROTOCOL_SHOP_ENTER_ACK,				&ClientTCPSocket::__Parse_Shop_Enter);
	RegisterParser( PROTOCOL_SHOP_LEAVE_ACK,				&ClientTCPSocket::__Parse_Shop_Leave);
	RegisterParser( PROTOCOL_SHOP_GET_SAILLIST_ACK,			&ClientTCPSocket::__Parse_Shop_GetItemList);
	RegisterParser( PROTOCOL_AUTH_ENTER_GIFTSHOP_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_GiftShop_Enter);
	RegisterParser( PROTOCOL_AUTH_LEAVE_GIFTSHOP_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_GiftShop_Leave);
	RegisterParser( PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK,	&ClientTCPSocket::__Parse_Auth_Shop_GetGiftList);
	RegisterParser( PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_AuthGift);
	RegisterParser( PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_SendGift);
	RegisterParser( PROTOCOL_AUTH_SHOP_RECV_GIFT_ACK,		&ClientTCPSocket::__Parse_Auth_Shop_RecvGift);
	RegisterParser( PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK,	&ClientTCPSocket::__Parse_Auth_Shop_Use_GiftCoupon);
	RegisterParser(PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_ACK, &ClientTCPSocket::__Parse_Auth_Shop_Item_Change);
}

#ifdef USE_AAS_SYSTEM_FULL
void ClientTCPSocket::__Parse_Shop_Enter( i3NetworkPacket * pPacket)
{
	UINT32 UTemp;
	INT32  i32Result;
	UINT8  ui8RecvShopInfo;	// 상점 데이터 수신 여부. 0 : 수신X , 1 : 수신O

	pPacket->ReadData(&i32Result,		sizeof(INT32));
	pPacket->ReadData(&ui8RecvShopInfo, sizeof(UINT8));
	pPacket->ReadData(&UTemp, sizeof(UINT32));

	if ( EV_SUCCESSED( i32Result ) )
	{
		CShop* pShop = CShop::i();
		if( 0 == ui8RecvShopInfo && true == pShop->GetFirstShopFileCheck() )
		{
			// 상점 데이터를 한번도 수신하지 않았다면 ShopFile로 부터 상점 데이터를 로드합니다.
			pShop->LoadShopFile();
		}
		//else if ( 1 == ui8RecvShopInfo)
		//{
		//	DesigUtil::ClearDesigItemInfosFromServer();
		//}

		UserInfoContext::i()->SetInvenServerTime(UTemp);
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_SHOP );
	}
	else
	{
		// 셧다운 제한으로 인해 상점에 진입할 수 없다는 에러메세지를 출력해주세요
	}
}
#else
void ClientTCPSocket::__Parse_Shop_Enter( i3NetworkPacket * pPacket)
{
	UINT32 UTemp;
	
	UINT8  ui8RecvShopInfo;	// 상점 데이터 수신 여부. 0 : 수신X , 1 : 수신O
	pPacket->ReadData(&ui8RecvShopInfo, sizeof(UINT8));

	CShop* pShop = CShop::i();
	if( 0 == ui8RecvShopInfo && true == pShop->GetFirstShopFileCheck() )
	{
		// 상점 데이터를 한번도 수신하지 않았다면 ShopFile로 부터 상점 데이터를 로드합니다.
		pShop->LoadShopFile();
	}
	// 상점에서 이미 아이템 목록을 받았고 여기서 초기화 하면 날라갑니다.
	// 날리면 안됩니다.
	//else if ( 1 == ui8RecvShopInfo)
	//{
	//	DesigUtil::ClearDesigItemInfosFromServer();
	//}

	pPacket->ReadData(&UTemp, sizeof(UINT32));
	UserInfoContext::i()->SetInvenServerTime(UTemp);
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_SHOP );
}
#endif

void ClientTCPSocket::__Parse_Shop_Leave( i3NetworkPacket * pPacket)
{
	PACKET_SHOP_LEAVE_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_LEAVE_SHOP, Recv.m_TResult );
}

void ClientTCPSocket::__Parse_Shop_GetItemList( i3NetworkPacket * pPacket)
{
	UINT32 UTemp;
	UINT8  ui8RecvShopInfo;	// 상점 데이터 수신 여부. 0 : 수신X , 1 : 수신O
	pPacket->ReadData(&ui8RecvShopInfo, sizeof(UINT8));

	CShop* pShop = CShop::i();
	if( 0 == ui8RecvShopInfo && true == pShop->GetFirstShopFileCheck() )
	{
		// 상점 데이터를 한번도 수신하지 않았다면 ShopFile로 부터 상점 데이터를 로드합니다.
		pShop->LoadShopFile();
	}
	// 상점 입장할 때 이미 받았기 때문에 여기서 초기화 하면 다시 날라갑니다.
	//else if ( 1 == ui8RecvShopInfo)
	//{
	//	DesigUtil::ClearDesigItemInfosFromServer();
	//}

	pPacket->ReadData(&UTemp,			sizeof(UINT32));
	UserInfoContext::i()->SetInvenServerTime(UTemp);
}

void ClientTCPSocket::__Parse_Auth_Get_PointAndCash( i3NetworkPacket * pPacket)
{
	INT32	i32Result;
	UINT32	ui32Point;
	UINT32	ui32Cash;

	pPacket->ReadData(&i32Result, sizeof(INT32));

	if (EV_SUCCESSED(i32Result))
	{
		pPacket->ReadData(&ui32Point,	sizeof(UINT32));
		pPacket->ReadData(&ui32Cash,	sizeof(UINT32));

		UserInfoContext::i()->SetMyPoint(ui32Point);
		UserInfoContext::i()->SetMyCash(ui32Cash);

		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
	}
	else
	{
		// TODO: 보유 포인트, 톨 갱신에 실패처리
	}
}

void ClientTCPSocket::__Parse_Auth_Get_PointAndCashForKR( i3NetworkPacket * pPacket)
{
#if defined( LOCALE_KOREA)
	// 한국 PB NCSA_COIN
	INT32	i32Result;
	UINT32	ui32Point;
	UINT32	ui32Cash;
	UINT32	ui32NCCoin;
	UINT32	ui32HappyCoin;

	pPacket->ReadData(&i32Result, sizeof(INT32));

	if (EV_SUCCESSED(i32Result))
	{
		pPacket->ReadData(&ui32Point,		sizeof(UINT32));
		pPacket->ReadData(&ui32NCCoin,		sizeof(UINT32));
		pPacket->ReadData(&ui32HappyCoin,	sizeof(UINT32));

		ui32Cash = ui32NCCoin + ui32HappyCoin;
		// NCSA_Coin 정보 갱신
		UserInfoContext::i()->SetMyNCSACoin(ui32NCCoin, ui32HappyCoin);

		UserInfoContext::i()->SetMyPoint(ui32Point);
		UserInfoContext::i()->SetMyCash(ui32Cash);
	}
	else
	{
		// TODO: 보유 포인트, 톨 갱신에 실패처리
	}
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
#endif
}

void ClientTCPSocket::__Parse_Auth_PlusPoint( i3NetworkPacket * pPacket)
{
	PACKET_SHOP_PLUS_POINT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	UserInfoContext::i()->SetMyPoint( Recv.m_ui32Point );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
}

void ClientTCPSocket::__Parse_Auth_CouponAwardItem( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData( &Temp, sizeof(INT32));

	if( EV_SUCCESSED( Temp ))
	{
		UINT32 UTemp;
		pPacket->ReadData(&UTemp, sizeof(UINT32)); 
		UserInfoContext::i()->SetInvenServerTime(UTemp);

		INT32 nCharacterCount;
		INT32 nWeaponCount;
		INT32 nItemCount;

		pPacket->ReadData(&nCharacterCount, sizeof(INT32));
		pPacket->ReadData(&nWeaponCount, sizeof(INT32));
		pPacket->ReadData(&nItemCount, sizeof(INT32));

		// Character
		if( 0 != nCharacterCount )
		{
			I3_BOUNDCHK( nCharacterCount, 1000);
			INT32			nCharacterSize	= sizeof(INVEN_BUFFER) * nCharacterCount;
			INVEN_BUFFER*	pCharacter		= (INVEN_BUFFER*)i3MemAlloc(nCharacterSize);

			pPacket->ReadData(pCharacter, nCharacterSize);
			CInvenList::i()->AddItem( pCharacter, nCharacterCount);

			I3MEM_SAFE_FREE(pCharacter);
		}

		// Weapon
		if( 0 != nWeaponCount )
		{
			I3_BOUNDCHK( nCharacterCount, 100000);
			INT32			nWeaponSize		= sizeof(INVEN_BUFFER) * nWeaponCount;
			INVEN_BUFFER*	pWeapon			= (INVEN_BUFFER*)i3MemAlloc(nWeaponSize);

			pPacket->ReadData(pWeapon, nWeaponSize);
			CInvenList::i()->AddItem(pWeapon, nWeaponCount);

			I3MEM_SAFE_FREE(pWeapon);
		}				

		// Item
		if( 0 != nItemCount)
		{
			I3_BOUNDCHK( nCharacterCount, 100000);
			INT32			nItemSize	= sizeof(INVEN_BUFFER) * nItemCount;
			INVEN_BUFFER*	pItem		= (INVEN_BUFFER*)i3MemAlloc(nItemSize);

			pPacket->ReadData(pItem, nItemSize);
			CInvenList::i()->AddItem(pItem, nItemCount);

			I3MEM_SAFE_FREE(pItem);
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_COUPON_AWARD, Temp);
}

void ClientTCPSocket::__Parse_Auth_SaleCouponList( i3NetworkPacket * pPacket)
{
#if defined(USE_SALE_COUPON)
	CShop * pShop = CShop::i();

	UINT32 ui32CouponCount;
	UINT32 ui32CurItemCount;
	UINT32 ui32StartIndex;

	SALE_COUPON* pSaleCoupon = pShop->GetCouponAllData();


	pPacket->ReadData(&ui32CouponCount,		sizeof(UINT32));
	pShop->SetCouponCount( ui32CouponCount );

	if(ui32CouponCount > MAX_SHOP_ITEM_COUNT)
	{
		I3PRINTLOG(I3LOG_QA,"Coupon data array overflow!!");
	}

	pPacket->ReadData(&ui32CurItemCount,	sizeof(UINT32));
	pPacket->ReadData(&ui32StartIndex,		sizeof(UINT32));

	if(ui32CouponCount > 0)
	{
		I3ASSERT( ui32CurItemCount < 512);

		INT32 nCouponSize = sizeof(SALE_COUPON) * ui32CurItemCount;
		pPacket->ReadData(&pSaleCoupon[ui32StartIndex],	nCouponSize);
	}
#endif
}


void ClientTCPSocket::__Parse_Auth_Shop_BuyGoods( i3NetworkPacket * pPacket)
{

	PACKET_SHOP_BUY_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
 
	UserInfoContext::i()->SetInvenServerTime(Recv.m_ui32InvenTime);
	UserInfoContext::i()->SetMyPoint(Recv.m_ui32Point);
	UserInfoContext::i()->SetMyCash(Recv.m_ui32Cash);

	// 짜증나게 되어있어서 제 마음대로 고치겠습니다...서버는 한개만 ACK 때렸는데..이걸 쪼개서 클라맘대로 여러개 이벤트를 보내는게 현재로선 무진장 무리수이다...
	// 그냥 한 뭉탱이로 처리하기로 한다....
	const INT32 numBuyResult = Recv.m_aBuyResult.GetCount();
	I3ASSERT_RETURN(numBuyResult > 0);

	if (Recv.m_aBuyResult[0].m_ui8Action == TYPE_SHOP_ITEM_EXTEND)		// 기간 연장 구매..모든 목록이 그냥 연장이다...(자료 구조가 이상한 것임)
	{
		I3ASSERT(numBuyResult == 1);									// 1개만 있는게 정상이랩니다..
		
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_BUY_EXTEND_GOODS, Recv.m_aBuyResult[0].m_i32Rv);
	}
	else
	if (Recv.m_aBuyResult[0].m_ui8Action == TYPE_SHOP_GOODS_BUY )		// 일반 구매..(서버패킷대로 그대로 1대1로 리시브처리합니다..)
	{
		i3::user_data ud = i3::user_data(RecvData_EVENT_BUY_GOODS());
		RecvData_EVENT_BUY_GOODS& data = i3::value_cast<RecvData_EVENT_BUY_GOODS>(ud);

		data.resultList.resize(numBuyResult);
		data.GoodsIDList.resize(numBuyResult);

		for (INT32 i = 0; i < numBuyResult; ++i)
		{
			data.resultList[i] = Recv.m_aBuyResult[i].m_i32Rv;
			data.GoodsIDList[i] = Recv.m_aBuyResult[i].m_TGoodsID;
		}

		GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_BUY_GOODS, ud, data.resultList[0]);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "Invalid action type : %d", Recv.m_aBuyResult[0].m_ui8Action);
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
}

void ClientTCPSocket::__Parse_Auth_Shop_AuthItem( i3NetworkPacket * pPacket)
{
	INT32 result;			
	
	pPacket->ReadData(&result, sizeof(INT32));

	if (EV_SUCCESSED(result)) 
	{
		//2013.4.1. 박기성
		//아이템 인증시 인벤 시간 동기화
		UINT32 invenTime;
		pPacket->ReadData(&invenTime, sizeof(UINT32));
		UserInfoContext::i()->SetInvenServerTime(invenTime);

		INVEN_BUFFER ItemBuffer;
		pPacket->ReadData(&ItemBuffer, sizeof(INVEN_BUFFER));

		// 인증후 아이템 ID 가 없으면 아이템은 삭제 된것이다. ( 개수제 -> 기간제로 바뀌었을경우, 개수제 모두사용 0으로 변경
		if( 0 == ItemBuffer._ui32ItemID ) 
		{
			CInvenList::i()->DeleteInvenBuffer(ItemBuffer._TItemWareDBIdx);
		}
		else
		{
			CInvenList::i()->UpdateInvenBuffer(&ItemBuffer);
		}

		CInvenList::i()->SetLatestUsedCashItemFlag(ItemBuffer._ui32ItemID);
	}


	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ITEM_AUTH, result);
}

void ClientTCPSocket::__Parse_Auth_Shop_DeleteItem( i3NetworkPacket * pPacket)
{
	INT32 result;			
	T_ItemDBIdx i64WareDBIndex = 0;
	pPacket->ReadData(&result,			sizeof(INT32));
	pPacket->ReadData(&i64WareDBIndex,	sizeof(T_ItemDBIdx));

	CInvenList::i()->DeleteInvenBuffer(i64WareDBIndex);

	// result 값
	// EVENT_ERROR_FAIL <-- 잘못된 아이템
	// EVENT_ERROR_EVENT_DELETE_ITEM_OVERLOAD
	// EVENT_ERROR_EVENT_DELETE_ITEM_NETWORK
	// EVENT_ERROR_EVENT_DELETE_ITEM_PARAMETER_USER
	// EVENT_ERROR_EVENT_DELETE_INFO_FAIL
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_AUTH_DELETE, result);
}

void ClientTCPSocket::__Parse_Auth_Lobby_RepairEquipment( i3NetworkPacket * pPacket)
{

	PACKET_SHOP_REPAIR_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
 
	UserInfoContext::i()->SetMyPoint(Recv.m_ui32Point);
	UserInfoContext::i()->SetMyCash(Recv.m_ui32Cash);

	// 개떡같이 여러번 보내는걸 한방에 압축할것..
	i3::user_data ud = i3::user_data(RecvData_EVENT_ITEM_REPAIR());
	RecvData_EVENT_ITEM_REPAIR& repairData = i3::value_cast<RecvData_EVENT_ITEM_REPAIR>(ud);

	const INT32 numResult = Recv.m_aRepairResult.GetCount();
	repairData.resultList.resize(numResult);
	for (INT32 i = 0; i < numResult; ++i)
	{
		repairData.resultList[i] = Recv.m_aRepairResult[i].m_i32Rv;
		// 수리한 아이템 ID = Recv.m_aRepairResult[i].m_TItemID // 써먹지를 않네요...wareDB인덱스같은게 더 유용할것 같기도 하고..
	}
	
	GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_ITEM_REPAIR, ud, repairData.resultList[0]);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);
}

void ClientTCPSocket::__Parse_Auth_Shop_Capsule( i3NetworkPacket * pPacket)
{
	PACKET_AUTH_SHOP_CAPSULE_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	CShopContext::i()->setCapsuleItemIdx( Recv.m_ui8Capsule );

	PBConsoleManager* pCmdMgr = PBConsoleManager ::getInstance();
	if(pCmdMgr)
	{
		PB_CCmdInfo* pCmdInfo = pCmdMgr->getCCmdInfo("UseRandomBox");
		if(pCmdInfo)
		{
			PBConsoleCmd* pCCmd =  pCmdInfo->vCmdElementList[0];
			if(pCCmd && pCCmd->getLogContext() == "LogCheck")
			{
				char LogContext[256] = {0,};
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(Recv.m_ui32ItemID);



				char ItemId[54] = {0};
				itoa( Recv.m_stItem[0].m_TItemID, ItemId, 10);

				strcat(LogContext,"ITEM ID - ");
				strcat(LogContext,ItemId);
				strcat(LogContext," ITEM NAME - ");
				
				// TODO : 파일 자체를 unicode로 바꿀지 말지 판단해야함 (2014.07.21.수빈)
				i3::rc_wstring wstrCapsuleItemResultName;
				
				if (pInfo)
					wstrCapsuleItemResultName = pInfo->GetCapsuleItemResultName(CShopContext::i()->getCapsuleItemIdx());
				else
				{
					I3ASSERT_0;
				}

				i3::string strCapsuleItemResultName;		i3::utf16_to_mb(wstrCapsuleItemResultName, strCapsuleItemResultName);

				strncat(LogContext, strCapsuleItemResultName.c_str(), strCapsuleItemResultName.size() );
				strncat(LogContext,"\n", i3::generic_string_size("\n") );
				pCCmd->setLogContext(LogContext);
				pCCmd->SaveLogFile("UseItemCmd.log",true);
			}
		}
	}

	COutGameQACommandSet * cmdSet = g_pFramework->getOutGameQACommandSet();
	if(cmdSet->isRandomBoxTestEnabled())
	{
		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(Recv.m_ui32ItemID);
		if( pInfo)
		{
			cmdSet->AddResultToBuffer(Recv.m_stItem[0].m_TItemID, pInfo->GetCapsuleItemResultName(CShopContext::i()->getCapsuleItemIdx()));
		}
	}
}

void ClientTCPSocket::__Parse_Auth_Shop_Jackpot( i3NetworkPacket * pPacket)
{
	PACKET_AUTH_SHOP_JACKPOT_ACK	RecvPacket;
	S2MORecvPacket( &RecvPacket, pPacket->GetPacketBuffer() );
	
	// 운영툴 공지	
	TTCHAR	szNick[ NET_NICK_NAME_SIZE ];
	UINT32	ui32ItemID;
	UINT8	ui8CapsulItem;
	
	RecvPacket.m_strNickName.GetValue( szNick, _countof(szNick) );
	ui32ItemID		= RecvPacket.m_ui32ItemID;
	ui8CapsulItem	= RecvPacket.m_ui8Capsule;

	szNick[ _countof(szNick) -1 ] = '\0';

	// Jackpot!\n%s 님께서 다음 상품에 당첨되었습니다.\n{col:210,0,0,255}%s{/col}
	ShopItemInfo * pShopItem = g_pShopItemInfoDataBase->getInfo(ui32ItemID);
	I3ASSERT(pShopItem);
	if ( ! pShopItem )
		return;

	i3::rc_wstring wstrResultName = pShopItem->GetCapsuleItemResultName(ui8CapsulItem);
	I3ASSERT(!wstrResultName.empty());
	if ( wstrResultName.empty() )
		return;

	// 9803 : 채팅창에 당첨된 유저 닉네임이 전부 출력되지 않도록 수정
	
	i3::string temp = "**";

	i3::erase_string(szNick, 0, i3::generic_string_size(temp));
	i3::insert_string(szNick, 0, temp, i3::generic_string_size(temp));
	 //wstrResultName.erase(0, 2);
	 //wstrResultName.insert(0, L"**");

	i3::rc_wstring wstrAnnounce;
	i3::sprintf(wstrAnnounce, GAME_RCSTRING("STR_SHOP_JACKPOT_ANNOUNCE2"), szNick, wstrResultName);		
	// szNick은 일단 형변환 기대..(2014.07.07.수빈)

	UINT8 r = 0, g = 0, b = 0, a = 0;
	r = static_cast<UINT8>(255);
	g = static_cast<UINT8>(192);
	b = static_cast<UINT8>(0);
	a = static_cast<UINT8>(255);

	INT32 pColorBuffer = ((a << 24) | (r << 16) | (g << 8) | (b << 0));

	g_pFramework->SetNotice_Permanant(wstrAnnounce, pColorBuffer);
}

void ClientTCPSocket::__Parse_Auth_Shop_ItemList( i3NetworkPacket * pPacket)
{
	CShop* pShop = CShop::i();
	pShop->Reset();

	CShopContext::i()->SetGoodsList(false);
	
	
	UINT32	ui32ItemCount;
	UINT32	ui32Version;
	SHOP_ITEM_DATA*		pItem		= pShop->GetItemAllData();

	pPacket->ReadData(&ui32ItemCount,		sizeof(UINT32));

	pShop->SetItemCount( ui32ItemCount );

	if( ui32ItemCount > MAX_SHOP_ITEM_COUNT)
	{
		I3PRINTLOG(I3LOG_QA,"Shop Item data array overflow!!");

		I3ASSERT( ui32ItemCount < MAX_SHOP_ITEM_COUNT);
	}

	UINT32	ui32CurrentItemCount;
	UINT32	ui32StartIdx;
	pPacket->ReadData(&ui32CurrentItemCount, sizeof(UINT32));
	pPacket->ReadData(&ui32StartIdx,		 sizeof(UINT32));
	
	for( UINT32 i = ui32StartIdx ; i < ui32StartIdx+ui32CurrentItemCount ; i++ )
	{
        pPacket->ReadData( &pItem[i],		sizeof(SHOP_ITEM_BASE));
	}

	pPacket->ReadData(&ui32Version,			sizeof(UINT32));

	I3ASSERT( pItem );

	DesigUtil::UpdateDesigItemInfosFromServer(ui32StartIdx, ui32CurrentItemCount);

#ifdef DEV_BUILD
	FILE* fp = fopen( "recvitems.txt", (ui32StartIdx == 0) ? "wt" : "at" );
	if (fp)
	{
		for (UINT32 i = ui32StartIdx; i < ui32StartIdx + ui32CurrentItemCount; i++)
		{
			fprintf(fp, "[%u] ItemID:%u, AuthType:%u, AuthExpireType:%u, AutoStart:%u, RestrictionType:%u, RestrictionNum:%u\n",
				i + 1, pItem[i]._ui32ItemID, pItem[i]._ui8AuthType, pItem[i]._ui8AuthExpireType, pItem[i]._ui8AutoStart, pItem[i]._ui8RestrictionType, pItem[i]._ui8Restriction);
		}
		fclose(fp);
	}
#endif
}

void ClientTCPSocket::__Parse_Auth_Shop_GoodsList( i3NetworkPacket * pPacket)
{
	CShop* pShop = CShop::i();

	UINT32	ui32GoodsCount;
	UINT32	ui32Version;
	SHOP_GOODS_EXPANSION*	pGoods		= pShop->GetGoodsAllData();

	pPacket->ReadData(&ui32GoodsCount,		sizeof(UINT32));

	pShop->SetGoodsCount( ui32GoodsCount );

	if( ui32GoodsCount > MAX_SHOP_GOODS_COUNT)
	{
		I3PRINTLOG(I3LOG_QA,"Shop Goods data array overflow!!");

		I3ASSERT( ui32GoodsCount <= MAX_SHOP_GOODS_COUNT);
	}			

	UINT32	ui32CurrentGoodsCount;
	UINT32	ui32StartIdx;
	pPacket->ReadData(&ui32CurrentGoodsCount, sizeof(UINT32));
	pPacket->ReadData(&ui32StartIdx,		 sizeof(UINT32));

	pPacket->ReadData( &pGoods[ui32StartIdx],		sizeof(SHOP_GOODS_EXPANSION)*ui32CurrentGoodsCount);

	
	pPacket->ReadData(&ui32Version,			sizeof(UINT32));

#ifdef DEV_BUILD
	FILE* fp = fopen( "recvgoods.txt", (ui32StartIdx == 0) ? "wt" : "at" );
	if (fp)
	{
		for (UINT32 i = ui32StartIdx; i < ui32StartIdx + ui32CurrentGoodsCount; i++)
		{
			fprintf(fp, "[%u] GoodsID:%u, Enable:%u, Point:%u, Cash:%u, SaleType:%u, Mark:%u\n",
				i + 1, pGoods[i]._ui32GoodsID, pGoods[i]._ui8Enable, pGoods[i].GetPoint(), pGoods[i].GetCash(), pGoods[i]._ui8SaleType, pGoods[i].GetMark());
		}
		fclose(fp);
	}
	
#endif
}

void ClientTCPSocket::__Parse_Auth_Shop_RepairList( i3NetworkPacket * pPacket)
{
	CShop* pShop = CShop::i();

	UINT32	ui32RepairCount;
	UINT32	ui32Version;
	SHOP_ITEM_REPAIR_DATA*	pRepair		= pShop->GetRepairAllData();

	pPacket->ReadData(&ui32RepairCount,		sizeof(UINT32));
	pShop->SetRepairCount( ui32RepairCount );

	if( ui32RepairCount > MAX_SHOP_ITEM_COUNT)
	{
		I3PRINTLOG(I3LOG_QA,"Shop Repair data array overflow!!");

		I3ASSERT( ui32RepairCount <= MAX_SHOP_ITEM_COUNT);
	}

	UINT32	ui32CurrentRepairCount;
	UINT32	ui32StartIdx;

	pPacket->ReadData(&ui32CurrentRepairCount, sizeof(UINT32));
	pPacket->ReadData(&ui32StartIdx,		 sizeof(UINT32));
	pPacket->ReadData( &pRepair[ui32StartIdx],		sizeof(SHOP_ITEM_REPAIR_DATA)*ui32CurrentRepairCount);
	pPacket->ReadData(&ui32Version,			sizeof(UINT32));

}

void ClientTCPSocket::__Parse_Auth_Shop_MatchingList( i3NetworkPacket * pPacket)
{
	CShop* pShop = CShop::i();
			
	UINT32	ui32MatchingCount;
	UINT32	ui32Version;
	SHOP_MATCHING_DATA*	pMatching	= pShop->GetMatchingAllData();
	
	pPacket->ReadData(&ui32MatchingCount,	sizeof(UINT32));
	pShop->SetMatchingCount( ui32MatchingCount );

	if( ui32MatchingCount > MAX_SHOP_MATCHING_COUNT)
	{
		I3PRINTLOG(I3LOG_QA,"Shop Matching data array overflow!!");

		I3ASSERT( ui32MatchingCount <= MAX_SHOP_MATCHING_COUNT);
	}	

	UINT32	ui32CurrentMatchingCount;
	UINT32	ui32StartIdx;
	pPacket->ReadData(&ui32CurrentMatchingCount, sizeof(UINT32));
	pPacket->ReadData(&ui32StartIdx,		 sizeof(UINT32));

	pPacket->ReadData( &pMatching[ui32StartIdx],		sizeof(SHOP_MATCHING_DATA)*ui32CurrentMatchingCount);

	pPacket->ReadData(&ui32Version,			sizeof(UINT32));

	// 상품 리스트가 쪼개서 넘어오므로 모두 받았는지 체크한후 최종 리스트를 구성한다.
	if( pShop->GetMatchingCount() <= ui32StartIdx + ui32CurrentMatchingCount)
	{
		pShop->SetGoodsList();

		IGShopContext* igshop = g_pFramework->GetIGShopContext();
		igshop->SetMainShopChange();

		// 버전 변경.
		pShop->SetVersion( ui32Version );

		// 상점 데이터를 파일에 기록합니다.
		if( false == pShop->SaveShopFile() )
		{
			// 파일 저장 실패시 로그 남깁니다.
			I3PRINTLOG(I3LOG_QA, "Error SaveShopFile T_T N");
		}
	}

#ifdef DEV_BUILD
	FILE* fp = fopen( "recvmatching.txt", (ui32StartIdx == 0) ? "wt" : "at" );
	if (fp)
	{
		for (UINT32 i = ui32StartIdx; i < ui32StartIdx + ui32CurrentMatchingCount; i++)
		{
			fprintf(fp, "[%u] GoodsID:%u, ItemID:%u, ItemArg:%u\n",
				i + 1, pMatching[i]._ui32GoodsID, pMatching[i]._ui32ItemID, pMatching[i]._ui32ItemArg);
		}
		fclose(fp);
	}
#endif
}

void ClientTCPSocket::__Parse_Auth_Shop_GiftShop_Enter( i3NetworkPacket * pPacket)
{
	PACKET_AUTH_ENTER_GIFTSHOP_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	UINT8  ui8RecvShopInfo = Recv.m_ui8SendShopInfo;	// 상점 데이터 수신 여부. 0 : 수신X , 1 : 수신O
	UserInfoContext::i()->SetInvenServerTime(Recv.m_ui32InvenTime);

	if (EV_SUCCESSED(Recv.m_i32Result))
	{
		CShop* pShop = CShop::i();
		if (0 == ui8RecvShopInfo && true == pShop->GetFirstShopFileCheck())
		{
			// 상점 데이터를 한번도 수신하지 않았다면 ShopFile로 부터 상점 데이터를 로드합니다.
			pShop->LoadShopFile();
		}
		else if (1 == ui8RecvShopInfo)
		{
			DesigUtil::ClearDesigItemInfosFromServer();
		}

		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ENTER_GIFTSHOP);
	}
	else
	{
		if (Recv.m_i32Result == EVENT_ERROR_GIFT_SHOP_ENTER_RANK_FAIL)
		{
			UINT8 rankValue = Recv.m_ui8GiftShopRank;
			GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_FAIL_ENTER_GIFTSHOP, rankValue);
		}
	}

}

void ClientTCPSocket::__Parse_Auth_Shop_GiftShop_Leave( i3NetworkPacket * pPacket)
{
	PACKET_AUTH_LEAVE_GIFTSHOP_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED(Recv.m_TResult) )
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_LEAVE_GIFTSHOP);
}

void ClientTCPSocket::__Parse_Auth_Shop_GetGiftList( i3NetworkPacket * pPacket)
{
	////	산기슭남 양승촨에게 주는 선물. Recv에 내마음이 담겨있음♥
	PACKET_AUTH_SHOP_GET_GIFTLIST_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if(EV_SUCCESSED(Recv.m_TResult))
	{
		UserInfoContext::i()->SetInvenServerTime(Recv.m_ui32InvenTime);

		if(Recv.m_ui8StartIdx == 0)
			MailBoxContext::i()->NoteGiftFreeAll();

		for( INT32 idx = 0 ; idx < Recv.m_aGift.GetCount() ; idx++ )
		{
			if( Recv.m_ui8StartIdx + idx >= MAX_SHOP_USER_GIFT_COUNT )
			{
				I3PRINTLOG(I3LOG_NOTICE, "Gift Note Count OverFlow" );
				break;
			}

			S2MO_GIFT * pGift = &Recv.m_aGift[idx];

			GIFT_BUFFER	GiftItem;
			GiftItem.Reset();

			GiftItem._i32GiftWareDBIdx	= pGift->m_TDBIdx;
			GiftItem._ui32GoodsID		= pGift->m_TGoodsID;
			GiftItem._eDeliver			= static_cast<ENUM_TYPE_GIFT_DELIVER>(pGift->m_ui8Deliver);
			GiftItem._ui32Expire		= pGift->m_ui32Expire;

			i3::safe_string_copy( GiftItem._strSenderNick,	pGift->m_strSenderNick,	pGift->m_ui8SenderNickSize );
			i3::safe_string_copy( GiftItem._strMessage,		pGift->m_strMessage,	pGift->m_ui8MessageSize );
		
			MailBoxContext::i()->AddGift( GiftItem, pGift->m_ui8SenderNickSize, pGift->m_ui8MessageSize );
		}

		// 모든 선물 리스트를 다받았다면.
		if( ( Recv.m_ui8StartIdx + Recv.m_aGift.GetCount()) >=  Recv.m_ui8TotalGiftCount  ) 
		{
			 GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_GIFT_LIST);
		}
	}
}

void ClientTCPSocket::__Parse_Auth_Shop_AuthGift( i3NetworkPacket * pPacket)
{
	////	선물받기 확인 결과 (확인후 선물이 인벤에 들어온다.)
	PACKET_AUTH_SHOP_AUTH_GIFT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	for( UINT8 i = 0; i < Recv.m_aResult.GetCount(); i++ )
	{
		MailBoxContext::i()->SetGiftReqResult(Recv.m_ui8Type, &Recv.m_aResult[i]);		
	}

	MailBoxContext::i()->IncreaseResultCount(Recv.m_aResult.GetCount());
	MailBoxContext::i()->SetNoteRefreshFlag();

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_AUTH_GIFT_GOODS);
}

void ClientTCPSocket::__Parse_Auth_Shop_SendGift( i3NetworkPacket * pPacket)
{

	PACKET_AUTH_SHOP_GOODS_GIFT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	UserInfoContext::i()->SetInvenServerTime(Recv.m_ui32InvenTime);
	UserInfoContext::i()->SetMyPoint(Recv.m_ui32Point);
	UserInfoContext::i()->SetMyCash(Recv.m_ui32Cash);

	i3::user_data ud = i3::user_data(RecvData_EVENT_SEND_GIFT());
	RecvData_EVENT_SEND_GIFT& data = i3::value_cast<RecvData_EVENT_SEND_GIFT>(ud);

	const INT32 numResult = Recv.m_aResult.GetCount();
	data.resultList.resize(numResult);

	for (INT32 i = 0; i < numResult; ++i)
	{
		data.resultList[i] = Recv.m_aResult[i].m_i32Rv;
	}

	GameEventReceiver::i()->PushReceivedGameEventUserData(EVENT_SEND_GIFT, ud, data.resultList[0]);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REFRESH_CASH);

}

void ClientTCPSocket::__Parse_Auth_Shop_RecvGift( i3NetworkPacket * pPacket)
{	
	// 실시간 선물 지급 정보 받기. 
	PACKET_AUTH_SHOP_RECV_GIFT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	UserInfoContext::i()->SetInvenServerTime(Recv.m_ui32InvenTime);

	char strErrorLog[MAX_PATH];
	GIFT_BUFFER GiftItem;

	GiftItem._eDeliver = TYPE_GIFT_DELIVER_NORMAL;
	GiftItem._ui32Expire = Recv.m_ui32Expire;
	i3::safe_string_copy( GiftItem._strSenderNick,	&Recv.m_strSenderNick,	MAX_USER_NICK );
	i3::safe_string_copy( GiftItem._strMessage,		&Recv.m_strMessage,		MAX_GIFT_MESSAGE_SIZE );
	
	// Login Event Popup
	i3::wstring strmessage;
	i3::sprintf( strmessage, L"%s", &Recv.m_strMessage);
	if(i3::generic_is_equal(strmessage, L"EVENT_10"))
	{
		//GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS"));
		MailBoxContext::i()->setRecvLoginEvent(true);
	}

	for( UINT8 i = 0; i < Recv.m_aGift.GetCount() ; i++ )
	{
		// 실패 로그 기록합니다.
		if( EV_FAILED( Recv.m_aGift[i].m_i32Rv ) )
		{
			i3::snprintf( strErrorLog, MAX_PATH, "NGF[%d] : %x %d ", i, Recv.m_aGift[i].m_i32Rv, Recv.m_aGift[i].m_ui32GoodsID);
			I3PRINTLOG(I3LOG_QA, strErrorLog );
			continue;
		}

		GiftItem._i32GiftWareDBIdx	= Recv.m_aGift[i].m_TGiftDBIdx;
		GiftItem._ui32GoodsID		= Recv.m_aGift[i].m_ui32GoodsID;

		MailBoxContext::i()->AddGift( GiftItem, (UINT8) Recv.m_strSenderNick.GetCount(), (UINT8) Recv.m_strMessage.GetCount() );

		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RECEIVE_GIFT);
	}
}

void ClientTCPSocket::__Parse_Auth_Shop_Use_GiftCoupon( i3NetworkPacket * pPacket)
{	
	PACKET_AUTH_SHOP_USE_COUPON_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_COUPON_AWARD, Recv.m_TResult);
}

void ClientTCPSocket::__Parse_Auth_Shop_Item_Change(i3NetworkPacket * pPacket)
{
	INT32 result = 0;
	pPacket->ReadData(&result, sizeof(INT32));

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ITEM_AUTH_DATA_CHAGNE); 
}