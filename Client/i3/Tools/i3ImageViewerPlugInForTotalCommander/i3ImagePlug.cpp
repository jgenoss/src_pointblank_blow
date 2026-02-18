// listplug.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "i3ImagePlug.h"

#include "BitmapImage.h"

//#define supportedextension1 ".c"
//#define supportedextension2 ".cpp"
//#define supportedextension3 ".h"
//#define supportedextension4 ".pas"

#define supportedextension1 ".i3i"

/* Note: in C, double quotes inside a string need to be escaped with a backslash!  */
//#define parsefunction "force | (ext=\"C\" | ext=\"CPP\") & FIND(\"{\") | (ext=\"H\") | (ext=\"PAS\" & FINDI(\"BEGIN\"))"
#define parsefunction "force | (ext=\"I3I\")"

HINSTANCE hinst;
char inifilename[MAX_PATH]="lsplugin.ini";  // Unused in this plugin, may be used to save data

CImage s_Image;
static i3Texture *s_pTexture = NULL;
INT32 s_Width = 0;
INT32 s_Height = 0;

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hinst=(HINSTANCE)hModule;
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

char* strlcpy(char* p, char*p2, int maxlen)
{
	if ((int)strlen(p2)>=maxlen) {
		strncpy(p, p2, maxlen);
		p[maxlen]=0;
	} else
		strcpy(p, p2);
	return p;
}

int lastloadtime=0;   // Workaround to RichEdit bug

int __stdcall ListNotificationReceived(HWND ListWin, int Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_COMMAND:
		//if (HIWORD(wParam)==EN_UPDATE && abs(int(GetCurrentTime()-lastloadtime))>1000) {
		//	int firstvisible=SendMessage(ListWin, EM_GETFIRSTVISIBLELINE, 0, 0);
		//	int linecount=SendMessage(ListWin, EM_GETLINECOUNT, 0, 0);
		//	if (linecount>0) {
		//		int percent=MulDiv(firstvisible, 100, linecount);
		//		PostMessage(GetParent(ListWin), WM_COMMAND, MAKELONG(percent, itm_percent), (LPARAM)ListWin);
		//	}
		//	return 0;
		//}
		break;
	case WM_NOTIFY:
		break;
	case WM_MEASUREITEM:
		break;
	case WM_DRAWITEM:
		break;
	}
	return 0;
}

void __stdcall ListCloseWindow(HWND hWnd )
{
	I3_SAFE_RELEASE( s_pTexture );
	s_Image.Destroy();
	DestroyWindow( hWnd );
	return;
}

HWND __stdcall ListLoad( HWND ParentWin, char* szFileName, int ShowFlags )
{
	HWND hWnd;
	RECT r;
	char *p;
	BOOL bSuccess = FALSE;

	if ( ( ShowFlags & lcp_forceshow) == 0 )
	{  // don't check extension in this case!
		p = strrchr(szFileName, '\\');
		if( p == NULL )
		{
			return NULL;
		}

		p = strrchr(p, '.');
		if( ( p == NULL ) || ( stricmp( p, supportedextension1 ) != 0 ) )
		{
			return NULL;
		}
	}

	i3MemoryInit();
	i3SceneRegisterMetas(); 

	lastloadtime = GetCurrentTime();

	GetClientRect( ParentWin, &r );

	// Create window invisible, only show when data fully loaded!
	hWnd = CreateWindow( "STATIC", "", WS_CHILD | WS_HSCROLL | WS_VSCROLL , 
		r.left, r.top, r.right-r.left, r.bottom-r.top, ParentWin, NULL, hinst, NULL);

	if( hWnd )
	{
		//PostMessage( ParentWin, WM_COMMAND, MAKELONG(lcp_ansi, itm_fontstyle), (LPARAM)hwnd);
		SetWindowText( hWnd, "This is i3i Plug-In for TotalCommander. ("__TIME__", "__DATE__")" );

		i3TextureMem *pTexMem = i3TextureMem::NewObject();
		pTexMem->SetRuntimePlatform( I3_PLATFORM_WINDOWS );
		s_pTexture = pTexMem;

		i3ImageFile ImageFile;
		if( ImageFile.Load( szFileName, s_pTexture ) )
		{
			s_Width		= s_pTexture->GetWidth();
			s_Height	= s_pTexture->GetHeight();

			s_Image.Create( s_Width, s_Height, 32 );

			bSuccess = TRUE;
		}

		if( !bSuccess )
		{
			DestroyWindow( hWnd );
			hWnd = NULL;
		}
	}
	
	lastloadtime=GetCurrentTime();
	
	if( hWnd )
	{
		ShowWindow( hWnd, SW_SHOW );
	}

	return hWnd;
}

int __stdcall ListSendCommand(HWND ListWin, int Command, int Parameter)
{
	switch (Command) {
	case lc_copy:
		//SendMessage(ListWin, WM_COPY, 0, 0);
		//return LISTPLUGIN_OK;
		return LISTPLUGIN_ERROR;

	case lc_newparams:
		return LISTPLUGIN_ERROR;

	case lc_selectall:
		//SendMessage(ListWin, EM_SETSEL, 0, -1);
		//return LISTPLUGIN_OK;
		return LISTPLUGIN_ERROR;

	case lc_setpercent:
		return LISTPLUGIN_ERROR;
	}
	return LISTPLUGIN_ERROR;
}

int _stdcall ListSearchText(HWND ListWin, char* SearchString, int SearchParameter)
{
	//return LISTPLUGIN_OK;
	return LISTPLUGIN_ERROR;
}

int __stdcall ListPrint(HWND ListWin, char* FileToPrint, char* DefPrinter, int PrintFlags, RECT* Margins)
{
	return 0;
}

void __stdcall ListGetDetectString(char* DetectString, int maxlen)
{
	strlcpy(DetectString, parsefunction, maxlen);
}

void __stdcall ListSetDefaultParams(ListDefaultParamStruct* dps)
{
	strlcpy(inifilename, dps->DefaultIniName, MAX_PATH-1);
}
