#ifndef __WEAPONSYSTEM_H__
#define __WEAPONSYSTEM_H__

#pragma once
#include "BattleDef.h"

// ============================================================================
// Weapon System - Port of original CWeapon, WeaponSlot, WeaponInfo, WeaponTable
// ============================================================================

// Weapon slot types (from original WEAPON_SLOT_TYPE)
enum WeaponSlotType
{
	WEAPON_SLOT_PRIMARY = 0,	// Main weapon (rifle, SMG, etc.)
	WEAPON_SLOT_SECONDARY,		// Pistol / sidearm
	WEAPON_SLOT_MELEE,			// Knife
	WEAPON_SLOT_THROWING1,		// Grenade slot 1
	WEAPON_SLOT_THROWING2,		// Grenade slot 2 (smoke, flash)
	WEAPON_SLOT_COUNT,
};

// Weapon class types (from GET_ITEM_SUBTYPE)
enum WeaponClassType
{
	WEAPON_CLASS_ASSAULT_RIFLE = 0,
	WEAPON_CLASS_SUB_MACHINE_GUN,
	WEAPON_CLASS_SNIPER_RIFLE,
	WEAPON_CLASS_SHOTGUN,
	WEAPON_CLASS_MACHINE_GUN,
	WEAPON_CLASS_ROCKET_LAUNCHER,
	WEAPON_CLASS_PISTOL,
	WEAPON_CLASS_MELEE,
	WEAPON_CLASS_THROWING_GRENADE,
	WEAPON_CLASS_THROWING_SMOKE,
	WEAPON_CLASS_THROWING_FLASH,
	WEAPON_CLASS_THROWING_HEAL,		// Medical kit
	WEAPON_CLASS_THROWING_ATTACK,	// WP Smoke / special throwable
	WEAPON_CLASS_COUNT,
};

// Attack types (from WEAPON::ATTACK_TYPE)
enum AttackType
{
	ATTACK_TYPE_NONE = 0,
	ATTACK_TYPE_L1,		// Left hand attack 1
	ATTACK_TYPE_L2,
	ATTACK_TYPE_L3,
	ATTACK_TYPE_R1,		// Right hand attack 1
	ATTACK_TYPE_R2,
	ATTACK_TYPE_R3,
	ATTACK_TYPE_R4,
	ATTACK_TYPE_MAX,
};

// Hit body parts (from CHARA_HIT_PART)
enum CharaHitPart
{
	CHARA_HIT_HEAD = 0,
	CHARA_HIT_CHEST,
	CHARA_HIT_STOMACH,
	CHARA_HIT_LEFT_ARM,
	CHARA_HIT_RIGHT_ARM,
	CHARA_HIT_LEFT_LEG,
	CHARA_HIT_RIGHT_LEG,
	CHARA_HIT_UNKNOWN,
	CHARA_HIT_MAXCOUNT = 12,	// Network index max
};

// Bullet shot type
enum BulletShotType
{
	BULLET_SHOT_INSTANT = 0,		// Hitscan
	BULLET_SHOT_DROPPING,			// Projectile (grenades, rockets)
};

// Item ID helpers (from original macros)
#define GET_ITEM_TYPE(id)			(((id) >> 24) & 0xFF)
#define GET_ITEM_SUBTYPE(id)		(((id) >> 16) & 0xFF)
#define GET_ITEM_NUMBER(id)			((id) & 0xFFFF)
#define MAKE_ITEM_ID(type, sub, num) (((type) << 24) | ((sub) << 16) | (num))

// Weapon damage info per weapon (port of WeaponInfo)
struct WeaponDamageInfo
{
	uint32_t	ui32ItemID;
	uint8_t		ui8SlotType;			// WeaponSlotType
	uint8_t		ui8WeaponClass;			// WeaponClassType

	// Damage per attack type
	float		fDamage[ATTACK_TYPE_MAX];
	float		fMaxDamage;				// Max random damage
	float		fCriticalDamageRate;	// Critical multiplier (e.g., 1.5)

	// Fire parameters
	float		fFireDelay;				// Seconds between shots
	float		fRange;					// Max effective range (cm)
	float		fExplosionRange;		// Grenade explosion radius
	float		fDPDRate;				// Damage per distance rate

	// Bullet info
	uint16_t	ui16LoadBullet;			// Bullets per magazine
	uint16_t	ui16MaxBullet;			// Max reserve bullets
	uint8_t		ui8FireBulletCount;		// Bullets per shot (shotgun = 8+)
	uint8_t		ui8FireDecBulletCount;	// Ammo consumed per shot

	// Movement
	float		fMoveSpeed;				// Speed modifier while equipped

	// Recoil
	float		fRecoilHorzAngle;
	float		fRecoilVertAngle;
	float		fDeviation;				// Accuracy deviation

	// Type flags
	bool		bIsLauncher;			// Is throwable/launcher
	bool		bIsIndicator;			// Has indicator
	uint8_t		ui8BulletType;			// BulletShotType

	WeaponDamageInfo()
	{
		memset(this, 0, sizeof(*this));
		fCriticalDamageRate = 1.5f;
	}

	bool IsValid() const { return ui32ItemID != 0 && fRange > 0.0f; }

	// Calculate damage with random variation
	int CalculateDamage(AttackType eAttack, bool bCritical, float fDamagePlus = 0.0f) const;

	// Get damage rate by hit body part
	static float GetDamageRateByHitPart(CharaHitPart ePart);
};

// Weapon instance - a weapon held by a character
struct WeaponInstance
{
	uint32_t	ui32ItemID;
	uint16_t	ui16LoadedBullets;		// Current magazine
	uint16_t	ui16ReserveBullets;		// Reserve ammo
	uint8_t		ui8SlotType;

	WeaponInstance()
		: ui32ItemID(0)
		, ui16LoadedBullets(0)
		, ui16ReserveBullets(0)
		, ui8SlotType(0)
	{}

	void Reset()
	{
		ui32ItemID = 0;
		ui16LoadedBullets = 0;
		ui16ReserveBullets = 0;
		ui8SlotType = 0;
	}

	bool IsValid() const { return ui32ItemID != 0; }

	// Consume bullets, returns false if empty
	bool DecreaseBullet(uint16_t count = 1);
};

// ============================================================================
// Weapon Table - Singleton lookup for weapon data
// ============================================================================

#define MAX_WEAPONS_PER_CLASS	128
#define MAX_WEAPON_COUNT		(WEAPON_CLASS_COUNT * MAX_WEAPONS_PER_CLASS)

class WeaponTable
{
public:
	static WeaponTable& GetInstance();

	// Initialize from ConfigXML data
	void Initialize();

	// Lookup weapon info by ItemID
	const WeaponDamageInfo* GetWeaponInfo(uint32_t ui32ItemID) const;

	// Lookup by class and number
	const WeaponDamageInfo* GetWeaponByClassNumber(uint8_t ui8Class, uint16_t ui16Number) const;

	// Get weapon range
	float GetWeaponRange(uint32_t ui32ItemID) const;

	// Get weapon slot type
	static WeaponSlotType GetSlotFromItemID(uint32_t ui32ItemID);

	// Get weapon class
	static WeaponClassType GetClassFromItemID(uint32_t ui32ItemID);

	// Is this a throwable weapon?
	static bool IsThrowableWeapon(uint32_t ui32ItemID);

	int GetWeaponCount() const { return m_i32WeaponCount; }

private:
	WeaponTable();

	WeaponDamageInfo	m_Weapons[MAX_WEAPON_COUNT];
	int					m_i32WeaponCount;

	// Index by class for fast lookup
	WeaponDamageInfo*	m_pByClass[WEAPON_CLASS_COUNT];
	int					m_i32ClassCount[WEAPON_CLASS_COUNT];
};

// ============================================================================
// Thrown Weapon (grenade) tracking
// ============================================================================

#define MAX_THROW_WEAPONS		32

struct ThrownWeapon
{
	uint32_t	ui32OwnerSlot;		// Who threw it
	uint32_t	ui32ItemID;			// Weapon item ID
	float		fPos[3];			// Current/last position
	float		fVelocity[3];		// Initial velocity
	float		fThrowTime;			// Game time when thrown
	bool		bActive;			// Still in flight
	bool		bExploded;			// Has exploded
	uint16_t	ui16NetIdx;			// Network index

	ThrownWeapon() { Reset(); }

	void Reset()
	{
		ui32OwnerSlot = 0;
		ui32ItemID = 0;
		memset(fPos, 0, sizeof(fPos));
		memset(fVelocity, 0, sizeof(fVelocity));
		fThrowTime = 0.0f;
		bActive = false;
		bExploded = false;
		ui16NetIdx = 0;
	}
};

class ThrowWeaponMgr
{
public:
	ThrowWeaponMgr();
	~ThrowWeaponMgr();

	void Reset();

	// Add a thrown weapon
	bool AddThrow(uint32_t ui32SlotIdx, uint32_t ui32ItemID,
				  const float* fPos, const float* fVelocity, float fGameTime);

	// Mark as exploded
	void OnExplode(uint16_t ui16NetIdx);

	// Get active thrown weapons for state sync
	int GetActiveCount() const;
	const ThrownWeapon* GetThrown(int idx) const;

private:
	ThrownWeapon	m_Throws[MAX_THROW_WEAPONS];
	int				m_i32Count;
};

// ============================================================================
// Dropped Weapon Manager
// ============================================================================

#define MAX_DROPPED_WEAPONS			32
#define DROPPED_WEAPON_DISAPPEAR_TIME	15.0f

struct DroppedWeapon
{
	uint32_t	ui32ItemID;
	float		fPos[3];
	uint16_t	ui16LoadedBullets;
	uint16_t	ui16ReserveBullets;
	uint8_t		ui8DropperSlot;		// Who dropped it
	float		fDropTime;			// Game time when dropped
	bool		bActive;
	uint16_t	ui16NetIdx;

	DroppedWeapon() { Reset(); }

	void Reset()
	{
		ui32ItemID = 0;
		memset(fPos, 0, sizeof(fPos));
		ui16LoadedBullets = 0;
		ui16ReserveBullets = 0;
		ui8DropperSlot = 0xFF;
		fDropTime = 0.0f;
		bActive = false;
		ui16NetIdx = 0;
	}

	// Has expired?
	bool IsExpired(float fGameTime) const
	{
		return bActive && (fGameTime - fDropTime > DROPPED_WEAPON_DISAPPEAR_TIME);
	}
};

class DroppedWeaponMgr
{
public:
	DroppedWeaponMgr();
	~DroppedWeaponMgr();

	void Reset(float fGameTime);

	// Drop a weapon at position
	int AddDroppedWeapon(uint32_t ui32SlotIdx, uint32_t ui32ItemID,
						 const float* fPos, uint16_t loaded, uint16_t reserve,
						 float fGameTime);

	// Pick up a weapon
	bool PickUpWeapon(int i32DropIdx, uint32_t* pOutItemID,
					  uint16_t* pOutLoaded, uint16_t* pOutReserve,
					  float fGameTime);

	// Update: expire old weapons
	void Update(float fGameTime);

	int GetActiveCount() const;
	const DroppedWeapon* GetDropped(int idx) const;

private:
	DroppedWeapon	m_Drops[MAX_DROPPED_WEAPONS];
	int				m_i32NextIdx;
	bool			m_bWeaponDestroy;	// Enable time-based expiry
};

#endif // __WEAPONSYSTEM_H__
