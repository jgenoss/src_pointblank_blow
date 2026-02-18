#pragma once

namespace INVEN
{
	enum DB_TYPE
	{
		DB_INVALID = -1,

		DB_WEAPON,
		DB_CHARA,
		DB_PARTS,
		DB_SKIN,
		DB_CASHITEM,
		DB_COUPON,

		DB_MAX_COUNT
	};

	inline
	DB_TYPE	ItemType2DBType( ITEM_TYPE item)
	{
		if (g_pEnvSet->IsV2Version())
		{
			switch (item)
			{
			case ITEM_TYPE_PRIMARY:
			case ITEM_TYPE_SECONDARY:
			case ITEM_TYPE_MELEE:
			case ITEM_TYPE_THROWING1:
			case ITEM_TYPE_THROWING2:		return DB_WEAPON;
			case ITEM_TYPE_CHARA:			return DB_CHARA;
			case ITEM_TYPE_HEADGEAR:
			case ITEM_TYPE_FACEGEAR:
			case ITEM_TYPE_UPPER:
			case ITEM_TYPE_LOWER:
			case ITEM_TYPE_GLOVE:
			case ITEM_TYPE_BELT:
			case ITEM_TYPE_HOLSTER:
			case ITEM_TYPE_BERET:			return DB_PARTS;
			case ITEM_TYPE_SKIN:			return DB_SKIN;
			case ITEM_TYPE_DINO:
			case ITEM_TYPE_MAINTENANCE:
			case ITEM_TYPE_WRAP_PERIOD:
			case ITEM_TYPE_WRAP_COUNT:
			case ITEM_TYPE_POINT:
			case ITEM_TYPE_INGAMEITEM:
			case ITEM_TYPE_DOMIITEM:
			case ITEM_TYPE_TEMP:			return DB_CASHITEM;
			case ITEM_TYPE_COUPON:
			case ITEM_TYPE_SALECOUPON:
			case ITEM_TYPE_GACHA_TICKET:	return DB_COUPON;
			}
		}
		else
		{
			switch (item)
			{
			case ITEM_TYPE_PRIMARY:
			case ITEM_TYPE_SECONDARY:
			case ITEM_TYPE_MELEE:
			case ITEM_TYPE_THROWING1:
			case ITEM_TYPE_THROWING2:		return DB_WEAPON;
			case ITEM_TYPE_HEADGEAR:
			case ITEM_TYPE_FACEGEAR:
			case ITEM_TYPE_BERET:
			case ITEM_TYPE_CHARA:			return DB_CHARA;
			case ITEM_TYPE_UPPER:
			case ITEM_TYPE_LOWER:
			case ITEM_TYPE_GLOVE:
			case ITEM_TYPE_BELT:
			case ITEM_TYPE_HOLSTER:			return DB_PARTS;
			case ITEM_TYPE_SKIN:			return DB_SKIN;
			case ITEM_TYPE_DINO:
			case ITEM_TYPE_MAINTENANCE:
			case ITEM_TYPE_WRAP_PERIOD:
			case ITEM_TYPE_WRAP_COUNT:
			case ITEM_TYPE_POINT:
			case ITEM_TYPE_INGAMEITEM:
			case ITEM_TYPE_DOMIITEM:
			case ITEM_TYPE_TEMP:			return DB_CASHITEM;
			case ITEM_TYPE_COUPON:
			case ITEM_TYPE_SALECOUPON:
			case ITEM_TYPE_GACHA_TICKET:	return DB_COUPON;
			}
		}


		return DB_INVALID;
	}

	inline
	DB_TYPE	ItemID2DBType( T_ItemID itemID)
	{
		return ItemType2DBType( (ITEM_TYPE) GET_ITEM_TYPE( itemID));
	}
};


