#include "stdafx.h"
#include "i3TDKPropertyString.h"

I3_CLASS_INSTANCE( i3TDKPropertyString);

i3TDKPropertyString::i3TDKPropertyString(void)
{
	SetStyle( I3TDK_PST_EDIT);
}

void i3TDKPropertyString::Get( char * pszValue, const INT32 nValueStrMax)
{
	i3::safe_string_copy( pszValue, m_str, nValueStrMax);
}

void i3TDKPropertyString::Set( const char * pszValue)
{
	m_str = pszValue; 
}

void i3TDKPropertyString::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	i3TDKProperty::CopyFrom( pOther, bRef);

	if( i3::same_of<i3TDKPropertyString*>(pOther))
	{
		i3TDKPropertyString * pIntOther = (i3TDKPropertyString *) pOther;

		Set( pIntOther->GetString());
	}
}

UINT32 i3TDKPropertyString::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 Length;

	Result = i3TDKProperty::OnSave( pResFile);

	// Length
	{
		Length = m_str.size();

		Rc = pStream->Write( &Length, sizeof(Length));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyString::OnSave()", "Could not write string length information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// String
	if( Length != 0 )
	{
		Rc = pStream->Write( m_str.c_str(), Length);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyString::OnSave()", "Could not write string data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TDKPropertyString::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 Length;

	Result = i3TDKProperty::OnLoad( pResFile);

	// Length
	{
		Rc = pStream->Read( &Length, sizeof(Length));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyString::OnLoad()", "Could not read string length information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Value
	if( Length > 0)
	{
		i3::rc_string temp_str(Length, 0);
		
		Rc = pStream->Read( const_cast<char*>(&temp_str[0]), Length);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyString::OnLoad()", "Could not read value data.");
			return STREAM_ERR;
		}
		Result += Rc;

		m_str = temp_str;
	}
	else
		m_str.clear();

	return Result;
}