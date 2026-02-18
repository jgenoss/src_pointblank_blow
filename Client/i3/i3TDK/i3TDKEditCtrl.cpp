#include "stdafx.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKUtilGDI.h"

WNDPROC g_pOldEditBoxProc = nullptr;

#define		ED_SPACE		2
#define		ED_BORDER		2

static void DrawEditBox( HWND hwnd, HDC dc, PAINTSTRUCT * pPS)
{
	HPEN hOldPen;
	HBRUSH hOldBrush;
	CRect rc;
	INT32 x1, y1, x2, y2;

	GetWindowRect( hwnd, &rc);

	x1 = - ED_BORDER;
	y1 = - ED_BORDER;
	x2 = rc.Width() - ED_BORDER;
	y2 = rc.Height() - ED_BORDER;

	hOldPen = (HPEN) ::SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_BORDER));
	hOldBrush = (HBRUSH) ::SelectObject( dc, GetStockObject( NULL_BRUSH));

	Rectangle( dc, x1, y1, x2, y2);

	SelectObject( dc, GetStockObject( BLACK_PEN));
	MoveToEx( dc, x1 + 1, y2 - 2, nullptr);
	LineTo( dc, x1 + 1, y1 + 1);
	LineTo( dc, x2 - 2, y1 + 1);

	SelectObject( dc, g_pTDKGlobalRes->getPen( I3TDK_COLOR_FORE_3D_LIT));
	LineTo( dc, x2 - 2, y2 - 2);
	LineTo( dc, x1 + 1, y2 - 2);

	SelectObject( dc, hOldPen);
	SelectObject( dc, hOldBrush);
}

LRESULT CALLBACK i3TDKDefEditBoxProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC pOldProc;
	LRESULT Result = 0;
	bool bOldProc = true;

	switch( msg)
	{
		case WM_NCCALCSIZE :
			{
				Result = 0;
				
				NCCALCSIZE_PARAMS * lpncsp = (NCCALCSIZE_PARAMS *) lParam;

				lpncsp->rgrc[0].left   += ED_SPACE;
				lpncsp->rgrc[0].top    += ED_SPACE;
				lpncsp->rgrc[0].right  -= ED_SPACE;
				lpncsp->rgrc[0].bottom -= ED_SPACE;
			}

			bOldProc = false;
			break;

		case WM_NCPAINT :
			HDC hdc;

			//hdc = GetDCEx( hwnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN);
			hdc = ::GetDC( hwnd);

			DrawEditBox( hwnd, hdc, nullptr);

			ReleaseDC(hwnd, hdc);

			bOldProc = false;
			break;
	}

	if( bOldProc)
	{
		pOldProc = (WNDPROC) GetProp( hwnd, _T("OldProc") );

		if( pOldProc != nullptr)
		{
			Result = CallWindowProc( pOldProc, hwnd, msg, wParam, lParam);
		}
	}


	return Result;
}
