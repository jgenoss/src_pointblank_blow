#ifndef __S2_MO_STRUCT_SHOP_H__
#define __S2_MO_STRUCT_SHOP_H__

struct PACKET_SHOP_LEAVE_REQ : public S2MOPacketBaseT<PROTOCOL_SHOP_LEAVE_REQ>
{
};

struct PACKET_SHOP_LEAVE_ACK : public S2MOPacketBaseResultT<PROTOCOL_SHOP_LEAVE_ACK>
{
};

struct PACKET_SHOP_PLUS_POINT_ACK : public S2MOPacketBaseT<PROTOCOL_SHOP_PLUS_POINT_ACK>
{
	S2MOValue<UINT32>												m_ui32GetPoint;
	S2MOValue<UINT32>												m_ui32Point;
	S2MOValue<UINT8>												m_ui8GetRoute;

	PACKET_SHOP_PLUS_POINT_ACK()
	{
		_SetValue( m_ui32GetPoint );
		_SetValue( m_ui32Point );
		_SetValue( m_ui8GetRoute );
	}
};

struct PACKET_SHOP_BUY_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK>
{
	S2MOValue<UINT32>												m_ui32InvenTime;
	S2MOValue<UINT32>												m_ui32Point;
	S2MOValue<UINT32>												m_ui32Cash;
	S2MOValue<S2MO_SHOP_BUY_RESULT, MAX_SHOP_GOODS_BUY_COUNT>		m_aBuyResult;

	PACKET_SHOP_BUY_ACK()
	{
		_SetValue( m_ui32InvenTime );
		_SetValue( m_ui32Point );
		_SetValue( m_ui32Cash );
		_SetValue( m_aBuyResult );
	}
};

struct PACKET_SHOP_REPAIR_ACK : public S2MOPacketBaseT<PROTOCOL_SHOP_REPAIR_ACK>
{
	S2MOValue<UINT32>												m_ui32Point;
	S2MOValue<UINT32>												m_ui32Cash;
	S2MOValue<S2MO_SHOP_REPAIR_RESULT, MAX_SHOP_GOODS_BUY_COUNT>	m_aRepairResult;

	PACKET_SHOP_REPAIR_ACK()
	{
		_SetValue( m_ui32Point );
		_SetValue( m_ui32Cash );
		_SetValue( m_aRepairResult );
	}
};

struct PACKET_AUTH_SHOP_CAPSULE_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_CAPSULE_ACK>
{
	S2MOValue<UINT32>												m_ui32ItemID;
	S2MOValue<UINT8>												m_ui8Capsule;
	S2MOValue<ST_ITEM, CAPSULE_REWARD_ITEM_COUNT>					m_stItem;

	PACKET_AUTH_SHOP_CAPSULE_ACK()
	{
		_SetValue( m_ui32ItemID );
		_SetValue( m_ui8Capsule );
		_SetValue( m_stItem );
	}
};

struct PACKET_AUTH_SHOP_JACKPOT_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_JACKPOT_ACK>
{
	S2MOStringW<NET_NICK_NAME_SIZE>									m_strNickName;		
	S2MOValue<UINT32>												m_ui32ItemID;	
	S2MOValue<UINT8>												m_ui8Capsule;

	PACKET_AUTH_SHOP_JACKPOT_ACK()
	{
		_SetValue( m_strNickName );
		_SetValue( m_ui32ItemID );
		_SetValue( m_ui8Capsule );		
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////	GIFT	///////////////////////////////////////////////////////////////////////////////////////////

// 선물함 리스트 요청
struct PACKET_AUTH_SHOP_GET_GIFTLIST_REQ : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_GET_GIFTLIST_REQ>
{
};

// 선물함 리스트 응답. 패킷 n개.
struct PACKET_AUTH_SHOP_GET_GIFTLIST_ACK : public S2MOPacketBaseResultT<PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK>
{
	S2MOValue<UINT32>											m_ui32InvenTime;						// 서버 시간.
	S2MOValue<UINT8>											m_ui8TotalGiftCount;					// 총 선물 갯수
	S2MOValue<UINT8>											m_ui8StartIdx;							// 선물 배열 시작 인덱스.
	S2MOValue<S2MO_GIFT, MAX_SHOP_USER_GIFT_COUNT>				m_aGift;								// 선물 정보

	PACKET_AUTH_SHOP_GET_GIFTLIST_ACK()
	{
		// Last Update : 2015-10-13 16:36:55
		_SetValue( m_ui32InvenTime );
		_SetValue( m_ui8TotalGiftCount );
		_SetValue( m_ui8StartIdx );
		_SetValue( m_aGift );
	}
};


// 선물 구입(보내기) 요청. ( 선물 보낸 사람 )
struct PACKET_AUTH_SHOP_GOODS_GIFT_REQ : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_GOODS_GIFT_REQ>
{
	S2MOStringW<NET_NICK_NAME_SIZE>								m_strReciverNick;						// 받을 유저 닉네임
	S2MOStringW<MAX_GIFT_MESSAGE_SIZE>							m_strMessage;							// 선물 메시지
	S2MOValue<BUY_BASKET, MAX_SHOP_GOODS_BUY_COUNT>				m_aBuyGift;								// 구입 선물 정보.

	PACKET_AUTH_SHOP_GOODS_GIFT_REQ()
	{
		// Last Update : 2015-10-13 15:29:40
		_SetValue( m_strReciverNick );
		_SetValue( m_strMessage );
		_SetValue( m_aBuyGift );
	}
};



// 선물 구입(보내기) 응답. ( 선물 보낸 사람 )
struct PACKET_AUTH_SHOP_GOODS_GIFT_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK>
{
	S2MOValue<UINT32>											m_ui32InvenTime;						// 서버시간
	S2MOValue<UINT32>											m_ui32Point;							// 포인트 잔액.
	S2MOValue<UINT32>											m_ui32Cash;								// 캐쉬 잔액.
	S2MOValue<S2MO_SHOP_BUY_RESULT, MAX_SHOP_GOODS_BUY_COUNT>	m_aResult;								// 구입 결과.

	PACKET_AUTH_SHOP_GOODS_GIFT_ACK()
	{
		// Last Update : 2015-10-13 15:25:50
		_SetValue( m_ui32InvenTime );
		_SetValue( m_ui32Point );
		_SetValue( m_ui32Cash );
		_SetValue( m_aResult );
	}
};

// 선물인증 요청. 
struct PACKET_AUTH_SHOP_AUTH_GIFT_REQ : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_AUTH_GIFT_REQ>
{
	S2MOValue<UINT8>											m_ui8Type;								// 인증 방식. ENUM_TYPE_SHOP
	S2MOValue<T_GiftDBIdx, MAX_SHOP_GOODS_BUY_COUNT>			m_aGiftIdx;								// 인증할 선물 DB Idx

	PACKET_AUTH_SHOP_AUTH_GIFT_REQ()
	{
		// Last Update : 2015-10-13 14:34:52
		_SetValue( m_ui8Type );
		_SetValue( m_aGiftIdx );
	}
};

// 선물인증 응답.
struct PACKET_AUTH_SHOP_AUTH_GIFT_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK>
{
	S2MOValue<UINT8>											m_ui8Type;								// 인증 방식. ENUM_TYPE_SHOP
	S2MOValue<AUTH_GIFT, MAX_SHOP_GOODS_BUY_COUNT>				m_aResult;

	PACKET_AUTH_SHOP_AUTH_GIFT_ACK()
	{
		// Last Update : 2015-10-13 14:06:38
		_SetValue( m_ui8Type );
		_SetValue( m_aResult );
	}
};

// 선물 받음 알림.
struct PACKET_AUTH_SHOP_RECV_GIFT_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_RECV_GIFT_ACK>
{
	S2MOValue<UINT32>											m_ui32InvenTime;						// 서버시간
	S2MOValue<UINT32>											m_ui32Expire;							//선물 만료날짜
	S2MOStringW<NET_NICK_NAME_SIZE>								m_strSenderNick;						//송신사 닉네임.
	S2MOStringW<MAX_GIFT_MESSAGE_SIZE>							m_strMessage;							//선물메시지.
	S2MOValue<S2MO_RECV_GIFT, MAX_SHOP_GOODS_BUY_COUNT>			m_aGift;								// 받은 선물 정보

	PACKET_AUTH_SHOP_RECV_GIFT_ACK()
	{
		// Last Update : 2015-10-13 13:45:40
		_SetValue( m_ui32InvenTime );
		_SetValue( m_ui32Expire );
		_SetValue( m_strSenderNick );
		_SetValue( m_strMessage );
		_SetValue( m_aGift );
	}
};


struct PACKET_AUTH_ENTER_GIFTSHOP_REQ : public S2MOPacketBaseT<PROTOCOL_AUTH_ENTER_GIFTSHOP_REQ>
{
	S2MOValue<char, NET_MD5_KEY_SIZE+1>							m_strShopMD5;

	PACKET_AUTH_ENTER_GIFTSHOP_REQ()
	{
		// Last Update : 2015-10-13 13:41:41
		_SetValue( m_strShopMD5 );
	}
};

struct PACKET_AUTH_ENTER_GIFTSHOP_ACK : public S2MOPacketBaseT<PROTOCOL_AUTH_ENTER_GIFTSHOP_ACK>
{
	S2MOValue<UINT8>											m_ui8SendShopInfo;	// 상점 데이터 전송 여부. 0 : 전송X , 1 : 전송O
	S2MOValue<UINT32>											m_ui32InvenTime;	// 서버 시간.
	S2MOValue<UINT8>											m_ui8GiftShopRank;	//랭크제한
	S2MOValue<INT32>											m_i32Result;		//랭크제한

	PACKET_AUTH_ENTER_GIFTSHOP_ACK()
	{
		// Last Update : 2015-10-13 13:40:14
		_SetValue( m_ui8SendShopInfo );
		_SetValue( m_ui32InvenTime );
		_SetValue( m_ui8GiftShopRank );
		_SetValue( m_i32Result);
	}
};

struct PACKET_AUTH_LEAVE_GIFTSHOP_REQ : public S2MOPacketBaseT<PROTOCOL_AUTH_LEAVE_GIFTSHOP_REQ>
{
};

struct PACKET_AUTH_LEAVE_GIFTSHOP_ACK : public S2MOPacketBaseResultT<PROTOCOL_AUTH_LEAVE_GIFTSHOP_ACK>
{
};

struct PACKET_AUTH_SHOP_USE_COUPON_REQ : public S2MOPacketBaseT<PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_REQ>
{
	S2MOStringA<GIFTCOUPON_LENGTH + 1>							m_strCouponCode;

	PACKET_AUTH_SHOP_USE_COUPON_REQ()
	{
		_SetValue( m_strCouponCode );
	}
};

struct PACKET_AUTH_SHOP_USE_COUPON_ACK : public S2MOPacketBaseResultT<PROTOCOL_AUTH_SHOP_USE_GIFTCOUPON_ACK>
{
};

#endif