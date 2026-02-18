#ifndef __S2_MO_STRUCT_GACHA_H__
#define __S2_MO_STRUCT_GACHA_H__

#include "../Gacha/GachaDefine.h"

struct PACKET_GACHA_ENTER_ACK : public S2MOPacketBaseT<PROTOCOL_GACHA_ENTER_ACK>
{
	S2MOValue<INT32>								m_i32ErrorCode;
	S2MOValue<T_GoodsID>							m_ui32LuckyGoodsID;							// ÀèÆÌ GoodsID
	S2MOStringW<NET_NICK_NAME_SIZE>					m_strLuckyUserNick;							// ÀèÆÌ ´çÃ· À¯Àú ´Ð³×ÀÓ
	S2MOValue<UINT32>								m_ui32LuckyTime;							// ÀèÆÌ ´çÃ· ½Ã°£. YYMMDDHHmm

	PACKET_GACHA_ENTER_ACK()
	{
		// Last Update : 2015-10-12 14:26:31
		_SetValue( m_i32ErrorCode );
		_SetValue( m_ui32LuckyGoodsID );
		_SetValue( m_strLuckyUserNick );
		_SetValue( m_ui32LuckyTime );
	}
};

struct PACKET_GACHA_PURCHASE_REQ : public S2MOPacketBaseT<PROTOCOL_GACHA_PURCHASE_REQ>
{
	S2MOValue<UINT8>								m_ui8BuyType;					// °áÁ¦ À¯Çü. SIA_GOODS_BUY		
	S2MOValue<INT32>								m_i32ItemIdx;					// ¹è¿­ ÀÎµ¦½º.
	S2MOValue<T_GoodsID>							m_TGoodsID;						// Gacha GoodsID. CShop::GetGachaGoodsID() ÇÔ¼ö·Î °Ë»ö. 
																					// ±âÁ¸ÀÇ m_eGachaGroup ,m_i32ItemKey, m_i32Amount(SHOP_MATCHING_DATAÀÇ _ui32ItemArg)°¡ Á¶ÇÕµÈ ÇüÅÂ.

	PACKET_GACHA_PURCHASE_REQ()
	{
		// Last Update : 2015-10-12 14:28:22
		_SetValue( m_ui8BuyType );
		_SetValue( m_i32ItemIdx );
		_SetValue( m_TGoodsID );
	}
};

struct PACKET_GACHA_PURCHASE_ACK : public S2MOPacketBaseT<PROTOCOL_GACHA_PURCHASE_ACK>
{
	S2MOValue<INT32>									m_i32ItemKey;
	S2MOValue<INT32>									m_i32ItemAmount;
	S2MOValue<GACHA_RESULT>								m_eResult;
	S2MOValue<UINT32>									m_ui32WinGoodsID;

	PACKET_GACHA_PURCHASE_ACK()
	{
		// Last Update : 2015-10-14 18:12:57
		_SetValue( m_i32ItemKey );
		_SetValue( m_i32ItemAmount );
		_SetValue( m_eResult );
		_SetValue( m_ui32WinGoodsID );
	}
};

struct PACKET_GACHA_WINNING_USER_ACK : public S2MOPacketBaseT<PROTOCOL_GACHA_WINNING_USER_ACK>
{
	S2MOValue<INT32>					m_eGachaResult;
	S2MOValue<UINT32>					m_ui32LuckyGoodsID;
	S2MOStringW<NET_NICK_NAME_SIZE>		m_strNickName;	
	S2MOValue<UINT32>					m_ui32LuckyTime;

	PACKET_GACHA_WINNING_USER_ACK()
	{
		_SetValue( m_eGachaResult );
		_SetValue( m_ui32LuckyGoodsID );
		_SetValue( m_strNickName );
		_SetValue( m_ui32LuckyTime );
	}

};

#endif