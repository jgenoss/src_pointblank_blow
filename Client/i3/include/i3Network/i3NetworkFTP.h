#if !defined( __I3NETWORKFTP_H )
#define __I3NETWORKFPT_H

/////////////////////////////////////////////////////////////////////////
// name : i3NetworkFTP
// desc : WinINET을 이용한 FTP입니다.
// maked by 정순구 2006. 6. 1
#if defined( I3_WINDOWS )
#include <wininet.h>
#endif	// I3_WINDOWS

#include "i3Base/string/ext/safe_string_copy.h"


#define MAX_FTPADDRESS	64
#define MAX_USERIDNAME	32
#define MAX_USERPASS	32

#define I3_FTP_DEFAULT_ID			"anonymous"
#define I3_FTP_DEFAULT_PASS			""

#define I3_FTP_DEFAULT_PORT			21

#define I3_FTP_FLAG_CHECKREMOTEDIR		0x00000001
#define I3_FTP_FLAG_CONNECTMAINTAIN		0x00000002	// 접속을 유지한다..
													// 이 플래그는 만들어질때 한번 접속되어지며 계속 접속 상태를 유지한다..
													// 수동으로 Disconnect 해야된다..

class I3_EXPORT_NETWORK i3NetworkFTP : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkFTP, i3ElementBase );
protected:
	HINTERNET	m_hInternet = nullptr;
	HINTERNET	m_hFtp = nullptr;

	UINT32		m_nFlags = 0;

	char		m_szAddress[MAX_FTPADDRESS] = { 0 };
	UINT16		m_nPort = INTERNET_DEFAULT_FTP_PORT;
	char		m_szUserID[MAX_USERIDNAME] = { 0 };
	char		m_szUserPass[MAX_USERPASS] = { 0 };

	char		m_szStr[256] = { 0 };

	DWORD		m_dwProg = 0;
	DWORD		m_dwCurrentDir = 0;

	INT32		m_nLocalDirLen = 0;
	char		m_szLocalDirectory[MAX_PATH] = { 0 };
	char		m_szRemoteDirectory[MAX_PATH] = { 0 };

public:
	UINT32	getFlags( void )					{ return m_nFlags; }
	void	setFlags( UINT32 nFlag )			{ m_nFlags = nFlag; }
	void	addFlags( UINT32 nFlag )			{ m_nFlags |= nFlag; }
	void	removeFlags( UINT32 nFlag )			{ m_nFlags &= ~nFlag; }

	char *	getAddress( void )					{ return m_szAddress; }
	void	setAddress( char * address )		{ i3::safe_string_copy( m_szAddress, address, MAX_FTPADDRESS ); }

	UINT16	getPort( void )						{ return m_nPort; }
	void	setPort( UINT16 nPort )				{ m_nPort = nPort; }

	char *	getUserID( void )					{ return m_szUserID; }
	void	setUserID( char * id )				{ i3::safe_string_copy( m_szUserID, id, MAX_USERIDNAME ); }

	char *	getUserPass( void )					{ return m_szUserPass; }
	void	setUserPass( char * pass )			{ i3::safe_string_copy( m_szUserPass, pass, MAX_USERPASS ); }

	char *	getLog( void )						{ return m_szStr; }

	// 해당 패스의 디렉토리가 있는지 검사( 없으면 생성한다. )	
	bool	CheckDirectory( const char * pszPath );

	// 해당 패스의 FTP서버의 디렉토리가 있는지 검사( 없으면 생성한다. -> 권한이 부여된 아이디어야한다.. )	
	bool	CheckRemoteDir( HINTERNET hFtp, const char * pszPath );

	// \ -> /	
	void	ConvertInternetDir( char * pszPath, INT32 nLength );
public:
	bool	Create( char * address, INT32 nPort = 0,
		char * id = nullptr, char * pass = nullptr, UINT32 nFlag = 0 );

	bool	Connect();
	bool	DisConnect();

	// 파일을 FTP 서버로부터 받습니다.
	bool	DownLoadFile( char * pszPath );
	bool	DownLoadFile( char * pszLocalPath, char * pszRemotePath );
	bool	DownLoadFile2( char * pszPath );

	// 파일을 FTP 서버에 올립니다..( 권한을 가진 ID로 로그인해야합니다.. )
	bool	UploadFile( const char * pszPath );
	bool	UploadFile( const char * pszLocalPath, const char * pszRemotePath );
	bool	UploadFile2( const char * pszPath );
	bool	UploadFile2( const char * pszLocalPath, const char * pszRemotePath );

	// FTP서버의 RemoteDir에서 해당 파일을 찾는다..
	bool	CheckFile( const char * pszPath );
	// FTP서버의 RemoteDir에서 해당 파일을 지운다.. false이면 해당 파일이 없거나 지우기 실패
	bool	DeleteFile( const char * pszPath );

	// Thread를 위해 준비중입니다..
	virtual void OnDownLoad() {}
	virtual void OnUpLoad() {}
};

#endif	// __I3NETWORKFTP_H
