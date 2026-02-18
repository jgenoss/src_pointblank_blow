#include "i3CommonType.h"
#include "i3NamedElement.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Memory.h"
#include "i3String.h"
#include "i3Macro.h"

I3_CLASS_INSTANCE( i3NamedElement, i3PersistantElement);

i3NamedElement::i3NamedElement(void)
{
	/*
	m_pszName			= NULL;
	m_nNameBufSize		= 0;
	m_bOptimizeNameBuf	= FALSE;
	*/
	m_szName[0] = 0;
}

i3NamedElement::~i3NamedElement(void)
{
	//_FreeNameBuf();
}

/*
void i3NamedElement::_FreeNameBuf()
{
	if( m_pszName != NULL)
	{
		i3MemFree( m_pszName);
		m_pszName = NULL;
		m_nNameBufSize = 0;
	}
}

void i3NamedElement::_SetName( const char * pszName )
{
	_FreeNameBuf();

	if(pszName == NULL )return; 
	if(strlen(pszName) == 0)return; 

	I3ASSERT( m_pszName == NULL );

	m_nNameBufSize = i3String::Length( pszName ) + 1;
	I3ASSERT( m_nNameBufSize > 0 );

	m_pszName = (char *) i3MemAlloc( m_nNameBufSize );
	I3ASSERT( m_pszName != NULL);

	i3String::Copy( m_pszName, pszName );

}

void i3NamedElement::SetNameBufSize( UINT32 nSize )
{
	if( nSize > m_nNameBufSize )
	{
		_FreeNameBuf();

		m_nNameBufSize = nSize;

		m_pszName = (char *) i3MemAlloc( m_nNameBufSize );
		I3ASSERT( m_pszName != NULL);

		m_pszName[0] = 0;
	}
}
*/

void i3NamedElement::SetName( const char * pszName )
{
	/*
	if( m_bOptimizeNameBuf )
	{
		if( m_pszName == NULL )
		{	// 기존 버퍼가 없는 경우, 메모리 할당 후 복사.
			_SetName( pszName );
		}
		else
		{	// 기존 버퍼가 있음.
			if( pszName == NULL )
			{	// 입력할려는 이름이 NULL일 경우, ""로 설정
				m_pszName[0] = 0;
			}
			else
			{
				UINT32 nNewBufSize = i3String::Length( pszName ) + 1;
				if( nNewBufSize > m_nNameBufSize )
				{	// 기존 버퍼보다 큰 버퍼가 필요한 경우, 메모리 재할당 후 복사
					_SetName( pszName );
				}
				else
				{	// 기존 버퍼가 충분히 큰 경우, 그냥 복사
					i3String::Copy( m_pszName, pszName );
				}
			}
		}
	}
	else
	{
		// 메모리 재할당 후 복사
		_SetName( pszName );
	}
	*/

	INT32 iSize = sizeof(m_szName);
	strncpy(m_szName, pszName, iSize);
}

void i3NamedElement::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	i3NamedElement * pNamedObj = (i3NamedElement *) pDest;

	if( hasName())
	{
		pNamedObj->SetName( GetName());
	}
}

void i3NamedElement::OnQueryStrings( i3StringList * pStrList)
{
	i3PersistantElement::OnQueryStrings( pStrList);

	/*
	if( m_pszName != NULL)
	{
		pStrList->Add( m_pszName);
	}
	*/
	if( m_szName[0] != 0)
	{
		pStrList->Add( m_szName);
	}
}

UINT32	i3NamedElement::OnSave( i3ResourceFile * pResFile)
{
	UINT8	Length;
	UINT32 Rc;

	/*
	if( m_pszName == NULL)
	{
		Length = 0;
	}
	else
	*/
	{
		Length = (UINT8) i3String::Length( m_szName);
	}

	// write length field.
	Rc = pResFile->GetStream()->Write( &Length, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NamedElement::OnSave()", "Could not write string length field.");
		return STREAM_ERR;
	}

	if( Length > 0)
	{
		Rc = pResFile->GetStream()->Write( m_szName, Length);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NamedElement::OnSave()", "Could not write name string.");
			return STREAM_ERR;
		}
	}

	return Length + sizeof(Length);
}

UINT32	i3NamedElement::OnLoad( i3ResourceFile * pResFile)
{
	UINT8 Length;
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &Length, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NamedElement::OnLoad()", "Could not read string length field.");
		return STREAM_ERR;
	}

	if( Length > 0)
	{
		char szTemp[260];

		Rc = pResFile->GetStream()->Read( szTemp, Length);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NamedElement::OnLoad()", "Could not read name string.");
			return STREAM_ERR;
		}

		szTemp[Length] = 0;

		SetName( szTemp);
	}

	return Length + sizeof(Length);
}

bool i3NamedElement::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( hasName())
		pXML->SetAttribute( "Name", GetName());
	else
		pXML->SetAttribute( "Name", "");

	return true;
}

bool i3NamedElement::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	const char * pszName = pXML->Attribute( "Name");

	if( (pszName != NULL) && (pszName[0] != 0))
	{
		SetName( pszName);
	}

	return true;
}

#if defined( I3_DEBUG)
void	i3NamedElement::_getInfoString( char * pszStr, const INT32 nStrMax )
{
	if( hasName())
		i3String::Copy( pszStr, GetName(), nStrMax);
	else
		i3ElementBase::_getInfoString( pszStr, nStrMax);
}

#endif
