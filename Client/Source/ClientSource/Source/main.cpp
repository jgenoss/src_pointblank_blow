#include "pch.h"

#ifdef USE_I3EXEC

#include <psapi.h>
//
#include "mmsystem.h"
#include "Game.h"

#include "resource.h"
#include "GlobalVariables.h"
#include "StageBattle.h"

extern HINSTANCE g_hInstance;

BOOL WINAPI DllMain( HINSTANCE hInst, DWORD reason, void * p)
{	
	g_hInstance = hInst;
	
	return TRUE;
}

#endif