#ifndef _SHOP_H_DEF
#define _SHOP_H_DEF

#include "ShopDef.h"

#pragma pack( push, 1 )
//#define SHOP_GOODS_BASE	SHOP_GOODS_BASE

struct SHOP_ITEM_DATA : public SHOP_ITEM_BASE
{
	UINT32					_ui32RepairPoint;				// 수리가격
	UINT32					_ui32RepairCash;				// 수리가격
	UINT32					_ui32Durability;				// 내구도
};

struct SHOP_GOODS_LIST
{	
	SHOP_GOODS_EXPANSION*	_pGoodsData;					// 상품 데이타를 담을 저장소
	UINT8					_ui8ItemCount;					// 상품이 가지고있는 아이템 개수
	SHOP_ITEM_DATA**		_ppItemData;					// 아이템 데이타		* m_ui8ItemCount
	UINT32*					_pui32ItemArg;					// 아이템 인증개수/시간 * m_ui8ItemCount
	INT32					_ui32ItemDataCount;				// _ppItemData 배열 생성 갯수
};
#pragma pack( pop )


class CShop : public i3::shared_ginst<CShop>
{

private:
	bool					m_IsLoadShop;
	bool					m_bFirstShopFileCheck;	// 상점 파일 체크가 처음인지.

	UINT32					m_ui32Version;

	UINT32					m_ui32GoodsVersion;
	UINT32					m_ui32ItemVersion;
	UINT32					m_ui32MatchingVersion;
	UINT32					m_ui32RepairVersion;
	UINT32					m_ui32CouponVersion;

	UINT32					m_ui32GoodsVerify;
	UINT32					m_ui32ItemVerify;
	UINT32					m_ui32MatchingVerify;
	UINT32					m_ui32RepairVerify;
	UINT32					m_ui32CouponVerify;

	UINT32					m_ui32GoodsCount;
	UINT32					m_ui32ItemCount;
	UINT32					m_ui32MatchingCount;
	UINT32					m_ui32RepairCount;
	UINT32					m_ui32CouponCount;

	SHOP_GOODS_EXPANSION	m_pGoods[ MAX_SHOP_GOODS_COUNT ];
	SHOP_ITEM_DATA			m_pItem[ MAX_SHOP_ITEM_COUNT ];
	SHOP_MATCHING_DATA		m_pMatching[ MAX_SHOP_MATCHING_COUNT ];
	SHOP_ITEM_REPAIR_DATA	m_pRepair[ MAX_SHOP_ITEM_COUNT ];

#if defined(USE_SALE_COUPON)
	SALE_COUPON				m_pCoupon[ MAX_SHOP_ITEM_COUNT ];
#endif
	SHOP_GOODS_LIST			m_pShopGoods[ MAX_SHOP_GOODS_COUNT ];

	UINT32					_GetFindGoodsIdx( UINT32 ui32GoodsID );
	UINT32					_GetFindItemIdx( UINT32 ui32ItemID );
	UINT32					_GetFindMatchingIdx( UINT32 ui32GoodsID );
#if defined(USE_SALE_COUPON)
	UINT32					_GetFindCouponIdx( UINT32 ui32CouponID );
#endif

public:
	CShop();
	~CShop();

	void					Reset()								{	m_IsLoadShop = false;			};
	void					Destory();
	bool					IsLoadShop()						{	return m_IsLoadShop;			};
	void					SetVersion( UINT32 ui32Version );
	void					SetGoodsCount( UINT32 ui32Count );
	void					SetItemCount( UINT32 ui32Count );
	void					SetRepairCount( UINT32 ui32Count );
	void					SetMatchingCount( UINT32 ui32Count );
	void					SetCouponCount( UINT32 ui32Count );	
			
	UINT32					ConvertGoodsID(UINT32 ui32Count);

	UINT32					GetVersion()						{	return m_ui32Version;			};
	// revision 54070 상점 패킷을 제대로 못 받을 때 처리 수정. 
	UINT32					GetGoodsVersion()					{	return m_ui32GoodsVersion;		};
	UINT32					GetItemVersion()					{	return m_ui32ItemVersion;		};
	UINT32					GetMatchingVersion()				{	return m_ui32MatchingVersion;	};
	UINT32					GetRepairVersion()					{	return m_ui32RepairVersion;		};
	UINT32					GetCouponVersion()					{	return m_ui32CouponVersion;		};

	void					SetGoodsVersion(UINT32 ui32Version)					{	m_ui32GoodsVersion = ui32Version;		};
	void					SetItemVersion(UINT32 ui32Version)					{	m_ui32ItemVersion = ui32Version;		};
	void					SetMatchingVersion(UINT32 ui32Version)				{	m_ui32MatchingVersion = ui32Version;	};
	void					SetRepairVersion(UINT32 ui32Version)				{	m_ui32RepairVersion = ui32Version;		};
	void					SetCouponVersion(UINT32 ui32Version)				{	m_ui32CouponVersion = ui32Version;		};

	void					SetGoodsVerify( UINT32 ui32Count);
	void					SetItemVerify( UINT32 ui32Count);
	void					SetRepairVerify( UINT32 ui32Count);
	void					SetMatchingVerify( UINT32 ui32Count);
	void					SetCouponVerify( UINT32 ui32Count);

	void					AddGoodsVerify( UINT32 ui32Count);
	void					AddItemVerify( UINT32 ui32Count);
	void					AddRepairVerify( UINT32 ui32Count);
	void					AddMatchingVerify( UINT32 ui32Count);
	void					AddCouponVerify( UINT32 ui32Count);

	bool					ShopListCompleted();
	void					ShopVersionReset();
	void					ShopVerifyReset();	// revision 54146

	UINT32					GetGoodsVerify()					{	return m_ui32GoodsVerify;		};
	UINT32					GetItemVerify()						{	return m_ui32ItemVerify;		};
	UINT32					GetMatchingVerify()					{	return m_ui32MatchingVerify;	};
	UINT32					GetRepairVerify()					{	return m_ui32RepairVerify;		};
	UINT32					GetCouponVerify()					{	return m_ui32CouponVerify;		};

	UINT32					GetGoodsCount( )					{	return m_ui32GoodsCount;		};
	UINT32					GetItemCount( )						{	return m_ui32ItemCount;			};
	UINT32					GetMatchingCount( )					{	return m_ui32MatchingCount;		};
	UINT32					GetRepairCount( )					{	return m_ui32RepairCount;		};
	UINT32					GetCouponCount(	)					{	return m_ui32CouponCount;		};

	bool					SetGoodsList();	
	bool					SetRepairList();

	SHOP_GOODS_EXPANSION*	GetGoodsAllData( )					{	return m_pGoods;				};
	SHOP_ITEM_DATA*			GetItemAllData( )					{	return m_pItem;					};
	SHOP_MATCHING_DATA*		GetMatchingAllData( )				{	return m_pMatching;				};
	SHOP_ITEM_REPAIR_DATA*	GetRepairAllData( )					{	return m_pRepair;				};
#if defined(USE_SALE_COUPON)
	SALE_COUPON*			GetCouponAllData( )					{	return m_pCoupon;				};
#endif

	SHOP_GOODS_EXPANSION*	GetGoodsDataByIndex( INT32 idx)		{	I3_BOUNDCHK( idx, MAX_SHOP_GOODS_COUNT);	return &m_pGoods[idx];		};
	SHOP_ITEM_DATA*			GetItemDataByIndex( INT32 idx)		{	I3_BOUNDCHK( idx, MAX_SHOP_ITEM_COUNT);	return &m_pItem[idx];		};
	SHOP_MATCHING_DATA*		GetMatchingDataByIndex( INT32 idx)	{	I3_BOUNDCHK( idx, MAX_SHOP_MATCHING_COUNT); return &m_pMatching[idx];	};
	SHOP_ITEM_REPAIR_DATA*	GetRepairDataByIndex( INT32 idx)	{	I3_BOUNDCHK( idx, MAX_SHOP_ITEM_COUNT);	return &m_pRepair[idx];		};
#if defined(USE_SALE_COUPON)
	SALE_COUPON*			GetCouponDataByIndex( INT32 idx)	{	I3_BOUNDCHK( idx, MAX_SHOP_ITEM_COUNT); return &m_pCoupon[idx];		};
#endif
	// 사용하는 함수
	SHOP_GOODS_LIST*		GetGoodsListData( UINT32 ui32GoodsID );

	SHOP_GOODS_EXPANSION*	GetGoodsData( UINT32 ui32GoodsID  );
	SHOP_ITEM_DATA*			GetItemData( UINT32 ui32ItemID  );
#if defined(USE_SALE_COUPON)
	SALE_COUPON*			GetCouponData( UINT32 ui32CouponID );
#endif

public:
	void				GetGoodsPrice(UINT32 ui32GoodsID, GOODS_PRICE_INFO & info, INT32 price_idx);
	void				GetGoodsPrice(UINT32 ui32GoodsID, GOODS_PRICE_INFO * pOutPrice);

	// 세트 아이템인지 판별한다.
	bool				IsPackageGoods(UINT32 ui32GoodsID);


	// 개수제인지 기간제인지 반환한다.
	UINT8				GetAuthType(UINT32 ui32ItemID);

// Shop버전 
private:
	HANDLE					m_hFile;
	char					m_strShopMD5Key[NET_MD5_KEY_SIZE+1];

public:

	bool					GetFirstShopFileCheck(void)			{	return m_bFirstShopFileCheck; };

	char*					GetShopMD5Key(void);
	void					MakeShopMD5Key(void);

	void					LoadShopFile();
	bool					OpenShopFile();
	bool					FindShopFile();
	bool					CreateShopFile();
	bool					SaveShopFile();
	bool					DeleteShopFile();

	T_GoodsID				GetGachaGoodsID( UINT8 ui8GachaGroup, INT32 i32ItemKey, INT32 i32Arg );
	SHOP_GOODS_EXPANSION*	GetGachaGoods( UINT8 ui8GachaGroup, INT32 i32ItemKey, INT32 i32Arg );

	void					GetGachaMatcing( UINT8 ui8GachaGroup, INT32 i32ItemKey, MINI_MATCHING* pMiniMatching );
};

#endif
