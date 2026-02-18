#include "stdafx.h"
#include "i3LevelElementList.h"

I3_CLASS_INSTANCE( i3LevelElementList);

i3LevelElementList::~i3LevelElementList(void)
{
	removeAll();
}

void i3LevelElementList::add( i3LevelElement * pElm)
{
	I3_MUST_ADDREF(pElm);

	m_ElmList.push_back( pElm);
}

void i3LevelElementList::removeAll(void)
{
	INT32 i;
	i3LevelElement * pElm;

	for( i = 0; i < getCount(); i++)
	{
		pElm = get( i);

		I3_MUST_RELEASE(pElm);
	}

	m_ElmList.clear();
}

void i3LevelElementList::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelElementList* >(pDest));

	i3LevelElementList * plist = (i3LevelElementList*)pDest;

	plist->removeAll();
	for( INT32 i = 0;i < getCount(); ++i)
	{
		plist->add( get( i));
	}
}

void	i3LevelElementList::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3LevelElement * pElm;

	for( i = 0; i < getCount(); i++)
	{
		pElm = get( i);

		pElm->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

UINT32	i3LevelElementList::OnSave( i3ResourceFile * pResFile)
{
	return SaveFromListToResourceFile(m_ElmList, pResFile);	//	m_ElmList.SaveTo( pResFile);
}

UINT32	i3LevelElementList::OnLoad( i3ResourceFile * pResFile)
{
	return	LoadFromResourceFileToList(m_ElmList, pResFile, true);		//	m_ElmList.LoadFrom( pResFile, true);
}
