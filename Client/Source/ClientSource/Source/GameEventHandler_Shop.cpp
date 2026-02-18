#include "pch.h"
#include "GameEventHandler.h"
#include "Shop.h"
#include "GachaContext.h"
#include "UI/CBFuction.h"
#include "UI/PurchaseMgr.h"
#include "UI/BuyUtil.h"
#include "UI/UIShopFunction.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/utf8_to_utf16.h"


const char * CGameEventHandler::__getShopEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_ITEM_AUTH					: return "EVENT_ITEM_AUTH";
	case EVENT_ITEM_AUTH_DATA				: return "EVENT_ITEM_AUTH_DATA";
	case EVENT_AUTH_DELETE					: return "EVENT_AUTH_DELETE";
	case EVENT_USE_ITEM_CHECK_NICK			: return "EVENT_USE_ITEM_CHECK_NICK";
	case EVENT_VIEW_OTHER_USER_ITEM_INFO	: return "EVENT_VIEW_OTHER_USER_ITEM_INFO";
	case EVENT_REQUEST_GOODS_LIST			: return "EVENT_REQUEST_GOODS_LIST";
	case EVENT_BUY_GOODS					: return "EVENT_BUY_GOODS";
	case EVENT_BUY_CHAR_GOODS				: return "EVENT_BUY_CHAR_GOODS";
	case EVENT_BUY_EXTEND_GOODS				: return "EVENT_BUY_EXTEND_GOODS";
	case EVENT_ITEM_REPAIR					: return "EVENT_ITEM_REPAIR";
	case EVENT_GET_REMAIN_MONEY				: return "EVENT_GET_REMAIN_MONEY";
	case EVENT_COUPON_AWARD					: return "EVENT_COUPON_AWARD";
	case EVENT_ENTER_GACHA					: return "EVENT_ENTER_GACHA";
	case EVENT_LEAVE_GACHA					: return "EVENT_LEAVE_GACHA";
	case EVENT_ENTER_GACHA_INFO				: return "EVENT_ENTER_GACHA_INFO";
	case EVENT_START_GACHA					: return "EVENT_START_GACHA";
	case EVENT_ENTER_GIFTSHOP				: return "EVENT_ENTER_GIFTSHOP";
	case EVENT_LEAVE_GIFTSHOP				: return "EVENT_LEAVE_GIFTSHOP";
	case EVENT_GET_GIFT_LIST				: return "EVENT_GET_GIFT_LIST";
	case EVENT_SEND_GIFT					: return "EVENT_SEND_GIFT";
	case EVENT_AUTH_GIFT_GOODS				: return "EVENT_AUTH_GIFT_GOODS";
	case EVENT_GACHA_STATE_REQ				: return "EVENT_GACHA_STATE_REQ";
	case EVENT_ITEM_AUTH_DATA_CHAGNE		: return "EVENT_ITEM_AUTH_DATA_CHAGNE";
	default:				return nullptr;
	}
}

void CGameEventHandler::__register_Shop( void)
{
	regSendEvent( EVENT_ITEM_AUTH,					&CGameEventHandler::__evtShop_AuthItem);
	regSendEvent( EVENT_ITEM_AUTH_DATA,				&CGameEventHandler::__evtShop_AuthItemData);
	regSendEvent( EVENT_AUTH_DELETE,				&CGameEventHandler::__evtShop_DeleteAuth);
	regSendEvent( EVENT_USE_ITEM_CHECK_NICK,		&CGameEventHandler::__evtShop_CheckNickForUseChangeNickItem);
	regSendEvent( EVENT_VIEW_OTHER_USER_ITEM_INFO,	&CGameEventHandler::__evtShop_AuthItemData);	// 패킷 안보내고 EVENT_ITEM_AUTH_DATA로 보내도록함.
	regSendEvent( EVENT_REQUEST_GOODS_LIST,			&CGameEventHandler::__evtShop_RequestGoodsList);
	regSendEvent( EVENT_BUY_GOODS,					&CGameEventHandler::__evtShop_BuyGoods);
	regSendEvent( EVENT_BUY_CHAR_GOODS,				&CGameEventHandler::__evtShop_BuyCharGoods, new PACKET_CHAR_CREATE_CHARA_REQ);	
	regSendEvent( EVENT_BUY_EXTEND_GOODS,			&CGameEventHandler::__evtShop_BuyExtendGoods);
	regSendEvent( EVENT_ITEM_REPAIR,				&CGameEventHandler::__evtShop_RepairItem);
	regSendEvent( EVENT_GET_REMAIN_MONEY,			&CGameEventHandler::__evtShop_GetRemainMoney);
	regSendEvent( EVENT_COUPON_AWARD,				&CGameEventHandler::__evtShop_AwardCoupon, new PACKET_AUTH_SHOP_USE_COUPON_REQ);

	regSendEvent( EVENT_ENTER_GACHA,				&CGameEventHandler::__evtGacha_Enter);
	regSendEvent( EVENT_LEAVE_GACHA,				&CGameEventHandler::__evtGacha_Leave);
	regSendEvent( EVENT_ENTER_GACHA_INFO,			&CGameEventHandler::__evtGacha_ItemInfo);
	regSendEvent( EVENT_START_GACHA,				&CGameEventHandler::__evtGacha_Start, new PACKET_GACHA_PURCHASE_REQ );
	regSendEvent( EVENT_GACHA_STATE_REQ,			&CGameEventHandler::__evtGacha_State_Req);
	regSendEvent( EVENT_GACHA_PURCHASE_COUNT,		&CGameEventHandler::__evtGacha_Purchase_count);

	regSendEvent( EVENT_ENTER_GIFTSHOP,				&CGameEventHandler::__evtGiftShop_Enter);
	regSendEvent( EVENT_LEAVE_GIFTSHOP,				&CGameEventHandler::__evtGiftShop_Leave);
	regSendEvent( EVENT_FAIL_ENTER_GIFTSHOP,		&CGameEventHandler::__evtGiftShop_GetRecvList);
	regSendEvent( EVENT_GET_GIFT_LIST,				&CGameEventHandler::__evtGiftShop_GetRecvList);
	regSendEvent( EVENT_SEND_GIFT,					&CGameEventHandler::__evtGiftShop_SendGoods, new PACKET_AUTH_SHOP_GOODS_GIFT_REQ );
	regSendEvent( EVENT_AUTH_GIFT_GOODS,			&CGameEventHandler::__evtGiftShop_AuthGoods, new PACKET_AUTH_SHOP_AUTH_GIFT_REQ );
	regSendEvent( EVENT_ITEM_AUTH_DATA_CHAGNE,		&CGameEventHandler::__evtShop_ChangeItemData);
}

bool CGameEventHandler::__evtShop_AuthItem( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ);
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(T_ItemDBIdx));
	return true;
}

bool CGameEventHandler::__evtShop_AuthItemData( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_ItemDBIdx));

	// 2,3번째 인수들은 이것저것 타입이 잡다하게 처리되는 상태이므로, 여기서 변환하지말고, 좀더 앞에서 변환시킨다. ( 2014.08.22.수빈)

	pInfo->_SendPacket.WriteData(pInfo->_pArg1, sizeof(UINT8));
	UINT8 i32Size = *((UINT8*)pInfo->_pArg1);
	if( 0 < i32Size )
	{
		pInfo->_SendPacket.WriteData(pInfo->_pArg2, i32Size);
	}
	return true;
}

bool CGameEventHandler::__evtShop_ChangeItemData(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_CHANGE_DATA_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_ItemDBIdx));

	pInfo->_SendPacket.WriteData(pInfo->_pArg1, sizeof(UINT8));
	UINT8 i32Size = *((UINT8*)pInfo->_pArg1);
	if (0 < i32Size)
	{
		pInfo->_SendPacket.WriteData(pInfo->_pArg2, i32Size);
	}
	return true;
}

bool CGameEventHandler::__evtShop_DeleteAuth( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SHOP_DELETE_ITEM_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(T_ItemDBIdx));
	return true;
}

bool CGameEventHandler::__evtShop_CheckNickForUseChangeNickItem( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_USE_ITEM_CHECK_NICK_REQ);

	const wchar_t* wstr = (const wchar_t*)pInfo->_pArg;


	TTCHAR szNick[NET_NICK_NAME_SIZE];	::memset(szNick, 0, sizeof(szNick));

	i3::safe_string_copy(szNick, wstr, NET_NICK_NAME_SIZE);

	pInfo->_SendPacket.WriteData(szNick, NET_NICK_NAME_SIZE * sizeof(TTCHAR));
	return true;
}

bool CGameEventHandler::__evtShop_RequestGoodsList( GAMEEVENTINFO * pInfo)
{
	char strMD5Key[NET_MD5_KEY_SIZE+1];
	i3::safe_string_copy( strMD5Key, CShop::i()->GetShopMD5Key(), NET_MD5_KEY_SIZE+1 );
	strMD5Key[NET_MD5_KEY_SIZE] = 0;


	pInfo->_SendPacket.SetProtocol(PROTOCOL_SHOP_GET_SAILLIST_REQ);
	pInfo->_SendPacket.WriteData(strMD5Key,			sizeof(char) * NET_MD5_KEY_SIZE ); 
	return true;
}

bool CGameEventHandler::__evtShop_BuyGoods( GAMEEVENTINFO * pInfo)
{
	i3NetworkPacket * pPacket = &pInfo->_SendPacket;

	//캐릭터 구매는 즉시 구입이므로 0번 인덱스로 하면 된다.
	pPacket->SetProtocol(PROTOCOL_AUTH_SHOP_GOODS_BUY_REQ);	//변경 한다는 패킷 전송

	const buy::DisplayedGoodsList& goodsList = PurchaseMgr::i()->GetPurchaseGoodsList();

	PACKET_H_AUTH_NEW_SHOP_GOODS_BUY_REQ header;
	header.m_ui8GoodsCount = (UINT8)goodsList.size();
	
	pPacket->WriteData(&header, sizeof(PACKET_H_AUTH_NEW_SHOP_GOODS_BUY_REQ));

	PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_BASKET body[USE_BASKET_COUNT];
	memset(body, 0, USE_BASKET_COUNT * sizeof(PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_BASKET));

	for (size_t i = 0; i < header.m_ui8GoodsCount; ++i)
	{
		const buy::DisplayedGoodsPtr& goods = goodsList[i];

		BUY_BASKET & basket = body[i].m_Basket;

		const SHOP_GOODS* pCurrGood = goods->GetCurrentGoods();

		basket.m_ui32GoodsID = pCurrGood->_GoodsID;
		basket.m_ui8BuyType = (UINT8)(goods->GetCurrentPriceType());	//	item.price_type;

#if defined(USE_SALE_COUPON)
		//T_ItemDBIdx n64CouponDBIdx = GetCouponIndex(i);
		//// GetCouponIndex 메서드로, 빈 값은 -1이 설정되기 때문에, 0으로 치환해준다.
		//if(n64CouponDBIdx <= 0)
			//	n64CouponDBIdx = 0;
		//body[ i ].m_Basket.m_ui32CouponID = CInvenList::i()->FindItemIDByDBIdx( INVEN::DB_COUPON, n64CouponDBIdx);
		//body[ i ].m_Basket.m_i64CouponIdx = n64CouponDBIdx;
#endif
	}
	pPacket->WriteData(&body, sizeof(PACKET_D_AUTH_NEW_SHOP_GOODS_BUY_BASKET) * header.m_ui8GoodsCount);

	
	return (pInfo->_SendPacket.GetReceivedSize()>0)?true:false;
}


//
// 캐릭터는 장바구니나 미리보기 구매가 안된다고 합니다...단일 구매만 고려하기로 합니다...
//

bool CGameEventHandler::__evtShop_BuyCharGoods( GAMEEVENTINFO * pInfo)
{
	S2MOPacketBase * pS2MOPacket = pInfo->m_S2MOPacket;
	PACKET_CHAR_CREATE_CHARA_REQ* pSend = static_cast<PACKET_CHAR_CREATE_CHARA_REQ*>(pS2MOPacket);

	const buy::DisplayedGoodsPtr& goods = PurchaseMgr::i()->GetPurchaseGoodsList()[0];
	
	T_ItemID item_id = goods->GetCurrentGoods()->_ItemID;

	pSend->m_TGoodsID = goods->GetCurrentGoods()->_GoodsID;
	pSend->m_ui8CreateType = (UINT8)goods->GetCurrentPriceType();
	pSend->m_ui8Class = CHARA_CLASS_NONE;

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, item_id);

	wchar_t wstrCharNick[NET_CHARA_NICK_NAME_SIZE];
	i3::safe_string_copy(wstrCharNick, wstrItemName, NET_CHARA_NICK_NAME_SIZE);
	pSend->m_strCharaName = wstrCharNick;
	
	return true;
}

bool CGameEventHandler::__evtShop_BuyExtendGoods( GAMEEVENTINFO * pInfo)
{

	i3NetworkPacket * pPacket = &pInfo->_SendPacket;

	pPacket->SetProtocol(PROTOCOL_AUTH_SHOP_ITEM_EXTEND_REQ);	//변경 한다는 패킷 전송

	PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ body[USE_BASKET_COUNT];
	memset(body, 0, USE_BASKET_COUNT * sizeof(PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ));

	const buy::DisplayedGoodsList& goodsList = PurchaseMgr::i()->GetPurchaseGoodsList();

	const size_t size = goodsList.size();
	for (size_t i = 0; i < size; ++i)
	{
		const buy::DisplayedGoodsPtr& goods = goodsList[i];

		BUY_BASKET & basket = body[i].m_ExtendItemData.m_Basket;
		basket.m_ui32GoodsID = goods->GetCurrentGoods()->_GoodsID;
		basket.m_ui8BuyType = (UINT8)goods->GetCurrentPriceType();
		I3ASSERT(goods->IsItemDBIndexValid());
		body[i].m_ExtendItemData.m_TWareDBIdx = goods->GetItemDBIndex();

#if defined(USE_SALE_COUPON)
		//T_ItemDBIdx n64CouponDBIdx = GetCouponIndex(i);

		// GetCouponIndex 메서드로, 빈 값은 -1이 설정되기 때문에, 0으로 치환해준다.
		//if(n64CouponDBIdx <= 0)
		//	n64CouponDBIdx = 0;

		//body[ i ].m_Basket.m_ui32CouponID = pInvenList->FindItemIDByDBIdx( INVEN::DB_COUPON, n64CouponDBIdx);
		//body[ i ].m_Basket.m_i64CouponIdx = n64CouponDBIdx;
#endif
	}

	pPacket->WriteData(&body, (UINT16)(sizeof(PACKET_AUTH_NEW_SHOP_ITEM_EXTEND_REQ) * size));
	
	return (pInfo->_SendPacket.GetReceivedSize()>0)?true:false;
}

bool CGameEventHandler::__evtShop_RepairItem( GAMEEVENTINFO * pInfo)
{
	T_ItemDBIdx  TItemDBIdx = (T_ItemDBIdx)(*(T_ItemDBIdx*)(pInfo->_pArg));

	UINT8 ui8CharaSlotIdx	= 0;	// 인벤토리의 특정 아이템 수리 이므로 의미 없습니다.
	UINT8 ui8RepairType		= (UINT8)S2MO_REPAIR_TYPE_ONE;

	pInfo->_SendPacket.SetProtocol( PROTOCOL_SHOP_REPAIR_REQ );
	pInfo->_SendPacket.WriteData( &ui8RepairType,	sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( &ui8CharaSlotIdx, sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( &TItemDBIdx,		sizeof(T_ItemDBIdx) );


	return true;
}

bool CGameEventHandler::__evtShop_GetRemainMoney( GAMEEVENTINFO * pInfo)
{
	// revision 62280 (Hansoft 5530) 유저 캐쉬 정보 요청을 계속하는 버그 수정
	if ( m_ReqMoneyTime + 10.f < g_pFramework->GetAccTime())
	{
		pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_GET_POINT_CASH_REQ);
		m_ReqMoneyTime = g_pFramework->GetAccTime();
		return true;
	}

	return false;
}

bool CGameEventHandler::__evtShop_AwardCoupon( GAMEEVENTINFO * pInfo)
{
	char strCoupon[GIFTCOUPON_LENGTH + 1];
	memcpy(strCoupon, pInfo->_pArg, GIFTCOUPON_LENGTH + 1);

	PACKET_AUTH_SHOP_USE_COUPON_REQ* pSend = static_cast<PACKET_AUTH_SHOP_USE_COUPON_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_strCouponCode = strCoupon;
	
	return true;
}

bool CGameEventHandler::__evtGiftShop_Enter( GAMEEVENTINFO * pInfo)
{
	char strMD5Key[NET_MD5_KEY_SIZE+1];

	i3::safe_string_copy( strMD5Key, CShop::i()->GetShopMD5Key(), NET_MD5_KEY_SIZE+1 );
	strMD5Key[NET_MD5_KEY_SIZE] = 0;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_ENTER_GIFTSHOP_REQ);
	pInfo->_SendPacket.WriteData( strMD5Key,		sizeof(char) * NET_MD5_KEY_SIZE ); 

	return true;
}

bool CGameEventHandler::__evtGiftShop_Leave( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_LEAVE_GIFTSHOP_REQ);
	return true;
}

bool CGameEventHandler::__evtGiftShop_GetRecvList( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ);
	return true;
}

bool CGameEventHandler::__evtGiftShop_SendGoods( GAMEEVENTINFO * pInfo)
{
	PACKET_AUTH_SHOP_GOODS_GIFT_REQ* pSend = static_cast<PACKET_AUTH_SHOP_GOODS_GIFT_REQ*>(pInfo->m_S2MOPacket);

	const buy::GiftInfo& giftInfo = PurchaseMgr::i()->GetGiftInfo();

	const i3::rc_wstring& str_receiver_nick = giftInfo.wstr_receiver_nick;
	wchar_t wstrReciverNick[NET_NICK_NAME_SIZE];
	i3::safe_string_copy(wstrReciverNick, str_receiver_nick, NET_NICK_NAME_SIZE);
	pSend->m_strReciverNick = wstrReciverNick;

	const i3::rc_wstring& str_gift_message = giftInfo.wstr_gift_message;
	wchar_t wstrMessage[NET_NICK_NAME_SIZE];
	i3::safe_string_copy(wstrMessage, str_gift_message, MAX_GIFT_MESSAGE_SIZE);
	pSend->m_strMessage = wstrMessage;

	const buy::DisplayedGoodsList& goodsList = PurchaseMgr::i()->GetPurchaseGoodsList();

	const size_t numGoods = goodsList.size();

	for (size_t i = 0; i < numGoods; ++i)
	{
		const buy::DisplayedGoodsPtr& goods = goodsList[i];
		BUY_BASKET & basket = pSend->m_aBuyGift[i];

		basket.m_ui32GoodsID = goods->GetCurrentGoods()->_GoodsID;
		basket.m_ui8BuyType = (UINT8)goods->GetCurrentPriceType();
	}


	return true;
}

bool CGameEventHandler::__evtGiftShop_AuthGoods( GAMEEVENTINFO * pInfo)
{
	const UINT8 ui8Type			= *(reinterpret_cast<const UINT8*>(pInfo->_pArg));
	const UINT8 ui8Count		= *(reinterpret_cast<const UINT8*>(pInfo->_pArg1));
	const T_GiftDBIdx* pDBIdx	= reinterpret_cast<const T_GiftDBIdx*>(pInfo->_pArg2);
		
	PACKET_AUTH_SHOP_AUTH_GIFT_REQ* pSend = static_cast<PACKET_AUTH_SHOP_AUTH_GIFT_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_ui8Type	= ui8Type;
	pSend->m_aGiftIdx.SetValue( pDBIdx, 0, ui8Count );

	return true;
}

bool CGameEventHandler::__evtGacha_ItemInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_ITEM_INFO_REQ);

	char strMD5Key[NET_MD5_KEY_SIZE+1];
	memcpy( strMD5Key, GachaContext::i()->GetGachaShopMD5Key(), NET_MD5_KEY_SIZE+1 );
	strMD5Key[NET_MD5_KEY_SIZE] = 0;

	pInfo->_SendPacket.WriteData( strMD5Key,		sizeof(char) * NET_MD5_KEY_SIZE ); 

	return true;
}

bool CGameEventHandler::__evtGacha_Enter( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_ENTER_REQ);

	return true;
}

bool CGameEventHandler::__evtGacha_Leave( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_LEAVE_REQ);
	return true;
}

bool CGameEventHandler::__evtGacha_Start( GAMEEVENTINFO * pInfo)
{
	PACKET_GACHA_PURCHASE_REQ* pSend = static_cast<PACKET_GACHA_PURCHASE_REQ*>(pInfo->m_S2MOPacket);
	pSend->m_ui8BuyType = *((UINT8*)pInfo->_pArg);
	pSend->m_TGoodsID	= *((UINT32*)pInfo->_pArg1);
	pSend->m_i32ItemIdx	= *((INT32*)pInfo->_pArg2);

	return true;
}

bool CGameEventHandler::__evtGacha_State_Req( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_SHOP_STATE_REQ);

	return true;
}

bool CGameEventHandler::__evtGacha_Purchase_count( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_GET_PURCHASE_COUNT_REQ);

	return true;
}
