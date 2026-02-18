#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Font.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Font, CMFCPropertyGridFontProperty)

#pragma warning(push)
#pragma warning(disable :4239) 
i3TDKPropertyGridProperty_Font::i3TDKPropertyGridProperty_Font( PROPERTYINFO* pInfo)
:	CMFCPropertyGridFontProperty( ToMFCString(pInfo->m_pszName), ToTLOGFONT(*((LPLOGFONTA)pInfo->m_pData)), CF_EFFECTS | CF_SCREENFONTS, 
								 ToTString(pInfo->m_pszDesc).c_str(), 0, RGB( ((COLOR*)pInfo->m_pUserData)->r, ((COLOR*)pInfo->m_pUserData)->g, ((COLOR*)pInfo->m_pUserData)->b)
								 )
{
	/*CMFCPropertyGridFontProperty(   const CString& strName,   LOGFONTA& lf,   DWORD dwFontDialogFlags = CF_EFFECTS | CF_SCREENFONTS,
										LPCTSTR lpszDescr = nullptr,   DWORD_PTR dwData = 0,   COLORREF color = (COLORREF)-1);*/

	m_pData = (LPLOGFONTA)pInfo->m_pData;
	m_pUserData = (COLOR*)pInfo->m_pUserData;
}

#pragma warning(pop)

void i3TDKPropertyGridProperty_Font::UpdateFromProperty( void* pData, void* pUserData)
{
	LPLOGFONT lf = GetLogFont();

	if( pData == nullptr)
		*m_pData = ToLOGFONTA(*lf);
	else
		*((LPLOGFONTA)pData) = ToLOGFONTA(*lf);

	//COLORREF cref = GetColor();

	/*
	if( pUserData == nullptr)
	{
		m_pUserData->r = cref & 0xff;
		m_pUserData->g = (cref >> 8) & 0xff;
		m_pUserData->b = (cref >> 16) & 0xff;
	}
	else
	{
		((COLOR*)pUserData)->r = cref & 0xff;
		((COLOR*)pUserData)->g = (cref >> 8) & 0xff;
		((COLOR*)pUserData)->b = (cref >> 16) & 0xff;
	}
	*/
}

void i3TDKPropertyGridProperty_Font::UpdateToProperty( void)
{
	if( m_pUserData != nullptr)
	{
		COLORREF col;
		col = RGB( m_pUserData->r, m_pUserData->g, m_pUserData->b);
		
	}
}

#endif