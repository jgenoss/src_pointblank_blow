#ifndef __INTEGRATION_API__
#define __INTEGRATION_API__

#include "../Innova/integration_api.h"

typedef int (__cdecl *FnInte_IInitializeA)( 
	__in_param const char* configPath,
	__out_param uint_64* service_id );

typedef int (__cdecl *FnInte_IUninitialize)( );

typedef int (__cdecl *FnInte_ILogin2A)( 
	__in_param const char* user_name,
	__in_param const char* password,
	__in_param int exp_time,
	__in_param const char* ip,
	__in_param int flags,
	__out_param innbill_login_result2_a* result );

typedef int(__cdecl *FnInte_ICheckUserStatus2A)(
	__in_param uint_64 user_id,
	__in_param const char* user_name,
	__out_param innbill_check_user_status_result* result );

typedef int(__cdecl *FnInte_IBytItem2A)(
	__in_param uint_64 user_id,
	__in_param const char* user_name,
	__in_param const char* item_id,
	__in_param double item_price,
	__in_param int item_price_type,
	__in_param int item_count,
	__in_param uint_64 service_transaction_id,
	__in_param const char* server_name,
	__in_param const char* character_name,
	__in_param const char* ip,
	__out_param uint_64* billing_transaction_id,
	__out_param double* user_balance );

typedef wchar_t*(__cdecl *FnInte_PErrorDescriptionW)(
	int error_code );

extern FnInte_IInitializeA g_pInte_IInitializeA;
extern FnInte_IUninitialize g_pInte_IUninitialize;
extern FnInte_ILogin2A g_pInte_ILogin2A;
extern FnInte_ICheckUserStatus2A g_pInte_ICheckUserStatus2A;
extern FnInte_IBytItem2A g_pInte_IBytItem2A;
extern FnInte_PErrorDescriptionW g_pInte_PErrorDescriptionW;

__declspec(selectany) FnInte_IInitializeA g_pInte_IInitializeA;
__declspec(selectany) FnInte_IUninitialize g_pInte_IUninitialize;
__declspec(selectany) FnInte_ILogin2A g_pInte_ILogin2A;
__declspec(selectany) FnInte_ICheckUserStatus2A g_pInte_ICheckUserStatus2A;
__declspec(selectany) FnInte_IBytItem2A g_pInte_IBytItem2A;
__declspec(selectany) FnInte_PErrorDescriptionW g_pInte_PErrorDescriptionW;

class cIntegrationAPI
{
private:
	HMODULE m_hDll;

public:
	cIntegrationAPI(void);
	virtual ~cIntegrationAPI(void);
	HMODULE GetDll() { return m_hDll; }
	BOOL	OnCreate( char* strPath );
	void	OnDestroy();
};

extern cIntegrationAPI * g_pIntegrationAPI;

#endif