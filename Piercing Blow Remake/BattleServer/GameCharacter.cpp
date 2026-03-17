#include "pch.h"
#include "GameCharacter.h"
#include <cmath>

GameCharacter::GameCharacter()
{
	Reset();
}

GameCharacter::~GameCharacter()
{
}

void GameCharacter::Reset()
{
	m_eCharClass = CHARA_CLASS_ASSAULT;
	m_bAlive = false;
	m_bOnLoad = false;
	m_i16HP = 0;
	m_i16MaxHP = 100;
	m_i16StoreHP = 0;
	m_bStoredHP = false;

	memset(m_fPos, 0, sizeof(m_fPos));
	memset(m_fRot, 0, sizeof(m_fRot));
	m_fPosRecvTime = 0.0f;
	m_ui16OnLoadIndex = 0;

	for (int i = 0; i < WEAPON_SLOT_COUNT; i++)
		m_Weapons[i].Reset();
	memset(m_ui32WeaponRespawn, 0, sizeof(m_ui32WeaponRespawn));

	m_fLastFireTime = 0.0f;
	m_ui32LastFireItemID = 0;
	m_eLastFireAttackType = ATTACK_TYPE_NONE;
	m_ui32FireCount = 0;
	m_fFireCheckTime = 0.0f;

	m_SpeedState.Reset();

	memset(m_aui32PartsID, 0, sizeof(m_aui32PartsID));

	m_i32HitCount = 0;
	m_i32CriticalHitCount = 0;

	m_fLastClientTime = 0.0f;
	m_fLastDediTime = 0.0f;
	m_fLatencyTime = 0.0f;
	m_fAverageLatency = 0.0f;
	m_ui32AverageCount = 0;
}

void GameCharacter::ResetBattleInfo()
{
	m_bAlive = false;
	m_bOnLoad = false;
	m_i16HP = m_i16MaxHP;
	m_bStoredHP = false;
	m_ui16OnLoadIndex = 0;

	m_fLastFireTime = 0.0f;
	m_ui32FireCount = 0;
	m_fFireCheckTime = 0.0f;

	m_SpeedState.Reset();

	m_i32HitCount = 0;
	m_i32CriticalHitCount = 0;
}

int32_t GameCharacter::AddHP(int16_t inc)
{
	m_i16HP += inc;
	if (m_i16HP > m_i16MaxHP)
		m_i16HP = m_i16MaxHP;
	return m_i16HP;
}

void GameCharacter::SetPosition(float x, float y, float z)
{
	m_fPos[0] = x;
	m_fPos[1] = y;
	m_fPos[2] = z;
}

void GameCharacter::SetRotation(float rx, float ry, float rz)
{
	m_fRot[0] = rx;
	m_fRot[1] = ry;
	m_fRot[2] = rz;
}

float GameCharacter::CalcSpeed(const float* pos0, const float* pos1, float fElapsed) const
{
	if (fElapsed < 0.001f)
		return 0.0f;

	float dx = pos1[0] - pos0[0];
	float dy = pos1[1] - pos0[1];
	float dz = pos1[2] - pos0[2];
	float dist = sqrtf(dx * dx + dy * dy + dz * dz);
	return dist / fElapsed;
}

// ============================================================================
// Weapon system
// ============================================================================

void GameCharacter::InitWeapon(uint32_t ui32ItemID, uint8_t ui8Slot)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return;

	m_Weapons[ui8Slot].ui32ItemID = ui32ItemID;
	m_Weapons[ui8Slot].ui8SlotType = ui8Slot;

	// Set bullets from weapon table
	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32ItemID);
	if (pInfo)
	{
		m_Weapons[ui8Slot].ui16LoadedBullets = pInfo->ui16LoadBullet;
		m_Weapons[ui8Slot].ui16ReserveBullets = pInfo->ui16MaxBullet;
	}

	// Save for respawn (primary/secondary only)
	if (ui8Slot < WEAPON_SLOT_MELEE)
		m_ui32WeaponRespawn[ui8Slot] = ui32ItemID;
}

uint32_t GameCharacter::GetEquipWeaponID(uint8_t ui8Slot) const
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return 0;
	return m_Weapons[ui8Slot].ui32ItemID;
}

void GameCharacter::SetEquipWeapon(uint32_t ui32ItemID, uint8_t ui8Slot)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return;
	m_Weapons[ui8Slot].ui32ItemID = ui32ItemID;
}

bool GameCharacter::DecreaseBullet(uint8_t ui8Slot, uint16_t count)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return false;
	return m_Weapons[ui8Slot].DecreaseBullet(count);
}

void GameCharacter::InitBullets()
{
	for (int i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		if (m_Weapons[i].ui32ItemID == 0)
			continue;

		const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(m_Weapons[i].ui32ItemID);
		if (pInfo)
		{
			m_Weapons[i].ui16LoadedBullets = pInfo->ui16LoadBullet;
			m_Weapons[i].ui16ReserveBullets = pInfo->ui16MaxBullet;
		}
	}
}

void GameCharacter::SetBullets(uint8_t ui8Slot, uint16_t loaded, uint16_t reserve)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return;
	m_Weapons[ui8Slot].ui16LoadedBullets = loaded;
	m_Weapons[ui8Slot].ui16ReserveBullets = reserve;
}

void GameCharacter::DropWeapon(uint8_t ui8Slot)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return;
	m_Weapons[ui8Slot].Reset();
}

bool GameCharacter::PickUpWeapon(uint32_t ui32ItemID, uint8_t ui8Slot)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return false;

	m_Weapons[ui8Slot].ui32ItemID = ui32ItemID;
	m_Weapons[ui8Slot].ui8SlotType = ui8Slot;

	const WeaponDamageInfo* pInfo = WeaponTable::GetInstance().GetWeaponInfo(ui32ItemID);
	if (pInfo)
	{
		m_Weapons[ui8Slot].ui16LoadedBullets = pInfo->ui16LoadBullet;
		m_Weapons[ui8Slot].ui16ReserveBullets = pInfo->ui16MaxBullet;
	}
	return true;
}

const WeaponInstance* GameCharacter::GetWeapon(uint8_t ui8Slot) const
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return nullptr;
	return &m_Weapons[ui8Slot];
}

WeaponInstance* GameCharacter::GetWeaponMut(uint8_t ui8Slot)
{
	if (ui8Slot >= WEAPON_SLOT_COUNT)
		return nullptr;
	return &m_Weapons[ui8Slot];
}

// ============================================================================
// Fire tracking
// ============================================================================

void GameCharacter::SetLastFireInfo(float fTime, uint32_t ui32ItemID, AttackType eAttack)
{
	m_fLastFireTime = fTime;
	m_ui32LastFireItemID = ui32ItemID;
	m_eLastFireAttackType = eAttack;
}

// ============================================================================
// Equipment
// ============================================================================

void GameCharacter::SetParts(int idx, uint32_t id)
{
	if (idx >= CHAR_EQUIP_PARTS_COUNT)
		return;
	m_aui32PartsID[idx] = id;
}

// ============================================================================
// Damage helpers
// ============================================================================

float GameCharacter::GetDefenceRate(CharaHitPart ePart) const
{
	// Base defence from helmet (head only)
	if (ePart == CHARA_HIT_HEAD && m_aui32PartsID[CHAR_EQUIP_HEAD] != 0)
		return 0.8f;	// 20% head damage reduction from helmet

	// Body armor
	if ((ePart == CHARA_HIT_CHEST || ePart == CHARA_HIT_STOMACH) &&
		m_aui32PartsID[CHAR_EQUIP_UPPER] != 0)
		return 0.9f;	// 10% body damage reduction from armor

	return 1.0f;	// No reduction
}

float GameCharacter::GetDamageIncRate(WeaponSlotType eSlot, WeaponClassType eClass) const
{
	// Base damage increase rate (no equipment bonuses in simplified version)
	return 1.0f;
}

// ============================================================================
// Hit tracking
// ============================================================================

bool GameCharacter::IsAbleToCritical(int hitRatioCnt) const
{
	if (hitRatioCnt <= 0)
		return false;

	// Critical hit ratio shouldn't exceed 1:1
	return (m_i32HitCount / hitRatioCnt) <= m_i32CriticalHitCount;
}
