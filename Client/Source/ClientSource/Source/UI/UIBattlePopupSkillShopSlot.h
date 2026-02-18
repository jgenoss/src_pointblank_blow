#pragma once

#include "iSlot.h"

class UIBattlePopupSkillShop;

class BattleSkillShopSlot : public UI::iSlot
{
	friend class UIBattlePopupSkillShop;

public:
	BattleSkillShopSlot(UIBattlePopupSkillShop* p);
	virtual ~BattleSkillShopSlot() {}

public:
	virtual void OnCreate(i3GameNode * pParent);
	void OnLoadAllScenes();
	void OnUnloadAllScenes();
	void OnRevive(i3RenderContext * pCtx);
	virtual void OnUpdate(REAL32 rDeltaSeconds);
	virtual void ProcessKey() = 0;

	void Request();
	virtual void Request(INT32 slotIdx) = 0;
	virtual void InitItems() = 0;
	virtual void UpdateItems() = 0;

	void SetVisible(bool visible) { mSlot.SetVisible(visible); }
	void SetEnableMouseOver(bool enable) { mSlot.SetEnableMouseOver(enable); }

protected:
	virtual void cb_Enabled_impl(INT32 slot_idx, i3UIScene* slot_scn) {}
	virtual void cb_Disabled_impl(INT32 slot_idx, i3UIScene* slot_scn) {}
	virtual void cb_Enter_impl(INT32 slot_idx, i3UIScene* slot_scn) {}
	virtual void cb_Leave_impl(INT32 slot_idx, i3UIScene* slot_scn) {}

	UIBattlePopupSkillShop* m_parent;
	INT32 m_totalPage;
	INT32 m_currPage;
};