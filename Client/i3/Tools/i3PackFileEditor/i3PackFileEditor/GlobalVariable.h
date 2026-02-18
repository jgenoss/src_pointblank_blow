#if !defined( __GLOBAL_VARIABLE_H)
#define __GLOBAL_VARIABLE_H

#include "i3PackFileEditorProfile.h"

class Ci3PackFileEditorView;

extern i3PackFileEditorProfile *	g_pProfile;
extern i3TDKUpdateSystem			g_pUpdateSys;
extern Ci3PackFileEditorView *		g_pView;

void Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta = NULL);
void RegisterUpdate( HWND hwnd, i3ClassMeta * pMeta, UINT32 EventMask);
void EventLog( const char * pszFile, const char * format, ...);
void ClearEventLog(void);
#endif
