// PopupListBox.cpp : implementation file
//
#include "stdafx.h"
#include "i3TDKPopupListBoxCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// i3TDKPopupListBoxCtrl

i3TDKPopupListBoxCtrl::i3TDKPopupListBoxCtrl()
{
	List.m_hWnd = nullptr;
}

BOOL i3TDKPopupListBoxCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT id)
{
	LPCTSTR lpszClass;
	BOOL Rv;
	RECT ERect;

	ParentWnd = pParentWnd;

	lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
									::LoadCursor( nullptr, IDC_ARROW));
	ASSERT( lpszClass != nullptr);

	Rv = CWnd::CreateEx( 0, lpszClass, _T(""), WS_POPUP, rect, pParentWnd, 0);
	ASSERT( Rv != 0);

	ERect.left = 0;
	ERect.top = 0;
	ERect.right = 1;
	ERect.bottom = 1;

	Rv = List.Create( dwStyle | WS_VISIBLE | WS_CHILD, ERect, this, id);
	ASSERT( Rv != FALSE);

	GetClientRect( &ERect);

	List.MoveWindow( &ERect, FALSE);

	if( dwStyle & WS_VISIBLE) 
		ShowWindow( SW_SHOW);

	return TRUE;
}

void i3TDKPopupListBoxCtrl::SetFont( CFont *pFont, BOOL bRedraw)
{
	List.SetFont( pFont, bRedraw);
}

int i3TDKPopupListBoxCtrl::GetCount(void)
{
	return List.GetCount();
}

int i3TDKPopupListBoxCtrl::GetHorizontalExtent(void)
{
	return List.GetHorizontalExtent();
}

int i3TDKPopupListBoxCtrl::GetText( int nIndex, LPTSTR strBuf)
{
	return List.GetText( nIndex, strBuf);
}

int i3TDKPopupListBoxCtrl::GetCurSel(void)
{
	return List.GetCurSel();
}

int i3TDKPopupListBoxCtrl::SetCurSel( int nSelect)
{
	return List.SetCurSel( nSelect);
}

int i3TDKPopupListBoxCtrl::AddString( LPCTSTR lpszText)
{
	return List.AddString( lpszText);
}

void i3TDKPopupListBoxCtrl::ResetContent(void)
{
	List.ResetContent();
}

int i3TDKPopupListBoxCtrl::SelectString( int nStartAfter, LPCTSTR lpszItem)
{
	return List.SelectString( nStartAfter, lpszItem);
}

BEGIN_MESSAGE_MAP(i3TDKPopupListBoxCtrl, CWnd)
	//{{AFX_MSG_MAP(i3TDKPopupListBoxCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


BOOL i3TDKPopupListBoxCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if( ParentWnd != nullptr) {
		::PostMessage( ParentWnd->m_hWnd, WM_COMMAND, wParam, lParam);
	}

	return true;
}


void i3TDKPopupListBoxCtrl::OnSize( UINT nType, int cx, int cy)
{
	RECT Rect;

	GetClientRect( &Rect);

	Rect.bottom += 1;
	if( List.m_hWnd != nullptr)
		List.MoveWindow( &Rect, TRUE);
}

void i3TDKPopupListBoxCtrl::OnSetFocus(CWnd* pOldWnd)
{
	List.SetFocus();
}
