#ifndef __GAMECHARACTER_H__
#define __GAMECHARACTER_H__

#pragma once
#include "WeaponSystem.h"
#include "SpeedState.h"

// ============================================================================
// GameCharacter - Full port of original CCharacter
// Per-slot character state: weapons, HP, position, speed, fire tracking
// Owned by BattleRoom, indexed by slot (0-15)
// ============================================================================

// Character class (from original CHARA_CLASS)
enum CharaClass
{
	CHARA_CLASS_ASSAULT = 0,
	CHARA_CLASS_SNIPER,
	CHARA_CLASS_HEAVY,
	CHARA_CLASS_SPEC_OPS,
	CHARA_CLASS_COUNT,
};

// Equipment parts (from original CHAR_EQUIPMENT_PARTS)
enum CharEquipParts
{
	CHAR_EQUIP_CHARA = 0,		// Character skin
	CHAR_EQUIP_HEAD,			// Helmet
	CHAR_EQUIP_UPPER,			// Upper body
	CHAR_EQUIP_LOWER,			// Lower body
	CHAR_EQUIP_HAND,			// Gloves
	CHAR_EQUIP_FOOT,			// Boots
	CHAR_EQUIP_FACE,			// Face item
	CHAR_EQUIP_BACKPACK,		// Backpack
	CHAR_EQUIP_PARTS_COUNT,
};

// Forward decl
class CollisionSystem;

class GameCharacter
{
public:
	GameCharacter();
	~GameCharacter();

	void		Reset();
	void		ResetBattleInfo();	// Reset per-round data

	// ============================================================
	// Life state
	// ============================================================
	bool		IsAlive() const				{ return m_bAlive; }
	void		SetAlive(bool bAlive)		{ m_bAlive = bAlive; }

	int16_t		GetHP() const				{ return m_i16HP; }
	void		SetHP(int16_t hp)			{ m_i16HP = hp; }
	int16_t		GetMaxHP() const			{ return m_i16MaxHP; }
	void		SetMaxHP(int16_t hp)		{ m_i16HP = m_i16MaxHP = hp; }

	int32_t		SubHP(int16_t dec)			{ m_i16HP -= dec; return m_i16HP; }
	int32_t		AddHP(int16_t inc);			// Capped at MaxHP

	// ============================================================
	// Position / Rotation (from N_PCINFO_POSROTATION)
	// ============================================================
	const float*	GetPosition() const		{ return m_fPos; }
	const float*	GetRotation() const		{ return m_fRot; }
	void		SetPosition(float x, float y, float z);
	void		SetRotation(float rx, float ry, float rz);

	float		GetPosRecvTime() const		{ return m_fPosRecvTime; }
	void		SetPosRecvTime(float t)		{ m_fPosRecvTime = t; }

	// Calculate speed between two positions
	float		CalcSpeed(const float* pos0, const float* pos1, float fElapsed) const;

	// ============================================================
	// Weapon system
	// ============================================================
	void		InitWeapon(uint32_t ui32ItemID, uint8_t ui8Slot);
	uint32_t	GetEquipWeaponID(uint8_t ui8Slot) const;
	void		SetEquipWeapon(uint32_t ui32ItemID, uint8_t ui8Slot);

	// Bullet management
	bool		DecreaseBullet(uint8_t ui8Slot, uint16_t count = 1);
	void		InitBullets();
	void		SetBullets(uint8_t ui8Slot, uint16_t loaded, uint16_t reserve);

	// Drop / Pickup
	void		DropWeapon(uint8_t ui8Slot);
	bool		PickUpWeapon(uint32_t ui32ItemID, uint8_t ui8Slot);

	// Get currently equipped weapon in slot
	const WeaponInstance* GetWeapon(uint8_t ui8Slot) const;
	WeaponInstance*		  GetWeaponMut(uint8_t ui8Slot);

	// ============================================================
	// Fire tracking (for fire rate hack detection)
	// ============================================================
	void		SetLastFireInfo(float fTime, uint32_t ui32ItemID, AttackType eAttack);
	float		GetLastFireTime() const		{ return m_fLastFireTime; }
	uint32_t	GetLastFireItemID() const	{ return m_ui32LastFireItemID; }
	AttackType	GetLastFireAttackType() const { return m_eLastFireAttackType; }

	uint32_t	GetFireCount() const		{ return m_ui32FireCount; }
	void		IncFireCount()				{ m_ui32FireCount++; }
	void		ResetFireCount()			{ m_ui32FireCount = 0; }
	float		GetFireCheckTime() const	{ return m_fFireCheckTime; }
	void		SetFireCheckTime(float t)	{ m_fFireCheckTime = t; }

	// ============================================================
	// Speed state (hack detection)
	// ============================================================
	SpeedState*	GetSpeedState()				{ return &m_SpeedState; }

	// ============================================================
	// Equipment / Parts
	// ============================================================
	uint32_t	GetParts(int idx) const		{ return (idx < CHAR_EQUIP_PARTS_COUNT) ? m_aui32PartsID[idx] : 0; }
	void		SetParts(int idx, uint32_t id);
	uint32_t*	GetAllParts()				{ return m_aui32PartsID; }

	// ============================================================
	// Damage calculation helpers
	// ============================================================
	float		GetDefenceRate(CharaHitPart ePart) const;
	float		GetDamageIncRate(WeaponSlotType eSlot, WeaponClassType eClass) const;

	// Character class
	void		SetCharClass(CharaClass eClass)	{ m_eCharClass = eClass; }
	CharaClass	GetCharClass() const			{ return m_eCharClass; }

	// ============================================================
	// Hit tracking (for critical hit ratio hack detection)
	// ============================================================
	void		IncHitCount()				{ m_i32HitCount++; }
	void		IncCriticalHitCount()		{ m_i32CriticalHitCount++; }
	int			GetHitCount() const			{ return m_i32HitCount; }
	int			GetCriticalHitCount() const	{ return m_i32CriticalHitCount; }
	bool		IsAbleToCritical(int hitRatioCnt) const;

	// ============================================================
	// Latency tracking
	// ============================================================
	float		GetLastClientTime() const	{ return m_fLastClientTime; }
	float		GetLastDediTime() const		{ return m_fLastDediTime; }
	float		GetLatencyTime() const		{ return m_fLatencyTime; }
	void		SetLastClientTime(float t)	{ m_fLastClientTime = t; }
	void		SetLastDediTime(float t)	{ m_fLastDediTime = t; }
	void		SetLatencyTime(float t)		{ m_fLatencyTime = t; }

	float		GetAverageLatency() const	{ return m_fAverageLatency; }
	void		AddLatencySample(float t)	{ m_fAverageLatency += t; m_ui32AverageCount++; }
	uint32_t	GetLatencySampleCount() const { return m_ui32AverageCount; }

	// ============================================================
	// Object carrying (for bomb mode, etc.)
	// ============================================================
	bool		IsOnLoad() const			{ return m_bOnLoad; }
	void		SetOnLoad(bool b)			{ m_bOnLoad = b; }
	uint16_t	GetOnLoadIndex() const		{ return m_ui16OnLoadIndex; }
	void		SetOnLoadIndex(uint16_t idx){ m_ui16OnLoadIndex = idx; }

	// Respawn weapons (primary/secondary saved for respawn)
	uint32_t	GetRespawnWeapon(uint8_t slot) const { return (slot < WEAPON_SLOT_MELEE) ? m_ui32WeaponRespawn[slot] : 0; }
	void		SetRespawnWeapon(uint32_t id, uint8_t slot) { if (slot < WEAPON_SLOT_MELEE) m_ui32WeaponRespawn[slot] = id; }

private:
	CharaClass		m_eCharClass;

	// Life
	bool			m_bAlive;
	bool			m_bOnLoad;			// Carrying an object (bomb, etc.)
	int16_t			m_i16HP;
	int16_t			m_i16MaxHP;
	int16_t			m_i16StoreHP;		// Stored HP for rollback
	bool			m_bStoredHP;

	// Position
	float			m_fPos[3];
	float			m_fRot[3];
	float			m_fPosRecvTime;		// Game time of last position update
	uint16_t		m_ui16OnLoadIndex;	// Object index being carried

	// Weapons (one per slot)
	WeaponInstance	m_Weapons[WEAPON_SLOT_COUNT];
	uint32_t		m_ui32WeaponRespawn[WEAPON_SLOT_MELEE]; // Saved for respawn

	// Fire tracking
	float			m_fLastFireTime;
	uint32_t		m_ui32LastFireItemID;
	AttackType		m_eLastFireAttackType;
	uint32_t		m_ui32FireCount;
	float			m_fFireCheckTime;

	// Speed hack detection
	SpeedState		m_SpeedState;

	// Equipment parts
	uint32_t		m_aui32PartsID[CHAR_EQUIP_PARTS_COUNT];

	// Hit tracking
	int32_t			m_i32HitCount;
	int32_t			m_i32CriticalHitCount;

	// Latency tracking
	float			m_fLastClientTime;
	float			m_fLastDediTime;
	float			m_fLatencyTime;
	float			m_fAverageLatency;
	uint32_t		m_ui32AverageCount;
};

#endif // __GAMECHARACTER_H__
