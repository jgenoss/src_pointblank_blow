#if !defined( __UI__Item_Info_Util_H__)
#define __UI__Item_Info_Util_H__

#include "UIItemInfo_Def.h"
#include "ShopDef.h"

namespace item_def {

/*********************************/
//일반 변환
item_def::Category get_category_tab(T_ItemID item_id);
item_def::Category get_category(T_ItemID item_id);
item_def::Category get_category_goodsID(T_GoodsID goods_id);
item_def::SideCategory get_side_category(T_ItemID item_id);
bool is_chara_category(T_ItemID item_id);
bool is_dino_category(T_ItemID item_id);
item_def::PartsType_ShopPreview get_shop_preview_parts_type(T_ItemID item_id);

WEAPON_SLOT_TYPE get_weapon_slot_type(T_ItemID item_id);
WEAPON_CLASS_TYPE get_weapon_class_type(T_ItemID item_id);
ITEM_TYPE get_weapon_type(T_ItemID item_id);
ITEM_TYPE get_chara_type(T_ItemID item_id);
ITEM_TYPE get_parts_type(T_ItemID item_id);
ITEM_TYPE get_item_type(T_ItemID item_id);	//아직 불안정
ITEM_TYPE get_type(T_ItemID item_id);

WEAPON_CLASS_TYPE get_weapon_usage(T_ItemID item_id);
CHARACTER_CLASS_TYPE get_chara_usage(T_ItemID item_id);
CHARACTER_CLASS_TYPE get_parts_usage(T_ItemID item_id);
ITEM_TYPE get_item_usage(T_ItemID item_id);
INT32 get_item_number(T_ItemID item_id); //아이템 항목이 아닌경우 -1을 리턴한다.
UINT get_usage(T_ItemID item_id);

/*********************************/
//타입 변환
item_def::Category convert(item_def::SideCategory category);
item_def::SideCategory convert(item_def::Category category);
CHAR_EQUIPMENT_PARTS convert(item_def::PartsType type);

item_def::WeaponType convert_weapon_type(ITEM_TYPE type);
UINT32 convert_chara_type(ITEM_TYPE type);
item_def::PartsType convert_parts_type(ITEM_TYPE type);
item_def::ItemType convert_item_type(ITEM_TYPE type);

//InvenList 클래스에서 정보 얻기 위해서 사용된다.
SHOP_GOODS_TYPE convert_goods_type(item_def::Category category);	
WEAPON_SLOT_TYPE convert_weapon_type(item_def::WeaponType type);
UINT32 convert_chara_type(UINT32 type);
CHARACTER_CLASS_TYPE convert_parts_type(item_def::PartsType type);
ITEM_TYPE convert_item_type(item_def::ItemType type);

// 베레모 변환.
item_def::PartsType	get_beret_parts_type(void);
CHARACTER_CLASS_TYPE get_beret_class_type(void);

/*********************************/
//일반 함수
bool is_default_item(T_ItemID item_id);
bool is_default_parts(T_ItemID item_id, T_ItemID chara_item_id);
bool is_shared_parts_item(T_ItemID item_id, T_ItemID chara_item_id);

ITEM_INFO get_default_item(T_ItemID item_id); //weapon
ITEM_INFO get_default_item(UINT32 idx, WEAPON_CLASS_TYPE type); //weapon

ITEM_INFO get_default_item(T_ItemID chara_item_id, INT32 idx); //parts
ITEM_INFO get_default_dino(void);

UINT get_shop_slot_idx(T_ItemID item_id);
UINT get_char_slot_idx(T_ItemID item_id);

bool is_locked_weapon(UINT8 mode_flag, T_ItemID item_id); //mode에 따라 lock(사용할 수 없는)된 무기 인지 알려준다. (true: lock 된 무기)
bool is_locked_weapon(UINT8 mode_flag, WEAPON_CLASS_TYPE classType, WEAPON_SLOT_TYPE usage);

bool is_direct_use_item(T_ItemID item_id);

}	//end of namespace item_def




/* ITEM_INFO Container
	보통 WeaponType / PartsType 과 매칭 되며, fuct_idx_to_slot_idx로 해당 idx를 얻는다.
*/
class ItemInfoList
{
	typedef i3::vector< ITEM_INFO > List;

public:
	ItemInfoList() {}
	~ItemInfoList() {}

public:
	void clear()	{	m_list.clear();	}
	void resize(size_t size)	{	m_list.resize( size ); }
	size_t size() const { return m_list.size(); }
	
	void push( const ITEM_INFO & info);		//m_list를 resize() 함수 호출 후에 사용할 경우 중간에 값을 넣을 수 없음. 되도록 clear() 후에 사용하자.
	void push( const ItemInfoList& list);

	void	remove( size_t idx);
	
	size_t find_idx( const ITEM_INFO & info ) const;
	size_t find_idx( const T_ItemID & itemId ) const;
	size_t find_idx( const T_ItemID & itemId, const T_ItemDBIdx & ware_idx ) const;
	size_t find_idx_by_ware_idx(const T_ItemDBIdx & ware_idx) const;

	void remove_unshared(const T_ItemID & charaID);
	void remove_duplicate_default_item();
	
	void default_part_id_make_to_mainchara();	//받은 인벤리스트를 가지고 디폴트 파츠를 만드는데 캐릭터별로 파츠 정보가 다른 구조 일 때 사용하는 함수 ( 1.5 구조용 )
	void default_skin_id_make_to_mainchara();	// 받은 인벤리스트에서 Skin을 가지고 있을 경우 Default일 경우에는 제외 시킨다.
	void default_part_id_make_to_union();		//받은 인벤리스트를 가지고 디폴트 파츠를 만드는데 캐릭터가 모든 파츠를 공유하는 구조 일 때 사용하는 함수 ( 1.0, 1.1 구조용 )

	void SearchKeyword(const WCHAR* wstr);
	
	const ITEM_INFO & operator[](size_t idx) const { return m_list[idx]; }
	ITEM_INFO & operator[](size_t idx) { return m_list[idx]; }

	void operator=( const ItemInfoList & rhs );

	const List& get_list() const { return m_list; }

private:
	List m_list;
};








/*********************************/
//자동 변환
struct ItemInfoConvert
{
	::ITEM_INFO m_info;

	/*********************************/
	ItemInfoConvert(const ::ITEM_INFO & info)	{ m_info = info;	}
	ItemInfoConvert(::T_ItemID  itemid)	{	m_info.m_TItemID = itemid;	m_info.m_TItemDBIdx = 0; }

	/*********************************/
	ItemInfoConvert& operator= (const ::T_ItemID itemid)	{	m_info.m_TItemID = itemid;	m_info.m_TItemDBIdx = 0;	}
	ItemInfoConvert& operator= (const ::ITEM_INFO & info)	{	m_info = info;	}

	/*********************************/
	item_def::Category get_category() const;
	item_def::SideCategory get_side_category() const;

	item_def::WeaponType get_weapon_type() const;
	item_def::WeaponComboType get_weapon_combo_type() const;

	UINT32 get_chara_type() const;

	item_def::SKINTYPE	get_skin_type() const;

	item_def::PartsType get_parts_type() const;
	item_def::PartsType get_beret_type() const;

	item_def::ItemType get_item_type() const;


	/*********************************/
	UINT32 get_shop_slot_idx() const;
	UINT32 get_char_slot_idx() const;

	bool is_default_item() const;
	bool is_default_item_to_DBidx(const T_ItemDBIdx itemdbidx ) const;
	bool is_default_parts(T_ItemID chara_item_id) const;	//현재 캐릭에 대한 기본 파츠 유무를 판단한다.
};

#endif
