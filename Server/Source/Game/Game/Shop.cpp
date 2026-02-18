#include "pch.h"
#include "Shop.h"

I3_CLASS_INSTANCE(CShop, i3ElementBase);

CShop::CShop()
{

	m_ui32Version			= 0;
	m_ui32VerionIdx		= 0;
	
	m_strShopMD5Key[NET_MD5_KEY_SIZE] = 0;

	i3mem::FillZero(&m_ShopData,		sizeof(SHOP_DATA)* MAX_SHOP_DATA_BUF);

	m_pUseShopData		= &m_ShopData[0];		
	m_pNotUseShopData	= &m_ShopData[1];		
}

CShop::~CShop()
{
	Destory();
}

void	CShop::Destory()
{	
}

void	CShop::SetRepairList()
{
	// Item 에 수리를 담는다. 사용하지 않는 버퍼에서.
	for( UINT32 i = 0 ; i < m_pNotUseShopData->m_ui32RepairCount ; i++ )
	{
		UINT32 ui32ItemIdx = GetFindItemIdx( m_pNotUseShopData->m_pRepair[i]._ui32ItemID );
		if( 0xFFFFFFFF == ui32ItemIdx ) continue;
		m_pNotUseShopData->m_pItemEx[ ui32ItemIdx ]._ui32RepairPoint= m_pNotUseShopData->m_pRepair[i]._ui32PricePoint;
		m_pNotUseShopData->m_pItemEx[ ui32ItemIdx ]._ui32RepairCash	= m_pNotUseShopData->m_pRepair[i]._ui32PriceCash;
		m_pNotUseShopData->m_pItemEx[ ui32ItemIdx ]._ui32Durability	= m_pNotUseShopData->m_pRepair[i]._ui32Durability;
	}

	return;
}

SHOP_GOODS_EXPANSION*	CShop::GetGoodsData( UINT32 ui32GoodsID  )
{
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32GoodsCount ; i++ )
	{
		if( m_pUseShopData->m_pGoods[ i ]._ui32GoodsID == ui32GoodsID ) return &m_pUseShopData->m_pGoods[ i ];
	}	

	return NULL;
}

SHOP_ITEM_BASE*		CShop::GetItemData( UINT32 ui32ItemID  )
{
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32ItemCount ; i++ )
	{
		if( m_pUseShopData->m_pItem[ i ]._ui32ItemID == ui32ItemID ) return &m_pUseShopData->m_pItem[ i ];
	}	

	return NULL;
}

SHOP_ITEM_DATA*		CShop::GetItemDataEx( UINT32 ui32ItemID )
{
	for( UINT32 i = 0 ; i < m_pUseShopData->m_ui32ItemCount ; i++ )
	{
		if( m_pUseShopData->m_pItem[ i ]._ui32ItemID == ui32ItemID ) return &m_pUseShopData->m_pItemEx[ i ];
	}	

	return NULL;
}

UINT32	CShop::GetFindItemIdx( UINT32 ui32ItemID )
{
	// 백업 버퍼.
	for( UINT32 i = 0 ; i < m_pNotUseShopData->m_ui32ItemCount ; i++ )
	{
		if( m_pNotUseShopData->m_pItem[ i ]._ui32ItemID == ui32ItemID ) return i;
	}

	return 0xFFFFFFFF;
}
void	CShop::ChangeShopVersion(UINT32 ui32NewShopVersion)
{
	m_ui32Version = ui32NewShopVersion; 

	// 현재 사용중인 상점 버퍼를 사용아지 않는걸로.
	m_pNotUseShopData = &m_ShopData[m_ui32VerionIdx];

	m_ui32VerionIdx++;
	if( m_ui32VerionIdx >= MAX_SHOP_DATA_BUF ) m_ui32VerionIdx = 0;

	// 현재 사용하지 않는 상점 버퍼를 사용중인 걸로.
	m_pUseShopData = &m_ShopData[m_ui32VerionIdx];

	return;
}

void	CShop::SetShopMD5Key(char* strShopMD5Key)	
{  
	i3String::Copy( m_strShopMD5Key, strShopMD5Key, NET_MD5_KEY_SIZE+1); 
	m_strShopMD5Key[NET_MD5_KEY_SIZE] = 0;
};

UINT8	CShop::GetItemInfo_byGoodsID(  UINT32 ui32GoodsID,  ST_ITEM * pItem )	// GoodsID로 ItemID와 ItemArg를 얻어온다.
{
	UINT8 ui8ItemCount = 0;
	SHOP_MATCHING_DATA*	pMatching = GetMatchingAllData();
	UINT32 ui32MatchingCount = GetMatchingCount();

	for( UINT32 i = 0; i < ui32MatchingCount; ++i )
	{
		if( ui8ItemCount >= MAX_SHOP_GOODS_ITEM_COUNT ) break;

		if( pMatching->_ui32GoodsID == ui32GoodsID )
		{
			pItem[ui8ItemCount].m_TItemID	= pMatching->_ui32ItemID;
			pItem[ui8ItemCount].m_TItemArg	= pMatching->_ui32ItemArg + pMatching->_ui32BonusArg;
			++ui8ItemCount;
		}

		++pMatching;
	}

	return ui8ItemCount;	 
}