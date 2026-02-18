#include "i3MathType.h"
#include "i3MatrixObjectList.h"


I3_CLASS_INSTANCE( i3MatrixObjectList);

i3MatrixObjectList::~i3MatrixObjectList(void)
{
	INT32 i;
	i3MatrixObject * p;

	for( i = 0; i < GetCount(); i++)
	{
		p = Get( i );
		I3_SAFE_RELEASE( p);
	}

	m_List.clear();
}

void i3MatrixObjectList::Set( INT32 idx, i3MatrixObject * pObj)
{
	I3_REF_CHANGE( m_List[ idx], pObj);
}

void i3MatrixObjectList::Add( i3MatrixObject * pObj)
{
	I3_MUST_ADDREF( pObj);
	m_List.push_back( pObj);
}

void i3MatrixObjectList::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
//	pList->AddFromList( pList);			// 실수 버그성으로 보이며, 다음과 같이 고친다...(2012.04.24.수빈)
	List.insert(List.end(), m_List.begin(), m_List.end());

	i3PersistantElement::OnBuildObjectList( List);
}

UINT32 i3MatrixObjectList::OnSave( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Rc, Result = 0;
	OBJREF id;
	i3Stream * pStream = pResFile->GetStream();

	// Count
	{
		i = GetCount();

		Rc = pStream->Write( &i, sizeof(INT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MatrixObjectList::OnSave()", "Could not write matrix object count.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0; i < GetCount(); i++)
	{
		id = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement  *) Get( i));

		Rc = pStream->Write( &id, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MatrixObjectList::OnSave()", "Could not write matrix object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3MatrixObjectList::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Rc, Result = 0;
	OBJREF id;
	i3Stream * pStream = pResFile->GetStream();

	// Count
	{
		Rc = pStream->Read( &i, sizeof(INT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MatrixObjectList::OnLoad()", "Could not read object matrix.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	SetCount( i);

	for( i = 0; i < GetCount(); i++)
	{
		Rc = pStream->Read( &id, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MatrixObjectList::OnLoad()", "Could not read matrix object reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		{
			i3MatrixObject * pObj;

			pObj = (i3MatrixObject *) pResFile->FindObjectByID( id);
			I3ASSERT( pObj != nullptr);

			Set( i, pObj);
		}
	}

	return Result;
}
