#if !defined( __UI__ShopInfoMgr_H__)
#define __UI__ShopInfoMgr_H__

#include "UIItemInfo_Def.h"
#include "ShopContext.h"


namespace shop_util {

struct ShopInfo
{
	ShopInfo() : info(nullptr), type(0), usage(0), sale_type(0) {}

	const SHOP_GOODS_PACK* info;
	UINT32 type, usage, sale_type;
	item_def::Category category;
};

class ShopPackInfoList
{
	typedef i3::vector< ShopInfo > List;

public:
	void clear()	{	m_list.clear();	}
	void resize(size_t size) { m_list.resize(size); }
	size_t size() const { return m_list.size(); }

	void push(const ShopInfo & info);
	void push(const SHOP_GOODS_PACK* pack, UINT32 type, UINT32 usage, UINT8 sale_type, item_def::Category category);
	void push(const ShopPackInfoList& list);

	void erase(INT32 idx);
	void insert_front(const ShopInfo & info);
	void SearchKeyword(const WCHAR* wstr);

	const ShopInfo& operator[](size_t idx) const { return m_list[idx]; }
	ShopInfo& operator[](size_t idx) { return m_list[idx]; }

	const List& get_list() const { return m_list; }

private:
	List m_list;
};

} //end of shop_util

class ShopInfoList
{
	enum { SHOP_SLOT = 0 , GIFT_SLOT = 1 };

public:
	void clear();
	void initialize(UINT8 ui8SlotType = 0);	//모든 상점 아이템을 셋팅한다.

	void list_up(item_def::Category category, UINT32 type);
	void list_up(item_def::WeaponComboType type);

	void list_up_all();
	void list_up_weapon();
	void list_up_chara();
	void list_up_item();

	const shop_util::ShopInfo& get(UINT32 idx) const;
	shop_util::ShopInfo& get(UINT32 idx);

	size_t size() const	{	return m_current_list.size();  }

	shop_util::ShopPackInfoList& get_list() { return m_current_list; }

	void sort(item_def::Category category);
	void SortType(item_def::Category category, SHOP_SORT_TYPE type);
	void SearchKeyword(const WCHAR* wstr);
	bool set_list_front_item(UINT32 ui32ItemID);

private:
	void arrange();

private:
	//입장시 모든 상점 아이템을 구한다.
	//category에 따라서 미리 구해 논 정보에서 filter 해서 정보를 구성하자.
	shop_util::ShopPackInfoList m_list[ item_def::CTG_MAX ];
	shop_util::ShopPackInfoList m_current_list;

	UINT8 m_slot_type;
};

namespace shop_util 
{
	//시연용임 삭제 가능
	void only_valid_item(shop_util::ShopPackInfoList& list); 
	void only_invalid_item(shop_util::ShopPackInfoList& list);
	void only_name_item(shop_util::ShopPackInfoList& list);
	void only_noname_item(shop_util::ShopPackInfoList& list);
}

class igShopInfoList
{
public:
	void list_up(UINT32 ctg);

public:
	shop_util::ShopPackInfoList m_list;
};

//class ig_domi_ShopInfoList
//{
//public:
//	void list_up(); // m_list 아이템중 0으로 셋팅될 가능성이 있다. (서버 상품 db에 없는 경우)
//
//	const SHOP_GOODS_PACK* find_domi_item_pack(CASHITEM_GROUP_TYPE buy_group_type);
//	const SHOP_GOODS_PACK* find_domi_cash_item_pack(CASHITEM_GROUP_TYPE buy_group_type);
//
//public:
//	shop_util::ShopPackInfoList m_domi_item_list, m_domi_cash_item_list;
//};

#endif
