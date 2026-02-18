#pragma once

#include "UIMultiTabSlot.h"

#include "UIInvenInfoMgr.h"
#include "UISecCount.h"

class InvenSlot : public iMultiTabSlot
{
	typedef InvenSlot my_type;

public:
	InvenSlot(iMultiTap *parent);
	virtual ~InvenSlot() {}

private:
	inven_util::InvenInfoList mInfoList;

public:
	void clicked(INT32 idx, bool from_parent = false);
	void double_clicked();
	void remove_clicked();
	void extend_clicked();
	void repair_clicked();

private:
	void update_btn_state(INT32 slot_idx);

public:
	virtual void OnCreate( i3GameNode * pParent) override;

private:
	void update_item_info_normal_impl(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	void update_item_info_normal_impl2(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	
	void update_item_info_normal(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	void update_item_info_front(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	typedef void (my_type::*pf_update_item_info)(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	pf_update_item_info pf_update_item_info_arr[2];
	pf_update_item_info pf_update_item_info_normal_arr[2];

	virtual void update_impl(REAL32 rDeltaSeconds) override;
	virtual void update_btn_state_at_first() override;
	virtual void update_item_info(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn, bool front_dp = false) override;
	virtual void update_item_attribute(INT32 slot_idx) override;

	virtual void Entrance() override;
	virtual void Entrance(item_def::Category category, UINT32 type, bool bscrool = true) override;
	virtual void Entrance(item_def::WeaponComboType type, bool bscrool = true) override;

	virtual void	SearchKeyword(const WCHAR* wstr, bool bFocus) override;

public:
	void update_dp_item_info();
	virtual void ResetList() override;

private:
	void	 sec_count_start(INT32 result_time) {}
	void	 sec_count_stop(INT32 result_time);
	void sec_count_change(INT32 result_time);
	void sec_count_done(INT32 result_time);

public:
	void go_stop();

private:
	TUISecCallback<my_type, &my_type::sec_count_start, &my_type::sec_count_change, &my_type::sec_count_done, &my_type::sec_count_stop> sec_count_cb;
};