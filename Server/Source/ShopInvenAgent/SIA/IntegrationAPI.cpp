#include "pch.h"
#include "IntegrationAPI.h"

cIntegrationAPI * g_pIntegrationAPI = NULL;

cIntegrationAPI::cIntegrationAPI()
{
	m_hDll = NULL;
}

cIntegrationAPI::~cIntegrationAPI()
{
	OnDestroy();
}

BOOL	cIntegrationAPI::OnCreate( char* strPath )
{
	char strFileLocation[SERVER_STRING_COUNT];
	i3String::Format( strFileLocation, SERVER_STRING_COUNT, "%s\\integration_api.dll", strPath );

	m_hDll = LoadLibrary( strFileLocation );
	//m_hDll = LoadLibrary( "integration_api.dll" );
	UINT32 ui32ErrorReason = GetLastError();
	ui32ErrorReason;
	if( NULL == m_hDll ) return FALSE;

	g_pInte_IInitializeA = (FnInte_IInitializeA)GetProcAddress( m_hDll, "innbill_initialize_a" );
	if( NULL == g_pInte_IInitializeA) return FALSE;
	
	g_pInte_IUninitialize	= (FnInte_IUninitialize)GetProcAddress( m_hDll, "innbill_uninitialize" );
	if( NULL == g_pInte_IUninitialize ) return FALSE;

	g_pInte_ILogin2A = (FnInte_ILogin2A)GetProcAddress( m_hDll, "innbill_login2_a" );
	if( NULL == g_pInte_ILogin2A ) return FALSE;

	g_pInte_ICheckUserStatus2A = (FnInte_ICheckUserStatus2A)GetProcAddress(m_hDll, "innbill_check_user_status2_a");
	if (NULL == g_pInte_ICheckUserStatus2A) return FALSE;

	g_pInte_IBytItem2A = (FnInte_IBytItem2A)GetProcAddress(m_hDll, "innbill_buy_item2_a");
	if (NULL == g_pInte_IBytItem2A) return FALSE;

	g_pInte_PErrorDescriptionW = (FnInte_PErrorDescriptionW)GetProcAddress(m_hDll, "innbill_error_description_w");
	if (NULL == g_pInte_PErrorDescriptionW) return FALSE;

	return TRUE;
}

void	cIntegrationAPI::OnDestroy()
{
	if (m_hDll)
		FreeLibrary(m_hDll);
}