#include "pch.h"
#include "UICharaInfoMgr.h"

#include "UIFloatCharacterView.h"
#include "../UserInfoContext.h"

namespace
{
	const COMMON_PARTS_INFO& get_common_parts()
	{
		return *CCharaInfoContext::i()->GetCommonPartsInfo();
	}
}

//////////////////////////////////////////////////////////////////////////

CharaEquipList::CharaEquipList(void)
{
	m_chara_list.resize(S2MO_MULTI_SLOT_COUNT);
	i3mem::FillZero( m_main_chara_slot, sizeof(m_main_chara_slot) );
}

void CharaEquipList::clear(void)
{
	for( size_t idx = 0 ; idx < m_chara_list.size() ; ++idx)
	{
		m_chara_list[idx].m_item[ item_def::SCTG_WEAPON ].clear();
		m_chara_list[idx].m_item[ item_def::SCTG_PARTS ].clear();
	}

	m_share_list.m_item[ item_def::SCTG_WEAPON ].clear();
	m_share_list.m_item[ item_def::SCTG_PARTS ].clear();

	i3mem::FillZero( m_main_chara_slot, sizeof(m_main_chara_slot) );
}

void CharaEquipList::clear(UINT32 chara_idx)
{
	m_chara_list[chara_idx].m_item[ item_def::SCTG_WEAPON ].clear();
	m_chara_list[chara_idx].m_item[ item_def::SCTG_PARTS ].clear();

	m_share_list.m_item[ item_def::SCTG_WEAPON ].clear();
	m_share_list.m_item[ item_def::SCTG_PARTS ].clear();

	i3mem::FillZero( m_main_chara_slot, sizeof(m_main_chara_slot) );
}

void CharaEquipList::resize(void)
{
	clear();

	for( size_t idx = 0 ; idx < m_chara_list.size() ; ++idx)
	{
		m_chara_list[idx].m_item[item_def::SCTG_WEAPON ].resize( item_def::WT_MAX );
		m_chara_list[idx].m_item[item_def::SCTG_PARTS ].resize( item_def::PT_MAX );
	}

	m_share_list.m_item[item_def::SCTG_PARTS ].resize(1);
}

void CharaEquipList::resize(UINT32 chara_idx)
{
	clear( chara_idx );

	m_chara_list[chara_idx].m_item[ item_def::SCTG_WEAPON ].resize( item_def::WT_MAX );
	m_chara_list[chara_idx].m_item[ item_def::SCTG_PARTS ].resize( item_def::PT_MAX );

	m_share_list.m_item[item_def::SCTG_PARTS ].resize(1);
}

//////////////////////////////////////////////////////////////////////////

UserEquipInfo::UserEquipInfo(void)
{
	reset();
}

UserEquipInfo::~UserEquipInfo(void)
{
	m_red_parts.clear();	
	m_blue_parts.clear();	
	m_red_parts.clear();	
}

void UserEquipInfo::reset(void)
{
	m_red_parts.clear();	m_red_parts.resize( item_def::PT_MAX );
	m_blue_parts.clear();	m_blue_parts.resize( item_def::PT_MAX );
	m_red_parts.clear();	m_weapon.resize( item_def::WT_MAX );

	i3mem::FillZero(m_common, sizeof(m_common));
}

void UserEquipInfo::set_user_equip_info( CharaEquipList & data, UINT32 red_char_idx, UINT32 blue_char_idx)
{
	if( CCharaInfoContext::i()->GetHasCharacterCount() == 0)
		return;

	m_red_parts		= data.m_chara_list[red_char_idx][item_def::SCTG_PARTS];
	m_blue_parts	= data.m_chara_list[blue_char_idx][item_def::SCTG_PARTS];
	m_weapon		= data.m_chara_list[red_char_idx][item_def::SCTG_WEAPON];

	{
		const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx( red_char_idx );
		const ITEM_INFO * chara_info = parts_info->GetPartsInfo(CHAR_EQUIPMENT_PARTS_CHARA);
		const_cast<ITEM_INFO*>(chara_info)->CopyTo(&m_common[UserEquipInfo::SUE_CHARA_RED]);
	}

	{
		const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx( blue_char_idx );
		const ITEM_INFO * chara_info = parts_info->GetPartsInfo(CHAR_EQUIPMENT_PARTS_CHARA);
		const_cast<ITEM_INFO*>(chara_info)->CopyTo(&m_common[UserEquipInfo::SUE_CHARA_BLUE]);
	}

	m_common[UserEquipInfo::SUE_HELMET]		= data.m_chara_list[red_char_idx][item_def::SCTG_PARTS][item_def::PT_HEADGEAR];
	m_common[UserEquipInfo::SUE_BERET]		= data.m_chara_list[red_char_idx][item_def::SCTG_PARTS][item_def::PT_BERET];
	m_common[UserEquipInfo::SUE_DINO]		= data.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO];


	// 기본 장비일 경우 Red 캐릭터 기준으로 디폴트 ItemID로 재설정해줍니다.
	if( m_common[UserEquipInfo::SUE_HELMET].m_TItemID > DEFAULT_PARTS_ITEM_ID_MIN )
	{
		T_ItemID charaID = CCharaInfoContext::i()->GetMainCharaInfo()->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
		const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo(charaID);

		ITEM_TYPE part_type = (ITEM_TYPE)GET_ITEM_TYPE(m_common[UserEquipInfo::SUE_HELMET].m_TItemID);

		m_common[UserEquipInfo::SUE_HELMET].m_TItemID = pCharaInfo->GetPartsItemID(EQUIP::ItemType2PartsType(part_type));
		m_common[UserEquipInfo::SUE_HELMET].m_TItemDBIdx = MAKE_DEFAULT_EQUIP_DBIDX(part_type);
	}
	else if( m_common[UserEquipInfo::SUE_HELMET].m_TItemID == 0 )
	{
		// 마스크 착용의 경우, 머리 부위에 마스크 Item정보를 넣어줍니다.
		m_common[UserEquipInfo::SUE_HELMET] = data.m_chara_list[red_char_idx][item_def::SCTG_PARTS][item_def::PT_FACEGEAR];
	}
}

bool UserEquipInfo::is_equip_part(const T_ItemID & itemId)
{
	if( is_parts_find_idx_team(itemId, TEAM_RED) != UINT_MAX )	return true;
	if( is_parts_find_idx_team(itemId, TEAM_BLUE) != UINT_MAX )	return true;
	if( is_common_find_idx(itemId) != UINT_MAX ) return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////

CharaInfoMgr* CharaInfoMgr::instance()
{
	static CharaInfoMgr g_instance;
	return &g_instance;
}

/************************//************************/
CharaInfoMgr::CharaInfoMgr()
{
	m_current_state.ctg = item_def::SCTG_NA;
	m_current_state.chara_idx = 0xFF;
	m_current_state.slot_idx = 0xFF;
	m_current_state.info = item_def::NULL_ITEM_INFO;
	m_current_state.modify_flag = 0;

	isAuthAndStateCharaChange = false;
}

CharaInfoMgr::~CharaInfoMgr()
{
	clear();
}

void CharaInfoMgr::clear()
{
	m_equip_list.clear();
	m_equip_backup_list.clear();
}

void CharaInfoMgr::resize()
{
	m_equip_list.resize();
	m_equip_backup_list.resize();
}

void CharaInfoMgr::resize(UINT32 chara_idx)
{
	m_equip_list.resize(chara_idx);
	m_equip_backup_list.resize(chara_idx);
}


/************************//************************/
void CharaInfoMgr::list_up()
{
	resize();

	for( UINT8 idx = 0; idx < S2MO_MULTI_SLOT_COUNT; ++idx)
	{
		ItemInfoList & weapon_list = m_equip_list.m_chara_list[idx][item_def::SCTG_WEAPON];
		ItemInfoList & parts_list = m_equip_list.m_chara_list[idx][item_def::SCTG_PARTS];

		const CHARA_WEAPON_INFO * weapon_info = CCharaInfoContext::i()->GetCharaWeaponInfoByIdx( idx);
		if( weapon_info != nullptr)
		{
			// vv3 - check 
			for( INT32 k = 0 ; k < CHAR_EQUIPMENT_WEAPON_COUNT ; ++k )
			{
				weapon_list[k] = weapon_info->m_Info[k];
			}
		}

		const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx( idx);
		if( parts_info != nullptr)
		{
			// vv3 - check
			for( INT32 k = 0 ; k < CHAR_EQUIPMENT_PARTS_COUNT ; ++k )
			{
				if( k == CHAR_EQUIPMENT_PARTS_CHARA)	continue;
				item_def::PartsType type = static_cast<item_def::PartsType>(k - CHAR_EQUIPMENT_PARTS_HEAD );
				parts_list[type] = parts_info->m_Info[k];
			}
		}

		m_equip_backup_list.m_chara_list[idx][item_def::SCTG_WEAPON] = weapon_list;
		m_equip_backup_list.m_chara_list[idx][item_def::SCTG_PARTS] = parts_list;
	}

	// 캐릭터의 메인 슬롯 동기화
	for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
	{
		m_equip_list.m_main_chara_slot[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx) );
		m_equip_backup_list.m_main_chara_slot[idx] = m_equip_list.m_main_chara_slot[idx];
	}

	const COMMON_PARTS_INFO& dino_part = get_common_parts();
	m_equip_list.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO] = dino_part.m_Info[CHAR_EQUIPMENT_COMMON_DINO];
	m_equip_backup_list.m_share_list[item_def::SCTG_PARTS] = m_equip_list.m_share_list[item_def::SCTG_PARTS];

	build_current_equip_info();
}

void CharaInfoMgr::list_up(UINT32 chara_idx)
{
	resize(chara_idx);

	{
		ItemInfoList & weapon_list = m_equip_list.m_chara_list[chara_idx][item_def::SCTG_WEAPON];
		ItemInfoList & parts_list = m_equip_list.m_chara_list[chara_idx][item_def::SCTG_PARTS];

		const CHARA_WEAPON_INFO * weapon_info = CCharaInfoContext::i()->GetCharaWeaponInfoByIdx( chara_idx );	
		if( weapon_info != nullptr)
		{
			// vv3 - check
			for( INT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i )
			{
				weapon_list[i] = weapon_info->m_Info[i]; 
			}
		}

		const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx( chara_idx );		
		if( parts_info != nullptr)
		{
			for( INT32 idx = 0 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx )
			{
				if( idx == CHAR_EQUIPMENT_PARTS_CHARA)	continue;
				item_def::PartsType type = static_cast<item_def::PartsType>(idx - CHAR_EQUIPMENT_PARTS_HEAD );
				parts_list[type] = parts_info->m_Info[idx];
			}
		}

		m_equip_backup_list.m_chara_list[chara_idx][item_def::SCTG_WEAPON] = weapon_list;
		m_equip_backup_list.m_chara_list[chara_idx][item_def::SCTG_PARTS] = parts_list;
	}

	// 캐릭터의 메인 슬롯 인덱스 동기화
	for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
	{
		m_equip_list.m_main_chara_slot[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx) );
		m_equip_backup_list.m_main_chara_slot[idx] = m_equip_list.m_main_chara_slot[idx];
	}

	const COMMON_PARTS_INFO& dino_part = get_common_parts();
	m_equip_list.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO] = dino_part.m_Info[CHAR_EQUIPMENT_COMMON_DINO];
	m_equip_backup_list.m_share_list[item_def::SCTG_PARTS] = m_equip_list.m_share_list[item_def::SCTG_PARTS];

	build_current_equip_info();
}

//************************//
const ITEM_INFO & CharaInfoMgr::find( UINT32 chara_idx, item_def::SideCategory category, UINT32 slot_idx) const
{
	if( chara_idx >= S2MO_MULTI_SLOT_COUNT ) return item_def::NULL_ITEM_INFO;
	if( m_equip_list.m_chara_list[ chara_idx ][ category ].size() == 0 ) return item_def::NULL_ITEM_INFO;

	return m_equip_list.m_chara_list[ chara_idx ][ category ][ slot_idx ];
}

const ITEM_INFO & CharaInfoMgr::find(item_def::SideCategory category, UINT32 slot_idx) const		//common
{
	return m_equip_list.m_share_list[ category ][ slot_idx ];
}

//************************//
size_t CharaInfoMgr::size(UINT32 chara_idx, item_def::SideCategory type) const
{
	return m_equip_list.m_chara_list[ chara_idx ][ type ].size();
}

UINT32 CharaInfoMgr::find_equip_chara_idx(const T_ItemID & itemId) const
{
	item_def::SideCategory category = item_def::get_side_category(itemId);
	if( category == item_def::SCTG_NA ) return UINT_MAX;

	UINT32 size = (UINT32)CCharaInfoContext::i()->GetHasCharacterCount();
	for(UINT32 i = 0; i<size; ++i)
	{
		const CHARA_INFO * pCharaInfo = CCharaInfoContext::i()->GetCharaInfoByList( i);
		UINT32 chara_idx = pCharaInfo->m_CharaBasicInfo.m_ui8CharaSlotIdx;

		if( m_equip_list.m_chara_list[chara_idx][category].find_idx(itemId) != UINT_MAX ) return chara_idx;
	}

	if( m_equip_list.m_share_list[category].find_idx(itemId) != UINT_MAX ) return 0;

	return UINT_MAX;
}

UINT32 CharaInfoMgr::find_equip_chara_idx(const T_ItemID & itemId, const T_ItemDBIdx & ware_idx) const
{
	item_def::SideCategory category = item_def::get_side_category(itemId);
	if( category == item_def::SCTG_NA ) return UINT_MAX;

	UINT32 size = (UINT32)CCharaInfoContext::i()->GetHasCharacterCount();
	for(UINT32 i = 0; i<size; ++i)
	{
		const CHARA_INFO * pCharaInfo = CCharaInfoContext::i()->GetCharaInfoByList( i);
		UINT32 chara_idx = pCharaInfo->m_CharaBasicInfo.m_ui8CharaSlotIdx;

		if( m_equip_list.m_chara_list[chara_idx][category].find_idx(itemId, ware_idx) != UINT_MAX ) 
			return chara_idx;
	}

	if( m_equip_list.m_share_list[category].find_idx(itemId, ware_idx) != UINT_MAX ) return 0;

	return UINT_MAX;
}


bool CharaInfoMgr::is_chara_equipped(const T_ItemID & itemId, UINT32 chara_idx) const
{
	item_def::SideCategory category = item_def::get_side_category(itemId);
	if( category == item_def::SCTG_NA ) return false;

	if( m_equip_list.m_chara_list[chara_idx][category].find_idx(itemId) != UINT_MAX )	return true;
	if( m_equip_list.m_share_list[category].find_idx(itemId) != UINT_MAX )				return true;

	return false;
}

bool CharaInfoMgr::is_chara_equipped(const T_ItemID & itemId, const T_ItemDBIdx & ware_idx, UINT32 chara_idx) const
{
	item_def::SideCategory category = item_def::get_side_category(itemId);
	if( category == item_def::SCTG_NA ) return false;
	
	if( m_equip_list.m_chara_list[chara_idx][category].find_idx(itemId, ware_idx) != UINT_MAX ) return true;
	if( m_equip_list.m_share_list[category].find_idx(itemId, ware_idx) != UINT_MAX ) return true;

	return false;
}

bool CharaInfoMgr::is_current_equip(const T_ItemID & itemID )
{
	return m_current_equip_info.is_equip_part(itemID);
}

bool CharaInfoMgr::replace_default_normal(const ITEM_INFO & item_info)
{
	bool ret = false;

	ItemInfoConvert convert(item_info);

	item_def::SideCategory ctg = convert.get_side_category();
	if(ctg == item_def::SCTG_NA) return ret;
	
	UINT32 idx = convert.get_char_slot_idx();

	// Assert로 묶으면 캐릭터 삭제 시 무조건 -1이 넘어온다.
	if( idx == UINT32(-1) )
		return false;

	for( INT32 i = 0; i < CCharaInfoContext::i()->GetHasCharacterCount(); ++i)
	{
		const CHARA_INFO * pCharaInfo = CCharaInfoContext::i()->GetCharaInfoByList( i);
		UINT32 chara_idx = pCharaInfo->m_CharaBasicInfo.m_ui8CharaSlotIdx;

		ITEM_INFO & info = m_equip_list.m_chara_list[ chara_idx ][ ctg ][ idx ];
		if( info == item_info )
		{
			switch( ctg )
			{
			case item_def::SCTG_WEAPON:	
				info = item_def::get_default_item(idx, item_def::get_weapon_class_type(convert.m_info.m_TItemID) );	
				break;
			case item_def::SCTG_PARTS:	
				{
					const CHARA_INFO * chara_info = CCharaInfoContext::i()->GetCharaInfo(i);
					
					// vv3 - check
					const CHARA_PARTS_INFO & parts_info = chara_info->m_CharaEquipInfo.m_CharPartsInfo;
					::T_ItemID chara_item_id = parts_info.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
					
					CHARACTER_CLASS_TYPE class_type = item_def::convert_parts_type( static_cast<item_def::PartsType>(idx));
					info = item_def::get_default_item(chara_item_id, EQUIP::ItemType2PartsType(class_type) );
				}
				break;
			}

			ret = true;
		}
	}

	return ret;
}

bool CharaInfoMgr::replace_default_dino(const T_ItemID & itemId)
{
	ItemInfoConvert convert(itemId);

	bool ret = false;

	item_def::SideCategory ctg = convert.get_side_category();
	I3_BOUNDCHK_RETURN(ctg, item_def::CTG_MAX, false);
	UINT32 idx = convert.get_char_slot_idx();

	if (idx == UINT32(-1))
		return false;

	::ITEM_INFO & info = m_equip_list.m_share_list[ ctg ][ idx ];
	if( info.m_TItemID == itemId  )
	{
		info = item_def::get_default_dino();

		ret = true;
	}

	return ret;
}

bool CharaInfoMgr::replace_default(const ITEM_INFO & item_info)
{
	bool ret = false;

	if( item_def::get_type(item_info.m_TItemID) == ITEM_TYPE_DINO )
		ret = replace_default_dino(item_info.m_TItemID);
	else
		ret = replace_default_normal(item_info);

	build_current_equip_info();

	return ret;
}

T_ItemDBIdx CharaInfoMgr::get_equip_ware_idx(const T_ItemID & itemId, UINT32 chara_idx) const
{
	item_def::SideCategory category = item_def::get_side_category(itemId);
	if( category == item_def::SCTG_NA ) return INVALID_WAREDB_INDEX;

	size_t idx = m_equip_list.m_chara_list[chara_idx][category].find_idx(itemId);
	if( idx != UINT_MAX )
	{
		const ITEM_INFO & item_info = m_equip_list.m_chara_list[chara_idx][category][idx];
		return item_info.m_TItemDBIdx;
	}

	idx = m_equip_list.m_share_list[category].find_idx(itemId);
	if( idx != UINT_MAX )
	{
		const ITEM_INFO & item_info = m_equip_list.m_share_list[chara_idx][idx];
		return item_info.m_TItemDBIdx;
	}

	return INVALID_WAREDB_INDEX;
}

void CharaInfoMgr::change_equip_item(const ITEM_INFO & item_info)
{
	item_def::SideCategory sctg = item_def::get_side_category(item_info.m_TItemID);
	if(item_def::SCTG_NA == sctg) 
	{
		I3PRINTLOG(I3LOG_NOTICE, "An attempt to equip an invalid ID happened.");
		return;
	}

	UINT32 slot_idx = item_def::get_char_slot_idx(item_info.m_TItemID);
	I3ASSERT_RETURN(slot_idx != UINT32(-1));

	UINT8 chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

	m_equip_list.m_chara_list[ chara_idx ][ sctg ][ slot_idx ] = item_info;
	adjust_equip_with_version(item_info.m_TItemID);

	build_current_equip_info();
}

void CharaInfoMgr::change_equip_item(const ItemInfoConvert & item_info_convert, const ITEM_INFO & item_info)
{
	item_def::SideCategory sideCat = item_info_convert.get_side_category();
	I3ASSERT_RETURN(sideCat != item_def::SCTG_NA);
	m_current_state.ctg = sideCat;
	
	m_current_state.info = item_info;

	UINT32 u32SlotIdx =	item_info_convert.get_char_slot_idx();
	I3ASSERT_RETURN(u32SlotIdx < 255);
	m_current_state.slot_idx = static_cast<UINT8>(u32SlotIdx);
	m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();

	//현재 상태에 따라서 ITEM_INFO를 구한다.
	ITEM_INFO  *chara_item_info = 0;
	bool dino_item = item_def::get_type(m_current_state.info.m_TItemID) == ITEM_TYPE_DINO;
	if( dino_item == true ) //dino는 공통(common 아이템)이므로 함수가 다르다.
		chara_item_info = &m_equip_list.m_share_list[ m_current_state.ctg ][ m_current_state.slot_idx ];
	else
		chara_item_info = &m_equip_list.m_chara_list[ m_current_state.chara_idx ][ m_current_state.ctg ][ m_current_state.slot_idx ];	

	if(*chara_item_info == m_current_state.info ) //현재 장착 중인 ITEM_INFO과 변경할 ITEM_INFO이 같을 경우 기본 ITEM_INFO으로 변경한다.
	{
		switch( m_current_state.ctg )
		{
		case item_def::SCTG_WEAPON:
			m_current_state.info = item_def::get_default_item(m_current_state.info.m_TItemID);
			break;
		case item_def::SCTG_PARTS:	
			{
				const CHARA_INFO& chara_info = *CCharaInfoContext::i()->GetCharaInfoBySlotIdx( m_current_state.chara_idx);

				const CHARA_PARTS_INFO& parts_info = chara_info.m_CharaEquipInfo.m_CharPartsInfo;
				// vv3 - check
				::T_ItemID chara_item_id = parts_info.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

				if( dino_item )
				{
					m_current_state.info = item_def::get_default_dino();
				}
				else
				{
					CHARACTER_CLASS_TYPE class_type = item_def::convert_parts_type( static_cast<item_def::PartsType>(m_current_state.slot_idx));
					m_current_state.info = item_def::get_default_item(chara_item_id, EQUIP::ItemType2PartsType(class_type) );
				}
			}
			break;
		}
	}

	*chara_item_info = m_current_state.info;

	if( dino_item == false )
		adjust_equip_with_version(chara_item_info->m_TItemID);

	build_current_equip_info();
}

void CharaInfoMgr::adjust_equip_with_version(T_ItemID item_id)
{
	// V2 버전은 처리하지 않습니다
	if( g_pEnvSet->IsV2Version() )
		return;

	if( item_def::get_side_category(item_id ) != item_def::SCTG_PARTS )
		return;

	for( INT32 team = 0 ; team < TEAM_COUNT ; ++team )
	{
		UINT8 chara_slot_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx(static_cast<TEAM_TYPE>( team) );

		const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx( chara_slot_idx );
		T_ItemID chara_item_id = parts_info->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		ItemInfoList & parts_list = m_equip_list.m_chara_list[ chara_slot_idx ][ item_def::SCTG_PARTS ];

		CHARA_PARTS_INFO parts;
		const CHARA_PARTS_INFO * pMyParts = UserInfoContext::i()->GetMyUICharaInfo()->getCharaParts(team);
		pMyParts->CopyTo( &parts);

		switch( EQUIP::ItemID2PartsType(item_id) )
		{
		case EQUIP::ePART_HEADGEAR :
			{
				// face gear = default
				ITEM_INFO default_item_info = item_def::get_default_item( chara_item_id, EQUIP::ePART_FACEGEAR );
				parts_list[item_def::PT_FACEGEAR] = default_item_info;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_FACE, default_item_info);

				// head gear subtype == 0 ? beret = nullptr
				T_ItemID head_equip_id = item_id;
				if( head_equip_id > DEFAULT_PARTS_ITEM_ID_MIN )
				{
					const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo(chara_item_id);
					head_equip_id = pCharaDBInfo->GetPartsItemID(EQUIP::ePART_HEADGEAR);
				}

				if( GET_ITEM_SUBTYPE( head_equip_id ) == 0 )
				{
					parts_list[item_def::PT_BERET] = item_def::NULL_ITEM_INFO;
					parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_BERET, item_def::NULL_ITEM_INFO);
				}

				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( &parts, team);
			}
			break;
		case EQUIP::ePART_FACEGEAR :
			{
				// head gear = nullptr
				ITEM_INFO head_item_info = item_def::NULL_ITEM_INFO; 

				// face gear == default ? head gear = default
				if( item_id > DEFAULT_PARTS_ITEM_ID_MIN )
					head_item_info = item_def::get_default_item( chara_item_id, EQUIP::ePART_HEADGEAR );

				parts_list[item_def::PT_HEADGEAR] = head_item_info;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_HEAD, head_item_info);

				// beret = nullptr
				parts_list[item_def::PT_BERET] = item_def::NULL_ITEM_INFO;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_BERET, item_def::NULL_ITEM_INFO);

				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( &parts, team);
			}
			break;
		case EQUIP::ePART_BERET :
			{
				// face gear != default
				if( parts_list[item_def::PT_FACEGEAR].m_TItemID < DEFAULT_PARTS_ITEM_ID_MIN )
				{
					// face gear = default
					ITEM_INFO default_face_info = item_def::get_default_item( chara_item_id, EQUIP::ePART_FACEGEAR );
					parts_list[item_def::PT_FACEGEAR] = default_face_info;
					parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_FACE, default_face_info);

					// head gear = default
					ITEM_INFO default_head_info = item_def::get_default_item( chara_item_id, EQUIP::ePART_HEADGEAR );
					parts_list[item_def::PT_HEADGEAR] = default_head_info;
					parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_HEAD, default_head_info);
				}


				// head gear subtype == 0 ? head gear = default 
				T_ItemID head_equip_id = parts_list[item_def::PT_HEADGEAR].m_TItemID;

				if( head_equip_id > DEFAULT_PARTS_ITEM_ID_MIN )
				{
					const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo(chara_item_id);
					head_equip_id = pCharaDBInfo->GetPartsItemID(EQUIP::ePART_HEADGEAR);
				}

				if( GET_ITEM_SUBTYPE( head_equip_id ) == 0 )
				{
					ITEM_INFO default_head_info = item_def::get_default_item( chara_item_id, EQUIP::ePART_HEADGEAR );
					parts_list[item_def::PT_HEADGEAR] = default_head_info;
					parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_HEAD, default_head_info);
				}

				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( &parts, team);
			}
			break;
		}
	}

}

void CharaInfoMgr::adjust_equip_with_version_UIChara(T_ItemID item_id)
{
	// V2 버전은 처리하지 않습니다
	if (g_pEnvSet->IsV2Version())
		return;

	if (item_def::get_side_category(item_id) != item_def::SCTG_PARTS)
		return;

	for (INT32 team = 0; team < TEAM_COUNT; ++team)
	{
		UINT8 chara_slot_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx(static_cast<TEAM_TYPE>(team));

		const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx(chara_slot_idx);
		T_ItemID chara_item_id = parts_info->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

		ItemInfoList & parts_list = m_equip_list.m_chara_list[chara_slot_idx][item_def::SCTG_PARTS];

		CHARA_PARTS_INFO parts;
		const CHARA_PARTS_INFO * pMyParts = UserInfoContext::i()->GetMyUICharaInfo()->getCharaParts(team);
		pMyParts->CopyTo(&parts);

		switch (EQUIP::ItemID2PartsType(item_id))
		{
		case EQUIP::ePART_HEADGEAR:
		{
			// face gear = default
			ITEM_INFO default_item_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_FACEGEAR);
			//parts_list[item_def::PT_FACEGEAR] = default_item_info;
			parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_FACE, default_item_info);

			// head gear subtype == 0 ? beret = nullptr
			T_ItemID head_equip_id = item_id;
			if (head_equip_id > DEFAULT_PARTS_ITEM_ID_MIN)
			{
				const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo(chara_item_id);
				head_equip_id = pCharaDBInfo->GetPartsItemID(EQUIP::ePART_HEADGEAR);
			}

			if (GET_ITEM_SUBTYPE(head_equip_id) == 0)
			{
				//parts_list[item_def::PT_BERET] = item_def::NULL_ITEM_INFO;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_BERET, item_def::NULL_ITEM_INFO);
			}

			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&parts, team);
		}
		break;
		case EQUIP::ePART_FACEGEAR:
		{
			// head gear = nullptr
			ITEM_INFO head_item_info = item_def::NULL_ITEM_INFO;

			// face gear == default ? head gear = default
			if (item_id > DEFAULT_PARTS_ITEM_ID_MIN)
				head_item_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_HEADGEAR);

			//parts_list[item_def::PT_HEADGEAR] = head_item_info;
			parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_HEAD, head_item_info);

			// beret = nullptr
			//parts_list[item_def::PT_BERET] = item_def::NULL_ITEM_INFO;
			parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_BERET, item_def::NULL_ITEM_INFO);

			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&parts, team);
		}
		break;
		case EQUIP::ePART_BERET:
		{
			// face gear != default
			if (parts_list[item_def::PT_FACEGEAR].m_TItemID < DEFAULT_PARTS_ITEM_ID_MIN)
			{
				// face gear = default
				ITEM_INFO default_face_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_FACEGEAR);
				//parts_list[item_def::PT_FACEGEAR] = default_face_info;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_FACE, default_face_info);

				// head gear = default
				ITEM_INFO default_head_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_HEADGEAR);
				//parts_list[item_def::PT_HEADGEAR] = default_head_info;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_HEAD, default_head_info);
			}


			// head gear subtype == 0 ? head gear = default 
			T_ItemID head_equip_id = parts_list[item_def::PT_HEADGEAR].m_TItemID;

			if (head_equip_id > DEFAULT_PARTS_ITEM_ID_MIN)
			{
				const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo(chara_item_id);
				head_equip_id = pCharaDBInfo->GetPartsItemID(EQUIP::ePART_HEADGEAR);
			}

			if (GET_ITEM_SUBTYPE(head_equip_id) == 0)
			{
				ITEM_INFO default_head_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_HEADGEAR);
				//parts_list[item_def::PT_HEADGEAR] = default_head_info;
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_HEAD, default_head_info);
			}

			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&parts, team);
		}
		break;
		}
	}

}

bool CharaInfoMgr::is_changed_equip_item(UINT32 chara_idx, item_def::SideCategory category)
{
	size_t size = m_equip_list.m_chara_list[chara_idx][category].size();
	for(size_t i=0;i<size;++i)
	{
		if( m_equip_list.m_chara_list[chara_idx][category][i] != m_equip_backup_list.m_chara_list[chara_idx][category][i] )
			return true;
	}
	return false;
}

void CharaInfoMgr::build_current_equip_info(void)
{
	UINT32 current_chara_idx	= CCharaInfoContext::i()->GetMainCharaSlotIdx();

	// 1.0은 전 캐릭터가 무기 / 헤드 & 베레모 파츠가 동일합니다.
	if( !g_pEnvSet->IsV2Version() )
	{
		EquipInfoList & main_chara_list = m_equip_list.m_chara_list[current_chara_idx];

		for( INT32 idx = 0 ; idx < CCharaInfoContext::i()->GetHasCharacterCount() ; ++idx)
		{
			if( idx == static_cast<INT32>(current_chara_idx) )
				continue;

			const CHARA_INFO * pCharaInfo = CCharaInfoContext::i()->GetCharaInfoByList( idx);
	
			UINT8 slot_idx = pCharaInfo->m_CharaBasicInfo.m_ui8CharaSlotIdx;

			EquipInfoList & chara_list = m_equip_list.m_chara_list[slot_idx];

			// weapon
			for( INT32 count = 0 ; count < item_def::WT_MAX; ++ count)
			{
				chara_list[item_def::SCTG_WEAPON][count] = main_chara_list[item_def::SCTG_WEAPON][count];
			}

			// parts
			for( INT32 count = 0 ; count < item_def::PT_MAX; ++ count)
			{			
				chara_list[item_def::SCTG_PARTS][count] = main_chara_list[item_def::SCTG_PARTS][count];
			}
		}
	}

	UINT32 red_chara_idx		= CCharaInfoContext::i()->GetMainCharaSlotIdx(TEAM_RED);
	UINT32 blue_chara_idx		= CCharaInfoContext::i()->GetMainCharaSlotIdx(TEAM_BLUE);	
	m_current_equip_info.set_user_equip_info(m_equip_list, red_chara_idx, blue_chara_idx);
}

//************************////************************//
//변경된 equip를 분석하고, 만약 변경이 된 경우 report에 flag 값을 기록한다.
bool CharaInfoMgr::check_changed_equip()
{
	m_report_private_list.clear();

	m_report_common.flag = 0;
	m_report_common.count = 0;

	//common parts
	const ItemInfoList& dino_list = m_equip_list.m_share_list[item_def::SCTG_PARTS];
	const ItemInfoList& backup_dino_list = m_equip_backup_list.m_share_list[item_def::SCTG_PARTS];
	if ( dino_list[item_def::PT_DINO] != backup_dino_list[item_def::PT_DINO] )
	{
		m_report_common.flag |= INVENTORY_FLAG_COMMON_PARTS;
	}

	///개인 무기/Parts는 캐릭터 별로 검사한다.
	for( INT32 i = 0; i < CCharaInfoContext::i()->GetHasCharacterCount(); ++i)
	{ 
		Private result;
		result.flag = 0;

		const CHARA_INFO * pCharaInfo = CCharaInfoContext::i()->GetCharaInfoByList( i);

		UINT8 idx = pCharaInfo->m_CharaBasicInfo.m_ui8CharaSlotIdx;
		
		// 캐릭터 고유 슬롯 번호로 설정되야 한다.
		//private weapon
		const ItemInfoList& weapon_list = m_equip_list.m_chara_list[idx][item_def::SCTG_WEAPON];
		const ItemInfoList& backup_weapon_list = m_equip_backup_list.m_chara_list[idx][item_def::SCTG_WEAPON];

		if( weapon_list.size() > 0 && backup_weapon_list.size() > 0)
		{
			for(int j = item_def::WT_PRIMARY; j<item_def::WT_MAX;++j )
			{
				if( backup_weapon_list[j] != weapon_list[j] )			
				{
					result.flag |= INVENTORY_FLAG_WEAPON;
					m_current_state.modify_flag |= CurrentState::MODIFY_WEAPON;
				}
			}
		}

		//private parts
		const ItemInfoList& parts_list = m_equip_list.m_chara_list[idx][item_def::SCTG_PARTS];
		const ItemInfoList& backup_parts_list = m_equip_backup_list.m_chara_list[idx][item_def::SCTG_PARTS];
		if( parts_list.size() > 0 && backup_parts_list.size() > 0)
		{
			for( INT32 j = item_def::PT_HEADGEAR; j<item_def::PT_MAX;++j )
			{
				if( backup_parts_list[j] != parts_list[j] )			
				{
					result.flag |= INVENTORY_FLAG_PARTS;
					m_current_state.modify_flag |= CurrentState::MODIFY_PARTS;
				}
			}
		}

		if( result.flag == 0 ) continue;

		result.list_idx = static_cast<UINT8>(i);

		result.chara_idx = idx;

		m_report_private_list.push_back( result );
	}

	m_report_common.count += m_report_private_list.size();

	//공통 weapon/parts에 대한 변경이 없고, 개인 weapon/parts에 대한 변경이 없다.
	if( m_report_common.flag == 0 && m_report_common.count == 0  )
		return false;

	//개인 weapon/parts에 대한 변경이 없지만, 공통 weapon/parts 에 대한 변경이 있다.
	//if( m_report_common.count == 0 && m_report_common.flag != 0)
	//{
	//	//생각해 볼 부분
	//	Private result;
	//	result.flag = INVENTORY_FLAG_COMMON_PARTS;
	//	result.list_idx = result.chara_idx = 0;

	//	m_report_private_list.push_back( result );
	//	m_report_common.count = 1;
	//}

	return true;
}


//************************////************************//
//변경된 equip를 분석하고, 만약 변경이 된 경우 report에 flag 값을 기록한다.
bool CharaInfoMgr::check_changed_main_chara_slot()
{
	for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx )
	{
		m_equip_list.m_main_chara_slot[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx) );
		if( m_equip_list.m_main_chara_slot[idx] != m_equip_backup_list.m_main_chara_slot[idx] )
		{
			m_current_state.modify_flag |= CurrentState::MODIFY_CHARA;
		}
	}
	
	return m_current_state.modify_chara();
}

//report flag 에 따라서 equip를 CCharaInfoContext에 적용한다.
void CharaInfoMgr::change_equip()
{
	// 캐릭터의 메인 슬롯 동기화
	for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
	{
		m_equip_list.m_main_chara_slot[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx) );
		m_equip_backup_list.m_main_chara_slot[idx] = m_equip_list.m_main_chara_slot[idx];
	}

	//공통 Parts
	if( (INVENTORY_FLAG_COMMON_PARTS  & m_report_common.flag) == INVENTORY_FLAG_COMMON_PARTS)
	{
		m_equip_backup_list.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO] = m_equip_list.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO];

		COMMON_PARTS_INFO& info = *CCharaInfoContext::i()->SetCommonPartsInfo();
		// vv3 - check
		info.SetParts( &m_equip_list.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO] );
	}

	//개인 무기/Parts 는 해당 캐릭터 Slot을 사용해야 된다.
	for(size_t i =0; i< m_report_private_list.size(); ++i)
	{
		UINT32 flag = m_report_private_list[ i ].flag;
		UINT32 list_idx = m_report_private_list[ i ].list_idx;

		CHARA_INFO * char_info = CCharaInfoContext::i()->SetCharaInfo( list_idx );

		UINT8 idxCharaSlot = char_info->m_CharaBasicInfo.m_ui8CharaSlotIdx;

		//개인 무기
		if( (INVENTORY_FLAG_WEAPON  & flag) == INVENTORY_FLAG_WEAPON)
		{
			const ItemInfoList& weapon_list = m_equip_list.m_chara_list[idxCharaSlot][item_def::SCTG_WEAPON];
			m_equip_backup_list.m_chara_list[idxCharaSlot][item_def::SCTG_WEAPON] = weapon_list;

			// vv3 - check
			CHARA_WEAPON_INFO& info = char_info->m_CharaEquipInfo.m_CharWeaponInfo;
			for( INT32 k = 0; k < CHAR_EQUIPMENT_WEAPON_COUNT ; ++k )
			{
				info.m_Info[k] = weapon_list[k];
			}
		}
		//개인 파츠
		if( (INVENTORY_FLAG_PARTS  & flag) == INVENTORY_FLAG_PARTS)
		{
			const ItemInfoList& parts_list = m_equip_list.m_chara_list[idxCharaSlot][item_def::SCTG_PARTS];
			m_equip_backup_list.m_chara_list[idxCharaSlot][item_def::SCTG_PARTS] = parts_list;

			// vv3 - check
			CHARA_PARTS_INFO & info = char_info->m_CharaEquipInfo.m_CharPartsInfo;
			for( INT32 idx = 0 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx )
			{
				if( idx == CHAR_EQUIPMENT_PARTS_CHARA)	continue;
				item_def::PartsType type = static_cast<item_def::PartsType>(idx - CHAR_EQUIPMENT_PARTS_HEAD );
				info.m_Info[ idx] = parts_list[ type];
			}
		}
	}
}


bool CharaInfoMgr::send_event_change_equip(void)
{
	// 캐릭터 / 장비 모두 변경되지 않은 경우엔 처리 안함.
	isAuthAndStateCharaChange = false;
	m_current_state.modify_flag = 0;

	// 둘 다 무조건 해야 함.
	bool changed_equip				= check_changed_equip();
	bool changed_main_chara_slot	= check_changed_main_chara_slot();
	if( !changed_equip && !changed_main_chara_slot)
		return false;
	
	UINT8 change_slot_cnt = static_cast<UINT8>(m_report_common.count);
	UINT8 send_slot_cnt = 0;

	do 
	{
		PACKET_CHAR_CHANGE_EQUIP_REQ ReqData;

		for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
			ReqData.m_ui8TeamSlot[idx] = m_equip_list.m_main_chara_slot[idx];

		ReqData.m_bCommonEquipFlag = (m_report_common.flag > 0 ) ? true : false;

		//공통 Parts
		if( ReqData.m_bCommonEquipFlag )
		{
			//화면에 안 보여지는거라 원본에서 읽어 온다.
			ReqData.m_aCommonEquips[ CHAR_EQUIPMENT_COMMON_DINO ] = m_equip_list.m_share_list[item_def::SCTG_PARTS][item_def::PT_DINO];
		}

		//변경된 캐릭터 Slot 수
		ReqData.m_ui8ChangeSlotCount = change_slot_cnt - send_slot_cnt;

		if( ReqData.m_ui8ChangeSlotCount > S2MO_MULTI_SLOT_PACKET_COUNT)
			ReqData.m_ui8ChangeSlotCount = S2MO_MULTI_SLOT_PACKET_COUNT;

		I3ASSERT( ReqData.m_ui8ChangeSlotCount <= S2MO_MULTI_SLOT_PACKET_COUNT);

		UINT8 start_idx = send_slot_cnt;

		S2_CHAR_CHANGE_EQUIP_INFO * pEquipInfo;

		for( UINT8 idx = 0; idx < ReqData.m_ui8ChangeSlotCount; ++idx)
		{
			UINT8 data_idx = idx + start_idx;

			pEquipInfo = &ReqData.m_ChangeEquipInfo[idx];

			// 공룡이다. 스킵
			if( m_report_private_list[ data_idx ].flag == INVENTORY_FLAG_COMMON_PARTS)
			{
				continue;
			}

			UINT8 list_idx = m_report_private_list[data_idx].list_idx;
			UINT8 chara_idx = m_report_private_list[data_idx].chara_idx;

			pEquipInfo->m_ui8SlotIdx = chara_idx;

			const ItemInfoList & weapon_list = m_equip_list.m_chara_list[chara_idx][item_def::SCTG_WEAPON];
			const ItemInfoList & parts_list = m_equip_list.m_chara_list[chara_idx][item_def::SCTG_PARTS];

			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_PRIMARY ]		= weapon_list[item_def::WT_PRIMARY];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_SECONDARY ]	= weapon_list[item_def::WT_SECONDARY];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_MELEE ]		= weapon_list[item_def::WT_MELEE];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_THROWING1 ]	= weapon_list[item_def::WT_THROWING];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_THROWING2 ]	= weapon_list[item_def::WT_ITEM];

			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_HEAD ]			= parts_list[item_def::PT_HEADGEAR];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_FACE ]			= parts_list[item_def::PT_FACEGEAR];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_UPPER ]		= parts_list[item_def::PT_UPPER];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_LOWER ]		= parts_list[item_def::PT_LOWER];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_GLOVE ]		= parts_list[item_def::PT_GLOVE];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_BELT ]			= parts_list[item_def::PT_BELT];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_HOLSTER ]		= parts_list[item_def::PT_HOLSTER];
			pEquipInfo->m_aCharEquipment[ CHAR_EQUIPMENT_SKIN ]			= parts_list[item_def::PT_SKIN];
			pEquipInfo->m_aCharEquipment [CHAR_EQUIPMENT_BERET ]		= parts_list[item_def::PT_BERET];

			const CHARA_INFO* info = CCharaInfoContext::i()->GetCharaInfoByList(list_idx);
			pEquipInfo->m_aCharEquipment[CHAR_EQUIPMENT_CHARA] = *(info->getPartsInfo()->GetPartsInfo(CHAR_EQUIPMENT_PARTS_CHARA));

			send_slot_cnt++;
		}

		GameEventSender::i()->SetEvent(EVENT_INVEN_CHANGE_CHARA_EQUIP, &ReqData);

	} while ( send_slot_cnt < change_slot_cnt );

	return true;
}