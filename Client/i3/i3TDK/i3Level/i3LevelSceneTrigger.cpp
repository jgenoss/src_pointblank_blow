#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

/////////////////////////////////////////////////
// i3LevelTrigger
INT32			i3LevelScene::AddTrigger( i3LevelTrigger * pTrig)
{
	I3_MUST_ADDREF(pTrig);

	size_t idx = m_TriggerList.size();
	m_TriggerList.push_back( pTrig);

	pTrig->OnAttachScene( this);

	i3Level::Update( nullptr, I3_TDK_UPDATE_ADD, pTrig);

	return INT32(idx);
}

void			i3LevelScene::RemoveTrigger( i3LevelTrigger * pTrig)
{
	

	size_t idx = i3::vu::index_of(m_TriggerList, pTrig);
	if( idx == m_TriggerList.size() )
		return;

	RemoveTrigger( INT32(idx));
}

void			i3LevelScene::RemoveTrigger( INT32 idx)
{
	i3LevelTrigger * pTrig;

	pTrig = GetTrigger( idx);

	pTrig->OnDetachScene( this);
	i3Level::Update( nullptr, I3_TDK_UPDATE_REMOVE, pTrig);

	I3_MUST_RELEASE(pTrig);

	m_TriggerList.erase(m_TriggerList.begin() + idx);
}

void			i3LevelScene::RemoveAllTrigger(void)
{
	i3LevelTrigger * pTrig;

	for(size_t i = 0; i < m_TriggerList.size(); i++)
	{
		pTrig = GetTrigger(i);

		pTrig->OnDetachScene( this);

		I3_MUST_RELEASE(pTrig);
	}

	m_TriggerList.clear();

	i3Level::Update( nullptr, I3_TDK_UPDATE_REMOVE, nullptr, i3LevelTrigger::static_meta());
}

INT32			i3LevelScene::FindTriggerByName( const char * pszName)
{
	
	i3LevelTrigger * pTrig;

	for(size_t i = 0; i < m_TriggerList.size(); i++)
	{
		pTrig = (i3LevelTrigger *) GetTrigger( i);

		if( i3::generic_is_iequal( pTrig->GetName(), pszName) )
			return i;
	}

	return -1;
}

void i3LevelScene::GetUniqueTriggerName( char * pszPrefix, char * pszName)
{
	INT32 i = 0;

	if( pszPrefix == nullptr)
		pszPrefix = "Trigger";

	sprintf( pszName, "%s", pszPrefix);

	while( FindTriggerByName( pszName) != -1)
	{
		sprintf( pszName, "%s%d", pszPrefix, i);

		i++;
	}
}

void i3LevelScene::UnfreezeAllTriggers(void)
{
	INT32 i;
	i3LevelTrigger * pTrig;

	for( i = 0; i < GetTriggerCount(); i++)
	{
		pTrig = GetTrigger(i);

		pTrig->Unfreeze();
	}
}

void i3LevelScene::FreezeAllTriggers(void)
{
	INT32 i;
	i3LevelTrigger * pTrig;

	for( i = 0; i < GetTriggerCount(); i++)
	{
		pTrig = GetTrigger(i);

		pTrig->Freeze();
	}
}
