#include "stdafx.h"
#include "i3TDKProperty.h"

I3_CLASS_INSTANCE( i3TDKProperty);

void i3TDKProperty::Set( const TCHAR * /*pszValueStr*/)
{
	if((m_bNotifyToOwner == TRUE) && (m_pOwner != nullptr))
	{
		m_pOwner->OnChangeProperty( this);
	}
}

void i3TDKProperty::CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef)
{
	i3TDKPropertyContainer::CopyFrom( pOther, bRef);

	if( i3::kind_of<i3TDKProperty* >(pOther))
	{
		i3TDKProperty * pProperty = (i3TDKProperty *) pOther;

		m_Style = pProperty->m_Style;
	} 

	if( (m_bNotifyToOwner == TRUE) && (m_pOwner != nullptr))
	{
		m_pOwner->OnChangeProperty( this);
	}
}

i3TDKProperty & i3TDKProperty::operator = (const i3TDKProperty & /*pProperty*/)
{
	return *this;
}


void i3TDKProperty::PopupModalDialog(void)
{
	I3ASSERT_0;
}
