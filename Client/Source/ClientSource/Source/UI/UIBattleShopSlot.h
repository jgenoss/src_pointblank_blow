#pragma once

#include "iSlot.h"
#include "UIShopInfoMgr.h"

namespace BsS //Battle Shop Slot
{
	enum	SlotSize
	{	
		ROW = 3, COL = 3, SLOT = ROW * COL, 
	};
}

class BattleShopSlot : public UI::iSlot
{
	friend class UIBattlePopupShop;

	class UIBattlePopupShop* mParent;
	class ItemImageMgr* mImageMgr;
	
public:
	BattleShopSlot(UIBattlePopupShop* p);
	virtual ~BattleShopSlot();

public:
	void clicked();
	void double_clicked();

public:
	void OnCreate( i3GameNode * pParent);
	void OnLoadAllScenes();
	void OnUnloadAllScenes();

	void OnUpdate( REAL32 rDeltaSeconds);

#if ENABLE_BATTLESHOP_SCROLL 
	void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code);
#endif

public:
	void MoveBackScene();
	void Entrance(UINT32 tap);
	void SelectSlot(); //현재 위치 아이템 선택
	void SelectSlot(INT32 sel_idx); //sel_idx == -1 이면 선택이 없다.

#if ENABLE_BATTLESHOP_SCROLL 
	void hit_test_enter();
	void hit_test_leave();
#endif
private:
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn ) override;

private:
	igShopInfoList m_item_list;
	//IGShopContext* ig_cont;
	//INT32 item_size; 
	//INT32 tap_index;
	INT32 selected_slot_idx;
};
