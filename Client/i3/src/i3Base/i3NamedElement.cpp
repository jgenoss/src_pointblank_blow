#include "i3CommonType.h"
#include "i3NamedElement.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Memory.h"
#include "i3String.h"
#include "i3Macro.h"
#include "i3Base/string/ext/safe_string_copy.h"

I3_CLASS_INSTANCE( i3NamedElement);

void i3NamedElement::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	if( hasName())
	{
		i3NamedElement * pNamedObj = (i3NamedElement *) pDest;

		pNamedObj->SetName( GetNameString());
	}
}

void i3NamedElement::OnQueryStrings( i3::vector<i3::rc_string>& StrList)
{
	i3PersistantElement::OnQueryStrings( StrList);

	if ( !m_Name.empty() )
		StrList.push_back(m_Name);
}

UINT32	i3NamedElement::OnSave( i3ResourceFile * pResFile)
{
	// 범위점검 혹은 자름루틴이 필요할수도 있으나...귀찮아서 우선 스킵...(2012.11.13.수빈)
	UINT8	Length = static_cast<UINT8>(m_Name.length());
	UINT32 Rc;
	

	// write length field.
	Rc = pResFile->GetStream()->Write( &Length, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NamedElement::OnSave()", "Could not write string length field.");
		return STREAM_ERR;
	}

	if( Length > 0)
	{

		Rc = pResFile->GetStream()->Write( m_Name.c_str(), Length);

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

	if( (pszName != nullptr) && (pszName[0] != 0))
	{
		SetName( pszName);
	}

	return true;
}

#if defined( I3_DEBUG)
void	i3NamedElement::_getInfoString( char * pszStr, const INT32 nStrMax )
{
	if( hasName())
		i3::safe_string_copy( pszStr, GetNameString(), nStrMax);
	else
		i3ElementBase::_getInfoString( pszStr, nStrMax);
}

#endif
