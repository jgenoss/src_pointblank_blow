#include "pch.h"
#include "Shop.h"

I3_CLASS_INSTANCE(CShop, i3ElementBase);

CShop::CShop()
{
	m_ui32Version			= 0;

	m_ui32GoodsCount		= 0;
	m_ui32ItemCount			= 0;
	m_ui32MatchingCount		= 0;
	m_ui32RepairCount		= 0;
	m_ui32CouponCount		= 0;

	i3mem::FillZero( m_aGoods,		sizeof(m_aGoods) );
	i3mem::FillZero( m_aItem,		sizeof(m_aItem) );
	i3mem::FillZero( m_aMatching,	sizeof(m_aMatching) );
	i3mem::FillZero( m_aRepair,		sizeof(m_aRepair) );
	i3mem::FillZero( m_aCoupon,		sizeof(m_aCoupon) );
}

CShop::~CShop()
{
	Destory();
}

void	CShop::Destory()
{	
}

UINT32	CShop::GetFindGoodsIdx( UINT32 ui32GoodsID )
{
	for( UINT32 i = 0 ; i < m_ui32GoodsCount ; i++ )
	{
		if( m_aGoods[ i ]._ui32GoodsID == ui32GoodsID )
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}

UINT32	CShop::GetFindItemIdx( UINT32 ui32ItemID )
{
	for( UINT32 i = 0 ; i < m_ui32ItemCount ; i++ )
	{
		if( m_aItem[ i ]._ui32ItemID == ui32ItemID )
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}

void	CShop::SetVersion( UINT32 ui32Version )
{
	m_ui32Version = ui32Version;
}

void	CShop::SetGoodsCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;
	m_ui32GoodsCount	= ui32Count;
}

void	CShop::SetItemCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;
	m_ui32ItemCount		= ui32Count;
}

void	CShop::SetRepairCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;
	m_ui32RepairCount	= ui32Count;
}

void	CShop::SetMatchingCount( UINT32 ui32Count )
{
	if( 0 >= ui32Count )	return;
	m_ui32MatchingCount	= ui32Count;
}

void CShop::SetCouponCount( UINT32 ui32Count )
{
	if( 0>= ui32Count )		return;
	m_ui32CouponCount = ui32Count;
}

BOOL	CShop::SetRepairList()
{
	// Item 에 수리를 담는다.
	for( UINT32 i = 0 ; i < m_ui32RepairCount ; i++ )
	{
		UINT32 ui32ItemIdx = GetFindItemIdx( m_aRepair[i]._ui32ItemID );
		m_aItemEx[ ui32ItemIdx ]._ui32RepairPoint	= m_aRepair[i]._ui32PricePoint;
		m_aItemEx[ ui32ItemIdx ]._ui32RepairCash	= m_aRepair[i]._ui32PriceCash;
		m_aItemEx[ ui32ItemIdx ]._ui32Durability	= m_aRepair[i]._ui32Durability;
	}
	return TRUE;
}

SHOP_GOODS_BASE*	CShop::GetGoodsData( UINT32 ui32GoodsID  )
{
	UINT32 ui32Idx = GetFindGoodsIdx( ui32GoodsID );
	if ( 0xFFFFFFFF == ui32Idx )
		return NULL;
	return &m_aGoods[ ui32Idx ];
}

SHOP_ITEM_BASE*		CShop::GetItemData( UINT32 ui32ItemID  )
{
	UINT32 ui32Idx = GetFindItemIdx( ui32ItemID );
	if ( 0xFFFFFFFF == ui32Idx )
		return NULL;
	return &m_aItem[ ui32Idx ];
}

SHOP_ITEM_DATA*		CShop::GetItemDataEx( UINT32 ui32ItemID )
{
	UINT32 ui32Idx = GetFindItemIdx( ui32ItemID );
	if ( 0xFFFFFFFF == ui32Idx )
		return NULL;
	return &m_aItemEx[ ui32Idx ];
}

SALE_COUPON*		CShop::GetCouponData( UINT32 ui32CouponID )
{
	for( UINT32 i = 0; i < m_ui32CouponCount; ++i )
	{
		if( m_aCoupon[i].m_ui32CouponID == ui32CouponID )	return &m_aCoupon[i];
	}

	return NULL;
}