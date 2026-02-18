#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

/*
/////////////////////////////////////////////////
// i3LevelAction
INT32			i3LevelScene::AddAction( i3LevelAction * pAction)
{
	INT32 idx;

	pAction->AddRef();

	idx = m_ActionList.Add( pAction);

	pAction->OnAttachScene( this);

	i3Level::Update( nullptr, I3_TDK_UPDATE_ADD, pAction);

	return idx;
}

void			i3LevelScene::RemoveAction( i3LevelAction * pAction)
{
	INT32 idx;

	idx = m_ActionList.IndexOf( pAction);
	if( idx == -1)
		return;

	RemoveAction( idx);
}

void			i3LevelScene::RemoveAction( INT32 idx)
{
	i3LevelAction * pAction;

	pAction = GetAction( idx);

	pAction->OnDetachScene( this);

	i3Level::Update( nullptr, I3_TDK_UPDATE_REMOVE, pAction);

	pAction->Release();

	m_ActionList.Delete( idx);
}

void			i3LevelScene::RemoveAllAction(void)
{
	INT32 i;
	i3LevelAction * pAction;

	for( i = 0; i < m_ActionList.GetCount(); i++)
	{
		pAction = GetAction(i);

		pAction->OnDetachScene( this);

		pAction->Release();
	}

	m_ActionList.Clear();

	//i3Level::Update( nullptr, I3_TDK_UPDATE_REMOVE, nullptr, i3LevelAction::GetClassMeta());
}

INT32			i3LevelScene::FindActionByName( const char * pszName)
{
	INT32 i;
	i3LevelAction * pAction;

	for( i = 0; i < m_ActionList.GetCount(); i++)
	{
		pAction = (i3LevelAction *) GetAction( i);

		if( i3String::Compare( pAction->GetName(), pszName) == 0)
			return i;
	}

	return -1;
}

void i3LevelScene::GetUniqueActionName( char * pszPrefix, char * pszName)
{
	INT32 i = 0;

	if( pszPrefix == nullptr)
		pszPrefix = "Action";

	sprintf( pszName, "%s", pszPrefix);

	while( FindActionByName( pszName) != -1)
	{
		sprintf( pszName, "%s%d", pszPrefix, i);

		i++;
	}
}

void i3LevelScene::UnfreezeAllActions(void)
{
	INT32 i;
	i3LevelAction * pAction;

	for( i = 0; i < GetActionCount(); i++)
	{
		pAction = GetAction(i);

		pAction->Unfreeze();
	}
}

void i3LevelScene::FreezeAllActions(void)
{
	INT32 i;
	i3LevelAction * pAction;

	for( i = 0; i < GetActionCount(); i++)
	{
		pAction = GetAction(i);

		pAction->Freeze();
	}
}
*/