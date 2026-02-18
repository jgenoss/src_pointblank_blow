#include "stdafx.h"
#include "i3PackFileEditorProfile.h"
#include "i3PackFileEditorDoc.h"
#include "i3PackFileEditorView.h"

i3PackFileEditorProfile *	g_pProfile = NULL;
Ci3PackFileEditorView *		g_pView = NULL;
i3TDKUpdateSystem			g_UpdateSys;

void Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta)
{
	g_UpdateSys.Update( hwnd, event, pObj, pMeta);
}

void RegisterUpdate( HWND hwnd, i3ClassMeta * pMeta, UINT32 EventMask)
{
	g_UpdateSys.AddNotify( hwnd, pMeta, EventMask);
}

static char tempstr[16384];

void EventLog( const char * pszFile, const char * format, ...)
{
	va_list marker;

	va_start( marker, format);

	vsprintf( tempstr, format, marker);

	{
		g_pView->Log( pszFile, tempstr);
	}
}

void ClearEventLog(void)
{
	g_pView->ClearLog();
}