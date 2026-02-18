#include "pch.h"
#include "SteamAPI.h"

cSteamAPI * g_pSteamAPI = NULL; 

cSteamAPI::cSteamAPI()
{
	m_hDll = NULL;
}

cSteamAPI::~cSteamAPI()
{
	OnDestroy();
}

BOOL	cSteamAPI::OnCreate( char* strPath )
{
	char strFileLocation[1024];
	i3String::Format( strFileLocation, 1024, "%s\\sdkencryptedappticket.dll", strPath );

	m_hDll = LoadLibrary( strFileLocation );

	if( NULL == m_hDll ) return FALSE;

	g_pSteam_BDecryptTicket		= (FnSteam_BDecryptTicket)GetProcAddress( m_hDll, "SteamEncryptedAppTicket_BDecryptTicket" );
	if( NULL == g_pSteam_BDecryptTicket ) return FALSE;
	
	g_pSteam_BIsTicketForApp	= (FnSteam_BIsTicketForApp)GetProcAddress( m_hDll, "SteamEncryptedAppTicket_BIsTicketForApp" );
	if( NULL == g_pSteam_BIsTicketForApp ) return FALSE;

	g_pSteam_GetTicketSteamID	= (FnSteam_GetTicketSteamID)GetProcAddress( m_hDll, "SteamEncryptedAppTicket_GetTicketSteamID" );
	if( NULL == g_pSteam_GetTicketSteamID ) return FALSE;

	return TRUE;
}

void	cSteamAPI::OnDestroy()
{
	if (m_hDll)
		FreeLibrary(m_hDll);
}