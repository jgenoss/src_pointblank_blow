#ifndef _SHOP_H_DEF
#define _SHOP_H_DEF

#pragma pack( push, 1 )
struct SHOP_ITEM_DATA : public SHOP_ITEM_EXPANSION
{
	UINT32					_ui32RepairPoint;				// 수리가격
	UINT32					_ui32RepairCash;				// 수리가격
	UINT32					_ui32Durability;				// 내구도
};
#pragma pack( pop )

class CShop: public i3ElementBase
{
	I3_CLASS_DEFINE(CShop);

private:
	UINT32					m_ui32Version;

	UINT32					m_ui32GoodsCount;
	UINT32					m_ui32ItemCount;
	UINT32					m_ui32MatchingCount;
	UINT32					m_ui32RepairCount;
	UINT32					m_ui32CouponCount;	// 할인쿠폰 개수

	SHOP_GOODS_BASE			m_aGoods[ MAX_SHOP_GOODS_COUNT ];
	SHOP_GOODS_EXPANSION	m_aGoodsEx[ MAX_SHOP_GOODS_COUNT ];
	SHOP_ITEM_BASE			m_aItem[ MAX_SHOP_ITEM_COUNT ];
	SHOP_ITEM_DATA			m_aItemEx[ MAX_SHOP_ITEM_COUNT ];
	SHOP_MATCHING_DATA		m_aMatching[ MAX_SHOP_MATCHING_COUNT ];
	SHOP_ITEM_REPAIR_DATA	m_aRepair[ MAX_SHOP_ITEM_COUNT ];
	SALE_COUPON				m_aCoupon[ MAX_SHOP_COUPON_COUNT ];	// 할인쿠폰 정보

public:
	CShop();
	~CShop();

	void					Destory();
	void					SetVersion( UINT32 ui32Version );
	void					SetGoodsCount( UINT32 ui32Count );
	void					SetItemCount( UINT32 ui32Count );
	void					SetRepairCount( UINT32 ui32Count );
	void					SetMatchingCount( UINT32 ui32Count );
	void					SetCouponCount( UINT32 ui32Count );

	UINT32					GetVersion()						{	return m_ui32Version;			};
	UINT32					GetGoodsCount( )					{	return m_ui32GoodsCount;		};
	UINT32					GetItemCount( )						{	return m_ui32ItemCount;			};
	UINT32					GetMatchingCount( )					{	return m_ui32MatchingCount;		};
	UINT32					GetRepairCount( )					{	return m_ui32RepairCount;		};
	UINT32					GetCouponCount( )					{	return m_ui32CouponCount;		};

	BOOL					SetGoodsList();	
	BOOL					SetRepairList();

	SHOP_GOODS_BASE*		GetGoodsAllData( )					{	return m_aGoods;				};
	SHOP_GOODS_EXPANSION*	GetGoodsAllDataEx( )				{	return m_aGoodsEx;				};
	SHOP_ITEM_BASE*			GetItemAllData( )					{	return m_aItem;					};
	SHOP_ITEM_DATA*			GetItemAllDataEx( )					{	return m_aItemEx;				};
	SHOP_MATCHING_DATA*		GetMatchingAllData( )				{	return m_aMatching;				};
	SHOP_ITEM_REPAIR_DATA*	GetRepairAllData( )					{	return m_aRepair;				};
	SALE_COUPON*			GetCouponAllData( )					{	return m_aCoupon;				};

	SHOP_GOODS_BASE*		GetGoods( UINT32 ui32Idx )			{	return &m_aGoods[ ui32Idx ];	};
	SHOP_GOODS_EXPANSION*	GetGoodsEx( UINT32 ui32Idx )		{	return &m_aGoodsEx[ ui32Idx ];	};
	SHOP_ITEM_BASE*			GetItem( UINT32 ui32Idx )			{	return &m_aItem[ ui32Idx ];		};
	SHOP_ITEM_DATA*			GetItemEx( UINT32 ui32Idx )			{	return &m_aItemEx[ ui32Idx ];	};
	// 사용하는 함수
	UINT32					GetFindGoodsIdx( UINT32 ui32GoodsID );
	UINT32					GetFindItemIdx( UINT32 ui32ItemID );
	

	SHOP_GOODS_BASE*		GetGoodsData( UINT32 ui32GoodsID );
	SHOP_ITEM_BASE*			GetItemData( UINT32 ui32ItemID );
	SHOP_ITEM_DATA*			GetItemDataEx( UINT32 ui32ItemID );
	SALE_COUPON*			GetCouponData( UINT32 ui32CouponID );
};

#endif
