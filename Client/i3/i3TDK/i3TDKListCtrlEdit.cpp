// i3TDKListCtrlEdit.cpp : implementation file
//
#include "stdafx.h"
#include "i3TDKListCtrlEdit.h"


// i3TDKListCtrlEdit

IMPLEMENT_DYNAMIC(i3TDKListCtrlEdit, CEdit)

BOOL i3TDKListCtrlEdit::Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID)
{
	if( IsWindow( m_hWnd))		DestroyWindow();

	dwStyle |= WS_CLIPCHILDREN | WS_BORDER | WS_CHILD | ES_AUTOHSCROLL;

	return CEdit::Create( dwStyle, rect, pParentWnd, ID);
}

void i3TDKListCtrlEdit::Excute( BOOL bBool, const TCHAR* pszInitText)
{
	HDC hdc = ::GetDC( m_hWnd);

	if( bBool)
	{
		m_bActive = bBool;

		CFont * pFont = GetParent()->GetFont();
		SetFont( pFont);

		SetBkColor( hdc, RGB( 255, 255, 255));
		SetBkMode( hdc, TRANSPARENT);

		if( pszInitText)
		{
			SetEditText( pszInitText);
		}		

		SetWindowText( GetEditText());

		if( m_nType & LCEB_FISRTSELECTION)		SetSel( 0, -1);

		ShowWindow( SW_SHOW);
		SetFocus();
	}
	else
	{
		ShowWindow( SW_HIDE);
		GetParent()->SetFocus();	// KillFocus
	}
}

void i3TDKListCtrlEdit::Init( LCEBInitStruct * pInitStruct)
{
	I3ASSERT( pInitStruct != nullptr);

	//	Create가 먼저 이루어져야 한다.
	if( ::IsWindow( m_hWnd) == FALSE)		
	{
		I3TRACE("[i3TDKListCtrlEdit::Init] error! Need to precede Edit window.");
		return;
	}

	m_nItem = pInitStruct->_nItem;
	m_nSubItem = pInitStruct->_nSubItem;	
	m_nType = pInitStruct->_nType;

	m_bActive = TRUE;

	SetEditText( pInitStruct->_pszInitText);

	SetWindowText( GetEditText());

	if( m_nType & LCEB_FISRTSELECTION)	
		SetSel( 0, -1);
}

void i3TDKListCtrlEdit::SetEditText( const TCHAR* pszText)
{
	m_strText = pszText;

	SetWindowText( pszText);
}

/*
void i3TDKListCtrlEdit::AllocBuffer( INT32 nSize, const char * pszSetText)
{
	while( m_nBufferSize < nSize)
	{
		m_nBufferSize *= 2;	//	크기를 2배 단위로 늘린다.
	}

	I3MEM_SAFE_FREE( m_pszText);
	m_pszText = (char *) i3MemAlloc( m_nBufferSize );

	if( pszSetText)
		i3::safe_string_copy( m_pszText, pszSetText, m_nBufferSize);
}
*/

BEGIN_MESSAGE_MAP(i3TDKListCtrlEdit, CEdit)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// i3TDKListCtrlEdit message handlers
BOOL i3TDKListCtrlEdit::PreTranslateMessage(MSG* pMsg)
{
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

int i3TDKListCtrlEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	//	부모 윈도우의 폰트와 같은 폰트를 사용한다.
	CFont * pFont = GetParent()->GetFont();
	SetFont( pFont);

	SetWindowText( GetEditText());
	SetFocus();
	SetSel( 0, -1);
	
	return 0;
}

void i3TDKListCtrlEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	
	//	Window는 제거된다.
	if( m_nType & LCEB_KILLFOCUSDESTROY)		DestroyWindow();

	m_bActive = FALSE;
}

void i3TDKListCtrlEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();

	m_bActive = FALSE;

}

void i3TDKListCtrlEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CSize size;
	HDC hdc = ::GetDC( m_hWnd);
	
	//	VK_ESCAPE는 변경된 값을 적용하지 않도록한다.
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		//	엔터키를 눌러 정상적으로 변경됬을 경우만 
		if( nChar == VK_RETURN)
		{				
			int len = GetWindowTextLength();
			m_strText.resize(len);
			
			GetWindowText( &m_strText[0], m_strText.size() + 1);	// 두번째인수는 버퍼크기의미에 가깝다.
			

			//	부모 윈도우에세 LVN_ENDLABELEDIT 이벤트를 보낸다.
			LV_DISPINFO		lvdi;
			lvdi.hdr.hwndFrom	= GetParent()->GetSafeHwnd();
			lvdi.hdr.idFrom		= GetDlgCtrlID();
			lvdi.hdr.code		= LVN_ENDLABELEDIT;

			lvdi.item.mask		= LVIF_TEXT;
			lvdi.item.iItem		= m_nItem;
			lvdi.item.iSubItem	= m_nSubItem;
			lvdi.item.pszText	= (TCHAR*) GetEditText();
			lvdi.item.cchTextMax= this->GetEditTextSize();

			GetParent()->SendMessage( WM_NOTIFY, 0, (LPARAM) &lvdi); 
		}		

		GetParent()->SetFocus();	// KillFocus
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
	
	//GetWindowText(str);
	//CWindowDC dc(this);
	CFont * pFont = GetParent()->GetFont();
	HFONT OldFont = (HFONT) SelectObject( hdc, pFont->GetSafeHandle());
	GetTextExtentPoint32( hdc, GetEditText(), GetEditTextSize(), &size);
	SelectObject( hdc, OldFont);
	//CFont * pFontDC = dc.SelectObject( pFont);
	//CSize size = dc.GetTextExtent( str);
	//dc.SelectObject( pFontDC);
	//size.cx += 5;

	//클라이언트 사각형 크기를 알아낸다.
	CRect rect, parentRect;
	GetClientRect( &rect);
	GetParent()->GetClientRect( &parentRect);

	//부모의 좌표로 사각형을 변환
	ClientToScreen( &rect);
	GetParent()->ScreenToClient( &rect);

	//컨트롤이 크기 변경될 필요가 있는지 체크하고 늘어날 공간이 있는지 체크
	if( size.cx > rect.Width())
	{
		if( size.cx + rect.left < parentRect.right)
		{
			rect.right = rect.left + size.cx;
		}
		else
		{
			rect.right = parentRect.right;
		}

		MoveWindow( &rect);
	}
}

void i3TDKListCtrlEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	//클라이언트 사각형 크기를 알아낸다.
	CSize size;
	CRect rect, parentRect;
	GetClientRect( &rect);
	GetParent()->GetClientRect( &parentRect);

	//부모의 좌표로 사각형을 변환
	ClientToScreen( &rect);
	GetParent()->ScreenToClient( &rect);

	//컨트롤이 크기 변경될 필요가 있는지 체크하고 늘어날 공간이 있는지 체크
	if( size.cx > rect.Width())
	{
		if( size.cx + rect.left < parentRect.right)
		{
			rect.right = rect.left + size.cx;
		}
		else
		{
			rect.right = parentRect.right;
		}

		MoveWindow( &rect);
	}
}
