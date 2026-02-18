// i3EditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "i3EditCtrl.h"
#include ".\i3editctrl.h"


// i3EditCtrl

IMPLEMENT_DYNAMIC(i3EditCtrl, CEdit)
i3EditCtrl::i3EditCtrl()
{
	m_bFloatMode = FALSE;
}

i3EditCtrl::~i3EditCtrl()
{
}


void i3EditCtrl::SetFont( HFONT hFont)
{
	CFont font;

	font.Attach( hFont);

	CWnd::SetFont( &font);

	font.Detach();
}

BEGIN_MESSAGE_MAP(i3EditCtrl, CEdit)
	ON_WM_KEYDOWN()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// i3EditCtrl message handlers


void i3EditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
		case  VK_RETURN :
			{
				NMHDR hdr;

				hdr.code = NM_RETURN;
				hdr.hwndFrom = m_hWnd;
				hdr.idFrom = GetWindowLong( m_hWnd, GWL_ID);

				GetParent()->SendNotifyMessage( WM_NOTIFY, (WPARAM) hdr.idFrom, (LPARAM) &hdr);

				m_bEntered = TRUE;
			}
			break;

		case VK_ESCAPE :
			if( m_bFloatMode)
			{
				ShowWindow( SW_HIDE);
			}
			else
			{
				Undo();
			}
			break;
	}
	

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void i3EditCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CEdit::OnShowWindow(bShow, nStatus);

	if( m_bFloatMode)
		m_bEntered = TRUE;
	else
		m_bEntered = FALSE;
}
