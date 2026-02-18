#include "i3NetworkDef.h"
#include "i3NetworkFTP.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3NetworkFTP);

bool i3NetworkFTP::Create( char * address, INT32 nPort, char * id, char * pass, UINT32 nFlag  )
{	
	setAddress( address );		// FTP address

	if( nPort == 0 )
		setPort( (UINT16) I3_FTP_DEFAULT_PORT );
	else
		setPort( (UINT16) nPort );			// port

	// 접속 아이디
	if( id == nullptr )
		setUserID( I3_FTP_DEFAULT_ID );
	else
		setUserID( id );

	// 패스워드
	if( pass == nullptr )
		setUserPass( I3_FTP_DEFAULT_PASS );
	else
		setUserPass( pass );

	// 플래그 세팅
	setFlags( nFlag );

	// 현재 로컬 폴더
	::GetCurrentDirectory( MAX_PATH, m_szLocalDirectory );

	m_nLocalDirLen	= i3::generic_string_size( m_szLocalDirectory );
	
	if( getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN )
	{
		if( !Connect() )
			return false;
	}

	// FTP Server directory
	if( getFlags() & I3_FTP_FLAG_CHECKREMOTEDIR )
	{
		if( !(getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN) )
		{
			if( !Connect() )
				return false;
		}

		{
			DWORD	dwCur = MAX_PATH;

			::FtpGetCurrentDirectory( m_hFtp, m_szRemoteDirectory, &dwCur );
			
			ConvertInternetDir( m_szRemoteDirectory, i3::generic_string_size( m_szRemoteDirectory) );

			if( !( getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN ) )
				DisConnect();
		}
	}

	return true;
}

bool i3NetworkFTP::Connect()
{
	// INTERNET_FLAG_PASSIVE mode
	// 접속 권한을 가져 오기 위해 패시브 모드로 접속한 폴더의 하위 폴더까지 권한을 가져옵니다.
	m_hInternet = ::InternetOpen( "I3FTP", INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, INTERNET_FLAG_PASSIVE );

	if( m_hInternet == nullptr )
		return false;

	m_hFtp = ::InternetConnect( m_hInternet, getAddress(), (INTERNET_PORT) m_nPort,
		getUserID(), getUserPass(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0 );

	if( m_hFtp == nullptr )
	{
		::InternetCloseHandle( m_hInternet );
		return false;
	}

	return true;
}

bool i3NetworkFTP::DisConnect()
{
	::InternetCloseHandle( m_hFtp );
	::InternetCloseHandle( m_hInternet );
	m_hFtp			= nullptr;
	m_hInternet		= nullptr;

	return true;
}

bool i3NetworkFTP::DownLoadFile( char * pszPath )
{
	char	szRemote[ MAX_PATH ];
	char	szExt[ _MAX_PATH ];
	
	_splitpath( pszPath, nullptr, nullptr, szRemote, szExt );
	strncat( szRemote, szExt, sizeof(szRemote)-1 );

	return DownLoadFile( pszPath, szRemote );
}

bool i3NetworkFTP::DownLoadFile( char * pszLocalPath, char * pszRemotePath )
{
	char	szPath[ MAX_PATH ];
	char	szFileName[ MAX_PATH ];
	char	szExt[ _MAX_PATH ];
	
	char	szFilePath[ MAX_PATH ];
	INT32	nLocalLen, nFileLen;
	
	_splitpath( pszLocalPath, nullptr, szPath, szFileName, szExt );
	strncat( szFileName, szExt, sizeof(szFileName)-1 );

	nLocalLen = i3::generic_string_size( pszLocalPath );
	nFileLen = i3::generic_string_size( szFileName );

	nLocalLen -= nFileLen;

	strncpy( szFilePath, m_szLocalDirectory, sizeof( szFilePath)-1 );
	strncat( szFilePath, "\\", sizeof( szFilePath)-1 );

	if( nLocalLen > 0 )
	{
		strncat( szFilePath, szPath, sizeof( szFilePath)-1 );

		CheckDirectory( szFilePath );
	}

	// 받을 디렉토리로 세팅
	::SetCurrentDirectory( szFilePath );

	sprintf( m_szStr, "Download : %s\n ", pszLocalPath );

	BOOL bResult = ::FtpGetFile( m_hFtp, pszRemotePath, szFileName, FALSE, FILE_ATTRIBUTE_NORMAL,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 0 );

	// 루트 디렉토리로 돌려놓는다.
	::SetCurrentDirectory( m_szLocalDirectory );

	return bResult ? true : false;
}

bool i3NetworkFTP::DownLoadFile2( char * pszPath )
{
	char	szNewFileName[ MAX_PATH ];

	char	buf[1024];
	char	szTemp[4096];

	DWORD	dwRead;
	i3FileStream	file;
	HINTERNET	hRemote;

	I3ASSERT( pszPath );
	I3ASSERT( m_hFtp );

	// open server file
	hRemote = ::FtpOpenFile( m_hFtp, pszPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0 );
	if( hRemote == nullptr )
	{
		I3TRACE( "i3NetworkFTP::DownLoadFile2() - can't ftp open file.\n" );
		return false;
	}

	// create local file
	::GetCurrentDirectory( MAX_PATH, szNewFileName );
	strncat( szNewFileName, "\\", sizeof( szNewFileName)-1 );
	strncat( szNewFileName, pszPath, sizeof( szNewFileName)-1 );
	
	if( false == file.Create( szNewFileName, STREAM_WRITE ) )
	{
		::InternetCloseHandle( hRemote );
		I3TRACE( "i3NetworkFTP::DownLoadFile2() - can't open file.\n" );
		return false;
	}

	m_dwProg = 0;

	for( ;; )
	{
		BOOL bResult = ::InternetReadFile( hRemote, buf, 1024, &dwRead );
		if( bResult == TRUE && dwRead == 0 )
			break;

		file.Printf( szTemp, buf );
		m_dwProg += dwRead;

		wsprintf( m_szStr, "%s %d", pszPath, m_dwProg);
	}

	file.Close();

	::InternetCloseHandle( hRemote );
	return true;
}

bool i3NetworkFTP::UploadFile( const char * pszPath )
{
	char	szExt[ _MAX_PATH ];
	char	szRemote[ MAX_PATH ];

	_splitpath( pszPath, nullptr, nullptr, szRemote, szExt );

	strncat( szRemote, szExt, sizeof(szRemote)-1 );
	return UploadFile( pszPath, szRemote );
}

bool i3NetworkFTP::UploadFile( const char * pszLocalPath, const char * pszRemotePath )
{
	bool	bResult;
	char	szRemoteAllPath[ MAX_PATH ];
	char	szRemotePath[ MAX_PATH ];
	char	szName[ MAX_PATH ];
	char	szExt[ _MAX_PATH ];

	INT32	nLocalLen, nRemoteLen;

	I3ASSERT( m_hFtp );

	_splitpath( pszRemotePath, nullptr, szRemotePath, szName, szExt );
	strncat( szName, szExt, sizeof(szName)-1 );

	nLocalLen = i3::generic_string_size( pszRemotePath );
	nRemoteLen = i3::generic_string_size( szName );

	nLocalLen -= nRemoteLen;

	// for all format directory name
	ConvertInternetDir( szRemotePath, i3::generic_string_size( szRemotePath ) );

	memset( szRemoteAllPath, 0, sizeof( szRemoteAllPath ) );
	strncpy( szRemoteAllPath, m_szRemoteDirectory + 1, sizeof( szRemoteAllPath)-1 );
	strncat( szRemoteAllPath, szRemotePath, sizeof( szRemoteAllPath)-1 );

	if( nLocalLen > 0 )
	{	
		if( !CheckRemoteDir( m_hFtp, szRemoteAllPath ) )
		{
			// 리모트 디렉토리 설정( 이 디렉토리에 저장을 한다. )
			if( !::FtpSetCurrentDirectory( m_hFtp, szRemoteAllPath ) )
			{
				I3PRINTLOG(I3LOG_FATAL,  "[i3NetworkFTP::UplodaFile] - error set current directory fail." );
				return false;
			}
		}
	}
	// 디렉토리에 쓰기 설정이 되어 있는지 확인..
	bResult = UploadFile2( pszLocalPath, pszRemotePath );
	
	// 리모트 루트리렉토리로 돌린다.
	::FtpSetCurrentDirectory( m_hFtp, m_szRemoteDirectory );

	return bResult;
}

bool i3NetworkFTP::UploadFile2( const char * pszPath )
{
	char	szExt[ _MAX_PATH ];
	char	szRemote[ MAX_PATH ];

	_splitpath( pszPath, nullptr, nullptr, szRemote, szExt );
	strncat( szRemote, szExt, sizeof( szRemote)-1 );

	return UploadFile2( pszPath, szRemote );
}

bool i3NetworkFTP::UploadFile2( const char * pszLocalPath, const char * pszRemotePath )
{
	HINTERNET	hRemote;
	char	szExt[ _MAX_PATH ];
	char	szRemote[ MAX_PATH ];
	DWORD	dwWritten;
	UINT32	nread;
	UINT32	Rc;

	char	buf[1024];

	i3FileStream	stream;

	I3ASSERT( m_hFtp );
	
	_splitpath( pszRemotePath, nullptr, nullptr, szRemote, szExt );
	strncat( szRemote, szExt, sizeof(szRemote)-1 );

	hRemote = ::FtpOpenFile( m_hFtp, szRemote, GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, 0 );
	if( hRemote == nullptr )
	{	
		I3TRACE( "i3NetworkFTP::UploadFile2() - %d\n", ::GetLastError() );

		return false;
	}
	
	if( !stream.Open( pszLocalPath, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3TRACE( "i3NetworkFTP::UploadFile2() - can't local file open.\n" );
		return false;
	}

	nread = 1024;

	for( m_dwProg = 0; m_dwProg < stream.GetSize(); )
	{
		stream.SetPosition( m_dwProg, STREAM_BEGIN );

		nread = stream.GetSize() - m_dwProg;

		if( nread > 1024 )
			nread = 1024;

		Rc = stream.Read( buf, nread );
		if( Rc == STREAM_ERR )
			break;

		::InternetWriteFile( hRemote, buf, nread, &dwWritten );
		m_dwProg += dwWritten;

		wsprintf( m_szStr, "%s %d", pszLocalPath, m_dwProg);
	}

	stream.Close();

	::InternetCloseHandle( hRemote );
	return true;
}

bool i3NetworkFTP::CheckFile( const char * pszPath )
{
	HINTERNET hSrch;
	WIN32_FIND_DATA wfd;

	char	szRemotePath[ MAX_PATH ];
	DWORD	dwCDir = MAX_PATH;

	I3ASSERT( m_hFtp );

	if( !(getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN) )
	{
		if( !Connect() )
			return false;
	}

	memset( szRemotePath, 0, sizeof( szRemotePath ) );
	::FtpGetCurrentDirectory( m_hFtp, szRemotePath, &dwCDir );
	
	strncat( szRemotePath, pszPath, sizeof( szRemotePath)-1 );

	hSrch = ::FtpFindFirstFile( m_hFtp, pszPath, &wfd, 0, 0 );
	if( hSrch == 0 )
	{
		return false;
	}

	::InternetCloseHandle( hSrch );

	if( !(getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN ) )
		DisConnect();

	return true;
}

bool i3NetworkFTP::DeleteFile( const char * pszPath )
{
	I3ASSERT( m_hFtp );

	return ::FtpDeleteFile( m_hFtp, pszPath ) ? true : false;
}

bool i3NetworkFTP::CheckDirectory( const char * pszPath )
{
#if defined( I3_WINDOWS )
	char	szPrvPath[ MAX_PATH ];
	char	szTemp[ MAX_PATH ];
	char	szDirectoryName[ 64 ];
	INT32	nLen;

	if( !::SetCurrentDirectory( pszPath ) )
	{
		i3mem::FillZero( szPrvPath, sizeof( szPrvPath ) );
		nLen = i3::generic_string_size( pszPath ) - 1;

		if( nLen > 0 )
		{
			memcpy( szPrvPath, pszPath, nLen );
			_splitpath( szPrvPath, nullptr, szTemp, szDirectoryName, nullptr);
			
			// 주의
			CheckDirectory( szTemp );
			::SetCurrentDirectory( szTemp );
			::CreateDirectory( szDirectoryName, 0 );
		}
		return false;
	}
#endif
	return true;
}

bool i3NetworkFTP::CheckRemoteDir( HINTERNET hFtp, const char * pszPath )
{
#if defined( I3_WINDOWS )
	char	szPrvPath[ MAX_PATH ];
	char	szTemp[ MAX_PATH ];
	char	szDirectoryName[ 64 ];
	INT32	nLen;
	
	if( !::FtpSetCurrentDirectory( hFtp, pszPath ) )
	{	
		nLen = i3::generic_string_size( pszPath ) - 1;

		if( nLen > 1 )
		{
			memset( szPrvPath, 0, sizeof( szPrvPath ) );
			memcpy( szPrvPath, pszPath, nLen );
			_splitpath( szPrvPath, nullptr, szTemp, szDirectoryName, nullptr);
			
			// 주의
			CheckRemoteDir( hFtp, szTemp );
			if( i3::generic_string_size( szTemp ) > 0 )
			{
				::FtpSetCurrentDirectory( hFtp, szTemp );
			}
			if( !::FtpCreateDirectory( hFtp, szDirectoryName) )
			{
				I3PRINTLOG(I3LOG_WARN,  "리모트 폴더를 생성하지 못했습니다." );
			}
		}
		return false;
	}
#endif
	return true;
}

void i3NetworkFTP::ConvertInternetDir( char * pszPath, INT32 nLength )
{
#if defined( I3_WINDOWS )
	INT32	i;

	for( i = 0; i < nLength; i++ )
	{
		if( pszPath[ i ] == '\n' )
			return;
		else if( pszPath[ i ] == '\\' )
			pszPath[ i ] = '/';
	}
#endif
}