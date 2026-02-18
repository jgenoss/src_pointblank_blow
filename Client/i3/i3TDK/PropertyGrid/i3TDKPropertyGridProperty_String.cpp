#include "stdafx.h"
#include "i3TDKPropertyGridProperty_String.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_String, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_String::i3TDKPropertyGridProperty_String(	PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)((char*)(pInfo->m_pData)), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
	m_nBufSize( *((INT32*)pInfo->m_pUserData)), m_pData( (char*)(pInfo->m_pData))
{
	i3MemFree( pInfo->m_pUserData);

	if( (pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0)
		Enable(FALSE);
}

void i3TDKPropertyGridProperty_String::UpdateFromProperty( void* pData)
{
	_bstr_t bs = (_bstr_t)(_variant_t)m_varValue;
	
	if( pData == nullptr)
		i3::string_ncopy_nullpad( m_pData, (const char*)bs, m_nBufSize-1); 
	else
		i3::string_ncopy_nullpad( (char*)pData, (const char*)bs, m_nBufSize-1);

	SetValue( (_variant_t)m_pData);
}

void i3TDKPropertyGridProperty_String::UpdateToProperty( void)
{
	SetValue( (_variant_t)m_pData);
}

///////////////

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_RcString, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_RcString::i3TDKPropertyGridProperty_RcString(	PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(reinterpret_cast<i3::rc_string*>(pInfo->m_pData)->c_str()), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
m_strData( reinterpret_cast<i3::rc_string*>(pInfo->m_pData) )
{
	if( (pInfo->m_nFlag & PROPERTYFLAG_NOEDIT) != 0)
		Enable(FALSE);
}

void i3TDKPropertyGridProperty_RcString::UpdateFromProperty( void* pData)
{
	_bstr_t bs = (_bstr_t)(_variant_t)m_varValue;

	if( pData == nullptr)
		(*m_strData).assign( (const char*)bs, bs.length() );  
	else
		(*reinterpret_cast<i3::rc_string*>(pData)).assign( (const char*)bs, bs.length() );  

	SetValue( (_variant_t) (*m_strData).c_str() );
}

void i3TDKPropertyGridProperty_RcString::UpdateToProperty( void)
{
	SetValue( (_variant_t) (*m_strData).c_str() );
}



#endif