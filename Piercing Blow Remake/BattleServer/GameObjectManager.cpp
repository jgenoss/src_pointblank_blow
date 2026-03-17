#include "pch.h"
#include "GameObjectManager.h"
#include "MapData.h"

GameObjectManager::GameObjectManager()
	: m_i32ObjectCount(0)
	, m_bInitialized(false)
{
}

GameObjectManager::~GameObjectManager()
{
	Shutdown();
}

bool GameObjectManager::Initialize(CMapData* pMapData)
{
	if (!pMapData)
		return false;

	Shutdown();

	int mapObjCount = pMapData->GetGameObjectCount();
	DS_GAME_OBJECT* pMapObjects = pMapData->GetGameObjects();

	if (mapObjCount <= 0 || !pMapObjects)
	{
		m_bInitialized = true;
		return true;	// No objects is valid
	}

	for (int i = 0; i < mapObjCount && m_i32ObjectCount < DS_MAX_OBJECT; i++)
	{
		m_Objects[m_i32ObjectCount].InitFromMapObject(&pMapObjects[i], i);
		m_i32ObjectCount++;
	}

	m_bInitialized = true;

	printf("[GameObjectManager] Initialized: %d objects (Targets=%d, WeaponBoxes=%d)\n",
		m_i32ObjectCount, GetTargetObjectCount(), GetWeaponBoxCount());

	return true;
}

void GameObjectManager::Shutdown()
{
	Reset();
	m_bInitialized = false;
}

void GameObjectManager::Reset()
{
	for (int i = 0; i < DS_MAX_OBJECT; i++)
		m_Objects[i].Reset();
	m_i32ObjectCount = 0;
}

void GameObjectManager::Update(DWORD dwNow)
{
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		m_Objects[i].Update(dwNow);
	}
}

GameObject* GameObjectManager::GetObject(int i32Index)
{
	if (i32Index < 0 || i32Index >= m_i32ObjectCount)
		return nullptr;
	return &m_Objects[i32Index];
}

GameObject* GameObjectManager::FindObjectByNetIndex(int i32NetIndex)
{
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		if (m_Objects[i].IsActive() && m_Objects[i].GetNetIndex() == i32NetIndex)
			return &m_Objects[i];
	}
	return nullptr;
}

int GameObjectManager::GetActiveCount() const
{
	int count = 0;
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		if (m_Objects[i].IsActive() && !m_Objects[i].IsDestroyed())
			count++;
	}
	return count;
}

bool GameObjectManager::ApplyDamageToObject(int i32NetIndex, int i32Damage)
{
	GameObject* pObj = FindObjectByNetIndex(i32NetIndex);
	if (!pObj || !pObj->IsActive())
		return false;

	pObj->ApplyDamage(i32Damage);
	return true;
}

int GameObjectManager::GetTargetObjectCount() const
{
	int count = 0;
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		if (m_Objects[i].IsActive() && m_Objects[i].IsTarget())
			count++;
	}
	return count;
}

int GameObjectManager::GetWeaponBoxCount() const
{
	int count = 0;
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		if (m_Objects[i].IsActive() && m_Objects[i].IsWeaponBox())
			count++;
	}
	return count;
}

GameObject* GameObjectManager::GetRedTarget()
{
	// In destruction mode, first target object is typically RED's target
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		if (m_Objects[i].IsActive() && m_Objects[i].IsTarget())
			return &m_Objects[i];
	}
	return nullptr;
}

GameObject* GameObjectManager::GetBlueTarget()
{
	// Second target object is typically BLUE's target
	int found = 0;
	for (int i = 0; i < m_i32ObjectCount; i++)
	{
		if (m_Objects[i].IsActive() && m_Objects[i].IsTarget())
		{
			found++;
			if (found >= 2)
				return &m_Objects[i];
		}
	}
	return nullptr;
}
