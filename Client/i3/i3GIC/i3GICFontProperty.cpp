#include "stdafx.h"
#include "i3GICFontProperty.h"
#include "i3GIC.h"

#if defined( I3_DEBUG)

IMPLEMENT_DYNAMIC(i3GICFontProperty, CMFCPropertyGridFontProperty)

i3GICFontProperty::i3GICFontProperty(const CString& strName, LOGFONT& lf, DWORD dwFontDialogFlags /*= CF_EFFECTS | CF_SCREENFONTS*/,
				LPCTSTR lpszDescr /*= NULL*/, DWORD_PTR dwData /*= 0*/, COLORREF color /*= (COLORREF)-1*/ )
				: CMFCPropertyGridFontProperty( strName, lf, dwFontDialogFlags, lpszDescr, dwData, color)
{
	m_pShape = NULL;
}

i3GICFontProperty::~i3GICFontProperty()
{
}

void i3GICFontProperty::UpdateChangeValue( void)
{
	I3ASSERT( m_pShape != NULL);
	LPLOGFONT lf = GetLogFont();

	((i3GICShapeTextRect*)m_pShape)->setFont( (const char *) lf->lfFaceName);
}

/*virtual*/ BOOL i3GICFontProperty::OnUpdateValue( void)
{
	BOOL bRv = CMFCPropertyGridFontProperty::OnUpdateValue();

	if( m_pShape != NULL)
		UpdateChangeValue();

	return bRv;
}


#endif	// I3_DEBUG

