#include "pch.h"

#include "UI_V11/UIToolTipNonWeapon.h"
#include "UI_V11/UISlotInvenChara_V11.h"
#include "UI_V11/UISlotInvenItem_V11.h"
#include "UI_V11/UIPhaseInventory_V11.h"

#include "UI/ItemImageMgr.h"
#include "UI/UIUtil.h"
#include "UI/UIShopFunction.h"

I3_CLASS_INSTANCE( UIToolTipNonWeapon);

UIToolTipNonWeapon::UIToolTipNonWeapon()
{
	m_pImageCtrl	= NULL;
	m_pInfoCtrl0		= NULL;
	m_pInfoCtrl1		= NULL;
	memset( &m_pOriginalsize, 0, sizeof(VEC2D)*2);
}

UIToolTipNonWeapon::~UIToolTipNonWeapon()
{
	I3_SAFE_RELEASE( m_pImageCtrl);
}

/*virtual*/ void UIToolTipNonWeapon::_InitializeAtLoad( i3UIScene * pScene)
{
	UIToolTipBase::_InitializeAtLoad( pScene);

	m_pInfoCtrl0 = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3_LCR");
	m_pInfoCtrl1 = (i3UIStaticText *)pScene->FindChildByName("i3UIStaticText8_LCR");

	m_pOriginalsize[0] = *m_pInfoCtrl0->getSize();
	m_pOriginalsize[1] = *m_pInfoCtrl1->getSize();

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet1" );
	I3ASSERT( m_pImageCtrl != NULL);
}

/*virtual*/ void UIToolTipNonWeapon::_ClearAtUnload( i3UIScene * pScene)
{
	UIToolTipBase::_ClearAtUnload( pScene);

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE( m_pImageCtrl);
}

UIPhaseInventory_V11 * UIToolTipNonWeapon::GetInventory( void)
{
	return (UIPhaseInventory_V11*) m_pParent;
}

/*virtual*/ void UIToolTipNonWeapon::OnCreate( i3GameNode * pParent)
{
	UIToolTipBase::OnCreate( pParent);
	AddScene("Scene/Main/PBRe_Tooltip_itemSlot.i3UIs");
}

/*virtual*/ bool UIToolTipNonWeapon::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	i3::rc_wstring szName;
	i3::rc_wstring szDesc;
	i3::wstring szCategory;

	INT32 ItemIdx = *(INT32*) pArg1;

	if( GetInventory()->isEnable() == false)
		return false;

	InvenInfo * pSlotInfo = GetInventory()->GetItem( ItemIdx);

	if( pSlotInfo == NULL)
		return false;

	if( UIToolTipBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	GetItemImageMgr()->SelectImage(m_pImageCtrl, pSlotInfo->_ItemID);
	
	INT32 curr_slot = *reinterpret_cast<INT32*>(pArg2);

	VEC3D* slot_pos_first;
	VEC3D* slot_pos_last;  
	VEC2D* slot_size ;   
	VEC3D* slot_pos; 

	ITEM_TYPE type = (ITEM_TYPE) GET_ITEM_TYPE( pSlotInfo->_ItemID );

	if( type == ITEM_TYPE_CHARA ||
		type == ITEM_TYPE_FACEGEAR ||
		type == ITEM_TYPE_HEADGEAR ||
		type == ITEM_TYPE_DINO )
	{
		GameUI::GetCharacterString( szName, szDesc, pSlotInfo->_ItemID);
		i3::rc_wstring szCharacCategory = usf::GetCharacterItemCategoryString(pSlotInfo->_ItemID);
		if (szCharacCategory.size() != 0)
			szCategory = szCharacCategory;

		slot_pos_first = GetInventory()->GetSlotInvenChara(0)->GetPos();
		slot_pos_last  = GetInventory()->GetSlotInvenChara(INVENTORY_SLOT_COUNT-1)->GetPos();
		slot_size      = GetInventory()->GetSlotInvenChara(0)->GetSize();
		slot_pos		= GetInventory()->GetSlotInvenChara(curr_slot)->GetPos();
	}
	else
	{
		GameUI::GetItemString( szName, szDesc,  pSlotInfo->_ItemID);
		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(pSlotInfo->_ItemID);
		if( pInfo == NULL ) return false;

		usf::GetItemItemCategoryString(szCategory, UIShop_Item_Category(pInfo->GetCatagory()), false);

		slot_pos_first = GetInventory()->GetSlotInvenItem(0)->GetPos();
		slot_pos_last  = GetInventory()->GetSlotInvenItem(INVENTORY_SLOT_COUNT-1)->GetPos();
		slot_size      = GetInventory()->GetSlotInvenItem(0)->GetSize();
		slot_pos	   = GetInventory()->GetSlotInvenItem(curr_slot)->GetPos();
	}
	
	VEC2D* tooltip_size   =	m_pFrameWnd->getSize();

	float pos_y = ( slot_pos_first->y + slot_pos_last->y + slot_size->y - tooltip_size->y ) * .5f;
	float pos_x = slot_pos->x - tooltip_size->x;

	INT32 pLength = szDesc.size();
	INT32 pAddsize = 0;
	if( pLength > 140)
	{
		pAddsize = 40;
	}

	m_pFrameWnd->setSize(tooltip_size->x, tooltip_size->y + pAddsize);
	m_pFrameWnd->setPos(pos_x, pos_y);

	m_pInfoCtrl0->setSize(m_pOriginalsize[0].x, m_pOriginalsize[0].y + pAddsize);
	m_pInfoCtrl1->setSize(m_pOriginalsize[1].x, m_pOriginalsize[1].y + pAddsize);

	GetSlide()->AddFrameWnd(m_pFrameWnd);
	

	LuaState * L = _CallLuaFunction( "SetMainInfo");
	i3Lua::PushStringUTF16To8(L, szName);
	i3Lua::PushStringUTF16To8(L, szDesc);
	i3Lua::PushStringUTF16To8(L, szCategory);		// 상점툴팁쪽에서 슬롯일부내용이 넘어와서 출력하는데, 우선 인벤툴팁은 공백으로 비웠습니다.(11.08.19.수빈)
	i3Lua::PushStringUTF16To8(L, L"");
	i3Lua::PushStringUTF16To8(L, L"");
	
	_EndLuaFunction(L, 5);

	return true;
}
