#include "i3LuaPCH.h"
#include "i3LuaMeta.h"
#include "i3LuaDebugWin.h"

#if defined( I3_WINDOWS) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3LuaRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return TRUE;
}
#endif

