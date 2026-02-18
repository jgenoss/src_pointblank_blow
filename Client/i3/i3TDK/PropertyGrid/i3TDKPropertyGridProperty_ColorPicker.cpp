#include "stdafx.h"

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty_ColorPicker.h"
#include "../i3TDKColorPicker.h"

#define AFX_PROP_HAS_BUTTON 0x0002

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_ColorPicker, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_ColorPicker::i3TDKPropertyGridProperty_ColorPicker( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName), COleVariant(_T("") ), ToTString(pInfo->m_pszDesc).c_str(), 0)
{
	m_dwFlags = AFX_PROP_HAS_BUTTON;

	m_pData = (COLOR*)pInfo->m_pData;
	
	Enable(TRUE);
}

i3TDKPropertyGridProperty_ColorPicker::~i3TDKPropertyGridProperty_ColorPicker(void)
{
	if( m_hBrush != nullptr)
	{
		::DeleteObject( m_hBrush);
		m_hBrush = nullptr;
	}
}

void i3TDKPropertyGridProperty_ColorPicker::UpdateColor(void)
{
	if( m_hBrush != nullptr)
	{
		::DeleteObject( m_hBrush);
		m_hBrush = nullptr;
	}
}

void i3TDKPropertyGridProperty_ColorPicker::OnDrawValue(CDC* pDC, CRect rect)
{
	CRect rt;
	HPEN hPen, hOldPen;
	HBRUSH hOldBrush;
	HDC dc = pDC->m_hDC;

	hPen = (HPEN) ::GetStockObject( BLACK_PEN);
	hOldPen = (HPEN) ::SelectObject( dc, hPen);

	if( m_hBrush == nullptr)
	{
		m_hBrush = CreateSolidBrush( RGB( m_pData->r, m_pData->g, m_pData->b));
	}

	hOldBrush = (HBRUSH) ::SelectObject( dc, m_hBrush);

	rt.left = rect.left + 2;
	rt.top = rect.top + 1;
	rt.right = rt.left + 32;
	rt.bottom = rect.bottom - 1;

	::Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

	CString conv;

	conv.Format( _T("%d, %d, %d, %d"), m_pData->r, m_pData->g, m_pData->b, m_pData->a);

	rt.left = rt.right + 3;
	rt.right = rect.right - 20;
	::DrawText( dc, (LPCTSTR) conv, -1, rt, DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	::SelectObject( dc, hOldPen);
	::SelectObject( dc, hOldBrush);
}

void i3TDKPropertyGridProperty_ColorPicker::OnClickButton(CPoint point)
{
	i3TDKColorPicker dlg;
	COLORREAL c;

	i3Color::Set( &c, m_pData);

	if( dlg.Execute( &c))
	{
		i3Color::Set( m_pData, dlg.getSelectedColor());

		UpdateColor();
	}
}

BOOL i3TDKPropertyGridProperty_ColorPicker::OnEdit(LPPOINT lptClick)
{
	return CMFCPropertyGridProperty::OnEdit( lptClick);
}

void i3TDKPropertyGridProperty_ColorPicker::AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)
{
	CMFCPropertyGridProperty::AdjustInPlaceEditRect( rectEdit, rectSpin);

	rectEdit.left += 32 + 2;
}

CString i3TDKPropertyGridProperty_ColorPicker::FormatProperty()
{
	CString str;

	str.Format( _T("%d, %d, %d, %d"), m_pData->r, m_pData->g, m_pData->b, m_pData->a);

	return str;
}

BOOL i3TDKPropertyGridProperty_ColorPicker::OnUpdateValue(void)
{
	CString strText;
	INT32 idx = 0;
	char szToken[256], sep;

	m_pWndInPlace->GetWindowText(strText);

	i3LineLexer lex;

	lex.SetMasterString( (char *) LPCTSTR( strText));

	while( lex.GetNextToken( szToken, &sep, ",") != -1)
	{
		switch( idx)
		{
			case 0 :		m_pData->r = (UINT8) MAX( 0, MIN( 255, atoi( szToken)));	break;
			case 1 :		m_pData->g = (UINT8) MAX( 0, MIN( 255, atoi( szToken)));	break;
			case 2 :		m_pData->b = (UINT8) MAX( 0, MIN( 255, atoi( szToken)));	break;
			case 3 :		m_pData->a = (UINT8) MAX( 0, MIN( 255, atoi( szToken)));	break;
		}

		idx++;
	}

	UpdateColor();

	m_pWndList->OnPropertyChanged(this);

	return TRUE;
}

void i3TDKPropertyGridProperty_ColorPicker::UpdateFromProperty( void* pData)
{
	// nullptr인 경우도 있다 : 자기 자신이 이미 변경되고 난 후인 경우...
	// 이 함수는 Multi-Select 되었을 때, 전면에 보여지는 Object 이 외의, 함께 선택된 Object의 Property 변경에
	// 중요하다. 즉, 이 함수가 구현되지 않으면, 함께 선택된 다른 Object들의 Property는 변경되지 않는다.
	if( pData != nullptr)
	{
		*(COLOR *) pData = *m_pData;
	}
}

#endif