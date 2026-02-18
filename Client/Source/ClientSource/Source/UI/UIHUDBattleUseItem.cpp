#include "pch.h"
#include "UIHUDBattleUseItem.h"

#include "BattleSlotContext.h"
#include "UI/UIUtil.h"
#include "UIHudUtil.h"
#include "i3Base/profile/profile.h"

UIHudBattleUseItemNotice::UIHudBattleUseItemNotice(UIHudManager * p) : iHudBase(p)
{
	m_pBoxImage = nullptr;
	m_pText = nullptr;
}

UIHudBattleUseItemNotice::~UIHudBattleUseItemNotice()
{
}

void UIHudBattleUseItemNotice::InitializeAtLoad(i3UIScene * pScene)
{
	i3_prof_func();

	AddFrameWnd(pScene, "BattleUseItem");

	m_pBoxImage = FindControl<i3UIButtonImageSet*>(pScene, "BoxImage");
	m_pText		= FindControl<i3UIStaticText*>(pScene, "ItemText");

	m_rNoticeTime = 0.f;
	m_rImageTime = 0.f;
	m_bNoticeEnable = false;
}

void UIHudBattleUseItemNotice::Update(REAL32 rDeltaSeconds)
{
	// 3초가 안지날 경우
	if (m_rNoticeTime < 3.f)
	{
		m_rNoticeTime += rDeltaSeconds;
	}
	else
	{
		m_rNoticeTime = 3.5f;
		iHudBase::SetEnable(false);
	}
}

void UIHudBattleUseItemNotice::SetParameter(INT32 arg1, INT32 arg2)
{
	// Arg1 : Slot Index
	// Arg2 : ItemID
	if (arg1 < 0 || SLOT_MAX_COUNT <= arg1) return;
	if (arg2 == 0) return;

	BATTLEUSEITEM_ICON ItemType;
	i3::rc_wstring	m_ItemText = _GetItemNameAndItemType(arg2, ItemType);
	i3::rc_wstring	m_NickName = BattleSlotContext::i()->getNickForSlot(arg1);

	i3::sprintf(m_String, GAME_RCSTRING("STR_TBL_HUD_REINFORCEDITEM_GET"), m_ItemText, m_NickName);
	m_pText->SetText(m_String);

	m_rNoticeTime = 0.f;
	m_bNoticeEnable = true;

	// Shape 변경
	if(ItemType != -1)
		m_pBoxImage->SetShapeIdx((INT32)ItemType);

	iHudBase::SetEnable(true);
}

void UIHudBattleUseItemNotice::event_start_battle(INT32 arg, const i3::user_data & UserData)
{
	i3_prof_func();

	if (hu_ut::is_test_changing_hud() == false)
		iHudBase::SetEnable(false);
}

void UIHudBattleUseItemNotice::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data & UserData)
{
	//필요는 없음..
	if (hu_ut::is_test_changing_hud() == false)
		iHudBase::SetEnable(false);
}

i3::rc_wstring UIHudBattleUseItemNotice::_GetItemNameAndItemType(T_ItemID itemID, BATTLEUSEITEM_ICON& shapeIdx)
{
	i3::rc_wstring ItemName;
	ITEM_TYPE itemtype = (ITEM_TYPE)GET_ITEM_TYPE(itemID);

	if (ITEM_TYPE_WRAP_COUNT == itemtype)
	{
		switch (itemID)
		{
			// 아이템 아이디를 박는 부분
			// HP
			case CASHITEM_ITEMID_HP_RECOVERY:
			{
				shapeIdx = BATTLEUSEITEM_HP_RECOVERY;
				ItemName = GAME_RCSTRING("STR_TBL_HUD_REINFORCEDITEM_GET_HP");
			}break;
			// Gravity
			case CASHITEM_ITEMID_GRAVITY_RESOTRE:
			{
				shapeIdx = BATTLEUSEITEM_GRAVITY_RESTORE;
				ItemName = GAME_RCSTRING("STR_TBL_HUD_REINFORCEDITEM_GET_GRAVITYRESTORE");
			}break;
			// Unbeatable
			case CASHITEM_ITEMID_CHARA_UNBEATABLE:
			{
				shapeIdx = BATTLEUSEITEM_UNBEATABLE;
				ItemName = GAME_RCSTRING("STR_TBL_HUD_REINFORCEDITEM_GET_UNBEATABLE");
			}break;
		}
	}

	// 무기 인지 체크
	else if (ITEM_TYPE_UNKNOWN < itemtype && itemtype < ITEM_TYPE_CHARA)
	{
		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);
		pWeaponInfo->GetWeaponName(ItemName);

		// 하드코딩.. Shape 번로를 구분할 방법을 찾아야 하는데..
		switch (itemID)
		{
		case 116002:
			shapeIdx = BATTLEUSEITEM_RPG7;
			break;
		case 105032:
			shapeIdx = BATTLEUSEITEM_BARRET;
			break;
		default :
			shapeIdx = BATTLEUSEITEM_NONE;
			break;
		}
	}

	return ItemName;
}
