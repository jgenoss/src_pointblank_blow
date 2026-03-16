#pragma once

#include "../../CommonSource/Gacha/GachaDefine.h"

#pragma pack(push, 1)

struct GACHA_ITEM_INFO
{
	INT32				m_i32ItemKey = 0;
	UINT32				m_ui32GoodsID = 0;
	
	INT32				m_i32RemainAmount = 0;		// 잔여 개수
	INT32				m_i32TotalAmount = 0;		// 전체 개수
	INT32				m_i32PurchaseAmount = 0;	// 구매 개수(유저별) - 클라이언트 <-> 게임서버

	INT32				m_i32RandomGoodsCnt = 0;
	UINT32				m_arRandomGoodsIDs[RANDOM_ITEM_COUNT_BY_ITEM];			// [initialize at constructor]

	INT32				m_i32GloryCount = 0;

	GACHA_ITEM_INFO()
	{
		i3mem::Fill(m_arRandomGoodsIDs, INVALID_GOODS_ID, sizeof(m_arRandomGoodsIDs));
	}
};

// 2014.11.20 chunjong.song GACHA_ITEM_INFO의 m_i32RandomGoodsCnt값이 쓰레기값이들어오고있음
inline INT32 GetRandomGoodsCntFromGachaItemInfo(const GACHA_ITEM_INFO* info)
{
	INT32 cnt=0;
	for(INT32 i=0; i<RANDOM_ITEM_COUNT_BY_ITEM; i++)
		if(info->m_arRandomGoodsIDs[i] != 0xffffffff) cnt++;

	return cnt;
}

struct GACHA_GROUP_INFO
{
	GACHA_GROUP_INFO()
	{
		i3mem::Fill(m_arRandomGoodsIDs, INVALID_GOODS_ID, sizeof(m_arRandomGoodsIDs));
	}

	GACHA_GROUP			m_eGachaGroup = GACHA_GROUP_NONE;

	UINT32				m_ui32LuckyGoodsID = INVALID_GOODS_ID;

	INT32				m_i32WinItemCount = 0;
	GACHA_ITEM_INFO		m_arWinItems[WIN_ITEM_COUNT_BY_TAB];

	INT32				m_i32RandomItemCnt = 0;
	UINT32				m_arRandomGoodsIDs[RANDOM_ITEM_COUNT_BY_TAB];		// [initialize at constructor]

	INT32 GetWinItemCount() { return m_i32WinItemCount; }
};

struct GACHA_GROUP_STATE
{
	INT32				m_arRemainCounts[WIN_ITEM_COUNT_BY_TAB];
};

struct GACHA_RESULT_LOG
{
	GACHA_RESULT		m_eWinningType;
	UINT32				m_ui32WinGoodsID;
};

struct PACKET_GACHA_SHOP_STATE
{
	GACHA_GROUP_STATE	m_arGachaGroups[GACHA_GROUP_COUNT];
};

struct PACKET_GACHA_NOTICE
{
	TTCHAR				m_szNotice[GACHA_NOTICE_LENGTH];
};

struct PACKET_GACHA_WINNING_USER
{
	GACHA_RESULT		m_eGachaResult;
	UINT32				m_ui32LuckyGoodsID;
	TTCHAR				m_szLuckyUser[NET_CHARA_NICK_NAME_SIZE];
	UINT32				m_ui32LuckyTime;							// 잭팟 당첨 시간. YYMMDDHHmm
};


struct PACKET_GACHA_RESULT_HEADER
{
	GACHA_ERROR			m_eErrorCode = GACHA_ERROR_SUCCESS;
	INT32				m_i32ItemIdx = 0;
	T_GoodsID			m_TGoodsID = 0;
	INT32				m_i32AccumPurchase = 0;
	INT32				m_i32ResultCount = 0;

	// 잔액
	UINT32				m_ui32Point = 0;
	UINT32				m_ui32Cash = 0;
	UINT32				m_ui32Coin = 0;
};

// from PACKET_GACHA_RESULT_HEADER.m_TGoodsID
#define GET_ITEMKEY_FROM_GACHA_RST_GOODSID(gachaRstGoodID)	(((T_GoodsID)gachaRstGoodID/100) % 1000)

// 그룹번호는 1부터시작 1:POOR, 2:AVERAGE, 3:GOOD
#define GET_GROUP_FROM_GACHA_RST_GOODSID(gachaRstGoodID) (((T_GoodsID)gachaRstGoodID/100000) % 100)


struct PACKET_GACHA_RESULT_DATA
{
	GACHA_RESULT		m_eGachaResult;
	UINT32				m_ui32LuckyGoodsID;
	UINT32				m_ui32WinGoodsID;
};

#pragma pack(pop)
