#include "stdafx.h"
#include "Tcpsocket.h"
#include "NetworkBase.h"
#include "Memory.h"
#include "String.h"
#include "MD5.h"
#include "Http.h"

NSM_Http::NSM_Http()
{
	m_hInternet				= NULL;
	m_hHttp					= NULL;
	m_hConnection			= NULL;
	m_hFile					= NULL;
	
	m_pReadBuf				= NULL;
	m_nReadBufSize			= 0;

	m_nSize					= 0;
	m_nRecv					= 0;

	m_nPort					= 0;
	m_nLastErrorCode		= 0;
	m_szUserName[0]			= '\0';
	m_szUserPW[0]			= '\0';
	m_szProtocol[0]			= '\0';
	m_szAddress[0]			= '\0';
	m_szURI[0]				= '\0';
	m_szPostArguments[0]	= '\0';
	m_szLastError[0]		= '\0';
	m_szPath[0]				= '\0';
	m_szURL[0]				= '\0';
	
	m_FileErrorCode			= 0;
	m_State					= HTTP_STATE_NONE;
	m_nDownTime				= 0;
	m_nLocktime				= 0;
	m_nReloadCount			= 0;
	m_nReloadTimeOut		= 0;
	m_nRestartCount			= 0;
	m_nRestartSleep			= 0;
	m_nSend					= 0;
	m_nTotalReload			= 0;
	m_nTotalRestart			= 0;
	m_nOutTimer				= 0;

	NSMString::Copy( m_szAgentName, DEFAULT_AGENT_NAME);

	m_OpenRequestFlag = INTERNET_FLAG_RELOAD|INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES;// | INTERNET_FLAG_SECURE
	ResetArguments();	
}

NSM_Http::~NSM_Http()
{
	Destroy(); 
}

BOOL NSM_Http::Connect( char* pszURL, REQUEST_METHOD method)
{
	BOOL ret = FALSE;

	switch( method)
	{
	case REQUEST_METHOD_GET:
	default:
		ret = _ConnectGET(pszURL);
		break;
	case REQUEST_METHOD_PUT:
		ret = _ConnectPUT(pszURL);
		break;
	case REQUEST_METHOD_POST:
		ret = _ConnectPOST(pszURL);
		break;
	case REQUEST_METHOD_POST_MULTIPARTSFORMDATA:
		break;
	}

	return ret;
}

BOOL NSM_Http::IsInternetConnect(void)
{
	DWORD dwFlags;

	BOOL bChkInternet = InternetGetConnectedState(&dwFlags, NULL);

	return bChkInternet;
}

void NSM_Http::DisConnect(void)
{
	_DisConnectPUT();
	_DisConnectPOST();
	_DisConnectGET();
}

void NSM_Http::CreateMethod( UINT32 nRestartCount, UINT32 nRestartSleep, UINT32 nReloadCount, UINT32 nReloadTimeout, UINT32 nBufferSize )
{
	// nRestartCount	= 재접속 횟수 (접속 실패시 내부에서 다시 접속을 시도합니다. )
	// nRestartTime		= 재접속 시간간격 ( 이시간마다 재접속을 시도합니다 )
	// nReloadCount		= 재시도 횟수 ( 파일을 다시 받도록 시도하는 횟수 )
	// nTimeoutCount	= 타임아웃시간 ( 이간격동안 받지못하거나 사이즈가 계속 0이면 재시도합니다. )

	m_nTotalRestart		= nRestartCount;
	m_nTotalReload		= nReloadCount;

	m_nRestartSleep		= nRestartSleep;
	m_nReloadTimeOut	= nReloadTimeout;
	
	m_nReadBufSize		= nBufferSize;

	m_pReadBuf = (char *) NSM_nsMemory::Alloc( m_nReadBufSize );
	NSM_nsMemory::FillZero( m_pReadBuf, m_nReadBufSize );
}

void NSM_Http::Destroy(void)
{
	Cancel();
	_DisConnectGET();
	NSM_SAFE_FREE( m_pReadBuf );

	ResetArguments();
}

BOOL NSM_Http::_CheckType( char * pszType )
{
	//INT32	rc = 1;	
	//if( pszType[0] != 't' )		rc = 0;
	//if( pszType[1] != 'e' )		rc = 0;
	//if( pszType[2] != 'x' )		rc = 0;
	//if( pszType[3] != 't' )		rc = 0;
	//if( pszType[4] != '/' )		rc = 0;
	
	if( pszType[5] != 'h' )			return TRUE; 
	if( pszType[6] != 't' )			return TRUE; 
	if( pszType[7] != 'm' )			return TRUE; 
	if( pszType[8] != 'l' )			return TRUE; 
		//if( rc > 0 )		return FALSE;

	return FALSE;
}

BOOL NSM_Http::_ConnectGET( char * pszURL )
{
	// Open Internet
	m_hInternet = ::InternetOpen( m_szAgentName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if( m_hInternet == NULL )
	{
		// Error
		NSMString::Copy( m_szLastError, "Cannot open internet");
		m_nLastErrorCode=::GetLastError();
		return FALSE;
	}
	
	return TRUE;
}

BOOL NSM_Http::_ConnectPUT( char * pszURL)
{	
	ParseURL(pszURL, m_szProtocol, m_szAddress, m_nPort, m_szURI);

	// Open Internet
	m_hInternet = ::InternetOpen( "I3NETWORK_HTTP", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if( m_hInternet == NULL )
	{
		// Error
		NSMString::Copy( m_szLastError, "Cannot open internet");
		m_nLastErrorCode=::GetLastError();
		return FALSE;
	}

	// Open URL
	m_hConnection = ::InternetConnect( m_hInternet, m_szAddress, m_nPort, m_szUserName, m_szUserPW, INTERNET_SERVICE_HTTP, 0, 0 );

	if( m_hConnection == NULL )
	{
		// Error
		NSMString::Copy( m_szLastError, "Cannot connect to internet");
		m_nLastErrorCode=::GetLastError();
		::InternetCloseHandle( m_hInternet );
		m_hInternet = NULL;
		return FALSE;
	}

	m_hHttp = ::HttpOpenRequest( m_hConnection, _HTTP_VERB_PUT, m_szURI, NULL, NULL, NULL, 0, 0 );

	if( m_hHttp == NULL )
	{
		// Error
		m_nLastErrorCode=::GetLastError();
		::CloseHandle(m_hConnection);
		::CloseHandle(m_hInternet);
		return FALSE;
	}

	return TRUE;
}

BOOL NSM_Http::_ConnectPOST(char* pszURL)
{
	m_State = HTTP_STATE_POST;

	ParseURL(pszURL, m_szProtocol, m_szAddress, m_nPort, m_szURI);

	// Initializes an application's use of the WinINet functions.
	{
		m_hInternet = InternetOpen(m_szAgentName, INTERNET_OPEN_TYPE_PRECONFIG , NULL, NULL, 0);
			
		if (!m_hInternet) 
		{
			NSMString::Copy( m_szLastError, "Cannot open internet");
			m_nLastErrorCode = ::GetLastError();
			return FALSE;
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
			NSMString::Copy( m_szLastError, "Cannot connect to internet");
			m_nLastErrorCode = ::GetLastError();
			::CloseHandle(m_hInternet);
			m_hInternet = NULL;
			return FALSE;
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
	if(::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
	{		
		m_nLastErrorCode=::GetLastError();
		::CloseHandle(m_hConnection);
		::CloseHandle(m_hInternet);
		m_hConnection = NULL;
		m_hInternet = NULL;
		return FALSE;
	}

	return TRUE;
}


void NSM_Http::_DisConnectGET(void)
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp = NULL;
	}

	if( m_hInternet )
	{
		::InternetCloseHandle( m_hInternet );
		m_hInternet = NULL;
	}
}

void NSM_Http::_DisConnectPUT()
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp			= NULL;
	}
	if( m_hConnection )
	{
		::InternetCloseHandle( m_hConnection );
		m_hConnection	= NULL;
	}
	if( m_hInternet )
	{
		::InternetCloseHandle( m_hInternet );
		m_hInternet		= NULL;
	}
}

void NSM_Http::_DisConnectPOST()
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp			= NULL;
	}
	if( m_hConnection )
	{
		::InternetCloseHandle( m_hConnection );
		m_hConnection	= NULL;
	}
	if( m_hInternet )
	{
		::InternetCloseHandle( m_hInternet );
		m_hInternet		= NULL;
	}
}

INT32 NSM_Http::DownFile(void)
{	
	UINT32	nAvailableSize = 65536;
	UINT32	nRead;
	UINT32	nWritten = 0;
	BOOL	Rv;

	if( m_State == HTTP_STATE_DOWNLOADING)
	{
		// 다운 받을 패킷량을 구한다.
		Rv = ::InternetQueryDataAvailable( m_hHttp, (LPDWORD) &nAvailableSize, 0, 0 );

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
		if( ::WriteFile( m_hFile, m_pReadBuf, nRead, (LPDWORD) &nWritten, NULL ) == FALSE )
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
			m_hFile = NULL; 

			m_nRestartCount = 0;
			m_nReloadCount	= 0;
			
			m_State	= HTTP_STATE_DOWNCOMPLETE;
		}
	}

ExitErr : 

	return m_State;
}

BOOL NSM_Http::ReloadCheck()
{
	m_nReloadCount++;
	if( m_nReloadCount > m_nTotalReload )
		return FALSE;

	m_State = HTTP_STATE_REDOWNLOAD;
	return TRUE;
}

BOOL NSM_Http::RestartCheck()
{
	m_nRestartCount++;
	if( m_nRestartCount > m_nTotalRestart )
		return FALSE;

	return TRUE;
}

INT32 NSM_Http::DownStart( char * pszURL, char * pszPath )
{
	BOOL	Rv;
	if( HTTP_STATE_DOWNLOADING == m_State )	return HTTP_ERR_REDOWNLOAD; 

	// 접속
	m_nSize = 0;
	m_nRecv	= 0;

	if( m_State != HTTP_STATE_REDOWNLOAD )
	{
		NSMString::Copy( m_szURL, pszURL );
		NSMString::Copy( m_szPath, pszPath );
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
	m_hFile = ::CreateFile( pszPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
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

void NSM_Http::Cancel()
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
	m_hFile = NULL;

	m_State = HTTP_STATE_NONE;
}

INT32 NSM_Http::UpStart( char * pszURL, char * pszPath )
{
	INT32 Rv = HTTP_ERR_REUPLOAD; 

	BY_HANDLE_FILE_INFORMATION	fileinfo;

	INTERNET_BUFFERS		ibuf;

	if( m_State != HTTP_STATE_UPLOADING )
	{
		// 접속
		m_nSize = 0;
		m_nRecv	= 0;

		if( _ConnectPUT( pszURL) == FALSE )
		{
			// 접속 실패입니다.
			m_State = HTTP_STATE_UPFAILE; 
			return HTTP_ERR_CONNECT;
		}

		ibuf.dwStructSize	= sizeof( INTERNET_BUFFERS );
		ibuf.Next			= NULL;
		ibuf.lpcszHeader	= NULL;
		ibuf.dwHeadersLength	= NULL;
		if( ::HttpSendRequest( m_hHttp, NULL, 0, NULL, NULL ) == FALSE )
		{
			// 명령을 보내지 못했습니다.
			m_State = HTTP_STATE_UPFAILE;
		}
		else
		{
			m_State = HTTP_STATE_UPLOADING;

			// 파일을 오픈합니다.
			m_hFile = ::CreateFile( pszPath, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY, NULL );
			// 파일 사이즈를 넣어줘야됩니다....
			::GetFileInformationByHandle( m_hFile, &fileinfo );
			
			Rv = m_nSize	= (fileinfo.nFileSizeHigh << 16) | ( fileinfo.nFileSizeLow );

			m_nSend = 0;
		}
	}

	return Rv;
}

INT32 NSM_Http::UpFile( void )
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
		if( ::ReadFile( m_hFile, m_pReadBuf, nAvailableSize, (LPDWORD) &nRead, NULL ) == FALSE )
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


BOOL NSM_Http::CheckFile( char * pszPath )
{
	BOOL Rv;
	Rv = _ConnectGET( pszPath );
	_DisConnectGET(); 
	return Rv;
}

INT32 NSM_Http::DeleteFile( char * pszPath )
{
	return 1;
}

BOOL NSM_Http::SendRequestPost(void)
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp = NULL;
	}

	// Creates an HTTP request handle.
	wchar_t* accept[2]={L"*/*",0}; 
	m_hHttp = HttpOpenRequest(m_hConnection, _HTTP_VERB_POST, m_szURI, NULL, "", (const char**) accept, m_OpenRequestFlag, 0 );
	if( FALSE == m_hHttp )
	{
		m_nLastErrorCode=::GetLastError();
		return FALSE;
	}

	try
	{	
		INT32 result =  HttpSendRequest(m_hHttp,
										DEFAULT_HTTP_HEADER,
										(DWORD)strlen(DEFAULT_HTTP_HEADER),
										(LPVOID)m_szPostArguments,
										(DWORD)NSMString::Length(m_szPostArguments));
		if (result) 	
		{
			return TRUE;
		}

		INT32 lastErr = ::GetLastError();

		if (lastErr == ERROR_INTERNET_INVALID_URL)
		{
			NSMString::Copy( m_szLastError, "Cannot perform http request, The URL is invalid.");
			m_nLastErrorCode = ::GetLastError();
			return FALSE;
		}
		else if (lastErr == ERROR_INTERNET_CANNOT_CONNECT)
		{
			NSMString::Copy( m_szLastError, "Cannot perform http request, The attempt to connect to the server failed.");
			m_nLastErrorCode = ::GetLastError();
			return FALSE;
		}			
		else
		{
			NSMString::Copy( m_szLastError, "Cannot perform http request");
			m_nLastErrorCode = ::GetLastError();
			return FALSE;
		}
	}
	catch(...) 
	{
		NSMString::Copy( m_szLastError, "Memory Exception occured");
		m_nLastErrorCode = ::GetLastError();
		return FALSE;
	}
}

BOOL NSM_Http::SendRequestGet(char* strTemp)
{
	if( m_hHttp )
	{
		::InternetCloseHandle( m_hHttp );
		m_hHttp = NULL;
	}

	// Open URL
	m_hHttp = ::InternetOpenUrl( m_hInternet, strTemp, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_CACHE_WRITE , 0 );
	if( NULL == m_hHttp )
	{
		// Error
		NSMString::Copy( m_szLastError, "Cannot connect to internet");
		m_nLastErrorCode = ::GetLastError();
		_DisConnectGET();
		return FALSE;
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
	//		return FALSE;
	//	}

	//	if( !::HttpQueryInfo( m_hHttp, HTTP_QUERY_CONTENT_TYPE, pTypeBuf, (LPDWORD) &nTypeSize, NULL ) )
	//	{
	//		_DisConnectGET();
	//		return FALSE;
	//	}

	//	// 파일 타입 파싱( html 파일인 경우 에러를 리턴합니다. )
	//	//if( !_CheckType( pTypeBuf ) )
	//	//{
	//	//	_DisConnectGET();
	//	//	return FALSE;
	//	//}

	//	m_nSize = (UINT32) NSMString::ToInt( pBuff );
	//	if( m_nSize == 0 )
	//	{
	//		_DisConnectGET();
	//		return FALSE;
	//	}
	//}

	return TRUE;
}

INT32	NSM_Http::RecvRequest()
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
		NSMString::Concat( m_pReadBuf, szTemp);
	} while( nResult && (dwNumberOfBytesRead != 0) );

	m_pReadBuf[ m_nReadBufSize-1 ] = '\0';
	return nRecvSize;
}

void NSM_Http::ParseURL(char* pszURL, char* pszProtocol, char* pszAddress, UINT16 &nPort, char* pszURI)
{
	DWORD dwPosition=0;
	BOOL bFlag=FALSE;

	while( NSMString::Length(pszURL) > 0 && dwPosition < (DWORD)NSMString::Length(pszURL) && NSMString::NCompare((pszURL+dwPosition), ":", 1))
		++dwPosition;

	if( !NSMString::NCompare((pszURL+dwPosition+1), "/", 1))
	{	
		// is PROTOCOL
		if(pszProtocol)
		{
			NSMString::NCopy(pszProtocol, pszURL, dwPosition);
			pszProtocol[dwPosition]=0;
		}
		bFlag=TRUE;
	}
	else
	{	
		// is HOST 
		if(pszProtocol)
		{
			NSMString::NCopy(pszProtocol, "http", 4);
			pszProtocol[5]=0;
		}
	}

	DWORD dwStartPosition=0;	
	if(bFlag)	dwStartPosition=dwPosition+=3;				
	else		dwStartPosition=dwPosition=0;

	bFlag=FALSE;
	while( NSMString::Length(pszURL) > 0 && dwPosition < (DWORD)NSMString::Length(pszURL) && NSMString::NCompare((pszURL+dwPosition), "/", 1))
			++dwPosition;

	DWORD dwFind = dwStartPosition;

	for( ; dwFind <= dwPosition ; dwFind++)
	{
		if( !NSMString::NCompare( (pszURL+dwFind), ":", 1))
		{ 
			// find PORT
			bFlag=TRUE;
			break;
		}
	}

	if( bFlag)
	{
		char sztmp[_TEXT_BUFFER_SIZE] = "";
		NSMString::NCopy( sztmp, (pszURL+dwFind+1), ((dwPosition-1) - dwFind));
		nPort = (UINT16)NSMString::ToInt(sztmp);
		NSMString::NCopy(pszAddress, (pszURL+dwStartPosition), dwFind-dwStartPosition);
	}
	else if( !NSMString::Compare(pszProtocol,"https"))
	{
		nPort = INTERNET_DEFAULT_HTTPS_PORT;
		NSMString::NCopy( pszAddress, (pszURL+dwStartPosition), dwPosition-dwStartPosition);
		
		// HTTPS프로토콜이라면, 플래그를 추가합니다.
		m_OpenRequestFlag |= INTERNET_FLAG_SECURE;
	}
	else 
	{
		nPort = INTERNET_DEFAULT_HTTP_PORT;
		NSMString::NCopy( pszAddress, (pszURL+dwStartPosition), dwPosition-dwStartPosition);
	}

	if( dwPosition < (DWORD)NSMString::Length(pszURL))
	{
		// find URI
		NSMString::NCopy( pszURI, (pszURL+dwPosition), NSMString::Length(pszURL)-dwPosition);
	}
	else
	{
		pszURI[0]=0;
	}

	return;
}

void NSM_Http::ResetArguments(void)
{
	NSM_nsMemory::FillZero( m_ArgumentArray, sizeof( HTTP_ARGUMENT)*MAX_HTTP_ARGUMENT);
	m_ArgumentArrayCount = 0;

//	for( INT32 i = 0; i < m_ArgumentList.GetCount(); i++)
//	{
//		HTTP_ARGUMENT* pArg = (HTTP_ARGUMENT*)m_ArgumentList.GetItem(i);
//		NSM_nsMemoryFree( pArg);
//	}

//	m_ArgumentList.Clear();
}

void NSM_Http::AddArgument( char* pszName, char* pszValue)
{
	char* src = pszValue;
	char* dst = m_ArgumentArray[m_ArgumentArrayCount].szValue;

	if( m_ArgumentArrayCount == MAX_HTTP_ARGUMENT)	return;
	if( NSMString::Length(pszValue) >= MAX_HTTP_ARG_SIZE)	return;

	NSMString::Copy( m_ArgumentArray[m_ArgumentArrayCount].szName, pszName );
	
	//일부 특수문자에 대한 변환처리
	while( (*dst = *src) != '\0' )
	{
		switch(*src)
		{
		case '%':
			NSM_COPY( dst, "%25", 3);
			dst += 3;
			break;
		case '&':
			NSM_COPY( dst, "%26", 3);
			dst += 3;
			break;
		case '+':
			NSM_COPY( dst, "%2B", 3);
			dst += 3;
			break;
		default:
			dst++;
			break;
		}

		src++;
	}

	m_ArgumentArray[m_ArgumentArrayCount].dwType = TYPE_POST_ARGUMENT_NORMAL;
	m_ArgumentArrayCount++;

//	HTTP_ARGUMENT * pArg = (HTTP_ARGUMENT*)NSM_nsMemoryAlloc(sizeof(HTTP_ARGUMENT));
//	NSM_nsMemory::FillZero( pArg, sizeof(HTTP_ARGUMENT));
//	NSMString::Copy( pArg->szName, pszName);
//	NSMString::Copy( pArg->szValue, pszValue);
//	pArg->dwType = NSM_Http::TYPE_POST_ARGUMENT_NORMAL;
//	m_ArgumentList.Add( pArg);
}

void NSM_Http::AddArgument( char* pszName, INT32 nValue)
{
	if( m_ArgumentArrayCount == MAX_HTTP_ARGUMENT) return;

	NSMString::Copy( m_ArgumentArray[m_ArgumentArrayCount].szName, pszName );
	NSMString::Format( m_ArgumentArray[m_ArgumentArrayCount].szValue, MAX_PATH, "%d", nValue );
	m_ArgumentArray[m_ArgumentArrayCount].dwType = TYPE_POST_ARGUMENT_NORMAL;
	m_ArgumentArrayCount++;

//	HTTP_ARGUMENT * pArg = (HTTP_ARGUMENT*)NSM_nsMemoryAlloc(sizeof(HTTP_ARGUMENT));
//	NSM_nsMemory::FillZero( pArg, sizeof(HTTP_ARGUMENT));
//	NSMString::Copy( pArg->szName, pszName);
//	sprintf(pArg->szValue, "%d", nValue);
//	pArg->dwType = TYPE_POST_ARGUMENT_NORMAL;
//	m_ArgumentList.Add( pArg);
}

void NSM_Http::SetUpPostArguments(void)
{
	m_szPostArguments[0] = '\0';	

	for( UINT32 i = 0; i < m_ArgumentArrayCount; i++)
	{
		NSMString::Concat( m_szPostArguments, m_ArgumentArray[i].szName);
		NSMString::Concat( m_szPostArguments, "=");
		NSMString::Concat( m_szPostArguments, m_ArgumentArray[i].szValue);
		if( i !=  m_ArgumentArrayCount - 1) NSMString::Concat( m_szPostArguments, "&");
	}

//	for( INT32 i = 0; i < m_ArgumentList.GetCount(); i++)
//	{
//		HTTP_ARGUMENT* pArg = (HTTP_ARGUMENT*)m_ArgumentList.GetItem(i);
//		NSMString::Concat( m_szPostArguments, pArg->szName);
//		NSMString::Concat( m_szPostArguments, "=");
//		NSMString::Concat( m_szPostArguments, pArg->szValue);
//		if( i !=  m_ArgumentList.GetCount() - 1) NSMString::Concat( m_szPostArguments, "&");
//	}
}

void NSM_Http::ConvertMD5( char* pOutputString, char* pInputString)
{
	UINT64	code[2];

	NSM_MD5::Gen( pInputString, NSMString::Length( pInputString), (char *)&code);
	NSM_MD5::GetMD5String( pOutputString, code);	
}
