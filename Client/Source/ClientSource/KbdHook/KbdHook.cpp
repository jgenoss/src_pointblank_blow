// KbdHook.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include "i3Framework.h"

static HHOOK g_hKeyHook = nullptr;
static HANDLE g_hInst;
static bool	s_bWindowMode = true;
static HWND	s_hwndGame;
static i3Viewer * s_pViewer = nullptr;

enum WATCH_KEY
{
	WKEY_ALT = 0,
	WKEY_WIN,
	WKEY_CTRL,
	WKEY_LALT,

	WKEY_MAX
};

static bool s_bWatchKey[ WKEY_MAX] = { false, false, false};

//#define LL_HOOK

// WH_KEYBOARD_LL
__declspec( dllexport) LRESULT CALLBACK KeyHookProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	UINT32 vKey = 0;
	UINT32 msg;
	bool bPressed;
	
#if defined( LL_HOOK)
	KBDLLHOOKSTRUCT * pInfo = (KBDLLHOOKSTRUCT *) lParam;

	vKey = pInfo->vkCode;
	msg = wParam;
	bPressed = (pInfo->flags & LLKHF_UP) == 0;
#else
	vKey = wParam;
	
	if( lParam & 0x80000000)
	{
		msg = WM_KEYUP;
		bPressed = false;
	}
	else
	{
		msg = WM_KEYDOWN;
		bPressed = true;
	}
#endif

	if( nCode == HC_ACTION)
	{
		bool bSkip = false;

		if( s_pViewer != nullptr)
		{
			switch( msg)
			{
#if defined( LL_HOOK)							// 도달할수 없는 것을 제끼도록 조건매크로 추가. (11.11.07.수빈)
				case WM_SYSKEYDOWN :
#endif
				case WM_KEYDOWN :		
					s_pViewer->OnKeyDown( vKey);
					break;
#if defined( LL_HOOK)							// 도달할수 없는 것을 제끼도록 조건매크로 추가. (11.11.07.수빈)
				case WM_SYSKEYUP :
#endif
				case WM_KEYUP :
					s_pViewer->OnKeyUp( vKey);
					break;
			}
		}

		// Alt+Tab
		switch( vKey)
		{
			case VK_F6:	
				{
					if(s_bWatchKey[ WKEY_LALT]) 
					{
						bSkip = true;
						I3TRACE("WKEY_LALT VK_F6 \n");
					}
						
				}
				break;
			case VK_HANJA:	//	OBT용 한자 버튼을 막아요. (한자버튼을 이용한 특수문자 입력을 막기위해 komet)
				{
					#if defined( LL_HOOK)
						pInfo->vkCode = 0;
					#endif

					bSkip = true;
				}
				break;
			case VK_TAB :
				if( s_bWindowMode == false)
				{
					if( s_bWatchKey[ WKEY_ALT] || s_bWatchKey[ WKEY_CTRL] || s_bWatchKey[ WKEY_CTRL])
					{
						#if defined( LL_HOOK)
						pInfo->vkCode = 0;
						pInfo->flags &= ~LLKHF_ALTDOWN;
						#endif

						bSkip = true;
					}
				}
				break;


			case VK_ESCAPE :
				if( s_bWindowMode == false)
				{
					if( s_bWatchKey[ WKEY_ALT] || s_bWatchKey[ WKEY_CTRL] || s_bWatchKey[ WKEY_CTRL])
					{
						bSkip = true;
					}
				}
				break;

			case VK_LCONTROL :
			case VK_RCONTROL :
				if((s_bWindowMode == false))
				{
					if( bPressed == false)
						s_bWatchKey[ WKEY_CTRL] = false;
					else
						s_bWatchKey[ WKEY_CTRL] = true;

					#if defined( LL_HOOK)
						pInfo->vkCode = 0;
					#endif

					bSkip = true;
				}
				break;

			case VK_LWIN :
			case VK_RWIN :
				if( (s_bWindowMode == false))
				{
					if( bPressed == false)
						s_bWatchKey[ WKEY_WIN] = false;
					else
						s_bWatchKey[ WKEY_WIN] = true;

					#if defined( LL_HOOK)
						pInfo->vkCode = 0;
					#endif

					bSkip = true;
				}
				break;

			case VK_RMENU :
			case VK_MENU :
			case VK_LMENU :
				if((GetForegroundWindow() == s_hwndGame) && (s_bWindowMode == false))
				{
					//pInfo->vkCode = 0;
					//pInfo->flags &= ~LLKHF_ALTDOWN;

					if( bPressed == false)
					{
						s_bWatchKey[ WKEY_ALT] = false;
					}
					else
					{
						s_bWatchKey[ WKEY_ALT] = true;
					}

					bSkip = true;
					
				}
				if(vKey == VK_MENU)
				{
					if( bPressed == false)
					{
						s_bWatchKey[ WKEY_LALT] = false;

						//Alt키 up일 경우는 포커스가 window 메뉴로 이동하기 때문에 
						//CallWindowProc을 하지 않도록 skip 함.
						bSkip = true;
					}
					else
					{
						s_bWatchKey[ WKEY_LALT] = true;
					}
				}
				break;
		}

		if(vKey == VK_F10)
		{
			bSkip = true;
		}
		else if('1' <= vKey && vKey <= '5') // hansoft. 10323.
		{
			if( s_bWatchKey[ WKEY_LALT] || s_bWatchKey[ WKEY_ALT])
				bSkip = true;
		}

		if( bSkip)
		{
			return 1;
		}
	}

	return CallNextHookEx( g_hKeyHook, nCode, wParam, lParam);
}

__declspec( dllexport)
bool	InstallKeyhook( DWORD idThread, HWND hwnd, bool bWindowMode)
{
	//	키보드 후킹
	if( g_hKeyHook == nullptr)
	{
		char conv[256];

		sprintf_s( conv, "[HOOK] %d\n", bWindowMode == true);
		OutputDebugString( conv);

		#if defined( LL_HOOK)
		g_hKeyHook = ::SetWindowsHookEx( WH_KEYBOARD_LL, KeyHookProc, (HINSTANCE) ::GetModuleHandle(NULL), 0);
		#else
			g_hKeyHook = ::SetWindowsHookEx( WH_KEYBOARD, KeyHookProc, (HINSTANCE) ::GetModuleHandle(NULL), idThread);
		#endif
	}

	memset( s_bWatchKey, 0, sizeof(s_bWatchKey));

	s_hwndGame = hwnd;
	s_bWindowMode = bWindowMode;
	//s_bWindowMode = true;

	return true;
}

__declspec( dllexport)
void	SetViewerKeyhook( i3Viewer * pViewer)
{
	s_pViewer = pViewer;
}

__declspec( dllexport)
bool	UninstallKeyhook(void)
{
	if( g_hKeyHook)
	{
		OutputDebugString( "[HOOK - UN] !\n");

		UnhookWindowsHookEx( g_hKeyHook);	//	키보드 후킹 종료
		g_hKeyHook = nullptr;
	}

	return true;
}

__declspec( dllexport) 
void	ResetWatchKey(void)
{
	memset( s_bWatchKey, 0, sizeof(s_bWatchKey));
}

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch( ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH :		g_hInst = hModule;	break;
		case DLL_PROCESS_DETACH :		
			break;
	}

    return TRUE;
}

*/