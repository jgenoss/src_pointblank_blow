#if !defined( __GLOB_VAR_H)
#define __GLOB_VAR_H

#include "AnimOptProject.h"


extern AnimOptProject	g_Project;
extern i3TDKUpdateSystem	g_Update;

void Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta = NULL);
void RegisterUpdate( HWND hwnd, i3ClassMeta * pMeta, UINT32 event);
void UnregisterUpdate( HWND hwnd);

#endif