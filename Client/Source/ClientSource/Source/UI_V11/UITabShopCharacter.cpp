#include "pch.h"

#include "InvenUtil.h"
#include "UI/UIShopFunction.h"
#include "UI/UIClanFunction.h"

#include "UI_V11/UITabShopCharacter.h"
#include "UI_V11/UIPhaseShop_V11.h"
#include "UI_V11/UIToolTipShop.h"

#include "Shop.h"
#include "ShopContext.h"
#include "Designation/DesigUtil.h"

UITabShopCharacter::UITabShopCharacter(UIPhaseSubBase* phaseShop, UIToolTipShop* pTT) :
UITabShopBase(phaseShop, pTT), m_category(UISHOP_SUBTAB_CHARACTER_ALL), m_currFilterType(SISFT_NEW)
{

}

UITabShopCharacter::~UITabShopCharacter()
{
}

void	UITabShopCharacter::_MakeGroupList_ByShopCategory(i3::vector<SHOP_GOODS_PACK*>& refGoodsList)
{
	//	필터를 거친 상품만 리스트에 넣는다.
	refGoodsList.clear();

	CShopContext * pShopContext = CShopContext::i();
	INT32 goodsPackCount = pShopContext->GetGoodsPackCount(SHOP_CHARACTER_GOODS);

	i3::rc_wstring keyword = m_phaseShop->GetKeywordString();

	for(INT32 i = 0 ; i < goodsPackCount ; i++)
	{
		SHOP_GOODS_PACK * pGoodsPack = const_cast<SHOP_GOODS_PACK*>(pShopContext->GetGoodsPack(SHOP_CHARACTER_GOODS, i));
		if( pGoodsPack == NULL)
			continue;

		SHOP_GOODS * pGoods = pGoodsPack->GetGoods(0);
		if( pGoods == NULL)
			continue;

		ITEM_TYPE item_type = (ITEM_TYPE) GET_ITEM_TYPE( pGoods->_ItemID);
		
		/*
		김대영 V3 1_1 상점 개발 노트 2015-05-19
		현재 캐릭터 관련 아이템이 어떻게 나뉠지 몰라서 이렇게 기록해 놓습니다. (승천님 화이팅!)
		캐릭터 메인 텝 : 캐릭터, 스킨 표시, 디노
		머리탭 : 헤드기어 표시
		마스크탭 : 페이스기어 표시
		캐릭터 바디 : UPPER, GLOVE
		캐릭터 로어 : LOWER, BELTE, HOLSTER
		레드팀,블루팀 클래스는 구현 안되어있으므로 삭제 (원래 없던 기능 )
		*/
		//	class
		switch(m_category)
		{
		
		case UISHOP_SUBTAB_CHARACTER_ALL:
			{
				if ( item_type != ITEM_TYPE_CHARA
					&& item_type != ITEM_TYPE_HEADGEAR
					&& item_type != ITEM_TYPE_FACEGEAR
					&& item_type != ITEM_TYPE_DINO )
					continue;
			}
			break;
		
		case UISHOP_SUBTAB_CHARACTER_MAINITEM:
			{
				if ( item_type != ITEM_TYPE_CHARA
					 //&& item_type != ITEM_TYPE_SKIN
					 && item_type != ITEM_TYPE_DINO )
					continue;
			}
			break;
		
		case UISHOP_SUBTAB_CHARACTER_HAIR:
			{
				if ( item_type != ITEM_TYPE_HEADGEAR 
					&& item_type != ITEM_TYPE_BERET )
					continue;
			}
			break;

		case UISHOP_SUBTAB_CHARACTER_MASK:
			{
				if ( item_type != ITEM_TYPE_FACEGEAR )
					continue;
			}
			break;
	
		case UISHOP_SUBTAB_CHARACTER_BODY:
			{
				if ( item_type != ITEM_TYPE_UPPER  &&
					 item_type != ITEM_TYPE_GLOVE	)
					continue;
			}
			break;

		case UISHOP_SUBTAB_CHARACTER_LOWER:
			{
				if ( item_type != ITEM_TYPE_LOWER  &&
					 item_type != ITEM_TYPE_BELT   &&
					 item_type != ITEM_TYPE_HOLSTER	)
					continue;
			}
			break;

		default:
			break;
		}

		if( pGoods->_SaleType == GOODS_SALE_EVENT )
			continue;
		
		if ( keyword.size() != 0 )
		{
			i3::rc_wstring strItemName;
			usf::GetShopItemName(strItemName, pGoods);
			i3::to_lower_copy(strItemName, strItemName.c_str()); i3::to_lower_copy(keyword, keyword.c_str());
			if( i3::contain_string(strItemName,keyword) < 0 )		
				continue;
		}
		
		refGoodsList.push_back(pGoodsPack);
	}
	
	switch (m_currFilterType)
	{
	case SISFT_NAME:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByName); break;
	case SISFT_POINT:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByPoint); break;
	case SISFT_CASH:	i3::sort(refGoodsList.begin(), refGoodsList.end(), cbSortByCash); break;
	default: break;
	}
			
}


void	UITabShopCharacter::Update_ToolTip_ByShopCategory(SHOP_GOODS_PACK* pGoodsPack, 
													 i3::rc_wstring& outItemName, i3::rc_wstring& outDesc, i3::rc_wstring& outCategory, i3::rc_wstring& outStatus, i3::rc_wstring& outRequired)
{
	SHOP_GOODS* pGoods = pGoodsPack->GetGoods(0);
	INT32 CharaID = pGoods->_ItemID;
	
	outCategory = usf::GetCharacterItemCategoryString(CharaID);

	UINT8 Type = 0;
	UINT32 Arg = 0;
	// 장비 여부
	if ( CCharaInfoContext::i()->FindCharaInfoIndexByItemID(CharaID) != -1 )
	{
		i3::wstring temp_outstatus;
		bool Expendable = (pGoods->_AuthType == ITEM_TYPE_COUNT) ;
		usf::GetGoodsUseDay(CalcItemStatus(CharaID), false, Expendable, Type, Arg, temp_outstatus);
		outStatus = temp_outstatus;
	}
	else
	{
		i3::stack_wstring temp_outstatus;
		bool IsCounted = (pGoods->_AuthType == ITEM_TYPE_COUNT) ;
		usf::GetGoodsUseDay_Zero(false, IsCounted, temp_outstatus);
		outStatus = temp_outstatus;
	}

	const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo(CharaID);
	if( pEquipInfo)
	{
		g_pCharaInfoDataBase->GetCharacterName(CharaID, outItemName); //sizeof(outItemName));
		pEquipInfo->getDescription(outDesc);
	}		
	else
	{
		i3::sprintf(outItemName, L"%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID); 
		outDesc.clear();
	}
	
	if (!( DesigUtil::CheckWearable_MyItemByDesig(CharaID) ))
	{
		i3::sprintf(outRequired, 
			L"{col:255,0,0,255}%s (%s){/col}", GAME_RCSTRING("STR_SHOP_CAN_BUYING"), GAME_RCSTRING("STR_SHOP_CANNOT_BUYING"));  // 호칭 필요..
	}
	else
	{
		if ( DesigUtil::IsItemLimitedByDesigSlot(CharaID) )
		{
			i3::sprintf(outRequired, 
				L"%s (%s)", GAME_RCSTRING("STR_SHOP_CAN_BUYING"),  GAME_RCSTRING("STR_TBL_DESIGNATION_TEXT") );  // 호칭 ..	
		}
		else
		{
			i3::sprintf(outRequired, L"%s",  GAME_RCSTRING("STR_SHOP_CAN_BUYING") );
		}
	}
}


ItemStatus	UITabShopCharacter::CalcItemStatus(INT32 itemID)
{
	T_ItemDBIdx ware_idx = CInvenList::i()->FindWareDBIdx_ByItemID(itemID);
	return usf::GetItemStatus(itemID, ware_idx);
}

void	UITabShopCharacter::OnLoadAllScenes()
{
	m_category = UISHOP_SUBTAB_CHARACTER_ALL;
	m_currFilterType = SISFT_NEW;
}


void	UITabShopCharacter::ChangeCharacterCategory( UIShop_SubTab_Character  category)
{
	if (m_category == category)
		return;

	// 상품 목록을 다시 만든다.
	m_category = category;
	m_phaseShop->ChangeSubCategory();

}

void UITabShopCharacter::OnClickMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}

void UITabShopCharacter::OnEnterMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->EnableCurrentSlotToolTip(SlotIndex, pGoodsPack);
}

void UITabShopCharacter::OnLeaveMouse_Slot(INT32 SlotIndex, SHOP_GOODS_PACK* pGoodsPack)
{
	m_toolTip->DisableToolTip(SlotIndex, pGoodsPack);
}

void UITabShopCharacter::CloseTooltip()
{
	m_toolTip->DisableToolTip_Forced();
}


void	UITabShopCharacter::_MakeFilterList_ByShopCategory()
{
	ucf::ComboString str_combo;
	str_combo	<< GAME_RCSTRING("STR_SHOP_FILTER_NEW") 
				<< GAME_RCSTRING("STR_SHOP_FILTER_NAME") 
				<< GAME_RCSTRING("STR_SHOP_FILTER_POINT") 
				<< GAME_RCSTRING("STR_SHOP_FILTER_CASH");

	LuaState* L = UILua::CallLuaFunction(m_phaseShop->GetMenuScene(), "SetFilterList");
	const i3::wstring cs_wstr = str_combo;
	i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());
	i3Lua::PushInteger(L, m_currFilterType);
	UILua::EndLuaFunction(L, 2);
}
