#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Combo.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Combo, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Combo::i3TDKPropertyGridProperty_Combo( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(char*)(nullptr), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
	m_OptionList( *static_cast<const i3::vector<i3::rc_string>*>(pInfo->m_pUserData)), m_pData( (INT32*)(pInfo->m_pData))
{
	
	/*I3ASSERT( *m_pData >= 0 && *m_pData < m_pOptionList->GetCount());*/

	for( size_t i=0; i< m_OptionList.size(); i++)
	{
#ifdef _UNICODE
		i3::stack_wstring wstrOption;	i3::mb_to_utf16(m_OptionList[i], wstrOption);
		const wchar_t* szOption = wstrOption.c_str();
#else
		const char* szOption =  m_OptionList[i].c_str();
#endif
		AddOption( szOption );
	}

	if( (*m_pData >= 0) && (*m_pData < (INT32) m_OptionList.size()))
	{
#ifdef _UNICODE
		i3::stack_wstring wstrOption;	i3::mb_to_utf16(m_OptionList[*m_pData], wstrOption);
		const wchar_t* szOption = wstrOption.c_str();
#else
		const char* szOption =  m_OptionList[*m_pData].c_str();
#endif
		SetValue( szOption );
	}
	else
		SetValue( _T("") );

	bool bNoEdit = ((pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0) ? true : false;

	if( bNoEdit)
		Enable( FALSE);

	AllowEdit( FALSE);
}

INT32 i3TDKPropertyGridProperty_Combo::_getIndex( const char* pszOption)
{
	for( size_t i=0; i< m_OptionList.size(); i++)
	{
		if( i3::generic_is_iequal( m_OptionList[i], pszOption) )
			return i;
	}

	return -1;
}

void i3TDKPropertyGridProperty_Combo::UpdateFromProperty( void* pData)
{
	_variant_t vt = (_variant_t)m_varValue;
	_bstr_t bs = (_bstr_t)vt;
	
	char conv[MAX_PATH];
	i3::string_ncopy_nullpad( conv, (const char*)bs, sizeof( conv));
	INT32 idx = _getIndex( conv);

	I3ASSERT( idx != -1);

	if( pData == nullptr)
		*m_pData = idx;
	else
		*((INT32*)pData) = idx;
}

#endif