// PropertyEditBox.cpp : implementation file
//

#include "stdafx.h"
#include "propertyeditbox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertyEditBox

CPropertyEditBox::CPropertyEditBox()
{
}

CPropertyEditBox::~CPropertyEditBox()
{
}

//void CPropertyEditBox::SetProperty( CProperty * Property)
void CPropertyEditBox::SetProperty(INT8* SetValue)
{
	char Buf[256];
	if (SetValue == NULL)	ASSERT(0);							//Error
	else					strcpy(Buf, (const char*)SetValue);//Write
	SetWindowText(Buf);
}

void CPropertyEditBox::GetProperty(INT8* GetValue)
{
	char Buf[64];
	GetWindowText( Buf, sizeof(Buf) - 1);	//Read
	strcpy((char*)GetValue, (const char *)Buf);	//Write
}

void CPropertyEditBox::ClearSelect(void)
{	
//	CurProperty = NULL;
	SetModify( FALSE);
}

BEGIN_MESSAGE_MAP(CPropertyEditBox, CEdit)
	//{{AFX_MSG_MAP(CPropertyEditBox)
	ON_WM_NCCALCSIZE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyEditBox message handlers

void CPropertyEditBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	lpncsp->rgrc[0].left   += 4;
	lpncsp->rgrc[0].top    += 2;
	lpncsp->rgrc[0].right  -= 2;
	lpncsp->rgrc[0].bottom -= 0;
}

void CPropertyEditBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	RECT Rect;

	GetWindowRect( &Rect);

	point.x += Rect.left;
	point.y += Rect.top;

	GetParent()->ScreenToClient( &point);

	::PostMessage( GetParent()->m_hWnd, WM_LBUTTONDBLCLK, (WPARAM)nFlags, MAKELONG( point.x, point.y));
	//CEdit::OnLButtonDblClk(nFlags, point);
}
