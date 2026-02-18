#include "stdafx.h"
#include "i3TDKPropertyBool.h"

I3_CLASS_INSTANCE( i3TDKPropertyBool);

i3TDKPropertyBool::i3TDKPropertyBool(void)
{
	SetStyle( I3TDK_PST_EDIT | I3TDK_PST_LIST);
}

void i3TDKPropertyBool::Get( char * pszValue, const INT32 nValueStrMax)
{
	i3::safe_string_copy( pszValue, m_Value ? "True" : "False", nValueStrMax);
}

void i3TDKPropertyBool::Set( const char * pszValue)
{
	if( i3::generic_is_iequal( pszValue, "TRUE") )
	{
		m_Value = TRUE;
	}
	else if( i3::generic_is_iequal( pszValue, "FALSE") )
	{
		m_Value = FALSE;
	}
	else
	{
		if( atoi( pszValue) == 0)
			m_Value = FALSE;
		else
			m_Value = TRUE;
	}
}

void i3TDKPropertyBool::GetValueList( i3::vector<i3::rc_string>& List)
{
	List.push_back("TRUE");
	List.push_back("FALSE");
}

void i3TDKPropertyBool::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	i3TDKProperty::CopyFrom( pOther, bRef);

	if( i3::same_of<i3TDKPropertyBool*>(pOther))
	{
		i3TDKPropertyBool * pIntOther = (i3TDKPropertyBool *) pOther;

		m_Value = pIntOther->m_Value;
	}
}

UINT32 i3TDKPropertyBool::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnSave( pResFile);

	// Value
	{
		Rc = pStream->Write( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyBool::OnSave()", "Could not write value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TDKPropertyBool::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TDKProperty::OnLoad( pResFile);

	// Value
	{
		Rc = pStream->Read( &m_Value, sizeof(m_Value));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TDKPropertyBool::OnLoad()", "Could not read value data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}