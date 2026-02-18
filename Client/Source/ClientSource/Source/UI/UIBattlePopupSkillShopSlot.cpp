#include "pch.h"
#include "UIBattlePopupSkillShopSlot.h"
#include "UIBattlePopupSkillShop.h"

namespace
{
	static const INT32 g_total_row_slot = 4;
	static const INT32 g_total_col_slot = 2;
	static const INT32 g_slot_cnt_per_page = 8;
}

BattleSkillShopSlot::BattleSkillShopSlot(UIBattlePopupSkillShop* p) : m_parent(p), m_totalPage(0),m_currPage(0)
{
}

void BattleSkillShopSlot::OnCreate( i3GameNode * pParent)
{
}

void BattleSkillShopSlot::OnLoadAllScenes()
{
	INT32 left = 0, top =0;
	m_parent->GetLeftTopPos(left, top); 
	mSlot.OnLoadAllScenes(left+8,top+82,0,0);

	mSlot.SetEnableMouseOver(true);
	iSlot::reset_select(g_slot_cnt_per_page);
}

void BattleSkillShopSlot::OnUnloadAllScenes()
{
	mSlot.OnUnloadAllScenes();
}

void BattleSkillShopSlot::OnRevive( i3RenderContext * pCtx)
{
	INT32 left = 0, top = 0;
	m_parent->GetLeftTopPos(left, top);
	mSlot.SetPosition(left, top);
}

void BattleSkillShopSlot::OnUpdate( REAL32 rDeltaSeconds)
{
	mSlot.OnUpdate();
}

void BattleSkillShopSlot::Request()
{
	INT32 slotIdx = iSlot::apply_select();
	Request(slotIdx);
}