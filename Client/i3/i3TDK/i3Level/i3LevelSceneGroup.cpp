#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"

void i3LevelScene::addGroup( i3LevelGroup * pGroup)
{
	I3_MUST_ADDREF(pGroup);

	m_GroupList.push_back( pGroup);
}

void i3LevelScene::removeGroup( i3LevelGroup * pGroup)
{
	I3_MUST_RELEASE(pGroup);

	i3::vu::remove(m_GroupList, pGroup);
}

void i3LevelScene::removeAllGroup(void)
{
	INT32 i;
	i3LevelGroup * pGroup;

	for( i = 0; i < getGroupCount(); i++)
	{
		pGroup = getGroup( i);

		I3_MUST_RELEASE(pGroup);
	}

	m_GroupList.clear();
}

void i3LevelScene::SetGroupToSelectedElements(  const i3::rc_string& strGroupName)
{
	i3LevelGroup * pGroup;

	pGroup = i3LevelGroup::new_object_ref();

	pGroup->SetName( strGroupName);

	addGroup( pGroup);

	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pGroup->AddElement( pElm);

		pElm->setGroup( pGroup);
	}
}

void i3LevelScene::ReleaseGroupFromSelectedElements(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	/// 선택된 Element들 중, Group이 설정된 것이 있다면
	// 설정 해제한다.
	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		i3LevelGroup * pGroup = pElm->getGroup();

		pGroup->RemoveElement( pElm);
		pElm->setGroup( nullptr);
	}

	// Group들 중에 아무런 Element를 포함하고 있지 않은 것이
	// 있다면 제거한다.
	for( i = 0; i < getGroupCount(); i++)
	{
		i3LevelGroup * pGroup = getGroup( i);

		if( pGroup->getElementCount() <= 0)
		{
			removeGroup( pGroup);

			i--;
		}
	}
}
