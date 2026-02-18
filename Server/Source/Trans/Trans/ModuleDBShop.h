#ifndef __MODULE_DB_SHOP_H
#define __MODULE_DB_SHOP_H

#include "ModuleDBShop_Script.h"
#include "S2RingBuffer.h"

#define DEF_DB_TIME_CHECK

struct LOG_SALE_LIST
{
	//UINT32	_ui32GameID;	//필요없음. 중간에 추가가 없다는 가정으로 만들어져 있다. 
	UINT32	_ui32SaleCount;
}; 


struct FieldShopInfo;

class CModuleDBShop :	public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleDBShop );
private		: 
	//-------------------------------------------------------------------------------------
	HANDLE					m_HDBThread;				//DataBase Main Thread

	HANDLE					m_hThreadWebGift;			//DataBase 

	HANDLE				*	m_pHThread;					//DataBase Thread N개 

	INT32				*	m_pi32ThreadIdx;			//DataBase Thread N개 

	BOOL					m_bIsRunningDB;
	BOOL					m_bIsRunningWebGift;
	BOOL				*	m_pbIsRunning;

	//-------------------------------------------------------------------------------------
	CADODatabase		*	m_pADOdb_DBMAIN_Shop;		//DataBase Main Thread 에서 사용 내부에서 저장이 필요한 LOG나 상품리스트를 가지고 온다. 
	CADODatabase		*	m_pADOdb_WEBGIFT;			//DataBase WebGift Thread 에서 사용
	CADODatabase		**	m_ppADOdb_DATABASE;			//DataBase Thread에서 사용 ( N개 사용 )

	S2RingBuffer		**	m_ppInShopItemBuffer;		//Item				Task와 통신
	S2RingBuffer		**	m_ppOutShopItemBuffer;		//	
	UINT32					m_ui32OutShopItemThreadIdx;	//

	S2RingBuffer		**	m_ppInShopBuyBuffer;		//Buy				Task와 통신
	S2RingBuffer		**	m_ppOutShopBuyBuffer;		//	
	UINT32					m_ui32OutShopBuyThreadIdx;	//

	S2RingBuffer		**	m_ppInShopBuffer;			//Shop				Task와 통신
	S2RingBuffer		**	m_ppOutShopBuffer;			//	
	UINT32					m_ui32OutShopThreadIdx;		//

	S2RingBuffer		**	m_ppInInventoryBuffer;		//Inventory			Task와 통신 
	S2RingBuffer		**	m_ppOutInventorytBuffer;	//
	UINT32					m_ui32OutInventoryThreadIdx;//

	S2RingBuffer		**	m_ppInGiftBuffer;			//Gift 선물하기		Task와 통신 
	S2RingBuffer		**	m_ppOutGiftBuffer;			//
	UINT32					m_ui32OutGiftThreadIdx;		//

	S2RingBuffer		**	m_ppInGiftListBuffer;		//Gift List			Task와 통신 
	S2RingBuffer		**	m_ppOutGiftListBuffer;		//
	UINT32					m_ui32OutGiftListThreadIdx;	//

	// WebGift

	S2RingBuffer		**	m_ppInWebGiftBuffer;		//Web Gift			TASK용 버퍼  
	S2RingBuffer		**	m_ppOutWebGiftBuffer;		//Web Gift			TASK용 버퍼
	UINT32					m_ui32OutWebGiftThreadIdx;	//

	S2RingBuffer		**	m_ppInRepairBuffer;			//Repair				Task와 통신
	S2RingBuffer		**	m_ppOutRepairBuffer;		//
	UINT32					m_ui32OutRepairThreadIdx;	//

	// GiftCoupon
	S2RingBuffer		**	m_ppInCouponBuffer;			//GiftCoupon		Task와 통신
	S2RingBuffer		**	m_ppOutCouponBuffer;		//
	UINT32					m_ui32OutCouponThreadIdx;	//

	// Gacha
	S2RingBuffer		**	m_ppInGachaBuffer;			//Gacha				Task와 통신
	S2RingBuffer		**	m_ppOutGachaBuffer;			//	
	UINT32					m_ui32OutGachaThreadIdx;	//

	BOOL					m_bUpdateShopInfo;

	CDBConfig*				m_pControlDBConfig;
	CDBConfig*				m_pSIAUserDBConfig[DATABASE_THREAD_COUNT];
	CDBConfig*				m_pGiftDBConfig;

private		: 	
	void	_EXEC_ShopBuy(		INT32 i32ThreadIdx,	DB_SHOP_GOODS * pIn);	// 캐쉬 구매 : Link 결제전 DB 체크용으로 STAND BY 로우를 무조건 INSERT합니다.
	void	_EXEC_ShopBuyLink(	INT32 i32ThreadIdx,	DB_SHOP_GOODS * pIn);	// 캐쉬 구매 : Link 결제후 STAND BY 로우를 UPDATE 또는 DELETE 합니다.
	void	_EXEC_GoodsInsert(	INT32 i32ThreadIdx, DB_SHOP_GOODS * pIn );		// 포인트 구매 
	INT32	_EXEC_ItemInsert(	INT32 i32ThreadIdx, T_UID TUID, DB_ITEM* pDBItem, UINT8 ui8JobAction = TYPE_SHOP_ITEM_INSERT, T_ItemDBIdx	TItemDBIdx = 0 , UINT8 * pui8MultiSlotState = NULL, bool* pbAddCharaSlot = NULL);
	INT32	_EXEC_ItemInsert(	INT32 i32ThreadIdx, T_UID TUID, SEND_GIFT* pSendItem, UINT8 ui8JobAction = TYPE_SHOP_ITEM_INSERT, T_ItemDBIdx	TItemDBIdx = 0, UINT8 * pui8MultiSlotState = NULL, bool* pbAddCharaSlot = NULL);
	void	_EXEC_ItemDelete(	INT32 i32ThreadIdx,	DB_SHOP_IN * pInsertItem, DB_SHOP_OUT * pOut); 
	void	_EXEC_ItemAuth(		INT32 i32ThreadIdx,	DB_SHOP_IN * pInsertItem, DB_SHOP_OUT * pOut); 
	void	_EXEC_ItemLoopAuth( INT32 i32ThreadIdx, DB_SHOP_IN * pInsertItem, DB_SHOP_OUT * pOut);	// 어떤 아이템 M 인증에 대해서 N번 만큼 For문 돌며 진행하고 싶은 경우. 한 번만 인증하기 위함
	void	_EXEC_GetItem(		INT32 i32ThreadIdx,	T_UID i64UID, DB_INVENTORY_OUT * pOut);

	// Gift
	INT32	_EXEC_CheckGiftList(INT32 i32ThreadIdx, T_UID i64UID, bool& bGiftAutoAuth );	// 선물함 체크 및 인벤토리 강제지급.( 갯수 초과, 만료날짜 )
	void	_EXEC_GiftInsert(	INT32 i32ThreadIdx,	SEND_GIFT * pGift ); // 선물 지급

	void	_EXEC_GetGiftList(	INT32 i32ThreadIdx,  DB_GIFT_OUT * pOut ); 	// 선물함 조회. 
	void	_EXEC_GiftBuyLink(	INT32 i32ThreadIdx,	 DB_GIFT_OUT * pOut );	// 선물 구매후 상태 UPDATE
	INT32	_EXEC_GiftBuyLink_Inven(INT32 i32ThreadIdx, DB_GIFT_OUT * pOut);	// 선물 구매후 상태 UPDATE(선물함OFF시 인벤토리 지급 업데이트)
	void	_EXEC_GiftAuth(		INT32 i32ThreadIdx,	 DB_GIFT_OUT * pOut );	// 선물 인증.( 인벤토리 지급 )
	void	_EXEC_GiftRead(		INT32 i32ThreadIdx,	 DB_GIFT_OUT * pOut );	// 선물 읽기.
	void	_EXEC_GiftDelete(	INT32 i32ThreadIdx,	 DB_GIFT_OUT * pOut );	// 선물 삭제.


	void	_EXEC_Repair(		INT32 i32ThreadIdx,	DB_REPAIR * pIn);
	void	_EXEC_RepairLink(	INT32 i32ThreadIdx,	DB_REPAIR * pIn);
	
	void	ErrorDBShop(	INT32 i32ThreadIdx, wchar_t * wstrSql ); 
	void	_DataBaseResultFailLog(INT32 i32ThreadIdx, wchar_t * wstrSql );

	INT32	_EXEC_DeleteRow(	INT32 i32ThreadIdx, T_UID TUID, T_ItemDBIdx	TItemDBIdx );

	INT32	_GiftAutoAuth( INT32 i32ThreadIdx, T_UID TUID, DB_ITEM *pItem, S2MO_GIFT *pGift, bool& bGiftAutoAuth, ENUM_TYPE_GIFT_DELIVER eDeliver );

	INT32	_EXEC_GachaBuy(		INT32 i32ThreadIdx,	DB_GACHA * pIn);

	//선물함 Off 시 사용하는 함수
	void	_EXEC_GiftShop_Insert(INT32 i32ThreadIdx, SEND_GIFT * pGift); 
	void	_EXEC_GiftShop_Link(  INT32 i32ThreadIdx, DB_GIFT_OUT * pOut);

	//TASK THREAD에서 요청 합니다. 
public		: 
	
	CModuleDBShop(); 
	virtual ~CModuleDBShop();
	INT32 OnCreate(void);
	bool OnDestroy(void); 
	void OnWorking(INT32 i32ThreadIdx);
	void OnMainWorking(void); 

	//---------------------------------------------------------------------------------------
	//SHOP 용 
	INT32	GetShopVersion_M(void); 
	INT32	GetShopData_M(void);
 

	// FiledShop 
	bool	GetFieldShopInfo_M( FieldShopInfo* pFieldShopInfo ); 

	//---------------------------------------------------------------------------------------
	//Shop Info Update 
	void UpdateShopInfo(void)		{ m_bUpdateShopInfo = TRUE; }	

	//---------------------------------------------------------------------------------------
	//Item용 
	BOOL	InShopItem( DB_SHOP_ITEM * pIn );					//Item

	//Pop Item Data 
	INT32	GetOutShopItemCount_T(void); 
	void	GetOutShopItemIdx_T(void); 
	DB_SHOP_ITEM *	GetOutShopItemData_T(void); 


	//---------------------------------------------------------------------------------------
	//Buy용 
	BOOL	InShopBuy( DB_SHOP_GOODS * pIn );					//Buy

	//Pop Buy Data 
	INT32	GetOutShopBuyCount_T(void); 
	void	GetOutShopBuyIdx_T(void); 
	DB_SHOP_GOODS *	GetOutShopBuyData_T(void); 

	//---------------------------------------------------------------------------------------
	//USER SHOP용 
	BOOL	InInsertItem( DB_SHOP_IN * pIn );			//Insert Inventory	

	//Pop Shop Data 
	INT32	GetOutShopCount_T(void); 
	void	GetOutShopIdx_T(void); 
	DB_SHOP_OUT *	GetOutShopData_T(void); 

	//---------------------------------------------------------------------------------------
	//USER INVENTRORY용
	BOOL	InInventory( DB_INVENTORY_IN* pIn ); 

	////Pop Inventory Data 
	INT32				GetOutInventoryCount_T(void);
	void				GetOutInventoryIdx_T(void);
	DB_INVENTORY_OUT *	GetOutInventoryData_T(void);

	//---------------------------------------------------------------------------------------
	//GIFT 용 
	BOOL				InGift( DB_GIFT_IN * pIn);
	BOOL				InInsertGift( SEND_GIFT * pIn);

	////Pop Gift Data 
	INT32				GetOutGiftCount_T(void);
	void				GetOutGiftIdx_T(void);
	SEND_GIFT *			GetOutGiftData_T(void);

	////Pop Gift List Data 
	INT32				GetOutGiftListCount_T(void);
	void				GetOutGiftListIdx_T(void);
	DB_GIFT_OUT *		GetOutGiftListData_T(void);

	////Pop Gift Web Data 
	INT32				GetOutWebGiftCount_T(void);
	void				GetOutWebGiftIdx_T(void);
	DB_WEB_GIFT_OUT *	GetOutWebGiftData_T(void);

	//// Repair
	BOOL				InRepair( DB_REPAIR * pIn );
	INT32				GetOutRepairCount_T(void);
	void				GetOutRepairIdx_T(void);
	DB_REPAIR *			GetOutRepairData_T(void);

	//---------------------------------------------------------------------------------------
	// GiftCoupon 용
	BOOL				InCoupon( DB_COUPON * pIn );
	INT32				GetOutCouponCount_T(void);
	void				GetOutCouponIdx_T(void);
	DB_COUPON *			GetOutCouponData_T(void);

	// Gacha
	BOOL	InGacha( DB_GACHA * pIn );				
	INT32	GetOutGachaCount_T(void); 
	void	GetOutGachaIdx_T(void); 
	DB_GACHA *	GetOutGachaData_T(void); 

	// Working for Web
	bool				OnCreateWebGift(void);
	void				OnWorkingWebGift(void);
};

extern CModuleDBShop * g_pModuleDBShop;
#endif 
