#ifndef __CHARACTERDEF_H__
#define __CHARACTERDEF_H__

#pragma once

// ============================================================================
// Character Equipment Definitions
// Simplified port from Common/CommonSource/CommonDef.h + CommonDef_Char.h
// ============================================================================

// Equipment slot counts (from CommonDef.h CHAR_EQUIPMENT enum)
#define CHAR_EQUIP_WEAPON_COUNT		5		// Primary, Secondary, Melee, Throwing1, Throwing2
#define CHAR_EQUIP_PARTS_COUNT		10		// Chara, Head, Face, Upper, Lower, Glove, Belt, Holster, Skin, Beret
#define CHAR_EQUIP_TOTAL_COUNT		15		// Weapon + Parts

// Weapon slots (from CHAR_EQUIPMENT_WEAPON enum)
enum CharEquipWeapon
{
	EQUIP_WEAPON_PRIMARY = 0,
	EQUIP_WEAPON_SECONDARY,
	EQUIP_WEAPON_MELEE,
	EQUIP_WEAPON_THROWING1,
	EQUIP_WEAPON_THROWING2,
};

// Parts slots (from CHAR_EQUIPMENT_PARTS enum)
enum CharEquipParts
{
	EQUIP_PARTS_CHARA = 0,		// Character body
	EQUIP_PARTS_HEAD,			// Headgear
	EQUIP_PARTS_FACE,			// Face gear
	EQUIP_PARTS_UPPER,			// Upper body
	EQUIP_PARTS_LOWER,			// Lower body
	EQUIP_PARTS_GLOVE,			// Gloves
	EQUIP_PARTS_BELT,			// Belt
	EQUIP_PARTS_HOLSTER,		// Holster
	EQUIP_PARTS_SKIN,			// Skin
	EQUIP_PARTS_BERET,			// Beret
};

// Multi-slot: max character loadout slots
#define MAX_CHARA_SLOT		5

// Multi-slot state (from CommonDef_Char.h MULTI_SLOT_STATE)
enum MultiSlotState
{
	MULTI_SLOT_NONE		= 0,
	MULTI_SLOT_WAIT		= 10,
	MULTI_SLOT_NORMAL	= 20,
	MULTI_SLOT_UNUSE	= 30,
};

// ============================================================================
// Simplified equipment data for GameSession
// ============================================================================

#pragma pack(push, 1)

// Per-character-slot equipment
struct GameCharaEquip
{
	uint32_t	ui32WeaponIds[CHAR_EQUIP_WEAPON_COUNT];		// ItemID per weapon slot
	uint32_t	ui32PartsIds[CHAR_EQUIP_PARTS_COUNT];		// ItemID per parts slot

	void Reset()
	{
		memset(ui32WeaponIds, 0, sizeof(ui32WeaponIds));
		memset(ui32PartsIds, 0, sizeof(ui32PartsIds));
	}
};

// Character slot info
struct GameCharaSlot
{
	uint8_t			ui8State;		// MultiSlotState
	uint32_t		ui32CharaId;	// Character ItemID (ITEM_TYPE_CHARA)
	GameCharaEquip	equip;			// Equipment for this slot

	void Reset()
	{
		ui8State = MULTI_SLOT_NONE;
		ui32CharaId = 0;
		equip.Reset();
	}
};

#pragma pack(pop)

#endif // __CHARACTERDEF_H__
