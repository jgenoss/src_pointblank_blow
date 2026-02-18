#pragma once

#include "../../../../Common/CommonSource/Gacha/GachaDefine.h"
#include "../../CommonStructure/ShareServer/ProtocolServerStruct.h"

#pragma pack(push, 1)

struct GACHA_RANDOM_ITEM
{
	UINT32				m_ui32GoodsId;
	INT32				m_i32Probability;
};

struct SS_GACHA_ITEM_INFO
{
	INT32				m_i32ItemKey;
	UINT32				m_ui32GoodsID;	// 당첨 상품 GoodsID
	

	INT32				m_i32RemainAmount;		// 잔여 개수
	INT32				m_i32TotalAmount;		// 전체 개수

	//UINT8				m_ui8PurchaseType;				// 결제 방식
	//UINT32				m_ui32Price[PURCHASE_COUNT];	// 결제 가격
	
	INT32				m_arWinProbs[PROB_SECTION_COUNT];

	INT32				m_i32RandomItemCnt;
	GACHA_RANDOM_ITEM	m_arRandomItems[RANDOM_ITEM_COUNT_BY_ITEM];

	INT32				m_i32GloryCount;		// 영예 카운트

};

struct SS_GACHA_GROUP_INFO
{
	SS_GACHA_GROUP_INFO() : m_eGachaGroup(GACHA_GROUP_NONE), m_ui32LuckyGoodsID(INVALID_GOODS_ID), m_i32LuckyProbability(0)//, m_i32GloryCount(0)
		, m_i32WinItemCount(0), m_i32RandomItemCnt(0) //m_ui32PaymentGoodsID(INVALID_GOODS_ID)//, m_i32RequireTicket(INVALID_TICKET_CNT)
	{
		for( INT32 i = 0; i < RANDOM_ITEM_COUNT_BY_ITEM; ++i )
			m_arRandomGoodsIDs[i] = INVALID_GOODS_ID;
	}

	GACHA_GROUP			m_eGachaGroup;
	//UINT32				m_ui32PaymentGoodsID;
	//INT32				m_i32RequireTicket;

	UINT32				m_ui32LuckyGoodsID;
	INT32				m_i32LuckyProbability;

	//INT32				m_i32GloryCount;

	INT32				m_i32WinItemCount;
	SS_GACHA_ITEM_INFO	m_arWinItems[WIN_ITEM_COUNT_BY_TAB];

	INT32				m_i32RandomItemCnt;
	UINT32				m_arRandomGoodsIDs[RANDOM_ITEM_COUNT_BY_TAB];
};

struct PACKET_GACHA_SS_PURCHASE_REQ	: public GACHA_PURCHACE_BASE
{
	T_UID				m_TUID;
};

struct PACKET_GACHA_SS_RESULT_HEADER
{
	GACHA_ERROR			m_eErrorCode;
	T_UID				m_TUID;
	INT32				m_i32ItemIdx;
	INT32				m_i32AccumPurchase;
	INT32				m_i32ResultCount;
	UINT32				m_ui32Cash;		// 유저 캐쉬 잔액.
	UINT8				m_ui8BuyType;	// 결제 유형. SIA_GOODS_BUY		
	T_GoodsID			m_TGoodsID;		// 결제(또는 사용)한 GachaTicket GoodsID.
	UINT32				m_ui32Price;	// 결제 금액.
};

#pragma pack(pop)
