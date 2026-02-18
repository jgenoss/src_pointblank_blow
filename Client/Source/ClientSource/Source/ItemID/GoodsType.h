#ifndef __GOODSTYPE_H
#define	__GOODSTYPE_H

//////////////////////////////////////////////////////////////////////////////////////
// GoodsID
typedef		UINT32		TGoodsID;	// Goods  ID

//////////////////////////////////////////////////////////////////////////
// 매크로 함수
//#define GET_GOODS_FLAG_MATCHING(Goodsflag)				( ((Goodsflag) / 100) *100 )
//#define GET_GOODS_FLAG_TYPE(Goodsflag)					( (Goodsflag) / 10000000 )
//#define GET_GOODS_FLAG_NUMBER(Goodsflag)				(((Goodsflag) % 10000000) / 100)
//#define MAKE_GOODS_FLAG(type,number)					(((type)*10000000)+((number)*100))
//
//#define GET_GOODS_FLAG_RS_TAB_IDX(Goodsflag)			(((Goodsflag) % 10000000) / 100)	// 굿즈 아이디로 뽑기 상점 탭번호 추출. 
//INT32 MAKE_ITEM_FLAG(INT32 itemClass, INT32 usage, INT32 classtype, INT32 number)
//{
//	return ((itemClass*100000000)+(usage*1000000)+(classtype*1000)+(number));
//}

//////////////////////////////////////////////////////////////////////////
//
// SaleType = 4 이면 이벤트 물품임(상점표시불가)

// 상품등록시 Arg값
enum GOODS_ID_ARG_TYPE
{
	// Period
	GOODS_ID_ARG_TYPE_1H	= 1,	// 3600,
	GOODS_ID_ARG_TYPE_2H	= 2,	// 7200,
	GOODS_ID_ARG_TYPE_3H	= 3,	// 10800,
	GOODS_ID_ARG_TYPE_1D	= 4,	// 86400,
	GOODS_ID_ARG_TYPE_2D	= 5,	// 172800,
	GOODS_ID_ARG_TYPE_3D	= 6,	// 259200,
	GOODS_ID_ARG_TYPE_5D	= 7,	// 432000,
	GOODS_ID_ARG_TYPE_7D	= 8,	// 604800,
	GOODS_ID_ARG_TYPE_10D	= 9,	// 864000,
	GOODS_ID_ARG_TYPE_14D	= 10,	// 1209600,
	GOODS_ID_ARG_TYPE_15D	= 11,	// 1296000,
	GOODS_ID_ARG_TYPE_30D	= 12,	// 2592000,
	GOODS_ID_ARG_TYPE_90D	= 13,	// 7776000,
	GOODS_ID_ARG_TYPE_365D	= 14,	// 31536000,

	// Count
	GOODS_ID_ARG_TYPE_1M	= 51,	// 5760,
	GOODS_ID_ARG_TYPE_5M	= 52,	// 10080,
	GOODS_ID_ARG_TYPE_6M	= 53,	// 20160,
	GOODS_ID_ARG_TYPE_10M	= 54,
	GOODS_ID_ARG_TYPE_11M	= 55,
	GOODS_ID_ARG_TYPE_25M	= 56,
	GOODS_ID_ARG_TYPE_30M	= 57,
	GOODS_ID_ARG_TYPE_50M	= 58,
	GOODS_ID_ARG_TYPE_75M	= 59,
	GOODS_ID_ARG_TYPE_90M	= 60,
	GOODS_ID_ARG_TYPE_100M	= 61,
	GOODS_ID_ARG_TYPE_250M	= 62,
	GOODS_ID_ARG_TYPE_500M	= 63,

	// Durability
	GOODS_ID_ARG_TYPE_DUR	= 99,
};

//////////////////////////////////////////////////////////////////////////
enum GOODS_BUY_GROUP
{
	GOODS_BUY_GROUP_NONE	= 0,
	
	GOODS_BUY_GROUP_PARTS,
	GOODS_BUY_GROUP_ETC,

	MAX_GOODS_BUY_GROUP
};

//////////////////////////////////////////////////////////////////////////
#endif