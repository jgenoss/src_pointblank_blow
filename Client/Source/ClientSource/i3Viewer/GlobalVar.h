#if !defined( __GLOBAL_VAR_H)
#define __GLOBAL_VAR_H

#ifdef USE_I3EXEC
extern HINSTANCE		g_hInstance;
#endif

#if defined( I3_DEBUG)
class CDebugPanel;

extern CDebugPanel *		g_pDebugPanel;

void ToggleDebugPanel(void);
void CloseDebugPanel(void);

#endif

#endif
