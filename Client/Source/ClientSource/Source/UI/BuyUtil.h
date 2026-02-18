#pragma once

namespace item_def
{
	enum PartsType_ShopPreview;
}

namespace buy
{

	class DisplayedGoods
	{
	public:
		DisplayedGoods(const SHOP_GOODS_PACK* pPack);
	
		const SHOP_GOODS_PACK*  GetGoodsPack() const { return m_Pack;  }

		void					SetCurrentDisplayIndex(INT32 displayIndex) { m_currDisplayIndex = displayIndex; }
		void					SetCurrentDisplayIndexByPriceType(GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType);
		
		INT32					GetCurrentDisplayIndex() const { return m_currDisplayIndex; }
		INT32					GetCurrentDisplayIndexByPriceType() const;

		INT32					GetCurrentPackIndex() const { return  m_vecDisplayPackIndex[m_currDisplayIndex]; }
		const SHOP_GOODS*		GetCurrentGoods() const {  return  m_Pack->GetGoods(GetCurrentPackIndex()); }
		GOODS_PRICE_TYPE		GetCurrentPriceType() const { return (m_currDisplayIndex < m_numPointItem) ? GOODS_PRICE_POINT : GOODS_PRICE_CASH; }
		const SHOP_GOODS*		GetPackByDisplayIndex(INT32 displayIdx) const { return m_Pack->GetGoods(m_vecDisplayPackIndex[displayIdx]); }
		INT32					GetDisplayCount() const { return m_vecDisplayPackIndex.size(); }
		
		T_ItemDBIdx				GetItemDBIndex() const { return m_itemDBIndex;  }
		void					SetItemDBIndex(T_ItemDBIdx dbIndex) { m_itemDBIndex = dbIndex;  }
		bool					IsItemDBIndexValid() const { return m_itemDBIndex != -1;  }
	private:
		const SHOP_GOODS_PACK*	m_Pack;
		i3::vector<INT32>		m_vecDisplayPackIndex;
		INT32					m_numPointItem;				// UI표시시엔 포인트->캐시 순으로 표시되며, 포인트아이템갯수가 필요하긴 함..
		INT32					m_currDisplayIndex;	// 기존것과 달리, price_type의 구분은 m_numPointItem의 유효범위 여부로 따지기로 합니다.

		T_ItemDBIdx				m_itemDBIndex;		// 인벤토리에서만 쓰일듯한데... 단일전역변수는 용납이 안되서, 이쪽으로 넣습니다..

	};
	
	typedef i3::shared_ptr<DisplayedGoods>			DisplayedGoodsPtr;
	typedef i3::vector< buy::DisplayedGoodsPtr >	DisplayedGoodsList;

	DisplayedGoodsPtr			MakeDisplayedGoodsPtr(const SHOP_GOODS_PACK* pPack);

}

namespace buy
{
	enum PurchaseRouteType 
	{ 
		PurchaseRoute_Immediate, 
		PurchaseRoute_Extend, 
		PurchaseRoute_Cart, 
		PurchaseRoute_Preview, 
		PurchaseRoute_Max 
	}; //즉시 구매, 기간 연장 구매, 카트 구매, 파츠 미리보기 구매

	struct PurchaseInfo
	{
		i3::vector< DisplayedGoodsPtr > vecGoods;	
		PurchaseRouteType		route;
	};

	void ClearPreview(); //주위요망 -> 이제 그딴거 없다...헷갈리는 코드들은 이제 그만...
	void ClearCart();
	void ClearImmediate();
	void Clear_ByPurchaseRoute(buy::PurchaseRouteType routeType);

	void PushImmediate(const SHOP_GOODS_PACK* pack);		// 뭐..프라이스인덱스 인수...그딴거 가독성만 드럽게 한다...
	void PushImmediate(const SHOP_GOODS_PACK* pack, INT32 displayIndex);
	void PushImmediate(const SHOP_GOODS_PACK* pack, GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType);
	void PushImmediateWithDBIndex(const SHOP_GOODS_PACK* pack, T_ItemDBIdx dbIndex);


	void AddShopGoodPackToPreview(const SHOP_GOODS_PACK* pack);
	void AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack);
	void AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack, INT32 displayIndex);
	
	void RemovePreview(item_def::PartsType_ShopPreview partsType);
	void RemoveCart(size_t idx);				// 이게 필요가 있는지 모르겠다..일단 작성..

	void RemoveByPurchaseRoute(buy::PurchaseRouteType buy_type, size_t idx);


	INT32 RemoveDisplayedGoodsInCart(const SHOP_GOODS* pGoods);			// 호환성때문에 따라가는 수정을 했으나, 내부구현이 좀 ㅈ같다...비교구문 내용이 주관적이다..

	bool  IsExistedShopGoodPackInCurrentCharaCart(const SHOP_GOODS_PACK* pPack);

	INT32 GetCount_Immediate();
	INT32 GetCount_Preview();
	INT32 GetCount_CurrCharaCart();
		
	INT32 GetCount_ByPurchaseRouteType(buy::PurchaseRouteType route);
	INT32 GetNumSlot_Preview();
	
	DisplayedGoods* GetDisplayedGoods_Immediate();
	DisplayedGoods* GetDisplayedGoods_Immediate(INT32 idx);
	DisplayedGoods* GetDisplayedGoods_ByPreviewSlot(item_def::PartsType_ShopPreview slot); // 실갯수가 아닌 슬롯인덱스입니다..
	DisplayedGoods* GetDisplayedGoods_ByPreviewActive(INT32 idxActive);
	DisplayedGoods*	GetDisplayedGoods_ByCurrCharaCart(INT32 idx);
	DisplayedGoods* GetDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType, INT32 idx);

	// UI상에서 피킹한 혹은 모아놓은 물품 목록을 복사하여 빼옵니다. ( 얕은복사로 처리해둡니다...)
	void			GatherDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType, 
														i3::vector< buy::DisplayedGoodsPtr>& outList);

	void			UpdateItemInfoListWithPreview(i3::vector< ITEM_INFO >& item_info_list);

		
	//  이상한 상품은 제거합니다.  (캐릭터 갯수 대비 동일 item파츠 갯수가 넘어가면 목록에 안들어가도록 조절함...)
	void			RemoveInvalidDisplayedGoods_CartPreview(buy::PurchaseRouteType routeType, bool& out_is_invalid);

	
}
