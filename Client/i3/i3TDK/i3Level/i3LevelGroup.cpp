#include "stdafx.h"
#include "i3LevelGroup.h"

I3_CLASS_INSTANCE( i3LevelGroup);

i3LevelGroup::~i3LevelGroup(void)
{
	RemoveAllElement();
}

void	i3LevelGroup::AddElement( i3LevelElement3D * pObj)
{
	I3ASSERT( pObj != nullptr);

	m_ElmList.push_back( pObj);
}

void	i3LevelGroup::RemoveElement( i3LevelElement3D * pObj)
{
	INT32 idx;

	I3ASSERT( pObj != nullptr);

	if( pObj->getGroup() == this)
	{
		pObj->m_pGroup = nullptr;
	}

	idx = FindElement( pObj);
	I3ASSERT( idx != -1);

	m_ElmList.erase( m_ElmList.begin() + idx);
}

INT32	i3LevelGroup::FindElement( i3LevelElement3D * pObj)
{
	INT32 i;

	for( i = 0; i < getElementCount(); i++)
	{
		if( getElement(i) == pObj)
			return i;
	}

	return -1;
}

void	i3LevelGroup::RemoveAllElement(void)
{
	m_ElmList.clear();
}

void	i3LevelGroup::Select(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < getElementCount(); i++)
	{
		pElm = getElement( i);

		pElm->Select();
	}
}

void	i3LevelGroup::Unselect(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < getElementCount(); i++)
	{
		pElm = getElement( i);

		pElm->Unselect();
	}
}

void	i3LevelGroup::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelGroup*>(pDest) );

	i3LevelGroup * pGroup = (i3LevelGroup*)pDest;
	i3LevelElement3D * pElm;

	pGroup->RemoveAllElement();
	for( INT32 i = 0;i < getElementCount(); ++i)
	{
		pElm = getElement( i);

		pGroup->AddElement( pElm);
	}
}
