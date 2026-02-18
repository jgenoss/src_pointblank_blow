#include "stdafx.h"
#include "i3TDKPropertyGridProperty_File.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_File, CMFCPropertyGridFileProperty)

i3TDKPropertyGridProperty_File::i3TDKPropertyGridProperty_File( PROPERTYINFO* pInfo)
:	CMFCPropertyGridFileProperty( ToMFCString(pInfo->m_pszName), TRUE, ToMFCString((const char*)(pInfo->m_pData)), nullptr, 0, 0, ToTString(pInfo->m_pszDesc).c_str(), 0/*(DWORD_PTR)((char*)(pInfo->m_pData))*/),
	m_nBufSize( (INT32)pInfo->m_nFlag), m_pData( (char*)(pInfo->m_pData))
{
	AllowEdit( FALSE);
}

void i3TDKPropertyGridProperty_File::OnClickButton( CPoint point)
{
	TCHAR szFilters[] = _T("모든파일(*.*)|*.*|");

	CFileDialog fileDlg(TRUE, _T("모든파일"), _T("*.*"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if( fileDlg.DoModal() == IDOK)
	{
		CString pszName = fileDlg.GetPathName();
#ifdef _UNICODE
		i3::utf16_to_mb(pszName, m_pData, m_nBufSize);
#else
		i3::safe_string_copy( m_pData, pszName, m_nBufSize);
#endif		
		SetValue( pszName);
	}

}

void i3TDKPropertyGridProperty_File::UpdateFromProperty( void* pData)
{
	// 왜 없지?? 
	/*_bstr_t bs = (_bstr_t)(_variant_t)m_varValue;
	
	i3::string_ncopy_nullpad( m_pData, bs, m_nBufSize-1); 

	SetValue( (_variant_t)m_pData);*/
}

void i3TDKPropertyGridProperty_File::UpdateToProperty( void)
{
	//SetValue( (_variant_t)m_pData);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_File_RcString, CMFCPropertyGridFileProperty)

i3TDKPropertyGridProperty_File_RcString::i3TDKPropertyGridProperty_File_RcString( PROPERTYINFO* pInfo)
:	CMFCPropertyGridFileProperty( ToMFCString(pInfo->m_pszName), TRUE, ToMFCString(reinterpret_cast<i3::rc_string*>(pInfo->m_pData)->c_str()), nullptr, 0, 0, ToTString(pInfo->m_pszDesc).c_str(), 0/*(DWORD_PTR)((char*)(pInfo->m_pData))*/),
	m_pData( reinterpret_cast<i3::rc_string*>(pInfo->m_pData) )
{
	AllowEdit( FALSE);
}

void i3TDKPropertyGridProperty_File_RcString::OnClickButton( CPoint point)
{
	TCHAR szFilters[] = _T("모든파일(*.*)|*.*|");

	CFileDialog fileDlg(TRUE, _T("모든파일"), _T("*.*"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if( fileDlg.DoModal() == IDOK)
	{
		CString pszName = fileDlg.GetPathName();

#ifdef _UNICODE
		i3::utf16_to_mb(pszName, *m_pData);
#else
		(*m_pData).assign( (LPCTSTR)pszName, pszName.GetLength());
#endif		
		SetValue( pszName);
	}

}

void i3TDKPropertyGridProperty_File_RcString::UpdateFromProperty( void* pData)
{
	// 왜 없지?? 
	/*_bstr_t bs = (_bstr_t)(_variant_t)m_varValue;
	
	i3::string_ncopy_nullpad( m_pData, bs, m_nBufSize-1); 

	SetValue( (_variant_t)m_pData);*/
}

void i3TDKPropertyGridProperty_File_RcString::UpdateToProperty( void)
{
	//SetValue( (_variant_t)m_pData);
}


#endif