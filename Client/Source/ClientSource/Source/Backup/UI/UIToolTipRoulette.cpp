#include "pch.h"

#include "UIToolTipRoulette.h"
#include "UIUtil.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "Shop.h"
#include "ui/UIItemInfo_Def.h"
#include "i3Base/string/ext/sprintf.h"


//I3_ABSTRACT_CLASS_INSTANCE( UIToolTipRoulette, UIToolTipBase);
I3_CLASS_INSTANCE( UIToolTipRoulette);

I3_CLASS_INSTANCE( UIToolTipRoulette_Weapon);//, UIToolTipRoulette );
I3_CLASS_INSTANCE( UIToolTipRoulette_NonWeapon);//, UIToolTipRoulette );

UIToolTipRoulette::UIToolTipRoulette(void) : m_pGoodId(0)
{
	m_pGoodId = 0;
	m_pImageSet = NULL;
}

UIToolTipRoulette::~UIToolTipRoulette(void)
{
}

void	UIToolTipRoulette::OnCreate(i3GameNode* pParent)
{
	UIToolTipBase::OnCreate(pParent);	
}

void	UIToolTipRoulette::EnableCurrentSlotToolTip(VEC2D pos, INT32 goodsid)
{

	i3UIFrameWnd* pFrameWnd = static_cast<i3UIFrameWnd*>(GetScene(0)->getFirstChild());
	pFrameWnd->setPos(pos.x, pos.y);
	
	GetScene(0)->EnableScene(true);
	GetScene(0)->ShowTop();

	this->SetMoreInfo(goodsid);

	UIToolTipRoulette::SetMainInfo(goodsid);
	
	m_pGoodId = goodsid;

}

void	UIToolTipRoulette::SetMainInfo(INT32 goodsid)
{
	SHOP_GOODS pGoods;
	pGoods._GoodsID = goodsid;
	pGoods._ItemID = g_pGameContext->ConvertGoodsID(goodsid);
	GetItemImageMgr()->SelectImage(m_pImageSet, pGoods._ItemID);
}

void	UIToolTipRoulette::DisableToolTip( )	//화면에서 제거..
{
	GetScene(0)->EnableScene(false);
	m_pGoodId = 0;
}

void	UIToolTipRoulette::DisableToolTip_Forced()
{
	GetScene(0)->EnableScene(false);
	m_pGoodId = 0;
}

void	UIToolTipRoulette::OnLoadAllScenes()
{
	m_pImageSet = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), this->GetImageSetName() );
	GetScene(0)->EnableScene(false);	// 우선 닫을 것..
}

void	UIToolTipRoulette::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_pImageSet);
}

//*********************************************************************//



UIToolTipRoulette_Weapon::UIToolTipRoulette_Weapon()
{
	
}

void UIToolTipRoulette_Weapon::OnCreate(i3GameNode* pParent)
{
	base::OnCreate(pParent);
	AddScene("UIRe/Scene/Main/PBRe_Tooltip_WeaponitemSlot.i3UIe");
}



const char* 	UIToolTipRoulette_Weapon::GetImageSetName() const
{
	return "i3UIButtonImageSet1";
}


void UIToolTipRoulette_Weapon::SetMoreInfo(INT32 goodsid)
{

	SHOP_GOODS pGoods;
	pGoods._GoodsID = goodsid;

	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(goodsid);
	if ( pGoodsList )
	{
		pGoods._ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;
		pGoods._AuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		pGoods._AuthArg = pGoodsList->_pui32ItemArg[0];
		pGoods._SaleMark = pGoodsList->_pGoodsData->_ui8Mark;
		pGoods._SaleDesignation = pGoodsList->_ppItemData[0]->_ui8Restriction;
	}

	i3::string itemName;
	i3::string itemDesc;
	char itemCategory[MAX_STRING_COUNT];
	i3::string itemStatus;
	char itemRequired[MAX_STRING_COUNT];
	ShopItemCategory cat = usf::GetShopItemCategory(&pGoods);
	switch (cat)
	{
	case SHOP_ITEM_CATEGORY_WEAPON: 
		Weapon_ByShopCategory( &pGoods, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
		break;
	case SHOP_ITEM_CATEGORY_CHARACTER:
		Character_ByShopCategory( &pGoods, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
		break;
	case SHOP_ITEM_CATEGORY_ITEM:
		Item_ByShopCategory( &pGoods, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
		break;
	default: 	I3ASSERT(0);	break;
	}

	//
	{
		LuaState* L = _CallLuaFunction(0, "SetMainInfo");
		i3Lua::PushA2UTF8String(L, itemName.c_str() );
		i3Lua::PushA2UTF8String(L, itemCategory);
		i3Lua::PushA2UTF8String(L, itemStatus.c_str());
		i3Lua::PushA2UTF8String(L, itemRequired);

		_EndLuaFunction(L, 4);

	}

	// 프로그레시브 바..
	char strRange[MAX_STRING_COUNT];
	char strBulletCount[MAX_STRING_COUNT];
	char temp[MAX_STRING_COUNT];

	INT32 damage, rapid, accuracy, recoil_v, recoil_h, speed;

	GameUI::GetNormalizedWeaponInfo(pGoods._ItemID, damage, rapid, accuracy, recoil_v, recoil_h, speed, 
		strRange, MAX_STRING_COUNT, strBulletCount, MAX_STRING_COUNT);
	
	{	// 루아 함수 작성..
		LuaState* L = _CallLuaFunction(0, "SetWeaponInfo");
		i3Lua::PushNumber(L, float(damage) / 100.f);
		i3Lua::PushNumber(L, float(rapid) / 100.f);
		i3Lua::PushNumber(L, float(accuracy) / 100.f);
		i3Lua::PushNumber(L, float(recoil_v) / 100.f);
		i3Lua::PushNumber(L, float(recoil_h) / 100.f);
		i3Lua::PushNumber(L, float(speed) / 100.f);
		
		GameUI::GetGaugeString(damage, temp, MAX_STRING_COUNT);
		i3Lua::PushA2UTF8String(L, temp);
		GameUI::GetGaugeString(rapid, temp, MAX_STRING_COUNT);
		i3Lua::PushA2UTF8String(L, temp);
		GameUI::GetGaugeString(accuracy, temp, MAX_STRING_COUNT);
		i3Lua::PushA2UTF8String(L, temp);
		GameUI::GetGaugeString(recoil_v, temp, MAX_STRING_COUNT);
		i3Lua::PushA2UTF8String(L, temp);
		GameUI::GetGaugeString(recoil_h, temp, MAX_STRING_COUNT);
		i3Lua::PushA2UTF8String(L, temp);
		GameUI::GetGaugeString(speed, temp, MAX_STRING_COUNT);
		i3Lua::PushA2UTF8String(L, temp);
			
		i3Lua::PushA2UTF8String(L, strRange);
		i3Lua::PushA2UTF8String(L, strBulletCount);
		
		_EndLuaFunction(L, 14);
	}
	
}

ItemStatus	UIToolTipRoulette::CalcItemStatus(INT32 itemID)
{
	UINT8	Type	= 0; UINT32	Arg		= 0;
	if ( !CInvenList::instance()->GetCharacterTypeByID( itemID, &Type, &Arg) )
		return ITEM_STATUS_NOHAVE;

	return ITEM_STATUS_HAVE;
}



void UIToolTipRoulette_NonWeapon::SetMoreInfo(INT32 goodsid)
{
	
	SHOP_GOODS pGoods;
	pGoods._GoodsID = goodsid;

	CShop* shop = g_pGameContext->GetShop();
	SHOP_GOODS_LIST * pGoodsList = shop->GetGoodsListData(goodsid);
	if ( pGoodsList )
	{
		pGoods._ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;
		pGoods._AuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		pGoods._AuthArg = pGoodsList->_pui32ItemArg[0];
		pGoods._SaleMark = pGoodsList->_pGoodsData->_ui8Mark;
		pGoods._SaleDesignation = pGoodsList->_ppItemData[0]->_ui8Restriction;
	}


	i3::string itemName;
	i3::string itemDesc;
	char itemCategory[MAX_STRING_COUNT] = {0,};
	i3::string itemStatus;
	char itemRequired[MAX_STRING_COUNT] = {0,};

	ShopItemCategory cat = usf::GetShopItemCategory(&pGoods);
	switch (cat)
	{
	case SHOP_ITEM_CATEGORY_WEAPON: 
		Weapon_ByShopCategory( &pGoods, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
		break;
	case SHOP_ITEM_CATEGORY_CHARACTER:
		Character_ByShopCategory( &pGoods, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
		break;
	case SHOP_ITEM_CATEGORY_ITEM:
		Item_ByShopCategory( &pGoods, itemName, itemDesc, itemCategory, itemStatus, itemRequired);
		break;
	default: 	I3ASSERT(0);	break;
	}

	//
	{
		LuaState* L = _CallLuaFunction(0, "SetMainInfo");
		i3Lua::PushA2UTF8String(L, itemName.c_str());
		i3Lua::PushA2UTF8String(L, itemDesc.c_str());
		i3Lua::PushA2UTF8String(L, itemCategory);
		i3Lua::PushA2UTF8String(L, itemStatus.c_str());
		i3Lua::PushA2UTF8String(L, itemRequired);

		_EndLuaFunction(L, 5);

	
	}

	//UI의 크기를 늘립니다. 
	INT32 pLength =	(INT32)itemDesc.length();		
	INT32 pAddsize = 0;
	if( pLength > 140)
	{
		pAddsize = 40;
	}
	
	m_pInfoCtrl[0]->setSize(m_pOriginalsize[0].x, m_pOriginalsize[0].y + pAddsize);
	m_pInfoCtrl[1]->setSize(m_pOriginalsize[1].x, m_pOriginalsize[1].y + pAddsize);
	m_pInfoCtrl2->setSize(m_pOriginalsize[2].x, m_pOriginalsize[2].y + pAddsize);
	
}


UIToolTipRoulette_NonWeapon::UIToolTipRoulette_NonWeapon()
{

	m_pInfoCtrl[0]			= NULL;
	m_pInfoCtrl[1]			= NULL;
	m_pInfoCtrl2			= NULL;
	memset( &m_pOriginalsize, 0, sizeof(VEC2D)*3);
}
UIToolTipRoulette_NonWeapon::~UIToolTipRoulette_NonWeapon()
{
}
void UIToolTipRoulette_NonWeapon::OnCreate(i3GameNode* pParent)
{
	base::OnCreate(pParent);
	AddScene("UIRe/Scene/Main/PBRe_Tooltip_itemSlot.i3UIe");

}

const char* 	UIToolTipRoulette_NonWeapon::GetImageSetName() const
{
	return "i3UIButtonImageSet1";
}


/*virtual*/ void UIToolTipRoulette_NonWeapon::_InitializeAtLoad( i3UIScene * pScene)
{
	UIToolTipBase::_InitializeAtLoad( pScene);

	m_pInfoCtrl[0] = (i3UIFrameWnd *)pScene->FindChildByName("main");
	m_pInfoCtrl[1] = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3_LCR");
	m_pInfoCtrl2 = (i3UIStaticText *)pScene->FindChildByName("i3UIStaticText8_LCR");

	m_pOriginalsize[0] = *m_pInfoCtrl[0]->getSize();
	m_pOriginalsize[1] = *m_pInfoCtrl[1]->getSize();
	m_pOriginalsize[2] = *m_pInfoCtrl2->getSize();

}

////***********************************************UTIL FUNCTION***********************************///////

void	UIToolTipRoulette::Character_ByShopCategory(SHOP_GOODS* pGoods, 
													i3::string& outItemName, i3::string& outDesc, char* outCategory, i3::string& outStatus, char* outRequired)
{
	INT32 CharaID = pGoods->_ItemID;

	StringCchCopy(outCategory, MAX_STRING_COUNT, usf::GetCharacterItemCategoryString(CharaID));

	//	속성:
	if ( GET_ITEM_FLAG_TYPE(CharaID) != ITEM_TYPE_POINT )
	{
		usf::GetShopPeriodCountString(pGoods, outStatus);
		//switch( itemUsage)
		//{
		//case ITEM_TYPE_MAINTENANCE:	i3::snprintf( outStatus, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_USABLE_TERM"), itemNum);/*%d일권*/	break;
		//case ITEM_TYPE_EXPENDABLE:	i3::safe_string_copy( outStatus, GAME_STRING("STR_POPUP_DISPOSABLE"), MAX_STRING_COUNT );/*1회용*/	break;
		//}
	}
	else
	{
		outStatus.clear();
	}

	const CEquipInfo * pEquipInfo = g_pCharaInfoDataBase->GetInfo( CharaID);
	if( pEquipInfo)
	{
		g_pCharaInfoDataBase->GetCharacterName(CharaID, outItemName); 
		pEquipInfo->getDescription(outDesc);
	}		
	else
	{
		i3::sprintf(outItemName, "%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID);
		outDesc[0] = 0;
	}
	
//	outRequired[0] = 0;
	char szUserTitle[256] = "";
	if (!IsPossibleEquipUserTitleCharacter(CharaID, g_pGameContext->GetTitleInfo(), szUserTitle))
	{
		StringCchPrintf(outRequired, MAX_STRING_COUNT, 
			"{col:255,0,0,255}%s (%s){/col}", GAME_STRING("STR_SHOP_CAN_BUYING"), GAME_STRING("STR_SHOP_CANNOT_BUYING"));  // 호칭 필요..
	}
	else
	{
		if (szUserTitle[0] != 0)
		{
			StringCchPrintf(outRequired, MAX_STRING_COUNT, 
				"%s (%s)", GAME_STRING("STR_SHOP_CAN_BUYING"),  GAME_STRING("STR_TBL_DESIGNATION_TEXT") );  // 호칭 ..	
		}
		else
		{
			StringCchPrintf(outRequired, MAX_STRING_COUNT, "%s",  GAME_STRING("STR_SHOP_CAN_BUYING") );
		}
	}
	
}


void	UIToolTipRoulette::Item_ByShopCategory(SHOP_GOODS* pGoods, 
											   i3::string& outItemName, i3::string& outDesc, char* outCategory, i3::string& outStatus, char* outRequired)
{
	if (usf::IsSetItem(pGoods))
	{
		outStatus.clear();
		outRequired[0] = 0;

		INT32 GoodsID = pGoods->_GoodsID;

		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(GoodsID);
		if (pInfo)
		{
			usf::GetItemItemCategoryString(outCategory, UIShop_Item_Category(pInfo->GetCatagory()), true);

			outItemName = pInfo->GetScriptName();
			outDesc = pInfo->GetDescription();
		}
		else
		{
			i3::sprintf(outItemName, "INVALID\nGoods:%d\nItem:%d", pGoods->_GoodsID, pGoods->_ItemID);
			outDesc.clear();
		}
		
		return;
	}

	INT32 itemID		= pGoods->_ItemID;

	ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(itemID);

	if( pInfo )
	{
		usf::GetItemItemCategoryString(outCategory, UIShop_Item_Category(pInfo->GetCatagory()), false);

		g_pShopItemInfoDataBase->GetItemName(itemID, outItemName );
		outDesc = pInfo->GetDescription();
	}
	else
	{
		i3::sprintf(outItemName, "%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID);
		outDesc.clear();
	}

//			pInfo->GetCatagory();

	// 보유 여부
	//INT32 itemUsage = GET_ITEM_FLAG_TYPE(itemID);
	//INT32 itemNum = GET_ITEM_FLAG_NUMBER(itemID);
	//	속성:
	if ( GET_ITEM_FLAG_TYPE(itemID) != ITEM_TYPE_POINT )
	{
		usf::GetShopPeriodCountString(pGoods, outStatus);
		//switch( itemUsage)
		//{
		//case ITEM_TYPE_MAINTENANCE:	i3::snprintf( outStatus, MAX_STRING_COUNT, GAME_STRING("STR_POPUP_USABLE_TERM"), itemNum);/*%d일권*/	break;
		//case ITEM_TYPE_EXPENDABLE:	i3::safe_string_copy( outStatus, GAME_STRING("STR_POPUP_DISPOSABLE"), MAX_STRING_COUNT );/*1회용*/	break;
		//}
	}
	else
	{
		outStatus.clear();
	}
	
	outRequired[0] = 0;
}

void	UIToolTipRoulette::Weapon_ByShopCategory(SHOP_GOODS* pGoods, 
												 i3::string& outItemName, i3::string& outDesc, char* outCategory, i3::string& outStatus, char* outRequired)
{
	INT32 itemID = pGoods->_ItemID;

	WEAPON_CLASS_TYPE ClassType = (WEAPON_CLASS_TYPE)GET_ITEM_FLAG_CLASS(itemID);
	INT32 WeaponNum = GET_ITEM_FLAG_NUMBER(itemID);
	// KEYWORD : WEAPON_MINIAXE	일반 나이프에 다른 무기를 들고 있는 경우입니다. 해당 무기가 표시되도록 세팅해줘야 합니다.
	GUI_WEAPON_VALID( ClassType, WeaponNum);

	if ( GET_ITEM_FLAG_TYPE(itemID) != ITEM_TYPE_POINT )
	{
		usf::GetShopPeriodCountString(pGoods, outStatus);
	}
	else
	{
		outStatus.clear();
	}

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( ClassType, WeaponNum);
	
	if (pInfo)
	{
		pInfo->GetWeaponName( outItemName);

		if( outItemName.empty() )
			i3::sprintf(outItemName, "NO NAME\nGoods:%d\nItem:%d", pGoods->_GoodsID, pGoods->_ItemID);
		
		pInfo->GetDescription(outDesc);

		// 슬롯에서 표시되었던 것을 이제 툴팁으로 옮김..
		// 1. 무기 카테고리..
		g_pWeaponInfoDataBase->GetWeaponCategoryEx(pInfo, outCategory, MAX_STRING_COUNT);
		
		if (IsItemByDesignation(itemID))	// 호칭 무기..  
		{
			if (CheckItemByDesignation(itemID, g_pGameContext->GetTitleInfo()))
			{	// 기존코드는 구입가능시 표시가 없었으나 이 경우엔 필요할지 모른다.
				StringCchPrintf(outRequired, MAX_STRING_COUNT, "%s (%s)", GAME_STRING("STR_SHOP_CAN_BUYING"), GAME_STRING("STR_TBL_DESIGNATION_TEXT"));		// 호칭..
			}
			else
			{
				#if defined(BUY_WEAPON_BY_CASH)	
					#if defined( ENABLE_BUY_CASH_WEAPON_IF_NEED_USER_TITLE) 
					UINT32 weaponBuyType = (UINT32)g_pGameContext->GetShop()->GetGoodsPrice(pGoods->_GoodsID, NULL, NULL);
					if (weaponBuyType == GOODS_PRICE_CASH)
					{
						StringCchPrintf(outRequired, MAX_STRING_COUNT, "%s (%s)", GAME_STRING("STR_SHOP_CAN_BUYING"), GAME_STRING("STR_TBL_DESIGNATION_TEXT"));		// 호칭..
					}
					else
					#endif
					{
						StringCchPrintf(outRequired, MAX_STRING_COUNT, "{col:255,0,0,255}%s (%s){/col}", GAME_STRING("STR_SHOP_CANNOT_ITEM_BUYING"), GAME_STRING("STR_SHOP_CANNOT_BUYING"));  // 호칭 필요..
					}
				#else
					StringCchPrintf(outRequired, MAX_STRING_COUNT, "{col:255,0,0,255}%s (%s){/col}", GAME_STRING("STR_SHOP_CANNOT_ITEM_BUYING"), GAME_STRING("STR_SHOP_CANNOT_BUYING"));  // 호칭 필요..
				#endif
			}
		}
		else
		{
			StringCchPrintf(outRequired, MAX_STRING_COUNT, "%s",  GAME_STRING("STR_SHOP_CAN_BUYING") );
		}
		
	}
	else
	{
		i3::sprintf(outItemName, "%s\nGoods:%d\nItem:%d", INVALID_TEXT, pGoods->_GoodsID, pGoods->_ItemID);

		// 여기서 컨티뉴를 건드리면 안되고, 나머지 정보는 리셋시켜서 함수전송..
		outDesc[0] = 0;
		outCategory[0] = 0;
		outRequired[0] = 0;
	}
}