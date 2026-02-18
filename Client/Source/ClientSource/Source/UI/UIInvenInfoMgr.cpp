#include "pch.h"
#include "UIInvenInfoMgr.h"

#include "UICharaInfoMgr.h"
#include "UIShopFunction.h"
#include "InvenList.h"
#include "MyRoomInfoContext.h"
#include "GameCharaEquip.h"

#include "ShopContext.h"
#include <functional>

namespace ii	//inven info
{
	void get_inven_info(ItemInfoList &, INVEN::DB_TYPE, UINT32);
	void get_inven_doin_info(ItemInfoList & list);
	UINT8 get_inven_info_Arg(ITEM_INFO info,INVEN::DB_TYPE, UINT32);
}

namespace fw	//filter weapon
{
	void filter(ItemInfoList&, const ItemInfoList&);	//SubInfoList에서 사용한다.

	void filter_primary(ItemInfoList&, const ItemInfoList&);
	void filter_assault(ItemInfoList&, const ItemInfoList&);
	void filter_smg(ItemInfoList&, const ItemInfoList&);
	void filter_sniper(ItemInfoList&, const ItemInfoList&);
	void filter_machine(ItemInfoList&, const ItemInfoList&);
	void filter_shotgun(ItemInfoList&, const ItemInfoList&);

	typedef void (*_pf_filter)(ItemInfoList&, const ItemInfoList&);
	_pf_filter pf_filter[] = {	filter_primary, 	filter_assault,	filter_smg, 	filter_sniper,	filter_machine,	filter_shotgun	};
}

namespace fc	//filter chara
{
	void filter_all(ItemInfoList&, const ItemInfoList&, UINT32);
	void filter_chara(ItemInfoList&, const ItemInfoList&, UINT32);
	void filter_type(ItemInfoList&, const ItemInfoList&, UINT32);
	void filter_chara_V10(ItemInfoList&, const ItemInfoList&, UINT32);
	void filter_chara_nomal_V10(ItemInfoList&, const ItemInfoList&, UINT32);
	void filter_type_V10(ItemInfoList&, const ItemInfoList&, UINT32);
}

namespace fp	//filter parts
{
	void filter(ItemInfoList&, const ItemInfoList&);	//SubInfoList에서 사용한다.
	//void filter_man(ItemInfoList&, const ItemInfoList&);
	//void filter_woman(ItemInfoList&, const ItemInfoList&);
	//void filter_skinny(ItemInfoList&, const ItemInfoList&);
	//void filter_g2(ItemInfoList&, const ItemInfoList&);

	//typedef void (*_pf_filter)(ItemInfoList&, const ItemInfoList&);
	//_pf_filter pf_filter[] = {	filter, 	filter_all,	filter_all, 	filter_all, filter_all, filter_all, filter_all	};
}


namespace fi	//filter item
{
	void filter_all(ItemInfoList&, const ItemInfoList&);
	void filter_enhance(ItemInfoList&, const ItemInfoList&);
	void filter_functional(ItemInfoList&, const ItemInfoList&);
	void filter_reward(ItemInfoList&, const ItemInfoList&);
	void filter_box(ItemInfoList&, const ItemInfoList&);
	void filter_set(ItemInfoList&, const ItemInfoList&);

	typedef void (*_pf_filter)(ItemInfoList&, const ItemInfoList&);
	_pf_filter pf_filter[] = {	filter_enhance, filter_functional, filter_reward,	filter_box, 	filter_set	};
}

namespace sort_f //sort function
{
	//% 용어 설명( 낮은게 앞으로 나타남 )
	//	1. 장착 상태 : 장착중 < 사용중 < 보유중 < 미 사용
	//	2. 무기 타입 : 주무기 < 보조무기 < 근접무기 < 수류탄(?) < 특수무기
	//	3. 사용 타입 : assault < smg < sniper < machine < shotgun
	//	4. 이름 : 0<1<11<8<9 <ㄱ<ㄴ~ ㅍ<ㅎ
	//	5. mark : new & hot 마크

	void status_type_name(ItemInfoList& inven_list); //weapon : 장착 상태 < 무기 타입 < 사용 타입 < 이름
	void status_name(ItemInfoList& inven_list); //chara, part : 장착 상태 < 이름
	void status_type_name_in_item(ItemInfoList& inven_list); //item : 장착 상태 < 이름
	void type_name(ItemInfoList& inven_list); //sublist weapon : 무기 타입 < 사용 타입 < 이름
	void name(ItemInfoList& inven_list); //sublist part : 이름
}

namespace locked //lock weapon remove
{
	void remove_weapon(ItemInfoList& inven_list);
}

namespace
{
	INVEN::DB_TYPE to_INVEN_DB_TYPE(item_def::Category category)
	{
		switch( category )
		{
		case item_def::CTG_WEAPON:	return INVEN::DB_WEAPON;
		case item_def::CTG_CHARA:	return INVEN::DB_CHARA;
		case item_def::CTG_PARTS:	return INVEN::DB_PARTS;
		case item_def::CTG_SKIN :	return INVEN::DB_SKIN;
		case item_def::CTG_ITEM:	return INVEN::DB_CASHITEM;
		default :	I3PRINTLOG(I3LOG_FATAL,  "You need to register the code according to the added shop classification!!!");	break;
		}
		return INVEN::DB_INVALID;
	}

	UINT32 to_inven_ref_db_type(item_def::Category category, UINT32 type)
	{
		switch(category)
		{
		case item_def::CTG_WEAPON:	return item_def::convert_weapon_type( (item_def::WeaponType)type );
		case item_def::CTG_CHARA:	return item_def::convert_chara_type( type );
		case item_def::CTG_PARTS:	return item_def::convert_parts_type( (item_def::PartsType)type );
		case item_def::CTG_SKIN :	return type;
		case item_def::CTG_ITEM:	return item_def::convert_item_type( (item_def::ItemType)type );
		default :	I3PRINTLOG(I3LOG_FATAL,  "You need to register the code according to the added shop classification!!!");	break;
		}
		return UINT_MAX;
	}
}


/*************************//*************************/
//InvenInfoList
void inven_util::InvenInfoList::list_up(item_def::Category category, UINT32 type)
{
	//인벤 클래스에서 정보를 얻고 각각에 type에 필터가 필요한 경우 필터링 한다.
	//필터링이 필요없는 경우는 인벤 정보 그대로 넘긴다.
	m_list.clear();

	INVEN::DB_TYPE db_type = to_INVEN_DB_TYPE(category);
	UINT32 ref_db_type = to_inven_ref_db_type(category, type);

	ItemInfoList list;
	ii::get_inven_info(list, db_type, ref_db_type);	//인벤 정보를 얻는다.

	//필터링
	switch(category)
	{
	case item_def::CTG_WEAPON:	
		m_list.push( list );
		break;

	case item_def::CTG_CHARA:
	{
		if (g_pEnvSet->IsV2Version() == true)
		{
			if (type == item_def::CT_ALL)
				fc::filter_all(m_list, list, 0);
			else if (type == item_def::CT_MAN
				|| type == item_def::CT_WOMAN)
				fc::filter_chara(m_list, list, type);
			else
				fc::filter_type(m_list, list, type);
		}
		else
		{
			switch (type)
			{
			case UINT_MAX: //전체 아이템
				fc::filter_all(m_list, list, 0);
				break;
			case FILTER_CHARA_TYPE_RED:	case FILTER_CHARA_TYPE_BLUE:
				fc::filter_chara_V10(m_list, list, type);
				break;
			case FILTER_CHARA_TYPE_NORMAL:
				fc::filter_chara_nomal_V10(m_list, list, type);
				break;
			default:
				fc::filter_type_V10(m_list, list, type);
				break;
			}
		}
	}
	break;
	case item_def::CTG_PARTS:
		{
			list.default_part_id_make_to_mainchara();
			m_list.push( list );
		}
		break;

	case item_def::CTG_SKIN:
		{
			list.default_skin_id_make_to_mainchara();
			m_list.push(list);
		}
		break;

	case item_def::CTG_ITEM: //아이템 카테고리에 공룡 아이템도 보인다.
		{
			if( type == item_def::CT_ALL )
				fi::filter_all(m_list, list);
			else
				fi::pf_filter[type](m_list, list);
		}
		break;
	}
}

void inven_util::InvenInfoList::list_up(item_def::WeaponComboType type)
{
	//주무기 정보를 얻고, 각각에 주무기 타입으로 필터링 한다.
	m_list.clear();

	ItemInfoList list;
	ii::get_inven_info(list, INVEN::DB_WEAPON, WEAPON_SLOT_PRIMARY);

	fw::pf_filter[type](m_list, list);
}

void inven_util::InvenInfoList::list_reset()
{
	m_list.clear();
}

UINT8 inven_util::InvenInfoList::GetInfoItemType(INT32 idx, item_def::Category category, UINT32 type)
{
	//주무기 정보를 얻고, 각각에 주무기 타입으로 필터링 한다.
	INVEN::DB_TYPE db_type = to_INVEN_DB_TYPE(category);
	UINT32 ref_db_type = to_inven_ref_db_type(category, type);

	UINT8 itemtype = ii::get_inven_info_Arg(m_list[idx], db_type, ref_db_type);

	return itemtype;
}

void inven_util::InvenInfoList::sort(item_def::Category category)
{
	//필터링
	switch(category)
	{
	case item_def::CTG_WEAPON:	
		sort_f::status_type_name(m_list);
		break;

	case item_def::CTG_CHARA:
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN :
		sort_f::status_name(m_list);
		break;

	case item_def::CTG_ITEM: //아이템 카테고리에 공룡 아이템도 보인다.
		sort_f::status_type_name_in_item(m_list);
		break;
	}
}

size_t inven_util::InvenInfoList::find_idx(const ITEM_INFO & info) const
{
	return m_list.find_idx(info);
}

size_t inven_util::InvenInfoList::find_idx(const T_ItemID & itemId) const
{
	return m_list.find_idx(itemId);
}

size_t inven_util::InvenInfoList::find_idx_by_ware_idx(const T_ItemDBIdx & ware_idx) const
{
	return m_list.find_idx_by_ware_idx(ware_idx);
}

void inven_util::InvenInfoList::SearchKeyword(const WCHAR* wstr)
{
	m_list.SearchKeyword(wstr);
}
/*************************//*************************/
//SubInfoList
void inven_util::SubInfoList::list_up(item_def::Category category, UINT32 type)
{
	m_list.clear();

	INVEN::DB_TYPE db_type = to_INVEN_DB_TYPE(category);
	UINT32 ref_db_type = to_inven_ref_db_type(category, type);

	ItemInfoList list;
	ii::get_inven_info(list, db_type, ref_db_type);

	//필터링
	switch(category)
	{
	case item_def::CTG_WEAPON:
		{
			fw::filter(m_list, list);
		}
		break;

	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
			const CHARA_INFO* info = CCharaInfoContext::i()->GetMainCharaInfo();
			
			// vv3 - check
			T_ItemID charaID = info->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

			list.remove_unshared(charaID);	//공유 할 수 없는건 제거 한다.
			
			fp::filter(m_list, list);
		}
		break;
	}
}
void inven_util::SubInfoList::list_up(item_def::SideCategory category, UINT32 type)
{
	list_up( item_def::convert(category), type);
}

void inven_util::SubInfoList::sort(item_def::Category category)
{
	//필터링
	switch(category)
	{
	case item_def::CTG_WEAPON:	
		sort_f::type_name(m_list);
		break;

	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		sort_f::name(m_list);
		break;
	}
}
void inven_util::SubInfoList::sort(item_def::SideCategory category)
{
	sort( item_def::convert(category) );
}

void inven_util::SubInfoList::remove_locked_weapon()
{
	locked::remove_weapon(m_list);
}

size_t inven_util::SubInfoList::find_idx(const ::ITEM_INFO & item)
{
	//무기일 경우엔 itemid만 비교한다.
	bool only_compare_itemid = (item_def::get_category(item.m_TItemID) == item_def::CTG_WEAPON);	

	size_t size = m_list.size();
	for(size_t i=0;i<size;++i)
	{
		if( only_compare_itemid )
		{ 
			if(m_list[i].m_TItemID == item.m_TItemID )		return i;
		}
		else
		{
			if( m_list[i] == item )	return i;
		}
	}
	return UINT_MAX;
}

void inven_util::get_dino_inven_list(ItemInfoList & list)
{
	ii::get_inven_doin_info(list);
}


namespace ii	//inven item
{
	void get_inven_info(ItemInfoList & list, INVEN::DB_TYPE db_type,  UINT32 ref_db_type)
	{
		list.clear();

		const i3::vector<INVEN_BUFFER*>& inven_db_list = CInvenList::i()->get_ref_list(db_type, ref_db_type);

		size_t size = inven_db_list.size();

		INT32 AprilFoolItemCount = 0;

		for(size_t i=0; i<size; ++i)
		{
			if	((inven_db_list[i]->_ui32ItemID == CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL_CLIENT) &&
				(GET_ITEM_TYPE(inven_db_list[i]->_ui32ItemID) == ITEM_TYPE_WRAP_PERIOD || GET_ITEM_TYPE(inven_db_list[i]->_ui32ItemID) == ITEM_TYPE_MAINTENANCE) )
					AprilFoolItemCount++;
		}

		list.resize( size - AprilFoolItemCount);

		AprilFoolItemCount = 0;

		for(size_t i=0; i<size; ++i)
		{
			if	((inven_db_list[i]->_ui32ItemID == CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL_CLIENT) &&
				(GET_ITEM_TYPE(inven_db_list[i]->_ui32ItemID) == ITEM_TYPE_WRAP_PERIOD || GET_ITEM_TYPE(inven_db_list[i]->_ui32ItemID) == ITEM_TYPE_MAINTENANCE) )
			{
				AprilFoolItemCount++;
				continue;
			}

			list[i - AprilFoolItemCount].m_TItemID = inven_db_list[i]->_ui32ItemID;
			list[i - AprilFoolItemCount].m_TItemDBIdx = inven_db_list[i]->_TItemWareDBIdx;
		}
	}

	UINT8 get_inven_info_Arg(ITEM_INFO info, INVEN::DB_TYPE db_type,  UINT32 ref_db_type)
	{
		const i3::vector<INVEN_BUFFER*>& inven_db_list = CInvenList::i()->get_ref_list(db_type, ref_db_type);

		size_t size = inven_db_list.size();
		for(size_t i=0; i<size; ++i)
		{
			if( (info.m_TItemID == inven_db_list[i]->_ui32ItemID)
				&& (info.m_TItemDBIdx == inven_db_list[i]->_TItemWareDBIdx) )
			{
				return inven_db_list[i]->_ui8ItemType;
			}
		}

		//기본 파츠는 이벤 리스트에 없음 따라서 여기서 다시 처리해서 넘김
		const CEquipInfo * pEquip = g_pCharaInfoDataBase->GetInfo(info.m_TItemID);
		if(pEquip != nullptr && pEquip->IsDefaultEquipment() )
			return (UINT8)INVEN::ItemID2DBType( info.m_TItemID );
		
		I3PRINTLOG(I3LOG_NOTICE, "HAVE NOT FOUND _ui8ItemType - with ItemID and ItemDBIdx from inven_db_list= %d, %d", info.m_TItemID, info.m_TItemDBIdx);
		return 0xFF;
	}

	void get_inven_doin_info(ItemInfoList & list)
	{
		ii::get_inven_info(list, INVEN::DB_CASHITEM, 0);

		for (size_t i=0; i<list.size(); i++)
		{
			const ITEM_INFO& info = list[i];
			if (ITEM_TYPE_DINO != GET_ITEM_TYPE(info.m_TItemID))
			{
				list.remove(i);
				i--;
			}
		}
	}
}

namespace fw	//filter weapon
{
	//인증 안 한 무기는 제외한다. (SubInfoList에서 사용한다.)
	void filter(ItemInfoList & inven_list, const ItemInfoList & list)
	{
		inven_list.clear();

		size_t size = list.size(); 
		for(size_t i=0;i<size;++i)
		{
			if( usf::GetItemStatus( list[i].m_TItemID, list[i].m_TItemDBIdx ) != ITEM_STATUS_NOTUSED)
				inven_list.push(list[i]);
		}
	}

	void filter_primary(ItemInfoList& inven_list, const ItemInfoList & list)
	{
		inven_list.clear();
		inven_list.push(list);
	}

	void filter_assault(ItemInfoList& inven_list, const ItemInfoList & list)
	{
		inven_list.clear();

		size_t size = list.size();
		for(size_t i=0;i<size;++i)
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(list[i].m_TItemID, true);
			WEAPON_CLASS_TYPE type = pInfo->GetTypeClass();
			if( type == WEAPON_CLASS_ASSAULT )
				inven_list.push(list[i]);
		}
	}

	void filter_smg(ItemInfoList& inven_list, const ItemInfoList & list)
	{
		inven_list.clear();

		size_t size = list.size();
		for(size_t i=0;i<size;++i)
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(list[i].m_TItemID, true);
			WEAPON_CLASS_TYPE type = pInfo->GetTypeClass();
			if( type == WEAPON_CLASS_SMG || type == WEAPON_CLASS_DUALSMG )
				inven_list.push(list[i]);
		}
	}

	void filter_sniper(ItemInfoList& inven_list, const ItemInfoList & list)
	{
		inven_list.clear();

		size_t size = list.size();
		for(size_t i=0;i<size;++i)
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(list[i].m_TItemID, true);
			WEAPON_CLASS_TYPE type = pInfo->GetTypeClass();
			if( type == WEAPON_CLASS_SNIPER )
				inven_list.push(list[i]);
		}
	}

	void filter_machine(ItemInfoList& inven_list, const ItemInfoList & list)
	{
		inven_list.clear();

		size_t size = list.size();
		for(size_t i=0;i<size;++i)
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(list[i].m_TItemID, true);
			WEAPON_CLASS_TYPE type = pInfo->GetTypeClass();
			if( type == WEAPON_CLASS_MG )
				inven_list.push(list[i]);
		}
	}

	void filter_shotgun(ItemInfoList& inven_list, const ItemInfoList & list)
	{
		inven_list.clear();

		size_t size = list.size();
		for(size_t i=0;i<size;++i)
		{
			CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(list[i].m_TItemID, true);
			WEAPON_CLASS_TYPE type = pInfo->GetTypeClass();
			if( type == WEAPON_CLASS_SHOTGUN )
				inven_list.push(list[i]);
		}
	}
}

namespace fc	
{
	void filter_all(ItemInfoList & inven_list, const ItemInfoList & list, UINT32 type)
	{
		inven_list.clear();

		size_t size = list.size();
		for(size_t i=0;i<size;++i)
		{
			if(	CHARA::ItemID2Class(list[i].m_TItemID) != CHARACTER_CLASS_CHARA_DINO)
				inven_list.push(list[i]);
		}
	}

	void filter_chara(ItemInfoList & inven_list, const ItemInfoList & list, UINT32 type)
	{
		inven_list.clear();

		size_t size = list.size();
		for (size_t i = 0; i < size; ++i)
		{
			T_ItemID item_id = list[i].m_TItemID;
			const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo(item_id);

			if (pInfo != nullptr)
			{
				if (pInfo->GetCharaSexTypeDISPLAY() == (INT32)type)
					inven_list.push(list[i]);
			}
		}
	}

	void filter_type(ItemInfoList & inven_list, const ItemInfoList & list, UINT32 type)
	{
		inven_list.clear();

		size_t size = list.size();
		for (size_t i = 0; i < size; ++i)
		{
			UINT32 list_type = GET_ITEM_TYPE(list[i].m_TItemID);
			if (type == list_type)
				inven_list.push(list[i]);
		}
	}

	void filter_chara_V10(ItemInfoList & inven_list, const ItemInfoList & list, UINT32 type)
	{
		inven_list.clear();

		size_t size = list.size();
		for (size_t i = 0; i < size; ++i)
		{
			T_ItemID item_id = list[i].m_TItemID;
			const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo(item_id);

			if (pInfo != nullptr && pInfo->GetHP() > 100)
			{
				CHARACTER_TEAM_TYPE team = CHARA::ItemID2Team(item_id) + 1;
				if (team == (INT32)type)
					inven_list.push(list[i]);
			}
		}
	}

	void filter_chara_nomal_V10(ItemInfoList & inven_list, const ItemInfoList & list, UINT32 type)
	{
		inven_list.clear();

		size_t size = list.size();
		for (size_t i = 0; i < size; ++i)
		{
			T_ItemID item_id = list[i].m_TItemID;
			const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo(item_id);

			if (pInfo != nullptr && pInfo->GetHP() == 100)
			{
				inven_list.push(list[i]);
			}
		}
	}

	void filter_type_V10(ItemInfoList & inven_list, const ItemInfoList & list, UINT32 type)
	{
		inven_list.clear();

		size_t size = list.size();
		for (size_t i = 0; i < size; ++i)
		{
			UINT32 list_type = GET_ITEM_TYPE(list[i].m_TItemID);
			if (type == list_type)
				inven_list.push(list[i]);
			else if (type == ITEM_TYPE_HEADGEAR && list_type == ITEM_TYPE_BERET)
				inven_list.push(list[i]);
		}
	}
}

namespace fp	
{
	//SubInfoList에서 사용한다.
	void filter(ItemInfoList & inven_list, const ItemInfoList & list)
	{
		//기본 파츠 : 한번만 넣는다.
		//중복 파츠 : 한번만 넣는다.(현재 착용중일 경우는 해당 item_info 를 넣는다.)
		//다른 캐릭 착용 파츠 : 넣지 않는다.
		//인증 안 된 파츠 : 넣지 않는다.

		// vv3 - check
		UINT32 chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
		int chara_itemid = CCharaInfoContext::i()->GetMainCharaPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		bool is_add_default_item = false;
		inven_list.clear();

		size_t size = list.size(); 
		for(size_t i=0;i<size;++i)
		{
			T_ItemID item_id = list[i].m_TItemID;

			if( item_def::is_default_parts(item_id, chara_itemid) ) //기본 파츠 판단
			{
				if( !is_add_default_item )
				{
					inven_list.push(list[i]);
					is_add_default_item = true;
				}
			}
			else
			{
				UINT32 find_chara_idx = CharaInfoMgr::instance()->find_equip_chara_idx(item_id, list[i].m_TItemDBIdx);
				if( chara_idx == find_chara_idx ) //현재 착용 유무
				{
					size_t find_idx = inven_list.find_idx(item_id);
					if( find_idx != UINT_MAX )
						inven_list[find_idx] = list[i];
					else
						inven_list.push(list[i]);
				}
				else if( find_chara_idx == UINT_MAX ) //타 캐릭 착용 유무
				{
					if( usf::GetItemStatus(item_id, list[i].m_TItemDBIdx) != ITEM_STATUS_NOTUSED) //인증 유무
					{
						if( inven_list.find_idx(item_id) == UINT_MAX )
							inven_list.push(list[i]);
					}
				}
			}
		}
	}
}

namespace fi	
{
	void filter_all(ItemInfoList & inven_list, const ItemInfoList & list)
	{
		inven_list.clear();
		inven_list.push(list);
	}

	namespace inner
	{
		void filter_type(ItemInfoList& inven_list, const ItemInfoList & list, INT32 type)
		{
			inven_list.clear();

			size_t size = list.size();
			for(size_t i=0;i<size;++i)
			{
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(list[i].m_TItemID);
				if( pInfo && pInfo->GetCatagory() == type)
					inven_list.push(list[i]);
			}
		}

		void filter_dino_and_type(ItemInfoList& inven_list, const ItemInfoList & list, INT32 type)
		{
			inven_list.clear();

			size_t size = list.size();
			for(size_t i=0;i<size;++i)
			{
				ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(list[i].m_TItemID); //type 필터
				if( pInfo && pInfo->GetCatagory() == type)
					inven_list.push(list[i]);

				CHARACTER_CLASS_TYPE Type = CHARA::ItemID2Class( list[i].m_TItemID ); //dino 필터
				if( Type == CHARACTER_CLASS_CHARA_DINO )
					inven_list.push(list[i]);
			}
		}

		void filter_set(ItemInfoList& inven_list, const ItemInfoList & list)
		{
			inven_list.clear();

			CShopContext* pShopContext = CShopContext::i();

			size_t size = list.size();
			for(size_t i=0;i<size;++i)
			{
				const SHOP_GOODS* pGoods = pShopContext->GetGoods(list[i].m_TItemID);
				if( pGoods && usf::IsSetItem(pGoods) == true )
				{
					ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo(pGoods->_GoodsID);
					if(pInfo && pInfo->GetCatagory() == item_def::IT_SET)
						inven_list.push(list[i]);
				}
			}
		}
	}


	void filter_enhance(ItemInfoList& inven_list, const ItemInfoList & list) //강화
	{
		inner::filter_type(inven_list, list, item_def::IT_ENHANCE);
	}
	void filter_functional(ItemInfoList& inven_list, const ItemInfoList & list) //기능
	{
		inner::filter_dino_and_type(inven_list, list, item_def::IT_FEATURE);
	}
	void filter_reward(ItemInfoList& inven_list, const ItemInfoList & list) //보상
	{
		inner::filter_type(inven_list, list, item_def::IT_REWARD);
	}
	void filter_box(ItemInfoList& inven_list, const ItemInfoList & list) //박스
	{
		inner::filter_type(inven_list, list, item_def::IT_BOX);
	}
	void filter_set(ItemInfoList& inven_list, const ItemInfoList & list) //세트
	{
		inner::filter_set(inven_list, list);
	}
}

#include "i3Base/string/compare/generic_iless.h"

namespace sort_f	
{
	namespace inner
	{
		struct fn_name
		{
			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				i3::rc_wstring lhs_name; usf::GetShopItemName(lhs_name, lhs);
				i3::rc_wstring rhs_name; usf::GetShopItemName(rhs_name, rhs);

				return i3::generic_iless()(lhs_name, rhs_name);
			//	return CompareString(lhs_name.c_str(), rhs_name.c_str(), true) < 0; //CompareString 함수가 좋은지 모르겠음..
			}
		};

		struct fn_status_name
		{
			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				ItemStatus lhs_status = usf::GetItemStatus( lhs.m_TItemID, lhs.m_TItemDBIdx );
				ItemStatus rhs_status = usf::GetItemStatus( rhs.m_TItemID, rhs.m_TItemDBIdx );
				if( lhs_status != rhs_status) return lhs_status < rhs_status;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_weapon_usage_name
		{
			void re_usage(WEAPON_CLASS_TYPE & usage) const
			{
				switch( usage )
				{
				case  WEAPON_CLASS_ASSAULT		:	usage = static_cast<WEAPON_CLASS_TYPE>(0); break;
				case  WEAPON_CLASS_SMG			:	
				case  WEAPON_CLASS_DUALSMG		:	usage = static_cast<WEAPON_CLASS_TYPE>(1); break;
				case  WEAPON_CLASS_SNIPER		:	usage = static_cast<WEAPON_CLASS_TYPE>(2); break;
				case  WEAPON_CLASS_MG			:	usage = static_cast<WEAPON_CLASS_TYPE>(3); break;
				case  WEAPON_CLASS_SHOTGUN		:	
				case  WEAPON_CLASS_DUALSHOTGUN	:	usage = static_cast<WEAPON_CLASS_TYPE>(4); break;
				}
			}

			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				WEAPON_CLASS_TYPE lhs_usage = item_def::get_weapon_usage(lhs.m_TItemID); re_usage(lhs_usage);
				WEAPON_CLASS_TYPE rhs_usage = item_def::get_weapon_usage(rhs.m_TItemID); re_usage(rhs_usage);
				if( lhs_usage != rhs_usage) return lhs_usage < rhs_usage;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_weapon_slot_name
		{
			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				WEAPON_SLOT_TYPE lhs_slot = item_def::get_weapon_slot_type(lhs.m_TItemID);
				WEAPON_SLOT_TYPE rhs_slot = item_def::get_weapon_slot_type(rhs.m_TItemID);

				if(lhs_slot == WEAPON_SLOT_PRIMARY && rhs_slot == WEAPON_SLOT_PRIMARY) return sort_f::inner::fn_weapon_usage_name()(lhs, rhs);
				if( lhs_slot != rhs_slot) return lhs_slot < rhs_slot;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_item_type_name
		{
			void re_type(ITEM_TYPE & type) const
			{
				if(type == ITEM_TYPE_DINO) type = (ITEM_TYPE)1;
				type = (ITEM_TYPE)0;
			}

			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				ITEM_TYPE lhs_type = (ITEM_TYPE)GET_ITEM_TYPE(lhs.m_TItemID); re_type(lhs_type);
				ITEM_TYPE rhs_type = (ITEM_TYPE)GET_ITEM_TYPE(rhs.m_TItemID); re_type(rhs_type);

				if( lhs_type != rhs_type) return lhs_type < rhs_type;

				return sort_f::inner::fn_name()(lhs, rhs);
			}
		};

		struct fn_status_type_name_in_item
		{
			void re_status(const ITEM_INFO & info, ItemStatus & status) const
			{
				if( GET_ITEM_TYPE(info.m_TItemID) == ITEM_TYPE_DINO )
				{
					if( status == ITEM_STATUS_NOHAVE) status = ITEM_STATUS_NOTUSED;
				}
			}

			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				ItemStatus lhs_status = usf::GetItemStatus( lhs.m_TItemID, lhs.m_TItemDBIdx ); re_status(lhs, lhs_status);
				ItemStatus rhs_status = usf::GetItemStatus( rhs.m_TItemID, rhs.m_TItemDBIdx ); re_status(rhs, rhs_status);
				if( lhs_status != rhs_status) return lhs_status < rhs_status;

				return sort_f::inner::fn_item_type_name()(lhs, rhs);
			}
		};

		struct fn_status_type_name
		{
			bool operator()(const ITEM_INFO & lhs, const ITEM_INFO & rhs) const
			{
				ItemStatus lhs_status = usf::GetItemStatus( lhs.m_TItemID, lhs.m_TItemDBIdx );
				ItemStatus rhs_status = usf::GetItemStatus( rhs.m_TItemID, rhs.m_TItemDBIdx );
				if( lhs_status != rhs_status) return lhs_status < rhs_status;

				return sort_f::inner::fn_weapon_slot_name()(lhs, rhs);
			}
		};
	}

	//-------------------------------////-------------------------------//
	void name(ItemInfoList& inven_list)
	{
		i3::vector< ITEM_INFO >& list = const_cast< i3::vector< ITEM_INFO >& >( inven_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_name() );
	}
	void type_name(ItemInfoList& inven_list)
	{
		i3::vector< ITEM_INFO >& list = const_cast< i3::vector< ITEM_INFO >& >( inven_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_weapon_slot_name() );
	}
	void status_name(ItemInfoList& inven_list)
	{
		i3::vector< ITEM_INFO >& list = const_cast< i3::vector< ITEM_INFO >& >( inven_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_status_name() );
	}
	void status_type_name_in_item(ItemInfoList& inven_list)
	{
		i3::vector< ITEM_INFO >& list = const_cast< i3::vector< ITEM_INFO >& >( inven_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_status_type_name_in_item() );
	}
	void status_type_name(ItemInfoList& inven_list)
	{
		i3::vector< ITEM_INFO >& list = const_cast< i3::vector< ITEM_INFO >& >( inven_list.get_list() );
		std::sort( list.begin(), list.end(), sort_f::inner::fn_status_type_name() );
	}
}

namespace locked
{
	void remove_weapon(ItemInfoList& inven_list)
	{
		struct fn : std::binary_function<ITEM_INFO, UINT8, bool>
		{
			bool operator()(const ITEM_INFO & info, UINT8 mode_flag) const
			{
				return item_def::is_locked_weapon(mode_flag, info.m_TItemID);
			}
		};

		i3::vector<ITEM_INFO>& list = const_cast< i3::vector< ITEM_INFO >& >( inven_list.get_list() );

		UINT8 mode_flag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
		i3::vector<ITEM_INFO>::iterator it = std::remove_if( list.begin(), list.end(), std::bind2nd(fn(),mode_flag) );
		list.resize(it - list.begin() );
	}
}