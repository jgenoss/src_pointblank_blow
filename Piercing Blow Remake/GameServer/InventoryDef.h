#ifndef __INVENTORYDEF_H__
#define __INVENTORYDEF_H__

#pragma once

// ============================================================================
// Inventory Definitions
// Simplified port from CommonDef_Item.h + SIA_Define.h
// ============================================================================

#define MAX_INVEN_COUNT				450

// Item ID encoding: MAKE_ITEM_ID(type, subtype, number)
// ItemID = type * 100000 + subtype * 1000 + number
#define MAKE_ITEM_ID(type, subtype, num)	((type) * 100000 + (subtype) * 1000 + (num))
#define GET_ITEM_TYPE(itemId)				(((itemId) % 10000000) / 100000)
#define GET_ITEM_SUBTYPE(itemId)			(((itemId) % 100000) / 1000)
#define GET_ITEM_NUMBER(itemId)				((itemId) % 1000)

// Goods ID encoding
#define GET_GOODSID_FROM_ITEMID(ItemID)		((ItemID) * 100)
#define GET_ITEMID_FROM_GOODSID(GoodsID)	((GoodsID) / 100)

// Item types (from ITEM_TYPE enum in CommonDef_Item.h)
enum GameItemType
{
	ITEM_TYPE_UNKNOWN = 0,

	// Weapons
	ITEM_TYPE_PRIMARY = 1,		// Main weapon
	ITEM_TYPE_SECONDARY,		// Sub weapon
	ITEM_TYPE_MELEE,			// Melee weapon
	ITEM_TYPE_THROWING1,		// Grenade
	ITEM_TYPE_THROWING2,		// Special (smoke, flash)

	// Character parts
	ITEM_TYPE_CHARA,			// Character body (6)
	ITEM_TYPE_HEADGEAR,			// Head gear
	ITEM_TYPE_FACEGEAR,			// Face gear
	ITEM_TYPE_UPPER,			// Upper body
	ITEM_TYPE_LOWER,			// Lower body (10)
	ITEM_TYPE_GLOVE,			// Gloves
	ITEM_TYPE_BELT,				// Belt
	ITEM_TYPE_HOLSTER,			// Holster
	ITEM_TYPE_SKIN,				// Skin

	ITEM_TYPE_DINO,				// Dinosaur (15)

	// Cash items / consumables
	ITEM_TYPE_MAINTENANCE,		// Period-based consumable (16)
	ITEM_TYPE_WRAP_PERIOD,		// Wrapped period item
	ITEM_TYPE_WRAP_COUNT,		// Wrapped count item
	ITEM_TYPE_COUPON,			// Coupon (19)
	ITEM_TYPE_POINT,			// Point item (20)

	ITEM_TYPE_SALECOUPON,		// Sale coupon
	ITEM_TYPE_RSCOUPON,			// Roulette coupon

	ITEM_TYPE_INGAMEITEM,		// In-game effect item
	ITEM_TYPE_DOMIITEM,			// Domination item

	ITEM_TYPE_GACHA_TICKET,		// Gacha ticket (25)
	ITEM_TYPE_QUESTCARD,		// Quest card (26)

	ITEM_TYPE_BERET,			// Beret (27)

	ITEM_TYPE_MISSION = 50,		// Mission item (C4, etc.)
	ITEM_TYPE_TEMP,				// Rocket launcher etc.
	ITEM_TYPE_SET = 99,			// Set item
};

// Weapon classes (from WEAPON_CLASS_TYPE enum)
enum GameWeaponClass
{
	WEAPON_CLASS_UNKNOWN = 0,
	WEAPON_CLASS_KNIFE,			// Melee
	WEAPON_CLASS_HANDGUN,		// Pistol
	WEAPON_CLASS_ASSAULT,		// Assault rifle
	WEAPON_CLASS_SMG,			// SMG
	WEAPON_CLASS_SNIPER,		// Sniper rifle
	WEAPON_CLASS_SHOTGUN,		// Shotgun
	WEAPON_CLASS_THROWING_GRENADE,	// Grenade
	WEAPON_CLASS_THROWING_CONTAIN,	// Smoke/Flash
	WEAPON_CLASS_MISSION,		// Mission items
	WEAPON_CLASS_MG,			// Machine gun
};

// Item attribute type (from INVEN_BUFFER._ui8ItemType)
enum GameItemAttrType
{
	ITEM_ATTR_UNUSED = 0,		// Not in use / permanent
	ITEM_ATTR_COUNT,			// Count-based
	ITEM_ATTR_PERIOD,			// Period-based (time-limited)
};

// ============================================================================
// Inventory item structure (simplified from INVEN_BUFFER)
// ============================================================================

#pragma pack(push, 1)

// Durability constants (Phase 14C)
#define DURABILITY_MAX				100		// 100% durability
#define DURABILITY_DECREASE_PER_BATTLE	5	// -5% per battle
#define DURABILITY_BROKEN_THRESHOLD		0	// 0% = broken, cannot equip
#define DURABILITY_REPAIR_COST_PER_POINT	50	// 50 GP per durability point

struct GameInventoryItem
{
	uint32_t	ui32ItemDBIdx;		// DB unique index (T_ItemDBIdx)
	uint32_t	ui32ItemId;			// Item ID (T_ItemID)
	uint8_t		ui8ItemType;		// GameItemAttrType (0=unused/permanent, 1=count, 2=period)
	uint32_t	ui32ItemArg;		// For count: remaining count. For period: expire timestamp.
	uint8_t		ui8Durability;		// 0-100, weapon/parts durability (Phase 14C)

	void Reset()
	{
		ui32ItemDBIdx = 0;
		ui32ItemId = 0;
		ui8ItemType = 0;
		ui32ItemArg = 0;
		ui8Durability = DURABILITY_MAX;
	}

	bool IsValid() const { return ui32ItemId != 0; }
	bool IsBroken() const { return ui8Durability <= DURABILITY_BROKEN_THRESHOLD; }
	bool HasDurability() const;	// Only weapons and parts have durability
	int  GetRepairCost() const;	// GP cost to repair to full
};

#pragma pack(pop)

// Durability helpers
inline bool GameInventoryItem::HasDurability() const
{
	if (!IsValid()) return false;
	int type = GET_ITEM_TYPE(ui32ItemId);
	// Weapons and character parts have durability
	return (type >= ITEM_TYPE_PRIMARY && type <= ITEM_TYPE_THROWING2) ||
		   (type >= ITEM_TYPE_CHARA && type <= ITEM_TYPE_SKIN) ||
		   type == ITEM_TYPE_BERET;
}

inline int GameInventoryItem::GetRepairCost() const
{
	if (!HasDurability() || ui8Durability >= DURABILITY_MAX)
		return 0;
	return (DURABILITY_MAX - ui8Durability) * DURABILITY_REPAIR_COST_PER_POINT;
}

// Helper: Check if an ItemID is a weapon type
inline bool IsWeaponItem(uint32_t itemId)
{
	int type = GET_ITEM_TYPE(itemId);
	return (type >= ITEM_TYPE_PRIMARY && type <= ITEM_TYPE_THROWING2);
}

// Helper: Check if an ItemID is a character parts type
inline bool IsPartsItem(uint32_t itemId)
{
	int type = GET_ITEM_TYPE(itemId);
	return (type >= ITEM_TYPE_CHARA && type <= ITEM_TYPE_SKIN) || type == ITEM_TYPE_BERET;
}

// ============================================================================
// Cash Item Group Types (from CASHITEM_GROUP_TYPE enum)
// Used for ITEM_TYPE_MAINTENANCE items - the "number" part of ItemID
// ============================================================================

enum CashItemGroupType
{
	CASHITEM_GROUP_UNKNOWN = 0,
	CASHITEM_GROUP_XP_PLUS_110 = 1,		// 110% EXP
	CASHITEM_GROUP_XP_PLUS_130 = 2,		// 130% EXP
	CASHITEM_GROUP_XP_PLUS_150 = 3,		// 150% EXP
	CASHITEM_GROUP_PT_PLUS_130 = 4,		// 130% GP (Point)
	CASHITEM_GROUP_COLOR_CLAN = 5,
	CASHITEM_GROUP_COLOR_NICK = 6,
	CASHITEM_GROUP_SHORT_RESPAWN = 7,
	CASHITEM_GROUP_AMMO_UP = 8,
	CASHITEM_GROUP_XP_PLUS_200 = 37,	// 200% EXP
	CASHITEM_GROUP_PT_PLUS_200 = 38,	// 200% GP (Point)
};

// Helper: Get EXP multiplier from CASHITEM_GROUP (returns 100-based, 100 = 1x)
inline uint16_t GetExpMultiplierFromCashGroup(int groupType)
{
	switch (groupType)
	{
	case CASHITEM_GROUP_XP_PLUS_110:	return 110;
	case CASHITEM_GROUP_XP_PLUS_130:	return 130;
	case CASHITEM_GROUP_XP_PLUS_150:	return 150;
	case CASHITEM_GROUP_XP_PLUS_200:	return 200;
	default: return 100;
	}
}

// Helper: Get GP multiplier from CASHITEM_GROUP (returns 100-based, 100 = 1x)
inline uint16_t GetPointMultiplierFromCashGroup(int groupType)
{
	switch (groupType)
	{
	case CASHITEM_GROUP_PT_PLUS_130:	return 130;
	case CASHITEM_GROUP_PT_PLUS_200:	return 200;
	default: return 100;
	}
}

#endif // __INVENTORYDEF_H__
