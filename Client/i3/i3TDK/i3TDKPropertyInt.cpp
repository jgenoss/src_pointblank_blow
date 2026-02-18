#include "stdafx.h"
#include "i3TDKPropertyInt.h"

I3_CLASS_INSTANCE( i3TDKPropertyInt);

i3TDKPropertyInt::i3TDKPropertyInt(void)
{
	SetStyle( I3TDK_PST_EDIT);
}

void i3TDKPropertyInt::Get( char * pszValue)
{
	sprintf( pszValue, "%d", m_Value);
}

void i3TDKPropertyInt::Set( const char * pszValue)
{
	m_Value = atoi( pszValue);
}

void i3TDKPropertyInt::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	i3TDKProperty::CopyFrom( pOther, bRef);

	if( i3::same_of<i3TDKPropertyInt*>(pOther))
	{
		i3TDKPropertyInt * pIntOther = (i3TDKPropertyInt *) pOther;

		m_Value = pIntOther->m_Value;
	}
}

UINT32 i3TDKPropertyInt::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnSave( pResFile);

	// Value
	{
		Rc = pStream->Write( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyInt::OnSave()", "Could not write value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TDKPropertyInt::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnLoad( pResFile);

	// Value
	{
		Rc = pStream->Read( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyInt::OnLoad()", "Could not read value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}