#include "stdafx.h"
#include "i3TDKObjectBase.h"

I3_CLASS_INSTANCE( i3TDKObjectBase);

i3TDKObjectBase::~i3TDKObjectBase(void)
{
	RemoveAllChild();
}

#ifdef I3_DEBUG
void i3TDKObjectBase::AddChild( i3TDKObjectBase *pChild, const char *szFile, INT32 nLine )
{
	return AddChild( pChild );
}
#endif

void i3TDKObjectBase::AddChildToHead( i3TDKObjectBase * pChild)
{
	I3ASSERT( pChild != nullptr);

	pChild->SetNext( m_pChild);

	I3_REF_CHANGE( m_pChild, pChild);

	m_pChild->SetParent( this);
}

void i3TDKObjectBase::AddChildToTail( i3TDKObjectBase * pChild)
{
	i3TDKObjectBase * pNode;

	I3ASSERT( pChild);

	if( m_pChild == nullptr)
	{
		m_pChild = pChild;
		I3_MUST_ADDREF(pChild);
	}
	else
	{
		pNode = m_pChild;

		while( pNode->GetNext() != nullptr)
			pNode = pNode->GetNext();

		pNode->SetNext( pChild);
	}

	pChild->SetParent( this);
}

void i3TDKObjectBase::RemoveChild( i3TDKObjectBase * pChild)
{
	i3TDKObjectBase * pPrev;

	I3ASSERT( pChild);

	if( m_pChild == pChild)
	{
		m_pChild = pChild->GetNext();

		I3_MUST_RELEASE(pChild);
	}
	else
	{
		pPrev = m_pChild;

		while((pPrev != nullptr) && (pPrev->GetNext() != pChild))
			pPrev = pPrev->GetNext();

		if( pPrev == nullptr)
			return;

		pPrev->SetNext( pChild->GetNext());

		pChild->SetNext( nullptr);
	}

	pChild->SetParent( nullptr);
}

void i3TDKObjectBase::RemoveAllChild(void)
{
	i3TDKObjectBase * pChild, * pNextChild;

	// 모든 Child에 대해서 OnUpdate를 Recursive하게 전달.	
	for( pChild = m_pChild; pChild != nullptr; )
	{
		pNextChild = pChild->GetNext();
		I3_SAFE_RELEASE( pChild );
		pChild = pNextChild;
	}

	m_pChild = nullptr;
}

// Index로 Child 얻기.
// 모든 자식 노드를 검색해야 되는 경우라면 이 함수를 쓰는 것은 아주 느릴 수 있음.
// 모든 자식 노드를 검색해야 될 경우엔 다음과 같이 사용하기 바람.
//
// for( i3TDKObjectBase *pChild = pNode->GetFirstChild(); pChild != nullptr; pChild = pChild->GetNext() )
//
i3TDKObjectBase * i3TDKObjectBase::GetChildByIndex( INT32 nIndex )
{
	INT32 i;
	i3TDKObjectBase *pChild = GetFirstChild();
	I3ASSERT( pChild != nullptr );

	for( i = 0; i < nIndex; i++ )
	{
		pChild = pChild->GetNext();
		I3ASSERT( pChild != nullptr );
	}

	return pChild;
}

i3TDKObjectBase * i3TDKObjectBase::FindChildByName( const char * pszName, BOOL bRecurs)
{
	i3TDKObjectBase * pNode = m_pChild;
	i3TDKObjectBase * pResult;

	while( pNode != nullptr)
	{
		if( i3::generic_is_iequal( pNode->GetName(), pszName) )
			return pNode;

		if( bRecurs)
		{
			pResult = pNode->FindChildByName( pszName, TRUE);

			if( pResult != nullptr)
				return pResult;
		}

		pNode = pNode->GetNext();
	}

	return nullptr;
}

void i3TDKObjectBase::SetNext( i3TDKObjectBase * pNext)
{
	I3_REF_CHANGE( m_pNext, pNext);
}

void	i3TDKObjectBase::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3TDKObjectBase * pNode;

	// Parent에 대한 Reference는 추가하지 않는다.
	
	pNode = m_pChild;
	while( pNode != nullptr)
	{
		pNode->OnBuildObjectList( List);

		pNode = pNode->GetNext();
	}

	if( m_pNext != nullptr)
	{
		m_pNext->OnBuildObjectList( List);
	}
}

#pragma pack( push, 4)

namespace i3
{
	namespace pack
	{
		struct TDK_OBJECT_BASE
		{
			OBJREF		m_pParent = 0;
			OBJREF		m_pNext = 0;
			INT8		pad[32] = { 0 };
		};
	}
}

#pragma pack( pop)

UINT32 i3TDKObjectBase::OnSave( i3ResourceFile * pResFile)
{
	i3::pack::TDK_OBJECT_BASE data;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	data.m_pParent = (OBJREF) pResFile->GetObjectPersistID( GetParent());
	data.m_pNext = (OBJREF) pResFile->GetObjectPersistID( GetNext());

	Rc = pStream->Write( &data, sizeof(data));
	CHECKIO( Rc);
	
	return Result;
}

bool	i3TDKObjectBase::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool rv;

	rv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( rv == false)
		return false;

	pXML->SetAttribute( "ParentLink",	(INT32) pFile->GetObjectPersistID( GetParent()));
	pXML->SetAttribute( "NextLink",		(INT32) pFile->GetObjectPersistID( GetNext()));

	return true;
}

UINT32 i3TDKObjectBase::OnLoad( i3ResourceFile * pResFile)
{
	i3::pack::TDK_OBJECT_BASE data;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3TDKObjectBase * pTemp;

	Rc = pStream->Read( &data, sizeof(data));
	CHECKIO( Rc);

	if( data.m_pParent != 0)
	{
		pTemp = (i3TDKObjectBase *) pResFile->FindObjectByID( data.m_pParent);

		pTemp->AddChildToTail( this);
	}
	else if( data.m_pNext != 0)
	{
		pTemp = (i3TDKObjectBase *) pResFile->FindObjectByID( data.m_pNext);

		SetNext( pTemp);
	}

	return Result;
}

bool i3TDKObjectBase::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return true;
}
