#ifndef _SHOP_H_DEF
#define _SHOP_H_DEF

#pragma pack( push, 1 )

struct  SHOP_GOODS_DATA : public  SHOP_GOODS_EXPANSION
{
	char			_strName[ MAX_ITEM_NAME ];	// Goods 이름
	UINT32			_ui32SaleCount;			// 판매량
	UINT32			_ui32GiftCount;
};

struct SHOP_ITEM_DATA : public SHOP_ITEM_BASE, public SHOP_ITEM_EXPANSION
{
	// SHOP_ITEM_BASE										// 상속
	// SHOP_ITEM_EXPANSION									// 상속
	UINT32				_ui32RepairPoint;					// 수리가격
	UINT32				_ui32RepairCash;					// 수리가격
	UINT32				_ui32Durability;					// 내구도
	UINT32				_ui32TermItemID;					// 개수제->기간제로 바뀔 인덱스
	UINT32				_ui32TermAuthSec;					// 개수제->기간제로 바뀔 시간
	UINT32				_ui32Expire;						// 아이템 만료시간
};

struct SHOP_GOODS_LIST
{	
	SHOP_GOODS_DATA*	_pGoodsData;						// 상품 데이타를 담을 저장소
	TItemCnt			_ui16ItemCount;						// 상품이 가지고있는 아이템 개수
	SHOP_ITEM_DATA**	_ppItemData;						// 아이템 데이타		* m_ui8ItemCount
	UINT32*				_pui32ItemArg;						// 아이템 인증개수/시간 * m_ui8ItemCount
};

// GoodsPrice DB기준.
struct SHOP_GOODS_PRICE_DB
{	
	UINT32				m_ui32GoodsID;						// 상품 ID
	UINT8				m_ui8PriceType;						// PRICE_TYPE
	UINT32				m_ui32Point;						// 포인트 
	UINT32				m_ui32Cash;							// 캐쉬
	UINT32				m_ui32Coin;							// 코인
	UINT8				m_ui8Mark;							// SIA_GOODS_MARK
	
};


struct SHOP_DATA
{
	UINT32					m_ui32GoodsCount;						// 상품 개수
	UINT32					m_ui32ItemCount;						// 아이템 개수
	UINT32					m_ui32MatchingCount;					// 매칭 개
	UINT32					m_ui32RepairCount;						// 수리데이타 개수
	UINT32					m_ui32PriceCount;						// 상품 가격 개수

	SHOP_GOODS_DATA			m_pGoods[MAX_SHOP_GOODS_COUNT];			// 상품 리스트 ( 리스트만 )		* m_ui32GoodsCount
	SHOP_ITEM_DATA			m_pItem[MAX_SHOP_ITEM_COUNT];			// 아이템 리스트 ( 리스트만 )		* m_ui32ItemCount
	SHOP_MATCHING_DATA		m_pMatching[MAX_SHOP_MATCHING_COUNT];	// 아이템 매칭 리스트 (리스트만)	* m_ui32MatchingCount
	SHOP_ITEM_REPAIR_DATA	m_pRepair[ MAX_SHOP_ITEM_COUNT ];		// 수리시스템 리스트( 리스트마 )	* m_ui32RepairCount
	SHOP_GOODS_LIST			m_pGoodsList[MAX_SHOP_GOODS_COUNT];		// 상품							* m_ui32GoodsCount 
	SHOP_GOODS_PRICE_DB		m_pPrice[MAX_SHOP_PRICE_COUNT];			// 상품 가격정보
};


#pragma pack( pop )

class CShop: public i3ElementBase
{
	I3_CLASS_DEFINE(CShop);

private:
	// FieldShop information
	FieldShopInfo			m_FieldShop;	

private:
	UINT32					m_ui32Version;		// 버전
	SHOP_DATA				m_ShopData[MAX_SHOP_DATA_BUF];			// 더블 버퍼.
	UINT32					m_ui32VerionIdx;						// 현재 사용하는 상점 데이터 배열 인덱스. m_ShopData 기준,

	SHOP_DATA*				m_pUseShopData;							// 현재 사용하는 상점 데이터.
	SHOP_DATA*				m_pNotUseShopData;						// 현재 사용하지 않는 상점 데이터.


	char					m_strShopMD5Key[NET_MD5_KEY_SIZE+1];	// ShopFile의 MD5Key
	HANDLE					m_hFile;								// ShopFile 핸들.

private:
	BOOL					_SetGoodsList(void);

	UINT32					_GetFindGoodsIdx( SHOP_DATA* pShopData, UINT32 ui32GoodsID  );
	UINT32					_GetFindItemIdx( SHOP_DATA* pShopData, UINT32 ui32ItemID );

public:
	CShop();
	~CShop();

	void					Destory();

	INT32					Update_DB(); 
	void					UpdateField_DB();
	void					ResetSaleCount_T(void)
	{
		for(UINT32 i = 0; i < m_pUseShopData->m_ui32GoodsCount; i++)
		{
			m_pUseShopData->m_pGoods[i]._ui32SaleCount = 0; 
			m_pUseShopData->m_pGoods[i]._ui32GiftCount = 0; 
		}
	}

	// Set은 사용하지 않는 버퍼에다.
	void					SetGoodsCount( UINT32 ui32Count)		{	m_pNotUseShopData->m_ui32GoodsCount		= ui32Count; };
	void					SetItemCount( UINT32 ui32Count)			{	m_pNotUseShopData->m_ui32ItemCount		= ui32Count; };
	void					SetRepairCount( UINT32 ui32Count)		{	m_pNotUseShopData->m_ui32RepairCount	= ui32Count; };
	void					SetMatchingCount( UINT32 ui32Count)		{	m_pNotUseShopData->m_ui32MatchingCount	= ui32Count; };

	void					InitShopAllData(void)					{ i3mem::FillZero( m_pNotUseShopData, sizeof(SHOP_DATA) ); };

	UINT32					GetFieldShopVersion();
	UINT32					GetVersion( void )			{ return m_ui32Version; }; 
	FieldShopInfo*			GetFieldShopInfo()			{ return &m_FieldShop; };
	

	// Get은 사용하는 버퍼에서.
	SHOP_GOODS_DATA	*		GetGoods(INT32 i32Idx)		{ return &m_pUseShopData->m_pGoods[i32Idx]; };
	SHOP_ITEM_DATA	*		GetItem(INT32 i32Idx)		{ return &m_pUseShopData->m_pItem[i32Idx]; };
	SHOP_MATCHING_DATA	*	GetMatching(INT32 i32Idx)	{ return &m_pUseShopData->m_pMatching[i32Idx]; };		
	SHOP_ITEM_REPAIR_DATA*	GetRepair(INT32 i32Idx)		{ return &m_pUseShopData->m_pRepair[i32Idx]; };

	SHOP_GOODS_DATA*		GetGoodsAllData( void )		{ return m_pUseShopData->m_pGoods; };				
	SHOP_ITEM_DATA*			GetItemAllData( void )		{ return m_pUseShopData->m_pItem; };					
	SHOP_MATCHING_DATA*		GetMatchingAllData( void )	{ return m_pUseShopData->m_pMatching; };
	SHOP_ITEM_REPAIR_DATA*	GetRepairAllData( void )	{ return m_pUseShopData->m_pRepair; };		 

	UINT32					GetGoodsCount( void )		{ return m_pUseShopData->m_ui32GoodsCount; }; 
	UINT32					GetItemCount( void )		{ return m_pUseShopData->m_ui32ItemCount; }; 				
	UINT32					GetMatchingCount( void )	{ return m_pUseShopData->m_ui32MatchingCount; }; 		
	UINT32					GetRepairCount( void )		{ return m_pUseShopData->m_ui32RepairCount; }; 			  			

	SHOP_GOODS_DATA*		GetGoodsData( UINT32 ui32GoodsID );
	SHOP_ITEM_DATA*			GetItemData( UINT32 ui32ItemID );	
	SHOP_MATCHING_DATA	*	GetMatchingData(UINT32 ui32GoodsID);	// 단일 상품에만 사용 가능

	// SIA로 부터 받아온 데이터를 담기위해 사용하지 않는 버퍼로.
	SHOP_GOODS_DATA*		GetGoodsAllDataNotUse( void )		{ return m_pNotUseShopData->m_pGoods; };				
	SHOP_ITEM_DATA*			GetItemAllDataNotUse( void )		{ return m_pNotUseShopData->m_pItem; };					
	SHOP_MATCHING_DATA*		GetMatchingAllDataNotUse( void )	{ return m_pNotUseShopData->m_pMatching; };
	SHOP_ITEM_REPAIR_DATA*	GetRepairAllDataNotUse( void )		{ return m_pNotUseShopData->m_pRepair; };		


	char*					GetShopMD5Key(void)					{  return m_strShopMD5Key; };
	void					SetShopMD5Key(char* strShopMD5Key);
	
	SHOP_DATA*				GetNotUseShopData( void )	{ return m_pNotUseShopData;	};
	void					ChangeShopVersion(UINT32 ui32NewShopVersion);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////		(구) SIA	///////////////////////////////////////////////////////////////
	SHOP_GOODS_LIST *		GetGoodsByID( UINT32 ui32GoodsID );					// ID 를 이용하여 Goods 를 얻어온다.
	SHOP_ITEM_DATA *		GetItemByID( UINT32 ui32ItemID );					// ID 를 이용하여 Item 를 얻어온다.

	BOOL					CreateShopFile();
	BOOL					SaveShopFile();
	BOOL					DeleteShopFile();

	void					MakeShopMD5Key(void);

	// 단일 상품만 가능
	UINT8					GetItemInfo_byGoodsID(  UINT32 ui32GoodsID, UINT32 *pui32ItemID, UINT32 *pui32ItemArg );	// GoodsID로 ItemID와 ItemArg를 얻어온다.

	// 단일, 셋트 모두 가능. 절대 소스 수정이 귀찮아서 통합안시켰음.
	UINT8					GetItemInfo_byGoodsID(  UINT32 ui32GoodsID,  ST_ITEM * pItem );	// GoodsID로 ItemID와 ItemArg를 얻어온다.

	BOOL					IsSalesItem(UINT32 ui32ItemID);	// 판매중인 상품인지 ItemID로 체크.
};

#endif