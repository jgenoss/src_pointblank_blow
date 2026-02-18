#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Real32.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Real32, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Real32::i3TDKPropertyGridProperty_Real32(	 PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(*(REAL32*)(pInfo->m_pData)), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0),
	m_pData( (REAL32*)(pInfo->m_pData))
{
	m_bHasRange = ((pInfo->m_nFlag & PROPERTYFLAG_HASRANGE) != 0) ? true : false;
	
	if( m_bHasRange)
	{
		REAL32* pRange = (REAL32*)(pInfo->m_pUserData);
		m_fMin = pRange[0];
		m_fMax = pRange[1];

		I3MEM_SAFE_FREE( pRange);
	}

	if( (pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0)
		Enable( FALSE);

	COleVariant var = GetValue();

	UpdateFromProperty();
}

i3TDKPropertyGridProperty_Real32::i3TDKPropertyGridProperty_Real32(	const CString& strName, const _variant_t& varValue,  DWORD_PTR dwData, LPCTSTR lpszDescr, 
										bool bRange, REAL32 fMin, REAL32 fMax)
:	i3TDKPropertyGridProperty( strName, varValue, lpszDescr, dwData, 0, 0, 0), 
	m_bHasRange( bRange), m_fMin( fMin), m_fMax( fMax)
{
	m_pData = NULL;
}

REAL32 i3TDKPropertyGridProperty_Real32::_GetMinMaxData( REAL32 fData)
{
	if( m_bHasRange)
	{
		if( fData > m_fMax)		return m_fMax;
		if( fData < m_fMin)		return m_fMin;
	}

	return fData;
}

void i3TDKPropertyGridProperty_Real32::UpdateFromProperty( void* pData)
{
	//_variant_t vt = (_variant_t)m_varValue;
	
	REAL32 val = _GetMinMaxData( (REAL32)(_variant_t)m_varValue);

	I3ASSERT( m_pData != NULL);

	if( pData == NULL)
		*m_pData = val;
	else
		*((REAL32*)pData) = val;

	SetValue( (_variant_t)val);
}

void i3TDKPropertyGridProperty_Real32::UpdateToProperty( void)
{
	I3ASSERT( m_pData != NULL);

	REAL32 val = _GetMinMaxData( *m_pData);

	*m_pData = val;

	SetValue( (_variant_t)val);
}

#endif