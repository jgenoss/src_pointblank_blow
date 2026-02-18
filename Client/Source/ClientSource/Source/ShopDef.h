#ifndef _SHOP_DEF_H
#define _SHOP_DEF_H

enum SHOP_ITEM_TYPE
{
	SHOP_ITEM_TYPE_NOTSETTING = -1,
	SHOP_ITEM_TYPE_NORMAL = 0,
	SHOP_ITEM_TYPE_NEW,			//	신상
	SHOP_ITEM_TYPE_HOT,			//	강추
	SHOP_ITEM_TYPE_EVENT,		//	이벤트 전용
	SHOP_ITEM_TYPE_PCROOM,		//	PC방 전용
	SHOP_ITEM_TYPE_SALE,		//	SALE

	SHOP_ITEM_TYPE_COUNT
};

// CommonDef.h에 GOODS_PRICE_TYPE라고 선언되어 있지만.
// 그 Enum은 비트 조합이 안되기 때문에
// 새로선언하였다.
typedef			UINT32		GOODS_PRICE_TYPE;

#define			GOODS_PRICE_UNKNOWN		(0x00000000)
#define			GOODS_PRICE_POINT		(0x00000001)
#define			GOODS_PRICE_CASH		(0x00000002)
#define			GOODS_PRICE_ALL			(GOODS_PRICE_POINT | GOODS_PRICE_CASH)


void	CopyPriceTypeText(GOODS_PRICE_TYPE PriceType, i3::rc_wstring& outText);

struct GOODS_PRICE_INFO
{
	UINT32	_Cash;		// 캐시 가격
	UINT32	_Point;		// 포인트 가격
	UINT32	_Type;		// 결제 방식 (Cash / Point / All)
	UINT8	_SaleMark;	// 상품 마크 0 - 없음, 1 - NEW, 2 - Hot	

	void	BuildPriceInfo(UINT32 ui32Cash, UINT32 ui32Point, UINT8 ui8Mark)
	{
		_Cash = ui32Cash;
		_Point = ui32Point;
		_SaleMark = ui8Mark;

		_Type = GOODS_PRICE_UNKNOWN;

		if ( ui32Point > 0 )
			_Type |= GOODS_PRICE_POINT;

		if ( ui32Cash > 0 )
			_Type |= GOODS_PRICE_CASH;

		if (ui32Cash == 0 && ui32Point == 0)
			_Type = GOODS_PRICE_POINT;
	}

	bool CheckPrice()
	{
		if (_Cash == 0 && _Point == 0)
			return false;

		return true;
	}


} ;

bool operator==(const GOODS_PRICE_INFO & lhs, const GOODS_PRICE_INFO & rhs);
bool operator!=(const GOODS_PRICE_INFO & lhs, const GOODS_PRICE_INFO & rhs);

struct SHOP_GOODS
{
	UINT32				_GoodsID;			// 상품 아이디
	UINT8				_AuthType;			// 1:기간재, 2:개수재
	UINT32				_AuthArg;			// _AuthType의 1일 경우 일(날일), 2일 경우 개수
	UINT32				_ItemID;
	UINT8				_SaleDesignation;	//	구매시 호칭 제한	(DESIGNATION_TYPE)
	UINT8				_SaleType;
	GOODS_PRICE_INFO	_PriceInfo[PRICE_TYPE_MAX]; // 가격 정보

	UINT32			GetPriceCash(INT32 idx)	 const	{ return _PriceInfo[idx]._Cash; }
	UINT32			GetPricePoint(INT32 idx) const	{ return _PriceInfo[idx]._Point; }
	UINT32			GetPriceType(INT32 idx)	 const	{ return _PriceInfo[idx]._Type; }
	UINT8			GetSaleMark(INT32 idx)	 const	{ return _PriceInfo[idx]._SaleMark; }

	bool			isShopGoods(void) const { return (_SaleType == GOODS_SALE_ALL || _SaleType == GOODS_SALE_BUY); }
	bool			isGiftGoods(void) const { return (_SaleType == GOODS_SALE_ALL || _SaleType == GOODS_SALE_GIFT); }
	
} ;

bool operator==(const SHOP_GOODS & lhs, const SHOP_GOODS & rhs);
bool operator!=(const SHOP_GOODS & lhs, const SHOP_GOODS & rhs);

struct	SHOP_GOODS_PACK
{
public:
	SHOP_GOODS_PACK();
	~SHOP_GOODS_PACK();

	INT32				GetGoodsCount(void) const;
	bool				AddGoods(SHOP_GOODS * pAddGoods);
	SHOP_GOODS *		GetGoods(INT32 index) const;
	void				Sort(bool (*pProc)( const SHOP_GOODS*, const SHOP_GOODS*) );
	void				ClearAll();
	INT32				GetDisplayShopGoodsIndex(bool bigger = false);									// 상점 슬롯에서 표시할 goods 뽑기.
	GOODS_PRICE_TYPE	GetPriceTypeAll(INT32 price_idx);												// 팩에 포함된 모든 상품의 가격형식을 비트조합해서 반환합니다.
	
private:
	i3::vector<SHOP_GOODS*> _pGoodsList;		//	SHOP_GOODS가 들어있음. 1개 or n개
};

bool operator==(const SHOP_GOODS_PACK & lhs, const SHOP_GOODS_PACK & rhs);
bool operator!=(const SHOP_GOODS_PACK & lhs, const SHOP_GOODS_PACK & rhs);

enum SHOP_GOODS_TYPE
{
	SHOP_GOODS_UNKNOWN = -1,
	SHOP_WEAPON_GOODS = 0,
	SHOP_CHARACTER_GOODS,
	SHOP_SKIN_GOODS,
	SHOP_CASH_ITEM_GOODS,
	SHOP_PACKAGE_GOODS,

	SHOP_GOODS_MAX
};


enum PCCAFE_TYPE
{
	PCCAFE_NONE		= 0,		//	일반
	PCCAFE_EVENT	= 1,		//	이벤트 한정
	PCCAFE_NORMAL	= 2,		//	PC방 한정
	PCCAFE_PREMIUM	= 3,		//	PREMIUM PC방 한정
};


enum SLOT_SALE_TEXT
{
	SALE_TEXT_FIRST,
	SALE_TEXT_SECOND,

	SALE_TEXT_MAX
};

//	활성화 될시 구입즉시 인증 여부를 물어본다. 비활성화시 구입시 인증을 묻지 않는다.
#define AUTH_FROM_BUY_GOODS

namespace EQUIP 
{
	enum GOODS 
	{
		GOODS_NONE = 0,
		GOODS_CHARA,
		GOODS_WEAPON,
	};

	//	장비가 변경된 경우 GOODS_EQUIP_CHARA 또는 GOODS_EQUIP_WEAPON를 플래그 값으로 반환한다. 0일 경우 변경된 장비가 없다.
	GOODS	EquipGoods( const SHOP_GOODS * pGoods, bool bUpdateViewModel = true);
}

//	선물하기시에 계급 제한
#define SHOP_GIFT_REQUIREMENT_RANK		4

#define	GOODS_DAY_1				1
#define	GOODS_DAY_3				3
#define	GOODS_DAY_7				7
#define	GOODS_DAY_30			30

// 인벤토리와 상점에서 상품이 표시되는 가로 개수.
#define			SLOT_HORI_COUNT_WEAPON			2
#define			SLOT_HORI_COUNT_CHARA			3
#define			SLOT_HORI_COUNT_ITEM			3
#define			SLOT_HORI_COUNT_PACKAGE			3		// 상점에서만 사용됩니다.

#define			SLOT_VERT_COUNT_WEAPON			3
#define			SLOT_VERT_COUNT_CHARA			2
#define			SLOT_VERT_COUNT_ITEM			2
#define			SLOT_VERT_COUNT_PACKAGE			2

// 슬롯에 표시되는 상품 개수.
#define			MAX_SLOT_FOR_SHOP_WEAPON		6
#define			MAX_SLOT_FOR_SHOP_CHARACTER		6
#define			MAX_SLOT_FOR_SHOP_ITEM			6
#define			MAX_SLOT_FOR_SHOP_PACKAGE		6



#define			ITEM_TERM_ADJUST_VALUE		120


enum	REPAIR_TYPE
{
	REPAIR_TYPE_UNKOWN,
	REPAIR_TYPE_CAN_FULL,
	REPAIR_TYPE_CAN_SOME,
	REPAIR_TYPE_CANNOT,
	REPAIR_TYPE_NEEDLESS,
};

bool				CheckYourRank( UINT8 bCheckRank);			//	계급 이상 자격 판단
bool				IsCashBuyingFromGoods( SHOP_GOODS * pGoods, UINT8 price_idx);		//	상품이 캐시인지 여부

// 기간제 아이템의 판매 기간을 계산합니다.
INT32 GetMaintenanceItemDay( T_ItemID ItemID, UINT8 AuthType, UINT32 AuthArg);
INT32 GetMaintenanceItemHour( T_ItemID ItemID, UINT8 AuthType, UINT32 AuthArg);




bool ExcuteSNDAWeb(void);

//	유료 코인 충전 웹페이지를 띄운다.
bool ExcuteCashWeb(void);

//	인터넷익스플로러 띄우기
bool ExecuteInternetExplorer(LPCWSTR url, LPCSTR postData /*= nullptr*/, bool isPopup /*= false*/);

// "보유중 : %d일"과 같이 텍스트를 만든다.
void	FormatHaveItemTermText(i3::stack_wstring& out, UINT32 Term);

// 내구도가 0인지 확인합니다. (기간이 만료되었는지 또는 보유개수가 0개인지 체크합니다.);
// 신규 내구도 시스템은 이거 쓰면 안됨 - 신규는 arg / max < 0.01 일때 제로
bool	GetEnduranceZero(UINT8 InvenItemAttr, UINT32 InvenItemArg);

bool	CanUse_ViewOtherUserItemInfo();



#endif

