#ifndef _UIBATTLE_POPUP_SKILLSHOP_SPSLOT_H_
#define _UIBATTLE_POPUP_SKILLSHOP_SPSLOT_H_

#include "UIBattlePopupSkillShopSlot.h"

class BattleSkillShopSlotSpItem : public BattleSkillShopSlot
{
public:
	BattleSkillShopSlotSpItem(UIBattlePopupSkillShop* p);
	virtual ~BattleSkillShopSlotSpItem();

	virtual void OnCreate(i3GameNode * pParent) override;
	virtual void Request(INT32 slotIdx) override;
	virtual void InitItems() override;
	virtual void UpdateItems() override;
	virtual void ProcessKey() override;

protected:
	virtual void cb_Enabled_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
	virtual void cb_Disabled_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
	virtual void cb_Enter_impl(INT32 slot_idx, i3UIScene* slot_scn) override;
	virtual void cb_Leave_impl(INT32 slot_idx, i3UIScene* slot_scn) override;

private:
	struct DOMI_SKILLSHOP_SP_ITEM
	{
		i3::rc_wstring wstr_name; i3::rc_wstring explain; INT32 type; INT32 shape_idx;
		INT32 cost; INT32 increase; INT32 lv; INT32 round; bool forSale;
		DOMI_SKILLSHOP_SP_ITEM() : type(0), cost(0), increase(0), lv(0), round(0), forSale(false) {}
	};

	typedef i3::vector<DOMI_SKILLSHOP_SP_ITEM*>::iterator	iterator;

	i3::vector<DOMI_SKILLSHOP_SP_ITEM*> m_items;
};

#endif