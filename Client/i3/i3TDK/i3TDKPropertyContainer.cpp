#include "stdafx.h"
#include "i3TDKPropertyContainer.h"
#include "i3TDKProperty.h"

I3_CLASS_INSTANCE( i3TDKPropertyContainer);

i3TDKPropertyContainer::~i3TDKPropertyContainer(void)
{
	DeleteAllProperties();
}

void i3TDKPropertyContainer::DeleteAllProperties(void)
{
	INT32 i;
	i3TDKProperty * pProperty;

	for( i = 0; i < GetPropertyCount(); i++)
	{
		pProperty = GetProperty( i);

		I3_MUST_RELEASE(pProperty);
	}

	m_PropertyList.clear();
}

void i3TDKPropertyContainer::AddProperty( i3TDKProperty * pProperty)
{
	I3_MUST_ADDREF(pProperty);

	m_PropertyList.push_back( pProperty);
}

void i3TDKPropertyContainer::RemoveProperty( INT32 Index)
{
	I3ASSERT( Index >= 0);
	I3ASSERT( Index < (INT32)m_PropertyList.size());

	m_PropertyList.erase( m_PropertyList.begin() + Index);
}

void i3TDKPropertyContainer::RemoveProperty( char * pszName)
{
	I3ASSERT( pszName != nullptr);

	i3TDKProperty * pProperty = FindProperty( pszName);

	I3ASSERT( pProperty != nullptr);

	i3::vu::remove(	m_PropertyList, pProperty);
}

void i3TDKPropertyContainer::RemoveProperty( i3TDKProperty * pProperty)
{
	I3ASSERT( pProperty != nullptr);

	i3::vu::remove(m_PropertyList, pProperty);
}


i3TDKProperty *	i3TDKPropertyContainer::FindProperty( const char * pszPropertyName)
{
	i3TDKProperty * pProperty;

	for( size_t i = 0; i < m_PropertyList.size(); i++)
	{
		pProperty = m_PropertyList[ i];

		if( i3::generic_is_iequal( pProperty->GetName(), pszPropertyName) )
		{
			return pProperty;
		}
	}

	return nullptr;
}

INT32 i3TDKPropertyContainer::GetPropertyIndex( i3TDKProperty * pProperty)
{
	size_t idx = i3::vu::index_of(m_PropertyList, pProperty);
	return ( idx != m_PropertyList.size() ) ? INT32(idx) : -1;
}

void i3TDKPropertyContainer::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	INT32 i;
	i3TDKProperty * pProperty, * pMyProperty;

	for( i = 0; i < pOther->GetPropertyCount(); i++)
	{
		pProperty = pOther->GetProperty( i);

		I3ASSERT( pProperty->hasName());

		pMyProperty = FindProperty( (char *) pProperty->GetName());

		if( pMyProperty)
		{
			// Reference Copy인지 그렇지 않은지에 대한 처리는 개별 Property에 따라
			// 다를 수 있다.
			pMyProperty->CopyFrom( pProperty, bRef);
		}
		#if defined( I3_DEBUG)
		else
		{
			i3Log( "i3TDKPropertyContainer::CopyFrom()", "Could not found corresponding property : %s\n", pProperty->GetName());
		}
		#endif
	}
}

// 하위 Property가 변경되었을 때, 그 사실을 알리기 위한 함수.
// 일반적으로 자식 Property들에서 자신의 Owner에게 자신의 변경사항을 알리기 위해 이 함수를 호출한다.
void i3TDKPropertyContainer::OnChangeProperty( i3TDKProperty * pProperty)
{
	INT32 i;
	i3TDKProperty * pTemp;

	for( i = 0; i < GetPropertyCount(); i++)
	{
		pTemp = GetProperty( i);

		pTemp->OnChangeProperty( pProperty);
	}
}


// Owner가 자신의 Property들에게 자신의 값이 변경되었기 때문에 각 Property들이 Update되어야 함을 알리기 위함 함수.
void i3TDKPropertyContainer::OnUpdateData(void)
{
	INT32 i;
	i3TDKProperty * pProperty;

	for( i = 0; i < GetPropertyCount(); i++)
	{
		pProperty = GetProperty( i);

		pProperty->OnUpdateData();
	}
}

// Override from i3PersistantElement
void i3TDKPropertyContainer::OnQueryStrings( i3::vector<i3::rc_string>& StrList)
{
	INT32 i;
	i3TDKProperty * pProperty;

	for( i = 0; i < GetPropertyCount(); i++)
	{
		pProperty = GetProperty( i);

		pProperty->OnQueryStrings( StrList);
	}

	i3NamedElement::OnQueryStrings( StrList);
}

UINT32 i3TDKPropertyContainer::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	UINT16 Count = (UINT16) GetPropertyCount();
	INT32 stridx, Sz;
	UINT32 PosSize, PosNext;
	i3Stream * pStream = pResFile->GetStream();
	i3TDKProperty * pProperty;
	INT32 i;

	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Property Count
	Rc = pStream->Write( &Count, sizeof(UINT16));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TDKPropertyContainer::OnSave()", "Could not write property count information.");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < Count; i++)
	{
        pProperty = m_PropertyList[i];

		if( pProperty->GetStyle() & I3TDK_PST_VOLATILE)
		{
			// PST_VOLATILE Style을 가진 Property는 저장할 필요가 없는 Property이기 때문에...
			continue;
		}

		// Property Name의 Index를 저장한다.
		{
			I3ASSERT( pProperty->hasName());
			size_t idx = i3::vu::index_of(pResFile->GetStringTable(), pProperty->GetName() );

			stridx = (idx != pResFile->GetStringTable().size()) ? INT32(idx) : -1;
				
			I3ASSERT( stridx != -1);

			Rc = pStream->Write( &stridx, sizeof(INT32));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TDKPropertyContainer::OnSave()", "Could not write property reference value.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		// Property가 저장되는 크기를 위한 공간 확보
		{
			// 현재 File Pointer 저장.
			PosSize = pStream->GetPosition();

			Sz = 0;
			Rc = pStream->Write( &Sz, sizeof(Sz));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TDKPropertyContainer::OnSave()", "Could not write property size value.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		// Property 저장
		{
			Sz = pProperty->OnSave( pResFile);
			if( Sz == STREAM_ERR)
			{
				return STREAM_ERR;
			}
			Result += Sz;

			// 현재 File Pointer 저장.
			PosNext = pStream->GetPosition();
		}

		// Proeprty Size 갱신
		{
			pStream->SetPosition( PosSize, STREAM_BEGIN);

			Rc = pStream->Write( &Sz, sizeof(Sz));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TDKPropertyContainer::OnSave()", "Could not write property size value.");
				return STREAM_ERR;
			}

			pStream->SetPosition( PosNext, STREAM_BEGIN);
		}
	}

	return Result;
}

UINT32 i3TDKPropertyContainer::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	UINT16 Count;
	i3TDKProperty * pProperty;
	INT32 stridx, Sz, i;
	UINT32 Pos;
	const char * pszPropertyName;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Property Count
	Rc = pStream->Read( &Count, sizeof(UINT16));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TDKPropertyContainer::OnLoad()", "Could not read property count information.");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < Count; i++)
	{
		// String Table에서 해당 Property의 이름의 Index가 저장되어 있다.
		{
			Rc = pStream->Read( &stridx, sizeof(INT32));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TDKPropertyContainer::OnLoad()", "Could not read property reference value.");
				return STREAM_ERR;
			}
			Result += Rc;

			I3ASSERT( stridx >= 0);

			Rc = pStream->Read( &Sz, sizeof(INT32));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TDKPropertyContainer::OnLoad()", "Could not read property size information.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		// Property의 이름을 구한다.
		pszPropertyName = pResFile->GetStringTable()[ stridx].c_str();

		// 이름으로 Property를 검색한다.
		pProperty = FindProperty( pszPropertyName);
		
		if((pProperty == nullptr) || (Sz == 0))
		{
			// Version Up에 따라 Property가 변경될 수 있기 때문에..
			// 허용 가능한 에러라고 판단한다.
			continue;
		}


		// Property Data를 읽어 들인다.
		{
			Pos = pStream->GetPosition();

			Rc = pProperty->OnLoad( pResFile);
			if( Rc == STREAM_ERR)
			{
				// Version Up에 따라 Property Data의 구조가 변경될 수 있기 때문에...
				// 허용 가능한 범위의 에러라고 판단한다.
			}

			pStream->SetPosition( Pos + Sz, STREAM_BEGIN);
		}
	}

	return Result;
}
