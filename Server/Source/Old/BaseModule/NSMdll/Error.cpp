#include "stdafx.h"
#include "Error.h"
#include "String.h"
#include <stdio.h>

static NSM_cExceptionHandler	s_pExpectionOuter;
LONG WINAPI CheckUnhandledException( PEXCEPTION_POINTERS pExceptionInfo );

EXPORT_BASE void NSM_nsError::SetExceptionHandler( char* pszDmpFileName )
{
	s_pExpectionOuter.SetPath( pszDmpFileName );

	SetUnhandledExceptionFilter( CheckUnhandledException );
}

EXPORT_BASE void NSM_nsError::SetExceptionHandlerOption( MINIDUMP_TYPE eOption )
{
	s_pExpectionOuter.SetExceptionHandlerOption( eOption );
}

LONG WINAPI CheckUnhandledException( PEXCEPTION_POINTERS pExceptionInfo )
{
	SetErrorMode( SEM_NOGPFAULTERRORBOX );
	s_pExpectionOuter.SetCreateMiniDump( pExceptionInfo );
	return 0;
}

NSM_cExceptionHandler::NSM_cExceptionHandler()
{
	m_eDumpType = MiniDumpNormal;
	strcpy( m_szDmpFileName, "i3BugReport.i3b" );
}

NSM_cExceptionHandler::~NSM_cExceptionHandler()
{
}

void	NSM_cExceptionHandler::SetPath( char* pszDmpFileName )
{
	if( NULL == pszDmpFileName ) return;

	strcpy( m_szDmpFileName, pszDmpFileName );
}

void	NSM_cExceptionHandler::SetExceptionHandlerOption( MINIDUMP_TYPE eOption )
{
	m_eDumpType = eOption;
}

struct MinidumpInfo
{
	MINIDUMP_EXCEPTION_INFORMATION	m_ExceptionInfo;
	char							m_strFileName[512];
	MINIDUMP_TYPE					m_eDumpType;
	HANDLE							m_hPro;
	DWORD							m_dwProcessID;
};

UINT32 _DefDumpThreadProc( MinidumpInfo* pstDumpInfo )
{

	HANDLE hFile = CreateFile( pstDumpInfo->m_strFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MiniDumpWriteDump( pstDumpInfo->m_hPro, pstDumpInfo->m_dwProcessID, hFile, pstDumpInfo->m_eDumpType, &pstDumpInfo->m_ExceptionInfo, NULL, NULL);

	CloseHandle( hFile );

	return 1;
}

void	NSM_cExceptionHandler::SetCreateMiniDump( PEXCEPTION_POINTERS pExceptionInfo )
{
	// miniDump ½ÇÇà
	MinidumpInfo stDumpInfo;

	stDumpInfo.m_hPro = GetCurrentProcess();
	stDumpInfo.m_dwProcessID = GetCurrentProcessId();
	stDumpInfo.m_ExceptionInfo.ThreadId = GetCurrentThreadId();
	stDumpInfo.m_ExceptionInfo.ExceptionPointers = pExceptionInfo;
	stDumpInfo.m_ExceptionInfo.ClientPointers = FALSE;
	stDumpInfo.m_eDumpType = (MINIDUMP_TYPE)m_eDumpType;
	
	SYSTEMTIME st; 
    GetLocalTime(&st); 

	sprintf( stDumpInfo.m_strFileName, "%s_%04d%02d%02d%02d%02d", m_szDmpFileName, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );

	HANDLE hThread = CreateThread(  NULL, 4096,	(LPTHREAD_START_ROUTINE)_DefDumpThreadProc,	&stDumpInfo, NULL, NULL);

	WaitForSingleObject( hThread, INFINITE );
	
	CloseHandle( hThread );
} 