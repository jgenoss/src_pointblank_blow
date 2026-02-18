// PropertyPopupListBox.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyPopupListBox.h"


// CPropertyPopupListBox

IMPLEMENT_DYNAMIC(CPropertyPopupListBox, CWnd)

CPropertyPopupListBox::CPropertyPopupListBox()
{
		List.m_hWnd = NULL;
}

CPropertyPopupListBox::~CPropertyPopupListBox()
{
}


BEGIN_MESSAGE_MAP(CPropertyPopupListBox, CWnd)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


BOOL CPropertyPopupListBox::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT id)
{
	LPCTSTR lpszClass;
	BOOL Rv;
	RECT ERect;

	ParentWnd = pParentWnd;

	lpszClass = AfxRegisterWndClass( CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
									::LoadCursor( NULL, IDC_ARROW));
	ASSERT( lpszClass != NULL);

	Rv = CWnd::CreateEx( 0, lpszClass, "", WS_POPUP, rect, pParentWnd, 0);
	ASSERT( Rv != NULL);

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

void CPropertyPopupListBox::SetFont( CFont *pFont, BOOL bRedraw)
{
	List.SetFont( pFont, bRedraw);
}

int CPropertyPopupListBox::GetCount(void)
{
	return List.GetCount();
}

int CPropertyPopupListBox::GetHorizontalExtent(void)
{
	return List.GetHorizontalExtent();
}

int CPropertyPopupListBox::GetText( int nIndex, LPTSTR strBuf)
{
	return List.GetText( nIndex, strBuf);
}

int CPropertyPopupListBox::GetCurSel(void)
{
	return List.GetCurSel();
}

int CPropertyPopupListBox::SetCurSel( int nSelect)
{
	return List.SetCurSel( nSelect);
}

int CPropertyPopupListBox::AddString( LPCTSTR lpszText)
{
	return List.AddString( lpszText);
}

void CPropertyPopupListBox::ResetContent(void)
{
	List.ResetContent();
}

int CPropertyPopupListBox::SelectString( int nStartAfter, LPCTSTR lpszItem)
{
	return List.SelectString( nStartAfter, lpszItem);
}


// CPropertyPopupListBox message handlers

void CPropertyPopupListBox::OnSize(UINT nType, int cx, int cy)
{
	
	RECT Rect;

	GetClientRect( &Rect);

	Rect.bottom += 1;
	if( List.m_hWnd != NULL)
		List.MoveWindow( &Rect, TRUE);
	CWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
}

void CPropertyPopupListBox::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
}

BOOL CPropertyPopupListBox::OnCommand( WPARAM wParam, LPARAM lParam)
{
	if( ParentWnd != NULL) 
	{
		::PostMessage( ParentWnd->m_hWnd, WM_COMMAND, wParam, lParam);
	}

	return true;
}