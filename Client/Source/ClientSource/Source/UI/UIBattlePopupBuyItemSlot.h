#pragma once

#include "iSlot.h"

class UIBattlePopupBuyItem;

class UIBattlePopupBuyItemSlot : public UI::iSlot
{
	friend class UIBattlePopupBuyItem;

public:
	UIBattlePopupBuyItemSlot();
	~UIBattlePopupBuyItemSlot() {}

	void SetParent(UIBattlePopupBuyItem *parent)	{ mParent = parent;	}

private:
	UIBattlePopupBuyItem *mParent;
	bool is_buyable_chara;

public:
	void clicked();
	void clicked_combo(INT32 tab);

public:
	void update_list();
	INT32 get_now_slot()const	{	return iSlot::get_now_slot();	}

public:
	void OnCreate( i3GameNode * pParent);

	void OnLoadAllScenes();
	void OnUnloadAllScenes();

	void OnUpdate( REAL32 rDeltaSeconds);
	void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code);

protected:
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
};
