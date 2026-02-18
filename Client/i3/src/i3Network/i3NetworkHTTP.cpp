#include "i3NetworkDef.h"
#include "i3NetworkHTTP.h"
#include "i3Base/string/compare/generic_is_nequal.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/atoi.h"

I3_CLASS_INSTANCE( i3NetworkHTTP);

i3NetworkHTTP::i3NetworkHTTP()
{
	i3mem::FillZero( m_ArgumentArray, sizeof(m_ArgumentArray));
}

i3NetworkHTTP::~i3NetworkHTTP()
{
	Destroy(); 
}

bool i3NetworkHTTP::Connect( char* pszURL, REQUEST_METHOD method)
{
	bool ret = false;

	switch( method)
	{
	case i3NetworkHTTP::REQUEST_METHOD_GET:
	default:
		ret = _ConnectGET(pszURL);
		break;
	case i3NetworkHTTP::REQUEST_METHOD_PUT:
		ret = _ConnectPUT(pszURL);
		break;
	case i3NetworkHTTP::REQUEST_METHOD_POST:
		ret = _ConnectPOST(pszURL);
		break;
	case i3NetworkHTTP::REQUEST_METHOD_POST_MULTIPARTSFORMDATA:
		break;
	}

	return ret;
}

bool i3NetworkHTTP::IsInternetConnect(void)
{
	DWORD dwFlags;

	return ::InternetGetConnectedState(&dwFlags, 0) ? true : false;
}

void i3NetworkHTTP::DisConnect(void)
{
	_DisConnectPUT();
	_DisConnectPOST();
	_DisConnectGET();
}

void i3NetworkHTTP::CreateMethod( UINT32 nRestartCount, UINT32 nRestartSleep, UINT32 nReloadCount, UINT32 nReloadTimeout, UINT32 nBufferSize )
{
	// nRestartCount	= 재접속 횟수 (접속 실패시 내부에서 다시 접속을 시도합니다. )
	// nRestartTime		= 재접속 시간간격 ( 이시간마다 재접속을 시도합니다 )
	// nReloadCount		= 재시도 횟수 ( 파일을 다시 받도록 시도하는 횟수 )
	// nTimeoutCount	= 타임아웃시간 ( 이간격동안 받지못하거나 사이즈가 계속 0이면 재시도합니다. )

	m_nTotalRestart		= nRestartCount;
	m_nTotalReload		= nReloadCount;

	m_nRestartSleep		= nRestartSleep;
	m_nReloadTimeOut	= nReloadTimeout;
	
	m_nOutTimer			= 0;

	m_nReadBufSize		= nBufferSize;

	m_pReadBuf = (char *) i3MemAlloc( m_nReadBufSize );
	i3mem::FillZero( m_pReadBuf, m_nReadBufSize );

	m_State				= HTTP_STATE_NONE;
}

void i3NetworkHTTP::Destroy(void)
{
	Cancel();
	_DisConnectGET();
	I3MEM_SAFE_FREE_POINTER( m_pReadBuf );

	ResetArguments();
}

bool i3NetworkHTTP::_CheckType( char * pszType )
{
	//INT32	rc = 1;	
	//if( pszType[0] != 't' )		rc = 0;
	//if( pszType[1] != 'e' )		rc = 0;
	//if( pszType[2] != 'x' )		rc = 0;
	//if( pszType[3] != 't' )		rc = 0;
	//if( pszType[4] != '/' )		rc = 0;
	
	if( pszType[5] != 'h' )			return true; 
	if( pszType[6] != 't' )			return true;
	if( pszType[7] != 'm' )			return true;
	if( pszType[8] != 'l' )			return true;
		//if( rc > 0 )		return false;

	return false;
}

bool i3NetworkHTTP::_ConnectGET( char * pszURL )
{
	// Open Internet
	m_hInternet = ::InternetOpen( m_szAgentName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if( m_hInternet == nullptr )
	{
		// Error
		i3::safe_string_copy( m_szLastError, "Cannot open internet", 1024);
		m_nLastErrorCode=::GetLastError();
		return false;
	}
	
	return true;
}

bool i3NetworkHTTP::_ConnectPUT( char * pszURL)
{	
	ParseURL(pszURL, m_szProtocol, m_szAddress, m_nPort, m_szURI);

	// Open Internet
	m_hInternet = ::InternetOpen( "I3NETWORK_HTTP", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if( m_hInternet == nullptr )
	{
		// Error
		i3::safe_string_copy( m_szLastError, "Cannot open internet", 1024);
		m_nLastErrorCode=::GetLastError();
		return false;
	}

	// Open URL
	m_hConnection = ::InternetConnect( m_hInternet, m_szAddress, m_nPort, m_szUserName, m_szUserPW, INTERNET_SERVICE_HTTP, 0, 0 );

	if( m_hConnection == nullptr )
	{
		// Error
		i3::safe_string_copy( m_szLastError, "Cannot connect to internet", 1024);
		m_nLastErrorCode=::GetLastError();
		::InternetCloseHandle( m_hInternet );
		m_hInternet = nullptr;
		return false;
	}

	m_hHttp = ::HttpOpenRequest( m_hConnection, _HTTP_VERB_PUT, m_szURI, NULL, NULL, NULL, 0, 0 );

	if( m_hHttp == nullptr )
	{
		// Error
		m_nLastErrorCode=::GetLastError();
		::InternetCloseHandle(m_hConnection);
		m_hConnection = nullptr;
		::InternetCloseHandle(m_hInternet);
		m_hInternet = nullptr;
		return false;
	}

	return true;
}

bool i3NetworkHTTP::_ConnectPOST(char* pszURL)
{
	m_State = HTTP_STATE_POST;

	ParseURL(pszURL, m_szProtocol, m_szAddress, m_nPort, m_szURI);

	// Initializes an application's use of the WinINet functions.
	{
		m_hInternet = ::InternetOpen(m_szAgentName, INTERNET_OPEN_TYPE_PRECONFIG , NULL, NULL, 0);
			
		if (!m_hInternet) 
		{
			i3::safe_string_copy( m_szLastError, "Cannot open internet", 1024);
			m_nLastErrorCode = ::GetLastError();
			return false;
		}
	}
		
	// Opens an File Transfer Protocol (FTP), Gopher, or HTTP session for a given site.
	{
		m_hConnection=::InternetConnect(m_hInternet,							// internet opened handle
										m_szAddress,							// server name
										(UINT16)m_nPort,						// ports
										m_szUserName,							// user name
										m_szUserPW,							// password 
										INTERNET_SERVICE_HTTP,				// service type
										INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION,	// service option																														
										0);		
		if (!m_hConnection) 
		{			
			i3::safe_string_copy( m_szLastError, "Cannot connect to internet", 1024);
			m_nLastErrorCode = ::GetLastError();
			::CloseHandle(m_hInternet);
			m_hInternet = nullptr;
			return false;
		}
	}

	{
		DWORD dwData = 0;
		DWORD dwSize = sizeof(dwData); 

		InternetQueryOption( m_hConnection, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &dwData, &dwSize);

		dwData = 40;
		InternetSetOption( m_hConnection, INTERNET_OPTION_MAX_CONNS_PER_1_0_SERVER, &dwData, sizeof(dwData) );
	}


	// Attempts to make a connection to the Internet.
	if(::InternetAttemptConnect(0) != ERROR_SUCCESS)
	{		
		m_nLastErrorCode=::GetLastError();
		::InternetCloseHandle(m_hConnection);
		::InternetCloseHandle(m_hInternet);
		m_hConnection = nullptr;
		m_hInternet = nullptr;
		return false;
	}

	return true;
}


void i3NetworkHTTP::_DisConnectGET(void)
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp = nullptr;
	}

	if( m_hInternet )
	{
		::InternetCloseHandle( m_hInternet );
		m_hInternet = nullptr;
	}
}

void i3NetworkHTTP::_DisConnectPUT()
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp			= nullptr;
	}
	if( m_hConnection )
	{
		::InternetCloseHandle( m_hConnection );
		m_hConnection	= nullptr;
	}
	if( m_hInternet )
	{
		::InternetCloseHandle( m_hInternet );
		m_hInternet		= nullptr;
	}
}

void i3NetworkHTTP::_DisConnectPOST()
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp			= nullptr;
	}
	if( m_hConnection )
	{
		::InternetCloseHandle( m_hConnection );
		m_hConnection	= nullptr;
	}
	if( m_hInternet )
	{
		::InternetCloseHandle( m_hInternet );
		m_hInternet		= nullptr;
	}
}

INT32 i3NetworkHTTP::DownFile(void)
{	
	UINT32	nAvailableSize = 65536;
	UINT32	nRead;
	UINT32	nWritten = 0;
	
	if( m_State == HTTP_STATE_DOWNLOADING)
	{	
		I3ASSERT( m_pReadBuf );

		// 다운 받을 패킷량을 구한다.
		BOOL Rv = ::InternetQueryDataAvailable( m_hHttp, (LPDWORD) &nAvailableSize, 0, 0 );

		// 실패 또는 사이즈 0이면 타임아웃을 체크
		if( !Rv || !nAvailableSize )
		{	
			m_nLocktime = ::GetCurrentTime() - m_nDownTime;
			m_nOutTimer	+= m_nLocktime;

			if( m_nOutTimer > m_nReloadTimeOut )
			{
				Cancel();
				// 재시도 합니다.
				if( ReloadCheck() )
				{	
					DownStart( m_szURL, m_szPath );
				}
				else
				{
					m_State = HTTP_STATE_DOWNFAILE;
					m_FileErrorCode = HTTP_ERR_DOWNSIZE;
				}

				goto ExitErr; 
			}

			m_nDownTime	= ::GetCurrentTime();
			return m_State;
		}

		// 파일을 읽어 온다. 혹시나 성백 
		Rv = ::InternetReadFile( m_hHttp, m_pReadBuf, nAvailableSize, (LPDWORD) &nRead );

		if( !Rv || !nRead )
		{
			m_nLocktime = ::GetCurrentTime() - m_nDownTime;
			m_nOutTimer	+= m_nLocktime;

			if( m_nOutTimer > m_nReloadTimeOut )
			{
				Cancel();
				// 재시도 합니다.
				if( ReloadCheck() )
				{	
					DownStart( m_szURL, m_szPath );
				}
				else
				{
					m_State = HTTP_STATE_DOWNFAILE;
					m_FileErrorCode = HTTP_ERR_READFILE;
				}
				goto ExitErr;
			}
			m_nDownTime	= ::GetCurrentTime();
			return m_State;
		}

		// 파일을 로컬에 저장
		if( ::WriteFile( m_hFile, m_pReadBuf, nRead, (LPDWORD) &nWritten, NULL) == FALSE )
		{
			Cancel();
			m_State = HTTP_STATE_DOWNFAILE;
			m_FileErrorCode = HTTP_ERR_WRITEFILE;
			goto ExitErr; 
		}

		// 다운로드 받은 양
		m_nRecv += nWritten;

		m_nOutTimer	= 0;
		m_nDownTime	= ::GetCurrentTime();

		if( m_nSize == m_nRecv )
		{
			// 다운로드 완료
			_DisConnectGET();
			::CloseHandle( m_hFile );
			m_hFile = nullptr; 

			m_nRestartCount = 0;
			m_nReloadCount	= 0;
			
			m_State	= HTTP_STATE_DOWNCOMPLETE;
		}
	}

ExitErr : 

	return m_State;
}

bool i3NetworkHTTP::ReloadCheck()
{
	m_nReloadCount++;
	if( m_nReloadCount > m_nTotalReload )
		return false;

	m_State = HTTP_STATE_REDOWNLOAD;
	return true;
}

bool i3NetworkHTTP::RestartCheck()
{
	m_nRestartCount++;
	if( m_nRestartCount > m_nTotalRestart )
		return false;

	return true;
}

INT32 i3NetworkHTTP::DownStart( char * pszURL, char * pszPath )
{
	bool	Rv;
	if( HTTP_STATE_DOWNLOADING == m_State )	return HTTP_ERR_REDOWNLOAD; 

	// 접속
	m_nSize = 0;
	m_nRecv	= 0;

	if( m_State != HTTP_STATE_REDOWNLOAD )
	{
		i3::safe_string_copy( m_szURL, pszURL, MAX_PATH );
		i3::safe_string_copy( m_szPath, pszPath, MAX_PATH );
	}
	
	m_nOutTimer	= 0;
	m_nDownTime	= ::GetCurrentTime();

	Rv = _ConnectGET( pszURL );

	while( !Rv )
	{
		m_nLocktime = ::GetCurrentTime() - m_nDownTime;
		m_nOutTimer += m_nLocktime;

		if( m_nOutTimer > m_nRestartSleep )
		{
			if( !RestartCheck() )
			{
				// 접속 실패입니다.
				m_nRestartCount	 = 0;
				m_State = HTTP_STATE_DOWNFAILE; 
				return HTTP_ERR_CONNECT;
			}

			Rv = _ConnectGET( pszURL );

			m_nDownTime = ::GetCurrentTime();
		}
	}

	// 파일 생성
	m_hFile = ::CreateFile( pszPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		m_State = HTTP_STATE_DOWNFAILE; 
		return HTTP_ERR_CREATEFILE; 
	}

	m_nRestartCount	 = 0;
	m_nReloadCount = 0;
	m_State = HTTP_STATE_DOWNLOADING;
	m_nOutTimer	= 0;
	m_nDownTime	= ::GetCurrentTime();

	return m_nSize;
}

void i3NetworkHTTP::Cancel()
{
	if( m_State == HTTP_STATE_DOWNLOADING )
	{
		_DisConnectGET();
	}
	else if( m_State == HTTP_STATE_UPLOADING )
	{
		_DisConnectPUT();
	}
	else if( m_State == HTTP_STATE_POST )
	{
		_DisConnectPOST();
	}

	::CloseHandle( m_hFile );
	m_hFile = nullptr;

	m_State = HTTP_STATE_NONE;
}

INT32 i3NetworkHTTP::UpStart( char * pszURL, char * pszPath )
{
	INT32 Rv = HTTP_ERR_REUPLOAD; 

	BY_HANDLE_FILE_INFORMATION	fileinfo;

	INTERNET_BUFFERS		ibuf;

	if( m_State != HTTP_STATE_UPLOADING )
	{
		// 접속
		m_nSize = 0;
		m_nRecv	= 0;

		if( _ConnectPUT( pszURL) == false)
		{
			// 접속 실패입니다.
			m_State = HTTP_STATE_UPFAILE; 
			return HTTP_ERR_CONNECT;
		}

		ibuf.dwStructSize	= sizeof( INTERNET_BUFFERS );
		ibuf.Next			= nullptr;
		ibuf.lpcszHeader	= nullptr;
		ibuf.dwHeadersLength	= 0;
		if( ::HttpSendRequest( m_hHttp, NULL, 0, NULL, 0) == FALSE )
		{
			// 명령을 보내지 못했습니다.
			m_State = HTTP_STATE_UPFAILE;
		}
		else
		{
			m_State = HTTP_STATE_UPLOADING;

			// 파일을 오픈합니다.
			m_hFile = ::CreateFile( pszPath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL);
			// 파일 사이즈를 넣어줘야됩니다....
			::GetFileInformationByHandle( m_hFile, &fileinfo );
			
			Rv = m_nSize	= (fileinfo.nFileSizeHigh << 16) | ( fileinfo.nFileSizeLow );

			m_nSend = 0;
		}
	}

	return Rv;
}

INT32 i3NetworkHTTP::UpFile( void )
{
	UINT32	nAvailableSize = 65536;
	UINT32	nWritten = 0;
	UINT32	nRead = 0;

	if( m_State == HTTP_STATE_UPLOADING)
	{
		// 업로드할 수 있는 패킷량을 구한다.
		if( ::InternetQueryDataAvailable( m_hHttp, (LPDWORD) &nAvailableSize, 0, 0 ) == FALSE )
		{
			Cancel();
			m_State = HTTP_STATE_UPFAILE;
			return HTTP_ERR_UPSIZE;
		}
		// 파일을 로컬에서 읽어온다.
		if( ::ReadFile( m_hFile, m_pReadBuf, nAvailableSize, (LPDWORD) &nRead, NULL) == FALSE )
		{			
			Cancel();
			m_State = HTTP_STATE_UPFAILE;
			return HTTP_ERR_READFILE;
		}

		// 패킷 전송
		if( ::InternetWriteFile( m_hHttp, m_pReadBuf, nRead, (LPDWORD) &nWritten ) == FALSE )
		{
			Cancel();
			m_State = HTTP_STATE_UPFAILE;
			return HTTP_ERR_WRITEFILE;
		}

		// 업로드한 양
		m_nSend += nWritten;

		if( m_nSize == m_nRecv )
		{
			// 다운로드 완료
			_DisConnectGET();
			::CloseHandle( m_hFile );
			m_State	= HTTP_STATE_UPCOMPLETE;
		}
	}

	return m_State; 
}


bool i3NetworkHTTP::CheckFile( char * pszPath )
{
	bool Rv;
	Rv = _ConnectGET( pszPath );
	_DisConnectGET(); 
	return Rv;
}

INT32 i3NetworkHTTP::DeleteFile( char * pszPath )
{
	return 1;
}

bool i3NetworkHTTP::SendRequestPost(void)
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp = nullptr;
	}

	// Creates an HTTP request handle.
	wchar_t* accept[2]={L"*/*",0}; 
	m_hHttp = ::HttpOpenRequest(m_hConnection, _HTTP_VERB_POST, m_szURI, NULL, "", (const char**) accept, m_OpenRequestFlag, 0 );
	if( m_hHttp == FALSE)
	{
		m_nLastErrorCode=::GetLastError();
		return false;
	}

	try
	{	
		INT32 result =  HttpSendRequest(m_hHttp,
										DEFAULT_HTTP_HEADER,
										(DWORD)strlen(DEFAULT_HTTP_HEADER),
										(LPVOID)m_szPostArguments,
										(DWORD)i3::generic_string_size(m_szPostArguments));
		if (result) 	
		{
			return true;
		}

		INT32 lastErr = ::GetLastError();

		if (lastErr == ERROR_INTERNET_INVALID_URL)
		{
			i3::safe_string_copy( m_szLastError, "Cannot perform http request, The URL is invalid.", 1024);
			m_nLastErrorCode = ::GetLastError();
			return false;
		}
		else if (lastErr == ERROR_INTERNET_CANNOT_CONNECT)
		{
			i3::safe_string_copy( m_szLastError, "Cannot perform http request, The attempt to connect to the server failed.", 1024);
			m_nLastErrorCode = ::GetLastError();
			return false;
		}			
		else
		{
			i3::safe_string_copy( m_szLastError, "Cannot perform http request", 1024);
			m_nLastErrorCode = ::GetLastError();
			return false;
		}
	}
	catch(...) 
	{
		i3::safe_string_copy( m_szLastError, "Memory Exception occured", 1024);
		m_nLastErrorCode = ::GetLastError();
		return false;
	}
}

bool i3NetworkHTTP::SendRequestGet(char* strTemp)
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp = nullptr;
	}

	// Open URL
	m_hHttp = ::InternetOpenUrl( m_hInternet, strTemp, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_CACHE_WRITE , 0 );
	if(nullptr == m_hHttp )
	{
		// Error
		i3::safe_string_copy( m_szLastError, "Cannot connect to internet", 1024);
		m_nLastErrorCode = ::GetLastError();
		_DisConnectGET();
		return false;
	}
	//// Get File Size
	//{
	//	char pBuff[ 32 ];
	//	UINT32 nSize = sizeof( pBuff );
	//	char	pTypeBuf[ 32 ];
	//	UINT32	nTypeSize = sizeof( pTypeBuf );
	//	
	//	if( !::HttpQueryInfo( m_hHttp, HTTP_QUERY_CONTENT_LENGTH, pBuff, (LPDWORD)&nSize, NULL ) )
	//	{
	//		//
	//		_DisConnectGET();
	//		return false;
	//	}

	//	if( !::HttpQueryInfo( m_hHttp, HTTP_QUERY_CONTENT_TYPE, pTypeBuf, (LPDWORD) &nTypeSize, NULL ) )
	//	{
	//		_DisConnectGET();
	//		return false;
	//	}

	//	// 파일 타입 파싱( html 파일인 경우 에러를 리턴합니다. )
	//	//if( !_CheckType( pTypeBuf ) )
	//	//{
	//	//	_DisConnectGET();
	//	//	return false;
	//	//}

	//	m_nSize = (UINT32) i3::atoi( pBuff );
	//	if( m_nSize == 0 )
	//	{
	//		_DisConnectGET();
	//		return false;
	//	}
	//}

	return true;
}

INT32	i3NetworkHTTP::RecvRequest()
{
	m_pReadBuf[ 0 ] = '\0';			// 초기화

	DWORD dwNumberOfBytesRead;
	char szTemp[1024];
	INT32 nResult; 
	INT32 nRecvSize = 0;

	do 
	{
		nResult = InternetReadFile( m_hHttp, szTemp, 1023, &dwNumberOfBytesRead );
		szTemp[ dwNumberOfBytesRead ] = '\0';
		nRecvSize += dwNumberOfBytesRead;
		if( m_nReadBufSize < nRecvSize )
		{
			nRecvSize -= dwNumberOfBytesRead;
			break;			
		}
		i3::generic_string_cat( m_pReadBuf, szTemp);
	} while( nResult && (dwNumberOfBytesRead != 0) );

	m_pReadBuf[ m_nReadBufSize-1 ] = '\0';

	return nRecvSize;
}

void i3NetworkHTTP::ParseURL(char* pszURL, char* pszProtocol, char* pszAddress, UINT16 &nPort, char* pszURI)
{
	DWORD dwPosition=0;
	bool bFlag = false;

	while( i3::generic_string_size(pszURL) > 0 && 
		dwPosition < (DWORD)i3::generic_string_size(pszURL) && 
		i3::generic_is_nequal((pszURL+dwPosition), ":", 1) == false )
//		i3String::NCompare((pszURL+dwPosition), ":", 1))
		++dwPosition;

//	if( !i3String::NCompare((pszURL+dwPosition+1), "/", 1))
	if ( i3::generic_is_nequal((pszURL+dwPosition+1), "/", 1))
	{	
		// is PROTOCOL
		i3::string_ncopy_nullpad(pszProtocol, pszURL, dwPosition);
		pszProtocol[dwPosition]=0;
		bFlag= true;
	}
	else
	{	
		// is HOST 
		i3::string_ncopy_nullpad(pszProtocol, "http", 4);
		pszProtocol[5]=0;
	}

	DWORD dwStartPosition=0;	
	if(bFlag)	dwStartPosition=dwPosition+=3;				
	else		dwStartPosition=dwPosition=0;

	bFlag = false;
	while( i3::generic_string_size(pszURL) > 0 && 
		dwPosition < (DWORD)i3::generic_string_size(pszURL) && 
		i3::generic_is_nequal((pszURL+dwPosition), "/", 1) == false)
//		i3String::NCompare((pszURL+dwPosition), "/", 1))
			++dwPosition;

	DWORD dwFind = dwStartPosition;

	for( ; dwFind <= dwPosition ; dwFind++)
	{
//		if( !i3String::NCompare( (pszURL+dwFind), ":", 1))
		if ( i3::generic_is_nequal((pszURL+dwFind), ":", 1))
		{ 
			// find PORT
			bFlag= true;
			break;
		}
	}

	if( bFlag)
	{
		char sztmp[_TEXT_BUFFER_SIZE] = "";
		i3::string_ncopy_nullpad( sztmp, (pszURL+dwFind+1), ((dwPosition-1) - dwFind));
		nPort = (UINT16)i3::atoi(sztmp);
		i3::string_ncopy_nullpad(pszAddress, (pszURL+dwStartPosition), dwFind-dwStartPosition);
	}
	else if( i3::generic_is_iequal(pszProtocol,"https") )
	{
		nPort = INTERNET_DEFAULT_HTTPS_PORT;
		i3::string_ncopy_nullpad( pszAddress, (pszURL+dwStartPosition), dwPosition-dwStartPosition);
		
		// HTTPS프로토콜이라면, 플래그를 추가합니다.
		m_OpenRequestFlag |= INTERNET_FLAG_SECURE;
	}
	else 
	{
		nPort = INTERNET_DEFAULT_HTTP_PORT;
		i3::string_ncopy_nullpad( pszAddress, (pszURL+dwStartPosition), dwPosition-dwStartPosition);
	}

	if( dwPosition < (DWORD)i3::generic_string_size(pszURL))
	{
		// find URI
		i3::string_ncopy_nullpad( pszURI, (pszURL+dwPosition), i3::generic_string_size(pszURL)-dwPosition);
	}
	else
	{
		pszURI[0]=0;
	}

	return;
}

void i3NetworkHTTP::ResetArguments(void)
{
	m_ArgumentArrayCount = 0;
}

bool i3NetworkHTTP::AddArgument( char* pszName, char* pszValue )
{
	if( MAX_HTTP_ARGUMENT <= m_ArgumentArrayCount )	return false;

	i3::safe_string_copy( m_ArgumentArray[m_ArgumentArrayCount].szName, pszName, MAX_PATH );
	i3::safe_string_copy( m_ArgumentArray[m_ArgumentArrayCount].szValue, pszValue, MAX_PATH );
	m_ArgumentArray[m_ArgumentArrayCount].dwType = i3NetworkHTTP::TYPE_POST_ARGUMENT_NORMAL;
	m_ArgumentArrayCount++;

	return true;
}

bool i3NetworkHTTP::AddArgument( char* pszName, INT32 nValue)
{
	if( MAX_HTTP_ARGUMENT <= m_ArgumentArrayCount )	return false;

	i3::safe_string_copy( m_ArgumentArray[m_ArgumentArrayCount].szName, pszName, MAX_PATH );
	i3::snprintf( m_ArgumentArray[m_ArgumentArrayCount].szValue, MAX_PATH, "%d", nValue );
	m_ArgumentArray[m_ArgumentArrayCount].dwType = i3NetworkHTTP::TYPE_POST_ARGUMENT_NORMAL;
	m_ArgumentArrayCount++;

	return true;
}

bool i3NetworkHTTP::AddArgument( char* pszName, INT64 i64Value)
{
	if( MAX_HTTP_ARGUMENT <= m_ArgumentArrayCount )	return false;

	i3::safe_string_copy( m_ArgumentArray[m_ArgumentArrayCount].szName, pszName, MAX_PATH );
	i3::snprintf( m_ArgumentArray[m_ArgumentArrayCount].szValue, MAX_PATH, "%I64d", i64Value );
	m_ArgumentArray[m_ArgumentArrayCount].dwType = i3NetworkHTTP::TYPE_POST_ARGUMENT_NORMAL;
	m_ArgumentArrayCount++;

	return true;
}

void i3NetworkHTTP::SetUpPostArguments(void)
{
	m_szPostArguments[0] = '\0';	

	for( UINT32 i = 0; i < m_ArgumentArrayCount; i++)
	{
		i3::generic_string_cat( m_szPostArguments, m_ArgumentArray[i].szName);
		i3::generic_string_cat( m_szPostArguments, "=");
		i3::generic_string_cat( m_szPostArguments, m_ArgumentArray[i].szValue);
		if( i !=  m_ArgumentArrayCount - 1) i3::generic_string_cat( m_szPostArguments, "&");
	}
}

void i3NetworkHTTP::ConvertMD5( char* pOutputString, char* pInputString, UINT32 nBufSize)
{
	UINT128 code;

	i3MD5::Gen( pInputString, i3::generic_string_size( pInputString), (char *)&code);
	i3MD5::GetMD5String( pOutputString, code, nBufSize);
}
