#if !defined( __UI_SHOP_FUNCTION_H__)
#define __UI_SHOP_FUNCTION_H__

enum ItemStatus;
enum ShopItemCategory;
enum UIShop_Item_Category;

namespace buy
{
	struct item;
	enum PurchaseRouteType;
}

namespace item_def
{
	enum PartsType_ShopPreview;
}

namespace usf
{
	namespace payment { }
}

namespace usf			// UIShopFunction 네임스페이스..
{
	extern UINT8 g_price_idx;
	void SetPriceIdx(UINT8 price_idx);

	/***********************/
	i3::rc_wstring GetShopItemStatusString(ItemStatus status);
	bool IsSetItem( const SHOP_GOODS* pGoods);	
	
	/*********************/
	void		GetShopItemDescription( i3::rc_wstring& outName, const SHOP_GOODS_PACK* pack);
	void		GetShopItemDescription( i3::rc_wstring& outName, const SHOP_GOODS* goods);
	void		GetShopItemDescription( i3::rc_wstring& outName, T_ItemID itemID);

	/*********************/
	void		GetShopItemName( i3::rc_wstring& outName, const SHOP_GOODS_PACK* pack);
	void		GetShopItemName( i3::rc_wstring& outName, const SHOP_GOODS* goods);
	void		GetShopItemName( i3::rc_wstring& outName, T_ItemID itemID);
	void		GetShopItemName( i3::rc_wstring& outName, ITEM_INFO item_info);
	void		GetShopItemMultiCharaName( i3::rc_wstring& outName, ITEM_INFO item_info);

	/*********************/
	i3::rc_wstring GetItemClassName( T_ItemID itemID); //주무기, 보조무기, 근접무기.., 헤드기어, 페이스기어, 상의, 하의... (없는 경우 "" 반환)
	i3::rc_wstring GetItemUsageName_Only_PrimaryWeapon( T_ItemID itemID); //돌격소총, SMG, 저격소총..(없는 경우 "" 반환)

	/*********************/
	i3::rc_wstring GetItemLineUpName_Only_Parts(item_def::PartsType_ShopPreview line_up);
	i3::rc_wstring GetItemLineUpName_Only_Parts( T_ItemID itemID);

	/*********************/
	void	GetShopPeriodCountString( T_ItemID itemID, i3::wstring& outstr);
	void	GetShopPeriodCountString( T_ItemID itemID, UINT8 authType, UINT32 authArg, i3::wstring& outStr);
	void	GetShopPeriodCountString(const SHOP_GOODS* pGoods, i3::wstring& outstr);
	void	GetShopPeriodCountString(SHOP_GOODS_PACK* pack, i3::wstring& outstr, const wchar_t* word= L";");
	void	GetShopPeriodCountStringByGoodsID(const T_GoodsID goodsID, i3::wstring& outstr);
	void GetShopPeriodCountString_only_Item( T_ItemID itemID, i3::wstring& outName); //아이템 항목만 가능하다.

	/*********************/
	ShopItemCategory	GetShopItemCategory( T_ItemID itemID);
	ShopItemCategory	GetShopItemCategory(const SHOP_GOODS* goods);

	/*********************/
	ItemStatus	 GetItemStatus( T_ItemID itemID, T_ItemDBIdx ware_idx);	//인벤용
	ItemStatus	 GetItemStatus(const SHOP_GOODS_PACK* goods_pack); //상점용

	/*********************/
	bool IsExtensionItem( T_ItemID itemID);
	bool IsExtensionItem( const SHOP_GOODS* pGoods);

	/*********************/
	bool HasExtensionDescImage( T_ItemID itemID);
	bool HasExtensionDescImage( SHOP_GOODS* pGoods);


	/*********************/
	UIShop_Item_Category GetItemItemCategory( SHOP_GOODS* goods);
	UIShop_Item_Category GetItemItemCategory( T_ItemID itemID);

	/*********************/
	void	FormatHaveItemSecTermText(UINT32 SecTerm, i3::wstring& outDay);		// 초단위로 모두 통일..
	
	void	GetGoodsUseDay(ItemStatus status, bool IsRepairItem, bool IsCountItem, UINT8 Type, UINT32 Arg, 
		i3::wstring& outStatus, bool includeState = true);
	void	GetItemUseDay(ItemStatus status, bool IsRepairItem, T_ItemID itemID, T_ItemDBIdx ware_idx, 
		i3::wstring& outStatus, bool includeState = true);	//inven 용

	void	GetGoodsUseDay_Zero(bool IsRepairItem, bool IsCountItem, i3::stack_wstring& outStatus);

	i3::rc_wstring GetCharacterItemCategoryString(INT32 characID);
	void	GetItemItemCategoryString(i3::wstring& out_wstr, UIShop_Item_Category cat, bool is_set_item);

	void GetShopGoodInfo(const SHOP_GOODS* pGoods, GOODS_PRICE_INFO & info);
	bool GetShopPriceString(SHOP_GOODS* pGoods, i3::wstring& outResult, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);
	bool GetShopPriceStringComma(SHOP_GOODS* pGoods, i3::wstring& inoutwstrResult, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);
	void	 GetShopPriceAndMyMoney(SHOP_GOODS* pGoods, UINT32& price, UINT32& mymoney, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);

	bool CheckGoodsPackBothPrices(SHOP_GOODS_PACK* pack);
	void	 CalcGoodsPackMapByPriceType( SHOP_GOODS_PACK* pack, INT32& out_num, INT32 (&out_array)[4], GOODS_PRICE_TYPE t = GOODS_PRICE_ALL );
	bool CalcGoodsPackFirstIdxByPriceType(  SHOP_GOODS_PACK* pack, INT32& out_idx, GOODS_PRICE_TYPE t = GOODS_PRICE_ALL);

	// 아이템 당 개수/날짜별 구매카테고리에 들어갈 제목문자열..
	INT32		GetGoodsCount(SHOP_GOODS_PACK* pack);
	SHOP_GOODS*	GetGoodsFromPack(SHOP_GOODS_PACK* pack, INT32 goods_idx);

/*
	void GetWeaponSpecificValues( T_ItemID itemID, 
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
		INT32& Recoil_H, INT32& Speed, char* strRange, const size_t sizeRangeMax, 
		char* strBulletCount, const size_t sizeBulletCountMax);
	void GetWeaponSpecificValues( SHOP_GOODS_PACK* pack, 
		INT32& Damage, INT32& Rapid, INT32& Accuracy, INT32& Recoil_V, 
		INT32& Recoil_H, INT32& Speed, char* strRange, const size_t sizeRangeMax,
		char* strBulletCount, const size_t sizeBulletCountMax );
*/	
/*
	void SetShopPointInfo(SHOP_GOODS * pGoods, GOODS_PRICE_TYPE price_type, 
		i3::rc_wstring& out_wstrSubjectPointCash,
		char* szHavePointCash, size_t sizeHavePointCash,
		char* szBuyPointCash, size_t sizeBuyPointCash,
		char* szRemainPointCash, size_t sizeRemainPointCash,
		const char* szMoneySuffix = "",
		const INT32	nCouponIdx = -1);
*/

	bool GetDurability( T_ItemID itemID, T_ItemDBIdx ware_idx, REAL32& rate );	// T_ItemDBIdx ( DB 인덱스 ) 를 모르면 0을 넣어주면 된다.

	//Goods.pef에 등록되지 않는 아이템을 상점에서 구매못하게 한다.
	bool CheckValidGoods_in_Pef( T_ItemID itemID);
	bool CheckValidGoods_in_Pef( const SHOP_GOODS_PACK * pack);

	//영구구입 아이템을 보유하고 있을 경우를 검사한다.
	enum ValidBuyType
	{
		vbt_Invalid = -1,
		
		vbt_None= 0,
		vbt_Ok = vbt_None,
		vbt_Perpetuity,	//영구제 상품이며, 이미 구입된 상태
		vbt_Only_Clan_master, //클랜 마스터만 구입 가능한 아이템
		vbt_Limited_Clan_member, //클랜원 증가 아이템은 최대 회원수를 넘을시 구매 불가 
		vbt_Pushed_Perpetuity,	//영구제 상품은 카트에 하나만 담을 수 있다.
		vbt_Disable_Buy_Parts_Any_more,	//파츠는 보유 캐릭터 수 보다 많이 구입 할 수 없습니다.
		vbt_Exceed_MaxPurchaseableCharacters, // 캐릭터 최대보유개수(9개) 이상으로 구매가 불가능합니다.
	};
	bool IsValidBuyPartsGoods(const SHOP_GOODS_PACK* pack);
	ValidBuyType IsValidBuyGoods(const SHOP_GOODS_PACK * pack);


	ValidBuyType IsValidBuyGoods_InCart(const SHOP_GOODS_PACK * pack);

	INT32 GetValidPurchaseCount(const SHOP_GOODS_PACK * pack, INT32 count);

	void Message_BuyResert(ValidBuyType fb, bool chat_msg = false);

	//아이템 사용 가능 여부를 검사한다.
	enum ValidUseType
	{
		vut_None= 0,
		vut_Ok = vut_None,
		vut_Duplicate,	//중복 사용할 수 없다.
	};
	ValidUseType IsUseableItem( T_ItemID itemID );
	void Message_UseResert(ValidUseType fb, bool chat_msg = false);

	UINT32 GetDisableDuplicateUseItemPair( T_ItemID item_id); //중복 사용할 수 없는 아이템 itemID 반대편 아이템을 알려준다. 없는 경우 0리턴
	
	bool CheckBuyItemPeriod( const SHOP_GOODS_PACK* pack, UINT32 buySlot );		// 인벤아이템 제한된 만료날짜 16.11.11 김민제


	bool PopupRestriction(UINT32 ui32ItemID);				// buyutil.h에 적당치 않으므로 이쪽으로 일단 옮겼습니다..
}

namespace usf
{
	namespace payment
	{
		//SHOP_GOODS_PACK에서 구입 가능한 금액을 반환한다.
		//Cash String + Point String 순으로 나온다.
		//예) POINT
		i3::wstring get_price_combo_string( const SHOP_GOODS_PACK* packs );
	
		//buy::item 구매 금액을 변경한다.
		
	}


	bool CheckValidGoods(const SHOP_GOODS_PACK * pack, bool chat_msg);	
	bool CheckValidGoods_InCart(const SHOP_GOODS_PACK * pack);

}

#endif