#if !defined( __I3_TDK_EDIT_CTRL_H)
#define __I3_TDK_EDIT_CTRL_H

extern WNDPROC g_pOldEditBoxProc;

LRESULT CALLBACK i3TDKDefEditBoxProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM);

#endif
