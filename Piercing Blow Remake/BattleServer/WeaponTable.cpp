#include "pch.h"
#include "WeaponTable.h"
#include "ConfigXML.h"
#include <cmath>

// ============================================================================
// ACWeaponTable - Anti-cheat weapon stats
// ============================================================================

static ACWeaponTable s_ACWeaponTableInstance;

ACWeaponTable::ACWeaponTable()
	: m_i32Count(0)
{
	memset(m_ClassDefaults, 0, sizeof(m_ClassDefaults));
}

ACWeaponTable& ACWeaponTable::GetInstance()
{
	return s_ACWeaponTableInstance;
}

void ACWeaponTable::Initialize()
{
	m_i32Count = 0;
	memset(m_ClassDefaults, 0, sizeof(m_ClassDefaults));

	PopulateDefaults();
	MergeConfigXML();

	// Compute derived fields for all entries
	for (int i = 0; i < m_i32Count; i++)
		ComputeDerived(m_Weapons[i]);

	for (int i = 0; i < AC_WEAPON_CLASS_COUNT; i++)
	{
		if (m_ClassDefaults[i].IsValid())
			ComputeDerived(m_ClassDefaults[i]);
	}

	printf("[ACWeaponTable] Initialized with %d weapons\n", m_i32Count);
}

// ============================================================================
// Hardcoded weapon database - values from original Piercing Blow
// ============================================================================
void ACWeaponTable::PopulateDefaults()
{
	// ============================
	// ASSAULT RIFLES (class 0)
	// ============================
	// MAKE_ITEM_ID(type=1, subtype=WEAPON_CLASS_ASSAULT_RIFLE, number)

	// AK-47 (iconic PB weapon)
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 1),
		28.0f, 38.0f, 5500.0f, 90, 30, 120, AC_WEAPON_ASSAULT);

	// M4A1
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 2),
		26.0f, 36.0f, 5200.0f, 85, 30, 120, AC_WEAPON_ASSAULT);

	// AUG A3
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 3),
		27.0f, 37.0f, 5400.0f, 88, 30, 120, AC_WEAPON_ASSAULT);

	// FAMAS
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 4),
		25.0f, 35.0f, 5000.0f, 75, 25, 100, AC_WEAPON_ASSAULT, 1, 3);

	// SG556
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 5),
		29.0f, 39.0f, 5600.0f, 92, 30, 120, AC_WEAPON_ASSAULT);

	// TAR-21
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 6),
		27.0f, 37.0f, 5300.0f, 86, 30, 120, AC_WEAPON_ASSAULT);

	// G36C
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 7),
		26.0f, 36.0f, 5100.0f, 84, 30, 120, AC_WEAPON_ASSAULT);

	// SCAR-H
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 8),
		30.0f, 40.0f, 5800.0f, 95, 20, 80, AC_WEAPON_ASSAULT);

	// AN-94
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 9),
		28.0f, 38.0f, 5400.0f, 80, 30, 120, AC_WEAPON_ASSAULT, 1, 2);

	// K2
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 10),
		27.0f, 37.0f, 5200.0f, 87, 30, 120, AC_WEAPON_ASSAULT);

	// ============================
	// SUBMACHINE GUNS (class 1)
	// ============================
	// P90
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 1),
		20.0f, 30.0f, 3500.0f, 65, 50, 150, AC_WEAPON_SMG);

	// MP5
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 2),
		22.0f, 32.0f, 3200.0f, 70, 30, 120, AC_WEAPON_SMG);

	// UMP45
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 3),
		24.0f, 34.0f, 3000.0f, 80, 25, 100, AC_WEAPON_SMG);

	// Kriss Super V
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 4),
		21.0f, 31.0f, 3400.0f, 55, 33, 132, AC_WEAPON_SMG);

	// MP7
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 5),
		19.0f, 29.0f, 3300.0f, 60, 40, 120, AC_WEAPON_SMG);

	// MAC-10
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 6),
		18.0f, 28.0f, 2800.0f, 50, 32, 128, AC_WEAPON_SMG);

	// PP-19 Bizon
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 7),
		20.0f, 30.0f, 3100.0f, 68, 64, 128, AC_WEAPON_SMG);

	// ============================
	// SNIPER RIFLES (class 2)
	// ============================
	// AWP/AWM
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 1),
		80.0f, 100.0f, 12000.0f, 1500, 5, 20, AC_WEAPON_SNIPER);

	// PSG-1
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 2),
		65.0f, 85.0f, 10000.0f, 300, 5, 20, AC_WEAPON_SNIPER);

	// Dragunov SVD
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 3),
		60.0f, 80.0f, 9500.0f, 280, 10, 30, AC_WEAPON_SNIPER);

	// M24
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 4),
		78.0f, 98.0f, 11500.0f, 1400, 5, 20, AC_WEAPON_SNIPER);

	// Barrett M82
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 5),
		90.0f, 110.0f, 13000.0f, 1600, 5, 15, AC_WEAPON_SNIPER);

	// MSG90
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 6),
		62.0f, 82.0f, 9800.0f, 290, 10, 30, AC_WEAPON_SNIPER);

	// ============================
	// SHOTGUNS (class 3)
	// ============================
	// Benelli M4
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SHOTGUN, 1),
		10.0f, 18.0f, 1500.0f, 800, 7, 28, AC_WEAPON_SHOTGUN, 8);

	// SPAS-12
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SHOTGUN, 2),
		11.0f, 19.0f, 1600.0f, 850, 8, 32, AC_WEAPON_SHOTGUN, 8);

	// Remington 870
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SHOTGUN, 3),
		12.0f, 20.0f, 1400.0f, 900, 6, 24, AC_WEAPON_SHOTGUN, 8);

	// USAS-12
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SHOTGUN, 4),
		9.0f, 16.0f, 1300.0f, 400, 10, 30, AC_WEAPON_SHOTGUN, 8);

	// Saiga-12
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_SHOTGUN, 5),
		10.0f, 17.0f, 1400.0f, 450, 8, 24, AC_WEAPON_SHOTGUN, 8);

	// ============================
	// MACHINE GUNS (class 4)
	// ============================
	// M60
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_MACHINE_GUN, 1),
		30.0f, 40.0f, 5000.0f, 80, 100, 200, AC_WEAPON_MG);

	// M249
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_MACHINE_GUN, 2),
		28.0f, 38.0f, 4800.0f, 75, 100, 200, AC_WEAPON_MG);

	// MG3
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_MACHINE_GUN, 3),
		26.0f, 36.0f, 4500.0f, 60, 100, 200, AC_WEAPON_MG);

	// PKM
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_MACHINE_GUN, 4),
		31.0f, 41.0f, 5200.0f, 85, 100, 200, AC_WEAPON_MG);

	// ============================
	// ROCKET LAUNCHERS (class 5)
	// ============================
	// RPG-7
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ROCKET_LAUNCHER, 1),
		80.0f, 120.0f, 6000.0f, 3000, 1, 3, AC_WEAPON_ROCKET);

	// AT4
	AddWeapon(MAKE_ITEM_ID(1, WEAPON_CLASS_ROCKET_LAUNCHER, 2),
		90.0f, 130.0f, 5500.0f, 3500, 1, 2, AC_WEAPON_ROCKET);

	// ============================
	// PISTOLS (class 6, type=2)
	// ============================
	// Glock 18
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 1),
		18.0f, 28.0f, 2500.0f, 120, 20, 60, AC_WEAPON_PISTOL);

	// Desert Eagle
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 2),
		32.0f, 42.0f, 3000.0f, 250, 7, 28, AC_WEAPON_PISTOL);

	// USP
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 3),
		22.0f, 32.0f, 2800.0f, 150, 12, 48, AC_WEAPON_PISTOL);

	// M1911
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 4),
		25.0f, 35.0f, 2600.0f, 180, 8, 32, AC_WEAPON_PISTOL);

	// Five-Seven
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 5),
		20.0f, 30.0f, 2700.0f, 140, 20, 60, AC_WEAPON_PISTOL);

	// Beretta M9
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 6),
		19.0f, 29.0f, 2500.0f, 130, 15, 60, AC_WEAPON_PISTOL);

	// CZ75
	AddWeapon(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 7),
		21.0f, 31.0f, 2600.0f, 135, 12, 48, AC_WEAPON_PISTOL);

	// ============================
	// MELEE (class 7, type=3)
	// ============================
	// Default knife
	AddWeapon(MAKE_ITEM_ID(3, WEAPON_CLASS_MELEE, 1),
		40.0f, 55.0f, 200.0f, 400, 0, 0, AC_WEAPON_MELEE);

	// Kukri
	AddWeapon(MAKE_ITEM_ID(3, WEAPON_CLASS_MELEE, 2),
		45.0f, 60.0f, 220.0f, 450, 0, 0, AC_WEAPON_MELEE);

	// Axe
	AddWeapon(MAKE_ITEM_ID(3, WEAPON_CLASS_MELEE, 3),
		50.0f, 70.0f, 200.0f, 550, 0, 0, AC_WEAPON_MELEE);

	// Tactical knife
	AddWeapon(MAKE_ITEM_ID(3, WEAPON_CLASS_MELEE, 4),
		35.0f, 50.0f, 200.0f, 350, 0, 0, AC_WEAPON_MELEE);

	// ============================
	// GRENADES (class 8, type=4)
	// ============================
	// Frag grenade
	AddWeapon(MAKE_ITEM_ID(4, WEAPON_CLASS_THROWING_GRENADE, 1),
		50.0f, 100.0f, 500.0f, 1000, 1, 2, AC_WEAPON_GRENADE);

	// Smoke grenade (no damage)
	AddWeapon(MAKE_ITEM_ID(5, WEAPON_CLASS_THROWING_SMOKE, 1),
		0.0f, 0.0f, 500.0f, 1000, 1, 1, AC_WEAPON_GRENADE);

	// Flash grenade (no damage)
	AddWeapon(MAKE_ITEM_ID(5, WEAPON_CLASS_THROWING_FLASH, 1),
		0.0f, 0.0f, 500.0f, 1000, 1, 1, AC_WEAPON_GRENADE);

	// ============================
	// Build class defaults (most permissive per class, for unknown weapon IDs)
	// ============================

	// Assault rifle defaults
	m_ClassDefaults[AC_WEAPON_ASSAULT].weaponId = 0xFFFF0000;
	m_ClassDefaults[AC_WEAPON_ASSAULT].minDamage = 20.0f;
	m_ClassDefaults[AC_WEAPON_ASSAULT].maxDamage = 45.0f;
	m_ClassDefaults[AC_WEAPON_ASSAULT].maxRange = 6000.0f;
	m_ClassDefaults[AC_WEAPON_ASSAULT].rateOfFireMs = 70;
	m_ClassDefaults[AC_WEAPON_ASSAULT].magazineSize = 40;
	m_ClassDefaults[AC_WEAPON_ASSAULT].maxReserveAmmo = 160;
	m_ClassDefaults[AC_WEAPON_ASSAULT].weaponClass = AC_WEAPON_ASSAULT;
	m_ClassDefaults[AC_WEAPON_ASSAULT].bulletsPerShot = 1;

	// SMG defaults
	m_ClassDefaults[AC_WEAPON_SMG].weaponId = 0xFFFF0001;
	m_ClassDefaults[AC_WEAPON_SMG].minDamage = 15.0f;
	m_ClassDefaults[AC_WEAPON_SMG].maxDamage = 38.0f;
	m_ClassDefaults[AC_WEAPON_SMG].maxRange = 4000.0f;
	m_ClassDefaults[AC_WEAPON_SMG].rateOfFireMs = 45;
	m_ClassDefaults[AC_WEAPON_SMG].magazineSize = 64;
	m_ClassDefaults[AC_WEAPON_SMG].maxReserveAmmo = 200;
	m_ClassDefaults[AC_WEAPON_SMG].weaponClass = AC_WEAPON_SMG;
	m_ClassDefaults[AC_WEAPON_SMG].bulletsPerShot = 1;

	// Sniper defaults
	m_ClassDefaults[AC_WEAPON_SNIPER].weaponId = 0xFFFF0002;
	m_ClassDefaults[AC_WEAPON_SNIPER].minDamage = 55.0f;
	m_ClassDefaults[AC_WEAPON_SNIPER].maxDamage = 120.0f;
	m_ClassDefaults[AC_WEAPON_SNIPER].maxRange = 14000.0f;
	m_ClassDefaults[AC_WEAPON_SNIPER].rateOfFireMs = 250;
	m_ClassDefaults[AC_WEAPON_SNIPER].magazineSize = 10;
	m_ClassDefaults[AC_WEAPON_SNIPER].maxReserveAmmo = 40;
	m_ClassDefaults[AC_WEAPON_SNIPER].weaponClass = AC_WEAPON_SNIPER;
	m_ClassDefaults[AC_WEAPON_SNIPER].bulletsPerShot = 1;

	// Shotgun defaults
	m_ClassDefaults[AC_WEAPON_SHOTGUN].weaponId = 0xFFFF0003;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].minDamage = 8.0f;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].maxDamage = 22.0f;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].maxRange = 1800.0f;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].rateOfFireMs = 350;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].magazineSize = 10;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].maxReserveAmmo = 40;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].weaponClass = AC_WEAPON_SHOTGUN;
	m_ClassDefaults[AC_WEAPON_SHOTGUN].bulletsPerShot = 8;

	// Pistol defaults
	m_ClassDefaults[AC_WEAPON_PISTOL].weaponId = 0xFFFF0004;
	m_ClassDefaults[AC_WEAPON_PISTOL].minDamage = 15.0f;
	m_ClassDefaults[AC_WEAPON_PISTOL].maxDamage = 45.0f;
	m_ClassDefaults[AC_WEAPON_PISTOL].maxRange = 3500.0f;
	m_ClassDefaults[AC_WEAPON_PISTOL].rateOfFireMs = 100;
	m_ClassDefaults[AC_WEAPON_PISTOL].magazineSize = 20;
	m_ClassDefaults[AC_WEAPON_PISTOL].maxReserveAmmo = 80;
	m_ClassDefaults[AC_WEAPON_PISTOL].weaponClass = AC_WEAPON_PISTOL;
	m_ClassDefaults[AC_WEAPON_PISTOL].bulletsPerShot = 1;

	// Melee defaults
	m_ClassDefaults[AC_WEAPON_MELEE].weaponId = 0xFFFF0005;
	m_ClassDefaults[AC_WEAPON_MELEE].minDamage = 30.0f;
	m_ClassDefaults[AC_WEAPON_MELEE].maxDamage = 75.0f;
	m_ClassDefaults[AC_WEAPON_MELEE].maxRange = 250.0f;
	m_ClassDefaults[AC_WEAPON_MELEE].rateOfFireMs = 300;
	m_ClassDefaults[AC_WEAPON_MELEE].magazineSize = 0;
	m_ClassDefaults[AC_WEAPON_MELEE].maxReserveAmmo = 0;
	m_ClassDefaults[AC_WEAPON_MELEE].weaponClass = AC_WEAPON_MELEE;
	m_ClassDefaults[AC_WEAPON_MELEE].bulletsPerShot = 0;

	// Grenade defaults
	m_ClassDefaults[AC_WEAPON_GRENADE].weaponId = 0xFFFF0006;
	m_ClassDefaults[AC_WEAPON_GRENADE].minDamage = 0.0f;
	m_ClassDefaults[AC_WEAPON_GRENADE].maxDamage = 110.0f;
	m_ClassDefaults[AC_WEAPON_GRENADE].maxRange = 600.0f;
	m_ClassDefaults[AC_WEAPON_GRENADE].rateOfFireMs = 800;
	m_ClassDefaults[AC_WEAPON_GRENADE].magazineSize = 1;
	m_ClassDefaults[AC_WEAPON_GRENADE].maxReserveAmmo = 3;
	m_ClassDefaults[AC_WEAPON_GRENADE].weaponClass = AC_WEAPON_GRENADE;
	m_ClassDefaults[AC_WEAPON_GRENADE].bulletsPerShot = 0;

	// MG defaults
	m_ClassDefaults[AC_WEAPON_MG].weaponId = 0xFFFF0007;
	m_ClassDefaults[AC_WEAPON_MG].minDamage = 22.0f;
	m_ClassDefaults[AC_WEAPON_MG].maxDamage = 45.0f;
	m_ClassDefaults[AC_WEAPON_MG].maxRange = 5500.0f;
	m_ClassDefaults[AC_WEAPON_MG].rateOfFireMs = 55;
	m_ClassDefaults[AC_WEAPON_MG].magazineSize = 150;
	m_ClassDefaults[AC_WEAPON_MG].maxReserveAmmo = 300;
	m_ClassDefaults[AC_WEAPON_MG].weaponClass = AC_WEAPON_MG;
	m_ClassDefaults[AC_WEAPON_MG].bulletsPerShot = 1;

	// Rocket defaults
	m_ClassDefaults[AC_WEAPON_ROCKET].weaponId = 0xFFFF0008;
	m_ClassDefaults[AC_WEAPON_ROCKET].minDamage = 70.0f;
	m_ClassDefaults[AC_WEAPON_ROCKET].maxDamage = 140.0f;
	m_ClassDefaults[AC_WEAPON_ROCKET].maxRange = 7000.0f;
	m_ClassDefaults[AC_WEAPON_ROCKET].rateOfFireMs = 2500;
	m_ClassDefaults[AC_WEAPON_ROCKET].magazineSize = 1;
	m_ClassDefaults[AC_WEAPON_ROCKET].maxReserveAmmo = 4;
	m_ClassDefaults[AC_WEAPON_ROCKET].weaponClass = AC_WEAPON_ROCKET;
	m_ClassDefaults[AC_WEAPON_ROCKET].bulletsPerShot = 1;
}

void ACWeaponTable::MergeConfigXML()
{
	if (!g_pConfigXML || !g_pConfigXML->IsInitialized())
		return;

	int xmlCount = g_pConfigXML->GetWeaponCount();
	for (int i = 0; i < xmlCount; i++)
	{
		const WeaponConfigData* pCfg = g_pConfigXML->GetWeaponConfig(0);
		if (!pCfg)
			continue;

		// Try to find existing entry
		for (int j = 0; j < m_i32Count; j++)
		{
			if (m_Weapons[j].weaponId == pCfg->ui32ItemID)
			{
				// XML damage overrides hardcoded if higher (more permissive)
				float xmlMaxDmg = pCfg->fDamage * 1.3f;	// 30% tolerance over XML base
				if (xmlMaxDmg > m_Weapons[j].maxDamage)
					m_Weapons[j].maxDamage = xmlMaxDmg;

				// XML range overrides if higher
				float xmlRange = pCfg->fRange * 1.15f;		// 15% tolerance
				if (xmlRange > m_Weapons[j].maxRange)
					m_Weapons[j].maxRange = xmlRange;

				// XML fire rate
				if (pCfg->fFireRate > 0.0f)
				{
					int xmlRofMs = (int)(1000.0f / pCfg->fFireRate);
					// Use the faster fire rate (more permissive)
					int tolerantRof = (int)(xmlRofMs * 0.8f); // 20% tolerance
					if (tolerantRof < m_Weapons[j].rateOfFireMs)
						m_Weapons[j].rateOfFireMs = tolerantRof;
				}
				break;
			}
		}
	}
}

void ACWeaponTable::AddWeapon(uint32_t weaponId, float minDmg, float maxDmg,
							   float range, int rofMs, int magSize, int reserveAmmo,
							   uint8_t wclass, uint8_t pelletsPerShot, uint8_t burstCount)
{
	if (m_i32Count >= AC_MAX_WEAPONS)
		return;

	WeaponStats& ws = m_Weapons[m_i32Count++];
	ws.weaponId = weaponId;
	ws.minDamage = minDmg;
	ws.maxDamage = maxDmg;
	ws.maxRange = range;
	ws.rateOfFireMs = rofMs;
	ws.magazineSize = magSize;
	ws.maxReserveAmmo = reserveAmmo;
	ws.weaponClass = wclass;
	ws.bulletsPerShot = pelletsPerShot;
	ws.burstCount = burstCount;
}

void ACWeaponTable::ComputeDerived(WeaponStats& ws)
{
	float hsMultiplier = GetHeadshotMultiplier(ws.weaponClass);
	float maxPartMultiplier = GetMaxBodyPartMultiplier();

	// Max damage with crit = maxDamage * headshot multiplier * body part multiplier
	ws.maxDamageWithCrit = ws.maxDamage * hsMultiplier * maxPartMultiplier;

	// For shotguns: total damage is per-pellet * pellet count
	float effectiveMaxDmg = ws.maxDamage;
	if (ws.bulletsPerShot > 1)
		effectiveMaxDmg *= ws.bulletsPerShot;

	// Max DPS = (effectiveMaxDmg * hsMultiplier) / (rateOfFireMs / 1000)
	if (ws.rateOfFireMs > 0)
		ws.maxDPS = (effectiveMaxDmg * hsMultiplier) / ((float)ws.rateOfFireMs / 1000.0f);
	else
		ws.maxDPS = effectiveMaxDmg * hsMultiplier * 10.0f;	// Melee fallback

	ws.maxTotalAmmo = ws.magazineSize + ws.maxReserveAmmo;
}

// ============================================================================
// Lookups
// ============================================================================

const WeaponStats* ACWeaponTable::GetStats(uint32_t weaponId) const
{
	for (int i = 0; i < m_i32Count; i++)
	{
		if (m_Weapons[i].weaponId == weaponId)
			return &m_Weapons[i];
	}
	return nullptr;
}

const WeaponStats* ACWeaponTable::GetClassDefaults(uint8_t weaponClass) const
{
	if (weaponClass >= AC_WEAPON_CLASS_COUNT)
		return nullptr;

	if (m_ClassDefaults[weaponClass].IsValid())
		return &m_ClassDefaults[weaponClass];

	return nullptr;
}

ACWeaponClass ACWeaponTable::ResolveWeaponClass(uint32_t weaponId)
{
	uint8_t sub = (uint8_t)GET_ITEM_SUBTYPE(weaponId);
	uint8_t type = (uint8_t)GET_ITEM_TYPE(weaponId);

	// Type-based overrides
	if (type == 3)	return AC_WEAPON_MELEE;
	if (type == 4)	return AC_WEAPON_GRENADE;
	if (type == 5)	return AC_WEAPON_GRENADE;

	// Subtype-based
	switch (sub)
	{
	case WEAPON_CLASS_ASSAULT_RIFLE:	return AC_WEAPON_ASSAULT;
	case WEAPON_CLASS_SUB_MACHINE_GUN:	return AC_WEAPON_SMG;
	case WEAPON_CLASS_SNIPER_RIFLE:		return AC_WEAPON_SNIPER;
	case WEAPON_CLASS_SHOTGUN:			return AC_WEAPON_SHOTGUN;
	case WEAPON_CLASS_MACHINE_GUN:		return AC_WEAPON_MG;
	case WEAPON_CLASS_ROCKET_LAUNCHER:	return AC_WEAPON_ROCKET;
	case WEAPON_CLASS_PISTOL:			return AC_WEAPON_PISTOL;
	case WEAPON_CLASS_MELEE:			return AC_WEAPON_MELEE;
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_SMOKE:
	case WEAPON_CLASS_THROWING_FLASH:
	case WEAPON_CLASS_THROWING_HEAL:
	case WEAPON_CLASS_THROWING_ATTACK:	return AC_WEAPON_GRENADE;
	default:							return AC_WEAPON_ASSAULT;
	}
}

float ACWeaponTable::GetHeadshotMultiplier(uint8_t weaponClass)
{
	switch (weaponClass)
	{
	case AC_WEAPON_SNIPER:		return 2.0f;	// Snipers have 2x headshot (on top of 4x body part)
	case AC_WEAPON_SHOTGUN:		return 1.2f;	// Shotguns lower HS multiplier
	case AC_WEAPON_MELEE:		return 1.5f;	// Melee backstab/headhit
	case AC_WEAPON_GRENADE:		return 1.0f;	// No headshot for explosives
	case AC_WEAPON_ROCKET:		return 1.0f;	// No headshot for explosives
	default:					return 1.5f;	// Standard weapons 1.5x crit
	}
}

float ACWeaponTable::GetMaxBodyPartMultiplier()
{
	// Head = 4x (from WeaponDamageInfo::GetDamageRateByHitPart)
	return 4.0f;
}

float ACWeaponTable::GetMaxPossibleDamage(uint32_t weaponId) const
{
	const WeaponStats* pStats = GetStats(weaponId);
	if (!pStats)
	{
		ACWeaponClass wclass = ResolveWeaponClass(weaponId);
		pStats = GetClassDefaults((uint8_t)wclass);
	}
	if (!pStats)
		return 1000.0f;	// Unknown weapon - very permissive fallback

	float maxDmg = pStats->maxDamageWithCrit;
	if (pStats->bulletsPerShot > 1)
		maxDmg *= pStats->bulletsPerShot;

	// Add 20% tolerance for equipment bonuses, buffs, etc.
	return maxDmg * 1.2f;
}

bool ACWeaponTable::IsDamageValid(uint32_t weaponId, int damage, bool bHeadshot) const
{
	if (damage <= 0)
		return true;	// Zero/negative damage is always valid (miss, etc.)

	float maxAllowed = GetMaxPossibleDamage(weaponId);

	// Without headshot, max is lower
	if (!bHeadshot)
	{
		const WeaponStats* pStats = GetStats(weaponId);
		if (!pStats)
		{
			ACWeaponClass wclass = ResolveWeaponClass(weaponId);
			pStats = GetClassDefaults((uint8_t)wclass);
		}
		if (pStats)
		{
			// Body shot max = maxDamage * body multiplier (chest = 1.0)
			// Allow stomach at 0.9 to be the reference, but arms/legs are lower
			// Use chest (1.0) as reference for non-headshot max
			maxAllowed = pStats->maxDamage * 1.2f;	// 20% tolerance
			if (pStats->bulletsPerShot > 1)
				maxAllowed *= pStats->bulletsPerShot;
		}
	}

	return (float)damage <= maxAllowed;
}

bool ACWeaponTable::IsFireRateValid(uint32_t weaponId, int elapsedMs) const
{
	const WeaponStats* pStats = GetStats(weaponId);
	if (!pStats)
	{
		ACWeaponClass wclass = ResolveWeaponClass(weaponId);
		pStats = GetClassDefaults((uint8_t)wclass);
	}
	if (!pStats)
		return true;	// Unknown weapon - allow

	// Apply 30% tolerance for network jitter
	int minAllowed = (int)(pStats->rateOfFireMs * 0.7f);

	return elapsedMs >= minAllowed;
}

bool ACWeaponTable::IsAmmoValid(uint32_t weaponId, int loaded, int reserve) const
{
	const WeaponStats* pStats = GetStats(weaponId);
	if (!pStats)
	{
		ACWeaponClass wclass = ResolveWeaponClass(weaponId);
		pStats = GetClassDefaults((uint8_t)wclass);
	}
	if (!pStats)
		return true;

	// Melee has no ammo
	if (pStats->weaponClass == AC_WEAPON_MELEE)
		return true;

	// Allow 10% over magazine for reload timing quirks
	int maxLoaded = (int)(pStats->magazineSize * 1.1f) + 1;
	int maxReserve = (int)(pStats->maxReserveAmmo * 1.1f) + 1;

	if (loaded > maxLoaded)
		return false;
	if (reserve > maxReserve)
		return false;

	return true;
}
