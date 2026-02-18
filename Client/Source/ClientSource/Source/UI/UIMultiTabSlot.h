#pragma once

#include "iSlot.h"
#include "ShopContext.h"

#include "i3Base/itl/signal/signal_common.h"

namespace item_def
{
	enum Category;
	enum WeaponComboType;
}

class iMultiTabSlot : public UI::iSlot
{
	typedef iMultiTabSlot me;
	friend class UIPhaseInventory;	//cb_update_slot 사용하기 위해

protected:
	class iMultiTap *mParent;
	class ItemImageMgr* mImageMgr;
	class UIImgSetCtrl* mDpImgSetCtrl;
	i3UIScene* mDpScene;
	i3UIScene* mDpAttScene;

	item_def::Category mCategory;
	UINT32 mType;
	item_def::WeaponComboType m_WeaponType;
	bool use_combo_type;
	SHOP_SORT_TYPE m_SortType;

	UINT8 mSlotType;

	i3::wstring m_SearchStr;

	UINT32 m_iBuyType;
	UINT32 m_iCurrentBuyType;
	UINT32 m_iCurrentBuySlot;

	i3::scoped_connection_set		m_SigConnection;

public:
	iMultiTabSlot(iMultiTap *parent);
	virtual ~iMultiTabSlot() {}

	i3UIScene * GetSlotScene(INT32 idx) { return mSlot.GetSlotScene(idx);}

public:
	void	update_slot(bool b1, bool b2);		// 나중에 아시는 분이 인수 네이밍 수정좀 해주셔요... 

public:
	void set_focus_scroll()	{	mSlot.SetScrollFocus();	}
	void set_display_item(UIImgSetCtrl* pSetCtrl, i3UIScene* dp_scn, i3UIScene* att_scn);
	void MoveBackScene()	{	mSlot.MoveBackScene();	}
	void SetSlotSkip(bool skip)	{ mSlot.SetSlotSkip(skip); }

	item_def::Category GetCategory() { return mCategory; }
	UINT32 GetType() { return mType; }
	
	UINT32 SetBuyType(UINT32 type) { return m_iCurrentBuyType = type; }
	UINT32 SetBuySlot(UINT32 slot) { return m_iCurrentBuySlot = slot; }

public:
	virtual void OnCreate( i3GameNode * pParent) = 0;

	virtual void OnLoadAllScenes();
	virtual void OnUnloadAllScenes();

	virtual void OnRevive();

	virtual void OnUpdate( REAL32 rDeltaSeconds);
	virtual void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code);

	virtual void Initialize() {}

	virtual void Entrance() = 0; //현재 category 그대로 갱신한다.
	virtual void Entrance(item_def::Category category, UINT32 type, bool bscrool = true) = 0;
	virtual void Entrance(item_def::WeaponComboType type, bool bscrool = true) = 0;
	virtual void SearchKeyword(const WCHAR* wstr,bool bFocus) {};
	virtual void SortType(item_def::Category category, SHOP_SORT_TYPE type) {};
	virtual void SetBuySlotStr(void) {}

	virtual void ResetList() { }

protected:
	virtual void cb_Load_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Unload_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Enabled_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Disabled_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Enter_impl( INT32 slot_idx, i3UIScene* slot_scn );
	virtual void cb_Leave_impl( INT32 slot_idx, i3UIScene* slot_scn );

	virtual void update_item_info(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn, bool front_dp = false) = 0;
	virtual void update_item_attribute(INT32 slot_idx) = 0;

	virtual void update_impl(REAL32 rDeltaSeconds) {}

	

public:
	virtual void update_btn_state_at_first() {}
	void update_empty_item_attribute();

protected:
	void update_weapon_attribute_impl(T_ItemID itemId, const char* lua_name);
	void update_chara_attribute_impl(T_ItemID itemId);
	void update_parts_attribute_impl(T_ItemID itemId);
	void update_item_attribute_impl(T_ItemID itemId);
	void update_item_attribute_impl(SHOP_GOODS* goods);
};