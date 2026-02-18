#pragma once

namespace ValidCheck {
	namespace Item {
		namespace Parts {
			inline bool		isCharaItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_CHARA)?true:false; }
			inline bool		isHeadgearItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_HEADGEAR)?true:false; }
			inline bool		isFacegearItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_FACEGEAR)?true:false; }
			inline bool		isUppergearItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_UPPER)?true:false; }
			inline bool		isLowergearItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_LOWER)?true:false; }
			inline bool		isGloveItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_GLOVE)?true:false; }
			inline bool		isBeltItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_BELT)?true:false; }
			inline bool		isHolsterItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_HOLSTER)?true:false; }
			inline bool		isSkinItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_SKIN)?true:false; }
			inline bool		isBeretItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_BERET)?true:false; }

			inline bool		isPartsItemID( CHAR_EQUIPMENT_PARTS part, T_ItemID id) {
				switch( part)
				{
				case CHAR_EQUIPMENT_PARTS_CHARA :		return isCharaItemID( id);
				case CHAR_EQUIPMENT_PARTS_HEAD :		return isHeadgearItemID( id);
				case CHAR_EQUIPMENT_PARTS_FACE :		return isFacegearItemID( id);
				case CHAR_EQUIPMENT_PARTS_UPPER :		return isUppergearItemID( id);
				case CHAR_EQUIPMENT_PARTS_LOWER :		return isLowergearItemID( id);
				case CHAR_EQUIPMENT_PARTS_GLOVE :		return isGloveItemID( id);
				case CHAR_EQUIPMENT_PARTS_BELT :		return isBeltItemID( id);
				case CHAR_EQUIPMENT_PARTS_HOLSTER :		return isHolsterItemID( id);
				case CHAR_EQUIPMENT_PARTS_SKIN :		return isSkinItemID( id);
				case CHAR_EQUIPMENT_PARTS_BERET :		return isBeretItemID( id);
				default :								return false;
				}
			}
		};

		inline bool		isDinoItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_DINO)?true:false; }

		namespace Weapon {
			inline bool		isPrimaryItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_PRIMARY)?true:false; }
			inline bool		isSecondaryItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_SECONDARY)?true:false; }
			inline bool		isMeleeItemID( T_ItemID id)		{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_MELEE)?true:false; }
			inline bool		isThrowing1ItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_THROWING1)?true:false; }
			inline bool		isThrowing2ItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_THROWING2)?true:false; }
			inline bool		isMissionItemID( T_ItemID id)	{	return (GET_ITEM_TYPE( id) == ITEM_TYPE_MISSION)?true:false; }

			inline bool		isWeaponItemID( WEAPON_SLOT_TYPE type, T_ItemID id) {
				switch(type)
				{
				case WEAPON_SLOT_PRIMARY :		return isPrimaryItemID( id);
				case WEAPON_SLOT_SECONDARY :	return isSecondaryItemID( id);
				case WEAPON_SLOT_MELEE :		return isMeleeItemID( id);
				case WEAPON_SLOT_THROWING1 :	return isThrowing1ItemID( id);
				case WEAPON_SLOT_THROWING2 :	return isThrowing2ItemID( id);
				case WEAPON_SLOT_MISSION :		return isMissionItemID( id);
				default :	return false;
				}
			}
		};
	}

	namespace Skill {
		inline bool		isPrimary( CHARA_CLASS val)			{	return (val == CHARA_CLASS_NONE || val < CHARA_CLASS_COUNT)?true:false; }
		inline bool		isCommon( COMMON_SKILL_TYPE val)	{	return (val == COMMON_SKILL_NONE || val < MAX_COMMON_SKILL)?true:false; }
		inline bool		isMain( MAIN_SKILL_TYPE val)		{	return (val == MAIN_SKILL_NONE || val < MAX_MAIN_SKILL)?true:false; }
		inline bool		isAssist( ASSIST_SKILL_TYPE val)	{	return (val == ASSIST_SKILL_NONE || val < MAX_ASSIST_SKILL)?true:false; }
	};
};
