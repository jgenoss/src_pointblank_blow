#ifndef _SHOP_H_DEF
#define _SHOP_H_DEF

#pragma pack( push, 1 )
typedef struct _tag_ShopItemData : public SHOP_ITEM_EXPANSION
{
	UINT32					_ui32RepairPoint;				// 수리가격
	UINT32					_ui32RepairCash;				// 수리가격
	UINT32					_ui32Durability;				// 내구도
}SHOP_ITEM_DATA;

struct SHOP_DATA
{
	UINT32					m_ui32GoodsCount;
	UINT32					m_ui32ItemCount;
	UINT32					m_ui32MatchingCount;
	UINT32					m_ui32RepairCount;
	
	SHOP_GOODS_EXPANSION	m_pGoods[ MAX_SHOP_GOODS_COUNT ];
	SHOP_ITEM_BASE			m_pItem[ MAX_SHOP_ITEM_COUNT ];
	SHOP_ITEM_DATA			m_pItemEx[ MAX_SHOP_ITEM_COUNT ];
	SHOP_MATCHING_DATA		m_pMatching[ MAX_SHOP_MATCHING_COUNT ];
	SHOP_ITEM_REPAIR_DATA	m_pRepair[ MAX_SHOP_ITEM_COUNT ];
};

#pragma pack( pop )


class CShop: public i3ElementBase
{
	I3_CLASS_DEFINE(CShop);

private:
	UINT32					m_ui32Version;		// 버전
	SHOP_DATA				m_ShopData[MAX_SHOP_DATA_BUF];			// 더블 버퍼.
	UINT32					m_ui32VerionIdx;						// 현재 사용하는 상점 데이터 배열 인덱스. m_ShopData 기준,

	SHOP_DATA*				m_pUseShopData;							// 현재 사용하는 상점 데이터.
	SHOP_DATA*				m_pNotUseShopData;						// 현재 사용하지 않는 상점 데이터.

	char					m_strShopMD5Key[NET_MD5_KEY_SIZE+1];	// ShopFile의 MD5Key

public:
	CShop();
	~CShop();

	void					Destory();
	void					SetVersion( UINT32 ui32Version)		{  m_ui32Version = ui32Version; };

	
	// Set은 사용하지 않는 버퍼에다.
	void					SetGoodsCount( UINT32 ui32Count)		{	m_pNotUseShopData->m_ui32GoodsCount		= ui32Count; };
	void					SetItemCount( UINT32 ui32Count)			{	m_pNotUseShopData->m_ui32ItemCount		= ui32Count; };
	void					SetRepairCount( UINT32 ui32Count)		{	m_pNotUseShopData->m_ui32RepairCount	= ui32Count; };
	void					SetMatchingCount( UINT32 ui32Count)		{	m_pNotUseShopData->m_ui32MatchingCount	= ui32Count; };
	
	void					InitShopAllData(void)					{ i3mem::FillZero( m_pNotUseShopData, sizeof(SHOP_DATA) ); };

	void					SetRepairList(void);
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	// 상점 데이터 조회는 현재 버퍼(m_ShopData)를 기준으로 합니다. 
	SHOP_GOODS_EXPANSION*	GetGoodsAllData(void)		{ return m_pUseShopData->m_pGoods; };
	SHOP_ITEM_BASE*			GetItemAllData(void)		{ return m_pUseShopData->m_pItem; };
	SHOP_ITEM_DATA*			GetItemAllDataEx(void)		{ return m_pUseShopData->m_pItemEx; };
	SHOP_MATCHING_DATA*		GetMatchingAllData(void)	{ return m_pUseShopData->m_pMatching; };
	SHOP_ITEM_REPAIR_DATA*	GetRepairAllData(void)		{ return m_pUseShopData->m_pRepair; };
	

	UINT32					GetVersion( void )			{ return m_ui32Version; }; 
	UINT32					GetGoodsCount( void )		{ return m_pUseShopData->m_ui32GoodsCount; }; 
	UINT32					GetItemCount( void )		{ return m_pUseShopData->m_ui32ItemCount; }; 				
	UINT32					GetMatchingCount( void )	{ return m_pUseShopData->m_ui32MatchingCount; }; 		
	UINT32					GetRepairCount( void )		{ return m_pUseShopData->m_ui32RepairCount; }; 			 	
	
	SHOP_GOODS_EXPANSION*	GetGoodsData( UINT32 ui32GoodsID );
	SHOP_ITEM_BASE*			GetItemData( UINT32 ui32ItemID );
	SHOP_ITEM_DATA*			GetItemDataEx( UINT32 ui32ItemID );
	
	UINT32					GetFindGoodsIdx( UINT32 ui32GoodsID);
	UINT32					GetFindItemIdx( UINT32 ui32ItemID);



	 
	///////////////////////////////////////////////////////////////////////////////////////////////
	// SIA로 부터 받아온 데이터를 담기위해 사용하지 않는 버퍼로.
	SHOP_GOODS_EXPANSION*	GetGoodsAllDataNotUse(void)		{ return m_pNotUseShopData->m_pGoods; };
	SHOP_ITEM_BASE*			GetItemAllDataNotUse(void)			{ return m_pNotUseShopData->m_pItem; };
	SHOP_ITEM_DATA*			GetItemAllDataExNotUse(void)		{ return m_pNotUseShopData->m_pItemEx; };
	SHOP_MATCHING_DATA*		GetMatchingAllDataNotUse(void)		{ return m_pNotUseShopData->m_pMatching; };
	SHOP_ITEM_REPAIR_DATA*	GetRepairAllDataNotUse(void)		{ return m_pNotUseShopData->m_pRepair; };

	char*					GetShopMD5Key(void)					{  return m_strShopMD5Key; };
	void					SetShopMD5Key(char* strShopMD5Key);
	
	SHOP_DATA*				GetNotUseShopData( void )	{ return m_pNotUseShopData;	};
	void					ChangeShopVersion(UINT32 ui32NewShopVersion);

	UINT8					GetItemInfo_byGoodsID(  UINT32 ui32GoodsID,  ST_ITEM * pItem );	// GoodsID로 ItemID와 ItemArg를 얻어온다.
};

#endif
