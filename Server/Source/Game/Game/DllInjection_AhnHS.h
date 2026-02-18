#ifndef __DLL_INJECTION_AHNHS_H__
#define __DLL_INJECTION_AHNHS_H__

#include "AntiCpXSvr.h"

// __AhnHS_CreateServerObject@4
typedef AHNHS_SERVER_HANDLE (__stdcall *FnAhnHS_CreateServerObject)(IN const char *pszFilePath);
extern FnAhnHS_CreateServerObject g_pAhnHS_CreateServerObject;

// __AhnHS_CloseServerHandle@4
typedef void (__stdcall *FnAhnHS_CloseServerHandle)(IN AHNHS_SERVER_HANDLE hServer);
extern FnAhnHS_CloseServerHandle g_pAhnHS_CloseServerHandle;

// __AhnHS_CreateClientObject@4
typedef AHNHS_CLIENT_HANDLE (__stdcall *FnAhnHS_CreateClientObject)(IN AHNHS_SERVER_HANDLE hServer);
extern FnAhnHS_CreateClientObject g_pAhnHS_CreateClientObject;

// __AhnHS_CloseClientHandle@4
typedef void (__stdcall *FnAhnHS_CloseClientHandle)(IN AHNHS_CLIENT_HANDLE hClient);
extern FnAhnHS_CloseClientHandle g_pAhnHS_CloseClientHandle;

// __AhnHS_MakeRequest@8
typedef unsigned long (__stdcall *FnAhnHS_MakeRequest)(IN AHNHS_CLIENT_HANDLE hClient, OUT PAHNHS_TRANS_BUFFER pRequestBuffer);
extern FnAhnHS_MakeRequest g_pAhnHS_MakeRequest;

// __AhnHS_VerifyResponseEx@16
typedef unsigned long (__stdcall *FnAhnHS_VerifyResponseEx)(IN AHNHS_CLIENT_HANDLE hClient, IN unsigned char *pbyResponse, IN unsigned long nResponseLength, OUT unsigned long *pulErrorCode);
extern FnAhnHS_VerifyResponseEx g_pAhnHS_VerifyResponseEx;

#endif