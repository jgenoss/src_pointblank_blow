#include "stdafx.h"
#include "i3TDKPropertyUint.h"

I3_CLASS_INSTANCE( i3TDKPropertyUint);

i3TDKPropertyUint::i3TDKPropertyUint(void)
{
	SetStyle( I3TDK_PST_EDIT);
}

void i3TDKPropertyUint::Get( char * pszValue)
{
	sprintf( pszValue, "%d", m_Value);
}

void i3TDKPropertyUint::Set( const char * pszValue)
{
	m_Value = atoi( pszValue);
}

void i3TDKPropertyUint::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	i3TDKProperty::CopyFrom( pOther, bRef);

	if( i3::same_of<i3TDKPropertyUint*>(pOther))
	{
		i3TDKPropertyUint * pIntOther = (i3TDKPropertyUint *) pOther;

		m_Value = pIntOther->m_Value;
	}
}

UINT32 i3TDKPropertyUint::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnSave( pResFile);

	// Value
	{
		Rc = pStream->Write( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyUint::OnSave()", "Could not write value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TDKPropertyUint::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnLoad( pResFile);

	// Value
	{
		Rc = pStream->Read( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyUint::OnLoad()", "Could not read value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}