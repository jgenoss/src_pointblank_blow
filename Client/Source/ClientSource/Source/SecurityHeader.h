#ifndef		__SECURITY_HEADER_H__
#define		__SECURITY_HEADER_H__


#if defined (_NPROTECT_)
#include "nProtect.h"


typedef		void (*TerminateHS_Proc)();
typedef		int (*AhnHSMakeResponse_Proc)(unsigned char *pbyRequest, unsigned long ulRequestLength, PAHNHS_TRANS_BUFFER pResponseBuffer);
typedef		void	(*AhnHS_SetUserId_Proc)(LPCTSTR szUserID);

#elif defined (_FROST_)
#include "../Frost/gameShieldLib/shieldSecurity.h"

#endif


#endif	// __SECURITY_HEADER_H__