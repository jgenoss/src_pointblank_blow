
#include "stdafx.h"
#include "i3OptiExpPlug.h"


static INT32 gs_lastErrVal = 0;

I3OPTIEXPPLUG_API INT32 OptiExpPlug_GetLastErr()
{
	return gs_lastErrVal;
}

I3OPTIEXPPLUG_API void OptiExpPlug_SetLastErr(INT32 errVal)
{
	gs_lastErrVal = errVal;
}

I3OPTIEXPPLUG_API void OptiExpPlug_CenterWindow(HWND hWnd, HWND hWndParent)
{
	if(!hWnd)
		return;

	DWORD dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
	HWND hWndCenter = hWndParent;

	if (hWndCenter == NULL)
	{
		HWND hWndTemp = ::GetDesktopWindow();

		//if (dwStyle & WS_CHILD)
		//	hWndCenter = ::GetParent(hWnd);
		//else
		//	hWndCenter = ::GetWindow(hWnd, GW_OWNER);

		//if (hWndCenter != NULL)
		//{
		//	// let parent determine alternate center window
		//		(HWND)::SendMessage(hWndCenter, WM_QUERYCENTERWND, 0, 0);

		//	if (hWndTemp != NULL)
		//		hWndCenter = hWndTemp;
		//}
	}

	RECT rcDlg;
	GetWindowRect(hWnd, &rcDlg);
	
	RECT rcArea;
	RECT rcCenter;

	if (!(dwStyle & WS_CHILD))
	{
		// don't center against invisible or minimized windows
		if (hWndCenter != NULL)
		{
			DWORD dwAlternateStyle = ::GetWindowLong(hWndCenter, GWL_STYLE);
			if (!(dwAlternateStyle & WS_VISIBLE) || (dwAlternateStyle & WS_MINIMIZE))
				hWndCenter = NULL;
		}

 		MONITORINFO mi;
		mi.cbSize = sizeof(mi);

		// center within appropriate monitor coordinates
		if (hWndCenter == NULL)
		{
			INT32 MonW = GetSystemMetrics(SM_CXSCREEN);
			INT32 MonH = GetSystemMetrics(SM_CYSCREEN);

			rcCenter.left =0, rcCenter.top = 0;
			rcCenter.right = MonW, rcCenter.bottom = MonH;

			rcArea = rcCenter;
		}
		else
		{
			::GetWindowRect(hWndCenter, &rcCenter);
			GetMonitorInfo(
				MonitorFromWindow(hWndCenter, MONITOR_DEFAULTTONEAREST), &mi);
			rcArea = mi.rcWork;
		}
	}
	else
	{
		::GetClientRect(hWndParent, &rcArea);
		::GetClientRect(hWndCenter, &rcCenter);
		::MapWindowPoints(hWndCenter, hWndParent, (POINT*)&rcCenter, 2);
	}

	LONG rcDlgW = rcDlg.right - rcDlg.left;
	LONG rcDlgH = rcDlg.bottom - rcDlg.top;

	int xLeft = (rcCenter.left + rcCenter.right) / 2 - rcDlgW / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - rcDlgH / 2;

	if (xLeft + rcDlgW > rcArea.right)
		xLeft = rcArea.right - rcDlgW;
	if (xLeft < rcArea.left)
		xLeft = rcArea.left;

	if (yTop + rcDlgH > rcArea.bottom)
		yTop = rcArea.bottom - rcDlgH;
	if (yTop < rcArea.top)
		yTop = rcArea.top;

	SetWindowPos(hWnd, NULL, xLeft, yTop, -1, -1,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

