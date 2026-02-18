#include "stdafx.h"
#include "GlobalVar.h"

COptionInfo *		g_pOption = NULL;
i3PackNode *		g_pPack = NULL;
CImageList *		g_pSysImageList = NULL;
char				g_szVolumeName[MAX_PATH] = {0,};
char				g_szPath[MAX_PATH] = {0,};

bool				g_bVerbose = true;

static char tempstr[4096];

void PRINTF( const char *format, ...)
{
	va_list marker;

	va_start( marker, format);

	vsprintf( tempstr, format, marker);

	OutputDebugString( tempstr);

	if( g_bVerbose)
	{
		HANDLE hOut = GetStdHandle( STD_OUTPUT_HANDLE);
		DWORD writelen;

		WriteConsole( hOut, tempstr, strlen( tempstr), &writelen, 0);
		//printf( tempstr);
		
	}
}