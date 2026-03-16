#include "i3CommonType.h"
#include "stdio.h"
#include "i3Debug.h"
#include "i3Macro.h"
#include "i3INet.h"
#include "i3String.h"

I3_EXPORT_BASE void *g_pi3DebugPointer = NULL;
INT32 g_ni3DebugIndex = -1;

#if defined(I3_PS2)
#include <sys/errno.h>
#include <libinsck.h>
#endif

#if defined( I3_PSP) || defined( I3_WIPI)		
#include <stdarg.h>	
#endif

#define I3_DEBUG_NET
#define	JCDEBUG_VLOGGER_PORT		5555

I3_EXPORT_BASE REAL32 g_rAccTime = 0.f;

static char tempstr[2048];
static char tempstrIncludeTime[2048];

#if !defined( I3_WIPI)
static char addstr[256];
static char tempstr2[256];
#endif

#if defined( I3_DEBUG)
char DebugServerIP[20] = "127.0.0.1";
#endif

#if defined( I3_WIPI)
char g_szi3TraceFileName[32] = "i3trace.txt";
#endif

#if defined( I3_WINDOWS)
	void I3PRINTF( const char *format, ...)
	{
		int kk = 0;
		kk;

		va_list marker;

		va_start( marker, format);

		vsprintf( tempstr, format, marker);

		va_end( marker);

		//_snprintf(tempstrIncludeTime, 2048, "%f %s", g_rAccTime, tempstr);

		OutputDebugString( tempstr);
	}

	void I3PRINTF( const WCHAR16 *format, ...)
	{
		va_list marker;

		va_start( marker, format);

		vswprintf( (WCHAR16*)tempstr, format, marker);

		va_end( marker);

		//_snprintf(tempstrIncludeTime, 2048, "%f %s", g_rAccTime, tempstr);

		OutputDebugStringW( (WCHAR16*)tempstr);
	}

#elif defined( I3_WINCE )
	void I3PRINTF( const char *format, ...)
	{
		va_list marker;

		va_start( marker, format);

		vsprintf( tempstr, format, marker);
		
		TCHAR TszTemp[4096] = {0, };

		MultiByteToWideChar( CP_ACP, 0, tempstr, -1, TszTemp, 4096 );
		
		OutputDebugString( TszTemp );
	}
#elif defined( I3_PSP)
	void I3PRINTF( const char * format, ...)
	{
		va_list marker;

		va_start( marker, format);

		vsprintf( tempstr, format, marker);

		fprintf( stdout, tempstr);
	}
#elif defined( I3_WIPI)	
	void I3PRINTF( const char * format, ...)
	{
		M_Int32 file;
		
		static bool s_bFirst = true;
		if( s_bFirst )
		{
			s_bFirst = false;
			file = MC_fsOpen((PSTR) g_szi3TraceFileName, MC_FILE_OPEN_WRTRUNC, MC_DIR_PRIVATE_ACCESS);
		}
		else
		{
			file = MC_fsOpen((PSTR) g_szi3TraceFileName, MC_FILE_OPEN_WRONLY, MC_DIR_PRIVATE_ACCESS);
		}

		I3ASSERT( file > M_E_ERROR);

		MC_fsSeek(file, 0, MC_FILE_SEEK_END);
	#ifdef I3_WIPI_SKT
		MC_fsWrite(file, (M_Byte *)format,  strlen(format));
	#else
		MC_fsWrite(file, (char *)format,  strlen(format));
	#endif
		MC_fsClose(file);

		MC_knlPrintk((PSTR)format);
	}
#endif

#if defined(I3_WINDOWS)
int DebugBox( HWND Owner, const char * lpszText, ...)
{
	va_list marker;

	va_start( marker, lpszText);

	vsprintf( tempstr2, lpszText, marker);

	if( Owner == NULL)
		Owner = GetDesktopWindow();

	int ret;
#if 1
	LPCTSTR lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	sprintf( addstr, "%s\r\n\r\nError code : %s", tempstr2, lpMsgBuf );

	ret = MessageBox( Owner, addstr, "GetLastError", MB_YESNO|MB_ICONINFORMATION );

	LocalFree( (LPVOID)lpMsgBuf );
#else
	sprintf( addstr, "%s\r\n\r\nI3TRACE => %s\r\n\r\nBreak Now?", tempstr2, tempstr );

	ret = MessageBox( Owner, addstr, "GetLastError", MB_YESNO|MB_ICONINFORMATION );
#endif
 
	return ( ret == IDYES ) ? 1 : 0;
}

int NotifyBox( HWND Owner, const char * lpszText, ...)
{
	va_list marker;

	va_start( marker, lpszText);

	vsprintf( tempstr2, lpszText, marker);

	if( Owner == NULL)
		Owner = GetDesktopWindow();

	int ret;

	sprintf( addstr, "%s", tempstr2);

	ret = MessageBox( Owner, addstr, "알림", MB_OK | MB_ICONINFORMATION );

	return ret;
}
#elif defined( I3_WINCE )
int DebugBox( HWND Owner, const char * lpszText, ...)
{
	va_list marker;

	va_start( marker, lpszText);

	vsprintf( tempstr2, lpszText, marker);

	if( Owner == NULL)
		Owner = GetDesktopWindow();

	int ret;
#if 1
	LPCTSTR lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	sprintf( addstr, "%s\r\n\r\nError code : %s", tempstr2, lpMsgBuf );

	TCHAR TszTemp[256] = {0, };

	MultiByteToWideChar( CP_ACP, 0, addstr, -1, TszTemp, 256 );

	ret = MessageBox( Owner, TszTemp, TEXT("GetLastError"), MB_YESNO|MB_ICONINFORMATION );

	LocalFree( (LPVOID)lpMsgBuf );
#else
	sprintf( addstr, "%s\r\n\r\nI3TRACE => %s\r\n\r\nBreak Now?", tempstr2, tempstr );

	TCHAR TszTemp[256] = {0, };

	MultiByteToWideChar( CP_ACP, 0, addstr, -1, TszTemp, 256 );

	ret = MessageBox( Owner, TszTemp, TEXT("GetLastError"), MB_YESNO|MB_ICONINFORMATION );
#endif
 
	return ( ret == IDYES ) ? 1 : 0;
}

int NotifyBox( HWND Owner, const char * lpszText, ...)
{
	va_list marker;

	va_start( marker, lpszText);

	vsprintf( tempstr2, lpszText, marker);

	if( Owner == NULL)
		Owner = GetDesktopWindow();

	int ret;

	sprintf( addstr, "%s", tempstr2);

	ret = MessageBox( Owner, LPCTSTR(addstr), TEXT("알림"), MB_OK | MB_ICONINFORMATION );

	return ret;
}
#elif defined( I3_WIPI)
int DebugBox( void *pNull, const char * lpszText, ...)
{
	I3TRACE( lpszText);
	return 1;
}

int NotifyBox( void *pNull, const char * lpszText, ...)
{
	I3TRACE( lpszText);
	return 1;
}
#else
int DebugBox( void *pNull, const char * lpszText, ...)
{
	va_list marker;

	va_start( marker, lpszText);

	vsprintf( tempstr, lpszText, marker);

	//sprintf( addstr, "\r\n\tERROR : %s\r\n\tMESSAGE : %s", GetErrorMessage( errno), tempstr);

	fprintf( stdout, addstr);

	return 1;
}
#endif //I3_WINDOWS

static i3LogCallBackFunc s_i3LogCB = NULL;
void i3SetLogCallBackFunc( i3LogCallBackFunc pFunc )
{
	s_i3LogCB = pFunc;
}

#if defined( I3_WINDOWS) || defined( I3_PSP) || defined( I3_WINCE )
void i3Log( const char * pszModule, const char *pszFormat, ...)
{
	va_list marker;

	va_start( marker, pszFormat);

	vsprintf( tempstr2, pszFormat, marker);

	sprintf( tempstr, "[%s] : %s", pszModule, tempstr2);

	strcat( tempstr, "\n");
	
	#if defined( I3_WINDOWS)
		OutputDebugString( tempstr);
		if( s_i3LogCB != NULL )
		{
			s_i3LogCB( tempstr );
		}
	#elif defined( I3_WINCE )
		TCHAR TszTemp[256] = {0, };

		MultiByteToWideChar( CP_ACP, 0, tempstr, -1, TszTemp, 256 );

		OutputDebugString( TszTemp );

		if( s_i3LogCB != NULL )
		{
			s_i3LogCB( tempstr );
		}

	#elif defined( I3_PSP) || defined( I3_WIPI)	
		//fprintf( stdout, tempstr);
		printf( tempstr );
	#elif defined( I3_PS2)
		printf( tempstr);
	#endif
}

#elif defined( I3_WIPI)
void i3Log( const char * pszModule, const char *pszFormat, ...)
{
	sprintf( (PSTR) tempstr, (PSTR) "%s : %s\n", pszModule, pszFormat);
	I3TRACE( tempstr);
}
#endif

#if defined( I3_WINDOWS) && defined( I3_DEBUG)
HANDLE s_hVLog = INVALID_HANDLE_VALUE;
i3Udp *		s_pLogSocket = NULL;

void i3VLogStart( const char * pszPath)
{
	if( s_hVLog != INVALID_HANDLE_VALUE)
	{
		CloseHandle( INVALID_HANDLE_VALUE);
	}

	if( pszPath != NULL)
	{
		s_hVLog = CreateFile( pszPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	s_pLogSocket = i3Udp::NewObject();

	s_pLogSocket->CreateSocket();
}

void i3VLogEnd(void)
{
	if( s_hVLog != INVALID_HANDLE_VALUE)
	{
		CloseHandle( s_hVLog);
		s_hVLog = INVALID_HANDLE_VALUE;
	}

	if( s_pLogSocket != NULL)
	{
		s_pLogSocket->DeleteSocket();

		s_pLogSocket->Release();
		s_pLogSocket = NULL;
	}
}

void i3VLog( char *pszFormat, ...)
{
	va_list marker;

	va_start( marker, pszFormat);

	vsprintf( tempstr, pszFormat, marker);

	if( s_hVLog != INVALID_HANDLE_VALUE)
	{
		DWORD writebytes;

		WriteFile( s_hVLog, tempstr, (INT32)strlen( tempstr), &writebytes, NULL);
	}

	if( s_pLogSocket != NULL)
	{
		i3DEBUGMSG		Msg;

		memset( &Msg, 0, sizeof( i3DEBUGMSG));
		strncpy( Msg.m_szModule, "_W", sizeof(Msg.m_szModule) - 1);
		strncpy( Msg.m_szMsg, tempstr, sizeof(Msg.m_szMsg) - 1);

		s_pLogSocket->Send( (const char *) &Msg, sizeof(Msg), inet_addr( DebugServerIP), JCDEBUG_VLOGGER_PORT);
	}
	else
	{
		I3TRACE( "Before use i3VLog() function, i3VLogStart() function must be called.\n");
	}
}
#endif //I3_DEBUG

#if defined( I3_DEBUG)
void	I3DUMP( char * pBuf, INT32 Len, INT32 Width)
{
	INT32 i, w = 0;

	I3TRACE( "%08X : ", pBuf);

	for( i = 0; i < Len; i++)
	{
		I3TRACE( "%02X ", *pBuf & 0xFF);

		w ++;
		pBuf++;

		if( w >= Width)
		{
			I3TRACE( "\n%08X : ", pBuf);
			w = 0;
		}
	}
}

void	I3DUMP2( char * pBuf, INT32 Len, INT32 Width)
{
	INT32 i, w = 0;
	UINT16 * pTemp = (UINT16 *) pBuf;

	I3TRACE( "%08X : ", pTemp);

	for( i = 0; i < Len; i += 2)
	{
		I3TRACE( "%04X ", *pTemp & 0xFFFF);

		w ++;
		pTemp++;

		if( w >= Width)
		{
			I3TRACE( "\n%08X : ", pTemp);
			w = 0;
		}
	}
}

void	I3DUMP4( char * pBuf, INT32 Len, INT32 Width)
{
	INT32 i, w = 0;
	UINT32 * pTemp = (UINT32 *) pBuf;

	I3TRACE( "%08X : ", pTemp);

	for( i = 0; i < Len; i += 4)
	{
		I3TRACE( "%08X ", *pTemp);

		w ++;
		pTemp++;

		if( w >= Width)
		{
			I3TRACE( "\n%08X : ", pTemp);
			w = 0;
		}
	}
}
#endif
