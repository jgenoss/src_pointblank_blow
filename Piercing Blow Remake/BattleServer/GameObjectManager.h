#ifndef __GAMEOBJECTMANAGER_H__
#define __GAMEOBJECTMANAGER_H__

#pragma once
#include "GameObject.h"

class CMapData;

// GameObjectManager - Per-room game object lifecycle management
// Port of DSObjectManager from the original DediServer
// Manages weapon boxes, targets, helicopters, and destructible objects
class GameObjectManager
{
public:
	GameObjectManager();
	~GameObjectManager();

	// Initialize objects from map data
	bool	Initialize(CMapData* pMapData);
	void	Shutdown();
	void	Reset();

	// Game loop
	void	Update(DWORD dwNow);

	// Object access
	GameObject*	GetObject(int i32Index);
	GameObject*	FindObjectByNetIndex(int i32NetIndex);
	int			GetObjectCount() const		{ return m_i32ObjectCount; }
	int			GetActiveCount() const;

	// Damage
	bool	ApplyDamageToObject(int i32NetIndex, int i32Damage);

	// Object queries
	int		GetTargetObjectCount() const;
	int		GetWeaponBoxCount() const;

	// Destruction mission support
	GameObject*	GetRedTarget();			// DESTRUCTION: RED team's target
	GameObject*	GetBlueTarget();		// DESTRUCTION: BLUE team's target

	bool	IsInitialized() const		{ return m_bInitialized; }

private:
	GameObject		m_Objects[DS_MAX_OBJECT];
	int				m_i32ObjectCount;
	bool			m_bInitialized;
};

#endif // __GAMEOBJECTMANAGER_H__
