#include "pch.h"
#include "BuyUtil.h"

#include "UIItemInfoUtil.h"


#include "UIUtil.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include <functional>



#include "UIShopFunction.h"

#include "i3Base/itl/algorithm/remove_if.h"
#include "i3Base/itl/range/algorithm/find_if.h"

namespace buy
{
	namespace
	{
		bool BuildDisplayList(const SHOP_GOODS_PACK* pPack,
			i3::vector<INT32>&	outvecDisplayPackIndex, INT32& outnumPointItem)
		{
			if (pPack == NULL)
				return false;

			const INT32 numGood = pPack->GetGoodsCount();

			for (INT32 i = 0; i < numGood; ++i)
			{
				const SHOP_GOODS* pGood = pPack->GetGoods(i);
				if (pGood->_SaleType == GOODS_SALE_EVENT)
					continue;

				if (pGood->GetPriceType(usf::g_price_idx) & GOODS_PRICE_POINT)
					outvecDisplayPackIndex.push_back(i);
			}

			outnumPointItem = outvecDisplayPackIndex.size();

			for (INT32 i = 0; i < numGood; ++i)
			{
				const SHOP_GOODS* pGood = pPack->GetGoods(i);

				if (pGood->_SaleType == GOODS_SALE_EVENT)
					continue;

				if (pGood->GetPriceType(usf::g_price_idx) & GOODS_PRICE_CASH)
					outvecDisplayPackIndex.push_back(i);
			}

			return !outvecDisplayPackIndex.empty();
		}

	}

	DisplayedGoodsPtr	MakeDisplayedGoodsPtr(const SHOP_GOODS_PACK* pPack) 
	{
		if (pPack == NULL) return DisplayedGoodsPtr();
		DisplayedGoodsPtr new_ptr = i3::make_shared<DisplayedGoods>(pPack);
		if (new_ptr->GetDisplayCount() == 0)
			new_ptr.reset();
		return new_ptr;
	}
	
	DisplayedGoods::DisplayedGoods(const SHOP_GOODS_PACK* pPack) : m_Pack(pPack), m_currDisplayIndex(0),
		m_numPointItem(0), m_itemDBIndex(-1)
	{
		BuildDisplayList(pPack, m_vecDisplayPackIndex, m_numPointItem);
	}

	void	DisplayedGoods::SetCurrentDisplayIndexByPriceType(GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType)
	{
		m_currDisplayIndex = (priceType == GOODS_PRICE_CASH) ? (m_numPointItem + displayIndexByPriceType) : displayIndexByPriceType;
	}

	INT32	DisplayedGoods::GetCurrentDisplayIndexByPriceType() const
	{
		return (m_currDisplayIndex >= m_numPointItem) ? (m_currDisplayIndex - m_numPointItem) : m_currDisplayIndex;
	}
	

}


//------------------------------////------------------------------//
namespace value
{


	typedef i3::vector< buy::DisplayedGoodsList > Chara_ItemList;

	buy::DisplayedGoodsPtr   immediate;			// 이것은 유효성 점검이 필요함..
	buy::DisplayedGoodsPtr	  preview[item_def::PartsType_ShopPreview_Max];	// 이거면 충분치 않은건가..유효성 점검 필요함..

	buy::DisplayedGoodsList	activePreviewList;		// 7개 중 슬롯 유효한 것만 모음...실시간 처리하까? 갯수가 적으니 통째로 업데이트하는 것이 효과적.



	value::Chara_ItemList	cart;				// 장바구니 겁나 복잡...캐릭터가 여러개였던 V3구조때문에 이중벡터로 됨...

	struct init_value
	{
	//	enum { MAX_CART_ITEMS = 9 }; 
		init_value()
		{
			value::cart.resize(1);						// 일단 1개만 있으면 되며, 필요할때 리사이징 하도록 합니다.
		}
	} _init_value;
}

//------------------------------////------------------------------//
namespace util
{

	buy::DisplayedGoodsList& GetDisplayedGoodsList_Cart();
	
}

buy::DisplayedGoodsList& util::GetDisplayedGoodsList_Cart()
{
	if (g_pEnvSet->IsV2Version() == false)
		return value::cart[0];

	INT32 idx =	CCharaInfoContext::i()->GetMainCharaSlotIdx(TEAM_NONE);			// 이거 문제 있음...인터페이스가...

	if (idx >= INT32(value::cart.size()))
	{
		I3_BOUNDCHK(idx, MULTI_SLOT_COUNT);
		value::cart.resize(idx + 1);				// 필요할때만 생성..
	}

	return value::cart[idx];
}


void buy::ClearImmediate()
{
	value::immediate.reset();
}

void buy::ClearPreview()
{
	i3::for_each(value::preview, value::preview + item_def::PartsType_ShopPreview_Max,
		i3::mem_fun_ref<void, DisplayedGoodsPtr>(&buy::DisplayedGoodsPtr::reset));	// 오버로딩함수여서, 템플릿 매개변수 직접 대입이 요구되었다.

	value::activePreviewList.clear();		
}

void buy::ClearCart()
{
	buy::DisplayedGoodsList& goodsList = util::GetDisplayedGoodsList_Cart();
	goodsList.clear();				// 귀찮어..걍 지워...
}

void buy::Clear_ByPurchaseRoute(buy::PurchaseRouteType route)
{
	static void (*fn[])() = 
	{ 
		buy::ClearImmediate, buy::ClearImmediate, buy::ClearCart, buy::ClearPreview
	};

	(*fn[route])();
}

//-- push

void buy::PushImmediate(const SHOP_GOODS_PACK* pack)	// 뭐..프라이스인덱스 인수...그딴거 가독성만 드럽게 한다...
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
}

void buy::PushImmediate(const SHOP_GOODS_PACK* pack, INT32 displayIndex)
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
	if (value::immediate)
		value::immediate->SetCurrentDisplayIndex(displayIndex);
}

void buy::PushImmediate(const SHOP_GOODS_PACK* pack, GOODS_PRICE_TYPE priceType, INT32 displayIndexByPriceType)
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
	if (value::immediate)
		value::immediate->SetCurrentDisplayIndexByPriceType(priceType, displayIndexByPriceType);
}

void  buy::PushImmediateWithDBIndex(const SHOP_GOODS_PACK* pack, T_ItemDBIdx dbIndex)
{
	value::immediate = MakeDisplayedGoodsPtr(pack);
	if (value::immediate)
		value::immediate->SetItemDBIndex(dbIndex);
}

namespace
{
	void UpdateActivePreviewListWithPreviewArrayImp()			// 전역변수들이 사용되다보니..인수는 쓰지 않도록 햇습니다..
	{
		value::activePreviewList.clear();

		for (INT32 i = 0; i < item_def::PartsType_ShopPreview_Max; ++i)
		{
			const buy::DisplayedGoodsPtr& goods_ptr = value::preview[i];
			if (goods_ptr)
			{
				value::activePreviewList.push_back(goods_ptr);
			}
		}
	}

}


void buy::AddShopGoodPackToPreview(const SHOP_GOODS_PACK* pack)
{
	item_def::PartsType_ShopPreview partsType = item_def::get_shop_preview_parts_type(pack->GetGoods(0)->_ItemID);
	I3ASSERT_RETURN(partsType != item_def::PartsType_ShopPreview_None);
	
	DisplayedGoodsPtr newGood = buy::MakeDisplayedGoodsPtr(pack);
	value::preview[partsType] = newGood;									// 이렇게 간단한 것을...부위별로 하나씩일뿐입니다...

	UpdateActivePreviewListWithPreviewArrayImp();
}


namespace
{
	void AddShopGoodPackToCartImp(const buy::DisplayedGoodsPtr& newGood)
	{
		buy::DisplayedGoodsList& currGoodsList = util::GetDisplayedGoodsList_Cart();

		if (currGoodsList.size() >= USE_BASKET_COUNT)			// 장바구니 최대갯수시 밀어내기가 되어야 합니다. (장바구니 구현 내용임)
			currGoodsList.erase(currGoodsList.begin());

		currGoodsList.push_back(newGood);
	}

}

void buy::AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack)
{
	DisplayedGoodsPtr newGood = buy::MakeDisplayedGoodsPtr(pack);
	AddShopGoodPackToCartImp(newGood);
}

void buy::AddShopGoodPackToCart(const SHOP_GOODS_PACK* pack, INT32 displayIndex)
{
	DisplayedGoodsPtr newGood = buy::MakeDisplayedGoodsPtr(pack);
	AddShopGoodPackToCartImp(newGood);

	if (newGood)
		newGood->SetCurrentDisplayIndex(displayIndex);					// 봐봐..일케 간단하지...
}

void buy::RemovePreview(item_def::PartsType_ShopPreview partsType)
{
	I3_BOUNDCHK_RETURN(INT32(partsType), INT32(item_def::PartsType_ShopPreview_Max));

	if (value::preview[partsType])
	{
		value::preview[partsType].reset();		// 걍 없애면 되지..
		UpdateActivePreviewListWithPreviewArrayImp();
	}
}

void buy::RemoveCart(size_t idx)				// 이게 필요가 있는지 모르겠다..일단 작성..
{
	buy::DisplayedGoodsList& goodsList = util::GetDisplayedGoodsList_Cart();
	if (idx >= goodsList.size()) 
		return;			// 어서트 대신 그냥 리턴합니다.
	i3::vu::erase_index(goodsList, idx);
}

void buy::RemoveByPurchaseRoute(buy::PurchaseRouteType buy_type, size_t idx)
{
	switch (buy_type)
	{
	case buy::PurchaseRoute_Immediate:	value::immediate.reset();	break;
	case buy::PurchaseRoute_Extend:		value::immediate.reset();	break;
	case buy::PurchaseRoute_Cart:		buy::RemoveCart(idx);		break;
	case buy::PurchaseRoute_Preview:	buy::RemovePreview(item_def::PartsType_ShopPreview(idx));	break;
	}
}

//-- size

INT32 buy::GetCount_Immediate()
{
	return (value::immediate != nullptr) ? 1 : 0;
}

INT32 buy::GetCount_Preview()
{
	return INT32(value::activePreviewList.size());				// 프리뷰배열을 쓰면 안됩니다.
}

INT32 buy::GetCount_CurrCharaCart()
{
	buy::DisplayedGoodsList& currCharaCartList = util::GetDisplayedGoodsList_Cart();
	return INT32(currCharaCartList.size());
}

INT32 buy::GetCount_ByPurchaseRouteType(buy::PurchaseRouteType route)
{
	static INT32(*fn[])() =
	{
		buy::GetCount_Immediate, buy::GetCount_Immediate, buy::GetCount_CurrCharaCart, buy::GetCount_Preview,
	};
	return (*fn[route])();
}

INT32 buy::GetNumSlot_Preview()
{
	return item_def::PartsType_ShopPreview_Max;				// 현재 고정값입니다...아이템갯수가 아닌 슬롯갯수입니다..
}

buy::DisplayedGoods* buy::GetDisplayedGoods_ByPreviewSlot(item_def::PartsType_ShopPreview slot) // 실갯수가 아닌 슬롯인덱스입니다..
{
	I3_BOUNDCHK_RETURN(slot, item_def::PartsType_ShopPreview_Max, NULL);
	return value::preview[slot].get();				// 무효시 널값이 잘 나와야 합니다..
}

buy::DisplayedGoods* buy::GetDisplayedGoods_Immediate()
{
	return value::immediate.get();
}

buy::DisplayedGoods* buy::GetDisplayedGoods_Immediate(INT32 idx)			// 의미없는 인덱스입니다. 루프를 위해서만 씁니다.
{
	return value::immediate.get();
}

buy::DisplayedGoods* buy::GetDisplayedGoods_ByPreviewActive(INT32 idxActive)
{
	return value::activePreviewList[idxActive].get();				//일단 경계검사 안합니다.  루프시 넘어가지 않도록 조금만 신경씁시다..
}

buy::DisplayedGoods*	buy::GetDisplayedGoods_ByCurrCharaCart(INT32 idx)
{
	buy::DisplayedGoodsList& currCharaCartList = util::GetDisplayedGoodsList_Cart();
	return currCharaCartList[idx].get();
}

buy::DisplayedGoods* buy::GetDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType, INT32 idx)
{
	static buy::DisplayedGoods* (*fn[])(INT32) =
	{
		buy::GetDisplayedGoods_Immediate, buy::GetDisplayedGoods_Immediate, //이 부분은 사용 안 됨
		buy::GetDisplayedGoods_ByCurrCharaCart, buy::GetDisplayedGoods_ByPreviewActive,
	};
	return (*fn[routeType])(idx);

}

void	buy::GatherDisplayedGoods_ByPurchaseRoute(buy::PurchaseRouteType routeType,
	i3::vector< buy::DisplayedGoodsPtr>& outList)
{
	outList.clear();			// 
	switch (routeType)
	{
	case buy::PurchaseRoute_Immediate:	if (value::immediate)	outList.push_back(value::immediate);	break;
	case buy::PurchaseRoute_Extend:		if (value::immediate)	outList.push_back(value::immediate);	break;
	case buy::PurchaseRoute_Cart:		outList = util::GetDisplayedGoodsList_Cart();					break;
	case buy::PurchaseRoute_Preview:	outList = value::activePreviewList;								break;
		break;
	}
}

void		buy::UpdateItemInfoListWithPreview(i3::vector< ITEM_INFO >& item_info_list)
{
	const INT32 numPreviewSlot = buy::GetNumSlot_Preview();

	for (INT32 i = 0; i < numPreviewSlot; ++i)
	{
		const DisplayedGoods* goods = buy::GetDisplayedGoods_ByPreviewSlot((item_def::PartsType_ShopPreview)i);

		if (goods != NULL)			// 슬롯이 비면 확실이 널값입니다.
		{
			item_info_list[i].m_TItemID = goods->GetGoodsPack()->GetGoods(0)->_ItemID;
			//GetWareDBIdx_ByID 함수는 파츠에 경우 올바른 m_TItemDBIdx 를 얻지 못한다. 
			//하지만 chara_view에서 올바른 m_TItemDBIdx를 원하지 않을거라 생각되기 때문에(m_TItemID 이 값이 중요)
			//GetWareDBIdx_ByID 이 함수를 사용한다. 
			item_info_list[i].m_TItemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID(item_info_list[i].m_TItemID);
		}
	}
}



//-- only cart
bool  buy::IsExistedShopGoodPackInCurrentCharaCart(const SHOP_GOODS_PACK* pPack)
{
	buy::DisplayedGoodsList& list = util::GetDisplayedGoodsList_Cart();
	
	struct fn : i3::binary_function< DisplayedGoodsPtr, const SHOP_GOODS_PACK*, bool>
	{
		bool operator()(const DisplayedGoodsPtr& lhs, const SHOP_GOODS_PACK* rhs) const
		{
			const SHOP_GOODS_PACK* pack_Lhs = lhs->GetGoodsPack();
			return *pack_Lhs == *rhs;					// 역참조 비교인데, 역참조가 실제로 필요한것인지 아닌지 좀더 따져보도록 하자..
		}
	};

	buy::DisplayedGoodsList::const_iterator it = i3::find_if(list.begin(), list.end(), i3::bind2nd(fn(), pPack));

	return it != list.end();
}

INT32 buy::RemoveDisplayedGoodsInCart(const SHOP_GOODS* pGoods)
{

	INT32 eraseCount = 0;

	const size_t numChara = value::cart.size();			// 각 슬롯캐릭터들이 장바구니 하나씩 들고 있는 것임..
	
	for (size_t i = 0; i < numChara; ++i)
	{
		buy::DisplayedGoodsList& goodsList = value::cart[i];

		struct fn : i3::binary_function< DisplayedGoodsPtr, const SHOP_GOODS*, bool>
		{
			bool operator()(const DisplayedGoodsPtr& goodsPtr, const SHOP_GOODS* pGoods) const
			{
				const SHOP_GOODS_PACK* pPack_Lhs = goodsPtr->GetGoodsPack();			// 존재할 것이다...

				const INT32 numGood = pPack_Lhs->GetGoodsCount();

				for (INT32 i = 0; i < numGood; ++i)					// 현재 굿이 아닌 전체를 다 돌아야 하는게 조금 약점스럽다..
				{
					const SHOP_GOODS* pGood_Lhs = pPack_Lhs->GetGoods(i);

					if (*pGood_Lhs != *pGoods)	// 역참조 값비교이다... 속도가 약간 걱정될 비교구문이다.. 하나라도 다르면 false라는데....컨텐츠상...비교구문이 좀 거시기하다....
						return false;
				}

				return true;					// 전체가 다 같아야 true인듯한데....이거 ㅈㄴ골때리네...정상적인 비교구문일수 없으나, 호환성때문에 일케 유지한다..
			}									// 현재 내구도 아이템의 중복삭제에만 쓰이니까..큰문제는 없으나, 함수자체는 국어적인 오류라고 볼수 있다...
		};										// 아무래도 너무 ㅈ 같아서 또 수정해야될듯 싶다....주변 코드가 어느 정도 정리되면 이 쓰레기 스런 구현을 수정하겠다...

		buy::DisplayedGoodsList::iterator it = i3::remove_if(goodsList.begin(), goodsList.end(),
			i3::bind2nd(fn(), pGoods));

		if (it != goodsList.end())
		{
			eraseCount += i3::distance(it, goodsList.end());
			goodsList.erase(it, goodsList.end());
		}
	}

	return eraseCount;
}

namespace
{
	bool HasMaximumItemCount(const SHOP_GOODS_PACK * pack)
	{
		SHOP_GOODS* pGoods = pack->GetGoods(0);

		ShopItemCategory catShop = (usf::GetShopItemCategory(pGoods->_ItemID));
		if (catShop != SHOP_ITEM_CATEGORY_CHARACTER)
			return false;

		item_def::Category catItem = item_def::get_category(pGoods->_ItemID);

		if (catItem != item_def::CTG_PARTS && catItem != item_def::CTG_SKIN)
			return false;
		
		return true;
	}
	
}


// 기존의 ㅈ같은 함수3개는 제거하고 웬간하면 이 함수에 집중합니다...(잘 쓰지도 않을거를 지저분하게 응용하지 맙시다...설계보다는 구현부터 개 구려서 고칩니다..)

void	buy::RemoveInvalidDisplayedGoods_CartPreview(buy::PurchaseRouteType routeType, bool& out_is_invalid)
{
	out_is_invalid = false;

	buy::DisplayedGoodsList* pGoodsList = NULL;
	switch (routeType)
	{
	case buy::PurchaseRoute_Cart:		pGoodsList = &util::GetDisplayedGoodsList_Cart();	break;
	case buy::PurchaseRoute_Preview:	pGoodsList = &value::activePreviewList; break;
	default:			return;
	}

	UINT32 chara_cnt = (UINT32)CCharaInfoContext::i()->GetHasCharacterCount();			// 이 함수 시점에서 고정값이다.

	buy::DisplayedGoodsList& goodsList = *pGoodsList;

	struct SHOP_GOODS_PACK_Available { const SHOP_GOODS_PACK* pack;	INT32	availableCount; };

	i3::vector<SHOP_GOODS_PACK_Available>	vecGoodsPack;	// 파츠인 경우에 넣고, 중복체크만 하면 됩니다.
	
	for (INT32 i = 0; i < INT32(goodsList.size()); ++i)
	{
		const buy::DisplayedGoodsPtr& goods = goodsList[i];

		const SHOP_GOODS_PACK* pGoodPack = goods->GetGoodsPack();

		if (HasMaximumItemCount(pGoodPack) == false)
			continue;
		
		struct fn : i3::binary_function< SHOP_GOODS_PACK_Available, const SHOP_GOODS_PACK*, bool>
		{
			bool operator()(const SHOP_GOODS_PACK_Available& lhs, const SHOP_GOODS_PACK* rhs) const { return *lhs.pack == *rhs; }
		};
		i3::vector<SHOP_GOODS_PACK_Available>::iterator it = i3::range::find_if(vecGoodsPack, i3::bind2nd(fn(), pGoodPack));
			
		INT32 availableCount = 0;

		if (it == vecGoodsPack.end())
		{
			SHOP_GOODS* pGoods = pGoodPack->GetGoods(0);
			INT32 invenCount = CInvenList::i()->FindItemCount(pGoods->_ItemID);

			SHOP_GOODS_PACK_Available p;
			p.pack = pGoodPack;
			p.availableCount = chara_cnt - invenCount;				// 현재 가용갯수..
			availableCount = p.availableCount;

			--p.availableCount;										// 하나를 줄인다..
			vecGoodsPack.push_back(p);
		}
		else
		{
			availableCount = (*it).availableCount;
			--(*it).availableCount;									// 계속 줄인다.. 몇개가 모자른지 확인 가능..
		}

		if (availableCount < 1)										// 캐릭터갯수보다 많이 가질수 없으므로, 지워야 한다고 합니다..
		{
			out_is_invalid = true;
			i3::vu::erase_index(goodsList, i);		--i;			// 삭제처리..continue는 하지 않음..
		}
	}
	
}
