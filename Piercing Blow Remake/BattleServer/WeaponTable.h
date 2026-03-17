#ifndef __WEAPONTABLE_AC_H__
#define __WEAPONTABLE_AC_H__

#pragma once
#include "WeaponSystem.h"

// ============================================================================
// WeaponStats - Anti-cheat weapon validation table
// Provides fast lookup of weapon parameters for server-side validation.
// Values sourced from original Piercing Blow weapon data.
// Used by AntiCheatManager subsystems (DamageValidator, RateOfFireChecker, etc.)
// ============================================================================

// Weapon class type for anti-cheat categorization
enum ACWeaponClass
{
	AC_WEAPON_ASSAULT = 0,
	AC_WEAPON_SMG,
	AC_WEAPON_SNIPER,
	AC_WEAPON_SHOTGUN,
	AC_WEAPON_PISTOL,
	AC_WEAPON_MELEE,
	AC_WEAPON_GRENADE,
	AC_WEAPON_MG,
	AC_WEAPON_ROCKET,
	AC_WEAPON_CLASS_COUNT,
};

// Per-weapon validation stats
struct WeaponStats
{
	uint32_t	weaponId;
	float		maxDamage;				// Maximum single-hit damage (before headshot multiplier)
	float		minDamage;				// Minimum single-hit damage
	float		maxRange;				// Maximum effective range in game units (cm)
	int			rateOfFireMs;			// Minimum milliseconds between shots
	int			magazineSize;			// Maximum rounds per magazine
	int			maxReserveAmmo;			// Maximum reserve ammunition
	uint8_t		weaponClass;			// ACWeaponClass
	uint8_t		bulletsPerShot;			// Pellets per trigger pull (shotgun = 8)
	uint8_t		burstCount;				// Burst fire count (0 = full auto / semi)
	uint8_t		pad;

	// Derived limits for anti-cheat
	float		maxDamageWithCrit;		// maxDamage * headshot multiplier
	float		maxDPS;					// Theoretical max DPS (for kill rate checks)
	int			maxTotalAmmo;			// magazineSize + maxReserveAmmo

	WeaponStats()
	{
		memset(this, 0, sizeof(*this));
	}

	bool IsValid() const { return weaponId != 0 && maxDamage > 0.0f; }
};

// ============================================================================
// ACWeaponTable - Singleton anti-cheat weapon stats lookup
// Separate from WeaponTable (which handles game logic) to keep AC data isolated
// ============================================================================

#define AC_MAX_WEAPONS		256

class ACWeaponTable
{
public:
	static ACWeaponTable&	GetInstance();

	// Initialize from hardcoded PB weapon data + ConfigXML overrides
	void		Initialize();

	// Lookup by exact item ID
	const WeaponStats*	GetStats(uint32_t weaponId) const;

	// Lookup by weapon class - returns class defaults for unknown weapons
	const WeaponStats*	GetClassDefaults(uint8_t weaponClass) const;

	// Resolve a weapon ID to its ACWeaponClass
	static ACWeaponClass	ResolveWeaponClass(uint32_t weaponId);

	// Get headshot damage multiplier for weapon class
	static float			GetHeadshotMultiplier(uint8_t weaponClass);

	// Get body part damage multiplier (mirrors WeaponDamageInfo::GetDamageRateByHitPart)
	static float			GetMaxBodyPartMultiplier();

	// Get max theoretical damage for a weapon (maxDamage * headshot * bodypart)
	float		GetMaxPossibleDamage(uint32_t weaponId) const;

	// Validate a damage value: returns true if plausible
	bool		IsDamageValid(uint32_t weaponId, int damage, bool bHeadshot) const;

	// Validate fire rate: returns true if enough time passed
	bool		IsFireRateValid(uint32_t weaponId, int elapsedMs) const;

	// Validate ammo: returns true if ammo count is within bounds
	bool		IsAmmoValid(uint32_t weaponId, int loaded, int reserve) const;

	int			GetWeaponCount() const { return m_i32Count; }

private:
	ACWeaponTable();

	// Add a weapon entry
	void		AddWeapon(uint32_t weaponId, float minDmg, float maxDmg,
						  float range, int rofMs, int magSize, int reserveAmmo,
						  uint8_t wclass, uint8_t pelletsPerShot = 1, uint8_t burstCount = 0);

	// Populate hardcoded defaults for all major PB weapons
	void		PopulateDefaults();

	// Merge with ConfigXML data if available
	void		MergeConfigXML();

	// Compute derived fields
	void		ComputeDerived(WeaponStats& ws);

	WeaponStats		m_Weapons[AC_MAX_WEAPONS];
	int				m_i32Count;

	// Per-class default entries (for unknown weapon IDs within a known class)
	WeaponStats		m_ClassDefaults[AC_WEAPON_CLASS_COUNT];
};

#endif // __WEAPONTABLE_AC_H__
