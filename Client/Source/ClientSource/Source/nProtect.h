#if !defined( __NPROTECT_H)
#define __NPROTECT_H

#if defined (_NPROTECT_)
#include "../nProtect/NPGameLib/NPGameLib.h"

#pragma comment (lib, "../nProtect/NPGameLib/NPGameLib.lib")		//	nProtect 게임 가드 라이브러리

extern char g_szHackMsg[];

namespace nProtect
{
	bool		Init( HWND hwndMain);
	bool		Close(void);

	bool		CheckAlive(void);
	bool		SendUserID( char * pszID);

	void		SetServerAuth2(GG_AUTH_DATA * pAuthData);
	void		SetServerAuth3(PBYTE pPacket, UINT32 size);
	bool Set_D3DDeviceInfo(DWORD dwD3DVer, LPVOID *lppD3DDevicePtr);
};

#endif

#endif
