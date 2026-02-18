// stdafx.cpp : source file that includes just the standard includes
// i3Win32.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if defined( I3SND_DSOUND)
LPDIRECTSOUND8		g_pDS			= nullptr;
#endif

#if (defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3SoundRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return TRUE;
}

#endif
