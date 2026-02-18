#include "i3InputType.h"

#if defined( I3_PSP)
#if defined( I3_COMPILER_GCC)
//SCE_MODULE_INFO( i3Input, 0, 1, 0);
//SCE_LIB_DECLARE( i3Input, 0);
#else
//SCE_LIB_VERSION_CHECK_OFF( i3Input);
#endif
#endif


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		i3System::HoldDllModule(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return true;
}

