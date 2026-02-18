// i3TDKFloatingEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKFloatingEditCtrl.h"
#include ".\i3tdkfloatingeditctrl.h"


// i3TDKFloatingEditCtrl

IMPLEMENT_DYNAMIC(i3TDKFloatingEditCtrl, CEdit)

BEGIN_MESSAGE_MAP(i3TDKFloatingEditCtrl, CEdit)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// i3TDKFloatingEditCtrl message handlers


void i3TDKFloatingEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( m_bNotifyEnter)
	{
		if( nChar == VK_RETURN)
		{
			::PostMessage( GetParent()->m_hWnd, WM_TDK_NOTIFY_ENTER, 0, 0);
		}
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

LRESULT i3TDKFloatingEditCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( m_bNotifyEnter)
	{
		switch( message)
		{
			case WM_GETDLGCODE : 
				return DLGC_WANTALLKEYS;

		}
	}

	return CEdit::WindowProc(message, wParam, lParam);
}
