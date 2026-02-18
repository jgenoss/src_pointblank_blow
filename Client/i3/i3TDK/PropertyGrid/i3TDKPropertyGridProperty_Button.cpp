#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Button.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Button, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Button::i3TDKPropertyGridProperty_Button( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), (_variant_t)(char*)(pInfo->m_pData), ToTString(pInfo->m_pszDesc).c_str(), 0, 0, 0, 0), 
	m_pProc( (PROC_BUTTONPROPERTY)(pInfo->m_pUserData))
{
	//I3ASSERT( m_pProc != nullptr);

	const char* pBtnName = (const char*)(pInfo->m_pData);
	if( pBtnName[0] != 0)
	{
#ifdef _UNICODE
		i3::stack_wstring wstrBtnName;	i3::mb_to_utf16(pBtnName, wstrBtnName);
		m_strBtnName = wstrBtnName.c_str();
#else
		m_strBtnName = pBtnName;
#endif
	}
	else
	{
		m_strBtnName = _T("...");
	}
	
	AllowEdit( FALSE);
}

void i3TDKPropertyGridProperty_Button::AdjustButtonRect()
{
	i3TDKPropertyGridProperty::AdjustButtonRect();

	//m_rectButton.left -= m_rectButton.Width();
}

void i3TDKPropertyGridProperty_Button::OnClickButton(CPoint point)
{
	if( m_pProc == nullptr)
		AfxMessageBox( _T("No PROC_BUTTONPROPERTY is set") );
	else
		m_pProc();
}

void i3TDKPropertyGridProperty_Button::OnDrawButton(CDC* pDC, CRect rectButton)
{
	pDC->DrawText( m_strBtnName.c_str(), -1, &rectButton, DT_CENTER);
	
	CMFCToolBarButton button;
	CRect rect = rectButton;
	
	CMFCVisualManager::AFX_BUTTON_STATE state = CMFCVisualManager::ButtonsIsHighlighted;

	CMFCVisualManager::GetInstance()-> OnFillButtonInterior(pDC, &button, rect, state);

	CMFCVisualManager::GetInstance()-> OnDrawButtonBorder(pDC, &button, rect, state);
}

void i3TDKPropertyGridProperty_Button::UpdateFromProperty( void* pData)
{
}

#endif