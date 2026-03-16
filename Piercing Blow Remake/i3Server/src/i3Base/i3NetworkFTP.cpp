#include "i3CommonType.h"
#include "i3NetworkFTP.h"
#include "i3StreamFile.h"

I3_CLASS_INSTANCE( i3NetworkFTP, i3ElementBase );

i3NetworkFTP::i3NetworkFTP()
{
	m_hFtp					= NULL;
	m_hInternet				= NULL;

	memset( m_szAddress, 0, sizeof(m_szAddress ) );
	m_nPort			= INTERNET_DEFAULT_FTP_PORT;
	memset( m_szUserID, 0, sizeof( m_szUserID ) );
	memset( m_szUserPass, 0, sizeof( m_szUserPass ) );

	m_dwCurrentDir			= 0;
	m_dwProg				= 0;
	m_nFlags				= 0;
	m_nLocalDirLen			= 0;
	m_szLocalDirectory[0]	= '\0';
	m_szRemoteDirectory[0]	= '\0';
	m_szStr[0]				= '\0';
	m_szAddress[0]			= '\0';
	m_szUserID[0]			= '\0';
	m_szUserPass[0]			= '\0';
}

i3NetworkFTP::~i3NetworkFTP()
{
}

BOOL i3NetworkFTP::Create( char * address, INT32 nPort, char * id, char * pass, UINT32 nFlag  )
{	
	setAddress( address );		// FTP address

	if( nPort == 0 )
		setPort( (UINT16) I3_FTP_DEFAULT_PORT );
	else
		setPort( (UINT16) nPort );			// port

	// 접속 아이디
	if( id == NULL )
		setUserID( I3_FTP_DEFAULT_ID );
	else
		setUserID( id );

	// 패스워드
	if( pass == NULL )
		setUserPass( I3_FTP_DEFAULT_PASS );
	else
		setUserPass( pass );

	// 플래그 세팅
	setFlags( nFlag );

	// 현재 로컬 폴더
	::GetCurrentDirectory( MAX_PATH, m_szLocalDirectory );

	m_nLocalDirLen	= i3String::Length( m_szLocalDirectory );
	
	if( getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN )
	{
		if( !Connect() )
			return FALSE;
	}

	// FTP Server directory
	if( getFlags() & I3_FTP_FLAG_CHECKREMOTEDIR )
	{
		if( !(getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN) )
		{
			if( !Connect() )
				return FALSE;
		}

		{
			DWORD	dwCur = MAX_PATH;

			::FtpGetCurrentDirectory( m_hFtp, m_szRemoteDirectory, &dwCur );
			
			ConvertInternetDir( m_szRemoteDirectory, i3String::Length( m_szRemoteDirectory) );

			if( !( getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN ) )
				DisConnect();
		}
	}

	return TRUE;
}

BOOL i3NetworkFTP::Connect()
{
	// INTERNET_FLAG_PASSIVE mode
	// 접속 권한을 가져 오기 위해 패시브 모드로 접속한 폴더의 하위 폴더까지 권한을 가져옵니다.
	m_hInternet = ::InternetOpen( "I3FTP", INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, INTERNET_FLAG_PASSIVE );

	if( m_hInternet == NULL )
		return FALSE;

	m_hFtp = ::InternetConnect( m_hInternet, getAddress(), (INTERNET_PORT) m_nPort,
		getUserID(), getUserPass(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0 );

	if( m_hFtp == NULL )
	{
		::InternetCloseHandle( m_hInternet );
		return FALSE;
	}

	return TRUE;
}

BOOL i3NetworkFTP::DisConnect()
{
	::InternetCloseHandle( m_hFtp );
	::InternetCloseHandle( m_hInternet );
	m_hFtp			= NULL;
	m_hInternet		= NULL;

	return TRUE;
}

BOOL i3NetworkFTP::DownLoadFile( char * pszPath )
{
	char	szRemote[ MAX_PATH ];
	char	szExt[ _MAX_PATH ];
	
	_splitpath( pszPath, NULL, NULL, szRemote, szExt );
	strcat( szRemote, szExt );

	return DownLoadFile( pszPath, szRemote );
}

BOOL i3NetworkFTP::DownLoadFile( char * pszLocalPath, char * pszRemotePath )
{
	BOOL	bResult = FALSE;
	char	szPath[ MAX_PATH ];
	char	szFileName[ MAX_PATH ];
	char	szExt[ _MAX_PATH ];
	
	char	szFilePath[ MAX_PATH ];
	INT32	nLocalLen, nFileLen;
	
	_splitpath( pszLocalPath, NULL, szPath, szFileName, szExt );
	strcat( szFileName, szExt );

	nLocalLen = i3String::Length( pszLocalPath );
	nFileLen = i3String::Length( szFileName );

	nLocalLen -= nFileLen;

	strcpy( szFilePath, m_szLocalDirectory );
	strcat( szFilePath, "\\" );

	if( nLocalLen > 0 )
	{
		strcat( szFilePath, szPath );

		CheckDirectory( szFilePath );
	}

	// 받을 디렉토리로 세팅
	::SetCurrentDirectory( szFilePath );

	sprintf( m_szStr, "Download : %s\n ", pszLocalPath );

	bResult = ::FtpGetFile( m_hFtp, pszRemotePath, szFileName, FALSE, FILE_ATTRIBUTE_NORMAL,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 0 );

	// 루트 디렉토리로 돌려놓는다.
	::SetCurrentDirectory( m_szLocalDirectory );

	return bResult;
}

BOOL i3NetworkFTP::DownLoadFile2( char * pszPath )
{
	char	szNewFileName[ MAX_PATH ];

	char	buf[1024];
	BOOL	bResult;
	char	szTemp[4096];

	DWORD	dwRead;
	i3FileStream	file;
	HINTERNET	hRemote;

	I3ASSERT( pszPath );
	I3ASSERT( m_hFtp );

	// open server file
	hRemote = ::FtpOpenFile( m_hFtp, pszPath, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0 );
	if( hRemote == NULL )
	{
		I3TRACE( "i3NetworkFTP::DownLoadFile2() - can't ftp open file.\n" );
		return FALSE;
	}

	// create local file
	::GetCurrentDirectory( MAX_PATH, szNewFileName );
	strcat( szNewFileName, "\\" );
	strcat( szNewFileName, pszPath );
	
	if( FALSE == file.Create( szNewFileName, STREAM_WRITE ) )
	{
		::InternetCloseHandle( hRemote );
		I3TRACE( "i3NetworkFTP::DownLoadFile2() - can't open file.\n" );
		return FALSE;
	}

	m_dwProg = 0;

	for( ;; )
	{
		bResult = ::InternetReadFile( hRemote, buf, 1024, &dwRead );
		if( bResult == TRUE && dwRead == 0 )
			break;

		file.Printf( szTemp, buf );
		m_dwProg += dwRead;

		wsprintf( m_szStr, "%s %d", pszPath, m_dwProg);
	}

	file.Close();

	::InternetCloseHandle( hRemote );
	return TRUE;
}

BOOL i3NetworkFTP::UploadFile( const char * pszPath )
{
	char	szExt[ _MAX_PATH ];
	char	szRemote[ MAX_PATH ];

	_splitpath( pszPath, NULL, NULL, szRemote, szExt );

	strcat( szRemote, szExt );
	return UploadFile( pszPath, szRemote );
}

BOOL i3NetworkFTP::UploadFile( const char * pszLocalPath, const char * pszRemotePath )
{
	BOOL	bResult;
	char	szRemoteAllPath[ MAX_PATH ];
	char	szRemotePath[ MAX_PATH ];
	char	szName[ MAX_PATH ];
	char	szExt[ _MAX_PATH ];

	INT32	nLocalLen, nRemoteLen;

	I3ASSERT( m_hFtp );

	_splitpath( pszRemotePath, NULL, szRemotePath, szName, szExt );
	strcat( szName, szExt );

	nLocalLen = i3String::Length( pszRemotePath );
	nRemoteLen = i3String::Length( szName );

	nLocalLen -= nRemoteLen;

	// for all format directory name
	ConvertInternetDir( szRemotePath, i3String::Length( szRemotePath ) );

	memset( szRemoteAllPath, 0, sizeof( szRemoteAllPath ) );
	strcpy( szRemoteAllPath, m_szRemoteDirectory + 1 );
	//strcat( szRemoteAllPath, "/" );
	strcat( szRemoteAllPath, szRemotePath );

	if( nLocalLen > 0 )
	{	
		if( !CheckRemoteDir( m_hFtp, szRemoteAllPath ) )
		{
			// 리모트 디렉토리 설정( 이 디렉토리에 저장을 한다. )
			if( !::FtpSetCurrentDirectory( m_hFtp, szRemoteAllPath ) )
			{
				I3FATAL( "[i3NetworkFTP::UplodaFile] - error set current directory fail.\n" );
				return FALSE;
			}
		}
	}
	// 디렉토리에 쓰기 설정이 되어 있는지 확인..
	bResult = UploadFile2( pszLocalPath, pszRemotePath );
	
	// 리모트 루트리렉토리로 돌린다.
	::FtpSetCurrentDirectory( m_hFtp, m_szRemoteDirectory );

	return bResult;
}

BOOL i3NetworkFTP::UploadFile2( const char * pszPath )
{
	char	szExt[ _MAX_PATH ];
	char	szRemote[ MAX_PATH ];

	_splitpath( pszPath, NULL, NULL, szRemote, szExt );
	strcat( szRemote, szExt );

	return UploadFile2( pszPath, szRemote );
}

BOOL i3NetworkFTP::UploadFile2( const char * pszLocalPath, const char * pszRemotePath )
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
	
	_splitpath( pszRemotePath, NULL, NULL, szRemote, szExt );
	strcat( szRemote, szExt );

	hRemote = ::FtpOpenFile( m_hFtp, szRemote, GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, 0 );
	if( hRemote == NULL )
	{	
		I3TRACE( "i3NetworkFTP::UploadFile2() - %d\n", ::GetLastError() );

		return FALSE;
	}
	
	if( !stream.Open( pszLocalPath, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3TRACE( "i3NetworkFTP::UploadFile2() - can't local file open.\n" );
		return FALSE;
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
	return TRUE;
}

BOOL i3NetworkFTP::CheckFile( const char * pszPath )
{
	HINTERNET hSrch;
	WIN32_FIND_DATA wfd;

	char	szRemotePath[ MAX_PATH ];
	DWORD	dwCDir = MAX_PATH;

	I3ASSERT( m_hFtp );

	if( !(getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN) )
	{
		if( !Connect() )
			return FALSE;
	}

	memset( szRemotePath, 0, sizeof( szRemotePath ) );
	::FtpGetCurrentDirectory( m_hFtp, szRemotePath, &dwCDir );
	
	strcat( szRemotePath, pszPath );

	hSrch = ::FtpFindFirstFile( m_hFtp, pszPath, &wfd, 0, 0 );
	if( hSrch == 0 )
	{
		return FALSE;
	}

	::InternetCloseHandle( hSrch );

	if( !(getFlags() & I3_FTP_FLAG_CONNECTMAINTAIN ) )
		DisConnect();

	return TRUE;
}

BOOL i3NetworkFTP::DeleteFile( const char * pszPath )
{
	I3ASSERT( m_hFtp );

	return ::FtpDeleteFile( m_hFtp, pszPath );
}

void i3NetworkFTP::OnDownLoad()
{
}

void i3NetworkFTP::OnUpLoad()
{

}

BOOL i3NetworkFTP::CheckDirectory( const char * pszPath )
{
#if defined( I3_WINDOWS )
	char	szPrvPath[ MAX_PATH ];
	char	szTemp[ MAX_PATH ];
	char	szDirectoryName[ 64 ];
	INT32	nLen;

	if( !::SetCurrentDirectory( pszPath ) )
	{
		i3mem::FillZero( szPrvPath, sizeof( szPrvPath ) );
		nLen = i3String::Length( pszPath ) - 1;

		if( nLen > 0 )
		{
			memcpy( szPrvPath, pszPath, nLen );
			_splitpath( szPrvPath, NULL, szTemp, szDirectoryName, NULL );
			
			// 주의
			CheckDirectory( szTemp );
			::SetCurrentDirectory( szTemp );
			::CreateDirectory( szDirectoryName, 0 );
		}
		return FALSE;
	}
#endif
	return TRUE;
}

BOOL i3NetworkFTP::CheckRemoteDir( HINTERNET hFtp, const char * pszPath )
{
#if defined( I3_WINDOWS )
	char	szPrvPath[ MAX_PATH ];
	char	szTemp[ MAX_PATH ];
	char	szDirectoryName[ 64 ];
	INT32	nLen;
	
	if( !::FtpSetCurrentDirectory( hFtp, pszPath ) )
	{	
		nLen = i3String::Length( pszPath ) - 1;

		if( nLen > 1 )
		{
			memset( szPrvPath, 0, sizeof( szPrvPath ) );
			memcpy( szPrvPath, pszPath, nLen );
			_splitpath( szPrvPath, NULL, szTemp, szDirectoryName, NULL );
			
			// 주의
			CheckRemoteDir( hFtp, szTemp );
			if( i3String::Length( szTemp ) > 0 )
			{
				::FtpSetCurrentDirectory( hFtp, szTemp );
			}
			if( !::FtpCreateDirectory( hFtp, szDirectoryName) )
			{
				I3WARN( "리모트 폴더를 생성하지 못했습니다.\n" );
			}
		}
		return FALSE;
	}
#endif
	return TRUE;
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