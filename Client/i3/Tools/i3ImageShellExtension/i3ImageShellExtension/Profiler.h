#include "TypeDefine.h"

typedef void(*PF_CALLBACK)(INT32 slot, const char * pszName, INT64 callCount, double elapsed, double elapseMin, double elapseMax, double elapseAvg, void * pUserData);

void INIT_PROFILE(void);
void SET_PROFILE(INT32 slot, const char * pszStr);
void START_PROFILE(INT32 slot);
void END_PROFILE(INT32 slot);
void RESET_PROFILE(bool bClearHistory = false);
void REPORT_PROFILE(char * pszDest = NULL);
//void REPORT_PROFILE(i3::string& inout);
void REPORT_PROFILE_CALLBACK(PF_CALLBACK pUserProc, void * pData);
bool CHECK_PROFILE(INT32 slot, double sec);

INT64	PF_GetCallCount(INT32 slot);
double	PF_GetElapsed(INT32 slot);

#define		__SP(a)						START_PROFILE( a)
#define		__EP(a)						END_PROFILE( a)
#define		__CP(a, sec)				CHECK_PROFILE( a, sec)
