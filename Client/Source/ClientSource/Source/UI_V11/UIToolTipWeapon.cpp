#include "pch.h"
#include "UI_V11/UIToolTipWeapon.h"
#include "UI_V11/UIPhaseInventory_V11.h"
#include "UI_V11/UISlotInvenWeapon_V11.h"

#include "UI/UIUtil.h"
#include "UI/ItemImageMgr.h"

I3_CLASS_INSTANCE( UIToolTipWeapon);

UIToolTipWeapon::UIToolTipWeapon()
{
	m_pImageCtrl = NULL;
}

UIToolTipWeapon::~UIToolTipWeapon()
{
	I3_SAFE_RELEASE( m_pImageCtrl);
}

UIPhaseInventory_V11 * UIToolTipWeapon::GetInventory( void)
{
	return (UIPhaseInventory_V11*) m_pParent;
}

/*virtual*/ void UIToolTipWeapon::_InitializeAtLoad( i3UIScene * pScene)
{
	UIToolTipBase::_InitializeAtLoad( pScene);

	m_pImageCtrl = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet1" );
	I3ASSERT( m_pImageCtrl != NULL);
}

/*virtual*/ void UIToolTipWeapon::_ClearAtUnload( i3UIScene * pScene)
{
	UIToolTipBase::_ClearAtUnload( pScene);

	m_pImageCtrl->RestoreSizePos();
	I3_SAFE_RELEASE( m_pImageCtrl);
}

/*virtual*/ void UIToolTipWeapon::OnCreate( i3GameNode * pParent)
{
	UIToolTipBase::OnCreate( pParent);
	AddScene( "Scene/Main/PBRe_Tooltip_WeaponitemSlot.i3UIs" );
}

/*virtual*/ bool UIToolTipWeapon::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIToolTipBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	T_ItemID ItemID = *(INT32*) pArg1;
	
	INT32 curr_slot = *reinterpret_cast<INT32*>(pArg2);

	VEC3D* slot_pos_first = GetInventory()->GetSlotInvenWeapon(0)->GetPos();
	VEC3D* slot_pos_last  = GetInventory()->GetSlotInvenWeapon(INVENTORY_SLOT_COUNT-1)->GetPos();
	VEC2D* slot_size      =	GetInventory()->GetSlotInvenWeapon(0)->GetSize();
	VEC2D* tooltip_size   =	m_pFrameWnd->getSize();
	float pos_y = ( slot_pos_first->y + slot_pos_last->y + slot_size->y - tooltip_size->y ) * .5f;

	VEC3D* slot_pos = GetInventory()->GetSlotInvenWeapon(curr_slot)->GetPos();
	float pos_x = slot_pos->x - tooltip_size->x;

	m_pFrameWnd->setPos(pos_x, pos_y);
	GetSlide()->AddFrameWnd(m_pFrameWnd);

	// Image Set
	GetItemImageMgr()->SelectImage(m_pImageCtrl, ItemID);

	// Information
	SetWeaponInfo(ItemID);

	return true;
}

void UIToolTipWeapon::SetWeaponInfo( T_ItemID num)
{
	i3::rc_wstring szName;
	i3::rc_wstring szDesc;
	i3::rc_wstring szCategory;

	GameUI::GetWeaponString( szName, &szDesc, &szCategory, num);

	LuaState* L = _CallLuaFunction(0, "SetMainInfo");
	i3Lua::PushStringUTF16To8(L, szName);
	i3Lua::PushStringUTF16To8(L, szCategory);		// 상점툴팁쪽에서 슬롯일부내용이 넘어와서 출력하는데, 우선 인벤툴팁은 공백으로 비웠습니다.(11.08.19.수빈)
	i3Lua::PushStringUTF16To8(L, L"");
	i3Lua::PushStringUTF16To8(L, L"");

	_EndLuaFunction(L, 4);
	
	i3::stack_wstring strRange;
	i3::stack_wstring strBulletCount;
	i3::stack_wstring temp;

	INT32 damage, rapid, accuracy, recoil_v, recoil_h, speed;

	GameUI::GetNormalizedWeaponInfo( num, damage, rapid, accuracy, recoil_v, recoil_h, speed, strRange, strBulletCount);

	L = _CallLuaFunction( "SetWeaponInfo");

	i3Lua::PushNumber(L, float(damage) / 100.f);
	i3Lua::PushNumber(L, float(rapid) / 100.f);
	i3Lua::PushNumber(L, float(accuracy) / 100.f);
	i3Lua::PushNumber(L, float(recoil_v) / 100.f);
	i3Lua::PushNumber(L, float(recoil_h) / 100.f);
	i3Lua::PushNumber(L, float(speed) / 100.f);

	GameUI::GetGaugeString(damage, temp);
	i3Lua::PushStringUTF16To8(L, temp);
	GameUI::GetGaugeString(rapid, temp);
	i3Lua::PushStringUTF16To8(L, temp);
	GameUI::GetGaugeString(accuracy, temp);
	i3Lua::PushStringUTF16To8(L, temp);
	GameUI::GetGaugeString(recoil_v, temp);
	i3Lua::PushStringUTF16To8(L, temp);
	GameUI::GetGaugeString(recoil_h, temp);
	i3Lua::PushStringUTF16To8(L, temp);
	GameUI::GetGaugeString(speed, temp);
	i3Lua::PushStringUTF16To8(L, temp);

	i3Lua::PushStringUTF16To8(L, strRange);
	i3Lua::PushStringUTF16To8(L, strBulletCount);
	
	_EndLuaFunction(L, 14);
}

