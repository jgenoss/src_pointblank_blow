#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Bool.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Bool, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Bool::i3TDKPropertyGridProperty_Bool( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(*(bool*)(pInfo->m_pData)), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
	m_pData( (bool*)(pInfo->m_pData))
{
	m_bNoEdit = ((pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0) ? true : false;

	if( m_bNoEdit)
	{
		Enable( FALSE);
	}
}

void i3TDKPropertyGridProperty_Bool::UpdateFromProperty( void* pData)
{
	if( pData == nullptr)
	{
		*m_pData = (bool)(_variant_t)m_varValue;
	}
	else
	{
		*((bool*)pData) = (bool)(_variant_t)m_varValue;
	}
}

void i3TDKPropertyGridProperty_Bool::UpdateToProperty( void)
{
	SetValue( (_variant_t)(*m_pData));
}

#endif