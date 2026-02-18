#include "stdafx.h"
#include "i3LevelPropertyContainer.h"
#include "i3LevelProperty.h"

I3_CLASS_INSTANCE( i3LevelPropertyContainer);

void	i3LevelPropertyContainer::AddProperty( i3LevelProperty * pProp)
{
	I3_MUST_ADDREF(pProp);

	m_PropertyList.push_back( pProp);
}

void	i3LevelPropertyContainer::RemoveProperty( i3LevelProperty * pProp)
{
	INT32 idx;
	
	idx = GetPropertyIndex( pProp);
	if( idx == -1)
		return;

	RemoveProperty( pProp);
}

void	i3LevelPropertyContainer::RemoveProperty( INT32 idx)
{
	i3LevelProperty * pProp;

	pProp = GetProperty( idx);
	
	I3_MUST_RELEASE(pProp);

	m_PropertyList.erase(m_PropertyList.begin() + idx);
}

INT32	i3LevelPropertyContainer::FindPropertyByName( const char * pszName)
{
	i3LevelProperty * pProp;

	for(size_t i = 0; i < m_PropertyList.size(); i++)
	{
		pProp = GetProperty( i);

		if( i3::generic_is_iequal( pProp->GetName(), pszName) )
			return i;
	}

	return -1;
}

INT32 i3LevelPropertyContainer::GetPropertyIndex( i3LevelProperty * pProp)
{
	size_t idx = i3::vu::index_of(m_PropertyList, pProp);
	return ( idx != m_PropertyList.size() ) ? INT32(idx) : -1;
}

void	i3LevelPropertyContainer::RemoveAllProperties(void)
{
	INT32 i;
	i3LevelProperty * pProp;

	for( i = 0; i < GetPropertyCount(); i++)
	{
		pProp = GetProperty( i);

		I3_MUST_RELEASE(pProp);
	}

	m_PropertyList.clear();
}

void i3LevelPropertyContainer::OnChangeProperty( i3LevelProperty * pProp)
{
}

void i3LevelPropertyContainer::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3LevelProperty * pProp;

	for( i = 0; i < GetPropertyCount(); i++)
	{
		pProp = GetProperty( i);

		pProp->OnBuildObjectList( List);
	}

	i3LevelElement::OnBuildObjectList( List);
}

void i3LevelPropertyContainer::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelElement::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelPropertyContainer* >(pDest));

	i3LevelPropertyContainer * pContainer = (i3LevelPropertyContainer*)pDest;

	i3LevelProperty * pProp;
	pContainer->RemoveAllProperties();
	for( INT32 i = 0;i < GetPropertyCount(); ++i)
	{
		pProp = GetProperty(i);

		pContainer->AddProperty( pProp);
	}
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)				// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct PROPERTY_CONTAINER
	{
		UINT8		m_ID[4] = { 'T', 'P', 'C', '1' };
		UINT32		m_Count = 0;
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelPropertyContainer::OnSave( i3ResourceFile * pResFile)
{
	pack::PROPERTY_CONTAINER Info;
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3LevelElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Info.m_Count = 0;

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelPropertyContainer::OnSave()", "Could not write property container information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3LevelPropertyContainer::OnLoad( i3ResourceFile * pResFile)
{
	pack::PROPERTY_CONTAINER Info;
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3LevelElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelPropertyContainer::OnSave()", "Could not write property container information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}
