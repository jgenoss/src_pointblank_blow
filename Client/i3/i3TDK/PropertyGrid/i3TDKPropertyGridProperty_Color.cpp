#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Color.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Color, CMFCPropertyGridColorProperty)

i3TDKPropertyGridProperty_Color::i3TDKPropertyGridProperty_Color( PROPERTYINFO* pInfo)
:	CMFCPropertyGridColorProperty( ToMFCString(pInfo->m_pszName), RGB(0, 0, 0), nullptr, ToTString(pInfo->m_pszDesc).c_str(), 0)
{
	m_pData = (COLOR*)pInfo->m_pData;
	COLORREF cref = RGB( m_pData->r, m_pData->g, m_pData->b);
	SetColor( cref);

	EnableOtherButton( _T("Other"), FALSE);

}

void i3TDKPropertyGridProperty_Color::UpdateFromProperty( void* pData)
{
	COLORREF cref = GetColor();
	
	UINT8 r = cref & 0xff;
	UINT8 g = (cref >> 8) & 0xff;
	UINT8 b = (cref >> 16) & 0xff;

	if( pData == nullptr)
	{
		m_pData->r = r;
		m_pData->g = g;
		m_pData->b = b;
	}
	else
	{
		((COLOR*)pData)->r = r;
		((COLOR*)pData)->g = g;
		((COLOR*)pData)->b = b;
	}
}

void i3TDKPropertyGridProperty_Color::UpdateToProperty( void)
{
	SetColor( RGB(m_pData->r, m_pData->g, m_pData->b));
}

#endif