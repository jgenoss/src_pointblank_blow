// ComboxExForListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3RSCGen.h"
#include "ComboxExForListCtrl.h"

// CComboxExForListCtrl

IMPLEMENT_DYNAMIC(CComboxExForListCtrl, CComboBox)
CComboxExForListCtrl::CComboxExForListCtrl(INT32 nItem, INT32 nSubItem, CStringList *plstItems, INT32 nSel)
{
	m_nItem		= nItem;
	m_nSubItem	= nSubItem;

	m_lstItems.AddTail( plstItems);

	m_nSel		= nSel;
	m_bESC		= FALSE;
}

CComboxExForListCtrl::~CComboxExForListCtrl()
{
}


BEGIN_MESSAGE_MAP(CComboxExForListCtrl, CComboBox)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCbnCloseup)
END_MESSAGE_MAP()

// CComboxExForListCtrl message handlers


int CComboxExForListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CFont * pFont = GetParent()->GetFont();
	SetFont( pFont);

	INT32 i;
	POSITION pos;
	INT32 nCount = (INT32)m_lstItems.GetCount();

	for( i = 0; i< nCount; ++i)
	{
		pos = m_lstItems.FindIndex( i);
		AddString( m_lstItems.GetAt( pos));
	}

	SetFocus();
	SetCurSel( m_nSel);

	return 0;
}

void CComboxExForListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	CString str;
	GetWindowText( str);

	//부모에게 통지를 보낸다.
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom	= GetParent()->m_hWnd;
	dispinfo.hdr.idFrom		= GetDlgCtrlID();
	dispinfo.hdr.code		= LVN_ENDLABELEDIT;

	dispinfo.item.mask		= LVIF_TEXT;
	dispinfo.item.iItem		= m_nItem;
	dispinfo.item.iSubItem	= m_nSubItem;
	dispinfo.item.pszText	= m_bESC?	NULL: LPSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax= str.GetLength();

	m_nSel = GetCurSel();

	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM) &dispinfo);

	DestroyWindow();
}

void CComboxExForListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE)
			m_bESC = TRUE;
			
		GetParent()->SetFocus();
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CComboxExForListCtrl::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	// TODO: Add your message handler code here
	delete this;
}

void CComboxExForListCtrl::OnCbnCloseup()
{
	// TODO: Add your control notification handler code here
}

BOOL CComboxExForListCtrl::PreTranslateMessage(MSG* pMsg)
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

	return CComboBox::PreTranslateMessage(pMsg);
}
