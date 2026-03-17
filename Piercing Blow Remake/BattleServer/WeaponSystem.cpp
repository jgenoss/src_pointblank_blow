#include "pch.h"
#include "WeaponSystem.h"
#include "ConfigXML.h"
#include <cmath>
#include <cstdlib>

// Thread-safe random number generation
// Uses Windows CryptGenRandom-backed xorshift for fast, thread-safe RNG
// (replaces non-thread-safe rand())
static uint32_t s_tls_rng_state = 0;

static uint32_t ThreadSafeRand()
{
	// Per-thread state using thread ID as seed
	if (s_tls_rng_state == 0)
		s_tls_rng_state = GetCurrentThreadId() ^ GetTickCount();

	// xorshift32
	uint32_t x = s_tls_rng_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	s_tls_rng_state = x;
	return x;
}

// ============================================================================
// WeaponDamageInfo
// ============================================================================

int WeaponDamageInfo::CalculateDamage(AttackType eAttack, bool bCritical, float fDamagePlus) const
{
	if (eAttack <= ATTACK_TYPE_NONE || eAttack >= ATTACK_TYPE_MAX)
		return 0;

	float baseDamage = fDamage[eAttack];

	// Random damage variation: (MaxDamage - BaseDamage) * rand[0,1]
	// Uses thread-safe RNG (not rand())
	if (fMaxDamage > baseDamage + 0.001f)
	{
		float randFactor = (float)(ThreadSafeRand() % 1000) / 1000.0f;
		float randomDamage = (fMaxDamage - baseDamage) * randFactor;
		baseDamage += randomDamage;
	}

	baseDamage += fDamagePlus;

	if (bCritical)
		baseDamage *= fCriticalDamageRate;

	return (int)baseDamage;
}

float WeaponDamageInfo::GetDamageRateByHitPart(CharaHitPart ePart)
{
	switch (ePart)
	{
	case CHARA_HIT_HEAD:		return 4.0f;	// Headshot = 4x damage
	case CHARA_HIT_CHEST:		return 1.0f;
	case CHARA_HIT_STOMACH:		return 0.9f;
	case CHARA_HIT_LEFT_ARM:
	case CHARA_HIT_RIGHT_ARM:	return 0.7f;
	case CHARA_HIT_LEFT_LEG:
	case CHARA_HIT_RIGHT_LEG:	return 0.6f;
	default:					return 1.0f;
	}
}

// ============================================================================
// WeaponInstance
// ============================================================================

bool WeaponInstance::DecreaseBullet(uint16_t count)
{
	if (ui16LoadedBullets >= count)
	{
		ui16LoadedBullets -= count;
		return true;
	}
	return false;
}

// ============================================================================
// WeaponTable (Singleton)
// ============================================================================

static WeaponTable s_WeaponTableInstance;

WeaponTable::WeaponTable()
	: m_i32WeaponCount(0)
{
	memset(m_pByClass, 0, sizeof(m_pByClass));
	memset(m_i32ClassCount, 0, sizeof(m_i32ClassCount));
}

WeaponTable& WeaponTable::GetInstance()
{
	return s_WeaponTableInstance;
}

void WeaponTable::Initialize()
{
	m_i32WeaponCount = 0;
	memset(m_pByClass, 0, sizeof(m_pByClass));
	memset(m_i32ClassCount, 0, sizeof(m_i32ClassCount));

	// Load from ConfigXML if available
	if (g_pConfigXML)
	{
		int xmlCount = g_pConfigXML->GetWeaponCount();
		for (int i = 0; i < xmlCount && m_i32WeaponCount < MAX_WEAPON_COUNT; i++)
		{
			const ConfigWeapon* pCfgWeapon = g_pConfigXML->GetWeapon(i);
			if (!pCfgWeapon)
				continue;

			WeaponDamageInfo& info = m_Weapons[m_i32WeaponCount];
			info.ui32ItemID = pCfgWeapon->ui32ItemID;
			info.fRange = pCfgWeapon->fRange;

			// Set damage based on range (simplified from original XML parsing)
			for (int a = ATTACK_TYPE_L1; a < ATTACK_TYPE_MAX; a++)
				info.fDamage[a] = pCfgWeapon->fDamage;
			info.fMaxDamage = pCfgWeapon->fDamage * 1.2f;
			info.fFireDelay = pCfgWeapon->fFireRate > 0.0f ? 1.0f / pCfgWeapon->fFireRate : 0.1f;
			info.fCriticalDamageRate = 1.5f;
			info.ui8WeaponClass = (uint8_t)GET_ITEM_SUBTYPE(pCfgWeapon->ui32ItemID);
			info.ui8SlotType = (uint8_t)GetSlotFromItemID(pCfgWeapon->ui32ItemID);
			info.ui16LoadBullet = 30;
			info.ui16MaxBullet = 120;
			info.ui8FireBulletCount = 1;
			info.ui8FireDecBulletCount = 1;
			info.fMoveSpeed = 1.0f;

			// Weapon class specific defaults
			uint8_t wclass = info.ui8WeaponClass;
			if (wclass == WEAPON_CLASS_SHOTGUN)
			{
				info.ui8FireBulletCount = 8;
				info.ui16LoadBullet = 7;
				info.ui16MaxBullet = 28;
			}
			else if (wclass == WEAPON_CLASS_SNIPER_RIFLE)
			{
				info.ui16LoadBullet = 5;
				info.ui16MaxBullet = 20;
				info.fCriticalDamageRate = 2.0f;
			}
			else if (wclass == WEAPON_CLASS_MELEE)
			{
				info.ui16LoadBullet = 0;
				info.ui16MaxBullet = 0;
			}
			else if (wclass == WEAPON_CLASS_THROWING_GRENADE ||
					 wclass == WEAPON_CLASS_THROWING_SMOKE ||
					 wclass == WEAPON_CLASS_THROWING_FLASH)
			{
				info.bIsLauncher = true;
				info.ui8BulletType = BULLET_SHOT_DROPPING;
				info.fExplosionRange = 500.0f;
				info.ui16LoadBullet = 1;
				info.ui16MaxBullet = 2;
			}
			else if (wclass == WEAPON_CLASS_ROCKET_LAUNCHER)
			{
				info.bIsLauncher = true;
				info.ui8BulletType = BULLET_SHOT_DROPPING;
				info.fExplosionRange = 600.0f;
				info.ui16LoadBullet = 1;
				info.ui16MaxBullet = 3;
			}
			else if (wclass == WEAPON_CLASS_PISTOL)
			{
				info.ui16LoadBullet = 12;
				info.ui16MaxBullet = 48;
			}

			m_i32WeaponCount++;
		}
	}

	// Add hardcoded defaults if no ConfigXML data
	if (m_i32WeaponCount == 0)
	{
		auto addDefault = [this](uint32_t id, float dmg, float range, float fireDelay,
								 uint16_t loadBullet, uint16_t maxBullet) {
			if (m_i32WeaponCount >= MAX_WEAPON_COUNT) return;
			WeaponDamageInfo& info = m_Weapons[m_i32WeaponCount++];
			info.ui32ItemID = id;
			info.ui8SlotType = (uint8_t)GetSlotFromItemID(id);
			info.ui8WeaponClass = (uint8_t)GET_ITEM_SUBTYPE(id);
			for (int a = ATTACK_TYPE_L1; a < ATTACK_TYPE_MAX; a++)
				info.fDamage[a] = dmg;
			info.fMaxDamage = dmg * 1.2f;
			info.fRange = range;
			info.fFireDelay = fireDelay;
			info.fCriticalDamageRate = 1.5f;
			info.ui16LoadBullet = loadBullet;
			info.ui16MaxBullet = maxBullet;
			info.ui8FireBulletCount = 1;
			info.ui8FireDecBulletCount = 1;
			info.fMoveSpeed = 1.0f;
		};

		// Default weapons: MAKE_ITEM_ID(type, subtype, number)
		addDefault(MAKE_ITEM_ID(1, WEAPON_CLASS_ASSAULT_RIFLE, 1), 35.0f, 5000.0f, 0.09f, 30, 120);
		addDefault(MAKE_ITEM_ID(1, WEAPON_CLASS_SUB_MACHINE_GUN, 1), 28.0f, 3500.0f, 0.07f, 30, 120);
		addDefault(MAKE_ITEM_ID(1, WEAPON_CLASS_SNIPER_RIFLE, 1), 90.0f, 10000.0f, 1.2f, 5, 20);
		addDefault(MAKE_ITEM_ID(1, WEAPON_CLASS_SHOTGUN, 1), 15.0f, 1500.0f, 0.8f, 7, 28);
		addDefault(MAKE_ITEM_ID(1, WEAPON_CLASS_MACHINE_GUN, 1), 32.0f, 4500.0f, 0.08f, 100, 200);
		addDefault(MAKE_ITEM_ID(2, WEAPON_CLASS_PISTOL, 1), 25.0f, 3000.0f, 0.15f, 12, 48);
		addDefault(MAKE_ITEM_ID(3, WEAPON_CLASS_MELEE, 1), 50.0f, 200.0f, 0.5f, 0, 0);
	}

	// Build class index
	for (int i = 0; i < m_i32WeaponCount; i++)
	{
		uint8_t wclass = m_Weapons[i].ui8WeaponClass;
		if (wclass < WEAPON_CLASS_COUNT)
		{
			if (m_i32ClassCount[wclass] == 0)
				m_pByClass[wclass] = &m_Weapons[i];
			m_i32ClassCount[wclass]++;
		}
	}

	printf("[WeaponTable] Initialized with %d weapons\n", m_i32WeaponCount);
}

const WeaponDamageInfo* WeaponTable::GetWeaponInfo(uint32_t ui32ItemID) const
{
	for (int i = 0; i < m_i32WeaponCount; i++)
	{
		if (m_Weapons[i].ui32ItemID == ui32ItemID)
			return &m_Weapons[i];
	}
	return nullptr;
}

const WeaponDamageInfo* WeaponTable::GetWeaponByClassNumber(uint8_t ui8Class, uint16_t ui16Number) const
{
	if (ui8Class >= WEAPON_CLASS_COUNT)
		return nullptr;

	if (!m_pByClass[ui8Class] || ui16Number >= (uint16_t)m_i32ClassCount[ui8Class])
		return nullptr;

	return &m_pByClass[ui8Class][ui16Number];
}

float WeaponTable::GetWeaponRange(uint32_t ui32ItemID) const
{
	const WeaponDamageInfo* pInfo = GetWeaponInfo(ui32ItemID);
	if (!pInfo) return 0.0f;

	// Throwables use explosion range
	if (pInfo->bIsLauncher)
		return pInfo->fExplosionRange;

	return pInfo->fRange;
}

WeaponSlotType WeaponTable::GetSlotFromItemID(uint32_t ui32ItemID)
{
	uint8_t type = (uint8_t)GET_ITEM_TYPE(ui32ItemID);
	switch (type)
	{
	case 1: return WEAPON_SLOT_PRIMARY;		// Primary (rifles, SMGs, etc.)
	case 2: return WEAPON_SLOT_SECONDARY;	// Pistol
	case 3: return WEAPON_SLOT_MELEE;		// Knife
	case 4: return WEAPON_SLOT_THROWING1;	// Grenade
	case 5: return WEAPON_SLOT_THROWING2;	// Smoke/flash
	default: return WEAPON_SLOT_PRIMARY;
	}
}

WeaponClassType WeaponTable::GetClassFromItemID(uint32_t ui32ItemID)
{
	uint8_t sub = (uint8_t)GET_ITEM_SUBTYPE(ui32ItemID);
	if (sub >= WEAPON_CLASS_COUNT)
		return WEAPON_CLASS_ASSAULT_RIFLE;
	return (WeaponClassType)sub;
}

bool WeaponTable::IsThrowableWeapon(uint32_t ui32ItemID)
{
	WeaponSlotType slot = GetSlotFromItemID(ui32ItemID);
	return (slot == WEAPON_SLOT_THROWING1 || slot == WEAPON_SLOT_THROWING2);
}

// ============================================================================
// ThrowWeaponMgr
// ============================================================================

ThrowWeaponMgr::ThrowWeaponMgr()
	: m_i32Count(0)
{
}

ThrowWeaponMgr::~ThrowWeaponMgr()
{
}

void ThrowWeaponMgr::Reset()
{
	for (int i = 0; i < MAX_THROW_WEAPONS; i++)
		m_Throws[i].Reset();
	m_i32Count = 0;
}

bool ThrowWeaponMgr::AddThrow(uint32_t ui32SlotIdx, uint32_t ui32ItemID,
							  const float* fPos, const float* fVelocity, float fGameTime)
{
	// Find free slot
	for (int i = 0; i < MAX_THROW_WEAPONS; i++)
	{
		if (!m_Throws[i].bActive)
		{
			m_Throws[i].ui32OwnerSlot = ui32SlotIdx;
			m_Throws[i].ui32ItemID = ui32ItemID;
			memcpy(m_Throws[i].fPos, fPos, sizeof(float) * 3);
			if (fVelocity)
				memcpy(m_Throws[i].fVelocity, fVelocity, sizeof(float) * 3);
			m_Throws[i].fThrowTime = fGameTime;
			m_Throws[i].bActive = true;
			m_Throws[i].bExploded = false;
			m_Throws[i].ui16NetIdx = (uint16_t)i;
			m_i32Count++;
			return true;
		}
	}
	return false;
}

void ThrowWeaponMgr::OnExplode(uint16_t ui16NetIdx)
{
	if (ui16NetIdx < MAX_THROW_WEAPONS && m_Throws[ui16NetIdx].bActive)
	{
		m_Throws[ui16NetIdx].bExploded = true;
		m_Throws[ui16NetIdx].bActive = false;
		m_i32Count--;
	}
}

int ThrowWeaponMgr::GetActiveCount() const
{
	return m_i32Count;
}

const ThrownWeapon* ThrowWeaponMgr::GetThrown(int idx) const
{
	if (idx < 0 || idx >= MAX_THROW_WEAPONS)
		return nullptr;
	return &m_Throws[idx];
}

// ============================================================================
// DroppedWeaponMgr
// ============================================================================

DroppedWeaponMgr::DroppedWeaponMgr()
	: m_i32NextIdx(0)
	, m_bWeaponDestroy(true)
{
}

DroppedWeaponMgr::~DroppedWeaponMgr()
{
}

void DroppedWeaponMgr::Reset(float fGameTime)
{
	for (int i = 0; i < MAX_DROPPED_WEAPONS; i++)
		m_Drops[i].Reset();
	m_i32NextIdx = 0;
}

int DroppedWeaponMgr::AddDroppedWeapon(uint32_t ui32SlotIdx, uint32_t ui32ItemID,
										const float* fPos, uint16_t loaded, uint16_t reserve,
										float fGameTime)
{
	// Find free slot or use circular next index
	int idx = -1;
	for (int i = 0; i < MAX_DROPPED_WEAPONS; i++)
	{
		int checkIdx = (m_i32NextIdx + i) % MAX_DROPPED_WEAPONS;
		if (!m_Drops[checkIdx].bActive)
		{
			idx = checkIdx;
			break;
		}
	}

	if (idx < 0)
	{
		// All full - overwrite oldest (circular)
		idx = m_i32NextIdx;
	}

	m_Drops[idx].ui32ItemID = ui32ItemID;
	memcpy(m_Drops[idx].fPos, fPos, sizeof(float) * 3);
	m_Drops[idx].ui16LoadedBullets = loaded;
	m_Drops[idx].ui16ReserveBullets = reserve;
	m_Drops[idx].ui8DropperSlot = (uint8_t)ui32SlotIdx;
	m_Drops[idx].fDropTime = fGameTime;
	m_Drops[idx].bActive = true;
	m_Drops[idx].ui16NetIdx = (uint16_t)idx;

	m_i32NextIdx = (idx + 1) % MAX_DROPPED_WEAPONS;
	return idx;
}

bool DroppedWeaponMgr::PickUpWeapon(int i32DropIdx, uint32_t* pOutItemID,
									 uint16_t* pOutLoaded, uint16_t* pOutReserve,
									 float fGameTime)
{
	if (i32DropIdx < 0 || i32DropIdx >= MAX_DROPPED_WEAPONS)
		return false;

	DroppedWeapon& drop = m_Drops[i32DropIdx];
	if (!drop.bActive)
		return false;

	if (drop.IsExpired(fGameTime))
	{
		drop.Reset();
		return false;
	}

	if (pOutItemID) *pOutItemID = drop.ui32ItemID;
	if (pOutLoaded) *pOutLoaded = drop.ui16LoadedBullets;
	if (pOutReserve) *pOutReserve = drop.ui16ReserveBullets;

	drop.Reset();
	return true;
}

void DroppedWeaponMgr::Update(float fGameTime)
{
	if (!m_bWeaponDestroy)
		return;

	for (int i = 0; i < MAX_DROPPED_WEAPONS; i++)
	{
		if (m_Drops[i].bActive && m_Drops[i].IsExpired(fGameTime))
		{
			m_Drops[i].Reset();
		}
	}
}

int DroppedWeaponMgr::GetActiveCount() const
{
	int count = 0;
	for (int i = 0; i < MAX_DROPPED_WEAPONS; i++)
	{
		if (m_Drops[i].bActive)
			count++;
	}
	return count;
}

const DroppedWeapon* DroppedWeaponMgr::GetDropped(int idx) const
{
	if (idx < 0 || idx >= MAX_DROPPED_WEAPONS)
		return nullptr;
	return &m_Drops[idx];
}
