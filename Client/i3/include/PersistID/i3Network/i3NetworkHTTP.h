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
}; 

class I3_EXPORT_NETWORK i3NetworkHTTP : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkHTTP );
protected:
	HINTERNET		m_hInternet;
	HINTERNET		m_hHttp;
	HINTERNET		m_hConnection;
	HANDLE			m_hFile;
	
	char *			m_pReadBuf;
	
	UINT32			m_nSend;

	INT32			m_State;
	INT32			m_LastErrorCode; 

	INT32			m_nTotalReload;
	INT32			m_nReloadCount;
	INT32			m_nTotalRestart;
	INT32			m_nRestartCount;

	UINT32			m_nRestartSleep;
	UINT32			m_nReloadTimeOut;

	UINT32			m_nDownTime;
	UINT32			m_nLocktime;
	UINT32			m_nOutTimer;

	char			m_szURL[ MAX_PATH ];
	char			m_szPath[ MAX_PATH ];

protected :
	BOOL	_Connect( const char * pszURL );
	BOOL	_ConnectPOST( const char * pszURL, const char * pszRemoteName, const char * pszFileName );

	void	_DisConnect();
	void	_DisConnectPOST();

	BOOL	_CheckType( const char * pszType );

public:
	
	char			m_szUser[ 32 ];
	char			m_szPass[ 32 ];

	UINT32			m_nSize;			// 파일 사이즈
	UINT32			m_nRecv;			// 받은 사이즈

	UINT32	getFileSize( void )				{ return m_nSize; }
	UINT32	getRecvSize( void )				{ return m_nRecv; }

public:
	i3NetworkHTTP();
	virtual ~i3NetworkHTTP();

	// nRestartCount	= 재접속 횟수 (접속 실패시 내부에서 다시 접속을 시도합니다. )
	// nRestartTime		= 재접속 시간간격 ( 이시간마다 재접속을 시도합니다 )
	// nReloadCount		= 재시도 횟수 ( 파일을 다시 받도록 시도하는 횟수 )
	// nTimeoutCount	= 타임아웃시간 ( 이간격동안 받지못하거나 사이즈가 계속 0이면 재시도합니다. )
	void	Create( UINT32 nRestartCount = 3, UINT32 nRestartSleep = 300, UINT32 nReloadCount = 3, UINT32 nReloadTimeout = 1000, UINT32 nBufferSize = 65536 );
	void	Destroy( void);
	void	Cancel(void);											// 작업 중지
	
	// 파일 다운로드( 1 가 반환될때까지 쓰레드에서 파일을 받습니다. )
	// return 0 -> 아직 파일을 받는 중입니다.
	// return -1 -> 접속 실패입니다.
	INT32	DownStart( const char * pszURL, const char * pszPath );	//다운 시작할때 
	INT32	DownFile( void );										//다운중

	// TRUE : 재시도
	// FALSE : 재시도 할 수 없습니다. 카운트를 다 썼음..
	BOOL	RestartCheck();
	BOOL	ReloadCheck();

	INT32	UpStart( const char * pszURL, const char * pszRemote, const char * pszPath );
	INT32	UpFile( void );

	// RemoteDir에서 해당 파일을 찾는다..
	BOOL	CheckFile( const char * pszPath );
	INT32	DeleteFile( const char * pszPath );
	INT32	GetState(void)			{ return m_State; } 
	INT32	GetLastError(void)		{ return m_LastErrorCode; }


	// POST방식은 작업예정.. 정순구
	void	CreatePOST( const char * pUserName, const char * pPass, UINT32 nBufferSize = 65536 );
};

#endif	// I3_WINDOWS

#endif	// __I3NETWORK_HTTP_H
