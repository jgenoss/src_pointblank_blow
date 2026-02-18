#include "i3OptPCH.h"
#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Scene.h"

#include "i3AllOpts.h"

I3_EXPORT_OPT
void i3OptRegisterMetas(void)
{
	// Dependency
	i3BaseRegisterMetas();
	i3MathRegisterMetas();
	i3GfxRegisterMetas();
	i3SceneRegisterMetas();
}

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
