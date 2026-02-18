#include "i3NetworkDef.h"

#if defined( I3_WINDOWS) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3GfxRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return true;
}
#endif


