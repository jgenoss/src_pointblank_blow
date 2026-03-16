#include "pch.h"
#include "S2MODatFile.h"

S2MODatFile::S2MODatFile()
{
	m_hFile				= INVALID_HANDLE_VALUE;
	m_strMD5Key[0]		= 0;
	m_strDatFileName[0] = 0;
}

S2MODatFile::~S2MODatFile()
{
	Destory();
}

BOOL	S2MODatFile::Create(char* strDatFileName)
{
	if( ::strlen( strDatFileName ) >= MAX_DAT_FILE_NAME )
	{
		return FALSE;
	}

	::strcpy_s( m_strDatFileName, MAX_DAT_FILE_NAME, strDatFileName );

	return TRUE;
}

void	S2MODatFile::Destory()
{
	
}

char*	S2MODatFile::GetMD5Key()
{	
	if( 0 == m_strMD5Key[0] )
	{
		_MakeMD5Key();
	}

	return m_strMD5Key; 
};

void	S2MODatFile::MakeMD5Key(void)
{
	// File 검색후 MD5Key를 생성합니다.
	if(FALSE == _FindDatFile() ) return;

	_MakeMD5Key();
	
	return;
}
 
void	S2MODatFile::_MakeMD5Key(void)
{
	// File로 MD5Key를 생성합니다.
	unsigned char md5Key[NET_MD5_HASH_SIZE] = {};
	UINT64 ui64Temp[2];
	
	S2MOMD5::S2MO_Getmd5_file(m_strDatFileName, md5Key);
	memcpy(ui64Temp, md5Key,  NET_MD5_HASH_SIZE);
	S2MOMD5::S2MO_GetMD5String(m_strMD5Key, NET_MD5_KEY_SIZE, ui64Temp);

	return;
}


T_RESULT S2MODatFile::SaveDatFile(void)
{
	// 기존에 파일이 있으면 지운다.  
	_DeleteDatFile();
	
	T_RESULT TResult = _MakeDatFile();
	if( !S2MO_IS_SUCCESS(TResult)  )
	{
		return TResult;
	}

	ProcessDatFile(FILE_CONTROL_WRITE);

	_MakeMD5Key();

	return TResult;
}

T_RESULT S2MODatFile::_MakeDatFile()
{
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}	
	
	m_hFile = ::CreateFileA(m_strDatFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return EVENT_ERROR_S2MODATFILE_CREATE_FILE; 
	}

	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

	return EVENT_ERROR_SUCCESS;
}


void S2MODatFile::_DeleteDatFile(void)
{
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}

	::DeleteFileA(m_strDatFileName);

	return;
}

void		S2MODatFile::ControlDatFile(FILE_CONTROL eFileControl, void* pData, INT32 i32DataSize, INT32* i32Length )
{
	if( m_hFile == INVALID_HANDLE_VALUE)	return ;

	switch( eFileControl )
	{
	case FILE_CONTROL_READ:
		::memset( pData,0, i32DataSize );
		::ReadFile(m_hFile, pData,	i32DataSize, (LPDWORD)i32Length, NULL);
		break;
	case FILE_CONTROL_WRITE:
		::WriteFile(m_hFile, pData,	i32DataSize, (LPDWORD)i32Length, NULL);
		break;
	}

	return;
}

 
	
T_RESULT		S2MODatFile::LoadDatFile(void)
{
	if( m_hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
	
	// Open File
	if(FALSE == _FindDatFile() ) return EVENT_ERROR_S2MODATFILE_FIND_FILE;

	m_hFile = ::CreateFileA(m_strDatFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( INVALID_HANDLE_VALUE == m_hFile )
	{			
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE; 
		return EVENT_ERROR_S2MODATFILE_CREATE_FILE; 
	}

	// Data Read
	ProcessDatFile(FILE_CONTROL_READ); 

	_MakeMD5Key();

	return EVENT_ERROR_SUCCESS;
}

BOOL			S2MODatFile::_FindDatFile(void)
{
	WIN32_FIND_DATAA	ffResult;
	HANDLE              hFind;
	BOOL                bContinue	= TRUE;
	BOOL				bRv			= FALSE;
 
	hFind = ::FindFirstFileA( m_strDatFileName, &ffResult);
       
	while( hFind !=INVALID_HANDLE_VALUE && bContinue)
	{    
		if( (ffResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			bRv = TRUE;
		}
              
		bContinue = ::FindNextFileA( hFind, &ffResult);
	}

	return bRv;
}