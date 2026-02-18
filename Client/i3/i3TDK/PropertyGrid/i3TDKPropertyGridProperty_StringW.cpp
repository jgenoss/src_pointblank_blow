#include "stdafx.h"
#include "i3TDKPropertyGridProperty_StringW.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/algorithm/replace_all.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_StringW, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_StringW::i3TDKPropertyGridProperty_StringW(	PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)((WCHAR16*)(pInfo->m_pData)), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
	m_nBufSize( *((INT32*)pInfo->m_pUserData)), m_pData( (WCHAR16*)(pInfo->m_pData))
{
	i3MemFree( pInfo->m_pUserData);

	if( (pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0)
		Enable(FALSE);
}

void i3TDKPropertyGridProperty_StringW::UpdateFromProperty( void* pData)
{
	_bstr_t bs = (_bstr_t)(_variant_t)m_varValue;
	
	if( pData == nullptr)
		i3::string_ncopy_nullpad( m_pData, (const WCHAR16*)bs, m_nBufSize-1); 
	else
		i3::string_ncopy_nullpad( (WCHAR16*)pData, (const WCHAR16*)bs, m_nBufSize-1);

	SetValue( (_variant_t)m_pData);
}

void i3TDKPropertyGridProperty_StringW::UpdateToProperty( void)
{
	SetValue( (_variant_t)m_pData);
}


///////////////

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_RcStringW, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_RcStringW::i3TDKPropertyGridProperty_RcStringW(	PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(reinterpret_cast<i3::rc_wstring*>(pInfo->m_pData)->c_str()), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
m_strData( reinterpret_cast<i3::rc_wstring*>(pInfo->m_pData) )
{
	if( (pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0)
		Enable(FALSE);
}

void i3TDKPropertyGridProperty_RcStringW::UpdateFromProperty( void* pData)
{
	_bstr_t bs = (_bstr_t)(_variant_t)m_varValue;

	if( pData == nullptr)
		(*m_strData).assign( (const wchar_t*)bs, bs.length() );  
	else
		(*reinterpret_cast<i3::rc_wstring*>(pData)).assign( (const wchar_t*)bs, bs.length() );  

	//툴에서 개행이 적용되게 변경
	i3::wstring			  localString;
	localString = (*m_strData).c_str();
	i3::replace_all(localString, L"\\n", L"\n");

	m_strData->assign(localString);

	SetValue( (_variant_t) (*m_strData).c_str() );
}

void i3TDKPropertyGridProperty_RcStringW::UpdateToProperty( void)
{
	SetValue( (_variant_t) (*m_strData).c_str() );
}







#endif