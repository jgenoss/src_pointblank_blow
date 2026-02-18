#ifndef _UBP_SKILL_SHOP_H_
#define _UBP_SKILL_SHOP_H_

#include "UIBattlePopupBase.h"

enum SKILLSHOP_SLOT_TYPE
{
	SKILLSHOP_SLOT_SP,
	SKILLSHOP_SLOT_CASH,
	SKILLSHOP_SLOT_MAX
};

class BattleSkillShopSlot;
class UIBattlePopupSkillShop : public UIBattlePopupBase
{
	I3_CLASS_DEFINE(UIBattlePopupSkillShop, UIBattlePopupBase);

public:
	UIBattlePopupSkillShop();
	virtual ~UIBattlePopupSkillShop();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scn) override;
	virtual bool OnEntranceStart(void* arg1 = nullptr, void* arg2 = nullptr) override;
	virtual void OnExitEnd(void) override;
	virtual void OnLoadAllScenes() override;
	virtual bool OnKeyInput(i3InputDeviceManager* input) override;
	virtual void OnUpdate(REAL32 tm) override;

	void GetLeftTopPos(INT32& outLeft, INT32 & outTop);
	void RefreshPage();
	void GoPrePage();
	void GoNextPage();
	void GoSkillTab();
	void GoItemTab();

	BattleSkillShopSlot* GetSlot(SKILLSHOP_SLOT_TYPE type) const { return m_slot[type]; }

private:
	BattleSkillShopSlot* m_slot[SKILLSHOP_SLOT_MAX];
	BattleSkillShopSlot* m_currSlot;
};

#endif