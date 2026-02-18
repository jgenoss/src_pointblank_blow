#ifndef __STEAM_API_H__
#define __STEAM_API_H__

#include "./steam/steamencryptedappticket.h"

typedef bool (__cdecl *FnSteam_BDecryptTicket)( 
	const uint8 *rgubTicketEncrypted,
	uint32 cubTicketEncrypted,
	uint8 *rgubTicketDecrypted,
	uint32 *pcubTicketDecrypted,
	const uint8 rgubKey[k_nSteamEncryptedAppTicketSymmetricKeyLen],
	int cubKey );

typedef bool (__cdecl *FnSteam_BIsTicketForApp)( 
	uint8 *rgubTicketDecrypted,
	uint32 cubTicketDecrypted,
	AppId_t nAppID );

typedef void (__cdecl *FnSteam_GetTicketSteamID)( 
	uint8 *rgubTicketDecrypted, 
	uint32 cubTicketDecrypted, 
	CSteamID *psteamID );

extern FnSteam_BDecryptTicket g_pSteam_BDecryptTicket;
extern FnSteam_BIsTicketForApp g_pSteam_BIsTicketForApp;
extern FnSteam_GetTicketSteamID g_pSteam_GetTicketSteamID;

class cSteamAPI
{
private:
	HMODULE m_hDll;

public:
	cSteamAPI(void); 
	virtual ~cSteamAPI(void); 

	BOOL	OnCreate( char* strPath );
	void	OnDestroy();
};

extern cSteamAPI * g_pSteamAPI; 

#endif