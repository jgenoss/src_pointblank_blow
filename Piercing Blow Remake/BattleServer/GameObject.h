#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#pragma once
#include "MapData.h"

// Game Object - Server-side representation of in-game objects
// Port of CDSObject from the original DediServer (DSObjectBase.h)
// Tracks HP, position, respawn state for weapon boxes, targets, helicopters, etc.
class GameObject
{
public:
	GameObject();
	~GameObject();

	// Initialize from map data game object
	void	InitFromMapObject(const DS_GAME_OBJECT* pMapObj, int i32Index);
	void	Reset();

	// State
	bool	IsActive() const		{ return m_bActive; }
	bool	IsDestroyed() const		{ return m_i32HP <= 0 && m_i32MaxHP > 0; }
	bool	NeedsRespawn() const;

	// Damage
	void	ApplyDamage(int i32Damage);
	void	Destroy();
	void	Respawn();

	// Update (check respawn timer)
	void	Update(DWORD dwNow);

	// Getters
	uint16_t	GetSType() const			{ return m_ui16SType; }
	int			GetNetIndex() const			{ return m_i32NetIndex; }
	int			GetHP() const				{ return m_i32HP; }
	int			GetMaxHP() const			{ return m_i32MaxHP; }
	const float* GetPosition() const		{ return m_fPos; }
	float		GetRespawnTime() const		{ return m_fRespawnTime; }
	int			GetGroupIndex() const		{ return m_i8GroupIdx; }
	uint8_t		GetWeaponType() const		{ return m_ui8WeaponType; }
	bool		IsWeaponBox() const			{ return (m_ui16SType & OBJECT_S_TYPE_W_BOX) != 0; }
	bool		IsTarget() const			{ return (m_ui16SType & OBJECT_S_TYPE_TARGET) != 0; }
	bool		IsHelicopter() const		{ return (m_ui16SType & OBJECT_S_TYPE_HELICOPTER) != 0; }
	bool		IsUseItemBox() const		{ return (m_ui16SType & OBJECT_S_TYPE_USEITEMBOX) != 0; }

private:
	bool		m_bActive;
	uint16_t	m_ui16SType;			// OBJECT_S_TYPE_*
	int			m_i32NetIndex;			// Network index for sync
	int			m_i32HP;				// Current HP
	int			m_i32MaxHP;				// Max HP
	float		m_fPos[3];				// Center position
	float		m_fRespawnTime;			// Seconds until respawn (for weapon boxes)
	DWORD		m_dwDestroyTime;		// When object was destroyed
	int8_t		m_i8GroupIdx;			// Group index
	uint8_t		m_ui8WeaponType;		// Weapon type for W_BOX

	// Battle use items
	uint8_t		m_ui8UseItemCount;
	uint32_t	m_ui32UseItemID[5];
	uint32_t	m_ui32UseItemProbability[5];
};

#endif // __GAMEOBJECT_H__
