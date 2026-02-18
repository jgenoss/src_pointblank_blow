#if !defined( __UI__InvenInfoMgr_H__)
#define __UI__InvenInfoMgr_H__

#include "UIItemInfoUtil.h"

namespace inven_util {

class InvenInfoList
{
public:
	void list_up(item_def::Category category, UINT32 type);
	void list_up(item_def::WeaponComboType type);
	void list_reset();

	const ITEM_INFO & find(UINT32 idx) const { return m_list[ idx ]; }
	ITEM_INFO & find(UINT32 idx) { return m_list[ idx ]; }

	size_t find_idx(const ITEM_INFO & info) const;
	size_t find_idx(const T_ItemID & itemId) const;
	size_t find_idx_by_ware_idx(const T_ItemDBIdx & ware_idx) const;

	UINT8 GetInfoItemType(INT32 idx, item_def::Category category, UINT32 type);

	size_t size() const	{	return m_list.size();  }
	void SearchKeyword(const WCHAR* wstr);
	void sort(item_def::Category category);

private:
	//inven slot list는 매번 갱신 하자.
	ItemInfoList m_list;
};

class SubInfoList
{
public:
	void list_up(item_def::Category category, UINT32 type);
	void list_up(item_def::SideCategory category, UINT32 type);

	void sort(item_def::Category category);
	void sort(item_def::SideCategory category);

	void remove_locked_weapon(); //weapon category 일 때 이용하라.

	const ITEM_INFO & find(UINT32 idx) const { return m_list[ idx ]; }
	ITEM_INFO & find(UINT32 idx) { return m_list[ idx ]; }

	size_t find_idx(const ITEM_INFO & item);

	size_t size() const	{	return m_list.size();  }

private:
	ItemInfoList m_list;
};

void get_dino_inven_list(ItemInfoList & list);

}	//end of namespace inven_util;


#endif
