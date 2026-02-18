#ifndef __ITEMTYPE_H
#define	__ITEMTYPE_H

#include "ItemTypeWeaponClass.h"
#include "ItemTypeCharaClass.h"
#include "GoodsType.h"

//////////////////////////////////////////////////////////////////////////////////////
// ItemID 
// 429 49 67 295 : 최대값
// 428 99 99 999 : 실허용값
// ItemID : 4byte
// 상위 1byte (FF) 
//  1개  -> 클라전용예외처리기능, 
//  10개 -> 이벤트 아이템 타입
//
// 하위 3byte(FFFFFFFF) : 실제 아이템 아이디
//  99 : ItemType : 기존의 ITEM_CLASS_TYPE + USAGE_TYPE (슬롯의 의미로 사용함)
//  99 : ItemClass : 기존의 CLASS_TYPE
// 999 : ItemIdx

// 아이템 조합 예시
//MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, WEAPON_CLASS_TYPE,    WEAPON_IDX)
//MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, CHARACTER_CLASS_TYPE, Chara_IDX)
//MAKE_ITEM_FLAG(ITEM_EVENT_TYPE, ITEM_TYPE, CASHITEM_CLASS_TYPE,  CASHITEM_GROUP_TYPE )

#define MAKE_ITEM_FLAG(event,type,class,number)			(((event)*10000000)+((type)*100000)+((class)*1000)+(number))

inline UINT32 GET_ITEM_TIME( T_ItemID ItemID) //아이템 아이디에서 날짜(시간)을 뽑아낸다.
{
	return (((ItemID) % 100000) / 1000);
}

//
//	Item class
//
// 아이템 아이디 첫번째에 들어갑니다. ( 2 )
//
// 유효 값 범위 : 0~99
// 아이템 아이디에서 저장되는 자릿수 : 
// 9900000
//~ 100000
//
//////////////////////////////////////////////////////////////////////////////////////
// 장착 검증용
//
//enum EQMIPEMENT_SLOT
//{
//	EQMIPEMENT_SLOT_PRIM			= ITEM_TYPE_PRIMARY,
//	EQMIPEMENT_SLOT_SUB				= ITEM_TYPE_SECONDARY,
//	EQMIPEMENT_SLOT_MELEE			= ITEM_TYPE_MELEE,
//	EQMIPEMENT_SLOT_THROWING1		= ITEM_TYPE_THROWING1,
//	EQMIPEMENT_SLOT_THROWING2		= ITEM_TYPE_THROWING2,
//
//	EQMIPEMENT_SLOT_CHAR			= ITEM_TYPE_CHARA,			// 캐릭터
//	EQMIPEMENT_SLOT_CHAR_HEADGEAR	= ITEM_TYPE_HEADGEAR,		// HeadGear
//	EQMIPEMENT_SLOT_CHAR_FACEGEAR	= ITEM_TYPE_FACEGEAR,		// FaceGear
//	EQMIPEMENT_SLOT_CHAR_UPPER		= ITEM_TYPE_UPPER,			// 상의
//	EQMIPEMENT_SLOT_CHAR_LOWER		= ITEM_TYPE_LOWER,			// 하의
//	EQMIPEMENT_SLOT_CHAR_GLOVE		= ITEM_TYPE_GLOVE,			// 장갑
//	EQMIPEMENT_SLOT_CHAR_BELT		= ITEM_TYPE_BELT,			// 벨트
//	EQMIPEMENT_SLOT_CHAR_HOLSTER	= ITEM_TYPE_HOLSTER,		// 권총집
//
//	EQMIPEMENT_SLOT_CHAR_ITEM		= ITEM_TYPE_ITEM,
//	EQMIPEMENT_SLOT_CHAR_DINO		= ITEM_TYPE_DINO,
//};

enum EQUIPMENT_SLOT
{
	EQUIPMENT_SLOT_NONE			= -1,

	// Weapon Slots
	EQUIPMENT_SLOT_PRIMARY		= 0,
	EQUIPMENT_SLOT_SECONDARY	,
	EQUIPMENT_SLOT_MELEE		,
	EQUIPMENT_SLOT_THROWING1	,
	EQUIPMENT_SLOT_THROWING2	,

	// Parts Slots
	EQUIPMENT_SLOT_CHARA		,
	EQUIPMENT_SLOT_HEAD			,
	EQUIPMENT_SLOT_FACE			,
	EQUIPMENT_SLOT_UPPER		,
	EQUIPMENT_SLOT_LOWER		,
	EQUIPMENT_SLOT_GLOVE		,
	EQUIPMENT_SLOT_BELT			,
	EQUIPMENT_SLOT_HOLSTER		,
	EQUIPMENT_SLOT_ITEM			,

	
	EQUIPMENT_SLOT_COUNT		,

	EQUIP_SLOT_WEAPON_COUNT		= EQUIPMENT_SLOT_THROWING2 - EQUIPMENT_SLOT_PRIMARY + 1,
	EQUIP_SLOT_PARTS_COUNT		= EQUIPMENT_SLOT_COUNT - EQUIP_SLOT_WEAPON_COUNT,
};

//////////////////////////////////////////////////////////////////////////
#endif