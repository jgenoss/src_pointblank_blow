#if !defined( __I3NETWORKFTP_H )
#define __I3NETWORKFPT_H

/////////////////////////////////////////////////////////////////////////
// name : i3NetworkFTP
// desc : WinINET을 이용한 FTP입니다.
// maked by 정순구 2006. 6. 1
#if defined( I3_WINDOWS )
#include <wininet.h>
#endif	// I3_WINDOWS

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#include "i3String.h"

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


class I3_EXPORT_BASE i3NetworkFTP : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkFTP );
protected:
	HINTERNET	m_hInternet;
	HINTERNET	m_hFtp;

	UINT32		m_nFlags;

	char		m_szAddress[ MAX_FTPADDRESS ];
	UINT16		m_nPort;
	char		m_szUserID[ MAX_USERIDNAME ];
	char		m_szUserPass[ MAX_USERPASS ];

	char		m_szStr[ 256];

	DWORD		m_dwProg;
	DWORD		m_dwCurrentDir;

	INT32		m_nLocalDirLen;
	char		m_szLocalDirectory[ MAX_PATH ];

	char		m_szRemoteDirectory[ MAX_PATH ];

public:
	UINT32	getFlags( void )					{ return m_nFlags; }
	void	setFlags( UINT32 nFlag )			{ m_nFlags = nFlag; }
	void	addFlags( UINT32 nFlag )			{ m_nFlags |= nFlag; }
	void	removeFlags( UINT32 nFlag )			{ m_nFlags &= ~nFlag; }

	char *	getAddress( void )					{ return m_szAddress; }
	void	setAddress( char * address )		{ i3String::Copy( m_szAddress, address, MAX_FTPADDRESS ); }

	UINT16	getPort( void )						{ return m_nPort; }
	void	setPort( UINT16 nPort )				{ m_nPort = nPort; }

	char *	getUserID( void )					{ return m_szUserID; }
	void	setUserID( char * id )				{ i3String::Copy( m_szUserID, id, MAX_USERIDNAME ); }

	char *	getUserPass( void )					{ return m_szUserPass; }
	void	setUserPass( char * pass )			{ i3String::Copy( m_szUserPass, pass, MAX_USERPASS ); }

	char *	getLog( void )						{ return m_szStr; }

	// 해당 패스의 디렉토리가 있는지 검사( 없으면 생성한다. )	
	BOOL	CheckDirectory( const char * pszPath );

	// 해당 패스의 FTP서버의 디렉토리가 있는지 검사( 없으면 생성한다. -> 권한이 부여된 아이디어야한다.. )	
	BOOL	CheckRemoteDir( HINTERNET hFtp, const char * pszPath );

	// \ -> /	
	void	ConvertInternetDir( char * pszPath, INT32 nLength );
public:
	i3NetworkFTP();
	virtual ~i3NetworkFTP();

	BOOL	Create( char * address, INT32 nPort = 0,
		char * id = NULL, char * pass = NULL, UINT32 nFlag = 0 );

	BOOL	Connect();
	BOOL	DisConnect();

	// 파일을 FTP 서버로부터 받습니다.
	BOOL	DownLoadFile( char * pszPath );
	BOOL	DownLoadFile( char * pszLocalPath, char * pszRemotePath );
	BOOL	DownLoadFile2( char * pszPath );

	// 파일을 FTP 서버에 올립니다..( 권한을 가진 ID로 로그인해야합니다.. )
	BOOL	UploadFile( const char * pszPath );
	BOOL	UploadFile( const char * pszLocalPath, const char * pszRemotePath );
	BOOL	UploadFile2( const char * pszPath );
	BOOL	UploadFile2( const char * pszLocalPath, const char * pszRemotePath );

	// FTP서버의 RemoteDir에서 해당 파일을 찾는다..
	BOOL	CheckFile( const char * pszPath );
	// FTP서버의 RemoteDir에서 해당 파일을 지운다.. FALSE 이면 해당 파일이 없거나 지우기 실패
	BOOL	DeleteFile( const char * pszPath );

	// Thread를 위해 준비중입니다..
	virtual void OnDownLoad();
	virtual void OnUpLoad();
};

#endif	// __I3NETWORKFTP_H
