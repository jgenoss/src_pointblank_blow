#pragma once

#define EQUIP_LOD_SLOPE		3.5f

namespace EQUIP
{
	//	장비 부위
	enum ePART
	{
		ePART_UNKNOWN = -1,

		ePART_VEST,		// 상체
		ePART_PANT,		// 하체
		ePART_GLOVE,	// 손
		ePART_CHARA,	// 캐릭터 얼굴

		ePART_HEADGEAR,	// 머리 장비품
		ePART_FACEGEAR,	// 얼굴
		ePART_HOLSTER,	// 총집
		ePART_BELT,		// 벨트

		ePART_SKIN,		// 스킨
		ePART_BERET,	// 베레모

		ePART_COUNT
	};

	enum eSKIN
	{
		eSKIN_UNKNOWN = -1,
		eSKIN_HEADGEAR,
		eSKIN_FACEGEAR,
		eSKIN_HOLSTER,
		eSKIN_BELT,
		eSKIN_COUNT
	};

	enum eBERET
	{
		eBERET_NONE = 0,
		eBERET_TARGET,
		eBERET_REINFORCE,
		eBERET_SUPER,

		eBERET_COUNT
	};


}

#ifndef AVATAR_TOOL

namespace EQUIP
{
	// VS2015버전은 인라인 함수 구현 내부에 대한 유효성 체크가 강화된 모양.. (2015.09.10.수빈)

	//////////////////////////////////////////////////////////////////////////
	//					 버전별 베레모 타입 get 함수	                    //
	//////////////////////////////////////////////////////////////////////////
	inline
	ePART GetePartToBeret(void)
	{
		if( g_pEnvSet->IsV2Version() )
			return ePART_HEADGEAR;

		return ePART_BERET;
	}

	inline
	CHAR_EQUIPMENT_PARTS GetCommonPartToBeret(void)
	{
		if( g_pEnvSet->IsV2Version() )
			return CHAR_EQUIPMENT_PARTS_HEAD;

		return CHAR_EQUIPMENT_PARTS_BERET;
	}

	//////////////////////////////////////////////////////////////////////////

	inline CHARACTER_CLASS_TYPE ItemType2Class( ITEM_TYPE item_type )
	{
		switch( item_type )
		{
		case ITEM_TYPE_CHARA :		return CHARACTER_CLASS_CHARA_TYPE;
		case ITEM_TYPE_HEADGEAR :	return CHARACTER_CLASS_HEADGEAR;
		case ITEM_TYPE_FACEGEAR :	return CHARACTER_CLASS_FACEGEAR;
		case ITEM_TYPE_UPPER :		return CHARACTER_CLASS_UPPER;
		case ITEM_TYPE_LOWER :		return CHARACTER_CLASS_LOWER;
		case ITEM_TYPE_GLOVE :		return CHARACTER_CLASS_GLOVE;
		case ITEM_TYPE_BELT :		return CHARACTER_CLASS_BELT;
		case ITEM_TYPE_HOLSTER :	return CHARACTER_CLASS_HOLSTER;
		case ITEM_TYPE_SKIN :		return CHARACTER_CLASS_SKIN;
		case ITEM_TYPE_DINO :		return CHARACTER_CLASS_CHARA_DINO;
		case ITEM_TYPE_MISSION :	return CHARACTER_CLASS_RESERVED;
		case ITEM_TYPE_BERET :		return CHARACTER_CLASS_BERET;
		default :
			return CHARACTER_CLASS_UNKNOWN;
		}
	}

	inline CHARACTER_CLASS_TYPE	ItemID2Class( T_ItemID itemID)	
	{
		return ItemType2Class( static_cast<ITEM_TYPE>(GET_ITEM_TYPE( itemID)) );
	}

	inline SKIN_CLASS_TYPE	ItemID2SkinClass( T_ItemID itemID)
	{
#if defined( I3_DEBUG)
		I3ASSERT( GET_ITEM_TYPE( itemID) == ITEM_TYPE_SKIN);
		UINT32 rv = GET_ITEM_SUBTYPE( itemID);
		I3ASSERT( rv < SKIN_CLASS_COUNT);
		return (SKIN_CLASS_TYPE) rv;
#else
		return (SKIN_CLASS_TYPE) GET_ITEM_SUBTYPE( itemID);
#endif
	}

	inline	ePART	ItemType2PartsType( CHARACTER_CLASS_TYPE class_type )
	{
		switch( class_type )
		{
		case CHARACTER_CLASS_CHARA_TYPE	:	return ePART_CHARA;
		case CHARACTER_CLASS_HEADGEAR	:	return ePART_HEADGEAR;
		case CHARACTER_CLASS_FACEGEAR	:	return ePART_FACEGEAR;
		case CHARACTER_CLASS_UPPER		:	return ePART_VEST;
		case CHARACTER_CLASS_LOWER		:	return ePART_PANT;
		case CHARACTER_CLASS_GLOVE		:	return ePART_GLOVE;
		case CHARACTER_CLASS_BELT		:	return ePART_BELT;
		case CHARACTER_CLASS_HOLSTER	:	return ePART_HOLSTER;
		case CHARACTER_CLASS_CHARA_DINO :	return ePART_CHARA;
		case CHARACTER_CLASS_SKIN		:	return ePART_SKIN;
		case CHARACTER_CLASS_BERET		:	return ePART_BERET;
		}

		return ePART_UNKNOWN;
	}

	inline	ePART	ItemType2PartsType( ITEM_TYPE item_type )
	{
		return ItemType2PartsType( ItemType2Class(item_type) );
	}

	inline	ePART	ItemID2PartsType( T_ItemID ItemID)
	{
		return ItemType2PartsType( static_cast<ITEM_TYPE>( GET_ITEM_TYPE( ItemID) ) );
	}

	typedef UINT32 RES_ID;
	inline RES_ID	ItemID2ResID( T_ItemID ItemID)	{ return (RES_ID) GET_ITEM_NUMBER( ItemID); }


	inline
	CHAR_EQUIPMENT_PARTS	Parts2CommonParts( ePART part)
	{
		switch( part)
		{
		case ePART_VEST :		return CHAR_EQUIPMENT_PARTS_UPPER;
		case ePART_PANT :		return CHAR_EQUIPMENT_PARTS_LOWER;
		case ePART_GLOVE :		return CHAR_EQUIPMENT_PARTS_GLOVE;
		case ePART_CHARA :		return CHAR_EQUIPMENT_PARTS_CHARA;
		case ePART_HEADGEAR :	return CHAR_EQUIPMENT_PARTS_HEAD;
		case ePART_FACEGEAR :	return CHAR_EQUIPMENT_PARTS_FACE;
		case ePART_HOLSTER :	return CHAR_EQUIPMENT_PARTS_HOLSTER;
		case ePART_BELT :		return CHAR_EQUIPMENT_PARTS_BELT;
		case ePART_SKIN :		return CHAR_EQUIPMENT_PARTS_SKIN;
		case ePART_BERET:		return GetCommonPartToBeret();

		}
		I3PRINTLOG(I3LOG_FATAL,  "invalid part!!!!!!");
		return CHAR_EQUIPMENT_PARTS_NONE;
	}

	inline	
	ePART	CommonParts2eParts( CHAR_EQUIPMENT_PARTS EquipParts )
	{
		switch( EquipParts )
		{
		case CHAR_EQUIPMENT_PARTS_CHARA		: return ePART_CHARA;
		case CHAR_EQUIPMENT_PARTS_HEAD		: return ePART_HEADGEAR;
		case CHAR_EQUIPMENT_PARTS_FACE		: return ePART_FACEGEAR;
		case CHAR_EQUIPMENT_PARTS_UPPER		: return ePART_VEST;
		case CHAR_EQUIPMENT_PARTS_LOWER		: return ePART_PANT;
		case CHAR_EQUIPMENT_PARTS_GLOVE		: return ePART_GLOVE;
		case CHAR_EQUIPMENT_PARTS_BELT		: return ePART_BELT;
		case CHAR_EQUIPMENT_PARTS_HOLSTER	: return ePART_HOLSTER;
		case CHAR_EQUIPMENT_PARTS_SKIN		: return ePART_SKIN;
		case CHAR_EQUIPMENT_PARTS_BERET		: return GetePartToBeret();
		}

		return ePART_UNKNOWN;
	}

	inline	bool	Valid( T_ItemID ItemID)
	{
		return (bool) (ItemID2PartsType( ItemID) != ePART_UNKNOWN);
	}

	enum COMMAND
	{
		CMD_CREATE = 0,
		CMD_DELETE,
		CMD_DROP,
		CMD_TAKE,
		CMD_ATTACH,
		CMD_DETACH,
	};
}

#endif