#if !defined( __I3_ERROR_HANDLER_WINDOWS_H)
#define __I3_ERROR_HANDLER_WINDOWS_H

#include "i3ErrorHandler.h"

I3ERROR_HANDLE DefWindowsLogProc( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData);

#endif
