#pragma once

#ifndef NC_BUILD

class ItemInfoList;

namespace itchk
{
	enum { cWeapon, cChara, cParts, cDino, cMax,};
	enum { iWeapon, iChara, iParts, iItem, iMax,};

	struct attribute
	{
		T_ItemDBIdx ware_idx;
		T_ItemID item_id;
		size_t chara_idx; //어떤 캐릭인지?
		bool exist_pef; //
	};
	typedef i3::vector<attribute> chk_list;

	void init_attribute(attribute & att, T_ItemDBIdx ware_idx, T_ItemID item_id, size_t chara_idx);

	void push_attribute(chk_list & list, const attribute& att);
	void push_attribute(chk_list & list, const ItemInfoList& item_info_list, size_t chara_idx);
	void push_attribute(chk_list & list, const i3::vector<INVEN_BUFFER*>& item_info_list);

	bool check_pef(const attribute& att);
	void check_pef(chk_list& list);

	size_t get_not_exist_pef_count(const chk_list& list);

	void check_mis_match_list(chk_list& ret_list, const chk_list& dest_list, const chk_list& src_list);

	i3::string make_context_log_stream(const attribute & att);
	i3::string make_context_log_stream(const char* title, const chk_list & list);
}


class CheckerEquipments
{
public:
	CheckerEquipments();

	void operator()();

private:
	void init_chara_equip();
	void init_equip();
	void init_equip_part_by_chara_slot_idx(itchk::chk_list & out_list, INT32 chara_slot_idx);
	void init_inven();
	void init_inven_parts(itchk::chk_list & out_list);

private:
	itchk::chk_list check_chara_equip_list[itchk::cMax];
	itchk::chk_list check_inven_equip_list[itchk::iMax];
};

#endif