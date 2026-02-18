#if !defined( __I3NETWORK_HTTP_H )
#define __I3NETWORK_HTTP_H

/////////////////////////////////////////////////////////////////////////////////////////
// HTTP File Download
// make by 정순구 2006. 6. 14
// Desc : htm, html 파일은 받을 수 없습니다.

#if defined( I3_WINDOWS )
#include <wininet.h>

#define	HTTP_ERR_CONNECT		-1
#define HTTP_ERR_REDOWNLOAD		-2
#define	HTTP_ERR_CREATEFILE		-3
#define	HTTP_ERR_DOWNSIZE		-4
#define HTTP_ERR_READFILE		-5
#define HTTP_ERR_WRITEFILE		-6
#define HTTP_ERR_REUPLOAD		-7
#define HTTP_ERR_REMOTECRETE	-8
#define HTTP_ERR_UPSIZE			-9


//InternetReadFile Max Count 
#define READFILE_FAIL_MAX_COUNT 10

#define DEFAULT_AGENT_NAME		"I3NETWORK_HTTP"
#define DEFAULT_HTTP_HEADER		"Content-Type: application/x-www-form-urlencoded\r\n"
#define _TEXT_BUFFER_SIZE		1024
#define _HTTP_VERB_GET			"GET"
#define _HTTP_VERB_PUT			"PUT"
#define _HTTP_VERB_POST			"POST"

#define MAX_HTTP_ARGUMENT		50

// ARGUMENTS STRUCTURE
struct HTTP_ARGUMENT
{						
	char	szName[MAX_PATH] = { 0 };
	char	szValue[MAX_PATH] = { 0 };
	DWORD	dwType = 0;
};

enum HTTP_STATE
{
	HTTP_STATE_NONE = 0,
	HTTP_STATE_DOWNLOADING,
	HTTP_STATE_REDOWNLOAD,
	HTTP_STATE_DOWNCOMPLETE,
	HTTP_STATE_DOWNFAILE,

	HTTP_STATE_UPLOADING,
	HTTP_STATE_UPCOMPLETE,
	HTTP_STATE_UPFAILE,

	HTTP_STATE_POST,
}; 

class I3_EXPORT_NETWORK i3NetworkHTTP : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkHTTP, i3ElementBase );

public:
	// REQUEST METHOD
	enum REQUEST_METHOD
	{															
		REQUEST_METHOD_UNKNOWN=0,
		REQUEST_METHOD_GET,
		REQUEST_METHOD_PUT,
		REQUEST_METHOD_POST,
		REQUEST_METHOD_POST_MULTIPARTSFORMDATA,
	};

	// POST TYPE 
	enum TYPE_POST_ARGUMENT
	{													
		TYPE_POST_ARGUMENT_UNKNOWN=0,
		TYPE_POST_ARGUMENT_NORMAL,
		TYPE_POST_ARGUMENT_BINARY,
	};

protected:
	HINTERNET		m_hInternet = nullptr;
	HINTERNET		m_hHttp = nullptr;
	HINTERNET		m_hConnection = nullptr;
	HANDLE			m_hFile = nullptr;
	
	//=======================================================================================================================================================================================
	//
	// Post 처리 (송명일 작업)
	//
	//=======================================================================================================================================================================================
	UINT32			m_OpenRequestFlag = INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_COOKIES;// | INTERNET_FLAG_SECURE;
	UINT16			m_nPort = 0;
	char			m_szAgentName[MAX_PATH] = { 0 };
	char			m_szUserName[MAX_PATH] = { 0 };
	char			m_szUserPW[MAX_PATH] = { 0 };
	char			m_szProtocol[_TEXT_BUFFER_SIZE] = { 0 };
	char			m_szAddress[_TEXT_BUFFER_SIZE] = { 0 };
	char			m_szURI[_TEXT_BUFFER_SIZE] = { 0 };
	char			m_szPostArguments[1024] = { 0 };
	HTTP_ARGUMENT	m_ArgumentArray[MAX_HTTP_ARGUMENT];	// HTTP Post데이터의 경우, 50개이상은 데이터를 받을수 없도록 처리했습니다.
	UINT32			m_ArgumentArrayCount = 0;


	//=======================================================================================================================================================================================
	//
	// File Download upload (정순구 작업)
	//
	//=======================================================================================================================================================================================
	char *			m_pReadBuf = nullptr;	
	INT32			m_nReadBufSize = 0;
	UINT32			m_nSend = 0;
	INT32			m_State = 0;
	INT32			m_FileErrorCode = 0;
	INT32			m_nTotalReload = 0;
	INT32			m_nReloadCount = 0;
	INT32			m_nTotalRestart = 0;
	INT32			m_nRestartCount = 0;
	UINT32			m_nRestartSleep = 0;
	UINT32			m_nReloadTimeOut = 0;
	UINT32			m_nDownTime = 0;
	UINT32			m_nLocktime = 0;
	UINT32			m_nOutTimer = 0;
	char			m_szURL[MAX_PATH] = { 0 };
	char			m_szPath[MAX_PATH] = { 0 };

	// Error code처리
	char			m_szLastError[1024] = { 0 };
	INT32			m_nLastErrorCode = 0;

protected :	
	bool	_ConnectGET( char * pszURL);
	bool	_ConnectPUT( char * pszURL);
	bool	_ConnectPOST( char* pszURL);

	void	_DisConnectGET();
	void	_DisConnectPUT();
	void	_DisConnectPOST();

	bool	_CheckType( char * pszType );

public:
	UINT32			m_nSize = 0;			// 파일 사이즈
	UINT32			m_nRecv = 0;			// 받은 사이즈

	UINT32			getFileSize( void )				{ return m_nSize; }
	UINT32			getRecvSize( void )				{ return m_nRecv; }

public:
	i3NetworkHTTP();
	virtual ~i3NetworkHTTP();

	bool	Connect( char* pszURL, REQUEST_METHOD method);
	bool	IsInternetConnect(void);
	void	DisConnect(void);

	//=======================================================================================================================================================================================
	//
	// Post 처리 (송명일 작업)
	//
	//=======================================================================================================================================================================================	
	INT32	RecvRequest();

	bool	SendRequestPost(void);	
	bool	SendRequestGet( char* strTemp );
	char*	GetRequestResult(void)			{	return m_pReadBuf;	}
	void	ParseURL(char* pszURL, char* pszProtocol, char* pszAddress, UINT16 &nPort, char* pszURI);	
	void	ResetArguments(void);
	bool	AddArgument( char* pszName, char* pszValue);
	bool	AddArgument( char* pszName, INT32 nValue);
	bool	AddArgument( char* pszName, INT64 i64Value);
	void	SetUpPostArguments(void);															// Post Arguments를 조합합니다.
	void	ConvertMD5( char* pOutputString, char* pInputString, UINT32 nBufSize);	


	//=======================================================================================================================================================================================
	//
	// File Download upload (정순구 작업)
	//
	//=======================================================================================================================================================================================
	void	CreateMethod( UINT32 nRestartCount = 3, UINT32 nRestartSleep = 300, UINT32 nReloadCount = 3, UINT32 nReloadTimeout = 1000, UINT32 nBufferSize = 65536 );
	void	Destroy( void);
	void	Cancel(void);											// 작업 중지
	
	// 파일 다운로드( 1 가 반환될때까지 쓰레드에서 파일을 받습니다. )
	// return 0 -> 아직 파일을 받는 중입니다.
	// return -1 -> 접속 실패입니다.
	INT32	DownStart( char * pszURL, char * pszPath );				//다운 시작할때 
	INT32	DownFile( void );										//다운중
	char*	GetReadBuffer()				{ return m_pReadBuf;		}

	// true : 재시도
	// false : 재시도 할 수 없습니다. 카운트를 다 썼음..
	bool	RestartCheck();
	bool	ReloadCheck();

	INT32	UpStart( char * pszURL, char * pszPath );
	INT32	UpFile( void );

	// RemoteDir에서 해당 파일을 찾는다..
	bool	CheckFile( char * pszPath );
	INT32	DeleteFile( char * pszPath );
	INT32	GetState(void)				{ return m_State;			} 

	INT32	GetFileError(void)			{ return m_FileErrorCode;	}	
	char*	GetLastErrorString(void)	{ return m_szLastError;		}
	INT32	GetLastErrorCode(void)		{ return m_nLastErrorCode;	}
	
	
};

#endif	// I3_WINDOWS

#endif	// __I3NETWORK_HTTP_H
