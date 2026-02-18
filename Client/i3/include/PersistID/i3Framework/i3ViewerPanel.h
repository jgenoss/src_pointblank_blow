#if !defined( __I3_VIEWER_PANEL_H)
#define __I3_VIEWER_PANEL_H

#if defined( I3_WINDOWS)
BOOL CALLBACK  _ViewerPanelProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdatePanelDuration( HWND hwnd, i3ViewerFramework * pFramework);
void UpdatePanelTime( HWND hwnd, i3ViewerFramework * pFramework);
void UpdateButtonPanel( HWND hwnd, i3ViewerFramework * pFramework);
#endif

#endif
