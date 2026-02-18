#include "stdafx.h"
#include "i3TDKPropertyReal.h"

I3_CLASS_INSTANCE( i3TDKPropertyReal);

i3TDKPropertyReal::i3TDKPropertyReal(void)
{
	SetStyle( I3TDK_PST_EDIT);
}

void i3TDKPropertyReal::Get( char * pszValue)
{
	sprintf( pszValue, "%.5f", m_Value);
}

void i3TDKPropertyReal::Set( const char * pszValue)
{
	m_Value = (REAL32)atof( pszValue);
}

void i3TDKPropertyReal::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	i3TDKProperty::CopyFrom( pOther, bRef);

	if( i3::same_of<i3TDKPropertyReal*>(pOther))
	{
		i3TDKPropertyReal * pIntOther = (i3TDKPropertyReal *) pOther;

		m_Value = pIntOther->m_Value;
	}
}

UINT32 i3TDKPropertyReal::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnSave( pResFile);

	// Value
	{
		Rc = pStream->Write( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyReal::OnSave()", "Could not write value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TDKPropertyReal::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnLoad( pResFile);

	// Value
	{
		Rc = pStream->Read( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyReal::OnLoad()", "Could not read value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}