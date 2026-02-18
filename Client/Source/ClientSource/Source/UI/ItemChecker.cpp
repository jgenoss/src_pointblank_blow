#include "pch.h"
#ifndef NC_BUILD
#include "ItemChecker.h"

#include "UICharaInfoMgr.h"
#include "InvenDef.h"
#include "UIInvenInfoMgr.h"
#include "UIShopFunction.h"
#include "UIUtil.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/itoa.h"

#include <fstream>
#include <functional>

// itchk ------------//
void itchk::init_attribute(attribute & att, T_ItemDBIdx ware_idx, T_ItemID item_id, size_t chara_idx)
{
	att.ware_idx = ware_idx;
	att.item_id = item_id;
	att.chara_idx = chara_idx;
	att.exist_pef = false;
}
//
void itchk::push_attribute(chk_list & list, const attribute& att)
{
	list.push_back(att);
}
void itchk::push_attribute(chk_list & list, const ItemInfoList& item_info_list, size_t chara_idx)
{
	itchk::attribute att;

	size_t size = item_info_list.size();
	for(size_t i=0;i<size;++i)
	{
		itchk::init_attribute(att, item_info_list[i].m_TItemDBIdx, item_info_list[i].m_TItemID, chara_idx);
		list.push_back(att);
	}
}
void itchk::push_attribute(chk_list & list, const i3::vector<INVEN_BUFFER*>& item_info_list)
{
	itchk::attribute att;

	size_t size = item_info_list.size();
	for(size_t i=0;i<size;++i)
	{
		itchk::init_attribute(att, item_info_list[i]->_TItemWareDBIdx, item_info_list[i]->_ui32ItemID, 0);
		list.push_back(att);
	}
}

//
bool itchk::check_pef(const attribute& att)
{
	if( att.item_id == 0 && att.ware_idx == 0 ) //존재하지 않는 아이템(파츠 장착 안 할 경우)
		return true;

	return usf::CheckValidGoods_in_Pef(att.item_id);
}
void itchk::check_pef(chk_list& list)
{
	struct Fn
	{
		void operator()(attribute& att) const
		{
			if( att.item_id == 0 && att.ware_idx == 0 )	//존재하지 않는 아이템(파츠 장착 안 할 경우)
				att.exist_pef = true;
			else
				att.exist_pef = usf::CheckValidGoods_in_Pef(att.item_id);
		}
	};
	i3::for_each(list.begin(), list.end(), Fn() );
}

//
size_t itchk::get_not_exist_pef_count(const chk_list& list)
{
	struct Fn : std::unary_function<itchk::attribute, bool>
	{
		bool operator()(const itchk::attribute& att) const
		{
			return att.exist_pef;
		}
	};
	return std::count_if(list.begin(), list.end(), std::not1( Fn() ) );
}

//
void itchk::check_mis_match_list(chk_list& ret_list, const chk_list& dest_list, const chk_list& src_list)
{
	ret_list.clear();

	struct Fn : public std::binary_function<itchk::attribute, itchk::attribute, bool>
	{
		bool operator()(const itchk::attribute& lhs, const itchk::attribute& rhs) const
		{
			if(rhs.item_id == 0 && rhs.ware_idx == 0 ) return true;	//존재하지 않는 아이템(파츠 장착 안 할 경우)
			if(rhs.ware_idx == 0 ) return (lhs.item_id == rhs.item_id);	//무기/캐릭 종류임
			return (lhs.item_id == rhs.item_id) && (lhs.ware_idx == rhs.ware_idx);	//파츠류
		}
	};

	chk_list::const_iterator dest_b_it = dest_list.begin();
	chk_list::const_iterator dest_e_it = dest_list.end();

	size_t size = src_list.size();
	for(size_t i=0;i<size;++i)
	{
		chk_list::const_iterator it = std::find_if(dest_b_it, dest_e_it, std::bind2nd(Fn(), src_list[i]) );

		if( it == dest_e_it )
		{
			ret_list.push_back( src_list[i] );
		}
	}
}

i3::string itchk::make_context_log_stream(const attribute & att)
{
	i3::string ret;
	i3::stack_string str_stack;
	
	ret.append("ItemId : ");	i3::itoa(att.item_id, str_stack); ret.append( str_stack );
	ret.append(" WareIdx : ");	i3::itoa((T_ItemDBIdx)att.ware_idx, str_stack); ret.append( str_stack );
	ret.append(" EquipCharaIdx : "); i3::itoa(att.chara_idx, str_stack);	ret.append( str_stack );
	ret.append(" PefExist : ");	
	ret += (att.exist_pef) ? "yes" : "no";
	ret.append( "\n" );

	return i3::move(ret);
}

i3::string itchk::make_context_log_stream(const char* title, const chk_list & list)
{
	i3::string ret = i3::format_string("\n%s\n", title);

	size_t size = list.size();
	for(size_t i=0;i<size;++i)
		ret.append( itchk::make_context_log_stream(list[i]) );

	return i3::move(ret);
}

// CheckerEquipments ------------//
CheckerEquipments::CheckerEquipments()
{
}

void CheckerEquipments::operator()()
{
	init_chara_equip();
	init_equip();
	init_inven();

	bool enable_log = false;

	size_t not_exist_pef_chara_equip_size =0;
	for(size_t i=0; i < itchk::cMax; ++i)
	{
		itchk::check_pef( check_chara_equip_list[i]);
		not_exist_pef_chara_equip_size += itchk::get_not_exist_pef_count(check_chara_equip_list[i]);
	}
	if( not_exist_pef_chara_equip_size != 0 )
	{
		i3::rc_wstring wstrCaption = L"Warning!!!";
		GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_MESSAGE_EQUIPPED_ERROR"), &wstrCaption);
		enable_log = true;
	}

	size_t not_exist_pef_inven_equip_size =0;
	for(size_t i=0; i < itchk::iMax; ++i)
	{
		itchk::check_pef( check_inven_equip_list[i]);
		not_exist_pef_inven_equip_size += itchk::get_not_exist_pef_count( check_inven_equip_list[i]);
	}
	if( not_exist_pef_inven_equip_size != 0 )
	{
		i3::rc_wstring wstrCaption = L"Warning!!!";
		GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STR_POPUP_MESSAGE_EQUIPPED_ERROR2"), &wstrCaption);
		enable_log = true;
	}


	itchk::chk_list mis_match_list[itchk::cMax];

	itchk::check_mis_match_list( mis_match_list[itchk::cWeapon], check_inven_equip_list[itchk::iWeapon], check_chara_equip_list[itchk::cWeapon] );
	if( mis_match_list[itchk::cWeapon].empty() == false )
	{
		//캐릭터가 장착한 [무기]중에 인벤에 없는 아이템 발견
		i3::rc_wstring wstrCaption = L"Warning!!!";
		GameUI::MsgBox(MSG_TYPE_ERROR, L"found equipped [weapon] are not in inventory", &wstrCaption);
		enable_log = true;
	}

	/*itchk::check_mis_match_list( mis_match_list[itchk::cChara], check_inven_equip_list[itchk::iChara], check_chara_equip_list[itchk::cChara] );
	if( mis_match_list[itchk::cChara].empty() == false )
	{
		GameUI::MsgBox(MSG_TYPE_ERROR, "found equipped [character] are not in inventory", "Warning!!!");
		enable_log = true;
	}*/

	itchk::check_mis_match_list( mis_match_list[itchk::cParts], check_inven_equip_list[itchk::iParts], check_chara_equip_list[itchk::cParts] );
	if( mis_match_list[itchk::cParts].empty() == false )
	{
		i3::rc_wstring wstrCaption = L"Warning!!!";
		GameUI::MsgBox(MSG_TYPE_ERROR, L"found equipped [parts] are not in inventory", &wstrCaption);
		enable_log = true;
	}

	/*itchk::check_mis_match_list( mis_match_list[itchk::cDino], check_inven_equip_list[itchk::iChara], check_chara_equip_list[itchk::cDino] );
	if( mis_match_list[itchk::cDino].empty() == false )
	{
		GameUI::MsgBox(MSG_TYPE_ERROR, "found equipped [dino] are not in inventory", "Warning!!!");
		enable_log = true;
	}*/

	if( enable_log )
	{
		i3::string log_stream;

		log_stream.append("** equipped item list **\n");
		log_stream.append( itchk::make_context_log_stream("weapon", check_chara_equip_list[itchk::cWeapon]) );
		log_stream.append( itchk::make_context_log_stream("character", check_chara_equip_list[itchk::cChara]) );
		log_stream.append( itchk::make_context_log_stream("parts", check_chara_equip_list[itchk::cParts]) );
		log_stream.append( itchk::make_context_log_stream("dino", check_chara_equip_list[itchk::cDino]) );
		
		log_stream.append("\n\n");
		log_stream.append("** in inventory item list ** (EquipCharaIdx == 0)\n");
		log_stream.append( itchk::make_context_log_stream("weapon", check_inven_equip_list[itchk::iWeapon]) );
		log_stream.append( itchk::make_context_log_stream("character", check_inven_equip_list[itchk::iChara]) );
		log_stream.append( itchk::make_context_log_stream("parts", check_inven_equip_list[itchk::iParts]) );
		log_stream.append( itchk::make_context_log_stream("item", check_inven_equip_list[itchk::iItem]) );

		log_stream.append("\n\n");
		log_stream.append("** equipped items list are not in inventory **\n");
		if( mis_match_list[itchk::cWeapon].empty() == false )
			log_stream.append( itchk::make_context_log_stream("weapon", mis_match_list[itchk::iWeapon]) );
		if( mis_match_list[itchk::iChara].empty() == false )
			log_stream.append( itchk::make_context_log_stream("character", mis_match_list[itchk::iChara]) );
		if( mis_match_list[itchk::iParts].empty() == false )
			log_stream.append( itchk::make_context_log_stream("parts", mis_match_list[itchk::iParts]) );
		if( mis_match_list[itchk::cDino].empty() == false )
			log_stream.append( itchk::make_context_log_stream("dino", mis_match_list[itchk::cDino]) );

		std::ofstream ofs("check_item_list.txt");
		ofs.write(log_stream.c_str(), log_stream.size() );
		ofs.close();
	}
}





void CheckerEquipments::init_chara_equip()
{
	check_chara_equip_list[itchk::cChara].clear();

	INT32 size = CCharaInfoContext::i()->GetHasCharacterCount();

	for( INT32 chara_idx =0; chara_idx < size; ++chara_idx )
	{
		const CHARA_PARTS_INFO& parts = *CCharaInfoContext::i()->GetCharaPartsInfo( chara_idx );
		// vv3 - check
		itchk::attribute context;
		itchk::init_attribute(context, parts.GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA), parts.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA), chara_idx);
		check_chara_equip_list[itchk::cChara].push_back( context );
	}

	//// 공룡 정보
	//const COMMON_PARTS_INFO * pInfo = CCharaInfoContext::i()->GetCommonPartsInfo();

	//itchk::attribute context;
	//itchk::init_attribute(context, pInfo->getDino()->m_TItemDBIdx, pInfo->getDino()->m_TItemID, size);
	//check_chara_equip_list[itchk::cChara].push_back( context );
}
void CheckerEquipments::init_equip()
{
	check_chara_equip_list[itchk::cWeapon].clear();
	check_chara_equip_list[itchk::cParts].clear();
	check_chara_equip_list[itchk::cDino].clear();

	INT32 size = CCharaInfoContext::i()->GetHasCharacterCount();
	for( INT32 i =0; i < size; ++i )
	{
		const ItemInfoList& weapon_list = CharaInfoMgr::instance()->get_item_info_list( i, item_def::SCTG_WEAPON );
		itchk::push_attribute(check_chara_equip_list[itchk::cWeapon], weapon_list, i);

		//const ItemInfoList& parts_list = CharaInfoMgr::instance()->get_item_info_list( i, item_def::SCTG_PARTS );
		init_equip_part_by_chara_slot_idx(check_chara_equip_list[itchk::cParts], i);
	}

	const ItemInfoList& shared_list = CharaInfoMgr::instance()->get_item_info_list();
	itchk::push_attribute(check_chara_equip_list[itchk::cDino], shared_list, 0);
}

void CheckerEquipments::init_equip_part_by_chara_slot_idx(itchk::chk_list & out_list, INT32 chara_slot_idx)
{
	const ItemInfoList& parts_list = CharaInfoMgr::instance()->get_item_info_list( chara_slot_idx, item_def::SCTG_PARTS );

	ItemInfoList chk_parts_list;
	chk_parts_list.clear();

	for( size_t idx = 0 ; idx < parts_list.size() ; ++idx )
	{
		// ItemID > DEFAULT_PARTS_ITEM_ID_MIN 인 경우는, 기본 장비이므로 체크하지 않는다.
		if( parts_list[idx].m_TItemID > DEFAULT_PARTS_ITEM_ID_MIN ) 
			continue;

		if(INVEN::ItemID2DBType(parts_list[idx].m_TItemID) != INVEN::DB_PARTS)
			continue;

		// 값이 0인 장비는 체크 안함.
		if( item_def::NULL_ITEM_INFO.IsSame( const_cast<ITEM_INFO*>( &parts_list[idx]) ) )
			continue;

		chk_parts_list.push(parts_list[idx]);
	}

	itchk::push_attribute(out_list, chk_parts_list, chara_slot_idx);
}

void CheckerEquipments::init_inven()
{
	CInvenList* inven = CInvenList::i();

	const i3::vector<INVEN_BUFFER*>& weapon_list = inven->get_main_list( INVEN::DB_WEAPON);
	check_inven_equip_list[itchk::iWeapon].clear();
	itchk::push_attribute(check_inven_equip_list[itchk::iWeapon], weapon_list);

	const i3::vector<INVEN_BUFFER*>& chara_list = inven->get_main_list( INVEN::DB_CHARA);
	check_inven_equip_list[itchk::iChara].clear();
	itchk::push_attribute(check_inven_equip_list[itchk::iChara], chara_list);

	check_inven_equip_list[itchk::iParts].clear();
	init_inven_parts(check_inven_equip_list[itchk::cParts]);

	const i3::vector<INVEN_BUFFER*>& cash_list = inven->get_main_list( INVEN::DB_CASHITEM);
	check_inven_equip_list[itchk::iItem].clear();
	itchk::push_attribute(check_inven_equip_list[itchk::iItem], cash_list);
}

void CheckerEquipments::init_inven_parts(itchk::chk_list & out_list)
{
	// Parts
	const i3::vector<INVEN_BUFFER*>& parts_list = CInvenList::i()->get_main_list( INVEN::DB_PARTS);

	i3::vector<INVEN_BUFFER*> chk_parts_list;
	chk_parts_list.clear();

	for( size_t idx = 0 ; idx < parts_list.size() ; ++idx )
	{
		// ItemID > DEFAULT_PARTS_ITEM_ID_MIN 인 경우는, 기본 장비이므로 체크하지 않는다.
		if( parts_list[idx]->_ui32ItemID > DEFAULT_PARTS_ITEM_ID_MIN ) 
			continue;

		chk_parts_list.push_back(parts_list[idx]);
	}

	itchk::push_attribute(out_list, chk_parts_list);

	// Skin 류
	const i3::vector<INVEN_BUFFER*>& skins_list = CInvenList::i()->get_main_list(INVEN::DB_SKIN);

	i3::vector<INVEN_BUFFER*> chk_skins_list;
	chk_skins_list.clear();

	for (size_t idx = 0; idx < skins_list.size(); ++idx)
	{
		// ItemID > DEFAULT_PARTS_ITEM_ID_MIN 인 경우는, 기본 장비이므로 체크하지 않는다.
		if (skins_list[idx]->_ui32ItemID > DEFAULT_PARTS_ITEM_ID_MIN)
			continue;

		chk_skins_list.push_back(skins_list[idx]);
	}

	itchk::push_attribute(out_list, chk_skins_list);
}

#endif