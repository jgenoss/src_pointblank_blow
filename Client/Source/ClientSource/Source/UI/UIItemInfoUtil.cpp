#include "pch.h"

#include "UIItemInfoUtil.h"
#include "ItemID/ItemType.h"
#include "ItemID/ItemTypeCharaClass.h"
#include "DinoInfo.h"

#include "UI/UIShopFunction.h"

#include "UISignal.h"

ITEM_INFO item_def::NULL_ITEM_INFO;

/*********************************//*********************************/
//기능상 변환
item_def::Category item_def::get_category(T_ItemID item_id)
{
	ITEM_TYPE ItemClass = (ITEM_TYPE)GET_ITEM_TYPE(item_id);

	switch (ItemClass)
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		return item_def::CTG_WEAPON;

	case ITEM_TYPE_DINO:
	case ITEM_TYPE_CHARA:
		return item_def::CTG_CHARA;

	case ITEM_TYPE_HEADGEAR:
	case ITEM_TYPE_FACEGEAR:
	case ITEM_TYPE_UPPER:
	case ITEM_TYPE_LOWER:
	case ITEM_TYPE_GLOVE:
	case ITEM_TYPE_BELT:
	case ITEM_TYPE_HOLSTER:
	case ITEM_TYPE_BERET:
		return item_def::CTG_PARTS;

	case ITEM_TYPE_SKIN:
		return item_def::CTG_SKIN;

	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_INGAMEITEM:		// 여기에 포함시킴..(12.02.24.수빈) (야전템의 경우, case by case로 상점아이템과 동일개념을 넣거나 넣지 않음)
	case ITEM_TYPE_DOMIITEM:
	case ITEM_TYPE_GACHA_TICKET:
		return item_def::CTG_ITEM;

		//그 외에것( CInvenList 에서 사용하기에 넣었음 )
	case ITEM_TYPE_POINT:
		return item_def::CTG_ITEM;

		//case ITEM_TYPE_ITEM	:
		//	return item_def::CTG_CHARA;
	}

	return item_def::CTG_NA;
}
//일반 변환
item_def::Category item_def::get_category_tab( T_ItemID item_id)
{
	ITEM_TYPE ItemClass = (ITEM_TYPE) GET_ITEM_TYPE(item_id);

	if (g_pEnvSet->IsV2Version())
	{
		switch (ItemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			return item_def::CTG_WEAPON;

		case ITEM_TYPE_DINO:
		case ITEM_TYPE_CHARA:
			return item_def::CTG_CHARA;

		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_FACEGEAR:
		case ITEM_TYPE_UPPER:
		case ITEM_TYPE_LOWER:
		case ITEM_TYPE_GLOVE:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_HOLSTER:
		case ITEM_TYPE_BERET:
			return item_def::CTG_PARTS;

		case ITEM_TYPE_SKIN:
			return item_def::CTG_SKIN;

		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_PERIOD:
		case ITEM_TYPE_WRAP_COUNT:
		case ITEM_TYPE_INGAMEITEM:		// 여기에 포함시킴..(12.02.24.수빈) (야전템의 경우, case by case로 상점아이템과 동일개념을 넣거나 넣지 않음)
		case ITEM_TYPE_DOMIITEM:
		case ITEM_TYPE_GACHA_TICKET:
			return item_def::CTG_ITEM;

			//그 외에것( CInvenList 에서 사용하기에 넣었음 )
		case ITEM_TYPE_POINT:
			return item_def::CTG_ITEM;

			//case ITEM_TYPE_ITEM	:
			//	return item_def::CTG_CHARA;
		}
	}
	else
	{
		switch (ItemClass)
		{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			return item_def::CTG_WEAPON;

		case ITEM_TYPE_DINO:
		case ITEM_TYPE_CHARA:
		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_FACEGEAR:
		case ITEM_TYPE_BERET:
			return item_def::CTG_CHARA;

		case ITEM_TYPE_UPPER:
		case ITEM_TYPE_LOWER:
		case ITEM_TYPE_GLOVE:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_HOLSTER:
			return item_def::CTG_PARTS;

		case ITEM_TYPE_SKIN:
			return item_def::CTG_SKIN;

		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_PERIOD:
		case ITEM_TYPE_WRAP_COUNT:
		case ITEM_TYPE_INGAMEITEM:		// 여기에 포함시킴..(12.02.24.수빈) (야전템의 경우, case by case로 상점아이템과 동일개념을 넣거나 넣지 않음)
		case ITEM_TYPE_DOMIITEM:
		case ITEM_TYPE_GACHA_TICKET:
			return item_def::CTG_ITEM;

			//그 외에것( CInvenList 에서 사용하기에 넣었음 )
		case ITEM_TYPE_POINT:
			return item_def::CTG_ITEM;

			//case ITEM_TYPE_ITEM	:
			//	return item_def::CTG_CHARA;
		}
	}


	return item_def::CTG_NA;
}

item_def::Category item_def::get_category_goodsID(T_GoodsID goods_id)
{
	//김대영 제작. 
	//굿즈 아이디로 아이템 카테고리만! 판별하는 함수입니다. 위의 get_category 함수랑 기능 자체는 동일합니다.

	ITEM_TYPE ItemClass = (ITEM_TYPE) GET_GOODS_TYPE(goods_id);
	
	switch(ItemClass)
	{
		case ITEM_TYPE_PRIMARY:
		case ITEM_TYPE_SECONDARY:
		case ITEM_TYPE_MELEE:
		case ITEM_TYPE_THROWING1:
		case ITEM_TYPE_THROWING2:
			return item_def::CTG_WEAPON;

		case ITEM_TYPE_DINO:
		case ITEM_TYPE_CHARA:
			return item_def::CTG_CHARA;

		case ITEM_TYPE_HEADGEAR:
		case ITEM_TYPE_FACEGEAR:
		case ITEM_TYPE_UPPER:
		case ITEM_TYPE_LOWER:
		case ITEM_TYPE_GLOVE:
		case ITEM_TYPE_BELT:
		case ITEM_TYPE_HOLSTER:
		case ITEM_TYPE_BERET:
			return item_def::CTG_PARTS;

		case ITEM_TYPE_SKIN	:
			return item_def::CTG_SKIN;

		case ITEM_TYPE_MAINTENANCE:
		case ITEM_TYPE_WRAP_PERIOD:
		case ITEM_TYPE_WRAP_COUNT:
		case ITEM_TYPE_INGAMEITEM:		// 여기에 포함시킴..(12.02.24.수빈) (야전템의 경우, case by case로 상점아이템과 동일개념을 넣거나 넣지 않음)
		case ITEM_TYPE_DOMIITEM:
		case ITEM_TYPE_GACHA_TICKET:
			return item_def::CTG_ITEM;

			//그 외에것( CInvenList 에서 사용하기에 넣었음 )
		case ITEM_TYPE_POINT:
			return item_def::CTG_ITEM;

			//goods id 로는 99로 되어있는건 모두 아이템 카테고리라 생각하도록 합니다.
		case ITEM_TYPE_SET:
			return item_def::CTG_ITEM;
	}

	return item_def::CTG_NA;
}

item_def::SideCategory item_def::get_side_category( T_ItemID item_id)
{
	return item_def::convert( item_def::get_category(item_id) );
}

bool item_def::is_chara_category( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_CHARA )
		return false;

	if( item_def::get_type(item_id) == ITEM_TYPE_DINO )
		return false;

	return true;
}

bool item_def::is_dino_category( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_CHARA )
		return false;

	return item_def::get_type(item_id) == ITEM_TYPE_DINO;
}

item_def::PartsType_ShopPreview item_def::get_shop_preview_parts_type( T_ItemID item_id)
{
	item_def::Category ctg = item_def::get_category(item_id);
	switch( ctg )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
			ITEM_TYPE item_type = item_def::get_parts_type(item_id);
			item_def::PartsType parts_type = item_def::convert_parts_type(item_type);

			if (PT_HEADGEAR <= parts_type && parts_type <= PT_HOLSTER)			// 딱 여기까지만 허용합니다...그외에는 모두 무시함..
				return (item_def::PartsType_ShopPreview)( parts_type );
		}
		break;
	}
	return item_def::PartsType_ShopPreview_None;
}


WEAPON_SLOT_TYPE item_def::get_weapon_slot_type( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_WEAPON )
		return WEAPON_SLOT_UNKNOWN;

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(item_id);
	if( pInfo == 0)
		return WEAPON_SLOT_UNKNOWN;

	return pInfo->GetTypeUsage();
}

WEAPON_CLASS_TYPE item_def::get_weapon_class_type( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_WEAPON )
		return WEAPON_CLASS_UNKNOWN;

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(item_id);
	if( pInfo == 0)
		return WEAPON_CLASS_UNKNOWN;

	return pInfo->GetTypeClass();
}

ITEM_TYPE item_def::get_weapon_type( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_WEAPON )
		return ITEM_TYPE_UNKNOWN;

	return (ITEM_TYPE) GET_ITEM_TYPE(item_id);
}

ITEM_TYPE item_def::get_chara_type( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_CHARA )
		return ITEM_TYPE_UNKNOWN;

	return (ITEM_TYPE) GET_ITEM_TYPE(item_id);
}

ITEM_TYPE item_def::get_parts_type( T_ItemID item_id)
{
	item_def::Category ctg = item_def::get_category(item_id);
	if( ctg != item_def::CTG_PARTS && ctg != item_def::CTG_CHARA && ctg != item_def::CTG_SKIN)
		return ITEM_TYPE_UNKNOWN;

	return (ITEM_TYPE) GET_ITEM_TYPE(item_id);
}

ITEM_TYPE item_def::get_item_type( T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_ITEM )
		return ITEM_TYPE_UNKNOWN;

	return (ITEM_TYPE) GET_ITEM_TYPE(item_id);
}

ITEM_TYPE item_def::get_type( T_ItemID item_id)
{
	return (ITEM_TYPE) GET_ITEM_TYPE(item_id);
}






WEAPON_CLASS_TYPE item_def::get_weapon_usage(T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_WEAPON )
		return WEAPON_CLASS_UNKNOWN;

	CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo(item_id);
	if( pInfo == 0)
		return WEAPON_CLASS_UNKNOWN;

	return pInfo->GetTypeClass();
}

CHARACTER_CLASS_TYPE item_def::get_chara_usage(T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_CHARA )
		return CHARACTER_CLASS_UNKNOWN;

	return CHARA::ItemID2Class( item_id);
}

CHARACTER_CLASS_TYPE item_def::get_parts_usage(T_ItemID item_id)
{
	item_def::Category category = item_def::get_category(item_id);
	if( category != item_def::CTG_PARTS && category != item_def::CTG_SKIN)
		return CHARACTER_CLASS_UNKNOWN;

	return CHARA::ItemID2Class(item_id);
}

ITEM_TYPE item_def::get_item_usage(T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_ITEM )
		return ITEM_TYPE_UNKNOWN;

	return (ITEM_TYPE) GET_ITEM_TYPE(item_id);
}

INT32 item_def::get_item_number(T_ItemID item_id)
{
	if( item_def::get_category(item_id) != item_def::CTG_ITEM )
		return -1;

	return (INT32) GET_ITEM_NUMBER(item_id);
}

UINT32 item_def::get_usage(T_ItemID item_id)
{
	return (UINT32) GET_ITEM_SUBTYPE(item_id);
}





/*********************************//*********************************/
//타입 변환
item_def::SideCategory item_def::convert(item_def::Category category)
{
	switch(category)
	{
	case item_def::CTG_WEAPON:		return item_def::SCTG_WEAPON;		break;
	case item_def::CTG_CHARA:		return item_def::SCTG_PARTS;		break; //dino가 여기에 속한다.
	case item_def::CTG_PARTS:		return item_def::SCTG_PARTS;		break;
	case item_def::CTG_SKIN:		return item_def::SCTG_PARTS;		break;
	}

	return item_def::SCTG_NA;
}

item_def::Category item_def::convert(item_def::SideCategory category)
{
	switch(category)
	{
	case item_def::SCTG_WEAPON:		return item_def::CTG_WEAPON;		break;
	case item_def::SCTG_PARTS:		return item_def::CTG_PARTS;		break;
	}

	return item_def::CTG_NA;
}

CHAR_EQUIPMENT_PARTS item_def::convert(item_def::PartsType type)
{
	if( type < item_def::PT_HEADGEAR || type > item_def::PT_BERET )
	{
		I3PRINTLOG(I3LOG_FATAL,  "insert code please!");
		return CHAR_EQUIPMENT_PARTS_CHARA;
	}
	
	UINT32 ui32Parts = (UINT32)type;
	++ui32Parts;
	return (CHAR_EQUIPMENT_PARTS)ui32Parts;
}

item_def::WeaponType item_def::convert_weapon_type(ITEM_TYPE type)
{
	switch(type)
	{
	case ITEM_TYPE_PRIMARY:	return item_def::WT_PRIMARY;
	case ITEM_TYPE_SECONDARY:	return item_def::WT_SECONDARY;
	case ITEM_TYPE_MELEE:	return item_def::WT_MELEE;
	case ITEM_TYPE_THROWING1:	return item_def::WT_THROWING;
	case ITEM_TYPE_THROWING2:	return item_def::WT_ITEM;
	}
	return item_def::WT_NA;
}
UINT32 item_def::convert_chara_type(ITEM_TYPE type)
{
	switch(type)
	{
	case ITEM_TYPE_CHARA:	return item_def::CT_MAN;
	case ITEM_TYPE_DINO:	return item_def::CT_MAN;
	//case ITEM_TYPE_MELEE:	return item_def::WT_MELEE;
	//case ITEM_TYPE_THROWING1:	return item_def::WT_THROWING;
	//case ITEM_TYPE_THROWING2:	return item_def::WT_ITEM;
	}
	return type;
}
item_def::PartsType item_def::convert_parts_type(ITEM_TYPE type)
{
	switch(type)
	{
	case ITEM_TYPE_HEADGEAR:	return item_def::PT_HEADGEAR;
	case ITEM_TYPE_FACEGEAR:	return item_def::PT_FACEGEAR;
	case ITEM_TYPE_UPPER:		return item_def::PT_UPPER;
	case ITEM_TYPE_LOWER:		return item_def::PT_LOWER;
	case ITEM_TYPE_GLOVE:		return item_def::PT_GLOVE;
	case ITEM_TYPE_BELT:		return item_def::PT_BELT;
	case ITEM_TYPE_HOLSTER:		return item_def::PT_HOLSTER;
	case ITEM_TYPE_SKIN:		return item_def::PT_SKIN;
	case ITEM_TYPE_DINO:		return item_def::PT_DINO;
	case ITEM_TYPE_BERET:		return item_def::get_beret_parts_type();
	case ITEM_TYPE_CHARA:		break;
	default :
		I3PRINTLOG(I3LOG_FATAL,  "insert code please!");
		break;
	}
	return item_def::PT_NA;
}

item_def::ItemType item_def::convert_item_type(ITEM_TYPE type)
{
	switch(type)
	{
	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:	
	case ITEM_TYPE_GACHA_TICKET:
		return item_def::IT_ENHANCE;
	}
	return item_def::IT_NA;
}


SHOP_GOODS_TYPE item_def::convert_goods_type(item_def::Category category)
{
	switch( category )
	{
	case item_def::CTG_WEAPON:	return SHOP_WEAPON_GOODS;
	case item_def::CTG_CHARA:	return SHOP_CHARACTER_GOODS;
	case item_def::CTG_PARTS:	return SHOP_CHARACTER_GOODS;
	case item_def::CTG_SKIN:	return SHOP_CHARACTER_GOODS;
	case item_def::CTG_ITEM:	return SHOP_WEAPON_GOODS;
	}

	return SHOP_GOODS_UNKNOWN;
}

WEAPON_SLOT_TYPE item_def::convert_weapon_type(item_def::WeaponType type)
{
	switch(type)
	{
	case item_def::WT_ALL:	return WEAPON_SLOT_ALL;
	case item_def::WT_PRIMARY:	return WEAPON_SLOT_PRIMARY;
	case item_def::WT_SECONDARY:	return WEAPON_SLOT_SECONDARY;
	case item_def::WT_MELEE:	return WEAPON_SLOT_MELEE;
	case item_def::WT_THROWING:	return WEAPON_SLOT_THROWING1;
	case item_def::WT_ITEM:	return WEAPON_SLOT_THROWING2;
	}
	return WEAPON_SLOT_UNKNOWN;
}

UINT32 item_def::convert_chara_type(UINT32 type)
{
	switch(type)
	{
	case item_def::CT_ALL:	return CHARACTER_CLASS_ALL;
	case item_def::CT_MAN:	return CHARACTER_CLASS_ALL;
	case item_def::CT_WOMAN:	return CHARACTER_CLASS_ALL;
	default:
	{
		return type;
	}
	}
}
CHARACTER_CLASS_TYPE item_def::convert_parts_type(item_def::PartsType type)
{
	switch(type)
	{
	case item_def::PT_ALL:		return CHARACTER_CLASS_HEADGEAR; //파츠는 전체란 0 값이 headgear를 나타낸다.
	case item_def::PT_FACEGEAR:	return CHARACTER_CLASS_FACEGEAR;
	case item_def::PT_HEADGEAR:	return CHARACTER_CLASS_HEADGEAR;
	case item_def::PT_UPPER:	return CHARACTER_CLASS_UPPER;
	case item_def::PT_LOWER:	return CHARACTER_CLASS_LOWER;
	case item_def::PT_BELT:		return CHARACTER_CLASS_BELT;
	case item_def::PT_GLOVE:	return CHARACTER_CLASS_GLOVE;
	case item_def::PT_HOLSTER:	return CHARACTER_CLASS_HOLSTER;
	case item_def::PT_SKIN :	return CHARACTER_CLASS_SKIN;
	case item_def::PT_BERET:	return item_def::get_beret_class_type();
	default :
		I3PRINTLOG(I3LOG_FATAL,  "insert code please!");
		break;
	}
	return CHARACTER_CLASS_UNKNOWN;
}

ITEM_TYPE item_def::convert_item_type(item_def::ItemType type)
{
	//미완성
	switch(type)
	{
	case item_def::IT_ALL:	return ITEM_TYPE_UNKNOWN;
	//case item_def::IT_ENHANCE:	return ITEM_TYPE_MAINTENANCE;
	//case item_def::IT_FEATURE:	return ITEM_TYPE_WRAP_PERIOD;
	//case item_def::IT_REWARD:	return ITEM_TYPE_MAINTENANCE;
	//case item_def::IT_BOX:	return ITEM_TYPE_MAINTENANCE;
	//case item_def::IT_SET:	return ITEM_TYPE_MAINTENANCE;
	}
	return ITEM_TYPE_UNKNOWN;
}

item_def::PartsType item_def::get_beret_parts_type(void)
{
	if( g_pEnvSet->IsV2Version() )
		 return item_def::PT_HEADGEAR;
	
	return item_def::PT_BERET;
}

CHARACTER_CLASS_TYPE item_def::get_beret_class_type(void)
{
	if( g_pEnvSet->IsV2Version() )
		return CHARACTER_CLASS_HEADGEAR;

	return CHARACTER_CLASS_BERET;
}

bool item_def::is_default_item(T_ItemID item_id)
{
	item_def::Category category = get_category(item_id);

	switch( category )
	{
	case item_def::CTG_WEAPON:	
		if( item_id == DEFAULT_WEAPON_ASSAULT )		return true;
		if( item_id == DEFAULT_WEAPON_SNIPER )		return true;
		if( item_id == DEFAULT_WEAPON_SMG )			return true;
		if( item_id == DEFAULT_WEAPON_SHOTGUN )		return true;
		if( item_id == DEFAULT_WEAPON_MG )			return true;
		if( item_id == DEFAULT_WEAPON_HANDGUN )		return true;
		if( item_id == DEFAULT_WEAPON_KNIFE )		return true;
		if( item_id == DEFAULT_WEAPON_THROWING1 )	return true;
		if( item_id == DEFAULT_WEAPON_THROWING2 )	return true;
		if( item_id == DEFAULT_WEAPON_MISSON )		return true;
		break;
	case item_def::CTG_CHARA:	
		if( item_id == DEFAULT_DINO )				return true;
		if( item_def::is_chara_category(item_id) )	return false;
	case item_def::CTG_PARTS:
		{
			if( item_id == 0 ) return true;

			INT32 charaCnt = g_pCharaInfoDataBase->GetInfoCount( EQUIP::ePART_CHARA);
			for( INT32 i = 0; i < charaCnt; i++)
			{
				const CCharaInfo * pCharaDBInfo = static_cast<const CCharaInfo*>(g_pCharaInfoDataBase->GetInfoByIndex( EQUIP::ePART_CHARA, (EQUIP::RES_ID) i));

				if( i3::same_of<CDinoInfo*>(pCharaDBInfo) )	continue;

				for( INT32 j = 0; j < EQUIP::ePART_COUNT; j++)
				{
					if( pCharaDBInfo->GetPartsItemID( (EQUIP::ePART) j) == item_id)
						return true;
				}
			}
		}
		break;

	case item_def::CTG_SKIN:
	case item_def::CTG_ITEM:
		break;

	default :
		I3ASSERT_0;	// 코드 작성 필요
		break;
	}

	return false;
}

bool item_def::is_default_parts(T_ItemID item_id, T_ItemID chara_item_id)
{
	item_def::Category category = get_category(item_id);

	switch( category )
	{
	case item_def::CTG_PARTS:
		{
			if( item_id == 0 ) return true;

			const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( chara_item_id);
			if( pCharaDBInfo != nullptr)
			{
				for( INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
				{
					if( pCharaDBInfo->GetPartsItemID( (EQUIP::ePART) i) == item_id)
						return true;
				}
			}
		}
		break;
	}
	return false;
}

bool item_def::is_shared_parts_item(T_ItemID item_id, T_ItemID chara_item_id)
{
	item_def::Category category = get_category(item_id);

	bool bShared = false;
	
	switch( category )
	{
	case item_def::CTG_PARTS:
	case item_def::CTG_SKIN:
		{
		//	UI::call_cb("is_ingore_shared", &bShared);
			UISignal::IsIgnoreShared( i3::ref(bShared) );

			if( item_id == 0 ) return false;

			const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( item_id );
			if( pInfo != nullptr && pInfo->IsSharedUniq() )
			{
				for( UINT32 j = 0; j < MAX_SHARE_RES; j++)
				{
					T_ItemID sharedItem = pInfo->GetShareResItemID( j);
					if( sharedItem != 0xFFFFFFFF)
					{
						if( chara_item_id == sharedItem)
						{
							bShared = true;
							break;
						}
					}
				}
			}
			else
			{
				bShared = true;

				const CCharaInfo * charinfo = g_pCharaInfoDataBase->GetCharaDBInfo(chara_item_id);
				if ( charinfo && charinfo->GetCharaSexType() == CHARA::eSEX_TYPE_SKINY_MALE)
				{
					EQUIP::ePART part = EQUIP::ItemID2PartsType(item_id);
					if (
							part != EQUIP::ePART_FACEGEAR	&&
							part != EQUIP::ePART_HEADGEAR	&&
							part != EQUIP::ePART_BERET		
					   )
						bShared = false;
				}
			}
		}
		break;
	}

	return bShared;
}

ITEM_INFO item_def::get_default_item(T_ItemID item_id)
{
	WEAPON_SLOT_TYPE slot_type = item_def::get_weapon_slot_type(item_id);
	WEAPON_CLASS_TYPE type = item_def::get_weapon_usage(item_id);

	return item_def::get_default_item(slot_type, type);
}

ITEM_INFO item_def::get_default_item(UINT32 idx, WEAPON_CLASS_TYPE type)
{
	ITEM_INFO ret = item_def::NULL_ITEM_INFO;

	switch(idx)
	{
	case item_def::WT_PRIMARY: 
		switch(type)
		{
		case WEAPON_CLASS_ASSAULT	: ret.m_TItemID = DEFAULT_WEAPON_ASSAULT;	break;
		case WEAPON_CLASS_SNIPER	: ret.m_TItemID = DEFAULT_WEAPON_SNIPER;	break;
		case WEAPON_CLASS_DUALSMG	:
		case WEAPON_CLASS_SMG		: ret.m_TItemID = DEFAULT_WEAPON_SMG;		break;
		case WEAPON_CLASS_SHOTGUN	: ret.m_TItemID = DEFAULT_WEAPON_SHOTGUN;	break;
		case WEAPON_CLASS_MG		: ret.m_TItemID = DEFAULT_WEAPON_MG;		break;
		}
		break;
	case item_def::WT_SECONDARY: ret.m_TItemID = DEFAULT_WEAPON_HANDGUN; break;
	case item_def::WT_MELEE: ret.m_TItemID = DEFAULT_WEAPON_KNIFE; break;
	case item_def::WT_THROWING: ret.m_TItemID = DEFAULT_WEAPON_THROWING1; break;
	case item_def::WT_ITEM: ret.m_TItemID = DEFAULT_WEAPON_THROWING2; break;
	default:
		return ret;
	}
	ret.m_TItemDBIdx = CInvenList::i()->FindWareDBIdx_ByItemID(ret.m_TItemID);
	return ret;
}


ITEM_INFO item_def::get_default_item(T_ItemID chara_item_id, INT32 idx)
{
	ITEM_INFO ret = item_def::NULL_ITEM_INFO;

	I3ASSERT_RETURN(idx >= 0, ret);

	const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( chara_item_id);

	if( pCharaDBInfo != nullptr)
	{
		if( (EQUIP::ePART)idx == EQUIP::ePART_CHARA )
			ret.m_TItemID = chara_item_id;
		else
			ret.m_TItemID = pCharaDBInfo->GetPartsItemID( (EQUIP::ePART) idx);

		if( ret.m_TItemID > 0)
		{
			ITEM_TYPE part_type = (ITEM_TYPE) GET_ITEM_TYPE(ret.m_TItemID);

			if( part_type >= ITEM_TYPE_HEADGEAR && part_type <= ITEM_TYPE_SKIN )
			{
				ret.m_TItemID = static_cast<T_ItemID>(MAKE_DEFAULT_PARTS_ITEMID(part_type) );
				ret.m_TItemDBIdx = MAKE_DEFAULT_EQUIP_DBIDX(part_type);
			}
			else
			{
				ret.m_TItemID = 0;
			}
		}
		else
		{
			// V2 Skin Load 시스템이 들어가면서 스킨일 경우에는 디폴트로 바꿉니다.
			if (g_pEnvSet->IsV2Version() == true)
			{
				if (ret.m_TItemID == 0 && (EQUIP::ePART) idx == EQUIP::ePART_SKIN)
				{
					ret.m_TItemID = static_cast<T_ItemID>((T_ItemID)MAKE_DEFAULT_PARTS_ITEMID(ITEM_TYPE_SKIN));
					ret.m_TItemDBIdx = MAKE_DEFAULT_EQUIP_DBIDX(ITEM_TYPE_SKIN);
				}
			}
		}
	}

	return ret;
}

ITEM_INFO item_def::get_default_dino(void)
{
	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType(DEFAULT_DINO);
	I3ASSERT( dbType == INVEN::DB_CASHITEM );

	INT32 usage_type = GET_ITEM_SUBTYPE( DEFAULT_DINO );
	INT32 idx = CInvenList::i()->FindItemIndexByItemID(dbType, DEFAULT_DINO, usage_type);

	ITEM_INFO ret;
	ret.m_TItemID = DEFAULT_DINO;
	ret.m_TItemDBIdx = CInvenList::i()->GetDBIdxByIndex(dbType, idx, usage_type );

	return ret;
}

UINT item_def::get_shop_slot_idx(T_ItemID item_id)
{
	return ItemInfoConvert(item_id).get_shop_slot_idx();
}

UINT item_def::get_char_slot_idx(T_ItemID item_id)
{
	return ItemInfoConvert(item_id).get_char_slot_idx();
}

bool item_def::is_locked_weapon(UINT8 mode_flag, T_ItemID item_id)
{
	WEAPON_CLASS_TYPE type = item_def::get_weapon_class_type(item_id);
	WEAPON_SLOT_TYPE slot = item_def::get_weapon_slot_type(item_id);
	return item_def::is_locked_weapon(mode_flag, type, slot);
}
bool item_def::is_locked_weapon(UINT8 mode_flag, WEAPON_CLASS_TYPE classType, WEAPON_SLOT_TYPE usage)
{
	switch( usage)
	{
	case WEAPON_SLOT_PRIMARY :
		if( TESTBIT( mode_flag, LOCK_SNIPER) )
		{
			if( classType != WEAPON_CLASS_SNIPER)
			{
				return true;
			}
		}
		else if( TESTBIT( mode_flag, LOCK_SHOTGUN) )
		{
			if( classType != WEAPON_CLASS_SHOTGUN)
			{
				return true;
			}
		}
		else if( TESTBIT( mode_flag, LOCK_KNUCKLE) )
		{
			if( classType != WEAPON_CLASS_KNIFE)
			{
				return true;
			}
		}
		break;

	case WEAPON_SLOT_MELEE :
		if( TESTBIT( mode_flag, LOCK_KNUCKLE) )
		{
			if( classType != WEAPON_CLASS_KNUCKLE)
			{ 
				return true;
			}
		}  
		break;
	} 
  
	return false;
}

bool item_def::is_direct_use_item(T_ItemID item_id)
{
	ITEM_TYPE type = item_def::get_item_type(item_id);

	if( type == ITEM_TYPE_INGAMEITEM ) return false;
	if( type == ITEM_TYPE_DOMIITEM ) return false;

	return true;
}


/*********************************/
//자동 변환
item_def::Category ItemInfoConvert::get_category() const 
{ 
	return item_def::get_category(m_info.m_TItemID);
}

item_def::SideCategory ItemInfoConvert::get_side_category()  const
{
	return item_def::convert( get_category() );
}

item_def::WeaponType ItemInfoConvert::get_weapon_type() const
{
	switch( item_def::get_weapon_slot_type(m_info.m_TItemID) )
	{
	case WEAPON_SLOT_PRIMARY:	return item_def::WT_PRIMARY;
	case WEAPON_SLOT_SECONDARY:	return item_def::WT_SECONDARY;
	case WEAPON_SLOT_MELEE:	return item_def::WT_MELEE;
	case WEAPON_SLOT_THROWING1:	return item_def::WT_THROWING;
	case WEAPON_SLOT_THROWING2:	return item_def::WT_ITEM;
	}

	return item_def::WT_NA;
}

item_def::WeaponComboType ItemInfoConvert::get_weapon_combo_type() const
{
	switch( item_def::get_weapon_class_type(m_info.m_TItemID) )
	{
	case WEAPON_CLASS_ASSAULT		: return item_def::WCT_ASSAULT;
	case WEAPON_CLASS_SMG			:
	case WEAPON_CLASS_DUALSMG		: return item_def::WCT_SMG;
	case WEAPON_CLASS_SNIPER		: return item_def::WCT_SNIPER;
	case WEAPON_CLASS_MG			: return item_def::WCT_MACHINE;
	case WEAPON_CLASS_SHOTGUN		:
	case WEAPON_CLASS_DUALSHOTGUN	: return item_def::WCT_SHOTGUN;
	}

	return item_def::WCT_NA;
}

UINT32 ItemInfoConvert::get_chara_type() const
{
	INT32 type = item_def::get_chara_type(m_info.m_TItemID);
	switch(type)
	{
	case ITEM_TYPE_CHARA:	case ITEM_TYPE_DINO:
		{
			const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo( m_info.m_TItemID);
			if( pInfo != nullptr)
			{
				return pInfo->GetCharaSexTypeDISPLAY();
			}
		}
		break;
	}

	return type;
}

item_def::SKINTYPE ItemInfoConvert::get_skin_type() const
{
	//m_info.m_TItemID
	return item_def::ST_COMMON;
}

item_def::PartsType ItemInfoConvert::get_parts_type() const
{
	switch( item_def::get_parts_type(m_info.m_TItemID) )
	{
	case ITEM_TYPE_HEADGEAR:	return item_def::PT_HEADGEAR;	// 왜 바꿔놨는지 모름.. 크래쉬나기때문에 원래 파츠에 맞게끔 바꿈. 순구
	case ITEM_TYPE_FACEGEAR:	return item_def::PT_FACEGEAR;
	case ITEM_TYPE_UPPER:		return item_def::PT_UPPER;
	case ITEM_TYPE_LOWER:		return item_def::PT_LOWER;
	case ITEM_TYPE_GLOVE:		return item_def::PT_GLOVE;
	case ITEM_TYPE_BELT:		return item_def::PT_BELT;
	case ITEM_TYPE_HOLSTER:		return item_def::PT_HOLSTER;
	case ITEM_TYPE_DINO:		return item_def::PT_DINO;
	case ITEM_TYPE_SKIN:		return item_def::PT_SKIN;
	case ITEM_TYPE_BERET:		return get_beret_type();
	case ITEM_TYPE_CHARA :		break;
	default :
		I3PRINTLOG(I3LOG_FATAL,  "insert code please!!");
		break;
	}

	return item_def::PT_NA;
}

item_def::PartsType ItemInfoConvert::get_beret_type() const
{
	if( g_pEnvSet->IsV2Version() )
		return item_def::PT_HEADGEAR;

	return item_def::PT_BERET;
}

item_def::ItemType ItemInfoConvert::get_item_type() const
{
	//switch( item_def::get_item_type(m_info.m_TItemID) )
	//{
	//case ITEM_TYPE_MAINTENANCE:	return item_def::IT_ENHANCE;
	//case ITEM_TYPE_EXPENDABLE:	return item_def::IT_FEATURE;
	//case ITEM_TYPE_UPPER:	return item_def::IT_REWARD;
	//case ITEM_TYPE_LOWER:	return item_def::IT_BOX;
	//case ITEM_TYPE_GLOVE:	return item_def::IT_SET;
	//case ITEM_TYPE_BELT:	return item_def::PT_GLOVE;
	//case ITEM_TYPE_HOLSTER:	return item_def::PT_HOLSTER;
	//}

	return item_def::IT_NA;
}

/*********************************/
UINT32 ItemInfoConvert::get_shop_slot_idx() const
{
	switch( get_category() )
	{
	case item_def::CTG_WEAPON:	return get_weapon_type();
	case item_def::CTG_CHARA:	return get_chara_type();
	case item_def::CTG_PARTS:	return get_parts_type();
	case item_def::CTG_SKIN :	return get_skin_type();
	case item_def::CTG_ITEM:	return get_item_type();
	}

	return UINT_MAX;
}

UINT32 ItemInfoConvert::get_char_slot_idx() const
{
	switch( get_side_category() )
	{
	case item_def::SCTG_WEAPON:	return get_weapon_type();
	case item_def::SCTG_PARTS:	return get_parts_type();
	}

	return UINT32(-1);
}


bool ItemInfoConvert::is_default_item() const
{
	return item_def::is_default_item(m_info.m_TItemID);
}

bool ItemInfoConvert::is_default_item_to_DBidx(const T_ItemDBIdx itemdbidx) const
{
	return (itemdbidx & DEFAULT_ITEM_DBIDX_MIN) != 0;
}

bool ItemInfoConvert::is_default_parts(T_ItemID chara_item_id) const
{
	return item_def::is_default_parts(m_info.m_TItemID, chara_item_id);
}



/************************//************************/
void ItemInfoList::push( const ITEM_INFO & info)
{
	m_list.push_back( info );
}

void ItemInfoList::push( const ItemInfoList& rhs)
{
	i3::copy( rhs.m_list.begin(), rhs.m_list.end(), i3::back_inserter(m_list) );
}

void ItemInfoList::remove( size_t idx)
{
	List::iterator It = m_list.begin();
	It += idx;
	m_list.erase( It);
}

void ItemInfoList::operator=( const ItemInfoList & rhs )
{
	m_list.clear();
	m_list.resize( rhs.size() );

	i3::copy( rhs.m_list.begin(), rhs.m_list.end(), m_list.begin() );
}

size_t ItemInfoList::find_idx( const T_ItemID & itemId ) const
{
	struct Fn
	{
		T_ItemID itemId;
		Fn(T_ItemID itemId) : itemId(itemId) {}

		bool operator()(const ITEM_INFO& item) const		
		{
			return item.m_TItemID == itemId;
		}
	};

	List::const_iterator it = std::find_if( m_list.begin(), m_list.end(), Fn(itemId) );
	if( it == m_list.end() ) return UINT_MAX;
	return it - m_list.begin();
}

size_t ItemInfoList::find_idx_by_ware_idx( const T_ItemDBIdx & ware_idx ) const
{
	struct Fn
	{
		T_ItemDBIdx ware_idx;
		Fn(T_ItemDBIdx ware_idx) : ware_idx(ware_idx) {}

		bool operator()(const ITEM_INFO& item) const		
		{
			return item.m_TItemDBIdx == ware_idx;
		}
	};

	List::const_iterator it = std::find_if( m_list.begin(), m_list.end(), Fn(ware_idx) );
	if( it == m_list.end() ) return UINT_MAX;
	return it - m_list.begin();
}

size_t ItemInfoList::find_idx( const T_ItemID & itemId, const T_ItemDBIdx & ware_idx ) const
{
	ITEM_INFO tmp;
	tmp.m_TItemID = itemId;
	tmp.m_TItemDBIdx = ware_idx;
	return find_idx(tmp);
}

size_t ItemInfoList::find_idx( const ITEM_INFO & info ) const
{
	List::const_iterator it = std::find( m_list.begin(), m_list.end(), info );
	if( it == m_list.end() ) return UINT_MAX;
	return it - m_list.begin();
}

void ItemInfoList::remove_unshared( const T_ItemID & charaID )
{
	List::iterator It = m_list.begin();
	while( It != m_list.end() )
	{
		if( It->m_TItemID != 0 )
		{
			if( item_def::is_shared_parts_item(It->m_TItemID, charaID) == false )
			{
				It = m_list.erase(It);
				continue;
			}
		}

		++It;
	}
}

void ItemInfoList::remove_duplicate_default_item()
{
	List::iterator It = m_list.begin();
	while( It != m_list.end() )
	{
		if( It->m_TItemID != 0 )
		{
			size_t dupl_cnt = std::count(It+1, m_list.end(), *It);
			if( dupl_cnt > 0 )
			{
				if( item_def::is_default_item(It->m_TItemID) )
				{
					It = m_list.erase(It);
					continue;
				}
			}
		}

		++It;
	}
}

void ItemInfoList::default_part_id_make_to_mainchara()
{
	T_ItemID charaID = CCharaInfoContext::i()->GetMainCharaInfo()->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
	const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo( charaID );
	List::iterator It = m_list.begin();
	while( It != m_list.end() )
	{
		if( It->m_TItemID > DEFAULT_PARTS_ITEM_ID_MIN )
		{
			ITEM_TYPE part_type = (ITEM_TYPE) GET_ITEM_TYPE(It->m_TItemID);

			It->m_TItemID	= pCharaInfo->GetPartsItemID( EQUIP::ItemType2PartsType( part_type ) );
			It->m_TItemDBIdx = MAKE_DEFAULT_EQUIP_DBIDX( part_type );
		}
		++It;
	}
}

void ItemInfoList::default_skin_id_make_to_mainchara()
{
	List::iterator It = m_list.begin();
	while (It != m_list.end())
	{
		if (It->m_TItemID > DEFAULT_PARTS_ITEM_ID_MIN)
		{
			It = m_list.erase(It);
			continue;
		}
		++It;
	}
}

void ItemInfoList::default_part_id_make_to_union()
{
	//아직 생각중
}

void ItemInfoList::SearchKeyword(const WCHAR* wstr)
{
	if (wstr[0] != 0)
	{
		i3::vector<ITEM_INFO>::iterator itr = m_list.begin();
		while (itr != m_list.end())
		{
			i3::rc_wstring name;
			usf::GetShopItemName(name, *itr);
			if (i3::icontain_string(name, wstr) == -1)
			{
				itr = m_list.erase(itr);
			}
			else
				itr++;
		}
	}
}