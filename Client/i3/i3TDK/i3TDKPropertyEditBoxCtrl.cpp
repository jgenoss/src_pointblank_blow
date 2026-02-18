// PropertyEditBox.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKPropertyEditBoxCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// i3TDKPropertyEditBoxCtrl

void i3TDKPropertyEditBoxCtrl::SetProperty( i3TDKProperty * Property)
{
	TCHAR Buf[256];

	if((GetModify() == TRUE) && (CurProperty != nullptr)) {
		GetWindowText( Buf, sizeof(Buf)/sizeof(TCHAR) - 1);
		CurProperty->Set( Buf);
	}

	CurProperty = Property;
	CurProperty->Get( Buf, 256);

	SetWindowText( Buf);
}

void i3TDKPropertyEditBoxCtrl::ClearSelect(void)
{
	CurProperty = nullptr;
	SetModify( FALSE);
}

BEGIN_MESSAGE_MAP(i3TDKPropertyEditBoxCtrl, CEdit)
	//{{AFX_MSG_MAP(i3TDKPropertyEditBoxCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// i3TDKPropertyEditBoxCtrl message handlers

void i3TDKPropertyEditBoxCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	lpncsp->rgrc[0].left   += 4;
	lpncsp->rgrc[0].top    += 2;
	lpncsp->rgrc[0].right  -= 2;
	lpncsp->rgrc[0].bottom -= 0;
}

void i3TDKPropertyEditBoxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	RECT Rect;

	GetWindowRect( &Rect);

	point.x += Rect.left;
	point.y += Rect.top;

	GetParent()->ScreenToClient( &point);

	::PostMessage( GetParent()->m_hWnd, WM_LBUTTONDBLCLK, (WPARAM)nFlags, MAKELONG( point.x, point.y));
	//CEdit::OnLButtonDblClk(nFlags, point);
}
