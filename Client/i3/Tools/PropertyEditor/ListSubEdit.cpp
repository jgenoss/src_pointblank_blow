// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "ListSubEdit.h"

IMPLEMENT_DYNAMIC(CListSubEdit, CEdit)

CListSubEdit::CListSubEdit(int nItem, int nSubItem, LPCTSTR szpString)   
: m_nItem(nItem), m_nSubItem(nSubItem), m_String(szpString), m_bEnter(false)   
{
}

CListSubEdit::~CListSubEdit()   
{   
}   


BEGIN_MESSAGE_MAP(CListSubEdit, CEdit)   
	ON_WM_CREATE()   
	ON_WM_KILLFOCUS()   
	ON_WM_CHAR()   
	ON_WM_NCDESTROY()   
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)   
END_MESSAGE_MAP()   

BOOL CListSubEdit::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_RETURN:	case VK_ESCAPE:
			::TranslateMessage(pMsg);   
			::DispatchMessage(pMsg);
			return TRUE;
			break;
		case 'A':
			if( GetAsyncKeyState( VK_CONTROL ) )
				CEdit::SetSel(0, -1);
			break;

		case 'C':
			if (GetAsyncKeyState(VK_CONTROL))
			{
				CEdit::Copy();
				return 1;
			}
			break;
		case 'X':
			if (GetAsyncKeyState(VK_CONTROL))
			{
				CEdit::Cut();
				return 1;
			}
			break;
		case 'V':
			if (GetAsyncKeyState(VK_CONTROL))
			{
				CEdit::Paste();
				return 1;
			}
			break;
		}
		break;
	}

	return CEdit::PreTranslateMessage(pMsg);   
}   

int CListSubEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)   
{   
	if (CEdit::OnCreate(lpCreateStruct) == -1)   
		return -1;   

	CFont* pFont = GetParent()->GetFont();   
	SetFont(pFont);   

	SetWindowText( m_String );
	SetFocus();   

	return 0;   
}   

void CListSubEdit::OnKillFocus(CWnd* pNewWnd)   
{   
	CEdit::OnKillFocus(pNewWnd);

	CString strTemp;   
	GetWindowText(strTemp);   

	LV_DISPINFO dispinfo;   
	dispinfo.hdr.hwndFrom   = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom     = GetDlgCtrlID();
	dispinfo.hdr.code       = LVN_ENDLABELEDIT;

	dispinfo.item.mask      = LVIF_TEXT;   
	dispinfo.item.iItem     = m_nItem;   
	dispinfo.item.iSubItem  = m_nSubItem;   
	dispinfo.item.pszText   = m_bEnter ? LPTSTR((LPCTSTR)strTemp) : NULL;   
	dispinfo.item.cchTextMax= strTemp.GetLength();   

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}   

void CListSubEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)   
{   
	if(nChar == VK_ESCAPE || nChar == VK_RETURN)
	{   
		if(nChar == VK_RETURN)   
		{
			m_bEnter = true;
		}
		GetParent()->SetFocus();
		return;   
	}   

	CEdit::OnChar(nChar, nRepCnt, nFlags);   
}   

void CListSubEdit::OnNcDestroy()   
{   
	CEdit::OnNcDestroy();   

	delete this;   
}   

void CListSubEdit::OnCloseUp()   
{   
	GetParent()->SetFocus();   
}