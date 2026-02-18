#ifndef __DLL_INJECTION_NPROTECT_H__
#define __DLL_INJECTION_NPROTECT_H__

#include "ggsrv30.h"

//InitCSAuth3

#ifdef USE_NPROTECT
// _InitCSAuth3
typedef UINT32 (*FnInitCSAuth3)(char* szModuleDir);
extern FnInitCSAuth3 g_pInitCSAuth3;

// _CloseCSAuth3
typedef UINT32 (*FnCloseCSAuth3)();
extern FnCloseCSAuth3 g_pCloseCSAuth3;

typedef CCSAuth3*(*FnCsAuth3)();
extern FnCsAuth3 g_pCsAuth3;
#endif

#endif