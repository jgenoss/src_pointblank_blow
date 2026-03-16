#pragma once

#define INVALID_ITEM_KEY				0xFFFFFFFF
#define INVALID_WIN_INDEX				0xFFFFFFFF
#define INVALID_TICKET_CNT				0xFFFFFFFF
#define INVALID_GOODS_ID				0xFFFFFFFF
#define INVALID_GLORY_CNT				0xFFFFFFFF

#define GACHA_NOTICE_COUNT				3
#define GACHA_NOTICE_LENGTH				100 
#define ITEM_DESC_LENGTH				32

#define GACHA_MAX_LUCKY_PROBABILITY		100000			// 0.001%

#define MAX_GACHA_GAME_COUNT			10 + 2

#define GACHA_PAYMENT_COUNT_MAX			3

#define GACHA_SHOP_UPDATE_TIME			2	// Sec

#define MAKE_GACHA_GOODSID(Tabidx,ItemIdx,Arg)	(((GOODS_TYPE_RSPRICE)*10000000 )+((Tabidx)*100000)+((ItemIdx)*100)+Arg

//
//enum PAYMENT_COUNT_TYPE				// 결제 갯수 유형
//{
//	PAYMENT_COUNT_1		= 0,		// 1개 결제
//	PAYMENT_COUNT_5,				// 5개 결제
//	PAYMENT_COUNT_10,				// 10개 결제
//
//	PAYMENT_COUNT_MAX,
//};


enum ITEM_COUNT
{
	WIN_ITEM_COUNT_BY_TAB		= 64,				// 탭별 당첨 아이템 개수

	RANDOM_ITEM_COUNT_BY_TAB	= 10,				// 탭별 랜덤 아이템 개수
	RANDOM_ITEM_COUNT_BY_ITEM	= 6,				// 아이템별 랜덤 아이템 개수
};

//enum BONUS_GAME
//{
//	BONUS_COUNT_1ST			= 1,				// 보너스 게임 횟수 1. PAYMENT_COUNT_5
//	BONUS_COUNT_2ND			= 2,				// 보너스 게임 횟수 2. PAYMENT_COUNT_10
//	BONUS_COUNT_MAX			= BONUS_COUNT_2ND,
//};

enum PROBABILITY
{
	PROB_SECTION_1ST		= 0,				// 첫 번째 구간의 당첨 확률
	PROB_SECTION_2ND		,					// 두 번째 구간의 당첨 확률
	PROB_SECTION_3RD		,					// 세 번째 구간의 당첨 확률
	PROB_SECTION_4TH		,					// 네 번째 구간의 당첨 확률
	PROB_SECTION_5TH		,					// 다섯 번째 구간의 당첨 확률
	PROB_SECTION_COUNT		,

	PROBABILITY_MAX			= 100,				// 구간별 확률의 총 합
	
	DEFAULT_PROBABILTY		= 10000,			// 기본 당첨 확률( 1 / DEFAULT_PROBABILTY )
};

//enum PURCHASE_TYPE
//{
//	PURCHASE_CASH			= 0,
//	PURCHASE_POINT			,
//	PURCHASE_TICKET			,
//	PURCHASE_COUNT			,	
//};

enum GACHA_RESULT
{
	GACHA_RESULT_NONE		= 0,
	GACHA_RESULT_LUCKY		,
	GACHA_RESULT_WIN		,
	GACHA_RESULT_GLORY		,
	GACHA_RESULT_RANDOM		,
};

enum GACHA_ERROR
{
	GACHA_ERROR_SUCCESS			= 0,
	GACHA_ERROR_INTERNAL		,		// 서버 내부 에러
	GACHA_ERROR_CHARGE			,		// 캐시, 티켓 부족
	GACHA_ERROR_INVALID_REQUEST	,		// 유효하지 않은 요청
	GACHA_ERROR_PAYMENT			,		// 결제 실패G
	GACHA_ERROR_DISABLED		,		// 사용할 수 없음
	GACHA_ERROR_USER			,		// 유효하지 않은 유저
};

enum GACHA_GROUP
{
	GACHA_GROUP_NONE		= -1,
	GACHA_GROUP_POOR		,
	GACHA_GROUP_AVERAGE		,
	GACHA_GROUP_GOOD		,
	GACHA_GROUP_COUNT		,
};
