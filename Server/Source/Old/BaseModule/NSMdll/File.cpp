#include "stdafx.h"
#include "File.h"

NSM_cFile::NSM_cFile()
{
	m_hFile			= NULL;
}

NSM_cFile::~NSM_cFile()
{
	Close();
}

BOOL	NSM_cFile::CreateFile( const char* strFileName )
{
	DWORD dwDesiredAccess	= GENERIC_READ;
	DWORD dwShareMode		= FILE_SHARE_WRITE;
	
	m_hFile = ::CreateFile( strFileName, dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if( m_hFile == INVALID_HANDLE_VALUE )		return FALSE;

	return TRUE;
}

BOOL	NSM_cFile::Read( char* pBuffer, UINT32 ui32Size )
{
	DWORD dwReturnSize;

	if( FALSE == ::ReadFile( m_hFile, pBuffer, ui32Size, &dwReturnSize, NULL ) ) return FALSE;

	// 읽을 사이즈와 읽은 후의 사이즈가 다를때 에러를 처리할 것인가..
	//if( ui32Size == ui32ReturnSize )	return FALSE;

	return TRUE;
}

void	NSM_cFile::Close()
{
	if( m_hFile )
	{
		CloseHandle( m_hFile );
		m_hFile = NULL;
	}
}

DWORD	NSM_cFile::GetFileSize()
{
	if( NULL == m_hFile )	return 0;

	DWORD dwFileSizeHigh;
	DWORD dwFileSizeLow = ::GetFileSize( m_hFile, &dwFileSizeHigh );

	return (dwFileSizeHigh << 16 ) | dwFileSizeLow;
}