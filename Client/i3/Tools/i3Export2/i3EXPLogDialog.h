#if !defined( __I3_EXP_LOG_H)
#define __I3_EXP_LOG_H

#define		WM_EXP_LOG			(WM_USER + 3)
#define		WM_EXP_CLEAR		(WM_USER + 4)

BOOL CALLBACK  _LogDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
