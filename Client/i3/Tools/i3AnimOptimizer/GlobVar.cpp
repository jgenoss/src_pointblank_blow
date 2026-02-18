#include "stdafx.h"
#include "AnimOptProject.h"

AnimOptProject	g_Project;
i3TDKUpdateSystem g_Update;

void Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta)
{
	g_Update.Update( hwnd, event, pObj, pMeta);
}

void RegisterUpdate( HWND hwnd, i3ClassMeta * pMeta, UINT32 event)
{
	g_Update.AddNotify( hwnd, pMeta, event);
}

void UnregisterUpdate( HWND hwnd)
{
	g_Update.RemoveNotify( hwnd);
}

