#include "stdafx.h"
#include "i3TDKPropertyGridProperty_UINT32_BitFlag.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_UINT32_BitFlag, i3TDKPropertyGridProperty)

namespace
{
	inline bool IsFlag(UINT32 data, UINT32 bitMask)
	{
		return (data & bitMask) == bitMask;
	}
	inline bool IsFlag(const PROPERTYINFO* info)
	{
		return IsFlag(*((UINT32*)info->m_pData), (UINT32)info->m_pUserData);
	}

}

i3TDKPropertyGridProperty_UINT32_BitFlag::i3TDKPropertyGridProperty_UINT32_BitFlag( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(IsFlag(pInfo))
							   , ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
	m_pData( (UINT32*)(pInfo->m_pData)), m_bitMask((UINT32)pInfo->m_pUserData)
{
	m_bNoEdit = ((pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0) ? true : false;

	if( m_bNoEdit)
	{
		Enable( FALSE);
	}
}

void i3TDKPropertyGridProperty_UINT32_BitFlag::UpdateFromProperty( void* pData)
{
	bool bVarVal = (bool)(_variant_t)m_varValue;

	if( pData == nullptr)
	{
		if (bVarVal)
			*m_pData |= m_bitMask;
		else
			*m_pData &= ~m_bitMask;
	}
	else
	{
		if (bVarVal)
			*((UINT32*)pData) |= m_bitMask;
		else
			*((UINT32*)pData) &= ~m_bitMask;
	}
}

void i3TDKPropertyGridProperty_UINT32_BitFlag::UpdateToProperty( void)
{
	bool bFlag = IsFlag(*m_pData, m_bitMask);
	SetValue( (_variant_t)(bFlag));
}

#endif
