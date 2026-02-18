// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKDialogBase.h"
#include "resource.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKEditCtrl.h"

// i3TDKDialogBase dialog

IMPLEMENT_DYNAMIC(i3TDKDialogBase, i3GameObjPropertyDialog)

BEGIN_MESSAGE_MAP(i3TDKDialogBase, i3GameObjPropertyDialog)
END_MESSAGE_MAP()


// i3TDKDialogBase message handlers

void i3TDKDialogBase::SubclassEditBox( HWND hwnd)
{
	WNDPROC pOldProc;
	DWORD style;

	style = GetWindowLong( hwnd, GWL_STYLE);
	SetWindowLong( hwnd, GWL_STYLE, style & ~WS_BORDER);

	style = GetWindowLong( hwnd, GWL_EXSTYLE);
	SetWindowLong( hwnd, GWL_EXSTYLE, style & ~(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

	pOldProc = (WNDPROC) GetWindowLong( hwnd, GWL_WNDPROC);

	SetProp( hwnd, _T("OldProc"), (HANDLE) pOldProc);

	SetWindowLong( hwnd, GWL_WNDPROC, (LONG) i3TDKDefEditBoxProc);

	{
		CRect rt;

		::GetWindowRect( hwnd, &rt);

		::SetWindowPos( hwnd, nullptr, 0, 0, rt.Width() - 1, rt.Height() - 1, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		::SetWindowPos( hwnd, nullptr, 0, 0, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

		::SendMessage( hwnd, EM_SETMARGINS, EC_LEFTMARGIN, MAKELONG( 2, 2));
	}

	//::SendMessage( hwnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG( 4, 4));
}

BOOL	CALLBACK i3TDKDialogBase::SubclassingControl( HWND hwnd, LPARAM lParam)
{
	TCHAR szClass[MAX_PATH];
	
	i3TDKDialogBase * pInst = (i3TDKDialogBase *) lParam;

	GetClassName( hwnd, szClass, sizeof(szClass) - 1);

	if( i3::generic_is_equal( szClass, _T("Edit") ) )
	{
		// Edit Box
		pInst->SubclassEditBox( hwnd);
	}

	return TRUE;
}

BOOL i3TDKDialogBase::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	//EnumChildWindows( m_hWnd, SubclassingControl, (LPARAM) this);
	//SubclassingControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT i3TDKDialogBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
//	HDC dc;

	/*
	switch( message)
	{
		case WM_CTLCOLORSTATIC :
		case WM_CTLCOLORLISTBOX :
		case WM_CTLCOLORBTN :
		case WM_CTLCOLORMSGBOX :
		case WM_CTLCOLORSCROLLBAR :

		case WM_CTLCOLORDLG :
			dc = (HDC) wParam;
			
			if (!g_pTDKGlobalRes) break;			// 툴종료시 이것의 널체크가 안되어 크래시가 나는데, 여기까지 오기 위해서는..
													// VS DEBUG메뉴/Exceptions/Win32 Exception항목을 모두 체크한다..(2012.09.07.수빈)
			SetBkColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_FORE_APP_BKGND));
			SetTextColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_NORMAL));
			return (LRESULT) g_pTDKGlobalRes->getBrush( I3TDK_COLOR_FORE_APP_BKGND);

		case WM_CTLCOLOREDIT : 
			{
				// 2013.12.30 chunjong.song -  editbox 와 static box 구별을위해, 
				// 색깔때문에 editbox란을 Readonly로 착각하는문제를 없애기위해
				dc = (HDC) wParam;	
				if (!g_pTDKGlobalRes) break;

				SetBkColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_BACK_WHITE));
				SetTextColor( dc, g_pTDKGlobalRes->getColor( I3TDK_COLOR_TEXT_BLACK));
				return (LRESULT) g_pTDKGlobalRes->getBrush( I3TDK_COLOR_BACK_WHITE);
			}
	}
	*/

	return i3GameObjPropertyDialog::WindowProc(message, wParam, lParam);
}
