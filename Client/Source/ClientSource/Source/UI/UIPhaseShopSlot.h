#if !defined( __UI_PHASE_Shop_Slot_H__)
#define __UI_PHASE_Shop_Slot_H__

#include "UIMultiTabSlot.h"

#include "UIShopInfoMgr.h"

class ShopSlot : public iMultiTabSlot
{
	typedef ShopSlot me;
	enum { SHOP_SLOT = 0, GIFT_SLOT = 1 };

public:
	ShopSlot(iMultiTap *parent);
	virtual ~ShopSlot() {}

private:

	ShopInfoList mInfoList;

	UINT32 m_ui32FirstSlotItemID;
	UINT8 m_ui8SlotType;
	i3::vector<i3::wstring> m_strBuyPoint, m_strBuyCash, m_strBuyPointPrice, m_strBuyCashPrice;

public:
	void clicked(INT32 idx, bool from_parent = false);
	void double_clicked(bool from_parent = false);
	bool IsRestriction(UINT32 ui32ItemID);
	void SetBuySlot(void);

public:
	virtual void OnCreate( i3GameNode * pParent) override;
	void set_list_front_item_by_ItemID(UINT32 ui32ItemID);

#ifdef I3_DEBUG
	UINT32 get_get();
#endif

private:
	void update_item_info_normal(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	void update_item_info_front(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	typedef void (me::*pf_update_item_info)(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn);
	pf_update_item_info pf_update_item_info_arr[2];

	virtual void update_item_info(UIImgSetCtrl* pSetCtrl, INT32 slot_idx, i3UIScene* scn, bool front_dp = false) override;
	virtual void update_item_attribute(INT32 slot_idx) override;
	void update_item_attribute2(INT32 slot_idx); //비교속성

	void	UpdateCharaView( INT32 slotIdx);

	bool	get_cart_enable(void);
	INT32	get_sale_mark(UINT8 sale_mark);
	INT32	get_item_status(ItemStatus item_status);

	virtual void Initialize() override;

	virtual void Entrance() override;
	virtual void Entrance(item_def::Category category, UINT32 type, bool bscrool = true) override;
	virtual void Entrance(item_def::WeaponComboType type, bool bscrool = true) override;

	virtual void OnUnloadAllScenes() override;

	virtual void	SearchKeyword(const WCHAR* wstr,bool bFocus) override;
	virtual void	SortType(item_def::Category category, SHOP_SORT_TYPE type) override;
	virtual void	SetBuySlotStr(void) override;

	bool is_shop_slot(void)	const { return m_ui8SlotType == SHOP_SLOT; }
};

#endif	

