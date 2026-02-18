#ifndef _UIBATTLE_POPUP_SKILLSHOP_CASHSLOT_H_
#define _UIBATTLE_POPUP_SKILLSHOP_CASHSLOT_H_

#include "UIBattlePopupSkillShopSlot.h"

class BattleSkillShopSlotCashItem : public BattleSkillShopSlot
{
public:
	BattleSkillShopSlotCashItem(UIBattlePopupSkillShop* p);
	virtual ~BattleSkillShopSlotCashItem();

	virtual void OnCreate(i3GameNode * pParent) override;
	virtual void OnUpdate(REAL32 tm) override;
	virtual void Request(INT32 slotIdx) override;
	virtual void InitItems() override;
	virtual void UpdateItems() override;
	virtual void ProcessKey() override;

	void DecreaseItemPossessionNum(INT32 group); // 보유 아이템 개수 1차감
	void OpenBattleShop();
	void OpenBattleShop(INT32 slotIdx);

protected:
	virtual void cb_Enabled_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
	virtual void cb_Disabled_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
	virtual void cb_Enter_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
	virtual void cb_Leave_impl(INT32 slot_idx, i3UIScene* slot_scn) override;

private:
	struct DOMI_SKILLSHOP_CASH_ITEM
	{
		i3::rc_wstring wstr_name; i3::rc_wstring explain; INT32 type; INT32 shape_idx;
		INT32 increase; INT32 group; INT32 min_remain;
		INT32 possessionNum;

		DOMI_SKILLSHOP_CASH_ITEM() : type(0), increase(0), group(0), possessionNum(0) {}
	};
	i3::vector<DOMI_SKILLSHOP_CASH_ITEM*> m_items;
};


#endif