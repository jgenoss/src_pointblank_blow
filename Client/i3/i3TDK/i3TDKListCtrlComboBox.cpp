// i3TDKListCtrlComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKListCtrlComboBox.h"


// i3TDKListCtrlComboBox

IMPLEMENT_DYNAMIC(i3TDKListCtrlComboBox, CComboBox)

BOOL i3TDKListCtrlComboBox::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID)
{
	if( IsWindow( m_hWnd))		DestroyWindow();

	dwStyle |= WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | WS_VSCROLL;

	return CComboBox::Create( dwStyle, rect, pParentWnd, ID);
}	

void i3TDKListCtrlComboBox::Init( LCCBInitStruct * pInitStruct)
{
	I3ASSERT( pInitStruct != nullptr);

	//	Create가 먼저 이루어져야 한다.
	if( ::IsWindow( m_hWnd) == FALSE)		
	{
		I3TRACE("[i3TDKListCtrlComboBox::Init] error! Need to precede ComboBox window.");
		return;
	}

	ClearElement();	

	m_nItem = pInitStruct->_nItem;
	m_nSubItem = pInitStruct->_nSubItem;
	m_nType = pInitStruct->_nType;

	m_bActive = TRUE;

}

BOOL i3TDKListCtrlComboBox::PreTranslateMessage(MSG* pMsg)
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

BEGIN_MESSAGE_MAP(i3TDKListCtrlComboBox, CComboBox)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCbnCloseup)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnCbnSelchange)
END_MESSAGE_MAP()



// i3TDKListCtrlComboBox message handlers


int i3TDKListCtrlComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CFont * pFont = GetParent()->GetFont();	
	SetFont( pFont);

	return 0;
}

void i3TDKListCtrlComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//	VK_ESCAPE는 취소 시킨다.
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		GetParent()->SetFocus();	// KillFocus
		return;
	}

	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void i3TDKListCtrlComboBox::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	//	Window는 제거된다.
	if( m_nType & LCCB_KILLFOCUSDESTROY)		DestroyWindow();

	m_bActive = FALSE;
}

void i3TDKListCtrlComboBox::OnNcDestroy()
{
	CComboBox::OnNcDestroy();

	m_bActive = FALSE;	
}

void i3TDKListCtrlComboBox::OnCbnCloseup()
{
	// TODO: Add your control notification handler code here
}

void i3TDKListCtrlComboBox::OnCbnSelchange()
{
	//	선택된 아이템
	TCHAR szText[256] = _T("");
	GetSelectedElement( szText);

	//	부모 윈도우에세 LVN_ENDLABELEDIT 이벤트를 보낸다.
	LV_DISPINFO		lvdi;
	lvdi.hdr.hwndFrom	= GetParent()->GetSafeHwnd();
	lvdi.hdr.idFrom		= GetDlgCtrlID();
	lvdi.hdr.code		= LVN_ENDLABELEDIT;

	lvdi.item.mask		= LVIF_TEXT;
	lvdi.item.iItem		= m_nItem;
	lvdi.item.iSubItem	= m_nSubItem;
	lvdi.item.pszText	= szText;
	lvdi.item.cchTextMax = i3::generic_string_size( szText);

	GetParent()->SendMessage( WM_NOTIFY, 0, (LPARAM) &lvdi); 

	GetParent()->SetFocus();	// KillFocus
}
