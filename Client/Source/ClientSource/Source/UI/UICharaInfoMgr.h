#if !defined( __UI__CharaInfoMgr_H__)
#define __UI__CharaInfoMgr_H__

#include "UIItemInfoUtil.h"

class CharaInfoMgr;

//////////////////////////////////////////////////////////////////////////

struct EquipInfoList
{
	ItemInfoList m_item[item_def::SCTG_MAX];

	ItemInfoList & operator [](UINT32 idx)
	{
		return m_item[idx];
	}

	const ItemInfoList & operator [](const UINT32 idx) const
	{
		return m_item[idx];
	}
};

typedef i3::vector<EquipInfoList> EquipInfoVector;

struct CharaEquipList 
{
	EquipInfoVector	m_chara_list;
	EquipInfoList	m_share_list;

	UINT8			m_main_chara_slot[TEAM_COUNT];

	CharaEquipList(void);

	void clear(void);
	void clear(UINT32 chara_idx);

	void resize(void);
	void resize(UINT32 chara_idx);
};

//////////////////////////////////////////////////////////////////////////

class UserEquipInfo
{
public:
	enum ShareUserEquip { SUE_CHARA_RED = 0, SUE_CHARA_BLUE, SUE_HELMET, SUE_BERET, SUE_DINO, SUE_MAX };

private:
	ItemInfoList m_red_parts;
	ItemInfoList m_blue_parts;
	ITEM_INFO	 m_common[UserEquipInfo::SUE_MAX];

	ItemInfoList m_weapon;

public:
	UserEquipInfo(void);
	~UserEquipInfo(void);

	void reset(void);
	void set_user_equip_info( CharaEquipList & data, UINT32 red_char_idx, UINT32 blue_char_idx );
	
	const ItemInfoList & get_parts_list( const TEAM_TYPE team = TEAM_RED ) const
	{
		return (team == TEAM_RED) ? m_red_parts : m_blue_parts;
	}

	const ItemInfoList & get_weapon_list( void) const
	{
		return m_weapon;
	}

	const ITEM_INFO & get_equip_item( const ShareUserEquip type ) const
	{
		return m_common[type];
	}
	
	bool is_equip_part(const T_ItemID & itemId);

	size_t is_parts_find_idx_team( T_ItemID itemId, const TEAM_TYPE team = TEAM_RED )
	{
		if( team == TEAM_RED )
			return m_red_parts.find_idx(itemId);
		else
			return m_blue_parts.find_idx(itemId);
	}

	size_t is_common_find_idx( T_ItemID itemId )
	{
		for( int idx = 0 ; idx < UserEquipInfo::SUE_MAX ; idx++ )
		{
			if( m_common[idx].m_TItemID == itemId)
				return idx;
		}

		return UINT_MAX;
	}

	
};

//////////////////////////////////////////////////////////////////////////

//SideMenu에서 사용하는 클래스 이다.
class CharaInfoMgr 
{
	CharaEquipList	m_equip_list;
	CharaEquipList	m_equip_backup_list;

	UserEquipInfo	m_current_equip_info;

public:
	struct CurrentState
	{
		enum { MODIFY_CHARA = 0x01, MODIFY_PARTS = 0x02, MODIFY_WEAPON = 0x04 };

		ITEM_INFO info;
		item_def::SideCategory ctg;
		item_def::SideCategory chara_menu_ctg; //왼쪽 메뉴 Category
		UINT32	slot_idx;
		UINT8	chara_idx;
		UINT8	modify_flag;	

		bool is_empty_info(void)		{ return (info == item_def::NULL_ITEM_INFO); }
		bool is_modify_flag(UINT8 flag)	{ return ((modify_flag & flag) == flag); }

		bool modify_chara(void)			{ return is_modify_flag(MODIFY_CHARA);	}
		bool modify_parts(void)			{ return is_modify_flag(MODIFY_PARTS);	}
		bool modify_weapon(void)		{ return is_modify_flag(MODIFY_WEAPON);	}

	} m_current_state;

private:
	void resize(); 
	void resize(UINT32 chara_idx);

private:
	bool replace_default_normal(const ITEM_INFO & item_info);
	bool replace_default_dino(const T_ItemID & itemId);

public:
	CharaInfoMgr();
	~CharaInfoMgr();

	static CharaInfoMgr* instance();

	void clear();

	void list_up();
	void list_up(UINT32 chara_idx);

	const ITEM_INFO & find(UINT32 chara_idx, item_def::SideCategory category, UINT32 slot_idx) const;
	const ITEM_INFO & find(item_def::SideCategory category, UINT32 slot_idx) const;	//common

	size_t size(UINT32 chara_idx, item_def::SideCategory category) const;

	UINT32 find_equip_chara_idx(const T_ItemID & itemId) const;	//모든 캐릭에 대해서 itemid를 보유하고 있는지 확인
	UINT32 find_equip_chara_idx(const T_ItemID & itemId, const T_ItemDBIdx & ware_idx) const;	//모든 캐릭에 대해서 itemid를 보유하고 있는지 확인
	
	bool is_chara_equipped(const T_ItemID & itemId, UINT32 chara_idx) const;	//해당 chara_idx 가 itemId를 들고 잇는지 확인
	bool is_chara_equipped(const T_ItemID & itemId, const T_ItemDBIdx & ware_idx, UINT32 chara_idx) const;	//해당 chara_idx 가 itemId를 들고 잇는지 확인
	
	bool is_current_equip(const T_ItemID & itemID );							//m_current_equip_info에 장비하고 있는지 확인 한다 ( 일단 인벤용 ) 

	bool replace_default(const ITEM_INFO & item_info);
	T_ItemDBIdx get_equip_ware_idx(const T_ItemID & itemId, UINT32 chara_idx) const;

	void change_equip_item(const ITEM_INFO & item_info);
	void change_equip_item(const ItemInfoConvert & item_info_convert, const ITEM_INFO & item_info);

	void adjust_equip_with_version(T_ItemID item_id);
	void adjust_equip_with_version_UIChara(T_ItemID item_id);
	
	bool is_changed_equip_item(UINT32 chara_idx, item_def::SideCategory category); //해당 category에 리스트가 변경 되었는지 확인

	const ItemInfoList & get_item_info_list( UINT32 chara_idx, item_def::SideCategory category)
	{
		return m_equip_list.m_chara_list[chara_idx][category];
	}

	const ItemInfoList & get_item_info_list( item_def::SideCategory category = item_def::SCTG_PARTS)
	{
		return m_equip_list.m_share_list[category]; 
	}

	void build_current_equip_info(void);

	const ItemInfoList & get_current_equip_parts( const TEAM_TYPE team = TEAM_RED ) const
	{
		return m_current_equip_info.get_parts_list(team);
	}

	const ItemInfoList & get_current_equip_weapon( void) const
	{
		return m_current_equip_info.get_weapon_list();
	}

	const ITEM_INFO & get_current_equip_item( const UserEquipInfo::ShareUserEquip type ) const
	{
		return m_current_equip_info.get_equip_item(type);
	}

public:
	struct Common
	{
		UINT32 count;		//변경된 사람 수 (공통 내용이면 1이다.)
		UINT32 flag;		//공통 아이템 변경 사항
	};
	struct Private
	{
		UINT32 flag;		//개인 무기/Parts 변경 flag
		UINT8 chara_idx;	//캐릭터 Slot 번호
		UINT8 list_idx;		//list idx
	};
	typedef i3::vector< Private > PrivateList;

	Common m_report_common;
	PrivateList m_report_private_list;
	bool m_error_main_chara;

public:
	bool check_changed_equip();				// 변경된 내용이 있는지 분석한다.
	bool check_changed_main_chara_slot();	// 메인 캐릭터가 변경되었는지 확인
	void change_equip();					// 분석된 내용을 rollback 또는 갱신 한다.
	bool send_event_change_equip(void);

private:
	bool	isAuthAndStateCharaChange;

public:
	void	SetAuthAndStateCharaChange() { isAuthAndStateCharaChange = true; }
	const bool	GetAuthAndStateCharaChange() { return isAuthAndStateCharaChange; }
};

#endif
