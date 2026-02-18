// i3Viewer.cpp : Defines the entry point for the application.


/**
    @mainpage  Project : DoxygenTest
    @section   Program 프로그램명
        - 프로그램명  :  Point Blank
*/

#include "stdafx.h"
#include <stdio.h>
#include "shellapi.h"
#include "resource.h"
#include "Msi.h"
#include "../../Themida/ThemidaSDK/Include/C/ThemidaSDK.h"
#include "GlobalVar.h"
#include "ConfigEx.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/pool/pool_dbg.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_directoryname.h"

#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/compare/generic_compare.h"
#include "GameLoginContext.h"



#include "../../LeucoShell/Include/Client/Lib/LeucoShell.h"

//#include "./Arxan/HWAD.h"

#ifndef USE_I3EXEC
#include "pch.h"
#include "GlobalVariables.h"
#include "GameInterface.h"
#endif

#include "i3Gfx/i3ShaderSourceCodeMgr.h"

#if defined( USE_HACK_DETECTED ) || defined( USE_DLL_DETECTED )
	#include "DllGuard.h"
#endif

#if defined( I3_DEBUG)
	#include "DebugPanel.h"
#endif


#if defined( _XIGNCODE_) 
xstatus XCALL MyUserInfoCallBack(xuint iid, xstr buffer, xsize size, xpvoid context)
{
	if ( GameLoginContext::i()->getUserUID() != -1 )
	{
		if (iid == XUID_USER_ACCOUNT)
		{
			char strUID[256] = { 0, };

#if defined( LOCALE_NORTHAMERICA)
			i3::sprintf(strUID, "%d_%d", _i64toa(GameLoginContext::i()->getUserUID(), strUID, 10), g_pServerInfo->GetCurrentIPIdx());
			StringCbCopyA(buffer, size, strUID);
#else
			StringCbCopyA(buffer, size, _i64toa(GameLoginContext::i()->getUserUID(), strUID, 10));
#endif
			I3PRINTLOG(I3LOG_NOTICE, "X::OK: %d", GameLoginContext::i()->getUserUID());
			return ZCWAVE_OK;
		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE, "X::DENIED: %d", GameLoginContext::i()->getUserUID());
			return ZCWAVE_E_USER_INFO_ACCESS_DENIED;
		}
	}
	I3PRINTLOG(I3LOG_NOTICE, "X::NOT_READY: %d", GameLoginContext::i()->getUserUID());
	return ZCWAVE_E_USER_INFO_IS_NOT_READY;
}
#define XIGNCODE_PATH  "\\XignCode"
#define XIGNCODE_KEY "Nw_cjspitNrb"
#endif

#include "../Source/SteamContext.h"

#if defined( LOCALE_BRAZIL)
// X-TRAP 스타트시 요구되는 키  2010-08-10 komet
// 이 키값은 X-TRAP 회사에서 받는다. 또한 X-TRAP FTP 서버 주소에 따라 값이 바뀐다.
// X-TRAP FTP 서버가 바뀔 경우 X-TRAP 회사에 다시 요청해서 받아야 한다.
// 소스의 Winmain.cpp와 파일 업데이터 소스 공용으로 사용된다. 파일 업데이터 소스에서 사용하는 값과 동일한 키여야 한다.
// 현재 아래 주소는 http://Path.pointblankonline.com.br/xtrap/ 사이트 값이다.
#define XTRAP_ARGUMENT_KEY				"660970B448EBDD3290256D9844CFE8620A40CEE085F1215ADA56B0D591F9B8FD613A943317CA9FE20D68BA611AF8A29956A80163AF23B3B437EECA91DCCF8EC60F7D04245246E437A4610510CB5F7E0399D78FF8DD262E24ABB0515BE770BA7A2BC783B2B0D2DCF58998"

// X-TRAP 테스트 서버
//#define XTRAP_ARGUMENT_KEY				"660970B448EBDD3290256D9844CFE8620A40CEE085F1215ADA56B0D591F9B8FD613A943317CA9FE20D68BA611AF8A29956A80163AF23B3B437EECA91DCCF8EC60F7D04245246E417A4610E5695477E1EF47455B309945CB0D27B7C7D383D0C40DBC997C22DB0B6C35D7EEE51C30CBE893F089AAE398F"

#endif

#if defined (LOCALE_LATIN_AMERICA)
// X-TRAP 스타트시 요구되는 키
// Patch URL : http://fhl-lcdn.pandonetworks.com/Xtrap-pb
#define XTRAP_ARGUMENT_KEY				"660970B496EBDDBEC5456D9844CFEF62F07458A929B9A6F47331DC64FC773FB6CD7535CE211ED52CF255E5836C73D4DC0D175E29D1CFD8C8666F340C1800318F0F7D04245246E401AD79435DDF5E391FB2443FCC992B7072B7375C4C8CD27E8D0DD34F06D669D2D9"
#endif

///////////////////////////////////////////	X-TRAP 연동 (Winmain에서 해야 함)
#if defined( _XTRAP_) 

#ifdef _MT
	#ifdef _DLL	//다중 스레드(/MT)
		#pragma comment(lib, "../../Xtrap/Lib/XTrap4Client_mtDll.lib")
		#pragma comment(lib, "../../Xtrap/Lib/XTrap_Unipher_mtDll.lib")
		#pragma comment(lib, "../../Xtrap/Lib/XTrap4Client_ClientPart_mtDll.lib")
		#pragma comment(lib, "../../Xtrap/Lib/XTrap4Client_ServerPart_mtDll.lib")
	#else	//다중 스레드 DLL(/MD)
		#pragma comment(lib, "../../Xtrap/Lib/XTrap4Client_mt.lib")
		#pragma comment(lib, "../../Xtrap/Lib/XTrap_Unipher_mt.lib")
		#pragma comment(lib, "../../Xtrap/Lib/XTrap4Client_ClientPart_mt.lib")
		#pragma comment(lib, "../../Xtrap/Lib/XTrap4Client_ServerPart_mt.lib")
	#endif
#endif

#include "../../Xtrap/Include/Xtrap_C_Interface.h"
#include "../../Xtrap/Include/Xtrap_S_Interface.h"
#include "../../Xtrap/Include/Xtrap_C_XtrapCode.h"	// revision 32248 해킹 탐지 코드 추가

#ifdef _XTRAP_DUMP
#include "../../Xtrap/Etc/HexaDump4XTrap.h"
#endif

// X-TRAP 자체에서 단독 쓰레드로 주기적으로 Alive 체크하기 위해
DWORD WINAPI XTrapCallbackAliveThreadFunc( LPVOID loParam)
{
	DWORD dwPeriod = 20000; // 20 sec
	while(TRUE)
	{
		Sleep( dwPeriod);

		XTrap_C_CallbackAlive( dwPeriod);
	}

	return 0;
}

//	X-TRAP 과 서버와 연동하기 위해 등록된 콜백 함수. komet
//  XTrap_CS_Step2 함수가 GameInterface DLL 에서 실행하지 않고 Winmain 에서 직접 실행하기위해 콜백을 등록하여 이용한다.
static INT32 CSStep2Proc( void * pInBuffer, void * pOutBuffer, void * pLength)
{		
#ifdef _XTRAP_DUMP
	HexaDump4XTrap_V1( "XtrapCheck.txt", pInBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE, "[*2 In Callback]");
#endif

	XTrap_CS_Step2( pInBuffer, pOutBuffer, XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS );

#ifdef _XTRAP_DUMP	
	HexaDump4XTrap_V1( "XtrapCheck.txt", pOutBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE, "[*3 Out Callback]");
#endif

	return 0;
}
#endif

#if defined(WINDOW_MODE_DISABLE)
extern "C"
{
#include "../WinLockDLL/WinLockDll.h"
};
#pragma comment (lib, "WinLockDLL.lib")
#endif

typedef void (* PROHIBIT_PROC)();
typedef void (* LOG_PROC)(LPCSTR logName);

#ifdef I3_WINDOWS

#define MAX_LOADSTRING 100

//	업데이트 실행후 게임 실행 확인 파라미터
#if defined(LOCALE_INDONESIA)
//#define RUN_PARAMETER	"wpvpxhakstp12#$"		// 2009-10-26 까지 사용하였음.
#define RUN_PARAMETER	"we.hate.hacker!"		// 2009-10-27 부터 사용합니다.
#else
#define RUN_PARAMETER	"wpvpxhakstp12#$"
#endif


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND g_hwndMain;
char				g_szStartStage[256] = {0,};
i3VideoInfo			g_VideoInfo;
bool		g_bCanUnhook = false;
CConfigEx** ppConfig = NULL;

HMODULE	g_hModule = NULL;

#ifdef USE_I3EXEC	
	i3Viewer *			_pViewer = NULL;
	i3Framework *		_pFramework = NULL;
	i3GameInterface *	_pInterface = NULL;
#else
	static i3Viewer * _pViewer = NULL;
	static i3Framework * _pFramework = NULL;
	static i3GameInterface* _pInterface = NULL;
	extern CConfigEx* g_pConfigEx;
#endif


char g_szPath[MAX_PATH] = {};
I3_CREATE_GAME_INTERFACE_PROC g_pProc = NULL;

HANDLE	g_hMutex = NULL;

STICKYKEYS	* g_pdwStickyKey = 0;
DWORD	g_dwStickyKeysFlagBackup = 0;

FILTERKEYS* g_pdwFilterKey = 0;
DWORD	g_dwFilterKeysFlagBackup = 0;

TOGGLEKEYS* g_pdwToggleKey = 0;
DWORD	g_dwToggleKeysFlagBackup = 0;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
__declspec( dllimport) bool	InstallKeyhook( DWORD idThread, HWND hwnd, bool bWindowMode);
__declspec( dllimport) bool	UninstallKeyhook( void);
__declspec( dllimport) void	ResetWatchKey( void);
__declspec( dllimport) void SetViewerKeyhook( i3Viewer * pViewer);


#ifdef USE_I3EXEC
char CheckingModule[][MAX_PATH] =
{
#if defined( LOCALE_NORTHAMERICA)
	"PiercingBlow.i3Exec",
#else
	"PointBlank.i3Exec",
#endif
};
char szTemp[256];
static bool	CheckModule()
{
	string	str;

	char Path[MAX_PATH] = {};
	char ExeFullPath[MAX_PATH] = {};

	// exe와 같은경로에 있는 Exec를 로드합니다.
	GetModuleFileName(NULL, ExeFullPath, MAX_PATH);
//	i3String::SplitPath(ExeFullPath, Path);	
	i3::extract_directoryname(ExeFullPath, Path);


	return true;
}
#endif
//====================================================================================
//TryLoadSymbol(), TryLoadGame() 함수 리턴타입, 주석 등 수정시 Xtrap 크래쉬 발생합니다.
//Xtrap에 문의 해야 됨.
//====================================================================================
static i3GameInterface * TryLoadSymbol( const char * pszPath)
{
#ifdef USE_I3EXEC
	char LogBuf[MAX_PATH];
	char szExt[127];

	i3::extract_fileext(pszPath, szExt);

	if( !i3::generic_compare(szExt, "DLL") &&
		!i3::generic_compare(szExt, "BIN") &&
		!i3::generic_compare(szExt, "i3Exec") )
	{
		i3::snprintf( LogBuf, MAX_PATH, "* Invalid Exec. extension : %s", szExt);
		I3PRINTLOG(I3LOG_VIEWER, LogBuf);
		return NULL;
	}


	i3::snprintf( LogBuf, MAX_PATH, "* load : %s", pszPath);
	I3PRINTLOG(I3LOG_VIEWER, LogBuf);

	char CurDir[MAX_PATH] = {};

	GetCurrentDirectory(MAX_PATH, CurDir);
	i3::snprintf( LogBuf, MAX_PATH, "* Current Dir : %s", CurDir);
	I3PRINTLOG(I3LOG_VIEWER, LogBuf);

	char ExeFullPath[MAX_PATH] = {};
	char LibPath[MAX_PATH] = {};

	// exe와 같은경로에 있는 Exec를 로드합니다.
	GetModuleFileName(NULL, ExeFullPath, MAX_PATH);
//	i3String::SplitPath(ExeFullPath, g_szPath);
	i3::extract_directoryname(ExeFullPath, g_szPath);
	i3::snprintf(LibPath, MAX_PATH, "%s\\%s", g_szPath, pszPath);

	g_hModule = ::LoadLibrary( LibPath );
	if ( g_hModule == NULL )
	{
		char szTemp[256];
		i3::snprintf( szTemp, 256, "* LibPath : %s ", LibPath);
		I3PRINTLOG(I3LOG_VIEWER, szTemp);
		I3PRINTLOG(I3LOG_VIEWER, "load lib fail");
		return NULL;
	}

	g_pProc = (I3_CREATE_GAME_INTERFACE_PROC) GetProcAddress( g_hModule, "i3CreateGameInterface");
	if( g_pProc == NULL)
	{
		I3PRINTLOG(I3LOG_VIEWER, "can not found proc");
		return NULL;
	}

	return NULL;
#else
	return NULL;
#endif

//	return NULL;
}

static i3GameInterface *	TryLoadGame()
{
#ifdef USE_I3EXEC
	return g_pProc( g_szPath );
#else
	return i3CreateGameInterface( g_szPath );
#endif

//	return NULL;
}


static void _ProcessArg( INT32 argc, const char * pszBuf)
{
	if( g_szStartStage[0] == 0)
	{
		i3::safe_string_copy( g_szStartStage, pszBuf, 256);
	}
}

static void	ParseParameters( const char * pszCmdLine)
{
	char * pszTemp = (char *) pszCmdLine;
	char szBuf[ MAX_PATH];
	INT32 idx = 0, argc = 0;
	bool bString = false;

	while( *pszTemp)
	{
		switch( *pszTemp)
		{
		case '"' :
			if( bString == false)
			{
				bString = true;
			}
			else
			{
				szBuf[ idx] = 0;

				_ProcessArg( argc, szBuf);
				argc++;

				bString = false;
			}
			idx = 0;
			break;

		case '\t' :
		case ' ' :
			if( bString )
			{
				// 문자열 중...
				szBuf[idx] = *pszTemp;
				idx++;
			}
			else
			{
				if( idx > 0)
				{
					szBuf[idx] = 0;

					_ProcessArg( argc, szBuf);
					argc++;

					idx = 0;
				}
			}
			break;

		default :
			szBuf[ idx] = *pszTemp;
			idx++;
			break;
		}

		pszTemp++;
	}

	if( idx > 0)
	{
		szBuf[ idx] = 0;
		_ProcessArg( argc, szBuf);
		argc++;
	}

	return;
}

void SetForegroundWindowForce(HWND hWnd)
{
	HWND hWndForeground = ::GetForegroundWindow();
	if(hWndForeground == hWnd) return;

	DWORD Strange = ::GetWindowThreadProcessId(hWndForeground, NULL);
	DWORD My = ::GetWindowThreadProcessId(hWnd, NULL);
	if( !::AttachThreadInput(Strange, My, TRUE) )
	{
		ASSERT(0);
	}
	::SetForegroundWindow(hWnd);
	::BringWindowToTop(hWnd);
	if( !::AttachThreadInput(Strange, My, FALSE) )
	{
		ASSERT(0);
	}
}

//
BOOL CreateViewerWindow(HINSTANCE hInstance)
{
	HWND hWnd;
	int nCmdShow, cx, cy, cxIcon, cyIcon, cxIconSm, cyIconSm;
	DWORD dwStyle;
	WNDCLASSEX wcex;
	UINT32 idIcon;
	char szTitle[MAX_PATH];
	INT32 x, y;

	I3PRINTLOG(I3LOG_VIEWER, "Viewer init >>>");

	hInst = hInstance; // Store instance handle in our global variable

	_pInterface->GetVideoConfig( &g_VideoInfo);

	I3PRINTLOG(I3LOG_VIEWER, "* 1");
	if( _pInterface->GetInitialWindowVisible() == false)
		nCmdShow = SW_HIDE;
	else
		nCmdShow = SW_SHOW;

	dwStyle = _pInterface->GetViewportStyle();
	idIcon = _pInterface->GetIcon();

	I3PRINTLOG(I3LOG_VIEWER, "* 2");

	cxIcon = GetSystemMetrics( SM_CXICON);
	cyIcon = GetSystemMetrics( SM_CYICON);
	cxIconSm = GetSystemMetrics( SM_CXSMICON);
	cyIconSm = GetSystemMetrics( SM_CYSMICON);

	wcex.cbSize = sizeof(WNDCLASSEX);
#if 1	/// UI Double click 일단 게임 진행에 있어 테스트가 필요함.. 순구
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
#else
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
#endif
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;

	if( idIcon == 0)
	{
		wcex.hIcon			= 0;
		wcex.hIconSm		= 0;
	}
	else
	{
		wcex.hIcon			= (HICON) LoadImage( g_hModule, (LPCTSTR) idIcon, IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		wcex.hIconSm		= (HICON) LoadImage( g_hModule, (LPCTSTR) idIcon, IMAGE_ICON, cxIconSm, cyIconSm, LR_DEFAULTCOLOR);
	}

	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)::GetStockObject(BLACK_BRUSH);							//(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_I3VIEWER;
	wcex.lpszClassName	= szWindowClass;

	RegisterClassEx( &wcex);

	I3PRINTLOG(I3LOG_VIEWER, "* 3");

	i3::safe_string_copy( szTitle, GAME_APP_NAME_A, MAX_PATH);

	if( g_VideoInfo.m_bFullScreen)
	{
		hWnd = CreateWindow( szWindowClass, szTitle, dwStyle, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	}
	else
	{
		RECT Rect;

		Rect.left = 0;
		Rect.top = 0;
		Rect.right = g_VideoInfo.m_Width;
		Rect.bottom = g_VideoInfo.m_Height;

		AdjustWindowRect( &Rect, dwStyle, FALSE);

		cx = Rect.right - Rect.left;
		cy = Rect.bottom - Rect.top;

		x = ( GetSystemMetrics( SM_CXSCREEN) >> 1) - (cx >> 1);
		y = ( GetSystemMetrics( SM_CYSCREEN) >> 1) - (cy >> 1);


		hWnd = CreateWindow( szWindowClass, szTitle, dwStyle, x, y, cx, cy, NULL, NULL, hInstance, NULL);
	}

	I3PRINTLOG(I3LOG_VIEWER, "* 4");

	if (!hWnd)
	{
		if (wcex.hIcon)
			::DestroyIcon(wcex.hIcon);
		if (wcex.hIconSm)
			::DestroyIcon(wcex.hIconSm);

		return FALSE;
	}

	g_hwndMain = hWnd;

	DragAcceptFiles( hWnd, TRUE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 게임에 불필요한 여러가지 키 입력을 막는다.
	/*{
	RegisterHotKey( hWnd, 0xb000, MOD_ALT,							VK_TAB);
	RegisterHotKey( hWnd, 0xb001, MOD_ALT | MOD_SHIFT,				VK_TAB);
	RegisterHotKey( hWnd, 0xb002, MOD_ALT,							VK_ESCAPE);
	RegisterHotKey( hWnd, 0xb003, MOD_ALT | MOD_SHIFT,				VK_ESCAPE);
	RegisterHotKey( hWnd, 0xb004, MOD_WIN,							VK_TAB);
	RegisterHotKey( hWnd, 0xb005, MOD_WIN | MOD_SHIFT,				VK_TAB);
	RegisterHotKey( hWnd, 0xb006, MOD_CONTROL,						VK_ESCAPE);
	RegisterHotKey( hWnd, 0xb010, 0, VK_HANJA);				//	한자 버튼
	RegisterHotKey( hWnd, 0xb020, MOD_WIN, VK_LWIN);		//	왼쪽 윈도우 버튼 (작동안하는 듯...)
	RegisterHotKey( hWnd, 0xb030, MOD_WIN, VK_RWIN);		//	오른쪽 윈도우 버튼 (작동안하는 듯...)
	RegisterHotKey( hWnd, 0xb040, MOD_ALT, VK_MENU);		//	메뉴(Alt) 버튼
	RegisterHotKey( hWnd, 0xb050, MOD_ALT, VK_RMENU);
	RegisterHotKey( hWnd, 0xb060, MOD_ALT, VK_LMENU);
	}*/

	I3PRINTLOG(I3LOG_VIEWER, "Viewer init <<<");

	return TRUE;
}

/*
void InstallVCRedist()
{
CString csProduct = "{9A25302D-30C0-39D9-BD6F-21E6EC160475}";

INSTALLSTATE t = MsiQueryProductState(csProduct);
if(INSTALLSTATE_DEFAULT != t)
{
I3PRINTLOG(I3LOG_VIEWER, "Install vcredist_x86.exe >>>");

UINT32 uiTemp = WinExec("vcredist_x86.exe /qb!",SW_SHOW);

if(uiTemp <= 31) {
char szTemp[256];
i3::snprintf( szTemp, 256, "*Return code : %d", uiTemp);
I3PRINTLOG(I3LOG_VIEWER, szTemp);
I3PRINTLOG(I3LOG_VIEWER, "Install vcredist_x86.exe fail ");
return;
}

while(INSTALLSTATE_DEFAULT != MsiQueryProductState(csProduct)) {
Sleep(30);
}

I3PRINTLOG(I3LOG_VIEWER, "Install vcredist_x86.exe <<<");
}
}
*/

#ifdef NC_BUILD
void _ProhibitFile_Install_();
//void _ProhibitFile_GetFileMD5_();
void _ProhibitFile_GetUserFileListMD5_();
#endif


BOOL _XignCode_Start_();
void _XignCode_Release_();
void _Xtrap_Start_();
void _Xtrap_Register_();


bool _WebLogin_();
void _InstallKeyhook_();

void _WindowModeDisable_Start_();
void _WindowModeDisable_Release_();

void LogFileMD5(char* path);

bool _CheckMultiRunning_();

void _ApplyLeftHander_();

void _ParseParameters_(LPTSTR lpCmdLine);
bool _PreGameExecution_(LPTSTR lpCmdLine);


void _Message_Fail_i3ExecFile_();
void _Message_Fail_Interface_();
void _Message_Fail_ViewerWindow_();
void _Message_Fail_PreFramework_();
void _Message_Fail_GraphicCard_();
void _Message_Fail_Create_Framework_();
void _Message_Fail_Initialize_Framework_();

void _RegistryLogName_();

static void _OutOfMemoryHandler();
void AddRegestryGameFile();

void cbZGuardTest(unsigned int nErrorCode)
{
	i3::string str_msg;
	i3::sprintf(str_msg,"ZGuard ERROR[%u]",nErrorCode);
	I3PRINTLOG(I3LOG_VIEWER, str_msg.c_str() );
	exit(0);
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UINT old_error_mode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

	//size_t gamecontextsize = sizeof(CGameContext);
	i3GfxResource::setExternalOutOfMemory( _OutOfMemoryHandler);
	//new int[5];

	//i3::set_break_alloc(6337);
	//_CrtSetBreakAlloc(1202853); //여기에 몇번째 메모리 할당이 일어나며 브레이크 포인터를 설정하게 셋팅한다.
	//	new int[32];

#ifdef USE_I3EXEC
	g_hInstance = hInstance; 
#endif

#if defined( USE_DLL_DETECTED )
	Install_ProcessModules();
#endif

#if defined( USE_HACK_DETECTED )
	Enable_GuardDll(hInstance, NULL, NULL);
#endif 

#if defined(I3_DEBUG)
	char ExecName[MAX_STRING_COUNT] = "";
	#ifdef USE_I3EXEC
		i3::snprintf( ExecName, MAX_STRING_COUNT, "%s_DEBUG.i3Exec", GAME_TITLE_NAME_A);
	#else
		i3::snprintf( ExecName, MAX_STRING_COUNT, "%s_DEBUG_UI2.exe", GAME_TITLE_NAME_A);
	#endif
#else
	char ExecName[MAX_STRING_COUNT] = "";
	#ifdef USE_I3EXEC
		i3::snprintf( ExecName, MAX_STRING_COUNT, "%s.i3Exec", GAME_TITLE_NAME_A);
	#else
		i3::snprintf( ExecName, MAX_STRING_COUNT, "%s_UI2.exe", GAME_TITLE_NAME_A);
	#endif
#endif

#if defined (I3_DEBUG)//메모리 세팅
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetBreakAlloc(2853056);

	AfxWinInit( hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif

	CoInitialize( NULL );

	// 전체화면에서 윈도우 입력도구모음 안나오게
	i3Ime::ShowLanguageBarWnd(false);

	__startlog( ExecName );
	MessageBox(0, lpCmdLine, 0, 0);
	I3PRINTLOG(I3LOG_VIEWER, "startLog");
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3PRINTLOG(I3LOG_VIEWER, lpCmdLine);
#endif

#if defined( USE_DLL_DETECTED )
	ModuleList List;
	Detect_ProcessModule(List);
#endif

	SteamContext::i()->SteamAPI_Init(lpCmdLine);

	AddRegestryGameFile();

	if (!_XignCode_Start_())
	{
		return 0;
	}

#ifdef USE_LEUCO_SHELL
	FILE* LS_LOG_FILE = fopen("./SET_LS_LOG.log", "rt");
	if (LS_LOG_FILE)
	{
		INT32 LogFlag = 0;
		fscanf(LS_LOG_FILE, "%d", &LogFlag);
		LS::COMMON::FUNCTION::SET_LOG_FLAG((BYTE)LogFlag);
		fclose(LS_LOG_FILE);
	}
	//
	X_LEUCO_SHELL_X

#endif


#if defined( USE_DLL_DETECTED )
	ModuleList List2;
	Detect_ProcessModule(List2);
#endif

	_Xtrap_Start_();

	//	InstallVCRedist();
	TryLoadSymbol( ExecName );

	_RegistryLogName_();

#ifdef NC_BUILD
	_ProhibitFile_Install_();
//	_ProhibitFile_GetFileMD5_();
#endif
	

	if( !_WebLogin_() )
	{
#if defined( USE_HACK_DETECTED )
		Disable_GuardDll();
#endif
		return 0;
	}

	_InstallKeyhook_();

//	Detect_ProcessModule(List);

	_WindowModeDisable_Start_();

	MSG msg;
	HACCEL hAccel;
	msg.wParam = WM_USER;

	_ParseParameters_(lpCmdLine);

	if( _PreGameExecution_(lpCmdLine) )
	{
#if defined( USE_HACK_DETECTED )
		Disable_GuardDll();
#endif
		return 0;
	}

#ifdef NC_BUILD
	_ProhibitFile_GetUserFileListMD5_();
#endif


	I3PRINTLOG(I3LOG_VIEWER, "TryLoadGame >>>");
	_pInterface = TryLoadGame();
	I3PRINTLOG(I3LOG_VIEWER, "TryLoadGame <<<");

	if( _pInterface == NULL)
	{
		_Message_Fail_i3ExecFile_();
		goto ExitPart;
	}

#ifdef USE_I3EXEC
	I3PRINTLOG(I3LOG_VIEWER, "CheckModule >>>");
	if ( ! CheckModule() )
	{
		goto ExitPart;
	}
	I3PRINTLOG(I3LOG_VIEWER, "CheckModule <<<");
#endif

	_Xtrap_Register_();
	

	if( !_CheckMultiRunning_() )
	{
		goto ExitPart;
	}

	// Initialize global strings
	I3PRINTLOG(I3LOG_VIEWER, "LoadString >>>");
	LoadString(hInstance, IDC_I3VIEWER, szWindowClass, MAX_LOADSTRING);
	I3PRINTLOG(I3LOG_VIEWER, "LoadString <<<");

	I3PRINTLOG(I3LOG_VIEWER, "Memory Initialize >>>");
	/*i3mem::SetDumpEnable(TRUE);
	i3MemoryInit( _pInterface->GetInitialMemSize(), "D:/PBMem.txt");*/
	i3MemoryInit( _pInterface->GetInitialMemSize());
	I3PRINTLOG(I3LOG_VIEWER, "Memory Initialize <<<");

	I3PRINTLOG(I3LOG_VIEWER, "Register Metas >>>");
	_pInterface->OnRegisterMeta();
	I3PRINTLOG(I3LOG_VIEWER, "Register Metas <<<");

	I3PRINTLOG(I3LOG_VIEWER, "File Size Check >>>==========================");
	LogFileMD5(".\\*.dll");
	LogFileMD5(".\\*.exe");
	I3PRINTLOG(I3LOG_VIEWER, "=============================================");


	I3PRINTLOG(I3LOG_VIEWER, "Init instance >>>");
	if( _pInterface->OnInitInstance( hInstance, g_hwndMain) == false)
	{
		_Message_Fail_Interface_();
		goto ExitPart;
	}
	I3PRINTLOG(I3LOG_VIEWER, "Init instance <<<");

#ifdef USE_I3EXEC
	ppConfig = (CConfigEx**) GetProcAddress( g_hModule, "g_pConfigEx");
#else
	ppConfig = &g_pConfigEx;
#endif

	


	// Perform application initialization:
	if (! CreateViewerWindow( hInstance))
	{
		_Message_Fail_ViewerWindow_();
		goto ExitPart;
	}

	//nProtect때문에 만듬
	I3PRINTLOG(I3LOG_VIEWER, "Pre create framework >>>");

#ifdef CHAMPETITION_BUILD
	// 강제로 셰이더 버전 올림으로써 강제로 셰이더를 재컴파일 시킨다.
	const UINT32 prevShaderVer = i3ShaderCache::GetRequestedISSVersion();
	const UINT32 currShaderVer = prevShaderVer + 1;
	i3ShaderCache::SetRequestedISSVersion(currShaderVer);
#endif

#ifdef USE_SHADERCODE_BY_RESOURCEPACK
	IShaderSourceCodeMgr* pRscPackShader = GetShaderSourceCodeMgr_ResourcePack();
	SetShaderSourceCodeMgr(pRscPackShader);
#endif

#if defined( _XTRAP_)
	// X-TRAP 연동. CraeteWindow 직후에 호출하는것이 좋다.
	XTrap_C_KeepAlive();
#else	// X-TRAP 외의 보안 모듈

	

	if(! _pInterface->CreatePreFramework(g_hwndMain ) )
	{
		_Message_Fail_PreFramework_();
		goto ExitPart;
	}
#endif
	I3PRINTLOG(I3LOG_VIEWER, "Pre create framework <<<");

	
	/////////////////////////////////////////////
	{
		_pViewer = i3Viewer::new_object();



		I3ASSERT( _pViewer != NULL );

		// _pViewer->AddState( I3_VIEWER_STATE_INGAME | I3_VIEWER_INPUT_NO_WINMSG | I3_VIEWER_STATE_MULTITHREAD);
		_pViewer->AddState( I3_VIEWER_STATE_INGAME | I3_VIEWER_INPUT_NO_WINMSG);

		I3PRINTLOG(I3LOG_VIEWER, "i3Viewer create >>>");

		//BOOL	DialogBoxMode = FALSE;

		//if ( (*ppConfig)->GetShop().ChargeCash_InGame )
		//{
		//	// 캐쉬 충전 또는 웹공지를 사용할 경우 DX위에서 웹화면을 이용하기 위한 옵션
		//	DialogBoxMode = TRUE;
		//}
		//else 
		//{
		//	DialogBoxMode = FALSE;
		//}
		//제압 미션 Web 표시로 인해서 항상 TRUE..

		if( _pViewer->Create( (void *) g_hwndMain, &g_VideoInfo, _pInterface->GetInputDeviceMask(), TRUE, TRUE) == FALSE)
		{
			_Message_Fail_GraphicCard_();
			goto ExitPart;
		}

#if defined(I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
		_pViewer->ActivateOccDebugInfo(true);
#endif

#if defined (_NPROTECT_)
		I3PRINTLOG(I3LOG_VIEWER, "Checking D3D file <<<");
		LPVOID lpD3DDevice = (IDirect3DDevice9*)_pViewer->GetRenderContext()->GetNativeDevice();
		if( nProtect::Set_D3DDeviceInfo(D3D_SDK_VERSION, (LPVOID*)&lpD3DDevice ) == false )
		{
			_Message_Fail_D3D_Device_();
			goto ExitPart;
		}
		I3PRINTLOG(I3LOG_VIEWER, "Checking D3D file  >>>");
#endif
		I3PRINTLOG(I3LOG_VIEWER, "i3Viewer create <<<");

		I3PRINTLOG(I3LOG_VIEWER, "i3Framework create >>>");

		if ( i3::contain_string(g_szStartStage, "V15") != -1 )
		{
			g_pEnvSet->SetUIVersion(0);
			g_szStartStage[0] = 0; 
		}
		else if ( i3::contain_string(g_szStartStage, "V11") != -1 )
		{
			g_pEnvSet->SetUIVersion(1);
			g_szStartStage[0] = 0; 
		}
		else if ( i3::contain_string(g_szStartStage, "V10") != -1 )
		{
			g_pEnvSet->SetUIVersion(2);
			g_szStartStage[0] = 0; 
		}
		else if ( i3::contain_string(g_szStartStage, "OLD_EQUIPMENT") != -1 )
		{
			g_szStartStage[0] = 0; 
		}

#ifdef USE_I3EXEC
		_pFramework = _pInterface->CreateFramework( _pViewer);
#else
		_pFramework = (CGameFramework*)_pInterface->CreateFramework( _pViewer);
#endif
		//UI  초기화
		((CGameFramework*)_pFramework)->SetMainLocale();

		if( _pFramework == NULL)
		{
			_Message_Fail_Create_Framework_();
			goto ExitPart;
		}
		I3PRINTLOG(I3LOG_VIEWER, "i3Framework create <<<");

		SetViewerKeyhook( _pViewer);

		// 만약 지정된 Start Stage가 있다면...
		// 바로가기 맵로딩테스트
		if( g_szStartStage[0] != 0)
		{
			char szStageName[128] = {};
			char szStageType[128] = {};
			char * pszDest = NULL, * pszSrc = NULL;
			INT32 i;

			// StageName(StageType) 형식의 입력을 Parsing한다.

			pszSrc = g_szStartStage;
			pszDest = szStageName;

			for( i = 0; *pszSrc != 0; i++)
			{
				switch( *pszSrc)
				{
				case '(' :
				case '[' :
				case ')' :
				case ']' :
					*pszDest = 0;

					pszDest = szStageType;
					break;

				default :
					*pszDest = *pszSrc;
					pszDest++;
					break;
				}

				pszSrc ++;
			}

			if( (pszDest != szStageName) && (pszDest != szStageType))
				*pszDest = 0;

			INT32 idx = _pFramework->FindStageInfoByName( szStageName);

			if( idx != -1)
			{
				i3StageInfo * pStartStage = _pFramework->getStageInfo( idx);

				_pFramework->SetStartStageInfo( pStartStage);
			}
			else if ( i3::contain_string(szStageName, "lang:") != -1 )
			{
				// 커맨드 라인으로 로드할 스트링테이블을 지정하는 커맨드 명령어 입니다.
				// 현재 커맨드라인 구조상 이 if문이 없다면 잘못된 스테이지 로드명령으로 입력되어
				// 문제가 발생하기 때문입니다.
				// 관련함수 : ParseLanguegeCommand
			}
			else
			{				
				i3StageInfo * pStartStage = i3StageInfo::new_object_ref();

				pStartStage->SetName( szStageType);
				pStartStage->setStageClassName( szStageType);

				_pFramework->AddStageInfo( pStartStage);
				_pFramework->SetStartStageInfo( pStartStage);
			}
		}

		I3PRINTLOG(I3LOG_VIEWER, "Initialize Framework >>>");

		if( _pInterface->OnInitFramework( _pFramework, lpCmdLine) == false)
		{
			_Message_Fail_Initialize_Framework_();
			goto ExitPart;
		}
		I3PRINTLOG(I3LOG_VIEWER, "Initialize Framework <<<");
	}

	_ApplyLeftHander_();

#ifndef USE_I3EXEC
	SetForegroundWindowForce(g_hWnd);
#endif

	//////////////////////////////////////////////////////////////////////////

	hAccel = LoadAccelerators(hInstance, (LPCTSTR)IDC_I3VIEWER);

	// Now we're ready to recieve and process Windows messages.
	BOOL bContinue = TRUE;
	//UINT32	IntervalForFrame = 1000 / _pInterface->GetTargetFPS();

	msg.message = WM_NULL;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	g_bCanUnhook = true;

#ifdef USE_LEUCO_SHELL
	LEUCO_SHELL_START();
#endif
	while((bContinue ) && (WM_QUIT != msg.message  ))
	{

		INT32 cnt;

		for( cnt = 0; (cnt < 30) && PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ); cnt++)
		{
			// Translate and dispatch the message
			if( 0 == TranslateAccelerator( g_hwndMain, hAccel, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		}

		if( _pViewer->IsState( I3_VIEWER_STATE_EXIT))
			bContinue = FALSE;
		else
			bContinue = _pInterface->OnDrive( _pViewer);
#ifdef USE_LEUCO_SHELL
		LEUCO_SHELL_RUN((PDWORD*)g_pViewer->GetRenderContext()->GetNativeDevice());
#endif
	}

ExitPart:

	ShowWindow(g_hwndMain, SW_HIDE);
	_WindowModeDisable_Release_();

	i3RegArray::ReleaseSkipFPBank();

	
	// Alt+Tab 풀기
	I3PRINTLOG(I3LOG_VIEWER, "Finish Game >>>");


	UninstallKeyhook();

	I3PRINTLOG(I3LOG_VIEWER, "* 1");

	if( g_hMutex != NULL)
	{
		CloseHandle( g_hMutex );
	}

	//종료할때 마우스이동 자유롭게 풀어준다
	ClipCursor(NULL);


	I3PRINTLOG(I3LOG_VIEWER, "* 2");
	if( _pInterface != NULL)
	{
		_pInterface->OnExitInstance();
		// 메모리 릭으로 메모리 해체 추가 - 박명진 2008. 5. 22
		I3PRINTLOG(I3LOG_VIEWER, "* 2.1");
	}

	I3PRINTLOG(I3LOG_VIEWER, "* 3");

	I3_SAFE_RELEASE( _pFramework);

	I3PRINTLOG(I3LOG_VIEWER, "* 4");
	I3_SAFE_RELEASE( _pViewer);

#ifndef USE_I3EXEC
	g_pViewer		= NULL;
	g_pFramework	= NULL;
#endif


	i3System::TerminateSys();

	#if defined( I3_DEBUG)
	//i3ElementBase::DumpObjects();

	//i3MemDump( I3MEMORY_DEBUG_FILE | I3MEMORY_DEBUG_SUM | I3MEMORY_DEBUG_EACH);
	#endif

	I3PRINTLOG(I3LOG_VIEWER, "* 5");
#ifdef USE_I3EXEC
	if( g_hModule != NULL)
	{
		FreeLibrary( g_hModule);
	}
#else

#endif
	I3PRINTLOG(I3LOG_VIEWER, "* 6");
	CoUninitialize();

	I3PRINTLOG(I3LOG_VIEWER, "Finish Game <<<");

	_XignCode_Release_();

#if defined( I3_DEBUG)
	// _pInterface를 delete하면 특정 컴퓨터에서 종료시 크래쉬하는 현상이 발생하여
	// 의도적으로 메모리 해제를 막습니다.
	// delete _pInterface;

	CloseDebugPanel();
#endif

#if defined( USE_HACK_DETECTED )
	Disable_GuardDll();
#endif

	::SetErrorMode(old_error_mode);
	
	return (int) msg.wParam;
}

void _RegistryLogName_()
{
#ifdef USE_I3EXEC
	LOG_PROC pProc = (LOG_PROC)GetProcAddress(g_hModule, "_setLogName_");
	if( pProc == NULL) 
	{
		I3PRINTLOG(I3LOG_VIEWER, "can not found _setLogName_ Proc");
	}
	else
	{
		pProc( __getLogName() ); 
	}
#else
	_setLogName_( __getLogName() );
#endif
}
BOOL _XignCode_Start_()
{
#if defined(_XIGNCODE_)

	if (!ZCWAVE_SysInit())
	{
		ULONG ErrorCode = GetLastError();
		I3PRINTLOG(I3LOG_NOTICE, "ErrorCode %08x \n", ErrorCode);
		return FALSE;
	}


	if (!ZCWAVE_SysEnter(XIGNCODE_KEY, XIGNCODE_PATH, 0))
	{
		ULONG ErrorCode = GetLastError();
		I3PRINTLOG(I3LOG_NOTICE, "ErrorCode %08x \n", ErrorCode);
		return FALSE;
	}

	if (!ZCWAVE_SendCommand(ZCMD_SET_USER_INFO_CALLBACKA, MyUserInfoCallBack, NULL))
	{
		ULONG ErrorCode = GetLastError();
		I3PRINTLOG(I3LOG_NOTICE, "ErrorCode %08x \n", ErrorCode);
		return FALSE;
	}


#endif

	return TRUE;
}
#ifdef NC_BUILD
void _ProhibitFile_Install_()
{
	I3PRINTLOG(I3LOG_VIEWER, "__ProhibitFile_Install >>>");

#ifdef USE_I3EXEC
	PROHIBIT_PROC pProc = (PROHIBIT_PROC)GetProcAddress(g_hModule, "__ProhibitFile_Install");
	if( pProc == NULL)
	{
		I3PRINTLOG(I3LOG_VIEWER, "can not found __ProhibitFile_Install Proc");
	}
	else
	{
		(*pProc)();
	}
#else
	__ProhibitFile_Install();
#endif

	I3PRINTLOG(I3LOG_VIEWER, "__ProhibitFile_Install <<<");
}

//void _ProhibitFile_GetFileMD5_()
//{
//	I3PRINTLOG(I3LOG_VIEWER, "_ProhibitFile_GetFileMD5_ >>>");
//
//	PROHIBIT_PROC pProc = (PROHIBIT_PROC)GetProcAddress(g_hModule, "__ProhibitFile_GetFileMD5");
//	if( pProc == NULL)
//	{
//		I3PRINTLOG(I3LOG_VIEWER, "can not found __ProhibitFile_GetFileMD5 Proc");
//	}
//	else
//	{
//		(*pProc)();
//	}
//
//	I3PRINTLOG(I3LOG_VIEWER, "_ProhibitFile_GetFileMD5_ <<<");
//}

void _ProhibitFile_GetUserFileListMD5_()
{
	I3PRINTLOG(I3LOG_VIEWER, "__ProhibitFile_GetUserFileListMD5 >>>");

#ifdef USE_I3EXEC
	PROHIBIT_PROC pProc = (PROHIBIT_PROC)GetProcAddress(g_hModule, "__ProhibitFile_GetUserFileListMD5");
	if( pProc == NULL)
	{
		I3PRINTLOG(I3LOG_VIEWER, "can not found __ProhibitFile_GetUserFileListMD5 Proc");
	}
	else
	{
		(*pProc)();
	}
#else
	__ProhibitFile_GetUserFileListMD5();
#endif
	I3PRINTLOG(I3LOG_VIEWER, "__ProhibitFile_GetUserFileListMD5 <<<");
}
#endif

void _XignCode_Release_()
{
#if defined(_XIGNCODE_)

	I3PRINTLOG(I3LOG_VIEWER, "XignCode Release >>>");
	ZCWAVE_SysExit();
	ZCWAVE_SysCleanup();
	I3PRINTLOG(I3LOG_VIEWER, "_XignCode_ <<<");

#endif
}

/////////////////////////////////////////////////////////////////////////////
//									X-Trap
/////////////////////////////////////////////////////////////////////////////
// revision 32248 해킹 탐지 코드 추가
#if defined( _XTRAP_)
void _XTRAP_CALLBACK__XtrapCallbackProcedure (IN unsigned int Reserve1, 
											  IN unsigned int Reserve2, 
											  IN unsigned int Reserve3, 
											  IN unsigned int Reserve4,
											  IN XTRAP_CODE *pXtrapCode)     // [in, out] Pointer to a XTRAP_CODE data structure
{
	if (!pXtrapCode) return;

	// revision 32966
	GameEventSender::i()->SetEvent(EVENT_XTRAP_HACK_DETECTED, &pXtrapCode->Status);
} 
#endif

void _Xtrap_Start_()
{
#if defined( _XTRAP_)
	I3PRINTLOG(I3LOG_VIEWER, "_Xtrap_ >>>");
	char szXTrapArg[] = XTRAP_ARGUMENT_KEY;

	// revision 32248 해킹 탐지 코드 추가
	XTrap_C_Start_Callback( (LPCSTR) szXTrapArg, NULL, static_cast<void*>(_XTRAP_CALLBACK__XtrapCallbackProcedure));

	// X-TRAP 주기적인 Alive check하는 쓰레드 생성
	DWORD dwThreadId;
	CreateThread( NULL, 0, XTrapCallbackAliveThreadFunc, NULL, 0, &dwThreadId);

	I3PRINTLOG(I3LOG_VIEWER, "X-TRAP Start");
	I3PRINTLOG(I3LOG_VIEWER, "_Xtrap_ <<<");
#endif
}

void _Xtrap_Register_()
{
#if defined( _XTRAP_)
	// i3GameInterface에 X-TRAP과 연동할 콜백 함수 등록
	_pInterface->SetInteractProc3( CSStep2Proc);
#endif
}



bool _WebLogin_()
{
#if defined( USE_ONLY_WEB_LOGIN )
	I3PRINTLOG(I3LOG_VIEWER, "Web Login Start");

	// 웹 세션 로그인이 아니라면 종료 - ServerAddr, MacAddr, SessKey 존재해야함
	BOOL beEnvironmentVariable = TRUE;
	char strEnvBufer[256];

	if (0 == ::GetEnvironmentVariable("ServerAddr", strEnvBufer, 256))
	{
		beEnvironmentVariable = FALSE;
	}

	if (0 == ::GetEnvironmentVariable("MacAddr", strEnvBufer, 256))
	{
		beEnvironmentVariable = FALSE;
	}

	if (0 == ::GetEnvironmentVariable("SessKey", strEnvBufer, 256))
	{
		beEnvironmentVariable = FALSE;
	}

	if (!beEnvironmentVariable)
	{
		if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
		{
			MessageBox( NULL, "실행에 필요한 환경변수를 읽어올 수 없습니다.\r\n종료합니다.", "Error", MB_OK);
		}
		else
		{
			MessageBox( NULL, "Could not retreive environment variables from the system.\r\nExit.", "Error", MB_OK);
		}
		return false;
	}

	I3PRINTLOG(I3LOG_VIEWER, "Web Login End");
#endif

	return true;
}

void _InstallKeyhook_()
{
	I3PRINTLOG(I3LOG_VIEWER, "Install Hook >>>");
	if( InstallKeyhook( GetCurrentThreadId(), NULL, false) == false)
	{
	}
	I3PRINTLOG(I3LOG_VIEWER, "Install Hook <<<");
}

void _WindowModeDisable_Start_()
{
#if defined(WINDOW_MODE_DISABLE)
	I3PRINTLOG(I3LOG_VIEWER, "Install WindowModeDisable! <<<");

	CODEREPLACE_START

	AltTab1_Enable_Disable(FALSE);
	AltTab2_Enable_Disable(NULL, FALSE);
	TaskSwitching_Enable_Disable(FALSE);
	TaskManager_Enable_Disable(FALSE);
	CtrlAltDel_Enable_Disable(FALSE);

	if( !g_pdwStickyKey )	{		g_pdwStickyKey = new STICKYKEYS;	}
	if( !g_pdwFilterKey )	{		g_pdwFilterKey = new FILTERKEYS;	}
	if( !g_pdwToggleKey ){		g_pdwToggleKey = new TOGGLEKEYS;	}

	// 고정키 막기 
	// 백업     
	g_pdwStickyKey->cbSize = sizeof(STICKYKEYS);
	g_pdwStickyKey->dwFlags = 0;
	SystemParametersInfo( SPI_GETSTICKYKEYS, sizeof(STICKYKEYS), g_pdwStickyKey, 0 );
	g_dwStickyKeysFlagBackup = g_pdwStickyKey->dwFlags;
	// 설정
	if( (g_dwStickyKeysFlagBackup & SKF_STICKYKEYSON) == 0 )
	{
		g_pdwStickyKey->dwFlags &= ~(SKF_CONFIRMHOTKEY | SKF_HOTKEYACTIVE);
		SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), g_pdwStickyKey, 0 );
	}

	// 필터키 막기
	// 백업
	g_pdwFilterKey->cbSize = sizeof(FILTERKEYS);
	g_pdwFilterKey->dwFlags = 0;
	SystemParametersInfo( SPI_GETFILTERKEYS, sizeof(FILTERKEYS), g_pdwFilterKey, 0 );
	g_dwFilterKeysFlagBackup = g_pdwFilterKey->dwFlags;
	// 설정
	if( (g_dwFilterKeysFlagBackup & FKF_FILTERKEYSON) == 0 )
	{
		g_pdwFilterKey->dwFlags &= ~(FKF_CONFIRMHOTKEY | FKF_HOTKEYACTIVE);
		SystemParametersInfo( SPI_SETFILTERKEYS, sizeof(FILTERKEYS), g_pdwFilterKey, 0 );
	}

	// 토글키 막기
	// 백업
	g_pdwToggleKey->cbSize = sizeof(TOGGLEKEYS);
	g_pdwToggleKey->dwFlags = 0;
	SystemParametersInfo( SPI_GETTOGGLEKEYS, sizeof(TOGGLEKEYS), g_pdwToggleKey, 0 );
	g_dwFilterKeysFlagBackup = g_pdwToggleKey->dwFlags;
	// 설정
	if( (g_dwFilterKeysFlagBackup & TKF_TOGGLEKEYSON) == 0 )
	{
		g_pdwToggleKey->dwFlags &= ~(TKF_CONFIRMHOTKEY | TKF_HOTKEYACTIVE);
		SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), g_pdwToggleKey, 0);
	}

	CODEREPLACE_END

	I3PRINTLOG(I3LOG_VIEWER, "Install WindowModeDisable! <<<");
#endif
}

void _WindowModeDisable_Release_()
{
#if defined(WINDOW_MODE_DISABLE)
	I3PRINTLOG(I3LOG_VIEWER, "Release WindowModeDisable! >>>");

	AltTab1_Enable_Disable(TRUE);
	AltTab2_Enable_Disable(NULL, TRUE);
	TaskSwitching_Enable_Disable(TRUE);
	TaskManager_Enable_Disable(TRUE);
	CtrlAltDel_Enable_Disable(TRUE);

	// 고정키 복구
	if( (g_dwStickyKeysFlagBackup & SKF_STICKYKEYSON) == 0 )
	{
		g_pdwStickyKey->dwFlags = g_dwStickyKeysFlagBackup;
		SystemParametersInfo( SPI_SETSTICKYKEYS, sizeof(STICKYKEYS), g_pdwStickyKey, 0 );
	}
	// 필터키 복구
	if( (g_dwFilterKeysFlagBackup & FKF_FILTERKEYSON) == 0 )
	{
		g_pdwFilterKey->dwFlags = g_dwFilterKeysFlagBackup;
		SystemParametersInfo( SPI_SETFILTERKEYS, sizeof(FILTERKEYS), g_pdwFilterKey, 0 );
	}
	// 토글키 복구
	if( (g_dwFilterKeysFlagBackup & TKF_TOGGLEKEYSON) == 0 )
	{
		g_pdwToggleKey->dwFlags = g_dwFilterKeysFlagBackup;
		SystemParametersInfo(SPI_SETTOGGLEKEYS, sizeof(TOGGLEKEYS), g_pdwToggleKey, 0);
	}

	if( g_pdwStickyKey )	{	delete g_pdwStickyKey;	g_pdwStickyKey = 0;	}
	if( g_pdwFilterKey )		{	delete g_pdwFilterKey;		g_pdwFilterKey = 0;	}
	if( g_pdwToggleKey )	{	delete g_pdwToggleKey;	g_pdwToggleKey = 0;}

	I3PRINTLOG(I3LOG_VIEWER, "Release WindowModeDisable <<<");
#endif
}


void LogFileMD5(char* path)
{
	_finddata_t fd;
	long handle;
	int result = 1;
	handle = _findfirst(path, &fd);
	if (handle != -1)
	{
		while (result != -1)
		{
			unsigned char md5Key[16] = {};
			UINT128 ui128Temp;
			char strMD5Key[33] = {};

			md5_file(fd.name, md5Key);
			memcpy(&ui128Temp, md5Key, sizeof(char) * 16);
			i3MD5::GetMD5String(strMD5Key, ui128Temp, sizeof(strMD5Key));

			I3PRINTLOG(I3LOG_VIEWER, "File: %s \t Size: %d\t (%s)\n", fd.name, fd.size, strMD5Key);
			result = _findnext(handle, &fd);
		}
		_findclose(handle);
	}
}


bool _CheckMultiRunning_()
{
#ifdef NC_BUILD
	// 이중 실행을 방지한다.
	I3PRINTLOG(I3LOG_VIEWER, "Check multiple running >>>");
	if( _pInterface->GetMultipleInstanceEnable() == false)
	{
		g_hMutex = OpenMutex( MUTEX_ALL_ACCESS, FALSE, _pInterface->GetTitle());
		if( g_hMutex == NULL )
		{
			g_hMutex = ::CreateMutex( NULL, TRUE, _pInterface->GetTitle());
		}
		else
		{
			char conv[256];

			if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
			{
				sprintf_s( conv, "%s 프로그램이 이미 실행 중입니다.", _pInterface->GetTitle());
			}
			else
			{
				sprintf_s( conv, "%s is running already.", _pInterface->GetTitle());
			}

			MessageBox( NULL, conv, NULL, 0 );
			return false;
		}
	}
	I3PRINTLOG(I3LOG_VIEWER, "Check multiple running <<<");
#endif
	return true;
}

void _ApplyLeftHander_()
{
	//20110413 하동익 : 왼손잡이 마우스 세팅
#if defined( LOCALE_ITALY)
	{
		bool bResult = GetSystemMetrics(SM_SWAPBUTTON)!=0?true:false;
		_pViewer->GetInputDeviceManager()->GetMouse()->SetUseLeftMouse(bResult);
	}
#endif
}

void _ParseParameters_(LPTSTR lpCmdLine)
{
#if defined (DEV_BUILD)

	// Steam 로그인에 한해서는 아래 처리하지 않습니다.
	if (SteamContext::i()->IsSteamUser())
		return;

	// 정의된 인자(스테이지명) 외에는 크래쉬하고 있어 배포버전에서는 막습니다.
	if( i3::contain_string( lpCmdLine, "UT_") != 0) // UT_ <- 는 Unit 테스트에 사용되어지는 문장입니다.
	{
		//	실행 파라메터 처리
		ParseParameters( lpCmdLine);
	}
#endif
}

bool _PreGameExecution_(LPTSTR lpCmdLine)
{
	I3PRINTLOG(I3LOG_VIEWER, "_ParseParameters_ >>>");

#if defined( LOCALE_NORTHAMERICA) || defined( LOCALE_THAILAND) \
	|| defined(LOCALE_MIDDLE_EAST) || defined( LOCALE_KOREA) || defined(LOCALE_TAIWAN)
        HKEY h_key = HKEY();
        // HKEY_LOCAL_MACHINE 항목에 속한 "SOFTWARE\\BandiMPEG1" 항목을 열고
        // h_key에 그 핸들값을 저장한다. RegSetValueEx 함수를 사용하기 위해서는 KEY_SET_VALUE 권한을
        // 사용해서 열어야 한다.
        int ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\BandiMPEG1",
                                                    0, KEY_SET_VALUE, &h_key);
		if(ret != ERROR_SUCCESS){
			SHELLEXECUTEINFO shellInfo;
			shellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shellInfo.fMask = NULL;
			shellInfo.hwnd = NULL;
			shellInfo.lpVerb = NULL;		//	vista에서 실행시 권한 상승
			shellInfo.lpFile = _T("BDMPEG1SETUP.EXE");
			shellInfo.lpParameters = "/S";
			shellInfo.lpDirectory = NULL;
			shellInfo.nShow = SW_SHOW;
			shellInfo.hInstApp = NULL;
			::ShellExecuteEx( &shellInfo);
		}
        if(h_key != NULL) RegCloseKey(h_key);
#endif
		

#if defined (NC_BUILD) 
	#if defined (LOCALE_INDONESIA) || defined (LOCALE_BRAZIL) || defined (LOCALE_TURKEY) \
		|| defined (LOCALE_NORTHAMERICA) || defined ( LOCALE_ITALY ) || defined (LOCALE_PHILIPPINES)|| defined( LOCALE_MIDDLE_EAST)
		//	라이브 경우 인도네시아는 업데이터 실행이 우선이다.
		//  업데이터에서 보내주는 인자로 실행을 확인한다.
		//  인자가 0이면 런쳐를 통해 실행한것이 아니다.

		if (i3::generic_string_size(lpCmdLine) <= 0)
		{
			SHELLEXECUTEINFO shellInfo;
			shellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shellInfo.fMask = NULL;
			shellInfo.hwnd = NULL;
			shellInfo.lpVerb = NULL;		//	vista에서 실행시 권한 상승

			shellInfo.lpFile = _T("PBLauncher.exe");

			shellInfo.lpParameters = NULL;
			shellInfo.lpDirectory = NULL;
			shellInfo.nShow = SW_SHOW;
			shellInfo.hInstApp = NULL;

			//	PBLauncher를 무조건 실행시킨다.
			::ShellExecuteEx( &shellInfo);

			HANDLE hProcess = ::GetCurrentProcess();

			DWORD       dwExitCode = 0;

			//	PBLauncher를 실행했으므로 게임 프로세스틑 종료한다.
			GetExitCodeProcess( hProcess, &dwExitCode);
			TerminateProcess( hProcess, dwExitCode);
			I3PRINTLOG(I3LOG_VIEWER, "_ParseParameters_ <<<");
			return true;
		}
	#elif defined( LOCALE_JAPAN)

		if (i3::generic_string_size(lpCmdLine) <= 0)
		{
			SHELLEXECUTEINFO shellInfo;
			shellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shellInfo.fMask = NULL;
			shellInfo.hwnd = NULL;
			shellInfo.lpVerb = NULL;		//	vista에서 실행시 권한 상승

			shellInfo.lpFile = _T("AraGameLauncher.exe");

			shellInfo.lpParameters = NULL;
			shellInfo.lpDirectory = NULL;
			shellInfo.nShow = SW_SHOW;
			shellInfo.hInstApp = NULL;

			// AraGameLauncher 실행시킨다.
			::ShellExecuteEx(&shellInfo);

			HANDLE hProcess = ::GetCurrentProcess();

			DWORD dwExitCode = 0;

			// AraGameLauncher를 실행했으므로 게임 프로세스틑 종료한다.
			GetExitCodeProcess(hProcess, &dwExitCode);
			TerminateProcess(hProcess, dwExitCode);
			I3PRINTLOG(I3LOG_VIEWER, "_ParseParameters_ <<<");
			return true;
		}
	#endif
#endif
		I3PRINTLOG(I3LOG_VIEWER, "_ParseParameters_ <<<");
	return false;
}

static void _OutOfMemoryHandler()
{
#ifdef USE_I3EXEC
	MessageBoxW(g_hwndMain, L"Out of Memory!", L"", MB_OK);
#else
	MessageBoxW(g_hwndMain, GAME_STRING("STR_POPUP_MESSAGE_WARNING"), L"" ,MB_OK);
#endif
}






void _Message_Fail_i3ExecFile_()
{
#ifdef I3_DEBUG
	NotifyBox( NULL, "i3Exec 파일을 로드 할 수 없습니다. i3Exec가 로딩할 때 필요한 엔진 dll 파일이 맞지 않을 수 있습니다..\n");
#else
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
	{
		NotifyBox( NULL, "i3Exec 파일을 로드 할 수 없습니다. 다시 설치해 주세요.\n");
	}
	else
	{
		NotifyBox( NULL, "Could not execute the game. Please install it again.\n");
	}
#endif
}

void _Message_Fail_Interface_()
{
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
	{
		MessageBox( NULL, "인터페이스의 초기화에 실패했습니다.(_pInterface)\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( NULL, "Could not initialize the program..(ViewerWindow)\r\nTermination.", "Error", MB_OK);
	}
}

void _Message_Fail_ViewerWindow_()
{
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
	{
		MessageBox( NULL, "프로그램의 초기화에 실패했습니다.(ViewerWindow)\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( NULL, "Could not initialize GameGuard module..\r\nTermination.", "Error", MB_OK);
	}
}

void _Message_Fail_PreFramework_()
{
#if defined (LOCALE_INDONESIA)
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)
	{
		MessageBox( NULL, "핵쉴드 초기화에 실패했습니다.\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( NULL, "Could not initialize HackShield module..\r\nTermination.", "Error", MB_OK);
	}
#elif defined (LOCALE_KOREA)
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)
	{
		MessageBox( NULL, "게임가드 초기화에 실패했습니다.\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( NULL, "Could not initialize GameGuard module..\r\nTermination.", "Error", MB_OK);
	}
#elif defined (LOCALE_RUSSIA)
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)
	{
		MessageBox( NULL, "프로스트 초기화에 실패했습니다.\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( NULL, "Could not initialize Frost module..\r\nTermination.", "Error", MB_OK);
	}
#else
	MessageBox( NULL, "Could not initialize Protection Solution..\r\nTermination.", "Error", MB_OK);
#endif
}

void _Message_Fail_GraphicCard_()
{
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
	{
		MessageBox( g_hwndMain, "그래픽 카드 초기화에 실패했습니다.\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( g_hwndMain, "Could not initialize graphic interface card..\r\nTermination.", "Error", MB_OK);
	}
}

void _Message_Fail_Create_Framework_()
{
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
	{
		MessageBox( g_hwndMain, "프레임의 생성에 실패했습니다.(_pFramework)\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( g_hwndMain, "Could not create the framework.(_pFramework)\r\nTermination.", "Error", MB_OK);
	}
}

void _Message_Fail_Initialize_Framework_()
{
	if( i3Language::GetCurrentCodePage() == I3_LANG_CODE_KOREAN)		//	한국어
	{
		MessageBox( g_hwndMain, "프레임의 초기화에 실패했습니다.(_pInterface)\r\n종료합니다.", "Error", MB_OK);
	}
	else
	{
		MessageBox( g_hwndMain, "Could not initialize the framework..(_pInterface)\r\nTermination.", "Error", MB_OK);
	}
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE :
		break;

	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
		// revision 56294 ]F10키를 누를 경우, 캐릭터의 이동이 프리즈되며 일정시간후 핵오탐발생
	case WM_SYSKEYDOWN:
		{
			if( wParam == VK_F10)
			{
				return TRUE;
			}
		}
		break;

#if defined( I3_DEBUG)
	case WM_KEYDOWN :
		if( wParam == VK_F9)
		{
			if( GetKeyState( VK_LCONTROL) & 0x80)
			{
				ToggleDebugPanel();
			}
		}
		break;
#endif

	case WM_SIZE :
		{
			bool bWindowMode;

			if( _pViewer != NULL)
				bWindowMode = ! _pViewer->isFullScreen();
			else
				bWindowMode = ! g_VideoInfo.m_bFullScreen;

			InstallKeyhook( GetCurrentThreadId(), hWnd, bWindowMode);
		}
		break;

	case WM_ENTERMENULOOP :
		i3Viewer::SetInputAccessRight( false);
		break;

	case WM_EXITMENULOOP:
		i3Viewer::SetInputAccessRight( true);
		break;

	case WM_ACTIVATE :
		{
			bool bWindowMode;

			if( _pViewer != NULL)
				bWindowMode = ! _pViewer->isFullScreen();
			else
				bWindowMode = ! g_VideoInfo.m_bFullScreen;

			//if( bWindowMode)
			{
				if( wParam == WA_INACTIVE)
				{

					/*
					if( g_bCapture == true)
					{
					ReleaseCapture();
					g_bCapture = false;
					}
					*/
					i3Viewer::SetInputAccessRight( false);
				}
				else
				{
					/*
					if( g_bCapture == false)
					{
					SetCapture( hWnd);
					g_bCapture = true;
					}
					*/
					// ACTiVE 위치
					ResetWatchKey();
					i3Viewer::SetInputAccessRight( true);
				}
			}
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

#endif I3_WINDOWS


void AddRegestryGameFile()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx((OSVERSIONINFO *)&osvi);

	//window8이상에서 레지스트리에 실행파일이 등록되어있지 않으면
	//처음실행시 게임화면이 블랙아웃으로 출력됨
	if (osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion > 1)
	{
		LONG error = 0;
		HKEY hKey = NULL;
		DWORD dwDisp, dwData;
		error = RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Direct3D\\Shims\\MaximizedWindowedMode", 0, KEY_ALL_ACCESS, &hKey);
		if (error != ERROR_SUCCESS)
		{
			RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Direct3D\\Shims\\MaximizedWindowedMode", 0, "REG_BINARY",
				REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &dwDisp);

			I3PRINTLOG(I3LOG_NOTICE, "regestry MaximizedWindowedMode forder create");
		}


		char szDir[MAX_PATH];
		::GetCurrentDirectory(sizeof(szDir), szDir);

		char szPath[MAX_PATH]; 
		char szQAPath[MAX_PATH];
#if defined( LOCALE_NORTHAMERICA)
		sprintf(szPath, "%s\\PiercingBlow.exe", szDir);
		sprintf(szQAPath, "%s\\PiercingBlowQA.exe", szDir);
#else
		sprintf(szPath, "%s\\PointBlank.exe", szDir);
		sprintf(szQAPath, "%s\\PointBlankQA.exe", szDir);
#endif

		DWORD dwType = REG_DWORD;
		DWORD dwSize = 255;
		error = RegQueryValueEx(hKey, szPath, NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if (error != ERROR_SUCCESS)
		{
			dwData = 0x1;
			error = RegSetValueEx(hKey, szPath, 0, REG_DWORD, (const BYTE*)&dwData, 4);
			if (error == ERROR_SUCCESS)
			{
#if defined( LOCALE_NORTHAMERICA)
				I3PRINTLOG(I3LOG_NOTICE, "MaximizedWindowedMode PiercingBlow.exe Add");
#else
				I3PRINTLOG(I3LOG_NOTICE, "MaximizedWindowedMode PointBlank.exe Add");
#endif

			}
		}

		dwType = REG_DWORD;
		dwSize = 255;
		error = RegQueryValueEx(hKey, szQAPath, NULL, &dwType, (LPBYTE)&dwData, &dwSize);
		if (error != ERROR_SUCCESS)
		{
			dwData = 0x1;
			error = RegSetValueEx(hKey, szQAPath, 0, REG_DWORD, (const BYTE*)&dwData, 4);
			if (error == ERROR_SUCCESS)
			{
#if defined( LOCALE_NORTHAMERICA)
				I3PRINTLOG(I3LOG_NOTICE, "MaximizedWindowedMode PiercingBlowQA.exe Add");
#else
				I3PRINTLOG(I3LOG_NOTICE, "MaximizedWindowedMode PointBlankQA.exe Add");
#endif

			}
		}

		if (hKey != NULL)
			RegCloseKey(hKey);
	}
}