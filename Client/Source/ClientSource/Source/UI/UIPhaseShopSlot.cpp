#include "pch.h"
#include "UIPhaseShopSlot.h"

#include "UIPhaseShop.h"
#include "UIPhaseGiftShop.h"

#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UIUtil.h"
#include "UICharaInfoMgr.h"
#include "UIMainFrame.h"
#include "UISideMenu.h"
#include "UIPopupBuyItem.h"
#include "PurchaseMgr.h"
#include "BuyUtil.h"

#include "Batmanager.h"
#include "UI/UISideMenu.h"
#include "UISMenu_Ability.h"
#include "MedalManager.h"
#include "Designation/DesigUtil.h"

#include "UISignal.h"

namespace
{
	ShopSlot * GetThis(void)
	{
		iMultiTap * pMultiTap = i3::kind_cast<iMultiTap*>(g_pFramework->GetUIMainframe()->GetCurrentSubPhase());

		if( pMultiTap != nullptr)
		{	
			if(i3::same_of<UIPhaseShop*>(pMultiTap) || i3::same_of<UIPhaseGiftShop*>(pMultiTap) )
			{
				return static_cast<ShopSlot*>(pMultiTap->GetItemSlot());
			}
		}

		return nullptr;
	}

	INT32 g_CurrentIdx = -1;
	INT32 g_compare_src_idx = 0;	//비교 원본 인덱스
	bool g_is_compare_item = false;	//비교 중인지
	bool g_show_compare_item = false;	//비교 보여주기
	bool g_is_start_compare = false;	//첫 비교

	enum	{	
		ROW = 3, COL = 3, SLOT = ROW * COL,
		CLICK = 0, DETAIL, CART, 
		COMPARE, CASH
	};
}

namespace inner
{
	void stop_compare(void);
}
//-----------------------------////-----------------------------//
extern "C" 
{
	int shop_menu_slot( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);

		ShopSlot * pThis = GetThis();
		if(pThis != nullptr)
			pThis->clicked(idx);

		return 0;
	}
	int shop_menu_slot2( LuaState * L)
	{
		ShopSlot * pThis = GetThis();
		if(pThis != nullptr)
			pThis->double_clicked();
		
		return 0;
	}
}

LuaFuncReg shop_slot_glue[] =
{
	{ "Click",			shop_menu_slot	},
	{ "Click2",			shop_menu_slot2	},
	{ nullptr,							nullptr}
};

//-----------------------------////-----------------------------//
ShopSlot::ShopSlot(iMultiTap *parent) : iMultiTabSlot(parent)
{
	pf_update_item_info_arr[0] = &me::update_item_info_normal;
	pf_update_item_info_arr[1] = &me::update_item_info_front;

	m_ui32FirstSlotItemID = 0;
	m_ui8SlotType = (UINT8)(parent->GetPhaseType() == USP_SHOP ? SHOP_SLOT : GIFT_SLOT);
}

//-----------------------------////-----------------------------//
void ShopSlot::clicked(INT32 idx, bool from_parent)
{
	if(from_parent) iSlot::set_select( iSlot::get_old_slot() );
	INT32 slot_idx = iSlot::apply_select();
	if( slot_idx == -1 ) return;


	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	SHOP_GOODS* goods = shop_info.info->GetGoods(0);
	bool update_slot_list = true;

	switch(idx)
	{
	case CLICK:
		{
			if (g_CurrentIdx == slot_idx)
				return;

			g_CurrentIdx = slot_idx;	 m_iBuyType = 0;	m_iCurrentBuyType = 0;	m_iCurrentBuySlot = 0;
			if( usf::CheckValidGoods_in_Pef(shop_info.info) == false ) break;

			if( mParent->get_current_menu_idx() == item_def::CTG_PARTS || mParent->get_current_menu_idx() == item_def::CTG_SKIN )
			{
				buy::AddShopGoodPackToPreview(shop_info.info);			// 함수는 간단이 만들었는데, 인수가 더럽네...
				UISignal::UpdatePreviewSlot();
			}
		}
		break;
	case DETAIL:
		{
			mParent->GetMainFrame()->OpenPopup(UPW_SHOPEXTENSION, const_cast<SHOP_GOODS_PACK*>(shop_info.info));
			update_slot_list = false;
			return;
		}
		break;
	case CART:
		{
			//Click 관련 처리를 안하고 바로 CART 버튼을 클릭했을 때를 위한 처리입니다.
			if (g_CurrentIdx != slot_idx)
			{
				g_CurrentIdx = slot_idx;	 m_iBuyType = 0;	m_iCurrentBuyType = 0;	m_iCurrentBuySlot = 0;
				if (usf::CheckValidGoods_in_Pef(shop_info.info) == false) break;
				
				if (g_pEnvSet->IsBaseOnUIVersion_1_0())
				{
					SetBuySlot();
					SetBuySlotStr();
				}
			}
			
			if( usf::CheckValidGoods_InCart(shop_info.info) == false ) return;
			if( usf::PopupRestriction(goods->_ItemID) == false) return;

			buy::AddShopGoodPackToCart(shop_info.info, m_iCurrentBuySlot);
					
		//	UI::call_cb("update_cart_board");
			UISignal::UpdateCartBoard();		//상품 지웟으니 cart board를 갱신한다.
		}
		break;
	case COMPARE:
		{
			g_is_compare_item = !g_is_compare_item;
			if(g_is_compare_item)
			{
				g_compare_src_idx = slot_idx; //비교 시작할 경우 원본 인덱스 저장
				g_is_start_compare = false; //비교하자마자 원본이랑 비교 유무를 나타냄
			}
			else
			{
			//	UI::call_cb("enable_compare_display_attr", reinterpret_cast<void*>(false) ); //비교 끝
				UISignal::EnableCompareDisplayAttr(false);
				g_show_compare_item = false;
			}

		//	UI::call_cb("set_compare_skip_button", &g_is_compare_item ); //비교 버튼 enable 여부(눌린 상태)
			UISignal::SetCompareSkipButton(g_is_compare_item); //비교 버튼 enable 여부(눌린 상태)
		}
		break;
	}

	//main frame에 아이템에 대한 속성 업데이트
	mParent->show_display_info();
	update_item_info(mDpImgSetCtrl, slot_idx, mDpScene, true);
	//update_item_attribute( slot_idx );

	//비교중, 원본 인덱스와 다름, 아직 비교 시작 안 함.. 즉,비교 버튼 눌린 상태에 다른 slot 클릭할 경우
	//if( g_is_compare_item && g_compare_src_idx != slot_idx)// && !g_is_start_compare )
	//{
	//	UI::call_cb("enable_compare_display_attr", reinterpret_cast<void*>(true) );  //비교 ui 활성화
	//	update_item_attribute2( g_compare_src_idx ); //원본 인덱스 정보 표시
	//	g_show_compare_item = true;
	//	g_is_start_compare = true; 
	//}

	bool bCompare = false;
	item_def::Category Category = item_def::get_category(goods->_ItemID);
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	SMenuAbility* sidemenu = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));

	if (pUIFrame->GetCurrentSubPhaseType() == USP_SHOP ||
		pUIFrame->GetCurrentSubPhaseType() == USP_GIFTSHOP)
	{
		sidemenu->EnableCtrlsByShopCategory(Category, false);
	}
	else
	{
		sidemenu->EnableMainInfo(false);

	}

	if (Category == item_def::CTG_WEAPON)
	{
		bCompare = true;
		if (sidemenu)
		{
			sidemenu->applyWeapon(g_pWeaponInfoDataBase->getWeaponInfo(goods->_ItemID, true));
			sidemenu->SetMainInfoUpdateType(AMUT_PREVIEW_WEAPON);
			g_pFramework->OnEvent(UIEVENT_UPDATE_ABILITY, 0, 0, I3_EVT_CODE_ACTIVATE);
		}

	}
	else
	{
		if (g_is_compare_item == true)
		{
			g_is_compare_item = !g_is_compare_item;
			if (g_is_compare_item)
			{
				g_compare_src_idx = slot_idx; //비교 시작할 경우 원본 인덱스 저장
				g_is_start_compare = false; //비교하자마자 원본이랑 비교 유무를 나타냄
			}
			else
			{
			//	UI::call_cb("enable_compare_display_attr", reinterpret_cast<void*>(false)); //비교 끝
				UISignal::EnableCompareDisplayAttr(false);
				g_show_compare_item = false;
			}

		//	UI::call_cb("set_compare_skip_button", &g_is_compare_item); //비교 버튼 enable 여부(눌린 상태)
			UISignal::SetCompareSkipButton(g_is_compare_item); // 비교 버튼 enable 여부(눌린 상태)
		}
	}

	if (m_ui8SlotType == GIFT_SLOT)
		bCompare = false;

	LuaState * L = UILua::CallLuaFunction(mDpAttScene, "EnableCompare");
	i3Lua::PushBoolean(L, bCompare);
	UILua::EndLuaFunction(L, 1);

	if (!g_is_compare_item)
	{
		update_item_attribute(slot_idx);
	}
	else
	{
		if (g_compare_src_idx != slot_idx)
		{
		//	UI::call_cb("enable_compare_display_attr", reinterpret_cast<void*>(true));  //비교 ui 활성화
			UISignal::EnableCompareDisplayAttr(true);

			update_item_attribute2(slot_idx); //원본 인덱스 정보 표시
			//g_show_compare_item = true;
			//g_is_start_compare = true;
		}
	}

	if (mCategory != item_def::CTG_NEWHOT)
		UpdateCharaView(slot_idx);

	if (update_slot_list)
		mSlot.UpdateList(mInfoList.size());

	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		SetBuySlot();
		SetBuySlotStr();
	}

}

void ShopSlot::SetBuySlot(void)
{
	INT32 iBuyType = 0;
	i3::wstring price_str;
	i3::wstring perido_str;

	INT32 slot_idx = iSlot::apply_select();
	if (slot_idx == -1) return;
	const shop_util::ShopInfo & shop_info = mInfoList.get(slot_idx);
	INT32 size = shop_info.info->GetGoodsCount();
	m_strBuyPoint.clear(); m_strBuyCash.clear(); m_strBuyPointPrice.clear(); m_strBuyCashPrice.clear();
	m_iCurrentBuySlot = 0;

	SHOP_GOODS* firstpack = shop_info.info->GetGoods(0);

	for (INT32 i = 0; i < size; ++i)
	{
		SHOP_GOODS* pack = shop_info.info->GetGoods(i);

		// 아이템 구매 리스트에 EVENT아이템은 제외합니다.
		if (pack->_SaleType == GOODS_SALE_EVENT)
			continue;

		usf::GetShopPeriodCountString(pack, perido_str);

		if (pack->GetPriceType(usf::g_price_idx) == GOODS_PRICE_ALL)
		{
			iBuyType = GOODS_PRICE_ALL;
			usf::GetShopPriceStringComma(pack, price_str, GOODS_PRICE_POINT);
			m_strBuyPoint.push_back(perido_str);
			m_strBuyPointPrice.push_back(price_str);

			usf::GetShopPriceStringComma(pack, price_str, GOODS_PRICE_CASH);
			m_strBuyCash.push_back(perido_str);
			m_strBuyCashPrice.push_back(price_str);
		}
		else
		{
			bool bCheck = usf::GetShopPriceStringComma(pack, price_str, pack->GetPriceType(usf::g_price_idx));

			if (bCheck == true)
			{
				if (pack->GetPriceType(usf::g_price_idx) == GOODS_PRICE_POINT)
				{
					if (iBuyType == GOODS_PRICE_CASH)
						iBuyType = GOODS_PRICE_ALL;
					else
						iBuyType = GOODS_PRICE_POINT;


					m_strBuyPoint.push_back(perido_str);
					m_strBuyPointPrice.push_back(price_str);
				}
				else
				{
					if (iBuyType == GOODS_PRICE_POINT)
						iBuyType = GOODS_PRICE_ALL;
					else
						iBuyType = GOODS_PRICE_CASH;

					m_strBuyCash.push_back(perido_str);
					m_strBuyCashPrice.push_back(price_str);

				}
			}

		}
	}

	if (firstpack->GetPriceType(usf::g_price_idx) == GOODS_PRICE_CASH)
		m_iCurrentBuyType = GOODS_PRICE_CASH;
	else
		m_iCurrentBuyType = GOODS_PRICE_POINT;

	m_iBuyType = iBuyType;

	LuaState * L = UILua::CallLuaFunction(mDpAttScene, "SetBuyTab");
	i3Lua::PushInteger(L, m_iBuyType);
	i3Lua::PushInteger(L, m_iCurrentBuyType);
	UILua::EndLuaFunction(L, 2);

}

void ShopSlot::SetBuySlotStr(void)
{
	LuaState * L = nullptr;

	INT32 choice_price     = 0;
	INT32 choice_radio_idx = 0;

	if (m_iCurrentBuyType == GOODS_PRICE_POINT)
	{
		L = UILua::CallLuaFunction(mDpAttScene, "InitButSlot");
		UILua::EndLuaFunction(L, 0);

		for (size_t i = 0; i < m_strBuyPoint.size(); i++)
		{
			L = UILua::CallLuaFunction(mDpAttScene, "SetBuySlot");
			i3Lua::PushInteger(L, i + 1);
			i3Lua::PushStringUTF16To8(L, m_strBuyPoint.at(i).c_str());	//name
			i3Lua::PushStringUTF16To8(L, m_strBuyPointPrice.at(i).c_str());	//name
			UILua::EndLuaFunction(L, 3);

			if (choice_price <= atoi(m_strBuyPointPrice.at(i)))
			{
				choice_price = atoi(m_strBuyPointPrice.at(i));
				choice_radio_idx = i;
			}
		}
	}
	else
	{
		L = UILua::CallLuaFunction(mDpAttScene, "InitButSlot");
		UILua::EndLuaFunction(L, 0);

		for (size_t i = 0; i < m_strBuyCash.size(); i++)
		{
			L = UILua::CallLuaFunction(mDpAttScene, "SetBuySlot");
			i3Lua::PushInteger(L, i + 1);
			i3Lua::PushStringUTF16To8(L, m_strBuyCash.at(i).c_str());	//name
			i3Lua::PushStringUTF16To8(L, m_strBuyCashPrice.at(i).c_str());	//name
			UILua::EndLuaFunction(L, 3);

			if (choice_price <= atoi(m_strBuyCash.at(i)))
			{
				choice_price = atoi(m_strBuyCash.at(i));
				choice_radio_idx = i;
			}
		}
	}
	
	m_iCurrentBuySlot = choice_radio_idx;
	L = UILua::CallLuaFunction(mDpAttScene, "SetPointRadiobtn");
	i3Lua::PushInteger(L, choice_radio_idx + 1);
	UILua::EndLuaFunction(L, 1);

}

void ShopSlot::double_clicked(bool from_parent)
{
	if(from_parent) iSlot::set_select( iSlot::get_old_slot() );

	INT32 slot_idx = iSlot::get_now_slot();
	if( slot_idx == -1 ) return;

	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	SHOP_GOODS* goods = shop_info.info->GetGoods(0);

	if( usf::CheckValidGoods(shop_info.info, false) == false ) return;
	if( usf::PopupRestriction(goods->_ItemID) == false) return;

	
	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
	{
		buy::PushImmediate(shop_info.info, GOODS_PRICE_TYPE(m_iCurrentBuyType), m_iCurrentBuySlot);
	}
	else
		buy::PushImmediate(shop_info.info);

	mParent->GetMainFrame()->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Immediate, &UISignal::FinishBuyPopup);	//	"finish_buy_popup");	//즉시구매
}

//-----------------------------////-----------------------------//
void ShopSlot::OnCreate( i3GameNode * pParent)
{
	//Slot 만들고
	mSlot.EnableScroll( true );
	mSlot.OnCreate( (UIBase*)mParent, ROW, COL);
	mSlot.RegisterSlotScene("Scene/Main/PointBlankRe_Shop_Slot_V3.i3UIs", shop_slot_glue, true);
	
	m_ppImgSet = new UIImgSetCtrl*[SLOT];
	MEMDUMP_NEW( m_ppImgSet, sizeof(UIImgSetCtrl*) * SLOT);

	::memset( m_ppImgSet, 0, sizeof(UIImgSetCtrl*) * SLOT);
}

void ShopSlot::UpdateCharaView( INT32 slotIdx)
{
	//if( is_shop_slot() )
	{
		const shop_util::ShopInfo & shop_info = mInfoList.get( slotIdx );
		if( usf::CheckValidGoods_in_Pef(shop_info.info) == true )
		{
			if (usf::IsSetItem(shop_info.info->GetGoods(0))) return;	// 세트아이템 미리보기 기능 제거.

			EQUIP::EquipGoods( shop_info.info->GetGoods(0) );
		}
	}
}

bool ShopSlot::get_cart_enable(void)
{
	if( is_shop_slot() )
		return ((mCategory == item_def::CTG_WEAPON) | (mCategory == item_def::CTG_ITEM) | (mCategory == item_def::CTG_NEWHOT));

	return false;
}

INT32 ShopSlot::get_sale_mark(UINT8 sale_mark)
{
	if( is_shop_slot() )
		return (sale_mark - 1);

	// 선물 상점은 Hot&New 마크 표기하지 않음.// Mark만 표기.
	return  -1;// sale_mark == 2 ? (sale_mark - 1) : -1;
}

INT32 ShopSlot::get_item_status(ItemStatus item_status)
{
	if( is_shop_slot() )	
		return (item_status - 1);

	return -1;
}

void ShopSlot::Initialize()
{
	mInfoList.initialize(m_ui8SlotType);
	//mInfoList.initialize(0);
}

void ShopSlot::set_list_front_item_by_ItemID(UINT32 ui32ItemID)
{
	m_ui32FirstSlotItemID = ui32ItemID;

	// Key : User Guide System
	mInfoList.set_list_front_item(m_ui32FirstSlotItemID);

	size_t size = mInfoList.size();
	mSlot.UpdateList( size );
}

void ShopSlot::Entrance()
{
	// Key : User Guide System
	{
		if( mInfoList.set_list_front_item(m_ui32FirstSlotItemID) )
			UserGuideMgr::i()->OnLoadGuideTargetInfo(mParent);
		else
			UserGuideMgr::i()->UnLoadGuideTargetInfo(mParent);
	}

	size_t size = mInfoList.size();
	mSlot.UpdateList( size );
}


void ShopSlot::Entrance(item_def::WeaponComboType type, bool bscrool)
{
	//무기 비교를 멈춘다.
	inner::stop_compare();

	use_combo_type = true;
	g_CurrentIdx = -1;

	mCategory = item_def::CTG_WEAPON;
	mType = (UINT32)type;

	mInfoList.list_up(type);
	mInfoList.SortType(mCategory, m_SortType);
	
	// Key : User Guide System
	{
		if(mParent != nullptr)
		{
			if( mInfoList.set_list_front_item(m_ui32FirstSlotItemID) )
				UserGuideMgr::i()->OnLoadGuideTargetInfo(mParent);
			else
				UserGuideMgr::i()->UnLoadGuideTargetInfo(mParent);
		}
	}

	size_t size = mInfoList.size();
	iSlot::reset_select( size );
	mSlot.UpdateList( size, bscrool);
}

void ShopSlot::Entrance(item_def::Category category, UINT32 type, bool bscrool)
{
	//무기 비교를 멈춘다.
	inner::stop_compare();

	use_combo_type = false;
	g_CurrentIdx = -1;

	mCategory = category;
	mType = type;

	mInfoList.list_up( category, type );
	mInfoList.SortType(mCategory, m_SortType);

	// Key : User Guide System
	{
		if(mParent != nullptr)
		{
			if( mInfoList.set_list_front_item(m_ui32FirstSlotItemID) )
				UserGuideMgr::i()->OnLoadGuideTargetInfo(mParent);
			else
				UserGuideMgr::i()->UnLoadGuideTargetInfo(mParent);
		}
	}

	size_t size = mInfoList.size();
	iSlot::reset_select( size );
	mSlot.UpdateList( size, bscrool);
}

void ShopSlot::SearchKeyword(const WCHAR* wstr,bool bFocus)
{
	if(bFocus == false &&  m_SearchStr == wstr)
		return;

	bool bDefaultText = false;
	if (i3::generic_compare(wstr, "") != 0)
		bDefaultText = false;
	else
		bDefaultText = true;

	LuaState * L = UILua::CallLuaFunction(mDpAttScene, "EnableSearchText");
	i3Lua::PushBoolean(L, bDefaultText);
	UILua::EndLuaFunction(L, 1);

	I3PRINTLOG(I3LOG_NOTICE, "%s", wstr);
	m_SearchStr = wstr;
	if (mCategory == item_def::CTG_NA)
		return;

	if (use_combo_type == true)
		Entrance((item_def::WeaponComboType)mType, false);
	else
		Entrance(mCategory, mType, false);

	mInfoList.SearchKeyword(m_SearchStr.c_str());
	size_t size = mInfoList.size();
	iSlot::reset_select(size);
	mSlot.UpdateList(size, bFocus);
}

void ShopSlot::SortType(item_def::Category category, SHOP_SORT_TYPE type)
{
	m_SortType = type;
	mInfoList.SortType(category, type);
	size_t size = mInfoList.size();
	iSlot::reset_select(size);
	mSlot.UpdateList(size);
}

void ShopSlot::OnUnloadAllScenes()
{
	iMultiTabSlot::OnUnloadAllScenes();

	m_ui32FirstSlotItemID = 0;
}

//-----------------------------////-----------------------------//

bool ShopSlot::IsRestriction(UINT32 ui32ItemID)
{
	bool bRestriction = true;
	if (g_pEnvSet->IsV2Version() == true)
	{
		SHOP_ITEM_DATA* pItemBase = CShop::i()->GetItemData(ui32ItemID);

		MedalViewInfo* pMedalInfo = CMedalManager::i()->GetMedalViewIdx(pItemBase->_ui16Condition);

		if (pItemBase->_ui8RestrictionType == TYPE_ITEM_RESTRICTION_MEDAL)
		{
			if (pMedalInfo == nullptr) {
				return false;
			}

			INT32 iLevel = pMedalInfo->getCurrentLevel();
			if (iLevel < pItemBase->_ui8Restriction)
				bRestriction = false;
		}
	}
	else
	{
		if (false == DesigUtil::CheckPurchasable_MyItemByDesig(ui32ItemID))
		{
			bRestriction = false;
		}
	}
	return bRestriction;
}

void ShopSlot::update_item_info_normal(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	bool bRestriction = false;
	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	
	bool bigger = false;
	if (g_pEnvSet->IsBaseOnUIVersion_1_0()) 
		bigger = true;

	SHOP_GOODS* goods = shop_info.info->GetGoods(const_cast<SHOP_GOODS_PACK*>(shop_info.info)->GetDisplayShopGoodsIndex(bigger));

	INT32 price_idx = PRICE_TYPE_SHOP_REAL;
	if(m_ui8SlotType == GIFT_SLOT)	price_idx = PRICE_TYPE_GIFT_REAL;

	bRestriction = IsRestriction(goods->_ItemID);

	mImageMgr->SelectImage(pSetCtrl, goods);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, goods);

	i3::wstring wstrTermCount;
	usf::GetShopPeriodCountString(goods, wstrTermCount);

	i3::wstring wstrPrice;
	usf::GetShopPriceString(goods, wstrPrice);

	bool detail = usf::HasExtensionDescImage(goods);

	LuaState * L = UILua::CallLuaFunction( scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, wstrItemName);	//name

	// 상품 가격 러시아 요청으로 미출력 처리(촉박한 패치 일정으로 인해 다음 Release부터 pef로 기능 추가 예정)
#if !defined(LOCALE_RUSSIA)
	i3Lua::PushStringUTF16To8(L, wstrPrice);	//price
#else
	i3Lua::PushStringUTF16To8(L, L"");			//price
#endif

	i3Lua::PushStringUTF16To8( L, wstrTermCount );	//perido

	i3Lua::PushBoolean( L, detail );	//detail

	i3Lua::PushBoolean( L, get_cart_enable() ? true : false);
	i3Lua::PushInteger( L, get_sale_mark(goods->GetSaleMark(price_idx)) );	//newhot
	i3Lua::PushBoolean( L,  !bRestriction);				//Loock
	i3Lua::PushInteger( L, get_item_status( usf::GetItemStatus(shop_info.info) ) );	//equip

	UILua::EndLuaFunction(L , 8);

	if( iSlot::is_selected(slot_idx) )
	{
		L = UILua::CallLuaFunction( scn, "Select");
		i3Lua::PushBoolean(L, g_is_compare_item);	// 원본/대상 아이템 비교 모드 여부
		i3Lua::PushBoolean(L, g_compare_src_idx != slot_idx); // 비교 대상 아이템 여부
		UILua::EndLuaFunction(L, 2);
	}
	else
	{
		L = UILua::CallLuaFunction( scn, "Unselect");
		UILua::EndLuaFunction(L , 0);
	}

	if( g_is_compare_item )
	{
		if( g_compare_src_idx == slot_idx )
		{
			L = UILua::CallLuaFunction( scn, "Select2"); //비교시에 원본 slot을 표시한다.
			UILua::EndLuaFunction(L , 0);
		}
	}
}

void ShopSlot::update_item_info_front(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn)
{
	scn->EnableScene(true);

	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	SHOP_GOODS* goods = shop_info.info->GetGoods(0);

	mImageMgr->SelectImage(pSetCtrl, goods);

	i3::rc_wstring wstrItemName;
	usf::GetShopItemName(wstrItemName, goods);

	bool bRestriction = false;
	bRestriction = IsRestriction(goods->_ItemID);

	LuaState * L = UILua::CallLuaFunction( scn, "InitSlot");
	i3Lua::PushStringUTF16To8( L, wstrItemName.c_str(), wstrItemName.size() );	//name
	i3Lua::PushStringUTF16To8( L, L"");	//price
	i3Lua::PushStringUTF16To8( L, L"");	//perido

	i3Lua::PushBoolean( L, false );	//detail
	i3Lua::PushBoolean( L, false );	//cart 표시

	i3Lua::PushInteger( L, -1 );	//newhot
	i3Lua::PushBoolean( L,  !bRestriction);		
	i3Lua::PushInteger( L, -1 );	//equip

	UILua::EndLuaFunction(L , 8);

	L = UILua::CallLuaFunction( scn, "Select");
	UILua::EndLuaFunction(L , 0);

	L = UILua::CallLuaFunction(scn, "EnablePreview");
	UILua::EndLuaFunction(L, 0);
}

void ShopSlot::update_item_info(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn, bool front_dp)
{
	(this->*pf_update_item_info_arr[front_dp])(pSetCtrl, slot_idx, scn);
}

void ShopSlot::update_item_attribute(INT32 slot_idx)
{
	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	SHOP_GOODS* goods = shop_info.info->GetGoods(0);

	
	item_def::Category Category = item_def::get_category(goods->_ItemID);

	if (usf::IsSetItem(goods) == true)
	{
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(goods->_GoodsID);
		if (pInfo && pInfo->GetCatagory() == item_def::IT_SET)
			Category = item_def::CTG_ITEM;
	}

	LuaState * L = UILua::CallLuaFunction(mDpAttScene, "SetInfoTap");
	i3Lua::PushInteger(L, (INT32)Category);
	UILua::EndLuaFunction(L, 1);

	switch(Category)
	{
	case item_def::CTG_WEAPON:	update_weapon_attribute_impl(goods->_ItemID, "SetWeaponInfo");	break;
	case item_def::CTG_CHARA:	update_chara_attribute_impl(goods->_ItemID);		break;
	case item_def::CTG_PARTS:	update_parts_attribute_impl(goods->_ItemID);	break;
	case item_def::CTG_SKIN :	update_parts_attribute_impl(goods->_ItemID);	break;
	case item_def::CTG_ITEM:	update_item_attribute_impl(goods);		break;
	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}
}

void ShopSlot::update_item_attribute2(INT32 slot_idx)
{
	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	SHOP_GOODS* goods = shop_info.info->GetGoods(0);
	update_weapon_attribute_impl(goods->_ItemID, "SetWeaponInfo2");
}

namespace inner
{
	void stop_compare(void)
	{
		g_is_start_compare = false;
		g_show_compare_item = false;
		g_is_compare_item = false;

	//	UI::call_cb("enable_compare_display_attr", reinterpret_cast<void*>(false) ); 
	//	UI::call_cb("set_compare_skip_button", reinterpret_cast<void*>(false) );

		UISignal::EnableCompareDisplayAttr(false);
		UISignal::SetCompareSkipButton(false);
	}
}

#ifdef I3_DEBUG
UINT32 ShopSlot::get_get()
{
	INT32 slot_idx = iSlot::apply_select();
	if( slot_idx == -1 ) return 0;

	const shop_util::ShopInfo & shop_info = mInfoList.get( slot_idx );
	SHOP_GOODS* goods = shop_info.info->GetGoods(0);

	return goods->_ItemID;
}
#endif