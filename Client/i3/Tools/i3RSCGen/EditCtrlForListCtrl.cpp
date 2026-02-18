// EditCtrlForListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3RSCGen.h"
#include "EditCtrlForListCtrl.h"
#include ".\editctrlforlistctrl.h"

// CEditCtrlForListCtrl

IMPLEMENT_DYNAMIC(CEditCtrlForListCtrl, CEdit)
CEditCtrlForListCtrl::CEditCtrlForListCtrl(INT32 nItem, INT32 nSubItem, CString strInit)
{
	m_nItem = nItem;
	m_nSubItem = nSubItem;
	m_strText.Append( strInit);

	m_bESC = FALSE;
}

CEditCtrlForListCtrl::~CEditCtrlForListCtrl()
{
}


BEGIN_MESSAGE_MAP(CEditCtrlForListCtrl, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
//	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CEditCtrlForListCtrl message handlers
BOOL CEditCtrlForListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		if( pMsg->wParam == VK_RETURN ||
			pMsg->wParam == VK_DELETE ||
			pMsg->wParam == VK_ESCAPE ||
			GetKeyState( VK_CONTROL))
		{
			::TranslateMessage( pMsg);
			::DispatchMessage( pMsg);
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}


void CEditCtrlForListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	CString str;
	GetWindowText( str);

	//컨트롤의 부모에게 통지보내기
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom	= GetParent()->m_hWnd;
	dispinfo.hdr.idFrom		= GetDlgCtrlID();
	dispinfo.hdr.code		= LVN_ENDLABELEDIT;

	dispinfo.item.mask		= LVIF_TEXT;
	dispinfo.item.iItem		= m_nItem;
	dispinfo.item.iSubItem	= m_nSubItem;
	dispinfo.item.pszText	= m_bESC? NULL: LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax= str.GetLength();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);
	
	DestroyWindow();
}

void CEditCtrlForListCtrl::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	delete this;
}

void CEditCtrlForListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE)
			m_bESC = TRUE;

		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
	
	CString str;

	GetWindowText(str);
	CWindowDC dc(this);
	CFont * pFont = GetParent()->GetFont();
	CFont * pFontDC = dc.SelectObject( pFont);
	CSize size = dc.GetTextExtent( str);
	dc.SelectObject( pFontDC);
	size.cx += 5;

	//클라이언트 사각형 크기를 알아낸다.
	CRect rect, parentrect;
	GetClientRect( &rect);
	GetParent()->GetClientRect( &parentrect);

	//부모의 좌표로 사각형을 변환
	ClientToScreen( &rect);
	GetParent()->ScreenToClient( &rect);

	//컨트롤이 크기 변경될 필요가 있는지 체크하고 늘어날 공간이 있는지 체크
	if( size.cx > rect.Width())
	{
		if( size.cx + rect.left < parentrect.right)
		{
			rect.right = rect.left + size.cx;
		}
		else
		{
			rect.right = parentrect.right;
		}

		MoveWindow( &rect);
	}
}

int CEditCtrlForListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CFont * pFont = GetParent()->GetFont();
	SetFont( pFont);

	SetWindowText( m_strText);
	SetFocus();
	SetSel( 0, -1);

	return 0;
}

