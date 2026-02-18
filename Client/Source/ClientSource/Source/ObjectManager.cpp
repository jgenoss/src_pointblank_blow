#include "pch.h"
#include "ObjectManager.h"
#include "GlobalVariables.h"
#include "GameObject.h"


CGameObjectManager::CGameObjectManager(void)
{
	INT32 i;

	for( i = 0; i < TEAM_COUNT; ++i)
	{
		m_pDestructionObject[i] = nullptr;
	}

	for( i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		m_pDefenceObject[ i] = nullptr;
	}
}

CGameObjectManager::~CGameObjectManager(void)
{
	DestroyObject();
}

void CGameObjectManager::DestroyObject()
{
	INT32 i;

	for( i = 0; i < TEAM_COUNT; ++i)
	{
		I3_SAFE_RELEASE( m_pDestructionObject[i]);
	}

	for( i = 0; i< MAX_OBJECT_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pDefenceObject[ i]);
	}
}

void CGameObjectManager::SetGameDestructionObject( TEAM_TYPE team, i3Object * pObj)
{
	I3_REF_CHANGE( m_pDestructionObject[team], (CGameControledObject*)pObj);
}

void CGameObjectManager::SetGameDefenceObject( INT32 idx, i3Object * pObj)
{
	I3_REF_CHANGE( m_pDefenceObject[ idx], (CGameControledObject*) pObj);
}

void CGameObjectManager::EnableObject()
{
	INT32 i;

	for( i = 0; i < TEAM_COUNT; ++i)
	{
		if( m_pDestructionObject[i] != nullptr)
		{
			m_pDestructionObject[i]->EnableObject();
		}
	}

	for( i = 0; i < MAX_OBJECT_COUNT; i++)
	{
		if( m_pDefenceObject[ i] != nullptr)
		{
			if( i3::same_of<CGameControledObject*>(m_pDefenceObject[ i]))
				m_pDefenceObject[ i]->EnableObject();
		}
	}
}

